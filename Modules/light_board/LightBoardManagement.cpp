#include "LightBoardManagement.h"
auto time_point_instrumentL_install = std::chrono::steady_clock::now();
auto time_point_instrumentL_cur = std::chrono::steady_clock::now();
std::chrono::duration<double> instrumentUsingTime_L;

auto time_point_instrumentR_install = std::chrono::steady_clock::now();
auto time_point_instrumentR_cur = std::chrono::steady_clock::now();
std::chrono::duration<double> instrumentUsingTime_R;

LightBoardManagement::LightBoardManagement(MessageQueue&  messagePool):m_messagePool(messagePool)
{
    connect(this,&LightBoardManagement::DealMsgSignal,this,&LightBoardManagement::dealWithMsg);
    m_readInstrumentTypeFlag.store(1);
    m_readInstrumentUsingTimeFlag.store(0);
    m_InstrumentStatus_Prev.initAll();

//    m_LightBoardOnBeam.LCMD("LRW",eFinalSideUNSED, LIGHT_COLOR_GREEN, LIGHT_MODEL_BLINK, 0, 0);

    connect(this, &LightBoardManagement::askInstrumentStatusSignal, &m_LightBoardOnBeam, &LightBoard::askMyInstrumentStatus);
//    connect(this, &LightBoardManagement::updateCounterInfoSignal, &m_LightBoardOnBeam, &LightBoard::updateCounter);
    connect(&m_LightBoardOnBeam, &LightBoard::updateLiftingRockerStatus, this, &LightBoardManagement::sendRockerStatus);

    startAskInstrumentStatusThread();
    startGetLightBoardStatus();
}

void LightBoardManagement::startGetLightBoardStatus()
{
    m_readInstrumentStatusThread = std::thread(&LightBoardManagement::getLightBoardStatus, this);
    m_readInstrumentStatusThread.detach();
}
void LightBoardManagement::getLightBoardStatus()
{
    std::cout<< "Trying to get Instrument Type"<< std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds(5 * 1000*1000));

    while(true)
    {
//        readInstrumentLightBoardStatus();
        std::this_thread::sleep_for(std::chrono::microseconds(1*1000*1000));
    }
}

void LightBoardManagement::startAskInstrumentStatusThread()
{
    m_askInstrumentStatusThread = std::thread(&LightBoardManagement::askInstrumentStatusThread, this);
    m_askInstrumentStatusThread.detach();
}

void LightBoardManagement::askInstrumentStatusThread()
{
    std::cout<< "Trying to acquire Instrument Type"<< std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds(5 *1000*1000));
    while(true)
    {
        emit askInstrumentStatusSignal();
        std::this_thread::sleep_for(std::chrono::microseconds(100*1000));
    }
}

void LightBoardManagement::sendRockerStatus()
{
    int myLiftingRockerStatus = m_LightBoardOnBeam.returnMyLiftingRockerStatus();
    SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvLightBoardIoState),"IO:"+QString::number(myLiftingRockerStatus));
}
void LightBoardManagement::readInstrumentLightBoardStatus()
{
    InstrumentStatus instrumenStatusTmp = m_LightBoardOnBeam.returnMyInstrumentStatus();

    const InstrumentStatus instrumenStatusPrev = m_InstrumentStatus_Prev;
    m_usedTimeCounter_L++;
    m_usedTimeCounter_R++;

    if(instrumenStatusTmp.connectionStatus_L != instrumenStatusPrev.connectionStatus_L)
    {
        updateInstrumentInformation(instrumenStatusTmp, instrumenStatusPrev, 'l');
    }
    else if((instrumenStatusTmp.connectionStatus_L == instrumenStatusPrev.connectionStatus_L) && instrumenStatusTmp.connectionStatus_L == "true")
    {
        time_point_instrumentL_cur = std::chrono::steady_clock::now();
        instrumentUsingTime_L = time_point_instrumentL_cur - time_point_instrumentL_install;
        if((m_usedTimeCounter_L % 120) == 0)
        {
            // 让计数器加1
            emit updateCounterInfoSignal('l');
            instrumenStatusTmp.usedTime_L ++;
            QList<QString> arglist;
            arglist.append("InstrumentUsingTimeL:" + QString::number(instrumenStatusTmp.usedTime_L * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvInstrumentData), arglist);
            arglist.clear();
        }
    }

    if(instrumenStatusTmp.connectionStatus_R != instrumenStatusPrev.connectionStatus_R)
    {
        updateInstrumentInformation(instrumenStatusTmp, instrumenStatusPrev, 'r');
    }
    else if((instrumenStatusTmp.connectionStatus_R == instrumenStatusPrev.connectionStatus_R) && instrumenStatusTmp.connectionStatus_R == "true")
    {
        time_point_instrumentR_cur = std::chrono::steady_clock::now();
        instrumentUsingTime_R = time_point_instrumentR_cur - time_point_instrumentR_install;
        if((m_usedTimeCounter_R % 120) == 0)
        {
            // 让计数器加1
            emit updateCounterInfoSignal('r');
            instrumenStatusTmp.usedTime_R ++;
            QList<QString> arglist;
            arglist.append("InstrumentUsingTimeR:" + QString::number(instrumenStatusTmp.usedTime_R * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvInstrumentData),arglist);
            arglist.clear();
        }
    }
    m_InstrumentStatus_Prev = instrumenStatusTmp;
}

