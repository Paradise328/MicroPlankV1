 #include "MasterConsole.h"
std::ofstream outfile2("filterDataCheck_0805.txt");
std::chrono::high_resolution_clock::time_point startTime_master;

void MasterConsole::updateConsoleDataThread()
{
    while(!m_isSystemTerminated && !m_isSystemReset)
    {
        std::this_thread::sleep_until(startTime_master + std::chrono::milliseconds(5));
        masterConsoleStatusCheck();
        startTime_master = std::chrono::high_resolution_clock::now();
    }
}

void MasterConsole::startUpdateConsoleDataThread()
{
    /*根据外设初始化主手*/
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
        LOG(INFO) << "MasterConsoleType: DessightMaster";
        m_transmitter.startReadingThread();
        m_updateConsoleDataThread = std::thread(&MasterConsole::updateConsoleDataThread, this);
        LOG(INFO) << "MasterConsole Data Update thread ID: " << m_updateConsoleDataThread.get_id();
        m_updateConsoleDataThread.detach();
    }
}

void MasterConsole::assembleDataFromUSBAndEthernet()
{
    auto handlePoseTmp = m_transmitter.returnHandlePose();

    if(handlePoseTmp.stepPedal == pedalMidPress){handlePoseTmp.enablePedal = 1;}
    else{handlePoseTmp.enablePedal = 0;}

    auto armAnglePerSide = m_armAnglePerSide;
    m_transmitter.m_armAnglePerSide = armAnglePerSide;

    auto handlePoseIR_Tmp = handlePoseTmp;
    auto handlePoseIIR_Tmp = handlePoseTmp;
    auto handlePoseIIIR_Tmp = handlePoseTmp;

    /*将viper数组中的数据取出进行滤波*/
    auto posDataFromViperTmp = handlePoseTmp.returnPNOData();

    std::array<double,3> raw_L;
    std::array<double,3> raw_R;

    raw_L[0]=posDataFromViperTmp[0][0];
    raw_L[1]=posDataFromViperTmp[0][1];
    raw_L[2]=posDataFromViperTmp[0][2];
    raw_R[0]=posDataFromViperTmp[1][0];
    raw_R[1]=posDataFromViperTmp[1][1];
    raw_R[2]=posDataFromViperTmp[1][2];

    auto kalman_result = KalmanStep(raw_L);

    // std::ofstream outfile("positionkalman.txt",std::ios::app);
    // outfile<<raw_L[1]<<" "<<raw_L[2] <<" "<<kalman_result[1]<<" "<<kalman_result[2]<<"\n";
    // outfile.close();

    /*保存滤波后的数据*/

    if(m_FilterCase == static_cast<int>(FilterCase::FilterOFF) || m_FilterCase == static_cast<int>(FilterCase::IRFilterOn))
    {
        auto PNODataAFIR_Tmp = returnIRFilteredData(posDataFromViperTmp);
        handlePoseIR_Tmp.setMyConsoleData(PNODataAFIR_Tmp);
        m_handlePose_Cur.store(handlePoseIR_Tmp);
    }
    else if (m_FilterCase == static_cast<int>(FilterCase::IIRFilterOn))
    {
        auto PNODataAFIIR_Tmp = returnIIRFilteredData(posDataFromViperTmp);
        handlePoseIIR_Tmp.setMyConsoleData(PNODataAFIIR_Tmp);
        // outfile2 <<posDataFromViperTmp[1][0] << " " << posDataFromViperTmp[1][1] << " " << posDataFromViperTmp[1][2] << " " <<handlePoseIIR_Tmp.handlePoseR_X <<" "<<handlePoseIIR_Tmp.handlePoseR_Y<<" "<<handlePoseIIR_Tmp.handlePoseR_Z<<"\n";
        m_handlePose_Cur.store(handlePoseIIR_Tmp);
    }
    else if (m_FilterCase == static_cast<int>(FilterCase::IIIRFilterOn))
    {
        auto PNODataAFIIIR_Tmp = returnIIIRFilteredData(posDataFromViperTmp);
        handlePoseIIIR_Tmp.setMyConsoleData(PNODataAFIIIR_Tmp);
        // std::cout << posDataFromViperTmp[0][0] << " " << posDataFromViperTmp[0][1] << " "<< posDataFromViperTmp[0][2]  << "\n";
        // std::cout << PNODataAFIIIR_Tmp[0][0] << " " << PNODataAFIIIR_Tmp[0][1] << " "<< PNODataAFIIIR_Tmp[0][2]  << "\n";
        m_handlePose_Cur.store(handlePoseIIIR_Tmp);
    }

}

