#include "StatusMenu.h"
#include "ui_StatusMenu.h"

StatusMenu::StatusMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusMenu)
{
    ui->setupUi(this);

}

StatusMenu::~StatusMenu()
{
    delete ui;
}

void StatusMenu::on_leftInstrumentCali_clicked()
{

}

void StatusMenu::on_rightInstrumentCali_clicked()
{

}

void StatusMenu::on_leftGimbalCali_clicked()
{

}

void StatusMenu::on_rightGimbalCali_clicked()
{

}

void StatusMenu::on_resetViper_clicked()
{

}

