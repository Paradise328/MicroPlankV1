#include "MikroPlanck.h"
#include "ui_MikroPlanck.h"

MikroPlanck::MikroPlanck(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MikroPlanck)
{
    ui->setupUi(this);

    robot_arm_widget=new RobotArmPage;
    end_effector_widget=new EndEffector;
    system_menu_widget=new StatusMenu;

    ui->ModeSelect->addWidget(system_menu_widget);
    ui->ModeSelect->addWidget(robot_arm_widget);
    ui->ModeSelect->addWidget(end_effector_widget);

    ui->ModeSelect->setCurrentWidget(system_menu_widget);
}

MikroPlanck::~MikroPlanck()
{
    delete ui;
}


void MikroPlanck::on_systemStatusWidget_clicked()
{
    ui->ModeSelect->setCurrentWidget(system_menu_widget);
}

void MikroPlanck::on_robotArmWidget_clicked()
{
    ui->ModeSelect->setCurrentWidget(robot_arm_widget);
}

void MikroPlanck::on_endEffectorWidget_clicked()
{
    ui->ModeSelect->setCurrentWidget(end_effector_widget);
}
