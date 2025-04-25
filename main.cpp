#include <QCoreApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <signal.h>
#include <unistd.h>
#include "Modules/LoggerModule/easylogging++.h"
#include "microPlank.h"

#include <QFile>
#include <QTextStream>


INITIALIZE_EASYLOGGINGPP
#define ELPP_THREAD_SAFE
#define ELPP_QT_LOGGING

/* To run the haptic device, need to give the permission
  lsusb Force Dimension omega.x haptic device
  sudo chmod o+w /dev/bus/usb/001/003  (use the real port entry)
*/


int main(int argc, char *argv[])
{
   QGuiApplication app(argc, argv);

   MotorDriverParameter motorDriverParameter;
   motorDriverParameter.jointMotorNum = 0;
   motorDriverParameter.endMotorNum = 6;//末端电机的数量
   motorDriverParameter.endGimbalMotorNum = 1;//云台
   motorDriverParameter.endJointMotorNum = 3;//大轴转动电机
   motorDriverParameter.slaveNum = 10;//EtherCAT 从站的数量
   motorDriverParameter.motorNum = 10;

//    motorDriverParameter.endGimbalMotorNum = 1;
//    motorDriverParameter.endJointMotorNum = 4;
//    motorDriverParameter.slaveNum = 4;
//    motorDriverParameter.motorNum = 4;

   //Log File Configuration
   const auto timeStamp = getCurrentTimeAsString();//时间戳生成
   const auto logFileName = "../logs/app_" + timeStamp + ".log";
   el::Configurations conf("/home/a/Desktop/MicroPlank_QTVersion/Config/logConf.conf");  // path have to be adapted
   conf.setGlobally(el::ConfigurationType::Filename, logFileName);
   el::Loggers::reconfigureAllLoggers(conf);
   LOG(INFO)<< "Successfully init Logger Config Module ";

   MicroPlank MicroPlank(app, MasterConsoleType::DessightMaster, motorDriverParameter);//初始化MicroPlank，输入值为Viper，和电机信息

   return app.exec();//对于Ethercat这步保证了实时控制
}
