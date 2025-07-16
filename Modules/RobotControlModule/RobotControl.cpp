#include "RobotControl.h"

std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTime;
std::chrono::duration<double, std::micro>   loopTime;

void RobotControl::readMyInitData()
{
    std::cout << "________________READ MY ROBOT DATA_________________" << std::endl;
    try
    {
        toml::table RobotData = toml::parse_file(m_robotConfigPath);

        /* Other Information */
        {
            /* Motion Scaling Coefficient */
            {
                const toml::array& Arr_Tmp = *(RobotData["MasterDevice"]["MotionScaling"]["Default"].as_array());//读取主手的放缩比
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
                const toml::array& Arr_Tmp = *(RobotData["Robot"]["GimablAndEndEffector_SpeedDirection"]["speedDirection_L"]["Defalut"].as_array());//电机转动方向
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

        /* Right EndEffector Information */
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
            /* Right  Endeffector Initial Rotation Around Y and X */
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
            /* Encoder per Revolution; */
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
                double angleYTmp0 = - 90;
                m_sourceRotAroundY = angleYTmp0 * M_PI / 180;
                LOG(INFO)<< "Master Sensor Source Rotation Around Y (in radius) = "<< m_sourceRotAroundY << "    In Degree: " << angleYTmp << std::endl ;
            }
        }
    }
    catch(const toml::parse_error& err)
    {
        LOG(ERROR) << "Failed to parse toml file: " << err.what();
    }
    loadEndeffectorConfig();
}

void RobotControl::loadEndeffectorConfig()
{
    //Read Instrument Information: Type, Size, ID
    std::vector<std::string> t_endEffectorInfoSplit_L, t_endEffectorInfoSplit_R;
    t_endEffectorInfoSplit_L = split(m_endEffectorLeft,"_");//用_把需要的参数划分开，比如说 类型_尺寸_ID
    t_endEffectorInfoSplit_R = split(m_endEffectorRight,"_");

    LOG(INFO) << "________________READ MY INSTRUMENT DATA_________________";
    LOG(INFO) << "Left Instrument Information: " << m_endEffectorLeft;
    LOG(INFO) << "Right Instrument Information: " << m_endEffectorRight;
    try
    {
        //Load Left Endeffector Information
        toml::table endEffectorData = toml::parse_file(m_configFilePath);//把toml文件复制到endEffectorData上
        {
            //Left Side
            {
                //Instrument Data;
                if(t_endEffectorInfoSplit_L[InstrumentType] != "None")//不是None就加载信息
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

void RobotControl::initiAllData()
{
    m_handlePosePrev.init();
    m_handlePoseInit_L.init();
    m_handlePoseInit_R.init();
    m_handlePoseOrg_L.initOrg_L();
    m_handlePoseOrg_R.initOrg_R();
    m_handlePoseLastLoop_L.initOrg_L();
    m_handlePoseLastLoop_R.initOrg_R();

    Eigen::Matrix3d MasterRotationMatrix;

    m_controlValuePrev_L = {0};
    m_controlValuePrev_R = {0};

    /* set ruckig params for trajectory generation */
    // m_ruckigInputState_R.max_velocity = {500000.0, 500000.0, 500000.0};//30000
    // m_ruckigInputState_R.max_acceleration = {25000.0, 25000.0, 25000.0};//6000
    // m_ruckigInputState_R.max_jerk = {10000.0, 10000.0, 10000.0};//3000

    // m_ruckigInputState_L.max_velocity = {500000.0, 500000.0, 500000.0};//30000
    // m_ruckigInputState_L.max_acceleration = {25000.0, 25000.0, 25000.0};//6000
    // m_ruckigInputState_L.max_jerk = {10000.0, 10000.0, 10000.0};//

    m_ruckigInputState_R.max_velocity = {500000.0, 500000.0, 500000.0};//30000
    m_ruckigInputState_R.max_acceleration = {15000.0, 15000.0, 15000.0};//6000
    m_ruckigInputState_R.max_jerk = {8000.0, 8000.0, 8000.0};//3000

    m_ruckigInputState_L.max_velocity = {500000.0, 500000.0, 500000.0};//30000
    m_ruckigInputState_L.max_acceleration = {20000.0, 20000.0, 20000.0};//6000
    m_ruckigInputState_L.max_jerk = {10000.0, 10000.0, 10000.0};//

}

void RobotControl::startMyThreads()
{
    goToHold();

    std::this_thread::sleep_for(std::chrono::milliseconds(4));

    m_flagControlThread.store(true);

    startControlThread();

    /*TODO: wtf is this sleep for for*/
    std::this_thread::sleep_for(std::chrono::milliseconds(4));

    // startGuidingArmControlThread();
}

void RobotControl::startControlThread()
{
    m_calculateControlDataThread = std::thread(&RobotControl::control, this);
    m_calculateControlDataThread.detach();
}

void RobotControl::control()
{
    QList<QString> arglist;
    arglist.append("speedCur:" + QString::number(m_speedPedalIndex_Cur));
    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::RecvMasterData), arglist);

    startTime = std::chrono::high_resolution_clock::now();

    while(m_flagControlThread && !m_isSystemTerminated)
    {

        receiveMotorData();
        // receiveMotorData_4Maxons();
        if(m_flagInTeleoperation.load())
        {
            teleoperation();
        }

        if(m_flagInCollaboration_EndJoint.load())
        {
            // collaboration();
        }
        if(m_flagInCollaboration_GuidingArm.load())
        {

        }
    }
}

void RobotControl::teleoperation()
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    auto handlePosePrev = m_handlePosePrev;
    auto handlePoseInit_R = m_handlePoseInit_R;
    auto handlePoseInit_L = m_handlePoseInit_L;

    auto motorEncoderCur_R = m_motorEncoderCur_R.load();
    auto motorEncoderCur_L = m_motorEncoderCur_L.load();
    auto motorEncoderInit_R = m_motorEncoderInit_R;
    auto motorEncoderInit_L = m_motorEncoderInit_L;

    std::array<int, MotorNumPerSide>   targetEncoder_R = {0};
    std::array<int, MotorNumPerSide>   targetEncoder_L = {0};
    std::array<int, MotorNumPerSide>   targetEncoderPrev_R = m_motorTargetEncoderPrev_R;
    std::array<int, MotorNumPerSide>   targetEncoderPrev_L = m_motorTargetEncoderPrev_L;

    std::array<int, MotorNumPerSide>   targetVelocity_R = {0};
    std::array<int, MotorNumPerSide>   targetVelocity_L = {0};

    std::array<double, ControlValueNum> controlValueCur_R = {0};
    std::array<double, ControlValueNum> controlValueCur_L = {0};

    int enableTagCur_L = enableCase_KeepPressPedal(handlePoseCur, 'l');
    int enableTagCur_R = enableCase_KeepPressPedal(handlePoseCur, 'r');

    /* Define the Motion Scaling and return Current Speed Index: {1,2,3,4} */
    setNewSpeed(handlePosePrev, handlePoseCur);

    if(enableTagCur_L == enableAction||enableTagCur_L == keepEnabling)
    {
        if(enableTagCur_L == enableAction)
        {
            handlePoseInit_L = handlePoseCur;

            motorEncoderInit_L = motorEncoderCur_L;

            m_alignmentNumber_L = 0;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_L, handlePoseCur, 'l');
        }

        controlValueCur_L = motionMapping_L(handlePoseCur);

        targetEncoder_L = calculateTargetEncoder(controlValueCur_L, motorEncoderInit_L, 'l');

        targetVelocity_L = calculateTargetVelocity(targetEncoder_L, targetEncoderPrev_L, 'l');

        if(enableTagCur_L == enableAction){
            targetVelocity_L = {0};
        }
    }
    else
    {
        targetEncoder_L = motorEncoderCur_L;
        targetEncoder_L[4] = m_motorTargetEncoderLast_L[4];
        targetEncoder_L[5] = m_motorTargetEncoderLast_L[5];
        targetEncoder_L[6] = m_motorTargetEncoderLast_L[6];
        targetEncoder_L[7] = m_motorTargetEncoderLast_L[7];
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            targetEncoder_L[8] = m_motorTargetEncoderLast_L[8];
            targetEncoder_L[9] = m_motorTargetEncoderLast_L[9];
        }
        targetVelocity_L = {0};
    }

    if(enableTagCur_R == enableAction||enableTagCur_R == keepEnabling)
    {
        if(enableTagCur_R == enableAction)
        {
            handlePoseInit_R = handlePoseCur;

            motorEncoderInit_R = motorEncoderCur_R;

            m_alignmentNumber_R = 0;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_R, handlePoseCur, 'r');
        }

        controlValueCur_R = motionMapping_R(handlePoseCur);

        targetEncoder_R = calculateTargetEncoder(controlValueCur_R, motorEncoderInit_R, 'r');

        targetVelocity_R = calculateTargetVelocity(targetEncoder_R, targetEncoderPrev_R, 'r');

        if(enableTagCur_R == enableAction){
            targetVelocity_R = {0};
        }
    }
    else
    {
        targetEncoder_R = motorEncoderCur_R;
        /*发送给maxon本身的encoder会出现抖动*/
        targetEncoder_R[4] = m_motorTargetEncoderLast_R[4];
        targetEncoder_R[5] = m_motorTargetEncoderLast_R[5];
        targetEncoder_R[6] = m_motorTargetEncoderLast_R[6];
        targetEncoder_R[7] = m_motorTargetEncoderLast_R[7];
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            targetEncoder_R[8] = m_motorTargetEncoderLast_R[8];
            targetEncoder_R[9] = m_motorTargetEncoderLast_R[9];
        }
        targetVelocity_R = {0};
    }

    sendMotorData(targetEncoder_R, targetVelocity_R, targetEncoder_L, targetVelocity_L);
    storeCurAsPrev(handlePoseCur, controlValueCur_L, motorEncoderCur_L, targetEncoder_L, enableTagCur_L, controlValueCur_R, motorEncoderCur_R, targetEncoder_R, enableTagCur_R);
}

/*力拖动*/
void RobotControl::collaboration(){
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    auto handlePosePrev = m_handlePosePrev;
    auto handlePoseInit_R = m_handlePoseInit_R;
    auto handlePoseInit_L = m_handlePoseInit_L;

    auto motorEncoderCur_R = m_motorEncoderCur_R.load();
    auto motorEncoderCur_L = m_motorEncoderCur_L.load();
    auto motorEncoderInit_R = m_motorEncoderInit_R;
    auto motorEncoderInit_L = m_motorEncoderInit_L;


    std::array<int, MotorNumPerSide>   targetEncoder_R = {0};
    std::array<int, MotorNumPerSide>   targetEncoder_L = {0};
    std::array<int, MotorNumPerSide>   targetEncoderPrev_R = m_motorTargetEncoderPrev_R;
    std::array<int, MotorNumPerSide>   targetEncoderPrev_L = m_motorTargetEncoderPrev_L;

    std::array<int, MotorNumPerSide>   targetVelocity_R = {0};
    std::array<int, MotorNumPerSide>   targetVelocity_L = {0};

    std::array<double, ControlValueNum> controlValueCur_R = {0};
    std::array<double, ControlValueNum> controlValueCur_L = {0};

    int enableTagCur_L = enableCase_KeepPressPedal(handlePoseCur, 'l');
    int enableTagCur_R = enableCase_KeepPressPedal(handlePoseCur, 'r');


    if(enableTagCur_L == enableAction)
    {
        LOG(INFO) << "enable!!!";
    }

    /* Define the Motion Scaling and return Current Speed Index: {1,2,3,4} */
    setNewSpeed(handlePosePrev, handlePoseCur);

    if(enableTagCur_L == enableAction||enableTagCur_L == keepEnabling)
    {
        if(enableTagCur_L == enableAction)
        {
            handlePoseInit_L = handlePoseCur;

            motorEncoderInit_L = motorEncoderCur_L;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_L, handlePoseCur, 'l');
        }

        controlValueCur_L = motionMapping_L_ForceControl(handlePoseCur);

        targetEncoder_L = calculateTargetEncoder(controlValueCur_L, motorEncoderInit_L, 'l');

        targetVelocity_L = calculateTargetVelocity(targetEncoder_L, targetEncoderPrev_L, 'l');

        if(enableTagCur_L == enableAction){
            targetVelocity_L = {0};
        }

    }
    else
    {
        targetVelocity_L = {0};
    }

    if(enableTagCur_R == enableAction||enableTagCur_R == keepEnabling)
    {
        if(enableTagCur_R == enableAction)
        {
            handlePoseInit_R = handlePoseCur;

            motorEncoderInit_R = motorEncoderCur_R;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_R, handlePoseCur, 'r');
        }

        controlValueCur_R = motionMapping_R_ForceControl(handlePoseCur);

        targetEncoder_R = calculateTargetEncoder(controlValueCur_R, motorEncoderInit_R, 'r');

        targetVelocity_R = calculateTargetVelocity(targetEncoder_R, targetEncoderPrev_R, 'r');

        if(enableTagCur_R == enableAction){
            targetVelocity_R = {0};
        }
    }
    else
    {
        targetVelocity_R = {0};
    }

    sendMotorData_4Maxons_ForceControl(targetEncoder_R, targetVelocity_R, targetEncoder_L, targetVelocity_L);
    storeCurAsPrev(handlePoseCur, controlValueCur_L, motorEncoderCur_L, targetEncoder_L, enableTagCur_L, controlValueCur_R, motorEncoderCur_R, targetEncoder_R, enableTagCur_R);
}

