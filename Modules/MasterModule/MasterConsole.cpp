#include "MasterConsole.h"

void MasterConsole::updateConsoleDataThread()
{
    while(!m_isSystemTerminated && !m_isSystemReset)
    {
        masterConsoleStatusCheck();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void MasterConsole::startUpdateConsoleDataThread()
{
   //init Master
   std::promise<bool> masterPromise;
   std::future<bool> masterFuture = masterPromise.get_future();
   if(m_MasterConsoleType == MasterConsoleType::Viper)
   {
      //Start Read-Viper-Data thread;
      m_viper.viper_start_directly();
      auto flagViper = m_viper.getReadyFuture().get();
      LOG(INFO) << "flagViper thread status: " << flagViper;
      if(flagViper == true)
      {
         m_isViperOk = true;
      }
      else
      {
         m_isViperOk = false;
         //send to security
      }

      bool flag422 = true;
      if(flag422 == true)
      {
         m_is422Ok = true;
      }
      else
      {
         m_is422Ok = false;
         //send to security
      }


      if(flagViper == true && flag422 == true)
      {
        m_updateConsoleDataThread = std::thread(&MasterConsole::updateConsoleDataThread, this);
        LOG(INFO) << "MasterConsole Data Update thread ID: " << m_updateConsoleDataThread.get_id();
        m_updateConsoleDataThread.detach();
        //send to Security everything is ok;
      }
    }

    if(m_MasterConsoleType == MasterConsoleType::Omega)
    {
        m_omega.startThread();
        //      const auto flagOmega = masterFuture.get();
        LOG(INFO) << "Master handle thread (Omega) starts!";
        bool flag422 = true;
        bool flagOmega = true;
        if(flag422 == true)
        {
            m_is422Ok = true;
        }
        else
        {
            m_is422Ok = false;
         //send to security
        }

        if(flagOmega == true && flag422 == true)
        {
            m_updateConsoleDataThread = std::thread(&MasterConsole::updateConsoleDataThread, this);
            LOG(INFO) << "MasterConsole Data Update thread ID: " << m_updateConsoleDataThread.get_id();
            m_updateConsoleDataThread.detach();
        }
    }

   if(m_MasterConsoleType == MasterConsoleType::DessightMaster)
   {
        m_is422Ok = m_transmitter.return422Status();
        if(m_is422Ok == true)
        {
            m_updateConsoleDataThread = std::thread(&MasterConsole::updateConsoleDataThread, this);
            LOG(INFO) << "MasterConsole Data Update thread ID: " << m_updateConsoleDataThread.get_id();
            m_updateConsoleDataThread.detach();
            SendInnerMsg(Module_Inner_E::Security, static_cast<int>(SecurityAction_E::RecvMasterSelfCheckResults), "ok");
        }
    }
}

void MasterConsole::assembleDataFromUSBAndEthernet()
{
    auto handlePoseTmp = m_transmitter.returnHandlePose();

    auto handlePose_Tmp = handlePoseTmp;
    auto handlePoseIR_Tmp = handlePoseTmp;
    auto handlePoseIIR_Tmp = handlePoseTmp;

    /*将viper数组中的数据取出进行滤波*/
    auto posDataFromViperTmp = handlePoseTmp.returnPNOData();
    auto handlePNO_Tmp = posDataFromViperTmp;
    auto handlePNOAFIR_Tmp = returnIRFilteredData(posDataFromViperTmp);
    auto handlePNOAFIIR_Tmp = returnIIRFilteredData(posDataFromViperTmp);

    /*保存滤波后的数据*/
    handlePose_Tmp.setMyConsoleData(handlePNO_Tmp);
    handlePoseIR_Tmp.setMyConsoleData(handlePNOAFIR_Tmp);
    handlePoseIIR_Tmp.setMyConsoleData(handlePNOAFIIR_Tmp);
    handlePose_Tmp.setQuaternionData(handlePNO_Tmp);
    handlePoseIR_Tmp.setQuaternionData(handlePNOAFIR_Tmp);
    handlePoseIIR_Tmp.setQuaternionData(handlePNOAFIIR_Tmp);

    if(m_FilterCase == static_cast<int>(FilterCase::FilterOFF))
    {
      m_handlePose_Cur.store(handlePose_Tmp);
    }
    else if(m_FilterCase == static_cast<int>(FilterCase::IRFilterOn))
    {
      m_handlePose_Cur.store(handlePoseIR_Tmp);
    }
    else if (m_FilterCase == static_cast<int>(FilterCase::IIRFilterOn))
    {
      m_handlePose_Cur.store(handlePoseIIR_Tmp);
    }
}


std::array<std::array<double,viperDataNumPerSensor>,2>  MasterConsole::returnIRFilteredData(const std::array<std::array<double,viperDataNumPerSensor>,2>& poseData_Cur)
{
   std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Cur;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Pre = m_poseDataAF_Pre;

   std::array<std::array<double,viperDataNumPerSensor>,2> t_Cur = poseData_Cur;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_Prev = m_poseData_Pre;

   for(int j = 0; j < 2; j++)
   {
      for(int i = 0; i < viperDataNumPerSensor; i++)
      {
         t_AF_Cur[j][i] = t_Cur[j][i] * m_IRnum[0] + t_Prev[j][i] * m_IRnum[1]
                         - t_AF_Pre[j][i] * m_IRden[1];
      }
   }
   
   m_poseData_Pre    = t_Cur;
   m_poseDataAF_Pre  = t_AF_Cur;
   return t_AF_Cur;
}

std::array<std::array<double,viperDataNumPerSensor>,2>  MasterConsole::returnIIRFilteredData(const std::array<std::array<double,viperDataNumPerSensor>,2>& poseData_Cur)
{
   std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Cur;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Pre = m_poseDataAF_Pre;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_PrePre = m_poseDataAF_PrePre;

   std::array<std::array<double,viperDataNumPerSensor>,2> t_Cur = poseData_Cur;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_Pre = m_poseData_Pre;
   std::array<std::array<double,viperDataNumPerSensor>,2> t_PrePre = m_poseData_PrePre;

   for(int j = 0; j < 2; j++)
   {
      for(int i = 0; i < viperDataNumPerSensor; i++)
      {
         t_AF_Cur[j][i] = t_Cur[j][i] * m_IRnum[0] + t_Pre[j][i] * m_IRnum[1] + t_PrePre[j][i] * m_IRnum[2]
                         - t_AF_Pre[j][i] * m_IRden[1] - t_AF_PrePre[j][i] * m_IRden[2];
      }
   }
   
   m_poseData_Pre       = t_Cur;
   m_poseData_PrePre    = t_Pre;
   m_poseDataAF_Pre     = t_AF_Cur;
   m_poseDataAF_PrePre  = t_AF_Pre;
   return t_AF_Cur;
}

void MasterConsole::masterConsoleBootSelfCheck()
{

    std::thread masterConsoleStatusCheck([this]
    {
        int retryCount = 3;
        while(retryCount > 0)
        {
            std::cout << "retryCount: " << retryCount << std::endl;
            if(m_MasterConsoleType == MasterConsoleType::Omega)
            {
                if(m_omega.returnOmegaStatus() == true)
                {
                    SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvMasterBootSta),"Ok");
                    return;
                }
                else if(m_omega.returnOmegaStatus() == false)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    if(retryCount > 0)
                    {
                        m_omega.initDevice();
                        LOG(INFO)<<"Try to restart Omega, count " << 4 - retryCount;
                    }
                    retryCount --;
                }

            }

        }
        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(MultipleDevAction_E::RecvMasterBootSta),"Err");
    });
    masterConsoleStatusCheck.detach();
}

