#include "RobotKinematics.h"

void RobotKinematics::readMyInitData()
{
    std::cout << "________________READ MY ROBOT DATA_________________" << std::endl;
    try 
    {
        toml::table RobotData = toml::parse_file(m_robotConfigPath);

        //Other Information
        {
            //Motion Scaling Coefficient
            {
                const toml::array& Arr_Tmp = *(RobotData["MasterDevice"]["MotionScaling"]["Default"].as_array());
                std::vector<int> motionScalingTmp;
                for(auto& element : Arr_Tmp) {motionScalingTmp.push_back(static_cast<int64_t>(*(element.as_integer())));}
                for(int i = 0; i < motionScalingTmp.size();i++) {m_motionScaling[i] = motionScalingTmp[i];}    
                LOG(INFO)<< "MotionScaling: " << m_motionScaling[0] <<" " << m_motionScaling[1] << " " << m_motionScaling[2] << " " << m_motionScaling[3];
            }
        }
        //Left EndEffector Information
        {
            //m_SpeedDirection_L
            {
                const toml::array& Arr_Tmp = *(RobotData["Robot"]["GimablAndEndEffector_SpeedDirection"]["speedDirection_L"]["Defalut"].as_array());
                std::vector<double> speedDirection_L;
                speedDirection_L.clear();
                for(auto& element : Arr_Tmp) {speedDirection_L.push_back(static_cast<int64_t>(*(element.as_integer())));}
                for(int i = 0; i < speedDirection_L.size();i++) {m_speedDirection_L[i] = speedDirection_L[i];}    
                LOG(INFO) << "m_SpeedDirection_L: " << m_speedDirection_L[0] << " " << m_speedDirection_L[1] << " " << m_speedDirection_L[2] << " " << m_speedDirection_L[3] << " "
                    << m_speedDirection_L[4] <<" " << m_speedDirection_L[5] << " " << m_speedDirection_L[6] <<" " << m_speedDirection_L[7] << std::endl;
            }
            //Left Endeffector Initial Rotation Around Y and X
            {
                const auto angleYTmp =*(RobotData["Robot"]["Pose"]["EndEffector"]["EndEffector_Left_InitPose"]["roty"].value<int>());
                double angleYTmp0 = 0;
                m_initRotAroundY_L = angleYTmp0 * M_PI / 180;
                LOG(INFO) <<"Left Endeffector Initial Rotation Around Y  (in radius) = " << m_initRotAroundY_L << "    In Degree: " << angleYTmp << std::endl;
                const auto angleXTmp =*(RobotData["Robot"]["Pose"]["EndEffector"]["EndEffector_Left_InitPose"]["rotx"].value<int>());
                double angleXTmp0 = 0;
                m_initRotAroundX_L = angleXTmp0 * M_PI / 180;
                LOG(INFO) <<"Left Endeffector Initial Rotation Around X (in radius) = " << m_initRotAroundX_L << "    In Degree: " << angleXTmp << std::endl;
            }

            //Encoder per Revolution;
            {
                const toml::array& Arr_Tmp = *(RobotData["Gimbal"]["Motor"]["EncoderPerRevolution"]["Compose1"].as_array());
                const toml::array& Lead_Tmp = *(RobotData["Gimbal"]["Screw"]["Lead"]["Compose1"].as_array());
                std::vector<int> encoderPerRevolution_L;
                std::vector<int> lead_L;
                encoderPerRevolution_L.clear();
                lead_L.clear();

                for(auto& element : Arr_Tmp) {encoderPerRevolution_L.push_back(static_cast<int64_t>(*(element.as_integer())));}
                LOG(INFO) << "Load EncoderPerRevolution Left: " << encoderPerRevolution_L[0] << " " << encoderPerRevolution_L[1] << " " << encoderPerRevolution_L[2];

                for(auto& element : Lead_Tmp) {lead_L.push_back(static_cast<int64_t>(*(element.as_integer())));}
                LOG(INFO) << "Load Lead Left: " << lead_L[0] << " " << lead_L[1] << " " << lead_L[2];

                for(int i = 0; i < lead_L.size(); i++){m_encoderPerMM_L[i] = encoderPerRevolution_L[i] * lead_L[i];}    
                LOG(INFO) << "Load EncoderPerMM Left: " << m_encoderPerMM_L[0] << " " << m_encoderPerMM_L[1] << " " << m_encoderPerMM_L[2];
            }

            //Encoder per delt Translational Motion Left
            {
                for(int i = 0; i < pedalSwitchNumber; i++)
                {
                    m_kGimbalSpeed_L[i]     = 10 * m_encoderPerMM_L[0] / m_motionScaling[i];  //encoder/s = encoder/cm * cm/s = (10 * encoder/mm) * cm/s;
                    m_kGimbalPosition_L[i]  = 10 * m_encoderPerMM_L[0] / m_motionScaling[i]; // encoder/cm = (10 * encoder/mm) * cm;
                    m_GimbalMaxSpeed_L[i]   = 50 * 10 * m_encoderPerMM_L[0]/ m_motionScaling[i];// encoder/s = 10 * encoder/mm * 50cm/s = 50 * 10 * encoder/mm * cm/s;
                } 
                LOG(INFO) << "Gimbal Encoder per m/s: " <<  m_kGimbalSpeed_L[0] << " " << m_kGimbalSpeed_L[1] << " " << m_kGimbalSpeed_L[2] << " " << m_kGimbalSpeed_L[3];
                LOG(INFO) << "Max Gimbal Encoder per m/s: " <<  m_GimbalMaxSpeed_L[0] << " " << m_GimbalMaxSpeed_L[1] << " " << m_GimbalMaxSpeed_L[2] << " " << m_GimbalMaxSpeed_L[3];
            }
        }

        //Right EndEffector Information
        {
            //m_SpeedDirection_R
            {
                const toml::array& Arr_Tmp = *(RobotData["Robot"]["GimablAndEndEffector_SpeedDirection"]["speedDirection_R"]["Defalut"].as_array());
                std::vector<double> speedDirection_R;
                speedDirection_R.clear();
                for(auto& element : Arr_Tmp) {speedDirection_R.push_back(static_cast<int64_t>(*(element.as_integer())));}
                for(int i = 0; i < speedDirection_R.size();i++) {m_speedDirection_R[i] = speedDirection_R[i];}    
                LOG(INFO) << "m_SpeedDirection_R: " << m_speedDirection_R[0] << " " << m_speedDirection_R[1] << " " << m_speedDirection_R[2] << " " << m_speedDirection_R[3] << " "
                    << m_speedDirection_R[4] <<" " << m_speedDirection_R[5] << " " << m_speedDirection_R[6] <<" " << m_speedDirection_R[7] << std::endl;
            }
            //Right  Endeffector Initial Rotation Around Y and X
            {
                const auto angleYTmp =*(RobotData["Robot"]["Pose"]["EndEffector"]["EndEffector_Right_InitPose"]["roty"].value<int>());
                double angleYTmp0 = 0;
                m_initRotAroundY_R = angleYTmp0 * M_PI / 180;
                LOG(INFO) <<"Right Endeffector Initial Rotation Around Y  (in radius) = " << m_initRotAroundY_R << "    In Degree: " << angleYTmp << std::endl;
                const auto angleXTmp =*(RobotData["Robot"]["Pose"]["EndEffector"]["EndEffector_Right_InitPose"]["rotx"].value<int>());
                double angleXTmp0 = 0;
                m_initRotAroundX_R = angleXTmp0 * M_PI / 180;
                LOG(INFO) << "Right Endeffector Initial Rotation Around X (in radius) = " << m_initRotAroundX_R << "    In Degree: " << angleXTmp << std::endl;
            }
            //Encoder per Revolution;
            {
                const toml::array& Arr_Tmp = *(RobotData["Gimbal"]["Motor"]["EncoderPerRevolution"]["Compose1"].as_array());
                const toml::array& Lead_Tmp = *(RobotData["Gimbal"]["Screw"]["Lead"]["Compose1"].as_array());
                std::vector<int> encoderPerRevolution_R;
                std::vector<int> lead_R;
                encoderPerRevolution_R.clear();
                lead_R.clear();

                for(auto& element : Arr_Tmp) {encoderPerRevolution_R.push_back(static_cast<int64_t>(*(element.as_integer())));}
                LOG(INFO) << "Load EncoderPerRevolution Right: " << encoderPerRevolution_R[0] << " " << encoderPerRevolution_R[1] << " " << encoderPerRevolution_R[2];

                for(auto& element : Lead_Tmp) {lead_R.push_back(static_cast<int64_t>(*(element.as_integer())));}
                LOG(INFO) << "Load Lead Right: " << lead_R[0] << " " << lead_R[1] << " " << lead_R[2];

                for(int i = 0; i < lead_R.size(); i++){m_encoderPerMM_R[i] = encoderPerRevolution_R[i] * lead_R[i];}    
                LOG(INFO) << "Load EncoderPerMM Right: " << m_encoderPerMM_R[0] << " " << m_encoderPerMM_R[1] << " " << m_encoderPerMM_R[2];
            }
        }

        //Master Console Information
        {
            //Source Initial Rotation
            {
                const auto angleYTmp =*(RobotData["MasterDevice"]["MasterDeviceInitPose"]["roty"].value<int>());
                double angleYTmp0 = -90;
                m_sourceRotAroundY = angleYTmp0 * M_PI / 180;
                LOG(INFO)<< "Master Sensor Source Rotation Around Y (in radius) = "<< m_sourceRotAroundY << "    In Degree: " << angleYTmp << std::endl ;
            }
        }
    }
    catch(const toml::parse_error& err)
    {
        LOG(ERROR) << "Failed to parse toml file: " << err.what();
    }
    std::cout <<"1111111" << std::endl;
    loadEndeffectorConfig();
}

