#include <iostream>

#include "easylogging++.h"
#include "PerformanceTestProgram.h"
#include "SlaveDevice.h"
#include "GalilSlave.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    PerformanceTestProgram pt;
    pt.init();

    
    
    while(1)
    {
        pt.run();
    }

    return 1;
}
