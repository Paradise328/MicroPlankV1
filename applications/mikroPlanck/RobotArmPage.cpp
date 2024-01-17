#include "RobotArmPage.h"
#include "ui_RobotArmPage.h"


RobotArmPage::RobotArmPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RobotArmPage)
{

    ui->setupUi(this);
    this->lp=new Lifting("192.168.42.60",8080);
    this->lb_ee=new LightBoard("192.168.42.80",8080);
    lb_ee->LCMD("LSC=green");
    lb_ee->LCMD("LSM=on");
    connect(this->lb_ee,&LightBoard::IOValUpdate,this,&RobotArmPage::on_stick_sta_change);

}

RobotArmPage::~RobotArmPage()
{
    delete ui;
}

void RobotArmPage::on_initTorque_clicked()
{
    init_torque(1);init_torque(2);init_torque(3);
}

void RobotArmPage::on_enableTorque_clicked()
{
    torque_mode(1);torque_mode(2);torque_mode(3);
}

void RobotArmPage::on_disableTorque_clicked()
{
    disable_joint(1);disable_joint(2);disable_joint(3);

}

void RobotArmPage::on_initSpeed_clicked()
{
    init_joint(1);init_joint(2);init_joint(3);
}

void RobotArmPage::on_enableSpeed_clicked()
{
    enable_joint(1);enable_joint(2);enable_joint(3);
}

void RobotArmPage::on_disableSpeed_clicked()
{
    disable_joint(1);disable_joint(2);disable_joint(3);
}

void RobotArmPage::on_joint1CCWfast_pressed()
{
    update_speed(1,JOINTSPEEDFAST);

}

void RobotArmPage::on_joint1CCWfast_released()
{
    update_speed(1,0);

}

