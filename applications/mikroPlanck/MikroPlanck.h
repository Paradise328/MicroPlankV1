#ifndef MIKROPLANCK_H
#define MIKROPLANCK_H

#include <QWidget>

#include <QMainWindow>
#include "RobotArmPage.h"
#include "EndEffector.h"
#include "StatusMenu.h"

namespace Ui {
class MikroPlanck;
}

class MikroPlanck : public QMainWindow
{
    Q_OBJECT

public:
    explicit MikroPlanck(QWidget *parent = 0);
    ~MikroPlanck();

    RobotArmPage *robot_arm_widget;
    EndEffector *end_effector_widget;
    StatusMenu *system_menu_widget;
private slots:
   // void on_ModeSelect_tabBarClicked(int index){};

    void on_systemStatusWidget_clicked();

    void on_robotArmWidget_clicked();

    void on_endEffectorWidget_clicked();

private:

    Ui::MikroPlanck *ui;


protected:

};

#endif // MIKROPLANCK_H
