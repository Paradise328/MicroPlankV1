#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <array>

//Data Type used in viper_transmitter.h
struct _handleOtherData
{
    uint16_t adcValueL;
    uint16_t adcValueR;
    uint16_t buttonValueL;
    uint16_t buttonValueR;
    uint32_t pedalValueIO;
    void init(){
        adcValueL = 0;
        adcValueR = 0;
        buttonValueL = 0;
        buttonValueR = 0;
        pedalValueIO = 0;
    }
};
using handleOtherData = struct _handleOtherData;


//Data used in ViperBasedHandle.h
typedef struct
{
    double sensor1_pos[7];
    double sensor2_pos[7];
}SensorData;

struct _viperData
{
    double          viperData_X;
    double          viperData_Y;
    double          viperData_Z;
    double          viperData_Arzimuth;
    double          viperData_Elevation;
    double          viperData_Roll;

};
using ViperData = struct _viperData;


typedef struct
{
    ViperData       viperDataL;
    ViperData       viperDataR;

    int             stepPedal;
    int             enableButton_L;
    int             enableButton_R;
    int             graspIndex_L;
    int             graspIndex_R;
    double          openAngleL;
    double          openAngleR;
    double          graspThreshold;
}PosDataFromViper;


struct HandlePose
{
    double          handlePoseL_X;
    double          handlePoseL_Y;
    double          handlePoseL_Z;
    double          handlePoseL_Arzimuth;
    double          handlePoseL_Elevation;
    double          handlePoseL_Roll;
    double          handlePoseL_OpenAngle;

    double          handlePoseR_X;
    double          handlePoseR_Y;
    double          handlePoseR_Z;
    double          handlePoseR_Arzimuth;
    double          handlePoseR_Elevation;
    double          handlePoseR_Roll;
    double          handlePoseR_OpenAngle;

    unsigned int    enableButton_L; //7: enable; 3: disable
    unsigned int    enableButton_R; //7: enable; 3: disable

    unsigned int    graspIndex_L;
    unsigned int    graspIndex_R;

    unsigned int    resetButton_L;
    unsigned int    resetButton_R;

    unsigned int    stepPedal;
    unsigned int    enablePedal;

    double          reserveArray1[3];
    double          reserveArray2[3];
    double          reserveArray3[3];
    
    void init()
    {
        handlePoseL_X = 0;
        handlePoseL_Y = 0;
        handlePoseL_Z = 0;
        handlePoseL_Arzimuth = 0;
        handlePoseL_Elevation = 0;
        handlePoseL_Roll = 0;
        handlePoseL_OpenAngle = 0;

        handlePoseR_X = 0;
        handlePoseR_Y = 0;
        handlePoseR_Z = 0;
        handlePoseR_Arzimuth = 0;
        handlePoseR_Elevation = 0;
        handlePoseR_Roll = 0;
        handlePoseR_OpenAngle = 0;

        enableButton_L = 7;
        enableButton_R = 7;

        graspIndex_L = 0;
        graspIndex_R = 0;

        resetButton_L = 0;
        resetButton_R = 0;
//        enableButton_L = 0; //2: enable; 3: disable
//        enableButton_R = 0; //2: enable; 3: disable
        stepPedal = 0;
    }
    void initOrg_L()
    {
        handlePoseL_X = 0;
        handlePoseL_Y = 0;
        handlePoseL_Z = 0;
        handlePoseL_Arzimuth = 0;
        handlePoseL_Elevation = 0;
        handlePoseL_Roll = 0;
        handlePoseL_OpenAngle = 0;

        handlePoseR_X = 0;
        handlePoseR_Y = 0;
        handlePoseR_Z = 0;
        handlePoseR_Arzimuth = -30;
        handlePoseR_Elevation = -15;
        handlePoseR_Roll = 0;
        handlePoseR_OpenAngle = 0;

        enableButton_L = 7;
        enableButton_R = 7;

        graspIndex_L = 0;
        graspIndex_R = 0;

        resetButton_L = 0;
        resetButton_R = 0;
//        enableButton_L = 0; //2: enable; 3: disable
//        enableButton_R = 0; //2: enable; 3: disable
        stepPedal = 0;
    }
    void initOrg_R()
    {
        handlePoseL_X = 0;
        handlePoseL_Y = 0;
        handlePoseL_Z = 0;
        handlePoseL_Arzimuth = 30;
        handlePoseL_Elevation = -15;
        handlePoseL_Roll = 0;
        handlePoseL_OpenAngle = 0;

        handlePoseR_X = 0;
        handlePoseR_Y = 0;
        handlePoseR_Z = 0;
        handlePoseR_Arzimuth = -30;
        handlePoseR_Elevation = -15;
        handlePoseR_Roll = 0;
        handlePoseR_OpenAngle = 0;

        enableButton_L = 3;
        enableButton_R = 3;

        graspIndex_L = 0;
        graspIndex_R = 0;

        resetButton_L = 0;
        resetButton_R = 0;
//        enableButton_L = 0; //2: enable; 3: disable
//        enableButton_R = 0; //2: enable; 3: disable
        stepPedal = 0;
    }
    void setMyConsoleData(const std::array<std::array<double,7>,2>& poseData_Cur , const handleOtherData& otherData_Cur)
    {
        handlePoseL_X = poseData_Cur[0][0];
        handlePoseL_Y = poseData_Cur[0][1];
        handlePoseL_Z = poseData_Cur[0][2];
        handlePoseL_Arzimuth = poseData_Cur[0][3];
        handlePoseL_Elevation = poseData_Cur[0][4];
        handlePoseL_Roll = poseData_Cur[0][5];
        // handlePoseL_OpenAngle = ;
        handlePoseR_X = poseData_Cur[1][0];
        handlePoseR_Y = poseData_Cur[1][1];
        handlePoseR_Z = poseData_Cur[1][2];
        handlePoseR_Arzimuth = poseData_Cur[1][3];
        handlePoseR_Elevation = poseData_Cur[1][4];
        handlePoseR_Roll = poseData_Cur[1][5];
        
    }
    void setMyConsoleData(const std::array<std::array<double,7>,2>& poseData_Cur)
    {
        handlePoseL_X = poseData_Cur[0][0];
        handlePoseL_Y = poseData_Cur[0][1];
        handlePoseL_Z = poseData_Cur[0][2];
        handlePoseL_Arzimuth = poseData_Cur[0][3];
        handlePoseL_Elevation = poseData_Cur[0][4];
        handlePoseL_Roll = poseData_Cur[0][5];
        // handlePoseL_OpenAngle = ;
        handlePoseR_X = poseData_Cur[1][0];
        handlePoseR_Y = poseData_Cur[1][1];
        handlePoseR_Z = poseData_Cur[1][2];
        handlePoseR_Arzimuth = poseData_Cur[1][3];
        handlePoseR_Elevation = poseData_Cur[1][4];
        handlePoseR_Roll = poseData_Cur[1][5];
    }