std::array<double, ControlValueNum> RobotControl::motionMapping_L(const HandlePose& handlePoseCur)
{   
    std::array<double, ControlValueNum> controlValueTmp_L = {0};

    if (m_alignmentNumber_L < 100) { m_alignmentNumber_L++; }

    /* alpha(pitch) */
    double alpha_Org_L = m_handlePoseOrg_L.handlePoseInSlaveFrameL_Elevation;
    double alpha_Init_L = m_handlePoseInit_L.handlePoseInSlaveFrameL_Elevation;
    double alpha_Last_L = m_handlePoseLastLoop_L.handlePoseInSlaveFrameL_Elevation;
    double alpha_Cur_L = handlePoseCur.handlePoseInSlaveFrameL_Elevation;

    double delt_alphaCur_L = (alpha_Cur_L - alpha_Init_L) * 180 / M_PI;
    double delt_alphaInit_L = (alpha_Init_L - alpha_Last_L) * 180 / M_PI;
    double delt_alphaOrg_L = (alpha_Last_L - alpha_Org_L) * 180 / M_PI;
    double delt_alpha_L = delt_alphaCur_L + delt_alphaInit_L * m_alignmentNumber_L / 100 + delt_alphaOrg_L;

    /* 计算 deltLength_alpha_L */
    double deltLength_alpha_L_1 = cableLengths_2(delt_alpha_L);
    double deltLength_alpha_L_2 = cableLengths_2(-delt_alpha_L);

    /* 计算单边 OpenAngle */
    double openAngle_L = handlePoseCur.handlePoseL_OpenAngle;
    auto openAngle_L_new = (openAngle_L < 0) ? 0.008 * pow(openAngle_L, 3) : pow(openAngle_L, 3)/400;

    /*计算 beta(yaw) */
    double beta_Org_L = m_handlePoseOrg_L.handlePoseInSlaveFrameL_Arzimuth;
    double beta_Init_L = m_handlePoseInit_L.handlePoseInSlaveFrameL_Arzimuth;
    double beta_Last_L = m_handlePoseLastLoop_L.handlePoseInSlaveFrameL_Arzimuth;
    double beta_Cur_L = handlePoseCur.handlePoseInSlaveFrameL_Arzimuth;

    double delt_betaCur_L = (beta_Cur_L - beta_Init_L) * 180 / M_PI;
    double delt_betaInit_L = (beta_Init_L - beta_Last_L) * 180 / M_PI;
    double delt_betaOrg_L = (beta_Last_L - beta_Org_L) * 180 / M_PI;
    double delt_beta_L = delt_betaCur_L + delt_betaInit_L * m_alignmentNumber_L / 100 + delt_betaOrg_L;
   \
    /*计算 yaw 的绳长变化*/
    double deltLength_beta_L_left_1 = cableLengths_3(-delt_alpha_L, delt_beta_L, -openAngle_L_new);
    double deltLength_beta_L_left_2 = cableLengths_3(-delt_alpha_L, - delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_1 = cableLengths_3(delt_alpha_L, delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_2 = cableLengths_3(delt_alpha_L, - delt_beta_L, -openAngle_L_new);

    /*计算 gamma（roll）*/
    double gamma_Org_L = m_handlePoseOrg_L.handlePoseInSlaveFrameL_Roll;
    double gamma_Init_L = m_handlePoseInit_L.handlePoseInSlaveFrameL_Roll;
    double gamma_Last_L = m_handlePoseLastLoop_L.handlePoseInSlaveFrameL_Roll;
    double gamma_Cur_L = handlePoseCur.handlePoseInSlaveFrameL_Roll;

    double delt_gammaCur_L = (gamma_Cur_L - gamma_Init_L) * 180 / M_PI;
    double delt_gammaInit_L = (gamma_Init_L - gamma_Last_L) * 180 / M_PI;
    double delt_gammaOrg_L = (gamma_Last_L - gamma_Org_L) * 180 / M_PI;
    double delt_gamma_L = delt_gammaCur_L + delt_gammaInit_L * m_alignmentNumber_L / 100 + delt_gammaOrg_L;

    /*position 解算*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/
    m_ruckigInputState_L.target_position = {(endEffectorInit_X_L + endEffectorDelta_X_L), (endEffectorInit_Y_L + endEffectorDelta_Y_L), (endEffectorInit_Z_L + endEffectorDelta_Z_L)};
    m_ruckigInputState_L.target_velocity = {0.0, 0.0, 0.0};
    m_ruckigInputState_L.target_acceleration = {0.0, 0.0, 0.0};

    auto plannerStatus = m_ruckigPlanner_L.update(m_ruckigInputState_L, m_ruckigOutputState_L);
    if(plannerStatus == ruckig::Result::Working){
        //        LOG(INFO) << "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
        ruckig::join(m_ruckigOutputState_L.new_position);
    }
    else
    {
        //        LOG(INFO) << "planner Status: " <<std::dec << plannerStatus;
    }
    m_ruckigOutputState_L.pass_to_input(m_ruckigInputState_L);

    double endEffectorTarget_X_L = m_ruckigOutputState_L.new_position[0];// / m_motionScaling[2]，初始值加上delta值为末端点应该移动到的位置，以此位置解算
    double endEffectorTarget_Y_L = m_ruckigOutputState_L.new_position[1];// / m_motionScaling[0]
    double endEffectorTarget_Z_L = m_ruckigOutputState_L.new_position[2];// / m_motionScaling[0]

    double jointAngle1_Init_L, jointAngle2_Init_L, jointAngle3_Init_L;
    jointAngle1_Init_L = m_endEffectorInitJointAngle_L[0];
    jointAngle2_Init_L = m_endEffectorInitJointAngle_L[1];
    jointAngle3_Init_L = m_endEffectorInitJointAngle_L[2];

    double jointAngle1_target_L, jointAngle2_target_L, jointAngle3_target_L;

    jointAngle2_target_L = -acos((endEffectorTarget_Y_L * endEffectorTarget_Y_L + endEffectorTarget_Z_L * endEffectorTarget_Z_L - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                 (2 * m_endArm_1 * m_endArm_2));

    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_L);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_L);

    jointAngle1_target_L = atan((coefficient_tmp1 * endEffectorTarget_Y_L + coefficient_tmp2 * endEffectorTarget_Z_L) /
                                 (coefficient_tmp2 * endEffectorTarget_Y_L - coefficient_tmp1 * endEffectorTarget_Z_L));

    jointAngle3_target_L = (m_armAnglePerSide - jointAngle1_target_L  - jointAngle2_target_L)
                           -(m_armAnglePerSide - jointAngle1_Init_L- jointAngle2_Init_L);

    //Control Value:
    //  [0]:         X-Axis          Target position
    //[1 - 3]:   End Joint 1 - 3     Target Angle

    controlValueTmp_L[0] = endEffectorTarget_X_L - endEffectorInit_X_L; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_L[1] = (jointAngle1_target_L - jointAngle1_Init_L) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_L[2] = (jointAngle2_target_L - jointAngle2_Init_L) * 180 / M_PI;
    controlValueTmp_L[3] = jointAngle3_target_L * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
        controlValueTmp_L[4] = (delt_beta_L + delt_alpha_L * m_compRatio_L) - openAngle_L_new;
        controlValueTmp_L[5] = (delt_beta_L + delt_alpha_L * m_compRatio_L) + openAngle_L_new;
        controlValueTmp_L[6] = -delt_alpha_L;
        controlValueTmp_L[7] = -delt_gamma_L;

        controlValueTmp_L[8] = 0;
        controlValueTmp_L[9] = 0;
    }

    /*6轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
        controlValueTmp_L[9] = -delt_gamma_L;
        controlValueTmp_L[8] = -deltLength_alpha_L_1;

        controlValueTmp_L[7] = -deltLength_beta_L_left_1;
        controlValueTmp_L[6] = -deltLength_beta_L_left_2;
        controlValueTmp_L[5] = -deltLength_beta_L_right_1;
        controlValueTmp_L[4] = -deltLength_beta_L_right_2;
    }
    return controlValueTmp_L;
}

std::array<double, ControlValueNum> RobotControl::motionMapping_R(const HandlePose& handlePoseCur)

{
    std::array<double, ControlValueNum> controlValueTmp_R = {0};

    if (m_alignmentNumber_R < 100) { m_alignmentNumber_R++; }
    LOG(INFO)<<"m_alignmentNumber_R: "<<m_alignmentNumber_R;

    /* alpha(pitch)*/
    double alpha_Org_R = m_handlePoseOrg_R.handlePoseInSlaveFrameR_Elevation;
    double alpha_Init_R = m_handlePoseInit_R.handlePoseInSlaveFrameR_Elevation;
    double alpha_Last_R = m_handlePoseLastLoop_R.handlePoseInSlaveFrameR_Elevation;
    double alpha_Cur_R = handlePoseCur.handlePoseInSlaveFrameR_Elevation;

    double delt_alphaCur_R = (alpha_Cur_R - alpha_Init_R) * 180 / M_PI;
    double delt_alphaInit_R = (alpha_Init_R - alpha_Last_R) * 180 / M_PI;
    double delt_alphaOrg_R = (alpha_Last_R - alpha_Org_R) * 180 / M_PI;
    double delt_alpha_R = delt_alphaCur_R + delt_alphaInit_R * m_alignmentNumber_R / 100 + delt_alphaOrg_R;

    /* 计算 deltLength_alpha_R*/
    double deltLength_alpha_R_1 = cableLengths_2(delt_alpha_R);
    double deltLength_alpha_R_2 = cableLengths_2(-delt_alpha_R);

    /* 计算单边 OpenAngle*/
    double openAngle_R = handlePoseCur.handlePoseR_OpenAngle;
    auto openAngle_R_new = (openAngle_R < 0) ? 0.008 * pow(openAngle_R, 3) : pow(openAngle_R, 3)/400;

    /*计算 beta(yaw)*/
    double beta_Org_R = m_handlePoseOrg_R.handlePoseInSlaveFrameR_Arzimuth;
    double beta_Init_R = m_handlePoseInit_R.handlePoseInSlaveFrameR_Arzimuth;
    double beta_Last_R = m_handlePoseLastLoop_R.handlePoseInSlaveFrameR_Arzimuth;
    double beta_Cur_R = handlePoseCur.handlePoseInSlaveFrameR_Arzimuth;

    double delt_betaCur_R = (beta_Cur_R - beta_Init_R) * 180 / M_PI;
    double delt_betaInit_R = (beta_Init_R - beta_Last_R) * 180 / M_PI;
    double delt_betaOrg_R = (beta_Last_R - beta_Org_R) * 180 / M_PI;
    double delt_beta_R = delt_betaCur_R + delt_betaInit_R * m_alignmentNumber_R / 100 + delt_betaOrg_R;

    /*计算 yaw 的绳长变化*/
    double deltLength_beta_R_left_1 = cableLengths_3(-delt_alpha_R, delt_beta_R, -openAngle_R_new);
    double deltLength_beta_R_left_2 = cableLengths_3(-delt_alpha_R, - delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_1 = cableLengths_3(delt_alpha_R, delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_2 = cableLengths_3(delt_alpha_R, - delt_beta_R, -openAngle_R_new);

    /*计算 gamma（roll）*/

    double gamma_Org_R = m_handlePoseOrg_R.handlePoseInSlaveFrameR_Roll;
    double gamma_Init_R = m_handlePoseInit_R.handlePoseInSlaveFrameR_Roll ;
    double gamma_Last_R = m_handlePoseLastLoop_R.handlePoseInSlaveFrameR_Roll;
    double gamma_Cur_R = handlePoseCur.handlePoseInSlaveFrameR_Roll;

    double delt_gammaCur_R = (gamma_Cur_R - gamma_Init_R) * 180 / M_PI;
    double delt_gammaInit_R = (gamma_Init_R - gamma_Last_R) * 180 / M_PI;
    double delt_gammaOrg_R = (gamma_Last_R - gamma_Org_R) * 180 / M_PI;
    double delt_gamma_R = delt_gammaCur_R + delt_gammaInit_R * m_alignmentNumber_R / 100 + delt_gammaOrg_R;

    LOG(INFO)<<"ROLL_R: "<<delt_gamma_R;
    LOG(INFO)<<"YAW_R: "<<delt_beta_R;
    LOG(INFO)<<"PITCH_R: "<<delt_alpha_R;

    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    double endEffectorDelta_X_R =  (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/
    m_ruckigInputState_R.target_position = {(endEffectorInit_X_R + endEffectorDelta_X_R),
                                            (endEffectorInit_Y_R + endEffectorDelta_Y_R),
                                            (endEffectorInit_Z_R + endEffectorDelta_Z_R)};
    m_ruckigInputState_R.target_velocity = {0.0, 0.0, 0.0};
    m_ruckigInputState_R.target_acceleration = {0.0, 0.0, 0.0};

    auto plannerStatus = m_ruckigPlanner_R.update(m_ruckigInputState_R, m_ruckigOutputState_R);
    if(plannerStatus == ruckig::Result::Working){

            //        LOG(INFO) << "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
        ruckig::join(m_ruckigOutputState_R.new_position);
    }
    else
    {
        //        LOG(INFO) << "planner Status: " <<std::dec << plannerStatus;
    }
    m_ruckigOutputState_R.pass_to_input(m_ruckigInputState_R);

    double endEffectorTarget_X_R = m_ruckigOutputState_R.new_position[0];// / m_motionScaling[2]，初始值加上delta值为末端点应该移动到的位置，以此位置解算
    double endEffectorTarget_Y_R = m_ruckigOutputState_R.new_position[1];// / m_motionScaling[0]
    double endEffectorTarget_Z_R = m_ruckigOutputState_R.new_position[2];// / m_motionScaling[0]

    double jointAngle1_Init_R, jointAngle2_Init_R, jointAngle3_Init_R;
    jointAngle1_Init_R = m_endEffectorInitJointAngle_R[0];
    jointAngle2_Init_R = m_endEffectorInitJointAngle_R[1];
    jointAngle3_Init_R = m_endEffectorInitJointAngle_R[2];

    double jointAngle1_target_R, jointAngle2_target_R, jointAngle3_target_R;
    jointAngle2_target_R = acos((endEffectorTarget_Y_R * endEffectorTarget_Y_R + endEffectorTarget_Z_R * endEffectorTarget_Z_R - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                (2 * m_endArm_1 * m_endArm_2));
    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_R);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_R);

    jointAngle1_target_R = atan((coefficient_tmp1 * endEffectorTarget_Y_R + coefficient_tmp2 * endEffectorTarget_Z_R) /
                               (coefficient_tmp2 * endEffectorTarget_Y_R - coefficient_tmp1 * endEffectorTarget_Z_R));

    jointAngle3_target_R = (m_armAnglePerSide - jointAngle1_target_R  - jointAngle2_target_R)
                           -(m_armAnglePerSide - jointAngle1_Init_R- jointAngle2_Init_R);

    /*输出值*/
    controlValueTmp_R[0] = endEffectorTarget_X_R - endEffectorInit_X_R; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_R[1] = (jointAngle1_target_R - jointAngle1_Init_R) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_R[2] = (jointAngle2_target_R - jointAngle2_Init_R) * 180 / M_PI;
    controlValueTmp_R[3] = jointAngle3_target_R * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
        controlValueTmp_R[4] = (delt_beta_R + delt_alpha_R * m_compRatio_R) - openAngle_R_new;
        controlValueTmp_R[5] = (delt_beta_R + delt_alpha_R * m_compRatio_R) + openAngle_R_new;
        controlValueTmp_R[6] = -delt_alpha_R;
        controlValueTmp_R[7] = -delt_gamma_R;

        controlValueTmp_R[8] = 0;
        controlValueTmp_R[9] = 0;
    }

    /*6轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
        controlValueTmp_R[9] = -delt_gamma_R;
        controlValueTmp_R[8] = -deltLength_alpha_R_1;

        controlValueTmp_R[7] = -deltLength_beta_R_left_1;
        controlValueTmp_R[6] = -deltLength_beta_R_left_2;
        controlValueTmp_R[5] = -deltLength_beta_R_right_1;
        controlValueTmp_R[4] = -deltLength_beta_R_right_2;
    }
    controlValueTmp_R[10] = handlePoseCur.graspIndex_R;
    return controlValueTmp_R;
}

std::array<double, ControlValueNum> RobotControl::motionMapping_L_ForceControl(const HandlePose& handlePoseCur)
{
    std::array<double, ControlValueNum> controlValueTmp_L = {0};

    /*position 解算*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/


    /*路径规划*/
    m_ruckigInputState_L.target_position = {(endEffectorInit_X_L + endEffectorDelta_X_L), (endEffectorInit_Y_L + endEffectorDelta_Y_L), (endEffectorInit_Z_L + endEffectorDelta_Z_L)};
    m_ruckigInputState_L.target_velocity = {0.0, 0.0, 0.0};
    m_ruckigInputState_L.target_acceleration = {0.0, 0.0, 0.0};

    auto plannerStatus = m_ruckigPlanner_L.update(m_ruckigInputState_L, m_ruckigOutputState_L);
    if(plannerStatus == ruckig::Result::Working){
        //        LOG(INFO) <<  "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
        ruckig::join(m_ruckigOutputState_L.new_position);
    }
    else
    {
        //        LOG(INFO) << "planner Status: " <<std::dec << plannerStatus;
    }
    m_ruckigOutputState_L.pass_to_input(m_ruckigInputState_L);

    double endEffectorTarget_X_L = m_ruckigOutputState_L.new_position[0];// / m_motionScaling[2]，初始值加上delta值为末端点应该移动到的位置，以此位置解算
    double endEffectorTarget_Y_L = m_ruckigOutputState_L.new_position[1];// / m_motionScaling[0]
    double endEffectorTarget_Z_L = m_ruckigOutputState_L.new_position[2];// / m_motionScaling[0]

    double jointAngle1_Init_L, jointAngle2_Init_L, jointAngle3_Init_L;
    jointAngle1_Init_L = m_endEffectorInitJointAngle_L[0];
    jointAngle2_Init_L = m_endEffectorInitJointAngle_L[1];
    jointAngle3_Init_L = m_endEffectorInitJointAngle_L[2];

    double jointAngle1_target_L, jointAngle2_target_L, jointAngle3_target_L;

    jointAngle2_target_L = -acos((endEffectorTarget_Y_L * endEffectorTarget_Y_L + endEffectorTarget_Z_L * endEffectorTarget_Z_L - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                 (2 * m_endArm_1 * m_endArm_2));
    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_L);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_L);

    jointAngle1_target_L = atan((coefficient_tmp1 * endEffectorTarget_Y_L + coefficient_tmp2 * endEffectorTarget_Z_L) /
                                 (coefficient_tmp2 * endEffectorTarget_Y_L - coefficient_tmp1 * endEffectorTarget_Z_L));

    jointAngle3_target_L = (m_armAnglePerSide - jointAngle1_target_L  - jointAngle2_target_L)
                           -(m_armAnglePerSide - jointAngle1_Init_L- jointAngle2_Init_L);

    controlValueTmp_L[0] = endEffectorTarget_X_L - endEffectorInit_X_L; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_L[1] = (jointAngle1_target_L - jointAngle1_Init_L) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_L[2] = (jointAngle2_target_L - jointAngle2_Init_L) * 180 / M_PI;
    controlValueTmp_L[3] = jointAngle3_target_L * 180 / M_PI;

    return controlValueTmp_L;
}

std::array<double, ControlValueNum> RobotControl:: motionMapping_R_ForceControl(const HandlePose& handlePoseCur)

{
    std::array<double, ControlValueNum> controlValueTmp_R = {0};

    /*进入使能时机械臂末端的位置*/
    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];

    /*endEffectorDelta为相对于endEffectorInit的位置改变量*/
    double endEffectorDelta_X_R =  (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/
    m_ruckigInputState_R.target_position = {(endEffectorInit_X_R + endEffectorDelta_X_R),
                                            (endEffectorInit_Y_R + endEffectorDelta_Y_R),
                                            (endEffectorInit_Z_R + endEffectorDelta_Z_R)};
    m_ruckigInputState_R.target_velocity = {0.0, 0.0, 0.0};
    m_ruckigInputState_R.target_acceleration = {0.0, 0.0, 0.0};

    auto plannerStatus = m_ruckigPlanner_R.update(m_ruckigInputState_R, m_ruckigOutputState_R);
    if(plannerStatus == ruckig::Result::Working){

            //        LOG(INFO) << "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
        ruckig::join(m_ruckigOutputState_R.new_position);
    }
    else
    {
        //        LOG(INFO) << "planner Status: " <<std::dec << plannerStatus;
    }
    m_ruckigOutputState_R.pass_to_input(m_ruckigInputState_R);

    double endEffectorTarget_X_R = m_ruckigOutputState_R.new_position[0];// / m_motionScaling[2]，初始值加上delta值为末端点应该移动到的位置，以此位置解算
    double endEffectorTarget_Y_R = m_ruckigOutputState_R.new_position[1];// / m_motionScaling[0]
    double endEffectorTarget_Z_R = m_ruckigOutputState_R.new_position[2];// / m_motionScaling[0]

    double jointAngle1_Init_R, jointAngle2_Init_R, jointAngle3_Init_R;
    jointAngle1_Init_R = m_endEffectorInitJointAngle_R[0];
    jointAngle2_Init_R = m_endEffectorInitJointAngle_R[1];
    jointAngle3_Init_R = m_endEffectorInitJointAngle_R[2];

    double jointAngle1_target_R, jointAngle2_target_R, jointAngle3_target_R;
    jointAngle2_target_R = acos((endEffectorTarget_Y_R * endEffectorTarget_Y_R + endEffectorTarget_Z_R * endEffectorTarget_Z_R - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                (2 * m_endArm_1 * m_endArm_2));
    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_R);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_R);

    jointAngle1_target_R = atan((coefficient_tmp1 * endEffectorTarget_Y_R + coefficient_tmp2 * endEffectorTarget_Z_R) /
                               (coefficient_tmp2 * endEffectorTarget_Y_R - coefficient_tmp1 * endEffectorTarget_Z_R));

    jointAngle3_target_R = (m_armAnglePerSide - jointAngle1_target_R  - jointAngle2_target_R)
                           -(m_armAnglePerSide - jointAngle1_Init_R- jointAngle2_Init_R);

    /*输出值*/
    controlValueTmp_R[0] = endEffectorTarget_X_R - endEffectorInit_X_R; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_R[1] = (jointAngle1_target_R - jointAngle1_Init_R) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_R[2] = (jointAngle2_target_R - jointAngle2_Init_R) * 180 / M_PI;
    controlValueTmp_R[3] = jointAngle3_target_R * 180 / M_PI;

    return controlValueTmp_R;
}

void RobotControl::storeCurAsPrev(const HandlePose& handlePoseCur,
                                  const std::array<double, ControlValueNum> controlValueCur_L,
                                  const std::array<int, MotorNumPerSide>& motorPositionCur_L,
                                  const std::array<int, MotorNumPerSide>& motorTargetEncoder_L, const int&  enableTagCur_L,
                                  const std::array<double, ControlValueNum> controlValueCur_R,
                                  const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                  const std::array<int, MotorNumPerSide>& motorTargetEncoder_R, const int&  enableTagCur_R)
{
    m_handlePosePrev = handlePoseCur;
    m_controlValuePrev_L = controlValueCur_L;
    m_motorPositionPrev_L = motorPositionCur_L;
    m_controlValuePrev_R = controlValueCur_R;
    m_motorPositionPrev_R = motorPositionCur_R;
    m_speedPedalIndex_Prev = m_speedPedalIndex_Cur;

    m_motorTargetEncoderPrev_L = motorTargetEncoder_L;
    m_motorTargetEncoderPrev_R = motorTargetEncoder_R;

    if ((enableTagCur_L == enableAction || enableTagCur_L == keepEnabling) && (m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling))
    {
        m_handlePoseLastLoop_L.handlePoseL_OpenAngle = handlePoseCur.handlePoseL_OpenAngle;
    }
    if ((enableTagCur_R == enableAction || enableTagCur_R == keepEnabling) && (m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling))
    {
        m_handlePoseLastLoop_R.handlePoseR_OpenAngle = handlePoseCur.handlePoseR_OpenAngle;
    }
    m_enableTagPrev_L = enableTagCur_L;
    m_enableTagPrev_R = enableTagCur_R;
}

double RobotControl::cableLengths_2(double alpha) const {
    // 计算控制yaw的绳长变化
    // 常量和参数
    double r2_2 = 1.2;
    double L2_1 = 1.2;
    double L2_3 = 1.8;
    double z2_1 = 2.3;
    double L2_k = 0.3;
    double pl2 = 0;
    double q_2 = alpha + 90;

    // 计算 pl2
    if (q_2 < 66.2763) {
        double c2_x = 0;
        double c2_y = 2.3;
        double L2_2 = sqrt(pow(L2_1, 2) + pow(L2_3, 2));

        double p2_x = c2_x + cos(q_2 * M_PI / 180.0) * L2_3 + cos((90 - q_2) * M_PI / 180.0) * L2_1;
        double p2_y = c2_y + sin(q_2 * M_PI / 180.0) * L2_3 - sin((90 - q_2) * M_PI / 180.0) * L2_1;
        pl2 = sqrt(pow(p2_x - L2_k, 2) + pow(p2_y, 2));

    } else if (q_2 >= 66.2763) {
        double l2_0 = 1.98494;
        double l2_1 = 1.8;
        double s2_1 = 23.7237;
        double s2_0 = (q_2 - 90 + s2_1) / 360.0 * 2 * M_PI * r2_2;
        pl2 = l2_0 + s2_0 + l2_1;
    }

    // 调整常数
    double pl2_90 = 4.2818;
    double pl2_c = pl2 - pl2_90; // 控制轴2的绳长

    return pl2_c;
}
double RobotControl::cableLengths_3(double alpha, double q_3_pre, double Openangle) const {
    // 计算控制pitch的绳长变化
    // 常量和参数
    double r3_3 = 1.15;
    double l3_1 = 1.2;
    double l3_2 = 1.4;
    double l3_3 = 0.9;

    double L4_1 = 1.360147;
    double L4_2 = 0.4;
    double r4_2 = 0.9;
    double r4_1 = 0.875 + 0.15;
    double z4_1 = 2.3;
    double q4_t = 74.6;

    double pl31 = 0, pl32 = 0;
    double q_2 = 90 + alpha;

    // 计算 pl31
    double alpha3 = asin(l3_3 / r3_3) * 180.0 / M_PI;
    double qt3 = alpha3 + 2.0163 ;//判断相切时使用 53.5163

    double q_3 = 90 + q_3_pre + Openangle;

    if (q_3 <= qt3) {
        double theta3 = q_3 - alpha3;
        pl31 = sqrt(pow(l3_1 - cos(theta3 * M_PI / 180.0) * r3_3, 2) + pow(l3_2 + sin(theta3 * M_PI / 180.0) * r3_3, 2));
    } else if (q_3 > qt3) {
        double l3_2_new = 1.44135;
        double a3_2 = q_3 - alpha3 - 2.0163;
        double s3_2 = a3_2 / 360.0 * 2 * M_PI * r3_3;
        pl31 = l3_2_new + s3_2;
    }

    // 计算 pl32（解耦长度）
    if (q_2 < 50.2743 && q_2 >= 0) {
        double l4_0 = 0.794;
        double c4_1_x = 1.5;
        double c4_1_y = -1.809;

        double p4_x = L4_1 * cos((q_2 - asin(L4_2 / L4_1) * 180.0 / M_PI) * M_PI / 180.0);
        double p4_y = L4_1 * sin((q_2 - asin(L4_2 / L4_1) * 180.0 / M_PI) * M_PI / 180.0);
        double l4_1 = sqrt(pow(p4_x - c4_1_x, 2) + pow(p4_y - c4_1_y, 2) - pow(r4_1, 2));

        double a4_1 = atan(l4_1 / r4_1) * 180.0 / M_PI;
        double b4_1 = std::abs(atan((p4_x - c4_1_x) / (p4_y - c4_1_y)) * 180.0 / M_PI);
        double lamda4_1 = 90 - a4_1 - b4_1 - 15.5103;
        double s4_01 = lamda4_1 / 360.0 * 2 * M_PI * r4_1;
        pl32 = l4_0 + s4_01 + l4_1;

    } else if (q_2 >= 50.2743 && q_2 <= 180) {
        double l4_2_new = (z4_1 - r4_2 * sin((90 - q4_t) * M_PI / 180.0)) / cos((90 - q4_t) * M_PI / 180.0);
        double a4_2 = asin(L4_2 / L4_1);
        double b4_2 = acos(r4_2 / L4_1);

        double s4_3 = (q_2 - a4_2 * 180.0 / M_PI - b4_2 * 180.0 / M_PI + 90 - q4_t) / 360.0 * 2 * M_PI * r4_2;
        double l4_3 = sqrt(pow(L4_1, 2) - pow(r4_2, 2));
        pl32 = l4_2_new + s4_3 + l4_3;
    }

    // 调整常数
    double pl31_90 = 2.17362379;
    double pl32_90 = 3.78157943;
    double pl3_c = pl31 + pl32 - pl31_90 - pl32_90; // 控制轴3的绳长

    return pl3_c;
}

void RobotControl::calculateEndEffectorPosition(const HandlePose& handlePoseCur, const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side)//yu 通过各电机的读数推断端点在坐标系中的位置
{

    if(side=='r'){
        /*计算关节初始角度*/
        m_endEffectorInitJointAngle_R[0] = -(static_cast<double>(motorPos_Cur[Joint1_R]) - static_cast<double>(JointEncoderInit_1_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        m_endEffectorInitJointAngle_R[1] = (static_cast<double>(motorPos_Cur[Joint2_R]) - static_cast<double>(JointEncoderInit_2_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        m_endEffectorInitJointAngle_R[2] = -(static_cast<double>(motorPos_Cur[Joint3_R]) - static_cast<double>(JointEncoderInit_3_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

        /*计算使能初始位置*/
        m_endEffectorInitPosition_R[0] = handlePoseCur.handlePoseR_X;/*使用绝对编码器位置*/
        m_endEffectorInitPosition_R[1] = m_endArm_1 * sin(m_endEffectorInitJointAngle_R[0]) + m_endArm_2 * sin(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1]);// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
        m_endEffectorInitPosition_R[2] = - m_endArm_1 * cos(m_endEffectorInitJointAngle_R[0]) - m_endArm_2 * cos(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1]);// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    }else if(side=='l')
    {
        /*计算关节初始角度*/
        m_endEffectorInitJointAngle_L[0] = -(static_cast<double>(motorPos_Cur[Joint1_L]) - static_cast<double>(JointEncoderInit_1_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        m_endEffectorInitJointAngle_L[1] = (static_cast<double>(motorPos_Cur[Joint2_L]) - static_cast<double>(JointEncoderInit_2_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        m_endEffectorInitJointAngle_L[2] = -(static_cast<double>(motorPos_Cur[Joint3_L]) - static_cast<double>(JointEncoderInit_3_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

        /*计算使能初始位置*/
        m_endEffectorInitPosition_L[0] = handlePoseCur.handlePoseL_X;
        m_endEffectorInitPosition_L[1] = m_endArm_1 * sin(m_endEffectorInitJointAngle_L[0]) + m_endArm_2 * sin(m_endEffectorInitJointAngle_L[0] + m_endEffectorInitJointAngle_L[1]);// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
        m_endEffectorInitPosition_L[2] = - m_endArm_1 * cos(m_endEffectorInitJointAngle_L[0]) - m_endArm_2 * cos(m_endEffectorInitJointAngle_L[0] + m_endEffectorInitJointAngle_L[1]);// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    }
}
std::array<int, MotorNumPerSide> RobotControl::calculateTargetEncoder(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                                       const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                                       const char& side)const
{
    std::array<int, MotorNumPerSide> targetEncoder = {0};
    if(side == 'r')
    {
        targetEncoder[0] = static_cast<int>(motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        for(int i = 0; i < 4; i++)
        {
            targetEncoder[i] = static_cast<int>(motorPosition_Init[i] + m_SpeedDirection_R[i] * controlValue_Cur[i] / 360 * JointEncoderPerRevolution);
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = controlValue_Cur[i] * m_kForcepPosition_R[i - 4];
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {
            for(int i = 4; i < 8; i++)
            {
                targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_encoderPerDegree_R[i - 4]);
            }
        }
    }
    else if(side == 'l')
    {
        targetEncoder[0] = static_cast<int>(motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        for(int i = 0; i < 4; i++)
        {
            targetEncoder[i] = static_cast<int>(motorPosition_Init[i] + m_SpeedDirection_L[i] * controlValue_Cur[i] / 360 * JointEncoderPerRevolution);
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = controlValue_Cur[i] * m_kForcepPosition_L[i - 4];
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {
            for(int i = 4; i < 8; i++)
            {
                targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_encoderPerDegree_L[i - 4]);
            }
        }
    }
    return targetEncoder;
}

std::array<int, MotorNumPerSide> RobotControl::calculateTargetVelocity(const std::array<int, MotorNumPerSide>& targetEncoderCur,
                                                                        const std::array<int, MotorNumPerSide>& targetEncoderPrev,
                                                                        const char& side)const
{
    std::array<int, MotorNumPerSide> targetVel = {0};

    for(int i = 0; i < 9; i++)
    {
        targetVel[i] = int((targetEncoderCur[i] - targetEncoderPrev[i]) / (0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
    }

    return targetVel;
}

void RobotControl::receiveMotorData()//yu接受传回来的数据
{
    std::array<int, MotorNumPerSide> motorEncoderData_R = {0};
    std::array<int, MotorNumPerSide> motorEncoderData_L = {0};
    std::array<int, GuidingMotorNum> motorEncoderData_Guiding = {0};

    motorEncoderData_R[0] = m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_0);
    motorEncoderData_R[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_0);
    motorEncoderData_R[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_0);
    motorEncoderData_R[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_0);
    motorEncoderData_R[4] = m_motorDriver->getActualPos(MotorType::MAXON, 0, arm_0);
    motorEncoderData_R[5] = m_motorDriver->getActualPos(MotorType::MAXON, 1, arm_0);
    motorEncoderData_R[6] = m_motorDriver->getActualPos(MotorType::MAXON, 2, arm_0);
    motorEncoderData_R[7] = m_motorDriver->getActualPos(MotorType::MAXON, 3, arm_0);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorEncoderData_R[8] = m_motorDriver->getActualPos(MotorType::MAXON, 4, arm_0);
        motorEncoderData_R[9] = m_motorDriver->getActualPos(MotorType::MAXON, 5, arm_0);
    }

    motorEncoderData_L[0] = m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_1);
    motorEncoderData_L[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_1);
    motorEncoderData_L[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_1);
    motorEncoderData_L[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_1);
    motorEncoderData_L[4] = m_motorDriver->getActualPos(MotorType::MAXON, 0, arm_1);
    motorEncoderData_L[5] = m_motorDriver->getActualPos(MotorType::MAXON, 1, arm_1);
    motorEncoderData_L[6] = m_motorDriver->getActualPos(MotorType::MAXON, 2, arm_1);
    motorEncoderData_L[7] = m_motorDriver->getActualPos(MotorType::MAXON, 3, arm_1);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorEncoderData_L[8] = m_motorDriver->getActualPos(MotorType::MAXON, 4, arm_1);
        motorEncoderData_L[9] = m_motorDriver->getActualPos(MotorType::MAXON, 5, arm_1);
    }

    motorEncoderData_Guiding[0] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_guiding);
    motorEncoderData_Guiding[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_guiding);
    motorEncoderData_Guiding[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_guiding);

    std::array<int, MotorNumPerSide> motorErrorCode_L = {0};
    std::array<int, MotorNumPerSide> motorErrorCode_R = {0};
    std::array<int, GuidingMotorNum> motorErrorCode_Guiding = {0};

    motorErrorCode_R[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_0);
    motorErrorCode_R[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_0);
    motorErrorCode_R[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_0);
    motorErrorCode_R[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_0);
    motorErrorCode_R[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_0);
    motorErrorCode_R[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_0);
    motorErrorCode_R[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_0);
    motorErrorCode_R[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_0);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorErrorCode_R[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_0);
        motorErrorCode_R[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_0);
    }

    motorErrorCode_L[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_1);
    motorErrorCode_L[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_1);
    motorErrorCode_L[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_1);
    motorErrorCode_L[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_1);
    motorErrorCode_L[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_1);
    motorErrorCode_L[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_1);
    motorErrorCode_L[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_1);
    motorErrorCode_L[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_1);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorErrorCode_L[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_1);
        motorErrorCode_L[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_1);
    }

    motorErrorCode_Guiding[0] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_guiding);
    motorErrorCode_Guiding[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_guiding);
    motorErrorCode_Guiding[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_guiding);

    std::array<int, MotorNumPerSide> motorStatusWord_L = {0};
    std::array<int, MotorNumPerSide> motorStatusWord_R = {0};
    std::array<int, GuidingMotorNum> motorStatusWord_Guiding = {0};

    motorStatusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
    motorStatusWord_R[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
    motorStatusWord_R[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
    motorStatusWord_R[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
    motorStatusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
    motorStatusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
    motorStatusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
    motorStatusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorStatusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        motorStatusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);
    }

    motorStatusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
    motorStatusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
    motorStatusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
    motorStatusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
    motorStatusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
    motorStatusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
    motorStatusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
    motorStatusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorStatusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        motorStatusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
    }

    std::array<int, MotorNumPerSide> motorVelocity_Guiding = {0};
    motorVelocity_Guiding[0] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 0, arm_0);
    motorVelocity_Guiding[1] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 1, arm_0);
    motorVelocity_Guiding[2] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 2, arm_0);

    std::array<int, MotorNumPerSide> motorOperationMode_R = {0};
    std::array<int, MotorNumPerSide> motorOperationMode_L = {0};
    std::array<int, MotorNumPerSide> motorOperationMode_Guiding = {0};

    motorOperationMode_R[0] =  m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
    motorOperationMode_R[1] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
    motorOperationMode_R[2] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
    motorOperationMode_R[3] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
    motorOperationMode_R[4] =  m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
    motorOperationMode_R[5] =  m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
    motorOperationMode_R[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
    motorOperationMode_R[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorOperationMode_R[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        motorOperationMode_R[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);
    }

    motorOperationMode_L[0] =  m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
    motorOperationMode_L[1] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
    motorOperationMode_L[2] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
    motorOperationMode_L[3] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
    motorOperationMode_L[4] =  m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
    motorOperationMode_L[5] =  m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
    motorOperationMode_L[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
    motorOperationMode_L[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorOperationMode_L[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        motorOperationMode_L[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);
    }
    motorOperationMode_Guiding[0] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
    motorOperationMode_Guiding[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
    motorOperationMode_Guiding[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);

    /*getDigitalInput*/
    std::array<int, 6> motorInputs_Guiding = {0};
    std::array<int, MotorNumPerSide> motorInputs_R = {0};
    std::array<int, MotorNumPerSide> motorInputs_L = {0};

    motorInputs_Guiding[0] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 0, arm_guiding)[0];
    motorInputs_Guiding[1] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 0, arm_guiding)[1];
    motorInputs_Guiding[2] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 1, arm_guiding)[0];
    motorInputs_Guiding[3] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 1, arm_guiding)[1];
    motorInputs_Guiding[4] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 2, arm_guiding)[0];
    motorInputs_Guiding[5] = m_motorDriver->getDigitalInputs(MotorType::ZERO_ERR, 2, arm_guiding)[1];

    motorInputs_R[0] = m_motorDriver->getDigitalInputs(MotorType::MOONS, 0, arm_0)[6];
    motorInputs_R[4] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 0, arm_0)[2];
    motorInputs_R[5] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 1, arm_0)[2];
    motorInputs_R[6] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 2, arm_0)[2];
    motorInputs_R[7] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 3, arm_0)[2];
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorInputs_R[8] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4, arm_0)[2];
        motorInputs_R[9] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5, arm_0)[2];
    }
    motorInputs_L[0] = m_motorDriver->getDigitalInputs(MotorType::MOONS, 0, arm_1)[6];
    motorInputs_L[4] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 0, arm_1)[2];
    motorInputs_L[5] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 1, arm_1)[2];
    motorInputs_L[6] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 2, arm_1)[2];
    motorInputs_L[7] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 3, arm_1)[2];
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        motorInputs_L[8] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4, arm_1)[2];
        motorInputs_L[9] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5, arm_1)[2];
    }
    m_motorEncoderCur_R.store(motorEncoderData_R);
    m_motorEncoderCur_L.store(motorEncoderData_L);

    m_motorStatusWordCur_R.store(motorStatusWord_R);
    m_motorStatusWordCur_L.store(motorStatusWord_L);

    m_digitalInputGuiding.store(motorInputs_Guiding);

    m_motorHomingStatus_L.store(motorInputs_L);//记录光电门是否被激活，为0时表示被激活
    m_motorHomingStatus_R.store(motorInputs_R);//记录光电门是否被激活，为0时表示被激活
}


