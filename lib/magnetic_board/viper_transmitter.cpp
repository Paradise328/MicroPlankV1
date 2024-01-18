#include "viper_transmitter.h"

VIPER_Transmitter::VIPER_Transmitter()
{

}


VIPER_Transmitter::VIPER_Transmitter(QString ip,quint16 port):Peripheral_Device(ip,port)
{
    this->type=DEV_VIPER_TRANSMITTER;

    this->Qhash_Cmd_Classify.insert("GETSINGLE",VIPER_TRANSMITTER_GET_SINGLE_DATA);
}


void VIPER_Transmitter::VCMD(QString cmd,int arg1,int arg2,int arg3)
{

    cmd=cmd.toUpper();
    QStringList slist=cmd.split("=");

    COMMU_FRAME cf;
    memset(&cf,0,sizeof (COMMU_FRAME));
    uint8_t action1[2]={0};
    int actionLen=2;
    action1[0]=this->Qhash_Cmd_Classify[slist[0]];

    switch (this->Qhash_Cmd_Classify[slist[0]])
    {

        case VIPER_TRANSMITTER_GET_SINGLE_DATA:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_READ_DATA;
//            action1[1]=this->Qhash_Liftmotion_Classify[slist[1]];
            break;
        }
    }

    if(arg1!=-1)actionLen++;
    if(arg2!=-1)actionLen++;
    if(arg3!=-1)actionLen++;

    uint8_t payloadsize=3+actionLen;
    uint8_t framelen=17+payloadsize;

    cf.preamble=PREAMBLE;
    cf.ID=this->GetRandNum();
    cf.size=framelen;
    cf.sender=DEV_HOST;

    cf.recever=this->type;

    cf.payload.size=payloadsize;
    cf.payload.argscounter=actionLen;
    unsigned int j=0;
    for(j=0;j<sizeof(action1);j++)
    {cf.payload.args[j]=action1[j];}

    if(arg1!=-1){cf.payload.args[j]=(uint8_t)arg1;j++;}
    if(arg2!=-1){cf.payload.args[j]=(uint8_t)arg2;j++;}
    if(arg3!=-1){cf.payload.args[j]=(uint8_t)arg3;j++;}

    Send_Frame(cf);
}