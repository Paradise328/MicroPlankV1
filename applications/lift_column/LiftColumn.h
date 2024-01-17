#ifndef LIFTCOLUMN_H
#define LIFTCOLUMN_H
#include "lifting.h"
#include <QMainWindow>

namespace Ui {
class LiftColumn;
}

class LiftColumn : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiftColumn(QWidget *parent = 0);
    ~LiftColumn();
    Lifting *lp=nullptr;


private slots:
    void on_FastUpButton_released();

    void on_FastUpButton_pressed();

    void on_SlowUpButton_pressed();

    void on_SlowUpButton_released();

    void on_SlowDownButton_pressed();

    void on_SlowDownButton_released();

    void on_FastDownButton_pressed();

    void on_FastDownButton_released();

private:
    Ui::LiftColumn *ui;
};

#endif // LIFTCOLUMN_H
