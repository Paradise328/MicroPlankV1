#include "RobotControl.h"
#include <algorithm>
#include<chrono>
#include <cmath>
#include <thread>
#include <random>
#include <string>
#include <sys/stat.h> // 用于 mkdir
#include <sys/types.h>
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTime;
std::chrono::duration<double, std::micro>   loopTime;
// std::ofstream outfile1("data_0721.txt");

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
    initCamera();
}

bool RobotControl::loadEndeffectorConfig(int axes)
{
    if (axes != 4 && axes != 6) { return false; }
    try
    {
        const auto data = toml::parse_file(m_configFilePath);
        const auto config = data["Instrument"]["CZQ"][axes == 4 ? "4MM" : "3MM"]["1"];
        const auto* values = config["EncoderPerDegree"]["Value"].as_array();
        if (!values || values->size() != static_cast<size_t>(axes)) {
            LOG(ERROR) << "EncoderPerDegree count does not match selected axes: " << axes;
            return false;
        }
        std::array<double, 6> encoders{};
        for (int i = 0; i < axes; ++i) {
            const auto value = (*values)[i].value<double>();
            if (!value || !std::isfinite(*value) || *value <= 0.0) { return false; }
            encoders[i] = *value;
        }
        // The existing 3MM profile has no compensation entry; retain its original default.
        const double compensation = config["CompensationRatio"]["Value"].value_or(0.765);
        if (!std::isfinite(compensation) || compensation <= 0.0) { return false; }
        // Commit only after the complete profile has been validated.
        if (axes == 4) {
            std::copy_n(encoders.begin(), 4, m_encoderPerDegree_R.begin());
        } else {
            m_encoderPerDegree_6maxon_R = encoders;
        }
        m_compRatio_R = compensation;
        LOG(INFO) << "Loaded instrument profile, Maxon axes: " << axes;
        return true;
    }
    catch (const toml::parse_error& err)
    {
        LOG(ERROR) << "Instrument configuration parse failed: " << err;
        return false;
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
    m_isLooping=false;


    if (m_pressureSensor == nullptr) {
        m_pressureSensor = new PressureSensor();
        // 初始化并连接 (这里传什么波特率都不重要了，因为底层已经锁死9600)
        bool ret = m_pressureSensor->initDevice("/dev/ttyUSB0", 9600);

        if(ret) {
            m_pressureSensor->setScaleFactor(100.0f);
            LOG(INFO) << "压力传感器初始化成功";
        } else {
            LOG(ERROR) << "压力传感器初始化失败 (可能是端口被占用)";
        }
    } else {
        LOG(INFO) << "压力传感器已存在，跳过重新初始化 (保持连接)";
    }

    Eigen::Matrix3d MasterRotationMatrix;

    m_controlValuePrev_L = {0};
    m_controlValuePrev_R = {0};

}

bool RobotControl::initTorqueSensorArray()
{
    if (m_torqueSensorArray.isConnected())
        return true;

    TorqueSensorArray::Config config;
    config.portName = m_torqueSensorPort;
    config.baudRate = 115200;
    config.parity = 'N';
    config.dataBits = 8;
    config.stopBits = 1;
    config.slaveId = 1;
    config.pollPeriodMs = 10;
    config.responseTimeoutMs = 50;
    config.filterCutoffHz = 20.0;
    config.staleTimeoutMs = 200;

    if (!m_torqueSensorArray.initDevice(config)) {
        LOG(WARNING) << "Torque sensor array is not connected on "
                     << m_torqueSensorPort;
        return false;
    }

    if (!m_torqueSensorArray.start()) {
        LOG(ERROR) << "Failed to start torque sensor polling";
        m_torqueSensorArray.disconnectDevice();
        return false;
    }

    LOG(INFO) << "Torque sensor array started; channels 1-4 map to Maxon 0-3";
    return true;
}

void RobotControl::initCamera()
{
    if(!m_camera.isOpened())
    {
        m_camera.open(0); // 打开默认摄像头 /dev/video0
        if(m_camera.isOpened())
        {
            // 设置分辨率 (640x480 读取速度较快，推荐)
            m_camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
            m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
            m_isTakingPhoto.store(false);
            LOG(INFO) << "Camera Initialized Successfully!";
        }
        else
        {
            LOG(ERROR) << "Failed to open Camera!";
        }
    }
}

// 2. 后台拍照任务 (这个函数会在单独线程跑，不仅不卡，还可以清空缓存)
void RobotControl::takePhotoTask(std::string stepName, double angle)
{
    if(!m_camera.isOpened())
    {
        m_isTakingPhoto.store(false);
        return;
    }

    cv::Mat frame;
    // 连续读几次清空缓存，确保拍到的是最新的
    m_camera.read(frame);
    m_camera.read(frame);

    if(m_camera.read(frame))
    {
        // 1. 确定基础路径
        std::string basePath = "/home/a/Desktop/TestPhotos/";

        // 2. 根据角度决定子文件夹
        // 使用 abs() 取绝对值，防止负角度（如 -10度）判断错误
        std::string subFolder;
        if (std::abs(angle) > 9.999) { // 加一点点容差，避免浮点数精度问题
            subFolder = "Angle_Over_20/";   // 大于30度
        } else {
            subFolder = "Angle_Under_20/";  // 小于等于30度
        }

        std::string fullSaveDir = basePath + subFolder;

        // // 3. 自动创建文件夹 (Linux 命令)
        // // mkdir -p 可以确保父目录不存在时自动创建，且文件夹已存在时不报错
        // std::string cmd = "mkdir -p " + fullSaveDir;
        // system(cmd.c_str());

        // 4. 生成带时间戳的文件名
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        // 路径拼起来： /home/a/Desktop/TestPhotos/Angle_Under_30/StepX_123456.jpg
        ss << fullSaveDir << stepName << "_" << now_c << ".jpg";

        // 5. 保存
        cv::imwrite(ss.str(), frame);
        LOG(INFO) << "Photo Saved to: " << ss.str() << " (Angle: " << angle << ")";
    }
    else
    {
        LOG(WARNING) << "Camera read frame failed.";
    }

    m_isTakingPhoto.store(false);
}

// 3. 触发接口
void RobotControl::triggerPhoto(std::string stepName, double angle)
{
    bool expected = false;
    if(m_isTakingPhoto.compare_exchange_strong(expected, true))
    {
        LOG(INFO) << "Triggering Photo for " << stepName << " at angle: " << angle;
        // 将 angle 传递给后台线程
        std::thread(&RobotControl::takePhotoTask, this, stepName, angle).detach();
    }
}

void RobotControl::startMyThreads()
{
    // Start sensor I/O only after RobotControl has been fully constructed.
    // Failure is non-fatal while the torque hardware is not installed.
    initTorqueSensorArray();

    goToHold();

    std::this_thread::sleep_for(std::chrono::milliseconds(4));

    m_flagControlThread.store(true);

    startControlThread();

    SendInnerMsg(Module_Inner_E::Uiinterface,
        static_cast<int>(UIAction_E::InstrumentTestStatus), "initialized");

}

void RobotControl::startControlThread()
{
    m_calculateControlDataThread = std::thread(&RobotControl::control, this);
    m_calculateControlDataThread.detach();
}


void RobotControl::control()
{
    LOG(INFO) << "Control Thread Started (High Speed).";
    m_flagCommThread = true;
    auto lastForcePublish = std::chrono::steady_clock::now();
    const auto publishElapsed = [this]() {
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - m_motionStartTime).count();
        if (seconds != m_lastTestElapsedSeconds) {
            m_lastTestElapsedSeconds = seconds;
            SendInnerMsg(Module_Inner_E::Uiinterface,
                static_cast<int>(UIAction_E::InstrumentTestElapsed), QString::number(static_cast<qlonglong>(seconds)));
        }
    };

    // while (m_flagControlThread && !m_isSystemTerminated)
    while (1)
    {
        // LOG(INFO)<<"TEL----------------------------";
        const int requestedAxes = m_pendingInstrumentAxes.exchange(0);
        if (requestedAxes == 4 || requestedAxes == 6) {
            m_rightTestHomed.store(false);
            if (loadEndeffectorConfig(requestedAxes)) {
                const auto configuration = requestedAxes == 4
                    ? EndeffectorConfiguration::fourMaxons : EndeffectorConfiguration::sixMaxons;
                m_endeffectorConfiguration_R = configuration;
                m_endeffectorConfiguration_L = configuration;
                m_endeffectorConfiguration = configuration;
                for (auto* compensation : {&m_instrument_Left1, &m_instrument_Left2,
                        &m_instrument_Left3, &m_instrument_Left4, &m_instrument_Right1,
                        &m_instrument_Right2, &m_instrument_Right3, &m_instrument_Right4}) {
                    *compensation = instrumentCompensate{};
                }
                m_instrumentAxes.store(requestedAxes);
                for (int axis = instrumentFirstMaxon(requestedAxes); axis < 6; ++axis) {
                    m_motorDriver->operationCSP(MotorType::MAXON, axis, arm_0);
                }
                m_testBusy.store(false);
                SendInnerMsg(Module_Inner_E::Uiinterface,
                    static_cast<int>(UIAction_E::InstrumentAxesStatus), QString::number(requestedAxes));
            } else {
                m_instrumentAxes.store(0);
                m_testBusy.store(false);
                SendInnerMsg(Module_Inner_E::Uiinterface,
                    static_cast<int>(UIAction_E::InstrumentAxesStatus), "failed");
            }
        }
        receiveMotorData();
        // auto start = std::chrono::high_resolution_clock::now();

        const int requestedMode = m_pendingTestMode.exchange(-1);
        if (requestedMode == -2) { // Stop is consumed by the same thread that sends motion commands.
            if (m_isLooping) { publishElapsed(); }
            m_isLooping = false;
            // Replace the previous motion target with the latest measured positions.
            m_motorTargetEncoderLast_R = m_motorEncoderCur_R.load();
            sendMotorData(m_motorTargetEncoderLast_R);
            goToHold();
            m_rightTestHomed.store(false);
            m_testBusy.store(false);
            SendInnerMsg(Module_Inner_E::Uiinterface,
                static_cast<int>(UIAction_E::InstrumentTestStatus), "stopped");
        } else if (isInstrumentTestMode(requestedMode)) {
            m_testMode = static_cast<InstrumentTestMode>(requestedMode);
            m_testCollisionStopped = false;
            const auto settings = instrumentTestSettings(m_testMode,
                m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons);
            setRobotControlMode(RobotControlMode::TeleOperation);
            // goToTeleOperation reports the failed motor and clears busy on failure.
            if (!m_flagInTeleoperation.load()) { continue; }
            startMotionByTime(settings.durationSeconds);
            lastForcePublish = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
            SendInnerMsg(Module_Inner_E::Uiinterface,
                static_cast<int>(UIAction_E::InstrumentForceBegin), QString::number(requestedMode));
            m_lastTestElapsedSeconds = -1;
            publishElapsed();
            SendInnerMsg(Module_Inner_E::Uiinterface,
                static_cast<int>(UIAction_E::InstrumentTestStatus), "running");
        }

        if(m_flagInTeleoperation.load()){
            teleoperation();
            const auto forceNow = std::chrono::steady_clock::now();
            if (m_testBusy.load() && m_testMode != InstrumentTestMode::PreRun
                && forceNow - lastForcePublish >= std::chrono::milliseconds(100)) {
                lastForcePublish = forceNow;
                const auto sample = m_pressureSensor ? m_pressureSensor->latestSample() : PressureSensor::Sample{};
                const bool valid = sample.valid && m_pressureBaselineValid;
                const double delta1 = sample.first - m_pressureZero1;
                const double force1 = delta1 == 0.0 ? 0.0 : delta1 / 0.08 + 0.25;
                const double force2 = (sample.second - m_pressureZero2) / 0.67;
                const double elapsed = std::chrono::duration<double>(forceNow - m_motionStartTime).count();
                // Invalid measurements are explicitly flagged; never plot held/stale values.
                const QString payload = QString::number(elapsed, 'f', 3) + "|"
                    + QString::number(sample.first, 'g', 12) + "|"
                    + QString::number(sample.second, 'g', 12) + "|"
                    + QString::number(force1, 'g', 12) + "|"
                    + QString::number(force2, 'g', 12) + "|" + (valid ? "1" : "0");
                SendInnerMsg(Module_Inner_E::Uiinterface,
                    static_cast<int>(UIAction_E::InstrumentForceSample), payload);
            }
            if (m_testBusy.load()) { publishElapsed(); }
            if (m_testBusy.load() && !m_isLooping) {
                // No further mapped command is sent after the sequence stops.
                goToHold();
                m_rightTestHomed.store(false);
                m_testBusy.store(false);
                SendInnerMsg(Module_Inner_E::Uiinterface,
                    static_cast<int>(UIAction_E::InstrumentTestStatus),
                    m_testCollisionStopped ? "collision" : "completed");
            }
        }
        usleep(5*1000);
        // std::this_thread::sleep_until(start + std::chrono::milliseconds(5));

    }


    // m_flagCommThread = false; // 停止通讯线程
}

