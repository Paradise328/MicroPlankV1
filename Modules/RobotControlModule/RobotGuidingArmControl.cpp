#include "RobotControl.h"
std::ofstream outfile("output.txt");
double tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
void RobotControl::startGuidingArmControlThread()
{
    m_guidingArmControlThread = std::thread(&RobotControl::guidingArmControl, this);
    m_guidingArmControlThread.detach();
}

/* control loop of guiding arm */
void RobotControl::guidingArmControl(){
    while(true){
        updateGuidingArmMotion();

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

        /* print guiding Arm Motion Values to Screen */
        // guidingArmPrinting();

        /* Log Arm info to a file */
        // guidingArmLogging();

        usleep(4 * 1000);
    }
}

bool RobotControl::judgeGuidingArmStable(){
    return true;
    int32_t velocity_joint0 = m_motorDriver->getActualVel(MotorType::ZERO_ERR, 0, arm_guiding);
    int32_t velocity_joint1 = m_motorDriver->getActualVel(MotorType::ZERO_ERR, 1, arm_guiding);
    int32_t velocity_joint2 = m_motorDriver->getActualVel(MotorType::ZERO_ERR, 2, arm_guiding);
    int stableVelocityThreshold = 3000;
    if(abs(velocity_joint0) <= stableVelocityThreshold
        && abs(velocity_joint1) <= stableVelocityThreshold
        && abs(velocity_joint2) <= stableVelocityThreshold){
        return true;
    }
    return false;
}

void RobotControl::initGuidingArm(){
    LOG(INFO)<<"[INIT GUIDING ARM]";
    m_guidingArm.m_guidingArmCurrentState = GuidingArmState::HOLD;
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 0, arm_guiding);
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 1, arm_guiding);
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 2, arm_guiding);
}

void RobotControl::disableGuidingArm(){
    LOG(INFO)<<"[DISABLE GUIDING ARM]";
    // m_guidingArm.m_guidingArmCurrentState = GuidingArmState::INIT;
    // m_motorDriver->setControlWord(MotorType::ZERO_ERR, 0, ControlCommand::DISABLE, arm_guiding);
    // m_motorDriver->setControlWord(MotorType::ZERO_ERR, 1, ControlCommand::DISABLE, arm_guiding);
    // m_motorDriver->setControlWord(MotorType::ZERO_ERR, 2, ControlCommand::DISABLE, arm_guiding);
    // m_motorDriver->setJointEnableStatus(MotorType::ZERO_ERR, 0, arm_guiding,  false);
    // m_motorDriver->setJointEnableStatus(MotorType::ZERO_ERR, 1, arm_guiding,  false);
    // m_motorDriver->setJointEnableStatus(MotorType::ZERO_ERR, 2, arm_guiding,  false);
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 0, arm_guiding);
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 1, arm_guiding);
    m_motorDriver->operationPV(MotorType::ZERO_ERR, 2, arm_guiding);
}

void RobotControl::guidingArmHold2Drag(){
    LOG(INFO)<<"[HOLD] SWITCH TO [DRAG]";
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 0, arm_guiding);
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 1, arm_guiding);
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 2, arm_guiding);
}

void RobotControl::guidingArmDamp2Hold(){
    LOG(INFO)<<"[DAMP] SWITCH TO [HOLD]";
    // m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 0, arm_guiding);
    // m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 1, arm_guiding);
    // m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 2, arm_guiding);
}

void RobotControl::guidingArmDrag2Damp(){
    LOG(INFO)<<"[DRAG] SWITCH TO [DAMP]";
    m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 0, arm_guiding);
    m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 1, arm_guiding);
    m_motorDriver->switchPT2PV(MotorType::ZERO_ERR, 2, arm_guiding);
}

void RobotControl::guidingArmDamp2Drag(){
    LOG(INFO)<<"[DAMP] SWITCH TO [DRAG]";
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 0, arm_guiding);
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 1, arm_guiding);
    m_motorDriver->switchPV2PT(MotorType::ZERO_ERR, 2, arm_guiding);
}

bool RobotControl::isGuidingArmDamping(){
    return false;
}

