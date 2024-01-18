#ifndef ENDEFFECTOR_H
#define ENDEFFECTOR_H

#include <QWidget>

namespace Ui {
class EndEffector;
}

class EndEffector : public QWidget
{
    Q_OBJECT

public:
    explicit EndEffector(QWidget *parent = nullptr);
    ~EndEffector();

private slots:
    void record_data();
    void on_initSpeed_clicked();

    void on_enableSpeed_clicked();

    void on_disableSpeed_clicked();

    void on_btnStartSpeed_clicked();

    void on_btnStopSpeed_clicked();

    void on_btnRecord_clicked();

private:
    Ui::EndEffector *ui;
};

#endif // ENDEFFECTOR_H
