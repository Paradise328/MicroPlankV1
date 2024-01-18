#ifndef LIFTING_H
#define LIFTING_H

#include "../../lib/peripheral_device/peripheral_device.h"
//#include <peripheral_device.h>
#include <QObject>

typedef enum eLiftingSta
{
    Lifting_STOP=0X00,
    Lifting_GOING_UP,
    Lifting_GOING_DOWN
}eLiftingSta;

typedef enum eLiftingSpeed
{
    Lifting_Slow=0X01,
    Lifting_Fast=0x02,
}eLiftingSpeed;

class Lifting : public Peripheral_Device
{
public:
    QHash<QString,eLiftingActions> Qhash_Cmd_Classify;
    QHash<QString,eLightColor> Qhash_LightColor_Classify;
    QHash<QString,eLightModel> Qhash_LightModel_Classify;
    QHash<QString,eLiftMotion> Qhash_Liftmotion_Classify;
    eLiftingSta LiftingSta;
    QTimer *SendTimer=nullptr;
    eLiftingSpeed LiftingSpeed=Lifting_Fast;

    Lifting();
    Lifting(QString ip,quint16 port);

//    void LCMD(QString cmd);
    void LCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);

    void SendTimer_Callback(void);

    void LiftingUp(eLiftingSpeed spd);
    void LiftingDown(eLiftingSpeed spd);
    void LiftingBrake(void);
};

#endif // LIFTING_H