void RobotControl::teleoperation()
{
    auto handlePoseCur = m_masterConsole.returnHandlePose();
    auto handlePosePrev = m_handlePosePrev;

    auto motorEncoderCur_R = m_motorEncoderCur_R.load();

    auto motorEncoderInit_R = m_motorEncoderInit_R;

    std::array<int, MotorNumPerSide>   targetEncoder_R = {0};


    std::array<double, MotorNumPerSide>   targetEncoder_R_forVelocity = {0};
    std::array<double, MotorNumPerSide>   targetEncoder_L_forVelocity = {0};

    std::array<int, MotorNumPerSide>   targetVelocity_R_new = {0};


    std::array<double, ControlValueNum> controlValueCur_R = {0};


    int enableTagCur_R = m_enableTagCur_R;

    targetPose(handlePoseCur);
    if (!m_isLooping) {
        return;
    }

    if(m_alignmentNumber_R != 0){
        enableTagCur_R = keepEnabling;
    }

    if(enableTagCur_R == enableAction||enableTagCur_R == keepEnabling)
    {
        if(enableTagCur_R == enableAction)
        {

            motorEncoderInit_R = motorEncoderCur_R;

            setControlInitHandleMotorPositionAndPose(motorEncoderInit_R, handlePoseCur, 'r');
        }

        controlValueCur_R = motionMapping_R(handlePoseCur,motorEncoderCur_R);

        targetEncoder_R = calculateTargetEncoder(controlValueCur_R, motorEncoderInit_R, 'r');
        std::ofstream outfile("openangle111",std::ios::app);
        outfile<<m_open<<" "<<targetEncoder_R[7]<<" "<<motorEncoderCur_R[7]<<"\n";
        outfile.close();

        if(enableTagCur_R == enableAction){
            targetVelocity_R_new = {0};
        }
    }
    else if(enableTagCur_R == disableAction || enableTagCur_R == keepDisabling)
    {
        targetEncoder_R = motorEncoderCur_R;

        for(int i = 4; i < 10; i++)
        {
            targetEncoder_R[i] = m_motorTargetEncoderLast_R[i];
        }

        targetVelocity_R_new = {0};
    }

    sendMotorData(targetEncoder_R);
    m_motorTargetEncoderPrev_L_new = targetEncoder_L_forVelocity;
    m_motorTargetEncoderPrev_R_new = targetEncoder_R_forVelocity;
    storeCurAsPrev(handlePoseCur, controlValueCur_R, motorEncoderCur_R, targetEncoder_R, enableTagCur_R);
}





void RobotControl::startMotionLoop(int loops) //启动函数
{
    m_totalLoops    = loops;
    m_currentLoop   = 0;
    m_useTimeLimit  = false;
    m_isLooping     = true;
    m_resetRequested = true;   // 让 targetPose 在下一次调用时重置状态机
}

void RobotControl::startMotionByTime(double second)
{
    m_useTimeLimit = true;             // 开启时间模式
    m_targetDurationSec = second;
    m_motionStartTime = std::chrono::steady_clock::now(); // 记录当前时间

    m_isLooping = true;
    m_resetRequested = true;

    LOG(INFO) << "Start motion for " << second <<"second ";
}


