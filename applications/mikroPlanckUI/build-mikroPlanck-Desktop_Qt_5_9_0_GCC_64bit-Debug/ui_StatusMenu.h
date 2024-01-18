/********************************************************************************
** Form generated from reading UI file 'StatusMenu.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATUSMENU_H
#define UI_STATUSMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StatusMenu
{
public:
    QRadioButton *radioButton;
    QToolButton *toolButton;
    QPushButton *pushButton;

    void setupUi(QWidget *StatusMenu)
    {
        if (StatusMenu->objectName().isEmpty())
            StatusMenu->setObjectName(QStringLiteral("StatusMenu"));
        StatusMenu->resize(960, 600);
        radioButton = new QRadioButton(StatusMenu);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(230, 150, 112, 23));
        toolButton = new QToolButton(StatusMenu);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setGeometry(QRect(170, 100, 26, 24));
        pushButton = new QPushButton(StatusMenu);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(170, 200, 89, 25));

        retranslateUi(StatusMenu);

        QMetaObject::connectSlotsByName(StatusMenu);
    } // setupUi

    void retranslateUi(QWidget *StatusMenu)
    {
        StatusMenu->setWindowTitle(QApplication::translate("StatusMenu", "Form", Q_NULLPTR));
        radioButton->setText(QApplication::translate("StatusMenu", "RadioButton", Q_NULLPTR));
        toolButton->setText(QApplication::translate("StatusMenu", "...", Q_NULLPTR));
        pushButton->setText(QApplication::translate("StatusMenu", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class StatusMenu: public Ui_StatusMenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATUSMENU_H
