#include "RobotControl.h"

std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTime;
std::chrono::duration<double, std::micro>   loopTime;
std::ofstream outfile1("data_0721.txt");

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
                const toml::array& Arr_Tmp = *(RobotData["MasterDevice"]["MotionScaling"]["Default"].as_array());/*解析toml文件，读取主手的放缩比*/
                std::vector<int> motionScalingTmp;
                for(auto& element : Arr_Tmp) {motionScalingTmp.push_back(static_cast<int64_t>(*(element.as_integer())));}
                for(int i = 0; i < motionScalingTmp.size();i++) {m_motionScaling[i] = motionScalingTmp[i];}
                LOG(INFO)<< "MotionScaling: " << m_motionScaling[0] << " " << m_motionScaling[1] << " " << m_motionScaling[2] << " " << m_motionScaling[3] << " " << m_motionScaling[4];
            }
        }
        /* Left EndEffector Information */
        {
            /* Left Endeffector Initial Rotation Around Y and X */
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
    /*Read Instrument Information: Type, Size, ID*/
    std::vector<std::string> t_endEffectorInfoSplit_L, t_endEffectorInfoSplit_R;
    t_endEffectorInfoSplit_L = split(m_endEffectorLeft,"_");/*用_把需要的参数划分开，比如说 类型_尺寸_ID*/
    t_endEffectorInfoSplit_R = split(m_endEffectorRight,"_");

    LOG(INFO) << "________________READ MY INSTRUMENT DATA_________________";
    LOG(INFO) << "Left Instrument Information: " << m_endEffectorLeft;
    LOG(INFO) << "Right Instrument Information: " << m_endEffectorRight;

    LOG(INFO)<<"m_endeffectorConfiguration: " <<static_cast<int>(m_endeffectorConfiguration);

    // LOG(INFO)<<"m_endeffectorConfiguration_L: " <<static_cast<int>(m_endeffectorConfiguration_L);
    // LOG(INFO)<<"m_endeffectorConfiguration_R: " <<static_cast<int>(m_endeffectorConfiguration_R);

    try
    {
        /*Load Left Endeffector Information*/
        toml::table endEffectorData = toml::parse_file(m_configFilePath);//把toml文件复制到endEffectorData上
        {
            /*Left Side*/
            {
                /*Instrument Data*/
                if(t_endEffectorInfoSplit_L[InstrumentType] != "None")
                {
                    {
                        /*Encoder per Degree*/
                        const toml::array& Arr_Tmp = *(endEffectorData["Instrument"]
                                                                      [t_endEffectorInfoSplit_L[InstrumentType]]
                                                                      [t_endEffectorInfoSplit_L[InstrumentSize]]
                                                                      [t_endEffectorInfoSplit_L[InstrumentID]]
                                                                      ["EncoderPerDegree"]["Value"].as_array());
                        std::vector<double> encoderPerDegreeL;
                        encoderPerDegreeL.clear();

                        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
                            for(auto& element : Arr_Tmp) {encoderPerDegreeL.push_back(static_cast<double>(*(element.as_floating_point())));}
                            for(int i = 0; i < encoderPerDegreeL.size();i++){m_encoderPerDegree_L[i] = encoderPerDegreeL[i];}
                            LOG(INFO) << "Load EncoderPerDegree Left: " << m_encoderPerDegree_L[0] << " " << m_encoderPerDegree_L[1] << " " << m_encoderPerDegree_L[2] << " " << m_encoderPerDegree_L[3];
                        }

                        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
                            for(auto& element : Arr_Tmp) {encoderPerDegreeL.push_back(static_cast<double>(*(element.as_floating_point())));}
                            for(int i = 0; i < encoderPerDegreeL.size();i++){m_encoderPerDegree_L[i] = encoderPerDegreeL[i];}
                            LOG(INFO) << "Load EncoderPerDegree Left: " << m_encoderPerDegree_L[0] << " " << m_encoderPerDegree_L[1] << " " << m_encoderPerDegree_L[2] << " " << m_encoderPerDegree_L[3]<< " " << m_encoderPerDegree_L[4] << " " << m_encoderPerDegree_L[5];
                        }

                        /*Cable Compensation Ratio*/
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

                        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
                            for(auto& element : Arr_Tmp) {encoderPerDegreeR.push_back(static_cast<double>(*(element.as_floating_point())));}
                            for(int i = 0; i < encoderPerDegreeR.size();i++){m_encoderPerDegree_R[i] = encoderPerDegreeR[i];}
                            LOG(INFO) << "Load EncoderPerDegree Left: " << m_encoderPerDegree_R[0] << " " << m_encoderPerDegree_R[1] << " " << m_encoderPerDegree_R[2] << " " << m_encoderPerDegree_R[3]<< " " << m_encoderPerDegree_R[4] << " " << m_encoderPerDegree_R[5];
                        }

                        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
                            for(auto& element : Arr_Tmp) {encoderPerDegreeR.push_back(static_cast<double>(*(element.as_floating_point())));}
                            for(int i = 0; i < encoderPerDegreeR.size();i++){m_encoderPerDegree_R[i] = encoderPerDegreeR[i];}
                            LOG(INFO) << "Load EncoderPerDegree Right: " << m_encoderPerDegree_R[0] << " " << m_encoderPerDegree_R[1] << " " << m_encoderPerDegree_R[2] << " " << m_encoderPerDegree_R[3];
                        }

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

        m_ruckigInputState_R.max_velocity = {200000.0, 200000.0, 200000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {8000.0, 8000.0, 8000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {2500.0, 2500.0, 2500.0};//8000.0

        m_ruckigInputState_L.max_velocity = {200000.0, 200000.0, 200000.0};
        m_ruckigInputState_L.max_acceleration = {8000.0, 8000.0, 8000.0};
        m_ruckigInputState_L.max_jerk = {2500.0, 2500.0, 2500.0};

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

    while(m_flagControlThread && !m_isSystemTerminated)
    {
        startTime = std::chrono::high_resolution_clock::now();
        receiveMotorData();

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
            updateGuidingArmMotion();
            updownMotion();

            updateGuidingArmState();

            switch(m_guidingArm.m_guidingArmCurrentState){
            case GuidingArmState::HOLD:
                applyGuidingArmVelocityControl();
                break;
            case GuidingArmState::DAMPING:
                applyGuidingArmDampingControl();
                break;
            case GuidingArmState::DRAG:
                applyGuidingArmForceControl();
                break;
            }
        }
        std::this_thread::sleep_until(startTime + std::chrono::milliseconds(5));
    }
}

void RobotControl::teleoperation()
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    auto handlePosePrev = m_handlePosePrev;
    // auto handlePoseInit_R = m_handlePoseInit_R;
    // auto handlePoseInit_L = m_handlePoseInit_L;

    auto motorEncoderCur_R = m_motorEncoderCur_R.load();
    auto motorEncoderCur_L = m_motorEncoderCur_L.load();
    auto motorEncoderInit_R = m_motorEncoderInit_R;
    auto motorEncoderInit_L = m_motorEncoderInit_L;

    std::array<int, MotorNumPerSide>   targetEncoder_R = {0};
    std::array<int, MotorNumPerSide>   targetEncoder_L = {0};

    std::array<double, MotorNumPerSide>   targetEncoder_R_forVelocity = {0};
    std::array<double, MotorNumPerSide>   targetEncoder_L_forVelocity = {0};

    std::array<int, MotorNumPerSide>   targetEncoderPrev_R = m_motorTargetEncoderPrev_R;
    std::array<int, MotorNumPerSide>   targetEncoderPrev_L = m_motorTargetEncoderPrev_L;

    std::array<double, MotorNumPerSide>   targetEncoderPrev_R_new = m_motorTargetEncoderPrev_R_new;
    std::array<double, MotorNumPerSide>   targetEncoderPrev_L_new = m_motorTargetEncoderPrev_L_new;

    // std::array<int, MotorNumPerSide>   targetVelocity_R = {0};
    // std::array<int, MotorNumPerSide>   targetVelocity_L = {0};

    std::array<int, MotorNumPerSide>   targetVelocity_R_new = {0};
    std::array<int, MotorNumPerSide>   targetVelocity_L_new = {0};

    std::array<double, ControlValueNum> controlValueCur_R = {0};
    std::array<double, ControlValueNum> controlValueCur_L = {0};

    // LOG(INFO) << "openAngle_L: " << handlePoseCur.handlePoseL_OpenAngle << " x: " << handlePoseCur.handlePoseL_X << " stepPedal: " << handlePoseCur.stepPedal;

    auto forceValue_R = m_force_L.load();
    auto forceValue_L = m_force_R.load();

    int enableTagCur_L = enableCase_KeepPressPedal(handlePoseCur, 'l');
    int enableTagCur_R = enableCase_KeepPressPedal(handlePoseCur, 'r');
    // LOG(INFO)<<"enableTagCur_L: "<<enableTagCur_L<<" enableTagCur_R: "<<enableTagCur_R;

    m_enableTagCur_L = enableTagCur_L;
    m_enableTagCur_R = enableTagCur_R;

    // LOG(INFO)<<" enableTagCur_L: "<<enableTagCur_L<<" enableTagCur_R: "<<enableTagCur_R;

    /* Define the Motion Scaling and return Current Speed Index: {1,2,3,4} */
    setNewSpeed(handlePosePrev, handlePoseCur);

    // TODO: do in force drag
    if(enableTagCur_L == enableAction||enableTagCur_L == keepEnabling)
    {
        if(enableTagCur_L == enableAction)
        {

            motorEncoderInit_L = motorEncoderCur_L;

            m_alignmentNumber_L = 0;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_L, handlePoseCur, 'l');
        }

        if(m_oprationMode == 1){

        controlValueCur_L = motionMapping_L(handlePoseCur,motorEncoderCur_L);
        }

        else{

        controlValueCur_L = test_motionMapping_L(handlePoseCur,motorEncoderCur_L);
        }

        targetEncoder_L = calculateTargetEncoder(controlValueCur_L, motorEncoderInit_L, 'l');

        targetEncoder_L_forVelocity = calculateTargetEncoder_new(controlValueCur_L, motorEncoderInit_L, 'l');

        targetVelocity_L_new = calculateTargetVelocity_new(targetEncoder_L_forVelocity, targetEncoderPrev_L_new, targetEncoder_L, motorEncoderCur_L, 'l');

        // LOG(INFO)<<std::dec<<"delt_targetEncoder_L[0]: "<<targetEncoder_L_forVelocity[0] - motorEncoderCur_L[0]<<" delt_targetEncoder_L[1]: "<<targetEncoder_L_forVelocity[1] - motorEncoderCur_L[1]<<" delt_targetEncoder_L[2]: "<<targetEncoder_L_forVelocity[2] - motorEncoderCur_L[2]<<" delt_targetEncoder_L[3]: "<<targetEncoder_L_forVelocity[3] - motorEncoderCur_L[3];

        // LOG(INFO)<<std::dec<<"targetEncoder_L[0]: "<<targetEncoder_L[0]<<" targetEncoder_L[1]: "<<targetEncoder_L[1]<<" targetEncoder_L[2]: "<<targetEncoder_L[2]<<" targetEncoder_L[3]: "<<targetEncoder_L[3];
        // std::ofstream outfile("velocityl.txt",std::ios::app);
        // outfile<<targetEncoder_L_forVelocity[0]<<" "<<motorEncoderCur_L[0] <<" "<<targetEncoder_L_forVelocity[1]<<" "<<motorEncoderCur_L[1]<<" "<<targetEncoder_L_forVelocity[2]<<" "<<motorEncoderCur_L[2] <<" "<<targetEncoder_L_forVelocity[3]<<" "<<motorEncoderCur_L[3]<<"\n";
        // outfile.close();

        if(enableTagCur_L == enableAction){
            targetVelocity_L_new = {0};
        }
    }
    else if(enableTagCur_L == disableAction || enableTagCur_L == keepDisabling)
    {
        targetEncoder_L = motorEncoderCur_L;

        for(int i = 4; i < 10; i++)
        {
           targetEncoder_L[i] = m_motorTargetEncoderLast_L[i];
        }

        targetVelocity_L_new = {0};
    }


    if(enableTagCur_R == enableAction||enableTagCur_R == keepEnabling)
    {
        if(enableTagCur_R == enableAction)
        {
            // handlePoseInit_R = handlePoseCur;

            motorEncoderInit_R = motorEncoderCur_R;

            m_alignmentNumber_R = 0;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_R, handlePoseCur, 'r');
        }

        controlValueCur_R = motionMapping_R(handlePoseCur,motorEncoderCur_R);

        targetEncoder_R = calculateTargetEncoder(controlValueCur_R, motorEncoderInit_R, 'r');

        targetEncoder_R_forVelocity = calculateTargetEncoder_new(controlValueCur_R, motorEncoderInit_R, 'r');

        targetVelocity_R_new = calculateTargetVelocity_new(targetEncoder_R_forVelocity, targetEncoderPrev_R_new, targetEncoder_R, motorEncoderCur_R, 'r');
        // LOG(INFO)<<std::dec<<"targetEncoder_R[0]: "<<targetEncoder_R[0]<<" targetEncoder_R[1]: "<<targetEncoder_R[1]<<" targetEncoder_R[2]: "<<targetEncoder_R[2]<<" targetEncoder_R[3]: "<<targetEncoder_R[3];
        // LOG(INFO)<<"targetVelocity_R_new: "<<targetVelocity_R_new;

        // std::ofstream outfile("new108.txt",std::ios::app);
        // outfile<<targetEncoder_R_forVelocity[0]<<" "<<motorEncoderCur_R[0] <<" "<<targetEncoder_R_forVelocity[1]<<" "<<motorEncoderCur_R[1]<<" "<<targetEncoder_R_forVelocity[2]<<" "<<motorEncoderCur_R[2] <<" "<<targetEncoder_R_forVelocity[3]<<" "<<motorEncoderCur_R[3]<<"\n";
        // outfile.close();

        if(enableTagCur_R == enableAction){
            targetVelocity_R_new = {0};
        }
    }
    else if(enableTagCur_R == disableAction || enableTagCur_R == keepDisabling)
    {
        targetEncoder_R = motorEncoderCur_R;
        /*发送给maxon本身的encoder会出现抖动*/

        for(int i = 4; i < 10; i++)
        {
           targetEncoder_R[i] = m_motorTargetEncoderLast_R[i];
        }

        targetVelocity_R_new = {0};
    }

    /*力拖动控制*/

    if(enableTagCur_L == ForceEnable || enableTagCur_L == KeepForce)
    {
        if(enableTagCur_L == ForceEnable)
        {
            motorEncoderInit_L = motorEncoderCur_L;

            setForceControlInitHandleMotorPosition(motorEncoderInit_L, handlePoseCur, 'l');
        }

        controlValueCur_L = motionMapping_L_ForceControl(handlePoseCur);

        targetEncoder_L = calculateTargetEncoder(controlValueCur_L, motorEncoderInit_L, 'l');

        targetEncoder_L_forVelocity = calculateTargetEncoder_new(controlValueCur_L, motorEncoderInit_L, 'l');

        targetVelocity_L_new = calculateTargetVelocity_new(targetEncoder_L_forVelocity, targetEncoderPrev_L_new, targetEncoder_L, motorEncoderCur_L, 'l');
        // LOG(INFO)<<std::dec<<"targetEncoder_L[0]: "<<targetEncoder_L_forVelocity[0]<<" targetEncoder_L[1]: "<<targetEncoder_L_forVelocity[1]<<" targetEncoder_L[2]: "<<targetEncoder_L_forVelocity[2]<<" targetEncoder_L[3]: "<<targetEncoder_L_forVelocity[3];
        // LOG(INFO)<<std::dec<<"targetVelocity_L_new: "<<targetVelocity_L_new;
        // std::ofstream outfile("velocityl.txt",std::ios::app);
        // outfile<<targetEncoder_L_forVelocity[0]<<" "<<motorEncoderCur_L[0] <<" "<<targetEncoder_L_forVelocity[1]<<" "<<motorEncoderCur_L[1]<<" "<<targetEncoder_L_forVelocity[2]<<" "<<motorEncoderCur_L[2] <<" "<<targetEncoder_L_forVelocity[3]<<" "<<motorEncoderCur_L[3]<<"\n";
        // outfile.close();

        if(enableTagCur_L == ForceEnable){
            targetVelocity_L_new = {0};
        }
    }
    else if(enableTagCur_L == ForceDisable)
    {
        targetVelocity_L_new = {0};
    }

    /*拖动*/
    if(enableTagCur_R == ForceEnable || enableTagCur_R == KeepForce)
    {
        if(enableTagCur_R == ForceEnable)
        {
            motorEncoderInit_R = motorEncoderCur_R;

            setForceControlInitHandleMotorPosition(motorEncoderInit_R, handlePoseCur, 'r');
        }

        controlValueCur_R = motionMapping_R_ForceControl(handlePoseCur);

        targetEncoder_R = calculateTargetEncoder(controlValueCur_R, motorEncoderInit_R, 'r');

        targetEncoder_R_forVelocity = calculateTargetEncoder_new(controlValueCur_R, motorEncoderInit_R, 'r');

        targetVelocity_R_new = calculateTargetVelocity_new(targetEncoder_R_forVelocity, targetEncoderPrev_R_new, targetEncoder_R, motorEncoderCur_R, 'r');

        // LOG(INFO)<<std::dec<<"targetEncoder_cur[1]: "<<targetEncoder_R_forVelocity[1]<<" targetEncoder_pre[1]: "<<targetEncoderPrev_R_new[1]<<" targetEncoder_R[1]: "<<targetEncoder_R[1]<<" motorEncoderCur_R[1]: "<<motorEncoderCur_R[1];

        // LOG(INFO)<<std::dec<<"targetVelocity_R[0]: "<<targetVelocity_R_new[0]<<" targetVelocity_R_new[1]: "<<targetVelocity_R_new[1]<<" targetVelocity_R_new[2]: "<<targetVelocity_R_new[2]<<" targetVelocity_R_new[3]: "<<targetVelocity_R_new[3];


        if(enableTagCur_R == ForceEnable){
            targetVelocity_R_new = {0};
        }
    }
    else if(enableTagCur_R == ForceDisable)
    {
        targetVelocity_R_new = {0};
    }

    sendMotorData(targetEncoder_R, targetVelocity_R_new, targetEncoder_L, targetVelocity_L_new);
    m_motorTargetEncoderPrev_L_new = targetEncoder_L_forVelocity;
    m_motorTargetEncoderPrev_R_new = targetEncoder_R_forVelocity;
    storeCurAsPrev(handlePoseCur, controlValueCur_L, motorEncoderCur_L, targetEncoder_L, enableTagCur_L, controlValueCur_R, motorEncoderCur_R, targetEncoder_R, enableTagCur_R);

}

