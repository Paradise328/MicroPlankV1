#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include "../MasterModule/MasterConsole.h"
#include "../MotorDriverModule/MotorDriver.h"
#include "../SystemUtilsModule/SystemUtils.h"
#include "../MathModule/lowpass_filter.h"
#include "DomainController.h"
#include "BlasControl/actuators_controler.h"
#include "BlasControl/BLA_API.h"
#include "BlasControl/communication.h"
#include "DomainController.h"
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <iostream>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <ruckig/ruckig.hpp>
#include <bitset>

constexpr int ControlValueNum = 15;
constexpr int MotorNumPerSide = 11;
constexpr int GuidingMotorNum = 3;

constexpr int JointEncoderPerRevolution = 524288;

constexpr int JointEncoderInit_1_R = 524288/2;//初始值，角度为0时的编码器值
constexpr int JointEncoderInit_2_R = 524288/2;
constexpr int JointEncoderInit_3_R = 524288/2;
constexpr int JointEncoderInit_1_L = 524288/2;
constexpr int JointEncoderInit_2_L = 524288/2;
constexpr int JointEncoderInit_3_L = 524288/2;

/*当l1=150mm，l2=180mm*/
constexpr double JointAngleMax60_1_R = -29.050564;
constexpr double JointAngleMin60_1_R = -61.936432;
constexpr double JointAngleMax60_2_R = 111.677606;
constexpr double JointAngleMin60_2_R = 79.025375;
constexpr double JointAngleMax60_3_R = -8.286049;
constexpr double JointAngleMin60_3_R = -35.656273;

constexpr double JointAngleMax60_1_L = 61.936432;
constexpr double JointAngleMin60_1_L = 29.050564;
constexpr double JointAngleMax60_2_L = -79.025375;
constexpr double JointAngleMin60_2_L = -111.677606;
constexpr double JointAngleMax60_3_L = 35.656273;
constexpr double JointAngleMin60_3_L = 8.286049;


//constexpr double MaxonEncoderPerRevolution = 15955.67867;
constexpr double EncoderPerGrade = 1456.356;

constexpr int Gimbal1_R = 7;

constexpr int Joint1_R = 1;
constexpr int Joint2_R = 2;
constexpr int Joint3_R = 3;
constexpr int Joint4_R = 4;
constexpr int Joint1_L = 1;
constexpr int Joint2_L = 2;
constexpr int Joint3_L = 3;
constexpr int Joint4_L = 4;

constexpr int DOF = 6;
constexpr int Position_DOF = 3;
constexpr int Joint_DOF = 3;

// constexpr int m_angle = 30;

namespace msm = boost::msm;
namespace mpl = boost::mpl;//Meta Programming Library

enum class GuidingArmState{
    INIT = 0x00,
    HOLD = 0x01,
    DRAG = 0x02,
    DAMPING = 0x03,
};

enum class GuidingArmFrictionState{
    IDLE = 0x00,
    MOVING = 0x01
};

class GuidingArm{
public:
    GuidingArmState                 m_guidingArmCurrentState = GuidingArmState::INIT;
    GuidingArmFrictionState         m_guidingArmCurrentFrictionStateJoint0 = GuidingArmFrictionState::IDLE;
    GuidingArmFrictionState         m_guidingArmCurrentFrictionStateJoint1 = GuidingArmFrictionState::IDLE;
    GuidingArmFrictionState         m_guidingArmCurrentFrictionStateJoint2 = GuidingArmFrictionState::IDLE;
    bool                            m_isGuidingArmStable;
    bool                            m_guidingArmEnableBtnPressed;
    int16_t                         m_actualCurrent0;
    int16_t                         m_actualCurrent1;
    int16_t                         m_actualCurrent2;
    int16_t                         m_actualTorque0;
    int16_t                         m_actualTorque1;
    int16_t                         m_actualTorque2;
    double                          m_actualVelocity0;
    double                          m_actualVelocity1;
    double                          m_actualVelocity2;
    std::array<double,3>            m_velocity;
    std::array<double,3>            m_velocity_1stOrder;
    std::array<double,3>            m_velocity_2ndOrder;

private:

};

class RobotControl:public QObject
{
    Q_OBJECT

public:

