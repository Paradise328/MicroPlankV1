/********************************************************************************
** Form generated from reading UI file 'EndEffector.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENDEFFECTOR_H
#define UI_ENDEFFECTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EndEffector
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *EndEffector)
    {
        if (EndEffector->objectName().isEmpty())
            EndEffector->setObjectName(QStringLiteral("EndEffector"));
        EndEffector->resize(960, 600);
        pushButton = new QPushButton(EndEffector);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(150, 180, 89, 25));

        retranslateUi(EndEffector);

        QMetaObject::connectSlotsByName(EndEffector);
    } // setupUi

    void retranslateUi(QWidget *EndEffector)
    {
        EndEffector->setWindowTitle(QApplication::translate("EndEffector", "Form", Q_NULLPTR));
        pushButton->setText(QApplication::translate("EndEffector", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EndEffector: public Ui_EndEffector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENDEFFECTOR_H
