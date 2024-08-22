#include"microPlank.h"



void MicroPlank::startSystem()
{
    std::cout << "start system !!!" << std::endl;

    // initMotorDriverThread();
    initMasterConsoleThread();

    while(true)
    {
        auto MasterData = m_masterConsole.returnHandlePose();
        auto MasterPNOData = MasterData.returnPNOData();

        std::cout << MasterPNOData << std::endl;
        usleep(2000); 
    }
}

void MicroPlank::initLoggerConfig()
{
    //Log File Configuration
    const auto timeStamp = getCurrentTimeAsString();
    const auto logFileName = "../logs/app_" + timeStamp + ".log";
    el::Configurations conf("../Config/logConf.conf");  // path have to be adapted
    conf.setGlobally(el::ConfigurationType::Filename, logFileName);
    el::Loggers::reconfigureAllLoggers(conf);
}

void MicroPlank::initMotorDriverThread()
{
    //Start MotorDriver thread
    std::promise<bool> promiseCommunication;
    std::future<bool> futureCommunication = promiseCommunication.get_future();
    const std::string pathPDOMapping = "../Config/PDO_mapping.toml";
    int endMotorNum = 4;
    int gimbalMotorNum = 0;
    int slaveNum  = 4;
    m_motorDriver = MotorDriver::getInstance(pathPDOMapping, endMotorNum, gimbalMotorNum, slaveNum);
    m_motorDriver->startThread(promiseCommunication);
    auto flagCommunication = futureCommunication.get();
    LOG(INFO) << "EtherCat Communication thread status: " << flagCommunication << std::endl;
}

void MicroPlank::initMasterConsoleThread()
{
    m_masterConsole.startUpdateConsoleDataThread();
}