void RobotControl::sendMotorData(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                 const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L)
{
    std::this_thread::sleep_until(startTime + std::chrono::milliseconds(4));
    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_R[0], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_R[1], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_R[2], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_R[3], arm_0);
    m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoder_R[4], arm_0);
    m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoder_R[5], arm_0);
    m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoder_R[6], arm_0);
    m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoder_R[7], arm_0);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoder_R[8], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoder_R[9], arm_0);
    }

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_L[0], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_L[1], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_L[2], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_L[3], arm_1);
    m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoder_L[4], arm_1);
    m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoder_L[5], arm_1);
    m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoder_L[6], arm_1);
    m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoder_L[7], arm_1);
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    {
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoder_L[8], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoder_L[9], arm_1);
    }

    startTime = std::chrono::high_resolution_clock::now();
}

void RobotControl::sendMotorData_4Maxons_ForceControl(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                 const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L)
{
    std::this_thread::sleep_until(startTime + std::chrono::milliseconds(4));
    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_R[0], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_R[1], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_R[2], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_R[3], arm_0);

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_L[0], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_L[1], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_L[2], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_L[3], arm_1);
\
    startTime = std::chrono::high_resolution_clock::now();
}

void RobotControl::sendMotorData_Teleop(const std::array<int, MotorNumPerSide>& targetEncoderCur_R, const std::array<int, MotorNumPerSide>& targetVelCur_R,
                                        const int& enableTag_R,
                                        const std::array<int, MotorNumPerSide>& targetEncoderCur_L, const std::array<int, MotorNumPerSide>& targetVelCur_L,
                                        const int& enableTag_L)
{
    if(enableTag_R == enableAction || enableTag_R == keepEnabling)
    {
        if(m_teleOperationdMode == TeleOperationMode::CSV_Mode)
        {
            m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVelCur_R[0], arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVelCur_R[1], arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVelCur_R[2], arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVelCur_R[3], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoderCur_R[4], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoderCur_R[5], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoderCur_R[6], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoderCur_R[7], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoderCur_R[8], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoderCur_R[9], arm_0);

        }
        else
        {
            m_motorDriver->setTargetVel(MotorType::MOONS, 0, 0, arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, 0, arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, 0, arm_0);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, 0, arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoderCur_R[4], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoderCur_R[5], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoderCur_R[6], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoderCur_R[7], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoderCur_R[8], arm_0);
            m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoderCur_R[9], arm_0);
        }
    }
    if(enableTag_L == enableAction || enableTag_L == keepEnabling)
    {
        if(m_teleOperationdMode == TeleOperationMode::CSV_Mode)
        {
            m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVelCur_L[0], arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVelCur_L[1], arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVelCur_L[2], arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVelCur_L[3], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoderCur_L[4], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoderCur_L[5], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoderCur_L[6], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoderCur_L[7], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoderCur_L[8], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoderCur_L[9], arm_1);
        }
        else
        {
            m_motorDriver->setTargetVel(MotorType::MOONS, 0, 0, arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, 0, arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, 0, arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, 0, arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoderCur_L[4], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoderCur_L[5], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoderCur_L[6], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoderCur_L[7], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoderCur_L[8], arm_1);
            m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoderCur_L[9], arm_1);
        }
    }
}

