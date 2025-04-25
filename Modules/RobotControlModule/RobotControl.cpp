#include "RobotControl.h"

void RobotControl::readMyInitData()
{
    std::cout << "________________READ MY ROBOT DATA_________________" << std::endl;
    try
    {
        toml::table RobotData = toml::parse_file(m_robotConfigPath);

        //Other Information
        {
            //Motion Scaling Coefficient
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

    std::cout << "________________READ MY INSTRUMENT DATA_________________" << std::endl;
    LOG(INFO) << "Left Instrument Information: "<< m_endEffectorLeft;
    LOG(INFO) << "Right Instrument Information: "<< m_endEffectorRight;
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
//    m_HandlePoseInit_L.init();
    m_HandlePoseInit_R.init();
    m_HandlePoseOrg_L.initOrg_L();
    m_HandlePoseOrg_R.initOrg_R();

    Eigen::Matrix3d MasterRotationMatrix;

    m_HandlePosePrev_R.init();
    m_HandlePoseLastLoop_L.initOrg_L();
    m_HandlePoseLastLoop_R.initOrg_R();

    m_ControlValuePrev_L = {0};
    m_ControlValuePrev_R = {0};

    m_PosScalingIndex_Cur = 0;
}

void RobotControl::startMyThreads()//yu
{
    startUpdataMasterConsoleDataThread();

    flagUpdateMasterConsoleData.store(true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    startConmunicateWithMotorDriverThread();
    LOG(INFO) << " startConmunicateWithMotorDriverThread ";

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    goToHold();

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    startControlThread();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    LOG(INFO) << "startControlThread ";
}

void RobotControl::startControlThread()
{
    m_calculateControlDataThread = std::thread(&RobotControl::control, this);
    m_calculateControlDataThread.detach();
}

void RobotControl::control()
{
    while(true)
    {
        if(m_flagInTeleoperation.load())
        {
            teleoperation();
        }

        if(m_flagInCollabration.load())
        {

        }
    }
}

void RobotControl::teleoperation()
{
    HandlePose      masterHandlePose_Init_L;
    HandlePose      masterHandlePose_Init_R;
    HandlePose      masterHandlePose_Prev_R;

//    std::array<double, ControlValueNum>   controlValuePrev_L = {0};
//    std::array<double, ControlValueNum>   controlValueCur_L = {0};
//    std::array<double, ControlValueNum>   controlValuePrev_R = {0};
//    std::array<double, ControlValueNum>   controlValueCur_R = {0};
//    std::array<int, MotorNum>   motorPositionInit_L = {0};
//    std::array<int, MotorNum>   motorPositionCur_L = {0};

    std::array<int, MotorNum>   motorPositionCur_R = {};

    std::array<std::array<int,11>, 3> motorControl_save;//保存当前motor位置，作为pre值
    motorControl_save = m_motorControl_save;

    std::array<int, MotorNum>   motorPositionInit_R;//teleoperation() record 500th as Init

    std::array<int,MotorNum>   targetEncoder;
    std::array<int,MotorNum>   targetVelocity;

    auto MotorPositionInit_R = m_MotorPositionInit_R;

//    //save previous sensor and forcep posture data and the opening angle and button as previous data
    masterHandlePose_Init_R = m_HandlePoseInit_R;
    masterHandlePose_Prev_R = m_HandlePosePrev_R;
  //  masterHandlePose_Cur  = m_HandlePose_Cur.load();

//    controlValuePrev_L = m_ControlValuePrev_L;
//    controlValuePrev_R = m_ControlValuePrev_R;

//    motorPositionInit_L = m_MotorPositionInit_L;
    motorPositionInit_R = m_MotorPositionInit_R;

//    int enableTagCur_L = keepDisabling;
//    int enableTagCur_R = keepDisabling;

     auto controlLoopNum = m_controlLoopNum;

//    //Extract the updated Encoder Value from Galilm_MotorCurEncoder
        motorPositionCur_R = m_MotorCurEncoder.load();

    auto handlePoseCur = m_HandlePose_Cur.load();

    handlePoseCur.enableButton_R = 3;//默认使能踏板踩下
    handlePoseCur.enableButton_L = 3;//默认使能踏板踩下

    m_OpenAngle_R = handlePoseCur.handlePoseR_OpenAngle;
//    LOG(INFO)<<"handlePoseCur_OPENANGLE: "<< m_OpenAngle_R;
    m_OpenAngle_L = handlePoseCur.handlePoseL_OpenAngle;

    m_graspIndex_R = handlePoseCur.graspIndex_R;
//    LOG(INFO)<<"grasp index: "<< m_graspIndex_R;


    //const int pedalEnable = 1;//enablePedal

    Eigen::Matrix3d PoseCur_R = handlePoseCur.getRotationDataR();//取回的是3d矩阵
    Eigen::Matrix3d PoseCur_L = handlePoseCur.getRotationDataL();//取回的是3d矩阵

//      LOG(INFO)<<"R: "<<PoseCur_R;
//      LOG(INFO)<<"L: "<<PoseCur_L;

    std::array<double, 3> PositionCur_R;
    std::array<double, 3> PositionInit_R;
    std::array<double, 3> PositionCur_L;
    std::array<double, 3> PositionInit_L;

    PositionCur_R[0] = handlePoseCur.handlePoseR_X;
    PositionCur_R[1] = handlePoseCur.handlePoseR_Y;
    PositionCur_R[2] = handlePoseCur.handlePoseR_Z;
    PositionCur_L[0] = handlePoseCur.handlePoseL_X;
    PositionCur_L[1] = handlePoseCur.handlePoseL_Y;
    PositionCur_L[2] = handlePoseCur.handlePoseL_Z;

    PositionInit_R[0] = masterHandlePose_Init_R.handlePoseR_X;
    PositionInit_R[1] = masterHandlePose_Init_R.handlePoseR_Y;
    PositionInit_R[2] = masterHandlePose_Init_R.handlePoseR_Z;
    PositionInit_L[0] = masterHandlePose_Init_R.handlePoseL_X;
    PositionInit_L[1] = masterHandlePose_Init_R.handlePoseL_Y;
    PositionInit_L[2] = masterHandlePose_Init_R.handlePoseL_Z;

    if(controlLoopNum >= m_waitTime)//由于controlLoopum在500后才开始读取motorControl值，所以motorControl[1]前2项值很大不要紧。
    {
    int Status_R = RobotControl::enableCase_KeepPressPedal(handlePoseCur, 'r');//是否踩下脚踏
    m_status_R = Status_R;
    }
    LOG(INFO)<<"Status: "<<m_status_R;

    std::array<double, 15> controlValues = RobotControl::YUmotionMappingR(PoseCur_R, PositionCur_R, PositionInit_R, MotorPositionInit_R);

    std::array<double, 15> controlValues_Cur = controlValues;

    handlePoseCur.handlePoseR_Arzimuth = controlValues[14];//存入从手关节角度
    handlePoseCur.handlePoseR_Elevation = controlValues[13];
    handlePoseCur.handlePoseR_Roll = controlValues[12];

    m_rollAngle = controlValues[12];
    m_yawAngle = controlValues[14];
    m_pitchAngle = controlValues[13];//保存当前值，判断角度

    auto controlValues_Prev = m_controlValues_Prev;

    motorControl = RobotControl::forwardKinematics(controlValues_Prev, controlValues_Cur,
                                               MotorPositionInit_R, motorControl_save[1], handlePoseCur);//motorPositionCur_R为直接读取的编码器数值

    m_motorControl_save = motorControl;//用作保存pre控制数值

    LOG(INFO) << std::dec << "number" << controlLoopNum;

    if(controlLoopNum >= m_waitTime)//由于controlLoopum在500后才开始读取motorControl值，所以motorControl[1]前2项值很大不要紧。
    {
           if(controlLoopNum == m_waitTime)
           {
               motorPositionInit_R[0] = motorPositionCur_R[0];
               motorPositionInit_R[1] = motorPositionCur_R[1];
               motorPositionInit_R[2] = motorPositionCur_R[2];
               motorPositionInit_R[3] = motorPositionCur_R[3];
               motorPositionInit_R[4] = motorPositionCur_R[4];
               motorPositionInit_R[5] = motorPositionCur_R[5];
               motorPositionInit_R[7] = motorPositionCur_R[6];
               motorPositionInit_R[8] = motorPositionCur_R[7];
               motorPositionInit_R[9] = motorPositionCur_R[8];
               motorPositionInit_R[10] = motorPositionCur_R[9];

               targetEncoder[0] = motorPositionInit_R[0];
               targetEncoder[1] = motorPositionInit_R[1];
               targetEncoder[2] = motorPositionInit_R[2];
               targetEncoder[3] = motorPositionInit_R[3];
               targetEncoder[4] = motorPositionInit_R[4];
               targetEncoder[5] = motorPositionInit_R[5];
               targetEncoder[7] = motorPositionInit_R[7];
               targetEncoder[8] = motorPositionInit_R[8];
               targetEncoder[9] = motorPositionInit_R[9];
               targetEncoder[10] = motorPositionInit_R[10];

               targetVelocity[0] = 0;
               targetVelocity[1] = 0;
               targetVelocity[2] = 0;
               targetVelocity[3] = 0;

               targetVelocity[4] = 0;
               targetVelocity[5] = 0;
               targetVelocity[7] = 0;
               targetVelocity[8] = 0;
               targetVelocity[9] = 0;
               targetVelocity[10] = 0;

               m_HandlePoseInit_R = handlePoseCur;
               setControlInitHandleMotorPositionAndPose(motorPositionInit_R, handlePoseCur, 'r');
               m_AlignmentNumber_R = 0;
         }

    if (m_status_R == 1 || m_status_R == 2){

           if(controlLoopNum > m_waitTime)
           {
             targetEncoder[0] = motorControl[1][0];
             targetEncoder[1] = motorControl[1][1];
             targetEncoder[2] = motorControl[1][2];
             targetEncoder[3] = motorControl[1][3];

             targetEncoder[4] = motorControl[1][4];
             targetEncoder[5] = motorControl[1][5];
             targetEncoder[7] = motorControl[1][7];
             targetEncoder[8] = motorControl[1][8];
             targetEncoder[9] = motorControl[1][9];
             targetEncoder[10] = motorControl[1][10];

             targetVelocity[0] = motorControl[0][0];
             targetVelocity[1] = motorControl[0][1];
             targetVelocity[2] = motorControl[0][2];
             targetVelocity[3] = motorControl[0][3];

             }

           //以下两个if只与csv模式相关
           if(controlLoopNum == m_waitTime + 1){//由于500时init值会改变导致controlcalue（delta）与pre（delt）差异过大
               targetVelocity[0] = 0;
               targetVelocity[1] = 0;
               targetVelocity[2] = 0;
               targetVelocity[3] = 0;
           }

           if(m_status_R == 1){
               targetVelocity[0] = 0;
               targetVelocity[1] = 0;
               targetVelocity[2] = 0;
               targetVelocity[3] = 0;
        }

        }else{//Status 3 4

        targetVelocity[0] = 0;
        targetVelocity[1] = 0;
        targetVelocity[2] = 0;
        targetVelocity[3] = 0;
    }

//      LOG(INFO) << "差值1： " << std::dec << targetEncoder[0] - m_motorPosition_Cur[0];
//      LOG(INFO) << "差值2： " << std::dec << targetEncoder[1] - m_motorPosition_Cur[1];
//      LOG(INFO) << "差值3： " << std::dec << targetEncoder[2] - m_motorPosition_Cur[2];
//      LOG(INFO) << "差值4： " << std::dec << targetEncoder[3] - m_motorPosition_Cur[3];
//      LOG(INFO) << "速度1： " << std::dec << targetVelocity[0];
//      LOG(INFO) << "速度2： " << std::dec << targetVelocity[1];
//      LOG(INFO) << "速度3： " << std::dec << targetVelocity[2];
//      LOG(INFO) << "速度4： " << std::dec << targetVelocity[3];

    }
    else//小于waitTime
    {
        targetEncoder[4] = motorPositionCur_R[4];
        targetEncoder[5] = motorPositionCur_R[5];
        targetEncoder[7] = motorPositionCur_R[6];
        targetEncoder[8] = motorPositionCur_R[7];
        targetEncoder[9] = motorPositionCur_R[8];
        targetEncoder[10] = motorPositionCur_R[9];

        targetEncoder[0] = motorPositionCur_R[0];
        targetEncoder[1] = motorPositionCur_R[1];
        targetEncoder[2] = motorPositionCur_R[2];
        targetEncoder[3] = motorPositionCur_R[3];

        targetVelocity[0] = 0;
        targetVelocity[1] = 0;
        targetVelocity[2] = 0;
        targetVelocity[3] = 0;
    }

        m_controlValues_MotorPrev[4] = motorPositionCur_R[4] / yum_kForcepPosition_R[4] / yum_SpeedDirection_R[4];//controlValues_Cur;电机实际位置
        m_controlValues_MotorPrev[5] = motorPositionCur_R[5] / yum_kForcepPosition_R[5] / yum_SpeedDirection_R[5];

        m_controlValues_Prev = controlValues_Cur;
        m_HandlePosePrev_R = handlePoseCur;

        m_motorPosition_Cur = motorControl[1];//更新现在Motor的位置
        m_controlLoopNum ++;

//       LOG(INFO) << " targetEncoder[0] = " << std::dec << targetEncoder[0];
//       LOG(INFO) << " targetEncoder[1] = " << std::dec << targetEncoder[1];
//       LOG(INFO) << " targetEncoder[2] = " << std::dec << targetEncoder[2];
//       LOG(INFO) << " targetEncoder[3] = " << std::dec << targetEncoder[3];

        LOG(INFO) << " targetEncoder[4] = " << std::dec << targetEncoder[4] << " cur: " << motorPositionCur_R[4];
        LOG(INFO) << " targetEncoder[5] = " << std::dec << targetEncoder[5] << " cur: " << motorPositionCur_R[5];
        LOG(INFO) << " targetEncoder[7] = " << std::dec << targetEncoder[7] << " cur: " << motorPositionCur_R[6];
        LOG(INFO) << " targetEncoder[8] = " << std::dec << targetEncoder[8] << " cur: " << motorPositionCur_R[7];
        LOG(INFO) << " targetEncoder[9] = " << std::dec << targetEncoder[9] << " cur: " << motorPositionCur_R[8];
        LOG(INFO) << " targetEncoder[10] = " << std::dec << targetEncoder[10] << " cur: " << motorPositionCur_R[9];

          m_MotorTargetEncoder.store(targetEncoder);
          m_MotorTargetVel.store(targetVelocity);
          auto digitalOutput = m_MotorHomingStatus.load();
          LOG(INFO) << "DigitalInput: " << digitalOutput[1];

       std::this_thread::sleep_for(std::chrono::milliseconds(4));//控制
/*              std::this_thread::sleep_for(std::chrono::milliseconds(30));//画圆
             std::this_thread::sleep_for(std::chrono::milliseconds(1000));//步进测试getRotationDataR
*/
    }


Eigen::Matrix3d RobotControl::ToMasterRotationMatrix(double q_0, double q_1, double q_2, double q_3){

    Eigen::Matrix3d MasterRotationMatrix;

    MasterRotationMatrix(0,0) = pow(q_0,2) + pow(q_1,2) - pow(q_2,2) - pow(q_3,2);
    MasterRotationMatrix(0,1) = 2 * (q_1 * q_2 - q_0 * q_3);
    MasterRotationMatrix(0,2) = 2 * (q_1 * q_3 + q_0 * q_2);
    MasterRotationMatrix(1,0) = 2 * (q_3 * q_0 + q_1 * q_2);
    MasterRotationMatrix(1,1) = pow(q_0,2) - pow(q_1,2) + pow(q_2,2) - pow(q_3,2);
    MasterRotationMatrix(1,2) = 2 * (q_2 * q_3 - q_0 * q_1);
    MasterRotationMatrix(2,0) = 2 * (q_1 * q_3 - q_0 * q_2);
    MasterRotationMatrix(2,1) = 2 * (q_1 * q_0 + q_3 * q_2);
    MasterRotationMatrix(2,2) = pow(q_0,2) - pow(q_1,2) - pow(q_2,2) + pow(q_3,2);

    return MasterRotationMatrix;
}

std::array<double, 15> RobotControl::YUmotionMappingR(
                                                  const Eigen::Matrix3d& handlePoseCur,
                                                  const std::array<double, 3> PositionCur,//Viper读取的值
                                                  const std::array<double, 3> PositionInit,
                                                  const std::array<int,MotorNum>& motorPos_Init)
{
    std::array<double, 15> controlValueR = {0};

    // 初始化成员变量
    Eigen::Matrix3d HandlePoseOrg_R = m_HandlePoseOrg_R.getRotationDataR();//Eigen::Matrix3d::Identity();//初始位置为
    Eigen::Matrix3d handlePosePrev = rotationMatrixPrev;

    if (m_AlignmentNumber_R < 100) { m_AlignmentNumber_R++; }
    LOG(INFO) << " AlignmentNumber: " << m_AlignmentNumber_R;

    // 映射矩阵
    Eigen::Matrix3d mappingMatrix;
    Eigen::Matrix3d Init_Rotation;//handlepose
    Eigen::Matrix3d LastLoop_Rotation;

    mappingMatrix << 1, 0, 0,
                     0, -1, 0,
                     0, 0, -1;

    Init_Rotation = m_HandlePoseInit_R.getRotationDataR();
    LastLoop_Rotation = m_HandlePoseLastLoop_R.getRotationDataR();

    Eigen::Matrix3d rotMaster_R = mappingMatrix * handlePoseCur * mappingMatrix.inverse();

    // 计算 rotSlaveMatrix_R
    double theta = -60; //根据实际情况赋值-60，绕x旋转
    double rotation_z = -30;//实际上是绕z轴转-30度
    double rotation_y = -30;//根据实际情况赋值-30
    double theta_angle = theta * M_PI / 180;
    double rotationZ_angle = rotation_z * M_PI / 180;
    double rotationY_angle = rotation_y * M_PI / 180;

    Eigen::Matrix3d rotationMatrix_2;
    rotationMatrix_2 << 1, 0, 0,
                        0, 0, -1,
                        0, 1, 0;

    Eigen::Matrix3d rotationMatrix_3;
    rotationMatrix_3 << 0, 0, 1,
                        0, 1, 0,
                        -1, 0, 0;

    Eigen::Matrix3d rotation_x;
    rotation_x << cos(theta_angle), -sin(theta_angle), 0,
                  sin(theta_angle),  cos(theta_angle), 0,
                  0,               0,                  1;

    Eigen::Matrix3d rotationMatrix_z;
    rotationMatrix_z << 1,                   0,                     0,
                        0,cos(rotationZ_angle), -sin(rotationZ_angle),
                        0,sin(rotationZ_angle),  cos(rotationZ_angle);

    Eigen::Matrix3d rotationMatrix_y;
    rotationMatrix_y << cos(rotationY_angle), 0, sin(rotationY_angle),
                        0,                    1,                    0,
                       -sin(rotationY_angle), 0, cos(rotationY_angle);

    Eigen::Matrix3d rotSlaveMatrix_R = rotationMatrix_2 * rotationMatrix_3 * rotation_x * rotationMatrix_y * rotationMatrix_z;

    // 计算各个旋转矩阵
    Eigen::Matrix3d rotMatrix_Org_R = rotSlaveMatrix_R.inverse() * mappingMatrix * HandlePoseOrg_R * mappingMatrix.inverse();
    Eigen::Matrix3d rotMatrix_Init_R = rotSlaveMatrix_R.inverse() * mappingMatrix * Init_Rotation * mappingMatrix.inverse();
    Eigen::Matrix3d rotMatrix_Prev_R = rotSlaveMatrix_R.inverse() * mappingMatrix * handlePosePrev * mappingMatrix.inverse();
    Eigen::Matrix3d rotMatrix_Last_R = rotSlaveMatrix_R.inverse() * mappingMatrix * LastLoop_Rotation * mappingMatrix.inverse();
    Eigen::Matrix3d rotMatrix_Cur_R = rotSlaveMatrix_R.inverse() * rotMaster_R;

    // alpha(pitch)
    double alpha_Org_R = std::asin(rotMatrix_Org_R(0, 2));
    double alpha_Init_R = std::asin(rotMatrix_Init_R(0, 2));
    double alpha_Prev_R = std::asin(rotMatrix_Prev_R(0, 2));
    double alpha_Last_R = std::asin(rotMatrix_Last_R(0, 2));
    double alpha_Cur_R = std::asin(rotMatrix_Cur_R(0, 2));

//        LOG(INFO)<<"gamma_Init_R: "<<alpha_Init_R * 180 / M_PI;
//        LOG(INFO)<<"gamma_Cur_R: "<<alpha_Cur_R * 180 / M_PI;
//        LOG(INFO)<<"gamma_Last_R: "<<alpha_Last_R * 180 / M_PI;

    double delt_alphaCur_R = (alpha_Cur_R - alpha_Init_R) * 180 / M_PI;
    double delt_alphaInit_R = (alpha_Init_R - alpha_Last_R) * 180 / M_PI;
    double delt_alphaOrg_R = (alpha_Last_R - alpha_Org_R) * 180 / M_PI;
    double delt_alpha_R = delt_alphaCur_R + delt_alphaInit_R * m_AlignmentNumber_R / 100 + delt_alphaOrg_R;

//        LOG(INFO) << "delt_gammaCur_R: " << delt_alphaCur_R;
//        LOG(INFO) << "delt_gammaInit_R: " << delt_alphaInit_R;
//        LOG(INFO) << "delt_gammaOrg_R: " << delt_alphaOrg_R;
//        LOG(INFO) << "delt_yaw_R: " << delt_alpha_R;

    // 计算 deltLength_alpha_R
    double deltLength_alpha_R_1 = cableLengths_2(delt_alpha_R);
    double deltLength_alpha_R_2 = cableLengths_2(-delt_alpha_R);
    // OpenAngle
    double openAngle_R = 0.5 * m_OpenAngle_R;
//    openAngle_R = 0;

//    double  overlapValue_Tmp = 0;
//    double  openAngle_Cur = 0;

//        if(m_graspIndex_R == 0)
//        {
//            overlapValue_Tmp = 0;
//        }
//        if(m_graspIndex_R == 1)
//        {
////            overlapValue_Tmp = -(openAngle_Cur - 0) *  2 * m_EncoderPerDegree_L[3];
//            overlapValue_Tmp = 0.5 * 2 * openAngle_Cur * openAngle_Cur;//一次函数会出现一开始升高太快，调用二次函数会先慢后快
//        }

//        openAngle_R = 0.5 * m_OpenAngle_R + overlapValue_Tmp;

    // 计算 beta(yaw)
    double beta_Org_R = atan2(-rotMatrix_Org_R(0, 1), rotMatrix_Org_R(0, 0));
    double beta_Init_R = atan2(-rotMatrix_Init_R(0, 1), rotMatrix_Init_R(0, 0));
    double beta_Prev_R = atan2(-rotMatrix_Prev_R(0, 1), rotMatrix_Prev_R(0, 0));
    double beta_Last_R = atan2(-rotMatrix_Last_R(0, 1), rotMatrix_Last_R(0, 0));
    double beta_Cur_R = atan2(-rotMatrix_Cur_R(0, 1), rotMatrix_Cur_R(0, 0));
//    LOG(INFO)<<"BETA_Init_R: "<<beta_Init_R * 180 / M_PI;
//    LOG(INFO)<<"BETA_Cur_R: "<<beta_Cur_R * 180 / M_PI;
//    LOG(INFO)<<"BETA_Last_R: "<<beta_Last_R * 180 / M_PI;
//    LOG(INFO) << "openAngle_R: " << openAngle_R;

    double delt_betaCur_R = (beta_Cur_R - beta_Init_R) * 180 / M_PI;
    double delt_betaInit_R = (beta_Init_R - beta_Last_R) * 180 / M_PI;
    double delt_betaOrg_R = (beta_Last_R - beta_Org_R) * 180 / M_PI;
    double delt_beta_R = delt_betaCur_R + delt_betaInit_R * m_AlignmentNumber_R / 100 + delt_betaOrg_R;
//    LOG(INFO) << "delt_BETACur_R: " << delt_betaCur_R;
//    LOG(INFO) << "delt_BETAInit_R: " << delt_betaInit_R;
//    LOG(INFO) << "delt_BETAOrg_R: " << delt_betaOrg_R;
//    LOG(INFO) << "delt_yaw_R: " << delt_beta_R;

    // 计算 yaw 的绳长变化
    double deltLength_beta_R_left_1 = cableLengths_3(-delt_alpha_R, delt_beta_R, -openAngle_R);
    double deltLength_beta_R_left_2 = cableLengths_3(-delt_alpha_R, - delt_beta_R, openAngle_R);
    double deltLength_beta_R_right_1 = cableLengths_3(delt_alpha_R, delt_beta_R, openAngle_R);
    double deltLength_beta_R_right_2 = cableLengths_3(delt_alpha_R, - delt_beta_R, -openAngle_R);

    // 计算 gamma（roll）
    double gamma_Org_R = atan2(-rotMatrix_Org_R(1, 2)/cos(beta_Org_R), rotMatrix_Org_R(2, 2)/cos(beta_Org_R));
    double gamma_Init_R = atan2(-rotMatrix_Init_R(1, 2)/cos(beta_Init_R), rotMatrix_Init_R(2, 2)/cos(beta_Init_R));
    double gamma_Prev_R = atan2(-rotMatrix_Prev_R(1, 2)/cos(beta_Prev_R), rotMatrix_Prev_R(2, 2)/cos(beta_Prev_R));
    double gamma_Last_R = atan2(-rotMatrix_Last_R(1, 2)/cos(beta_Last_R), rotMatrix_Last_R(2, 2)/cos(beta_Last_R));
    double gamma_Cur_R = atan2(-rotMatrix_Cur_R(1, 2)/cos(beta_Cur_R), rotMatrix_Cur_R(2, 2)/cos(beta_Cur_R));

//    if ((m_HandlePosePrev_R.handlePoseR_Roll -140) > gamma_Cur_R * 180 / M_PI){
//        handflag = -1;
//    }
//    if ((m_HandlePosePrev_R.handlePoseR_Roll + 140) < gamma_Cur_R * 180 / M_PI){
//        handflag = 1;
//    }

//    if ((beta_Cur_R * 180 / M_PI > 90) || (beta_Cur_R * 180 / M_PI < -90)){//yaw角
//        if (handflag == -1){
//            gamma_Cur_R = gamma_Cur_R + M_PI;
////            gamma_Init_R = gamma_Init_R + M_PI;//GAMMA INIT的问题因为加了180导致

//        }
//        if (handflag == 1){
//            gamma_Cur_R = gamma_Cur_R - M_PI;
////            gamma_Init_R = gamma_Init_R - M_PI;
//        }
//    }

//    if(m_controlLoopNum >= m_waitTime - 2){//当controlLoop等于1时 cur值为无穷大值
//        if(m_controlLoopNum == m_waitTime - 2){
////            m_gamma_Init_R = gamma_Cur_R;
////            m_gamma_Last_R = 0;
//        }
//    if(m_status_R == 1 || m_controlLoopNum == m_waitTime){
//        m_gamma_Init_R = gamma_Cur_R;
//    }
//    if(m_status_R == 3){
//        m_gamma_Last_R = gamma_Cur_R;
//    }
//    }
//    LOG(INFO) << "Status: " << m_status_R;

//    std::cout<<"handflag: "<<handflag<<std::endl;
    LOG(INFO)<<"gamma_Init_R: "<<gamma_Init_R * 180 / M_PI;
    LOG(INFO)<<"gamma_Cur_R: "<<gamma_Cur_R * 180 / M_PI;
    LOG(INFO)<<"gamma_Last_R: "<<gamma_Last_R * 180 / M_PI;
    LOG(INFO)<<"gamma_org_R: "<<gamma_Org_R * 180 / M_PI;

    double delt_gammaCur_R = (gamma_Cur_R - gamma_Init_R) * 180 / M_PI;
    double delt_gammaInit_R = (gamma_Init_R - gamma_Last_R) * 180 / M_PI;
    double delt_gammaOrg_R = (gamma_Last_R - gamma_Org_R) * 180 / M_PI;
    double delt_gamma_R = delt_gammaCur_R + delt_gammaInit_R * m_AlignmentNumber_R / 100 + delt_gammaOrg_R;

    LOG(INFO) << "delt_gammaCur_R: " << delt_gammaCur_R;
    LOG(INFO) << "delt_gammaInit_R: " << delt_gammaInit_R;
    LOG(INFO) << "delt_gammaOrg_R: " << delt_gammaOrg_R;
    LOG(INFO) << "delt_gamma_R: " << delt_gamma_R;


//       if(alpha_Cur_R * 180 / M_PI > 70){
//           alpha_Cur_R = 70 * M_PI / 180;
//       }
//       else if(alpha_Cur_R * 180 / M_PI < -70){
//           alpha_Cur_R = -70 * M_PI / 180;
//       }

//       if(beta_Cur_R * 180 / M_PI > 90){
//           beta_Cur_R = 90 * M_PI / 180;
//       }
//       else if(beta_Cur_R * 180 / M_PI < -90){
//           beta_Cur_R = - 90 * M_PI / 180;
//       }

 //输出解算后的角度值
    LOG(INFO)<< "roll: " << gamma_Cur_R * 180 / M_PI;

    LOG(INFO)<< "pitch: " << alpha_Cur_R * 180 / M_PI;

    LOG(INFO) << "yaw: " << beta_Cur_R * 180 / M_PI;

    // 设置控制值
    controlValueR[4] = delt_gamma_R; //roll
    controlValueR[5] = deltLength_alpha_R_1; //pitch
    controlValueR[6] = deltLength_alpha_R_2; //pitch

    // yaw
    controlValueR[7] = deltLength_beta_R_left_1;//yaw
    controlValueR[8] = deltLength_beta_R_left_2;//yaw
    controlValueR[9] = deltLength_beta_R_right_1;//yaw
    controlValueR[10] = deltLength_beta_R_right_2;//yaw

    controlValueR[12] = gamma_Cur_R * 180 / M_PI; //row
    controlValueR[13] = alpha_Cur_R * 180 / M_PI; //pitch
    controlValueR[14] = beta_Cur_R * 180 / M_PI; //yaw

/*
    // Grasp控制
    if (handlePosePrev.graspIndex_R == 0 && handlePoseCur.graspIndex_R == 1) {//开始夹合
        controlValueR[6] = 1;
        if (m_HandlePoseLastLoop_R.graspIndex_R == 1) {//离开时为夹合状态
            controlValueR[6] = 2;
        }
    } else if (handlePosePrev.graspIndex_R == 1 && handlePoseCur.graspIndex_R == 0) {开始松手
        controlValueR[6] = 2;
    } else if (handlePosePrev.graspIndex_R == 1 && handlePoseCur.graspIndex_R == 1) {保持夹和
        controlValueR[6] = 3;
    } else {
        controlValueR[6] = 0;
    }
*/
    // 更新 m_HandlePoseLastLoop_R
    //m_HandlePoseLastLoop_R = handlePoseCur;

    //以下为Position解算

    double jointAngle1_Init_R, jointAngle2_Init_R, jointAngle3_Init_R;//（500次循环）motorPos_Init为从电机中返回的值（编码器值），将此转换为相对于零点的弧度值，在第500此循环时开始使能时作为初始值
    jointAngle1_Init_R = -(static_cast<double>(motorPos_Init[Joint1_R]) - static_cast<double>(JointEncoderRevolution1) / 2.0) /   //JointEncoderPerRevolution1 作为标定点零点
            static_cast<double>(JointEncoderPerRevolution) * 360.0 / 180.0 * M_PI;
    jointAngle2_Init_R = (static_cast<double>(motorPos_Init[Joint2_R]) - static_cast<double>(JointEncoderRevolution2) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 / 180.0 * M_PI;
    jointAngle3_Init_R = -(static_cast<double>(motorPos_Init[Joint3_R]) - static_cast<double>(JointEncoderRevolution3) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 / 180.0 * M_PI;

    double x_Init_R = m_endEffectorInitPos_R[0];// 端点相对于零点的坐标实际位置
    double y_Init_R = m_endEffectorInitPos_R[1];// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double z_Init_R = m_endEffectorInitPos_R[2];// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    //以下为姿态补偿部分
    double instrument_1_length = 3.83;
    double instrument_2_length = 4.10;
    Eigen::Vector3d vector_l1 = {instrument_1_length, 0, 0};
    Eigen::Vector3d vector_l2 = {instrument_2_length, 0, 0};

    Eigen::Matrix3d rotationMatrix_yaw;
    rotationMatrix_yaw << cos(beta_Cur_R),-sin(beta_Cur_R),       0,
                          sin(beta_Cur_R), cos(beta_Cur_R),       0,
                          0,                             0,       1;

    Eigen::Matrix3d rotationMatrix_pitch;
    rotationMatrix_pitch << cos(alpha_Cur_R),   0,  sin(alpha_Cur_R),
                            0,                  1,                 0,
                           -sin(alpha_Cur_R),   0,  cos(alpha_Cur_R);

    Eigen::Matrix3d rotationMatrix_roll;
    rotationMatrix_roll << 1,                   0,                 0,
                           0,    cos(gamma_Cur_R), -sin(gamma_Cur_R),
                           0,    sin(gamma_Cur_R),  cos(gamma_Cur_R);

    Eigen::Vector3d buchang_l1;
    buchang_l1 = rotSlaveMatrix_R * rotationMatrix_roll * rotationMatrix_pitch * vector_l1;

    Eigen::Vector3d buchang_l2;
    buchang_l2 = rotSlaveMatrix_R * rotationMatrix_roll * rotationMatrix_pitch * rotationMatrix_yaw * vector_l2;

    Eigen::Vector3d buchang_sum;
    buchang_sum = buchang_l1 + buchang_l2;

    Eigen::Vector3d buchang_delta;
    buchang_delta.x() = buchang_sum.x() - 3.965;
    buchang_delta.y() = buchang_sum.y() - 3.4338;
    buchang_delta.z() = buchang_sum.z() + 5.9475;//补偿在init时候也要保存下来

//      以下为姿态改变时导致的xyz位移补偿，在每次进入使能时需要被添加在init中
//    double x_Delt_R = (PositionCur[0] - PositionInit[0]) / 0.7 - (buchang_delta.x());//主手运动距离除以7 //Viper单位为mm
//    double y_Delt_R = -(PositionCur[1] - PositionInit[1]) / 0.7 - (buchang_delta.y());
//    double z_Delt_R = -(PositionCur[2] - PositionInit[2]) / 0.7 - (buchang_delta.z());

    double x_Delt_R = (PositionCur[0] - PositionInit[0]) / 0.1 / m_scale;//主手运动距离除以7 //Viper单位为mm
    double y_Delt_R = -(PositionCur[1] - PositionInit[1]) / 0.1 / m_scale;
    double z_Delt_R = -(PositionCur[2] - PositionInit[2]) / 0.1 / m_scale;


    //上面三行由于画圈需要被注释掉了，以下为修改部分
/*
    double x_Delt_R;
    double y_Delt_R;
    double z_Delt_R;
    double r_now;

    double x_R;
    double y_R;
    double z_R;

    if(m_controlLoopNum >= m_waitTime + 10){
    if(test_angle < 360){
        r_now = r_test / 10;
        x_Delt_R = sin(test_angle * M_PI / 180.0) * r_now;
        y_Delt_R = - (r_now - cos(test_angle * M_PI /180.0) * r_now);

        x_R = sin(test_angle * M_PI / 180.0) * r_test / 10;
        y_R = -(r_now - cos(test_angle * M_PI /180.0) * r_test / 10);
        delta_z = delta_z + 0.0001;
        z_Delt_R = delta_z;
        test_angle = test_angle + 0.2;
    }
    else if(test_angle < 720){
        r_now = r_test / 15;
        x_Delt_R = sin(test_angle * M_PI / 180.0) * r_now;
        y_Delt_R = -(r_now - cos(test_angle * M_PI /180.0) * r_now);

        x_R = sin(test_angle * M_PI / 180.0) * r_test / 10;
        y_R = -(r_now - cos(test_angle * M_PI /180.0) * r_test / 10);

        delta_z = delta_z + 0.0001;
        z_Delt_R = delta_z;
        test_angle = test_angle + 0.2;
    }
    else if(test_angle < 1080){
        r_now = r_test / 20;
        x_Delt_R = sin(test_angle * M_PI / 180.0) * r_now;
        y_Delt_R = -(r_now - cos(test_angle * M_PI /180.0) * r_now);

        x_R = sin(test_angle * M_PI / 180.0) * r_test / 10;
        y_R = -(r_now - cos(test_angle * M_PI /180.0) * r_test / 10);

        delta_z = delta_z + 0.0001;
        z_Delt_R = delta_z;
        test_angle = test_angle + 0.2;
    }
    else{
        x_Delt_R = 0;
        y_Delt_R = 0;
        z_Delt_R = 0;
    }

    }else{
        x_Delt_R = 0;
        y_Delt_R = 0;
        z_Delt_R = 0;
    }

    LOG(INFO) << "x1: " << PositionCur[0];
    LOG(INFO) << "y1: " << PositionCur[1];
    LOG(INFO) << "z1: " << PositionCur[2];
    LOG(INFO) << "x2: " << x_R;
    LOG(INFO) << "y2: " << y_R;
    LOG(INFO) << "z2: " << z_Delt_R;
*/

//    以下为重复定位精度测试

/*    double length = 30;//边长
    int test_step = 500;
    Eigen::Vector3d Point_0 = {0, 0, 0};
    Eigen::Vector3d Point_1 = {-0.5 * length, 0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_2 = {-0.5 * length, -0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_3 = {0.5 * length, -0.5 * length, 0.5 * length};
    Eigen::Vector3d Point_4 = {0.5 * length, 0.5 * length, 0.5 * length};

    if(m_controlLoopNum > m_waitTime){ // && (m_controlLoopNum < 5 * test_step * 5.1)){
//LOG(INFO)<< "test_index" << test_index;
    switch (test_index) {
    case 0:
        test_x = test_x + (Point_1.x() - Point_0.x())/test_step;
        test_y = test_y + (Point_1.y() - Point_0.y())/test_step;
        test_z = test_z + (Point_1.z() - Point_0.z())/test_step;
        break;
    case 1:
        test_x = test_x + (Point_2.x() - Point_1.x())/test_step;
        test_y = test_y + (Point_2.y() - Point_1.y())/test_step;
        test_z = test_z + (Point_2.z() - Point_1.z())/test_step;
        break;
    case 2:
        test_x = test_x + (Point_3.x() - Point_2.x())/test_step;
        test_y = test_y + (Point_3.y() - Point_2.y())/test_step;
        test_z = test_z + (Point_3.z() - Point_2.z())/test_step;
        break;
    case 3:
        test_x = test_x + (Point_4.x() - Point_3.x())/test_step;
        test_y = test_y + (Point_4.y() - Point_3.y())/test_step;
        test_z = test_z + (Point_4.z() - Point_3.z())/test_step;
        break;
    case 4:
        test_x = test_x + (Point_0.x() - Point_4.x())/test_step;
        test_y = test_y + (Point_0.y() - Point_4.y())/test_step;
        test_z = test_z + (Point_0.z() - Point_4.z())/test_step;
        break;
    }

    test_time = test_time + 1;
    auto position = m_MotorCurEncoder.load();

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
    }else if(test_time == 5 * test_step){
        test_time = 0;
        test_index = 0;
        test_circle = test_circle + 1;
    }

}
    double x_Delt_R = test_x;
    double y_Delt_R = test_y;
    double z_Delt_R = test_z;
*/

//以下为画重复定位精度测试
/*

    double x_finish;
    double y_finish;
    double z_finish;

if((test_circle == 6)&&(test_time = 0)){
   x_finish = test_x;//精度测试
   y_finish = test_y;
   z_finish = test_z;
}
if((test_circle == 6)&&(test_time = 5)){
LOG(INFO)<<"=====================================================================================================================================";
}

if(test_circle >= 6){
    x_Delt_R = x_finish;
    y_Delt_R = y_finish;
    z_Delt_R = z_finish;
}


    以下为一次循环代码

    double length = 30;//边长
    int test_step = 500;
    Eigen::Vector3d Point_0 = {0, 0, 0};
    Eigen::Vector3d Point_1 = {-0.5 * length, 0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_2 = {-0.5 * length, -0.5 * length, -0.5 * length};
    Eigen::Vector3d Point_3 = {0.5 * length, -0.5 * length, 0.5 * length};
    Eigen::Vector3d Point_4 = {0.5 * length, 0.5 * length, 0.5 * length};

    if(m_controlLoopNum > m_waitTime){ // && (m_controlLoopNum < 5 * test_step * 5.1)){
//LOG(INFO)<< "test_index" << test_index;
    switch (test_index) {
    case 0:
        test_x = test_x + (Point_1.x() - Point_0.x())/test_step;
        test_y = test_y + (Point_1.y() - Point_0.y())/test_step;
        test_z = test_z + (Point_1.z() - Point_0.z())/test_step;
        break;
    case 1:
        test_x = test_x + (Point_2.x() - Point_1.x())/test_step;
        test_y = test_y + (Point_2.y() - Point_1.y())/test_step;
        test_z = test_z + (Point_2.z() - Point_1.z())/test_step;
        break;
    case 2:
        test_x = test_x + (Point_3.x() - Point_2.x())/test_step;
        test_y = test_y + (Point_3.y() - Point_2.y())/test_step;
        test_z = test_z + (Point_3.z() - Point_2.z())/test_step;
        break;
    case 3:
        test_x = test_x + (Point_4.x() - Point_3.x())/test_step;
        test_y = test_y + (Point_4.y() - Point_3.y())/test_step;
        test_z = test_z + (Point_4.z() - Point_3.z())/test_step;
        break;
    case 4:
        test_x = test_x + (Point_0.x() - Point_4.x())/test_step;
        test_y = test_y + (Point_0.y() - Point_4.y())/test_step;
        test_z = test_z + (Point_0.z() - Point_4.z())/test_step;
        break;
    }

    test_time = test_time + 1;
    auto position = m_MotorCurEncoder.load();

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
    }else if(test_time == 5 * test_step){
        test_index = 5;
    }

}
    double x_Delt_R = test_x;
    double y_Delt_R = test_y;
    double z_Delt_R = test_z;

    double x_finish;
    double y_finish;
    double z_finish;


    if(test_time == 5 * test_step){
       x_finish = test_x;//精度测试
       y_finish = test_y;
       z_finish = test_z;
    }

    if(test_index==5){
        x_Delt_R = x_finish;
        y_Delt_R = y_finish;
        z_Delt_R = z_finish;
               LOG(INFO)<<"=================================================================================================================================================";
    }

*/


//以下为来回走直线代码

/*

std::array<int, MotorNum>   motorPositionCur = {};
motorPositionCur = m_MotorCurEncoder.load();//第1 第2 第3个电机记录下来

if(m_controlLoopNum>m_waitTime){

if(index<100)
{
    test_y = test_y + 0.05;
    index = index+1;
}

else if(index<300)
{
    test_y = test_y - 0.05;
    index = index+1;
}

else if(index<500)
{
    test_y = test_y + 0.05;
    index = index+1;
}

else if(index<700)
{
    test_y = test_y - 0.05;
    index = index +1;
}

else if(index<900)
{
    test_y = test_y + 0.05;
    index = index +1;
}

else if(index<1000)
{
    test_y = test_y - 0.05;
    index = index +1;
}
}

*/

    //以下19行为路径规划

        m_ruckigInputState.target_position = {(x_Init_R + x_Delt_R), (y_Init_R + y_Delt_R), (z_Init_R + z_Delt_R)};
        m_ruckigInputState.target_velocity = {0.0, 0.0, 0.0};
        m_ruckigInputState.target_acceleration = {0.0, 0.0, 0.0};

        auto plannerStatus = m_ruckigPlanner.update(m_ruckigInputState, m_ruckigOutputState);
        if(plannerStatus == ruckig::Result::Working){

    //        LOG(INFO) << "The planned new position is: " << ruckig::join(m_ruckigOutputState.new_position);
            ruckig::join(m_ruckigOutputState.new_position);
        }
        else
        {
    //        LOG(INFO) << "planner Status: " <<std::dec << plannerStatus;
        }
        m_ruckigOutputState.pass_to_input(m_ruckigInputState);

        double x_Target_R = m_ruckigOutputState.new_position[0];// / m_motionScaling[2]，初始值加上delta值为末端点应该移动到的位置，以此位置解算
        double y_Target_R = m_ruckigOutputState.new_position[1];// / m_motionScaling[0]
        double z_Target_R = m_ruckigOutputState.new_position[2];// / m_motionScaling[0]


//    double x_Target_R = x_Init_R + x_Delt_R;
//    double y_Target_R = y_Init_R + y_Delt_R;
//    double z_Target_R = z_Init_R + z_Delt_R;


////          LOG(INFO) << "circle: " << test_circle;
//        LOG(INFO)<< "motor: " << endEffectorPositionTmp[1];
//        LOG(INFO)<< "data: " << y_Target_R;
//        LOG(INFO)<< "Number: " << m_AlignmentNumber_R;

//LOG(INFO)<< "x_target: " << x_Target_R ;
//LOG(INFO)<< "y_target: " << y_Target_R ;
//LOG(INFO)<< "z_target: " << z_Target_R ;


    double jointAngle1_target_R, jointAngle2_target_R, jointAngle3_target_R;
    jointAngle2_target_R = acos((y_Target_R * y_Target_R + z_Target_R * z_Target_R - (m_endArm_1 * m_endArm_1  + m_endArm_2 * m_endArm_2))/
                                (2 * m_endArm_1 * m_endArm_2));
    // equations for solving q2:
    //      y = sq1 * (l1 + l2 * cq2) + l2 * cq2 * sq2;
    //      Z = - cq1 * (l1 + l2 * cq2) + l2 * sq1 * sq2;
    double coefficient_tmp1 = m_endArm_1 + m_endArm_2 * cos(jointAngle2_target_R);

    double coefficient_tmp2 = m_endArm_2 * sin(jointAngle2_target_R);

    jointAngle1_target_R = atan(-abs(coefficient_tmp1 * y_Target_R + coefficient_tmp2 * z_Target_R) /
                                 abs(coefficient_tmp2 * y_Target_R - coefficient_tmp1 * z_Target_R));

    //Control Value:
    //  [0]:         X-Axis          Target position
    //[1 - 3]:   End Joint 1 - 3     Target Angle

    controlValueR[0] = x_Target_R - x_Init_R; //(handpose_cur-handpose_init)单位为mm
    controlValueR[1] = (jointAngle1_target_R - jointAngle1_Init_R) * 180 / M_PI;//输出为各关节角度
    controlValueR[2] = (jointAngle2_target_R - jointAngle2_Init_R) * 180 / M_PI;
    controlValueR[3] = (Angle_30 - jointAngle1_target_R * 180 / M_PI - jointAngle2_target_R * 180 / M_PI)
            -(Angle_30 - jointAngle1_Init_R * 180 / M_PI - jointAngle2_Init_R * 180 / M_PI);

    return controlValueR;
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


std::array<std::array<int,11>, 3>  RobotControl::forwardKinematics(const std::array<double, 15>& controlValue_Prev, const std::array<double, 15>& controlValue_Cur,
        const std::array<int, 11>& motorPosition_Init, const std::array<int, 11>& motorPosition_Cur,const HandlePose& handlePoseCur){

        std::array<std::array<int,11>, 3> targetSpeedPosition;

        //[0][0-9]speed control [1][0-9]position control

/*
//        //以下为第4个zero erro
//           if(abs(controlValue_Cur[4] - controlValue_Prev[4]) > 0.001)//roll 电机精度360度/524288
//           {
//            targetSpeedPosition[0][4] = int((controlValue_Cur[4] - controlValue_Prev[4]) * yum_kForcepPosition_R[4]/(1.0/240.0));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
//            targetSpeedPosition[1][4] = yum_SpeedDirection_R[4] * controlValue_Cur[4] * yum_kForcepPosition_R[4] + JointEncoderPerRevolution / 2; //;
//            }else
//            {
//            targetSpeedPosition[0][4] = 0;
//            targetSpeedPosition[1][4] = motorPosition_Cur[4];
//            }

//        for(int i = 5; i < 7; i++ )//pitch的两个电机
//        {
//          if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > 0.0004)//电缸精度10mm/16834
//           {
//            targetSpeedPosition[0][i] = int((controlValue_Cur[i] - controlValue_Prev[i]) * yum_kForcepPosition_R[i]/(1.0/240.0));//单位 位每秒
//            targetSpeedPosition[1][i] = (yum_SpeedDirection_R[i] * controlValue_Cur[i] * yum_kForcepPosition_R[i] + 8192.0);//现阶段考虑位置控制
//            if(abs(targetSpeedPosition[0][i]) > yum_ForcepSpeedLimit[i])
//            {
//              targetSpeedPosition[0][i] = yum_SpeedDirection_R[i] * (targetSpeedPosition[0][i] + 100000.0) / abs(targetSpeedPosition[0][i] + 100000.0) * yum_ForcepSpeedLimit[i];//240Hz
//            }
//            }else
//            {
//            targetSpeedPosition[0][i] = 0;
//            targetSpeedPosition[1][i] = motorPosition_Cur[i];
//            }
//        }

*/

        //Maxon Roll

         targetSpeedPosition[0][4] = int(0.01 * yum_SpeedDirection_R[4] *(controlValue_Cur[4] - controlValue_Prev[4]) * yum_kForcepPosition_R[4]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][4] = yum_SpeedDirection_R[4] * controlValue_Cur[4] * yum_kForcepPosition_R[4];// + MaxonEncoderPerRevolution / 2;

//        Maxon pitch

         targetSpeedPosition[0][5] = int(0.01 * yum_SpeedDirection_R[5] *(controlValue_Cur[5] - controlValue_Prev[5]) * yum_kForcepPosition_R[5]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][5] = yum_SpeedDirection_R[5] * controlValue_Cur[5] * yum_kForcepPosition_R[5];

//        Maxon yaw11

         targetSpeedPosition[0][7] = int(0.01 * yum_SpeedDirection_R[7] *(controlValue_Cur[7] - controlValue_Prev[7]) * yum_kForcepPosition_R[7]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][7] = yum_SpeedDirection_R[7] * controlValue_Cur[7] * yum_kForcepPosition_R[7];

//        Maxon yaw12

         targetSpeedPosition[0][8] = int(0.01 * yum_SpeedDirection_R[8] *(controlValue_Cur[8] - controlValue_Prev[8]) * yum_kForcepPosition_R[7]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][8] = yum_SpeedDirection_R[8] * controlValue_Cur[8] * yum_kForcepPosition_R[7];

//        Maxon yaw21

         targetSpeedPosition[0][9] = int(0.01 * yum_SpeedDirection_R[9] *(controlValue_Cur[9] - controlValue_Prev[9]) * yum_kForcepPosition_R[8]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][9] = yum_SpeedDirection_R[9] * controlValue_Cur[9] * yum_kForcepPosition_R[8];

//        Maxon yaw22

         targetSpeedPosition[0][10] = int(0.01 * yum_SpeedDirection_R[10] *(controlValue_Cur[10] - controlValue_Prev[10]) * yum_kForcepPosition_R[9]/(0.004));//单位 位每秒 (Viper更新频率为240Hz，for循环的更新频率为400Hz）
         targetSpeedPosition[1][10] = yum_SpeedDirection_R[10] * controlValue_Cur[10] * yum_kForcepPosition_R[9];


//        for(int i = 7; i < 11; i++ )//yaw的四个电机
//        {
//          if(abs(controlValue_Cur[i] - controlValue_Prev[i]) > 0.0004)//电缸精度10mm/16834
//           {
//            targetSpeedPosition[0][i] = int((controlValue_Cur[i] - controlValue_Prev[i]) * yum_kForcepPosition_R[i]/(1.0/240.0));//单位 位每秒
//            targetSpeedPosition[1][i] = (yum_SpeedDirection_R[i] * controlValue_Cur[i] * yum_kForcepPosition_R[i] + 8192.0);//现阶段考虑位置控制
//            if(abs(targetSpeedPosition[0][i]) > yum_ForcepSpeedLimit[i])
//            {
//              targetSpeedPosition[0][i] = yum_SpeedDirection_R[i] * (targetSpeedPosition[0][i]+ 100000.0) / abs(targetSpeedPosition[0][i] + 100000.0) * yum_ForcepSpeedLimit[i];//240Hz
//            }
//            }else
//            {
//            targetSpeedPosition[0][i] = 0;
//            targetSpeedPosition[1][i] = motorPosition_Cur[i];
//            }
//        }


//         targetSpeedPosition[1][0] =  static_cast<int>(motorPosition_Init[0] + yum_SpeedDirection_R[0] * controlValue_Cur[0]*2000);

         targetSpeedPosition[0][0] = int(yum_SpeedDirection_R[0] * (controlValue_Cur[0] - controlValue_Prev[0]) * yum_kForcepPosition_R[0]/(0.004));
         targetSpeedPosition[1][0] =  static_cast<int>(motorPosition_Init[0] + yum_SpeedDirection_R[0] * controlValue_Cur[0] * yum_kForcepPosition_R[0]);//4000 code每mm

         targetSpeedPosition[0][1] = int(yum_SpeedDirection_R[1] * (controlValue_Cur[1] - controlValue_Prev[1]) * yum_kForcepPosition_R[1]/(0.004));
         targetSpeedPosition[1][1] =  static_cast<int>(motorPosition_Init[1] + yum_SpeedDirection_R[1] * controlValue_Cur[1] / 360 * JointEncoderPerRevolution);

         targetSpeedPosition[0][2] = int(yum_SpeedDirection_R[2] * (controlValue_Cur[2] - controlValue_Prev[2]) * yum_kForcepPosition_R[2]/(0.004));
         targetSpeedPosition[1][2] =  static_cast<int>(motorPosition_Init[2] + yum_SpeedDirection_R[2] * controlValue_Cur[2] / 360 * JointEncoderPerRevolution);

         targetSpeedPosition[0][3] = int(yum_SpeedDirection_R[3] * (controlValue_Cur[3] - controlValue_Prev[3]) * yum_kForcepPosition_R[3]/(0.004));
         targetSpeedPosition[1][3] =  static_cast<int>(motorPosition_Init[3] + yum_SpeedDirection_R[3] * controlValue_Cur[3] / 360 * JointEncoderPerRevolution);


        return targetSpeedPosition;
 }


std::array<double, 4>  RobotControl::calculateEndeffectorAngle(const std::array<double, 4> masterJointAngle)const
{
    std::array<double, 4> materJointAngle_Rad = {0};

    std::array<double, 4> endEffectorAngle_Rad = {0};

    std::array<double, 4> endEffectorAngle = {0};

    for(int i = 0; i < 3; i++)
    {
        materJointAngle_Rad[i] = masterJointAngle[i] / 180  * M_PI;
    }

    //
    Eigen::Matrix3d rotMatrixAroundX, rotMatrixAroundY, rotMatrixAroundZ;

    rotMatrixAroundX = Eigen::AngleAxisd(materJointAngle_Rad[0], Eigen::Vector3d::UnitX());

    rotMatrixAroundY = Eigen::AngleAxisd(materJointAngle_Rad[1], Eigen::Vector3d::UnitY());

    rotMatrixAroundZ = Eigen::AngleAxisd(materJointAngle_Rad[2], Eigen::Vector3d::UnitZ());

    Eigen::Matrix3d rotMatrix_Master = rotMatrixAroundZ * rotMatrixAroundY * rotMatrixAroundX;

    auto r11 = rotMatrix_Master(0,0);
    auto r12 = rotMatrix_Master(0,1);
    auto r13 = rotMatrix_Master(0,2);
    auto r21 = rotMatrix_Master(1,0);
    auto r22 = rotMatrix_Master(1,1);
    auto r23 = rotMatrix_Master(1,2);
    auto r31 = rotMatrix_Master(2,0);
    auto r32 = rotMatrix_Master(2,1);
    auto r33 = rotMatrix_Master(2,2);

    endEffectorAngle_Rad[0] = atan2(-r12, r11);
    endEffectorAngle_Rad[1] = atan2(r13, sqrt(r11 * r11 + r12 * r12));
    endEffectorAngle_Rad[2] = atan2(r23, r33);

    for(int i = 0; i < 3; i++)
    {
        endEffectorAngle[i] = endEffectorAngle_Rad[i] / M_PI * 180;
    }
    return endEffectorAngle;
}


std::array<double, DOF> RobotControl::calculateEndEffectorPosition(const HandlePose& handlePoseCur, const std::array<int,MotorNum>& motorPos_Cur, const char& side)//yu 通过各电机的读数推断端点在坐标系中的位置
{

if(side=='r'){
    double jointAngle1_Cur_R, jointAngle2_Cur_R, jointAngle3_Cur_R;//标定零点是编码器值的一半
    jointAngle1_Cur_R = -(static_cast<double>(motorPos_Cur[Joint1_R]) - static_cast<double>(JointEncoderRevolution1) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle2_Cur_R = (static_cast<double>(motorPos_Cur[Joint2_R]) - static_cast<double>(JointEncoderRevolution2) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle3_Cur_R = -(static_cast<double>(motorPos_Cur[Joint3_R]) - static_cast<double>(JointEncoderRevolution3) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    double x_Cur_R = handlePoseCur.handlePoseR_X;
    double y_Cur_R = m_endArm_1 * sin(jointAngle1_Cur_R) + m_endArm_2 * sin(jointAngle1_Cur_R + jointAngle2_Cur_R);// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double z_Cur_R = - m_endArm_1 * cos(jointAngle1_Cur_R) - m_endArm_2 * cos(jointAngle1_Cur_R + jointAngle2_Cur_R);// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    std::array<double, DOF> endEffectorPositionTmp = {x_Cur_R, y_Cur_R, z_Cur_R};//在世界坐标系中x y z的值

    return endEffectorPositionTmp;
}
else if(side=='l'){
    double jointAngle1_Cur_L, jointAngle2_Cur_L, jointAngle3_Cur_L;//标定零点是编码器值的一半
    jointAngle1_Cur_L = -(static_cast<double>(motorPos_Cur[Joint1_R]) - static_cast<double>(JointEncoderRevolution1) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle2_Cur_L = (static_cast<double>(motorPos_Cur[Joint2_R]) - static_cast<double>(JointEncoderRevolution2) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    jointAngle3_Cur_L = -(static_cast<double>(motorPos_Cur[Joint3_R]) - static_cast<double>(JointEncoderRevolution3) / 2.0) /
            static_cast<double>(JointEncoderPerRevolution) * 360.0 /180.0 * M_PI;
    double x_Cur_L = handlePoseCur.handlePoseR_X;
    double y_Cur_L = m_endArm_1 * sin(jointAngle1_Cur_L) + m_endArm_2 * sin(jointAngle1_Cur_L + jointAngle2_Cur_L);// +m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)
    double z_Cur_L = - m_endArm_1 * cos(jointAngle1_Cur_L) - m_endArm_2 * cos(jointAngle1_Cur_L + jointAngle2_Cur_L);// -m_endArm_3 * sin(jointAngle1_Init_R + jointAngle2_Init_R + jointAngle3_Init_R)

    std::array<double, DOF> endEffectorPositionTmp = {x_Cur_L, y_Cur_L, z_Cur_L};//在世界坐标系中x y z的值

    return endEffectorPositionTmp;
}

}


std::array<int, MotorNum> RobotControl::calculateTargetPosition(const std::array<double, ControlValueNum>& controlValue_Prev, const std::array<double, ControlValueNum>& controlValue_Cur,
                                                        const std::array<int, MotorNum>& motorPosition_Init, const std::array<int, MotorNum>& motorPosition_Cur,
                                                        const HandlePose& masterHandlePose_Cur) const
{
    std::array<int, MotorNum> targetEncoder = {0};

    targetEncoder[0] =  static_cast<int>((controlValue_Cur[0] - controlValue_Prev[0]) * 10000000 * 500);//丝杠
    targetEncoder[1] =  static_cast<int>(motorPosition_Init[1] - controlValue_Cur[1] / 360 * JointEncoderPerRevolution);//1-3为电机，方向相反
    targetEncoder[2] =  static_cast<int>(motorPosition_Init[2] - controlValue_Cur[2] / 360 * JointEncoderPerRevolution);
    targetEncoder[3] =  static_cast<int>(motorPosition_Init[3] - controlValue_Cur[3] / 360 * JointEncoderPerRevolution);
    targetEncoder[4] = static_cast<int>(motorPosition_Init[4] + controlValue_Cur[4] * m_encoderPerDegree_R[0]);
    targetEncoder[5] = static_cast<int>(motorPosition_Init[5] + controlValue_Cur[5] * m_encoderPerDegree_R[1]);
    targetEncoder[6] = static_cast<int>(motorPosition_Init[6] + controlValue_Cur[6] * m_encoderPerDegree_R[2]);
    targetEncoder[7] = static_cast<int>(motorPosition_Init[7] + controlValue_Cur[7] * m_encoderPerDegree_R[3]);

//    LOG(INFO) << std::dec <<"Init Encoder: " << motorPosition_Init <<"  target Encoder: " << targetEncoder;
    return targetEncoder;
}

void RobotControl::startUpdataMasterConsoleDataThread()//yu
{
    m_updateMasterConsoleThread = std::thread(&RobotControl::updateMasterConsoleData, this);//新建一个线程

    m_updateMasterConsoleThread.detach();//对此线程进行分离
}

void RobotControl::updateMasterConsoleData()//yu
{
    while(flagUpdateMasterConsoleData.load())
    {
        auto Pose = m_masterConsole.returnHandlePose();//3个位置夹3个姿态，参数Pose为HandlePose类

        std::array<std::array<double, viperDataNumPerSensor>, 2> Matrix = Pose.returnQuaternionData();

        double q_l_0 = Matrix[0][3];//0读取的是左手Viper的值
        double q_l_1 = Matrix[0][4];
        double q_l_2 = Matrix[0][5];
        double q_l_3 = Matrix[0][6];

        double q_r_0 = Matrix[1][3];//0读取的是右手Viper的值
        double q_r_1 = Matrix[1][4];
        double q_r_2 = Matrix[1][5];
        double q_r_3 = Matrix[1][6];

        Eigen::Matrix3d rotationR = RobotControl::ToMasterRotationMatrix(q_r_0, q_r_1, q_r_2, q_r_3);
        Eigen::Matrix3d rotationL = RobotControl::ToMasterRotationMatrix(q_l_0, q_l_1, q_l_2, q_l_3);
        Pose.setRotationDataR(rotationR);
        Pose.setRotationDataL(rotationL);


        m_HandlePose_Cur.store(Pose);

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void RobotControl::startConmunicateWithMotorDriverThread()//yu
{

    m_communicateWithMotorDriverThread = std::thread(&RobotControl::communicateWithMotorDriver, this);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    m_communicateWithMotorDriverThread.detach();
}

void RobotControl::communicateWithMotorDriver()//yu
{

    while(true)            
    {
        if(m_flagCommunicateWithMotorDriver.load())
        {
          receiveMotorData();
        }

        if(m_flagInTeleoperation.load())
        {
           sendMotorData();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(4));

    }
}

void RobotControl::receiveMotorData()//yu接受传回来的数据
{
    std::array<int, MotorNum> motorEncoderData = {0};

    motorEncoderData[0] = m_motorDriver->getActualPos(MotorType::MOONS, 0);
    motorEncoderData[1] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 0);
    motorEncoderData[2] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 1);
    motorEncoderData[3] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 2);
//    motorEncoderData[4] = m_motorDriver->getActualPos(MotorType::ZERO_ERR, 3);
    motorEncoderData[4] = m_motorDriver->getActualPos(MotorType::MAXON, 0);
    motorEncoderData[5] = m_motorDriver->getActualPos(MotorType::MAXON, 1);
    motorEncoderData[6] = m_motorDriver->getActualPos(MotorType::MAXON, 2);
    motorEncoderData[7] = m_motorDriver->getActualPos(MotorType::MAXON, 3);
    motorEncoderData[8] = m_motorDriver->getActualPos(MotorType::MAXON, 4);
    motorEncoderData[9] = m_motorDriver->getActualPos(MotorType::MAXON, 5);


    std::array<int, MotorNum> motorErrorCode = {0};

    motorErrorCode[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0);
    motorErrorCode[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0);
    motorErrorCode[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1);
    motorErrorCode[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2);
//    motorErrorCode[4] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 3);
    motorErrorCode[4] =  m_motorDriver->getErrorCode(MotorType::MAXON, 0);
    motorErrorCode[5] =  m_motorDriver->getErrorCode(MotorType::MAXON, 1);
    motorErrorCode[6] =  m_motorDriver->getErrorCode(MotorType::MAXON, 2);
    motorErrorCode[7] =  m_motorDriver->getErrorCode(MotorType::MAXON, 3);
    motorErrorCode[8] =  m_motorDriver->getErrorCode(MotorType::MAXON, 4);
    motorErrorCode[9] =  m_motorDriver->getErrorCode(MotorType::MAXON, 5);


    std::array<int, MotorNum> motorStatusWord = {0};

    motorStatusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
    motorStatusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
    motorStatusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
    motorStatusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//    motorStatusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
    motorStatusWord[4] =  m_motorDriver->getStatusWord(MotorType::MAXON, 0);
    motorStatusWord[5] =  m_motorDriver->getStatusWord(MotorType::MAXON, 1);
    motorStatusWord[6] =  m_motorDriver->getStatusWord(MotorType::MAXON, 2);
    motorStatusWord[7] =  m_motorDriver->getStatusWord(MotorType::MAXON, 3);
    motorStatusWord[8] =  m_motorDriver->getStatusWord(MotorType::MAXON, 4);
    motorStatusWord[9] =  m_motorDriver->getStatusWord(MotorType::MAXON, 5);

    std::array<int, MotorNum> motorVelocity = {0};

    motorVelocity[0] =  m_motorDriver->getActualVel(MotorType::MOONS, 0);
    motorVelocity[1] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 0);
    motorVelocity[2] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 1);
    motorVelocity[3] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 2);
//    motorVelocity[4] =  m_motorDriver->getActualVel(MotorType::ZERO_ERR, 3);

    std::array<int, MotorNum> motorCurrent = {0};

    motorCurrent[0] =  m_motorDriver->getActualCur(MotorType::MOONS, 0);//getActualCur(const MotorType& type, const int& index)
    motorCurrent[1] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 0);
    motorCurrent[2] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 1);
    motorCurrent[3] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 2);
//    motorCurrent[4] =  m_motorDriver->getActualCur(MotorType::ZERO_ERR, 3);

    //getActualTrq
    std::array<int, MotorNum> motorTrq = {0};

    motorTrq[0] =  0;//m_motorDriver->getActualTrq(MotorType::MOONS, 0);
    motorTrq[1] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 0);
    motorTrq[1] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 1);
    motorTrq[1] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 2);
//    motorTrq[1] =  m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 3);


    std::array<int, MotorNum> motorOperationMode = {0};

    motorOperationMode[0] =  m_motorDriver->getOperationMode(MotorType::MOONS, 0);
    motorOperationMode[1] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
    motorOperationMode[2] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
    motorOperationMode[3] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
//    motorOperationMode[4] =  m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 3);
    motorOperationMode[4] =  m_motorDriver->getOperationMode(MotorType::MAXON, 0);
    motorOperationMode[5] =  m_motorDriver->getOperationMode(MotorType::MAXON, 1);
    motorOperationMode[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 2);
    motorOperationMode[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 3);
    motorOperationMode[6] =  m_motorDriver->getOperationMode(MotorType::MAXON, 4);
    motorOperationMode[7] =  m_motorDriver->getOperationMode(MotorType::MAXON, 5);

    //getDigitalInput
    std::array<int, 8> motorInputs = {0};

    motorInputs[0] = m_motorDriver->getDigitalInputs(MotorType::MOONS, 0)[6];
//    motorInputs[4] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 0)[0];
//    motorInputs[5] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 1)[0];
//    motorInputs[6] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 2)[0];
//    motorInputs[7] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 3)[0];
//    motorInputs[8] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4)[0];
//    motorInputs[9] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5)[0];

    motorInputs[1] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 0)[2];
    motorInputs[2] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 1)[2];
    motorInputs[3] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 2)[2];
    motorInputs[4] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 3)[2];
    motorInputs[5] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 4)[2];
    motorInputs[6] = m_motorDriver->getDigitalInputs(MotorType::MAXON, 5)[2];


