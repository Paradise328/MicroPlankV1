#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#pragma once
#include <QObject>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QtQml>
#include <time.h>
#include <unistd.h>
#include <QMetaObject>
#include <QColor>
#include <QSignalSpy>
#include <QDebug>
#include <QThread>
#include <cstring>
#include <QVariant>
#include "Modules/SystemUtilsModule/SystemUtils.h"
//#include "Modules/MasterModule/MasterConsole.h"
#include "Modules/MsgModule/messagequeue.h"
#include "Modules/LoggerModule/easylogging++.h"

extern QVector<int> joint_status_mode;


typedef struct
{
    bool MainHandSta;
    bool LifingSta;
    bool ArmSta;
    bool LeftfinialSta;
    bool RightfinialSta;
}Peripheral_Sta_t;

typedef enum UI_Button_e
{
    UI_Button_Gray=0x00,
    UI_Button_Default=0x01,
    UI_Button_Highlight=0x02
}UI_Button_e;


class UIinterface: public QObject
{
    /* UI Interface array
        [0]setSpeedLevel--1/2/3/4/5
        [1]setKinematicsState--0 forwardkinematics/1 inversekinematics
        [2]setLockState--0 disable/1 enable
        [3]setStepState--0 stepforward/1 stepbackward
        [4]axis1SoftLimit--0 normal/1 abnormal
        [5]axis2SoftLimit--0 normal/1 abnormal
        [6]axis3SoftLimit--0 normal/1 abnormal
        [7]safety--0 safe/1 danger
        [8]conserve1
        [9]conserve2
    */

    Q_OBJECT

    Q_PROPERTY(int* uiSignal READ getUISignal WRITE setUISignal NOTIFY uiSignalChanged )
    Q_PROPERTY(int level  READ getlevel WRITE setlevel NOTIFY LevelChanged)
//    Q_PROPERTY(bool handleEn  READ gethandleEn WRITE sethandleEn NOTIFY handleEnChanged)

public:

    UIinterface(QGuiApplication &app, MessageQueue&  messagePool);

    Q_INVOKABLE void  setUISignal(int* uiSignal);
    Q_INVOKABLE void  setCurrentPage(int current_page_num);
    Q_INVOKABLE int   systemStart();
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

//    Q_INVOKABLE void setMotorDriverStatus();

    Q_INVOKABLE void startSystem();

    Q_INVOKABLE void setRobotControlMode(int action);

    Q_INVOKABLE void setInstrumentAngle(int initInstrumentAngle);

    void setConnections();

    void setQMLComponent();

    void initAllData();

    void setInitStatus();

    void SetHandle_L_En(bool en);

    void SetHandle_R_En(bool en);

//    void SetReadDataThread(MasterConsole *rdp);
    int  GetCurrentPage(void);

    void SetMainhandStaimage(int sta);
    void SetLiftingStaimage(int sta);
    void SetArmStaimage(int sta);
    void SetLeftfinialStaimage(int sta);
    void SetRightfinialStaimage(int sta);

    void setMasterConsoleCheckString(QString s);
    void setMasterConsoleStatus(QString statusWord);
    void SetLiftingcheckString(QString s);
    void SetArmcheckString(QString s);
    void SetLeftfinialcheckString(QString s);
    void SetRightfinialcheckString(QString s);

    void setModulesStatusPopUpWindow();


    void SetReserveButtonSta(int sta);
    void SetAxisLimit(QString Axis,QString limitsta);

    void dealWithMsg();
    void GetAmMsg(Message_Inner_T msg);

public Q_SLOTS:
    void onRobotArmEnable_Clicked();
//    void onRobotArmDisable_Clicked();
    void onButton_InstrumentInstalled_Clicked();
    void onButton_ReleaseInstrument_Clicked();
    void onButton_CaliGimbalL_Clicked();
    void onButton_CaliGimbalR_Clicked();
    void onButton_CaliInstrumentL_Clicked();
    void onButton_CaliInstrumentR_Clicked();
    void onButton_PowerOff();
    void resetButton_Calibration();

    void onButton_Fastup_Pressed();
    void onButton_Fastup_Released();
    void onButton_Slowup_Pressed();
    void onButton_Slowup_Released();

    void onButton_Fastdown_Pressed();
    void onButton_Fastdown_Released();
    void onButton_Slowdown_Pressed();
    void onButton_Slowdown_Released();

    void onLightBoardStickChange(uint8_t sta);



signals:

    void startWholeSystemSignal();