    explicit RobotControl(MasterConsole& masterConsole, MotorDriver* motorDriver, DomainController* domainController,MessageQueue&  messagePool):
        m_masterConsole(masterConsole),
        m_motorDriver(motorDriver),
        m_domainController(domainController),
        m_messagePool(messagePool),
        m_teleOperationdMode(TeleOperationMode::CSV_Mode),
        m_ruckigPlanner_R(0.005),
        m_ruckigPlanner_L(0.005),
        m_flagControlThread(false),
        m_isSystemTerminated(false),
        m_guidingArm1stOrder({0}),
        m_guidingArm2ndOrder({0}),
        m_endeffectorConfiguration(EndeffectorConfiguration::fourMaxons),
        // m_endeffectorConfiguration(EndeffectorConfiguration::sixMaxons),
        m_filter_1storder_guiding(m_guidingArm1stOrder, 250.0, 60.0),
        m_filter_2ndorder_guiding(m_guidingArm2ndOrder, 250.0, 60.0)
    {
        readMyInitData();

        initiAllData();

        connect(this, &RobotControl::DealMsgSignal, this, &RobotControl::dealWithMsg);

        openTorqueSensor();
    }

    void                            GetAmMsg(Message_Inner_T msg);

    MotorDriver*                    getMyMotorDriver(){return m_motorDriver;}

    void                            startMyThreads();


    void                            setMyMotorDriver(MotorDriver* motorDriver){m_motorDriver = motorDriver;}

signals:

    void                            DealMsgSignal();

private:

    Actuators_Controler             AC;

    MotorDriver*                    m_motorDriver;

    Viper_Transmitter*              m_viper_Transmitter;

    DomainController*               m_domainController;

    std::array<double,3>            m_forceAccumulationBufferLeft;

    std::array<double,3>            m_forceAccumulationBufferRight;

    std::array<double,3>            m_forceAccumulationBufferLeft_judge;

    std::array<double,3>            m_forceAccumulationBufferRight_judge;

    MasterConsoleType               m_masterConsoleType;

    MasterConsole&                  m_masterConsole;

    std::atomic<bool>               m_isSystemTerminated;


    /*消息队列相关函数*/
    MessageQueue                    &m_messagePool;

    QQueue<Message_Inner_T>         m_MsgGottenQueue;

    QReadWriteLock                  m_MsgGottenRWLock;

    void                            SendInnerMsg(Module_Inner_E recever,int Action,QString arg);