//    LOG(INFO) << "receive Motor Data: Position: " << std::dec << motorEncoderData;

//    LOG(INFO) << "StatusWord: " << std::hex << motorStatusWord << std::endl;

//    LOG(INFO) << "ErrCode: " << std::hex << motorErrorCode << std::endl;

//    LOG(INFO) << "Velocity: " << std::dec << motorVelocity;

//    LOG(INFO) << "Current: " << std::dec << motorCurrent;

//    LOG(INFO) << "Trq: " << std::dec << motorTrq;
//     LOG(INFO)  << "digital光电门： " << motorInputs;

    m_MotorCurEncoder.store(motorEncoderData);
    m_MotorCurStatusWord.store(motorStatusWord);
    m_MotorHomingStatus.store(motorInputs);//记录光电门是否被激活，为0时表示被激活

//    std::cout <<  StatusTmp[0] <<std::endl;

}

void RobotControl::sendMotorData()//发送数据
{
    auto curRobotControlMode = m_curRobotControlMode.load();
    auto motorTargetEncoder = m_MotorTargetEncoder.load();
    auto motorTargetVel = m_MotorTargetVel.load();
//    LOG(INFO) << "Motor Target Encoder: " << std::dec << motorTargetEncoder;

//    std::cout << "RobotControlMode: " << static_cast<int>(curRobotControlMode) << std::endl;
    if(curRobotControlMode == RobotControlMode::TeleOperation)
    {
        if(m_controlLoopNum >= m_waitTime){

        m_motorDriver->setTargetPos(MotorType::MOONS, 0, static_cast<int>(motorTargetEncoder[0]));
        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 0, static_cast<int>(motorTargetEncoder[1]));
        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 1, static_cast<int>(motorTargetEncoder[2]));
        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 2, static_cast<int>(motorTargetEncoder[3]));

        m_motorDriver->setTargetPos(MotorType::MAXON, 0, static_cast<int>(motorTargetEncoder[4]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 1, static_cast<int>(motorTargetEncoder[5]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 2, static_cast<int>(motorTargetEncoder[7]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 3, static_cast<int>(motorTargetEncoder[8]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, static_cast<int>(motorTargetEncoder[9]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, static_cast<int>(motorTargetEncoder[10]));

//        LOG(INFO) << "Moons: " << std::dec << motorTargetEncoder[0];
//        LOG(INFO) << "Zero 0: " << std::dec << motorTargetEncoder[1];
//        LOG(INFO) << "Zero 1: " << std::dec << motorTargetEncoder[2];
//        LOG(INFO) << "Zero 2: " << std::dec << motorTargetEncoder[3];
//        LOG(INFO) << "Zero 3: " << std::dec << motorTargetEncoder[4];

        }

     }
    if(curRobotControlMode == RobotControlMode::Collabration)
    {
//        m_motorDriver->setTargetPos(MotorType::MAXON, joint_A, static_cast<int>(motorTargetEncoder[0]));
//        m_motorDriver->setTargetPos(MotorType::MOONS, 0, static_cast<int>(motorTargetEncoder[0]));
//        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 0, static_cast<int>(motorTargetEncoder[1]));
//        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 1, static_cast<int>(motorTargetEncoder[2]));
//        m_motorDriver->setTargetPos(MotorType::ZERO_ERR, 2, static_cast<int>(motorTargetEncoder[3]));
    }

    if(curRobotControlMode == RobotControlMode::TestOperation)
    {
        m_motorDriver->setTargetVel(MotorType::MOONS, 0, static_cast<int>(motorTargetVel[0]));
        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, static_cast<int>(motorTargetVel[1]));
        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, static_cast<int>(motorTargetVel[2]));
        m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, static_cast<int>(motorTargetVel[3]));

        if(m_controlLoopNum >= m_waitTime){
        m_motorDriver->setTargetPos(MotorType::MAXON, 0, static_cast<int>(motorTargetEncoder[4]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 1, static_cast<int>(motorTargetEncoder[5]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 2, static_cast<int>(motorTargetEncoder[7]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 3, static_cast<int>(motorTargetEncoder[8]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 4, static_cast<int>(motorTargetEncoder[9]));
        m_motorDriver->setTargetPos(MotorType::MAXON, 5, static_cast<int>(motorTargetEncoder[10]));
        }
//        LOG(INFO)<<"velocity: "<<motorTargetVel;
    }
}

void RobotControl::openTorqueSensor()
{
    m_Torque_Sensor_Serial_422=new QSerialPort();
    QString name="/dev/ttyUSB6";
    m_Torque_Sensor_Serial_422->setPortName(name);
    m_Torque_Sensor_Serial_422->setBaudRate(921600);
    m_Torque_Sensor_Serial_422->setDataBits(QSerialPort::Data8);
    m_Torque_Sensor_Serial_422->setParity(QSerialPort::NoParity);
    m_Torque_Sensor_Serial_422->setStopBits(QSerialPort::OneStop);
    m_Torque_Sensor_Serial_422->setFlowControl(QSerialPort::NoFlowControl);
    if (m_Torque_Sensor_Serial_422->open(QIODevice::ReadWrite)) {
        LOG(INFO)<<"Torque_Sensor_422 open successful";
    } else {
        LOG(ERROR)<<"Torque_Sensor_422 open fail";
    }
    connect(m_Torque_Sensor_Serial_422, &QSerialPort::readyRead, this, &RobotControl::onTorqueSensorDataIn);
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

                        qDebug()<<"CRC OK";
                        uint8_t errorcode=datatemp.at(2);
                        // if(errorcode==0){
                        //         TorqueSensorData

                        // }


                        // qDebug()<<"Crctemp="<<Crctemp;
                        // qDebug()<<"crclow="<<crclow;
                        // qDebug()<<"crchigh="<<crchigh;
                    }

                    //readHandleOtherData(datatemp);
                }
                m_Data_Torque_Sensor_Serial_Receved.remove(0,86);
                len=this->m_Data_Torque_Sensor_Serial_Receved.length();
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
    converter.byte[0]=data.at(startindex);
    converter.byte[1]=data.at(startindex+1);
    converter.byte[2]=data.at(startindex+2);
    converter.byte[3]=data.at(startindex+3);
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
        case static_cast<int>(RobotControlMode::Collabration):
        {
            goToCollabration();
            break;
        }
        case static_cast<int>(RobotControlMode::Hold):
        {
            goToHold();
            break;
        }
        case static_cast<int>(RobotControlMode::TestOperation)://控制为csv模式
        {
            goToTestOperation();
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
            m_flagInTeleoperation.store(false);//退出远程
            m_flagCommunicateWithMotorDriver.store(true);//暂停与电机驱动器的通信
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            LOG(INFO) << "SWITCH TO HOLD ON MODE, current Statis is: IN INIT STATUS";

              m_motorDriver->operationCSV(MotorType::MOONS, 0);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0);//周期同步速度模式
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2);
//              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 3);
              m_motorDriver->operationCSV(MotorType::MAXON, 0);
              m_motorDriver->operationCSV(MotorType::MAXON, 1);
              m_motorDriver->operationCSV(MotorType::MAXON, 2);
              m_motorDriver->operationCSV(MotorType::MAXON, 3);
              m_motorDriver->operationCSV(MotorType::MAXON, 4);
              m_motorDriver->operationCSV(MotorType::MAXON, 5);


            std::array<int, MotorNum> errCode = {0};
              errCode[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0);
              errCode[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0);
              errCode[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1);
              errCode[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2);
//              errCode[4] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 3);
              errCode[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0);
              errCode[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1);
              errCode[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2);
              errCode[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3);
              errCode[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4);
              errCode[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5);

            LOG(INFO) << "Set Motor Status Finish, Error Code: " << std::hex <<errCode;

            std::array<int, MotorNum> statusWord = {0};
              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//              statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
              statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
              statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
              statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
              statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
              statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
              statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);

            LOG(INFO) << "Set Motor Status Finish, Status Word: " << std::hex <<statusWord;

            std::array<int, MotorNum> modeDisplay = {0};
              modeDisplay[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0);
              modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
              modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
              modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
