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
