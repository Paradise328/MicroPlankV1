/********************************************************************************
** Form generated from reading UI file 'MikroPlanck.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIKROPLANCK_H
#define UI_MIKROPLANCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MikroPlanck
{
public:
    QWidget *centralWidget;
    QStackedWidget *ModeSelect;
    QWidget *page;
    QWidget *page_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *systemStatusWidget;
    QPushButton *robotArmWidget;
    QPushButton *endEffectorWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MikroPlanck)
    {
        if (MikroPlanck->objectName().isEmpty())
            MikroPlanck->setObjectName(QStringLiteral("MikroPlanck"));
        MikroPlanck->resize(1280, 800);
        centralWidget = new QWidget(MikroPlanck);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ModeSelect = new QStackedWidget(centralWidget);
        ModeSelect->setObjectName(QStringLiteral("ModeSelect"));
        ModeSelect->setGeometry(QRect(39, 70, 960, 600));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        ModeSelect->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        ModeSelect->addWidget(page_2);
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(80, 10, 371, 27));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        systemStatusWidget = new QPushButton(widget);
        systemStatusWidget->setObjectName(QStringLiteral("systemStatusWidget"));

        horizontalLayout->addWidget(systemStatusWidget);

        robotArmWidget = new QPushButton(widget);
        robotArmWidget->setObjectName(QStringLiteral("robotArmWidget"));

        horizontalLayout->addWidget(robotArmWidget);

        endEffectorWidget = new QPushButton(widget);
        endEffectorWidget->setObjectName(QStringLiteral("endEffectorWidget"));

        horizontalLayout->addWidget(endEffectorWidget);

        MikroPlanck->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MikroPlanck);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1280, 22));
        MikroPlanck->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MikroPlanck);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MikroPlanck->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MikroPlanck);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MikroPlanck->setStatusBar(statusBar);

        retranslateUi(MikroPlanck);

        ModeSelect->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MikroPlanck);
    } // setupUi

    void retranslateUi(QMainWindow *MikroPlanck)
    {
        MikroPlanck->setWindowTitle(QApplication::translate("MikroPlanck", "MikroPlanck", Q_NULLPTR));
        systemStatusWidget->setText(QApplication::translate("MikroPlanck", "system status", Q_NULLPTR));
        robotArmWidget->setText(QApplication::translate("MikroPlanck", "robot arm", Q_NULLPTR));
        endEffectorWidget->setText(QApplication::translate("MikroPlanck", "end effector", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MikroPlanck: public Ui_MikroPlanck {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIKROPLANCK_H
