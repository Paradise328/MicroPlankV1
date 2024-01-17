#ifndef STATUSMENU_H
#define STATUSMENU_H

#include <QWidget>
#include "../../lib/GalilControl/GalilControl.h"
#include "Read_Handle_Data.h"

namespace Ui {
class StatusMenu;
}

class StatusMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMenu(QWidget *parent = 0);
    ~StatusMenu();
signals:
    void cali_l_instrument();
    void cali_l_gimbal();
    void cali_r_instrument();
    void cali_r_gimbal();
private slots:
    void on_leftInstrumentCali_clicked();

    void on_rightInstrumentCali_clicked();

    void on_leftGimbalCali_clicked();

    void on_rightGimbalCali_clicked();

    void on_resetViper_clicked();

private:
    Ui::StatusMenu *ui;
};

#endif // STATUSMENU_H