void RobotControl::targetPose(HandlePose& handlePoseCur)//每次循环对角度的操作
{

    // LOG(INFO) << "targetPose is running in Thread ID: " << std::this_thread::get_id();
    static int actionStep = 0;           // 动作步骤 0~14
    static int setCounter = 1;
    const auto testSettings = instrumentTestSettings(m_testMode,
        m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons);

    static double lastRawTargetYaw = 0.0;
    static double lastRawTargetYawact = 0.0;
    static double retreatTargetDisp = 0.0;

    double& s_zero1 = m_pressureZero1;
    double& s_zero2 = m_pressureZero2;


    // 【新增 2】定义碰撞触发阈值 (单位取决于传感器校准，假设是 N 或 kg)
    // 请根据实际情况调整这个值！如果太灵敏就改大，太迟钝就改小
    const double PRESSURE_COLLISION_THRESHOLD = 0.05;


    static std::string s_currentCsvPath = "";

    // 如果刚刚调用了 startMotionLoop / startMotionDuration，可以用这个标志重置状态机
    if (m_resetRequested) {
        actionStep     = 0;
        setCounter     = 1;
        lastRawTargetYaw = 0.0;
        lastRawTargetYawact = 0.0;
        m_moonsTargetDisp = 0.0;
        m_TargetRollAngle_pre  = 0.0;
        m_TargetPitchAngle_pre = 0.0;
        m_TargetYawAngle_pre   = 0.0;
        m_handlePoseLastLoop_R.handlePoseR_Elevation = 0.0;
        m_handlePoseLastLoop_R.handlePoseR_Arzimuth  = 0.0;
        m_handlePoseLastLoop_R.handlePoseR_Roll      = 0.0;
        m_handlePoseLastLoop_R.handlePoseR_OpenAngle = 0.0;
        const auto baseline = m_pressureSensor ? m_pressureSensor->latestSample() : PressureSensor::Sample{};
        m_pressureBaselineValid = baseline.valid;
        if (baseline.valid) {
            s_zero1 = baseline.first;
            s_zero2 = baseline.second;
            LOG(INFO) << "归零完成. 基准值 P1:" << s_zero1 << " P2:" << s_zero2;
        } else {
            s_zero1 = 0.0;
            s_zero2 = 0.0;
            LOG(WARNING) << "⚠️ 传感器未连接，无法归零";
        }

        //生成文件夹，记录压力趋势
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        // 文件名带时间戳，防止覆盖： /home/a/Desktop/TestPhotos/CycleData_17023123.csv
        ss << "/home/a/Desktop/TestPhotos/PressureData_" << now_c << ".csv";
        s_currentCsvPath = ss.str();

        std::ofstream outfile(s_currentCsvPath);
        if (outfile.is_open()) {
            outfile << "Cycle,Pressure(Net),Forcenet_1,Forcenet_2\n";
            outfile.close();
            LOG(INFO) << "📄 数据文件已创建: " << s_currentCsvPath;
        } else {
            LOG(ERROR) << "❌ 无法创建文件，请检查路径！";
        }
        m_resetRequested = false;
    }
    // 当前角度（度）
    double currentRoll  = m_TargetRollAngle_pre  * 180.0 / M_PI;
    double currentPitch = m_TargetPitchAngle_pre * 180.0 / M_PI;
    double currentYaw   = m_TargetYawAngle_pre   * 180.0 / M_PI;
    auto encoderData = m_motorEncoderCur_R.load();
    int32_t moonsEncoderVal = encoderData[10];
    // LOG(INFO)<<"moonsEncoderVal: "<<moonsEncoderVal;
    // if (m_pressureSensor && m_pressureSensor->isConnected()) {
    // double currentRawPressure = 0.0;
    // currentRawPressure = m_pressureSensor->getLatestPressure();
    // LOG(INFO)<<"當前壓力值"<<currentRawPressure;
    // }

    double currentDisp = static_cast<double>(moonsEncoderVal) / 10000.0;

    double RollAngle;
    double PitchAngle;
    double YawAngle;

    /*循环为0.005s，角度最快速度约180度每秒，所以每次循环角度改变不能大于0.9度*/
    const double MAX_DELTA_ANGLE = 0.9;
    double delt_roll  = 0.0;
    double delt_pitch = 0.0;
    double delt_yaw   = 0.0;

    // 目标角度定义（度）
    double targetRoll  = currentRoll;
    double targetPitch = currentPitch;
    double targetYaw   = currentYaw/2;
    double targetDisp  = m_moonsTargetDisp;
    // m_moonsTargetDisp  = currentDisp;
    // 【新增】临时变量，存储当前这一步想要达到的“名义目标”
    double rawTargetYaw = 0.0;

    // ==============================
    // 根据动作步骤设置目标角度
    // ==============================
    if (!m_isLooping) {
        return;
    }
    else{
        switch(actionStep)
        {
        case 0: // 初始状态
            targetRoll   = 0.0;
            targetPitch  = 0.0;
            targetYaw    = 0.0;
            targetDisp   = 0.0;
            // LOG(INFO)<<"进入循环";
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            if (reachTarget(currentRoll, currentPitch, currentYaw, currentDisp,
                            targetRoll, targetPitch, targetYaw, targetDisp)) {
                lastRawTargetYaw = rawTargetYaw;


                actionStep = testSettings.stepAfterZero;
            }
            break;

        case 1: // 动作1：角度闭合（开合角负10度，俯仰角60度, 旋转轴顺时针70度）
            targetRoll  = 60.0;
            targetPitch = 50.0;
            targetYaw   = 0.0;
            targetDisp  = 0.0;
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环1";
            if (reachTarget(currentRoll, currentPitch, currentYaw, currentDisp,
                            targetRoll, targetPitch, targetYaw, targetDisp)) {
                lastRawTargetYaw = rawTargetYaw;
                actionStep = 2;
            }
            break;

        case 2: // 动作1：旋转轴逆时针140度（70 -> -70）
            targetRoll  = -60.0;
            targetPitch = 50.0;
            // targetYaw   = 0.0;
            targetYaw   = 20.0;
            targetDisp  = 0.0;
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环2";
            if (reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)) {
                lastRawTargetYaw = rawTargetYaw;
                actionStep = 3;
            }
            break;

        case 3: // 动作2：回正（俯仰角回到0度，开合闭合）
            targetRoll  = 60.0;
            targetPitch = -50.0;
            // targetYaw   = 0.0;
            targetYaw   = 0.0;
            targetDisp  = 0.0;
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环3";
            if (reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)) {
                lastRawTargetYaw = rawTargetYaw;

                actionStep = 4;
            }
            break;

        case 4: // 动作2：左右钳头开合30度（开合角到70度）
            targetRoll  = -60.0;
            targetPitch = -50.0;
            // targetYaw   = 0.0;
            targetYaw   = testSettings.sweepYaw;
            targetDisp  = 0.0;
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环4";
            if (reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)) {
                lastRawTargetYaw = rawTargetYaw;
                actionStep = testSettings.stepAfterSweep;
            }
            break;

        case 5: // 推进并检测碰撞
        {
            targetRoll = 0.0;
            targetPitch = 0.0;
            targetYaw = 45.0;
            targetDisp = 4.9;
            // LOG(INFO)<<"开始推进111111111111111111111111111111111";

            // 1. 获取当前实时压力
            double curP1 = 0.0, curP2 = 0.0;
            if (m_pressureSensor) {
                curP1 = m_pressureSensor->getLatestPressure_1();
                curP2 = m_pressureSensor->getLatestPressure_2();
                             // LOG(INFO) <<  " Net1:" << curP1 << " Net2:" << curP2;
            }

            double net1 = std::abs(curP1 - s_zero1);
            double net2 = std::abs(curP2 - s_zero2);

            // 2. 碰撞检测逻辑
            if ( net1 > PRESSURE_COLLISION_THRESHOLD || net2 > PRESSURE_COLLISION_THRESHOLD)
            {
                LOG(INFO) << "检测到碰撞! 净压力1: " << net1 <<"净压力2:" << net2 ;

                retreatTargetDisp = currentDisp - 2.0;

                actionStep = 99;
                break;
            }

            // 3. 正常推进的判断逻辑
            bool isMotionDone = reachTarget(currentRoll, currentPitch, currentYaw, currentDisp,
                                            targetRoll, targetPitch, targetYaw, targetDisp);

            if(isMotionDone && (std::abs(m_moonsTargetDisp - targetDisp) < 0.1)) {
            //     static std::chrono::steady_clock::time_point holdStartTime;
            //     static bool isTimerStarted = false;

            //     // 1. 刚到位的第一帧，记录当前起始时间戳
            //     if (!isTimerStarted) {
            //         holdStartTime = std::chrono::steady_clock::now();
            //         isTimerStarted = true;
            //         LOG(INFO) <<"当前Yaw值:（"<< targetYaw << "） [Case 2] 运动已到位，开始 5 秒静止等待...";
            //     }

            //     // 2. 计算当前已经保持了多少秒
            //     auto currentTime = std::chrono::steady_clock::now();
            //     double elapsedSeconds = std::chrono::duration<double>(currentTime - holdStartTime).count();

            //     static int printThrottle = 0;
            //     if (printThrottle++ % 200 == 0) { // 5ms周期，200次约等于1秒
            //         LOG(INFO) << "静止等待中... 已过 " << elapsedSeconds << " 秒";
            //     }

            //     // 4. === [关键修改] 满足 5 秒时长后，仅进行【单次】读取与记录 ===
            //     if (elapsedSeconds >= 4.0) {//1.0
                lastRawTargetYaw = rawTargetYaw;
                actionStep = 6;
                LOG(INFO) << "Step 1 Finished";
                }
            break;
        }
        case 99:
        {

            targetRoll = 0.0;
            targetPitch = 0.0;
            targetYaw = 0.0;


            targetDisp = retreatTargetDisp;


            bool isRetreatDone = reachTarget(currentRoll, currentPitch, currentYaw, currentDisp,
                                             targetRoll, targetPitch, targetYaw, targetDisp);

            if(isRetreatDone && (std::abs(m_moonsTargetDisp - targetDisp) < 0.1)) {


                // 退回安全位置后，再安全地停止整个循环
                m_isLooping = false;
                actionStep = 0; // 重置状态，等待下一次整体启动
                m_testCollisionStopped = true;
            }
            break;
        }
        case 6:
        {
            targetRoll = 0.0;
            targetPitch = 0.0;

            // double calculatedYaw = -8.0 - ((setCounter-1)/6)*1.0;
            // // 限制极值，最大减到 -14.0 为止
            // if (calculatedYaw < -20.0) {
            //     calculatedYaw = -20.0;
            // }
            // targetYaw = calculatedYaw;
            targetYaw = testSettings.gripYaw;
            targetDisp = 4.9;//4.9

            // 检查运动是否到位
            if(reachTarget(currentRoll, currentPitch, currentYaw, currentDisp,
                            targetRoll, targetPitch, targetYaw, targetDisp))
            {

                // ⏱️ 静态计时变量
                static std::chrono::steady_clock::time_point holdStartTime;
                static bool isTimerStarted = false;

                // 1. 刚到位的第一帧，记录当前起始时间戳
                if (!isTimerStarted) {
                    holdStartTime = std::chrono::steady_clock::now();
                    isTimerStarted = true;
                    LOG(INFO) <<"当前Yaw值:（"<< targetYaw << "） [Case 2] 运动已到位，开始 5 秒静止等待...";
                }

                // 2. 计算当前已经保持了多少秒
                auto currentTime = std::chrono::steady_clock::now();
                double elapsedSeconds = std::chrono::duration<double>(currentTime - holdStartTime).count();

                static int printThrottle = 0;
                if (printThrottle++ % 200 == 0) { // 5ms周期，200次约等于1秒
                    LOG(INFO) << "静止等待中... 已过 " << elapsedSeconds << " 秒";
                }

                // 4. === [关键修改] 满足 5 秒时长后，仅进行【单次】读取与记录 ===
                if (elapsedSeconds >= 2.0) {//1.0
                    LOG(INFO) << "5 秒静止结束！开始读取稳定力值数据...";

                    float currentForce_1 = 0.0f;
                    float currentForce_2 = 0.0f;

                    if (m_pressureSensor && m_pressureSensor->isConnected()) {
                        currentForce_1 = m_pressureSensor->getLatestPressure_1();
                        currentForce_2 = m_pressureSensor->getLatestPressure_2();
                    } else {
                        LOG(WARNING) << "⚠️ 压力传感器未连接！";
                    }
                    double netForce_1;
                    if(currentForce_1 - s_zero1 == 0){
                        netForce_1 = 0.0;
                    }
                    else{
                        netForce_1 = (currentForce_1 - s_zero1) / 0.08 + 0.25;
                    }


                    double netForce_2 = (currentForce_2 - s_zero2) / 0.67;
                    double finalNetPressure = netForce_1 + netForce_2;

                    // if (finalNetPressure <= 5.0){
                    //     m_isLooping = false;
                    //     actionStep = 0;
                    // }

                    LOG(INFO) << "Force_1: " << currentForce_1 << " Force_2: " << currentForce_2;

                    // 打开文件并追加单行数据
                    std::ofstream outfile(s_currentCsvPath, std::ios::app);
                    if (outfile.is_open()) {
                        outfile << setCounter << ","
                                << finalNetPressure << ","
                                << netForce_1 << ","
                                << netForce_2 << "\n";
                        outfile.close();
                        LOG(INFO) << std::dec <<"记录数据 [Cycle " << setCounter << "]: Total:" << finalNetPressure
                                  << " Net1:" << netForce_1 << " Net2:" << netForce_2;
                    } else {
                        LOG(ERROR) << "写入数据失败: " << s_currentCsvPath;
                    }

                    // 5. 数据记录完毕，重置本地计时状态，切入 case 3
                    isTimerStarted = false;

                    lastRawTargetYaw = rawTargetYaw;
                    actionStep = 7;//7
                }
            }
            break;
        }

        case 7:
            targetRoll = 0.0;
            targetPitch = 0.0;
            targetYaw = 30.0;
            targetDisp = 4.9;

            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环19";
            if(reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)){
                lastRawTargetYaw = rawTargetYaw;
                actionStep = 8;
            }
            break;

        case 8:
            targetRoll = 0.0;
            targetPitch = 0.0;
            targetYaw = 30.0;
            targetDisp = 0.0;//传感器离开钳口
            // LOG(INFO)<<"当前绝对位置"<<m_moonsTargetDisp;

            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环20";
            if(reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)){
                lastRawTargetYaw = rawTargetYaw;
                actionStep = 9;
            }
            break;


        case 9: // 所有动作完成，归零，拍照
            targetRoll  = 0.0;
            targetPitch = 0.0;
            rawTargetYaw = 0.0; // 【名义目标】
            targetDisp = 0.0;

            // 【动态判断逻辑】
            // if (rawTargetYaw > lastRawTargetYaw) {
            //     targetYaw = rawTargetYaw + 5.0; // 变大 -> 加5度
            //     lastRawTargetYawact = targetYaw;
            // } else if(rawTargetYaw < lastRawTargetYaw){
            //     targetYaw = rawTargetYaw - 6.0; // 变小 -> 减5度
            //     lastRawTargetYawact = targetYaw;
            // }else{
            //     targetYaw = lastRawTargetYawact;
            // }
            // LOG(INFO)<<"进入循环21";

            if (reachTarget(currentRoll, currentPitch, currentYaw,currentDisp,
                            targetRoll, targetPitch, targetYaw,targetDisp)) {

                lastRawTargetYaw = rawTargetYaw;
                setCounter++;





                // 判断是按次数停，还是按时间停

                if (m_useTimeLimit)
                {
                    // 【时间模式】
                    auto now = std::chrono::steady_clock::now();
                    // 计算已经跑了多少秒
                    double elapsed = std::chrono::duration<double>(now - m_motionStartTime).count();

                    if (elapsed >= m_targetDurationSec) {
                        // 时间到了，停止
                        m_isLooping = false;
                        actionStep = 0;
                        LOG(INFO) << "Time is up! Finished running for " << elapsed << " seconds.";
                    } else {

                        actionStep = 0;
                        // LOG(INFO) << "Loop again... Time elapsed: " << elapsed;
                    }
                }
                else
                {
                    // 【原有的次数模式】
                    LOG(INFO)<<"m_currentLoop"<<m_currentLoop;
                    m_currentLoop++;
                    if (m_currentLoop >= m_totalLoops) {
                        m_isLooping = false;
                        actionStep = 0;
                        goToHold();

                    } else {
                        actionStep = 0;

                    }
                }

            }
            break;  

        }
    }

    const double LINEAR_STEP = 0.01; // 每次循环走 0.05mm (速度控制)

    if (m_moonsTargetDisp < targetDisp)
    {
        // 需要向前走
        m_moonsTargetDisp += LINEAR_STEP;
        // 防止超调 (Overshoot)
        if (m_moonsTargetDisp > targetDisp) m_moonsTargetDisp = targetDisp;
    }
    else if (m_moonsTargetDisp > targetDisp)
    {
        // 需要向后退
        m_moonsTargetDisp -= LINEAR_STEP;
        // 防止超调
        if (m_moonsTargetDisp < targetDisp) m_moonsTargetDisp = targetDisp;
    }
        // LOG(INFO)<<" targetDisp: "<<targetDisp<<" m_moonsTargetDisp: "<<m_moonsTargetDisp;

    const double SMOOTH_FACTOR = 0.02;

    RollAngle  = currentRoll  + (targetRoll  - currentRoll)  * SMOOTH_FACTOR;
    PitchAngle = currentPitch + (targetPitch - currentPitch) * SMOOTH_FACTOR;
    YawAngle   = currentYaw   + (targetYaw   - currentYaw)   * SMOOTH_FACTOR;

    // 安全检查：如果离目标非常近了（小于 0.1 度），直接吸附过去，防止永远不到位
    if(fabs(targetRoll - RollAngle) < 0.1) RollAngle = targetRoll;
    if(fabs(targetPitch - PitchAngle) < 0.1) PitchAngle = targetPitch;
    if(fabs(targetYaw - YawAngle) < 0.1) YawAngle = targetYaw;

    handlePoseCur.handlePoseR_Roll      = RollAngle  / 180.0 * M_PI;//rotation角
    handlePoseCur.handlePoseR_Elevation = PitchAngle / 180.0 * M_PI;
    // handlePoseCur.handlePoseR_Arzimuth  = YawAngle   / 180.0 * M_PI;//方位角。偏航角

    handlePoseCur.handlePoseR_OpenAngle = YawAngle / 180.0 * M_PI;
    m_open = targetYaw;

    m_TargetRollAngle_pre  = handlePoseCur.handlePoseR_Roll;
    m_TargetPitchAngle_pre = handlePoseCur.handlePoseR_Elevation;
    m_TargetYawAngle_pre   = handlePoseCur.handlePoseR_OpenAngle;
}

