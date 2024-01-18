#ifndef GALILCONTROL_H
#define GALILCONTROL_H

#define _CRT_SECURE_NO_WARNINGS         //use traditional C calls like sprintf()
#define GALIL_EXAMPLE_OK 0              //return code for correct code execution
#define GALIL_EXAMPLE_ERROR -100        //return code for error in example code

#include <vector>
#include <tuple>
#include <vector>
#include <memory>
#include <string>
#include <math.h>
#include <iostream>
#include <termios.h>
#include <iomanip>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include <ratio>
#include <linux/input.h>

#include "gclib.h"
#include "gclibo.h"

class Robot;
extern std::unique_ptr<Robot> robot;

inline void galil(GReturn rc)
{
    if (rc != G_NO_ERROR)
        throw rc;
}

//! An example of error handling and debugging information.
inline void error(GCon g, GReturn rc)
{
    char buf[G_SMALL_BUFFER];
    GError(rc, buf, G_SMALL_BUFFER); //Get Error Information
    std::cout <<buf<<"\n";

    if (g)
    {
        GSize size = sizeof(buf);
        GUtility(g, G_UTIL_ERROR_CONTEXT, buf, &size);

        if (buf[0]){
            printf(buf);
            printf("\n"); //further context
        }

        if ((rc == G_BAD_RESPONSE_QUESTION_MARK)
            && (GCommand(g, "TC1", buf, G_SMALL_BUFFER, 0) == G_NO_ERROR))
        {
            printf(buf);
            printf("\n"); //Error code from controller
        }
    }
}

//SOME FUNCTIONS FOR THE SYSTEM
int scanKeyboard();
double *tellIN(GCon g);
double *tellIN2(GCon g);
double *tellAN(GCon g);
double *tellPos(GCon g);

//DEFINE CONTROL MODES
void TmpTest(GCon g);
void ForwardKinematics(GCon g);
void InverseKinematics(GCon g);
void FinalControl(GCon g);
void PulseTest(GCon g);
void InverseKinematicsShow(GCon g);

int INSTRUMENT_Calibration(GCon g,char);
int GIMBAL_Calibration(GCon g,char);
int EncoderTest(GCon g);
int ConnectionTest(GCon g);
int INSTRUMENT_GetInfo(GCon g);

//MOVEMENT FUNCTIONS FOR 4080
GReturn GoPosition(GCon g, int enc1,int enc2,int enc3);
GReturn PositionRelative(GCon g, char axis, int pos);
GReturn SetDefaultSpeed(GCon g);
GReturn Jog(GCon g, char axis, int speed);
GReturn Jog(GCon g, char axis1, int speed1, char axis2, int speed2, char axis3, int speed3);
GReturn Jog(GCon g, char axis1, int speed1, char axis2, int speed2, char axis3, int speed3, char axis4, int speed4);
GReturn InitMotor(GCon g);
GReturn FindEdge(GCon g);
GReturn MotorOff(GCon g, char axis);
GReturn MotorOff(GCon g);
GReturn Galil_control_l(GCon g, int target_l[]);
GReturn Galil_control_r(GCon g, int target_r[]);


//Init Galil
void initGali(GCon g);

#endif // GALILCONTROL_H
