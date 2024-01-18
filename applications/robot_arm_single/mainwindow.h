#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
extern int robotarm_enable_btn;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setArmEnableStatus(int);

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

    void on_toggleServo_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
