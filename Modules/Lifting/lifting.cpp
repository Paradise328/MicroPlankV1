#include "lifting.h"

Lifting::Lifting(MessageQueue& messagePool):m_messagePool(messagePool)
{
    this->type=DEV_LIFTING;

    this->Qhash_Cmd_Classify.insert("LC",LIFTING_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LM",LIFTING_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("MN",LIFTING_ACTIONS_SET_LIFT);
    this->Qhash_Cmd_Classify.insert("FS",LIFTING_ACTIONS_SET_FOOTSUPPORT);

    this->Qhash_LightColor_Classify.insert("RED",LIGHT_COLOR_RED);
    this->Qhash_LightColor_Classify.insert("BLUE",LIGHT_COLOR_BLUE);
    this->Qhash_LightColor_Classify.insert("GREEN",LIGHT_COLOR_GREEN);
    this->Qhash_LightColor_Classify.insert("WHITE",LIGHT_COLOR_WHITE);
    this->Qhash_LightColor_Classify.insert("YELLOW",LIGHT_COLOR_YELLOW);

    this->Qhash_LightModel_Classify.insert("ON",LIGHT_MODEL_ON);
    this->Qhash_LightModel_Classify.insert("BLINK",LIGHT_MODEL_BLINK);
    this->Qhash_LightModel_Classify.insert("OFF",LIGHT_MODEL_OFF);

    this->Qhash_Liftmotion_Classify.insert("ON",LIFTING_MOTION_POWER_ON);
    this->Qhash_Liftmotion_Classify.insert("OFF",LIFTING_MOTION_POWER_ON);
    this->Qhash_Liftmotion_Classify.insert("UP",LIFTING_MOTION_UP);
    this->Qhash_Liftmotion_Classify.insert("DOWN",LIFTING_MOTION_DOWN);
    this->Qhash_Liftmotion_Classify.insert("BRAKE",LIFTING_MOTION_BRAKE);

    this->LiftingSta=Lifting_STOP;
    this->LiftingSpeed=Lifting_Fast;

    this->SendTimer=new QTimer;
    this->SendTimer->setSingleShot(false);
    connect(this->SendTimer,&QTimer::timeout,this,&Lifting::SendTimer_Callback);
    connect(this,&Lifting::DealMsgSighal,this,&Lifting::dealWithMsg);
//    connect(this,&Lifting::LiftingBrakeSignal,this,&Lifting::LiftingBrake);
//    connect(this,&Lifting::LiftingDownBrak,this,&Lifting::LiftingBrake);
//    connect(this,&Lifting::LiftingDownBrakeSignal,this,&Lifting::LiftingBrake);
//    connect(this,&Lifting::LiftingDownBrakeSignal,this,&Lifting::LiftingBrake);
//    connect(this,&Lifting::LiftingDownBrakeSignal,this,&Lifting::LiftingBrake);
//        QObject::connect(this, SIGNAL(Resend()), this, SLOT(onResend()));

}


Lifting::Lifting(QString ip,quint16 port,MessageQueue& messagePool):Peripheral_Device(ip,port),m_messagePool(messagePool)
{
    this->type=DEV_LIFTING;

    this->Qhash_Cmd_Classify.insert("LC",LIFTING_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LM",LIFTING_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("MN",LIFTING_ACTIONS_SET_LIFT);
    this->Qhash_Cmd_Classify.insert("FS",LIFTING_ACTIONS_SET_FOOTSUPPORT);

    this->Qhash_LightColor_Classify.insert("RED",LIGHT_COLOR_RED);
    this->Qhash_LightColor_Classify.insert("BLUE",LIGHT_COLOR_BLUE);
    this->Qhash_LightColor_Classify.insert("GREEN",LIGHT_COLOR_GREEN);
    this->Qhash_LightColor_Classify.insert("WHITE",LIGHT_COLOR_WHITE);
    this->Qhash_LightColor_Classify.insert("YELLOW",LIGHT_COLOR_YELLOW);

    this->Qhash_LightModel_Classify.insert("ON",LIGHT_MODEL_ON);
    this->Qhash_LightModel_Classify.insert("BLINK",LIGHT_MODEL_BLINK);
    this->Qhash_LightModel_Classify.insert("OFF",LIGHT_MODEL_OFF);

    this->Qhash_Liftmotion_Classify.insert("ON",LIFTING_MOTION_POWER_ON);
    this->Qhash_Liftmotion_Classify.insert("OFF",LIFTING_MOTION_POWER_ON);
    this->Qhash_Liftmotion_Classify.insert("UP",LIFTING_MOTION_UP);
    this->Qhash_Liftmotion_Classify.insert("DOWN",LIFTING_MOTION_DOWN);
    this->Qhash_Liftmotion_Classify.insert("BRAKE",LIFTING_MOTION_BRAKE);

    this->LiftingSta=Lifting_STOP;
    this->FootSupportSta=Lifting_STOP;

    this->SendTimer=new QTimer;
    this->SendTimer->setSingleShot(false);
    this->FootSupportSendTimer=new QTimer;
    this->FootSupportSendTimer->setSingleShot(false);
    connect(this->SendTimer,&QTimer::timeout,this,&Lifting::SendTimer_Callback);
    connect(this->FootSupportSendTimer,&QTimer::timeout,this,&Lifting::FootSupportSendTimer_Callback);
    connect(this,&Lifting::DealMsgSighal,this,&Lifting::dealWithMsg);
}

//void Lifting::LCMD(QString cmd)
//{
//    cmd=cmd.toUpper();
//    QStringList slist=cmd.split("=");

//    uint8_t action[2]={0};
//    action[0]=this->Qhash_Cmd_Classify[slist[0]];

//    switch (this->Qhash_Cmd_Classify[slist[0]])
//    {
//        case LIFTING_ACTIONS_SET_COLOR:
//        {
//            action[1]=this->Qhash_LightColor_Classify[slist[1]];
//            break;
//        }
//        case LIFTING_ACTIONS_SET_MODEL:
//        {
//             action[1]=this->Qhash_LightModel_Classify[slist[1]];
//             break;
//        }
//        case LIFTING_ACTIONS_SET_LIFT:
//        {
//            action[1]=this->Qhash_Liftmotion_Classify[slist[1]];
//            break;
//        }
//    }

//    uint8_t payloadsize=3+sizeof(action);
//    uint8_t framelen=17+payloadsize;


//    COMMU_FRAME cf;

//    memset(&cf,0,sizeof (COMMU_FRAME));
//    cf.preamble=PREAMBLE;
//    cf.ID=this->GetRandNum();
//    cf.size=framelen;
//    cf.sender=DEV_HOST;
//    cf.recever=DEV_LIFTING;

//    cf.payload.type=CMD_CTRL_WRITE;
//    cf.payload.size=payloadsize;
//    cf.payload.argscounter=sizeof(action);
//    for(unsigned int j=0;j<sizeof(action);j++)
//    {cf.payload.args[j]=action[j];}

//    Send_Frame(cf);

//}

void Lifting::LCMD(QString cmd,int arg1,int arg2,int arg3)
{
    cmd=cmd.toUpper();
    QStringList slist=cmd.split("=");

    uint8_t action1[2]={0};
    action1[0]=this->Qhash_Cmd_Classify[slist[0]];

    switch (this->Qhash_Cmd_Classify[slist[0]])
    {
        case LIFTING_ACTIONS_SET_COLOR:
        {
            action1[1]=this->Qhash_LightColor_Classify[slist[1]];
            break;
        }
        case LIFTING_ACTIONS_SET_MODEL:
        {
             action1[1]=this->Qhash_LightModel_Classify[slist[1]];
             break;
        }
        case LIFTING_ACTIONS_SET_LIFT:
        {
            action1[1]=this->Qhash_Liftmotion_Classify[slist[1]];
            break;
        }
        case LIFTING_ACTIONS_SET_FOOTSUPPORT:
        {

            action1[1]=this->Qhash_Liftmotion_Classify[slist[1]];
            break;
        }
    }

    int actionLen=2;
    if(arg1!=-1)actionLen++;
    if(arg2!=-1)actionLen++;
    if(arg3!=-1)actionLen++;

    uint8_t payloadsize=3+actionLen;
    uint8_t framelen=17+payloadsize;

    COMMU_FRAME cf;

    memset(&cf,0,sizeof (COMMU_FRAME));
    cf.preamble=PREAMBLE;
    cf.ID=this->GetRandNum();
    cf.size=framelen;
    cf.sender=DEV_HOST;
    cf.recever=DEV_LIFTING;
    cf.payload.type=CMD_CTRL_WRITE;
    cf.payload.size=payloadsize;
    cf.payload.argscounter=actionLen;
    unsigned int j=0;
    for(j=0;j<sizeof(action1);j++)
    {cf.payload.args[j]=action1[j];}
    if(arg1!=-1)cf.payload.args[j]=(uint8_t)arg1;j++;
    if(arg2!=-1)cf.payload.args[j]=(uint8_t)arg2;j++;
    if(arg3!=-1)cf.payload.args[j]=(uint8_t)arg3;j++;

    Send_Frame(cf);
}

void Lifting::SendTimer_Callback(void)
{
    if(m_LiftingDriveEnable==false){
        this->LiftingSta=Lifting_STOP;
        this->SendTimer->stop();
        this->LCMD("MN=BRAKE");
        return;
    }
    switch(LiftingSta)
    {
        case Lifting_STOP:this->SendTimer->stop();this->LCMD("MN=BRAKE");break;
        case Lifting_GOING_UP:this->LCMD("MN=UP",this->LiftingSpeed);break;
        case Lifting_GOING_DOWN:this->LCMD("MN=DOWN",this->LiftingSpeed);break;
        default:break;
    }
}


void Lifting::FootSupportSendTimer_Callback(void)
{
    if(m_LiftingDriveEnable==false){
        this->FootSupportSta=Lifting_STOP;
        this->FootSupportSendTimer->stop();
        this->LCMD("FS=BRAKE");
        return;
    }
     qDebug()<<"FootSupportSendTimer_Callback";
    switch(FootSupportSta)
    {
        case Lifting_STOP:this->FootSupportSendTimer->stop();this->LCMD("FS=BRAKE");break;
        case Lifting_GOING_UP:this->LCMD("FS=UP",this->FootSupportSpeed);break;
        case Lifting_GOING_DOWN:this->LCMD("FS=DOWN",this->FootSupportSpeed);break;
        default:break;
    }
}

void Lifting::LiftingUp(eLiftingSpeed spd)
{
    if(m_LiftingDriveEnable==false){
        this->LiftingSta=Lifting_STOP;
        this->SendTimer->stop();
        this->LCMD("MN=BRAKE");
        return;
    }
    this->LiftingSpeed=spd;
    switch(LiftingSta)
    {
        case Lifting_STOP:
                    this->LiftingSta=Lifting_GOING_UP;
                    this->SendTimer->start(100);
                    this->LCMD("MN=UP",this->LiftingSpeed);
                    break;
        case Lifting_GOING_UP:this->SendTimer->start(100);break;
        case Lifting_GOING_DOWN:
                    this->LiftingSta=Lifting_GOING_UP;
                    this->SendTimer->stop();
                    this->LCMD("MN=BRAKE");
                    this->SendTimer->start(100);
                    this->LCMD("MN=UP",this->LiftingSpeed);
                    break;
        default:break;
    }
}

void Lifting::LiftingDown(eLiftingSpeed spd)
{
    if(m_LiftingDriveEnable==false){
        this->LiftingSta=Lifting_STOP;
        this->SendTimer->stop();
        this->LCMD("MN=BRAKE");
        return;
    }

    this->LiftingSpeed=spd;
    switch(LiftingSta)
    {
        case Lifting_STOP:
                    this->LiftingSta=Lifting_GOING_DOWN;
                    this->SendTimer->start(100);
                    this->LCMD("MN=DOWN",this->LiftingSpeed);
                    break;
        case Lifting_GOING_DOWN:this->SendTimer->start(100);break;
        case Lifting_GOING_UP:
                    this->LiftingSta=Lifting_GOING_DOWN;
                    this->SendTimer->stop();
                    this->LCMD("MN=BRAKE");
                    this->SendTimer->start(100);
                    this->LCMD("MN=DOWN",this->LiftingSpeed);
                    break;
        default:break;
    }
}

void Lifting::LiftingBrake(void)
{
    this->LiftingSta=Lifting_STOP;
    this->SendTimer->stop();
    this->LCMD("MN=BRAKE");
}


void Lifting::FootSupportUp(eLiftingSpeed spd)
{
    if(m_LiftingDriveEnable==false){
        this->FootSupportSta=Lifting_STOP;
        this->FootSupportSendTimer->stop();
        this->LCMD("FS=BRAKE");
        return;
    }
    this->FootSupportSpeed=spd;
    switch(FootSupportSta)
    {
        case Lifting_STOP:
                    this->FootSupportSta=Lifting_GOING_UP;
                    this->FootSupportSendTimer->start(100);
                    this->LCMD("FS=UP",this->FootSupportSpeed);
                    break;
        case Lifting_GOING_UP:this->FootSupportSendTimer->start(100);break;
        case Lifting_GOING_DOWN:
                    this->FootSupportSta=Lifting_GOING_UP;
                    this->FootSupportSendTimer->stop();
                    this->LCMD("FS=BRAKE");
                    this->FootSupportSendTimer->start(100);
                    this->LCMD("FS=UP",this->FootSupportSpeed);
                    break;
        default:break;
    }
}

void Lifting::FootSupportDown(eLiftingSpeed spd)
{
    if(m_LiftingDriveEnable==false){
        this->FootSupportSta=Lifting_STOP;
        this->FootSupportSendTimer->stop();
        this->LCMD("FS=BRAKE");
        return;
    }

    this->FootSupportSpeed=spd;
    switch(FootSupportSta)
    {
        case Lifting_STOP:
                    this->FootSupportSta=Lifting_GOING_DOWN;
                    this->FootSupportSendTimer->start(100);
                    this->LCMD("FS=DOWN",this->FootSupportSpeed);
                    break;
        case Lifting_GOING_DOWN:this->FootSupportSendTimer->start(100);break;
        case Lifting_GOING_UP:
                    this->FootSupportSta=Lifting_GOING_DOWN;
                    this->FootSupportSendTimer->stop();
                    this->LCMD("FS=BRAKE");
                    this->FootSupportSendTimer->start(100);
                    this->LCMD("FS=DOWN",this->FootSupportSpeed);
                    break;
        default:break;
    }
}

void Lifting::FootSupportBrake(void)
{
    this->FootSupportSta=Lifting_STOP;
    this->FootSupportSendTimer->stop();
    this->LCMD("FS=BRAKE");
}



void Lifting::SelfCheck(void)
{
    uint8_t payloadsize=3;
    uint8_t framelen=17+payloadsize;
    disconnect(this,&Lifting::NewDataIn,this,&Lifting::DataIn);
    connect(this,&Lifting::NewDataIn,this,&Lifting::DataIn);

    COMMU_FRAME cf;
    memset(&cf,0,sizeof (COMMU_FRAME));
    cf.preamble=PREAMBLE;
    cf.ID=this->GetRandNum();
    cf.size=framelen;
    cf.sender=DEV_HOST;
    cf.recever=this->type;

    cf.payload.type=CMD_TEST;
    cf.payload.size=payloadsize;
    cf.payload.argscounter=0;
    try
    {
       Send_Frame(cf);
    }
    catch (std::exception e)
    {
       qWarning()<<"FlushConns err:"<<e.what();
    }
}



void Lifting::DataIn(QByteArray data)
{
    if(QString(data).contains("Lifting"))
    {
        qDebug()<<QString(data);
        return;
    }
    COMMU_FRAME cftemp;
    eDepackReturn edr=this->Depack_Frame(data,cftemp);
     qDebug()<<"Lifting::DataIn";
    if(edr==Depack_SUCCESS)
    {
        qDebug()<<"Depack_SUCCESS";
        if(cftemp.payload.type==CMD_TEST&&cftemp.payload.args[0]==0x00)
        {
            disconnect(this,&Lifting::NewDataIn,this,&Lifting::DataIn);
            qDebug()<<"cmd_test";
            SelfCheckOK=true;
        }
    }
}


bool Lifting::IsSelfCheckOk(void)
{
    return SelfCheckOK;
}


void Lifting::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSighal();
}

void Lifting::dealWithMsg()
{
    Message_Inner_T msg;
    int counter = 0;
    while(1)
    {
        m_MsgGottenRWLock.lockForRead();
        if(m_MsgGottenQueue.isEmpty()==false){
            msg=m_MsgGottenQueue.dequeue();
        }else{
             m_MsgGottenRWLock.unlock();
             break;
        }
        m_MsgGottenRWLock.unlock();
        if(msg.Recver!=Module_Inner_E::AssistDevice_Lifting&&msg.Recver!=Module_Inner_E::MultipleModules)
            return;

       QHash<int,QString>::ConstIterator i=msg.Request.constBegin();
        while(i!=msg.Request.constEnd())
        {
            switch (i.key())
            {
                case static_cast<int>(AssistDevice_LiftingAction_E::LiftingBrake):
                {
                    LiftingBrake();
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::LiftingUp):
                {
                    if(i.value()=="Fast"){
                        LiftingUp(Lifting_Fast);
                    }else if(i.value()=="Slow"){

                        //LiftingUp(Lifting_Slow);
                    }
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::LiftingDown):
                {
                    if(i.value()=="Fast"){
                        LiftingDown(Lifting_Fast);
                    }else if(i.value()=="Slow"){

                        //LiftingDown(Lifting_Slow);
                    }
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::FootSupportUp):
                {
                    qDebug()<<"FootSupportUp(Lifting_Slow);";
                    FootSupportUp(Lifting_Fast);
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::FootSupportDown):
                {
                    qDebug()<<" FootSupportDown(Lifting_Slow);";
                    FootSupportDown(Lifting_Fast);
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::FootSupportBrake):
                {
                    FootSupportBrake();
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::BootSelfCheck):
                {
                    BootSelfCheck();
                    break;
                }
                case static_cast<int>(AssistDevice_LiftingAction_E::LiftingDriverSwitch):
                {
                    if(i.value()=="off"){
                        SetLiftingDriveEnSta(false);
                    }else if(i.value()=="on"){
                        SetLiftingDriveEnSta(true);
                    }
                    break;
                }
                case static_cast<int>(MultipleDevAction_E::RecvLightBoardIoState):
                {
                    QStringList slist  =i.value().split(":");
                    if(slist[1] == "1"){
                        LiftingUp(Lifting_Fast);
                    }else if(slist[1] == "2"){
                        LiftingDown(Lifting_Fast);
                    }
                    else{
                        LiftingBrake();
                    }
                    break;
                }
                default:break;
            }
            i++;
        }
    }

}


void Lifting::SendInnerMsg(Module_Inner_E recever,int Action,QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender=Module_Inner_E::AssistDevice_Lifting;
    msgTemp.Recver=recever;
    msgTemp.Request.insert(Action,arg);
//    emit SendMsgSignal(msgTemp);
    m_messagePool.SendMessage(msgTemp);
}

void Lifting::SetLiftingDriveEnSta(bool en)
{
    m_LiftingDriveEnable=en;
}

void Lifting::BootSelfCheck()
{
    uint8_t RetryCount=3;
        do{
            this->SelfCheck();
            SteadyDelay(2000);
            // LOG(INFO)<<"IsSelfCheckOk: "<<this->IsSelfCheckOk();
            if(this->IsSelfCheckOk())
            {
                SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvLiftingBootSta),"Ok");
                return ;
            }
            else
            {
               if(RetryCount==0)
               {
                   SendInnerMsg(Module_Inner_E::MultipleModules,static_cast<int>(MultipleDevAction_E::RecvLiftingBootSta),"Err");
               }
               else RetryCount--;
            }
        }while(RetryCount>0);

}
