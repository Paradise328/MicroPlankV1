#include "LiftColumn.h"
#include "ui_LiftColumn.h"
//#include
LiftColumn::LiftColumn(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiftColumn)
{
    ui->setupUi(this);

    this->lp=new Lifting("192.168.42.60",8080);
}

LiftColumn::~LiftColumn()
{
    delete ui;
}

void LiftColumn::on_FastUpButton_released()
{
    qDebug()<<"FastUpButton_released";

    this->lp->LiftingBrake();

}

void LiftColumn::on_FastUpButton_pressed()
{
    qDebug()<<"FastUpButton_pressed";

    this->lp->LiftingUp(Lifting_Fast);

}

void LiftColumn::on_SlowUpButton_pressed()
{
    this->lp->LiftingUp(Lifting_Slow);

}

void LiftColumn::on_SlowUpButton_released()
{
    this->lp->LiftingBrake();

}

void LiftColumn::on_SlowDownButton_pressed()
{
    this->lp->LiftingDown(Lifting_Slow);

}

void LiftColumn::on_SlowDownButton_released()
{
    this->lp->LiftingBrake();

}

void LiftColumn::on_FastDownButton_pressed()
{
    this->lp->LiftingDown(Lifting_Fast);

}

void LiftColumn::on_FastDownButton_released()
{
    this->lp->LiftingBrake();

}