void LightBoardManagement::updateInstrumentInformation(const InstrumentStatus& instrumentStatusCur, const InstrumentStatus& instrumenTypePrev, const char& side)
{
    //Send Instrument Data to Slave Device Center


    //Send to UI:
    //若在术前准备阶段：1.末端器械已取下 2.需要重新归零
    //若在手术中阶段：提示器械掉落
    //Send to Security:
    //若在术前准备阶段：无
    //Send to Slave Device:
    //若在术前准备阶段：重新读取配置文件及参数
    //若在手术中阶段：禁止更换器械，若掉落，security弹窗提示

    //左侧末端
    if(side == 'l')
    {
        if(instrumentStatusCur.connectionStatus_L == "true" && instrumenTypePrev.connectionStatus_L == "false")
        {
            time_point_instrumentL_install = std::chrono::steady_clock::now();
            m_usedTimeCounter_L = 0;
            QList<QString> arglist;
            arglist.append("InstrumentTypeL:" + QString(instrumentStatusCur.type_L));            
            arglist.append("InstrumentUsingTimeL:" + QString::number(instrumentStatusCur.usedTime_L * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::RecvInstrumentData),arglist);
            arglist.clear();

            QString instrumenInfomation_L;
            instrumenInfomation_L = "l:" + QString(instrumentStatusCur.type_L) + "_" + QString(instrumentStatusCur.size_L) + "_" + QString::number(instrumentStatusCur.ID_L);
            SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::Update_EndeffectorData),instrumenInfomation_L);
        }
        else{
            QList<QString> arglist;
            arglist.append("InstrumentTypeL:" + QString(instrumentStatusCur.type_L));
            arglist.append("InstrumentUsingTimeL:" + QString::number(instrumentStatusCur.usedTime_L * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::RecvInstrumentData),arglist);
            arglist.clear();

            QString instrumenInfomation_L;
            instrumenInfomation_L = "l:None";
            SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::Update_EndeffectorData),instrumenInfomation_L);
        }
    }

    //右侧末端
    if(side == 'r')
    {
        if(instrumentStatusCur.connectionStatus_R == "true" && instrumenTypePrev.connectionStatus_R == "false")
        {
            time_point_instrumentR_install = std::chrono::steady_clock::now();
            m_usedTimeCounter_R = 0;

            QList<QString> arglist;
            arglist.append("InstrumentTypeR:" + QString(instrumentStatusCur.type_R));
            arglist.append("InstrumentUsingTimeR:" + QString::number(instrumentStatusCur.usedTime_R * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvInstrumentData),arglist);
            arglist.clear();

            QString instrumenInfomation_R;
            instrumenInfomation_R = "r:" + QString(instrumentStatusCur.type_R) + "_" + QString(instrumentStatusCur.size_R) + "_" + QString::number(instrumentStatusCur.ID_R);
            SendInnerMsg(Module_Inner_E::RobotControl,static_cast<int>(RobotControlAction_E::Update_EndeffectorData),instrumenInfomation_R);
        }
        else{
            QList<QString> arglist;
            arglist.append("InstrumentTypeR:" + QString(instrumentStatusCur.type_R));
            arglist.append("InstrumentUsingTimeR:" + QString::number(instrumentStatusCur.usedTime_R * 2));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvInstrumentData),arglist);
            arglist.clear();

            QString instrumenInfomation_R;
            instrumenInfomation_R = "r:None";
            SendInnerMsg(Module_Inner_E::RobotControl,static_cast<int>(RobotControlAction_E::Update_EndeffectorData),instrumenInfomation_R);
        }
    }
}

void LightBoardManagement::readTrolleyLightBoardStatus()
{
//   if(m_LightBoardInTrolley.returnLightBoardStatus() == LightBoardInTrolleyStatus::NORMAL)
//   {
//       QString arg;
//       SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvLightBoardInTrolleyData),arg);
//   }
}

void LightBoardManagement::SendInnerMsg(Module_Inner_E recever,int Action,QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender=Module_Inner_E::AssistDevice_LightBoardManagement;
    msgTemp.Recver=recever;
    msgTemp.Request.insert(Action,arg);
    m_messagePool.SendMessage(msgTemp);
}

void LightBoardManagement::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::AssistDevice_LightBoardManagement;
    msgTemp.Recver=recever;
    QString stringste;
    for(int i=0;i<arglist.length();i++)
    {
        stringste += arglist[i];
        stringste += ";";
    }
    msgTemp.Request.insert(Action,stringste);
    m_messagePool.SendMessage(msgTemp);
}


