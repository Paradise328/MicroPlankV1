#include"microPlank.h"


void MicroPlank::startStarSystemThread()
{
    std::thread startSystemThread = std::thread(&MicroPlank::startSystem,this);
    startSystemThread.detach();
}

void MicroPlank::onSendMeg(Message_Inner_T &msg)
{

}

void MicroPlank::startSystem()
{
    LOG(INFO) << "start system ";

    /* 开启主手线程，并进行对m_isMasterConsoleOk的赋值 */
    startMasterConsole();//存储422ok（通信是否成功）以及组合主手数据

    startDomainControllerThread();

    /* 开启MotorDriver线程 */
    initMotorDriver();

    std::this_thread::sleep_for(std::chrono::seconds(20));

    startMsgThread();

    /* 开启robotControl线程 */
    startRobotControl();

    //// m_security.performSystemCheck();
    m_security.systemBootSelfCheck();

    // startLightBoard();

   //  while(1)
   //  {
   //  m_lightboard->setLightBoardColor(LIGHT_COLOR_GREEN,LIGHT_MODEL_ON);
   //  m_lightboard->askMyInstrumentStatus();
   // // delay(100ms)
   //  }

    /* 开启security监控线程 */
    startSecurityModule();//开机自检，主要与ui交互
}

void MicroPlank::startMasterConsole()
{
    m_masterConsole.startUpdateConsoleDataThread();
}

void MicroPlank::startSecurityModule()
{
    m_security.startSystemMonitor(m_masterConsole, m_motorDriver);
    LOG(INFO)<<"Successfully start Security Thread ";
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

void MicroPlank::startLightBoard()
{
    // QThread *thread = QThread::create([this](){
    //     m_lightboard->setLightBoardColor(LIGHT_COLOR_GREEN,LIGHT_MODEL_ON);
    //     while(1)
    //     {
    //         m_lightboard->askMyInstrumentStatus();
    //         SteadyDelay(100);
    //     }
    // });
    // thread->start();
    // QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

}

void MicroPlank::startForceSensor()
{
    LOG(INFO)<<"+++ in start force sensor function";
    m_forceSensor->initDevice();
}

void MicroPlank::startDomainControllerThread()
{
    //m_domainController_Left->startThread();
    QThread *thread = QThread::create([this](){
        while(1){
            m_domainController->read_Write_Data(1);
            SteadyDelay(5);
            m_domainController->read_Write_Data(2);
            SteadyDelay(5);
        }
    });
    thread->start();
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
}

void MicroPlank::startMsgThread()
{
    m_msgThread = std::thread(&MicroPlank::messagePoll, this);
    m_msgThread.detach();
    LOG(INFO)<<" Successfully start Message Thread ";
}

void MicroPlank::messagePoll()
{
    while(!m_isSystemTerminated)
    {
         Message_Inner_T msg_t = m_MsgPool.GetMessage();
         switch (msg_t.Sender)
         {
            case Module_Inner_E::Uiinterface:break;
            case Module_Inner_E::MasterConsole:break;
            case Module_Inner_E::Security:break;
            case Module_Inner_E::MotorDriver:break;
            case Module_Inner_E::RobotControl:break;
            case Module_Inner_E::AssistDevice_Lifting:break;
            case Module_Inner_E::Audio:break;
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
            case Module_Inner_E::AssistDevice_Lifting:  m_lifting->GetAmMsg(msg_t);     break;
            case Module_Inner_E::Audio:         m_audioPlay.GetAmMsg(msg_t);            break;
            case Module_Inner_E::MultipleModules:
                 {
                    m_security.GetAmMsg(msg_t);
                    m_masterConsole.GetAmMsg(msg_t);
                    m_uiInterface.GetAmMsg(msg_t);
                    m_motorDriver->GetAmMsg(msg_t);
                    m_robotControl.GetAmMsg(msg_t);
                    m_lifting->GetAmMsg(msg_t);
                    m_audioPlay.GetAmMsg(msg_t);
                    m_LightBoardManagement.GetAmMsg(msg_t);
                    break;
                 }
            default:continue;
         }
    }
}
