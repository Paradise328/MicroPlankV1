#ifndef ROBOTARMPAGE_H
#define ROBOTARMPAGE_H

#include <QWidget>
//#include "robotarm.h"
//#include "lifting.h"
#include "../../lib/robot_arm/robotarm.h"
#include "../../lib/lift_column/lifting.h"
#include "../../lib/light_board/lightboard.h"
#include <QMetaType>

#include <QVector>
#define JOINTSPEEDFAST 4000
#define JOINTSPEEDSLOW 1500
extern int robotarm_enable_btn;

namespace Ui {
class RobotArmPage;
}

class RobotArmPage : public QWidget
{
    Q_OBJECT

public:
    explicit RobotArmPage(QWidget *parent = 0);
    ~RobotArmPage();
    Lifting *lp=nullptr;
    LightBoard *lb_ee=nullptr;
    void setArmEnableStatus(int);
    void setArmSM(QVector<int> sm);

private slots:

    void on_initTorque_clicked();

    void on_enableTorque_clicked();

    void on_disableTorque_clicked();

    void on_initSpeed_clicked();

    void on_enableSpeed_clicked();

    void on_disableSpeed_clicked();

    void on_joint1CCWfast_pressed();

    void on_joint1CCWfast_released();

    void on_joint1CCWslow_pressed();

    void on_joint1CCWslow_released();

    void on_joint3CCWfast_pressed();

    void on_joint3CCWfast_released();

    void on_joint2CCWfast_pressed();

    void on_joint2CCWfast_released();

    void on_joint2CCWslow_pressed();

    void on_joint2CCWslow_released();

    void on_joint3CCWslow_pressed();

    void on_joint3CCWslow_released();

    void on_joint1CWslow_pressed();

    void on_joint1CWslow_released();

    void on_joint1CWfast_pressed();

    void on_joint1CWfast_released();

    void on_joint2CWslow_pressed();

    void on_joint2CWslow_released();

    void on_joint2CWfast_pressed();

    void on_joint2CWfast_released();

    void on_joint3CWslow_pressed();

    void on_joint3CWslow_released();

    void on_joint3CWfast_pressed();

    void on_joint3CWfast_released();

    void on_setBrake_stateChanged(int arg1);

    void on_setBrake_clicked(bool checked);

    void on_setATorqueSlope_clicked();

    void on_setBTorqueSlope_clicked();

    void on_setCTorqueSlope_clicked();

    void on_FastUpButton_released();

    void on_FastUpButton_pressed();

    void on_SlowUpButton_pressed();

    void on_SlowUpButton_released();

    void on_SlowDownButton_pressed();

    void on_SlowDownButton_released();

    void on_FastDownButton_pressed();

    void on_FastDownButton_released();
 
    void on_toggleServo_clicked();

    void on_stick_sta_change(uint8_t sta);

private:
    Ui::RobotArmPage *ui;
};


#endif // ROBOTARMPAGE_H