//              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 3);
              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0);
              modeDisplay[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1);
              modeDisplay[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2);
              modeDisplay[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3);
              modeDisplay[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4);
              modeDisplay[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5);
            LOG(INFO) << "Set Motor Data for Hold Finish, Operation Display: " << modeDisplay;
            break;
        }
        case static_cast<int>(RobotControlMode::TeleOperation)://通信模式
        {

            m_flagInTeleoperation.store(false);
            m_flagCommunicateWithMotorDriver.store(true);
            m_controlLoopNum = 0;//出范围后，计数为0
            if(m_status_R == 1 || m_status_R == 2){
               m_HandlePoseLastLoop_R = m_HandlePose_Cur.load();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));//当前线程暂停 100 毫秒

              m_motorDriver->operationCSV(MotorType::MOONS, 0);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2);
//              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 3);
              m_motorDriver->operationCSV(MotorType::MAXON, 0);
              m_motorDriver->operationCSV(MotorType::MAXON, 1);
              m_motorDriver->operationCSV(MotorType::MAXON, 2);
              m_motorDriver->operationCSV(MotorType::MAXON, 3);
              m_motorDriver->operationCSV(MotorType::MAXON, 4);
              m_motorDriver->operationCSV(MotorType::MAXON, 5);

            std::array<int, MotorNum> statusWord = {0};
              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//              statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
              statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
              statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
              statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
              statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
              statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
              statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);
            LOG(INFO) << "Set Motor Status Finish, Status Word: " << std::hex << statusWord;

            std::array<int, MotorNum> modeDisplay = {0};
              modeDisplay[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0);
              modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
              modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
              modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
