#include "lightboardintrolley.h"

LightBoardInTrolley::LightBoardInTrolley(MessageQueue&  messagePool):m_messagePool(messagePool)
{

}

LightBoardInTrolley::LightBoardInTrolley(QString ip,quint16 port,MessageQueue&  messagePool):Peripheral_Device(ip,port),m_messagePool(messagePool)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint8_t>("uint8_t&");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t&");


    this->Qhash_Cmd_Classify.insert("LSC",LIGHTBOARD_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LSM",LIGHTBOARD_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("LRD",LIGHTBOARD_ACTIONS_Read_Data);
    this->Qhash_Cmd_Classify.insert("LFI",LIGHTBOARD_ACTIONS_Final_Inc);

    this-> Qhash_ReadDataTyp_Classify.insert("FAI",FinalAllInformation);
    this-> Qhash_ReadDataTyp_Classify.insert("FPD",FianlProductDate);
    this-> Qhash_ReadDataTyp_Classify.insert("FPB",FianlProductBat);
    this-> Qhash_ReadDataTyp_Classify.insert("FT",FianlType);
    this-> Qhash_ReadDataTyp_Classify.insert("FCS",FianlConnectSta);
    this-> Qhash_ReadDataTyp_Classify.insert("FCT",FianlCounter);
    this-> Qhash_ReadDataTyp_Classify.insert("IOV",IOVal);

    this->Qhash_LightColor_Classify.insert("RED",LIGHT_COLOR_RED);
    this->Qhash_LightColor_Classify.insert("GREEN",LIGHT_COLOR_GREEN);
    this->Qhash_LightColor_Classify.insert("YELLOW",LIGHT_COLOR_YELLOW);

    this->Qhash_LightModel_Classify.insert("ON",LIGHT_MODEL_ON);
    this->Qhash_LightModel_Classify.insert("BLINK",LIGHT_MODEL_BLINK);
    this->Qhash_LightModel_Classify.insert("OFF",LIGHT_MODEL_OFF);

    this->type=DEV_LIGHT;

    m_readFinalStep.store(ReadFianlInfoStep::NoneAction);
    initStatus();
    connect(this,&LightBoardInTrolley::NewDataIn,this,&LightBoardInTrolley::DataIn);
}

void LightBoardInTrolley::initStatus()
{
    m_LightBoarInTrolleyStatus = LightBoardInTrolleyStatus::WARNING;
}

void LightBoardInTrolley::changeStatus(const LightBoardInTrolleyStatus& status)
{
    if(status == LightBoardInTrolleyStatus::WARNING)
    {
        LCMD("LSC=green");
        LCMD("LSM=blink");

    }
    if(status == LightBoardInTrolleyStatus::NORMAL)
    {
        LCMD("LSC=green");
        LCMD("LSM=blink");
    }
    if(status == LightBoardInTrolleyStatus::ERROR)
    {
        LCMD("LSC=red");
        LCMD("LSM=blink");
    }
}


LightBoardInTrolleyStatus LightBoardInTrolley::returnLightBoardStatus()
{
    return m_LightBoarInTrolleyStatus;
}

void LightBoardInTrolley::LCMD(QString cmd,eFinalSide Finalside,int arg2,int arg3)
{
    cmd=cmd.toUpper();
    QStringList slist=cmd.split("=");

    uint8_t action1[10]={0};
    int actionLen=0;
     COMMU_FRAME cf;
     memset(&cf,0,sizeof (COMMU_FRAME));
    action1[0]=this->Qhash_Cmd_Classify[slist[0]];

    switch (this->Qhash_Cmd_Classify[slist[0]])
    {
        case LIGHTBOARD_ACTIONS_SET_COLOR:
        {
            action1[1]=this->Qhash_LightColor_Classify[slist[1]];
            actionLen=2;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        case LIGHTBOARD_ACTIONS_SET_MODEL:
        {
             action1[1]=this->Qhash_LightModel_Classify[slist[1]];
             actionLen=2;
             cf.payload.type=CMD_CTRL_WRITE;
             break;
        }
        case LIGHTBOARD_ACTIONS_Read_Data:
        {
            action1[1]=this->Qhash_ReadDataTyp_Classify[slist[1]];//Finalside;
            actionLen=2;
            cf.payload.type=CMD_READ_DATA;
            break;
        }
        case LIGHTBOARD_ACTIONS_Final_Inc:
        {
            action1[1]=Finalside;
            actionLen=2;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        default:return;
    }


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
    int j=0;
    for(j=0;j<actionLen;j++)
    {
        cf.payload.args[j]=action1[j];
    }

    if(arg2!=-1){cf.payload.args[j]=(uint8_t)arg2;j++;}
    if(arg3!=-1){cf.payload.args[j]=(uint8_t)arg3;j++;}

    Send_Frame(cf);
}


void LightBoardInTrolley::DataIn(QByteArray data)
{
    static uint8_t IOValdatOld=0;
    uint8_t IOValdat=0;
    FinalDataRead_t FDRTemp;
    COMMU_FRAME cftemp;
    if(QString(data).contains("LightBoard"))
    {
        qDebug()<<QString(data);
        return;
    }

    eDepackReturn edr=this->Depack_Frame(data,cftemp);

    if(edr==Depack_SUCCESS)
    {
        QString argBufString="";
        if(cftemp.payload.type == CMD_UPLOAD_DATA)
        {
            switch (cftemp.payload.args[0])
            {
                case FinalAllInformation:argBufString+="FAI:";break;
                case FianlProductDate:argBufString+="PD:";break;
                case FianlProductBat:argBufString+="PB:";break;
                case FianlType:argBufString+="FT:";break;
                case FianlConnectSta:argBufString+="CS:";break;
                case FianlCounter:argBufString+="FC:";break;
                case IOVal:
                {
                    argBufString+="IO:";
                    break;
                }
                default:break;
            }
            if(argBufString=="IO:")
            {
                IOValdat=cftemp.payload.args[1];
                if(IOValdat!=IOValdatOld){
                    argBufString+=QString::number(IOValdat);
                }
                IOValdatOld=IOValdat;
            }
        }
    }
}

