#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include <modbus/modbus.h> // 需要安装 libmodbus-dev
#include "../LoggerModule/easylogging++.h" // 假设您使用与RobotControl相同的日志系统

class PressureSensor
{
public:
    explicit PressureSensor();
    ~PressureSensor();

    // 初始化并连接传感器
    // portName: 例如 "/dev/ttyUSB0"
    // baudRate: 根据截图应为 115200
    bool initDevice(const char* portName = "/dev/ttyUSB0", int baudRate = 115200);

    // 断开连接
    void disconnectDevice();

    // 获取最新的压力值（线程安全，可直接在 targetPose 中调用）
    float getLatestPressure() const;

    // 获取连接状态
    bool isConnected() const;

private:
    // 后台读取线程函数
    void pollingLoop();

    // Modbus 上下文
    modbus_t* ctx;

    // 线程控制
    std::atomic<bool> m_running;
    std::atomic<bool> m_connected;
    std::thread m_thread;

    // 数据存储（原子操作，无需额外锁）
    std::atomic<float> m_currentPressure;

    // 传感器参数（根据您的 CuteCom 指令）
    const int SLAVE_ID = 1;        // <0x01>
    const int READ_ADDR = 0x0006;  // <0x00 0x06>
    const int READ_LEN = 2;        // <0x00 0x02>
};

#endif // PRESSURESENSOR_H
