#include "RobotProgram.h"
int NUM_OPTIONS = 9;
int calibration_condition;
RobotProgram::RobotProgram()
{
    printf("\n");
    printf("Initializing the system.\n\n");
}

RobotProgram::~RobotProgram(){}

int RobotProgram::InitProgram(){

    printf("The Program is initialized here\n");

    /* TODO:INIT MOTOR, CHECK ENCODER */
    return 1;
}
bool RobotProgram::SystemStatus(){
    return 1;
}

void RobotProgram::CallGalil(){
    GReturn rc = GALIL_EXAMPLE_OK;
    char buf[G_SMALL_BUFFER];

    //var used to refer to a unique connection. A valid connection is nonzero.
    GCon g_l = 0;
    GCon g_r = 0;
    while(1){
        try{
            std::cout << "Connecting to IP: "<< "192.168.42.30" << std::endl;
            galil(GOpen("192.168.42.40", &g_r));
            galil(GOpen("192.168.42.30", &g_l));
            //call galil control commands in a control menu
            rc = MainControl(g_l,g_r);
        }
        catch(GReturn gr){
            error(g_l, gr);
            error(g_r, gr);
            return ;
        }
    }
}

void RobotProgram::PrintMainMenu(int calibration_condition){
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
    if(calibration_condition==0){
        std::cout << "          |     !!NEED CALIBRATION FIRST!!       |"<<std::endl;
        std::cout << "          |     !!!PRESS 1 TO CALIBRATE!!!       |"<<std::endl;
        std::cout << "          ========================================" << std::endl;
    }
    std::cout << "\nInput your option: " << std::endl;
}
GReturn RobotProgram::MainControl(GCon g_l,GCon g_r){
    std::cout << "Press ENTER to continue......\n";
    std::cin.ignore(5, '\n');
    int abrt;
    int calibration_count = 0;
    int option;
    system("clear");

    // print main menu
    PrintMainMenu(calibration_condition);

    // read a selected option
    std::cin >> option;

    std::cout << "option: " << option << std::endl;

    // Check if the input option is valid
    while (std::cin.fail() || option > NUM_OPTIONS)
    {
        std::cout << "False input" << std::endl;
        std::cout << "Give your option again" << std::endl;
        std::cin.clear();

        // Extracts characters from the input sequence and discards them, until either n characters have been extracted, or one compares equal to delim.
        std::cin.ignore(5, '\n');

        std::cout << "Option in range [0, " << NUM_OPTIONS <<"]" << std::endl;
        std::cin >> option;
    }

    switch(option)
    {
        case 1:
            std::cout << "MODE 1: INSTRUMENT_left Calibration/Initial Position; " << std::endl;
            INSTRUMENT_Calibration(g_l,'l');
            break;
        case 2:
            std::cout << "MODE 2: INSTRUMENT_right Calibration/Initial Position; " << std::endl;
            INSTRUMENT_Calibration(g_r,'r');
            break;
        case 3:
            std::cout << "MODE 3: GIMBAL_left Calibration/Initial Position; " << std::endl;
            GIMBAL_Calibration(g_l,'l');

            break;
        case 4:
            std::cout << "MODE 4: GIMBAL_right Calibration/Initial Position; " << std::endl;
            GIMBAL_Calibration(g_r,'r');
            
            break;
        case 5:
            std::cout << "MODE 5: Final Control Mode; " << std::endl;
          //  INSTRUMENT_GetInfo(g)

            break;
        case 6:
            std::cout << "MODE 6: Turn off Motors; " << std::endl;
            MotorOff(g_l);
            MotorOff(g_r);
            break;
        case 7:
            std::cout << "MODE 7: Pulse Test Mode; " << std::endl;
            break;
        case 8:
            std::cout << "MODE 8: Draw a Picture; " << std::endl;
            //InverseKinematicsShow(robot->g);
            break;
        case 9:
            std::cout << "case 9: Repeative Calibration Test Mode; " << std::endl;
        
            break;
        case 0:
            system("clear");
            std::cout << " Exit! " << std::endl;
            exit(1);
            break;
    }
    return GALIL_EXAMPLE_OK;
}
