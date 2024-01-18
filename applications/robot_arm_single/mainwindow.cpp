#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../lib/robot_arm/robotarm.h"
//#include "robotarm.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
#define JOINTSPEEDFAST 4000
#define JOINTSPEEDSLOW 1500
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setArmEnableStatus(int enable){
    if(enable){
        ui->EnableButtonStatus->setText("Enable\n");
       // ui->EnableButtonStatus->setTextColor(toR);
    }
    else{
        ui->EnableButtonStatus->setText("Disable\n");
    }
}

void MainWindow::on_initTorque_clicked()
{
    init_torque(1);init_torque(2);init_torque(3);
}

void MainWindow::on_enableTorque_clicked()
{
    torque_mode(1);torque_mode(2);torque_mode(3);
}

void MainWindow::on_disableTorque_clicked()
{
    disable_joint(1);disable_joint(2);disable_joint(3);

}

void MainWindow::on_initSpeed_clicked()
{
    init_joint(1);init_joint(2);init_joint(3);
}

void MainWindow::on_enableSpeed_clicked()
{
    enable_joint(1);enable_joint(2);enable_joint(3);
}

void MainWindow::on_disableSpeed_clicked()
{
    disable_joint(1);disable_joint(2);disable_joint(3);
}

void MainWindow::on_joint1CCWfast_pressed()
{
    update_speed(1,JOINTSPEEDFAST);

}

void MainWindow::on_joint1CCWfast_released()
{
    update_speed(1,0);

}

void MainWindow::on_joint1CCWslow_pressed()
{
    update_speed(1,JOINTSPEEDSLOW);

}

void MainWindow::on_joint1CCWslow_released()
{
    update_speed(1,0);

}

void MainWindow::on_joint3CCWfast_pressed()
{
    update_speed(3,JOINTSPEEDFAST);

}

void MainWindow::on_joint3CCWfast_released()
{
    update_speed(3,0);

}

void MainWindow::on_joint2CCWfast_pressed()
{
    update_speed(2,JOINTSPEEDFAST);

}

void MainWindow::on_joint2CCWfast_released()
{
    update_speed(2,0);

}

void MainWindow::on_joint2CCWslow_pressed()
{
    update_speed(2,JOINTSPEEDSLOW);

}

void MainWindow::on_joint2CCWslow_released()
{
    update_speed(2,0);

}

void MainWindow::on_joint3CCWslow_pressed()
{
    update_speed(3,JOINTSPEEDSLOW);

}

void MainWindow::on_joint3CCWslow_released()
{
    update_speed(3,0);

}

void MainWindow::on_joint1CWslow_pressed()
{
    update_speed(1,-JOINTSPEEDSLOW);

}

void MainWindow::on_joint1CWslow_released()
{
    update_speed(1,0);

}

void MainWindow::on_joint1CWfast_pressed()
{
    update_speed(1,-JOINTSPEEDFAST);

}

void MainWindow::on_joint1CWfast_released()
{
    update_speed(1,0);

}

void MainWindow::on_joint2CWslow_pressed()
{
    update_speed(2,-JOINTSPEEDSLOW);

}

void MainWindow::on_joint2CWslow_released()
{
    update_speed(2,0);

}

void MainWindow::on_joint2CWfast_pressed()
{
    update_speed(2,-JOINTSPEEDFAST);

}

void MainWindow::on_joint2CWfast_released()
{
    update_speed(2,0);

}

void MainWindow::on_joint3CWslow_pressed()
{
    update_speed(3,-JOINTSPEEDSLOW);

}

void MainWindow::on_joint3CWslow_released()
{
    update_speed(3,0);

}

void MainWindow::on_joint3CWfast_pressed()
{
    update_speed(3,-JOINTSPEEDFAST);

}

void MainWindow::on_joint3CWfast_released()
{
    update_speed(3,0);

}

void MainWindow::on_setBrake_stateChanged(int arg1)
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
void MainWindow::on_setBrake_clicked(bool checked)
{

}

void MainWindow::on_setATorqueSlope_clicked()
{
    int ATorqueSlope=ui->setATorqueSlopeValue->text().toInt();
    if(ATorqueSlope>1000){
        ATorqueSlope=1000;
    }if(ATorqueSlope<0){
        ATorqueSlope=0;
    }
    set_SDO_6087(1,ATorqueSlope);
}

void MainWindow::on_setBTorqueSlope_clicked()
{
    int BTorqueSlope=ui->setBTorqueSlopeValue->text().toInt();
    if(BTorqueSlope>1000){
        BTorqueSlope=1000;
    }if(BTorqueSlope<0){
        BTorqueSlope=0;
    }
    set_SDO_6087(2,BTorqueSlope);
}

void MainWindow::on_setCTorqueSlope_clicked()
{
    int CTorqueSlope=ui->setCTorqueSlopeValue->text().toInt();
    if(CTorqueSlope>1000){
        CTorqueSlope=1000;
    }if(CTorqueSlope<0){
        CTorqueSlope=0;
    }
    set_SDO_6087(3,CTorqueSlope);
}

void MainWindow::on_toggleServo_clicked()
{
  //  toggle_servo(1);toggle_servo(2);toggle_servo(3);
    toggle_control_mode(1);toggle_control_mode(2);toggle_control_mode(3);
}
