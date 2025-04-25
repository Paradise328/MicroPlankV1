#include "FdOmega.h"

int FdOmega::initDevice(){

    std::cout << "Force Dimension - Virtual Tele-operation Example " << dhdGetSDKVersionStr() << ")" << std::endl;
    std::cout << "Copyright (C) 2001-2023 Force Dimension" << std::endl;
    std::cout << "All Rights Reserved." << std::endl << std::endl;

    m_deviceID = drdOpen();
    if (m_deviceID < 0)
    {
        LOG(ERROR) << "Error: Failed to open haptic device (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }

    // Initialize haptic device if required.
    LOG(INFO) << "Initializing haptic device...";
    if (!drdIsInitialized(m_deviceID) && drdAutoInit(m_deviceID) < 0)
    {
        LOG(ERROR) << "Error: Failed to initialize device (" << dhdErrorGetLastStr() << ")";
        return -1;
    }

    if (!drdIsRunning(m_deviceID) && drdStart(m_deviceID) < 0)
    {
        LOG(ERROR) << "Error: failed to start robotic regulation (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }

    double initialPosition[DHD_MAX_DOF] = {0, 0, 0, 0, 0, 0, 0, 0};
    if (drdMoveTo(initialPosition, true, m_deviceID) < 0)
    {
        LOG(ERROR) << "Error: failed to align master with slave (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }

    /* This is very important */
    if (drdStop(true, m_deviceID) < 0)
    {
        LOG(ERROR) << "Error: failed to stop robotic regulation (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }

    // Enable button emulation on devices featuring a gripper.
    if (dhdHasActiveGripper(m_deviceID) && dhdEmulateButton(DHD_ON, m_deviceID) < 0)
    {
        LOG(ERROR) << "Error: failed to enable button emulation (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }

    if(dhdEnableForce(DHD_ON) < 0){
        LOG(ERROR) << "Error: Failed to set force mode (" << dhdErrorGetLastStr() << ")" ;
        return -1;
    }    
    LOG(INFO) << dhdGetSystemName() << " device detected." ;
    m_isOmegaOK.store(true);
    return 0;
}

void FdOmega::turnOffDevice()
{
    if(drdClose() != 0)
    {
        LOG(ERROR) << " Error: Failed to turn off haptic device!";
    }
    else
    {
        LOG(INFO) << "Successfull to turn off haptic device!";
    }
}

void FdOmega::startThread(){
    std::thread masterHandleThread = std::thread(&FdOmega::masterHandleThread, this);
    LOG(INFO) <<"Start master handle thread: " ;
    masterHandleThread.detach();
}

void FdOmega::masterHandleThread(){

    if(initDevice() != 0)
    {
        m_isOmegaOK.store(false);
        LOG(ERROR) << " Error: Failed to initialize haptic device!";
        return;
    }
    std::array<double,omegaDataNumPerSensor> omegaDataTmp;
    double hd_pos_x, hd_pos_y, hd_pos_z, hd_ori_theta1, hd_ori_theta2, hd_ori_theta3;
    bool loopStarted = false;
    while(!m_threadTerminated)
    {
        if (dhdSetForceAndTorqueAndGripperForce(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, m_deviceID) < 0)
        {
            LOG(ERROR) << "Error: Failed to set haptic force (" << dhdErrorGetLastStr() << ")" ;
            break;
        }
        // double *px, double *py, double *pz, double *oa, double *ob, double *og, char ID = -1
        dhdGetPositionAndOrientationDeg(&hd_pos_x, &hd_pos_y, &hd_pos_z, &hd_ori_theta1, &hd_ori_theta2, &hd_ori_theta3);
        omegaDataTmp[0] = - hd_pos_x;// Unit M
        omegaDataTmp[1] = - hd_pos_y;// Unit M
        omegaDataTmp[2] = hd_pos_z;// Unit M
        omegaDataTmp[3] = - hd_ori_theta1;
        omegaDataTmp[4] = - hd_ori_theta2;
        omegaDataTmp[5] = hd_ori_theta3;

        m_omegaPNOData.store(omegaDataTmp, std::memory_order_release);
//        LOG(INFO)<<"omegaDataTmp[2]: " << omegaDataTmp;
        if (!loopStarted)
        {
//            promiseOmega.set_value(true);
            loopStarted = true;
        }
    }
    
    if (drdClose() < 0)
    {
        LOG(ERROR) << "Error: Failed to close the connection (" << dhdErrorGetLastStr() << ")" ;
        dhdSleep(2.0);
        return ;
    }
    LOG(INFO) << "Omega thread ends!";
}
