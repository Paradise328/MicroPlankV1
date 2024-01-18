#ifndef VIPER_H
#define VIPER_H


#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>
#include <unistd.h>
#include <QtDebug>
#include <QThread>
#include <QTimer>
#include <cstring>
#include <QMutex>

#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialBus/QModbusDevice>
#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QtSerialBus/QModbusDataUnit>
#include <QtSerialBus/QModbusClient>

/*
apt-get install libqt5serialport5
apt-get install libqt5serialbus5
apt-get install libqt5serialport5-dev
apt-get install libqt5serialbus5-dev

*/
#include "ViperInterface.h"

const uint32_t CRC_SIZE=sizeof(uint32_t);




typedef struct
{
    float sensor1_pos[7];
    float sensor2_pos[7];



}SensorData;
typedef struct
{
    float Handle1[8];
    float Handle2[8];
}Posture;



using namespace std;

class Viper:public QObject

{
    QByteArray Revdata;
    QByteArray adcdata;
    QSerialPort *m_serial = nullptr;
    QSerialPort *n_serial = nullptr;
    SensorData Sensor_Data,Org_Data;
    float Open_Angle,Org_Angle;

//    int SensorNum;
    int i_counter =0;
    float twizzer1_l = 10;
    float twizzer1_d = 0.4;
    Posture current_Post,init_Post;



public:
    Viper();
    float QbyteArrayToFloat(QByteArray arr,unsigned char StartIndex);
    void OnRespon();
    void OnRespon_4();
    void openSerialPort();
    void openSerialPort_4();
    void handleError(QSerialPort::SerialPortError error);
    void handleError_4(QSerialPort::SerialPortError error);
    QMutex m_mutex,m_mutex1;

    void Viper_wait(int wait_time);
    void init_OrgData();//set the Orgpoint
    SensorData get_OrgData();//get Org Data;

//    void init_H_OrgPost(SensorData data);
//    Posture get_H_CurPost(SensorData data){return calc_H_Post(data);};
    Posture return_H_CurPost(){return this->current_Post;};
    Posture return_H_InitPost(){return this->init_Post;};
    Posture return_H_control_InitPost(){return this->control_ini_Post;};
    Posture calc_H_Post(SensorData data,float open_angle);
    Posture control_ini_Post;
    float control_ini_Angle;
    void set_SerialCfg();
    void set_Unit();
    void send_CMD(const QByteArray &cmd);
    void ReadAdc();
    void get_PNO();

    float return_OA();
    SensorData return_PNO();
};




#endif // VIPER_H