double RobotControl::backlashCompensate(double raw_angle, const char& side) {

    instrumentCompensate *m_instrument_tmp = NULL;
    char armside;

    double COMP_VAL;
    size_t WINDOW_SIZE;
    double EPS;
    double DEAD_ZONE;
    double TRANSITION_STEPS;

    if(side == '1'){
        m_instrument_tmp = &m_instrument_Left1;
        armside = 'l';
    }
    else if(side == '2'){
        m_instrument_tmp = &m_instrument_Left2;
        armside = 'l';
    }
    else if(side == '3'){
        m_instrument_tmp = &m_instrument_Left3;
        armside = 'l';
    }
    else if(side == '4'){
        m_instrument_tmp = &m_instrument_Left4;
        armside = 'l';
    }
    else if(side == '5'){
        m_instrument_tmp = &m_instrument_Right1;
        armside = 'r';
    }
    else if(side == '6'){
        m_instrument_tmp = &m_instrument_Right2;
        armside = 'r';
    }
    else if(side == '7'){
        m_instrument_tmp = &m_instrument_Right3;
        armside = 'r';
    }
    else if(side == '8'){
        m_instrument_tmp = &m_instrument_Right4;
        armside = 'r';
    }
    if(armside == 'r'){
        if(m_endeffectorConfiguration_R == EndeffectorConfiguration::sixMaxons){
            COMP_VAL = COMP_VAL_3MM;
            WINDOW_SIZE = WINDOW_SIZE_3MM;
            EPS = EPS_3MM;
            DEAD_ZONE = DEAD_ZONE_3MM;
            TRANSITION_STEPS = TRANSITION_STEPS_3MM;
        }
        else if(m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons){
            COMP_VAL = COMP_VAL_4MM;
            WINDOW_SIZE = WINDOW_SIZE_4MM;
            EPS = EPS_4MM;
            DEAD_ZONE = DEAD_ZONE_4MM;
            TRANSITION_STEPS = TRANSITION_STEPS_4MM;
        }
    }
    else if(armside == 'l'){
        if(m_endeffectorConfiguration_L == EndeffectorConfiguration::sixMaxons){
            COMP_VAL = COMP_VAL_3MM;
            WINDOW_SIZE = WINDOW_SIZE_3MM;
            EPS = EPS_3MM;
            DEAD_ZONE = DEAD_ZONE_3MM;
            TRANSITION_STEPS = TRANSITION_STEPS_3MM;
        }
        else if(m_endeffectorConfiguration_L == EndeffectorConfiguration::fourMaxons){
            COMP_VAL = COMP_VAL_4MM;
            WINDOW_SIZE = WINDOW_SIZE_4MM;
            EPS = EPS_4MM;
            DEAD_ZONE = DEAD_ZONE_4MM;
            TRANSITION_STEPS = TRANSITION_STEPS_4MM;
        }
    }

    if(m_instrument_tmp != NULL){
        m_instrument_tmp->m_openAngleDeque.push_back(raw_angle);
        if (m_instrument_tmp->m_openAngleDeque.size() > WINDOW_SIZE) m_instrument_tmp->m_openAngleDeque.pop_front();

        if (m_instrument_tmp->m_openAngleDeque.size() == WINDOW_SIZE) {
            bool is_increasing = true, is_decreasing = true, window_valid = true;
            for (size_t i = 1; i < WINDOW_SIZE; ++i) {
                double diff = m_instrument_tmp->m_openAngleDeque[i] - m_instrument_tmp->m_openAngleDeque[i-1];
                if(std::abs(diff)<=DEAD_ZONE){window_valid=false;break;}
                if (diff <= EPS) is_increasing = false;  // 非严格递增
                if (diff >= -EPS) is_decreasing = false; // 非严格递减
            }
            // 关键逻辑：只有在方向发生切换时，才更新补偿值
            if(window_valid){
                if (is_increasing && !m_instrument_tmp->dir) {
                    m_instrument_tmp->dir = true;
                    m_instrument_tmp->offset_target = COMP_VAL;
                    m_instrument_tmp->offset_transition = TRANSITION_STEPS;
                    // LOG(INFO)<<"increase";
                }
                else if (is_decreasing && m_instrument_tmp->dir) {
                    m_instrument_tmp->dir = false;
                    m_instrument_tmp->offset_target = -COMP_VAL;
                    m_instrument_tmp->offset_transition = TRANSITION_STEPS;
                    // LOG(INFO)<<"decrease";
                }
            }
        }

        if(m_instrument_tmp->offset_transition > 0){
            double step = (m_instrument_tmp->offset_target - m_instrument_tmp->offset) / m_instrument_tmp->offset_transition;
            m_instrument_tmp->offset += step;
            m_instrument_tmp->offset_transition--;
            if(m_instrument_tmp->offset_transition == 0){
                m_instrument_tmp->offset = m_instrument_tmp->offset_target;
            }
        }

        return raw_angle + m_instrument_tmp->offset;

    }

}
// 辅助函数：限制角度增量
double RobotControl::limitDelta(double delta, double maxDelta)
{
    if(delta > maxDelta) return maxDelta;
    if(delta < -maxDelta) return -maxDelta;
    return delta;
}

