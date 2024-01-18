#include "UIinterface.h"
#include <QDebug>
//#include "RobotProgram.h"
#include <QThread>
#include <cstring>
<<<<<<< HEAD

#include <QVariant>

=======
#include "../../lib/GalilControl/GalilControl.h"

#include <QVariant>
extern GCon g_l,g_r;
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
// UI Interface Array
//
// [0]setSpeedLevel--1/2/3/4/5
// [1]setKinematicsState--0 forwardkinematics/1 inversekinematics
// [2]setLockState--0 disable/1 enable
// [3]setStepState--0 stepforward/1 stepbackward
// [4]axis1SoftLimit--0 normal/1 abnormal
// [5]axis2SoftLimit--0 normal/1 abnormal
// [6]axis3SoftLimit--0 normal/1 abnormal
// [7]safety--0 safe/1 danger
// [8]conserve1
// [9]conserve2



UIinterface::UIinterface(QQmlApplicationEngine &eng) :engine(eng)
{
    this->Text_InstrumentType_L= this->engine.rootObjects().first()->findChild<QObject*>("txtInstrumentType_L");
    this->Text_InstrumentType_R= this->engine.rootObjects().first()->findChild<QObject*>("txtInstrumentType_R");
    this->Button_robotArmEnable=this->engine.rootObjects().first()->findChild<QObject*>("robotArmEnable");
    this->Button_robotArmDisable=this->engine.rootObjects().first()->findChild<QObject*>("robotArmDisable");

    this->Button_Fastup=this->engine.rootObjects().first()->findChild<QObject*>("btnFastUp");
    this->Button_Slowup=this->engine.rootObjects().first()->findChild<QObject*>("btnSlowUp");
    this->Button_Fastdown=this->engine.rootObjects().first()->findChild<QObject*>("btnFastDown");
    this->Button_Slowdown=this->engine.rootObjects().first()->findChild<QObject*>("btnSlowDown");
    this->Button_ReleaseInstrument=this->engine.rootObjects().first()->findChild<QObject*>("btnReleaseInstrument");
    this->Button_InstrumentInstalled=this->engine.rootObjects().first()->findChild<QObject*>("btnInstrumentInstalled");

<<<<<<< HEAD
=======
    this->Button_CaliGimbalL=this->engine.rootObjects().first()->findChild<QObject*>("btnCaliGimbalL");
    this->Button_CaliGimbalR=this->engine.rootObjects().first()->findChild<QObject*>("btnCaliGimbalR");
    this->Button_CaliInstrumentL=this->engine.rootObjects().first()->findChild<QObject*>("btnCaliInstrumentL");
    this->Button_CaliInstrumentR=this->engine.rootObjects().first()->findChild<QObject*>("btnCaliInstrumentR");
    this->Button_Enable_l=this->engine.rootObjects().first()->findChild<QObject*>("btnEnable_l");
    this->Button_Enable_r=this->engine.rootObjects().first()->findChild<QObject*>("btnEnable_r");


>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    this->ArmEnable=false;

    connect(this->Button_robotArmEnable,SIGNAL(clicked()),this,SLOT(onRobotArmEnable_Clicked()));
    connect(this->Button_robotArmDisable,SIGNAL(clicked()),this,SLOT(onRobotArmDisable_Clicked()));
<<<<<<< HEAD
=======
    connect(this->Button_CaliGimbalL,SIGNAL(clicked()),this,SLOT(onButton_CaliGimbalL_Clicked()));
    connect(this->Button_CaliGimbalR,SIGNAL(clicked()),this,SLOT(onButton_CaliGimbalR_Clicked()));
    connect(this->Button_CaliInstrumentL,SIGNAL(clicked()),this,SLOT(onButton_CaliInstrumentL_Clicked()));
    connect(this->Button_CaliInstrumentR,SIGNAL(clicked()),this,SLOT(onButton_CaliInstrumentR_Clicked()));
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202

    SetButtonSta(this->Button_robotArmDisable,UI_Button_Highlight);

    SetButtonSta(this->Button_Fastup,UI_Button_Gray);
    SetButtonSta(this->Button_Slowup,UI_Button_Gray);
    SetButtonSta(this->Button_Fastdown,UI_Button_Gray);
    SetButtonSta(this->Button_Slowdown,UI_Button_Gray);
//SetButtonSta(this->Button_robotArmEnable,UI_Button_Default);

<<<<<<< HEAD
=======
//    SetHandle_L_En(true);
//    SetHandle_R_En(true);

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
}

