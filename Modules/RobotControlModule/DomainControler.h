#ifndef DOMAIN_CONTROLER_H
#define DOMAIN_CONTROLER_H

#include "../PeripheralDeviceModule/peripheral_device.h"
#include <atomic>
#include <QSerialPort>
#include <iostream>
#include <stdio.h>
#include <fstream>
//#include "easylogging++.h"
//include "../LoggerModule/easylogging++.h"
//#include "MasterHandleDataStruct.h"
#include <eigen3/Eigen/Dense>





class DomainControler : public Peripheral_Device
{
public:
    DomainControler(uint32_t ID=0);
    DomainControler(QString ip,quint16 port,uint32_t ID=0);
    void VCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);
    void SendTestFrame(void);
    void StartContinus();
    void reset();
    void read_Write_Data();
    float Uint8ArrToFloat(uint8_t *arr,unsigned char StartIndex);
    bool isSelfCheck(){return this->SelfCheckOK;}
    void openSerialPort(qint32 baud);
    void closeSerialPort();
    int findFrameHead(QByteArray &data);
    void On422DataIn(void);
    eSendReturn Send_Frame_By_422(COMMU_FRAME cftemp);


    void                readHandleOtherData(QByteArray qba);
    std::array<double,2> returnOpenAngle();

     std::atomic<std::array<double,2>> m_Openangle;

     uint32_t MagneticScale_Counter=0;

     float ForceSensor_Fx=0;
     float ForceSensor_Fy=0;
     float ForceSensor_Fz=0;
     float ForceSensor_Mx=0;
     float ForceSensor_My=0;
     float ForceSensor_Mz=0;

     uint8_t DI_val=0;

private:
    bool                SelfCheckOK = false;
    uint32_t            m_ID=0;
    QSerialPort *m_serial_422 = nullptr;
    QByteArray  Data422Recvin;
    QHash<QString,eDomainControler_Actions> Qhash_Cmd_Classify;

    int                 m_Counter;

};

#endif // DOMAIN_CONTROLER_H