// 辅助函数：判断是否到达目标角度
bool RobotControl::reachTarget(double currentRoll, double currentPitch, double currentYaw,double currentDisp,
                               double targetRoll, double targetPitch, double targetYaw,double m_moonsTargetDisp)
{

    const double ANGLE_TOLERANCE = 0.3; // 角度误差允许 0.5 度
    const double DIST_TOLERANCE = 0.2;

    bool rollReached  = fabs(currentRoll  - targetRoll)  < ANGLE_TOLERANCE;
    bool pitchReached = fabs(currentPitch - targetPitch) < ANGLE_TOLERANCE;
    bool yawReached   = fabs(currentYaw   - targetYaw)   < ANGLE_TOLERANCE;
    bool DispReached  = fabs(currentDisp  - m_moonsTargetDisp) < DIST_TOLERANCE;

    if (!rollReached || !pitchReached || !yawReached || !DispReached) {
        return false;
    }

    return true;
}

void RobotControl::runCommunication()
{
    LOG(INFO) << "Communication Thread Started.";

    while (m_flagCommThread)
    {
        if (m_suspendCommunication.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue; // 跳过本次循环，不碰硬件，不抢锁
        }
        receiveMotorData();
        auto actualPos = m_motorEncoderCur_R.load(); // 从驱动层拿数据

        // --------------------------------------------
        // 2. 快速交换数据 (把货卸到仓库)
        // --------------------------------------------
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);

            // 把读到的最新位置告诉主线程
            m_shared_MotorEncoderCur_R = actualPos;

        }

        // 这里的 sendMotorData 也可以放在锁外面发，减少锁的时间
        // sendMotorData(...);

        // 稍微休息一下，防止把 CPU 占满
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}