void UIinterface::onRobotArmEnable_Clicked()
{
    ArmEnable=true;
    SetButtonSta(this->Button_robotArmEnable,UI_Button_Highlight);
    SetButtonSta(this->Button_robotArmDisable,UI_Button_Default);

//    setlevel(1);
    SetButtonSta(this->Button_Fastup,UI_Button_Default);
    SetButtonSta(this->Button_Slowup,UI_Button_Default);
    SetButtonSta(this->Button_Fastdown,UI_Button_Default);
    SetButtonSta(this->Button_Slowdown,UI_Button_Default);
}

void UIinterface::onRobotArmDisable_Clicked()
{
    ArmEnable=false;
    SetButtonSta(this->Button_robotArmEnable,UI_Button_Default);
    SetButtonSta(this->Button_robotArmDisable,UI_Button_Highlight);
    SetButtonSta(this->Button_Fastup,UI_Button_Gray);
    SetButtonSta(this->Button_Slowup,UI_Button_Gray);
    SetButtonSta(this->Button_Fastdown,UI_Button_Gray);
    SetButtonSta(this->Button_Slowdown,UI_Button_Gray);
//    setlevel(3);
}

<<<<<<< HEAD
=======
void UIinterface::onButton_CaliGimbalL_Clicked()
{
    printf("gimbal calibration left\n");
    GIMBAL_Calibration(g_l,'l');
}
void UIinterface::onButton_CaliGimbalR_Clicked()
{
    printf("gimbal calibration right\n");
    GIMBAL_Calibration(g_r,'r');
}
void UIinterface::onButton_CaliInstrumentL_Clicked()
{
    printf("instrument CaliInstrumentL\n");
    INSTRUMENT_Calibration(g_l,'l');
}
void UIinterface::onButton_CaliInstrumentR_Clicked()
{
    printf("instrument CaliInstrumentR\n");
    INSTRUMENT_Calibration(g_r,'r');
}

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
void UIinterface::onLightBoardStickChange(uint8_t sta)
{
    if(ArmEnable!=true)return;
    switch (sta) {
        case 1:
        {
            this->m_lifting_p->LiftingUp(Lifting_Fast);
            this->SetButtonSta(this->Button_Fastup,UI_Button_Highlight);
            break;
        }
        case 2:
        {
            this->m_lifting_p->LiftingDown(Lifting_Fast);
            this->SetButtonSta(this->Button_Fastdown,UI_Button_Highlight);
            break;
        }
        default:
        {
            this->m_lifting_p->LiftingBrake();
            this->SetButtonSta(this->Button_Fastup,UI_Button_Default);
            this->SetButtonSta(this->Button_Fastdown,UI_Button_Default);
            break;
        }
    }
}

void UIinterface::onButton_InstrumentInstalled_Clicked()
{

}


void UIinterface::onButton_ReleaseInstrument_Clicked()
{


}

void  UIinterface::setCurrentPage(int current_page_num)
{
    this->CurrentPage=current_page_num;
<<<<<<< HEAD
=======
    /*
    if(current_page_num==0){
        MotorOff(g_r);
        MotorOff(g_l);
    }if(current_page_num==1){
        initGali(g_l);initGali(g_r);

    }*/
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    emit CurrentChanged(this->CurrentPage);
}

int  UIinterface::GetCurrentPage(void)
{
    return this->CurrentPage;
}

void UIinterface::SetButtonSta(QObject* btn,UI_Button_e btnsta)
{
    if(btn!=nullptr)
    {
        QMetaObject::invokeMethod(btn, "setsta",Q_ARG(QVariant, btnsta));
    }
    else
    {
        qDebug()<<"btn null";
    }
}

void UIinterface::SetInstrumentType_L(QString type)
{
    if(this->Text_InstrumentType_L!=nullptr)
    {
        this->Text_InstrumentType_L->setProperty("text",type);
        QMetaObject::invokeMethod(this->Text_InstrumentType_L, "doLayout");
    }
}

void  UIinterface::SetInstrumentType_R(QString type)
{
    if(this->Text_InstrumentType_R!=nullptr)
    {
        this->Text_InstrumentType_R->setProperty("text",type);
        QMetaObject::invokeMethod(this->Text_InstrumentType_R, "doLayout");
    }
}