void RobotControl::openTorqueSensor()
{
    // m_Torque_Sensor_Serial_422=new QSerialPort();
    // QString name="/dev/ttyUSB6";
    // m_Torque_Sensor_Serial_422->setPortName(name);
    // m_Torque_Sensor_Serial_422->setBaudRate(921600);
    // m_Torque_Sensor_Serial_422->setDataBits(QSerialPort::Data8);
    // m_Torque_Sensor_Serial_422->setParity(QSerialPort::NoParity);
    // m_Torque_Sensor_Serial_422->setStopBits(QSerialPort::OneStop);
    // m_Torque_Sensor_Serial_422->setFlowControl(QSerialPort::NoFlowControl);
    // if (m_Torque_Sensor_Serial_422->open(QIODevice::ReadWrite)) {
    //     LOG(INFO)<<"Torque_Sensor_422 open successful";
    // } else {
    //     LOG(ERROR)<<"Torque_Sensor_422 open fail";
    // }
    // connect(m_Torque_Sensor_Serial_422, &QSerialPort::readyRead, this, &RobotControl::onTorqueSensorDataIn);
}

void RobotControl::closeTorqueSensor()
{
    m_Torque_Sensor_Serial_422->close();
    m_Torque_Sensor_Serial_422->deleteLater();
}