void RobotKinematics::loadEndeffectorConfig()
{
    //Read Instrument Information: Type, Size, ID
    std::vector<std::string> t_endEffectorInfoSplit_L, t_endEffectorInfoSplit_R;
    t_endEffectorInfoSplit_L = split(m_endEffectorLeft,"_");
    t_endEffectorInfoSplit_R = split(m_endEffectorRight,"_");

    std::cout << "________________READ MY INSTRUMENT DATA_________________" << std::endl;
    LOG(INFO) << "Left Instrument Information: "<< m_endEffectorLeft;
    LOG(INFO) << "Right Instrument Information: "<< m_endEffectorRight;
    try
    {
        //Load Left Endeffector Information
        toml::table endEffectorData = toml::parse_file(m_configFilePath);
        {
            //Left Side
            {
                //Instrument Data;
                if(t_endEffectorInfoSplit_L[InstrumentType] != "None")
                {
                    {
                        //Encoder per Degree;
                        const toml::array& Arr_Tmp = *(endEffectorData["Instrument"]
                                [t_endEffectorInfoSplit_L[InstrumentType]]
                                [t_endEffectorInfoSplit_L[InstrumentSize]]
                                [t_endEffectorInfoSplit_L[InstrumentID]]
                                ["EncoderPerDegree"]["Value"].as_array());
                        std::vector<double> encoderPerDegreeL;
                        encoderPerDegreeL.clear();
                        for(auto& element : Arr_Tmp) {encoderPerDegreeL.push_back(static_cast<double>(*(element.as_floating_point())));}
                        for(int i = 0; i < encoderPerDegreeL.size();i++){m_encoderPerDegree_L[i] = encoderPerDegreeL[i];}    
                        LOG(INFO) << "Load EncoderPerDegree Left: " << m_encoderPerDegree_L[0] << " " << m_encoderPerDegree_L[1] << " " << m_encoderPerDegree_L[2] << " " << m_encoderPerDegree_L[3];

                        //Cable Compensation Ratio;
                        m_compRatio_L = 0;
                        m_compRatio_L = *(endEffectorData["Instrument"]
                                [t_endEffectorInfoSplit_L[InstrumentType]]
                                [t_endEffectorInfoSplit_L[InstrumentSize]]
                                [t_endEffectorInfoSplit_L[InstrumentID]]
                                ["CompensationRatio"]["Value"].value<double>());
                        LOG(INFO) << "Load CompensationRatio Left = " << m_compRatio_L;
                    }
                }
            }

            //Right Side
            {
                if(t_endEffectorInfoSplit_R[InstrumentType] != "None")
                {
                    {
                        //Encoder per Degree;
                        const toml::array& Arr_Tmp = *(endEffectorData["Instrument"]
                                [t_endEffectorInfoSplit_R[InstrumentType]]
                                [t_endEffectorInfoSplit_R[InstrumentSize]]
                                [t_endEffectorInfoSplit_R[InstrumentID]]
                                ["EncoderPerDegree"]["Value"].as_array());
                        std::vector<double> encoderPerDegreeR;
                        encoderPerDegreeR.clear();
                        for(auto& element : Arr_Tmp) {encoderPerDegreeR.push_back(static_cast<double>(*(element.as_floating_point())));}
                        for(int i = 0; i < encoderPerDegreeR.size();i++){m_encoderPerDegree_R[i] = encoderPerDegreeR[i];}    
                        LOG(INFO) << "Load EncoderPerDegree Right: " << m_encoderPerDegree_R[0] << " " << m_encoderPerDegree_R[1] << " " << m_encoderPerDegree_R[2] << " " << m_encoderPerDegree_R[3];

                        //Cable Compensation Ratio;
                        m_compRatio_R = 0;
                        m_compRatio_R = *(endEffectorData["Instrument"]
                                [t_endEffectorInfoSplit_R[InstrumentType]]
                                [t_endEffectorInfoSplit_R[InstrumentSize]]
                                [t_endEffectorInfoSplit_R[InstrumentID]]
                                ["CompensationRatio"]["Value"].value<double>());
                        LOG(INFO) << "Load CompensationRatio Right = " << m_compRatio_R;
                    }
                }
                LOG(INFO)<< "Successfully load the right Instrument Infomation: ";
            }
        }
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "Parse error: " << err.what() << std::endl;
        LOG(ERROR)<< "Parsing failed:\n" << err;
    }
};

