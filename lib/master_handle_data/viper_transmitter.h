#ifndef VIPER_TRANSMITTER_H
#define VIPER_TRANSMITTER_H

#include "../../lib/peripheral_device/peripheral_device.h"

class VIPER_Transmitter : public Peripheral_Device
{
public:
    VIPER_Transmitter();
    VIPER_Transmitter(QString ip,quint16 port);
    void VCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);
    void Reset_Viper(void);
    void return_Data(QByteArray qba);
    QHash<QString,eViper_Transmitter_Actions> Qhash_Cmd_Classify;
    float org_l,org_r,cur_l,cur_r;
    int Speed_pedal,counter_i,Enable_pedal_l,Enable_pedal_r;

};

#endif // VIPER_TRANSMITTER_H
