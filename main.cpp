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

    MicroPlank MicroPlank(MasterConsoleType::Viper);
    MicroPlank.startSystem();

    return 1;
}
