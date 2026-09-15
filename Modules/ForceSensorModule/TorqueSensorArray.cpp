#include "TorqueSensorArray.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <limits>

#include "../LoggerModule/easylogging++.h"

TorqueSensorArray::TorqueSensorArray() = default;

TorqueSensorArray::TorqueSensorArray(const Config& config)
    : m_config(config)
{
}

TorqueSensorArray::~TorqueSensorArray()
{
    disconnectDevice();
}

bool TorqueSensorArray::initDevice()
{
    return initDevice(m_config);
}

bool TorqueSensorArray::initDevice(const Config& config)
{
    disconnectDevice();
    m_config = config;

    if (m_config.pollPeriodMs <= 0 || m_config.responseTimeoutMs <= 0 ||
        m_config.staleTimeoutMs <= 0 || m_config.filterCutoffHz < 0.0) {
        LOG(ERROR) << "Invalid torque sensor configuration";
        return false;
    }

    std::lock_guard<std::mutex> busLock(m_busMutex);
    m_context = modbus_new_rtu(m_config.portName.c_str(),
                               m_config.baudRate,
                               m_config.parity,
                               m_config.dataBits,
                               m_config.stopBits);
    if (m_context == nullptr) {
        LOG(ERROR) << "Unable to create torque sensor Modbus context";
        return false;
    }

    const std::uint32_t timeoutUs =
        static_cast<std::uint32_t>(m_config.responseTimeoutMs) * 1000U;
    modbus_set_response_timeout(m_context, timeoutUs / 1000000U,
                                timeoutUs % 1000000U);
    modbus_set_slave(m_context, m_config.slaveId);

    if (modbus_connect(m_context) == -1) {
        LOG(ERROR) << "Torque sensor connection failed: "
                   << modbus_strerror(errno);
        modbus_free(m_context);
        m_context = nullptr;
        return false;
    }

    m_connected.store(true);
    invalidateAll();
    LOG(INFO) << "V24-SZBSQ4 connected on " << m_config.portName
              << ", slave " << m_config.slaveId;
    return true;
}

bool TorqueSensorArray::start()
{
    if (!m_connected.load() || m_context == nullptr)
        return false;

    bool expected = false;
    if (!m_running.compare_exchange_strong(expected, true))
        return true;

    m_pollingThread = std::thread(&TorqueSensorArray::pollingLoop, this);
    return true;
}

void TorqueSensorArray::stop()
{
    m_running.store(false);
    if (m_pollingThread.joinable())
        m_pollingThread.join();
}

void TorqueSensorArray::disconnectDevice()
{
    stop();

    std::lock_guard<std::mutex> busLock(m_busMutex);
    if (m_context != nullptr) {
        modbus_close(m_context);
        modbus_free(m_context);
        m_context = nullptr;
    }
    m_connected.store(false);
    invalidateAll();
}

bool TorqueSensorArray::isConnected() const
{
    return m_connected.load();
}

bool TorqueSensorArray::isRunning() const
{
    return m_running.load();
}

bool TorqueSensorArray::isChannelValid(std::size_t motorIndex) const
{
    if (motorIndex >= ChannelCount)
        return false;

    const Sample sample = getTorque(motorIndex);
    if (!sample.valid)
        return false;

    const std::uint64_t ageUs = nowUs() - sample.timestampUs;
    return ageUs <= static_cast<std::uint64_t>(m_config.staleTimeoutMs) * 1000ULL;
}

TorqueSensorArray::Sample TorqueSensorArray::getTorque(std::size_t motorIndex) const
{
    if (motorIndex >= ChannelCount)
        return {};

    std::lock_guard<std::mutex> lock(m_sampleMutex);
    return m_samples[motorIndex];
}

std::array<TorqueSensorArray::Sample, TorqueSensorArray::ChannelCount>
TorqueSensorArray::getAllTorques() const
{
    std::lock_guard<std::mutex> lock(m_sampleMutex);
    return m_samples;
}

bool TorqueSensorArray::setCalibration(std::size_t motorIndex,
                                       double scaleToNm,
                                       double offsetNm,
                                       double direction)
{
    if (motorIndex >= ChannelCount || !std::isfinite(scaleToNm) ||
        !std::isfinite(offsetNm) || !std::isfinite(direction) ||
        scaleToNm == 0.0 || direction == 0.0) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_sampleMutex);
    m_calibration[motorIndex] = {scaleToNm, offsetNm,
                                 direction > 0.0 ? 1.0 : -1.0};
    return true;
}

