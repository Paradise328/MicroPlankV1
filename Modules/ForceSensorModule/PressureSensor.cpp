#include "PressureSensor.h"
#include <unistd.h>
#include <cmath>
#include <cstring>

PressureSensor::PressureSensor()
    : ctx(nullptr), m_running(false), m_connected(false),
    m_currentPressure(0.0f), m_scaleFactor(100.0f) // 默认假设有2位小数(除以100)
{
}

PressureSensor::~PressureSensor()
{
    disconnectDevice();
}

bool PressureSensor::initDevice(const char* portName, int baudRate)
{
    // 1. 创建 Modbus RTU 实例
    // 根据您的截图：9600 波特率, 'N' 无校验, 8 数据位, 1 停止位
    int fixedbaudRate = 9600;
    ctx = modbus_new_rtu(portName, fixedbaudRate, 'N', 8, 1);

    if (ctx == nullptr) {
        LOG(ERROR) << "Unable to create the libmodbus context";
        return false;
    }

    // 2. 设置超时时间 (500ms)
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

    LOG(INFO) << "RDD-DG Pressure Sensor Connected on " << portName << " Baud:" << baudRate;
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

// 之前报错缺少的函数，现在补上了
void PressureSensor::setScaleFactor(float factor)
{
    if(factor > 0.0001f) {
        m_scaleFactor.store(factor);
        LOG(INFO) << "Pressure Sensor Scale Factor set to: " << factor;
    }
}

void PressureSensor::pollingLoop()
{
    uint16_t tab_reg[64]; // 接收缓冲区

    while (m_running)
    {
        // 发送指令: 01 03 00 50 00 02 [CRC]
        int rc = modbus_read_registers(ctx, READ_ADDR, READ_LEN, tab_reg);

        if (rc != -1) {
            // --- RDD-DG 数据解析 (根据截图验证) ---
            // 截图返回: 01 03 04 [00 00] [00 00] ...
            // 前两个字节是高位，后两个字节是低位

            // 1. 拼接成 32位 整数 (Int32)
            // tab_reg[0] 是高16位，tab_reg[1] 是低16位
            int32_t rawValue = (static_cast<int32_t>(tab_reg[0]) << 16) | tab_reg[1];

            // 2. 转换为物理值
            // 如果 rawValue 是 2000，系数是 100.0，则结果是 20.00
            float finalVal = static_cast<float>(rawValue) / m_scaleFactor.load();

            m_currentPressure.store(finalVal);

        } else {
            // 读取失败，通常是因为超时或线松了
            LOG(WARNING) << "Modbus Read Failed";
        }

        // 采样频率：20ms (50Hz)
        usleep(20 * 1000);
    }
}