//              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 3);
              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0);
              modeDisplay[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1);
              modeDisplay[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2);
              modeDisplay[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3);
              modeDisplay[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4);
              modeDisplay[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5);
            LOG(INFO) << "Set Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay;

            LOG(INFO) << "SWITCH TO HOLD ON MODE, current Statis is: IN TELEOPERATION";
            break;

        }

        case static_cast<int>(RobotControlMode::TestOperation)://通信模式
        {
            m_flagInTeleoperation.store(false);
            m_flagCommunicateWithMotorDriver.store(true);
            m_controlLoopNum = 0;//出范围后，计数为0
            if(m_status_R == 1 || m_status_R == 2){
               m_HandlePoseLastLoop_R = m_HandlePose_Cur.load();
               m_status_R = 4;
               //m_last_roll=m_cur_roll;
               m_EnableTagPrev_R = keepDisabling;
               LOG(INFO)<<"save the LastLoop! ";
               LOG(INFO)<<"CSV 2 HOLD m_HandlePoseLastLoop_R: " << m_HandlePoseLastLoop_R.handlePoseR_OpenAngle;

            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));//当前线程暂停 100 毫秒

            m_motorDriver->operationCSV(MotorType::MOONS, 0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2);
//        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 3);
            m_motorDriver->operationCSV(MotorType::MAXON, 0);
            m_motorDriver->operationCSV(MotorType::MAXON, 1);
            m_motorDriver->operationCSV(MotorType::MAXON, 2);
            m_motorDriver->operationCSV(MotorType::MAXON, 3);
            m_motorDriver->operationCSV(MotorType::MAXON, 4);
            m_motorDriver->operationCSV(MotorType::MAXON, 5);
            break;

        }
        case static_cast<int>(RobotControlMode::Collabration):
        {
            m_flagInCollabration.store(false);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1);
            m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2);

            std::array<int, MotorNum> statusWord = {0};
            statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
            statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
            statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
            LOG(INFO) << "InitMotor Finish, Status Word: " << statusWord;
            LOG(INFO) << statusWord;

            std::array<int, MotorNum> modeDisplay = {0};
            modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
            modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
            modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
            LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: ";
            LOG(INFO) << modeDisplay;

            LOG(INFO) << "SWITCH TO HOLD ON MODE, current Statis is: IN COLLABRATION";
            break;
        }
        case static_cast<int>(RobotControlMode::Hold):
        {
            LOG(INFO) << "SWITCH TO HOLD MODE, current Statis is: IN HOLD STAUTS";
            break;
        }
    }

    m_curRobotControlMode.store(RobotControlMode::Hold);//gotohold
    m_flagInHold.store(true);

}