void RobotControl::onTorqueSensorDataIn()
{
    if(m_Torque_Sensor_Serial_422->canReadLine())
    {

        m_Data_Torque_Sensor_Serial_Receved+=m_Torque_Sensor_Serial_422->readAll();
        int len=this->m_Data_Torque_Sensor_Serial_Receved.length();
        if(len>=2){
            uint8_t fh0=m_Data_Torque_Sensor_Serial_Receved.at(0);
            uint8_t fh1=m_Data_Torque_Sensor_Serial_Receved.at(1);
            if(fh0!=0x55||fh1!=0xaa){//is frame head  ok?
                m_Data_Torque_Sensor_Serial_Receved.clear();
                return;
            }

            while(len>=23){
                if((this->m_Data_Torque_Sensor_Serial_Receved.at(21)==0x0D)&&((this->m_Data_Torque_Sensor_Serial_Receved.at(22)==0x0a)))
                {
                    QByteArray datatemp=m_Data_Torque_Sensor_Serial_Receved.left(23);
                    uint32_t Crctemp= CalcCrc16((uint8_t *)(datatemp.data()), 19);

                    uint8_t crchigh=datatemp.at(19);
                    uint8_t crclow=datatemp.at(20);

                    if(Crctemp==(crchigh*256+crclow))
                    {

                        //                       qDebug()<<"CRC OK";
                        uint8_t errorcode=datatemp.at(2);
                        TorqueSensorData torqueSensorData_Left_Temp;
                        torqueSensorData_Left_Temp.ErrorCode=errorcode;
                        if(errorcode==0x00)
                        {
                            torqueSensorData_Left_Temp.TorqueValueSensor1=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,3);
                            torqueSensorData_Left_Temp.TorqueValueSensor2=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,7);
                            torqueSensorData_Left_Temp.TorqueValueSensor3=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,11);
                            torqueSensorData_Left_Temp.TorqueValueSensor4=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,15);
                        }
                        else
                        {
                            torqueSensorData_Left_Temp.TorqueValueSensor1=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor2=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor3=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor4=0;
                        }
                        m_torqueSensorData_Left.store(torqueSensorData_Left_Temp);

                    }
                    //readHandleOtherData(datatemp);
                }
                m_Data_Torque_Sensor_Serial_Receved.remove(0,23);
                len = this->m_Data_Torque_Sensor_Serial_Receved.length();
            }
        }
    }
}

float RobotControl::covertQbytearrayToFloat(QByteArray data, int startindex)
{
    union{
        quint8 byte[4];
        float floatval;
    }converter;
    converter.byte[0]=data.at(startindex+3);
    converter.byte[1]=data.at(startindex+2);
    converter.byte[2]=data.at(startindex+1);
    converter.byte[3]=data.at(startindex);
    return converter.floatval;
}


void RobotControl::setRobotControlMode(const RobotControlMode& tartgetRobotControlMode)
{
    switch(static_cast<int>(tartgetRobotControlMode))
    {
    case static_cast<int>(RobotControlMode::TeleOperation):
    {
        goToTeleOperation();
        break;
    }
    case static_cast<int>(RobotControlMode::Collaboration_GuidingArm):
    {
        goToCollaboration_GuidingArm();
        break;
    }
    case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
    {
        goToCollaboration_EndJoint();
        break;
    }
    case static_cast<int>(RobotControlMode::Hold):
    {
        goToHold();
        break;
    }
    default: break;
    }
}

