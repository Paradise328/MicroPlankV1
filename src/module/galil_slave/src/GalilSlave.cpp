#include "GalilSlave.h"

GalilSlave::GalilSlave(){
    LOG(INFO) <<"slave device initialized with galil";
}

int GalilSlave::open(const char* address){
    try{
        LOG(INFO) << "Connecting to IP: " << address;
        galil(GOpen(address, &g));
        //call galil control commands in a control menu
    }
    catch(GReturn gr){
        error(g, gr);
        return 0;
    }

    return 1;
}

int GalilSlave::setServo(char* axis){
    LOG(INFO)<<"Set Servo on: " << axis;
    sprintf(buf,  "SH %s", axis);
    galil(GCmd(g,buf));

    return 1;
}
int GalilSlave::motorOff(char* axis){
    LOG(INFO)<<"Motor off on: " << axis;
    sprintf(buf, "MO %s", axis);
    galil(GCmd(g,buf));

    return 1;
}