void LightBoardManagement::dealWithMsg()
{
    Message_Inner_T msg;
    while(1)
    {
        m_MsgGottenRWLock.lockForRead();
        if(m_MsgGottenQueue.isEmpty()==false){
            msg=m_MsgGottenQueue.dequeue();
        }else{
             m_MsgGottenRWLock.unlock();
             break;
        }
        m_MsgGottenRWLock.unlock();
        if(msg.Recver!=Module_Inner_E::AssistDevice_LightBoardManagement&&msg.Recver!=Module_Inner_E::MultipleModules)
            return;

        QHash<int,QString>::ConstIterator i=msg.Request.constBegin();
        while(i!=msg.Request.constEnd())
        {
            switch (i.key())
            {
                case static_cast<int>(MultipleDevAction_E::RecvSystemWarning)://RecvLightBoardIoState:
                {
                if(i.value()=="Normal")
                {
                    m_LightBoardOnBeam.setLightBoardColor(LIGHT_COLOR_GREEN, LIGHT_MODEL_ON);
                    // m_LightBoardInTrolley.LCMD("LSC=green");
                    // m_LightBoardInTrolley.LCMD("LSM=on");
                }
                else if(i.value()=="LightWarning")
                {
                    m_LightBoardOnBeam.setLightBoardColor(LIGHT_COLOR_GREEN, LIGHT_MODEL_BLINK);
                    // m_LightBoardInTrolley.LCMD("LSC=green");
                    // m_LightBoardInTrolley.LCMD("LSM=blink");
                }
                else if(i.value()=="MiddleWarning")
                {
                    m_LightBoardOnBeam.setLightBoardColor(LIGHT_COLOR_YELLOW, LIGHT_MODEL_BLINK);
                    // m_LightBoardInTrolley.LCMD("LSC=yellow");
                    // m_LightBoardInTrolley.LCMD("LSM=blink");
                }
                else if(i.value()=="HeavyWarning")
                {
                    m_LightBoardOnBeam.setLightBoardColor(LIGHT_COLOR_RED, LIGHT_MODEL_ON);
                    // m_LightBoardInTrolley.LCMD("LSC=red");
                    // m_LightBoardInTrolley.LCMD("LSM=blink");
                }



//                    if(i.value()=="Normal")
//                    {
//                        m_LightBoardOnBeam.LCMD("LSC=green");
//                        m_LightBoardOnBeam.LCMD("LSM=on");
//                        m_LightBoardInTrolley.LCMD("LSC=green");
//                        m_LightBoardInTrolley.LCMD("LSM=on");
//                    }
//                    else if(i.value()=="LightWarning")
//                    {
//                        m_LightBoardOnBeam.LCMD("LSC=green");
//                        m_LightBoardOnBeam.LCMD("LSM=blink");
//                        m_LightBoardInTrolley.LCMD("LSC=green");
//                        m_LightBoardInTrolley.LCMD("LSM=blink");
//                    }
//                    else if(i.value()=="MiddleWarning")
//                    {
//                        m_LightBoardOnBeam.LCMD("LSC=yellow");
//                        m_LightBoardOnBeam.LCMD("LSM=blink");
//                        m_LightBoardInTrolley.LCMD("LSC=yellow");
//                        m_LightBoardInTrolley.LCMD("LSM=blink");
//                    }
//                    else if(i.value()=="HeavyWarning")
//                    {
//                        m_LightBoardOnBeam.LCMD("LSC=red");
//                        m_LightBoardOnBeam.LCMD("LSM=blink");
//                        m_LightBoardInTrolley.LCMD("LSC=red");
//                        m_LightBoardInTrolley.LCMD("LSM=blink");
//                    }
                    break;
                }
                case static_cast<int>(AssistDevice_LightBoardManagementAction_E::BootSelfCheck):
                {

                    break;
                }
                case static_cast<int>(AssistDevice_LightBoardManagementAction_E::TurnOffLight):
                {
                m_LightBoardOnBeam.setLightBoardColor(LIGHT_COLOR_YELLOW, LIGHT_MODEL_OFF);
                    // m_LightBoardInTrolley.LCMD("LSM=off");
//                    LOG(INFO)<<"Turn Off Light";
                    break;
                }
                case static_cast<int>(AssistDevice_LightBoardManagementAction_E::CheckInstrumentInfomation):
                {
//                    if(i.value()=="Time")
//                    {
//                        m_readInstrumentTypeFlag.store(0);
//                        m_readInstrumentUsingTimeFlag.store(1);
//                    }
//                    if(i.value()=="Type")
//                    {
//                        m_readInstrumentTypeFlag.store(1);
//                        m_readInstrumentUsingTimeFlag.store(0);
//                    }
                    break;
                }
                default:break;
            }
            i++;
        }
    }

}

void LightBoardManagement::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}
