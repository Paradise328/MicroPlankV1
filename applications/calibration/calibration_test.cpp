#include <QCoreApplication>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include <ratio>
#include <linux/input.h>
#include <math.h>
#include <vector>
#include "RobotProgram.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //construct the program
 
    RobotProgram rp;
    

    rp.InitProgram();
    while(1)
    {
        //print the program menu
        rp.CallGalil();
    }

    printf("hello galil\n");
    return a.exec();
}
