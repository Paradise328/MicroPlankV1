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
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            m_motorDriver->operationCSP(MotorType::MAXON, axis, arm_0);
        }

        std::array<int, MotorNumPerSide> errCode_R = {0};

        errCode_R[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            errCode_R[4 + axis] = m_motorDriver->getErrorCode(MotorType::MAXON, axis, arm_0);
        }


        LOG(INFO) << "Set Motor Status Finish, Error Code Right: " << std::hex <<errCode_R;
//        LOG(INFO) << "Set Motor Status Finish, Error Code Left: " << std::hex <<errCode_L;

        std::array<int, MotorNumPerSide> statusWord_R = {0};

        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            statusWord_R[4 + axis] = m_motorDriver->getStatusWord(MotorType::MAXON, axis, arm_0);
        }

        LOG(INFO) << "Set Motor Status Finish, Status Word Right: " << std::hex <<statusWord_R;
//        LOG(INFO) << "Set Motor Status Finish, Status Word Left: " << std::hex <<statusWord_L;

        std::array<int, MotorNumPerSide> modeDisplay_R = {0};

        modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            modeDisplay_R[4 + axis] = m_motorDriver->getOperationMode(MotorType::MAXON, axis, arm_0);
        }

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

        m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            m_motorDriver->operationCSP(MotorType::MAXON, axis, arm_0);
        }

//        std::array<int, MotorNumPerSide> statusWord_l = {0};
        std::array<int, MotorNumPerSide> statusWord_r = {0};

        statusWord_r[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            statusWord_r[4 + axis] = m_motorDriver->getStatusWord(MotorType::MAXON, axis, arm_0);
        }

        LOG(INFO) << "Set Right Arm Motor Status Finish, Status Word: " << std::hex << statusWord_r;
//        LOG(INFO) << "Set Left  Arm Motor Status Finish, Status Word: " << std::hex << statusWord_l;

        std::array<int, MotorNumPerSide> modeDisplay_r = {0};
//        std::array<int, MotorNumPerSide> modeDisplay_l = {0};

        modeDisplay_r[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            modeDisplay_r[4 + axis] = m_motorDriver->getOperationMode(MotorType::MAXON, axis, arm_0);
        }

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
    m_flagInTeleoperation.store(false);

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
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            statusWord_R[4 + axis] = m_motorDriver->getStatusWord(MotorType::MAXON, axis, arm_0);
        }

        usleep (50 * 1000);

        LOG(INFO) << "Go to Teleoperation start, Status Word of Right Arm: " << std::hex << statusWord_R;
        m_motorDriver->setTargetPos(MotorType::MOONS, 0,
            m_motorDriver->getActualPos(MotorType::MOONS, 0, arm_0), arm_0);
        m_motorDriver->operationCSP(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            m_motorDriver->operationCSP(MotorType::MAXON, axis, arm_0);
        }

        usleep(50 * 1000);

        std::array<int, MotorNumPerSide> modeDisplay_R = {0};

        modeDisplay_R[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            modeDisplay_R[4 + axis] = m_motorDriver->getOperationMode(MotorType::MAXON, axis, arm_0);
        }

        usleep(50 * 1000);
        LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Right Arm: " << std::hex << modeDisplay_R;

        statusWord_R[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            statusWord_R[4 + axis] = m_motorDriver->getStatusWord(MotorType::MAXON, axis, arm_0);
        }

        usleep (50 * 1000);

        LOG(INFO) << "Go to Teleoperation Finish, Status Word of Right Arm: " << std::hex << statusWord_R;
        break;

    }
    }

    // Require actual drive readiness, not merely successful command submission.
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
        for (int axis = instrumentFirstMaxon(m_instrumentAxes.load()); axis < 6; ++axis) {
            check(MotorType::MAXON, axis, QStringLiteral("Maxon %1").arg(axis));
        }
        if (failure.isEmpty()) { break; }
        usleep(50 * 1000);
    }
    if (!failure.isEmpty()) {
        m_isLooping = false;
        m_flagInHold.store(false);
        m_curRobotControlMode.store(RobotControlMode::Hold);
        m_rightTestHomed.store(false);
        m_testBusy.store(false);
        LOG(ERROR) << "Test start blocked: " << failure.toStdString();
        SendInnerMsg(Module_Inner_E::Uiinterface,
            static_cast<int>(UIAction_E::InstrumentTestStatus),
            QStringLiteral("motor_not_ready:") + failure);
        return;
    }

    m_curRobotControlMode.store(RobotControlMode::TeleOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);







}