std::array<double, ControlValueNum> RobotControl::motionMapping_L(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_L)
{   
    std::array<double, ControlValueNum> controlValueTmp_L = {0};

    if (m_alignmentNumber_L < 100) { m_alignmentNumber_L++; }

    /* alpha(pitch) */
    double alpha_Org_L = m_handlePoseOrg_L.handlePoseL_Elevation;
    double alpha_Init_L = m_handlePoseInit_L.handlePoseL_Elevation;
    double alpha_Last_L = m_handlePoseLastLoop_L.handlePoseL_Elevation;
    double alpha_Cur_L = handlePoseCur.handlePoseL_Elevation;

    double delt_alphaCur_L = (alpha_Cur_L - alpha_Init_L) * 180 / M_PI;
    double delt_alphaInit_L = (alpha_Init_L - alpha_Last_L) * 180 / M_PI;
    double delt_alphaOrg_L = (alpha_Last_L - alpha_Org_L) * 180 / M_PI;
    double delt_alpha_L = delt_alphaCur_L + delt_alphaInit_L * m_alignmentNumber_L / 100 + delt_alphaOrg_L;
    m_delt_alpha_L = delt_alpha_L / 180 * M_PI;

    /* 计算 deltLength_alpha_L */
    double deltLength_alpha_L_1 = cableLengths_2(delt_alpha_L);
    double deltLength_alpha_L_2 = cableLengths_2(-delt_alpha_L);

    /* 计算单边 OpenAngle */
    double openAngle_L = handlePoseCur.handlePoseL_OpenAngle;
    double openAngle_L_new;
    // auto openAngle_L_new = (openAngle_L < 0) ? 0.008 * pow(openAngle_L, 3) : pow(openAngle_L, 3)/600;
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
    openAngle_L_new = (openAngle_L < 0) ? 0.018 * pow(openAngle_L, 3) : pow(openAngle_L, 3)/600;
    }
    else if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
    openAngle_L_new = calculateNewOpenangle(openAngle_L);
    }

    /*计算 beta(yaw) */
    double beta_Org_L = m_handlePoseOrg_L.handlePoseL_Arzimuth;
    double beta_Init_L = m_handlePoseInit_L.handlePoseL_Arzimuth;
    double beta_Last_L = m_handlePoseLastLoop_L.handlePoseL_Arzimuth;
    double beta_Cur_L = handlePoseCur.handlePoseL_Arzimuth;

    double delt_betaCur_L = (beta_Cur_L - beta_Init_L) * 180 / M_PI;
    double delt_betaInit_L = (beta_Init_L - beta_Last_L) * 180 / M_PI;
    double delt_betaOrg_L = (beta_Last_L - beta_Org_L) * 180 / M_PI;
    double delt_beta_L = delt_betaCur_L + delt_betaInit_L * m_alignmentNumber_L / 100 + delt_betaOrg_L;
    m_delt_beta_L = delt_beta_L / 180 * M_PI;//preper for LastLoop

    // LOG(INFO)<< std::dec << "m_alignmentNumber_L: " <<m_alignmentNumber_L<< " delt_beta_L: " << delt_beta_L
     // <<" beta_Init_L: "<<beta_Init_L * 180 / M_PI<<" beta_Last_L: "<<beta_Last_L * 180 / M_PI<<" beta_Cur_L: "<<beta_Cur_L * 180 / M_PI;

    // LOG(INFO)<< std::dec << " delt_alpha_L: " << delt_alpha_L <<" alpha_Init_L: "<<alpha_Init_L * 180 / M_PI
    //          <<" alpha_Last_L: "<<alpha_Last_L * 180 / M_PI<<" alpha_Cur_L: "<<alpha_Cur_L * 180 / M_PI;

    double deltLength_beta_L_left_1 = cableLengths_3(delt_alpha_L, delt_beta_L, -openAngle_L_new);
    double deltLength_beta_L_left_2 = cableLengths_3(delt_alpha_L, - delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_1 = cableLengths_3(-delt_alpha_L, delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_2 = cableLengths_3(-delt_alpha_L, - delt_beta_L, -openAngle_L_new);

    /*计算 gamma（roll）*/
    double gamma_Org_L = m_handlePoseOrg_L.handlePoseL_Roll;
    double gamma_Init_L = m_handlePoseInit_L.handlePoseL_Roll;
    double gamma_Last_L = m_handlePoseLastLoop_L.handlePoseL_Roll;
    double gamma_Cur_L = handlePoseCur.handlePoseL_Roll;

    double delt_gammaCur_L = (gamma_Cur_L - gamma_Init_L) * 180 / M_PI;
    double delt_gammaInit_L = (gamma_Init_L - gamma_Last_L) * 180 / M_PI;
    double delt_gammaOrg_L = (gamma_Last_L - gamma_Org_L) * 180 / M_PI;
    double delt_gamma_L = delt_gammaCur_L + delt_gammaInit_L * m_alignmentNumber_L / 100 + delt_gammaOrg_L;
    m_delt_gamma_L = delt_gamma_L / 180 * M_PI;

    /*position 解算*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    /*以下为姿态补偿部分*/

    double instrument_1_length = 0.0;//3.83 六轴器械
    double instrument_2_length = 3.1;//4.10
    Eigen::Vector3d vector_l1 = {0,0,instrument_1_length};
    Eigen::Vector3d vector_l2 = {0,0,instrument_2_length};
    Eigen::Matrix3d rotSlaveMatrix_L;
    Eigen::Matrix3d rotation1;
    Eigen::Matrix3d rotation2;
    Eigen::Matrix3d rotation3;
    double theta = 30.0;

    rotation1 << 1,                   0,                 0,
            0,    cos(-m_armAnglePerSide / 180.0 * M_PI), -sin(-m_armAnglePerSide / 180.0 * M_PI),
            0,    sin(-m_armAnglePerSide / 180.0 * M_PI),  cos(-m_armAnglePerSide / 180.0 * M_PI);

    rotation1 = Eigen::AngleAxisd(-m_armAnglePerSide / 180.0 * M_PI, Eigen::Vector3d::UnitX());

    rotation2 = Eigen::AngleAxisd(theta / 180.0 * M_PI, Eigen::Vector3d::UnitY());

    rotation3 = Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitX());

    rotSlaveMatrix_L = rotation3 * rotation2 * rotation1;


    Eigen::Matrix3d rotationMatrix_roll;
    rotationMatrix_roll << cos(delt_gamma_L / 180.0 * M_PI),-sin(delt_gamma_L / 180.0 * M_PI),       0,
                          sin(delt_gamma_L / 180.0 * M_PI), cos(delt_gamma_L / 180.0 * M_PI),       0,
                          0,                             0,       1;

    Eigen::Matrix3d rotationMatrix_pitch;
    rotationMatrix_pitch << cos(delt_alpha_L / 180.0 * M_PI),   0,  sin(delt_alpha_L / 180.0 * M_PI),
                            0,                  1,                 0,
                           -sin(delt_alpha_L / 180.0 * M_PI),   0,  cos(delt_alpha_L / 180.0 * M_PI);

    Eigen::Matrix3d rotationMatrix_yaw;
    rotationMatrix_yaw << 1,                   0,                 0,
                           0,    cos(delt_beta_L / 180.0 * M_PI), -sin(delt_beta_L / 180.0 * M_PI),
                           0,    sin(delt_beta_L / 180.0 * M_PI),  cos(delt_beta_L / 180.0 * M_PI);

    Eigen::Vector3d compensation_l1;
    compensation_l1 = rotSlaveMatrix_L * rotationMatrix_roll * rotationMatrix_pitch * vector_l1;

    Eigen::Vector3d compensation_l2;
    compensation_l2 = rotSlaveMatrix_L * rotationMatrix_roll * rotationMatrix_pitch * rotationMatrix_yaw * vector_l2;

    Eigen::Vector3d compensation_sum;
    compensation_sum = compensation_l1 + compensation_l2;

    Eigen::Vector3d compensation_delta;
    compensation_delta.x() = compensation_sum.x() - m_compensation_LastL.x();// - 3.4338;
    compensation_delta.y() = compensation_sum.y() - m_compensation_LastL.y();// + 3.9650;
    compensation_delta.z() = compensation_sum.z() - m_compensation_LastL.z();// + 5.9475;
    // LOG(INFO)<<"delt_gamma_L: "<<delt_gamma_L;

    // double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.x();
    // double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.y();
    // double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.z();

    m_compensation_CurL.x() = compensation_sum.x();
    m_compensation_CurL.y() = compensation_sum.y();
    m_compensation_CurL.z() = compensation_sum.z();

    double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/

    m_ruckigInputState_L.target_position = {(endEffectorInit_X_L + endEffectorDelta_X_L), (endEffectorInit_Y_L + endEffectorDelta_Y_L), (endEffectorInit_Z_L + endEffectorDelta_Z_L)};

    auto plannerStatus = m_ruckigPlanner_L.update(m_ruckigInputState_L, m_ruckigOutputState_L);
    if(plannerStatus == ruckig::Result::Working){
               // LOG(INFO) << "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
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

    // std::ofstream outfile("positiontest.txt",std::ios::app);
    // outfile<<m_motionScaling[m_speedPedalIndex_Cur]<<" "<<endEffectorTarget_X_L<<" "<<endEffectorTarget_Y_L <<" "<<endEffectorTarget_Z_L<<" "<<endEffectorDelta_X_L<<" "<<endEffectorDelta_Y_L<<" "<<endEffectorDelta_Z_L<<"\n";
    // outfile.close();

/*自适应低通滤波*/
    std::array<double,3> endEffector_L;

    endEffector_L[0] = endEffectorTarget_X_L;
    endEffector_L[1] = endEffectorTarget_Y_L;
    endEffector_L[2] = endEffectorTarget_Z_L;

    std::array<double,3> endEffectorFilter_L = OneEuroStep(endEffector_L,'l');

    endEffectorTarget_X_L = endEffectorFilter_L[0];
    endEffectorTarget_Y_L = endEffectorFilter_L[1];
    endEffectorTarget_Z_L = endEffectorFilter_L[2];

    // std::ofstream outfile("Position.txt",std::ios::app);
    // outfile<<m_motionScaling[m_speedPedalIndex_Cur]<<" "<<endEffectorTarget_X_L<<" "<<endEffectorTarget_Y_L <<" "<<endEffectorTarget_Z_L<<" "<<endEffectorFilter_L[0]<<" "<<endEffectorFilter_L[1]<<" "<<endEffectorFilter_L[2]<<"\n";
    // outfile.close();

    /*jacobi part 1*/
    double endEffectorVelocity_X_L = m_ruckigOutputState_L.new_velocity[0];
    double endEffectorVelocity_Y_L = m_ruckigOutputState_L.new_velocity[1];
    double endEffectorVelocity_Z_L = m_ruckigOutputState_L.new_velocity[2];


    /*机械臂逆运动学解算*/
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

    // jointAngle1_target_L = atan((coefficient_tmp1 * endEffectorTarget_Y_L + coefficient_tmp2 * endEffectorTarget_Z_L) /
    //                              (coefficient_tmp2 * endEffectorTarget_Y_L - coefficient_tmp1 * endEffectorTarget_Z_L));

    jointAngle1_target_L = atan2((coefficient_tmp1 * endEffectorTarget_Y_L + coefficient_tmp2 * endEffectorTarget_Z_L) ,
                                 (coefficient_tmp2 * endEffectorTarget_Y_L - coefficient_tmp1 * endEffectorTarget_Z_L));

    jointAngle3_target_L = (m_armAnglePerSide - jointAngle1_target_L  - jointAngle2_target_L)
                           -(m_armAnglePerSide - jointAngle1_Init_L- jointAngle2_Init_L);

    /*jacobi part 2*/

    double jointAngle1_velocity_L, jointAngle2_velocity_L, jointAngle3_velocity_L;

    /* [velocity_y, velocity_z] = [a,b; c,d] * [joint1_velocity; joint2_velocity]*/
    double a = m_endArm_1 * cos(jointAngle1_target_L) + m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    double b = m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    double c = m_endArm_1 * sin(jointAngle1_target_L) + m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);
    double d = m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);

    jointAngle1_velocity_L = (d * endEffectorVelocity_Y_L - b * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    jointAngle2_velocity_L = (- c * endEffectorVelocity_Y_L + a * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    jointAngle3_velocity_L = - jointAngle1_velocity_L - jointAngle2_velocity_L;

    m_jointAngle0_velocity_L = 1 * endEffectorVelocity_X_L * 4000;
    m_jointAngle1_velocity_L = -1 * jointAngle1_velocity_L * 1456.36;
    m_jointAngle2_velocity_L = 1 * jointAngle2_velocity_L * 1456.36;
    m_jointAngle3_velocity_L = -1 * jointAngle3_velocity_L * 1456.36;

    // LOG(INFO)<<"jointAngle1_velocity_L: "<<m_jointAngle1_velocity_L<<" jointAngle2_velocity_L: "<<m_jointAngle2_velocity_L<<" jointAngle3_velocity_L: "<<m_jointAngle3_velocity_L <<" 规划速度： "<<endEffectorVelocity_Y_L << " " <<endEffectorVelocity_Z_L << " ";


    //Control Value:
    //  [0]:         X-Axis          Target position
    //[1 - 3]:   End Joint 1 - 3     Target Angle

    controlValueTmp_L[0] = endEffectorTarget_X_L - endEffectorInit_X_L; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_L[1] = (jointAngle1_target_L - jointAngle1_Init_L) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_L[2] = (jointAngle2_target_L - jointAngle2_Init_L) * 180 / M_PI;
    controlValueTmp_L[3] = jointAngle3_target_L * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){

        controlValueTmp_L[6] = (delt_beta_L - delt_alpha_L * m_compRatio_L) - openAngle_L_new;
        controlValueTmp_L[7] = (delt_beta_L - delt_alpha_L * m_compRatio_L) + openAngle_L_new;
        controlValueTmp_L[8] = delt_alpha_L;
        controlValueTmp_L[9] = -delt_gamma_L;

        controlValueTmp_L[5] = 0;
        controlValueTmp_L[4] = 0;
    }

    /*6轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
        controlValueTmp_L[9] = -delt_gamma_L;
        controlValueTmp_L[8] = deltLength_alpha_L_1;

        controlValueTmp_L[7] = -deltLength_beta_L_left_1;
        controlValueTmp_L[6] = -deltLength_beta_L_left_2;
        controlValueTmp_L[5] = -deltLength_beta_L_right_1;
        controlValueTmp_L[4] = -deltLength_beta_L_right_2;
    }
    return controlValueTmp_L;
}

std::array<double, ControlValueNum> RobotControl::motionMapping_R(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_R)

{
    std::array<double, ControlValueNum> controlValueTmp_R = {0};

    if (m_alignmentNumber_R < 100) { m_alignmentNumber_R++; }

    auto mag_R = m_magneticEncoder_R.load();
    // LOG(INFO)<<std::dec<<" mag_R: "<<mag_R;

    /* alpha(pitch)*/
    double alpha_Org_R = m_handlePoseOrg_R.handlePoseR_Elevation;
    double alpha_Init_R = m_handlePoseInit_R.handlePoseR_Elevation;
    double alpha_Last_R = m_handlePoseLastLoop_R.handlePoseR_Elevation;
    double alpha_Cur_R = handlePoseCur.handlePoseR_Elevation;

    double delt_alphaCur_R = (alpha_Cur_R - alpha_Init_R) * 180 / M_PI;
    double delt_alphaInit_R = (alpha_Init_R - alpha_Last_R) * 180 / M_PI;
    double delt_alphaOrg_R = (alpha_Last_R - alpha_Org_R) * 180 / M_PI;
    double delt_alpha_R = delt_alphaCur_R + delt_alphaInit_R * m_alignmentNumber_R / 100 + delt_alphaOrg_R;
    m_delt_alpha_R = delt_alpha_R / 180 * M_PI;


    /* 计算 deltLength_alpha_R*/
    double deltLength_alpha_R_1 = cableLengths_2(delt_alpha_R);
    double deltLength_alpha_R_2 = cableLengths_2(-delt_alpha_R);

    /* 计算单边 OpenAngle*/
    double openAngle_R = handlePoseCur.handlePoseR_OpenAngle;
    double openAngle_R_new;
    // auto openAngle_R_new = (openAngle_R < 0) ? 0.008 * pow(openAngle_R, 3) : pow(openAngle_R, 3)/600;
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
    openAngle_R_new = (openAngle_R < 0) ? 0.018 * pow(openAngle_R, 3) : pow(openAngle_R, 3)/600;
    }
    else if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
    openAngle_R_new = calculateNewOpenangle(openAngle_R);
    }
    /*计算 beta(yaw)*/
    double beta_Org_R = m_handlePoseOrg_R.handlePoseR_Arzimuth;
    double beta_Init_R = m_handlePoseInit_R.handlePoseR_Arzimuth;
    double beta_Last_R = m_handlePoseLastLoop_R.handlePoseR_Arzimuth;
    double beta_Cur_R = handlePoseCur.handlePoseR_Arzimuth;

    double delt_betaCur_R = (beta_Cur_R - beta_Init_R) * 180 / M_PI;
    double delt_betaInit_R = (beta_Init_R - beta_Last_R) * 180 / M_PI;
    double delt_betaOrg_R = (beta_Last_R - beta_Org_R) * 180 / M_PI;
    double delt_beta_R = delt_betaCur_R + delt_betaInit_R * m_alignmentNumber_R / 100 + delt_betaOrg_R;
    m_delt_beta_R = delt_beta_R / 180 * M_PI;

    /*计算 yaw 的绳长变化*/

    double deltLength_beta_R_left_1 = cableLengths_3(delt_alpha_R, delt_beta_R, -openAngle_R_new);
    double deltLength_beta_R_left_2 = cableLengths_3(delt_alpha_R, - delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_1 = cableLengths_3(-delt_alpha_R, delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_2 = cableLengths_3(-delt_alpha_R, - delt_beta_R, -openAngle_R_new);

    /*计算 gamma（roll）*/

    double gamma_Org_R = m_handlePoseOrg_R.handlePoseR_Roll;
    double gamma_Init_R = m_handlePoseInit_R.handlePoseR_Roll ;
    double gamma_Last_R = m_handlePoseLastLoop_R.handlePoseR_Roll;
    double gamma_Cur_R = handlePoseCur.handlePoseR_Roll;

    double delt_gammaCur_R = (gamma_Cur_R - gamma_Init_R) * 180 / M_PI;
    double delt_gammaInit_R = (gamma_Init_R - gamma_Last_R) * 180 / M_PI;
    double delt_gammaOrg_R = (gamma_Last_R - gamma_Org_R) * 180 / M_PI;
    double delt_gamma_R = delt_gammaCur_R + delt_gammaInit_R * m_alignmentNumber_R / 100 + delt_gammaOrg_R;
    m_delt_gamma_R = delt_gamma_R / 180 * M_PI;


    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    // LOG(INFO) << std::dec << "m_alignmentNumber_R: " << m_alignmentNumber_R <<" delt_beta_R: "<< delt_beta_R <<" beta_Init_R: "<< beta_Init_R*180/M_PI <<" beta_Last_R: "<< beta_Last_R*180/M_PI <<" beta_Cur_R: "<< beta_Cur_R*180/M_PI;
    /*以下为姿态补偿部分*/

   double instrument_1_length = 0.0;//3.83
   double instrument_2_length = 3.1;//4.10
   Eigen::Vector3d vector_l1 = {0,0,instrument_1_length};
   Eigen::Vector3d vector_l2 = {0,0,instrument_2_length};
   Eigen::Matrix3d rotSlaveMatrix_R;
   Eigen::Matrix3d rotation1;
   Eigen::Matrix3d rotation2;
   Eigen::Matrix3d rotation3;
   double theta = 30.0;

   rotation1 << 1,                   0,                 0,
       0,    cos(m_armAnglePerSide / 180.0 * M_PI), -sin(m_armAnglePerSide / 180.0 * M_PI),
       0,    sin(m_armAnglePerSide / 180.0 * M_PI),  cos(m_armAnglePerSide / 180.0 * M_PI);

   rotation1 = Eigen::AngleAxisd(m_armAnglePerSide / 180.0 * M_PI, Eigen::Vector3d::UnitX());

   rotation2 = Eigen::AngleAxisd(theta / 180.0 * M_PI, Eigen::Vector3d::UnitY());

   rotation3 = Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitX());

   rotSlaveMatrix_R = rotation3 * rotation2 * rotation1;


   Eigen::Matrix3d rotationMatrix_roll;
   rotationMatrix_roll << cos(delt_gamma_R / 180.0 * M_PI),-sin(delt_gamma_R / 180.0 * M_PI),       0,
       sin(delt_gamma_R / 180.0 * M_PI), cos(delt_gamma_R / 180.0 * M_PI),       0,
       0,                             0,       1;

   Eigen::Matrix3d rotationMatrix_pitch;
   rotationMatrix_pitch << cos(delt_alpha_R / 180.0 * M_PI),   0,  sin(delt_alpha_R / 180.0 * M_PI),
       0,                  1,                 0,
       -sin(delt_alpha_R / 180.0 * M_PI),   0,  cos(delt_alpha_R / 180.0 * M_PI);

   Eigen::Matrix3d rotationMatrix_yaw;
   rotationMatrix_yaw << 1,                   0,                 0,
       0,    cos(delt_beta_R / 180.0 * M_PI), -sin(delt_beta_R / 180.0 * M_PI),
       0,    sin(delt_beta_R / 180.0 * M_PI),  cos(delt_beta_R / 180.0 * M_PI);

   Eigen::Vector3d compensation_l1;
   compensation_l1 = rotSlaveMatrix_R * rotationMatrix_roll * rotationMatrix_pitch * vector_l1;

   Eigen::Vector3d compensation_l2;
   compensation_l2 = rotSlaveMatrix_R * rotationMatrix_roll * rotationMatrix_pitch * rotationMatrix_yaw * vector_l2;

   Eigen::Vector3d compensation_sum;
   compensation_sum = compensation_l1 + compensation_l2;

   Eigen::Vector3d compensation_delta;
   compensation_delta.x() = compensation_sum.x() - m_compensation_LastR.x();//-3.5074;//- 3.4338;
   compensation_delta.y() = compensation_sum.y() - m_compensation_LastR.y();//-4.05;//-3.9650;
   compensation_delta.z() = compensation_sum.z() - m_compensation_LastR.z();//+ 6.075;//5.9475;

   // double endEffectorDelta_X_R = (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.x();
   // double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.y();
   // double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur] - compensation_delta.z();

   m_compensation_CurR.x() = compensation_sum.x();
   m_compensation_CurR.y() = compensation_sum.y();
   m_compensation_CurR.z() = compensation_sum.z();

    double endEffectorDelta_X_R =  (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/


    /*路径规划*/
    m_ruckigInputState_R.target_position = {(endEffectorInit_X_R + endEffectorDelta_X_R),
                                            (endEffectorInit_Y_R + endEffectorDelta_Y_R),
                                            (endEffectorInit_Z_R + endEffectorDelta_Z_R)};

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

    double endEffectorTarget_X_R = m_ruckigOutputState_R.new_position[0];
    double endEffectorTarget_Y_R = m_ruckigOutputState_R.new_position[1];
    double endEffectorTarget_Z_R = m_ruckigOutputState_R.new_position[2];


    /*自适应低通滤波*/
    std::array<double,3> endEffector_R;

    endEffector_R[0] = endEffectorTarget_X_R;
    endEffector_R[1] = endEffectorTarget_Y_R;
    endEffector_R[2] = endEffectorTarget_Z_R;

    std::array<double,3> endEffectorFilter_R = OneEuroStep(endEffector_R,'r');

    endEffectorTarget_X_R = endEffectorFilter_R[0];
    endEffectorTarget_Y_R = endEffectorFilter_R[1];
    endEffectorTarget_Z_R = endEffectorFilter_R[2];


    /*jacobi*/
    double endEffectorVelocity_X_R = m_ruckigOutputState_R.new_velocity[0];
    double endEffectorVelocity_Y_R = m_ruckigOutputState_R.new_velocity[1];
    double endEffectorVelocity_Z_R = m_ruckigOutputState_R.new_velocity[2];


    /*机械臂逆运动学结算*/
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

    jointAngle1_target_R = atan2((coefficient_tmp1 * endEffectorTarget_Y_R + coefficient_tmp2 * endEffectorTarget_Z_R),
                               (coefficient_tmp2 * endEffectorTarget_Y_R - coefficient_tmp1 * endEffectorTarget_Z_R));

    jointAngle3_target_R = (m_armAnglePerSide - jointAngle1_target_R  - jointAngle2_target_R)
                           -(m_armAnglePerSide - jointAngle1_Init_R- jointAngle2_Init_R);

    /*jacobi*/

    double jointAngle1_velocity_R, jointAngle2_velocity_R, jointAngle3_velocity_R;

    /* [velocity_y, velocity_z] = [a,b; c,d] * [joint1_velocity; joint2_velocity]*/
    double a = m_endArm_1 * cos(jointAngle1_target_R) + m_endArm_2 * cos(jointAngle1_target_R + jointAngle2_target_R);
    double b = m_endArm_2 * cos(jointAngle1_target_R + jointAngle2_target_R);
    double c = m_endArm_1 * sin(jointAngle1_target_R) + m_endArm_2 * sin(jointAngle1_target_R + jointAngle2_target_R);
    double d = m_endArm_2 * sin(jointAngle1_target_R + jointAngle2_target_R);

    jointAngle1_velocity_R = (d * endEffectorVelocity_Y_R - b * endEffectorVelocity_Z_R) / (a * d - b * c) * 180 / M_PI;
    jointAngle2_velocity_R = (- c * endEffectorVelocity_Y_R + a * endEffectorVelocity_Z_R) / (a * d - b * c) * 180 / M_PI;
    jointAngle3_velocity_R = - jointAngle1_velocity_R - jointAngle2_velocity_R;

    m_jointAngle0_velocity_R = 1 * endEffectorVelocity_X_R * 4000;
    m_jointAngle1_velocity_R = -1 * jointAngle1_velocity_R * 1456.36;
    m_jointAngle2_velocity_R = 1 * jointAngle2_velocity_R * 1456.36;
    m_jointAngle3_velocity_R = -1 * jointAngle3_velocity_R * 1456.36;

    /*输出值*/
    controlValueTmp_R[0] = endEffectorTarget_X_R - endEffectorInit_X_R; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_R[1] = (jointAngle1_target_R - jointAngle1_Init_R) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_R[2] = (jointAngle2_target_R - jointAngle2_Init_R) * 180 / M_PI;
    controlValueTmp_R[3] = jointAngle3_target_R * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){

        controlValueTmp_R[6] = (delt_beta_R - delt_alpha_R * m_compRatio_R) - openAngle_R_new;
        controlValueTmp_R[7] = (delt_beta_R - delt_alpha_R * m_compRatio_R) + openAngle_R_new;
        controlValueTmp_R[8] = delt_alpha_R;
        controlValueTmp_R[9] = -delt_gamma_R;

        controlValueTmp_R[4] = 0;
        controlValueTmp_R[5] = 0;
    }

    /*6轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
        controlValueTmp_R[9] = -delt_gamma_R;
        controlValueTmp_R[8] = deltLength_alpha_R_1;
        controlValueTmp_R[7] = -deltLength_beta_R_left_1;
        controlValueTmp_R[6] = -deltLength_beta_R_left_2;
        controlValueTmp_R[5] = -deltLength_beta_R_right_1;
        controlValueTmp_R[4] = -deltLength_beta_R_right_2;
    }
    controlValueTmp_R[10] = handlePoseCur.graspIndex_R;
    return controlValueTmp_R;
}

std::array<double, ControlValueNum> RobotControl::test_motionMapping_L(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_L)
{
    std::array<double, ControlValueNum> controlValueTmp_L = {0};

    if (m_alignmentNumber_L < 100) { m_alignmentNumber_L++; }

    /* alpha(pitch) */
    double alpha_Org_L = m_handlePoseOrg_L.handlePoseL_Elevation;
    double alpha_Init_L = m_handlePoseInit_L.handlePoseL_Elevation;
    double alpha_Last_L = m_handlePoseLastLoop_L.handlePoseL_Elevation;
    double alpha_Cur_L = handlePoseCur.handlePoseL_Elevation;

    /*夹持角度测试*/
    // alpha_Cur_L = -30.0/180 * M_PI;//测t2
    // alpha_Cur_L = 60.0/180 * M_PI;//测拉力

    //beta_Cur_L = 60.0;//测t1
    //gamma_Cur_L = 90.0;

    double delt_alphaCur_L = (alpha_Cur_L - alpha_Init_L) * 180 / M_PI;
    double delt_alphaInit_L = (alpha_Init_L - alpha_Last_L) * 180 / M_PI;
    double delt_alphaOrg_L = (alpha_Last_L - alpha_Org_L) * 180 / M_PI;
    double delt_alpha_L = delt_alphaCur_L + delt_alphaInit_L * m_alignmentNumber_L / 100 + delt_alphaOrg_L;
    if(m_oprationMode == 2 || m_oprationMode == 3){
    delt_alpha_L = 0.0;
    }

    double openAngle_L;
    double endEffectorDelta_X_L;
    double endEffectorDelta_Y_L;
    double endEffectorDelta_Z_L;

    /* 计算 deltLength_alpha_L */
    double deltLength_alpha_L_1 = cableLengths_2(delt_alpha_L);
    double deltLength_alpha_L_2 = cableLengths_2(-delt_alpha_L);

    /* 计算单边 OpenAngle */
    openAngle_L = handlePoseCur.handlePoseL_OpenAngle;

    openAngle_L = 0.0;
    auto openAngle_L_new = (openAngle_L < 0) ? 0.008 * pow(openAngle_L, 3) : pow(openAngle_L, 3)/600;

    /*计算 beta(yaw) */
    double beta_Org_L = m_handlePoseOrg_L.handlePoseL_Arzimuth;
    double beta_Init_L = m_handlePoseInit_L.handlePoseL_Arzimuth;
    double beta_Last_L = m_handlePoseLastLoop_L.handlePoseL_Arzimuth;
    double beta_Cur_L = handlePoseCur.handlePoseL_Arzimuth;

    double delt_betaCur_L = (beta_Cur_L - beta_Init_L) * 180 / M_PI;
    double delt_betaInit_L = (beta_Init_L - beta_Last_L) * 180 / M_PI;
    double delt_betaOrg_L = (beta_Last_L - beta_Org_L) * 180 / M_PI;
    double delt_beta_L = delt_betaCur_L + delt_betaInit_L * m_alignmentNumber_L / 100 + delt_betaOrg_L;
    if(m_oprationMode == 2 || m_oprationMode == 3){
    delt_beta_L = 0.0;
    }
    \
    /*计算 yaw 的绳长变化*/
    double deltLength_beta_L_left_1 = cableLengths_3(delt_alpha_L, delt_beta_L, -openAngle_L_new);
    double deltLength_beta_L_left_2 = cableLengths_3(delt_alpha_L, - delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_1 = cableLengths_3(-delt_alpha_L, delt_beta_L, openAngle_L_new);
    double deltLength_beta_L_right_2 = cableLengths_3(-delt_alpha_L, - delt_beta_L, -openAngle_L_new);

    /*计算 gamma（roll）*/
    double gamma_Org_L = m_handlePoseOrg_L.handlePoseL_Roll;
    double gamma_Init_L = m_handlePoseInit_L.handlePoseL_Roll;
    double gamma_Last_L = m_handlePoseLastLoop_L.handlePoseL_Roll;
    double gamma_Cur_L = handlePoseCur.handlePoseL_Roll;

    double delt_gammaCur_L = (gamma_Cur_L - gamma_Init_L) * 180 / M_PI;
    double delt_gammaInit_L = (gamma_Init_L - gamma_Last_L) * 180 / M_PI;
    double delt_gammaOrg_L = (gamma_Last_L - gamma_Org_L) * 180 / M_PI;
    double delt_gamma_L = delt_gammaCur_L + delt_gammaInit_L * m_alignmentNumber_L / 100 + delt_gammaOrg_L;
    if(m_oprationMode == 2 || m_oprationMode == 3){
    delt_gamma_L = 0.0;
    }

    /*position 解算*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)


 /*重复定位精度运动测试*/

    if(m_oprationMode == 2){
    int test_step = 1000;
    double length = 10;//边长

    Eigen::Vector3d Point_0 = {0, 0, 0};
    Eigen::Vector3d Point_1 = {-0.5 * length, 0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_2 = {-0.5 * length, -0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_3 = {0.5 * length, -0.5 * length, 0.5 * length};
    Eigen::Vector3d Point_4 = {0.5 * length, 0.5 * length, 0.5 * length};

    switch (test_index) {
    case 1:
        test_x = test_x + (Point_1.x() - Point_0.x())/test_step;
        test_y = test_y + (Point_1.y() - Point_0.y())/test_step;
        test_z = test_z + (Point_1.z() - Point_0.z())/test_step;
        break;
    case 2:
        test_x = test_x + (Point_2.x() - Point_1.x())/test_step;
        test_y = test_y + (Point_2.y() - Point_1.y())/test_step;
        test_z = test_z + (Point_2.z() - Point_1.z())/test_step;
        break;
    case 3:
        test_x = test_x + (Point_3.x() - Point_2.x())/test_step;
        test_y = test_y + (Point_3.y() - Point_2.y())/test_step;
        test_z = test_z + (Point_3.z() - Point_2.z())/test_step;
        break;
    case 4:
        test_x = test_x + (Point_4.x() - Point_3.x())/test_step;
        test_y = test_y + (Point_4.y() - Point_3.y())/test_step;
        test_z = test_z + (Point_4.z() - Point_3.z())/test_step;
        break;
    case 5:
        test_x = test_x + (Point_0.x() - Point_4.x())/test_step;
        test_y = test_y + (Point_0.y() - Point_4.y())/test_step;
        test_z = test_z + (Point_0.z() - Point_4.z())/test_step;
        break;
    }

    test_time = test_time + 1;

    if((test_time >= 0) && (test_time < test_step)){
        test_index = 0;
    }else if((test_time >= test_step) && (test_time < 2 * test_step)){
        test_index = 1;
    }else if((test_time >= 2 * test_step) && (test_time < 3 * test_step)){
        test_index = 2;
    }else if((test_time >= 3 * test_step) && (test_time < 4 * test_step)){
        test_index = 3;
    }else if((test_time >= 4 * test_step) && (test_time < 5 * test_step)){
        test_index = 4;
    }else if((test_time >= 5 * test_step) && (test_time < 6 * test_step)){
        test_index = 5;
    }else if(test_time == 6 * test_step){
        test_index = 6;
    }

    endEffectorDelta_X_L = test_x;
    endEffectorDelta_Y_L = test_y;
    endEffectorDelta_Z_L = test_z;

    if(test_index == 6){

        LOG(INFO)<<"=================================================================================================================================================";
    }

    }

    /*步进测试*/
    if(m_oprationMode == 3){
    if(test_index < 25){//每次测试步进25*0.004=0.1mm

        test_x = test_x + 0.004;
        test_index = test_index + 1;
    }

    endEffectorDelta_X_L = test_x;
    endEffectorDelta_Y_L = test_y;
    endEffectorDelta_Z_L = test_z;

    // LOG(INFO)<<"endEffectorDelta_X_L: "<<endEffectorDelta_X_L;
    }


    /*记录位置*/
    if(m_oprationMode == 2){
    double X_L = (handlePoseCur.handlePoseL_X) / 0.1;
    double Y_L = -(handlePoseCur.handlePoseL_Y)/ 0.1;
    double Z_L = -(handlePoseCur.handlePoseL_Z) / 0.1;

    // LOG(INFO) << "X_L: " << X_L << " Y_L: "<<Y_L<<" Z_L: "<<Z_L;

    // std::ofstream outfile("viper.txt",std::ios::app);
    // outfile<<X_L<<" "<<Y_L<<" "<<Z_L<< "\n";
    //  if(test_index == 6){
    //     outfile<<" =========================================================================== "<<"\n";
    //  }
    // outfile.close();
    }



    // endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    // endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    // endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

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


    /*jacobi*/
    double endEffectorVelocity_X_L = m_ruckigOutputState_L.new_velocity[0];
    double endEffectorVelocity_Y_L = m_ruckigOutputState_L.new_velocity[1];
    double endEffectorVelocity_Z_L = m_ruckigOutputState_L.new_velocity[2];



    double jointAngle1_Init_L, jointAngle2_Init_L, jointAngle3_Init_L;
    jointAngle1_Init_L = m_endEffectorInitJointAngle_L[0];
    jointAngle2_Init_L = m_endEffectorInitJointAngle_L[1];
    jointAngle3_Init_L = m_endEffectorInitJointAngle_L[2];

    double jointAngle1_target_L, jointAngle2_target_L, jointAngle3_target_L;

    jointAngle2_target_L = -acos((endEffectorTarget_Y_L * endEffectorTarget_Y_L + endEffectorTarget_Z_L * endEffectorTarget_Z_L - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                 (2 * m_endArm_1 * m_endArm_2));

    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_L);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_L);

    jointAngle1_target_L = atan2((coefficient_tmp1 * endEffectorTarget_Y_L + coefficient_tmp2 * endEffectorTarget_Z_L) ,
                                (coefficient_tmp2 * endEffectorTarget_Y_L - coefficient_tmp1 * endEffectorTarget_Z_L));

    jointAngle3_target_L = (m_armAnglePerSide - jointAngle1_target_L  - jointAngle2_target_L)
                           -(m_armAnglePerSide - jointAngle1_Init_L- jointAngle2_Init_L);



    /*jacob*/

    double jointAngle1_velocity_L, jointAngle2_velocity_L, jointAngle3_velocity_L;

    /* [velocity_y, velocity_z] = [a,b; c,d] * [joint1_velocity; joint2_velocity]*/
    double a = m_endArm_1 * cos(jointAngle1_target_L) + m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    double b = m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    double c = m_endArm_1 * sin(jointAngle1_target_L) + m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);
    double d = m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);

    jointAngle1_velocity_L = (d * endEffectorVelocity_Y_L - b * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    jointAngle2_velocity_L = (- c * endEffectorVelocity_Y_L + a * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    jointAngle3_velocity_L = - jointAngle1_velocity_L - jointAngle2_velocity_L;

    m_jointAngle0_velocity_L = 1 * endEffectorVelocity_X_L * 4000;
    m_jointAngle1_velocity_L = -1 * jointAngle1_velocity_L * 1456.36;
    m_jointAngle2_velocity_L = 1 * jointAngle2_velocity_L * 1456.36;
    m_jointAngle3_velocity_L = -1 * jointAngle3_velocity_L * 1456.36;




    controlValueTmp_L[0] = endEffectorTarget_X_L - endEffectorInit_X_L; //(handpose_cur-handpose_init) 单位为 mm
    controlValueTmp_L[1] = (jointAngle1_target_L - jointAngle1_Init_L) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_L[2] = (jointAngle2_target_L - jointAngle2_Init_L) * 180 / M_PI;
    controlValueTmp_L[3] = jointAngle3_target_L * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
        controlValueTmp_L[4] = (delt_beta_L - delt_alpha_L * m_compRatio_L) - openAngle_L_new;
        controlValueTmp_L[5] = (delt_beta_L - delt_alpha_L * m_compRatio_L) + openAngle_L_new;
        controlValueTmp_L[6] = delt_alpha_L;
        controlValueTmp_L[7] = -delt_gamma_L;

        controlValueTmp_L[8] = 0;
        controlValueTmp_L[9] = 0;
    }

    /*6轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
        controlValueTmp_L[9] = -delt_gamma_L;
        controlValueTmp_L[8] = deltLength_alpha_L_1;

        controlValueTmp_L[7] = -deltLength_beta_L_left_1;
        controlValueTmp_L[6] = -deltLength_beta_L_left_2;
        controlValueTmp_L[5] = -deltLength_beta_L_right_1;
        controlValueTmp_L[4] = -deltLength_beta_L_right_2;

    }
    return controlValueTmp_L;
}

std::array<double, ControlValueNum> RobotControl::test_motionMapping_R(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_R)
{
    std::array<double, ControlValueNum> controlValueTmp_R = {0};

    if (m_alignmentNumber_R < 100) { m_alignmentNumber_R++; }

    /* alpha(pitch) */
    double alpha_Org_R = m_handlePoseOrg_R.handlePoseR_Elevation;
    double alpha_Init_R = m_handlePoseInit_R.handlePoseR_Elevation;
    double alpha_Last_R = m_handlePoseLastLoop_R.handlePoseR_Elevation;
    double alpha_Cur_R = handlePoseCur.handlePoseR_Elevation;

    /*夹持角度测试*/
    // alpha_Cur_L = -30.0/180 * M_PI;//测t2
    // alpha_Cur_L = 60.0/180 * M_PI;//测拉力

    //beta_Cur_L = 60.0;//测t1
    //gamma_Cur_L = 90.0;

    double delt_alphaCur_R = (alpha_Cur_R - alpha_Init_R) * 180 / M_PI;
    double delt_alphaInit_R = (alpha_Init_R - alpha_Last_R) * 180 / M_PI;
    double delt_alphaOrg_R = (alpha_Last_R - alpha_Org_R) * 180 / M_PI;
    double delt_alpha_R = delt_alphaCur_R + delt_alphaInit_R * m_alignmentNumber_R / 100 + delt_alphaOrg_R;
    delt_alpha_R = 0.0;

    double openAngle_R;
    double endEffectorDelta_X_R;
    double endEffectorDelta_Y_R;
    double endEffectorDelta_Z_R;

    /* 计算 deltLength_alpha_L */
    double deltLength_alpha_R_1 = cableLengths_2(delt_alpha_R);
    double deltLength_alpha_R_2 = cableLengths_2(-delt_alpha_R);

    /* 计算单边 OpenAngle */
    openAngle_R = handlePoseCur.handlePoseR_OpenAngle;

    openAngle_R = 0.0;
    auto openAngle_R_new = (openAngle_R < 0) ? 0.008 * pow(openAngle_R, 3) : pow(openAngle_R, 3)/600;

    /*计算 beta(yaw) */
    double beta_Org_R = m_handlePoseOrg_R.handlePoseR_Arzimuth;
    double beta_Init_R = m_handlePoseInit_R.handlePoseR_Arzimuth;
    double beta_Last_R = m_handlePoseLastLoop_R.handlePoseR_Arzimuth;
    double beta_Cur_R = handlePoseCur.handlePoseR_Arzimuth;

    double delt_betaCur_R = (beta_Cur_R - beta_Init_R) * 180 / M_PI;
    double delt_betaInit_R = (beta_Init_R - beta_Last_R) * 180 / M_PI;
    double delt_betaOrg_R = (beta_Last_R - beta_Org_R) * 180 / M_PI;
    double delt_beta_R = delt_betaCur_R + delt_betaInit_R * m_alignmentNumber_R / 100 + delt_betaOrg_R;
    delt_beta_R = 0.0;
    \
        /*计算 yaw 的绳长变化*/
    double deltLength_beta_R_left_1 = cableLengths_3(-delt_alpha_R, delt_beta_R, -openAngle_R_new);
    double deltLength_beta_R_left_2 = cableLengths_3(-delt_alpha_R, - delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_1 = cableLengths_3(delt_alpha_R, delt_beta_R, openAngle_R_new);
    double deltLength_beta_R_right_2 = cableLengths_3(delt_alpha_R, - delt_beta_R, -openAngle_R_new);

    /*计算 gamma（roll）*/
    double gamma_Org_R = m_handlePoseOrg_R.handlePoseR_Roll;
    double gamma_Init_R = m_handlePoseInit_R.handlePoseR_Roll;
    double gamma_Last_R = m_handlePoseLastLoop_R.handlePoseR_Roll;
    double gamma_Cur_R = handlePoseCur.handlePoseR_Roll;

    double delt_gammaCur_R = (gamma_Cur_R - gamma_Init_R) * 180 / M_PI;
    double delt_gammaInit_R = (gamma_Init_R - gamma_Last_R) * 180 / M_PI;
    double delt_gammaOrg_R = (gamma_Last_R - gamma_Org_R) * 180 / M_PI;
    double delt_gamma_R = delt_gammaCur_R + delt_gammaInit_R * m_alignmentNumber_R / 100 + delt_gammaOrg_R;
    delt_gamma_R = 0.0;

    /*position 解算*/
    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)


 /*重复定位精度运动测试*/
    int test_step = 3000;
    double length = 20;//边长

    Eigen::Vector3d Point_0 = {0, 0, 0};
    Eigen::Vector3d Point_1 = {-0.5 * length, 0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_2 = {-0.5 * length, -0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_3 = {0.5 * length, -0.5 * length, 0.5 * length};
    Eigen::Vector3d Point_4 = {0.5 * length, 0.5 * length, 0.5 * length};

    switch (test_index) {
    case 1:
        test_x = test_x + (Point_1.x() - Point_0.x())/test_step;
        test_y = test_y + (Point_1.y() - Point_0.y())/test_step;
        test_z = test_z + (Point_1.z() - Point_0.z())/test_step;
        break;
    case 2:
        test_x = test_x + (Point_2.x() - Point_1.x())/test_step;
        test_y = test_y + (Point_2.y() - Point_1.y())/test_step;
        test_z = test_z + (Point_2.z() - Point_1.z())/test_step;
        break;
    case 3:
        test_x = test_x + (Point_3.x() - Point_2.x())/test_step;
        test_y = test_y + (Point_3.y() - Point_2.y())/test_step;
        test_z = test_z + (Point_3.z() - Point_2.z())/test_step;
        break;
    case 4:
        test_x = test_x + (Point_4.x() - Point_3.x())/test_step;
        test_y = test_y + (Point_4.y() - Point_3.y())/test_step;
        test_z = test_z + (Point_4.z() - Point_3.z())/test_step;
        break;
    case 5:
        test_x = test_x + (Point_0.x() - Point_4.x())/test_step;
        test_y = test_y + (Point_0.y() - Point_4.y())/test_step;
        test_z = test_z + (Point_0.z() - Point_4.z())/test_step;
        break;
    }

    test_time = test_time + 1;

    if((test_time >= 0) && (test_time < test_step)){
        test_index = 0;
    }else if((test_time >= test_step) && (test_time < 2 * test_step)){
        test_index = 1;
    }else if((test_time >= 2 * test_step) && (test_time < 3 * test_step)){
        test_index = 2;
    }else if((test_time >= 3 * test_step) && (test_time < 4 * test_step)){
        test_index = 3;
    }else if((test_time >= 4 * test_step) && (test_time < 5 * test_step)){
        test_index = 4;
    }else if((test_time >= 5 * test_step) && (test_time < 6 * test_step)){
        test_index = 5;
    }else if(test_time == 6 * test_step){
        test_index = 6;
    }

    endEffectorDelta_X_R = test_x;
    endEffectorDelta_Y_R = test_y;
    endEffectorDelta_Z_R = test_z;

    if(test_index == 6){

        LOG(INFO)<<"=================================================================================================================================================";
    }


    // if(test_index < 25){//每次测试步进25*0.004=0.1mm

    //     test_x = test_x + 0.004;
    //     test_index = test_index + 1;
    // }

    // endEffectorDelta_X_L = test_x;
    // endEffectorDelta_Y_L = test_y;
    // endEffectorDelta_Z_L = test_z;

// LOG(INFO)<<"endEffectorDelta_X_L: "<<endEffectorDelta_X_L;

    /*记录位置*/
    double X_R = (handlePoseCur.handlePoseR_X) / 0.1;
    double Y_R = -(handlePoseCur.handlePoseR_Y)/ 0.1;
    double Z_R = -(handlePoseCur.handlePoseR_Z) / 0.1;

    // LOG(INFO) << "X_L: " << X_L << " Y_L: "<<Y_L<<" Z_L: "<<Z_L;

    std::ofstream outfile("viperjacobi_R.txt",std::ios::app);
    outfile<<X_R<<" "<<Y_R<<" "<<Z_R<< "\n";
     if(test_index == 6){
        outfile<<" =========================================================================== "<<"\n";
     }
    outfile.close();

    // endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    // endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    // endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/
    m_ruckigInputState_R.target_position = {(endEffectorInit_X_R + endEffectorDelta_X_R), (endEffectorInit_Y_R + endEffectorDelta_Y_R), (endEffectorInit_Z_R + endEffectorDelta_Z_R)};
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


    /*jacobi*/
    // double endEffectorVelocity_X_L = m_ruckigOutputState_L.new_velocity[0];
    // double endEffectorVelocity_Y_L = m_ruckigOutputState_L.new_velocity[1];
    // double endEffectorVelocity_Z_L = m_ruckigOutputState_L.new_velocity[2];


    double jointAngle1_Init_R, jointAngle2_Init_R, jointAngle3_Init_R;
    jointAngle1_Init_R = m_endEffectorInitJointAngle_R[0];
    jointAngle2_Init_R = m_endEffectorInitJointAngle_R[1];
    jointAngle3_Init_R = m_endEffectorInitJointAngle_R[2];

    double jointAngle1_target_R, jointAngle2_target_R, jointAngle3_target_R;

    jointAngle2_target_R = -acos((endEffectorTarget_Y_R * endEffectorTarget_Y_R + endEffectorTarget_Z_R * endEffectorTarget_Z_R - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                 (2 * m_endArm_1 * m_endArm_2));

    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_R);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_R);

    jointAngle1_target_R = atan2((coefficient_tmp1 * endEffectorTarget_Y_R + coefficient_tmp2 * endEffectorTarget_Z_R) ,
                                (coefficient_tmp2 * endEffectorTarget_Y_R - coefficient_tmp1 * endEffectorTarget_Z_R));

    jointAngle3_target_R = (m_armAnglePerSide - jointAngle1_target_R  - jointAngle2_target_R)
                           -(m_armAnglePerSide - jointAngle1_Init_R- jointAngle2_Init_R);

    //Control Value:
    //  [0]:         X-Axis          Target position
    //[1 - 3]:   End Joint 1 - 3     Target Angle


    /*jacob*/

    // double jointAngle1_velocity_L, jointAngle2_velocity_L, jointAngle3_velocity_L;

    // /* [velocity_y, velocity_z] = [a,b; c,d] * [joint1_velocity; joint2_velocity]*/
    // double a = m_endArm_1 * cos(jointAngle1_target_L) + m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    // double b = m_endArm_2 * cos(jointAngle1_target_L + jointAngle2_target_L);
    // double c = m_endArm_1 * sin(jointAngle1_target_L) + m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);
    // double d = m_endArm_2 * sin(jointAngle1_target_L + jointAngle2_target_L);

    // jointAngle1_velocity_L = (d * endEffectorVelocity_Y_L - b * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    // jointAngle2_velocity_L = (- c * endEffectorVelocity_Y_L + a * endEffectorVelocity_Z_L) / (a * d - b * c) * 180 / M_PI;
    // jointAngle3_velocity_L = - jointAngle1_velocity_L - jointAngle2_velocity_L;

    // m_jointAngle0_velocity_L = 1 * endEffectorVelocity_X_L * 4000;
    // m_jointAngle1_velocity_L = -1 * jointAngle1_velocity_L * 1456.36;
    // m_jointAngle2_velocity_L = 1 * jointAngle2_velocity_L * 1456.36;
    // m_jointAngle3_velocity_L = -1 * jointAngle3_velocity_L * 1456.36;


    controlValueTmp_R[0] = endEffectorTarget_X_R - endEffectorInit_X_R; //(handpose_cur-handpose_init)单位为mm
    controlValueTmp_R[1] = (jointAngle1_target_R - jointAngle1_Init_R) * 180 / M_PI;//输出为各关节角度
    controlValueTmp_R[2] = (jointAngle2_target_R - jointAngle2_Init_R) * 180 / M_PI;
    controlValueTmp_R[3] = jointAngle3_target_R * 180 / M_PI;

    /*4轴器械*/
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
        controlValueTmp_R[4] = (delt_beta_R - delt_alpha_R * m_compRatio_R) - openAngle_R_new;
        controlValueTmp_R[5] = (delt_beta_R - delt_alpha_R * m_compRatio_R) + openAngle_R_new;
        controlValueTmp_R[6] = delt_alpha_R;
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
    return controlValueTmp_R;
}

std::array<double, ControlValueNum> RobotControl::motionMapping_L_ForceControl(const HandlePose& handlePoseCur)
{
    std::array<double, ControlValueNum> controlValueTmp_L = {0};

    /*进入使能时机械臂末端的位置*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];

    auto forceValue = m_force_L.load();
    // LOG(INFO)<<"forceValue: "<<forceValue;

    for(int i = 0; i < 3; i++){
        if(forceValue[i] > 4){
            forceValue[i] = 4;
        }else if(forceValue[i] < -4){
            forceValue[i] = -4;
        }
                LOG(INFO)<<"forceValue_L "<< i <<" : "<<forceValue[i];
    }

    m_forceAccumulationBufferLeft = {
        m_forceAccumulationBufferLeft[0] + forceValue[0] * 0.025,//* 1500.0
        m_forceAccumulationBufferLeft[1] + forceValue[1] * 0.025,//* 1500.0
        m_forceAccumulationBufferLeft[2] + forceValue[2] * 0.025};//* 1500.0

    double endEffectorDelta_X_L = m_forceAccumulationBufferLeft[0] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = m_forceAccumulationBufferLeft[1] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = m_forceAccumulationBufferLeft[2] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    /*路径规划*/
    m_ruckigInputState_L.target_position = {(endEffectorInit_X_L + endEffectorDelta_X_L), (endEffectorInit_Y_L + endEffectorDelta_Y_L), (endEffectorInit_Z_L + endEffectorDelta_Z_L)};

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

    // auto forceValue = m_domainController->getForce(0,m_armAnglePerSide);
    auto forceValue = m_force_R.load();

    for(int i = 0; i < 3; i++){
        if(forceValue[i] > 4){
            forceValue[i] = 4;
        }else if(forceValue[i] < -4){
            forceValue[i] = -4;
        }
        LOG(INFO)<<"forceValue_R "<< i <<" : "<<forceValue[i];
    }


    m_forceAccumulationBufferRight = {
        m_forceAccumulationBufferRight[0] + forceValue[0] * 0.025,//* 1500.0
        m_forceAccumulationBufferRight[1] + forceValue[1] * 0.025,//* 1500.0
        m_forceAccumulationBufferRight[2] + forceValue[2] * 0.025};//* 1500.0

    double endEffectorDelta_X_R = m_forceAccumulationBufferRight[0] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = m_forceAccumulationBufferRight[1] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = m_forceAccumulationBufferRight[2] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/


    /*路径规划*/
    m_ruckigInputState_R.target_position = {(endEffectorInit_X_R + endEffectorDelta_X_R),
                                            (endEffectorInit_Y_R + endEffectorDelta_Y_R),
                                            (endEffectorInit_Z_R + endEffectorDelta_Z_R)};

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

    double endEffectorTarget_X_R = m_ruckigOutputState_R.new_position[0];
    double endEffectorTarget_Y_R = m_ruckigOutputState_R.new_position[1];
    double endEffectorTarget_Z_R = m_ruckigOutputState_R.new_position[2];

    // LOG(INFO)<<"endEffectorTarget_Y_R"<<endEffectorTarget_Y_R<<" endEffectorDelta_Y_R: "<<endEffectorDelta_Y_R<<" endEffectorInit_Y_R: "<<endEffectorInit_Y_R;

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

double RobotControl::calculateNewOpenangle(double masterOpenangle){
    double newOpenangle;
    if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons){
    if(masterOpenangle < -3){
        // newOpenangle = masterOpenangle * 0.9286 + 1.286;/*-8*/
        // newOpenangle = masterOpenangle * 1.2143 + 2.1428;/*-10*/
        newOpenangle = masterOpenangle * 1.5 + 3;/*-12*/
    }else if(masterOpenangle >= -3 && masterOpenangle< 7){
        newOpenangle = 0.5 * masterOpenangle;
    }else if(masterOpenangle >= 7){
        newOpenangle =  masterOpenangle * 1.269 - 5.3846;
    }
    }

    if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons){
    if(masterOpenangle < -3){
        // newOpenangle = masterOpenangle * 3.14286 + 2.1429;/*25*/
        newOpenangle = masterOpenangle * 3.8575 + 8.5714;/*30*/
    }else if(masterOpenangle >= -3 && masterOpenangle< 7){
        newOpenangle = 1.0 * masterOpenangle;
    }else if(masterOpenangle >= 7){
        newOpenangle =  masterOpenangle * 1.6429 - 4.5;
    }
    }

    return newOpenangle;
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

    /*出使能时记录当前姿态位置*/
    if (enableTagCur_L == disableAction)
    {
        m_handlePoseLastLoop_L = handlePoseCur;

        m_handlePoseLastLoop_L.handlePoseL_Elevation = m_delt_alpha_L;
        m_handlePoseLastLoop_L.handlePoseL_Roll = m_delt_gamma_L;
        m_handlePoseLastLoop_L.handlePoseL_Arzimuth = m_delt_beta_L;
    }

    if (enableTagCur_R == disableAction)
    {
        m_handlePoseLastLoop_R = handlePoseCur;

        m_handlePoseLastLoop_R.handlePoseR_Elevation = m_delt_alpha_R;
        m_handlePoseLastLoop_R.handlePoseR_Roll = m_delt_gamma_R;
        m_handlePoseLastLoop_R.handlePoseR_Arzimuth = m_delt_beta_R;
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

        double a4_1 = atan2(l4_1 , r4_1) * 180.0 / M_PI;
        double b4_1 = std::abs(atan2((p4_x - c4_1_x) , (p4_y - c4_1_y)) * 180.0 / M_PI);
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

/*自适应低通*/
/*
std::array<double, 3> RobotControl::OneEuroStep(const std::array<double, 3>&raw, const char& side)
{
        const double dt_used = (std::isfinite(dt) && dt > 0.0) ? dt : 0.004;

        auto alphaFromFc = [dt_used](double fc){
            constexpr double TWO_PI = 6.28318530717958647692;
            fc = std::max(fc, 1e-9);                         // 频率下限保护
            const double tau = 1.0 / (TWO_PI * fc);
            return dt_used / (dt_used + tau);
        };

        std::array<double,3> out{};

        for (int i = 0; i < 3; ++i) {
            if (!inited_[i]) {
                inited_[i] = true;
                xhat_[i]   = raw[i];
                vhat_[i]   = 0.0;
                out[i]     = xhat_[i];
                continue;
            }

            // 1) 速度估计：脏微分 + 一阶低通
            double dx = (raw[i] - xhat_[i]) / dt_used;
            if (vel_dead > 0.0 && std::abs(dx) < vel_dead) dx = 0.0;

            vhat_[i] += alphaFromFc(fd) * (dx - vhat_[i]);

            // 2) 自适应截止频率
            double fc = fmin + beta * std::abs(vhat_[i]);
            if (fc_max > 0.0) fc = std::min(fc, fc_max);

            // 3) 位置一阶低通
            xhat_[i] += alphaFromFc(fc) * (raw[i] - xhat_[i]);

            out[i] = xhat_[i];
        }

        return out;
    }
*/

std::array<double, 3> RobotControl::OneEuroStep(const std::array<double, 3>& raw, const char& side)
{
    std::array<double,3> out{};

    if(side=='l'){
    const double dt_used = (std::isfinite(dt) && dt > 0.0) ? dt : 0.004;
    int win_N = std::max(1, int(std::round(win_len / dt_used)));

    auto alphaFromFc = [dt_used](double fc) {
        constexpr double TWO_PI = 6.28318530717958647692;
        fc = std::max(fc, 1e-9);
        double tau = 1.0 / (TWO_PI * fc);
        return dt_used / (dt_used + tau);
    };

    auto alphaFromTau = [dt_used](double tau) {
        return dt_used / (tau + dt_used);
    };

    for (int i = 0; i < 3; ++i) {
        if (!inited_L[i]) {
            inited_L[i]    = true;
            xhat_L[i]      = raw[i];
            vhat_L[i]      = 0.0;
            ahat_L[i]      = 0.0;
            fc_smooth_L[i] = fmin;
            dispHist_L[i].clear();
            out[i] = xhat_L[i];
            continue;
        }

        // 1) 速度估计
        double vx = (raw[i] - xhat_L[i]) / dt_used;
        if (vel_dead > 0.0 && std::abs(vx) < vel_dead) vx = 0.0;

        double a_d = alphaFromFc(fd);
        double vprev = vhat_L[i];
        vhat_L[i] += a_d * (vx - vhat_L[i]);

        // 2) 加速度估计
        if (beta_a > 0.0) {
            double ax = (vhat_L[i] - vprev) / dt_used;
            ahat_L[i] += a_d * (ax - ahat_L[i]);
        } else {
            ahat_L[i] = 0.0;
        }

        // 3) 窗口累计位移
        double disp_inc = std::abs(raw[i] - xhat_L[i]);
        dispHist_L[i].push_back(disp_inc);
        if ((int)dispHist_L[i].size() > win_N)
            dispHist_L[i].pop_front();

        double disp_accum = 0.0;
        for (double d : dispHist_L[i]) disp_accum += d;

        // 4) 原始 fc
        double fc_raw = fmin
                      + beta   * std::abs(vhat_L[i])
                      + beta_a * std::abs(ahat_L[i])
                      + beta_d * disp_accum;
        if (fc_max > 0.0)
            fc_raw = std::min(fc_raw, fc_max);

        // 5) fc 平滑（快起慢降）
        double alpha_fc = (fc_raw > fc_smooth_L[i])
                          ? alphaFromTau(tau_up)
                          : alphaFromTau(tau_down);
        fc_smooth_L[i] += alpha_fc * (fc_raw - fc_smooth_L[i]);

        // 6) 位置一阶低通
        double a_p = alphaFromFc(fc_smooth_L[i]);
        xhat_L[i] += a_p * (raw[i] - xhat_L[i]);

        out[i] = xhat_L[i];
    }
    }

    if(side=='r'){
        const double dt_used = (std::isfinite(dt) && dt > 0.0) ? dt : 0.004;
        int win_N = std::max(1, int(std::round(win_len / dt_used)));

        auto alphaFromFc = [dt_used](double fc) {
            constexpr double TWO_PI = 6.28318530717958647692;
            fc = std::max(fc, 1e-9);
            double tau = 1.0 / (TWO_PI * fc);
            return dt_used / (dt_used + tau);
        };

        auto alphaFromTau = [dt_used](double tau) {
            return dt_used / (tau + dt_used);
        };

        for (int i = 0; i < 3; ++i) {
            if (!inited_R[i]) {
                inited_R[i]    = true;
                xhat_R[i]      = raw[i];
                vhat_R[i]      = 0.0;
                ahat_R[i]      = 0.0;
                fc_smooth_R[i] = fmin;
                dispHist_R[i].clear();
                out[i] = xhat_R[i];
                continue;
            }

            // 1) 速度估计
            double vx = (raw[i] - xhat_R[i]) / dt_used;
            if (vel_dead > 0.0 && std::abs(vx) < vel_dead) vx = 0.0;

            double a_d = alphaFromFc(fd);
            double vprev = vhat_R[i];
            vhat_R[i] += a_d * (vx - vhat_R[i]);

            // 2) 加速度估计
            if (beta_a > 0.0) {
                double ax = (vhat_R[i] - vprev) / dt_used;
                ahat_R[i] += a_d * (ax - ahat_R[i]);
            } else {
                ahat_R[i] = 0.0;
            }

            // 3) 窗口累计位移
            double disp_inc = std::abs(raw[i] - xhat_R[i]);
            dispHist_R[i].push_back(disp_inc);
            if ((int)dispHist_R[i].size() > win_N)
                dispHist_R[i].pop_front();

            double disp_accum = 0.0;
            for (double d : dispHist_R[i]) disp_accum += d;

            // 4) 原始 fc
            double fc_raw = fmin
                          + beta   * std::abs(vhat_R[i])
                          + beta_a * std::abs(ahat_R[i])
                          + beta_d * disp_accum;
            if (fc_max > 0.0)
                fc_raw = std::min(fc_raw, fc_max);

            // 5) fc 平滑（快起慢降）
            double alpha_fc = (fc_raw > fc_smooth_R[i])
                              ? alphaFromTau(tau_up)
                              : alphaFromTau(tau_down);
            fc_smooth_R[i] += alpha_fc * (fc_raw - fc_smooth_R[i]);

            // 6) 位置一阶低通
            double a_p = alphaFromFc(fc_smooth_R[i]);
            xhat_R[i] += a_p * (raw[i] - xhat_R[i]);

            out[i] = xhat_R[i];
    }
    }

    return out;
}

void RobotControl::calculateEndEffectorPosition_init(const HandlePose& handlePoseCur, const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side)//yu 通过各电机的读数推断端点在坐标系中的位置
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
        m_endEffectorInitPosition_R[1] = m_endArm_1 * sin(m_endEffectorInitJointAngle_R[0]) + m_endArm_2 * sin(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1]); //+ m_endArm_3 * sin(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1] + m_endEffectorInitJointAngle_R[2]);
        m_endEffectorInitPosition_R[2] = - m_endArm_1 * cos(m_endEffectorInitJointAngle_R[0]) - m_endArm_2 * cos(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1]); //- m_endArm_3 * cos(m_endEffectorInitJointAngle_R[0] + m_endEffectorInitJointAngle_R[1] + m_endEffectorInitJointAngle_R[2]);
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
std::array<double, 3> RobotControl::calculateEndEffectorPosition(const std::array<int,MotorNumPerSide>& motorPos_Cur, const char& side)//yu 通过各电机的读数推断端点在坐标系中的位置
{
    std::array<double, 3> endEffectorPosition = {0};
    if(side=='r'){
        /*计算关节角度*/
        double endEffectorJointAngle1_R = -(static_cast<double>(motorPos_Cur[Joint1_R]) - static_cast<double>(JointEncoderInit_1_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        double endEffectorJointAngle2_R = (static_cast<double>(motorPos_Cur[Joint2_R]) - static_cast<double>(JointEncoderInit_2_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        double endEffectorJointAngle3_R = -(static_cast<double>(motorPos_Cur[Joint3_R]) - static_cast<double>(JointEncoderInit_3_R)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

        /*计算使能初始位置*/
        endEffectorPosition[0] = motorPos_Cur[0]/4000;/*使用绝对编码器位置*/
        endEffectorPosition[1] = m_endArm_1 * sin(endEffectorJointAngle1_R) + m_endArm_2 * sin(endEffectorJointAngle1_R + endEffectorJointAngle2_R);
        endEffectorPosition[2] = - m_endArm_1 * cos(endEffectorJointAngle1_R) - m_endArm_2 * cos(endEffectorJointAngle1_R + endEffectorJointAngle2_R);
    }else if(side=='l')
    {
        /*计算关节角度*/
        double endEffectorJointAngle1_L = -(static_cast<double>(motorPos_Cur[Joint1_L]) - static_cast<double>(JointEncoderInit_1_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        double endEffectorJointAngle2_L = (static_cast<double>(motorPos_Cur[Joint2_L]) - static_cast<double>(JointEncoderInit_2_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
        double endEffectorJointAngle3_L = -(static_cast<double>(motorPos_Cur[Joint3_L]) - static_cast<double>(JointEncoderInit_3_L)) /
                            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

        /*计算使能位置*/
        endEffectorPosition[0] = motorPos_Cur[0]/4000.0;/*使用绝对编码器位置*/
        endEffectorPosition[1] = m_endArm_1 * sin(endEffectorJointAngle1_L) + m_endArm_2 * sin(endEffectorJointAngle1_L + endEffectorJointAngle2_L);
        endEffectorPosition[2] = - m_endArm_1 * cos(endEffectorJointAngle1_L) - m_endArm_2 * cos(endEffectorJointAngle1_L + endEffectorJointAngle2_L);
    }
    return endEffectorPosition;
}
std::array<int, MotorNumPerSide> RobotControl::calculateTargetEncoder(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                                       const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                                       const char& side)const
{
    std::array<int, MotorNumPerSide> targetEncoder = {0};
    if(side == 'r')
    {
        targetEncoder[0] = static_cast<int>(motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        for(int i = 1; i < 4; i++)
        {
            targetEncoder[i] = static_cast<int>(motorPosition_Init[i] + m_SpeedDirection_R[i] * controlValue_Cur[i] / 360 * JointEncoderPerRevolution);
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_kForcepPosition_small_R[13 - i]);
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {

             targetEncoder[4] = 0;
             targetEncoder[5] = 0;
             targetEncoder[9] = static_cast<int>(controlValue_Cur[9] * m_encoderPerDegree_R[3]);
             targetEncoder[8] = static_cast<int>(controlValue_Cur[8] * m_encoderPerDegree_R[2]);
             targetEncoder[7] = static_cast<int>(controlValue_Cur[7] * m_encoderPerDegree_R[1]);
             targetEncoder[6] = static_cast<int>(controlValue_Cur[6] * m_encoderPerDegree_R[0]);
        }
    }
    else if(side == 'l')
    {
        targetEncoder[0] = static_cast<int>(motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        for(int i = 1; i < 4; i++)
        {
            targetEncoder[i] = static_cast<int>(motorPosition_Init[i] + m_SpeedDirection_L[i] * controlValue_Cur[i] / 360 * JointEncoderPerRevolution);
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_kForcepPosition_small_L[13 - i]);
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {

            targetEncoder[4] =0;
            targetEncoder[5] =0;

            targetEncoder[9] = static_cast<int>(controlValue_Cur[9] * m_encoderPerDegree_R[3]);
            targetEncoder[8] = static_cast<int>(controlValue_Cur[8] * m_encoderPerDegree_R[2]);
            targetEncoder[7] = static_cast<int>(controlValue_Cur[7] * m_encoderPerDegree_R[1]);
            targetEncoder[6] = static_cast<int>(controlValue_Cur[6] * m_encoderPerDegree_R[0]);
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
        targetVel[i] = int((targetEncoderCur[i] - targetEncoderPrev[i]) / (0.005));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
    }
    return targetVel;
}

std::array<double, MotorNumPerSide> RobotControl::calculateTargetEncoder_new(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                                      const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                                      const char& side)const
{
    std::array<double, MotorNumPerSide> targetEncoder = {0};
    if(side == 'r')
    {
        targetEncoder[0] = (motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        double rounded = round(targetEncoder[0] * 1000.0) / 1000.0;
        targetEncoder[0] = rounded;

        targetEncoder[0] = (motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        for(int i = 1; i < 4; i++)
        {
            targetEncoder[i] = (motorPosition_Init[i] + m_SpeedDirection_R[i] * controlValue_Cur[i] / 360.0 * JointEncoderPerRevolution);
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = (controlValue_Cur[i] * m_kForcepPosition_small_R[13 - i]);
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {
            for(int i = 4; i < 8; i++)
            {
                targetEncoder[i] = (controlValue_Cur[i] * m_kForcepPosition_R[i - 4]);
            }
        }
    }
    else if(side == 'l')
    {
        targetEncoder[0] = (motorPosition_Init[0] + controlValue_Cur[0] * 4000);
        double rounded = round(targetEncoder[0] * 1000.0) / 1000.0;
        targetEncoder[0] = rounded;

        for(int i = 1; i < 4; i++)
        {
            targetEncoder[i] = (motorPosition_Init[i] + m_SpeedDirection_L[i] * controlValue_Cur[i] / 360.0 * JointEncoderPerRevolution);
            double rounded = round(targetEncoder[i] * 1000.0) / 1000.0;
            targetEncoder[i] = rounded;
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
        {
            for(int i = 4; i < 10; i++)
            {
                targetEncoder[i] = controlValue_Cur[i] * m_kForcepPosition_small_L[13 - i];
            }
        }
        if(m_endeffectorConfiguration == EndeffectorConfiguration::fourMaxons)
        {
            for(int i = 4; i < 8; i++)
            {
                targetEncoder[i] = (controlValue_Cur[i] * m_kForcepPosition_L[i - 4]);
            }
        }
    }

    return targetEncoder;
}

std::array<int, MotorNumPerSide> RobotControl::calculateTargetVelocity_new(const std::array<double, MotorNumPerSide>& targetEncoderCur,
                                                                        const std::array<double, MotorNumPerSide>& targetEncoderPrev,
                                                                        const std::array<int, MotorNumPerSide>& targetEncoder,
                                                                        const std::array<int, MotorNumPerSide>& motorEncoderCur,
                                                                        const char& side)
{
    std::array<int, MotorNumPerSide> targetVel = {0};

    if(side == 'l'){
    for(int i = 0; i < 9; i++)
    {
        targetVel[i] = static_cast<int>((targetEncoderCur[i] - targetEncoderPrev[i]) / (0.005));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
    }

    /*jacobi velocity*/

      targetVel[0] = m_jointAngle0_velocity_L;
      targetVel[1] = m_jointAngle1_velocity_L;
      targetVel[2] = m_jointAngle2_velocity_L;
      targetVel[3] = m_jointAngle3_velocity_L;

    // double K_p = 1.0;
    // double K_d = 0.05;
    // double d_t = 0.005;
    double K_p = 1.0;
    double K_d = 0.03;
    double d_t = 0.005;

    std::array<int, MotorNumPerSide> delt_Encoder{0};
    std::array<int, MotorNumPerSide> EncodeErr_Cur {0};

        for(int i = 0; i < 4; i++){
            EncodeErr_Cur[i] = targetEncoder[i] - motorEncoderCur[i];
            delt_Encoder[i] = K_p * EncodeErr_Cur[i] + K_d * (EncodeErr_Cur[i] - m_EncodeErr_Pre_L[i]) / d_t;
            // if(EncodeErr_Cur[i] > 10){
                targetVel[i] = targetVel[i] + delt_Encoder[i];
            // }
            m_EncodeErr_Pre_L[i] = EncodeErr_Cur[i];
        }

    }

    if(side == 'r'){

        for(int i = 0; i < 9; i++)
        {
            targetVel[i] = static_cast<int>((targetEncoderCur[i] - targetEncoderPrev[i]) / (0.005));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
        }
        /*jacobi velocity*/

          targetVel[0] = m_jointAngle0_velocity_R;
          targetVel[1] = m_jointAngle1_velocity_R;
          targetVel[2] = m_jointAngle2_velocity_R;
          targetVel[3] = m_jointAngle3_velocity_R;

        double K_p = 1.0;
        double K_d = 0.03;
        double d_t = 0.005;

        std::array<int, MotorNumPerSide> delt_Encoder{0};
        std::array<int, MotorNumPerSide> EncodeErr_Cur {0};

        for(int i = 0; i < 4; i++){
            EncodeErr_Cur[i] = targetEncoder[i] - motorEncoderCur[i];
            delt_Encoder[i] = K_p * EncodeErr_Cur[i] + K_d * (EncodeErr_Cur[i] - m_EncodeErr_Pre_R[i]) / d_t;
            // if(EncodeErr_Cur[i] > 10){
                targetVel[i] = targetVel[i] + delt_Encoder[i];
            // }
            m_EncodeErr_Pre_R[i] = EncodeErr_Cur[i];
        }
    }
    return targetVel;
}

/* 接受传回来的数据 */
void RobotControl::receiveMotorData()
{
    // std::lock_guard(m_motorDriver->m_cyclicMutex);

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
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorEncoderData_R[8] = m_motorDriver->getActualPos(MotorType::MAXON, 4, arm_0);
        motorEncoderData_R[9] = m_motorDriver->getActualPos(MotorType::MAXON, 5, arm_0);
    // }

    motorEncoderData_L[0] = m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_1);
    motorEncoderData_L[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_1);
    motorEncoderData_L[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_1);
    motorEncoderData_L[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_1);
    motorEncoderData_L[4] = m_motorDriver->getActualPos(MotorType::MAXON, 0, arm_1);
    motorEncoderData_L[5] = m_motorDriver->getActualPos(MotorType::MAXON, 1, arm_1);
    motorEncoderData_L[6] = m_motorDriver->getActualPos(MotorType::MAXON, 2, arm_1);
    motorEncoderData_L[7] = m_motorDriver->getActualPos(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorEncoderData_L[8] = m_motorDriver->getActualPos(MotorType::MAXON, 4, arm_1);
        motorEncoderData_L[9] = m_motorDriver->getActualPos(MotorType::MAXON, 5, arm_1);
    // }

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
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorErrorCode_R[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_0);
        motorErrorCode_R[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_0);
    // }

    motorErrorCode_L[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_1);
    motorErrorCode_L[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_1);
    motorErrorCode_L[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_1);
    motorErrorCode_L[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_1);
    motorErrorCode_L[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_1);
    motorErrorCode_L[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_1);
    motorErrorCode_L[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_1);
    motorErrorCode_L[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorErrorCode_L[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_1);
        motorErrorCode_L[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_1);
    // }

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
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorStatusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        motorStatusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);
    // }

    motorStatusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
    motorStatusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
    motorStatusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
    motorStatusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
    motorStatusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
    motorStatusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
    motorStatusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
    motorStatusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorStatusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        motorStatusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
    // }

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
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorOperationMode_R[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        motorOperationMode_R[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);
    // }

    motorOperationMode_L[0] =  m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
    motorOperationMode_L[1] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
    motorOperationMode_L[2] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
    motorOperationMode_L[3] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
    motorOperationMode_L[4] =  m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
    motorOperationMode_L[5] =  m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
    motorOperationMode_L[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
    motorOperationMode_L[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorOperationMode_L[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        motorOperationMode_L[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);
    // }
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
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorInputs_R[8] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4, arm_0)[2];
        motorInputs_R[9] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5, arm_0)[2];
    // }
    motorInputs_L[0] = m_motorDriver->getDigitalInputs(MotorType::MOONS, 0, arm_1)[6];
    motorInputs_L[4] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 0, arm_1)[2];
    motorInputs_L[5] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 1, arm_1)[2];
    motorInputs_L[6] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 2, arm_1)[2];
    motorInputs_L[7] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 3, arm_1)[2];
        motorInputs_L[8] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4, arm_1)[2];
        motorInputs_L[9] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5, arm_1)[2];

    /*磁栅尺*/

    auto magneticEncoder_L = m_domainController->getMagneticScale(1);
    auto magneticEncoder_R = m_domainController->getMagneticScale(2);

    // LOG(INFO)<<"magneticEncoder_L: "<<magneticEncoder_L<<" magneticEncoder_R: "<<magneticEncoder_R;
    /*力*/
    auto force_l = m_domainController->getForce(1,m_armAnglePerSide);
    auto force_r = m_domainController->getForce(0,m_armAnglePerSide);


    /*digital*/
    auto domainDigtalPrev_L = m_domainDigtalPre_L;
    auto domainDigtalPrev_R = m_domainDigtalPre_R;

    auto domainDigitalCur_L = m_domainController->getDigitalInput(1);
    auto domainDigitalCur_R = m_domainController->getDigitalInput(2);

    setEndJointDragEnableStatus(domainDigitalCur_L, domainDigitalCur_R);

    m_prevRobotControlMode.store(m_curRobotControlMode.load());


/*new*/
    std::array<int, MotorNumPerSide> motorTrq_R = {0};
    std::array<int, MotorNumPerSide> motorTrq_L = {0};

    // motorTrq_L[0] =  m_motorDriver->getActualTrq(MotorType::MOONS, 0, arm_1);
    motorTrq_L[1] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 0, arm_1);
    motorTrq_L[2] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 1, arm_1);
    motorTrq_L[3] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 2, arm_1);
    // motorTrq_L[4] =  m_motorDriver->getActualTrq(MotorType::MAXON, 0, arm_1);
    // motorTrq_L[5] =  m_motorDriver->getActualTrq(MotorType::MAXON, 1, arm_1);
    // motorTrq_L[6] =  m_motorDriver->getActualTrq(MotorType::MAXON, 2, arm_1);
    // motorTrq_L[7] =  m_motorDriver->getActualTrq(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
    //     motorTrq_L[6] =  m_motorDriver->getActualTrq(MotorType::MAXON, 4, arm_1);
    //     motorTrq_L[7] =  m_motorDriver->getActualTrq(MotorType::MAXON, 5, arm_1);
    // }


    std::array<int, MotorNumPerSide> motorCur_R = {0};
    std::array<int, MotorNumPerSide> motorCur_L = {0};

    motorCur_L[0] =  m_motorDriver->getActualCur(MotorType::MOONS, 0, arm_1);
    motorCur_L[1] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 0, arm_1);
    motorCur_L[2] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 1, arm_1);
    motorCur_L[3] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 2, arm_1);
    motorCur_L[4] =  m_motorDriver->getActualCur(MotorType::MAXON, 0, arm_1);
    motorCur_L[5] =  m_motorDriver->getActualCur(MotorType::MAXON, 1, arm_1);
    motorCur_L[6] =  m_motorDriver->getActualCur(MotorType::MAXON, 2, arm_1);
    motorCur_L[7] =  m_motorDriver->getActualCur(MotorType::MAXON, 3, arm_1);
    // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
    // {
        motorCur_L[6] =  m_motorDriver->getActualCur(MotorType::MAXON, 4, arm_1);
        motorCur_L[7] =  m_motorDriver->getActualCur(MotorType::MAXON, 5, arm_1);
    // }


    std::array<int, MotorNumPerSide> motorFollowingPosErr_R = {0};
    std::array<int, MotorNumPerSide> motorFollowingPosErr_L = {0};

    // motorFollowingPosErr_L[0] =  m_motorDriver->getFollowingPosErr(MotorType::MOONS, 0, arm_1);
    motorFollowingPosErr_L[1] =  m_motorDriver->getFollowingPosErr(MotorType::ZERO_ERR, 0, arm_1);
    motorFollowingPosErr_L[2] =  m_motorDriver->getFollowingPosErr(MotorType::ZERO_ERR, 1, arm_1);
    motorFollowingPosErr_L[3] =  m_motorDriver->getFollowingPosErr(MotorType::ZERO_ERR, 2, arm_1);

    m_motorEncoderCur_R.store(motorEncoderData_R);
    m_motorEncoderCur_L.store(motorEncoderData_L);

    m_motorStatusWordCur_R.store(motorStatusWord_R);
    m_motorStatusWordCur_L.store(motorStatusWord_L);

    m_digitalInputGuiding.store(motorInputs_Guiding);

    m_motorHomingStatus_L.store(motorInputs_L);//记录光电门是否被激活，为0时表示被激活
    m_motorHomingStatus_R.store(motorInputs_R);//记录光电门是否被激活，为0时表示被激活

    // LOG(INFO)<<" L_f: "<<motorInputs_L[4]<<" L_e: "<<motorInputs_L[5]<<" L_d: "<<motorInputs_L[6]<<" L_c: "<<motorInputs_L[7]<<" L_b: "<<motorInputs_L[8]<<" L_a: "<<motorInputs_L[9];

    // LOG(INFO)<<" r_f: "<<motorInputs_R[4]<<" r_e: "<<motorInputs_R[5]<<" r_d: "<<motorInputs_R[6]<<" r_c: "<<motorInputs_R[7]<<" r_b: "<<motorInputs_R[8]<<" r_a: "<<motorInputs_R[9];

    m_motorOperationMode_L.store(motorOperationMode_L);
    m_motorErrorCode_L.store(motorErrorCode_L);
    m_motorTrq_L.store(motorTrq_L);
    m_motorCur_L.store(motorCur_L);
    m_motorFollowingPosErr_L.store(motorFollowingPosErr_L);

    m_magneticEncoder_L.store(magneticEncoder_L);
    m_magneticEncoder_R.store(magneticEncoder_R);

    m_force_L.store(force_l);
    m_force_R.store(force_r);

    m_domainDigtalPre_L = domainDigitalCur_L;
    m_domainDigtalPre_R = domainDigitalCur_R;

}

void RobotControl::setEndJointDragEnableStatus(const uint8_t& domainDigitalCur_L, const uint8_t& domainDigitalCur_R)
{
    // LOG(INFO)<<"DRAG_L: "<<static_cast<int>(domainDigitalCur_L)<<" DRAG_R: "<<static_cast<int>(domainDigitalCur_R);
    if(static_cast<int>(domainDigitalCur_L) == 5)
    {
        m_endJointDragBtnCounter_L++;
    }else if(static_cast<int>(domainDigitalCur_L) != 5)
    {
        m_endJointDragBtnCounter_L = 0;
        m_dragButtonPressCur_L = 0;
    }

    if(static_cast<int>(domainDigitalCur_R) == 5)
    {
        m_endJointDragBtnCounter_R++;
    }else if(static_cast<int>(domainDigitalCur_R) != 5)
    {
        m_endJointDragBtnCounter_R = 0;
        m_dragButtonPressCur_R = 0;
    }

    /*如果按下时间大于100*/
    if(m_endJointDragBtnCounter_L >= 100)
    {
        m_endJointDragBtnCounter_L = 100;
        m_dragButtonPressCur_L = 1;
    }
    if(m_endJointDragBtnCounter_R >= 100)
    {
        m_endJointDragBtnCounter_R = 100;
        m_dragButtonPressCur_R = 1;
    }
    // LOG(INFO)<<"m_dragButtonPressCur_L: "<<m_dragButtonPressCur_L<<" m_endJointDragBtnCounter_L: "<<m_endJointDragBtnCounter_L<<"m_dragButtonPressCur_R: "<<m_dragButtonPressCur_R<<" m_endJointDragBtnCounter_R: "<<m_endJointDragBtnCounter_R;
    // LOG(INFO)<<"m_dragButtonPressCur_R: "<<m_dragButtonPressCur_R<<" domainDigitalCur_R: "<< static_cast<int>(domainDigitalCur_R) << "  counter: " << m_endJointDragBtnCounter_R;
}

void RobotControl::sendMotorData(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                 const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L)
{
    std::lock_guard(m_motorDriver->m_cyclicMutex);

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_R[0], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_R[1], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_R[2], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_R[3], arm_0);

    if(m_enableTagCur_R == enableAction || m_enableTagCur_R == keepEnabling || m_enableTagCur_R == disableAction || m_enableTagCur_R == keepDisabling){
        m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoder_R[4], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoder_R[5], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoder_R[6], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoder_R[7], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoder_R[8], arm_0);
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoder_R[9], arm_0);
    }

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_L[0], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_L[1], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_L[2], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_L[3], arm_1);

    if(m_enableTagCur_L == enableAction || m_enableTagCur_L == keepEnabling || m_enableTagCur_L == disableAction || m_enableTagCur_L == keepDisabling){
        m_motorDriver->setTargetPos(MotorType::MAXON, 0, targetEncoder_L[4], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 1, targetEncoder_L[5], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 2, targetEncoder_L[6], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 3, targetEncoder_L[7], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, targetEncoder_L[8], arm_1);
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, targetEncoder_L[9], arm_1);
    }

}

void RobotControl::sendMotorData_4Maxons_ForceControl(const std::array<int, MotorNumPerSide>& targetEncoder_R, const std::array<int, MotorNumPerSide>& targetVel_R,
                                 const std::array<int, MotorNumPerSide>& targetEncoder_L, const std::array<int, MotorNumPerSide>& targetVel_L)
{

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_R[0], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_R[1], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_R[2], arm_0);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_R[3], arm_0);

    m_motorDriver->setTargetVel(MotorType::MOONS, 0, targetVel_L[0], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_L[1], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_L[2], arm_1);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_L[3], arm_1);

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
                // m_handlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_R = motorTargetEncoder_R;
                m_compensation_LastR = m_compensation_CurR;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'r'))
        {
            if((m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling || m_enableTagPrev_R == ForceDisable) && (m_dragButtonPressCur_R + m_dragButtonPressCur_L == 0)
                && masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == true)
            {
                enableFlag = enableAction;
                // m_handlePoseInit_R = masterHandlePose_Cur;
            }
            else if ((m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling || m_enableTagPrev_R == ForceDisable) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'r') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
                enableFlag = keepDisabling;
            }
            else if ((m_enableTagPrev_R == enableAction || m_enableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                // m_handlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_R = motorTargetEncoder_R;
                m_compensation_LastR = m_compensation_CurR;
            }
            else if ((m_enableTagPrev_R == enableAction || m_enableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
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
                // m_handlePoseLastLoop_L = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_L = motorTargetEncoder_L;
                m_compensation_LastL = m_compensation_CurL;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'l'))
        {
            if((m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling || m_enableTagPrev_L == ForceDisable) && (m_dragButtonPressCur_R + m_dragButtonPressCur_L == 0)
                && masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == true)
            {
                enableFlag = enableAction;
                // m_handlePoseInit_L = masterHandlePose_Cur;
            }
            else if ((m_enableTagPrev_L == disableAction || m_enableTagPrev_L == keepDisabling || m_enableTagPrev_L == ForceDisable) &&
                     ((masterHandlePose_Cur.enablePedal == pedalEnable && isPoseMatch(masterHandlePose_Cur,'l') == false)||(masterHandlePose_Cur.enablePedal == pedalDisable)))
            {
                enableFlag = keepDisabling;
            }
            else if ((m_enableTagPrev_L == enableAction || m_enableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalDisable)
            {
                // m_handlePoseLastLoop_L = masterHandlePose_Cur;
                enableFlag = disableAction;
                m_motorTargetEncoderLast_L = motorTargetEncoder_L;
                m_compensation_LastL = m_compensation_CurL;
            }
            else if ((m_enableTagPrev_L == enableAction || m_enableTagPrev_L == keepEnabling) &&
                     masterHandlePose_Cur.enablePedal == pedalEnable)
            {
                enableFlag = keepEnabling;
            }
        }
    }

    /*力模式*/

    if(side == 'r'){

        if(m_dragButtonPressCur_R == 1)
        {
            judgePositonForceControl('r');/*change the delt*/
            if(!isForcePositionRight(masterHandlePose_Cur, 'r')){/*不满足力控制条件*/
                if(m_enableTagPrev_R == ForceDisable || m_enableTagPrev_R == keepDisabling)
                {
                    enableFlag = keepDisabling;
                }
                if(m_enableTagPrev_R == ForceEnable || m_enableTagPrev_R == KeepForce)
                {
                // m_handlePoseLastLoop_L = masterHandlePose_Cur;
                    enableFlag = ForceDisable;
                    for(int i=0; i<4 ; i++){
                    // m_motorTargetEncoderLast_R[i] = motorTargetEncoder_R[i];
                     }
                }
            }
            else if(isForcePositionRight(masterHandlePose_Cur, 'l'))
            {
                if(m_dragButtonPressPre_R == 0 && m_enableTagPrev_R == keepDisabling)
                {
                    enableFlag = ForceEnable;
                }
                else if (m_dragButtonPressPre_R == 1 && (m_enableTagPrev_R == ForceEnable || m_enableTagPrev_R == KeepForce))
                {
                    enableFlag = KeepForce;
                }
            }
        }
        else if(m_dragButtonPressCur_R == 0)
        {
            if(m_dragButtonPressPre_R == 1 && (m_enableTagPrev_R == KeepForce || m_enableTagPrev_R == ForceEnable)){
                enableFlag = ForceDisable;
                for(int i=0; i<4 ; i++){
                   // m_motorTargetEncoderLast_R[i] = motorTargetEncoder_R[i];
                }

            }
        }
        m_dragButtonPressPre_R = m_dragButtonPressCur_R;
    }


    else if(side == 'l'){

        if(m_dragButtonPressCur_L == 1)
        {
            judgePositonForceControl('l');
            if(!isForcePositionRight(masterHandlePose_Cur, 'l')){/*不满足力控制条件*/
                if(m_enableTagPrev_L == ForceDisable || m_enableTagPrev_L == keepDisabling)
                {
                    enableFlag = keepDisabling;
                }
                if(m_enableTagPrev_L == ForceEnable || m_enableTagPrev_L == KeepForce)
                {
                    enableFlag = ForceDisable;
                    for(int i=0; i<4 ; i++){
                       m_motorTargetEncoderLast_L[i] = motorTargetEncoder_L[i];
                    }
                }
            }
            else if(isForcePositionRight(masterHandlePose_Cur, 'l'))
            {
                if(m_dragButtonPressPre_L == 0 && m_enableTagPrev_L == keepDisabling)
                {
                    enableFlag = ForceEnable;
                }
                else if (m_dragButtonPressPre_L == 1 && (m_enableTagPrev_L == ForceEnable || m_enableTagPrev_L == KeepForce))
                {
                    enableFlag = KeepForce;
                }
            }
        }
        else if(m_dragButtonPressCur_L == 0)
        {
            if(m_dragButtonPressPre_L == 1 && (m_enableTagPrev_L == KeepForce || m_enableTagPrev_L == ForceEnable)){
                enableFlag = ForceDisable;
                for(int i=0; i<4; i++){
                //    m_motorTargetEncoderLast_L[i] = motorTargetEncoder_L[i];
                }
            }
        }
       m_dragButtonPressPre_L = m_dragButtonPressCur_L;
    }

    return enableFlag;
}

