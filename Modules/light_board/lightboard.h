#ifndef LIGHTBOARD_H
#define LIGHTBOARD_H
#include "Modules/PeripheralDeviceModule/peripheral_device.h"

//#include "peripheral_device.h"
#include "Modules/MsgModule/messagequeue.h"
#include "lightBoardDataStruct.h"

#include <QtConcurrent>



struct _InstrumentStatus
{
    QString         connectionStatus_L;
    QString         connectionStatus_R;
    QString         type_L;
    QString         type_R;
    QString         size_L;
    QString         size_R;
    int             ID_L;
    int             ID_R;

    int             usedTime_L;
    int             usedTime_R;
    void initAll()
    {
        connectionStatus_L = "false";
        connectionStatus_R = "false";
        type_L = "None";
        type_R = "None";
        size_L = "None";
        size_R = "None";
        ID_L = 0;
        ID_R = 0;
        usedTime_L = 0;
        usedTime_R = 0;
    }
    void init_L()
    {
        connectionStatus_L = "false";
        type_L = "None";
        size_L = "None";
        ID_L = 0;
        usedTime_L = 0;
    }
    void init_R()
    {
        connectionStatus_R = "false";
        type_R = "None";
        size_R = "None";
        ID_R = 0;
        usedTime_R = 0;
    }
    };
using InstrumentStatus = struct _InstrumentStatus;

// typedef enum eLightColor
// {
//       LIGHT_COLOR_NONE=0,
//    LIGHT_COLOR_RED=0xC1,
//    LIGHT_COLOR_GREEN=0xC2,
//    LIGHT_COLOR_YELLOW=0xC3
// }eLightColor;

// typedef enum eLightModel
// {
//    LIGHT_MODEL_ON=0xA1,
//    LIGHT_MODEL_BLINK=0xA2,
//    LIGHT_MODEL_OFF=0xA3
// }eLightModel;

class LightBoard : public Peripheral_Device
{
    Q_OBJECT
    QHash<QString,eLightBoardActions> Qhash_Cmd_Classify;
    QHash<QString,eReadDataType> Qhash_ReadDataTyp_Classify;
    QHash<QString,eLightColor> Qhash_LightColor_Classify;
    QHash<QString,eLightModel> Qhash_LightModel_Classify;
    std::atomic<ReadFianlInfoStep >m_readFinalStep;
    FinalDataRead_t m_finalDataTem;
public:
    LightBoard(MessageQueue&  messagePool);
    explicit LightBoard(QString ip,quint16 port,MessageQueue&   messagePool);
    void BootSelfCheck();

    void LCMD(QString cmd,eFinalSide Finalside=eFinalSideUNSED,int arg2=-1,int arg3=-1,int arg4=-1,int arg5=-1);
    void DataIn(QByteArray data);

    void SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
    void dealWithMessage(Message_Inner_T msg);
    void StartBootSelfCheckThread(void);
    void ReadFianlAllInformation(void);
    void setMyInstrumentStatus(const uint8_t& instrumentType, const u_int16_t& usedCounter, const char& side);
    void setMyLiftingRockerStatus(const uint8_t& statusTmp);

    void updateCounter(const char& side);

    InstrumentStatus returnMyInstrumentStatus(){return m_InstrumentStatusBoth;}
    int returnMyLiftingRockerStatus(){return m_LiftingRockerStatus;}
    void setLightBoardColor(eLightColor color, eLightModel model);
    void CommTimeoutTimer_Handle();
public slots:
    void askMyInstrumentStatus();
private:
    std::mutex      Lock_LCMD;
    MessageQueue&   m_messagePool;
    QThread m_BootSelfCheckThread;
    InstrumentStatus m_InstrumentStatusBoth;
    int     m_changeCount = 0;
    int     m_LiftingRockerStatus;
    eLightColor     m_LightBoardColor = LIGHT_COLOR_GREEN;
    eLightModel     m_LightBoardModel = LIGHT_MODEL_BLINK;
    int     m_UpdateCounter_L = 0;
    int     m_UpdateCounter_R = 0;
    QTimer  m_CommTimeOutTimer;

signals:
    void IOValUpdate(uint8_t IOVal);
    void FinalDataUpdate(FinalDataRead_t FinalData);
    void SendMsgSignal(const Message_Inner_T msg);
    void updateLiftingRockerStatus();

};

Q_DECLARE_METATYPE(uint8_t);
Q_DECLARE_METATYPE(FinalDataRead_t);

#endif // LIGHTBOARD_H
