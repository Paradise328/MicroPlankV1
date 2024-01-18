#ifndef HANDLE_H
#define HANDLE_H


#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>
#include <unistd.h>
#include <QtDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialBus/QModbusDevice>
#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QtSerialBus/QModbusDataUnit>
#include <QtSerialBus/QModbusClient>
#include <QThread>
#include <QTimer>
#include <QSerialPort>
#include <cstring>
#include <QMutex>
#include "ViperInterface.h"
#include "viper_transmitter.h"
const uint32_t CRC_SIZE=sizeof(uint32_t);

#define SetBaud 0


typedef struct
{
    float sensor1_pos[7];
    float sensor2_pos[7];
}SensorData;


typedef struct
{
    float Handle1[8];
    float Handle2[8];
    int Pedal_Speed_Index;
    int enable_pedal_l;
    int enable_pedal_r;

    int grasp_index_l;
    int grasp_index_r;
}Posture;



using namespace std;

class Handle:public QThread

{
    Q_OBJECT;
    QByteArray Revdata;
    QByteArray adcdata;
    QSerialPort *m_serial = nullptr;
    QSerialPort *n_serial = nullptr;




public:
    Handle();

    SensorData Sensor_Data,Org_Data;
    float Org_Angle_L,Org_Angle_R;
    int i_counter,H_Speed_Pedal;
    Posture current_Post,init_Post;


    float QbyteArrayToFloat(QByteArray arr,unsigned char StartIndex);
    void OnRespon();
    void openSerialPort(qint32 baud);
    void handleError(QSerialPort::SerialPortError error);
    QMutex m_mutex,m_mutex1;
    VIPER_Transmitter *V;

    static void Handle_wait(int wait_time);
    void init_OrgData();//set the Orgpoint
    SensorData get_OrgData();//return Org Data;
    SensorData return_PNO();//return current Sensor Data.

    Posture return_H_CurPost(){return this->current_Post;};
    Posture return_H_InitPost(){return this->init_Post;};
    Posture return_H_control_InitPost(){return this->control_ini_Post;};
    Posture calc_H_Post(SensorData data);
    Posture control_ini_Post;
    void set_SerialCfg();
    void set_Unit();
    void send_CMD(const QByteArray &cmd);
    void get_PNO();



signals:
    void Viper_Data_Update();




};



#endif // HANDLE_H
