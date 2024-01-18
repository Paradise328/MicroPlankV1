#include "MikroPlanck.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MikroPlanck w;
    w.show();

    // robot arm thread
    pthread_t robot_arm_thread_id;
    pthread_create(&robot_arm_thread_id, NULL, robotarmThread, NULL);

    return a.exec();
}