void MasterConsole::initFilter()
{
    m_poseData_Pre = {0};
    m_poseData_PrePre = {0};
    m_poseData_PrePrePre = {0};
    m_poseDataAF_Pre = {0};
    m_poseDataAF_PrePre = {0};
    m_poseDataAF_PrePrePre = {0};
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
      for(int i = 0; i < 7; i++)
      {
        t_AF_Cur[j][i] = t_Cur[j][i] * m_IIRnum[0] + t_Pre[j][i] * m_IIRnum[1] + t_PrePre[j][i] * m_IIRnum[2]
                     - t_AF_Pre[j][i] * m_IIRden[1] - t_AF_PrePre[j][i] * m_IIRden[2];
      }
   }

   m_poseData_Pre       = t_Cur;
   m_poseData_PrePre    = t_Pre;
   m_poseDataAF_Pre     = t_AF_Cur;
   m_poseDataAF_PrePre  = t_AF_Pre;

   return t_AF_Cur;
}

std::array<std::array<double,viperDataNumPerSensor>,2>  MasterConsole::returnIIIRFilteredData(const std::array<std::array<double,viperDataNumPerSensor>,2>& poseData_Cur)
{
    std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Cur;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_Pre = m_poseDataAF_Pre;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_PrePre = m_poseDataAF_PrePre;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_AF_PrePrePre = m_poseDataAF_PrePrePre;

    std::array<std::array<double,viperDataNumPerSensor>,2> t_Cur = poseData_Cur;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_Pre = m_poseData_Pre;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_PrePre = m_poseData_PrePre;
    std::array<std::array<double,viperDataNumPerSensor>,2> t_PrePrePre = m_poseData_PrePrePre;

    for(int j = 0; j < 2; j++)
    {
        for(int i = 0; i < 7; i++)
        {
            t_AF_Cur[j][i] = t_Cur[j][i] * m_IIIRnum[0] + t_Pre[j][i] * m_IIIRnum[1] + t_PrePre[j][i] * m_IIIRnum[2] + t_PrePrePre[j][i] * m_IIIRnum[3]
                             - t_AF_Pre[j][i] * m_IIIRden[1] - t_AF_PrePre[j][i] * m_IIIRden[2] - t_AF_PrePrePre[j][i] * m_IIIRden[3];
        }
    }

    m_poseData_Pre          = t_Cur;
    m_poseData_PrePre       = t_Pre;
    m_poseData_PrePrePre    = t_PrePre;

    m_poseDataAF_Pre        = t_AF_Cur;
    m_poseDataAF_PrePre     = t_AF_Pre;
    m_poseDataAF_PrePrePre  = t_AF_PrePre;

    return t_AF_Cur;
}

