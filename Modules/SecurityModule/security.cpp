#include "security.h"

Security::Security(MessageQueue& messagePool):m_messagePool(messagePool)
{
    m_systemOperationMode.store(SystemMode::BootSelfCheck);
    selfCheckStep.store(SelfCheckStepEnum::No_Checking);
    checkShutDownSystem.store(shutDownSystemEnum::No_ShutDown);
    m_systemModuleStatus = {false, false, false};

    connect(this, &Security::DealMsgSignal, this, &Security::dealWithMsg);
}

void Security::startSystemMonitor(MasterConsole& masterConsole, MotorDriver* motorDriver)
{
    try {
        m_systemMonitorThread = std::thread(&Security::systemMonitor, this, std::ref(masterConsole), motorDriver);
        m_systemMonitorThread.detach();
    } catch (const std::exception& e) {
        LOG(ERROR) << "Failed to start system monitor thread: " << e.what() << std::endl;
    }
}

void Security::performSystemCheck(MasterConsole& masterConsole, RobotControl& robotControl)
{

}

void Security::systemMonitor(MasterConsole& masterConsole, MotorDriver* motorDriver)
{
    SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvSystemBootSta),"Ok");

    while(!m_flagIsSystemTerminated)
    {
        auto systemOperationMode = m_systemOperationMode.load();

        auto systemModuleStatus_Prev = m_systemModuleStatus.load();

        auto masterConsoleStatus_Cur = masterConsole.returnMasterConsoleStatus();/*422ok*/
        auto etherCATCommunicationStatus_Cur = motorDriver->returnMotorDriverStatus();


        auto masterConsoleStatus_Prev = systemModuleStatus_Prev[0];
        auto etherCATCommunicationStatus_Prev = systemModuleStatus_Prev[1];
        auto liftingArmStatus_Prev = systemModuleStatus_Prev[2];
        // LOG(INFO) << "masterConsoleStatus_Cur: " << masterConsoleStatus_Cur << "  systemModuleStatus_Prev: " << systemModuleStatus_Prev[0];

        switch(systemOperationMode)
        {
            case SystemMode::BootSelfCheck:
                /*开机自检过程*/
                {
                    if(masterConsoleStatus_Prev == false && masterConsoleStatus_Cur == true)
                    {
                        LOG(INFO) << "Master Console Successfully Connected! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvBootSelfCheckStatus),"Master:Ok");
                    }
                    if(etherCATCommunicationStatus_Prev == false && etherCATCommunicationStatus_Cur == true)
                    {
                        LOG(INFO) << "etherCATCommunication Successfully Connected! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvBootSelfCheckStatus),"EtherCAT:Ok");
                    }
                    break;
                }
            case SystemMode::PreOperation:
                /*术前*/
                {
                    /*主手重连*/
                    if(masterConsoleStatus_Prev == false && masterConsoleStatus_Cur == true)
                    {
                        LOG(INFO) << "Master Console Successfully Reconnected! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvModulesStatus),"Master:Ok");
                    }
                    /*主手断连*/
                    else if(masterConsoleStatus_Prev == true && masterConsoleStatus_Cur == false)
                    {
                        LOG(INFO) << "Master Console Connection Lost! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvModulesStatus),"Master:Err");
                    }
                    break;
                }
            case SystemMode::InOperation_TeleOperation:
                /*术中: 主从控制*/
                {
                    /*主手重连*/
                    if(masterConsoleStatus_Prev == false && masterConsoleStatus_Cur == true)
                    {
                        LOG(INFO) << "Master Console Successfully Reconnected! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvModulesStatus),"Master:Ok");
                    }
                    /*主手断连*/
                    else if(masterConsoleStatus_Prev == true && masterConsoleStatus_Cur == false)
                    {
                        LOG(INFO) << "Master Console Connection Lost! ";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(SecurityAction_E::RecvModulesStatus),"Master:Err");
                    }
                    break;
                }
            case SystemMode::InOperation_Collaboration:
                /*术中:协作*/
                {
                    break;
                }
            case SystemMode::ShutDownProcess:
                /*关机*/
                {
                    break;
                }
            case SystemMode::RestartProcess:
                /*重启*/
                {
                    break;
                }
            default:break;
        }
        setModuleStatus(masterConsoleStatus_Cur, true, true);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void Security::setModuleStatus(const bool& masterConsoleStatus,
                               const bool& robotControlStatus,
                               const bool& liftingArmStatus)
{
    std::array<bool, SystemModuleNum> systemModuleStatusPrev_Tmp, systemModuleStatusCur_Tmp;
    systemModuleStatusPrev_Tmp = m_systemModuleStatus.load();
    systemModuleStatusCur_Tmp = {masterConsoleStatus, robotControlStatus, liftingArmStatus};

    bool isAllModuleAllOkPrev = std::all_of(systemModuleStatusPrev_Tmp.begin(), systemModuleStatusPrev_Tmp.end(), [](bool isAllTruePrev) {return isAllTruePrev;});
    bool isAllModuleAllOkCur = std::all_of(systemModuleStatusCur_Tmp.begin(), systemModuleStatusCur_Tmp.end(), [](bool isAllTrueCur) {return isAllTrueCur;});

    if(m_systemOperationMode.load() == SystemMode::BootSelfCheck)
    {
        /*自检过程中，全部模块变为true的时刻，UI中自检中字样切换为继续字样，并可点击进入术前准备模式*/
        if(!isAllModuleAllOkPrev && isAllModuleAllOkCur)
        {
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvSystemBootSta),"Ok");
            m_systemOperationMode.store(SystemMode::PreOperation);
        }
    }
    m_systemModuleStatus.store(systemModuleStatusCur_Tmp);
}


