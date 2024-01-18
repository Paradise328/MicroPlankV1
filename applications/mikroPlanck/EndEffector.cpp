#include "EndEffector.h"
#include "ui_EndEffector.h"
#include "../../lib/robot_arm/robotarm.h"
#include <QDebug>
#include <fstream>
#include <unistd.h>
#include <pthread.h>

EndEffector::EndEffector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EndEffector)
{
    ui->setupUi(this);
}

EndEffector::~EndEffector()
{
    delete ui;
}


//Void EndEffector::

void EndEffector::on_initSpeed_clicked()
{
    init_joint(1);init_joint(2);init_joint(3);

}


void EndEffector::on_enableSpeed_clicked()
{
    enable_joint(1);
    enable_joint(2);
    enable_joint(3);
}


void EndEffector::on_disableSpeed_clicked()
{
    disable_joint(1);
    disable_joint(2);
    disable_joint(3);
}


int record_while_speed_control = 0;
QString filename = "";

void EndEffector::on_btnStartSpeed_clicked()
{
    //read speed,set speed var
    int setspeed=ui->textSetSpeed->toPlainText().toInt();
    record_while_speed_control=1;

    update_speed(1,setspeed);

}


void EndEffector::on_btnStopSpeed_clicked()
{
    //set speed to zero
    record_while_speed_control=0;
    update_speed(1,0);
}




void EndEffector::on_btnRecord_clicked()
{
    filename=ui->textFileName->toPlainText();
    filename+=".csv";
}

void EndEffector::record_data()
{

}

