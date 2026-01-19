#include <QCoreApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QFile>
#include <QTextStream>
#include <signal.h>
#include <unistd.h>
#include "Modules/LoggerModule/easylogging++.h"
#include "microPlank.h"
#include "Modules/ForceSensorModule/ForceSensor.h"
INITIALIZE_EASYLOGGINGPP
#define ELPP_THREAD_SAFE
#define ELPP_QT_LOGGING

/*
  To run the haptic device, need to give the permission
  lsusb Force Dimension omega.x haptic device
  sudo chmod o+w /dev/bus/usb/001/003  (use the real port entry)
*/

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    MotorDriverParameter motorDriverParameter;

    motorDriverParameter.guidingJointMotorNum = 0;           /* number of motor to control the arm */
    motorDriverParameter.armNum = 1;
    motorDriverParameter.endGimbalMotorNum = 1;              /* total number of motor to control end-effector gimbal */
    motorDriverParameter.endGimbalMotorNumPerArm = 1;        /* number of motor to control end-effector gimbal on each arm */
    motorDriverParameter.endJointMotorNum = 0;               /* total number of motor to control end-effector joint ，2个机械臂 zero err 数量*/
    motorDriverParameter.endJointMotorNumPerArm = 0;         /* number of motor to control end-effector joint on each arm 2个机械臂 Maxon 数量*/
    motorDriverParameter.endInstrumentMotorNum = 6;//8          /* total number of motor to control end-effector instruments 8*/
    motorDriverParameter.endInstrumentMotorNumPerArm = 6;//4    /* number of motor to control end-effector instruments on each arm*/
    motorDriverParameter.motorNumPerArm = 7;//8                /* 8 for 4 Motors*/
    motorDriverParameter.slaveNum = 7;//19                      /* total number of ethercat slaves*/
    motorDriverParameter.motorNum = 7;//19                      /* total number of ethercat slaves*/

    motorDriverParameter.forceSensorNumPerArm = 0;

    /* Log File Configuration */
    const auto timeStamp = getCurrentTimeAsString();
    const auto logFileName = "../logs/app_" + timeStamp + ".log";
    el::Configurations conf("/home/a/Desktop/codes/MikroPlanckV1/Config/logConf.conf");  // path have to be adapted
    conf.setGlobally(el::ConfigurationType::Filename, logFileName);
    el::Loggers::reconfigureAllLoggers(conf);
    LOG(INFO)<< "Successfully init Logger Config Module ";

   std::unordered_map<std::string, std::string> audioMap = {
       {"1", "/home/a/Desktop/codes/MikroPlanckV1/Modules/UIModule/sound/1.wav"},
       {"2", "/home/a/Desktop/codes/MikroPlanckV1/Modules/UIModule/sound/2.wav"},
       {"3", "/home/a/Desktop/codes/MikroPlanckV1/Modules/UIModule/sound/3.wav"},
       {"4", "/home/a/Desktop/codes/MikroPlanckV1/Modules/UIModule/sound/4.wav"},
       {"enable", "/home/a/Desktop/codes/MikroPlanckV1/Modules/UIModule/sound/enable.wav"},
   };


   MicroPlank MicroPlank(app, MasterConsoleType::DessightMaster, motorDriverParameter, audioMap);//初始化MicroPlank，输入值为Viper，和电机信息

   return app.exec();
}
