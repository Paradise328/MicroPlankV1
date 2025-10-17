#include "lightboard.h"
#include "Modules/SystemUtilsModule/SystemUtils.h"

LightBoard::LightBoard(MessageQueue&  messagePool):m_messagePool(messagePool)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint8_t>("uint8_t&");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t&");

}

LightBoard::LightBoard(QString ip,quint16 port,MessageQueue&  messagePool):Peripheral_Device(ip,port),m_messagePool(messagePool)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint8_t>("uint8_t&");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t");
    qRegisterMetaType<FinalDataRead_t>("FinalDataRead_t&");


    this->Qhash_Cmd_Classify.insert("LSC",LIGHTBOARD_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LSM",LIGHTBOARD_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("LRD",LIGHTBOARD_ACTIONS_Read_Data);
    this->Qhash_Cmd_Classify.insert("LFI",LIGHTBOARD_ACTIONS_Final_Inc);
    this->Qhash_Cmd_Classify.insert("LRW",LIGHTBOARD_ACTIONS_RW);

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
    m_InstrumentStatusBoth.initAll();
//    m_InstrumentStatusBoth.load().initAll();

    connect(this,&LightBoard::NewDataIn,this,&LightBoard::DataIn);
}

void LightBoard::askMyInstrumentStatus()
{
    this->LCMD("LRW",eFinalSideUNSED, m_LightBoardColor, m_LightBoardModel, m_UpdateCounter_L, m_UpdateCounter_R);
    m_UpdateCounter_L=0;
    m_UpdateCounter_R=0;
}

void LightBoard::updateCounter(const char& side)
{
    if(side == 'l')
    {
        m_UpdateCounter_L=1;
    }
    if(side == 'r')
    {
        m_UpdateCounter_R=1;
    }
}

void LightBoard::setMyInstrumentStatus(const uint8_t& instrumentType, const u_int16_t& usedCounter, const char& side)
{
    if(side == 'l')
    {
        m_InstrumentStatusBoth.connectionStatus_L = "true";
        if(instrumentType == size_4mm)
        {
            m_InstrumentStatusBoth.type_L = "CZQ";
            m_InstrumentStatusBoth.size_L = "4MM";
        }
        if(instrumentType == size_8mm)
        {
            m_InstrumentStatusBoth.type_L = "CZQ";
            m_InstrumentStatusBoth.size_L = "8MM";
        }
        m_InstrumentStatusBoth.ID_L = 1;
        m_InstrumentStatusBoth.usedTime_L = usedCounter;
    }
    if(side == 'r')
    {
        m_InstrumentStatusBoth.connectionStatus_R = "true";
        if(instrumentType == size_4mm)
        {
            m_InstrumentStatusBoth.type_R = "CZQ";
            m_InstrumentStatusBoth.size_R = "4MM";
        }
        if(instrumentType == size_8mm)
        {
            m_InstrumentStatusBoth.type_R = "CZQ";
            m_InstrumentStatusBoth.size_R = "8MM";
        }
        m_InstrumentStatusBoth.ID_R = 1;
        m_InstrumentStatusBoth.usedTime_R = usedCounter;
    }
}

void LightBoard::setMyLiftingRockerStatus(const uint8_t& statusTmp)
{

}

void LightBoard::setLightBoardColor(eLightColor color, eLightModel model)
{
    m_LightBoardColor = color;
    m_LightBoardModel = model;
    // LOG(INFO)<<"set Light Color and Model SUCCESSFULL!";
}


void LightBoard::LCMD(QString cmd,eFinalSide Finalside,int arg2,int arg3,int arg4,int arg5)
{
    static long Callcounter=0;
//    qDebug()<<"lightboard LCMD:"<<Callcounter++<<" :"<<cmd;
//    std::lock_guard<std::mutex> lck(Lock_LCMD);
//    Lock_LCMD.lock();
    cmd = cmd.toUpper();
    QStringList slist = cmd.split("=");

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
//            qDebug()<<"LCMD inside LFI";
            break;
        }
        case LIGHTBOARD_ACTIONS_RW:
        {
            action1[0]=arg2;
            action1[1]=arg3;
            action1[2]=arg4;
            action1[3]=arg5;
            actionLen=4;
            cf.payload.type=CMD_READ_WRITE;
    //            qDebug()<<"LCMD inside LFI";
            break;
        }
        default:return;
    }

//    if(arg2!=-1)actionLen++;
//    if(arg3!=-1)actionLen++;

    uint8_t payloadsize = 3+actionLen;
    uint8_t framelen = 17 + payloadsize;

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

//    if(arg2!=-1){cf.payload.args[j]=(uint8_t)arg2;j++;}
//    if(arg3!=-1){cf.payload.args[j]=(uint8_t)arg3;j++;}

    Send_Frame(cf);
//    Lock_LCMD.unlock();
}

void LightBoard::CommTimeoutTimer_Handle()
{
    this->on_Disconnected();
    qDebug()<<"CommTimeoutTimer  Timerout!!!";

}

