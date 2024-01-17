#include "lightboard.h"

LightBoard::LightBoard()
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint8_t>("uint8_t&");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t&");
}


LightBoard::LightBoard(QString ip,quint16 port):Peripheral_Device(ip,port)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint8_t>("uint8_t&");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t&");


    this->Qhash_Cmd_Classify.insert("LSC",LIGHTBOARD_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LSM",LIGHTBOARD_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("LRD",LIGHTBOARD_ACTIONS_Read_Data);
    this->Qhash_Cmd_Classify.insert("LFI",LIGHTBOARD_ACTIONS_Final_Inc);

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

    connect(this,&LightBoard::NewDataIn,this,&LightBoard::DataIn);
}



void LightBoard::LCMD(QString cmd,eFinalSide Finalside,int arg2,int arg3)
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
            action1[1]=Finalside;
            action1[2]=this->Qhash_ReadDataTyp_Classify[slist[1]];
            actionLen=3;
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
    unsigned int j=0;
    for(j=0;j<sizeof(action1);j++)
    {cf.payload.args[j]=action1[j];}

    if(arg2!=-1){cf.payload.args[j]=(uint8_t)arg2;j++;}
    if(arg3!=-1){cf.payload.args[j]=(uint8_t)arg3;j++;}

    Send_Frame(cf);
}


void LightBoard::DataIn(QByteArray data)
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

        if(cftemp.payload.type==CMD_UPLOAD_DATA)
        {
            switch (cftemp.payload.args[0])
            {
                case FianlProductDate:
                case FianlProductBat:
                case FianlType:
                case FianlConnectSta:
                case FianlCounter:
                {
                    FDRTemp.DataType=(eReadDataType)cftemp.payload.args[0];
                    FDRTemp.FinalSide=(eFinalSide)cftemp.payload.args[1];
                    uint32_t datatemp=cftemp.payload.args[2];
                    datatemp<<=8;
                    datatemp|=cftemp.payload.args[3];
                    datatemp<<=8;
                    datatemp|=cftemp.payload.args[4];
                    FDRTemp.data=datatemp;
                    emit FinalDataUpdate(FDRTemp);
                    break;
                }
                case IOVal:
                {
                    IOValdat=cftemp.payload.args[1];
                    if(IOValdat!=IOValdatOld) { emit IOValUpdate(IOValdat);}
                    IOValdatOld=IOValdat;
                    break;
                }
                default:break;
            }
        }
    }
}


