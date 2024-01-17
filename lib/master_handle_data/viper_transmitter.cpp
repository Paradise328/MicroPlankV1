#include "viper_transmitter.h"

VIPER_Transmitter::VIPER_Transmitter()
{
     connect(this,&VIPER_Transmitter::NewDataIn,this,&VIPER_Transmitter::return_Data);
}


VIPER_Transmitter::VIPER_Transmitter(QString ip,quint16 port):Peripheral_Device(ip,port)
{
    counter_i=0;
    Speed_pedal = 0;
    Enable_pedal_l=0;
    Enable_pedal_r=0;


    this->type=DEV_VIPER_TRANSMITTER;

    this->Qhash_Cmd_Classify.insert("GETSINGLE",VIPER_TRANSMITTER_GET_SINGLE_DATA);
    this->Qhash_Cmd_Classify.insert("RESETVIPER",VIPER_TRANSMITTER_RESET_VIPER);
    connect(this,&VIPER_Transmitter::NewDataIn,this,&VIPER_Transmitter::return_Data);
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
            break;
        }
        case VIPER_TRANSMITTER_RESET_VIPER:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        default:break;
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

void VIPER_Transmitter::Reset_Viper(void)
{
    this->VCMD("RESETVIPER");
}

void VIPER_Transmitter::return_Data(QByteArray qba)
{
//        QByteArray qba=viper_transmitter->DataRecin;
//        viper_transmitter->DataRecin.clear();
    COMMU_FRAME cftemp;
    uint32_t ORTemp=0;
    uint32_t HandleVAL1=0;
    uint32_t HandleVAL2=0;
    uint16_t ADCVAL1;
    uint16_t ADCVAL2;
    uint16_t Keyval1;
    uint16_t Keyval2;

    uint32_t IOVAL=0;
    eDepackReturn edr=this->Depack_Frame(qba,cftemp);


    if(edr==Depack_SUCCESS)
    {

        if(cftemp.payload.args[0]==Dev_Sta_OK)
        {
//            Angle_Sensor_mutex.lock();
            ORTemp=cftemp.payload.args[1];
            HandleVAL1|=ORTemp;
            ORTemp=cftemp.payload.args[2];
            HandleVAL1|=(ORTemp<<8);
            ORTemp=cftemp.payload.args[3];
            HandleVAL1|=(ORTemp<<16);
            ORTemp=cftemp.payload.args[4];
            HandleVAL1|=(ORTemp<<24);

            ORTemp=cftemp.payload.args[5];
            HandleVAL2|=ORTemp;
            ORTemp=cftemp.payload.args[6];
            HandleVAL2|=(ORTemp<<8);
            ORTemp=cftemp.payload.args[7];
            HandleVAL2|=(ORTemp<<16);
            ORTemp=cftemp.payload.args[8];
            HandleVAL2|=(ORTemp<<24);

            IOVAL=cftemp.payload.args[9];
            ADCVAL1=HandleVAL2;
            Keyval1=HandleVAL2>>16;
            ADCVAL2=HandleVAL1;
            Keyval2=HandleVAL1>>16;
            if(counter_i==10)
            {
                org_l=ADCVAL1*360.0/4096.0;
                org_r=ADCVAL2*360.0/4096.0;
            }

            cur_l=ADCVAL1*360.0/4096.0;
            cur_r=ADCVAL2*360.0/4096.0;
            Speed_pedal = IOVAL;
            Enable_pedal_l = Keyval1;
            Enable_pedal_r = Keyval2;
//            printf("T_pedal=%d\n",T_pedal);
//            Angle_Sensor_mutex.unlock();

//            system("clear");
//            qDebug()<<"ADCVAL1="<<QString::number(ADCVAL1,10)
//            <<"\nKeyval1="<<QString::number(Keyval1,16)
//            <<"\nADCVAL2="<<QString::number(ADCVAL2,10)
//            <<"\nKeyval2="<<QString::number(Keyval2,16)
//            <<"\nIOVAL="<<QString::number(IOVAL,16);


//            time_end=std::chrono::steady_clock::now();
//           if( std::chrono::duration_cast<std::chrono::microseconds>(time_end-time_start).count()>3000)
//           {
//           }
//            qDebug()<<"read data gap time"<<std::chrono::duration_cast<std::chrono::microseconds>(time_end-time_start).count()<<"us";

        }
        else if(cftemp.payload.args[0]==Dev_Sta_LEFTHANDLE_ERROR)
        {
             qDebug()<<"LEFTHANDLE_ERROR!";

        }
        else if(cftemp.payload.args[0]==Dev_Sta_RIGHTHANDLE_ERROR)
        {
             qDebug()<<"RIGHTHANDLE_ERROR!";

        }
        else
        {

            qDebug()<<"UNKNOW_ERROR!";
        }
        qba.clear();

    }
    else
    {
        qDebug()<<"Depack_Frame Fail";

    }
    if(counter_i<12){counter_i++;}
}