void RobotControl::goToCollabration()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
        case static_cast<int>(RobotControlMode::TeleOperation):
        {
            LOG(INFO) << "SWITCH TO COLLABRATION MODE, current Statis is: IN TELEOPERATION";
            break;
        }
        case static_cast<int>(RobotControlMode::Collabration):
        {
            LOG(INFO) << "SWITCH TO COLLABRATION MODE, current Statis is: IN COLLABRATION";
            break;
        }
        case static_cast<int>(RobotControlMode::Hold):
        {

              m_motorDriver->operationCST(MotorType::ZERO_ERR, 0);
//            m_motorDriver->operationCST(MotorType::ZERO_ERR, 1);
//            m_motorDriver->operationCST(MotorType::ZERO_ERR, 2);

            std::array<int, MotorNum> statusWord = {0};
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
//            statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
//            statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
            LOG(INFO) << "InitMotor Finish, Status Word: " << statusWord;
            LOG(INFO) << statusWord;

            std::array<int, MotorNum> modeDisplay = {0};
              modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
//            modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
//            modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
            LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: ";
            LOG(INFO) << modeDisplay;

            LOG(INFO) << "SWITCH TO COLLABRATION MODE, current Statis is: IN HOLD STAUTS";
            break;
        }
    }
    m_curRobotControlMode.store(RobotControlMode::Collabration);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_flagInCollabration.store(true);
}

