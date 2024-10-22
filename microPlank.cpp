#include"microPlank.h"

void MicroPlank::startSystem()
{
    std::cout << "start system !!!" << std::endl;
    initLoggerConfig();
    initMotorDriverThread();
    initMotor();
    startMasterConsoleThread();
    startTeleoperationControlThread();
    while(true){};
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
    int endMotorNum = 1;
    int gimbalMotorNum = 0;
    int slaveNum  = 1;
    m_motorDriver = MotorDriver::getInstance(pathPDOMapping, endMotorNum, gimbalMotorNum, slaveNum);
    m_motorDriver->startThread(promiseCommunication);
    auto flagCommunication = futureCommunication.get();
    LOG(INFO) << "EtherCat Communication thread status: " << flagCommunication << std::endl;
}

void MicroPlank::initMotor()
{
    m_motorDriver->enableMotor(MotorType::MAXON,joint_A,true);
    m_motorDriver->setOperationMode(MotorType::MAXON, joint_A, OperationMode::CSP);
    auto statusWord = m_motorDriver->getStatusWord(MotorType::MAXON, joint_A);
    std::cout << "statusWord: " << statusWord << std::endl;
}

void MicroPlank::startMasterConsoleThread()
{
    m_masterConsole.startUpdateConsoleDataThread();
}

void MicroPlank::startTeleoperationControlThread()
{
    m_teleoperationThread = std::thread(&MicroPlank::teleoperationControlThread, this);
    LOG(INFO) << "Start teleoperation thread";
    m_teleoperationThread.detach();
}

void MicroPlank::teleoperationControlThread()
{
    std::ofstream outputFile("output3ms.txt");  // 打开文件进行写入
    if (!outputFile.is_open()) {
        std::cerr << "无法打开文件！" << std::endl;
        return;
    }
    int i = 0;
    std::array<std::array<double, 7>, 2> initMotorPos ={{0}};

    while(true)
    {
        i++;
        auto MasterData = m_masterConsole.returnHandlePose();
        auto MasterPNOData = MasterData.returnPNOData();
        if (i == 1)
        {
            initMotorPos = MasterPNOData;
        }
        auto position = m_motorDriver->getActualPos(MotorType::MAXON, joint_A);
        auto velocity = m_motorDriver->getActualVel(MotorType::MAXON, joint_A);
        auto errCode  = m_motorDriver->getErrorCode(MotorType::MAXON, joint_A);
        auto statusWord = m_motorDriver->getStatusWord(MotorType::MAXON, joint_A);
        // m_motorDriver.s
        // m_motorDriver->getActualPos(MotorType::MAXON, joint_A);
        // std::cout << "errCode: " << errCode << std::endl;
        // std::cout << "statusWord: " << statusWord << std::endl;

        m_motorDriver->setTargetPos(MotorType::MAXON, joint_A, static_cast<int>((MasterPNOData[1][3]-initMotorPos[1][3])*100));
        
        // outputFile <<i<<" "<<static_cast<int>(MasterPNOData[1][3]*500) <<" "<< position << " " << velocity << std::endl;
        usleep(3000); 
    }
}


