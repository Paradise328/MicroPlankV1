#ifndef LIGHTBOARDMANAGEMENT_H
#define LIGHTBOARDMANAGEMENT_H

#include <QObject>
// #include <QtWidgets/QWidget>
#include <atomic>
#include "../PeripheralDeviceModule/peripheral_device.h"
#include "../MsgModule/messagequeue.h"
#include "lightboard.h"
#include "lightboardintrolley.h"
#include "Modules/RobotControlModule/DomainController.h"
#include "../SystemUtilsModule/SystemUtils.h"
#include <QMetaType>
class LightBoardManagement: public QObject
{
    Q_OBJECT
public:
    explicit LightBoardManagement(MessageQueue& messagePool);
    void GetAmMsg(Message_Inner_T msg);

private:
    MessageQueue& m_messagePool;
    LightBoard m_LightBoardOnBeam = LightBoard("192.168.42.82",8080,m_messagePool);
    // LightBoardInTrolley m_LightBoardInTrolley = LightBoardInTrolley("192.168.42.82",8080,m_messagePool);
    DomainController*  m_domainController = new DomainController();
    QThread m_BootSelfCheckThread;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock  m_MsgGottenRWLock;
    QString     m_InstrumentType_L;
    QString     m_InstrumentType_R;

    std::atomic<int>  m_readInstrumentTypeFlag;
    std::atomic<int>  m_readInstrumentUsingTimeFlag;
    InstrumentStatus  m_InstrumentStatus_Prev;

    mutable int       m_InstrumentInitUsingTime_L = 0;
    mutable int       m_InstrumentInitUsingTime_R = 0;
    mutable int       m_InstrumentCurUsingTime_L = 0;
    mutable int       m_InstrumentCurUsingTime_R = 0;
    mutable int       m_usedTimeCounter_L = 0;
    mutable int       m_usedTimeCounter_R = 0;

    std::thread m_readInstrumentStatusThread;
    void startGetLightBoardStatus();
    void endGetInstrumentStatus();
    void getLightBoardStatus();

    std::thread m_askInstrumentStatusThread;
    void startAskInstrumentStatusThread();
    void endAskInstrumentStatusThread();
    void askInstrumentStatusThread();

    void readInstrumentLightBoardStatus();
    void updateInstrumentInformation(const InstrumentStatus& instrumentStatusCur, const InstrumentStatus& instrumenTypePrev, const char& side);
    void readTrolleyLightBoardStatus();
    void readInstrumentStatus();
    void sendRockerStatus();

    void SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
    void dealWithMsg();
    void StartBootSelfCheckThread(void);

signals:
    void IOValUpdate(uint8_t IOVal);
    void FinalDataUpdate(FinalDataRead_t FinalData);

    void SendMsgSignal(const Message_Inner_T msg);
    void DealMsgSignal();

    void askInstrumentStatusSignal();
    void updateCounterInfoSignal(const char& side);

};



#endif // LIGHTBOARDMANAGEMENT_H
