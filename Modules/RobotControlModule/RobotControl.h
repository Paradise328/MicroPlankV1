#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include "../MasterModule/MasterConsole.h"
#include "../MotorDriverModule/MotorDriver.h"
#include "../SystemUtilsModule/SystemUtils.h"
#include "../MathModule/lowpass_filter.h"
#include "BlasControl/actuators_controler.h"
#include "BlasControl/BLA_API.h"
#include "BlasControl/communication.h"

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
// constexpr int JointEncoderInit_1_R = 324400;//初始值，角度为0时的编码器值
// constexpr int JointEncoderInit_2_R = 66000;
// constexpr int JointEncoderInit_3_R = 360500;
// constexpr int JointEncoderInit_1_L = 372000;
// constexpr int JointEncoderInit_2_L = 281500;
// constexpr int JointEncoderInit_3_L = 215996;

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

constexpr double JointAngleMax30_1_R = -7.332655;
constexpr double JointAngleMin30_1_R = -35.917514;
constexpr double JointAngleMax30_2_R = 116.17924;
constexpr double JointAngleMin30_2_R = 111.677606;
constexpr double JointAngleMax30_3_R = -35.656273;
constexpr double JointAngleMin30_3_R = -48.54409;

constexpr double JointAngleMax30_1_L = 35.917514;
constexpr double JointAngleMin30_1_L = 7.332655;
constexpr double JointAngleMax30_2_L = -111.677606;
constexpr double JointAngleMin30_2_L = -116.17924;
constexpr double JointAngleMax30_3_L = 48.54409;
constexpr double JointAngleMin30_3_L = 35.656273;

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


    //    std::array<double, 11>    m_ForcepSpeedLimits = {87381, 13107, 13107, 13107, 13107, 13107, 13107, 1, 87381, 87381, 87381};//单位 位每秒，电机最大速度限制60度每秒，电缸最大速度限制8mm每秒
    //    std::array<double, 11>    m_SpeedDirection_L = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //    std::array<double, 11>    m_kForcepPosition_L = {1456.356, 1638.4, 1638.4, 1638.4, 1638.4, 1638.4, 1638.4, 1, 1456.356, 1456.356, 1456.356};//电机1456.356位每度 电缸1683.4位每毫米

    explicit RobotControl(MasterConsole& masterConsole, MotorDriver* motorDriver, MessageQueue&  messagePool):
        m_masterConsole(masterConsole),
        m_motorDriver(motorDriver),
        m_messagePool(messagePool),
        m_teleOperationdMode(TeleOperationMode::CSV_Mode),
        m_ruckigPlanner_R(0.004),
        m_ruckigPlanner_L(0.004),
        m_flagControlThread(false),
        m_isSystemTerminated(false),
        m_guidingArm1stOrder({0}),
        m_guidingArm2ndOrder({0}),
        m_endeffectorConfiguration(EndeffectorConfiguration::fourMaxons),
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

    double                          m_initMoonsEncode;//光电门为0时moons编码器数值

    /*控制函数*/
    std::thread                     m_calculateControlDataThread;

    void                            startControlThread();

    void                            control();

    std::atomic<bool>               m_flagControlThread;

    /* hold = 0, teleopration = 1, collopration = 2*/
    int                             m_oprationMode = 1;
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

    EndeffectorConfiguration        m_endeffectorConfiguration = EndeffectorConfiguration::fourMaxons;

    void                            setRobotControlMode(const RobotControlMode& tartgetRobotControlMode);

    std::atomic<RobotControlMode>   m_curRobotControlMode = RobotControlMode::InitMode;

    /* 上电状态 */
    void                            initMotor();

    void                            goToHold();

    std::atomic<bool>               m_flagInHold = false;

    /* teleoperation */
    TeleOperationMode               m_teleOperationdMode;

    void                            goToTeleOperation();

    void                            teleoperation();

    void                            collaboration();

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

    /*与MotorDriver通信*/
    void                            receiveMotorData();
    void                            sendMotorData(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                                  const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L);

    void                            sendMotorData_4Maxons_ForceControl(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                               const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L);

    /*EtherCAT各从站信息*/
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorEncoderCur_R;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorEncoderCur_L;
    std::atomic<std::array<int, 3>>           m_endEffectorTarget_L;
    std::atomic<std::array<int, 3>>           m_endEffectorTarget_R;

    std::atomic<std::array<int, MotorNumPerSide>>           m_motorOperationMode_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorErrorCode_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorStatusWord_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorTrq_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorCur_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorFollowingPosErr_L;

    std::atomic<std::array<int, MotorNumPerSide>>           m_motorStatusWordCur_R;
    std::atomic<std::array<int, MotorNumPerSide>>           m_motorStatusWordCur_L;
    std::atomic<std::array<int, 6>>                         m_digitalInputGuiding;
    std::atomic<std::array<int, MotorNumPerSide>>           m_MotorTargetVel_L;
    std::atomic<std::array<int, MotorNumPerSide>>           m_MotorTargetVel_R;

    /*进入使能时计算初始位置*/
    void                           calculateEndEffectorPosition(const HandlePose& handlePoseCur, const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side);

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
    void                            MaxonGoHome_4Maxons(const char& side);

    void                            changeAngle_L();
    void                            changeAngle_R();

    mutable int                     m_armAnglePerSide = 30;
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

    double                          m_initRotAroundY_L, m_initRotAroundX_L;  //Read From Toml
    double                          m_initRotAroundY_R, m_initRotAroundX_R;  //Read From Toml
    double                          m_sourceRotAroundY = -90 * M_PI / 180;  //Read From Toml
    // double                          m_theta = 30 ;

    std::array<int,    4>           m_motionScaling;//Read From Toml
    std::array<double, 2>           m_EncoderPerDegreeScalingFactor = {1, 1.2};

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

    std::array<double, MotorNumPerSide>    m_kForcepPosition_R = {4000, 1456.356, 1456.356, 1456.356, 110.8, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 846.473};
    std::array<double, MotorNumPerSide>    m_kForcepPosition_small_R = {4000, 1456.356, 1456.356, 1456.356, 110.8 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 846.473};

    std::array<double, MotorNumPerSide>    m_kForcepPosition_L = {4000, 1456.356, 1456.356, 1456.356, 110.8, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 4063.76768, 846.473};
    std::array<double, MotorNumPerSide>    m_kForcepPosition_small_L = {4000, 1456.356, 1456.356, 1456.356, 110.8 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 4063.76768 * 2.75, 846.473};

    double                          m_compRatio_L = 0.765;//Read From Toml
    double                          m_compRatio_R = 0.765;//Read From Toml

    /*速度档位*/
    void                            setNewSpeed(const HandlePose& handlePosePrev,
                                                const HandlePose& handlePoseCur);
    mutable int                     m_posScalingIndex_Cur = poseScalingOff;
    mutable int                     m_speedPedalIndex_Cur = pedalSwitchTwo;
    mutable int                     m_speedPedalIndex_Prev = pedalSwitchTwo;

    /*使能方式,使能状态判断*/
    int                             enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char& side);/*使能方式:踩脚踏*/
    bool                            isPoseRight(const HandlePose& masterHandlePose_Cur, const char& side) const;
    bool                            isPoseMatch(const HandlePose& masterHandlePose_Cur, const char& side) const;
    mutable int                     m_enableTagPrev_L ;
    mutable int                     m_enableTagPrev_R ;
    mutable int                     m_enableTagCur_L ;
    mutable int                     m_enableTagCur_R ;

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

    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderLast_L = {0};
    mutable std::array<int, MotorNumPerSide>                m_motorTargetEncoderLast_R = {0};

    /*保存当前状态*/
    void                        storeCurAsPrev(const HandlePose& handlePoseCur,
                                               const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                               const std::array<int, MotorNumPerSide>& motorPositionCur_L, const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                               const int&  enableTagCur_L, const int&  enableTagCur_R);

    /*通过主手目标位置解算各轴转动角度*/
    std::array<double, ControlValueNum>          motionMapping_L(const HandlePose& handlePoseCur);
    std::array<double, ControlValueNum>          motionMapping_R(const HandlePose& handlePoseCur);

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

    /*控制循环结束后*/
    void                    storeCurAsPrev(const HandlePose& handlePoseCur, const std::array<double, ControlValueNum> controlValueCur_L,
                                            const std::array<int, MotorNumPerSide>& motorPositionCur_L,
                                            const std::array<int, MotorNumPerSide>& motorTargetEncoder_L, const int&  enableTagCur_L,
                                            const std::array<double, ControlValueNum> controlValueCur_R,
                                            const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                            const std::array<int, MotorNumPerSide>& motorTargetEncoder_R, const int&  enableTagCur_R);

    // int handflag = 0;

    // double m_gamma_Init_R;
    // double m_gamma_Last_R = 0;

    double m_last_roll;
    double m_cur_roll;
    int m_status_R;
    int m_status_L;

    std::array<double, 15> m_controlValues_Prev_R{};
    std::array<double, 15> m_controlValues_Prev_L{};
    std::array<double, 15> m_controlValues_MotorPrev{};

    Eigen::Matrix3d m_rotationMatrixInit = Eigen::Matrix3d::Identity();
    Eigen::Matrix3d m_rotationMatrixPrev = Eigen::Matrix3d::Identity();
    Eigen::Matrix3d m_rotationMatrixLast = Eigen::Matrix3d::Identity();


    double                          m_endArm_1 = 150;
    double                          m_endArm_2 = 180;
    double                          m_endArm_3 = 349.66;//单位mm

    /*以下为画圆测试修改部分*/
    double delta_x;
    double delta_y;
    double delta_z = 0;
    double r_test = 10;//画圆半径为100mm
    double test_angle = 0;

    /*以下为重复定位测试修改的部分*/
    int test_index;
    int test_time;
    int test_circle;
    double m_x_out;
    double m_y_out;
    double m_z_out;

    int m_waitTime = 200;//适用于运行状态

    mutable std::array<int, MotorNumPerSide>                m_motorPositionPrev_L = {0};
    mutable std::array<int, MotorNumPerSide>                m_motorPositionPrev_R = {0};

    mutable std::array<double, Position_DOF>                m_endEffectorInitPosition_R = {0};//{0,119.09,-187.9};//{0};
    mutable std::array<double, Position_DOF>                m_endEffectorInitPosition_L = {0};//{0,-119.09,-187.9};

    mutable std::array<double, Position_DOF>                m_endEffectorInitJointAngle_R = {0};
    mutable std::array<double, Position_DOF>                m_endEffectorInitJointAngle_L = {0};

    // double m_yawAngle_L = 0;
    // double m_pitchAngle_L = 0;
    // double m_yawAngle_R = 0;
    // double m_pitchAngle_R = 0;

    Eigen::Matrix3d rotationMatrixPrev = Eigen::Matrix3d::Identity();

    std::array<std::array<int,11>, 3> m_motorControl_R;
    std::array<std::array<int,11>, 3> m_motorControl_L;

    std::array<std::array<int,11>, 3> m_motorControl_save_R;
    std::array<std::array<int,11>, 3> m_motorControl_save_L;

    double cableLengths_2(double alpha) const;//11111
    double cableLengths_3(double q_2, double q_3_pre, double openAngle) const;
    std::array<std::array<int,11>, 3> forwardKinematics_R(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur,
                                                           const std::array<int, MotorNumPerSide>& motorPosition_Init, const std::array<int, MotorNumPerSide>& motorPosition_Cur,
                                                           const HandlePose& handlePoseCur);
    std::array<std::array<int,11>, 3> forwardKinematics_L(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur,
                                                           const std::array<int, MotorNumPerSide>& motorPosition_Init, const std::array<int, MotorNumPerSide>& motorPosition_Cur,const HandlePose& handlePoseCur); //1111111111111
    int enableCase_keepPressButton(const HandlePose& masterHandlePose_Cur, const char side);

    double                  calculateOverlapValue(const std::array<int, MotorNumPerSide>& motorPosition_Cur, const HandlePose& masterHandlePose_Cur,const char& side) const;
    void                    setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side);

    static Eigen::Matrix3d ToQuaternionRotationMatrix(double q_L0, double q_L1, double q_L2, double q_L3);
    static Eigen::Matrix3d ToEulerRotationMatrix(double Azimuth, double Elevation, double Roll);

    /*Maxon电机*/
    std::array<int,6> m_maxonInit;
    /*用于步进测试*/
    double test_x;
    double test_y;
    double test_z;
    double x_cur;
    double y_cur;
    double z_cur;
    double index;
    int savetime;

    /*扭矩传感*/
    void                            openTorqueSensor();

    void                            closeTorqueSensor();

    void                            onTorqueSensorDataIn();

    float                           covertQbytearrayToFloat(QByteArray data,int startindex);

    QSerialPort                     *m_Torque_Sensor_Serial_422 = nullptr;

    QByteArray                      m_Data_Torque_Sensor_Serial_Receved;

    std::atomic<TorqueSensorData>   m_torqueSensorData_Left;

    std::atomic<TorqueSensorData>   m_torqueSensorData_Right;
};

#endif // ROBOTCONTROL_H