std::array<double,3> MasterConsole::KalmanStep(const std::array<double,3>& raw)
{
    std::array<double,3> out{};

    // 状态转移矩阵 F, 观测矩阵 H
    double F[2][2] = {{1, dt}, {0, 1}};
    double H[2] = {1, 0};

    // 过程噪声 Q
    double q11 = sigma_a*sigma_a * (dt*dt*dt/3.0);
    double q12 = sigma_a*sigma_a * (dt*dt/2.0);
    double q22 = sigma_a*sigma_a * dt;

    double Q[2][2] = {{q11, q12}, {q12, q22}};

    for (int i = 0; i < 3; ++i) {
        if (!inited_[i]) {
            // 初始化
            x_est_[i][0] = raw[i];   // 初始位置
            x_est_[i][1] = 0.0;      // 初始速度
            P_[i] = {{{1,0},{0,1}}};
            inited_[i] = true;
            out[i] = raw[i];
            continue;
        }

        // ---- 预测 ----
        double x_pred[2];
        x_pred[0] = F[0][0]*x_est_[i][0] + F[0][1]*x_est_[i][1];
        x_pred[1] = F[1][0]*x_est_[i][0] + F[1][1]*x_est_[i][1];

        double P_pred[2][2];
        for(int r=0;r<2;++r){
            for(int c=0;c<2;++c){
                P_pred[r][c] = F[r][0]*P_[i][0][c] + F[r][1]*P_[i][1][c];
            }
        }
        // P_pred = F P F^T + Q
        double P_pred_full[2][2];
        for(int r=0;r<2;++r){
            for(int c=0;c<2;++c){
                P_pred_full[r][c] = P_pred[r][0]*F[c][0] + P_pred[r][1]*F[c][1] + Q[r][c];
            }
        }

        // ---- 更新 ----
        double z = raw[i];
        double y = z - (H[0]*x_pred[0] + H[1]*x_pred[1]); // 创新

        double S = H[0]*(P_pred_full[0][0]*H[0] + P_pred_full[0][1]*H[1])
                   + H[1]*(P_pred_full[1][0]*H[0] + P_pred_full[1][1]*H[1])
                   + R;

        double K[2];
        K[0] = (P_pred_full[0][0]*H[0] + P_pred_full[0][1]*H[1]) / S;
        K[1] = (P_pred_full[1][0]*H[0] + P_pred_full[1][1]*H[1]) / S;

        // 更新状态
        x_est_[i][0] = x_pred[0] + K[0]*y;
        x_est_[i][1] = x_pred[1] + K[1]*y;

        // 更新协方差: P = (I - K H) P_pred
        double KH[2][2] = {{K[0]*H[0], K[0]*H[1]},
                           {K[1]*H[0], K[1]*H[1]}};
        double I_KH[2][2] = {{1-KH[0][0], -KH[0][1]},
                             {-KH[1][0], 1-KH[1][1]}};
        for(int r=0;r<2;++r){
            for(int c=0;c<2;++c){
                P_[i][r][c] = I_KH[r][0]*P_pred_full[0][c] + I_KH[r][1]*P_pred_full[1][c];
            }
        }

        // 输出位置
        out[i] = x_est_[i][0];
    }

    return out;
}

void MasterConsole::masterConsoleBootSelfCheck()/*检测master状态，实时灯板控制*/
{

    std::thread masterConsoleStatusCheck([this]
    {
        int retryCount = 3;
        while(retryCount > 0)
        {
            printf("retryCount: %d\n", retryCount);
            if(m_MasterConsoleType == MasterConsoleType::DessightMaster)
            {
                if(m_transmitter.return422Status() == true)
                {
                    SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvMasterBootSta),"Ok");
                    return;
                }
                else if(m_transmitter.return422Status() == true == false)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    if(retryCount > 0)
                    {
                        m_transmitter.initDevice();
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
            assembleDataFromUSBAndEthernet();
            m_isMasterConsoleOk.store(is422Ok);
            // LOG(INFO)<<"MASTER 422Ok: "<<is422Ok;
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
                case static_cast<int>(MasterConsoleAction_E::SwitchInstrumentAngle):
                {

                    if(i.value() == "30")
                    {
                        LOG(INFO) << "Instrument Angle double Side : 15";
                        m_armAnglePerSide = 15;
                    }else if(i.value() ==  "60")
                    {
                        LOG(INFO) << "Instrument Angle double Side : 30";
                        m_armAnglePerSide = 30;
                    }
                    break;
                }

                case static_cast<int>(MasterConsoleAction_E::Open3rdFilter):
                {
                    m_FilterCase.store(static_cast<int>(FilterCase::IIIRFilterOn));
                    LOG(INFO) << "Open 3rd Filter";
                    initFilter();
                    break;
                }
                case static_cast<int>(MasterConsoleAction_E::Open2rdFilter):
                {
                    m_FilterCase.store(static_cast<int>(FilterCase::IIRFilterOn));
                    initFilter();
                    LOG(INFO) << "Open 2rd Filter";

                    break;
                }
                default:break;
            }
            i++;
        }
    }
}


