#ifndef MAIN_H
#define MAIN_H
#include <QThread>
#include <QApplication>
#include <chrono>
#include <ratio>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <linux/input.h>
#include <QMetaType>
#include <QVector>
#include <unistd.h>
#include <queue>
#include <deque>
//#include "robotarm.h"
#include "../../lib/GalilControl/GalilControl.h"
#include "../../lib/robot_arm/robotarm.h"
#include "../../lib/peripheral_device/CommunicationProtol.h"
#include "../../lib/peripheral_device/peripheral_device.h"
#include "Read_Handle_Data.h"
#include "Forcep_control_thread.h"
#include "../../lib/master_handle_data/Handle.h"

#define UIINTERFACE_USED 1

Q_DECLARE_METATYPE(QVector<int>);
//GCon g_l,g_r;
#define SteadyDelay(time_ms)\
{\
    auto Delay_start = std::chrono::steady_clock::now();\
    auto Delay_end = Delay_start;\
    std::chrono::duration<double> Delay_time;\
      do{\
           Delay_end = std::chrono::steady_clock::now();\
           Delay_time =Delay_end-Delay_start;\
           if((Delay_time.count()*1000)>=time_ms)\
           {\
               break;\
           }\
            QCoreApplication::processEvents(QEventLoop::AllEvents,2);\
      }while(1);\
}


class robot_arm_thread: public QThread
{
    Q_OBJECT

public:
    robot_arm_thread()
    {
<<<<<<< HEAD
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QVector<int>>("QVector<int>&");
=======
        qRegisterMetaType<QVector<int>>("QVector<int>");
        qRegisterMetaType<QVector<int>>("QVector<int>&");
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    }

signals:
    void setArmEnableButton(int i);
    void setArmStatusMode(QVector<int> sm);
protected:
    void run();

};
#endif // MAIN_H
