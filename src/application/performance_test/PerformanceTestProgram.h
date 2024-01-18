#ifndef PERFORMANCETESTPROGRAM_H
#define PERFORMANCETESTPROGRAM_H

#include <iostream>
#include <fstream>
#include "easylogging++.h"
#include "SlaveDevice.h"
#include "GalilSlave.h"


class PerformanceTestProgram
{
private:
    const char* RIGHT_ARM_ADDRESS = "192.168.42.30";
    
    SlaveDevice *rightArm = nullptr;

public:
    PerformanceTestProgram();
    ~PerformanceTestProgram();

    // init the program
    bool init();
    // init the programme
    bool run();

    void printMainMenu(int t_needCalibration);
};

#endif 
