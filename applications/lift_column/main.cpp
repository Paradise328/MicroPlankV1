#include "LiftColumn.h"
#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    usleep(2000*1000);

    LiftColumn *lift_column=new LiftColumn();
    lift_column->show();

    return a.exec();
}