void RobotKinematics::initiAllData()
{
    m_HandlePoseInit_L.init();
    m_HandlePoseInit_R.init();
    m_HandlePoseOrg_L.initOrg_L();
    m_HandlePoseOrg_R.initOrg_R();

    m_HandlePosePrev.init();
    m_HandlePoseLastLoop_L.initOrg_L();
    m_HandlePoseLastLoop_R.initOrg_R();

    m_ControlValuePrev_L = {0};
    m_ControlValuePrev_R = {0};

    m_PosScalingIndex_Cur = 0;

}


std::array<std::array<int,MotorNum>,3>   RobotKinematics::positionControl(const HandlePose& masterHandlePose, const std::array<int, MotorNum>& motorPosition_Cur, const int& controlLoopCount)
{
    HandlePose      masterHandlePose_Init_L;
    HandlePose      masterHandlePose_Init_R;
    HandlePose      masterHandlePose_Prev;
    HandlePose      masterHandlePose_Cur;

    std::array<double, ControlValueNum>   controlValuePrev_L = {0};
    std::array<double, ControlValueNum>   controlValueCur_L = {0};
    std::array<double, ControlValueNum>   controlValuePrev_R = {0};
    std::array<double, ControlValueNum>   controlValueCur_R = {0};
    std::array<int, MotorNum>   motorPositionInit_L = {0};
    std::array<int, MotorNum>   motorPositionCur_L = {0};
    std::array<int, MotorNum>   motorPositionInit_R = {0};
    std::array<int, MotorNum>   motorPositionCur_R = {0};
    std::array<std::array<int,MotorNum>,3>    targetSpeedPosition={{0}};

    masterHandlePose_Init_L = m_HandlePoseInit_L;
    masterHandlePose_Prev = m_HandlePosePrev;
    masterHandlePose_Cur = masterHandlePose;
    motorPositionInit_L = m_MotorPositionInit_L;
    motorPositionCur_L = motorPosition_Cur;
    int enableTagCur_L = keepDisabling;
    int enableTagCur_R = keepDisabling;

    controlValuePrev_L = m_ControlValuePrev_L;

    // enableTagCur_L = enableCase_KeepPressPedal(masterHandlePose_Cur, 'l');
    // enableTagCur_R = enableCase_KeepPressPedal(masterHandlePose_Cur, 'r'); 

    
    if(controlLoopCount == 1000)
    {
        enableTagCur_L = enableAction;
    }
    else if(controlLoopCount > 1000)
    {
        enableTagCur_L = keepEnabling;
    }
    else
    {
        enableTagCur_L = disableAction;
    }

    //Control loop
    {
        if(enableTagCur_L == enableAction||enableTagCur_L == keepEnabling)
        {
            if(enableTagCur_L == enableAction)
            {
                masterHandlePose_Init_L = masterHandlePose_Cur;
                motorPositionInit_L = motorPositionCur_L;
                setControlInitHandleMotorPositionAndPose(motorPositionCur_L, masterHandlePose_Cur, 'l');
            }
            controlValueCur_L = motionMappingL(masterHandlePose_Init_L, masterHandlePose_Prev, masterHandlePose_Cur);
            LOG(INFO) << "controlValueCur_L: " << controlValueCur_L;
            targetSpeedPosition = calTargetPosition(controlValuePrev_L, controlValueCur_L, motorPositionInit_L, motorPositionCur_L,masterHandlePose_Cur, 'l');
        }
    }

    storeCurAsPrev(masterHandlePose_Cur, controlValueCur_L, controlValueCur_R, motorPositionCur_L, motorPositionCur_R, enableTagCur_L, enableTagCur_R);
    return targetSpeedPosition;
}

