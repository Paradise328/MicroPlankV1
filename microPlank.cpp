#include"microPlank.h"


void MicroPlank::startStarSystemThread()
{
    m_startSystemThread = std::thread(&MicroPlank::startSystem,this);
    m_startSystemThread.detach();
}


void MicroPlank::startSystem()
{
    LOG(INFO) << "start system " << std::endl;

    startMasterConsole();

    // initMotorDriver();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    startMsgThread();

    // startRobotControl();

    startSecurityModule();

    m_security.systemBootSelfCheck();
}

void MicroPlank::initLoggerConfig()
{
    //Log File Configuration
    const auto timeStamp = getCurrentTimeAsString();
    const auto logFileName = "../logs/app_" + timeStamp + ".log";
    el::Configurations conf("/home/chenxinbo/Desktop/CodeRepository/MicroPlank_QTVersion/MicroPlank_QTVersion/Config/logConf.conf");  // path have to be adapted
    conf.setGlobally(el::ConfigurationType::Filename, logFileName);
    el::Loggers::reconfigureAllLoggers(conf);
    LOG(INFO)<< "Successfully init Logger Config Module ";
}

void MicroPlank::startMasterConsole()
{
    m_masterConsole.startUpdateConsoleDataThread();
    LOG(INFO)<<"Successfully start Master Console Thread ";
}

void MicroPlank::startSecurityModule()
{
    m_security.startSystemMonitor(m_masterConsole);
    LOG(INFO)<<"Successfully start Master Console Thread ";
}


void MicroPlank::initMotorDriver() //std::promise<bool> &promiseInitMotorDriver
{
    m_initMotorDriverThread = std::thread(&MicroPlank::initMotorDriverThread, this);
    m_initMotorDriverThread.detach();
}

void MicroPlank::initMotorDriverThread()//std::promise<bool> &promiseInitMotorDriver
{
    //Start MotorDriver thread

    std::promise<bool> promiseCommunication;//通信是否成功
    std::future<bool> futureCommunication = promiseCommunication.get_future();//用于主线程等待并获取通信线程的初始化结果

    m_motorDriver = m_robotControl.getMyMotorDriver()->getInstance(m_motorDriverParameter, m_MsgPool);//1获取电机驱动器，getMyMotorDriver()返回一个Motordriver指针。2获取 MotorDriver 的唯一实例

    m_robotControl.setMyMotorDriver(m_motorDriver);//设置电机驱动器，更新 m_robotControl 中的状态，使其知道当前电机驱动器的实例

    m_motorDriver->startThread(promiseCommunication);//电机驱动器，调用startThread方法启动线程

    auto flagCommunication = futureCommunication.get();//阻塞直到promise赋上值

    LOG(INFO) << "EtherCat Communication thread status: " << flagCommunication << std::endl;

    usleep(20 * 1000);
}

void MicroPlank::startRobotControl()
{
     m_robotControl.startMyThreads();
}

void MicroPlank::startMsgThread()
{
    m_msgThread = std::thread(&MicroPlank::messagePoll, this);
    LOG(INFO)<<"Successfully start Message Thread ";
    m_msgThread.detach();
}

void MicroPlank::messagePoll()
{
    while(flagMsgPool)
    {
         Message_Inner_T msg_t = m_MsgPool.GetMessage();
         switch (msg_t.Sender)
         {
            case Module_Inner_E::Uiinterface:break;
            case Module_Inner_E::MasterConsole:break;
            case Module_Inner_E::Security:break;
            case Module_Inner_E::MotorDriver:break;
            case Module_Inner_E::RobotControl:break;
            default:continue;
         }
         switch(msg_t.Recver)
         {
            std::cout << static_cast<int>(msg_t.Recver) << std::endl;
            case Module_Inner_E::Security:      m_security.GetAmMsg(msg_t);             break;
            case Module_Inner_E::MasterConsole: m_masterConsole.GetAmMsg(msg_t);        break;
            case Module_Inner_E::Uiinterface:   m_uiInterface.GetAmMsg(msg_t);          break;
            case Module_Inner_E::MotorDriver:   m_motorDriver->GetAmMsg(msg_t);         break;
            case Module_Inner_E::RobotControl:  m_robotControl.GetAmMsg(msg_t);         break;
            case Module_Inner_E::MultipleModules:
                 {
                    m_security.GetAmMsg(msg_t);
                    m_masterConsole.GetAmMsg(msg_t);
                    m_uiInterface.GetAmMsg(msg_t);
                    m_motorDriver->GetAmMsg(msg_t);
                    m_robotControl.GetAmMsg(msg_t);
                    break;
                 }
            default:continue;
         }
    }
}

void MicroPlank::setRemoteControlFlag(bool startRemoteControlflag)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if(startRemoteControlflag == true)
    {
        flagRemoteControl.store(startRemoteControlflag);
//        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    else if(startRemoteControlflag == false)
    {
        flagRemoteControl.store(startRemoteControlflag);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
//        m_motorDriver->
    }
}