void RobotControl::goToTeleOperation()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
        case static_cast<int>(RobotControlMode::TeleOperation):
        {
            LOG(INFO) << "SWITCH TO TELEOPERATION MODE, current Statis is: IN TELEOPERATION";

        }
        case static_cast<int>(RobotControlMode::Collabration):
        {
            LOG(INFO) << "SWITCH TO TELEOPERATION MODE, current Statis is: IN COLLABRATION";

        }
        case static_cast<int>(RobotControlMode::Hold):
        {

            LOG(INFO) << "SWITCH TO TELEOPERATION MODE, current Statis is: IN HOLD STAUTS";

            std::array<int, MotorNum> statusWord = {0};
              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//              statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
              statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
              statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
              statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
              statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
              statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
              statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);
              usleep (50 * 1000);

            LOG(INFO) << "InitMotor Finish, Status Word: " << std::hex << statusWord;

              m_motorDriver->operationCSP(MotorType::MOONS, 0);
              m_motorDriver->operationCSP(MotorType::ZERO_ERR, 0);
              m_motorDriver->operationCSP(MotorType::ZERO_ERR, 1);
              m_motorDriver->operationCSP(MotorType::ZERO_ERR, 2);
//              m_motorDriver->operationCSP(MotorType::ZERO_ERR, 3);
              m_motorDriver->operationCSP(MotorType::MAXON, 0);
              m_motorDriver->operationCSP(MotorType::MAXON, 1);
              m_motorDriver->operationCSP(MotorType::MAXON, 2);
              m_motorDriver->operationCSP(MotorType::MAXON, 3);
              m_motorDriver->operationCSP(MotorType::MAXON, 4);
              m_motorDriver->operationCSP(MotorType::MAXON, 5);

            std::array<int, MotorNum> modeDisplay = {0};
              modeDisplay[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0);
              modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
              modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
              modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
