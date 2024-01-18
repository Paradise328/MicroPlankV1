/********************************************************************************
** Form generated from reading UI file 'RobotArmPage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROBOTARMPAGE_H
#define UI_ROBOTARMPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RobotArmPage
{
public:
    QPushButton *disableSpeed;
    QPushButton *joint3CCWslow;
    QLabel *label_4;
    QPushButton *enableSpeed;
    QLabel *label_5;
    QLabel *label_8;
    QPushButton *joint2CCWslow;
    QLabel *label_9;
    QLabel *label_3;
    QPushButton *enableTorque;
    QLabel *label;
    QPushButton *initTorque;
    QPushButton *joint2CWslow;
    QPushButton *joint1CCWslow;
    QLabel *label_2;
    QPushButton *joint3CWslow;
    QLabel *label_7;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *FastUpButton;
    QSpacerItem *verticalSpacer;
    QPushButton *SlowUpButton;
    QSpacerItem *verticalSpacer_3;
    QPushButton *SlowDownButton;
    QSpacerItem *verticalSpacer_2;
    QPushButton *FastDownButton;
    QPushButton *setBTorqueSlope;
    QLineEdit *setATorqueSlopeValue;
    QPushButton *joint2CCWfast;
    QCheckBox *setBrake;
    QLabel *label_6;
    QPushButton *setCTorqueSlope;
    QLineEdit *setBTorqueSlopeValue;
    QPushButton *joint1CWfast;
    QLineEdit *setCTorqueSlopeValue;
    QPushButton *disableTorque;
    QPushButton *setATorqueSlope;
    QPushButton *initSpeed;
    QPushButton *joint1CCWfast;
    QPushButton *joint3CWfast;
    QPushButton *joint1CWslow;
    QPushButton *joint2CWfast;
    QPushButton *joint3CCWfast;

    void setupUi(QWidget *RobotArmPage)
    {
        if (RobotArmPage->objectName().isEmpty())
            RobotArmPage->setObjectName(QStringLiteral("RobotArmPage"));
        RobotArmPage->resize(960, 600);
        disableSpeed = new QPushButton(RobotArmPage);
        disableSpeed->setObjectName(QStringLiteral("disableSpeed"));
        disableSpeed->setGeometry(QRect(810, 140, 111, 25));
        joint3CCWslow = new QPushButton(RobotArmPage);
        joint3CCWslow->setObjectName(QStringLiteral("joint3CCWslow"));
        joint3CCWslow->setGeometry(QRect(440, 250, 89, 25));
        label_4 = new QLabel(RobotArmPage);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(570, 190, 67, 17));
        enableSpeed = new QPushButton(RobotArmPage);
        enableSpeed->setObjectName(QStringLiteral("enableSpeed"));
        enableSpeed->setGeometry(QRect(688, 140, 111, 25));
        label_5 = new QLabel(RobotArmPage);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(570, 220, 67, 17));
        label_8 = new QLabel(RobotArmPage);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(60, 90, 181, 20));
        joint2CCWslow = new QPushButton(RobotArmPage);
        joint2CCWslow->setObjectName(QStringLiteral("joint2CCWslow"));
        joint2CCWslow->setGeometry(QRect(440, 220, 89, 25));
        label_9 = new QLabel(RobotArmPage);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(600, 90, 181, 20));
        label_3 = new QLabel(RobotArmPage);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(310, 360, 181, 20));
        enableTorque = new QPushButton(RobotArmPage);
        enableTorque->setObjectName(QStringLiteral("enableTorque"));
        enableTorque->setGeometry(QRect(698, 410, 111, 25));
        label = new QLabel(RobotArmPage);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(280, 20, 351, 20));
        initTorque = new QPushButton(RobotArmPage);
        initTorque->setObjectName(QStringLiteral("initTorque"));
        initTorque->setGeometry(QRect(580, 410, 111, 25));
        joint2CWslow = new QPushButton(RobotArmPage);
        joint2CWslow->setObjectName(QStringLiteral("joint2CWslow"));
        joint2CWslow->setGeometry(QRect(670, 220, 89, 25));
        joint1CCWslow = new QPushButton(RobotArmPage);
        joint1CCWslow->setObjectName(QStringLiteral("joint1CCWslow"));
        joint1CCWslow->setGeometry(QRect(440, 190, 89, 25));
        label_2 = new QLabel(RobotArmPage);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(310, 140, 181, 20));
        joint3CWslow = new QPushButton(RobotArmPage);
        joint3CWslow->setObjectName(QStringLiteral("joint3CWslow"));
        joint3CWslow->setGeometry(QRect(670, 250, 89, 25));
        label_7 = new QLabel(RobotArmPage);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(310, 390, 161, 17));
        layoutWidget = new QWidget(RobotArmPage);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(50, 160, 82, 311));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        FastUpButton = new QPushButton(layoutWidget);
        FastUpButton->setObjectName(QStringLiteral("FastUpButton"));

        verticalLayout->addWidget(FastUpButton);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        SlowUpButton = new QPushButton(layoutWidget);
        SlowUpButton->setObjectName(QStringLiteral("SlowUpButton"));

        verticalLayout->addWidget(SlowUpButton);

        verticalSpacer_3 = new QSpacerItem(20, 120, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_3);

        SlowDownButton = new QPushButton(layoutWidget);
        SlowDownButton->setObjectName(QStringLiteral("SlowDownButton"));

        verticalLayout->addWidget(SlowDownButton);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        FastDownButton = new QPushButton(layoutWidget);
        FastDownButton->setObjectName(QStringLiteral("FastDownButton"));

        verticalLayout->addWidget(FastDownButton);

        setBTorqueSlope = new QPushButton(RobotArmPage);
        setBTorqueSlope->setObjectName(QStringLiteral("setBTorqueSlope"));
        setBTorqueSlope->setGeometry(QRect(670, 480, 71, 25));
        setATorqueSlopeValue = new QLineEdit(RobotArmPage);
        setATorqueSlopeValue->setObjectName(QStringLiteral("setATorqueSlopeValue"));
        setATorqueSlopeValue->setGeometry(QRect(750, 450, 113, 25));
        joint2CCWfast = new QPushButton(RobotArmPage);
        joint2CCWfast->setObjectName(QStringLiteral("joint2CCWfast"));
        joint2CCWfast->setGeometry(QRect(340, 220, 89, 25));
        setBrake = new QCheckBox(RobotArmPage);
        setBrake->setObjectName(QStringLiteral("setBrake"));
        setBrake->setGeometry(QRect(750, 280, 161, 25));
        label_6 = new QLabel(RobotArmPage);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(570, 250, 67, 17));
        setCTorqueSlope = new QPushButton(RobotArmPage);
        setCTorqueSlope->setObjectName(QStringLiteral("setCTorqueSlope"));
        setCTorqueSlope->setGeometry(QRect(670, 510, 71, 25));
        setBTorqueSlopeValue = new QLineEdit(RobotArmPage);
        setBTorqueSlopeValue->setObjectName(QStringLiteral("setBTorqueSlopeValue"));
        setBTorqueSlopeValue->setGeometry(QRect(750, 480, 113, 25));
        joint1CWfast = new QPushButton(RobotArmPage);
        joint1CWfast->setObjectName(QStringLiteral("joint1CWfast"));
        joint1CWfast->setGeometry(QRect(780, 190, 89, 25));
        setCTorqueSlopeValue = new QLineEdit(RobotArmPage);
        setCTorqueSlopeValue->setObjectName(QStringLiteral("setCTorqueSlopeValue"));
        setCTorqueSlopeValue->setGeometry(QRect(750, 510, 113, 25));
        disableTorque = new QPushButton(RobotArmPage);
        disableTorque->setObjectName(QStringLiteral("disableTorque"));
        disableTorque->setGeometry(QRect(820, 410, 111, 25));
        setATorqueSlope = new QPushButton(RobotArmPage);
        setATorqueSlope->setObjectName(QStringLiteral("setATorqueSlope"));
        setATorqueSlope->setGeometry(QRect(670, 450, 71, 25));
        initSpeed = new QPushButton(RobotArmPage);
        initSpeed->setObjectName(QStringLiteral("initSpeed"));
        initSpeed->setGeometry(QRect(570, 140, 111, 25));
        joint1CCWfast = new QPushButton(RobotArmPage);
        joint1CCWfast->setObjectName(QStringLiteral("joint1CCWfast"));
        joint1CCWfast->setGeometry(QRect(340, 190, 89, 25));
        joint3CWfast = new QPushButton(RobotArmPage);
        joint3CWfast->setObjectName(QStringLiteral("joint3CWfast"));
        joint3CWfast->setGeometry(QRect(780, 250, 89, 25));
        joint1CWslow = new QPushButton(RobotArmPage);
        joint1CWslow->setObjectName(QStringLiteral("joint1CWslow"));
        joint1CWslow->setGeometry(QRect(670, 190, 89, 25));
        joint2CWfast = new QPushButton(RobotArmPage);
        joint2CWfast->setObjectName(QStringLiteral("joint2CWfast"));
        joint2CWfast->setGeometry(QRect(780, 220, 89, 25));
        joint3CCWfast = new QPushButton(RobotArmPage);
        joint3CCWfast->setObjectName(QStringLiteral("joint3CCWfast"));
        joint3CCWfast->setGeometry(QRect(340, 250, 89, 25));

        retranslateUi(RobotArmPage);

        QMetaObject::connectSlotsByName(RobotArmPage);
    } // setupUi

    void retranslateUi(QWidget *RobotArmPage)
    {
        RobotArmPage->setWindowTitle(QApplication::translate("RobotArmPage", "Form", Q_NULLPTR));
        disableSpeed->setText(QApplication::translate("RobotArmPage", "disable", Q_NULLPTR));
        joint3CCWslow->setText(QApplication::translate("RobotArmPage", "ccw3<", Q_NULLPTR));
        label_4->setText(QApplication::translate("RobotArmPage", "JOINT1", Q_NULLPTR));
        enableSpeed->setText(QApplication::translate("RobotArmPage", "enable_speed", Q_NULLPTR));
        label_5->setText(QApplication::translate("RobotArmPage", "JOINT2", Q_NULLPTR));
        label_8->setText(QApplication::translate("RobotArmPage", "Lift Column", Q_NULLPTR));
        joint2CCWslow->setText(QApplication::translate("RobotArmPage", "ccw2<", Q_NULLPTR));
        label_9->setText(QApplication::translate("RobotArmPage", "Robot Arm", Q_NULLPTR));
        label_3->setText(QApplication::translate("RobotArmPage", "torque compensate mode", Q_NULLPTR));
        enableTorque->setText(QApplication::translate("RobotArmPage", "enable_torque", Q_NULLPTR));
        label->setText(QApplication::translate("RobotArmPage", "Robot Arm and Lift Conlumn Control", Q_NULLPTR));
        initTorque->setText(QApplication::translate("RobotArmPage", "init_torque", Q_NULLPTR));
        joint2CWslow->setText(QApplication::translate("RobotArmPage", ">cw2", Q_NULLPTR));
        joint1CCWslow->setText(QApplication::translate("RobotArmPage", "ccw1<", Q_NULLPTR));
        label_2->setText(QApplication::translate("RobotArmPage", "speed control mode", Q_NULLPTR));
        joint3CWslow->setText(QApplication::translate("RobotArmPage", ">cw3", Q_NULLPTR));
        label_7->setText(QApplication::translate("RobotArmPage", "set torque slope sdo:", Q_NULLPTR));
        FastUpButton->setText(QApplication::translate("RobotArmPage", "FastUp", Q_NULLPTR));
        SlowUpButton->setText(QApplication::translate("RobotArmPage", "SlowUp", Q_NULLPTR));
        SlowDownButton->setText(QApplication::translate("RobotArmPage", "SlowDown", Q_NULLPTR));
        FastDownButton->setText(QApplication::translate("RobotArmPage", "FastDown", Q_NULLPTR));
        setBTorqueSlope->setText(QApplication::translate("RobotArmPage", "set B", Q_NULLPTR));
        joint2CCWfast->setText(QApplication::translate("RobotArmPage", "ccw2<<", Q_NULLPTR));
        setBrake->setText(QApplication::translate("RobotArmPage", "enable torque control", Q_NULLPTR));
        label_6->setText(QApplication::translate("RobotArmPage", "JOINT3", Q_NULLPTR));
        setCTorqueSlope->setText(QApplication::translate("RobotArmPage", "set C", Q_NULLPTR));
        joint1CWfast->setText(QApplication::translate("RobotArmPage", ">>cw1", Q_NULLPTR));
        disableTorque->setText(QApplication::translate("RobotArmPage", "disable", Q_NULLPTR));
        setATorqueSlope->setText(QApplication::translate("RobotArmPage", "set A", Q_NULLPTR));
        initSpeed->setText(QApplication::translate("RobotArmPage", "init_speed", Q_NULLPTR));
        joint1CCWfast->setText(QApplication::translate("RobotArmPage", "ccw1<<", Q_NULLPTR));
        joint3CWfast->setText(QApplication::translate("RobotArmPage", ">>cw3", Q_NULLPTR));
        joint1CWslow->setText(QApplication::translate("RobotArmPage", ">cw1", Q_NULLPTR));
        joint2CWfast->setText(QApplication::translate("RobotArmPage", ">>cw2", Q_NULLPTR));
        joint3CCWfast->setText(QApplication::translate("RobotArmPage", "ccw3<<", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class RobotArmPage: public Ui_RobotArmPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROBOTARMPAGE_H
