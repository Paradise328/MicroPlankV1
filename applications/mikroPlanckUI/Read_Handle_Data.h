#ifndef READ_HANDLE_DATA_H
#define READ_HANDLE_DATA_H
#include <QCoreApplication>
#include <QApplication>
#include <iostream>
#include<QThread>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include <ratio>
#include <linux/input.h>
#include <math.h>
#include <stdlib.h>
#include <termio.h>
#include <QMutex>
#include <QQueue>
#include <QTime>
#include "../../lib/master_handle_data/Handle.h"
extern Posture ControlPos_Data;
extern SensorData ControlS_Data;
class ReadData:public QThread
{
public:
    ReadData();

    void Save_Data(void);
    bool ReadFirst=true;
    uint8_t keyvalold=0;
//    VIPER_Transmitter *viper_transmitter=nullptr;
    Handle  *Forcep;
    void return_Data(QByteArray qba);
    void run();
    void Data_output();
    QMutex save_data_lock;

//    double return_OA();

};

#endif // READ_HANDLE_DATA_H