//              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 3);
              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0);
              modeDisplay[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1);
              modeDisplay[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2);
              modeDisplay[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3);
              modeDisplay[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4);
              modeDisplay[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5);

            LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: " << std::hex << modeDisplay;

              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//              statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
              statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
              statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
              statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
              statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
              statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
              statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);
            LOG(INFO) << "InitMotor Finish, Status Word: " << std::hex << statusWord;
        }
    }

    m_curRobotControlMode.store(RobotControlMode::TeleOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);

    m_flagInHold.store(false);

    m_flagCommunicateWithMotorDriver.store(true);
}


void RobotControl::goToTestOperation()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
        case static_cast<int>(RobotControlMode::TeleOperation):
        {
            LOG(INFO) << "SWITCH TO TEST MODE, current Statis is: IN TELEOPERATION";

        }
        case static_cast<int>(RobotControlMode::Collabration):
        {
            LOG(INFO) << "SWITCH TO TEST MODE, current Statis is: IN COLLABRATION";

        }
        case static_cast<int>(RobotControlMode::Hold):
        {

            LOG(INFO) << "SWITCH TO TEST MODE, current Statis is: IN HOLD STAUTS";

            std::array<int, MotorNum> statusWord = {0};
              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//            statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
            statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
            statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
            statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
            statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
            statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
            statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);

            LOG(INFO) << "InitMotor Finish, Status Word: " << std::hex << statusWord;

              m_motorDriver->operationCSV(MotorType::MOONS, 0);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1);
              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2);
//              m_motorDriver->operationCSV(MotorType::ZERO_ERR, 3);
            m_motorDriver->operationCSP(MotorType::MAXON, 0);
            m_motorDriver->operationCSP(MotorType::MAXON, 1);
            m_motorDriver->operationCSP(MotorType::MAXON, 2);
            m_motorDriver->operationCSP(MotorType::MAXON, 3);
            m_motorDriver->operationCSP(MotorType::MAXON, 4);
            m_motorDriver->operationCSP(MotorType::MAXON, 5);

            std::array<int, MotorNum> modeDisplay = {0};
              modeDisplay[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0);
              modeDisplay[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0);
              modeDisplay[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1);
              modeDisplay[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2);
//              modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 3);
            modeDisplay[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0);
            modeDisplay[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1);
            modeDisplay[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2);
            modeDisplay[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3);
            modeDisplay[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4);
            modeDisplay[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5);

            LOG(INFO) << "Set Motor Data for Init Status Finish, Operation Display: " << std::hex << modeDisplay;

              statusWord[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0);
              statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0);
              statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1);
              statusWord[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2);
//              statusWord[4] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 3);
            statusWord[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0);
            statusWord[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1);
            statusWord[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2);
            statusWord[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3);
            statusWord[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4);
            statusWord[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5);

            LOG(INFO) << "InitMotor Finish, Status Word: " << std::hex << statusWord;
        }
    }

    m_curRobotControlMode.store(RobotControlMode::TestOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);

    m_flagInHold.store(false);

    m_flagCommunicateWithMotorDriver.store(true);
}

std::array<std::array<int,MotorNum>,3> RobotControl::positionControl(const HandlePose& masterHandlePose, const std::array<int, MotorNum>& motorPosition_Cur,  const int& controlLoopCount)
{

}

//std::array<std::array<int,MotorNum>,3> RobotControl::positionControl(const HandlePose& masterHandlePose, const std::array<int, MotorNum>& motorPosition_Cur,  const int& controlLoopCount)
//{
//    HandlePose      masterHandlePose_Init_L;
//    HandlePose      masterHandlePose_Init_R;
//    HandlePose      masterHandlePose_Prev;
//    HandlePose      masterHandlePose_Cur;

//    std::array<double, ControlValueNum>   controlValuePrev_L = {0};
//    std::array<double, ControlValueNum>   controlValueCur_L = {0};
//    std::array<double, ControlValueNum>   controlValuePrev_R = {0};
//    std::array<double, ControlValueNum>   controlValueCur_R = {0};
//    std::array<int, MotorNum>   motorPositionInit_L = {0};
//    std::array<int, MotorNum>   motorPositionCur_L = {0};
//    std::array<int, MotorNum>   motorPositionInit_R = {0};
//    std::array<int, MotorNum>   motorPositionCur_R = {0};
//    std::array<std::array<int,MotorNum>,3>    targetSpeedPosition={{0}};

//    masterHandlePose_Init_L = m_HandlePoseInit_L;
//    masterHandlePose_Prev = m_HandlePosePrev_R;
//    masterHandlePose_Cur = masterHandlePose;
//    motorPositionInit_L = m_MotorPositionInit_L;
//    motorPositionCur_L = motorPosition_Cur;
//    int enableTagCur_L = keepDisabling;
//    int enableTagCur_R = keepDisabling;

//    controlValuePrev_L = m_ControlValuePrev_L;

//    // enableTagCur_L = enableCase_KeepPressPedal(masterHandlePose_Cur, 'l');
//    // enableTagCur_R = enableCase_KeepPressPedal(masterHandlePose_Cur, 'r');


//    if(controlLoopCount == 1000)
//    {
//        enableTagCur_L = enableAction;
//    }
//    else if(controlLoopCount > 1000)
//    {
//        enableTagCur_L = keepEnabling;
//    }
//    else
//    {
//        enableTagCur_L = disableAction;
//    }

//    //Control loop
//    {
//        if(enableTagCur_L == enableAction||enableTagCur_L == keepEnabling)
//        {
//            if(enableTagCur_L == enableAction)
//            {
//                masterHandlePose_Init_L = masterHandlePose_Cur;
//                motorPositionInit_L = motorPositionCur_L;
//                setControlInitHandleMotorPositionAndPose(motorPositionCur_L, masterHandlePose_Cur, 'l');
//            }
//            controlValueCur_L = motionMappingL(masterHandlePose_Init_L, masterHandlePose_Prev, masterHandlePose_Cur);
//            LOG(INFO) << "controlValueCur_L: " << controlValueCur_L;
//            targetSpeedPosition = calTargetPosition(controlValuePrev_L, controlValueCur_L, motorPositionInit_L, motorPositionCur_L,masterHandlePose_Cur, 'l');
//        }
//    }
//    storeCurAsPrev(masterHandlePose_Cur, controlValueCur_L, controlValueCur_R, motorPositionCur_L, motorPositionCur_R, enableTagCur_L, enableTagCur_R);
//    return targetSpeedPosition;
//}