void MasterConsole::masterConsoleStatusCheck()
{
    auto masterConsoleType = m_MasterConsoleType;
    switch(static_cast<int>(masterConsoleType))
    {
    case static_cast<int>(MasterConsoleType::DessightMaster):
        {
            bool is422Ok = m_transmitter.return422Status();
            m_isMasterConsoleOk.store(is422Ok);
            break;
        }
        default:
        {
            break;
        }
    }
}

void MasterConsole::turnOffMasterConsole()
{
    if(m_MasterConsoleType == MasterConsoleType::Omega)
    {
        m_omega.turnOffDevice();
    }
}

void MasterConsole::SendInnerMsg(Module_Inner_E recever,int Action, QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Security;
    msgTemp.Recver = recever;
    msgTemp.Request.insert(Action, arg);
    m_messagePool.SendMessage(msgTemp);
}

void MasterConsole::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Security;
    msgTemp.Recver = recever;
    for(int i = 0; i < arglist.length(); i++)
    {
        msgTemp.Request.insert(Action, arglist[i]);
    }
    m_messagePool.SendMessage(msgTemp);
}


void MasterConsole::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}

void MasterConsole::dealWithMsg()
{
    Message_Inner_T msg;
    while(1){
        m_MsgGottenRWLock.lockForRead();
        if(m_MsgGottenQueue.isEmpty() == false)
        {
            msg = m_MsgGottenQueue.dequeue();
        }else
        {
            m_MsgGottenRWLock.unlock();
            break;
        }
        m_MsgGottenRWLock.unlock();

        if(msg.Recver != Module_Inner_E::MasterConsole && msg.Recver != Module_Inner_E::MultipleModules){return;}

        QHash<int,QString>::ConstIterator i = msg.Request.constBegin();
        while(i != msg.Request.constEnd())
        {
            switch (i.key())
            {
                case static_cast<int>(MultipleDevAction_E::RecvLightBoardIoState)://RecvLightBoardIoState:
                {
                    break;
                }
                case static_cast<int>(MasterConsoleAction_E::BootSelfCheck):
                {
                    masterConsoleStatusCheck();
                    break;
                }
                case static_cast<int>(MasterConsoleAction_E::MasterShutDown):
                {
                    turnOffMasterConsole();
                    SendInnerMsg(Module_Inner_E::Security, static_cast<int>(SecurityAction_E::RecvMotorDriverShutDown), "Ok");
                    break;
                }


                default:break;
            }
            i++;
        }
    }
}


