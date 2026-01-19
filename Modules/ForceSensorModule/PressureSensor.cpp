#include "PressureSensor.h"
#include <unistd.h> // for usleep

PressureSensor::PressureSensor()
    : ctx(nullptr), m_running(false), m_connected(false), m_currentPressure(0.0f)
{
}

PressureSensor::~PressureSensor()
{
    disconnectDevice();
}

bool PressureSensor::initDevice(const char* portName, int baudRate)
{
    // 1. 创建 Modbus RTU 实例
    // 根据 CuteCom 截图：115200 波特率, 'N' 无校验, 8 数据位, 1 停止位
    ctx = modbus_new_rtu(portName, baudRate, 'N', 8, 1);

    if (ctx == nullptr) {
        LOG(ERROR) << "Unable to create the libmodbus context";
        return false;
    }

    // 2. 设置超时时间 (例如 500ms)
    // 避免传感器未响应时卡死线程
    modbus_set_response_timeout(ctx, 0, 500000);

    // 3. 设置从机地址 (Slave ID = 1)
    modbus_set_slave(ctx, SLAVE_ID);

    // 4. 建立连接
    if (modbus_connect(ctx) == -1) {
        LOG(ERROR) << "Connection failed: " << modbus_strerror(errno);
        modbus_free(ctx);
        ctx = nullptr;
        return false;
    }

    LOG(INFO) << "Pressure Sensor Connected on " << portName;
    m_connected = true;
    m_running = true;

    // 5. 启动后台采集线程
    m_thread = std::thread(&PressureSensor::pollingLoop, this);

    return true;
}

void PressureSensor::disconnectDevice()
{
    m_running = false;

    if (m_thread.joinable()) {
        m_thread.join();
    }

    if (ctx) {
        modbus_close(ctx);
        modbus_free(ctx);
        ctx = nullptr;
    }
    m_connected = false;
    LOG(INFO) << "Pressure Sensor Disconnected";
}

float PressureSensor::getLatestPressure() const
{
    return m_currentPressure.load();
}

bool PressureSensor::isConnected() const
{
    return m_connected.load();
}

void PressureSensor::pollingLoop()
{
    uint16_t tab_reg[64]; // 接收缓冲区

    while (m_running)
    {
        int rc = modbus_read_registers(ctx, READ_ADDR, READ_LEN, tab_reg);

        if (rc != -1) {
            // --- [新增诊断代码] ---
            // 打印原始寄存器值，这样我们就能手动算一算到底对不对
            // 只要看日志里的 Hex，就知道是 20.0 还是 0.0
            static int printCount = 0;
            if (printCount++ % 50 == 0) { // 防止刷屏，每50次打印一次
                LOG(INFO) << "HEX RAW: "
                          << std::hex << tab_reg[0] << " " << tab_reg[1]
                          << std::dec;
            }
            // ---------------------

            // 尝试不同的解析方式（如果 Hex 不是 0 但解析结果是 0，就换这个）
            float val = modbus_get_float_abcd(tab_reg);
            // float val = modbus_get_float_cdab(tab_reg); // 备选方案
            // float val = modbus_get_float_badc(tab_reg); // 备选方案
            // float val = modbus_get_float_dcba(tab_reg); // 备选方案

            m_currentPressure.store(val);
        }
        usleep(20 * 1000);
    }
}