int RobotControl::enableCase_KeepPressPedal(const HandlePose& masterHandlePose_Cur, const char side)//yu 记录离开使能时的位置，以及回到使能时m_AlignmentNumber_R=0
{
    int enableFlag = 4;//4为维持解除状态

    if(side == 'r')
    {
        if(!isPoseRight(masterHandlePose_Cur, 'r')){//cur处于非使能状态
            if(m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling)//pre维持非使能
            {
                enableFlag = keepDisabling;//4
            }
//            if(m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling)//pre处于使能状态，此时进入disable
            if(m_EnableTagPrev_R == keepEnabling)
            {
                m_HandlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;//3
                //m_last_roll=m_cur_roll;
            }
        }
        else if(isPoseRight(masterHandlePose_Cur, 'r'))//cur处于使能状态
        {
            if((m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling)
                && masterHandlePose_Cur.enableButton_R == buttonPress && isPoseMatch(masterHandlePose_Cur,'r') == true)
            {
               m_HandlePoseInit_R = masterHandlePose_Cur;//记录主手初始状态
               setControlInitHandleMotorPositionAndPose(m_MotorCurEncoder.load(), m_HandlePoseInit_R, 'r');//记录机械臂初始状态
               m_AlignmentNumber_R = 0;//进入使能状态，重新检查归0
               LOG(INFO) << "=====================================================================================================================================";

               enableFlag = enableAction; //1
            }
            // else if ((m_EnableTagPrev_R == disableAction || m_EnableTagPrev_R == keepDisabling))
            //          //&&((masterHandlePose_Cur.enableButton_R == buttonPress && isPoseMatch(masterHandlePose_Cur,'l') == false)||(masterHandlePose_Cur.enableButton_L == buttonLoose)))
            // {
            //    enableFlag = keepDisabling;
            // }//此处为钳子位置不匹配的时，依然不更新

            else if ((m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enableButton_R == buttonLoose)//此时使能踏板没有踩下，enableButton_L=7为松开
            {
                m_HandlePoseLastLoop_R = masterHandlePose_Cur;
                enableFlag = disableAction;//3
            }
            else if ((m_EnableTagPrev_R == enableAction || m_EnableTagPrev_R == keepEnabling) &&
                     masterHandlePose_Cur.enableButton_R == buttonPress)//此时为正常运行（pre和cur都在使能，且踏板踩下）
            {
                enableFlag = keepEnabling;//2
            }
        }
    }


//    }
//    else{
//        m_HandlePoseLastLoop_R = masterHandlePose_Cur;
//        enableFlag = disableAction;//3
//    }


    m_EnableTagPrev_R = enableFlag;

    return enableFlag;
}



// Check if the master device is in appropriate work space;
bool RobotControl::isPoseRight(const HandlePose& masterHandlePose_Cur, const char side) const//yu 且要满足flag_openangle等于true时（false为status=3时openangle太小）
{
    if(side =='l'){
        if (masterHandlePose_Cur.handlePoseL_Elevation > 67 || masterHandlePose_Cur.handlePoseL_Elevation < -67){
           return false;
        }
        else if (masterHandlePose_Cur.handlePoseL_Arzimuth > 88 || masterHandlePose_Cur.handlePoseL_Arzimuth < -88){
            return false;
        }
        else{
            return true;
        }
    }
    if(side =='r'){
     // std::cout << masterHandlePose_Cur.handlePoseR_Elevation <<std::endl;

//        if (masterHandlePose_Cur.handlePoseR_Elevation > 70 || masterHandlePose_Cur.handlePoseR_Elevation < -70){
////        if (masterHandlePose_Cur.handlePoseR_Elevation > 180 || masterHandlePose_Cur.handlePoseR_Elevation < -180){
//           return false;
//        }
////        else if (masterHandlePose_Cur.handlePoseR_Arzimuth > 180 || masterHandlePose_Cur.handlePoseR_Arzimuth < -180){
//        else if (masterHandlePose_Cur.handlePoseR_Arzimuth > 120 || masterHandlePose_Cur.handlePoseR_Arzimuth < -120){
//            return false;
//        }
//        else{
//            return true;
//        }

                if((masterHandlePose_Cur.handlePoseR_X < -50) || (masterHandlePose_Cur.handlePoseR_X > 50)){
                    return false;
                }
                else if((masterHandlePose_Cur.handlePoseR_Y < -50) || (masterHandlePose_Cur.handlePoseR_Y > 50)){
                    return false;
                }
                else if((masterHandlePose_Cur.handlePoseR_Z < -50) || (masterHandlePose_Cur.handlePoseR_Z > 10)){
                    return false;
                }
                else if (m_yawAngle > 88 || m_yawAngle < -88){
                     return false;
                }
                else if (m_pitchAngle > 70 || m_pitchAngle < -70){
                     return false;
                }
                else{
                    return true;
                }
     }
//    LOG(INFO)<<"X: "<<masterHandlePose_Cur.handlePoseR_X;
//    LOG(INFO)<<"Y: "<<masterHandlePose_Cur.handlePoseR_Y;
//    LOG(INFO)<<"Z: "<<masterHandlePose_Cur.handlePoseR_Z;

 }

bool RobotControl::isPoseMatch(const HandlePose& masterHandlePose_Cur, const char side) const//yu
{
    int delta = 0;
    if (side == 'l')
    {
        if (abs(masterHandlePose_Cur.handlePoseL_OpenAngle - m_HandlePoseLastLoop_L.handlePoseL_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}

    }
    if (side == 'r')
    {
        if (abs(masterHandlePose_Cur.handlePoseR_OpenAngle - m_HandlePoseLastLoop_R.handlePoseR_OpenAngle) < 1)
        {
            return true;
        }
        else {return false;}
    }
//    return true;

}

void RobotControl::setControlInitHandleMotorPositionAndPose(const std::array<int, MotorNum>& motorPositionCur, const HandlePose& handlePoseCur, const char& side)//yu
{

    if(side == 'l')
    {
        m_HandlePoseInit_L = handlePoseCur;
        m_MotorPositionInit_L = motorPositionCur;
    }
    if(side == 'r')
    {
        m_HandlePoseInit_R = handlePoseCur;
        m_MotorPositionInit_R = motorPositionCur;//需要把右边电机值存储进去
        m_endEffectorInitPos_R = calculateEndEffectorPosition(handlePoseCur, motorPositionCur,'r');
        m_endEffectorInitPos_R[0] = m_HandlePoseInit_R.handlePoseR_X;

        m_ruckigInputState.current_position = {m_endEffectorInitPos_R[0], m_endEffectorInitPos_R[1], m_endEffectorInitPos_R[2]};//m_endEffectorInitPos为初始化时编码器值 xyz
        m_ruckigInputState.current_velocity = {0.0, 0.0, 0.0};
        m_ruckigInputState.current_acceleration = {0.0, 0.0, 0.0};
    }
}

void RobotControl::endJointGoHome(const char& side)//yu
{
    if(side == 'r')
    {
            std::thread calibration([this](){
                m_motorDriver->operationCSV(MotorType::MOONS, 0);         
                m_motorDriver->gotoTargetPos_PPMode(MotorType::ZERO_ERR, 0, 2550, 389936);//initPosition  触发shutdown switchon enable 指令
                m_motorDriver->gotoTargetPos_PPMode(MotorType::ZERO_ERR, 1, 2550, 197072);
                m_motorDriver->gotoTargetPos_PPMode(MotorType::ZERO_ERR, 2, 2550, 382345);

                usleep(50 * 1000);
                m_motorDriver->setTargetPos(MotorType::MOONS, 0, 120000);//回复到的相对位置
                auto homeStatusTmp = m_MotorHomingStatus.load();

                if(homeStatusTmp[0] == true){
                    m_motorDriver->setTargetVel(MotorType::MOONS, 0, -20000);//-30000
                }
                usleep(150 * 1000);

                m_motorDriver->setControlWord(MotorType::ZERO_ERR, 0, ControlCommand::MOTION_START_PP);//MotorType,index,Command 控制命令位上升沿触发 bit4从off转换到on
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, 1, ControlCommand::MOTION_START_PP);
                m_motorDriver->setControlWord(MotorType::ZERO_ERR, 2, ControlCommand::MOTION_START_PP);

                while(true)
                {
                    auto homeStatusTmp = m_MotorHomingStatus.load();
                    if(homeStatusTmp[0] != true){ //如果等于0
                        m_motorDriver->setTargetVel(MotorType::MOONS, 0, 0);
                    }
                    if(homeStatusTmp[0] != true){
                        LOG(INFO) << "Found Edge of Gimbal Motor!";//所有电机归位跳出此循环
                        break;
                    }
                    usleep(5 * 1000);
                }

                LOG(INFO)<<"goToTarget position";
                m_motorDriver->operationPP(MotorType::MOONS, 0);//存在setOperationMode将模式改为PP 并发送targetposition作为相对位置

                usleep(50 * 1000);

                while(true)
                {
                    auto statusWordTmp = m_MotorCurStatusWord.load();//当状态字(0x6041)的第10位=1的时候说明状态已经切换，即目标位置到达
                    std::bitset<32> statusWordInBinary_moons_1(statusWordTmp[0]);//右边Moons电机的状态字
                    std::bitset<32> statusWordInBinary_zero_1(statusWordTmp[1]);
                    std::bitset<32> statusWordInBinary_zero_2(statusWordTmp[2]);
                    std::bitset<32> statusWordInBinary_zero_3(statusWordTmp[3]);
//                    if(statusWordInBinary_jointE[10] == 1 && statusWordInBinary_jointF[10] == 1 && statusWordInBinary_jointG[10] == 1)
//                    {
                    if(statusWordInBinary_moons_1[10] == 1 && statusWordInBinary_zero_1[10] == 1 && statusWordInBinary_zero_2[10] == 1 && statusWordInBinary_zero_3[10] == 1){//如果状态字第10项为0,就完成了init
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");//目的为与uiinterface通信，reset button
                        LOG(INFO) << "FinishCalibration";
                        break;
                    }
                    usleep(5 * 1000);
                }
            });
            calibration.detach();
    }
}

void RobotControl::MaxonGoHome(const char& side)//yu
{
    m_flagInHold.store(false);
    auto motor_cur = m_MotorCurEncoder.load();
    usleep(50);
    m_HandlePoseLastLoop_R.initOrg_R();
    Eigen::Matrix3d MasterRotationMatrix_x;
    Eigen::Matrix3d MasterRotationMatrix_y;
    Eigen::Matrix3d MasterRotationMatrix_z;
    Eigen::Matrix3d MasterRotationMatrix_total;
//绕x转-30度
    MasterRotationMatrix_x(0,0) = 1;
    MasterRotationMatrix_x(0,1) = 0;
    MasterRotationMatrix_x(0,2) = 0;
    MasterRotationMatrix_x(1,0) = 0;
    MasterRotationMatrix_x(1,1) = 0.886025;
    MasterRotationMatrix_x(1,2) = 0.5;
    MasterRotationMatrix_x(2,0) = 0;
    MasterRotationMatrix_x(2,1) = -0.5;
    MasterRotationMatrix_x(2,2) = 0.886025;
//handle初始状态为绕y轴转动-60度
    MasterRotationMatrix_y(0,0) = 0.5;
    MasterRotationMatrix_y(0,1) = 0;
    MasterRotationMatrix_y(0,2) = -0.886025;
    MasterRotationMatrix_y(1,0) = 0;
    MasterRotationMatrix_y(1,1) = 1;
    MasterRotationMatrix_y(1,2) = 0;
    MasterRotationMatrix_y(2,0) = 0.886025;
    MasterRotationMatrix_y(2,1) = 0;
    MasterRotationMatrix_y(2,2) = 0.5;
//绕z转-45
//    MasterRotationMatrix_z(0,0) = 0.707106;
//    MasterRotationMatrix_z(0,1) = 0.707106;
//    MasterRotationMatrix_z(0,2) = 0;
//    MasterRotationMatrix_z(1,0) = -0.707106;
//    MasterRotationMatrix_z(1,1) = 0.707106;
//    MasterRotationMatrix_z(1,2) = 0;
//    MasterRotationMatrix_z(2,0) = 0;
//    MasterRotationMatrix_z(2,1) = 0;
//    MasterRotationMatrix_z(2,2) = 1;
//    //绕z转-30
    MasterRotationMatrix_z(0,0) = 0.866025;//cos
    MasterRotationMatrix_z(0,1) = 0.5;//-sin
    MasterRotationMatrix_z(0,2) = 0;//0
    MasterRotationMatrix_z(1,0) = -0.5;//sin
    MasterRotationMatrix_z(1,1) = 0.866025;//cos
    MasterRotationMatrix_z(1,2) = 0;//0
    MasterRotationMatrix_z(2,0) = 0;//0
    MasterRotationMatrix_z(2,1) = 0;//0
    MasterRotationMatrix_z(2,2) = 1;//1

    MasterRotationMatrix_total = MasterRotationMatrix_y * MasterRotationMatrix_z * MasterRotationMatrix_x;
//   MasterRotationMatrix_total =MasterRotationMatrix_y;


    m_HandlePoseLastLoop_R.setRotationDataR(MasterRotationMatrix_total);
    m_HandlePoseOrg_R.setRotationDataR(MasterRotationMatrix_total);
//    m_HandlePoseLastLoop_R.handlePoseR_OpenAngle = 0;

    if(side == 'r')
    {
                 std::thread calibration([this](){
                    m_motorDriver->operationHOME(MotorType::MAXON, 0);
                    m_motorDriver->operationHOME(MotorType::MAXON, 1);
                    m_motorDriver->operationHOME(MotorType::MAXON, 2);
                    m_motorDriver->operationHOME(MotorType::MAXON, 3);
                    m_motorDriver->operationHOME(MotorType::MAXON, 4);
                    m_motorDriver->operationHOME(MotorType::MAXON, 5);
                    LOG(INFO) << "Start Maxon homing! ";
                    while(true)
                    {
                        auto homeStatusTmp = m_MotorHomingStatus.load();
                        if(homeStatusTmp[1] == false &&
                            homeStatusTmp[2] == false &&
                            homeStatusTmp[3] == false &&
                            homeStatusTmp[4] == false &&
                            homeStatusTmp[5] == false &&
                            homeStatusTmp[6] == false)
                        {
                            LOG(INFO) << "Finish homing of endeffector motor!";
                            SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");
                            break;
                        }
                        usleep(20 * 1000);
                    }



//                    m_motorDriver->operationCSV(MotorType::MAXON, 2);
//                    m_motorDriver->operationCSV(MotorType::MAXON, 5);
//                    usleep(20 * 1000);
//                    m_motorDriver->setTargetVel(MotorType::MAXON, 2, -2000);
//                    m_motorDriver->setTargetVel(MotorType::MAXON, 5, -2000);

//                    while(true)
//                    {
//                        //读取扭矩传感器数值
//                        if(true){//如果任意一个力传感器大于校准目标值
//                            m_motorDriver->setTargetVel(MotorType::MAXON, 2, 0);
//                            m_motorDriver->setTargetVel(MotorType::MAXON, 5, 0);
//                        }
//                        if(true){
//                             LOG(INFO) << "Finish Maxon Motor calibration!";//所有电机归位跳出此循环
//                        break;
//                        }
//                        usleep(5 * 1000);
//                     }
//                    //记录此时的力大小
//                    auto Motorencode = m_MotorCurEncoder.load();
//                    m_maxonInit[0] = Motorencode[4];
//                    m_maxonInit[1] = Motorencode[5];
//                    m_maxonInit[2] = Motorencode[6];
//                    m_maxonInit[3] = Motorencode[7];
//                    m_maxonInit[4] = Motorencode[8];
//                    m_maxonInit[5] = Motorencode[9];



                });
                calibration.detach();
    }
}

void RobotControl::storeCurAsPrev(const HandlePose& handlePoseCur,
                                         const std::array<double, ControlValueNum> controlValueCur_L, const std::array<double, ControlValueNum> controlValueCur_R,
                                         const std::array<int, MotorNum>& motorPositionCur_L, const std::array<int, MotorNum>& motorPositionCur_R,
                                         const int&  enableTagCur_L, const int&  enableTagCur_R ) const
{
    m_HandlePosePrev_R = handlePoseCur;
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

double RobotControl::calculateOverlapValue(const std::array<int, MotorNum>& motorPosition_Cur,
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
//                    setRobotControlMode(RobotControlMode::TeleOperation);
                    setRobotControlMode(RobotControlMode::TestOperation);
                    test_x=0.0;
                    test_y=0.0;
                    test_z=0.0;
                    savetime=0;
                //    test_angle = 0;//画圆时每个循环增加的角度
                    test_index = 0;//重复测试时各条边的序号
                    test_time = 0;//重复测试时每个循环增加的长度
                    test_circle = 0;
                //    index = 0;
                    break;
                }
                case static_cast<int>(RobotControlAction_E::GoToCollabrationMode):
                {
                    LOG(INFO)<<"Get INFO Execuate Control Set in RobotControl: Go To Collabration";
                    setRobotControlMode(RobotControlMode::Collabration);
                    break;
                }
                case static_cast<int>(RobotControlAction_E::StartEndJointMotorHoming)://
                {
                    char side = i.value().toUtf8().data()[0];//将 QString 转换为 char
                    if(side == 'l')
                    {
                        LOG(INFO)<<"Get INFO start Homing command in RobotControl: Left End Joint Motor Homing!";
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        endJointGoHome('l');
                        LOG(INFO)<<"Left End Joint finish Homing !";
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"l");//接收者为uiinterface，操作为finischCalibration，l边
                    }
                    if(side == 'r')
                    {
                        LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right End Joint Motor Homing!";
                        endJointGoHome('r');
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");
                    }
                    break;
                }

            case static_cast<int>(RobotControlAction_E::StartEndEffectorMotorHoming):{
                char side = i.value().toUtf8().data()[0];
                if(side == 'r')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right Instrument Homing!";
//                    InstrumentGoHome('r');
                    MaxonGoHome('r');
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");
                }
                break;
            }


                default:break;
            }
            i++;
        }
    }
}
