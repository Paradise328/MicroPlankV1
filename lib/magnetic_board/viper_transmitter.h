#ifndef VIPER_TRANSMITTER_H
#define VIPER_TRANSMITTER_H

#include "peripheral_device.h"

class VIPER_Transmitter : public Peripheral_Device
{
public:
    VIPER_Transmitter();
    VIPER_Transmitter(QString ip,quint16 port);
    void VCMD(QString cmd,int arg1=-1,int arg2=-1,int arg3=-1);


    QHash<QString,eViper_Transmitter_Actions> Qhash_Cmd_Classify;
};

#endif // VIPER_TRANSMITTER_H