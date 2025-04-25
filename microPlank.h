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
// #include "Modules/RobotControlModule/RobotControl.h"

class MicroPlank:public QObject
{
public:
    MicroPlank() = delete;
    explicit MicroPlank(QGuiApplication &app,
                        const MasterConsoleType& MasterConsoleType,
                        const MotorDriverParameter& motorDriverParameter):
                        m_app(app),
                        m_masterConsoleType(MasterConsoleType),
                        m_motorDriverParameter(motorDriverParameter)
                        {
                            connect(&m_uiInterface, &UIinterface::startWholeSystemSignal,this, &MicroPlank::startStarSystemThread);

//                            initLoggerConfig();

                         };

    void        startStarSystemThread();
public slots:
    void        onSendMeg(Message_Inner_T &msg);

private:

    QGuiApplication   &m_app;

    MessageQueue      m_MsgPool = MessageQueue(nullptr);

    MotorDriverParameter m_motorDriverParameter;

    std::thread       m_msgThread;

    void              startSystem();

    std::thread       m_startSystemThread;

    MasterConsoleType   m_masterConsoleType;

    MasterConsole       m_masterConsole = MasterConsole(m_masterConsoleType, m_MsgPool);

    Security            m_security = Security(m_MsgPool);

    UIinterface         m_uiInterface = UIinterface(m_app, m_MsgPool);

    MotorDriver*        m_motorDriver = new MotorDriver(m_motorDriverParameter, m_MsgPool);//为一个指针

    RobotControl        m_robotControl = RobotControl(m_masterConsole, m_motorDriver, m_MsgPool);

    bool                flagMsgPool = true;

    std::atomic<bool>   flagRemoteControl = false;

    void                startMasterConsole();

    void                startSecurityModule();

    std::thread         m_initMotorDriverThread;

    void                initMotorDriver();//std::promise<bool> &promiseInitMotorDriver

    void                initMotorDriverThread();//std::promise<bool> &promiseInitMotorDriver
    void                startRobotControl();

    void            initLoggerConfig();
    void            startMsgThread();
    void            messagePoll();
    void            setRemoteControlFlag(bool startRemoteControlflag);


};

#endif // MICROPLANK_H
