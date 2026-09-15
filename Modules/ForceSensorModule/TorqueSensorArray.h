#ifndef TORQUESENSORARRAY_H
#define TORQUESENSORARRAY_H

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

#include <modbus/modbus.h>

class TorqueSensorArray
{
public:
    static constexpr std::size_t ChannelCount = 4;

    struct Config
    {
        std::string portName = "/dev/ttyUSB0";
        int baudRate = 115200;
        char parity = 'N';
        int dataBits = 8;
        int stopBits = 1;
        int slaveId = 1;
        int pollPeriodMs = 10;
        int responseTimeoutMs = 50;
        double filterCutoffHz = 20.0;
        int staleTimeoutMs = 200;
    };

    struct Sample
    {
        // The transmitter's calibrated engineering value. Its physical unit
        // depends on the transmitter/sensor calibration.
        double rawValue = 0.0;
        // Converted value: direction * rawValue * scaleToNm + offsetNm.
        double torqueNm = 0.0;
        double filteredTorqueNm = 0.0;
        std::uint64_t timestampUs = 0;
        std::uint64_t sequence = 0;
        bool valid = false;
    };

    TorqueSensorArray();
    explicit TorqueSensorArray(const Config& config);
    ~TorqueSensorArray();

    TorqueSensorArray(const TorqueSensorArray&) = delete;
    TorqueSensorArray& operator=(const TorqueSensorArray&) = delete;

    bool initDevice();
    bool initDevice(const Config& config);
    bool start();
    void stop();
    void disconnectDevice();

    bool isConnected() const;
    bool isRunning() const;
    bool isChannelValid(std::size_t motorIndex) const;

    Sample getTorque(std::size_t motorIndex) const;
    std::array<Sample, ChannelCount> getAllTorques() const;

    // motorIndex 0..3 maps directly to transmitter channels 1..4 and Maxon 0..3.
    bool setCalibration(std::size_t motorIndex,
                        double scaleToNm,
                        double offsetNm = 0.0,
                        double direction = 1.0);

    // Hardware zero commands from the V24-SZBSQ4 protocol.
    bool zeroAll();
    bool zeroChannel(std::size_t motorIndex);

private:
    static constexpr int kFirstFloatRegister = 0;
    static constexpr int kFloatRegisterCount = 8;
    static constexpr int kZeroAllCoil = 0;
    static constexpr int kFirstChannelZeroCoil = 10;

    struct Calibration
    {
        double scaleToNm = 1.0;
        double offsetNm = 0.0;
        double direction = 1.0;
    };

    void pollingLoop();
    bool readAllChannels(std::array<double, ChannelCount>& values);
    static float decodeFloatInverse(std::uint16_t firstRegister,
                                    std::uint16_t secondRegister);
    static std::uint64_t nowUs();
    void invalidateAll();

    Config m_config;
    modbus_t* m_context = nullptr;
    std::atomic<bool> m_connected{false};
    std::atomic<bool> m_running{false};
    std::thread m_pollingThread;

    mutable std::mutex m_sampleMutex;
    std::array<Sample, ChannelCount> m_samples{};
    std::array<Calibration, ChannelCount> m_calibration{};

    // libmodbus context is not used concurrently without this lock.
    mutable std::mutex m_busMutex;
};

#endif // TORQUESENSORARRAY_H