void RobotArmPage::on_joint1CCWslow_pressed()
{
    update_speed(1,JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint1CCWslow_released()
{
    update_speed(1,0);

}

void RobotArmPage::on_joint3CCWfast_pressed()
{
    update_speed(3,JOINTSPEEDFAST);

}

void RobotArmPage::on_joint3CCWfast_released()
{
    update_speed(3,0);

}

void RobotArmPage::on_joint2CCWfast_pressed()
{
    update_speed(2,JOINTSPEEDFAST);

}

void RobotArmPage::on_joint2CCWfast_released()
{
    update_speed(2,0);

}

void RobotArmPage::on_joint2CCWslow_pressed()
{
    update_speed(2,JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint2CCWslow_released()
{
    update_speed(2,0);

}

void RobotArmPage::on_joint3CCWslow_pressed()
{
    update_speed(3,JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint3CCWslow_released()
{
    update_speed(3,0);

}

void RobotArmPage::on_joint1CWslow_pressed()
{
    update_speed(1,-JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint1CWslow_released()
{
    update_speed(1,0);

}

void RobotArmPage::on_joint1CWfast_pressed()
{
    update_speed(1,-JOINTSPEEDFAST);

}

void RobotArmPage::on_joint1CWfast_released()
{
    update_speed(1,0);

}

void RobotArmPage::on_joint2CWslow_pressed()
{
    update_speed(2,-JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint2CWslow_released()
{
    update_speed(2,0);

}

void RobotArmPage::on_joint2CWfast_pressed()
{
    update_speed(2,-JOINTSPEEDFAST);

}

void RobotArmPage::on_joint2CWfast_released()
{
    update_speed(2,0);

}

void RobotArmPage::on_joint3CWslow_pressed()
{
    update_speed(3,-JOINTSPEEDSLOW);

}

void RobotArmPage::on_joint3CWslow_released()
{
    update_speed(3,0);

}

void RobotArmPage::on_joint3CWfast_pressed()
{
    update_speed(3,-JOINTSPEEDFAST);

}

void RobotArmPage::on_joint3CWfast_released()
{
    update_speed(3,0);

}

void RobotArmPage::on_setBrake_stateChanged(int arg1)
{
    /*
    set_brake_4602(1,arg1);
    usleep(1000*10);
    set_brake_4602(2,arg1);
    usleep(1000*10);
    set_brake_4602(3,arg1);
    usleep(1000*10);
    */
    if(arg1){
    init_torque(1);init_torque(2);init_torque(3);
    torque_mode(1);torque_mode(2);torque_mode(3);
    }else{
    disable_joint(1);disable_joint(2);disable_joint(3);

    }
}
void RobotArmPage::on_setBrake_clicked(bool checked)
{

}

void RobotArmPage::on_setATorqueSlope_clicked()
{
    int ATorqueSlope=ui->setATorqueSlopeValue->text().toInt();
    if(ATorqueSlope>1000){
        ATorqueSlope=1000;
    }if(ATorqueSlope<0){
        ATorqueSlope=0;
    }
    set_SDO_6087(1,ATorqueSlope);
}

void RobotArmPage::on_setBTorqueSlope_clicked()
{
    int BTorqueSlope=ui->setBTorqueSlopeValue->text().toInt();
    if(BTorqueSlope>1000){
        BTorqueSlope=1000;
    }if(BTorqueSlope<0){
        BTorqueSlope=0;
    }
    set_SDO_6087(2,BTorqueSlope);
}

void RobotArmPage::on_setCTorqueSlope_clicked()
{
    int CTorqueSlope=ui->setCTorqueSlopeValue->text().toInt();
    if(CTorqueSlope>1000){
        CTorqueSlope=1000;
    }if(CTorqueSlope<0){
        CTorqueSlope=0;
    }
    set_SDO_6087(3,CTorqueSlope);
}

void RobotArmPage::setArmEnableStatus(int enable){
    if(enable){
        ui->EnableButtonStatus->setText("Enable\n");
       // ui->EnableButtonStatus->setTextColor(toR);
    }
    else{
        ui->EnableButtonStatus->setText("Disable\n");
    }
}
void RobotArmPage::setArmSM(QVector<int> sm){

    ui->jointStatus1->setText(QString::number(sm[0],16));
    ui->jointStatus2->setText(QString::number(sm[1],16));
    ui->jointStatus3->setText(QString::number(sm[2],16));
    ui->jointMode1->setText(QString::number(sm[3]));
    ui->jointMode2->setText(QString::number(sm[4]));
    ui->jointMode3->setText(QString::number(sm[5]));
    if(sm[0]==0||sm[1]==0||sm[2]==0){
        ui->initTorque->setDisabled(1);
        ui->initSpeed->setDisabled(1);
        ui->enableSpeed->setDisabled(1);
        ui->enableTorque->setDisabled(1);
        ui->disableSpeed->setDisabled(1);
        ui->disableTorque->setDisabled(1);
        ui->joint1CCWfast->setDisabled(1);
        ui->joint1CWfast->setDisabled(1);
        ui->joint2CCWfast->setDisabled(1);
        ui->joint2CWfast->setDisabled(1);
        ui->joint3CCWfast->setDisabled(1);
        ui->joint3CWfast->setDisabled(1);
        ui->joint1CCWslow->setDisabled(1);
        ui->joint1CWslow->setDisabled(1);
        ui->joint2CCWslow->setDisabled(1);
        ui->joint2CWslow->setDisabled(1);
        ui->joint3CCWslow->setDisabled(1);
        ui->joint3CWslow->setDisabled(1);
        ui->toggleServo->setDisabled(1);
    }else{
        ui->initTorque->setDisabled(0);
        ui->initSpeed->setDisabled(0);
        ui->enableSpeed->setDisabled(0);
        ui->enableTorque->setDisabled(0);
        ui->disableSpeed->setDisabled(0);
        ui->disableTorque->setDisabled(0);
        ui->joint1CCWfast->setDisabled(0);
        ui->joint1CWfast->setDisabled(0);
        ui->joint2CCWfast->setDisabled(0);
        ui->joint2CWfast->setDisabled(0);
        ui->joint3CCWfast->setDisabled(0);
        ui->joint3CWfast->setDisabled(0);
        ui->joint1CCWslow->setDisabled(0);
        ui->joint1CWslow->setDisabled(0);
        ui->joint2CCWslow->setDisabled(0);
        ui->joint2CWslow->setDisabled(0);
        ui->joint3CCWslow->setDisabled(0);
        ui->joint3CWslow->setDisabled(0);
        ui->toggleServo->setDisabled(0);
    }
}

void RobotArmPage::on_FastUpButton_released()
{
    this->lp->LiftingBrake();

}

void RobotArmPage::on_FastUpButton_pressed()
{
    this->lp->LiftingUp(Lifting_Fast);
}

void RobotArmPage::on_SlowUpButton_pressed()
{
    this->lp->LiftingUp(Lifting_Slow);
}

void RobotArmPage::on_SlowUpButton_released()
{
    this->lp->LiftingBrake();
}

void RobotArmPage::on_SlowDownButton_pressed()
{
    this->lp->LiftingDown(Lifting_Slow);
}

void RobotArmPage::on_SlowDownButton_released()
{
    this->lp->LiftingBrake();
}

void RobotArmPage::on_FastDownButton_pressed()
{
    this->lp->LiftingDown(Lifting_Fast);
}

void RobotArmPage::on_FastDownButton_released()
{
    this->lp->LiftingBrake();

}

void RobotArmPage::on_toggleServo_clicked()
{

}


void RobotArmPage::on_stick_sta_change(uint8_t sta)
{
  // qDebug()<<"stick sta="<<sta;
   if(sta==1){this->lp->LiftingUp(Lifting_Fast);}
   else if(sta==2){this->lp->LiftingDown(Lifting_Fast);}
   else {this->lp->LiftingBrake();}

}
