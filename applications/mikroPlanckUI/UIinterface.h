#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#pragma once
#include <QObject>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <time.h>
#include <unistd.h>
#include <QMetaObject>
#include <QColor>
#include "../../lib/robot_arm/robotarm.h"
#include "../../lib/lift_column/lifting.h"
#include "../../lib/light_board/lightboard.h"


typedef enum UI_Button_e
{
    UI_Button_Gray=0x00,
    UI_Button_Default=0x01,
    UI_Button_Highlight=0x02
}UI_Button_e;

class UIinterface: public QObject
{
    // UI Interface array
    //
    // [0]setSpeedLevel--1/2/3/4/5
    // [1]setKinematicsState--0 forwardkinematics/1 inversekinematics
    // [2]setLockState--0 disable/1 enable
    // [3]setStepState--0 stepforward/1 stepbackward
    // [4]axis1SoftLimit--0 normal/1 abnormal
    // [5]axis2SoftLimit--0 normal/1 abnormal
    // [6]axis3SoftLimit--0 normal/1 abnormal
    // [7]safety--0 safe/1 danger
    // [8]conserve1
    // [9]conserve2

    Q_OBJECT

    Q_PROPERTY(int* uiSignal READ getUISignal WRITE setUISignal NOTIFY uiSignalChanged )
    Q_PROPERTY(int level  READ getlevel WRITE setlevel NOTIFY LevelChanged)
<<<<<<< HEAD
=======
//    Q_PROPERTY(bool handleEn  READ gethandleEn WRITE sethandleEn NOTIFY handleEnChanged)
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202

    public:


    UIinterface(QQmlApplicationEngine &eng);
//    typedef enum _Pages_e
//    {
//        before_opreation=0x00,
//        during_opreation=0x01
//    }Pages_e;
//    Q_ENUM(Pages_e)

    Q_INVOKABLE void  setUISignal(int* uiSignal);
    Q_INVOKABLE void  setCurrentPage(int current_page_num);
    Q_INVOKABLE int  systemStart();
    Q_INVOKABLE void  systemStop();
    int* getUISignal();
    Q_INVOKABLE int getSignal(int index);
    void SetInstrumentType_L(QString type);//设置左侧末端器械类型
    void SetInstrumentType_R(QString type);//设置右侧末端器械类型

    void SetButtonSta(QObject* btn,UI_Button_e btnsta);

    Q_INVOKABLE void setlevel(int level);
    Q_INVOKABLE int getlevel(void);
    Q_INVOKABLE void enableRobotArm();
    Q_INVOKABLE void disableRobotArm();

    Q_INVOKABLE void liftingBrake();
    Q_INVOKABLE void liftingFastup();

    Q_INVOKABLE void liftingSlowup();

    Q_INVOKABLE void liftingFastdown();

    Q_INVOKABLE void liftingSlowdown();

<<<<<<< HEAD
=======
    void SetHandle_L_En(bool en);

    void SetHandle_R_En(bool en);

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    void SetLifting(Lifting *lfp);
    void SetLightBoard(LightBoard *lgp);
    int  GetCurrentPage(void);

public Q_SLOTS:
    void onRobotArmEnable_Clicked();
    void onRobotArmDisable_Clicked();
    void onButton_InstrumentInstalled_Clicked();
    void onButton_ReleaseInstrument_Clicked();
<<<<<<< HEAD
=======
    void onButton_CaliGimbalL_Clicked();
    void onButton_CaliGimbalR_Clicked();
    void onButton_CaliInstrumentL_Clicked();
    void onButton_CaliInstrumentR_Clicked();
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    void onLightBoardStickChange(uint8_t sta);

    signals:
    void uiSignalChanged(int* uiSignal);
    void LevelChanged(int l);
    void CurrentChanged(int p);

<<<<<<< HEAD



    private:
=======
    private:
    bool m_handle_L_En=false;
    bool m_handle_R_En=false;
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    Lifting *m_lifting_p=nullptr;
    LightBoard *m_lightboard_p=nullptr;
    QQmlApplicationEngine &engine;
    int* my_uiSignal;
    bool ArmEnable=false;
    int level=0;
    int CurrentPage=0;
    QObject *Text_InstrumentType_L=nullptr;
    QObject *Text_InstrumentType_R=nullptr;

    QObject *Button_robotArmEnable=nullptr;
    QObject *Button_robotArmDisable=nullptr;
    QObject *Button_Fastup=nullptr;
    QObject *Button_Slowup=nullptr;
    QObject *Button_Fastdown=nullptr;
    QObject *Button_Slowdown=nullptr;
    QObject *Button_ReleaseInstrument=nullptr;
    QObject *Button_InstrumentInstalled=nullptr;
<<<<<<< HEAD
=======
    QObject *Button_CaliGimbalL=nullptr;
    QObject *Button_CaliGimbalR=nullptr;
    QObject *Button_CaliInstrumentL=nullptr;
    QObject *Button_CaliInstrumentR=nullptr;
    QObject *Button_Enable_l=nullptr;
    QObject *Button_Enable_r=nullptr;

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
};

class UIthread: public QObject
{
    Q_OBJECT
    public slots:
    void onKinematics();

};
#endif // UIINTERFACE_H
