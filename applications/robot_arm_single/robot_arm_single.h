#ifndef ROBOT_ARM_SINGLE_H
#define ROBOT_ARM_SINGLE_H
#include <QThread>

//#include "robotarm.h"
#include "../../lib/robot_arm/robotarm.h"
class main_thread: public QThread
{
    Q_OBJECT

public:
    main_thread(){}

signals:
    void setArmEnableButton(int i);
protected:
    void run();

};
#endif // ROBOT_ARM_SINGLE_H