void RobotControl::goToHold()
{
    auto curRobotControlMode = m_curRobotControlMode.load();//读取当前机器人控制模式

    switch (static_cast<int>(curRobotControlMode))//判断模式
    {
    case static_cast<int>(RobotControlMode::InitMode)://初始化模式
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN INIT STATUS";

        //set operation mode for guiding arm to pv
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 0, arm_guiding);
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 1, arm_guiding);
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 2, arm_guiding);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_0);
        // m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_0);
        // m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_1);
        // m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_1);
        // m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_1);


        std::array<int, MotorNumPerSide> errCode_R = {0};
        std::array<int, MotorNumPerSide> errCode_L = {0};
        errCode_R[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_0);
        errCode_R[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_0);
        errCode_R[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_0);
        errCode_R[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_0);
        errCode_R[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_0);
        errCode_R[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_0);
        errCode_R[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_0);
        errCode_R[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_0);
        // errCode_R[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_0);
        // errCode_R[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_0);

        errCode_L[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_1);
        errCode_L[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_1);
        errCode_L[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_1);
        errCode_L[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_1);
        errCode_L[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_1);
        errCode_L[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_1);
        errCode_L[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_1);
        errCode_L[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_1);
        // errCode_L[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_1);
        // errCode_L[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Motor Status Finish, Error Code Right: " << std::hex <<errCode_R;
        LOG(INFO) << "Set Motor Status Finish, Error Code Left: " << std::hex <<errCode_L;

        std::array<int, MotorNumPerSide> statusWord_R = {0};
        std::array<int, MotorNumPerSide> statusWord_L = {0};
        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_R[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        statusWord_R[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        statusWord_R[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        // statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        // statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        // statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        // statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Motor Status Finish, Status Word Right: " << std::hex <<statusWord_R;
        LOG(INFO) << "Set Motor Status Finish, Status Word Left: " << std::hex <<statusWord_L;

        std::array<int, MotorNumPerSide> modeDisplay_R = {0};
        std::array<int, MotorNumPerSide> modeDisplay_L = {0};
        modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_R[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay_R[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay_R[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay_R[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_R[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_R[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_R[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        // modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        // modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_L[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
        modeDisplay_L[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
        modeDisplay_L[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
        modeDisplay_L[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
        modeDisplay_L[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
        modeDisplay_L[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
        modeDisplay_L[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
        modeDisplay_L[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
        // modeDisplay_L[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        // modeDisplay_L[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Motor Data for Hold Finish, Operation Display Right: " << modeDisplay_R;
        LOG(INFO) << "Set Motor Data for Hold Finish, Operation Display Left: " << modeDisplay_L;
        break;
    }
    case static_cast<int>(RobotControlMode::TeleOperation)://通信模式
    {
        if(m_status_R == 1 || m_status_R == 2){
            m_handlePoseLastLoop_R = m_masterConsole.returnHandlePose();
            m_enableTagPrev_R = keepDisabling;
        }

        if(m_status_L == 1 || m_status_L == 2){
            m_handlePoseLastLoop_L = m_masterConsole.returnHandlePose();
            m_enableTagPrev_L = keepDisabling;
        }

        m_flagInTeleoperation.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));//当前线程暂停 100 毫秒
        LOG(INFO)<<"FROM TeleOperation TO HOLD";

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_0);
        // m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_0);
        // m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_1);
        // m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_1);
        // m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_1);

        std::array<int, MotorNumPerSide> statusWord = {0};
        statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        // statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        // statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);
        LOG(INFO) << "Set Motor Status Finish, Status Word: " << std::hex << statusWord;

        std::array<int, MotorNumPerSide> modeDisplay = {0};
        modeDisplay[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        // modeDisplay[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        // modeDisplay[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);
        LOG(INFO) << "Set Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay;

        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN TELEOPERATION";
        break;

    }

    case static_cast<int>(RobotControlMode::TestOperation)://通信模式
    {
        LOG(INFO)<<"FROM TestOperation TO HOLD";
        m_flagInTeleoperation.store(false);
        m_controlLoopNum = 0;//出范围后，计数为0
        if(m_status_R == 1 || m_status_R == 2){
            // m_handlePoseLastLoop_R = m_handlePose_Cur.load();
            m_handlePoseLastLoop_R = m_masterConsole.returnHandlePose();
            m_status_R = 4;
            //m_last_roll=m_cur_roll;
            m_enableTagPrev_R = keepDisabling;
            LOG(INFO)<<"save the LastLoop! ";
            LOG(INFO)<<"CSV 2 HOLD m_handlePoseLastLoop_R: " << m_handlePoseLastLoop_R.handlePoseR_OpenAngle;
            LOG(INFO)<<"CSV 2 HOLD m_handlePoseLastLoop_L: " << m_handlePoseLastLoop_L.handlePoseL_OpenAngle;
        }

        if(m_status_L == 1 || m_status_L == 2){
            m_handlePoseLastLoop_L = m_masterConsole.returnHandlePose();
            m_status_L = 4;
            //m_last_roll=m_cur_roll;
            m_enableTagPrev_L = keepDisabling;
            LOG(INFO)<<"save the LastLoop! ";
            LOG(INFO)<<"CSV 2 HOLD m_handlePoseLastLoop_R: " << m_handlePoseLastLoop_R.handlePoseR_OpenAngle;
            LOG(INFO)<<"CSV 2 HOLD m_handlePoseLastLoop_L: " << m_handlePoseLastLoop_L.handlePoseL_OpenAngle;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));//当前线程暂停 100 毫秒

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_1);
        m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_1);

        break;

    }
    case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
    {
        m_flagInCollaboration_EndJoint.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);

        std::array<int, MotorNumPerSide> statusWord = {0};
        statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        LOG(INFO) << "InitMotor Finish, Status Word: " << statusWord;
        LOG(INFO) << statusWord;

        std::array<int, MotorNumPerSide> modeDisplay = {0};
        modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: ";
        LOG(INFO) << modeDisplay;

        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN Collaboration";
        break;
    }
    case static_cast<int>(RobotControlMode::Hold):
    {
        LOG(INFO) << "SWITCH TO HOLD MODE, previous Statis is: IN HOLD STAUTS";
        break;
    }
    }

    m_curRobotControlMode.store(RobotControlMode::Hold);//gotohold
    m_flagInHold.store(true);
}

void RobotControl::goToCollaboration_EndJoint()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
    case static_cast<int>(RobotControlMode::TeleOperation):
    {
        LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN TELEOPERATION";
        break;
    }
    case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
    {
        LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN Collaboration_EndJoint";
        break;
    }
    case static_cast<int>(RobotControlMode::Hold):
    {

        m_motorDriver->operationCST(MotorType::ZERO_ERR, 0, arm_0);
        //            m_motorDriver->operationCST(MotorType::ZERO_ERR, 1, arm_0);
        //            m_motorDriver->operationCST(MotorType::ZERO_ERR, 2, arm_0);

        std::array<int, MotorNumPerSide> statusWord = {0};
        statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        //            statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        //            statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        LOG(INFO) << "InitMotor Finish, Status Word: " << statusWord;
        LOG(INFO) << statusWord;

        std::array<int, MotorNumPerSide> modeDisplay = {0};
        modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        //            modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        //            modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: ";
        LOG(INFO) << modeDisplay;

        LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN HOLD STAUTS";
        break;
    }
    }
    m_curRobotControlMode.store(RobotControlMode::Collaboration_EndJoint);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_flagInCollaboration_EndJoint.store(true);
}

void RobotControl::goToCollaboration_GuidingArm()
{
    LOG(INFO) << "In RobotControl::goToCollaboration_GuidingArm()";
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
        case static_cast<int>(RobotControlMode::TeleOperation):
        {
            LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN TELEOPERATION";
            break;
        }
        case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
        {

            LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN Collaboration_EndJoint";
            break;
        }
        case static_cast<int>(RobotControlMode::Hold):
        {

            m_motorDriver->operationCST(MotorType::ZERO_ERR, 0, arm_guiding);
            m_motorDriver->operationCST(MotorType::ZERO_ERR, 1, arm_guiding);
            m_motorDriver->operationCST(MotorType::ZERO_ERR, 2, arm_guiding);

            std::array<int, MotorNumPerSide> statusWord = {0};
            statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_guiding);
            statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_guiding);
            statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_guiding);
            LOG(INFO) << "InitMotor Finish, Status Word: " << statusWord;
            LOG(INFO) << statusWord;

            std::array<int, MotorNumPerSide> modeDisplay = {0};
            modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
            //            modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
            //            modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
            LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: ";
            LOG(INFO) << modeDisplay;

            LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN HOLD STAUTS";
            break;
        }
    }
    m_curRobotControlMode.store(RobotControlMode::Collaboration_EndJoint);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInCollaboration_GuidingArm.store(true);
}

void RobotControl::goToTeleOperation()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
        case static_cast<int>(RobotControlMode::TeleOperation):
        {
            LOG(INFO) << "SWITCH TO TELEOPERATION MODE, previous Statis is: IN TELEOPERATION";

        }
        case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
        {
            LOG(INFO) << "SWITCH TO TELEOPERATION MODE, previous Statis is: IN Collaboration_EndJoint";

        }
        case static_cast<int>(RobotControlMode::Hold):
        {

            m_flagInHold.store(false);
            usleep(10 * 1000);

            LOG(INFO) << "Switch TO Teleoperation Mode, previous Statis is: IN HOLD STAUTS";

            std::array<int, MotorNumPerSide> statusWord_R = {0};
            std::array<int, MotorNumPerSide> statusWord_L = {0};
            statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
            statusWord_R[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
            statusWord_R[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
            statusWord_R[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
            statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
            statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
            statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
            statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
            // statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
            // statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

            statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
            statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
            statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
            statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
            statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
            statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
            statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
            statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
            // statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
            // statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
            usleep (50 * 1000);

            LOG(INFO) << "Go to Teleoperation start, Status Word of Right Arm: " << std::hex << statusWord_R;
            LOG(INFO) << "Go to Teleoperation start, Status Word of Left Arm: " << std::hex << statusWord_L;

            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);
            m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
            m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
            m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
            m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
            // m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
            // m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
            m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_1);
            m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_1);
            m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_1);
            m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_1);
            // m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_1);
            // m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_1);


            std::array<int, MotorNumPerSide> modeDisplay_R = {0};
            std::array<int, MotorNumPerSide> modeDisplay_L = {0};
            modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
            modeDisplay_R[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
            modeDisplay_R[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
            modeDisplay_R[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
            modeDisplay_R[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
            modeDisplay_R[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
            modeDisplay_R[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
            modeDisplay_R[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
            // modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
            // modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

            modeDisplay_L[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
            modeDisplay_L[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
            modeDisplay_L[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
            modeDisplay_L[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
            modeDisplay_L[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
            modeDisplay_L[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
            modeDisplay_L[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
            modeDisplay_L[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
            // modeDisplay_L[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
            // modeDisplay_L[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);
            usleep(50 * 1000);
            LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Right Arm: " << std::hex << modeDisplay_R;
            LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Left Arm: " << std::hex << modeDisplay_L;

            statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
            statusWord_R[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
            statusWord_R[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
            statusWord_R[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
            statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
            statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
            statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
            statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
            // statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
            // statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

            statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
            statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
            statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
            statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
            statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
            statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
            statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
            statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
            // statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
            // statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
            usleep (50 * 1000);

            LOG(INFO) << "Go to Teleoperation Finish, Status Word of Right Arm: " << std::hex << statusWord_R;
            LOG(INFO) << "Go to Teleoperation Finish, Status Word of Left Arm: " << std::hex << statusWord_L;
        }
    }

    m_curRobotControlMode.store(RobotControlMode::TeleOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);

}

int RobotControl::enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char& side)//yu 记录离开使能时的位置，以及回到使能时m_alignmentNumber_R=0
{
    int enableFlag = keepDisabling;
    auto motorTargetEncoder_L = m_motorEncoderCur_L.load();
    auto motorTargetEncoder_R = m_motorEncoderCur_R.load();

    if(side == 'r')
    {
        if(!isPoseRight(masterHandlePose_Cur, 'r'))
        {
            if(m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling)
            {
                enableFlag = keepDisabling;
            }
            if(m_enableTagPrev_R == enableAction || m_enableTagPrev_R == keepEnabling)//pre处于使能状态，此时进入disable
            {
                m_handlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_R = motorTargetEncoder_R;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'r'))
        {
            if((m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling)
                && masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == true)
            {
                enableFlag = enableAction;
                m_handlePoseInit_R = masterHandlePose_Cur;
            }
            else if ((m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
                enableFlag = keepDisabling;
            }
            else if ((m_enableTagPrev_R == enableAction || m_enableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                m_handlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_R = motorTargetEncoder_R;
            }
            else if ((m_enableTagPrev_R == enableAction || m_enableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
        m_enableTagPrev_R = enableFlag;
    }

    else if(side == 'l')
    {
        if(!isPoseRight(masterHandlePose_Cur, 'l')){
            if(m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling)
            {
                enableFlag = keepDisabling;
            }
            if(m_enableTagPrev_L == enableAction || m_enableTagPrev_L == keepEnabling)
            {
                m_handlePoseLastLoop_L = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_L = motorTargetEncoder_L;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'l'))
        {
            if((m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling)
                && masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == true)
            {
                enableFlag = enableAction;
                m_handlePoseInit_L = masterHandlePose_Cur;
            }
            else if ((m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
                enableFlag = keepDisabling;
            }
            else if ((m_enableTagPrev_L == enableAction || m_enableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                m_handlePoseLastLoop_L = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_L = motorTargetEncoder_L;
            }
            else if ((m_enableTagPrev_L == enableAction || m_enableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
    }
    return enableFlag;
}

// Check if the master device is in appropriate work space;
bool RobotControl::isPoseRight(const HandlePose& masterHandlePose_Cur, const char& side) const//yu 且要满足flag_openangle等于true时（false为status=3时openangle太小）
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    // auto handlePoseCurshuru = masterHandlePose_Cur;

    // LOG(INFO)<<" PITCH1: "<<handlePoseCurshuru.handlePoseInSlaveFrameR_Elevation * 180 / M_PI<<"  YAW1: "<<handlePoseCurshuru.handlePoseInSlaveFrameR_Arzimuth * 180 / M_PI<<"    Roll: "<<handlePoseCurshuru.handlePoseInSlaveFrameR_Roll * 180 / M_PI;

    /*右手*/
    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];

    double endEffectorDelta_X_R = (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_R = endEffectorDelta_X_R + endEffectorInit_X_R;//X用磁栅尺限位
    double endEffectorTarget_Y_R = endEffectorDelta_Y_R + endEffectorInit_Y_R;
    double endEffectorTarget_Z_R = endEffectorDelta_Z_R + endEffectorInit_Z_R;

    /*左手*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];

    double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_L = endEffectorDelta_X_L + endEffectorInit_X_L;//X用磁栅尺限位
    double endEffectorTarget_Y_L = endEffectorDelta_Y_L + endEffectorInit_Y_L;
    double endEffectorTarget_Z_L = endEffectorDelta_Z_L + endEffectorInit_Z_L;


    if(m_oprationMode == 1){

     if(side =='l'){

        if (handlePoseCur.handlePoseInSlaveFrameL_Elevation * 180 / M_PI < 75 && handlePoseCur.handlePoseInSlaveFrameL_Elevation * 180 / M_PI > -75){//67

             if (handlePoseCur.handlePoseInSlaveFrameL_Arzimuth * 180 / M_PI < 90 && handlePoseCur.handlePoseInSlaveFrameL_Arzimuth * 180 / M_PI > -90){//87

                 if((masterHandlePose_Cur.handlePoseL_X > -60) && (masterHandlePose_Cur.handlePoseL_X < 60)){

                     if((masterHandlePose_Cur.handlePoseL_Y > -60) && (masterHandlePose_Cur.handlePoseL_Y < 60)){

                         if((masterHandlePose_Cur.handlePoseL_Z > -60) && (masterHandlePose_Cur.handlePoseL_Z < 10)){

                             // if(((endEffectorTarget_Y_L < -150) && (handlePoseCur.handlePoseL_Y - m_handlePosePrev.handlePoseL_Y) < 0) || (endEffectorTarget_Y_L > -150)){

                             //     if(((endEffectorTarget_Y_L > -90) && (handlePoseCur.handlePoseL_Y - m_handlePosePrev.handlePoseL_Y) > 0) || (endEffectorTarget_Y_L < -90)){

                             //         if(((endEffectorTarget_Z_L < -218) && (handlePoseCur.handlePoseL_Z - m_handlePosePrev.handlePoseL_Z) < 0) || (endEffectorTarget_Z_L > -218)){

                             //             if(((endEffectorTarget_Z_L > -158) && (handlePoseCur.handlePoseL_Z - m_handlePosePrev.handlePoseL_Z) > 0) || (endEffectorTarget_Z_L < -158)){

                             return true;
                             //             }
                             //         }
                             //     }
                             // }


                         }
                     }
                }
             }
        }
        else{
            return false;
        }
     }



     if(side =='r'){

        if (handlePoseCur.handlePoseInSlaveFrameR_Elevation * 180 / M_PI < 75  && handlePoseCur.handlePoseInSlaveFrameR_Elevation * 180 / M_PI > -75){

             if (handlePoseCur.handlePoseInSlaveFrameR_Arzimuth * 180 / M_PI < 90 && handlePoseCur.handlePoseInSlaveFrameR_Arzimuth * 180 / M_PI > -90){

                 if((masterHandlePose_Cur.handlePoseR_X > -60) && (masterHandlePose_Cur.handlePoseR_X < 60)){

                     if((masterHandlePose_Cur.handlePoseR_Y > -60) && (masterHandlePose_Cur.handlePoseR_Y < 60)){

                         if((masterHandlePose_Cur.handlePoseR_Z > -60) && (masterHandlePose_Cur.handlePoseR_Z < 10)){

                             // if(((endEffectorTarget_Y_R < 90) && (handlePoseCur.handlePoseR_Y - m_handlePosePrev.handlePoseR_Y) < 0) || (endEffectorTarget_Y_R > 90)){

                                 // if(((endEffectorTarget_Y_R > 150) && (handlePoseCur.handlePoseR_Y - m_handlePosePrev.handlePoseR_Y) > 0) || (endEffectorTarget_Y_R < 150)){

                                 //     if(((endEffectorTarget_Z_R < -218) && (handlePoseCur.handlePoseR_Z - m_handlePosePrev.handlePoseR_Z) < 0) || (endEffectorTarget_Z_R > -218)){

                                 //         if(((endEffectorTarget_Z_R > -158) && (handlePoseCur.handlePoseR_Z - m_handlePosePrev.handlePoseR_Z) > 0) || (endEffectorTarget_Z_R < -158)){

                                             return true;
                             //             }
                             //         }
                             //     }
                             // }


                         }
                     }
                }
             }
        }
        else{
            return false;
        }

     }
    }

    if(m_oprationMode == 2){
        return true;
    }
    usleep(2*1000);
}

bool RobotControl::isPoseMatch(const HandlePose& masterHandlePose_Cur, const char& side) const//yu
{
  if(m_oprationMode == 1)
  {
    if (side == 'l')
    {
         LOG(INFO)<<"masterHandlePose_Cur.handlePoseL_OpenAngle: "<<masterHandlePose_Cur.handlePoseL_OpenAngle;
        if (abs(masterHandlePose_Cur.handlePoseL_OpenAngle - m_handlePoseLastLoop_L.handlePoseL_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}

    }
    if (side == 'r')
    {
        if (abs(masterHandlePose_Cur.handlePoseR_OpenAngle - m_handlePoseLastLoop_R.handlePoseR_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}
    }
  }

  if(m_oprationMode == 2)
  {
       return true;
  }

}

void RobotControl::setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side)//yu
{
    if(side == 'l')
    {
        m_handlePoseInit_L = handlePoseCur;
        m_motorEncoderInit_L = motorPositionCur;
        calculateEndEffectorPosition(handlePoseCur, motorPositionCur,'l');
        m_alignmentNumber_L = 0;

        m_ruckigInputState_L.current_position = {m_endEffectorInitPosition_L[0], m_endEffectorInitPosition_L[1], m_endEffectorInitPosition_L[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_L.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_L.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    }
    if(side == 'r')
    {
        m_handlePoseInit_R = handlePoseCur;
        m_motorEncoderInit_R = motorPositionCur;//需要把右边电机值存储进去
        calculateEndEffectorPosition(handlePoseCur, motorPositionCur,'r');
        m_alignmentNumber_R = 0;

        m_ruckigInputState_R.current_position = {m_endEffectorInitPosition_R[0], m_endEffectorInitPosition_R[1], m_endEffectorInitPosition_R[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_R.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_R.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    }
}

void RobotControl::endJointGoHome(const char& side)//yu
{
    if(side == 'r')
    {
        std::thread calibration([this](){
            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
            usleep(50 * 1000);

            m_motorDriver->setTargetPos(MotorType::MOONS, 0, 120000, arm_0);//回复到的相对位置

            auto homeStatusTmp = m_motorHomingStatus_R.load();
            if(homeStatusTmp[0] == true){
                m_motorDriver->setTargetVel(MotorType::MOONS, 0, -20000, arm_0);//-30000
            }
            usleep(150 * 1000);

            while(true)
            {
                auto homeStatusTmp = m_motorHomingStatus_R.load();
                if(homeStatusTmp[0] != true){ //如果等于0
                    m_motorDriver->setTargetVel(MotorType::MOONS, 0, 0, arm_0);
                    LOG(INFO) << "Found Edge of Gimbal Motor!";//所有电机归位跳出此循环
                    break;
                }
                usleep(2 * 1000);
            }

            LOG(INFO)<<"goToTarget position";
            m_motorDriver->operationPP(MotorType::MOONS, 0, arm_0);//存在setOperationMode将模式改为PP 并发送targetposition作为相对位置
            usleep(50 * 1000);

            zeroErrGoHome('r');

            while(true)
            {
                auto statusWordTmp = m_motorStatusWordCur_R.load();//当状态字(0x6041)的第10位=1的时候说明状态已经切换，即目标位置到达
                std::bitset<32> statusWordInBinary_moons_1(statusWordTmp[0]);//右边Moons电机的状态字
                std::bitset<32> statusWordInBinary_zero_1(statusWordTmp[1]);
                std::bitset<32> statusWordInBinary_zero_2(statusWordTmp[2]);
                std::bitset<32> statusWordInBinary_zero_3(statusWordTmp[3]);
                //                    if(statusWordInBinary_jointE[10] == 1 && statusWordInBinary_jointF[10] == 1 && statusWordInBinary_jointG[10] == 1)
                //                    {
                if(statusWordInBinary_moons_1[10] == 1 && statusWordInBinary_zero_1[10] == 1 && statusWordInBinary_zero_2[10] == 1 && statusWordInBinary_zero_3[10] == 1){//如果状态字第10项为0,就完成了init
                //     m_moonsCaliFinish_R = 1;
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");//目的为与uiinterface通信，reset button
                    LOG(INFO) << "FinishCalibration";
                    break;
                }
                usleep(2 * 1000);
            }
        });
        calibration.detach();
        usleep(20 * 1000);
    }

    if(side == 'l'){
        std::thread calibration([this](){
            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
            usleep(50 * 1000);

            m_motorDriver->setTargetPos(MotorType::MOONS, 0, 120000, arm_1);//回复到的相对位置

            auto homeStatusTmpL = m_motorHomingStatus_L.load();

            if(homeStatusTmpL[0] == true){
                m_motorDriver->setTargetVel(MotorType::MOONS, 0, -20000, arm_1);//-30000
            }
            usleep(150 * 1000);

            while(true)
            {
                auto homeStatusTmpL = m_motorHomingStatus_L.load();
                if(homeStatusTmpL[0] != true){ //如果等于0
                    m_motorDriver->setTargetVel(MotorType::MOONS, 0, 0, arm_1);
                    LOG(INFO) << "Found Edge of Gimbal Motor!";//所有电机归位跳出此循环
                    break;
                }
                usleep(2 * 1000);
            }

            LOG(INFO)<<"goToTarget position";
            m_motorDriver->operationPP(MotorType::MOONS, 0, arm_1);//存在setOperationMode将模式改为PP 并发送targetposition作为相对位置
            usleep(50 * 1000);

            zeroErrGoHome('l');

            while(true)
            {
                auto statusWordTmp = m_motorStatusWordCur_L.load();//当状态字(0x6041)的第10位=1的时候说明状态已经切换，即目标位置到达
                std::bitset<32> statusWordInBinary_moons_1(statusWordTmp[0]);//右边Moons电机的状态字
                std::bitset<32> statusWordInBinary_zero_1(statusWordTmp[1]);
                std::bitset<32> statusWordInBinary_zero_2(statusWordTmp[2]);
                std::bitset<32> statusWordInBinary_zero_3(statusWordTmp[3]);
                // LOG(INFO)<<"statusWordInBinary_zero_1[10]: "<<statusWordInBinary_zero_1[10];
                if(statusWordInBinary_moons_1[10] == 1 && statusWordInBinary_zero_1[10] == 1 && statusWordInBinary_zero_2[10] == 1 && statusWordInBinary_zero_3[10] == 1){//如果状态字第10项为0,就完成了init
                    m_moonsCaliFinish_L = 1;
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");//目的为与uiinterface通信，reset button
                    LOG(INFO) << "FinishCalibration";
                    break;
                }
                usleep(2 * 1000);
            }
        });
        calibration.detach();
        usleep(20 * 1000);
    }
}

void RobotControl::zeroErrGoHome(const char& side)
{
    if(side == 'r')
    {
        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setOperationMode(MotorType::ZERO_ERR, i, OperationMode::PP, arm_0);
        }
        usleep(50 * 1000);

        std::array<int, 3>  operationMode = {0};
        for(int i = 0; i < 3; i++)
        {
            operationMode[i] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, i, arm_0);
        }
        LOG(INFO) << "1. Get Current Operation Mode: " << std::hex << operationMode;

        std::array<int, 3> targetPosition;

        if(m_armAnglePerSide == 30){
            targetPosition = {JointEncoderPerRevolution / 2 + static_cast<int>(45.2336 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 + static_cast<int>(97.0599 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 + static_cast<int>(21.8223 * EncoderPerGrade)};
        }
        if(m_armAnglePerSide == 15){
            targetPosition = {JointEncoderPerRevolution / 2 + static_cast<int>(21.2081 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 + static_cast<int>(97.53 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 + static_cast<int>(61.3219 * EncoderPerGrade)};
        }

        for(int i = 0; i < 3; i ++)
        {
            m_motorDriver->setTargetPos(MotorType::ZERO_ERR, i, targetPosition[i], arm_0);
            m_motorDriver->setProfileVel(MotorType::ZERO_ERR, i, 2550, arm_0);
            m_motorDriver->setProfileAcc(MotorType::ZERO_ERR, i, PROFILE_ACC, arm_0);
            m_motorDriver->setProfileDec(MotorType::ZERO_ERR, i, PROFILE_DEC, arm_0);
            usleep(5 * 1000);
        }
        usleep(100 * 1000);

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SHUT_DOWN, arm_0);
            usleep(5 * 1000);
        }
        usleep(400 * 1000);
        LOG(INFO) << "2. Motor Shut Down, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SWITCH_ON, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "3. Motor Switch On, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ENABLE, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "4. Motor Enable, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_TRI_PP, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "5. Motor Enable Trigger, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_IM_TRI_PP, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "6. Motor Enable Trigger Immediately Motion, Current Status: " << std::hex << m_motorStatusWordCur_R.load();
    }
    else if(side == 'l')
    {
        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setOperationMode(MotorType::ZERO_ERR, i, OperationMode::PP, arm_1);
        }
        usleep(50 * 1000);

        std::array<int, 3>  operationMode = {0};
        for(int i = 0; i < 3; i++)
        {
            operationMode[i] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, i, arm_1);
        }
        LOG(INFO) << "1. Get Current Operation Mode: " << std::hex << operationMode;

        std::array<int, 3> targetPosition;

        if(m_armAnglePerSide == 30){
            targetPosition = {JointEncoderPerRevolution / 2 - static_cast<int>(45.2336 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 - static_cast<int>(97.0599 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 - static_cast<int>(21.8223 * EncoderPerGrade)};
        }
        if(m_armAnglePerSide == 15){
            targetPosition = {JointEncoderPerRevolution / 2 - static_cast<int>(21.2081 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 - static_cast<int>(97.53 * EncoderPerGrade),
                              JointEncoderPerRevolution / 2 - static_cast<int>(61.3219 * EncoderPerGrade)};
        }

        for(int i = 0; i < 3; i ++)
        {
            m_motorDriver->setTargetPos(MotorType::ZERO_ERR, i, targetPosition[i], arm_1);
            m_motorDriver->setProfileVel(MotorType::ZERO_ERR, i, 2550, arm_1);
            m_motorDriver->setProfileAcc(MotorType::ZERO_ERR, i, PROFILE_ACC, arm_1);
            m_motorDriver->setProfileDec(MotorType::ZERO_ERR, i, PROFILE_DEC, arm_1);
            usleep(5 * 1000);
        }
        usleep(100 * 1000);

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SHUT_DOWN, arm_1);
            usleep(5 * 1000);
        }
        usleep(400 * 1000);
        LOG(INFO) << "2. Motor Shut Down, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SWITCH_ON, arm_1);
        }
        usleep(50 * 1000);
        LOG(INFO) << "3. Motor Switch On, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ENABLE, arm_1);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "4. Motor Enable, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_TRI_PP, arm_1);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "5. Motor Enable Trigger, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_IM_TRI_PP, arm_1);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "6. Motor Enable Trigger Immediately Motion, Current Status: " << std::hex << m_motorStatusWordCur_L.load();
    }
}

void RobotControl::setNewSpeed(const HandlePose& handlePosePrev, const HandlePose& handlePoseCur)
{
    QList<QString> arglist;
    if(handlePoseCur.stepPedal == pedalAcceleration && handlePosePrev.stepPedal == pedalNoAction)
    {
        if(m_speedPedalIndex_Cur < pedalSwitchNumber - 1)
        {
            m_speedPedalIndex_Cur++;
            LOG(INFO)<<"Acceleration to Level "<< m_speedPedalIndex_Cur + 1;
            arglist.append("speedCur:"+QString::number(m_speedPedalIndex_Cur));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvMasterData),arglist);
        }
        else
        {
            LOG(INFO)<<"Already reached highest Motion-Scaling factor";
        }
    }
    if(handlePoseCur.stepPedal == pedalDeceleration && handlePosePrev.stepPedal == pedalNoAction)
    {
        if(m_speedPedalIndex_Cur > 0)
        {
            m_speedPedalIndex_Cur--;
            LOG(INFO)<<"Deceleration to Level "<< m_speedPedalIndex_Cur + 1;
            arglist.append("speedCur:"+QString::number(m_speedPedalIndex_Cur));
            SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvMasterData),arglist);
        }
        else
        {
            LOG(INFO)<<"Already reached lowest Motion-Scaling factor";
        }
    }
}

void RobotControl::MaxonGoHome(const char& side)//yu
{
    if(side == 'r'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_R = {0};
        m_handlePoseLastLoop_R.initOrg_R();

        std::thread calibration([this](){
            m_motorDriver->operationHOME(MotorType::MAXON, 0, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 1, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 2, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 3, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 4, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 5, arm_0);

            LOG(INFO) << "Start right Maxon homing! ";
            while(true)
            {
                auto homeStatusTmp = m_motorHomingStatus_R.load();
                if(homeStatusTmp[4] == false &&
                    homeStatusTmp[5] == false &&
                    homeStatusTmp[6] == false &&
                    homeStatusTmp[7] == false &&
                    homeStatusTmp[8] == false &&
                    homeStatusTmp[9] == false)
                {
                    m_maxonCaliFinish_R = 1;
                    // SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");
                    LOG(INFO) << "Finish homing of endeffector motor Right!";
                    break;
                }
                usleep(20 * 1000);
            }

            //                    usleep(8000*1000);


            //                    auto torqueData = m_torqueSensorData_Left.load();

            //                    usleep(20 * 1000);
            //                    float torqueInit1 = torqueData.TorqueValueSensor1;
            //                    float torqueInit2 = torqueData.TorqueValueSensor2;
            //                    float torqueInit3 = torqueData.TorqueValueSensor3;
            //                    float torqueInit4 = torqueData.TorqueValueSensor4;


            //                    LOG(INFO)<<"torque init3: "<<torqueInit1;
            //                    LOG(INFO)<<"torque init4: "<<torqueInit2;
            //                    LOG(INFO)<<"torque init5: "<<torqueInit3;
            //                    LOG(INFO)<<"torque init6: "<<torqueInit4;

            //                    float torqueCur1;
            //                    float torqueCur2;
            //                    float torqueCur3;
            //                    float torqueCur4;

            //                    m_motorDriver->operationCSV(MotorType::MAXON, 2, arm_0);
            //                    m_motorDriver->operationCSV(MotorType::MAXON, 3, arm_0);
            //                    m_motorDriver->operationCSV(MotorType::MAXON, 4, arm_0);
            //                    m_motorDriver->operationCSV(MotorType::MAXON, 5, arm_0);
            //                    usleep(20 * 1000);
            //                    m_motorDriver->setTargetVel(MotorType::MAXON, 2, -10, arm_0);
            //                    m_motorDriver->setTargetVel(MotorType::MAXON, 3, 10, arm_0);
            //                    m_motorDriver->setTargetVel(MotorType::MAXON, 4, 10, arm_0);
            //                    m_motorDriver->setTargetVel(MotorType::MAXON, 5, -10, arm_0);

            //                    while(true)
            //                    {
            //                        torqueData = m_torqueSensorData_Left.load();
            //                        torqueCur1 = torqueData.TorqueValueSensor1 - torqueInit1;
            //                        torqueCur2 = torqueData.TorqueValueSensor2 - torqueInit2;
            //                        torqueCur3 = torqueData.TorqueValueSensor3 - torqueInit3;
            //                        torqueCur4 = torqueData.TorqueValueSensor4 - torqueInit4;

            //                        LOG(INFO)<<"torqueCur3: "<<torqueCur1;
            //                        LOG(INFO)<<"torqueCur4: "<<torqueCur2;
            //                        LOG(INFO)<<"torqueCur5: "<<torqueCur3;
            //                        LOG(INFO)<<"torqueCur6: "<<torqueCur4;

            //                         LOG(INFO)<<"torque3: "<<torqueData.TorqueValueSensor1;
            //                         LOG(INFO)<<"torque4: "<<torqueData.TorqueValueSensor2;
            //                         LOG(INFO)<<"torque5: "<<torqueData.TorqueValueSensor3;
            //                         LOG(INFO)<<"torque6: "<<torqueData.TorqueValueSensor4;

            //                        //读取扭矩传感器数值
            ////                        if(true){//如果任意一个力传感器大于校准目标值
            //                        if(torqueCur1 < -0.015 || torqueCur2 > 0.015){
            //                            m_motorDriver->setTargetVel(MotorType::MAXON, 2, 0, arm_0);
            //                            m_motorDriver->setTargetVel(MotorType::MAXON, 3, 0, arm_0);
            //                            LOG(INFO)<<"1111111111111111111111111111111111111111////////////////////////////////////////////////////////";
            //                        }
            ////                        if(torqueCur2 < -0.04 || torqueCur2 > 0.04){
            ////                            m_motorDriver->setTargetVel(MotorType::MAXON, 3, 0, arm_0);
            ////                        }
            //                        if(torqueCur3 < -0.015 || torqueCur3 > 0.015){
            //                            m_motorDriver->setTargetVel(MotorType::MAXON, 4, 0, arm_0);
            //                            m_motorDriver->setTargetVel(MotorType::MAXON, 5, 0, arm_0);
            //                            LOG(INFO)<<"2222222222222222222222222222222222222222////////////////////////////////////////////////////////";
            //                        }
            ////                        if(torqueCur4 < -0.04 || torqueCur3 > 0.04){
            ////                            m_motorDriver->setTargetVel(MotorType::MAXON, 5, 0, arm_0);
            ////                        }
            ////                        if(true){
            ////                             LOG(INFO) << "Finish Maxon Motor calibration!";//所有电机归位跳出此循环
            ////                        break;
            ////                        }
            //                        usleep(5 * 1000);
            //                     }
            //                    //记录此时的力大小
            //                    auto Motorencode = m_motorEncoderCur_L.load();
            //                    m_maxonInit[0] = Motorencode[4];
            //                    m_maxonInit[1] = Motorencode[5];
            //                    m_maxonInit[2] = Motorencode[6];
            //                    m_maxonInit[3] = Motorencode[7];
            //                    m_maxonInit[4] = Motorencode[8];
            //                    m_maxonInit[5] = Motorencode[9];

        });
        calibration.detach();
        usleep(20 * 1000);
    }


    if(side=='l'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_L = {0};
        m_handlePoseLastLoop_L.initOrg_L();

        std::thread calibration([this](){
            m_motorDriver->operationHOME(MotorType::MAXON, 0, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 1, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 2, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 3, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 4, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 5, arm_1);

            LOG(INFO) << "Start left Maxon homing! ";
            while(true)
            {

                auto homeStatusTmp = m_motorHomingStatus_L.load();
                LOG(INFO) << " m_motorHomingStatus_L: " << homeStatusTmp;

                if(homeStatusTmp[4] == false &&
                    homeStatusTmp[5] == false &&
                    homeStatusTmp[6] == false &&
                    homeStatusTmp[7] == false &&
                    homeStatusTmp[8] == false &&
                    homeStatusTmp[9] == false)
                {
                    m_maxonCaliFinish_L = 1;
                    LOG(INFO) << "Finish homing of endeffector motor Left!";
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");
                    break;
                }
                usleep(20 * 1000);
            }

        });
        calibration.detach();
        usleep(20 * 1000);
    }

}

void RobotControl::MaxonGoHome_4Maxons(const char& side)//yu
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    if(side == 'r'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_R = {0};
        m_handlePoseLastLoop_R.initOrg_R();
        m_handlePoseOrg_R.initOrg_R();
        Eigen::Matrix3d MasterRotationMatrix_x;
        Eigen::Matrix3d MasterRotationMatrix_y;
        Eigen::Matrix3d MasterRotationMatrix_z;
        Eigen::Matrix3d MasterRotationMatrix_total;

        double rotation_angle_Y = 0;
        double rotation_angle_X = 0;
        double rotation_angle_Z = -m_armAnglePerSide;

        MasterRotationMatrix_y(0,0) = cos(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(0,1) = 0;
        MasterRotationMatrix_y(0,2) = sin(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(1,0) = 0;
        MasterRotationMatrix_y(1,1) = 1;
        MasterRotationMatrix_y(1,2) = 0;
        MasterRotationMatrix_y(2,0) = -sin(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(2,1) = 0;
        MasterRotationMatrix_y(2,2) = cos(rotation_angle_Y * 3.1415926 / 180.0);

        MasterRotationMatrix_x(0,0) = 1;
        MasterRotationMatrix_x(0,1) = 0;
        MasterRotationMatrix_x(0,2) = 0;
        MasterRotationMatrix_x(1,0) = 0;
        MasterRotationMatrix_x(1,1) = cos(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(1,2) = sin(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(2,0) = 0;
        MasterRotationMatrix_x(2,1) = sin(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(2,2) = cos(rotation_angle_X * 3.1415926 / 180.0);

        MasterRotationMatrix_z(0,0) = cos(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(0,1) = -sin(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(0,2) = 0;
        MasterRotationMatrix_z(1,0) = sin(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(1,1) = cos(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(1,2) = 0;
        MasterRotationMatrix_z(2,0) = 0;
        MasterRotationMatrix_z(2,1) = 0;
        MasterRotationMatrix_z(2,2) = 1;

        MasterRotationMatrix_total = MasterRotationMatrix_y * MasterRotationMatrix_z * MasterRotationMatrix_x;

        // m_handlePoseLastLoop_R.setRotationDataR(MasterRotationMatrix_total);
        // m_handlePoseOrg_R.setRotationDataR(MasterRotationMatrix_total);

        // m_handlePoseLastLoop_R.handlePoseR_OpenAngle = 0;

        std::thread calibration([this](){
            m_motorDriver->operationHOME(MotorType::MAXON, 0, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 1, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 2, arm_0);
            m_motorDriver->operationHOME(MotorType::MAXON, 3, arm_0);

            LOG(INFO) << "Start right Maxon homing! ";
            while(true)
            {
                auto homeStatusTmp = m_motorHomingStatus_R.load();
                // LOG(INFO) << "homeStatusTmpR: " << homeStatusTmp;

                if(homeStatusTmp[4] == false &&
                    homeStatusTmp[5] == false &&
                    homeStatusTmp[6] == false &&
                    homeStatusTmp[7] == false)
                {
                    m_maxonCaliFinish_R = 1;
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");
                    LOG(INFO) << "Finish homing of endeffector motor Right!";
                    break;
                }
                usleep(20 * 1000);
            }

        });
        calibration.detach();
        usleep(20 * 1000);
    }


    if(side=='l'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_L = {0};
        m_handlePoseLastLoop_L.initOrg_L();
        m_handlePoseOrg_L.initOrg_R();
        Eigen::Matrix3d MasterRotationMatrix_x;
        Eigen::Matrix3d MasterRotationMatrix_y;
        Eigen::Matrix3d MasterRotationMatrix_z;
        Eigen::Matrix3d MasterRotationMatrix_total;

        double rotation_angle_Y = 0;
        double rotation_angle_X = 0;
        double rotation_angle_Z = m_armAnglePerSide;

        MasterRotationMatrix_y(0,0) = cos(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(0,1) = 0;
        MasterRotationMatrix_y(0,2) = sin(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(1,0) = 0;
        MasterRotationMatrix_y(1,1) = 1;
        MasterRotationMatrix_y(1,2) = 0;
        MasterRotationMatrix_y(2,0) = -sin(rotation_angle_Y * 3.1415926 / 180.0);
        MasterRotationMatrix_y(2,1) = 0;
        MasterRotationMatrix_y(2,2) = cos(rotation_angle_Y * 3.1415926 / 180.0);

        MasterRotationMatrix_x(0,0) = 1;
        MasterRotationMatrix_x(0,1) = 0;
        MasterRotationMatrix_x(0,2) = 0;
        MasterRotationMatrix_x(1,0) = 0;
        MasterRotationMatrix_x(1,1) = cos(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(1,2) = sin(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(2,0) = 0;
        MasterRotationMatrix_x(2,1) = sin(rotation_angle_X * 3.1415926 / 180.0);
        MasterRotationMatrix_x(2,2) = cos(rotation_angle_X * 3.1415926 / 180.0);

        MasterRotationMatrix_z(0,0) = cos(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(0,1) = -sin(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(0,2) = 0;
        MasterRotationMatrix_z(1,0) = sin(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(1,1) = cos(rotation_angle_Z * 3.1415926 / 180.0);
        MasterRotationMatrix_z(1,2) = 0;
        MasterRotationMatrix_z(2,0) = 0;
        MasterRotationMatrix_z(2,1) = 0;
        MasterRotationMatrix_z(2,2) = 1;

        MasterRotationMatrix_total = MasterRotationMatrix_y * MasterRotationMatrix_z * MasterRotationMatrix_x;

        // m_handlePoseLastLoop_L.setRotationDataL(MasterRotationMatrix_total);
        // m_handlePoseOrg_L.setRotationDataL(MasterRotationMatrix_total);

        // m_handlePoseLastLoop_L = handlePoseCur;
        // m_handlePoseOrg_L = handlePoseCur;
        // m_handlePoseLastLoop_L.handlePoseL_OpenAngle = 0;

        std::thread calibration([this](){
            m_motorDriver->operationHOME(MotorType::MAXON, 0, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 1, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 2, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 3, arm_1);

            LOG(INFO) << "Start left Maxon homing! ";
            while(true)
            {

                auto homeStatusTmp = m_motorHomingStatus_L.load();
                // LOG(INFO) << " m_motorHomingStatus_L: " << homeStatusTmp;

                if(homeStatusTmp[4] == false &&
                    homeStatusTmp[5] == false &&
                    homeStatusTmp[6] == false &&
                    homeStatusTmp[7] == false)
                {
                    m_maxonCaliFinish_L = 1;
                    LOG(INFO) << "Finish homing of endeffector motor Left!";
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");
                    break;
                }
                usleep(20 * 1000);
            }

        });
        calibration.detach();
        usleep(20 * 1000);
    }

}

void RobotControl::storeCurAsPrev(const HandlePose& handlePoseCur,
                                  const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                  const std::array<int, MotorNumPerSide>& motorPositionCur_L, const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                  const int&  enableTagCur_L, const int&  enableTagCur_R )
{
    m_handlePosePrev = handlePoseCur;
    m_controlValuePrev_L = controlValueCur_L;
    m_controlValuePrev_R = controlValueCur_R;
    m_motorPositionPrev_L = motorPositionCur_L;
    m_motorPositionPrev_R = motorPositionCur_R;
    m_speedPedalIndex_Prev = m_speedPedalIndex_Cur;
    if ((enableTagCur_L == enableAction||enableTagCur_L == keepEnabling) && (m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling))
    {
        m_handlePoseLastLoop_L.handlePoseL_OpenAngle = handlePoseCur.handlePoseL_OpenAngle;
    }
    if ((enableTagCur_R == enableAction||enableTagCur_R == keepEnabling) && (m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling))
    {
        m_handlePoseLastLoop_R.handlePoseR_OpenAngle = handlePoseCur.handlePoseR_OpenAngle;
    }
    m_enableTagPrev_L = enableTagCur_L;
    m_enableTagPrev_R = enableTagCur_R;
}

double RobotControl::calculateOverlapValue(const std::array<int, MotorNumPerSide>& motorPosition_Cur,
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
            overlapValue_Tmp = 0.5 * 2 * openAngle_Cur * openAngle_Cur;//一次函数会出现一开始升高太快，调用二次函数会先慢后快
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
            overlapValue_Tmp = 0.5 * 2 * openAngle_Cur * openAngle_Cur;
        }
    }
    return abs(overlapValue_Tmp);
}

void RobotControl::SendInnerMsg(Module_Inner_E recever,int Action,QString arg)
{
    Message_Inner_T msgTemp;//实例化一条命令
    msgTemp.Sender = Module_Inner_E::RobotControl;
    msgTemp.Recver = recever;//此命令的接收者
    msgTemp.Request.insert(Action,arg);//action，l or r
    m_messagePool.SendMessage(msgTemp);//发送此命令
}

void RobotControl::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::RobotControl;
    msgTemp.Recver = recever;
    QString stringste;

    for(int i=0;i<arglist.length();i++)
    {
        stringste += arglist[i];
        stringste += ";";
    }
    msgTemp.Request.insert(Action, stringste);
    m_messagePool.SendMessage(msgTemp);
}


void RobotControl::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}

void RobotControl::dealWithMsg()
{
    Message_Inner_T msg;

    while(1){

        m_MsgGottenRWLock.lockForRead();

        if(m_MsgGottenQueue.isEmpty() == false)
        {
            msg = m_MsgGottenQueue.dequeue();
        }
        else
        {
            m_MsgGottenRWLock.unlock();
            break;
        }
        m_MsgGottenRWLock.unlock();

        if(msg.Recver != Module_Inner_E::RobotControl && msg.Recver != Module_Inner_E::MultipleModules){return;}

        QHash<int,QString>::ConstIterator i = msg.Request.constBegin();

        while(i != msg.Request.constEnd())
        {
            switch (i.key())//检测到按键点击
            {
            case static_cast<int>(RobotControlAction_E::GoToHoldOnMode):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Motor Hold On" ;
                setRobotControlMode(RobotControlMode::Hold);
                // m_oprationMode = 0;
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToTeleOperationMode):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To TeleOperation";
                setRobotControlMode(RobotControlMode::TeleOperation);
                // m_oprationMode = 1;

                if((m_maxonCaliFinish_R==1)&&(m_maxonCaliFinish_L==1)&&(m_moonsCaliFinish_L==1)&&(m_moonsCaliFinish_R==1)){
                // setRobotControlMode(RobotControlMode::TestOperation);
                }
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToCollaborationMode):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To Collaboration";
                // setRobotControlMode(RobotControlMode::Collaboration);
                // m_oprationMode = 2;
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToGuidingArmCollaboration):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To  Guiding Arm Collaboration";
                initGuidingArm();
                break;
            }
            case static_cast<int>(RobotControlAction_E::DisableGuidingArmCollaboration):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Disable Guiding Arm Collaboration";
                // setRobotControlMode(RobotControlMode::Collaboration);
                disableGuidingArm();
                break;
            }


            case static_cast<int>(RobotControlAction_E::StartEndJointMotorHoming)://
            {
                char side = i.value().toUtf8().data()[0];//将 QString 转换为 char
                if(side == 'l')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Left End Joint Motor Homing!";
                    endJointGoHome('l');
                }
                if(side == 'r')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right End Joint Motor Homing!";
                    endJointGoHome('r');
                }
                break;
            }

            case static_cast<int>(RobotControlAction_E::StartEndEffectorMotorHoming):{
                char side = i.value().toUtf8().data()[0];
                if(side == 'r')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right Instrument Homing!";
                    // MaxonGoHome('r');
                    MaxonGoHome_4Maxons('r');
                }
                if(side == 'l')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right Instrument Homing!";
                    // MaxonGoHome('l');
                    MaxonGoHome_4Maxons('l');
                }
                break;
            }
            case static_cast<int>(RobotControlAction_E::SwitchInstrumentAngle):
            {

                if(i.value() == "30")
                {
                    LOG(INFO) << "Instrument Angle : 30";
                    m_armAnglePerSide = 15;
                }else if(i.value() ==  "60")
                {
                    LOG(INFO) << "Instrument Angle : 60";
                    m_armAnglePerSide = 30;
                }
                break;
            }

            case static_cast<int>(RobotControlAction_E::MotorOff):
            {
                m_isSystemTerminated.store(true);
                usleep(50 * 1000);
                m_motorDriver->disableAllMotors();
                SendInnerMsg(Module_Inner_E::Security, static_cast<int>(SecurityAction_E::RecvMotorDriverShutDown),"Ok");
            }
            default:break;
            }
            i++;
        }
    }
}