void UIinterface::setlevel(int level)
{
    this->level=level;
<<<<<<< HEAD
    qDebug()<<" emit this->LevelChange";
=======
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    emit LevelChanged(this->level);

}

<<<<<<< HEAD
=======
void UIinterface::SetHandle_L_En(bool en)
{
    this->m_handle_L_En=en;
    if(Button_Enable_l!=nullptr)
    {
        qDebug()<<" Button_Enable_l setsta";
        QMetaObject::invokeMethod(Button_Enable_l, "setsta",Q_ARG(QVariant, en));
    }
}


void UIinterface::SetHandle_R_En(bool en)
{
    this->m_handle_R_En=en;
    if(Button_Enable_r!=nullptr)
    {
        qDebug()<<" Button_Enable_r setsta";
        QMetaObject::invokeMethod(Button_Enable_r, "setsta",Q_ARG(QVariant, en));
    }

}

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
void UIinterface::SetLightBoard(LightBoard *lgp)
{
    this->m_lightboard_p=lgp;
    if(this->m_lightboard_p!=nullptr)
    {
        connect(this->m_lightboard_p,&LightBoard::IOValUpdate,this,&UIinterface::onLightBoardStickChange);
    }
}

int UIinterface::getlevel()
{
    return this->level;
}


void UIinterface::SetLifting(Lifting *lfp)
{
  m_lifting_p=lfp;
}
void UIinterface::liftingBrake()
{
    if(m_lifting_p!=nullptr)
    this->m_lifting_p->LiftingBrake();
}

void UIinterface::liftingFastup()
{
    if(m_lifting_p!=nullptr)
    m_lifting_p->LiftingUp(Lifting_Fast);

}

void UIinterface::liftingSlowup()
{
    if(m_lifting_p!=nullptr)
    m_lifting_p->LiftingUp(Lifting_Slow);
}

void UIinterface::liftingFastdown()
{
    if(m_lifting_p!=nullptr)
    m_lifting_p->LiftingDown(Lifting_Fast);
}

void UIinterface::liftingSlowdown()
{
    if(m_lifting_p!=nullptr)
    m_lifting_p->LiftingDown(Lifting_Slow);
}

void UIinterface::setUISignal(int* uiSignal)
{
    my_uiSignal=uiSignal;
    emit uiSignalChanged(uiSignal);
    qDebug()<<"set my_uiSignal: "<<endl;
    for(int i=0;i<10;i++)
    {
        qDebug()<<my_uiSignal[i];
    }
}
int* UIinterface::getUISignal()
{
    return my_uiSignal;
}
int UIinterface::systemStart()
{
//    Calibration(robot->g);
//    galil(GCmd(robot->g, "mode=2"));
    //ForwardKinematics(robot->g);
    qDebug()<<"system start"<<endl;
    return 1;
}
int UIinterface::getSignal(int index)
{
    return my_uiSignal[index];
}
void UIinterface::systemStop()
{
    qDebug()<<"quit the program/shut down"<<endl;
    qDebug()<<"from thread id:"<<QThread::currentThreadId()<<endl;
//    galil(GCmd(robot->g,"ST ABCDE"));
}
void UIthread::onKinematics()
{
    qDebug()<<"forward kinematics"<<endl;
    qDebug()<<"from thread id:"<<QThread::currentThreadId()<<endl;
  //  galil(GCmd(robot->g,"XQ#DOG,4"));
//    FinalControl(robot->g);
}

void UIinterface::enableRobotArm()
{
    qDebug()<<"enable robot arm";
    qDebug()<<"from thread id:"<<QThread::currentThreadId()<<endl;
    init_torque(1);init_torque(2);init_torque(3);
    usleep(1000);
    torque_mode(1);torque_mode(2);torque_mode(3);
  //  galil(GCmd(robot->g,"XQ#DOG,4"));
//    FinalControl(robot->g);
}

void UIinterface::disableRobotArm()
{
    qDebug()<<"disable robot arm";
    qDebug()<<"from thread id:"<<QThread::currentThreadId()<<endl;
    disable_joint(1);disable_joint(2);disable_joint(3);
  //  galil(GCmd(robot->g,"XQ#DOG,4"));
//    FinalControl(robot->g);
}