void LightBoard::DataIn(QByteArray data)
{
    static bool Comm_Start=false;
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
        if(cftemp.payload.type==CMD_UPLOAD_DATA)
        {
            if(Comm_Start==false){
                m_CommTimeOutTimer.setSingleShot(true);
                connect(&m_CommTimeOutTimer,&QTimer::timeout,this,&LightBoard::CommTimeoutTimer_Handle);
                m_CommTimeOutTimer.start(500);
                 Comm_Start=true;
                 qDebug()<<"CommTimeoutTimer  start";
            }
            else{
                m_CommTimeOutTimer.stop();
                m_CommTimeOutTimer.start(500);
//                qDebug()<<"CommTimeoutTimer  reset";
            }
            /*switch (cftemp.payload.args[0])
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
            if(argBufString == "IO:")
            {
                IOValdat = cftemp.payload.args[1];
                if(IOValdat != IOValdatOld){
                    m_changeCount ++;
                    argBufString += QString::number(IOValdat);
                    m_LiftingRockerStatus = IOValdat;
                    qDebug()<<"m_changeCount: "<<m_changeCount;
                    emit updateLiftingRockerStatus();
                }
                IOValdatOld = IOValdat;
            }
            else if(argBufString=="FAI:")*/
//            qDebug()<<"DataIn IN";
//            for(int i = 0;i<50;i++)
//            {
//                qDebug()<<cftemp.payload.args[i];
//            }
 //           qDebug()<<"DataIn OUT";
            {
//                LOG(INFO)<<"FAI in DataIn";
                uint16_t    MBSta_L=0;
                uint16_t    MBSta_R=0;
                MBSta_L=cftemp.payload.args[0];
                MBSta_L<<=8;
                MBSta_L+=cftemp.payload.args[1];
//                if(eFinalLeft==(eFinalSide)cftemp.payload.args[1])
//                {
                    uint8_t connectsta=cftemp.payload.args[2];
                    if(connectsta == 0){
                        uint8_t instrumentType = cftemp.payload.args[3];
                        u_int16_t usedCounter = cftemp.payload.args[4];
                        usedCounter <<= 8;
                        usedCounter+=cftemp.payload.args[5];
                        setMyInstrumentStatus(instrumentType, usedCounter, 'l');
                    }
                    else
                    {
                        m_InstrumentStatusBoth.init_L();
                    }
//                }

                    MBSta_R=cftemp.payload.args[22];
                    MBSta_R<<=8;
                    MBSta_R+=cftemp.payload.args[23];
//                if(eFinalRight==(eFinalSide)cftemp.payload.args[22])
//                {
                    connectsta=cftemp.payload.args[24];
                    if(connectsta == 0){
                        uint8_t instrumentType=cftemp.payload.args[25];
                        u_int16_t usedCounter=cftemp.payload.args[26];
                        usedCounter<<=8;
                        usedCounter+=cftemp.payload.args[27];
                        setMyInstrumentStatus(instrumentType, usedCounter, 'r');
                    }
                    else
                    {
                        m_InstrumentStatusBoth.init_R();
                    }
//                }

                    IOValdat = cftemp.payload.args[44];
                    if(IOValdat != IOValdatOld){
                        m_changeCount ++;
                        argBufString += QString::number(IOValdat);
                        m_LiftingRockerStatus = IOValdat;
                        qDebug()<<"m_changeCount: "<<m_changeCount;
                        emit updateLiftingRockerStatus();
                    }
                    IOValdatOld = IOValdat;
            }

        }
    }
}

void LightBoard::StartBootSelfCheckThread(void)
{
//    this->moveToThread(&m_BootSelfCheckThread);
//    m_BootSelfCheckThread.start();
//    connect(this,&LightBoard::StartBootSelfCheckThread,this,&LightBoard::BootSelfCheck);
//    emit StartBootSelfCheckThread();
}

void LightBoard::BootSelfCheck()
{
    QtConcurrent::run([](){
        qDebug()<<"in run1"<<QThread::currentThreadId();
    });
}

void LightBoard::SendInnerMsg(Module_Inner_E recever,int Action,QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender=Module_Inner_E::AssistDevice_Lightboard;
    msgTemp.Recver=recever;
    msgTemp.Request.insert(Action,arg);
//    emit SendMsgSignal(msgTemp);
    m_messagePool.SendMessage(msgTemp);
}

void LightBoard::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender=Module_Inner_E::AssistDevice_Lightboard;
    msgTemp.Recver=recever;
    QString stringtemp;
    for(int i=0;i<arglist.length();i++)
    {
      stringtemp+=arglist[i];
      stringtemp+=";";
    }
    stringtemp.chop(1);
    msgTemp.Request.insert(Action,stringtemp);
    m_messagePool.SendMessage(msgTemp);
}

void LightBoard::dealWithMessage(Message_Inner_T msg)
{
    if(msg.Recver!=Module_Inner_E::AssistDevice_Lightboard && msg.Recver != Module_Inner_E::MultipleModules)
        return;

    QHash<int,QString>::ConstIterator i=msg.Request.constBegin();
    while(i!=msg.Request.constEnd())
    {
//        switch (i.key())
//        {
//            case static_cast<int>(AssistDevice_LightBoardAction_E::ReadFianlData):
//            {
//                if(i.value()=="Left"){
//                    LCMD("LRD=FCT",eFinalLeft);
//                }else if(i.value()=="Right"){
//                    LCMD("LRD=FCT",eFinalRight);
//                }
//                break;
//            }
//            default:break;
//        }
        i++;
    }

}
