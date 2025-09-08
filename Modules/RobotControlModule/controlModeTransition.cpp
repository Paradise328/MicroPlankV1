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
