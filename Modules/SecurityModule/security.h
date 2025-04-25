#ifndef SECURITY_H
#define SECURITY_H

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <iostream>
#include <thread>
#include <chrono>
#include "Modules/MsgModule/messagequeue.h"
#include "Modules/LoggerModule/easylogging++.h"
#include "Modules/RobotControlModule/RobotControl.h"

enum class _SelfCheckStepEnum
{
    No_Checking = 0,
    MasterConsole_Checking,
    AssistLifting_Checking,
    AssistArm_Checking,
    Slave_Checking,
    Respons_Waiting,
    AllOK,
    Err,
};
using SelfCheckStepEnum = _SelfCheckStepEnum;

enum class _SystemWarningStatus
{
    Normal = 0,
    LightWarning,
    MiddleWarning,
    HeavyWarning,
};
using SystemWarningStatus = _SystemWarningStatus;

enum class _shutDownSystemEnum
{
    No_ShutDown = 0xA0,
    EtherCATOff,
    MasterConsoleOff,
    Respons_Waiting,
    ShutDownFinish,
};
using shutDownSystemEnum = _shutDownSystemEnum;

class Security:public QObject
{
    Q_OBJECT
    friend class MasterConsole;
    friend class RobotControle;

public:
    Security(MessageQueue& messagePool);

    void        systemBootSelfCheck();

    void        dealWithMsg();
    void        GetAmMsg(Message_Inner_T msg);
    void        setSelfCheckStep(SelfCheckStepEnum selfcheckstep);

    /*外部开启系统监控线程*/
    void        startSystemMonitor(MasterConsole& masterConsole);

private:
    std::atomic<SelfCheckStepEnum> selfCheckStep;
    std::atomic<shutDownSystemEnum> checkShutDownSystem;
    bool        flagSelfCheck = true;
    std::atomic<bool>       flagShutDownSystem = true;

    SystemWarningStatus m_systemWarningStatus = SystemWarningStatus::Normal;

    /*监控所有模块运行状态*/
    bool            m_flagIsSystemTerminated = false;
    std::thread     m_systemMonitorThread;
    void            systemMonitor(MasterConsole& masterConsole);
    std::atomic<std::array<bool, SystemModuleNum>> m_systemModuleStatus;
    void            setModuleStatus(const bool& masterConsoleStatus, const bool& robotControlStatus, const bool& liftingArmStatus);

    /*设置系统当前状态*/
    void            setSystemStatus(SystemWarningStatus sta) ;

    /*消息队列相关函数*/
    MessageQueue    &m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock  m_MsgGottenRWLock;
    void            SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void            SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);

    /*关闭系统及进程*/
    void            KillProcess();
    void            shutDownSystem();

signals:
    void            SendMsgSignal(Message_Inner_T &msg);
    void            DealMsgSignal();
};

#endif // SECURITY_H
