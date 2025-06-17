#ifndef FORCESENSOR_H
#define FORCESENSOR_H

#include <QSerialPort>
#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QTimer>
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <array>
#include <vector>
#include <unistd.h>
#include <mutex>
#include "../LoggerModule/easylogging++.h"

class ForceSensor: public QObject
{
    Q_OBJECT

public:
    explicit ForceSensor(QObject* parent = nullptr);
    ~ForceSensor();

    double F_x;
    double F_y;
    double F_z;
    double M_x;
    double M_y;
    double M_z;
    QByteArray              Data422Recvin;

    double getFx() const;
    double getFy() const;
    double getFz() const;
    double getMx() const;
    double getMy() const;
    double getMz() const;

    void initDevice();
    void startForceSensor();
    void stopForceSensor();
    void readForceSensorOnce(); /* NOT USED YET */

    bool parseFrame(const QByteArray& frame);
    float readFloatLE(const char* ptr);
    // static void* readForceSensorCyclic(void* arg);

    std::array<double,6> getForceSensor();

signals:
    void forceUpdated(); // 数据更新时发出信号，可连接 UI 刷新等操作

private slots:
    void handleReadyRead(); // 串口数据到达时调用
    void processBuffer();   // 分帧处理缓存数据

private:
    pthread_t threadId;
    QByteArray m_buffer;

    std::atomic<bool> running;  /*flag to mark if the sensor is running*/
    mutable std::mutex m_dataMutex;

    int baudRate;
    bool openSerialPort(qint32 baudrate);
    void processData(const QByteArray& data);
    std::thread readForceSensorThread;
    void On422DataIn(void);
    QSerialPort             *m_serial_forceSensor = nullptr;
};




#endif