int RobotKinematics::enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char side)
{
    int enableFlag = 4;

    if(side == 'l')
    {
        if(!isPoseRight(masterHandlePose_Cur, 'l'))
        {
            if(m_EnableTagPrev_L == disableAction || m_EnableTagPrev_L == keepDisabling)
            {
                enableFlag = keepDisabling;
            }
            if(m_EnableTagPrev_L == enableAction || m_EnableTagPrev_L == keepEnabling)
            {
                m_HandlePoseLastLoop_L = m_HandlePosePrev;
                m_AlignmentNumber_L = 0;
                enableFlag = disableAction;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'l'))
        {
            if((m_EnableTagPrev_L == disableAction || m_EnableTagPrev_L == keepDisabling) &&
                masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == true)
            {
               m_HandlePoseInit_L = masterHandlePose_Cur;
               m_AlignmentNumber_L = 0;
               enableFlag = enableAction;
            }
            else if ((m_EnableTagPrev_L == disableAction || m_EnableTagPrev_L == keepDisabling) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
               enableFlag = keepDisabling;
            }
            else if ((m_EnableTagPrev_L == enableAction || m_EnableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                m_HandlePoseLastLoop_L = m_HandlePosePrev;
                enableFlag = disableAction;
            }
            else if ((m_EnableTagPrev_L == enableAction || m_EnableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
    }

    if(side == 'r' )
    {
        if(!isPoseRight(masterHandlePose_Cur, 'r'))
        {
            if(m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling)
            {
                enableFlag = keepDisabling;
            }
            if(m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling)
            {
                m_HandlePoseLastLoop_R = m_HandlePosePrev;
                enableFlag = disableAction;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'r'))
        {
            if((m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling) &&
                masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == true)
            {
               m_HandlePoseInit_R = masterHandlePose_Cur;
               m_AlignmentNumber_R = 0;
               enableFlag = enableAction;
            }
            else if ((m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
               enableFlag = keepDisabling;
            }
            else if ((m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                m_HandlePoseLastLoop_R = m_HandlePosePrev;
                enableFlag = disableAction;
            }
            else if ((m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
    }
    return enableFlag;
}


// Check if the master device is in appropriate work space;
bool RobotKinematics::isPoseRight(const HandlePose& masterHandlePose_Cur, const char side) const
{
    if(side =='l')
    {
        if(masterHandlePose_Cur.handlePoseL_Roll && masterHandlePose_Cur.handlePoseL_Roll > -120
                && masterHandlePose_Cur.handlePoseL_Elevation < 70 && masterHandlePose_Cur.handlePoseL_Elevation > -70
                && masterHandlePose_Cur.handlePoseL_Z < 0)
        {
            return true;
        }
        else{
            return false;}
    }

    if(side =='r')
    {
        if(masterHandlePose_Cur.handlePoseR_Roll && masterHandlePose_Cur.handlePoseR_Roll > -120
                && masterHandlePose_Cur.handlePoseR_Elevation < 70 && masterHandlePose_Cur.handlePoseR_Elevation > -70
                && masterHandlePose_Cur.handlePoseR_Z < 0)
        {
            return true;
        }
        else{
            return false;}
    }
}

bool RobotKinematics::isPoseMatch(const HandlePose& masterHandlePose_Cur, const char side) const
{
    if (side == 'l')
    {
        if(abs(masterHandlePose_Cur.handlePoseL_OpenAngle - m_HandlePoseLastLoop_L.handlePoseL_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}

    }
    if (side == 'r')
    {
        if(abs(masterHandlePose_Cur.handlePoseR_OpenAngle - m_HandlePoseLastLoop_R.handlePoseR_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}
    }
}

void RobotKinematics::setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNum>& motorPositionCur, const HandlePose& handlePoseCur, const char&side) const
{
    if(side == 'l')
    {
        m_HandlePoseInit_L = handlePoseCur;
        m_MotorPositionInit_L = motorPositionCur;
    }
    if(side == 'r')
    {
        m_HandlePoseInit_R = handlePoseCur;
        m_MotorPositionInit_R = motorPositionCur;
    }
}

std::array<double, ControlValueNum>  RobotKinematics::motionMappingL(const HandlePose& handlePoseInit, const HandlePose& handlePosePrev, const HandlePose& handlePoseCur) const
{
    std::array<double, ControlValueNum> controlValueTmp = {0};

    if(m_AlignmentNumber_L<100){m_AlignmentNumber_L ++;}

    // rotation matrix:
    // x              y                   z
    //CA*CE     CA*SE*SR-SA*CR      CA*SE*CR+SA*SR
    //SA*CE     CA*CR+SA*SE*SR      SA*SE*CR-CA*SR
    //-SE           CE*SR               CE*CR

    //Initial, Previous, Current Pose from Viper Tracking System
    double arzimuth_Org_L   = m_HandlePoseOrg_L.handlePoseL_Arzimuth * M_PI / 180;
    double arzimuth_Init_L  = handlePoseInit.handlePoseL_Arzimuth * M_PI / 180;
    double arzimuth_Prev_L  = handlePosePrev.handlePoseL_Arzimuth * M_PI / 180;
    double arzimuth_Last_L  = m_HandlePoseLastLoop_L.handlePoseL_Arzimuth * M_PI / 180;
    double arzimuth_Cur_L   = handlePoseCur.handlePoseL_Arzimuth *  M_PI / 180;

    double elevation_Org_L = m_HandlePoseOrg_L.handlePoseL_Elevation * M_PI / 180;
    double elevation_Init_L = handlePoseInit.handlePoseL_Elevation * M_PI / 180;
    double elevation_Prev_L = handlePosePrev.handlePoseL_Elevation * M_PI / 180;
    double elevation_Last_L = m_HandlePoseLastLoop_L.handlePoseL_Elevation * M_PI / 180;
    double elevation_Cur_L  = handlePoseCur.handlePoseL_Elevation *  M_PI / 180;

    double roll_Org_L       = m_HandlePoseOrg_L.handlePoseL_Roll * M_PI / 180;
    double roll_Init_L      = handlePoseInit.handlePoseL_Roll * M_PI / 180;
    double roll_Prev_L      = handlePosePrev.handlePoseL_Roll * M_PI / 180;
    double roll_Last_L      = m_HandlePoseLastLoop_L.handlePoseL_Roll * M_PI / 180;
    double roll_Cur_L       = handlePoseCur.handlePoseL_Roll *  M_PI / 180;

    //Define the Euler rotation Matrix
    Eigen::Matrix3d     yawMatrix_Org_L,    yawMatrix_Init_L,       yawMatrix_Prev_L,       yawMatrix_Cur_L,        yawMatrix_Last_L;
    Eigen::Matrix3d     pitchMatrix_Org_L,  pitchMatrix_Init_L,     pitchMatrix_Prev_L,     pitchMatrix_Cur_L,      pitchMatrix_Last_L;
    Eigen::Matrix3d     rollMatrix_Org_L,   rollMatrix_Init_L,      rollMatrix_Prev_L,      rollMatrix_Cur_L,       rollMatrix_Last_L;
    Eigen::Matrix3d     rotAroundX_L,       rotAroundY_L,           rotAroundZ_L;
    Eigen::Matrix3d     rotMatrix_Init_L,   rotMatrix_Org_L,        rotMatrix_Prev_L,       rotMatrix_Cur_L,        rotMatrix_Last_L;

    //Master Handle yawMatrix
    yawMatrix_Org_L     = Eigen::AngleAxisd(arzimuth_Org_L, Eigen::Vector3d::UnitZ());
    yawMatrix_Init_L    = Eigen::AngleAxisd(arzimuth_Init_L, Eigen::Vector3d::UnitZ());
    yawMatrix_Prev_L    = Eigen::AngleAxisd(arzimuth_Prev_L, Eigen::Vector3d::UnitZ());
    yawMatrix_Last_L    = Eigen::AngleAxisd(arzimuth_Last_L, Eigen::Vector3d::UnitZ());
    yawMatrix_Cur_L     = Eigen::AngleAxisd(arzimuth_Cur_L, Eigen::Vector3d::UnitZ());

    //Master Handle pitchMatrix
    pitchMatrix_Org_L   = Eigen::AngleAxisd(elevation_Org_L, Eigen::Vector3d::UnitY());
    pitchMatrix_Init_L  = Eigen::AngleAxisd(elevation_Init_L, Eigen::Vector3d::UnitY());
    pitchMatrix_Prev_L  = Eigen::AngleAxisd(elevation_Prev_L, Eigen::Vector3d::UnitY());
    pitchMatrix_Last_L  = Eigen::AngleAxisd(elevation_Last_L, Eigen::Vector3d::UnitY());
    pitchMatrix_Cur_L   = Eigen::AngleAxisd(elevation_Cur_L, Eigen::Vector3d::UnitY());

    //Master Handle rollMatrix
    rollMatrix_Org_L    = Eigen::AngleAxisd(roll_Org_L, Eigen::Vector3d::UnitX());
    rollMatrix_Init_L   = Eigen::AngleAxisd(roll_Init_L, Eigen::Vector3d::UnitX());
    rollMatrix_Prev_L   = Eigen::AngleAxisd(roll_Prev_L, Eigen::Vector3d::UnitX());
    rollMatrix_Last_L   = Eigen::AngleAxisd(roll_Last_L, Eigen::Vector3d::UnitX());
    rollMatrix_Cur_L    = Eigen::AngleAxisd(roll_Cur_L, Eigen::Vector3d::UnitX());

    //Initial two rotations
    rotAroundX_L = Eigen::AngleAxisd(m_initRotAroundX_L, Eigen::Vector3d::UnitX());
    rotAroundY_L = Eigen::AngleAxisd((m_sourceRotAroundY - m_initRotAroundY_L), Eigen::Vector3d::UnitY());

    //Define the rotation mapping matirx
    Eigen::Matrix3d mappingMatrix;
    mappingMatrix << 0,  0, 1,
                     0,  1, 0,
                    -1,  0, 0;

    //Calculate the RotationMatrixv
    rotMatrix_Org_L     = rotAroundX_L * rotAroundY_L * yawMatrix_Org_L * pitchMatrix_Org_L * rollMatrix_Org_L * mappingMatrix;
    rotMatrix_Init_L    = rotAroundX_L * rotAroundY_L * yawMatrix_Init_L * pitchMatrix_Init_L * rollMatrix_Init_L * mappingMatrix;
    rotMatrix_Prev_L    = rotAroundX_L * rotAroundY_L * yawMatrix_Prev_L * pitchMatrix_Prev_L * rollMatrix_Prev_L * mappingMatrix;
    rotMatrix_Last_L    = rotAroundX_L * rotAroundY_L * yawMatrix_Last_L  * pitchMatrix_Last_L  * rollMatrix_Last_L  * mappingMatrix;
    rotMatrix_Cur_L     = rotAroundX_L * rotAroundY_L * yawMatrix_Cur_L  * pitchMatrix_Cur_L  * rollMatrix_Cur_L  * mappingMatrix;

    //delt: all the delt with different weights;    delt_Cur: Pos_Cur - Pos_Init;    delt_Init:  Pos_Init - Pos_LastLoop;     delt_Org:   Pos_LastLoop - Pos_Org;
    double delt_alpha_L = 0; double delt_alphaCur_L = 0; double delt_alphaInit_L = 0; double delt_alphaOrg_L = 0;
    double delt_beta_L  = 0; double delt_betaCur_L = 0;  double delt_betaInit_L = 0;  double delt_betaOrg_L = 0;
    double delt_gamma_L = 0; double delt_gammaCur_L = 0; double delt_gammaInit_L = 0; double delt_gammaOrg_L = 0;
    double alpha_Init_L = 0; double alpha_Prev_L = 0; double alpha_Cur_L = 0;   double alpha_Last_L = 0;  double alpha_Org_L = 0;
    double beta_Init_L  = 0;  double beta_Prev_L = 0;  double beta_Cur_L = 0;    double beta_Last_L = 0;   double beta_Org_L = 0;
    double gamma_Init_L = 0; double gamma_Prev_L = 0; double gamma_Cur_L = 0;   double gamma_Last_L = 0;  double gamma_Org_L = 0;

    //YAW of Slave Forceps
    alpha_Org_L     = atan2(rotMatrix_Org_L(1,0), rotMatrix_Org_L(0,0));
    alpha_Init_L    = atan2(rotMatrix_Init_L(1,0), rotMatrix_Init_L(0,0));
    alpha_Cur_L     = atan2(rotMatrix_Cur_L(1,0),  rotMatrix_Cur_L(0,0));
    alpha_Last_L    = atan2(rotMatrix_Last_L(1,0),  rotMatrix_Last_L(0,0));

    delt_alphaCur_L     = (alpha_Cur_L - alpha_Init_L) * 180 / M_PI;
    delt_alphaInit_L    = (alpha_Init_L - alpha_Last_L) * 180 / M_PI;
    delt_alphaOrg_L     = (alpha_Last_L - alpha_Org_L) * 180 / M_PI;
    delt_alpha_L        = delt_alphaCur_L + delt_alphaInit_L * m_AlignmentNumber_L / 100 + delt_alphaOrg_L;

    //PITCH of Slave Forceps
    beta_Org_L      = atan2(-rotMatrix_Org_L(2,0),sqrt(rotMatrix_Org_L(2,1) * rotMatrix_Org_L(2,1) + rotMatrix_Org_L(2,2) * rotMatrix_Org_L(2,2)));
    beta_Init_L     = atan2(-rotMatrix_Init_L(2,0),sqrt(rotMatrix_Init_L(2,1) * rotMatrix_Init_L(2,1) + rotMatrix_Init_L(2,2) * rotMatrix_Init_L(2,2)));
    beta_Cur_L      = atan2(-rotMatrix_Cur_L(2,0),sqrt(rotMatrix_Cur_L(2,1) * rotMatrix_Cur_L(2,1) + rotMatrix_Cur_L(2,2) * rotMatrix_Cur_L(2,2)));
    beta_Last_L     = atan2(-rotMatrix_Last_L(2,0),sqrt(rotMatrix_Last_L(2,1) * rotMatrix_Last_L(2,1) + rotMatrix_Last_L(2,2) * rotMatrix_Last_L(2,2)));

    delt_betaCur_L  = (beta_Cur_L - beta_Init_L) * 180 / M_PI;
    delt_betaInit_L = (beta_Init_L - beta_Last_L) * 180 / M_PI;
    delt_betaOrg_L  = (beta_Last_L - beta_Org_L) * 180 / M_PI;
    delt_beta_L     = delt_betaCur_L + delt_betaInit_L * m_AlignmentNumber_L / 100 + delt_betaOrg_L;

    //ROLL of Slave Forceps
    gamma_Org_L     = atan2(rotMatrix_Org_L(2,1), rotMatrix_Org_L(2,2));
    gamma_Init_L    = atan2(rotMatrix_Init_L(2,1), rotMatrix_Init_L(2,2));
    gamma_Cur_L     = atan2(rotMatrix_Cur_L(2,1), rotMatrix_Cur_L(2,2));
    gamma_Last_L    = atan2(rotMatrix_Last_L(2,1), rotMatrix_Last_L(2,2));

    delt_gammaCur_L     = (gamma_Cur_L - gamma_Init_L) * 180 / M_PI;
    delt_gammaInit_L    = (gamma_Init_L - gamma_Last_L) * 180 / M_PI;
    delt_gammaOrg_L     = (gamma_Last_L - gamma_Org_L) * 180 / M_PI;
    delt_gamma_L        = delt_gammaCur_L + delt_gammaInit_L * m_AlignmentNumber_L / 100 + delt_gammaOrg_L;

    //Calculate the translational motion
    Eigen::Vector3d  sensorPosition_Init_L, sensorPosition_Cur_L;
    sensorPosition_Cur_L << handlePoseCur.handlePoseL_X,
                            handlePoseCur.handlePoseL_Y,
                            handlePoseCur.handlePoseL_Z;
    sensorPosition_Init_L << handlePoseInit.handlePoseL_X,
                             handlePoseInit.handlePoseL_Y,
                             handlePoseInit.handlePoseL_Z;
    Eigen::Matrix3d rotAroundWorldY;
    rotAroundWorldY = Eigen::AngleAxisd(m_sourceRotAroundY + M_PI/2, Eigen::Vector3d::UnitY());

    Eigen::Vector3d  masterPositionViaSensor;
    masterPositionViaSensor<<-2.0,
                                0,
                             -7.5;

    Eigen::Vector3d  endPosition_Init_L, endPosition_Cur_L;
    
    //Sensor Position As the Calculation Position
    endPosition_Init_L = rotAroundWorldY * sensorPosition_Init_L;
    endPosition_Cur_L  = rotAroundWorldY * sensorPosition_Cur_L;

    //Handle Position As the Calculation Position
    // endPosition_Init_L = rotAroundWorldY * (sensorPosition_Init_L + yawMatrix_Init_L * pitchMatrix_Init_L * rollMatrix_Init_L * masterPositionViaSensor);
    // endPosition_Cur_L = rotAroundWorldY * (sensorPosition_Cur_L + yawMatrix_Cur_L * pitchMatrix_Cur_L * rollMatrix_Cur_L * masterPositionViaSensor) ;

    double deltM_X_L  = endPosition_Cur_L[0] - endPosition_Init_L[0];
    double deltM_Y_L  = endPosition_Cur_L[1] - endPosition_Init_L[1];
    double deltM_Z_L  = endPosition_Cur_L[2] - endPosition_Init_L[2];


    double openAngle_L = handlePoseCur.handlePoseL_OpenAngle;
    openAngle_L  = 0;

    //roll
    controlValueTmp[0] = delt_alpha_L * m_EncoderPerDegreeScalingFactor[m_PosScalingIndex_Cur];

    //Pitch
    controlValueTmp[1] = delt_beta_L * m_EncoderPerDegreeScalingFactor[m_PosScalingIndex_Cur];

    //Yaw
    controlValueTmp[2] = (delt_gamma_L - delt_beta_L * m_compRatio_L) * m_EncoderPerDegreeScalingFactor[m_PosScalingIndex_Cur] + openAngle_L;

    //Yaw
    controlValueTmp[3] = (delt_gamma_L - delt_beta_L * m_compRatio_L) * m_EncoderPerDegreeScalingFactor[m_PosScalingIndex_Cur] - openAngle_L;

    // controlValueTmp[0] = handlePoseCur.handlePoseR_Roll - handlePoseInit.handlePoseR_Roll;
    // controlValueTmp[1] = handlePoseCur.handlePoseR_Elevation - handlePoseInit.handlePoseR_Elevation;
    // controlValueTmp[2] = handlePoseCur.handlePoseR_Arzimuth - handlePoseInit.handlePoseR_Arzimuth;
    // controlValueTmp[3] = handlePoseCur.handlePoseR_Arzimuth - handlePoseInit.handlePoseR_Arzimuth;

    //X
    controlValueTmp[4] = deltM_X_L;

    //Y
    controlValueTmp[5] = deltM_Y_L;

    //Z
    controlValueTmp[6] = deltM_Z_L;

    //Grasp Case: Case 1: grasp action; Case 2: lose action; Case 3: keep grasping; Case 0: keep lose
    if(handlePosePrev.graspIndex_L == 0 && handlePoseCur.graspIndex_L == 1)
    {
        controlValueTmp[7] = 1;
        if(m_HandlePoseLastLoop_L.graspIndex_L == 1)
        {
            controlValueTmp[7] = 2;
        }
    }
    else if(handlePosePrev.graspIndex_L == 1 && handlePoseCur.graspIndex_L == 0)
        {controlValueTmp[7] = 2;}
    else if(handlePosePrev.graspIndex_L == 1 && handlePoseCur.graspIndex_L == 1)
        {controlValueTmp[7] = 3;}
    else {controlValueTmp[7] = 0;}


    return controlValueTmp;
}

std::array<std::array<int, MotorNum>,3>   RobotKinematics::calTargetPosition(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur, 
                                                const std::array<int, MotorNum>& motorPosition_Init, const std::array<int, MotorNum>& motorPosition_Cur,
                                                const HandlePose& masterHandlePose_Cur, const char& side) const
{
    //[0][0-7]:Target Speed ABCDEFG;
    //[1][0-7]:Target Position ABCDEFG;
    //[2][0]:Case 1: grasp action; Case 2: lose action; Case 3: keep grasping; Case 0: keep lose;
    std::array<std::array<int,MotorNum>,3>  targetValueTmp;
    int grasp_dir[2]={1,1};

    if(side == 'l')
    {
        int grasp_index = 0 , grasp_action=0;
        if(controlValue_Cur[7] == 1 || controlValue_Cur[7] == 3) {grasp_index = 1;}
        if(controlValue_Cur[7] == 1){grasp_action = 1;}
        double graspEncoderVal = calculateOverlapValue(motorPosition_Cur,masterHandlePose_Cur,'l');
        // for(int i = 0;i < 4; i++)
        // {
        //     if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > 0.01)
        //     {
        //         targetPositionTmp[i] = m_speedDirection_L[i] * controlValue_Cur[i] * m_encoderPerDegree_L[i];
        //     }
        //     else
        //     {
        //         targetPositionTmp[i] = motorPosition_Cur[i];
        //     }
        // }
        for(int i =0; i < 2; i++)
        {
            if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > InstrumentMotionThreshold)
            {
                targetValueTmp[0][i] = static_cast<int>((controlValue_Cur[i] - controlValue_Prev[i]) *  m_kForcepSpeed);
                if(abs(targetValueTmp[0][i]) > m_ForcepSpeedLimit)
                {
                    targetValueTmp[0][i] = m_speedDirection_L[i] * targetValueTmp[0][i] / abs(targetValueTmp[0][i]) * m_ForcepSpeedLimit;
                }
                targetValueTmp[1][i] = m_speedDirection_L[i] * controlValue_Cur[i] * m_encoderPerDegree_L[i];
            }
            else{
                targetValueTmp[0][i] = 0;
                targetValueTmp[1][i] = motorPosition_Cur[i];
            }
        }
    // C-Axis and D-Axis
        for(int i = 2; i < 4;i++)
        {
            if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > 0.01)
            {
                targetValueTmp[0][i] = int(m_speedDirection_L[i] * (controlValue_Cur[i] - controlValue_Prev[i]) * m_kForcepSpeed);
                if(abs(targetValueTmp[0][i]) > abs(m_ForcepSpeedLimit * m_speedDirection_L[i]))
                {
                    targetValueTmp[0][i] = m_speedDirection_L[i] * targetValueTmp[0][i] / abs(targetValueTmp[0][i]) * m_ForcepSpeedLimit;
                }
                targetValueTmp[1][i] = m_speedDirection_L[i] * controlValue_Cur[i] * m_encoderPerDegree_L[i] + grasp_index * graspEncoderVal * grasp_dir[i-2];
            } 
            else
            {
                targetValueTmp[1][i] = m_speedDirection_L[i] * controlValue_Cur[i] * m_encoderPerDegree_L[i] + grasp_index * graspEncoderVal * grasp_dir[i-2];
                targetValueTmp[0][i] =(targetValueTmp[1][i] - motorPosition_Cur[i]) * 100;// +1000000 * grasp_dir[i-2] * grasp_index +
            }
        }

        // E-Axis, F-Axis and G_Axis
        for(int i = 4;i < 7; i++)
        {
            if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > GimbalMotionThreshold)//unit:cm
            {
                targetValueTmp[0][i] = static_cast<int>(m_speedDirection_L[i] * (controlValue_Cur[i] - controlValue_Prev[i]) /TimePerControlLoop * m_kGimbalSpeed_L[m_SpeedPedalIndex_Cur]);
                if(abs(targetValueTmp[0][i]) > abs(m_GimbalMaxSpeed_L[m_SpeedPedalIndex_Cur]))
                {
                    targetValueTmp[0][i] = targetValueTmp[0][i] / abs(targetValueTmp[0][i]) * m_GimbalMaxSpeed_L[m_SpeedPedalIndex_Cur];
                }
            }
            else
            {
                targetValueTmp[0][i] = 0;
            }
        }
    }

   return targetValueTmp;
}

void RobotKinematics::storeCurAsPrev(const HandlePose& handlePoseCur,
                                         const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                         const std::array<int, MotorNum>& motorPositionCur_L, const std::array<int, MotorNum>& motorPositionCur_R,
                                         const int&  enableTagCur_L, const int&  enableTagCur_R ) const
{
    m_HandlePosePrev = handlePoseCur;
    m_ControlValuePrev_L = controlValueCur_L;
    m_ControlValuePrev_R = controlValueCur_R;
    m_MotorPositionPrev_L = motorPositionCur_L;
    m_MotorPositionPrev_R = motorPositionCur_R;
    m_SpeedPedalIndex_Prev = m_SpeedPedalIndex_Cur;
    if ((enableTagCur_L == enableAction||enableTagCur_L == keepEnabling) && (m_EnableTagPrev_L == disableAction || m_EnableTagPrev_L == keepDisabling))
       {
        m_HandlePoseLastLoop_L.handlePoseL_OpenAngle = handlePoseCur.handlePoseL_OpenAngle;
    }
    if ((enableTagCur_R == enableAction||enableTagCur_R == keepEnabling) && (m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling))
       {
        m_HandlePoseLastLoop_R.handlePoseR_OpenAngle = handlePoseCur.handlePoseR_OpenAngle;
    }
    m_EnableTagPrev_L = enableTagCur_L;
    m_EnableTagPrev_R = enableTagCur_R;
}

double RobotKinematics::calculateOverlapValue(const std::array<int, MotorNum>& motorPosition_Cur,
                                          const HandlePose& masterHandlePose_Cur,const char& side) const
{
    double  overlapValue_Tmp = 0;
    double  openAngle_Cur = 0;
    if(side == 'l'){
        openAngle_Cur = masterHandlePose_Cur.handlePoseL_OpenAngle;
        if(masterHandlePose_Cur.graspIndex_L == 0)
        {
            overlapValue_Tmp = 0;
        }
        if(masterHandlePose_Cur.graspIndex_L == 1)
        {
//            overlapValue_Tmp = -(openAngle_Cur - 0) *  2 * m_EncoderPerDegree_L[3];
            overlapValue_Tmp = 0.5 * m_encoderPerDegree_L[3] * openAngle_Cur * openAngle_Cur;
        }
    }
    if(side == 'r'){
        openAngle_Cur = masterHandlePose_Cur.handlePoseR_OpenAngle;
        if(masterHandlePose_Cur.graspIndex_R == 0)
        {
            overlapValue_Tmp = 0;
        }
        if(masterHandlePose_Cur.graspIndex_R == 1)
        {
//            overlapValue_Tmp = -(openAngle_Cur - 0)  * 2 * m_EncoderPerDegree_R[3];
            overlapValue_Tmp = 0.5 * m_encoderPerDegree_R[3] * openAngle_Cur * openAngle_Cur;
        }
    }
    return abs(overlapValue_Tmp);
}