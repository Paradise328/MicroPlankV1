#include "MasterConsole.h"
MasterConsole* MasterConsole::m_selfPointer = nullptr;

void MasterConsole::updateConsoleDataThread()
{
   while(true)
   {
      assembleDataFromUSBAndEthernet();
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
         m_isViperOk == true;
      }
      else
      {
         m_isViperOk == false;
         //send to security
      }

      bool flag422 = true;
      if(flag422 == true)
      {
         m_is422Ok == true;
      }
      else
      {
         m_is422Ok == false;
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
      m_omega.startThread(masterPromise);
      const auto flagOmega = masterFuture.get();
      LOG(INFO) << "Master handle thread (Omega) starts!";
      bool flag422 = true;
      if(flag422 == true)
      {
         m_is422Ok == true;
      }
      else
      {
         m_is422Ok == false;
         //send to security
      }

      if(flagOmega == true && flag422 == true)
      {
         m_updateConsoleDataThread = std::thread(&MasterConsole::updateConsoleDataThread, this);
         LOG(INFO) << "MasterConsole Data Update thread ID: " << m_updateConsoleDataThread.get_id();
         m_updateConsoleDataThread.detach();
         //send to Security everything is ok;
      }
   }
}

void MasterConsole::assembleDataFromUSBAndEthernet()
{
   auto posDataFromViperTmp = m_viper.return_viperData();
   if(m_MasterConsoleType == MasterConsoleType::Omega){
      auto posDataFromOmegaTmp = m_omega.return_omegaData();
      posDataFromViperTmp[1] = posDataFromOmegaTmp;
      }
   
   handleOtherData otherDataFromWhere;//Get other Data From other Device;



   HandlePose handlePose_Tmp, handlePoseIR_Tmp, handlePoseIIR_Tmp;
   auto handlePNO_Tmp = posDataFromViperTmp;
   auto handlePNOAFIR_Tmp = returnIRFilteredData(posDataFromViperTmp);
   auto handlePNOAFIIR_Tmp = returnIIRFilteredData(posDataFromViperTmp);

   handlePose_Tmp.setMyConsoleData(handlePNO_Tmp);
   handlePoseIR_Tmp.setMyConsoleData(handlePNOAFIR_Tmp);
   handlePoseIIR_Tmp.setMyConsoleData(handlePNOAFIIR_Tmp);

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

std::array<std::array<double,viperDataNumPerSensor>,2>  MasterConsole::returnIRFilteredData(const std::array<std::array<double,7>,2>& poseData_Cur)
{
   std::array<std::array<double,7>,2> t_AF_Cur;
   std::array<std::array<double,7>,2> t_AF_Pre = m_poseDataAF_Pre;

   std::array<std::array<double,7>,2> t_Cur = poseData_Cur;
   std::array<std::array<double,7>,2> t_Prev = m_poseData_Pre;

   for(int j = 0; j < 2; j++)
   {
      for(int i = 0; i < 7; i++)
      {
         t_AF_Cur[j][i] = t_Cur[j][i] * m_IRnum[0] + t_Prev[j][i] * m_IRnum[1]
                         - t_AF_Pre[j][i] * m_IRden[1];
      }
   }
   
   m_poseData_Pre    = t_Cur;
   m_poseDataAF_Pre  = t_AF_Cur;
   return t_AF_Cur;
}

std::array<std::array<double,viperDataNumPerSensor>,2>  MasterConsole::returnIIRFilteredData(const std::array<std::array<double,7>,2>& poseData_Cur)
{
   std::array<std::array<double,7>,2> t_AF_Cur;
   std::array<std::array<double,7>,2> t_AF_Pre = m_poseDataAF_Pre;
   std::array<std::array<double,7>,2> t_AF_PrePre = m_poseDataAF_PrePre;

   std::array<std::array<double,7>,2> t_Cur = poseData_Cur;
   std::array<std::array<double,7>,2> t_Pre = m_poseData_Pre;
   std::array<std::array<double,7>,2> t_PrePre = m_poseData_PrePre;

   for(int j = 0; j < 2; j++)
   {
      for(int i = 0; i < 7; i++)
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

void  MasterConsole::setMyFilterCase(const int& filterCase)
{
   m_selfPointer->m_FilterCase = filterCase;
}