bool RobotControl::isPoseRight(const HandlePose& masterHandlePose_Cur, const char& side) const
{
    if(m_oprationMode == 2 || m_oprationMode == 3){
        return true;
    }
    // auto handlePoseCur = m_masterConsole.returnHandlePose();
    auto handlePoseCur = masterHandlePose_Cur;

    auto motorEncoder_R = m_motorEncoderCur_R.load();
    auto motorEncoder_L = m_motorEncoderCur_L.load();

    auto moonsRelPosition_R = (motorEncoder_R[0] - m_moonsEncodeZero_R) / 4000;
    auto moonsRelPosition_L = (motorEncoder_L[0] - m_moonsEncodeZero_L) / 4000;

    // LOG(INFO)<<std::dec<<" moonsRelPosition_L: "<<moonsRelPosition_L<<" moonsRelPosition_R: "<<moonsRelPosition_R;

    /*右手*/
    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];

    double endEffectorDelta_X_R = (handlePoseCur.handlePoseR_X - m_handlePoseInit_R.handlePoseR_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = -(handlePoseCur.handlePoseR_Y - m_handlePoseInit_R.handlePoseR_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = -(handlePoseCur.handlePoseR_Z - m_handlePoseInit_R.handlePoseR_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_R = endEffectorDelta_X_R + endEffectorInit_X_R;//X用磁栅尺限位
    double endEffectorTarget_Y_R = endEffectorDelta_Y_R + endEffectorInit_Y_R + m_endArm_3 * sin(m_armAnglePerSide / 180.0 * M_PI);
    double endEffectorTarget_Z_R = endEffectorDelta_Z_R + endEffectorInit_Z_R - m_endArm_3 * cos(m_armAnglePerSide / 180.0 * M_PI);

    /*左手*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];

    double endEffectorDelta_X_L = (handlePoseCur.handlePoseL_X - m_handlePoseInit_L.handlePoseL_X) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = -(handlePoseCur.handlePoseL_Y - m_handlePoseInit_L.handlePoseL_Y) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = -(handlePoseCur.handlePoseL_Z - m_handlePoseInit_L.handlePoseL_Z) / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_L = endEffectorDelta_X_L + endEffectorInit_X_L;//X用磁栅尺限位
    double endEffectorTarget_Y_L = endEffectorDelta_Y_L + endEffectorInit_Y_L + m_endArm_3 * sin(-m_armAnglePerSide / 180.0 * M_PI);
    double endEffectorTarget_Z_L = endEffectorDelta_Z_L + endEffectorInit_Z_L - m_endArm_3 * cos(-m_armAnglePerSide / 180.0 * M_PI);

    // LOG(INFO)<<" endEffectorTarget_Y_L: "<<endEffectorTarget_Y_L<<" endEffectorTarget_Z_L: "<<endEffectorTarget_Z_L<<" endEffectorTarget_Y_R: "<<endEffectorTarget_Y_R<<" endEffectorTarget_Z_R: "<<endEffectorTarget_Z_R;

    // LOG(INFO)<<" endEffectorTarget_Y_L: "<<endEffectorTarget_Y_L<<" endEffectorInit_Y_L: "<<endEffectorInit_Y_L<<" arm3_y: "<<m_endArm_3 * sin(-m_armAnglePerSide / 180.0 * M_PI)<<" endEffectorTarget_Z_L: "<<endEffectorTarget_Z_L<<" endEffectorInit_Z_L: "<<endEffectorInit_Z_L<<" arm3_Z: "<<- m_endArm_3 * cos(-m_armAnglePerSide / 180.0 * M_PI);

     if(side =='l'){

        if (handlePoseCur.handlePoseL_Roll * 180 / M_PI < 143 && handlePoseCur.handlePoseL_Roll * 180 / M_PI > -143){

        if (handlePoseCur.handlePoseL_Elevation * 180 / M_PI < 75 && handlePoseCur.handlePoseL_Elevation * 180 / M_PI > -75){

             if (handlePoseCur.handlePoseL_Arzimuth * 180 / M_PI < 130 && handlePoseCur.handlePoseL_Arzimuth * 180 / M_PI > -130){

                 if((masterHandlePose_Cur.handlePoseL_X > -55) && (masterHandlePose_Cur.handlePoseL_X < 80)){

                     if((masterHandlePose_Cur.handlePoseL_Y > -50) && (masterHandlePose_Cur.handlePoseL_Y < 50)){

                         if((masterHandlePose_Cur.handlePoseL_Z > -80) && (masterHandlePose_Cur.handlePoseL_Z < -8)){

                             if(((endEffectorTarget_Y_L < -226) && (handlePoseCur.handlePoseL_Y - m_handlePosePrev.handlePoseL_Y) < 0) || (endEffectorTarget_Y_L >= -226)){/*Viper与机械臂坐标系yz轴相反*/

                                 if(((endEffectorTarget_Y_L > -126) && (handlePoseCur.handlePoseL_Y - m_handlePosePrev.handlePoseL_Y) > 0) || (endEffectorTarget_Y_L <= -126)){

                                     if(((endEffectorTarget_Z_L < -580) && (handlePoseCur.handlePoseL_Z - m_handlePosePrev.handlePoseL_Z) < 0) || (endEffectorTarget_Z_L >= -580)){

                                         if(((endEffectorTarget_Z_L > -400) && (handlePoseCur.handlePoseL_Z - m_handlePosePrev.handlePoseL_Z) > 0) || (endEffectorTarget_Z_L <= -400)){

                                             // if(((moonsRelPosition_L < -30) && (handlePoseCur.handlePoseL_X - m_handlePosePrev.handlePoseL_X) > 0) || (moonsRelPosition_L >= -30)){

                                             //     if(((moonsRelPosition_L > 30) && (handlePoseCur.handlePoseL_X - m_handlePosePrev.handlePoseL_X) < 0) || (moonsRelPosition_L <= 30)){
                                                    return true;
                                             //     }
                                             // }
                                         }
                                     }
                                 }
                             }


                         }
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
         // LOG(INFO)<<std::dec<<" moonsRelPosition_R---------------------------------------------------------------------------------: "<<moonsRelPosition_R;
return true;
        if (handlePoseCur.handlePoseR_Roll * 180 / M_PI < 143 && handlePoseCur.handlePoseR_Roll * 180 / M_PI > -143){//67

        if (handlePoseCur.handlePoseR_Elevation * 180 / M_PI < 75  && handlePoseCur.handlePoseR_Elevation * 180 / M_PI > -75){

             if (handlePoseCur.handlePoseR_Arzimuth * 180 / M_PI < 130 && handlePoseCur.handlePoseR_Arzimuth * 180 / M_PI > -130){

                 if((masterHandlePose_Cur.handlePoseR_X > -55) && (masterHandlePose_Cur.handlePoseR_X < 80)){

                     if((masterHandlePose_Cur.handlePoseR_Y > -50) && (masterHandlePose_Cur.handlePoseR_Y < 50)){

                         if((masterHandlePose_Cur.handlePoseR_Z > -80) && (masterHandlePose_Cur.handlePoseR_Z < -8)){

                             if(((endEffectorTarget_Y_R < 126) && (handlePoseCur.handlePoseR_Y - m_handlePosePrev.handlePoseR_Y) < 0) || (endEffectorTarget_Y_R >= 126)){

                                 if(((endEffectorTarget_Y_R > 226) && (handlePoseCur.handlePoseR_Y - m_handlePosePrev.handlePoseR_Y) > 0) || (endEffectorTarget_Y_R <= 226)){

                                     if(((endEffectorTarget_Z_R < -580) && (handlePoseCur.handlePoseR_Z - m_handlePosePrev.handlePoseR_Z) < 0) || (endEffectorTarget_Z_R >= -580)){

                                         if(((endEffectorTarget_Z_R > -400) && (handlePoseCur.handlePoseR_Z - m_handlePosePrev.handlePoseR_Z) > 0) || (endEffectorTarget_Z_R <= -400)){

                                             // if(((moonsRelPosition_R < -30) && (handlePoseCur.handlePoseR_X - m_handlePosePrev.handlePoseR_X) > 0) || (moonsRelPosition_R >= -30)){

                                             //     if(((moonsRelPosition_R > 30) && (handlePoseCur.handlePoseR_X - m_handlePosePrev.handlePoseR_X) < 0) || (moonsRelPosition_R <= 30)){

                                                    return true;
                                             //     }
                                             // }
                                         }
                                     }
                                 }
                             }


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

}

void RobotControl::judgePositonForceControl(const char& side){
    if(side=='l'){

        // auto forceValue = m_domainController->getForce(1,m_armAnglePerSide);
        auto forceValue = m_force_L.load();

        for(int i = 0; i < 3; i++){
            if(forceValue[i] > 4){
                forceValue[i] = 4;
            }else if(forceValue[i] < -4){
                forceValue[i] = -4;
            }
        }
        m_forceAccumulationBufferLeft_judge = {
            m_forceAccumulationBufferLeft_judge[0] + forceValue[0] * 0.025,//* 1500.0
            m_forceAccumulationBufferLeft_judge[1] + forceValue[1] * 0.025,//* 1500.0
            m_forceAccumulationBufferLeft_judge[2] + forceValue[2] * 0.025};//* 1500.0
    }

    if(side=='r'){

        // auto forceValue = m_domainController->getForce(0,m_armAnglePerSide);
        auto forceValue = m_force_R.load();

        for(int i = 0; i < 3; i++){
            if(forceValue[i] > 4){
                forceValue[i] = 4;
            }else if(forceValue[i] < -4){
                forceValue[i] = -4;
            }
        }
        m_forceAccumulationBufferRight_judge = {
            m_forceAccumulationBufferRight_judge[0] + forceValue[0] * 0.025,//* 1500.0
            m_forceAccumulationBufferRight_judge[1] + forceValue[1] * 0.025,//* 1500.0
            m_forceAccumulationBufferRight_judge[2] + forceValue[2] * 0.025};//* 1500.0
    }
}

bool RobotControl::isForcePositionRight(const HandlePose& handlePoseCur, const char& side)//yu 且要满足flag_openangle等于true时（false为status=3时openangle太小）
{
    // auto handlePoseCur = m_masterConsole.returnHandlePose();

    auto forceValue_L = m_force_L.load();
    auto forceValue_R = m_force_R.load();
    // LOG(INFO)<<"m_endJointDragBtnCounter_R: "<<m_endJointDragBtnCounter_R<<" m_endJointDragBtnCounter_L:"<<m_endJointDragBtnCounter_L<<"forceValue_L: "<<forceValue_L<<" forceValue_R: "<<forceValue_R;

    /*右手*/
    double endEffectorInit_X_R = m_endEffectorInitPosition_R[0];
    double endEffectorInit_Y_R = m_endEffectorInitPosition_R[1];
    double endEffectorInit_Z_R = m_endEffectorInitPosition_R[2];

    double endEffectorDelta_X_R = m_forceAccumulationBufferRight[0] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_R = m_forceAccumulationBufferRight[1] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_R = m_forceAccumulationBufferRight[2] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_R = endEffectorDelta_X_R + endEffectorInit_X_R;//X用磁栅尺限位
    double endEffectorTarget_Y_R = endEffectorDelta_Y_R + endEffectorInit_Y_R + m_endArm_3 * sin(m_armAnglePerSide / 180.0 * M_PI);
    double endEffectorTarget_Z_R = endEffectorDelta_Z_R + endEffectorInit_Z_R - m_endArm_3 * cos(m_armAnglePerSide / 180.0 * M_PI);

    /*左手*/
    double endEffectorInit_X_L = m_endEffectorInitPosition_L[0];
    double endEffectorInit_Y_L = m_endEffectorInitPosition_L[1];
    double endEffectorInit_Z_L = m_endEffectorInitPosition_L[2];

    double endEffectorDelta_X_L = m_forceAccumulationBufferLeft_judge[0] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Y_L = m_forceAccumulationBufferLeft_judge[1] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/
    double endEffectorDelta_Z_L = m_forceAccumulationBufferLeft_judge[2] / 0.1 / m_motionScaling[m_speedPedalIndex_Cur];/*unit: mm*/

    double endEffectorTarget_X_L = endEffectorDelta_X_L + endEffectorInit_X_L;//X用磁栅尺限位
    double endEffectorTarget_Y_L = endEffectorDelta_Y_L + endEffectorInit_Y_L + m_endArm_3 * sin(-m_armAnglePerSide / 180.0 * M_PI);
    double endEffectorTarget_Z_L = endEffectorDelta_Z_L + endEffectorInit_Z_L - m_endArm_3 * cos(-m_armAnglePerSide / 180.0 * M_PI);

    // LOG(INFO)<<" judge: endEffectorTarget_Y_L: "<<endEffectorTarget_Y_L<<" endEffectorTarget_Z_L: "<<endEffectorTarget_Z_L<<" endEffectorTarget_Y_R: "<<endEffectorTarget_Y_R<<" endEffectorTarget_Z_R: "<<endEffectorTarget_Z_R;
    // LOG(INFO)<<" judge: endEffectorTarget_Y_L: "<<endEffectorTarget_Y_L<<" endEffectorTarget_Z_L: "<<endEffectorTarget_Z_L;

     if(side =='l'){/*y_init:-177 z_init:-520*/

         if(((endEffectorTarget_Y_L < -276) && (forceValue_L[1] > 0)) || (endEffectorTarget_Y_L > -276)){
             // LOG(INFO)<<"m_domainController->getForce(1,m_armAnglePerSide)[1]: " << m_domainController->getForce(1,m_armAnglePerSide)[1]<< " m_forceAccumulationBufferLeft_judge_PRE[1]: "<< m_forceAccumulationBufferLeft_judge_pre[1];

             if(((endEffectorTarget_Y_L > -56) && (forceValue_L[1] < 0)) || (endEffectorTarget_Y_L < -56)){

                 if(((endEffectorTarget_Z_L < -580) && (forceValue_L[2] > 0)) || (endEffectorTarget_Z_L > -580)){

                     if(((endEffectorTarget_Z_L > -420) && (forceValue_L[2] < 0)) || (endEffectorTarget_Z_L < -420)){

         return true;
                     }
                 }
             }
         }

        else{
            return false;
        }
     }

     if(side =='r'){

         if(((endEffectorTarget_Y_R < 126) && (forceValue_R[1] > 0)) || (endEffectorTarget_Y_R > 126)){

             if(((endEffectorTarget_Y_R > 276) && (forceValue_R[1] < 0)) || (endEffectorTarget_Y_R < 276)){

                 if(((endEffectorTarget_Z_R < -580) && (forceValue_R[2] > 0)) || (endEffectorTarget_Z_R > -580)){

                     if(((endEffectorTarget_Z_R > -420) && (forceValue_R[2] < 0)) || (endEffectorTarget_Z_R < -420)){

                         return true;
                     }
                 }
             }
         }

        else{
            return false;
        }

     }
}

bool RobotControl::isPoseMatch(const HandlePose& masterHandlePose_Cur, const char& side) const//yu
{
  if(m_oprationMode == 2 || m_oprationMode == 3)
  {
      return true;
  }
    if (side == 'l')
    {
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

void RobotControl::setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side)//yu
{
    // m_ruckigInputState_R.max_velocity = {200000.0, 200000.0, 200000.0};//500000.0
    // m_ruckigInputState_R.max_acceleration = {10000.0, 12000.0, 14000.0};//15000.0
    // m_ruckigInputState_R.max_jerk = {3000.0, 4000.0, 5000.0};//8000.0

    // m_ruckigInputState_L.max_velocity = {200000.0, 200000.0, 200000.0};
    // m_ruckigInputState_L.max_acceleration = {10000.0, 12000.0, 14000.0};
    // m_ruckigInputState_L.max_jerk = {3000.0, 4000.0, 5000.0};


    switch(m_speedPedalIndex_Cur){
    // case pedalSwitchOne:

    //     m_ruckigInputState_R.max_velocity = {45000.0, 45000.0, 45000.0};//500000.0
    //     m_ruckigInputState_R.max_acceleration = {25000.0, 25000.0, 25000.0};//15000.0
    //     m_ruckigInputState_R.max_jerk = {15000.0, 15000.0, 15000.0};//8000.0

    //     m_ruckigInputState_L.max_velocity = {45000.0, 45000.0, 45000.0};
    //     m_ruckigInputState_L.max_acceleration = {25000.0, 25000.0, 25000.0};
    //     m_ruckigInputState_L.max_jerk = {15000.0, 15000.0, 15000.0};
    //     LOG(INFO)<<"SCALING 20x ";

    // case pedalSwitchTwo:

    //     m_ruckigInputState_R.max_velocity = {45000.0, 45000.0, 45000.0};
    //     m_ruckigInputState_R.max_acceleration = {20000.0, 20000.0, 20000.0};
    //     m_ruckigInputState_R.max_jerk = {12000.0, 12000.0, 12000.0};

    //     m_ruckigInputState_L.max_velocity = {45000.0, 45000.0, 45000.0};
    //     m_ruckigInputState_L.max_acceleration = {20000.0, 20000.0, 20000.0};
    //     m_ruckigInputState_L.max_jerk = {12000.0, 12000.0, 12000.0};
    //     LOG(INFO)<<"SCALING 15x ";

    // case pedalSwitchThree:

    //     m_ruckigInputState_R.max_velocity = {55000.0, 55000.0, 55000.0};
    //     m_ruckigInputState_R.max_acceleration = {14000.0, 14000.0, 14000.0};
    //     m_ruckigInputState_R.max_jerk = {8000.0, 8000.0, 8000.0};

    //     m_ruckigInputState_L.max_velocity = {55000.0, 55000.0, 55000.0};
    //     m_ruckigInputState_L.max_acceleration = {14000.0, 14000.0, 14000.0};
    //     m_ruckigInputState_L.max_jerk = {8000.0, 8000.0, 8000.0};
    //     LOG(INFO)<<"SCALING 12x ";

    // case pedalSwitchFour:

    //     m_ruckigInputState_R.max_velocity = {55000.0, 55000.0, 55000.0};
    //     m_ruckigInputState_R.max_acceleration = {12000.0, 12000.0, 12000.0};
    //     m_ruckigInputState_R.max_jerk = {6000.0, 6000.0, 6000.0};

    //     m_ruckigInputState_L.max_velocity = {55000.0, 55000.0, 55000.0};
    //     m_ruckigInputState_L.max_acceleration = {12000.0, 12000.0, 12000.0};
    //     m_ruckigInputState_L.max_jerk = {6000.0, 6000.0, 6000.0};
    //     LOG(INFO)<<"SCALING 10x ";

    // case pedalSwitchFive:

    //     m_ruckigInputState_R.max_velocity = {65000.0, 65000.0, 65000.0};
    //     m_ruckigInputState_R.max_acceleration = {10000.0, 10000.0, 10000.0};
    //     m_ruckigInputState_R.max_jerk = {5000.0, 5000.0, 5000.0};

    //     m_ruckigInputState_L.max_velocity = {65000.0, 65000.0, 65000.0};
    //     m_ruckigInputState_L.max_acceleration = {10000.0, 10000.0, 10000.0};
    //     m_ruckigInputState_L.max_jerk = {5000.0, 5000.0, 5000.0};
    //     LOG(INFO)<<"SCALING 7x ";
    case pedalSwitchOne:

        m_ruckigInputState_R.max_velocity = {45000.0, 45000.0, 45000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {25000.0, 25000.0, 25000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {15000.0, 15000.0, 15000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {45000.0, 45000.0, 45000.0};
        m_ruckigInputState_L.max_acceleration = {25000.0, 25000.0, 25000.0};
        m_ruckigInputState_L.max_jerk = {15000.0, 15000.0, 15000.0};
        LOG(INFO)<<"SCALING 20x ";

    case pedalSwitchTwo:

        m_ruckigInputState_R.max_velocity = {40000.0, 40000.0, 40000.0};
        m_ruckigInputState_R.max_acceleration = {15000.0, 15000.0, 15000.0};
        m_ruckigInputState_R.max_jerk = {10000.0, 10000.0, 10000.0};

        m_ruckigInputState_L.max_velocity = {40000.0, 40000.0, 40000.0};
        m_ruckigInputState_L.max_acceleration = {15000.0, 15000.0, 15000.0};
        m_ruckigInputState_L.max_jerk = {10000.0, 10000.0, 10000.0};
        LOG(INFO)<<"SCALING 15x ";

    case pedalSwitchThree:

        m_ruckigInputState_R.max_velocity = {45000.0, 45000.0, 45000.0};
        m_ruckigInputState_R.max_acceleration = {12000.0, 12000.0, 12000.0};
        m_ruckigInputState_R.max_jerk = {6000.0, 6000.0, 6000.0};

        m_ruckigInputState_L.max_velocity = {45000.0, 45000.0, 45000.0};
        m_ruckigInputState_L.max_acceleration = {12000.0, 12000.0, 12000.0};
        m_ruckigInputState_L.max_jerk = {6000.0, 6000.0, 6000.0};
        LOG(INFO)<<"SCALING 12x ";

    case pedalSwitchFour:

        m_ruckigInputState_R.max_velocity = {55000.0, 55000.0, 55000.0};
        m_ruckigInputState_R.max_acceleration = {10000.0, 10000.0, 10000.0};
        m_ruckigInputState_R.max_jerk = {4000.0, 4000.0, 4000.0};

        m_ruckigInputState_L.max_velocity = {55000.0, 55000.0, 55000.0};
        m_ruckigInputState_L.max_acceleration = {10000.0, 10000.0, 10000.0};
        m_ruckigInputState_L.max_jerk = {4000.0, 4000.0, 4000.0};
        LOG(INFO)<<"SCALING 10x ";

    case pedalSwitchFive:

        m_ruckigInputState_R.max_velocity = {65000.0, 65000.0, 65000.0};
        m_ruckigInputState_R.max_acceleration = {7000.0, 7000.0, 7000.0};
        m_ruckigInputState_R.max_jerk = {3000.0, 3000.0, 3000.0};

        m_ruckigInputState_L.max_velocity = {65000.0, 65000.0, 65000.0};
        m_ruckigInputState_L.max_acceleration = {7000.0, 7000.0, 7000.0};
        m_ruckigInputState_L.max_jerk = {3000.0, 3000.0, 3000.0};
        LOG(INFO)<<"SCALING 7x ";
    }



    if(side == 'l')
    {
        m_handlePoseInit_L = handlePoseCur;
        m_motorEncoderInit_L = motorPositionCur;
        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur,'l');
        m_alignmentNumber_L = 0;

        m_ruckigInputState_L.current_position = {m_endEffectorInitPosition_L[0], m_endEffectorInitPosition_L[1], m_endEffectorInitPosition_L[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_L.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_L.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        /*oneEuroFilter*/
        inited_L = {false, false, false};
        xhat_L = {0, 0, 0};
        vhat_L = {0.0, 0.0, 0.0};

        /*Velocity_Kompensation*/
        m_EncodeErr_Pre_L = {0};
        m_EncodeErr_Int = {0};

        /*test*/
        test_x = 0;
        test_y = 0;
        test_z = 0;
        test_time = 0;
        test_index = 0;
        step_flag = 0;
        angle = 0;
    }
    if(side == 'r')
    {
        m_handlePoseInit_R = handlePoseCur;
        m_motorEncoderInit_R = motorPositionCur;//需要把右边电机值存储进去
        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur,'r');
        m_alignmentNumber_R = 0;

        /*Velocity_Kompensation*/
        m_EncodeErr_Pre_R = {0};

        m_ruckigInputState_R.current_position = {m_endEffectorInitPosition_R[0], m_endEffectorInitPosition_R[1], m_endEffectorInitPosition_R[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_R.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_R.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        /*oneEuroFilter*/
        inited_R = {false, false, false};
        xhat_R = {0, 0, 0};
        vhat_R = {0.0, 0.0, 0.0};

        /*test*/
        test_x = 0;
        test_y = 0;
        test_z = 0;
        test_time = 0;
        test_index = 0;
        step_flag = 0;
        angle = 0;
    }
}

void RobotControl::setForceControlInitHandleMotorPosition(const std::array<int, MotorNumPerSide>& motorPositionCur, const HandlePose& handlePoseCur, const char& side){

    switch(m_speedPedalIndex_Cur){
    case pedalSwitchOne:

        m_ruckigInputState_R.max_velocity = {25000.0, 25000.0, 25000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {20000.0, 20000.0, 20000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {10000.0, 10000.0, 10000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {25000.0, 25000.0, 25000.0};
        m_ruckigInputState_L.max_acceleration = {20000.0, 20000.0, 20000.0};
        m_ruckigInputState_L.max_jerk = {10000.0, 10000.0, 10000.0};
        LOG(INFO)<<"SCALING 20x ";

    case pedalSwitchTwo:

        m_ruckigInputState_R.max_velocity = {25000.0, 25000.0, 25000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {20000.0, 20000.0, 20000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {10000.0, 10000.0, 10000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {25000.0, 25000.0, 25000.0};
        m_ruckigInputState_L.max_acceleration = {20000.0, 20000.0, 20000.0};
        m_ruckigInputState_L.max_jerk = {10000.0, 10000.0, 10000.0};
        LOG(INFO)<<"SCALING 15x ";

    case pedalSwitchThree:

        m_ruckigInputState_R.max_velocity = {30000.0, 30000.0, 30000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {18000.0, 25000.0, 25000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {8000.0, 12000.0, 12000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {30000.0, 30000.0, 30000.0};
        m_ruckigInputState_L.max_acceleration = {18000.0, 20000.0, 20000.0};
        m_ruckigInputState_L.max_jerk = {8000.0, 10000.0, 10000.0};
        LOG(INFO)<<"SCALING 12x ";

    case pedalSwitchFour:

        m_ruckigInputState_R.max_velocity = {35000.0, 35000.0, 35000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {15000.0, 18000.0, 18000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {8000.0, 10000.0, 10000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {35000.0, 35000.0, 35000.0};
        m_ruckigInputState_L.max_acceleration = {15000.0, 18000.0, 18000.0};
        m_ruckigInputState_L.max_jerk = {8000.0, 10000.0, 10000.0};
        LOG(INFO)<<"SCALING 10x ";

    case pedalSwitchFive:

        m_ruckigInputState_R.max_velocity = {40000.0, 40000.0, 40000.0};//500000.0
        m_ruckigInputState_R.max_acceleration = {10000.0, 10000.0, 10000.0};//15000.0
        m_ruckigInputState_R.max_jerk = {6000.0, 6000.0, 6000.0};//8000.0

        m_ruckigInputState_L.max_velocity = {40000.0, 40000.0, 40000.0};
        m_ruckigInputState_L.max_acceleration = {10000.0, 10000.0, 10000.0};
        m_ruckigInputState_L.max_jerk = {6000.0, 6000.0, 6000.0};
        LOG(INFO)<<"SCALING 7x ";
    }


    if(side == 'l')
    {
        m_forceAccumulationBufferLeft = {0,0,0};
        m_forceAccumulationBufferLeft_judge = {0,0,0};
        m_handlePoseInit_L.handlePoseL_X = handlePoseCur.handlePoseL_X;
        m_handlePoseInit_L.handlePoseL_Y = handlePoseCur.handlePoseL_Y;
        m_handlePoseInit_L.handlePoseL_Z = handlePoseCur.handlePoseL_Z;

        for(int i=0; i<4; i++){
        m_motorEncoderInit_L[i] = motorPositionCur[i];
        }
        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur,'l');

        m_ruckigInputState_L.current_position = {m_endEffectorInitPosition_L[0], m_endEffectorInitPosition_L[1], m_endEffectorInitPosition_L[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_L.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_L.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        /*oneEuroFilter*/
        inited_L = {false, false, false};
        xhat_L = {0, 0, 0};
        vhat_L = {0.0, 0.0, 0.0};

        /*Velocity_Kompensation*/
        m_EncodeErr_Pre_L = {0};

    }
    if(side == 'r')
    {
        m_forceAccumulationBufferRight = {0,0,0};
        m_forceAccumulationBufferRight_judge = {0,0,0};
        m_handlePoseInit_R.handlePoseR_X = handlePoseCur.handlePoseR_X;
        m_handlePoseInit_R.handlePoseR_Y = handlePoseCur.handlePoseR_Y;
        m_handlePoseInit_R.handlePoseR_Z = handlePoseCur.handlePoseR_Z;

        for(int i=0; i<4; i++){
            m_motorEncoderInit_R[i] = motorPositionCur[i];
        }

        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur,'r');

        m_ruckigInputState_R.current_position = {m_endEffectorInitPosition_R[0], m_endEffectorInitPosition_R[1], m_endEffectorInitPosition_R[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState_R.current_velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        m_ruckigInputState_R.current_acceleration = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        /*oneEuroFilter*/
        inited_R = {false, false, false};
        xhat_R = {0, 0, 0};
        vhat_R = {0.0, 0.0, 0.0};

        /*Velocity_Kompensation*/
        m_EncodeErr_Pre_R = {0};
    }
}

void RobotControl::endJointGoHome(const char& side)//yu
{
    /*磁栅尺确定相对编码器的绝对位置*/

    int moonsEncode_init_L;
    int moonsEncode_init_R;

    long magneticEncoder_Cur_L;
    long magneticEncoder_Cur_R;

    auto moonsEncode_abs_L = m_motorEncoderCur_L.load();
    auto moonsEncode_abs_R = m_motorEncoderCur_R.load();

    magneticEncoder_Cur_L = static_cast<long>(m_magneticEncoder_L.load()) - static_cast<long>(MagneticEncoder_Init_L);
    magneticEncoder_Cur_R = static_cast<long>(m_magneticEncoder_R.load()) - static_cast<long>(MagneticEncoder_Init_R);

    moonsEncode_init_L = moonsEncode_abs_L[0] - 4 * (magneticEncoder_Cur_L);
    moonsEncode_init_R = moonsEncode_abs_R[0] - 4 * (magneticEncoder_Cur_R);

    LOG(INFO)<<std::dec<<"L_INIT: "<<moonsEncode_init_L<<" Encode_abs_L: "<<moonsEncode_abs_L[0]<<" magEncoder_Cur_L: "<<magneticEncoder_Cur_L<<"   R_INIT: "<<moonsEncode_init_R<<" Encode_abs_R: "<<moonsEncode_abs_R[0]<<" magEncoder_Cur_R: "<<magneticEncoder_Cur_R;

    if(side == 'r')
    {
        std::thread calibration([this](){
            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
            usleep(50 * 1000);

            /*机械臂前往x极值处归零，避免两臂相撞*/
            // long magneticEncoder_Cur_L = static_cast<long>(m_magneticEncoder_L.load()) - static_cast<long>(MagneticEncoder_Init_L);
            // long magneticEncoder_Cur_R = static_cast<long>(m_magneticEncoder_R.load()) - static_cast<long>(MagneticEncoder_Init_R);
            // int target_Moons_L;
            // int target_Moons_R;
            // int MoonsGoZero_L;
            // int MoonsGoZero_R;

            // int targetEncode_Moons_L;
            // int targetEncode_Moons_R;

            // if(magneticEncoder_Cur_L < magneticEncoder_Cur_R){
            //     target_Moons_L = (MagneticEncoder_Min_L - magneticEncoder_Cur_L)/1000;
            //     target_Moons_R = (MagneticEncoder_Max_R - magneticEncoder_Cur_R)/1000;
            //     MoonsGoZero_L =  30 * 4000;
            //     MoonsGoZero_R =  -30 * 4000;

            //     LOG(INFO)<<"Riht arm is front";
            // }
            // else if(magneticEncoder_Cur_L >= magneticEncoder_Cur_R){
            //     target_Moons_L = (MagneticEncoder_Max_L - magneticEncoder_Cur_L)/1000;
            //     target_Moons_R = (MagneticEncoder_Min_R - magneticEncoder_Cur_R)/1000;
            //     MoonsGoZero_L =  -30 * 4000;
            //     MoonsGoZero_R =  30 * 4000;

            //     LOG(INFO)<<"Left arm is front";
            // }

            // targetEncode_Moons_R = 4000 * target_Moons_R;
            // targetEncode_Moons_L = 4000 * target_Moons_L;

/*找光电门归零*/
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
            usleep(1000 * 1000);


            // m_motorDriver->setTargetPos(MotorType::MOONS, 0, targetEncode_Moons_R, arm_0);/*先走到极值处*/
            // usleep(50 * 1000);
            // LOG(INFO)<<"SET MAX/MIN POSION OF RIGHT ARM! ";


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

                if(statusWordInBinary_moons_1[10] == 1 && statusWordInBinary_zero_1[10] == 1 && statusWordInBinary_zero_2[10] == 1 && statusWordInBinary_zero_3[10] == 1){//如果状态字第10项为0,就完成了init

                    LOG(INFO) << "GO MAX/MIN FINISH";
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");//目的为与uiinterface通信，reset button
                    break;
                }
                usleep(5 * 1000);
            }

            /*磁栅尺标定*/

            long magneticEncoder_Cur_R;

            auto moonsEncode_abs_R = m_motorEncoderCur_R.load();

            magneticEncoder_Cur_R = static_cast<long>(m_magneticEncoder_R.load()) - static_cast<long>(MagneticEncoder_Init_R);

            m_moonsEncodeZero_R = moonsEncode_abs_R[0] - 4 * (magneticEncoder_Cur_R);


            usleep(50 * 1000);
            //  m_motorDriver->setTargetPos(MotorType::MOONS, 0, MoonsGoZero_R, arm_0);/*再走到中间位置*/
            //  LOG(INFO)<<"SET ZERO POSION OF RIGHT ARM! ";
            // usleep(5 * 1000);

            // m_motorDriver->operationPP(MotorType::MOONS, 0, arm_0);//存在setOperationMode将模式改为PP 并发送targetposition作为相对位置
            // usleep(50 * 1000);

            // while(true)
            // {
            //     auto statusWordTmp = m_motorStatusWordCur_R.load();//当状态字(0x6041)的第10位=1的时候说明状态已经切换，即目标位置到达
            //     std::bitset<32> statusWordInBinary_moons_1(statusWordTmp[0]);
            //     if(statusWordInBinary_moons_1[10] == 1){
            //         LOG(INFO) << "FinishCalibration";
            //         //     m_moonsCaliFinish_R = 1;
            //             SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");//目的为与uiinterface通信，reset button
            //         break;
            //     }
            // }

        });
        calibration.detach();
        usleep(20 * 1000);
    }

    if(side == 'l'){
        std::thread calibration([this](){
            m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
            usleep(50 * 1000);

            // long magneticEncoder_Cur_L = static_cast<long>(m_magneticEncoder_L.load()) - static_cast<long>(MagneticEncoder_Init_L);
            // long magneticEncoder_Cur_R = static_cast<long>(m_magneticEncoder_R.load()) - static_cast<long>(MagneticEncoder_Init_R);
            // int target_Moons_L;
            // int target_Moons_R;
            // int MoonsGoZero_L;
            // int MoonsGoZero_R;

            // int targetEncode_Moons_L;
            // int targetEncode_Moons_R;

            // if(magneticEncoder_Cur_L < magneticEncoder_Cur_R){
            //     target_Moons_L = (MagneticEncoder_Min_L - magneticEncoder_Cur_L)/1000;
            //     target_Moons_R = (MagneticEncoder_Max_R - magneticEncoder_Cur_R)/1000;
            //     MoonsGoZero_L =  30 * 4000;
            //     MoonsGoZero_R =  -30 * 4000;

            //     LOG(INFO)<<"Riht arm is front";
            // }
            // else if(magneticEncoder_Cur_L >= magneticEncoder_Cur_R){
            //     target_Moons_L = (MagneticEncoder_Max_L - magneticEncoder_Cur_L)/1000;
            //     target_Moons_R = (MagneticEncoder_Min_R - magneticEncoder_Cur_R)/1000;
            //     MoonsGoZero_L =  -30 * 4000;
            //     MoonsGoZero_R =  30 * 4000;

            //     LOG(INFO)<<"Left arm is front";
            // }

            // targetEncode_Moons_R = 4000 * target_Moons_R;
            // targetEncode_Moons_L = 4000 * target_Moons_L;

/*找编码器归零*/
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
            usleep(1000 * 1000);

            // m_motorDriver->setTargetPos(MotorType::MOONS, 0, targetEncode_Moons_L, arm_1);/*先走到极值处*/
            // usleep(50 * 1000);
            // LOG(INFO)<<"SET MAX/MIN POSION OF RIGHT ARM! ";

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
                    LOG(INFO) << "GO MAX/MIN FINISH";
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");//目的为与uiinterface通信，reset button
                    break;
                }
                usleep(5 * 1000);
            }

                /*磁栅尺标定*/

                long magneticEncoder_Cur_L;

                auto moonsEncode_abs_L = m_motorEncoderCur_L.load();

                magneticEncoder_Cur_L = static_cast<long>(m_magneticEncoder_L.load()) - static_cast<long>(MagneticEncoder_Init_L);

                m_moonsEncodeZero_L = moonsEncode_abs_L[0] - 4 * (magneticEncoder_Cur_L);

                usleep(50 * 1000);

                //  m_motorDriver->setTargetPos(MotorType::MOONS, 0, MoonsGoZero_L, arm_1);/*再走到中间位置*/
                //  LOG(INFO)<<"SET ZERO POSION OF RIGHT ARM! ";
                // usleep(5 * 1000);

                // m_motorDriver->operationPP(MotorType::MOONS, 0, arm_1);//存在setOperationMode将模式改为PP 并发送targetposition作为相对位置
                // usleep(50 * 1000);

                // while(true)
                // {
                //     auto statusWordTmp = m_motorStatusWordCur_R.load();//当状态字(0x6041)的第10位=1的时候说明状态已经切换，即目标位置到达
                //     std::bitset<32> statusWordInBinary_moons_1(statusWordTmp[0]);
                //     if(statusWordInBinary_moons_1[10] == 1){
                //         m_moonsCaliFinish_L = 1;
                //         SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");//目的为与uiinterface通信，reset button
                //         LOG(INFO) << "FinishCalibration";
                //         break;
                //     }
                // }

                usleep(2 * 1000);
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

            // targetPosition = {JointEncoderPerRevolution / 2 + static_cast<int>(67.9495 * EncoderPerGrade),
            //                   JointEncoderPerRevolution / 2 + static_cast<int>(120.0839 * EncoderPerGrade),
            //                   JointEncoderPerRevolution / 2 + static_cast<int>(22.1344 * EncoderPerGrade)};
        targetPosition = {JointEncoderPerRevolution / 2 + static_cast<int>(62.0 * EncoderPerGrade),
                          JointEncoderPerRevolution / 2 + static_cast<int>(126.0 * EncoderPerGrade),
                          JointEncoderPerRevolution / 2 + static_cast<int>(34.0 * EncoderPerGrade)};

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
            if(m_motorDriver->getErrorCode(MotorType::ZERO_ERR, i, arm_0) != 0)
            {
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::CLEAR_ERROR, arm_0);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SHUT_DOWN, arm_0);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SWITCH_ON, arm_0);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ENABLE, arm_0);
                usleep(100 * 1000);
            }
            usleep(5 * 1000);
        }
        usleep(400 * 1000);

        LOG(INFO) << "2. Motor Enable, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_TRI_PP, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "3. Motor Enable Trigger, Current Status: " << std::hex << m_motorStatusWordCur_R.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_IM_TRI_PP, arm_0);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "4. Motor Enable Trigger Immediately Motion, Current Status: " << std::hex << m_motorStatusWordCur_R.load();
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

            // targetPosition = {JointEncoderPerRevolution / 2 - static_cast<int>(67.9495 * EncoderPerGrade),
            //                   JointEncoderPerRevolution / 2 - static_cast<int>(120.0839 * EncoderPerGrade),
            //                   JointEncoderPerRevolution / 2 - static_cast<int>(22.1344* EncoderPerGrade)};
        targetPosition = {JointEncoderPerRevolution / 2 - static_cast<int>(62.0 * EncoderPerGrade),
                          JointEncoderPerRevolution / 2 - static_cast<int>(126.0 * EncoderPerGrade),
                          JointEncoderPerRevolution / 2 - static_cast<int>(34.0* EncoderPerGrade)};

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
            if(m_motorDriver->getErrorCode(MotorType::ZERO_ERR, i, arm_1) != 0)
            {
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::CLEAR_ERROR, arm_1);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SHUT_DOWN, arm_1);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::SWITCH_ON, arm_1);
                usleep(100 * 1000);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ENABLE, arm_1);
                usleep(100 * 1000);
            }
            usleep(5 * 1000);
        }
        usleep(400 * 1000);
        LOG(INFO) << "2. Motor Enable, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_TRI_PP, arm_1);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "3. Motor Enable Trigger, Current Status: " << std::hex << m_motorStatusWordCur_L.load();

        for(int i = 0; i < 3; i++)
        {
            m_motorDriver->setControlWord(MotorType::ZERO_ERR, i, ControlCommand::ZEROERR_ENABLE_IM_TRI_PP, arm_1);
            usleep(5 * 1000);
        }
        usleep(50 * 1000);
        LOG(INFO) << "4. Motor Enable Trigger Immediately Motion, Current Status: " << std::hex << m_motorStatusWordCur_L.load();
    }
    m_armAnglePerSide = 30.0;
    LOG(INFO)<<"----------------------------------------------------------  m_armAnglePerSide: "<<m_armAnglePerSide;
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
            if(m_speedPedalIndex_Cur == pedalSwitchFive)
            {
                SendInnerMsg(Module_Inner_E::MasterConsole,static_cast<int>(MasterConsoleAction_E::Open3rdFilter),arglist);
            }
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
            if(m_speedPedalIndex_Cur == pedalSwitchFour)
            {
                SendInnerMsg(Module_Inner_E::MasterConsole,static_cast<int>(MasterConsoleAction_E::Open2rdFilter),arglist);
            }
        }
        else
        {
            LOG(INFO)<<"Already reached lowest Motion-Scaling factor";
        }
    }
    // 调用播放提示音函数
    if(m_speedPedalIndex_Cur != m_speedPedalIndex_Prev)
    {
        QString argstring;
        argstring.clear();
        argstring.append("speedCur:"+QString::number(m_speedPedalIndex_Cur+1));
        SendInnerMsg(Module_Inner_E::Audio,static_cast<int>(Voice_Action::VOICE_RecvMasterData),argstring);
    }
}

// 播放提示音实现方法
void RobotControl::setVoiceCommand()
{
    if(m_speedPedalIndex_Cur != m_speedPedalIndex_Prev)
    {
        QString argstring;
        argstring.clear();
        argstring.append("speedCur:"+QString::number(m_speedPedalIndex_Cur+1));
        SendInnerMsg(Module_Inner_E::Audio,static_cast<int>(Voice_Action::VOICE_RecvMasterData),argstring);
    }
}

void RobotControl::MaxonGoHome(const char& side)//yu
{
    if(side == 'r'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_R = {0};
        m_handlePoseLastLoop_R.initOrg_R();
        m_handlePoseOrg_R.initOrg_R();

        m_compensation_CurR.x() = 3.4338;
        m_compensation_CurR.y() = 3.9650;
        m_compensation_CurR.z() = -5.9475;

        m_compensation_LastR.x() = 3.4338;
        m_compensation_LastR.y() = 3.9650;
        m_compensation_LastR.z() = -5.9475;

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
        m_handlePoseOrg_L.initOrg_R();

        m_compensation_CurL.x() = 3.4338;
        m_compensation_CurL.y() = -3.9650;
        m_compensation_CurL.z() = -5.9475;

        m_compensation_LastL.x() = 3.4338;
        m_compensation_LastL.y() = -3.9650;
        m_compensation_LastL.z() = -5.9475;

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

void RobotControl::changeAngle_L(){

    auto motorPos_Cur_L = m_motorEncoderCur_L.load();
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    // std::array<int, MotorNumPerSide> motorEncoderData_L = {0};

    setControlInitHandleMotorPositionAndPose(motorPos_Cur_L, handlePoseCur, 'l');//handlePoseCur目的是读取x初始坐标

    double angle_init_L;
    double angle_target_L;
    int targetEncoder_1_pre = 0;
    int targetEncoder_2_pre = 0;
    int targetEncoder_3_pre = 0;

    if( m_armAnglePerSide == 30){
        angle_init_L = -30.0 / 180.0 * M_PI;
        angle_target_L = -15.0 / 180.0 * M_PI;
    }

    if( m_armAnglePerSide == 15){
        angle_init_L = -15.0 / 180.0 * M_PI;
        angle_target_L = -30.0 / 180.0 * M_PI;
    }

       double jointAngle1_Cur_L, jointAngle2_Cur_L, jointAngle3_Cur_L;
       jointAngle1_Cur_L = -(static_cast<double>(motorPos_Cur_L[Joint1_L] - JointEncoderPerRevolution / 2)) /
               static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
       jointAngle2_Cur_L = (static_cast<double>(motorPos_Cur_L[Joint2_L] - JointEncoderPerRevolution / 2)) /
               static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
       jointAngle3_Cur_L = -(static_cast<double>(motorPos_Cur_L[Joint3_L] - JointEncoderPerRevolution / 2)) /
               static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

        double y_Cur_L = m_endArm_1 * sin(jointAngle1_Cur_L) + m_endArm_2 * sin(jointAngle1_Cur_L + jointAngle2_Cur_L) + m_endArm_3 * sin(jointAngle1_Cur_L + jointAngle2_Cur_L + jointAngle3_Cur_L);
        double z_Cur_L = - m_endArm_1 * cos(jointAngle1_Cur_L) - m_endArm_2 * cos(jointAngle1_Cur_L + jointAngle2_Cur_L) - m_endArm_3 * cos(jointAngle1_Cur_L + jointAngle2_Cur_L + jointAngle3_Cur_L);
        double alpha_L;

        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);

        usleep(20 * 1000);

        for(int i = 0; i <= 2000; i++){

            alpha_L = angle_init_L + (angle_target_L - angle_init_L)/2000 * i;

                       // LOG(INFO)<<"alpha: "<<alpha * 180 / M_PI;

            double yp = y_Cur_L - m_endArm_3 * sin(alpha_L);
            double zp = z_Cur_L + m_endArm_3 * cos(alpha_L);

            double q1, q2, q3;
            q2 = -acos((yp * yp + zp * zp - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                       (2 * m_endArm_1 * m_endArm_2));

            double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(q2);

            double coefficient_tmp2 = m_endArm_2 * sin(q2);

            q1 = atan((coefficient_tmp1 * yp + coefficient_tmp2 * zp) /
                      (coefficient_tmp2 * yp - coefficient_tmp1 * zp));

            q3 = alpha_L - q1 - q2;

                       // LOG(INFO)<<"q1: "<< q1 / M_PI * 180 << " q2: " << q2 / M_PI * 180 <<" q3: "<< q3 / M_PI * 180;

            auto targetEncoder_1 = static_cast<int>(m_motorEncoderInit_L[1] + m_SpeedDirection_L[1] * (q1 - m_endEffectorInitJointAngle_L[0]) * 180 / M_PI / 360 * JointEncoderPerRevolution);
            auto targetEncoder_2 = static_cast<int>(m_motorEncoderInit_L[2] + m_SpeedDirection_L[2] * (q2 - m_endEffectorInitJointAngle_L[1]) * 180 / M_PI / 360 * JointEncoderPerRevolution);
            auto targetEncoder_3 = static_cast<int>(m_motorEncoderInit_L[3] + m_SpeedDirection_L[3] * (q3 - m_endEffectorInitJointAngle_L[2]) * 180 / M_PI / 360 * JointEncoderPerRevolution);

            auto targetVel_1 = int((targetEncoder_1 - targetEncoder_1_pre) / (0.004));
            auto targetVel_2 = int((targetEncoder_2 - targetEncoder_2_pre) / (0.004));
            auto targetVel_3 = int((targetEncoder_3 - targetEncoder_3_pre) / (0.004));

            targetEncoder_1_pre = targetEncoder_1;
            targetEncoder_2_pre = targetEncoder_2;
            targetEncoder_3_pre = targetEncoder_3;

            if((i == 0)||(i ==2000)){
                targetVel_1 = 0;
                targetVel_2 = 0;
                targetVel_3 = 0;
            }

            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_1, arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_2, arm_1);
            m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_3, arm_1);

            // motorEncoderData_L[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_1);
            // motorEncoderData_L[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_1);
            // motorEncoderData_L[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_1);

             // LOG(INFO)<<std::dec<<"motorEncoderData_L[1]: "<<motorEncoderData_L[1]<<" motorEncoderData_L[2]: "<<motorEncoderData_L[2]<<" motorEncoderData_L[3]: "<<motorEncoderData_L[3];

            // LOG(INFO)<<"index: "<<i<<std::dec<<"targetVel_1: "<<targetVel_1<<" targetVel_2: "<<targetVel_2<<" targetVel_3: "<<targetVel_3;

            std::this_thread::sleep_for(std::chrono::milliseconds(4));
        }

}

void RobotControl::changeAngle_R(){
    auto motorPos_Cur_R = m_motorEncoderCur_R.load();
    auto handlePoseCur = m_masterConsole.returnHandlePose();

    setControlInitHandleMotorPositionAndPose(motorPos_Cur_R, handlePoseCur, 'r');//handlePoseCur目的是读取x初始坐标

    double angle_init_R;
    double angle_target_R;

    int targetEncoder_1_pre = 0;
    int targetEncoder_2_pre = 0;
    int targetEncoder_3_pre = 0;

    if( m_armAnglePerSide == 30){
        angle_init_R = 30.0 / 180.0 * M_PI;
        angle_target_R = 15.0 / 180.0 * M_PI;
    }

    if( m_armAnglePerSide == 15){
        angle_init_R = 15.0 / 180.0 * M_PI;
        angle_target_R = 30.0 / 180.0 * M_PI;
    }
    double jointAngle1_Cur_R, jointAngle2_Cur_R, jointAngle3_Cur_R;
    jointAngle1_Cur_R = -(static_cast<double>(motorPos_Cur_R[Joint1_R] - JointEncoderPerRevolution / 2)) /
                        static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle2_Cur_R = (static_cast<double>(motorPos_Cur_R[Joint2_R] - JointEncoderPerRevolution / 2)) /
                        static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle3_Cur_R = -(static_cast<double>(motorPos_Cur_R[Joint3_R] - JointEncoderPerRevolution / 2)) /
                        static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;

    double y_Cur_R = m_endArm_1 * sin(jointAngle1_Cur_R) + m_endArm_2 * sin(jointAngle1_Cur_R + jointAngle2_Cur_R) + m_endArm_3 * sin(jointAngle1_Cur_R + jointAngle2_Cur_R + jointAngle3_Cur_R);
    double z_Cur_R = - m_endArm_1 * cos(jointAngle1_Cur_R) - m_endArm_2 * cos(jointAngle1_Cur_R + jointAngle2_Cur_R) - m_endArm_3 * cos(jointAngle1_Cur_R + jointAngle2_Cur_R + jointAngle3_Cur_R);
    double alpha_R;

    m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
    m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
    m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);

    // LOG(INFO)<<"y_Cur_R: "<<y_Cur_R<<" Z_Cur_R: "<<z_Cur_R;

    for(int i = 0; i <= 2000; i++){

        alpha_R = angle_init_R + (angle_target_R - angle_init_R)/2000 * i;

        double yp = y_Cur_R - m_endArm_3 * sin(alpha_R);
        double zp = z_Cur_R + m_endArm_3 * cos(alpha_R);

        double q1, q2, q3;
        q2 = acos((yp * yp + zp * zp - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                  (2 * m_endArm_1 * m_endArm_2));

        double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(q2);

        double coefficient_tmp2 = m_endArm_2 * sin(q2);

        q1 = atan((coefficient_tmp1 * yp + coefficient_tmp2 * zp) /
                  (coefficient_tmp2 * yp - coefficient_tmp1 * zp));


        q3 = alpha_R - q1 - q2;

        // LOG(INFO)<<"q1: "<< q1 / M_PI * 180 << " q2: " << q2 / M_PI * 180 <<" q3: "<< q3 / M_PI * 180;
        auto targetEncoder_1 = static_cast<int>(m_motorEncoderInit_R[1] + m_SpeedDirection_R[1] * (q1 - m_endEffectorInitJointAngle_R[0]) * 180 / M_PI / 360 * JointEncoderPerRevolution);
        auto targetEncoder_2 = static_cast<int>(m_motorEncoderInit_R[2] + m_SpeedDirection_R[2] * (q2 - m_endEffectorInitJointAngle_R[1]) * 180 / M_PI / 360 * JointEncoderPerRevolution);
        auto targetEncoder_3 = static_cast<int>(m_motorEncoderInit_R[3] + m_SpeedDirection_R[3] * (q3 - m_endEffectorInitJointAngle_R[2]) * 180 / M_PI / 360 * JointEncoderPerRevolution);

        auto targetVel_1 = int((targetEncoder_1 - targetEncoder_1_pre) / (0.004));
        auto targetVel_2 = int((targetEncoder_2 - targetEncoder_2_pre) / (0.004));
        auto targetVel_3 = int((targetEncoder_3 - targetEncoder_3_pre) / (0.004));

        targetEncoder_1_pre = targetEncoder_1;
        targetEncoder_2_pre = targetEncoder_2;
        targetEncoder_3_pre = targetEncoder_3;

        if((i == 0)||(i ==2000)){
            targetVel_1 = 0;
            targetVel_2 = 0;
            targetVel_3 = 0;
        }

        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, targetVel_1, arm_0);
        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, targetVel_2, arm_0);
        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, targetVel_3, arm_0);

        // motorEncoderData_R[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0, arm_0);
        // motorEncoderData_R[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1, arm_0);
        // motorEncoderData_R[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2, arm_0);

        // LOG(INFO)<<"index: "<<i<<std::dec<<"targetVel_1: "<<targetVel_1<<" targetVel_2: "<<targetVel_2<<" targetVel_3: "<<targetVel_3;
        // LOG(INFO)<<"index: "<<i<<std::dec<<"targetEncoder_1: "<<targetEncoder_1<<" targetEncoder_2: "<<targetEncoder_2<<" targetEncoder_3: "<<targetEncoder_3;

        std::this_thread::sleep_for(std::chrono::milliseconds(4));

    }
}

void RobotControl::MaxonGoHome_(const char& side)//yu
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    if(side == 'r'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_R = {0};
        m_handlePoseLastLoop_R.initOrg_R();
        m_handlePoseOrg_R.initOrg_R();

        // m_compensation_CurR.x() = 3.4338;
        // m_compensation_CurR.y() = 3.9650;
        // m_compensation_CurR.z() = -5.9475;

        // m_compensation_LastR.x() = 3.4338;
        // m_compensation_LastR.y() = 3.9650;
        // m_compensation_LastR.z() = -5.9475;

        m_compensation_CurR.x() = 1.3423;//1.7754;
        m_compensation_CurR.y() = 1.55;//2.050;
        m_compensation_CurR.z() = -2.325;//-3.075;


        m_compensation_LastR.x() = 1.3423;//1.7754;
        m_compensation_LastR.y() = 1.55;//2.050;
        m_compensation_LastR.z() = -2.325;//-3.075;


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

        // m_compensation_CurL.x() = 3.4338;
        // m_compensation_CurL.y() = -3.9650;
        // m_compensation_CurL.z() = -5.9475;

        // m_compensation_LastL.x() = 3.4338;
        // m_compensation_LastL.y() = -3.9650;
        // m_compensation_LastL.z() = -5.9475;

        m_compensation_CurL.x() = 1.3423;//1.7754;
        m_compensation_CurL.y() = -1.55;//-2.050;
        m_compensation_CurL.z() = -2.325;//-3.075;


        m_compensation_LastL.x() = 1.3423;//1.7754;
        m_compensation_LastL.y() = -1.55;//-2.050;
        m_compensation_LastL.z() = -2.325;//-3.075;

        std::thread calibration([this](){
            m_motorDriver->operationHOME(MotorType::MAXON, 0, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 1, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 2, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 3, arm_1);

            // if(m_endeffectorConfiguration == EndeffectorConfiguration::sixMaxons)
            // {
            m_motorDriver->operationHOME(MotorType::MAXON, 4, arm_1);
            m_motorDriver->operationHOME(MotorType::MAXON, 5, arm_1);
            // }

            LOG(INFO) << "Start left Maxon homing! ";
            while(true)
            {
                auto homeStatusTmp = m_motorHomingStatus_L.load();
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
    Message_Inner_T msgTemp;//实例化一条命令，发送者，接受者，命令内容
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
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToTeleOperationMode):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To TeleOperation";
                setRobotControlMode(RobotControlMode::TeleOperation);

                if((m_maxonCaliFinish_R==1)&&(m_maxonCaliFinish_L==1)&&(m_moonsCaliFinish_L==1)&&(m_moonsCaliFinish_R==1)){
                // setRobotControlMode(RobotControlMode::TeleOperation);
                }
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToCollaborationMode):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To Collaboration";
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToGuidingArmCollaboration):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To  Guiding Arm Collaboration";
                setRobotControlMode(RobotControlMode::Collaboration_GuidingArm);
                break;
            }
            case static_cast<int>(RobotControlAction_E::DisableGuidingArmCollaboration):
            {
                LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Disable Guiding Arm Collaboration";
                setRobotControlMode(RobotControlMode::Hold);
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
                    MaxonGoHome_('r');
                }
                if(side == 'l')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right Instrument Homing!";
                    // MaxonGoHome('l');
                    MaxonGoHome_('l');
                }
                break;
            }
            case static_cast<int>(RobotControlAction_E::SwitchInstrumentAngle):
            {

                if(i.value() == "30")
                {
                    LOG(INFO) << "Instrument Angle : 30";
                    if(m_armAnglePerSide == 30){
                        changeAngle_L();
                        changeAngle_R();
                    }
                    m_armAnglePerSide = 15;
                    LOG(INFO)<<"current armAngle per side! "<<m_armAnglePerSide;

                }else if(i.value() ==  "60")
                {
                    LOG(INFO) << "Instrument Angle : 60";
                    if(m_armAnglePerSide == 15){
                        changeAngle_L();
                        changeAngle_R();
                    }
                    m_armAnglePerSide = 30;
                    LOG(INFO)<<"current armAngle per side! "<<m_armAnglePerSide;
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
