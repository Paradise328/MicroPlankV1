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
    explicit EndEffector(QWidget *parent = 0);
    ~EndEffector();

private:
    Ui::EndEffector *ui;
};

#endif // ENDEFFECTOR_H