void RobotControl::applyGuidingArmForceControl(){
    /* apply PT update in this function */
    /* set target torque in order to compensate the friction */
    LOG(INFO)<<"[MODE:DRAG]";
    double target_trq_0 = signDouble(m_guidingArm.m_velocity_1stOrder[0]) * 20.0;
    double target_trq_1 = signDouble(m_guidingArm.m_velocity_1stOrder[1]) * 15.0;
    double target_trq_2 = signDouble(m_guidingArm.m_velocity_1stOrder[2]) * 15.0;

    tmp1 = signDouble(m_guidingArm.m_velocity_1stOrder[0]);
    tmp2 = signDouble(m_guidingArm.m_velocity_1stOrder[1]);
    tmp3 = signDouble(m_guidingArm.m_velocity_1stOrder[2]);
    tmp4 = target_trq_0;
    tmp5 = target_trq_1;
    tmp6 = target_trq_2;
    m_motorDriver->setTargetTrq(MotorType::ZERO_ERR, 0, static_cast<int16_t>(target_trq_0), arm_guiding);
    m_motorDriver->setTargetTrq(MotorType::ZERO_ERR, 1, static_cast<int16_t>(target_trq_1), arm_guiding);
    m_motorDriver->setTargetTrq(MotorType::ZERO_ERR, 2, static_cast<int16_t>(target_trq_2), arm_guiding);
}

void RobotControl::applyGuidingArmDampingControl(){
    /* apply a decay-damping when the button is released */
    LOG(INFO)<<"[MODE:DAMP]";
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, 0, arm_guiding);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, 0, arm_guiding);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, 0, arm_guiding);
}

void RobotControl::applyGuidingArmVelocityControl(){
    /* apply PV update in this function */
    // LOG(INFO)<<"[MODE:HOLD]";
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 0, 0, arm_guiding);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 1, 0, arm_guiding);
    m_motorDriver->setTargetVel(MotorType::ZERO_ERR, 2, 0, arm_guiding);
}

/* update state machine of guiding arm */
void RobotControl::updateGuidingArmState(){
    // LOG(INFO) << "updateGuidingArmState";
    switch(m_guidingArm.m_guidingArmCurrentState){
        case GuidingArmState::HOLD :
            if(m_guidingArm.m_guidingArmEnableBtnPressed){
                LOG(INFO) << "guidingArmHold2Drag() ";
                guidingArmHold2Drag();
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::DRAG;
            }else{
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::HOLD;
            }
            break;
        case GuidingArmState::DRAG :
            if(!m_guidingArm.m_guidingArmEnableBtnPressed){
                LOG(INFO) << "guidingArmDrag2Damp() ";
                guidingArmDrag2Damp();
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::DAMPING;
            }else{
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::DRAG;
            }
            break;
        case GuidingArmState::DAMPING :
            if(m_guidingArm.m_guidingArmEnableBtnPressed){
                LOG(INFO) << "guidingArmDamp2Drag() ";
                guidingArmDamp2Drag();
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::DRAG;
            }else{
                if(!isGuidingArmDamping() || m_guidingArm.m_isGuidingArmStable){
                    LOG(INFO) << "guidingArmDamp2Hold() ";
                    guidingArmDamp2Hold();
                    m_guidingArm.m_guidingArmCurrentState = GuidingArmState::HOLD;
                }
                m_guidingArm.m_guidingArmCurrentState = GuidingArmState::DAMPING;
            }
            break;
        default:
            break;
    }
}

double RobotControl::signDouble(double target){
    double k = 0.19;
    return std::tanh(k*target);
}

