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
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_1);


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
        errCode_R[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_0);
        errCode_R[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_0);

        errCode_L[0] = m_motorDriver->getErrorCode(MotorType::MOONS, 0, arm_1);
        errCode_L[1] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 0, arm_1);
        errCode_L[2] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 1, arm_1);
        errCode_L[3] = m_motorDriver->getErrorCode(MotorType::ZERO_ERR, 2, arm_1);
        errCode_L[4] = m_motorDriver->getErrorCode(MotorType::MAXON, 0, arm_1);
        errCode_L[5] = m_motorDriver->getErrorCode(MotorType::MAXON, 1, arm_1);
        errCode_L[6] = m_motorDriver->getErrorCode(MotorType::MAXON, 2, arm_1);
        errCode_L[7] = m_motorDriver->getErrorCode(MotorType::MAXON, 3, arm_1);
        errCode_L[8] = m_motorDriver->getErrorCode(MotorType::MAXON, 4, arm_1);
        errCode_L[9] = m_motorDriver->getErrorCode(MotorType::MAXON, 5, arm_1);

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
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);

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
        modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_L[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
        modeDisplay_L[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
        modeDisplay_L[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
        modeDisplay_L[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
        modeDisplay_L[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
        modeDisplay_L[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
        modeDisplay_L[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
        modeDisplay_L[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
        modeDisplay_L[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        modeDisplay_L[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Motor Data for Hold Finish, Operation Display Right: " << modeDisplay_R;
        LOG(INFO) << "Set Motor Data for Hold Finish, Operation Display Left: " << modeDisplay_L;
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
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_0);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_1);

        std::array<int, MotorNumPerSide> statusWord_l = {0};
        std::array<int, MotorNumPerSide> statusWord_r = {0};
        statusWord_r[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_r[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        statusWord_r[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        statusWord_r[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        statusWord_r[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_r[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_r[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_r[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_r[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_r[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_l[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_l[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_l[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_l[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_l[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_l[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_l[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_l[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_l[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_l[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Right Arm Motor Status Finish, Status Word: " << std::hex << statusWord_r;
        LOG(INFO) << "Set Left  Arm Motor Status Finish, Status Word: " << std::hex << statusWord_l;

        std::array<int, MotorNumPerSide> modeDisplay_r = {0};
        std::array<int, MotorNumPerSide> modeDisplay_l = {0};

        modeDisplay_r[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_r[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay_r[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay_r[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay_r[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_r[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_r[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_r[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_r[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_r[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_l[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_l[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay_l[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay_l[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay_l[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_l[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_l[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_l[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_l[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_l[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);
        LOG(INFO) << "Set Right Arm Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay_r;
        LOG(INFO) << "Set Left  Arm Motor Data for Teleoperation Finish, Operation Display: " << modeDisplay_l;

        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN TELEOPERATION";
        break;

    }
    case static_cast<int>(RobotControlMode::Collaboration_EndJoint):
    {
        m_flagInCollaboration_EndJoint.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::array<int, MotorNumPerSide> statusWord_l = {0};
        std::array<int, MotorNumPerSide> statusWord_r = {0};
        statusWord_r[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_0);
        statusWord_r[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_0);
        statusWord_r[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_0);
        statusWord_r[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_0);
        statusWord_r[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_0);
        statusWord_r[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_0);
        statusWord_r[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_0);
        statusWord_r[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_0);
        statusWord_r[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_r[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_l[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_l[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_l[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_l[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_l[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_l[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_l[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_l[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_l[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_l[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);

        LOG(INFO) << "Set Right Arm Motor Status Finish, Status Word: " << std::hex << statusWord_r;
        LOG(INFO) << "Set Left  Arm Motor Status Finish, Status Word: " << std::hex << statusWord_l;

        std::array<int, MotorNumPerSide> modeDisplay_r = {0};
        std::array<int, MotorNumPerSide> modeDisplay_l = {0};

        modeDisplay_r[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_r[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay_r[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay_r[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay_r[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_r[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_r[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_r[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_r[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_r[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_l[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_0);
        modeDisplay_l[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_0);
        modeDisplay_l[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_0);
        modeDisplay_l[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_0);
        modeDisplay_l[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_0);
        modeDisplay_l[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_0);
        modeDisplay_l[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_0);
        modeDisplay_l[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_0);
        modeDisplay_l[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_l[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);
        LOG(INFO) << "Set Right Arm Motor Data for Hold Finish, Operation Display: " << modeDisplay_r;
        LOG(INFO) << "Set Left  Arm Motor Data for Hold Finish, Operation Display: " << modeDisplay_l;
        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN Collaboration-EndJoint";
        break;
    }
    case static_cast<int>(RobotControlMode::Collaboration_GuidingArm):
    {
        m_flagInCollaboration_GuidingArm.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));


        LOG(INFO) << "SWITCH TO HOLD ON MODE, previous Statis is: IN Collaboration_GuidingArm";
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
    LOG(INFO) << "In RobotControl::goToCollaboration_EndJoint";

    switch (static_cast<int>(curRobotControlMode))
    {
    case static_cast<int>(RobotControlMode::TeleOperation):
    {
        m_flagInTeleoperation.store(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

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
        modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_L[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
        modeDisplay_L[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
        modeDisplay_L[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
        modeDisplay_L[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
        modeDisplay_L[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
        modeDisplay_L[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
        modeDisplay_L[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
        modeDisplay_L[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
        modeDisplay_L[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        modeDisplay_L[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);
        usleep(50 * 1000);
        LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Right Arm: " << std::hex << modeDisplay_R;
        LOG(INFO) << "Set Operation Mode for Teleoperation Finish, Operation Display of Left Arm: " << std::hex << modeDisplay_L;

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
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        LOG(INFO) << "Go to Collaboration-EndJoint Finish, Status Word of Right Arm: " << std::hex << statusWord_R;
        LOG(INFO) << "Go to Collaboration-EndJoint Finish, Status Word of Left Arm: " << std::hex << statusWord_L;

        LOG(INFO) << "SWITCH TO Collaboration-EndJoint MODE, previous Statis is: IN TELEOPERATION";
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
        m_guidingArm.m_guidingArmCurrentState = GuidingArmState::HOLD;
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 0, arm_guiding);
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 1, arm_guiding);
        m_motorDriver->operationPV(MotorType::ZERO_ERR, 2, arm_guiding);

        std::array<int, MotorNumPerSide> statusWord = {0};
        statusWord[0] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_guiding);
        statusWord[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_guiding);
        statusWord[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_guiding);
        LOG(INFO) << "status Word of Guiding Arm: " << statusWord;

        LOG(INFO) << "SWITCH TO Collaboration MODE, previous Statis is: IN HOLD STAUTS";
        break;
    }
    }
    m_curRobotControlMode.store(RobotControlMode::Collaboration_GuidingArm);

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
        /*进入手术保存当前初始机械臂位置*/
        auto motorPositionCur_R = m_motorEncoderCur_R.load();
        auto motorPositionCur_L = m_motorEncoderCur_L.load();

        long magneticEncoder_Cur_L = static_cast<long>(m_magneticEncoder_L.load()) - static_cast<long>(MagneticEncoder_Init_L);
        long magneticEncoder_Cur_R = static_cast<long>(m_magneticEncoder_R.load()) - static_cast<long>(MagneticEncoder_Init_R);
        auto motorEncode_L = m_motorEncoderCur_L.load();
        auto motorEncode_R = m_motorEncoderCur_R.load();

        m_motorEncodeInit_X_L = motorEncode_L[0] - magneticEncoder_Cur_L * 4;
        m_motorEncodeInit_X_R = motorEncode_R[0] - magneticEncoder_Cur_R * 4;

        LOG(INFO)<<std::dec<<"motorEncode_R: "<<motorEncode_R[0]<<" m_motorEncodeInit_X_R: "<<m_motorEncodeInit_X_R<<" magneticEncoder_Cur_R: "<<magneticEncoder_Cur_R;
        LOG(INFO)<<std::dec<<"mag_R: "<<static_cast<long>(m_magneticEncoder_R.load())<<" INIT_R: "<<static_cast<long>(MagneticEncoder_Init_R);

        auto handlePoseCur = m_masterConsole.returnHandlePose();/*只用x轴位置*/

        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur_L,'l');
        calculateEndEffectorPosition_init(handlePoseCur, motorPositionCur_R,'r');

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
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
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
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_0);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_0);

        m_motorDriver->operationCSV(MotorType::MOONS, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 0, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 1, arm_1);
        m_motorDriver->operationCSV(MotorType::ZERO_ERR, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 0, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 1, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 2, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 3, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 4, arm_1);
        m_motorDriver->operationCSP(MotorType::MAXON, 5, arm_1);
        usleep(50 * 1000);

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
        modeDisplay_R[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_0);
        modeDisplay_R[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_0);

        modeDisplay_L[0] = m_motorDriver->getOperationMode(MotorType::MOONS, 0, arm_1);
        modeDisplay_L[1] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 0, arm_1);
        modeDisplay_L[2] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 1, arm_1);
        modeDisplay_L[3] = m_motorDriver->getOperationMode(MotorType::ZERO_ERR, 2, arm_1);
        modeDisplay_L[4] = m_motorDriver->getOperationMode(MotorType::MAXON, 0, arm_1);
        modeDisplay_L[5] = m_motorDriver->getOperationMode(MotorType::MAXON, 1, arm_1);
        modeDisplay_L[6] = m_motorDriver->getOperationMode(MotorType::MAXON, 2, arm_1);
        modeDisplay_L[7] = m_motorDriver->getOperationMode(MotorType::MAXON, 3, arm_1);
        modeDisplay_L[8] = m_motorDriver->getOperationMode(MotorType::MAXON, 4, arm_1);
        modeDisplay_L[9] = m_motorDriver->getOperationMode(MotorType::MAXON, 5, arm_1);
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
        statusWord_R[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_0);
        statusWord_R[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_0);

        statusWord_L[0] = m_motorDriver->getStatusWord(MotorType::MOONS, 0, arm_1);
        statusWord_L[1] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 0, arm_1);
        statusWord_L[2] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 1, arm_1);
        statusWord_L[3] = m_motorDriver->getStatusWord(MotorType::ZERO_ERR, 2, arm_1);
        statusWord_L[4] = m_motorDriver->getStatusWord(MotorType::MAXON, 0, arm_1);
        statusWord_L[5] = m_motorDriver->getStatusWord(MotorType::MAXON, 1, arm_1);
        statusWord_L[6] = m_motorDriver->getStatusWord(MotorType::MAXON, 2, arm_1);
        statusWord_L[7] = m_motorDriver->getStatusWord(MotorType::MAXON, 3, arm_1);
        statusWord_L[8] = m_motorDriver->getStatusWord(MotorType::MAXON, 4, arm_1);
        statusWord_L[9] = m_motorDriver->getStatusWord(MotorType::MAXON, 5, arm_1);
        usleep (50 * 1000);

        LOG(INFO) << "Go to Teleoperation Finish, Status Word of Right Arm: " << std::hex << statusWord_R;
        LOG(INFO) << "Go to Teleoperation Finish, Status Word of Left Arm: " << std::hex << statusWord_L;
    }
    }

    m_curRobotControlMode.store(RobotControlMode::TeleOperation);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_flagInTeleoperation.store(true);

}