std::array<double, ControlValueNum> RobotControl::motionMapping_R(const HandlePose& handlePoseCur, const std::array<int, MotorNumPerSide>& motorPositionCur_R)

{
    std::array<double, ControlValueNum> controlValueTmp_R = {0};

    if (m_alignmentNumber_R < 200) { m_alignmentNumber_R++; }

    /* alpha(pitch)*/
    double alpha_Org_R = m_handlePoseOrg_R.handlePoseR_Elevation;
    double alpha_Init_R = m_handlePoseInit_R.handlePoseR_Elevation;
    double alpha_Last_R = m_handlePoseLastLoop_R.handlePoseR_Elevation;
    double alpha_Cur_R = handlePoseCur.handlePoseR_Elevation;

    double delt_alphaCur_R = (alpha_Cur_R - alpha_Init_R) * 180 / M_PI;// 1. 当前动作量：医生现在的手相对于刚踩下脚踏时的变化
    double delt_alphaInit_R = (alpha_Init_R - alpha_Last_R) * 180 / M_PI;// 2. 初始偏差量（Gap）：刚踩下脚踏时的位置 - 上次松开时的位置
    double delt_alphaOrg_R = (alpha_Last_R - alpha_Org_R) * 180 / M_PI;// 3. 历史累积量：上次结束时的绝对位置（相对于零位）
    double delt_alpha_R = delt_alphaCur_R + delt_alphaInit_R * m_alignmentNumber_R / 200 + delt_alphaOrg_R;//总目标位置 = 当前动作 + (偏差量 × 进度百分比) + 历史位置
    m_delt_alpha_R = delt_alpha_R / 180 * M_PI;

    /* 计算 deltLength_alpha_R*/
    double deltLength_alpha_R_1 = cableLengths_2(delt_alpha_R);
    double deltLength_alpha_R_2 = cableLengths_2(-delt_alpha_R);

    /* 计算单边 OpenAngle*/
    double openAngle_R = handlePoseCur.handlePoseR_OpenAngle;
    double openAngle_R_new;

    openAngle_R_new = calculateNewOpenangle(openAngle_R);
    // }
    /*计算 beta(yaw)*/
    double beta_Org_R = m_handlePoseOrg_R.handlePoseR_OpenAngle;
    double beta_Init_R = m_handlePoseInit_R.handlePoseR_OpenAngle;
    double beta_Last_R = m_handlePoseLastLoop_R.handlePoseR_OpenAngle;
    double beta_Cur_R = handlePoseCur.handlePoseR_OpenAngle;

    double delt_betaCur_R = (beta_Cur_R - beta_Init_R) * 180 / M_PI;
    double delt_betaInit_R = (beta_Init_R - beta_Last_R) * 180 / M_PI;
    double delt_betaOrg_R = (beta_Last_R - beta_Org_R) * 180 / M_PI;
    double delt_beta_R = delt_betaCur_R + delt_betaInit_R * m_alignmentNumber_R / 200 + delt_betaOrg_R;
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
    double delt_gamma_R = delt_gammaCur_R + delt_gammaInit_R * m_alignmentNumber_R / 200 + delt_gammaOrg_R;
    m_delt_gamma_R = delt_gamma_R / 180 * M_PI;
    // /*4轴器械*/
    if(m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons){

        controlValueTmp_R[6] = -((delt_beta_R - delt_alpha_R * m_compRatio_R) - openAngle_R_new);
        // LOG(INFO)<<"轴6的角度是："<<delt_alpha_R<<":"<<delt_beta_R<<":"<<openAngle_R_new<<":"<<m_compRatio_R;
        controlValueTmp_R[7] = (delt_beta_R - delt_alpha_R * m_compRatio_R) + openAngle_R_new;
        controlValueTmp_R[8] = delt_alpha_R;
        controlValueTmp_R[9] = -delt_gamma_R;

        controlValueTmp_R[4] = 0;
        controlValueTmp_R[5] = 0;
    }
    /*6轴器械*/
    if(m_endeffectorConfiguration_R == EndeffectorConfiguration::sixMaxons){
    // 9号电机：Roll
    controlValueTmp_R[9] = -delt_gamma_R;
    // 8号电机：Pitch
    controlValueTmp_R[8] = deltLength_alpha_R_1;

    // 4,5,6,7号电机：Yaw + Open
    controlValueTmp_R[6] = -deltLength_beta_R_left_1;
    controlValueTmp_R[7] = -deltLength_beta_R_left_2;
    controlValueTmp_R[5] = -deltLength_beta_R_right_1;
    controlValueTmp_R[4] = -deltLength_beta_R_right_2;

    controlValueTmp_R[7] = backlashCompensate(controlValueTmp_R[7], '1');
    controlValueTmp_R[6] = backlashCompensate(controlValueTmp_R[6], '2');
    controlValueTmp_R[5] = backlashCompensate(controlValueTmp_R[5], '3');
    controlValueTmp_R[4] = backlashCompensate(controlValueTmp_R[4], '4');

    }
    controlValueTmp_R[10] = m_moonsTargetDisp;
    controlValueTmp_R[11] = handlePoseCur.graspIndex_R;


    return controlValueTmp_R;
}

double RobotControl::calculateNewOpenangle(double masterOpenangle){
    double newOpenangle;

        if(m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons){
        if(masterOpenangle < -3){
            // newOpenangle = masterOpenangle * 0.9286 + 1.286;/*-8*/
            // newOpenangle = masterOpenangle * 1.2143 + 2.1428;/*-10*/
            newOpenangle = masterOpenangle * 1.5 + 3;/*-12*/
        }else if(masterOpenangle >= -3 && masterOpenangle< 7){
            newOpenangle = 0.5 * masterOpenangle;
        }else if(masterOpenangle >= 7){
            // newOpenangle =  masterOpenangle * 1.269 - 5.3846;
            newOpenangle =  masterOpenangle * masterOpenangle * 0.0592 - 0.2988 * masterOpenangle + 2.6908;
        }
        }


        if(m_endeffectorConfiguration_R == EndeffectorConfiguration::sixMaxons){
    if(masterOpenangle < -3){
        newOpenangle = masterOpenangle * 3.8575 + 8.5714;//8.5714
    }else if(masterOpenangle >= -3 && masterOpenangle< 7){
        newOpenangle = 1.0 * masterOpenangle;
    }else if(masterOpenangle >= 7){
        newOpenangle =  masterOpenangle * 1.6429 - 4.5;//4.5

    }
    }


    return newOpenangle;
    }