bool TorqueSensorArray::zeroAll()
{
    std::lock_guard<std::mutex> busLock(m_busMutex);
    if (!m_connected.load() || m_context == nullptr)
        return false;
    const bool success = modbus_write_bit(m_context, kZeroAllCoil, 1) == 1;
    if (success)
        invalidateAll();
    return success;
}

bool TorqueSensorArray::zeroChannel(std::size_t motorIndex)
{
    if (motorIndex >= ChannelCount)
        return false;

    std::lock_guard<std::mutex> busLock(m_busMutex);
    if (!m_connected.load() || m_context == nullptr)
        return false;
    const bool success = modbus_write_bit(
        m_context,
        kFirstChannelZeroCoil + static_cast<int>(motorIndex),
        1) == 1;
    if (success)
        invalidateAll();
    return success;
}

void TorqueSensorArray::pollingLoop()
{
    constexpr double pi = 3.14159265358979323846;
    auto nextWakeup = std::chrono::steady_clock::now();

    while (m_running.load()) {
        nextWakeup += std::chrono::milliseconds(m_config.pollPeriodMs);

        std::array<double, ChannelCount> rawValues{};
        if (readAllChannels(rawValues)) {
            const std::uint64_t timestamp = nowUs();
            const double dt = static_cast<double>(m_config.pollPeriodMs) / 1000.0;
            const double alpha = m_config.filterCutoffHz <= 0.0
                ? 1.0
                : (2.0 * pi * m_config.filterCutoffHz * dt) /
                  (1.0 + 2.0 * pi * m_config.filterCutoffHz * dt);

            std::lock_guard<std::mutex> lock(m_sampleMutex);
            for (std::size_t i = 0; i < ChannelCount; ++i) {
                const Calibration calibration = m_calibration[i];
                const double torque = calibration.direction * rawValues[i] *
                                      calibration.scaleToNm + calibration.offsetNm;
                Sample& sample = m_samples[i];
                sample.rawValue = rawValues[i];
                sample.torqueNm = torque;
                sample.filteredTorqueNm = sample.valid
                    ? sample.filteredTorqueNm + alpha * (torque - sample.filteredTorqueNm)
                    : torque;
                sample.timestampUs = timestamp;
                ++sample.sequence;
                sample.valid = std::isfinite(torque);
            }
        } else {
            invalidateAll();
        }

        std::this_thread::sleep_until(nextWakeup);
    }
}

bool TorqueSensorArray::readAllChannels(std::array<double, ChannelCount>& values)
{
    std::uint16_t registers[kFloatRegisterCount]{};
    {
        std::lock_guard<std::mutex> busLock(m_busMutex);
        if (!m_connected.load() || m_context == nullptr)
            return false;

        const int count = modbus_read_registers(m_context,
                                                kFirstFloatRegister,
                                                kFloatRegisterCount,
                                                registers);
        if (count != kFloatRegisterCount) {
            LOG(WARNING) << "Torque sensor Modbus read failed: "
                         << modbus_strerror(errno);
            return false;
        }
    }

    for (std::size_t i = 0; i < ChannelCount; ++i) {
        values[i] = static_cast<double>(decodeFloatInverse(registers[2 * i],
                                                           registers[2 * i + 1]));
        if (!std::isfinite(values[i]))
            return false;
    }
    return true;
}

float TorqueSensorArray::decodeFloatInverse(std::uint16_t firstRegister,
                                            std::uint16_t secondRegister)
{
    // Manual states "Float Inverse / register swap": register order is CD AB.
    const std::uint32_t bits =
        (static_cast<std::uint32_t>(secondRegister) << 16U) |
         static_cast<std::uint32_t>(firstRegister);
    float value = 0.0F;
    static_assert(sizeof(value) == sizeof(bits), "Unexpected float size");
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

std::uint64_t TorqueSensorArray::nowUs()
{
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

void TorqueSensorArray::invalidateAll()
{
    std::lock_guard<std::mutex> lock(m_sampleMutex);
    for (Sample& sample : m_samples)
        sample.valid = false;
}
