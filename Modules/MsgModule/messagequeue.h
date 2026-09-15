#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QObject>

#include<QHash>
#include <QMetaType>
#include "threadsafequeue.h"
#include <QDebug>

enum class _RobotControlAction_E
{
    BootSelfCheck = 0x00,
    StartControlThread = 0x01,
    GoToTeleOperationMode,
    GoToCollaborationMode,
    GoToGuidingArmCollaboration,
    DisableGuidingArmCollaboration,
    GoToInstrumentArmCollaboration,
    GoToTestOperation,
    GoToHoldOnMode,
    StartEndJointTransportHoming,
    StartEndJointMotorHoming,
    StartEndEffectorMotorHoming,
    SwitchInstrumentAngle,
    MotorOff,
    Update_EndeffectorData,
    StartInstrumentTest = 0x100
};
using RobotControlAction_E = enum _RobotControlAction_E;

enum class _AssistDevice_LiftingAction_E
{
    BootSelfCheck=0x10,
    LiftingDriverSwitch=0x11,
    LiftingUp=0x12,
    LiftingDown=0x13,
    LiftingBrake=0x16,
    FootSupportUp=0x17,
    FootSupportDown=0x18,
    FootSupportBrake=0x19,
};
using AssistDevice_LiftingAction_E = enum _AssistDevice_LiftingAction_E;

enum class _AssistDevice_ArmAction_E
{
    BootSelfCheck=0x20,
    ArmDriverSwitch=0x21,
};
using AssistDevice_ArmAction_E = enum _AssistDevice_ArmAction_E;

enum class _MotorDriverAction_E
{
    BootSelfCheck = 0x30,
    EtherCAT_Off,
    EtherCAT_On,
    Motor_Lock,
    Motor_Unlock,
};
using MotorDriverAction_E = enum _MotorDriverAction_E;

enum class _AssistDevice_LightBoardManagementAction_E
{
    BootSelfCheck=0x60,
    CheckInstrumentInfomation,
    EndCheckInstrumentInfomation,
    TurnOffLight
};
using AssistDevice_LightBoardManagementAction_E = enum _AssistDevice_LightBoardManagementAction_E;

enum class _UIAction_E
{
    /*RecvMasterData=0x30,
    RecvSlaveStatus,
    FinishCalibration,
    RecvSystemBootSta,
    RecvModuleStatus*/


    RecvMasterData=0x30,
    RecvSlaveStatus,
    FinishCalibration,
    RecvInstrumentData,
    RecvLightBoardInTrolleyData,
    Emergency,
    SetFilterCase,
    SetPosScaling,
    FinishCalibrationOneSide,
    RecvSystemBootSta,
    RecvModuleStatus,
    InstrumentTestStatus = 0x101

};
using UIAction_E = enum _UIAction_E;

enum class _MasterConsoleAction_E
{
    BootSelfCheck=0x40,
    MasterShutDown,
    SwitchInstrumentAngle,
    Open3rdFilter,
    Open2rdFilter,
};
using MasterConsoleAction_E = enum _MasterConsoleAction_E;

enum class _SecurityAction_E
{
    RecvBootSelfCheckStatus = 0x50,
    RecvModulesStatus,
    RecvMotorDriverShutDown,
    RecvMasterConsoleShutDown,
    PowerOff,
    RecvEthercatStatus,
};
using SecurityAction_E = enum _SecurityAction_E;

enum class _Voice_Action{
    VOICE_RecvMasterData=0xC1,
    VOICE_GoToIO,
    VOICE_GoToPO,
    VOCIE_SelfCheck,
    VOCIE_Calibration,
};
using Voice_Action = enum _Voice_Action;

enum class _MultipleDevAction_E
{
    RecvLightBoardIoState= 0xA1,
    RecvFinalData=0xA2,
    RecvLiftingBootSta=0xA3,
    RecvMasterBootSta=0xA4,
    RecvLeftFinialBootSta=0xA5,
    RecvRightFinialBootSta=0xA6,
    RecvHorizontalArmBootSta=0xA7,
    RecvSystemBootSta=0xA8,
    RecvSystemWarning=0xA9,
};
using MultipleDevAction_E = enum _MultipleDevAction_E;

enum class _Module_Inner_e
{
    MasterConsole,
    RobotControl,
    MotorDriver,
    Uiinterface,
    Security,
    AssistDevice_Lightboard,
    AssistDevice_Lifting,
    AssistDevice_Arm,
    AssistDevice_LightBoardManagement,
    Audio,
    MultipleModules,
};
using Module_Inner_E = enum _Module_Inner_e;

struct _Message_Inner_t
{
    Module_Inner_E Sender;
    Module_Inner_E Recver;
    QHash<int,QString> Request;
};
using Message_Inner_T = struct _Message_Inner_t;

class MessageQueue : public ThreadSafeQueue<Message_Inner_T>
{
   // Q_OBJECT
public:
//    MessageQueue();
    explicit MessageQueue(QObject *parent = nullptr);//{};

    void SendMessage(const Message_Inner_T &msg);
    Message_Inner_T GetMessage(void);
signals:

};
//Q_DECLARE_METATYPE(Message_Inner_T);

#endif // MESSAGEQUEUE_H
