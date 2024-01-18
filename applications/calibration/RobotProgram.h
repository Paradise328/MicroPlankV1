#ifndef ROBOTPROGRAM_H
#define ROBOTPROGRAM_H

#include <cstdlib>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <fstream>

#include "gclib.h"
#include "gclibo.h"
#include "GalilControl.h"

class RobotProgram
{
private:


public:
    RobotProgram();
    ~RobotProgram();

    //init the motors
    int InitProgram();

    //indicate the system status
    bool SystemStatus();

    // Exit the program
    void Exit();

    // Show the main menu
    void CallGalil();

    void PrintMainMenu(int need_calibration);
    void MainMenu();
    GReturn MainControl(GCon g_l,GCon g_r);
};

#endif // ROBOTPROGRAM_H
