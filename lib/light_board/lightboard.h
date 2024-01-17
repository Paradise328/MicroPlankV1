#ifndef LIGHTBOARD_H
#define LIGHTBOARD_H
#include "../../lib/peripheral_device/peripheral_device.h"

//#include "peripheral_device.h"

typedef enum eLightBoardActions
{
    LIGHTBOARD_ACTIONS_SET_COLOR=0x1C,
    LIGHTBOARD_ACTIONS_SET_MODEL=0x2C,
    LIGHTBOARD_ACTIONS_Read_Data=0x3C,
    LIGHTBOARD_ACTIONS_Final_Inc=0x4C
}eLightBoardActions;

typedef enum eReadDataType
{
    FianlProductDate=0xC1,
    FianlProductBat=0xC2,
    FianlType=0xC3,
    FianlConnectSta=0xC4,
    FianlCounter=0xC5,
    IOVal=0xC6,
}eReadDataType;

typedef enum eFinalSide
{
    eFinalSideUNSED=-1,
    eFinalLeft,
    eFinalRight
}eFinalSide;

typedef struct
{
  eFinalSide FinalSide;
  eReadDataType DataType;
  uint32_t data;
}FinalDataRead_t;

//typedef enum eLightColor
//{
//	  LIGHT_COLOR_NONE=0,
//    LIGHT_COLOR_RED=0xC1,
//    LIGHT_COLOR_GREEN=0xC2,
//    LIGHT_COLOR_YELLOW=0xC3
//}eLightColor;

//typedef enum eLightModel
//{
//    LIGHT_MODEL_ON=0xA1,
//    LIGHT_MODEL_BLINK=0xA2,
//    LIGHT_MODEL_OFF=0xA3
//}eLightModel;

class LightBoard : public Peripheral_Device
{
    Q_OBJECT
    QHash<QString,eLightBoardActions> Qhash_Cmd_Classify;
    QHash<QString,eReadDataType> Qhash_ReadDataTyp_Classify;
    QHash<QString,eLightColor> Qhash_LightColor_Classify;
    QHash<QString,eLightModel> Qhash_LightModel_Classify;
public:
    LightBoard();
    LightBoard(QString ip,quint16 port);

    void LCMD(QString cmd,eFinalSide Finalside=eFinalSideUNSED,int arg2=-1,int arg3=-1);
    void DataIn(QByteArray data);
signals:
    void IOValUpdate(uint8_t IOVal);
    void FinalDataUpdate(FinalDataRead_t FinalData);
};
Q_DECLARE_METATYPE(uint8_t);
Q_DECLARE_METATYPE(FinalDataRead_t);

#endif // LIGHTBOARD_H