void RobotControl::storeCurAsPrev(const HandlePose& handlePoseCur,
                                  const std::array<double, ControlValueNum> controlValueCur_R,
                                  const std::array<int, MotorNumPerSide>& motorPositionCur_R,
                                  const std::array<int, MotorNumPerSide>& motorTargetEncoder_R, const int&  enableTagCur_R)
{
    m_handlePosePrev = handlePoseCur;

    m_controlValuePrev_R = controlValueCur_R;
    m_motorPositionPrev_R = motorPositionCur_R;
    m_speedPedalIndex_Prev = m_speedPedalIndex_Cur;


    m_motorTargetEncoderPrev_R = motorTargetEncoder_R;

    /*出使能时记录当前姿态位置*/

    if (enableTagCur_R == disableAction)
    {
        m_handlePoseLastLoop_R = handlePoseCur;

        m_handlePoseLastLoop_R.handlePoseR_Elevation = m_delt_alpha_R;
        m_handlePoseLastLoop_R.handlePoseR_Roll = m_delt_gamma_R;
        m_handlePoseLastLoop_R.handlePoseR_Arzimuth = m_delt_beta_R;
    }

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

    if (q_3 <= qt3) {     //还未相切时
        double theta3 = q_3 - alpha3;
        pl31 = sqrt(pow(l3_1 - cos(theta3 * M_PI / 180.0) * r3_3, 2) + pow(l3_2 + sin(theta3 * M_PI / 180.0) * r3_3, 2));
    } else if (q_3 > qt3) {
        double l3_2_new = 1.44135; //why
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

std::array<int, MotorNumPerSide> RobotControl::calculateTargetEncoder(const std::array<double, ControlValueNum>& controlValue_Cur,
                                                                       const std::array<int, MotorNumPerSide>& motorPosition_Init,
                                                                       const char& side)const
{
    std::array<int, MotorNumPerSide> targetEncoder = {0};
    if(side == 'r')
    {
        if(m_endeffectorConfiguration_R == EndeffectorConfiguration::sixMaxons){

            for(int i = 4; i < 10; i++)
            {
                // targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_kForcepPosition_small_R[13 - i]);//m_encoderPerDegree_6maxon_L
                targetEncoder[i] = static_cast<int>(controlValue_Cur[i] * m_encoderPerDegree_6maxon_R[i - 4]);
            }
            targetEncoder[10] = m_moonsHomeOffsetPulses + static_cast<int>(controlValue_Cur[10] * 10000.0);
        }
        if(m_endeffectorConfiguration_R == EndeffectorConfiguration::fourMaxons){
            targetEncoder[4]=0;
            targetEncoder[5]=0;
            targetEncoder[9]=static_cast<int>(controlValue_Cur[9]*m_encoderPerDegree_R[3]);
            targetEncoder[8]=static_cast<int>(controlValue_Cur[8]*m_encoderPerDegree_R[2]);
            targetEncoder[7]=static_cast<int>(controlValue_Cur[7]*m_encoderPerDegree_R[1]);
            targetEncoder[6]=static_cast<int>(controlValue_Cur[6]*m_encoderPerDegree_R[0]);
            targetEncoder[10] = m_moonsHomeOffsetPulses + static_cast<int>(controlValue_Cur[10] * 10000.0);
        }


    }
    return targetEncoder;
}

/* 接受传回来的数据 */
void RobotControl::receiveMotorData()
{


    // std::lock_guard(m_motorDriver->m_cyclicMutex);

    std::array<int, MotorNumPerSide> motorEncoderData_R = {0};
    std::array<int, MotorNumPerSide> motorEncoderData_L = {0};
    std::array<int, GuidingMotorNum> motorEncoderData_Guiding = {0};

    for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
        motorEncoderData_R[4 + axis] = m_motorDriver->getActualPos(MotorType::MAXON, axis, arm_0);
    }
        motorEncoderData_R[10] = m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_0);
    //读取鸣志的实时电流 (使用 getActualCur)
    int16_t currentVal = m_motorDriver->getActualCur(MotorType::MOONS, 0, arm_0);
        // static int logCounter = 0;
        // // 每 100 次循环（即 0.5秒）打印一次，不要 5ms 打印一次
        // if (logCounter++ % 100 == 0) {
        //     LOG(INFO) << std::dec << " 实时电流 " << currentVal;
        // }
    m_moonsActualCurrent.store(currentVal);




    std::array<int, MotorNumPerSide> motorErrorCode_R = {0};

    for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
        motorErrorCode_R[4 + axis] = m_motorDriver->getErrorCode(MotorType::MAXON, axis, arm_0);
    }
        motorErrorCode_R[10] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_0);


    std::array<int, MotorNumPerSide> motorStatusWord_R = {0};

    for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
        motorStatusWord_R[4 + axis] = m_motorDriver->getStatusWord(MotorType::MAXON, axis, arm_0);
    }
        motorStatusWord_R[10] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);


    std::array<int, MotorNumPerSide> motorOperationMode_R = {0};

    for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
        motorOperationMode_R[4 + axis] = m_motorDriver->getOperationMode(MotorType::MAXON, axis, arm_0);
    }
        motorOperationMode_R[10] =  m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);





    /*getDigitalInput*/
    std::array<int, 7> motorInputs_Guiding = {0};
    std::array<int, MotorNumPerSide> motorInputs_R = {0};

    for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
        motorInputs_R[4 + axis] = m_motorDriver->getDigitalInputs(MotorType::MAXON, axis, arm_0)[2];
    }
        motorInputs_R[0] = m_motorDriver->getDigitalInputs(MotorType::MOONS, 0, arm_0)[6];


    m_prevRobotControlMode.store(m_curRobotControlMode.load());

/*new*/
    std::array<int, MotorNumPerSide> motorTrq_R = {0};
    std::array<int, MotorNumPerSide> motorTrq_L = {0};

    m_motorEncoderCur_R.store(motorEncoderData_R);

    m_motorStatusWordCur_R.store(motorStatusWord_R);

    m_digitalInputGuiding.store(motorInputs_Guiding);

    m_motorHomingStatus_R.store(motorInputs_R);//记录光电门是否被激活，为0时表示被激活

    m_motorOperationMode_R.store(motorOperationMode_R);

    m_motorErrorCode_R.store(motorErrorCode_R);


}

void RobotControl::sendMotorData(const std::array<int, MotorNumPerSide>& targetEncoder_R)
{


    // std::lock_guard(m_motorDriver->m_cyclicMutex);


        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            m_motorDriver->setTargetPos(MotorType::MAXON, axis, targetEncoder_R[4 + axis], arm_0);
        }
        m_motorDriver->setTargetPos(MotorType::MOONS, 0, targetEncoder_R[10], arm_0);




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
            if((m_enableTagPrev_R == disableAction || m_enableTagPrev_R == keepDisabling || m_enableTagPrev_R == ForceDisable)
                 && isPoseMatch(masterHandlePose_Cur,'r') == true)
            {
                enableFlag = enableAction;
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

    return enableFlag;
}

bool RobotControl::isPoseRight(const HandlePose& masterHandlePose_Cur, const char& side) const
{
     return true;
}

bool RobotControl::isForcePositionRight(const HandlePose& handlePoseCur, const char& side)//
{
    return true;
}

