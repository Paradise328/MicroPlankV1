#ifndef ROBOTKINEMATICS_H
#define ROBOTKINEMATICS_H

#include <cmath>
#include <toml++/toml.h>
#include "easylogging++.h"
#include "MathUtils.h"
#include "SharedStruct.h"

constexpr int ControlValueNum = 8;
constexpr int MotorNum = 7;

class RobotKinematics{
public:

    explicit RobotKinematics(const std::string& robotConfigPath): 
                m_configFilePath(robotConfigPath)
                {
                    readMyInitData();
                    initiAllData();
                };

    // void        positionControl(const HandlePose& masterHandlePose, const std::array<int, MotorNum>& motorPosition_Cur,  const int& controlLoopCount);
    std::array<std::array<int,MotorNum>,3> positionControl(const HandlePose& masterHandlePose, const std::array<int, MotorNum>& motorPosition_Cur,  const int& controlLoopCount);
            
            
private:
    std::string                     m_configFilePath;
    std::string                     m_robotConfigPath = "../Config/RobotData.toml";
    mutable std::string             m_endEffectorLeft   = "CZQ_4MM_1";
    mutable std::string             m_endEffectorRight  = "CZQ_4MM_1";

    double                          m_initRotAroundY_L, m_initRotAroundX_L;  //Read From Toml
    double                          m_initRotAroundY_R, m_initRotAroundX_R;  //Read From Toml
    double                          m_sourceRotAroundY;  //Read From Toml
    std::array<int,   4>            m_motionScaling;//Read From Toml
    std::array<double, 2>           m_EncoderPerDegreeScalingFactor = {1, 1.2};

    std::array<double, 4>           m_encoderPerDegree_L = {0};//Read From Toml
    std::array<double, 4>           m_encoderPerDegree_R = {0};//Read From Toml
    std::array<int, 3>              m_encoderPerMM_L = {0};//Read From Toml
    std::array<int, 3>              m_encoderPerMM_R = {0};//Read From Toml

    int                             m_kForcepSpeed     = 50000;
    int                             m_ForcepSpeedLimit = 150000;
    std::array<double, pedalSwitchNumber>    m_kGimbalPosition_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_kGimbalPosition_R = {0};//Encoder per delt Translational Motion Right
    std::array<double, pedalSwitchNumber>    m_kGimbalSpeed_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_kGimbalSpeed_R = {0};//Encoder per delt Translational Motion Right
    std::array<double, pedalSwitchNumber>    m_GimbalMaxSpeed_L = {0};//Encoder per delt Translational Motion Left
    std::array<double, pedalSwitchNumber>    m_GimbalMaxSpeed_R = {0};// Encoder per delt Translat ional Motion Right
  

    std::array<double, MotorNum>    m_speedDirection_L = {0};//Read From Toml
    std::array<double, MotorNum>    m_speedDirection_R = {0};//Read From Toml

    double                          m_compRatio_L;//Read From Toml
    double                          m_compRatio_R;//Read From Toml

    mutable int                     m_PosScalingIndex_Cur = poseScalingOff;
    mutable int                     m_SpeedPedalIndex_Cur = pedalSwitchTwo;
    mutable int                     m_SpeedPedalIndex_Prev = pedalSwitchTwo;

    mutable HandlePose              m_HandlePoseInit_L;
    mutable HandlePose              m_HandlePoseInit_R;
    mutable HandlePose              m_HandlePosePrev;
    mutable HandlePose              m_HandlePoseLastLoop_L;
    mutable HandlePose              m_HandlePoseLastLoop_R;
    mutable HandlePose              m_HandlePoseOrg_L;
    mutable HandlePose              m_HandlePoseOrg_R;
    mutable double                  m_AlignmentNumber_L;
    mutable double                  m_AlignmentNumber_R;

    mutable std::array<double, ControlValueNum>      m_ControlValuePrev_L = {0};
    mutable std::array<double, ControlValueNum>      m_ControlValuePrev_R = {0};

    mutable std::array<int, MotorNum>                m_MotorPositionPrev_L = {0};
    mutable std::array<int, MotorNum>                m_MotorPositionPrev_R = {0};
    mutable std::array<int, MotorNum>                m_MotorPositionInit_L = {0};
    mutable std::array<int, MotorNum>                m_MotorPositionInit_R = {0};

    void                    readMyInitData();
    void                    loadEndeffectorConfig();
    void                    initiAllData();
    
    std::array<std::array<int, MotorNum>,3>   calTargetPosition(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur, 
                                                    const std::array<int, MotorNum>& motorPosition_Init, const std::array<int, MotorNum>& motorPosition_Cur,
                                                    const HandlePose& masterHandlePose_Cur, const char& side)const; 
    std::array<double, ControlValueNum>  motionMappingL(const HandlePose& handlePoseInit, const HandlePose& handlePosePrev, const HandlePose& handlePoseCur) const; 
    std::array<double, ControlValueNum>  motionMappingR(const HandlePose& handlePoseInit, const HandlePose& handlePosePrev, const HandlePose& handlePoseCur) const; 
    double                               calculateOverlapValue(const std::array<int, MotorNum>& motorPosition_Cur, const HandlePose& masterHandlePose_Cur,const char& side) const;
    void setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNum>& motorPositionCur, const HandlePose& handlePoseCur, const char&side) const;

    int                     enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char side); //KeepPress Pedal
    void                    storeCurAsPrev(const HandlePose& handlePoseCur,
                                    const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                    const std::array<int, MotorNum>& motorPositionCur_L, const std::array<int, MotorNum>& motorPositionCur_R,
                                    const int&  enableTagCur_L, const int&  enableTagCur_R ) const;

    //Prev enable case(Case 1: enable action return 0;Case 2: disable action return 1; Case 3: keep enabling return 2; Case 4: keep unabling return 3
    mutable int             m_EnableTagPrev_L = 0;
    mutable int             m_EnableTagPrev_R = 0;
    mutable int             m_EnableTagCur_L = 0;
    mutable int             m_EnableTagCur_R = 0;

    bool                    isPoseRight(const HandlePose& masterHandlePose_Cur, const char side) const;
    bool                    isPoseMatch(const HandlePose& masterHandlePose_Cur, const char side) const;
};

#endif 
