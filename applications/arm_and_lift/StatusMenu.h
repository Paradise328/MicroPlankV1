#ifndef STATUSMENU_H
#define STATUSMENU_H

#include <QWidget>

namespace Ui {
class StatusMenu;
}

class StatusMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMenu(QWidget *parent = 0);
    ~StatusMenu();

private:
    Ui::StatusMenu *ui;
};

#endif // STATUSMENU_H