bool RobotControl::isPoseMatch(const HandlePose& masterHandlePose_Cur, const char& side) const
{
    return true;
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

    if(side == 'l')
    {
        m_handlePoseInit_L = handlePoseCur;
        m_motorEncoderInit_L = motorPositionCur;
        m_alignmentNumber_L = 0;
    }
    if(side == 'r')
    {
        m_handlePoseInit_R = handlePoseCur;
        m_motorEncoderInit_R = motorPositionCur;//需要把右边电机值存储进去
        m_alignmentNumber_R = 0;
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
            // SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvMasterData),arglist);
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


void RobotControl::MaxonGoHome_(const char& side)//yu
{
    if(side == 'r'){
        const int axes = m_instrumentAxes.load();
        if (axes != 4 && axes != 6) { return; }
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_R = {0};
        // m_flagInTeleoperation.store(false);
        m_handlePoseLastLoop_R.initOrg_R();
        m_handlePoseOrg_R.initOrg_R();

        std::thread calibration([this, axes](){
            LOG(INFO) << "👉 [CALIBRATION THREAD] ID: " << std::this_thread::get_id();
            for (int axis = instrumentFirstMaxon(axes); axis < 6; ++axis) {
                m_motorDriver->operationHOME(MotorType::MAXON, axis, arm_0);
            }
            // m_motorDriver->operationHOME(MotorType::MOONS, 0, arm_0);
            int32_t moonsTargetPos = m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_0);

            // B. 设置初始目标为当前位置 (防止猛冲)
            m_motorDriver->setTargetPos(MotorType::MOONS, 0, moonsTargetPos, arm_0);

            // C. 切换到 CSP 模式 (位置控制模式)
            m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);

            // LOG(INFO) << "Start Right Arm Homing! Moons Init Pos: " << moonsTargetPos;

            // D. 定义后退速度 (每 5ms 后退多少脉冲)
            // 假设 20000/圈。想让它每秒转 0.5 圈 = 10000 脉冲/秒
            // 循环周期约 5ms (200Hz) -> 每次减 50 脉冲
            // 如果觉得慢，可以把这个值改大；觉得快，改小。
            int32_t stepSize = 50;

            // --------------------------------------------------------
            // 3. 循环检测与控制
            // --------------------------------------------------------
            while(true)
            {
          // --- 获取状态 ---
                auto homeStatusTmp = m_motorHomingStatus_R.load();
               // LOG(INFO)<<"状态量"<<homeStatusTmp;

            // --- 判断 Maxon 是否完成 (4-9号) ---
                bool isMaxonReady = true;
                for (int axis = instrumentFirstMaxon(axes); axis < 6; ++axis) {
                    isMaxonReady = isMaxonReady && homeStatusTmp[4 + axis] == false;
                }

                // --- 判断 Moons 是否完成 (10号) ---
                // 你的条件：homeStatusTmp[10] == false 代表撞到了/到位了
                bool isMoonsSensorTriggered = (homeStatusTmp[0] == false);

                // 【核心逻辑】手动控制 Moons 往后转

                if(!isMoonsSensorTriggered)
                {
                    // 还没到位 -> 目标位置减小 (往后退)
                    moonsTargetPos -= stepSize;
                    // LOG(INFO) << "实时脉冲 " << moonsTargetPos;

                    // 发送新的目标位置
                    m_motorDriver->setTargetPos(MotorType::MOONS, 0, moonsTargetPos, arm_0);

                }
                else
                {
                    // 撞到了 (== false) -> 保持当前位置不动
                    // 不再减小 moonsTargetPos，只是重复发送当前值让它锁住
                    m_motorDriver->setTargetPos(MotorType::MOONS, 0, moonsTargetPos, arm_0);

                    // 可以在这里清零编码器偏移量 (逻辑上的清零)
                    // 但物理上先让它停住
                }

                // --- 全部完成判断 ---
                if(isMaxonReady && isMoonsSensorTriggered)
                {
                    // Hold the measured positions before leaving homing mode.
                    m_motorDriver->setTargetPos(MotorType::MOONS, 0,
                        m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_0), arm_0);
                    m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);
                    for (int axis = instrumentFirstMaxon(axes); axis < 6; ++axis) {
                        m_motorDriver->operationCSP(MotorType::MAXON, axis, arm_0);
                    }
                    QString failure;
                    for (int attempt = 0; attempt < 40; ++attempt) {
                        failure.clear();
                        const auto check = [&](MotorType type, int axis, const QString& name) {
                            const auto error = m_motorDriver->getErrorCode(type, axis, arm_0);
                            const auto status = m_motorDriver->getStatusWord(type, axis, arm_0);
                            const auto mode = m_motorDriver->getOperationMode(type, axis, arm_0);
                            if (error != 0 || (status & 0x006f) != 0x0027
                                || mode != static_cast<int>(OperationMode::CSP)) {
                                failure += QStringLiteral("%1: error=0x%2, status=0x%3, mode=%4; ")
                                    .arg(name).arg(error, 0, 16).arg(status, 0, 16).arg(mode);
                            }
                        };
                        check(MotorType::MOONS, 0, QStringLiteral("Moons 0"));
                        for (int axis = instrumentFirstMaxon(axes); axis < 6; ++axis) {
                            check(MotorType::MAXON, axis, QStringLiteral("Maxon %1").arg(axis));
                        }
                        if (failure.isEmpty()) { break; }
                        usleep(50 * 1000);
                    }
                    if (!failure.isEmpty()) {
                        m_maxonCaliFinish_R = 0;
                        m_moonsCaliFinish_R = 0;
                        m_rightTestHomed.store(false);
                        m_rightTestHoming.store(false);
                        LOG(ERROR) << "Motor not ready after homing: " << failure.toStdString();
                        SendInnerMsg(Module_Inner_E::Uiinterface,
                            static_cast<int>(UIAction_E::InstrumentTestStatus),
                            QStringLiteral("motor_not_ready:") + failure);
                        return;
                    }
                    // 1. 记录归零完成时的编码器绝对值（脉冲）
                    // 假设这时候 moonsTargetPos 是 -97012 (这就是你的机械零点)
                    m_moonsHomeOffsetPulses = moonsTargetPos;

                    // 2. 将控制目标重置为 0.0 (毫米)
                    m_moonsTargetDisp = 0.0;

                    // 3. 标记完成状态
                    m_maxonCaliFinish_R = 1;
                    m_moonsCaliFinish_R = 1;

                    // Homing is complete and the mechanism is stationary.
                    // Zero the four torque channels once here so homing loads
                    // are not stored as the sensor zero point.
                    if (!initTorqueSensorArray()) {
                        LOG(WARNING) << "Skip torque sensor zero: sensor array is unavailable";
                    } else if (!m_torqueSensorArray.zeroAll()) {
                        LOG(ERROR) << "Torque sensor zero failed after right instrument homing";
                    } else {
                        LOG(INFO) << "Torque sensor channels 1-4 zeroed after right instrument homing";
                    }

                    // ... (发送消息、Hold 等) ...
                    m_flagInHold.store(true);
                    m_alignmentNumber_R = 0;
                    m_enableTagCur_R = 1;
                    initiAllData();
                    m_rightTestHomed.store(true);
                    m_rightTestHoming.store(false);
                    SendInnerMsg(Module_Inner_E::Uiinterface, static_cast<int>(UIAction_E::FinishCalibration),"r");

                    LOG(INFO) << "Finish Homing! Zero Point Set at: " << moonsTargetPos;
                    break; // 退出 while 循环
                }

                // 控制循环频率 (非常重要，决定了手动归零的速度和平滑度)
                usleep(5 * 1000); // 5ms

            }
        });
        calibration.detach();
        usleep(20*1000);
    }

    if(side=='l'){
        m_flagInHold.store(false);
        usleep(50);
        m_motorTargetEncoderLast_L = {0};
        m_handlePoseLastLoop_L.initOrg_L();
        m_handlePoseOrg_L.initOrg_R();

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
                LOG(INFO) << "homeStatusTmpL: " << homeStatusTmp;
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
                    m_flagInHold.store(true);
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
            case static_cast<int>(RobotControlAction_E::SelectInstrumentAxes):
            {
                bool ok = false;
                const int axes = i.value().toInt(&ok);
                if (!ok || (axes != 4 && axes != 6) || !m_flagControlThread.load()
                    || m_testBusy.load() || m_rightTestHoming.load()) {
                    SendInnerMsg(Module_Inner_E::Uiinterface,
                        static_cast<int>(UIAction_E::InstrumentAxesStatus), "failed");
                    break;
                }
                m_testBusy.store(true);
                m_rightTestHomed.store(false);
                m_pendingInstrumentAxes.store(axes);
                break;
            }
            case static_cast<int>(RobotControlAction_E::StartInstrumentTest):
            {
                bool ok = false;
                const int mode = i.value().toInt(&ok);
                if (m_testBusy.load() || m_rightTestHoming.load()) {
                    break; // Duplicate requests cannot restart a running sequence.
                }
                if (!ok || !isInstrumentTestMode(mode) || !m_flagControlThread.load()
                    || m_instrumentAxes.load() == 0 || !m_rightTestHomed.load()) {
                    SendInnerMsg(Module_Inner_E::Uiinterface,
                        static_cast<int>(UIAction_E::InstrumentTestStatus), "rejected");
                    break;
                }
                m_testBusy.store(true);
                m_pendingTestMode.store(mode);
                break;
            }
            case static_cast<int>(RobotControlAction_E::StopInstrumentTest):
            {
                // Also acknowledge a stop racing with natural completion. Block restart/homing
                // until control() has consumed it; -2 replaces any start still waiting there.
                m_testBusy.store(true);
                m_pendingTestMode.store(-2);
                break;
            }
            case static_cast<int>(RobotControlAction_E::GoToTeleOperationMode):
            {
                // Test-only UI must supply an explicit, validated mode.
                LOG(WARNING) << "Use StartInstrumentTest with a selected test mode";
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

            case static_cast<int>(RobotControlAction_E::StartEndJointTransportHoming):
            {
//                TransportGoHome();
                LOG(INFO)<<"Get INFO start Homing command in RobotControl: Transport Homing!";

                break;
            }

            case static_cast<int>(RobotControlAction_E::StartEndJointMotorHoming):
            {
                char side = i.value().toUtf8().data()[0];//将 QString 转换为 char
                if(side == 'l')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Left End Joint Motor Homing!";
//                    endJointGoHome('l');
                }
                if(side == 'r')
                {
                    LOG(INFO)<<"Get INFO start Homing command in RobotControl: Right End Joint Motor Homing!";
//                    endJointGoHome('r');
                }
                break;
            }

            case static_cast<int>(RobotControlAction_E::StartEndEffectorMotorHoming):{
                if (i.value().isEmpty()) { break; }
                char side = i.value().toUtf8().data()[0];
                if(side == 'r')
                {
                    if (!m_flagControlThread.load() || m_instrumentAxes.load() == 0 || m_testBusy.load()
                        || m_rightTestHoming.exchange(true)) { break; }
                    m_rightTestHomed.store(false);
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
//                        changeAngle_L();
//                        changeAngle_R();
                    }
                    m_armAnglePerSide = 15;
                    LOG(INFO)<<"current armAngle per side! "<<m_armAnglePerSide;

                }else if(i.value() ==  "60")
                {
                    LOG(INFO) << "Instrument Angle : 60";
                    if(m_armAnglePerSide == 15){
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
