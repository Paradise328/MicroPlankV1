#include <fstream>
#include <iostream>
#include <math.h>
#include <future>
#include <linux/input.h>
#include "microPlank.h"

INITIALIZE_EASYLOGGINGPP
#define ELPP_THREAD_SAFE
#define ELPP_QT_LOGGING

int main(){

    std::string configFilePath = "/home/chenxinbo/Desktop/yunxiao/MikroPlanckV1/RobotData.toml";
    MicroPlank MicroPlank(MasterConsoleType::Omega,configFilePath);
    MicroPlank.startSystem();

    return 1;
}
