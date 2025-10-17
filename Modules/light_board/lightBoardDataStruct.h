#ifndef LIGHTBOARDDATASTRUCT_H
#define LIGHTBOARDDATASTRUCT_H
#include <iostream>


typedef enum eLightBoardActions
{
    LIGHTBOARD_ACTIONS_SET_COLOR=0x1C,
    LIGHTBOARD_ACTIONS_SET_MODEL=0x2C,
    LIGHTBOARD_ACTIONS_Read_Data=0x3C,
    LIGHTBOARD_ACTIONS_Final_Inc=0x4C,
    LIGHTBOARD_ACTIONS_RW=0x5C
}eLightBoardActions;

typedef enum eReadDataType
{
    FinalAllInformation=0xC0,
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

enum class _ReadFianlInfoStep
{
    NoneAction=0xA0,
    ReadFianlProductDate,
    ReadFianlProductBat,
    ReadFianlType,
    ReadFianlConnectSta,
    ReadFianlCounter,
    ResponsWaiting,
    ReadALlOK,
    ReadErr,
};

constexpr int size_8mm = 2;
constexpr int size_4mm = 1;

constexpr int type_CZQ = 1;
constexpr int type_XWJ = 2;

using ReadFianlInfoStep=_ReadFianlInfoStep;
#endif // LIGHTBOARDDATASTRUCT_H