void Security::setSelfCheckStep(SelfCheckStepEnum selfcheckstep)
{
    selfCheckStep.store(selfcheckstep);
}

void Security::setSystemStatus(SystemWarningStatus sta)
{
    m_systemWarningStatus = sta;
    QString statusString = "";
    switch (sta)
    {
        case SystemWarningStatus::Normal:
        {
            statusString="Normal";
            break;
        }
        case SystemWarningStatus::LightWarning:
        {
            statusString="LightWarning";
            break;
        }
        case SystemWarningStatus::MiddleWarning:
        {
            statusString="MiddleWarning";
            break;
        }
        case SystemWarningStatus::HeavyWarning:
        {
            statusString="HeavyWarning";
            break;
        }
        default:break;
    }
    SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvSystemWarning),statusString);
  //  selfCheckStep.store(selfcheckstep);
}


void Security::shutDownSystem()
{
    LOG(INFO)<<"Security: PowerOff" ;

    checkShutDownSystem.store(shutDownSystemEnum::EtherCATOff);
    std::thread shutDownSystemThread([this]
    {
        while(flagShutDownSystem)
        {
            auto checkShutDownSystemTemp = checkShutDownSystem.load();
            switch(checkShutDownSystemTemp)
            {
                case shutDownSystemEnum::EtherCATOff:
                {
                    SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::MotorOff), "");
                    checkShutDownSystem.store(shutDownSystemEnum::Respons_Waiting);
                    break;
                }
                case shutDownSystemEnum::MasterConsoleOff:
                {
                    SendInnerMsg(Module_Inner_E::MasterConsole, static_cast<int>(MasterConsoleAction_E::MasterShutDown), "");
                }
                case shutDownSystemEnum::ShutDownFinish:
                {
                    el::Loggers::flushAll();
                    sleep(3);
                    KillProcess();
                    break;
                }
                case  shutDownSystemEnum::Respons_Waiting:
                {
                    break;
                }

                default:break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    shutDownSystemThread.detach();
}

void Security::systemBootSelfCheck()
{
   // selfCheckStep.store(SelfCheckStepEnum::MasterConsole_Checking);
    selfCheckStep.store(SelfCheckStepEnum::AllOK);

    std::thread systemBootSelfCheckThread([this]
    {
        while(flagSelfCheck)
        {
            auto selfCheckStepTemp = selfCheckStep.load();/*selfCheckStep通过Msg与外界的类关联*/
            switch (selfCheckStepTemp)
            {
                case SelfCheckStepEnum::MasterConsole_Checking:
                {
                    SendInnerMsg(Module_Inner_E::MasterConsole,static_cast<int>(MasterConsoleAction_E::BootSelfCheck),"");
                    selfCheckStep.store(SelfCheckStepEnum::Respons_Waiting);
                    break;
                }

                case SelfCheckStepEnum::AllOK:
                {
                    SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvSystemBootSta),"Ok");
                    setSystemStatus(SystemWarningStatus::Normal);/*SystemWarningStatus与warning状态（灯板）相关*/
                    return;
                }

                case SelfCheckStepEnum::Err:
                {
                    setSystemStatus(SystemWarningStatus::HeavyWarning);
                    SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvSystemBootSta),"Err");
                    return; break;
                }
                default:break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    systemBootSelfCheckThread.detach();
}



void Security::SendInnerMsg(Module_Inner_E recever, int Action, QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Security;
    msgTemp.Recver = recever;
    msgTemp.Request.insert(Action, arg);
    m_messagePool.SendMessage(msgTemp);
}

void Security::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Security;
    msgTemp.Recver = recever;
    for(int i = 0; i < arglist.length();i++)
    {
        msgTemp.Request.insert(Action, arglist[i]);
    }
    m_messagePool.SendMessage(msgTemp);
}


void Security::dealWithMsg()
{
    Message_Inner_T msg;
    while(1){
        m_MsgGottenRWLock.lockForRead();
        if(m_MsgGottenQueue.isEmpty()==false){
            msg=m_MsgGottenQueue.dequeue();
        }else{
             m_MsgGottenRWLock.unlock();
             break;
        }
        m_MsgGottenRWLock.unlock();
        if(msg.Recver!=Module_Inner_E::Security&&msg.Recver!=Module_Inner_E::MultipleModules)
            return;
        QHash<int,QString>::ConstIterator i=msg.Request.constBegin();
        while(i!=msg.Request.constEnd())
        {
            switch (i.key())
            {
                case static_cast<int>(MultipleDevAction_E::RecvLiftingBootSta):
                {
                    if(msg.Sender == Module_Inner_E::AssistDevice_Lifting)
                    {
                        if(i.value() == "Ok")
                        {
                            setSelfCheckStep(SelfCheckStepEnum::AssistArm_Checking);
                        }else{
                            setSelfCheckStep(SelfCheckStepEnum::Err);
                        }
                    }
                    break;
                }
                case static_cast<int>(MultipleDevAction_E::RecvHorizontalArmBootSta):
                {
                    if(msg.Sender == Module_Inner_E::AssistDevice_Arm)
                    {
                        if(i.value() == "Ok")
                        {
                            setSelfCheckStep(SelfCheckStepEnum::Slave_Checking);
                        }else{
                            setSelfCheckStep(SelfCheckStepEnum::Err);
                        }

                    }
                    break;
                }

                case static_cast<int>(MultipleDevAction_E::RecvMasterBootSta):
                {
                    if(msg.Sender == Module_Inner_E::MasterConsole)
                    {
                        if(i.value() == "Ok")
                        {
                            setSelfCheckStep(SelfCheckStepEnum::AllOK);

                        }else{
                            setSelfCheckStep(SelfCheckStepEnum::Err);
                            SendInnerMsg(Module_Inner_E::MultipleModules, static_cast<int>(MultipleDevAction_E::RecvSystemBootSta),"Err");
                        }
                    }
                    break;
                }
                case static_cast<int>(SecurityAction_E::RecvMotorDriverShutDown):
                {
                    if(checkShutDownSystem.load() == shutDownSystemEnum::Respons_Waiting)
                    {
                        if(i.value() == "Ok")
                        {
                            checkShutDownSystem.store(shutDownSystemEnum::ShutDownFinish);
                        }
                    }
                    break;
                }
                case static_cast<int>(SecurityAction_E::PowerOff):
                {
                    shutDownSystem();
                    break;
                }
                default:break;
            }
            i++;
        }
    }
}


void Security::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}

void Security::KillProcess()
{
    qint64 pid = QCoreApplication::applicationPid();//获取当前进程的PID

    QString cmd = QString("kill -9 %1").arg(pid);//Linux 系统下，强制杀死当前进程命令

    system(cmd.toLocal8Bit().data());
}
