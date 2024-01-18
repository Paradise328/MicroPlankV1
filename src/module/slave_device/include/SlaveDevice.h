/**
 *  Control the slave device with GALIL dmc-4080 controller
 *  
 *  this interface is developed on Ubuntu 18.04 LTS
 * 
 *  Copyright (c) Hangzhou Dessight Corp.
 * 
*/

#ifndef SLAVEDEVICE_H
#define SLAVEDEVICE_H

#include <iostream>
#include "easylogging++.h"


class SlaveDevice
{
   
public:
    SlaveDevice();
    
    //send command
    int open();
    virtual int open(const char* add){};
    int close();
    int init();
    virtual int reset()=0;
    virtual int setServo(char* )=0;
    virtual int motorOff(char* )=0;
    int motorStop();
    int goPositionRelative();
    int goPositionAbsolute();
    int calibrate();
    int jog();
    
    // get return from controller
    int readEncoder();
    int readDigitalInput();
    int readAnalogInput();

    // combined functions
    int GIMBAL_FindEdge();
    int GIMBAL_Calibration();
    int INSTRUMENT_FindEdge();
    int INSTRUMENT_Calibration();



private:
    
};

#endif // SLAVEDEVICE_H
