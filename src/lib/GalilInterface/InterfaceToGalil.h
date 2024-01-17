#ifndef INTERFACETOGALIL_H
#define INTERFACETOGALIL_H

#define _CRT_SECURE_NO_WARNINGS         //use traditional C calls like sprintf()
#define GALIL_EXAMPLE_OK 0              //return code for correct code execution
#define GALIL_EXAMPLE_ERROR -100        //return code for error in example code

#include <tuple>
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
#include <array>
#include <vector>
#include <linux/input.h>

#include "gclib.h"
#include "gclibo.h"
#include "GalilException.h"
#include "easylogging++.h"
#include "utils.h"
#include <mutex>
//#include "../app/slave_device/SlaveDevice.h"



#define T_ERROR       ((int)0)
#define T_NOERROR     ((int)1)

#define EncoderTestSuccess 1
#define EncoderError -11
#define EncoderTestbyAxisFail -1
#define EncoderTestbyAxisSuccess 1
#define CONNECTION_FAILED 0
#define CONNECTION_SUCCEED 1

constexpr int portNumber = 8;
constexpr int HANDLE_SIGNAL = 8;
constexpr int ROBOT_SIGNAL = 9;
constexpr int FOOT_SWITCH = 10;
constexpr int ROBOT_POWER = 12;
constexpr int DB44 = 14;
constexpr int targetNumber = 2;

// 
inline void galil(GReturn rc){
    if (rc != G_NO_ERROR){
        char buf[G_SMALL_BUFFER];
        GError(rc, buf, G_SMALL_BUFFER); //Get Error Information
        LOG(ERROR) << "Error code: " << rc << ", Error info: " << buf;
        throw ExcpGalilGeneral();
    }
}

class InterfaceToGalil{
public:
    friend class SlaveDevice;
//    friend class UIinterface;

    InterfaceToGalil(){};
    explicit InterfaceToGalil(GCon t_gcon): m_gcon(t_gcon) {};
    

    std::array<int, portNumber> readEncoderValueDual() const;

    int connectionTest() const;
    int encoderTestbyAxis(const char& axis) const;
    int encoderTest() const;

    GReturn motorOff() const;
    GReturn motorOff(const char& axis) const;
    GReturn motorStop() const;
    GReturn motorStop(const char& axis) const;

    GReturn goPosition(const char& axis, const int& t_encoder) const;
    GReturn goPosition(const int& t_encoderA, const int& t_encoderB, const int& t_encoderC) const;

    GReturn jog(const char& t_axis1, const int& t_speed1) const;
    GReturn jog(const char& t_axis1, const int& t_speed1, 
                const char& t_axis2, const int& t_speed2) const;
    GReturn jog(const char& t_axis1, const int& t_speed1, 
                const char& t_axis2, const int& t_speed2, 
                const char& t_axis3, const int& t_speed3) const;
    GReturn jog(const char& t_axis1, const int& t_speed1, 
                const char& t_axis2, const int& t_speed2, 
                const char& t_axis3, const int& t_speed3,
                const char& t_axis4, const int& t_speed4) const;

    GReturn positionRelative(const char& axis, const int& t_pos) const;



    //used in Microsurgical Robot
    GReturn goTargetSpeedPosition(std::array<std::array<int,portNumber>, 3> targetPosition) const;
    // read encodervalue
    std::array<int, portNumber> readEncoderValue() const;
    // tell position+tell Encoder
    std::array<int, portNumber> readMixEncoderValue() const;
    std::array<int, portNumber> readEncoderValue(GDataRecord &t_record) const;
    // read digital input from 4080
    std::array<int, 2 * portNumber> readDigitialInput() const;
    std::array<int, portNumber> readDigitalInput(GDataRecord &t_record) const;
    // read analog input from 4080
    std::array<double, portNumber> readAnalogInput() const;
    // read Axis input from 4080
    std::array<int, portNumber> readSwitches(GDataRecord &t_record) const;

    // read axis info
    // 0:Position Latch has occurred        1:Home switch status    2:Reverse Limit switch inactive
    // 3:Forward Limit switch inactive      4:    Reserved (0)      5:    Motor off
    // 6:Position error exceeds error limit 7:    Axis in motion
    std::array<int, portNumber> readAxisInfo() const;

    void        initGalil(char side);
    void        resetGalil(char side);

    GReturn     GIMBAL_FindEdge(char side);
    GReturn     INSTRUMENT_FindEdge(char side);
    int         INSTRUMENT_Calibration(char side);
    GReturn     INSTRUMENT_Calibration2(char side);
    int         GIMBAL_Calibration(char side);

    void        setServo() const;


    GCon        return_m_gcon() const {return m_gcon;}

    GReturn     readRecordData(GDataRecord &t_record) const;

private:
    GCon            m_gcon; // initialize the galil
    GCon            m_gconDataRecord = 0;
    mutable         std::mutex m_mtx;
    mutable         std::array <int,8> m_switchesDataPreL = {0};
    mutable         std::array <int,8> m_switchesDataPreR = {0};

};

#endif