void RobotControl::updateGuidingArmMotion(){
    std::array<int, 6>  digitalInputGuiding = m_digitalInputGuiding.load();

    m_guidingArm.m_guidingArmEnableBtnPressed = static_cast<bool>(digitalInputGuiding[4]);
    m_guidingArm.m_isGuidingArmStable = judgeGuidingArmStable();

    m_guidingArm.m_actualCurrent0 = m_motorDriver->getActualCur(MotorType::ZERO_ERR, 0, arm_guiding);
    m_guidingArm.m_actualTorque0 = m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 0, arm_guiding);
    m_guidingArm.m_actualVelocity0 = static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 0, arm_guiding));

    m_guidingArm.m_actualCurrent1 = m_motorDriver->getActualCur(MotorType::ZERO_ERR, 1, arm_guiding);
    m_guidingArm.m_actualTorque1 = m_motorDriver->getActualTrq(MotorType::ZERO_ERR, 1, arm_guiding);
    m_guidingArm.m_actualVelocity1 = static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 1, arm_guiding));

    m_guidingArm.m_actualCurrent2 = m_motorDriver -> getActualCur(MotorType::ZERO_ERR, 2, arm_guiding);
    m_guidingArm.m_actualTorque2 = m_motorDriver -> getActualTrq(MotorType::ZERO_ERR, 2, arm_guiding);
    m_guidingArm.m_actualVelocity2 = static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 2, arm_guiding));
    m_guidingArm.m_velocity = {
        static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 0, arm_guiding)),
        static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 1, arm_guiding)),
        static_cast<double>(m_motorDriver->getActualVel(MotorType::ZERO_ERR, 2, arm_guiding))
    };
    m_guidingArm.m_velocity_1stOrder = m_filter_1storder_guiding.update(m_guidingArm.m_velocity);
    m_guidingArm.m_velocity_2ndOrder = m_filter_2ndorder_guiding.update(m_guidingArm.m_velocity);
}

void RobotControl::guidingArmPrinting(){
    system("clear");

    LOG(INFO) << "m_flag_guidingArmEnableBtn: " << m_guidingArm.m_guidingArmEnableBtnPressed ;
    LOG(INFO) << "m_guidingArmCurrentState: " << static_cast<int>(m_guidingArm.m_guidingArmCurrentState) ;
    LOG(INFO) << "guiding arm joint 0 operation mode: " << m_motorDriver -> getOperationMode(MotorType::ZERO_ERR, 0, arm_guiding);
    LOG(INFO) << "guiding arm joint 0 actual current: " << std::dec<< m_guidingArm.m_actualCurrent0;
    LOG(INFO) << "guiding arm joint 0 actual torque: " << std::dec << m_guidingArm.m_actualTorque0;
    LOG(INFO) << "guiding arm joint 0 actual velocity: " << std::dec << m_guidingArm.m_actualVelocity0;
    LOG(INFO) << "guiding arm joint 1 operation mode: " << m_motorDriver -> getOperationMode(MotorType::ZERO_ERR, 1, arm_guiding);
    LOG(INFO) << "guiding arm joint 1 actual current: " << std::dec << m_guidingArm.m_actualCurrent1;
    LOG(INFO) << "guiding arm joint 1 actual torque: " << std::dec << m_guidingArm.m_actualTorque1;
    LOG(INFO) << "guiding arm joint 1 actual velocity: " << std::dec << m_guidingArm.m_actualVelocity1;
    LOG(INFO) << "guiding arm joint 2 operation mode: " << m_motorDriver -> getOperationMode(MotorType::ZERO_ERR, 2, arm_guiding);
    LOG(INFO) << "guiding arm joint 2 actual current: " << std::dec << m_guidingArm.m_actualCurrent2;
    LOG(INFO) << "guiding arm joint 2 actual torque: " << std::dec << m_guidingArm.m_actualTorque2;
    LOG(INFO) << "guiding arm joint 2 actual velocity: " << std::dec << m_guidingArm.m_actualVelocity2;
    LOG(INFO) << "guiding arm joint 2 1st-filtered velocity: " << std::dec << m_guidingArm.m_velocity_1stOrder[2];
    LOG(INFO) << "tmp values: " << std::dec << tmp1 << " " << tmp2 << " " << tmp3;
    LOG(INFO) << "tmp values: " << std::dec << tmp4 << " " << tmp5 << " " << tmp6;
}

void RobotControl::guidingArmLogging(){
    if(m_guidingArm.m_guidingArmCurrentState != GuidingArmState::DRAG){
        return ;
    }
    // outfile <<
    for(int i = 0; i < 2; ++i){
        outfile << m_guidingArm.m_velocity[i] << " "
                << m_guidingArm.m_velocity_1stOrder[i] << " "
                << m_guidingArm.m_velocity_2ndOrder[i] << " ";
    }
    outfile << "\n";
}

double RobotControl::computeGuidingArmFrictionTorque(double velocity, double tau_c, double B, double k=1.0){
    if(m_guidingArm.m_guidingArmCurrentFrictionStateJoint0 ==
        GuidingArmFrictionState::IDLE) return 0.0;

    return tau_c*signDouble(k*velocity)+B*velocity;
}


