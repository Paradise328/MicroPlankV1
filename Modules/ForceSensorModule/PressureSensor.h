#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include <modbus/modbus.h> // 必须安装 libmodbus-dev
#include "../LoggerModule/easylogging++.h"

class PressureSensor
{
public:
    explicit PressureSensor();
    ~PressureSensor();

    // 初始化：默认波特率改为 9600 (根据您的截图)
    bool initDevice(const char* portName = "/dev/ttyUSB0", int baudRate = 9600);

    void disconnectDevice();

    // 获取最新的压力值
    float getLatestPressure_1() const;
    float getLatestPressure_2() const;
    // 获取连接状态
    bool isConnected() const;

    // 设置小数位系数 (例如仪表显示2位小数，系数设为100.0)
    void setScaleFactor(float factor);

private:
    // 后台轮询线程
    void pollingLoop();

    modbus_t* ctx;
    std::atomic<bool> m_running;
    std::atomic<bool> m_connected;
    std::thread m_thread;

    std::atomic<float> m_currentPressure_1;
    std::atomic<float> m_currentPressure_2;
    std::atomic<float> m_scaleFactor;

    // RDD-DG 变送器参数 (根据截图确认)
    const int SLAVE_ID = 1;        // 01
    const int READ_ADDR = 0x01C2;  // 00 50 (寄存器起始地址)
    const int READ_LEN = 4;        // 00 02 (读取2个寄存器 = 32位)
};

#endif // PRESSURESENSOR_H
