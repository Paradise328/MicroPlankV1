#include "PerformanceTestProgram.h"
#include <unistd.h>

PerformanceTestProgram::PerformanceTestProgram()
{
    LOG(INFO)<<"construct the program";
    rightArm = new GalilSlave();

}

PerformanceTestProgram::~PerformanceTestProgram(){}


bool PerformanceTestProgram::init(){
    LOG(INFO)<<"initialize the program";
    rightArm -> open(RIGHT_ARM_ADDRESS);

    return 1;
}
bool PerformanceTestProgram::run(){
   // LOG(INFO)<<"running the program";   
    rightArm -> setServo("ACE");
    usleep(1000*500);

    rightArm -> motorOff("ACE");
    usleep(1000*500);
    
    return 1;
}

void PerformanceTestProgram::printMainMenu(int t_needCalibration){
    system("clear");
    std::cout << "          ========================================" << std::endl;
    std::cout << "          |      MIKROELENA FUNCTION MENU        |" << std::endl;
    std::cout << "          ----------------------------------------" << std::endl;
    std::cout << "          |  1: left Instrument Calibration      |" << std::endl;
    std::cout << "          |  2: right Instrument Calibration     |" << std::endl;
    std::cout << "          |  3: left Gimal Calibration           |" << std::endl;
    std::cout << "          |  4: right Gimbal Calibration         |" << std::endl;
    std::cout << "          |  5: XXXXXXXXXXXXXXXXXXXXXX           |" << std::endl;
    std::cout << "          |  6: Turn off Motors                  |" << std::endl;
    std::cout << "          |  7: XXXXXXXXXXXXXXXXXXXXXX           |" << std::endl;
    std::cout << "          |  8: XXXXXXXXXXXXXXXXXXXXXX           |" << std::endl;
    std::cout << "          |  9: XXXXXXXXXXXXXXXXXXXXXX           |" << std::endl;
    std::cout << "          |  0: Exit the Program                 |" << std::endl;
    std::cout << "          ========================================" << std::endl;

    //if calibration_condition variable is not 1, need calibration first before using fk and ik
    if(t_needCalibration==0){
        std::cout << "          |     !!NEED CALIBRATION FIRST!!       |"<<std::endl;
        std::cout << "          |     !!!PRESS 1 TO CALIBRATE!!!       |"<<std::endl;
        std::cout << "          |======================================|" << std::endl;
    }
    std::cout << "\nInput your option: " << std::endl;
}

