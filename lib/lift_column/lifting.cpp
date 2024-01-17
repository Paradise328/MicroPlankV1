#include "lifting.h"

Lifting::Lifting()
{
    this->type=DEV_LIFTING;

    this->Qhash_Cmd_Classify.insert("LC",LIFTING_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LM",LIFTING_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("MN",LIFTING_ACTIONS_SET_LIFT);

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

//        QObject::connect(this, SIGNAL(Resend()), this, SLOT(onResend()));

}


Lifting::Lifting(QString ip,quint16 port):Peripheral_Device(ip,port)
{
    this->type=DEV_LIFTING;

    this->Qhash_Cmd_Classify.insert("LC",LIFTING_ACTIONS_SET_COLOR);
    this->Qhash_Cmd_Classify.insert("LM",LIFTING_ACTIONS_SET_MODEL);
    this->Qhash_Cmd_Classify.insert("MN",LIFTING_ACTIONS_SET_LIFT);

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

    this->SendTimer=new QTimer;
    this->SendTimer->setSingleShot(false);
    connect(this->SendTimer,&QTimer::timeout,this,&Lifting::SendTimer_Callback);
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
    switch(LiftingSta)
    {
        case Lifting_STOP:this->SendTimer->stop();this->LCMD("MN=BRAKE");break;
        case Lifting_GOING_UP:this->LCMD("MN=UP",this->LiftingSpeed);break;
        case Lifting_GOING_DOWN:this->LCMD("MN=DOWN",this->LiftingSpeed);break;
        default:break;
    }
}

void Lifting::LiftingUp(eLiftingSpeed spd)
{
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


