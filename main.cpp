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


   motorDriverParameter.guidingJointMotorNum = 3;           /* number of motor to control the arm */
   motorDriverParameter.armNum = 2;
   motorDriverParameter.endGimbalMotorNum = 2;              /* total number of motor to control end-effector gimbal */
   motorDriverParameter.endGimbalMotorNumPerArm = 1;        /* number of motor to control end-effector gimbal on each arm */
   motorDriverParameter.endJointMotorNum = 6;               /* total number of motor to control end-effector joint */
   motorDriverParameter.endJointMotorNumPerArm = 3;         /* number of motor to control end-effector joint on each arm */
   motorDriverParameter.endInstrumentMotorNum = 12;         /* total number of motor to control end-effector instruments */
   motorDriverParameter.endInstrumentMotorNumPerArm = 6;    /* number of motor to control end-effector instruments on each arm */
   motorDriverParameter.slaveNum = 23;                      /* total number of ethercat slaves*/
   motorDriverParameter.motorNum = 23;                      /* total number of ethercat slaves*/
   motorDriverParameter.forceSensorNumPerArm = 0;

   /* Log File Configuration */
   const auto timeStamp = getCurrentTimeAsString();
   const auto logFileName = "../logs/app_" + timeStamp + ".log";
   el::Configurations conf("/home/a/Desktop/codes/MikroPlanckV1/Config/logConf.conf");  // path have to be adapted
   conf.setGlobally(el::ConfigurationType::Filename, logFileName);
   el::Loggers::reconfigureAllLoggers(conf);
   LOG(INFO)<< "Successfully init Logger Config Module ";

   MicroPlank MicroPlank(app, MasterConsoleType::DessightMaster, motorDriverParameter);//初始化MicroPlank，输入值为Viper，和电机信息

   return app.exec();
}
