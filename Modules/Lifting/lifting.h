#ifndef LIFTING_H
#define LIFTING_H

#include "Modules/PeripheralDeviceModule/peripheral_device.h"
#include <QObject>
#include "Modules/MsgModule/messagequeue.h"
#include <QtConcurrent>
#include "Modules/SystemUtilsModule/SystemUtils.h"
#include "Modules/LoggerModule/easylogging++.h"

typedef enum eLiftingSta
{
    Lifting_STOP=0X00,
    Lifting_GOING_UP,
    Lifting_GOING_DOWN
}eLiftingSta;

typedef enum eLiftingSpeed
{
    Lifting_Slow=0X01,
    Lifting_Fast=0x02,
}eLiftingSpeed;

class Lifting : public Peripheral_Device
{
    Q_OBJECT
public:
    QHash<QString,eLiftingActions> Qhash_Cmd_Classify;
    QHash<QString,eLightColor> Qhash_LightColor_Classify;
    QHash<QString,eLightModel> Qhash_LightModel_Classify;
    QHash<QString,eLiftMotion> Qhash_Liftmotion_Classify;
    eLiftingSta LiftingSta;
    eLiftingSta FootSupportSta;
    QTimer *SendTimer=nullptr;
    QTimer *FootSupportSendTimer=nullptr;
    eLiftingSpeed LiftingSpeed=Lifting_Fast;
    eLiftingSpeed FootSupportSpeed=Lifting_Fast;
    bool SelfCheckOK=false;
    bool m_LiftingDriveEnable=false;
    MessageQueue&   m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock  m_MsgGottenRWLock;

    Lifting(MessageQueue& messagePool);
    Lifting(QString ip,quint16 port,MessageQueue& messagePool);

//    void LCMD(QString cmd);
    void LCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);

    void SendTimer_Callback(void);
    void FootSupportSendTimer_Callback(void);

    void LiftingUp(eLiftingSpeed spd);
    void LiftingDown(eLiftingSpeed spd);
    void LiftingBrake(void);

    void FootSupportUp(eLiftingSpeed spd);
    void FootSupportDown(eLiftingSpeed spd);
    void FootSupportBrake(void);

    void SelfCheck(void);
    void DataIn(QByteArray data);

    bool IsSelfCheckOk(void);

    void dealWithMsg();
    void GetAmMsg(Message_Inner_T msg);

    void BootSelfCheck(void);
    void SendInnerMsg(Module_Inner_E recever,int Action,QString arg);

    void SetLiftingDriveEnSta(bool en);

signals:
    void SendMsgSignal(const Message_Inner_T msg);
    void DealMsgSighal();

//    void LiftingUpFastSignal();
//    void LiftingUpSlowSignal();
//    void LiftingDownFastSignal();
//    void LiftingDownSlowSignal();
//    void LiftingBrakeSignal();
};

#endif // LIFTING_H
