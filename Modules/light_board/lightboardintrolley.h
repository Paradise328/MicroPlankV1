#ifndef LIGHTBOARDINTROLLEY_H
#define LIGHTBOARDINTROLLEY_H
#include "../PeripheralDeviceModule/peripheral_device.h"
#include "../MsgModule/messagequeue.h"
#include <QtConcurrent>
#include "lightBoardDataStruct.h"

typedef enum LightBoardInTrolleyStatus
{
    NORMAL = 0x101,
    WARNING,
    ERROR,
}LightBoardInTrolleyStatus;

class LightBoardInTrolley : public Peripheral_Device
{
    Q_OBJECT
    QHash<QString,eLightBoardActions> Qhash_Cmd_Classify;
    QHash<QString,eReadDataType> Qhash_ReadDataTyp_Classify;
    QHash<QString,eLightColor> Qhash_LightColor_Classify;
    QHash<QString,eLightModel> Qhash_LightModel_Classify;
    std::atomic<ReadFianlInfoStep >m_readFinalStep;
    FinalDataRead_t m_finalDataTem;
public:
    LightBoardInTrolley(MessageQueue&  messagePool);
    explicit LightBoardInTrolley(QString ip, quint16 port, MessageQueue& messagePool);

    void    LCMD(QString cmd,eFinalSide Finalside=eFinalSideUNSED,int arg2=-1,int arg3=-1);
    void    DataIn(QByteArray data);
    LightBoardInTrolleyStatus    returnLightBoardStatus();
private:
    MessageQueue&   m_messagePool;
    LightBoardInTrolleyStatus m_LightBoarInTrolleyStatus;
    void    initStatus();
    void    changeStatus(const LightBoardInTrolleyStatus& status);

signals:
    void    IOValUpdate(uint8_t IOVal);
    void    FinalDataUpdate(FinalDataRead_t FinalData);
    void    SendMsgSignal(const Message_Inner_T msg);

};
//Q_DECLARE_METATYPE(uint8_t);

#endif // LIGHTBOARDINTROLLEY_H
