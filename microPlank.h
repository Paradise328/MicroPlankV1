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





class MicroPlank
{
public:
    MicroPlank();
    MicroPlank(const MasterConsoleType& MasterConsoleType):m_masterConsole(MasterConsoleType){};

    void        startSystem();
    void        initLoggerConfig();
    void        initMotorDriverThread();
    void        initMasterConsoleThread();

private:
    MasterConsoleType   m_masterConsoleType;
    MasterConsole       m_masterConsole;
    MotorDriver*        m_motorDriver;
};

#endif // MICROPLANK_H