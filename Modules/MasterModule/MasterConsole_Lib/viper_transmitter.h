#ifndef VIPER_TRANSMITTER_H
#define VIPER_TRANSMITTER_H
#include "../../PeripheralDeviceModule//peripheral_device.h"
#include "../../LoggerModule/easylogging++.h"
#include "../../SystemUtilsModule/SystemUtils.h"
#include "../Viper_Lib/viper_ui.h"
#include <eigen3/Eigen/Dense>
#include <atomic>
#include <QSerialPort>
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <atomic>

class Viper_Transmitter : public Peripheral_Device
{

public:

    explicit Viper_Transmitter():
        m_communicateTemp(0),
        m_isMonitorTerminated(false)
        {
            this->type=DEV_VIPER_TRANSMITTER;
            this->Qhash_Cmd_Classify.insert("GETSINGLE",VIPER_TRANSMITTER_GET_SINGLE_DATA);
            this->Qhash_Cmd_Classify.insert("RESETVIPER",VIPER_TRANSMITTER_RESET_VIPER);
            this->Qhash_Cmd_Classify.insert("RESETHANDLE",VIPER_TRANSMITTER_RESET_HANDLE);
            this->Qhash_Cmd_Classify.insert("STARTCONTINUS",VIPER_TRANSMITTER_START_CONTINUS);
            initDevice();
        }

    HandlePose              returnHandlePose(){return m_handlePoseData.load();}

    bool                    return422Status(){return m_is422Ok;}

private:

    void                    initDevice();
    /*开启viper连续发送模式*/
    viper_ui                m_viper;
    void                    StartContinus();
    void                    Reset_Viper(void);

    /*开启主控台串口, 进行422通信等相关操作*/
    std::thread             m_422Thread;
    QSerialPort             *m_serial_422 = nullptr;
    QByteArray              Data422Recvin;
    QHash<QString,eViper_Transmitter_Actions> Qhash_Cmd_Classify;
    bool                    openSerialPort(qint32 baud);
    void                    closeSerialPort();
    void                    DataIn(QByteArray data);
    void                    On422DataIn(void);
    eSendReturn             Send_Frame_By_422(COMMU_FRAME cftemp);
    void                    VCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);

    /*主控台状态监测线程*/
    std::atomic<uint32_t>   m_communicateTemp;
    uint32_t                m_communicateReserve;
    std::thread             m_statusMonitor;
    void                    statusMonitor();
    bool                    m_isMonitorTerminated;
    bool                    m_is422Ok = false;
    /*主控台数据*/
    std::atomic<HandlePose> m_handlePoseData;
    std::array<double, 2>   calculateOpenAngle(const uint16_t& adcValueL, const uint16_t& adcValueR)const;
    void                    readHandleData(QByteArray qba);
    void                    readHandleData_Quaternion(QByteArray qba);

    /*字符转换*/
    float                   Uint8ArrToFloat(uint8_t *arr,unsigned char StartIndex);

};

#endif // VIPER_TRANSMITTER_H
