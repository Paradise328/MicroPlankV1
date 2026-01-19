#include "RobotControl.h"

void RobotControl::setRobotControlMode(const RobotControlMode& tartgetRobotControlMode)
{
    switch(static_cast<int>(tartgetRobotControlMode))
    {
    case static_cast<int>(RobotControlMode::TeleOperation):
    {
        goToTeleOperation();
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

        m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

        std::array<int, MotorNumPerSide> errCode_R = {0};

        errCode_R[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_0);
        errCode_R[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_0);
        errCode_R[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_0);
        errCode_R[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_0);
        errCode_R[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_0);
        errCode_R[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_0);
        errCode_R[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_0);


        LOG(INFO) << "Set Motor Status Finish, Error Code Right: " << std::hex <<errCode_R;
//        LOG(INFO) << "Set Motor Status Finish, Error Code Left: " << std::hex <<errCode_L;

        std::array<int, MotorNumPerSide> statusWord_R = {0};

        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        LOG(INFO) << "Set Motor Status Finish, Status Word Right: " << std::hex <<statusWord_R;
//        LOG(INFO) << "Set Motor Status Finish, Status Word Left: " << std::hex <<statusWord_L;

        std::array<int, MotorNumPerSide> modeDisplay_R = {0};

        modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_R[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_R[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_R[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_R[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        SendInnerMsg(Module_Inner_E::Uiinterface,static_cast<int>(UIAction_E::RecvSystemBootSta),"Ok");
        break;
    }
    case static_cast<int>(RobotControlMode::TeleOperation)://通信模式
    {
        if(m_enableTagPrev_R == 1 || m_enableTagPrev_R == 2 || m_enableTagPrev_R == 5 || m_enableTagPrev_R == 6){
            if(m_enableTagPrev_R == 1 || m_enableTagPrev_R == 2){
                m_handlePoseLastLoop_R = m_masterConsole.returnHandlePose();
            }
            m_enableTagPrev_R = keepDisabling;
        }

        if(m_enableTagPrev_L == 1 || m_enableTagPrev_L == 2 || m_enableTagPrev_L == 5 || m_enableTagPrev_L == 6){
            if(m_enableTagPrev_L == 1 || m_enableTagPrev_L == 2){
                m_handlePoseLastLoop_L = m_masterConsole.returnHandlePose();
            }
            m_enableTagPrev_L = keepDisabling;
        }

        m_flagInTeleoperation.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));//当前线程暂停 100 毫秒
        LOG(INFO)<<"FROM TeleOperation TO HOLD";

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

//        std::array<int, MotorNumPerSide> statusWord_l = {0};
        std::array<int, MotorNumPerSide> statusWord_r = {0};

        statusWord_r[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_r[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_r[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_r[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_r[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_r[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_r[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        LOG(INFO) << "Set Right Arm Motor Status Finish, Status Word: " << std::hex << statusWord_r;
//        LOG(INFO) << "Set Left  Arm Motor Status Finish, Status Word: " << std::hex << statusWord_l;

        std::array<int, MotorNumPerSide> modeDisplay_r = {0};
//        std::array<int, MotorNumPerSide> modeDisplay_l = {0};

        modeDisplay_r[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_r[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_r[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_r[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_r[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_r[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_r[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        LOG(INFO) << "Set Right Arm Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay_r;
//        LOG(INFO) << "Set Left  Arm Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay_l;

        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN TELEOPERATION";
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





void RobotControl::goToTeleOperation()
{
    auto curRobotControlMode = m_curRobotControlMode.load();

    switch (static_cast<int>(curRobotControlMode))
    {
    case static_cast<int>(RobotControlMode::TeleOperation):
    {
        LOG(INFO) << "SWITCH TO TELEOPERATION MODE, previous Statis is: IN TELEOPERATION";
        break;

    }
    case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
    {
        LOG(INFO) << "SWITCH TO TELEOPERATION MODE, previous Statis is: IN Collaboration_EndJoint";
        break;

    }
    case static_cast<int>(RobotControlMode::Hold):
    {

        m_flagInHold.store(false);
        usleep(10 * 1000);

        LOG(INFO) << "Switch TO Teleoperation Mode, previous Statis is: IN HOLD STAUTS";

        std::array<int, MotorNumPerSide> statusWord_R = {0};

        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        usleep (50 * 1000);

        LOG(INFO) << "Go to Teleoperation start, Status Word of Right Arm: " << std::hex << statusWord_R;
        m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

        usleep(50 * 1000);

        std::array<int, MotorNumPerSide> modeDisplay_R = {0};

        modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_R[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_R[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_R[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_R[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        usleep(50 * 1000);
        LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Right Arm: " << std::hex << modeDisplay_R;

        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_R[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_R[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_R[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_R[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        usleep (50 * 1000);

        LOG(INFO) << "Go to Teleoperation Finish, Status Word of Right Arm: " << std::hex << statusWord_R;
        break;

    }
    }

    m_curRobotControlMode.store(RobotControlMode::TeleOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);





        // // 🚀 启动后台线程
        // std::thread startupThread([this]()
        //                           {
        //                               LOG(INFO) << "[Startup] Sequence Started. ID: " << std::this_thread::get_id();
        //                               m_suspendCommunication.store(true);


        //                               // 给一点时间让 Comm 线程交出锁并进入休眠
        //                               std::this_thread::sleep_for(std::chrono::milliseconds(50));


        //                               for(int i = 0; i < 6; i++) {
        //                                   m_motorDriver->setOperationMode(MotorType::MAXON, i, OperationMode::CSP, arm_0);
        //                               }
        //                               LOG(INFO) << "[Startup] Setting CSP Mode...";
        //                               // } <--- 删掉花括号

        //                               std::this_thread::sleep_for(std::chrono::milliseconds(100));


        //                               // =======================================================
        //                               // 第二步：故障复位
        //                               // =======================================================
        //                               // { <--- 删掉花括号
        //                               // std::lock_guard... <--- 删掉锁！
        //                               for(int i = 0; i < 6; i++) {
        //                                   // 使用 static_cast 强转，确保编译通过
        //                                   m_motorDriver->setControlWord(MotorType::MAXON, i, static_cast<ControlCommand>(128), arm_0);
        //                               }
        //                               LOG(INFO) << "[Startup] Resetting Faults...";
        //                               // } <--- 删掉花括号

        //                               std::this_thread::sleep_for(std::chrono::milliseconds(200));


        //                               // =======================================================
        //                               // 第三步：使能电机
        //                               // =======================================================
        //                               // { <--- 删掉花括号
        //                               // std::lock_guard... <--- 删掉锁！
        //                               for(int i = 0; i < 6; i++) {
        //                                   m_motorDriver->setControlWord(MotorType::MAXON, i, static_cast<ControlCommand>(6), arm_0);
        //                                   m_motorDriver->setControlWord(MotorType::MAXON, i, static_cast<ControlCommand>(15), arm_0);
        //                               }
        //                               LOG(INFO) << "[Startup] Enabling Motors...";
        //                               // } <--- 删掉花括号

        //                               std::this_thread::sleep_for(std::chrono::milliseconds(500));


        //                               // =======================================================
        //                               // 第四步：同步数据
        //                               // =======================================================
        //                               // 这里必须加锁！因为这是纯数据的读写，没有调用 Driver 的复杂函数
        //                               // 这一步通常是安全的，因为 load() 和简单赋值不会死锁
        //                               //{
        //                                  // std::lock_guard<std::mutex> lock(m_motorDriver->m_cyclicMutex);

        //                                   receiveMotorData();
        //                                   auto currentPos = m_motorEncoderCur_R.load();

        //                                   m_motorEncoderInit_R = currentPos;
        //                                   m_motorTargetEncoderPrev_R = currentPos;

        //                                   m_safetyStartupCounter = 0;
        //                                   m_resetRequested = true;

        //                                   LOG(INFO) << "[Startup] Data Synced. Ready to Rock!";
        //                               //}

        //                               // =======================================================
        //                               // 第五步：完成
        //                               // =======================================================
        //                               m_curRobotControlMode.store(RobotControlMode::TeleOperation);
        //                               m_flagInTeleoperation.store(true);

        //                               LOG(INFO) << "[Startup] Transition Complete.";
        //                           });

        // startupThread.detach();



}