    std::array<std::array<double, 7>, 2> returnPNOData()
    {
        std::array<std::array<double, 7>, 2> posdeData_Tmp;
        posdeData_Tmp[0][0] = handlePoseL_X;
        posdeData_Tmp[0][1] = handlePoseL_Y;
        posdeData_Tmp[0][2] = handlePoseL_Z;
        posdeData_Tmp[0][3] = handlePoseL_Arzimuth;
        posdeData_Tmp[0][4] = handlePoseL_Elevation;
        posdeData_Tmp[0][5] = handlePoseL_Roll;
        posdeData_Tmp[0][6] = handlePoseL_OpenAngle;

        posdeData_Tmp[1][0] = handlePoseR_X;
        posdeData_Tmp[1][1] = handlePoseR_Y;
        posdeData_Tmp[1][2] = handlePoseR_Z;
        posdeData_Tmp[1][3] = handlePoseR_Arzimuth;
        posdeData_Tmp[1][4] = handlePoseR_Elevation;
        posdeData_Tmp[1][5] = handlePoseR_Roll;
        posdeData_Tmp[1][6] = handlePoseR_OpenAngle;
        return posdeData_Tmp;
    }

};

//Master Filter Case
enum class FilterCase
{
    FilterOFF = 0x00,
    IRFilterOn = 0x01,
    IIRFilterOn = 0x02,
};


constexpr int adcValueOpen_L = 2845;
constexpr int adcValueClose_L = 3030;

constexpr int adcValueOpen_R = 2865;
constexpr int adcValueClose_R = 3030;

constexpr double graspThreshold = 0;

//masterHandleSelfCheck
constexpr int masterAllOk = 1;

constexpr int pedalSwitchNumber = 4;
constexpr int pedalSwitchOne = 0;
constexpr int pedalSwitchTwo = 1;
constexpr int pedalSwitchThree = 2;
constexpr int pedalSwitchFour = 3;

constexpr int poseScalingOff = 0;
constexpr int poseScalingOn = 1;

constexpr int enableAction      = 1;
constexpr int keepEnabling      = 2;
constexpr int disableAction     = 3;
constexpr int keepDisabling     = 4;

constexpr int buttonPress       = 3;
constexpr int buttonSmallPress  = 6;
constexpr int buttonLoose       = 7;

constexpr int pedalAcceleration = 12;
constexpr int pedalDeceleration = 9;
constexpr int pedalNoAction     = 8;
constexpr int pedalMidPress     = 10;

constexpr int actualEnable      = 1;
constexpr int actualDisable     = 0;


constexpr int SinglePressButton = 0;
constexpr int KeepPressButton   = 1;
constexpr int SinglePressPedal  = 2;
constexpr int KeepPressPedal    = 3;

constexpr int pedalEnable       = 1;
constexpr int pedalDisable      = 0;

constexpr int InstrumentType    = 0;
constexpr int InstrumentSize    = 1;
constexpr int InstrumentID      = 2;

constexpr double TimePerControlLoop = 0.01; //s

constexpr double GimbalMotionThreshold = 0.01;
constexpr double InstrumentMotionThreshold = 0.01;

enum class GraspCase
{
    KeepLoose       = 0x00,
    GraspAction     = 0x01,
    LoseAction      = 0x02,
    KeepGrasping    = 0x03,
};

constexpr int joint_A = 0;
constexpr int joint_B = 1;
constexpr int joint_C = 2;
constexpr int joint_D = 3;
constexpr int joint_E = 4;
constexpr int joint_F = 5;
constexpr int joint_G = 6;


#endif // SHAREDSTRUCT_H