    void                            SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);

    void                            dealWithMsg();

    /*初始化数据*/
    void                            readMyInitData();

    void                            loadEndeffectorConfig();

    void                            initiAllData();

    double                          m_initMoonsEn5code;//光电门为0时moons编码器数值

    //自适应低通滤波参数

    std::array<bool,3> inited_L {false, false, false};
    std::array<double,3> xhat_L   {0.0, 0.0, 0.0};
    std::array<double,3> vhat_L   {0.0, 0.0, 0.0};
    std::array<double,3> ahat_L   {0.0, 0.0, 0.0};
    std::array<double,3> fc_smooth_L   {0.0, 0.0, 0.0};\

    std::array<bool,3> inited_R {false, false, false};
    std::array<double,3> xhat_R   {0.0, 0.0, 0.0};
    std::array<double,3> vhat_R   {0.0, 0.0, 0.0};
    std::array<double,3> ahat_R   {0.0, 0.0, 0.0};
    std::array<double,3> fc_smooth_R   {0.0, 0.0, 0.0};

    //位移累积（滑动窗口）
    std::deque<double> dispHist_L[3];
    std::deque<double> dispHist_R[3];

    double fmin     = 0.1;//1.2;   // 低速/静止截止 [Hz]
    double beta     = 3.53;//0.43;  // 速度->截止斜率 [Hz/(单位/秒)]
    double fd       = 18.0;  // 速度通道固定截止 [Hz]
    double dt       = 0.004; // 采样周期（固定 4 ms；若用真实 dt 就每帧更新它）
    double fc_max   = 40.0;  // f_c 上限（<=0 则不限制）
    double vel_dead = 0.0;   // 速度微小死区（0 关闭）
    double beta_a = 0.2;//加速度权重
    double beta_d = 0.1;//累积位移权重
    double win_len = 0.3;//位移窗口累计
    double tau_up = 0.05;//fc下降时间常数
    double tau_down = 0.3;//fc上升时间常数

    /*速度補償*/

    std::array<int, MotorNumPerSide> m_EncodeErr_Pre_L;
    std::array<int, MotorNumPerSide> m_EncodeErr_Pre_R;

    std::array<double,4> m_EncodeErr_Int;

    /*控制函数*/
    std::thread                     m_calculateControlDataThread;

    void                            startControlThread();

    void                            control();

    std::atomic<bool>               m_flagControlThread;

    /* guiding arm control function and variables */
    GuidingArm                      m_guidingArm;
    std::thread                     m_guidingArmControlThread;
    std::array<double,3>           m_guidingArm1stOrder;
    std::array<double,3>           m_guidingArm2ndOrder;
    LowpassFilter1stOrder<std::array<double,3>>    m_filter_1storder_guiding;
    LowpassFilter2ndOrder<std::array<double,3>>    m_filter_2ndorder_guiding;
    void                            updateGuidingArmMotion();
    void                            updateGuidingArmState();
    bool                            judgeGuidingArmStable();
    void                            startGuidingArmControlThread();
    void                            guidingArmControl();
    void                            initGuidingArm();
    void                            disableGuidingArm();
    void                            guidingArmPrinting();
    void                            guidingArmLogging();
    double                          signDouble(double target);
    double                          computeGuidingArmFrictionTorque(double velocity, double tau_c, double B, double k);
    bool                            isGuidingArmDamping();
    void                            guidingArmHold2Drag();
    void                            guidingArmDrag2Damp();
    void                            guidingArmDamp2Drag();
    void                            guidingArmDamp2Hold();
    void                            applyGuidingArmForceControl();
    void                            applyGuidingArmDampingControl();
    void                            applyGuidingArmVelocityControl();

    /* 控制模式 */

    EndeffectorConfiguration        m_endeffectorConfiguration;// = EndeffectorConfiguration::fourMaxons;

    void                            setRobotControlMode(const RobotControlMode& tartgetRobotControlMode);

    std::atomic<RobotControlMode>   m_curRobotControlMode = RobotControlMode::InitMode;
    std::atomic<RobotControlMode>   m_prevRobotControlMode = RobotControlMode::Hold;

    /*测试模式： 1主从 2重复定位精度测试 3步进测试 4夹持测试*/
    int m_oprationMode = 1;

    /* 上电状态 */
    void                            initMotor();

    void                            goToHold();

    std::atomic<bool>               m_flagInHold = false;

    /* teleoperation */
    TeleOperationMode               m_teleOperationdMode;

    void                            goToTeleOperation();

    void                            teleoperation();

    std::atomic<bool>               m_flagInTeleoperation = false;

    void                            sendMotorData_Teleop(const std::array<int, MotorNumPerSide>& targetEncoderCur_R, const std::array<int, MotorNumPerSide>& targetVelCur_R,
                                                         const int& enableTag_R,
                                                         const std::array<int, MotorNumPerSide>& targetEncoderCur_L, const std::array<int, MotorNumPerSide>& targetVelCur_L,
                                                         const int& enableTag_L);

    /*吊杆拖拽*/
    void                            goToCollaboration_EndJoint();
    void                            goToCollaboration_GuidingArm();

    void                            collaboration_EndJoint();
    void                            collaboration_GuidingArm();

    std::atomic<bool>               m_flagInCollaboration_EndJoint = false;
    std::atomic<bool>               m_flagInCollaboration_GuidingArm = false;

    uint8_t                         m_domainDigtalPre_L;
    uint8_t                         m_domainDigtalPre_R;

    /*与MotorDriver通信*/
    void                            receiveMotorData();
    /* communicate with domain controller */

    void                            sendMotorData(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                                  const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L);

    void                            sendMotorData_4Maxons_ForceControl(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                               const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L);

    /*EtherCAT各从站信息*/
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorEncoderCur_R;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorEncoderCur_L;
    std::atomic<std::array<int, 3>>                         m_endEffectorTarget_L;
    std::atomic<std::array<int, 3>>                         m_endEffectorTarget_R;

    std::atomic<std::array<int, MotorNumPerSide>>           m_motorOperationMode_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorErrorCode_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorTrq_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorCur_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorFollowingPosErr_L;

    std::atomic<std::array<int, MotorNumPerSide>>           m_motorStatusWordCur_R;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorStatusWordCur_L;
    std::atomic<std::array<int, 6>>                         m_digitalInputGuiding;
    std::atomic<std::array<int, MotorNumPerSide>>           m_MotorTargetVel_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_MotorTargetVel_R;

    std::atomic<std::array<double,3>>  m_force_L;
    std::atomic<std::array<double,3>>  m_force_R;

    std::atomic<uint32_t>                       m_magneticEncoder_L;
    std::atomic<uint32_t>                       m_magneticEncoder_R;

    std::array<double,3>            OneEuroStep(const std::array<double,3>&raw,const char& side);
    /*进入使能时计算初始位置*/
    void                            calculateEndEffectorPosition_init(const HandlePose& handlePoseCur, const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side);

    /*机械臂实际位置*/
    std::array<double, 3>           calculateEndEffectorPosition(const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side);

    /*轨迹规划部分*/
    ruckig::Ruckig<DOF>             m_ruckigPlanner_R;

    ruckig::InputParameter<DOF>     m_ruckigInputState_R;

    ruckig::OutputParameter<DOF>    m_ruckigOutputState_R;

    ruckig::Trajectory<DOF>         m_ruckigPlannedTrajectory_R;

    ruckig::Ruckig<DOF>             m_ruckigPlanner_L;

    ruckig::InputParameter<DOF>     m_ruckigInputState_L;

    ruckig::OutputParameter<DOF>    m_ruckigOutputState_L;

    ruckig::Trajectory<DOF>         m_ruckigPlannedTrajectory_L;

    /*复位功能*/
    void                            endJointGoHome(const char& side);
    void                            zeroErrGoHome(const char& side);

    void                            MaxonGoHome(const char& side);
    void                            MaxonGoHome_(const char& side);

    void                            changeAngle_L();
    void                            changeAngle_R();

    int                             MagneticEncoder_Init_L = 13875025;
    int                             MagneticEncoder_Max_L = 30000;
    int                             MagneticEncoder_Min_L = -30000;

    int                             MagneticEncoder_Init_R = 9506863;
    int                             MagneticEncoder_Max_R = 30000;
    int                             MagneticEncoder_Min_R = -30000;

    int                             m_moonsEncodeZero_L;/*x轴0位编码器值*/
    int                             m_moonsEncodeZero_R;

    mutable double                     m_armAnglePerSide = 30.0;
    std::atomic<std::array<int, MotorNumPerSide>>                  m_motorHomingStatus_R;
    std::atomic<std::array<int, MotorNumPerSide>>                  m_motorHomingStatus_L;

    bool m_maxonCaliFinish_R = 0;
    bool m_maxonCaliFinish_L = 0;
    bool m_jointCaliFinish_R = 0;
    bool m_jointCaliFinish_L = 0;
    bool m_moonsCaliFinish_R = 0;
    bool m_moonsCaliFinish_L = 0;

    /*从本地文件读取系统参数等*/
    std::string                     m_configFilePath  = "/home/a/Desktop/codes/MikroPlanckV1/Config/EndeffectorData.toml";
    std::string                     m_robotConfigPath = "/home/a/Desktop/codes/MikroPlanckV1/Config/RobotData.toml";

    mutable std::string             m_endEffectorLeft   = "CZQ_4MM_1";
    mutable std::string             m_endEffectorRight  = "CZQ_4MM_1";

    // mutable std::string             m_endEffectorLeft   = "CZQ_3MM_1";
    // mutable std::string             m_endEffectorRight  = "CZQ_3MM_1";

    double                          m_initRotAroundY_L, m_initRotAroundX_L;  //Read From Toml
    double                          m_initRotAroundY_R, m_initRotAroundX_R;  //Read From Toml
    double                          m_sourceRotAroundY = -90 * M_PI / 180;  //Read From Toml
    // double                          m_theta = 30 ;

    std::array<int,    5>           m_motionScaling;//Read From Toml
    // std::array<double, 2>           m_EncoderPerDegreeScalingFactor = {1, 1.2};

    std::array<double, 4>           m_encoderPerDegree_L;//Read From Toml
    std::array<double, 4>           m_encoderPerDegree_R;//Read From Toml
    std::array<int, 3>              m_encoderPerMM_L = {0};//Read From Toml
    std::array<int, 3>              m_encoderPerMM_R = {0};//Read From Toml

    int                             m_kForcepSpeed     = 50000;
    int                             m_ForcepSpeedLimit = 150000;
    int                             m_controlLoopNum = 0;

    std::array<double, pedalSwitchNumber>    m_kGimbalPosition_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_kGimbalPosition_R = {0};//Encoder per delt Translational Motion Right
    std::array<double, pedalSwitchNumber>    m_kGimbalSpeed_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_kGimbalSpeed_R = {0};//Encoder per delt Translational Motion Right
    std::array<double, pedalSwitchNumber>    m_GimbalMaxSpeed_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_GimbalMaxSpeed_R = {0};// Encoder per delt Translat ional Motion Right

    std::array<double, MotorNumPerSide>    m_speedDirection_L = {0};//Read From Toml
    std::array<double, MotorNumPerSide>    m_speedDirection_R = {0};//Read From Toml

    std::array<double, MotorNumPerSide>    m_SpeedDirection_R = {1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1};
    std::array<double, MotorNumPerSide>    m_SpeedDirection_L = {1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1};

    std::array<double, MotorNumPerSide>    m_kForcepPosition_R = {4000, 1456.356, 1456.356, 1456.356, 110.8, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768};
    std::array<double, MotorNumPerSide>    m_kForcepPosition_small_R = {4000, 1456.356, 1456.356, 1456.356, 110.8 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75};

    std::array<double, MotorNumPerSide>    m_kForcepPosition_L = {4000, 1456.356, 1456.356, 1456.356, 110.8, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768};
    std::array<double, MotorNumPerSide>    m_kForcepPosition_small_L = {4000, 1456.356, 1456.356, 1456.356, 110.8 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75};

    double                          m_compRatio_L = 0.765;//Read From Toml
    double                          m_compRatio_R = 0.765;//Read From Toml

    /*速度档位*/
    void                            setNewSpeed(const HandlePose& handlePosePrev,
                                                const HandlePose& handlePoseCur);
    void                            setVoiceCommand();
    mutable int                     m_posScalingIndex_Cur = poseScalingOff;
    mutable int                     m_speedPedalIndex_Cur = pedalSwitchTwo;
    mutable int                     m_speedPedalIndex_Prev = pedalSwitchTwo;

    /*使能方式,使能状态判断*/
    int                             enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char& side);/*使能方式:踩脚踏*/
    bool                            isPoseRight(const HandlePose& masterHandlePose_Cur, const char& side) const;
    bool                            isForcePositionRight(const HandlePose& masterHandlePose_Cur, const char& side);

    bool                            isPoseMatch(const HandlePose& masterHandlePose_Cur, const char& side) const;
    mutable int                     m_enableTagPrev_L = 4;
    mutable int                     m_enableTagPrev_R = 4;
    mutable int                     m_enableTagCur_L ;
    mutable int                     m_enableTagCur_R ;
    mutable int                     m_endJointDragBtnCounter_L;
    mutable int                     m_endJointDragBtnCounter_R;
    void                            setEndJointDragEnableStatus(const uint8_t& domainDigitalCur_L, const uint8_t& domainDigitalCur_R);

    mutable int                     m_alignmentNumber_L;
    mutable int                     m_alignmentNumber_R;

    /*控制中所需计算部分*/
    std::atomic<HandlePose>         m_handlePose_Cur;
    mutable HandlePose              m_handlePosePrev;
    mutable HandlePose              m_handlePoseInit_L;
    mutable HandlePose              m_handlePoseInit_R;
    mutable HandlePose              m_handlePoseLastLoop_L;
    mutable HandlePose              m_handlePoseLastLoop_R;
    mutable HandlePose              m_handlePoseOrg_L;
    mutable HandlePose              m_handlePoseOrg_R;

    mutable std::array<int, MotorNumPerSide>                m_motorEncoderInit_R;
    mutable std::array<int, MotorNumPerSide>                m_motorEncoderInit_L;

    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderPrev_L;
    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderPrev_R;

    mutable std::array<double, MotorNumPerSide>                m_motorTargetEncoderPrev_L_new;
    mutable std::array<double, MotorNumPerSide>                m_motorTargetEncoderPrev_R_new;

    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderLast_L = {0};
    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderLast_R = {0};

    double m_jointAngle0_velocity_L;
    double m_jointAngle1_velocity_L;
    double m_jointAngle2_velocity_L;
    double m_jointAngle3_velocity_L;

    double m_jointAngle0_velocity_R;
    double m_jointAngle1_velocity_R;
    double m_jointAngle2_velocity_R;
    double m_jointAngle3_velocity_R;

    double m_delt_beta_L;/*LastLoopAngle*/
    double m_delt_gamma_L;
    double m_delt_alpha_L;
    double m_delt_beta_R;/*LastLoopAngle*/
    double m_delt_gamma_R;
    double m_delt_alpha_R;

    /*保存当前状态*/
    void                        storeCurAsPrev(const HandlePose& handlePoseCur,
                                               const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                               const std::array<int, MotorNumPerSide>& motorPositionCur_L, const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                               const int&  enableTagCur_L, const int&  enableTagCur_R);

    /*通过主手目标位置解算各轴转动角度*/
    std::array<double, ControlValueNum>          motionMapping_L(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_L);
    std::array<double, ControlValueNum>          motionMapping_R(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_R);

    std::array<double, ControlValueNum>          test_motionMapping_L(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_L);
    std::array<double, ControlValueNum>          test_motionMapping_R(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_R);

    std::array<double, ControlValueNum>          motionMapping_L_ForceControl(const HandlePose& handlePoseCur);
    std::array<double, ControlValueNum>          motionMapping_R_ForceControl(const HandlePose& handlePoseCur);

    mutable std::array<double, ControlValueNum>      m_controlValuePrev_L = {0};
    mutable std::array<double, ControlValueNum>      m_controlValuePrev_R = {0};

    /*计算电机目标位置、速度*/
    std::array<int, MotorNumPerSide>            calculateTargetEncoder(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                                       const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                                       const char& side)const;
    std::array<int, MotorNumPerSide>            calculateTargetVelocity(const std::array<int, MotorNumPerSide>& targetEncoderCur,
                                                                        const std::array<int, MotorNumPerSide>& targetEncoderPrev,
                                                                        const char& side)const;

    std::array<double, MotorNumPerSide>            calculateTargetEncoder_new(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                            const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                            const char& side)const;

    std::array<int, MotorNumPerSide>            calculateTargetVelocity_new(const std::array<double, MotorNumPerSide>& targetEncoderCur,
                                                             const std::array<double, MotorNumPerSide>& targetEncoderPrev,
                                                             const std::array<int, MotorNumPerSide>& targetEncoderCur_int,
                                                             const std::array<int, MotorNumPerSide>& motorEncode,
                                                             const char& side);

    /*控制循环结束后*/
    void                    storeCurAsPrev(const HandlePose& handlePoseCur, const std::array<double, ControlValueNum> controlValueCur_L,
                                            const std::array<int, MotorNumPerSide>& motorPositionCur_L,
                                            const std::array<int, MotorNumPerSide>& motorTargetEncoder_L, const int&  enableTagCur_L,
                                            const std::array<double, ControlValueNum> controlValueCur_R,
                                            const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                            const std::array<int, MotorNumPerSide>& motorTargetEncoder_R, const int&  enableTagCur_R);


    double m_last_roll;
    double m_cur_roll;


    std::array<double, 15> m_controlValues_Prev_R{};
    std::array<double, 15> m_controlValues_Prev_L{};
    std::array<double, 15> m_controlValues_MotorPrev{};

    Eigen::Matrix3d m_rotationMatrixInit = Eigen::Matrix3d::Identity();
    Eigen::Matrix3d m_rotationMatrixPrev = Eigen::Matrix3d::Identity();
    Eigen::Matrix3d m_rotationMatrixLast = Eigen::Matrix3d::Identity();


    double                          m_endArm_1 = 150;
    double                          m_endArm_2 = 180;
    double                          m_endArm_3 = 349.66;//单位mm

    /*以下为重复定位测试修改的部分*/
    int test_index;
    int test_time;

    double m_x_out;
    double m_y_out;
    double m_z_out;

    double test_x;
    double test_y;
    double test_z;
    int step_flag;
    double angle;
    double pitch;

    /*角度改变*/
    double x_cur;
    double y_cur;
    double z_cur;

    int savetime;


    /*以下为重复定位测试修改的部分*/
    Eigen::Vector3d m_compensation_LastR;
    Eigen::Vector3d m_compensation_LastL;

    Eigen::Vector3d m_compensation_CurR;
    Eigen::Vector3d m_compensation_CurL;

    mutable std::array<int, MotorNumPerSide>                m_motorPositionPrev_L = {0};
    mutable std::array<int, MotorNumPerSide>                m_motorPositionPrev_R = {0};

    mutable std::array<double, MotorNumPerSide>                m_motorPositionPrev_L_new = {0};
    mutable std::array<double, MotorNumPerSide>                m_motorPositionPrev_R_new = {0};

    mutable std::array<double, Position_DOF>                m_endEffectorInitPosition_R = {0};//{0,119.09,-187.9};//{0};
    mutable std::array<double, Position_DOF>                m_endEffectorInitPosition_L = {0};//{0,-119.09,-187.9};

    mutable std::array<double, Position_DOF>                m_endEffectorInitJointAngle_R = {0};
    mutable std::array<double, Position_DOF>                m_endEffectorInitJointAngle_L = {0};

    Eigen::Matrix3d rotationMatrixPrev = Eigen::Matrix3d::Identity();

    std::array<std::array<int,11>, 3> m_motorControl_R;
    std::array<std::array<int,11>, 3> m_motorControl_L;

    std::array<std::array<int,11>, 3> m_motorControl_save_R;
    std::array<std::array<int,11>, 3> m_motorControl_save_L;

    double cableLengths_2(double alpha) const;
    double cableLengths_3(double q_2, double q_3_pre, double openAngle) const;
    std::array<std::array<int,11>, 3> forwardKinematics_R(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur,
                                                           const std::array<int, MotorNumPerSide>& motorPosition_Init, const std::array<int, MotorNumPerSide>& motorPosition_Cur,
                                                           const HandlePose& handlePoseCur);
    std::array<std::array<int,11>, 3> forwardKinematics_L(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur,
                                                           const std::array<int, MotorNumPerSide>& motorPosition_Init, const std::array<int, MotorNumPerSide>& motorPosition_Cur,const HandlePose& handlePoseCur); //1111111111111
    int enableCase_keepPressButton(const HandlePose& masterHandlePose_Cur, const char side);

    double                  calculateOverlapValue(const std::array<int, MotorNumPerSide>& motorPosition_Cur, const HandlePose& masterHandlePose_Cur,const char& side) const;
    void                    setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side);
    void                    setForceControlInitHandleMotorPosition(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side);
    void                    judgePositonForceControl(const char& side);

    static Eigen::Matrix3d ToQuaternionRotationMatrix(double q_L0, double q_L1, double q_L2, double q_L3);
    static Eigen::Matrix3d ToEulerRotationMatrix(double Azimuth, double Elevation, double Roll);

    /*Maxon电机*/
    std::array<int,6> m_maxonInit;

    /*扭矩传感*/
    void                            openTorqueSensor();

    void                            closeTorqueSensor();

    void                            onTorqueSensorDataIn();

    float                           covertQbytearrayToFloat(QByteArray data,int startindex);

    QSerialPort                     *m_Torque_Sensor_Serial_422 = nullptr;

    QByteArray                      m_Data_Torque_Sensor_Serial_Receved;

    std::atomic<TorqueSensorData>   m_torqueSensorData_Left;

    std::atomic<TorqueSensorData>   m_torqueSensorData_Right;

    /*力拖动*/
    int                             m_dragButtonPressCur_L = 0;
    int                             m_dragButtonPressCur_R = 0;
    int                             m_dragButtonPressPre_L = 0;
    int                             m_dragButtonPressPre_R = 0;

};

#endif // ROBOTCONTROL_H
