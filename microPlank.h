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
#include "Modules/MasterModule/MasterConsole.h"
#include "Modules/MathModule/include/MathUtils.h"
#include "Modules/MotorDriverModule/include/MotorDriver.h"
#include "Modules/RobotModule/include/RobotKinematics.h"
#include "Modules/SystemUtilsModule/include/SystemUtils.h"


class MicroPlank
{
public:
    MicroPlank();
    MicroPlank(const MasterConsoleType& MasterConsoleType, 
               const std::string& robotConfigPath):
               m_masterConsole(MasterConsoleType),
               m_robotKinematics(robotConfigPath)
               {};

    void        startSystem();

private:
    MasterConsoleType   m_masterConsoleType;
    MasterConsole       m_masterConsole;
    MotorDriver*        m_motorDriver;
    RobotKinematics     m_robotKinematics;
    std::thread         m_teleoperationThread;
    void        initLoggerConfig();
    void        initMotorDriverThread();
    void        initMotor();
    void        startTeleoperationControlThread();
    void        teleoperationControlThread();
    void        startMasterConsoleThread();

};

#endif // MICROPLANK_H