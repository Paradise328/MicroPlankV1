#ifndef MASTERCONSOLE_H
#define MASTERCONSOLE_H

#include <eigen3/Eigen/Dense>
#include <atomic>
#include "Modules/LoggerModule/easylogging++.h"
#include "Modules/MathModule/MathUtils.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include "Viper_Lib/viper_ui.h"
#include "Omega_Lib/FdOmega.h"
#include "Modules/MsgModule/messagequeue.h"
#include "Modules/SystemUtilsModule/SystemUtils.h"
#include "MasterConsole_Lib/viper_transmitter.h"

enum class MasterConsoleType
{
    Viper           = 0x00,
    Omega           = 0x01,
    Touch           = 0x02,
    DessightMaster  = 0x03,
};

class MasterConsole: public QObject
{
    Q_OBJECT
    friend class RobotControl;
public:
    MasterConsole() = delete;
    explicit MasterConsole(MasterConsoleType masterConsoleType, MessageQueue&  messagePool):
                            m_MasterConsoleType(masterConsoleType),
                            m_messagePool(messagePool),
                            m_isSystemTerminated(false),
                            m_isSystemReset(false),
                            m_FilterCase(static_cast<int>(FilterCase::IIRFilterOn))
                            {
                                LOG(INFO) << "构造主手函数";
                                connect(this, &MasterConsole::DealMsgSignal, this, &MasterConsole::dealWithMsg);//当第一个函数被触发后，就执行第二个函数
                            }

    void                        updateConsoleDataThread();

    /*外部调用*/
    void                        startUpdateConsoleDataThread();

    /*用于RobotControl类调用主控台中主手信息*/
    HandlePose                  returnHandlePose(){return m_handlePose_Cur.load();}//3个位置和3个姿态

    /*用于Security调用主控台状态*/
    bool                        returnMasterConsoleStatus(){return m_isMasterConsoleOk.load();}

    /*消息队列*/
    void                        GetAmMsg(Message_Inner_T msg);

private:

    MasterConsoleType           m_MasterConsoleType; //Console Type;
    viper_ui                    m_viper;
    FdOmega                     m_omega;
    Viper_Transmitter           m_transmitter;

    std::thread                 m_updateConsoleDataThread;
    void                        turnOffMasterConsole();
    void                        assembleDataFromUSBAndEthernet();

    /*主控台模块状态指示*/
    bool                        m_isViperOk = false;/*Viper型主手*/
    bool                        m_isOmegaOk = false;/*Omega型主手*/
    bool                        m_is422Ok = false;/*脚踏、主手张开角度*/
    std::atomic<bool>           m_isMasterConsoleOk = false;/*主控台*/
    void                        masterConsoleStatusCheck();
    void                        masterConsoleBootSelfCheck();

    /*系统是否关闭，是否复位, 关闭时关闭所有线程*/
    std::atomic<bool>           m_isSystemTerminated = false;
    std::atomic<bool>           m_isSystemReset = false;

    mutable HandlePose          m_handlePose_Pre;
    mutable HandlePose          m_handlePose_PrePre;
    mutable HandlePose          m_handlePoseAF_Pre;
    mutable HandlePose          m_handlePoseAF_PrePre;
    std::atomic<HandlePose>     m_handlePose_Cur;
    std::atomic<std::array<std::array<double,viperDataNumPerSensor>,2>> m_sensorData_Cur;

    std::atomic<int>            m_FilterCase; //0: No Filter; 1: IR Filter; 2: IIR Filter
    const std::vector<double>   m_IRnum{0.0337, 0.0337};  // Numerator coefficients
    const std::vector<double>   m_IRden{1.0, -0.9325};
    const std::vector<double>   m_IIRnum{0.001160721940461, 0.002321443880922, 0.001160721940461};  // Numerator coefficients ()
    const std::vector<double>   m_IIRden{1.0, -1.901343793847116, 0.905986681608960};// (b)
    mutable std::array<std::array<double,viperDataNumPerSensor>,2> m_poseData_Cur, m_poseData_Pre, m_poseData_PrePre, m_poseDataAF_Pre, m_poseDataAF_PrePre; 
    std::array<std::array<double,viperDataNumPerSensor>,2>        returnIRFilteredData(const std::array<std::array<double,viperDataNumPerSensor>,2>& poseData_Cur);
    std::array<std::array<double,viperDataNumPerSensor>,2>        returnIIRFilteredData(const std::array<std::array<double,viperDataNumPerSensor>,2>& poseData_Cur);

    //MessageQueue relative function
    MessageQueue            &m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock          m_MsgGottenRWLock;
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
    void                    dealWithMsg();

signals:
   void                     DealMsgSignal();
   void                     SendMsgSignal(const Message_Inner_T msg);
};

#endif
