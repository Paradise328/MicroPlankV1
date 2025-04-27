#ifndef PERIPHERAL_DEVICE_H
#define PERIPHERAL_DEVICE_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QHostInfo>
#include "CommunicationProtol.h"
#include "CRC16.h"
#include <QTime>
#include <QThread>
#include <QCoreApplication>
#include <QMetaType>
#include <QTimer>
#include <QList>
#include <QtGlobal>
#include <QRandomGenerator>
//#include "easylogging++.h"
#include "../LoggerModule/easylogging++.h"


#define CON_COUNT_MAX 3
typedef enum ePeriDevConnSta
{
    DISCONNECTED = 0,
    CONNECTED=1,
} ePeriDevConnSta;

typedef enum eSendReturn
{
    SEND_SUCCESS = 0,
    SEND_ERROR=1,
} eSendReturn;

typedef enum eDepackReturn
{
    Depack_SUCCESS = 0,
    Depack_ERROR=1,
} eDepackReturn;

typedef struct _Res_Sta
{
    QByteArray Frame;
    uint8_t WaitTime;//等待回应时间
    uint8_t ReSendLeft;//剩余重发次数
}Res_Sta;


typedef struct _COM_ITEM
{
   QString CMD1;
   QString CMD2;
   int argsize;
   void *argsp;
} COM_ITEM;


class Peripheral_Device :public QThread
{
    Q_OBJECT
public:
    Peripheral_Device();
    Peripheral_Device(QString ip,quint16 port);
    QString Ip;
    QString name;

    quint16 Port;
    quint16 m_socketport;

    QTcpSocket *m_socket=nullptr;

    eDeviceType type;
    ePeriDevConnSta ConnSta;
    QHash<uint32_t,Res_Sta> Qhash_Res;
    QList<QTcpSocket*>ConList;//为增加通信鲁棒性，建立多个连接
    QList<quint16>ReconnPortList;
    QTimer *ConnTimer=nullptr;
    QTimer *ReconnTimer=nullptr;
    int PortOffset=0;
    QByteArray  DataRecin;
    bool IsNewDataIn=false;

    void Datain_CallBack(QByteArray ba);
    eSendReturn Send_Data(uint8_t data[],qint32 len);
    eSendReturn Send_Data(QByteArray data_qba);
    eSendReturn Send_Frame(COMMU_FRAME cftemp,uint8_t ReSendTimes=3,bool NeedAck=false);
    void on_Data_In(void);
    void GetData(QByteArray ba);

    void on_Connected(void);
    void on_Disconnected(void);
    eDepackReturn Depack_Frame(QByteArray data,COMMU_FRAME &cf);
    uint8_t Packet_Frame(QByteArray &datap,COMMU_FRAME cftemp);
    static uint32_t GetRandNum(void);
    bool Change_Usable_Con();
    void run() override;

    void ConnTimer_Callback(void);
    void ReconnTimer_Callback(void);
    void DynamicsDelay(int nms);

signals:
    void Resend(QByteArray& dat);
    void FlushConns(void);
    void NewDataIn(QByteArray& dat);
//    void NewDataIn(void);

public slots:
    void onResend(QByteArray& dat);
    void OnFlushConns(void);
};
Q_DECLARE_METATYPE(QByteArray);

#endif // PERIPHERAL_DEVICE_H
