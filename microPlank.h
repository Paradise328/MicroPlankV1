#ifndef MICROPLANK_H
#define MICROPLANK_H

#include <chrono>
#include <ratio>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <linux/input.h>
#include <unistd.h>
#include <queue>
#include <deque>
#include <exception>
#include <iostream>
#include <string>
#include <execinfo.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <regex>
#include <QObject>
#include <QThread>
#include <memory>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <ruckig/ruckig.hpp> //Trajectory planning
//#include "Modules/MasterModule/MasterConsole.h"
#include "Modules/MathModule/MathUtils.h"
#include "Modules/SystemUtilsModule/SystemUtils.h"
#include "Modules/MsgModule/messagequeue.h"
#include "Modules/SecurityModule/security.h"
#include "Modules/UIModule/UIinterface.h"
//#include "Modules/MotorDriverModule/MotorDriver.h"
#include "Modules/RobotControlModule/RobotControl.h"

class MicroPlank:public QObject
{

public:
    MicroPlank() = delete;
    explicit MicroPlank(QGuiApplication &app,
                        const MasterConsoleType& MasterConsoleType,
                        const MotorDriverParameter& motorDriverParameter):
                        m_app(app),
                        m_masterConsoleType(MasterConsoleType),
                        m_motorDriverParameter(motorDriverParameter),
                        m_isSystemTerminated(false)
                        {
                            connect(&m_uiInterface, &UIinterface::startWholeSystemSignal,this, &MicroPlank::startStarSystemThread);
                        }

    void        startStarSystemThread();

public slots:

    void        onSendMeg(Message_Inner_T &msg);

private:

    QGuiApplication     &m_app;

    MasterConsoleType   m_masterConsoleType;

    /*整机系统开启线程*/
    void                startSystem();

    std::atomic<bool>   m_isSystemTerminated;

    /*开启通信模块线程*/
    MessageQueue        m_MsgPool = MessageQueue(nullptr);

    std::thread         m_msgThread;

    void                startMsgThread();

    void                messagePoll();

    /*开启EtherCAT通讯线程*/
    MotorDriverParameter m_motorDriverParameter;

    std::thread         m_initMotorDriverThread;

    void                initMotorDriver();//std::promise<bool> &promiseInitMotorDriver

    void                initMotorDriverThread();//std::promise<bool> &promiseInitMotorDriver


    /*开启主手初始化线程*/

    void                startMasterConsole();

    /*开启安全模块线程*/
    void                startSecurityModule();


    /*开启机器人控制模块线程*/
    void                startRobotControl();


    /*各模块定义及初始化*/
    MasterConsole       m_masterConsole = MasterConsole(m_masterConsoleType, m_MsgPool);

    Security            m_security = Security(m_MsgPool);

    UIinterface         m_uiInterface = UIinterface(m_app, m_MsgPool);

    MotorDriver*        m_motorDriver = new MotorDriver(m_motorDriverParameter, m_MsgPool);//为一个指针

    RobotControl        m_robotControl = RobotControl(m_masterConsole, m_motorDriver, m_MsgPool);
};

#endif // MICROPLANK_H
