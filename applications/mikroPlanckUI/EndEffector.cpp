#include "EndEffector.h"
#include "ui_EndEffector.h"

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