    void uiSignalChanged(int* uiSignal);
    void LevelChanged(int  l);
    void CurrentChanged(int p);
    void SendMsgSignal(const Message_Inner_T msg);
    void DealMsgSignal();

private:
    bool                    m_HandleEnable_L = false;
    bool                    m_HandleEnable_R = false;
    bool                    m_ArmEnable = false;
    int                     m_Level = 0;
    int                     m_CurrentPage=0;
    mutable int             m_PrevPage = 0;
    mutable int             m_GimbalCalibrationFlag_L = 0;
    mutable int             m_GimbalCalibrationFlag_R = 0;
    mutable int             m_InstrumentCalibrationFlag_L = 0;
    mutable int             m_InstrumentCalibrationFlag_R = 0;

    mutable int             m_systemMode;

//    MasterConsole           *m_MasterConsole = nullptr;
//    SlaveDevice             *m_SlaveDevice  = nullptr;

    QGuiApplication         &m_app;
    MessageQueue&            m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock  m_MsgGottenRWLock;

    QQmlApplicationEngine   m_Engine;
    QmlLanguage             m_qmlLanguage = QmlLanguage(m_app, m_Engine);
    QUrl                    m_url = QUrl(QStringLiteral("qrc:/main.qml"))  ;

    QObject                 *m_PO_Text_InstrumentType_L = nullptr;
    QObject                 *m_PO_Text_InstrumentType_R = nullptr;
    QObject                 *m_PO_Text_InstrumentInstallSta_L = nullptr;
    QObject                 *m_PO_Text_InstrumentInstallSta_R = nullptr;
    QObject                 *m_PO_Text_UsedTime_L = nullptr;
    QObject                 *m_PO_Text_UsedTime_R = nullptr;

    QObject                 *m_PO_Button_RobotArmEnable = nullptr;

  //  QObject                 *m_Button_RobotArmDisable = nullptr;
    QObject                 *m_PO_Button_Fastup = nullptr;
    QObject                 *m_PO_Button_Slowup = nullptr;
    QObject                 *m_PO_Button_Fastdown = nullptr;
    QObject                 *m_PO_Button_Slowdown = nullptr;
//    QObject                 *m_Button_ReleaseInstrument = nullptr;
//    QObject                 *m_Button_InstrumentInstalled = nullptr;
    QObject                 *m_PO_Button_CaliGimbal_L = nullptr;
    QObject                 *m_PO_Button_CaliGimbal_R = nullptr;
    QObject                 *m_PO_Button_CaliInstrument_L = nullptr;
    QObject                 *m_PO_Button_CaliInstrument_R = nullptr;
    QObject                 *m_PowerOffButton = nullptr;
    // QObject                 *m_PO_GoToOperation = nullptr;
    // QObject                 *m_IO_BackToPrepare = nullptr;

    QObject                 *m_IO_Text_Enable_L = nullptr;
    QObject                 *m_IO_Text_Enable_R = nullptr;

    QObject                 *m_IO_Text_InstrumentType_L = nullptr;
    QObject                 *m_IO_Text_InstrumentType_R = nullptr;

    QObject                 *m_IO_Text_UsedTime_L = nullptr;
    QObject                 *m_IO_Text_UsedTime_R = nullptr;

    QObject                 *m_IO_Image_InstrumentAxisA_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisB_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisC_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisD_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisE_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisF_L_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisG_L_Front = nullptr;

    QObject                 *m_IO_Image_InstrumentAxisA_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisB_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisC_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisD_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisE_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisF_L_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisG_L_Back = nullptr;

    QObject                 *m_IO_Image_InstrumentAxisA_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisB_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisC_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisD_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisE_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisF_R_Front = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisG_R_Front = nullptr;

    QObject                 *m_IO_Image_InstrumentAxisA_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisB_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisC_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisD_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisE_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisF_R_Back = nullptr;
    QObject                 *m_IO_Image_InstrumentAxisG_R_Back = nullptr;

    QObject                 *m_IO_Image_Level1 = nullptr;
    QObject                 *m_IO_Image_Level2 = nullptr;
    QObject                 *m_IO_Image_Level3 = nullptr;
    QObject                 *m_IO_Image_Level4 = nullptr;

    QObject                 *m_INIT_Image_MasterConsoleStatus = nullptr;
    QObject                 *m_INIT_Image_LiftingStatus = nullptr;
    QObject                 *m_INIT_Image_ArmStatus = nullptr;
    QObject                 *m_INIT_Image_LeftFinialStatus = nullptr;
    QObject                 *m_INIT_Image_RightFinialStatus = nullptr;

    QObject                 *m_INIT_Text_MasterConsoleCheck = nullptr;
    QObject                 *m_INIT_Text_LiftingCheck = nullptr;
    QObject                 *m_INIT_Text_ArmCheck = nullptr;
    QObject                 *m_INIT_Text_LeftFinialCheck = nullptr;
    QObject                 *m_INIT_Text_RightFinialCheck = nullptr;

    QObject                 *m_INIT_Button_Reserve = nullptr;


    int*                    m_UiSignal;

    void                    SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
};

#endif // UIINTERFACE_H
