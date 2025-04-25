#include "UIinterface.h"

UIinterface::UIinterface(QGuiApplication &app,MessageQueue&  messagePool) :m_app(app),m_messagePool(messagePool)
{
     m_Engine.rootContext()->setContextProperty("qmlLanguage", &m_qmlLanguage);
     m_Engine.rootContext()->setContextProperty("UIinterface", this);
//     qmlRegisterUncreatableType<RobotControlAction_E>("RobotControlAction_E", 1, 0, "RobotControlAction", "Cannot create RobotControlAction in QML");


     m_url = QUrl(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&m_Engine, &QQmlApplicationEngine::objectCreated,
                         &m_app, [this](QObject *obj, const QUrl &objUrl) {
            if (!obj && m_url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

     m_Engine.load(m_url);

     std::cout << "Open UI " << std::endl;

     setQMLComponent();
     setConnections();
     setInitStatus();

     connect(this,&UIinterface::DealMsgSignal,this,&UIinterface::dealWithMsg);

}

void UIinterface::setQMLComponent()
{
    //QML Components: Check
    this->m_INIT_Image_MasterConsoleStatus = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_mainhandsta");
    this->m_INIT_Image_LiftingStatus = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_liftingsta");
    this->m_INIT_Image_ArmStatus = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_armsta");
    this->m_INIT_Image_LeftFinialStatus = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_leftfinialsta");
    this->m_INIT_Image_RightFinialStatus = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_rightfinialsta");
    this->m_INIT_Text_MasterConsoleCheck = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_txtMasterConsoleCheck");;
    this->m_INIT_Text_LiftingCheck = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_txt_liftingcheck");;
    this->m_INIT_Text_ArmCheck = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_txt_armcheck");
    this->m_INIT_Text_LeftFinialCheck = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_txt_leftfinialcheck");
    this->m_INIT_Text_RightFinialCheck = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_txt_rightfinialcheck");

    this->m_INIT_Button_Reserve = this->m_Engine.rootObjects().first()->findChild<QObject*>("init_ContinuousBtn");


    this->m_PO_Text_InstrumentInstallSta_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtInstrumentInstallSta_L");
    this->m_PO_Text_InstrumentInstallSta_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtInstrumentInstallSta_R");
    this->m_PO_Text_InstrumentType_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtInstrumentType_L");
    this->m_PO_Text_InstrumentType_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtInstrumentType_R");
    this->m_PO_Text_UsedTime_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtUsedTime_L");
    this->m_PO_Text_UsedTime_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_txtUsedTime_R");

    this->m_PO_Button_RobotArmEnable = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_robotArmEnable");
  //  this->m_Button_RobotArmDisable = this->m_Engine.rootObjects().first()->findChild<QObject*>("robotArmDisable");

    this->m_PO_Button_Fastup = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnFastUp");
    this->m_PO_Button_Slowup = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnSlowUp");
    this->m_PO_Button_Fastdown = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnFastDown");
    this->m_PO_Button_Slowdown = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnSlowDown");
  //  this->m_Button_ReleaseInstrument = this->m_Engine.rootObjects().first()->findChild<QObject*>("btnReleaseInstrument");
 //   this->m_Button_InstrumentInstalled = this->m_Engine.rootObjects().first()->findChild<QObject*>("btnInstrumentInstalled");

    this->m_PO_Button_CaliGimbal_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnCaliGimbalL"); //存储一个指向 QML 中按钮对象的指针， 保存从 QML 中找到的按钮
    this->m_PO_Button_CaliGimbal_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnCaliGimbalR");
    this->m_PO_Button_CaliInstrument_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnCaliInstrumentL");
    this->m_PO_Button_CaliInstrument_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnCaliInstrumentR");//调用右侧末端归零qml

    this->m_PO_GoToOperation= this->m_Engine.rootObjects().first()->findChild<QObject*>("po_btnEnterOpreation");
    this->m_IO_BackToPrepare= this->m_Engine.rootObjects().first()->findChild<QObject*>("io_btnReturnPreOpreation");


    this->m_PowerOffButton = this->m_Engine.rootObjects().first()->findChild<QObject*>("btn_sureShutDownSystem");

    this->m_IO_Text_Enable_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtEnable_L");
    this->m_IO_Text_Enable_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtEnable_R");

    this->m_IO_Text_InstrumentType_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtInstrumentType_L");
    this->m_IO_Text_InstrumentType_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtInstrumentType_R");

    this->m_IO_Text_UsedTime_L = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtUsedTime_L");
    this->m_IO_Text_UsedTime_R = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_txtUsedTime_R");


    this->m_IO_Image_Level1 = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_level1");
    this->m_IO_Image_Level2 = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_level2");
    this->m_IO_Image_Level3 = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_level3");
    this->m_IO_Image_Level4 = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_level4");

     this->m_IO_Image_InstrumentAxisA_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_A_Front_l");
     this->m_IO_Image_InstrumentAxisB_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_B_Front_l");
     this->m_IO_Image_InstrumentAxisC_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_C_l");
     this->m_IO_Image_InstrumentAxisD_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_D_l");
     this->m_IO_Image_InstrumentAxisE_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_E_Front_l");
     this->m_IO_Image_InstrumentAxisF_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_F_Front_l");
     this->m_IO_Image_InstrumentAxisG_L_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_G_Front_l");

     this->m_IO_Image_InstrumentAxisA_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_A_Back_l");
     this->m_IO_Image_InstrumentAxisB_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_B_Back_l");
//     this->m_IO_Image_InstrumentAxisC_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_C_l");
//     this->m_IO_Image_InstrumentAxisD_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_D_l");
     this->m_IO_Image_InstrumentAxisE_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_E_Back_l");
     this->m_IO_Image_InstrumentAxisF_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_F_Back_l");
     this->m_IO_Image_InstrumentAxisG_L_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_G_Back_l");


     this->m_IO_Image_InstrumentAxisA_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_A_Front_r");
     this->m_IO_Image_InstrumentAxisB_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_B_Front_r");
     this->m_IO_Image_InstrumentAxisC_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_C_r");
     this->m_IO_Image_InstrumentAxisD_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_D_r");
     this->m_IO_Image_InstrumentAxisE_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_E_Front_r");
     this->m_IO_Image_InstrumentAxisF_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_F_Front_r");
     this->m_IO_Image_InstrumentAxisG_R_Front = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_G_Front_r");

     this->m_IO_Image_InstrumentAxisA_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_A_Back_r");
     this->m_IO_Image_InstrumentAxisB_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_B_Back_r");
//     this->m_IO_Image_InstrumentAxisC_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("picConditionC_r");
//     this->m_IO_Image_InstrumentAxisD_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("picConditionD_r");
     this->m_IO_Image_InstrumentAxisE_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_E_Back_r");
     this->m_IO_Image_InstrumentAxisF_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_F_Back_r");
     this->m_IO_Image_InstrumentAxisG_R_Back = this->m_Engine.rootObjects().first()->findChild<QObject*>("io_picCondition_Axis_G_Back_r");
}


void UIinterface::setConnections()
{
    connect(this,&UIinterface::DealMsgSignal,this,&UIinterface::dealWithMsg);

    connect(this->m_PO_Button_RobotArmEnable,SIGNAL(clicked()),this,SLOT(onRobotArmEnable_Clicked()));//进入csp模式
   // connect(this->m_Button_RobotArmDisable,SIGNAL(clicked()),this,SLOT(onRobotArmDisable_Clicked()));
    connect(this->m_PO_Button_CaliGimbal_L,SIGNAL(clicked()),this,SLOT(onButton_CaliGimbalL_Clicked()));//左侧云台归零
    connect(this->m_PO_Button_CaliGimbal_R,SIGNAL(clicked()),this,SLOT(onButton_CaliGimbalR_Clicked()));//clicked()是接收到的信号，SLOT是一个槽函数，定义了信号被触发时的具体处理逻辑。
    connect(this->m_PO_Button_CaliInstrument_L,SIGNAL(clicked()),this,SLOT(onButton_CaliInstrumentL_Clicked()));//左侧器械归零
    connect(this->m_PO_Button_CaliInstrument_R,SIGNAL(clicked()),this,SLOT(onButton_CaliInstrumentR_Clicked()));

    connect(this->m_PO_Button_Fastup,SIGNAL(pressed()),this,SLOT(onButton_Fastup_Pressed()));
    connect(this->m_PO_Button_Fastup,SIGNAL(released()),this,SLOT(onButton_Fastup_Released()));
    connect(this->m_PO_Button_Slowup,SIGNAL(pressed()),this,SLOT(onButton_Slowup_Pressed()));
    connect(this->m_PO_Button_Slowup,SIGNAL(released()),this,SLOT(onButton_Slowup_Released()));

    connect(this->m_PO_Button_Fastdown,SIGNAL(pressed()),this,SLOT(onButton_Fastdown_Pressed()));
    connect(this->m_PO_Button_Fastdown,SIGNAL(released()),this,SLOT(onButton_Fastdown_Released()));
    connect(this->m_PO_Button_Slowdown,SIGNAL(pressed()),this,SLOT(onButton_Slowdown_Pressed()));
    connect(this->m_PO_Button_Slowdown,SIGNAL(released()),this,SLOT(onButton_Slowdown_Released()));

    //switch page
    connect(this->m_PO_GoToOperation,SIGNAL(clicked()),this,SLOT(PO_GoToOperation_Clicked()));
    connect(this->m_IO_BackToPrepare,SIGNAL(clicked()),this,SLOT(IO_BackToPrepare_Clicked()));

    connect(this->m_PowerOffButton,SIGNAL(clicked()),this,SLOT(onButton_PowerOff()));

    LOG(INFO) << "Successfully set connections in UI Interface ";
}

void UIinterface::initAllData()
{
    m_systemMode = 0;

}

void UIinterface::setInitStatus()
{
    this->m_ArmEnable = false;

    SetButtonSta(this->m_PO_Button_Fastup, UI_Button_Gray);
    SetButtonSta(this->m_PO_Button_Slowup, UI_Button_Gray);
    SetButtonSta(this->m_PO_Button_Fastdown, UI_Button_Gray);
    SetButtonSta(this->m_PO_Button_Slowdown, UI_Button_Gray);

    QMetaObject::invokeMethod(this->m_INIT_Button_Reserve, "setsta", Q_ARG(QVariant, 3));
}


void UIinterface::onRobotArmEnable_Clicked()
{
    if(m_ArmEnable==false)
    {
        m_ArmEnable=true;
        SetButtonSta(this->m_PO_Button_RobotArmEnable,UI_Button_Highlight);

        SetButtonSta(this->m_PO_Button_Fastup,UI_Button_Default);
        SetButtonSta(this->m_PO_Button_Slowup,UI_Button_Default);
        SetButtonSta(this->m_PO_Button_Fastdown,UI_Button_Default);
        SetButtonSta(this->m_PO_Button_Slowdown,UI_Button_Default);

        SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingDriverSwitch),"on");
        SendInnerMsg(Module_Inner_E::AssistDevice_Arm,static_cast<int>(AssistDevice_ArmAction_E::ArmDriverSwitch),"on");
    }
    else
    {
        m_ArmEnable=false;
        SetButtonSta(this->m_PO_Button_RobotArmEnable,UI_Button_Default);

        SetButtonSta(this->m_PO_Button_Fastup,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_Slowup,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_Fastdown,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_Slowdown,UI_Button_Gray);

        SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingDriverSwitch),"off");
        SendInnerMsg(Module_Inner_E::AssistDevice_Arm,static_cast<int>(AssistDevice_ArmAction_E::ArmDriverSwitch),"off");

    }
}

void UIinterface::startSystem()
{
    emit startWholeSystemSignal();
}

void UIinterface::setRobotControlMode(int action)
{
    std::cout << "Action in set Robot Control Mode Function: " << action << std::endl;
    switch (action) {
    case 1:
        SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::GoToHoldOnMode), "");   break;
    case 2:
        SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::GoToCollabrationMode), "");   break;
    case 3:
        SendInnerMsg(Module_Inner_E::RobotControl, static_cast<int>(RobotControlAction_E::GoToTeleOperationMode), "");   break;
    default:break;
    }
}

void UIinterface::SetMainhandStaimage(int sta)
{
    if(this->m_INIT_Image_MasterConsoleStatus!=nullptr)
    {
        QMetaObject::invokeMethod(this->m_INIT_Image_MasterConsoleStatus, "setsta",Q_ARG(QVariant, sta));
    }
}

void UIinterface::SetLiftingStaimage(int sta)
{
    if(this->m_INIT_Image_LiftingStatus!=nullptr)
    {
      //  this->Image_Liftingsta->setProperty("states","Normal");
        QMetaObject::invokeMethod(this->m_INIT_Image_LiftingStatus, "setsta",Q_ARG(QVariant, sta));
    }
}

void UIinterface::SetArmStaimage(int sta)
{
    if(this->m_INIT_Image_ArmStatus!=nullptr)
    {
     //   this->Image_Armsta->setProperty("states","Normal");
        QMetaObject::invokeMethod(this->m_INIT_Image_ArmStatus, "setsta",Q_ARG(QVariant, sta));

    }
}
void UIinterface::SetLeftfinialStaimage(int sta)
{
    if(this->m_INIT_Image_LeftFinialStatus!=nullptr)
    {
        QMetaObject::invokeMethod(this->m_INIT_Image_LeftFinialStatus, "setsta",Q_ARG(QVariant, sta));
    }
}
void UIinterface::SetRightfinialStaimage(int sta)
{
    if(this->m_INIT_Image_RightFinialStatus!=nullptr)
    {
        QMetaObject::invokeMethod(this->m_INIT_Image_RightFinialStatus, "setsta",Q_ARG(QVariant, sta));
    }
}

void UIinterface::SetReserveButtonSta(int sta)
{
    if(this->m_INIT_Button_Reserve!=nullptr)
    {
        QMetaObject::invokeMethod(this->m_INIT_Button_Reserve, "setsta",Q_ARG(QVariant, sta));
    }
}


void UIinterface::setMasterConsoleCheckString(QString s)
{
    if(this->m_INIT_Text_MasterConsoleCheck!=nullptr)
    {
        this->m_INIT_Text_MasterConsoleCheck->setProperty("text",s);
        QMetaObject::invokeMethod(this->m_INIT_Text_MasterConsoleCheck, "doLayout");
    }
}


void UIinterface::SetLiftingcheckString(QString s)
{
    if(this->m_INIT_Text_LiftingCheck!=nullptr)
    {
        this->m_INIT_Text_LiftingCheck->setProperty("text",s);
        QMetaObject::invokeMethod(this->m_INIT_Text_LiftingCheck, "doLayout");
    }
}

void UIinterface::SetArmcheckString(QString s)
{
    if(this->m_INIT_Text_ArmCheck!=nullptr)
    {
        this->m_INIT_Text_ArmCheck->setProperty("text",s);
        QMetaObject::invokeMethod(this->m_INIT_Text_ArmCheck, "doLayout");
    }
}

void UIinterface::SetLeftfinialcheckString(QString s)
{
    if(this->m_INIT_Text_LeftFinialCheck!=nullptr)
    {
        this->m_INIT_Text_LeftFinialCheck->setProperty("text",s);
        QMetaObject::invokeMethod(this->m_INIT_Text_LeftFinialCheck, "doLayout");
    }
}


void UIinterface::SetRightfinialcheckString(QString s)
{
    if(this->m_INIT_Text_RightFinialCheck!=nullptr)
    {
        this->m_INIT_Text_RightFinialCheck->setProperty("text",s);
        QMetaObject::invokeMethod(this->m_INIT_Text_RightFinialCheck, "doLayout");
    }
}


void UIinterface::onButton_CaliGimbalL_Clicked()//点击左云台归零
{
    LOG(INFO)<<"Gimbal calibration button has been clicked: Gimbal calibration left";
    if((m_GimbalCalibrationFlag_L + m_GimbalCalibrationFlag_R + m_InstrumentCalibrationFlag_L + m_InstrumentCalibrationFlag_R) == 0)
    {
        SetButtonSta(this->m_PO_Button_CaliGimbal_L,UI_Button_Highlight);
        SetButtonSta(this->m_PO_Button_CaliGimbal_R,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_R,UI_Button_Gray);
        SendInnerMsg(Module_Inner_E::RobotControl,static_cast<int>(RobotControlAction_E::StartEndJointMotorHoming),"l");
        m_GimbalCalibrationFlag_L = 1;
    }

}
void UIinterface::onButton_CaliGimbalR_Clicked()//点击右云台归零
{
    LOG(INFO)<<"Gimbal calibration button has been clicked: Gimbal calibration right";
    if((m_GimbalCalibrationFlag_L + m_GimbalCalibrationFlag_R + m_InstrumentCalibrationFlag_L + m_InstrumentCalibrationFlag_R) == 0)
    {
        SetButtonSta(this->m_PO_Button_CaliGimbal_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliGimbal_R,UI_Button_Highlight);
        SetButtonSta(this->m_PO_Button_CaliInstrument_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_R,UI_Button_Gray);
        SendInnerMsg(Module_Inner_E::RobotControl,static_cast<int>(RobotControlAction_E::StartEndJointMotorHoming),"r");
        m_GimbalCalibrationFlag_R = 1;
    }

}
void UIinterface::onButton_CaliInstrumentL_Clicked()//点击器械归零
{
    printf("instrument CaliInstrumentL\n");
    if((m_GimbalCalibrationFlag_L + m_GimbalCalibrationFlag_R + m_InstrumentCalibrationFlag_L + m_InstrumentCalibrationFlag_R) == 0)//如果其他按钮都没有被按下
    {
        SetButtonSta(this->m_PO_Button_CaliGimbal_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliGimbal_R,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_L,UI_Button_Highlight);
        SetButtonSta(this->m_PO_Button_CaliInstrument_R,UI_Button_Gray);
//        SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::INSTRUMENT_Calibration_Start),"l");
        m_InstrumentCalibrationFlag_L = 1;
    }

}
void UIinterface::onButton_CaliInstrumentR_Clicked()
{
    printf("instrument CaliInstrumentR\n");
    if((m_GimbalCalibrationFlag_L + m_GimbalCalibrationFlag_R + m_InstrumentCalibrationFlag_L + m_InstrumentCalibrationFlag_R) == 0)
    {
        SetButtonSta(this->m_PO_Button_CaliGimbal_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliGimbal_R,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_L,UI_Button_Gray);
        SetButtonSta(this->m_PO_Button_CaliInstrument_R,UI_Button_Highlight);
        SendInnerMsg(Module_Inner_E::RobotControl,static_cast<int>(RobotControlAction_E::StartEndEffectorMotorHoming),"r");
        m_InstrumentCalibrationFlag_R = 1;
    }

}

void UIinterface::resetButton_Calibration()
{
    LOG(INFO)<<"In UI Interface: reset Calibration Button";
    m_GimbalCalibrationFlag_L = 0;
    m_GimbalCalibrationFlag_R = 0;
    m_InstrumentCalibrationFlag_L = 0;
    m_InstrumentCalibrationFlag_R = 0;
    SetButtonSta(this->m_PO_Button_CaliGimbal_L,UI_Button_Default);
    SetButtonSta(this->m_PO_Button_CaliGimbal_R,UI_Button_Default);
    SetButtonSta(this->m_PO_Button_CaliInstrument_L,UI_Button_Default);
    SetButtonSta(this->m_PO_Button_CaliInstrument_R,UI_Button_Default);
}


void UIinterface::PO_GoToOperation_Clicked()
{
    qDebug()<<"Change Page from 0 to 1";
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"l");
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"r");
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::ExecuateControlSet),"true");
}

void UIinterface::IO_BackToPrepare_Clicked()
{
    qDebug()<<"Change Page from 1 to 0";
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"l");
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"r");
//    SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::ExecuateControlSet),"false");
    qDebug()<<"Change Page from 1 to 0 finish";
}


void UIinterface::onLightBoardStickChange(uint8_t sta)
{
    if(m_ArmEnable!=true)return;
    switch (sta) {
        case 1:
        {
           // this->m_Lifting_p->LiftingUp(Lifting_Fast);
            this->SetButtonSta(this->m_PO_Button_Fastup,UI_Button_Highlight);
            break;
        }
        case 2:
        {
            //this->m_Lifting_p->LiftingDown(Lifting_Fast);
            this->SetButtonSta(this->m_PO_Button_Fastdown,UI_Button_Highlight);
            break;
        }
        default:
        {
            //this->m_Lifting_p->LiftingBrake();
            this->SetButtonSta(this->m_PO_Button_Fastup,UI_Button_Default);
            this->SetButtonSta(this->m_PO_Button_Fastdown,UI_Button_Default);
            break;
        }
    }
}

void UIinterface::onButton_InstrumentInstalled_Clicked()
{

}

void UIinterface::onButton_ReleaseInstrument_Clicked()
{


}

void  UIinterface::setCurrentPage(int current_page_num)
{
    this->m_CurrentPage=current_page_num;

    if(current_page_num==0){
        qDebug()<<"current page 0";
        if(m_PrevPage == 0)
        {
            qDebug()<<"Change Page from 0 to 0";
//            SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"l");
//            SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"r");
        }
        if(m_PrevPage == 1)
        {
            qDebug()<<"Change Page from 1 to 0";
//            SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"l");
//            SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"r");
//            SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::ExecuateControlSet),"false");
            qDebug()<<"Change Page from 1 to 0 finish";
        }
    }if(current_page_num==1 && m_PrevPage == 0){
        qDebug()<<"Change Page from 0 to 1";
//        SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"l");
//        SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::GALIL_INIT),"r");
//        SendInnerMsg(Module_Inner_E::Slave,static_cast<int>(SlaveAction_E::ExecuateControlSet),"true");
    }
    emit CurrentChanged(this->m_CurrentPage);
    m_PrevPage = m_CurrentPage;
}

int  UIinterface::GetCurrentPage(void)
{
    return this->m_CurrentPage;
}

void UIinterface::SetButtonSta(QObject* btn,UI_Button_e btnsta)
{
    if(btn!=nullptr)
    {
        QMetaObject::invokeMethod(btn, "setsta",Q_ARG(QVariant, btnsta));
    }
    else
    {
        qDebug()<<"btn null";
    }
}

void UIinterface::SetInstrumentType_L(QString type)
{
    if(this->m_PO_Text_InstrumentType_L!=nullptr)
    {
        this->m_PO_Text_InstrumentType_L->setProperty("text",type);
        QMetaObject::invokeMethod(this->m_PO_Text_InstrumentType_L, "doLayout");
    }
}

void  UIinterface::SetInstrumentType_R(QString type)
{
    if(this->m_PO_Text_InstrumentType_R!=nullptr)
    {
        this->m_PO_Text_InstrumentType_R->setProperty("text",type);
        QMetaObject::invokeMethod(this->m_PO_Text_InstrumentType_R, "doLayout");
    }
}


void UIinterface::setlevel(int level)
{

    this->m_Level = level;
//    emit LevelChanged(this->m_Level );;
//    SteadyDelay(10);
    switch (this->m_Level ) {
    case 1:
    {
         QMetaObject::invokeMethod(this->m_IO_Image_Level1, "setsta",Q_ARG(QVariant, 1));
          QMetaObject::invokeMethod(this->m_IO_Image_Level2, "setsta",Q_ARG(QVariant, 0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level3, "setsta",Q_ARG(QVariant, 0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level4, "setsta",Q_ARG(QVariant, 0));

        break;
    }
    case 2:
    {
         QMetaObject::invokeMethod(this->m_IO_Image_Level1, "setsta",Q_ARG(QVariant, 0));
          QMetaObject::invokeMethod(this->m_IO_Image_Level2, "setsta",Q_ARG(QVariant, 1));
           QMetaObject::invokeMethod(this->m_IO_Image_Level3, "setsta",Q_ARG(QVariant, 0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level4, "setsta",Q_ARG(QVariant, 0));

        break;
    }
    case 3:
    {
         QMetaObject::invokeMethod(this->m_IO_Image_Level1, "setsta",Q_ARG(QVariant, 0));
          QMetaObject::invokeMethod(this->m_IO_Image_Level2, "setsta",Q_ARG(QVariant,0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level3, "setsta",Q_ARG(QVariant, 1));
           QMetaObject::invokeMethod(this->m_IO_Image_Level4, "setsta",Q_ARG(QVariant, 0));

        break;
    }
    case 4:
    {
         QMetaObject::invokeMethod(this->m_IO_Image_Level1, "setsta",Q_ARG(QVariant, 0));
          QMetaObject::invokeMethod(this->m_IO_Image_Level2, "setsta",Q_ARG(QVariant,0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level3, "setsta",Q_ARG(QVariant, 0));
           QMetaObject::invokeMethod(this->m_IO_Image_Level4, "setsta",Q_ARG(QVariant, 1));

        break;
    }
    default:break;

    }
}

void UIinterface::SetHandle_L_En(bool en)
{
    this->m_HandleEnable_L=en;
    if(m_IO_Text_Enable_L!=nullptr)
    {
//        qDebug()<<" Button_Enable_l setsta";
        QMetaObject::invokeMethod(m_IO_Text_Enable_L, "setsta",Q_ARG(QVariant, en));
    }
}


void UIinterface::SetHandle_R_En(bool en)
{
    this->m_HandleEnable_R=en;
    if(m_IO_Text_Enable_R!=nullptr)
    {
//        qDebug()<<" Button_Enable_r setsta";
        QMetaObject::invokeMethod(m_IO_Text_Enable_R, "setsta",Q_ARG(QVariant, en));
    }

}


void UIinterface::SetAxisLimit(QString Axis,QString limitsta)
{
    int ilimitsta=limitsta.toInt() - 160;
    if(Axis=="LimitConditionL_A")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_L_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_L_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_L_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_L_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionL_B")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_L_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_L_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_L_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_L_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionL_C")
    {
        if( ilimitsta == 0)
        {
        }
        else if(ilimitsta == 1)
        {
        }
        else
        {
        }
    }
    else if(Axis=="LimitConditionL_D")
    {
        if( ilimitsta == 0)
        {
        }
        else if(ilimitsta == 1)
        {
        }
        else
        {
        }
    }
    else if(Axis=="LimitConditionL_E")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_L_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_L_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_L_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_L_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionL_F")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_L_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_L_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_L_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_L_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionL_G")
    {
        if( ilimitsta == 0)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_L_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_L_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_L_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_L_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    if(Axis=="LimitConditionR_A")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_R_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_R_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_R_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisA_R_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionR_B")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_R_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_R_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_R_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisB_R_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionR_C")
    {

    }
    else if(Axis=="LimitConditionR_D")
    {
    }
    else if(Axis=="LimitConditionR_E")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_R_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_R_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_R_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisE_R_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionR_F")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_R_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_R_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_R_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisF_R_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
    else if(Axis=="LimitConditionR_G")
    {
        if( ilimitsta == 0)
        {
         QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_R_Back, "setsta",Q_ARG(QVariant, 1));
        }
        else if(ilimitsta == 1)
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_R_Front, "setsta",Q_ARG(QVariant, 1));
        }
        else
        {
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_R_Front, "setsta",Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this->m_IO_Image_InstrumentAxisG_R_Back, "setsta",Q_ARG(QVariant, 0));
        }
    }
}


int UIinterface::getlevel()
{
    qDebug()<<"IN GETLEVEL m_Level: "<<m_Level;
    return this->m_Level;
}


void UIinterface::onButton_Fastup_Pressed()
{
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingUp),"Fast");
}


void UIinterface::onButton_Fastup_Released()
{
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingBrake),"");
}

void UIinterface::onButton_Slowup_Pressed()
{
    qDebug()<<"onButton_Slowup_Pressed()";
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::FootSupportUp),"Slow");
}

void UIinterface::onButton_Slowup_Released()
{
    qDebug()<<"onButton_Slowup_Released()";
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::FootSupportBrake),"");
}


void UIinterface::onButton_Fastdown_Pressed()
{
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingDown),"Fast");
}

void UIinterface::onButton_Fastdown_Released()
{
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingBrake),"");
}

void UIinterface::onButton_Slowdown_Pressed()
{
     qDebug()<<"onButton_Slowdown_Pressed()";
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::FootSupportDown),"Slow");
}

void UIinterface::onButton_Slowdown_Released()
{
     qDebug()<<"onButton_Slowdown_Released()";
    SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::FootSupportBrake),"");

}

void UIinterface::liftingBrake()
{
 //   SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingBrake),"");
//    if(m_Lifting_p != nullptr)
//    this->m_Lifting_p->LiftingBrake();
}

void UIinterface::liftingFastup()
{
//    if(m_Lifting_p != nullptr)
//    m_Lifting_p->LiftingUp(Lifting_Fast);
  //  SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingUp),"Fast");
}

void UIinterface::liftingSlowup()
{
//    if(m_Lifting_p != nullptr)
//    m_Lifting_p->LiftingUp(Lifting_Slow);
 //   SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingUp),"Slow");
}

void UIinterface::liftingFastdown()
{
//    if(m_Lifting_p != nullptr)
//    m_Lifting_p->LiftingDown(Lifting_Fast);
 //   SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingDown),"Fast");
}

void UIinterface::liftingSlowdown()
{
//    if(m_Lifting_p != nullptr)
//    m_Lifting_p->LiftingDown(Lifting_Slow);
  //  SendInnerMsg(Module_Inner_E::AssistDevice_Lifting,static_cast<int>(AssistDevice_LiftingAction_E::LiftingDown),"Slow");
}

void UIinterface::setUISignal(int* uiSignal)
{
    m_UiSignal = uiSignal;
    emit uiSignalChanged(uiSignal);
    qDebug()<<"set my_uiSignal: ";
    for(int i=0;i<10;i++)
    {
        qDebug()<<m_UiSignal[i];
    }
}
int* UIinterface::getUISignal()
{
    return m_UiSignal;
}
int UIinterface::systemStart()
{
//    Calibration(robot->g);
//    galil(GCmd(robot->g, "mode=2"));
    //ForwardKinematics(robot->g);
    qDebug()<<"system start";
    return 1;
}
int UIinterface::getSignal(int index)
{
    return m_UiSignal[index];
}
void UIinterface::systemStop()
{
    qDebug()<<"quit the program/shut down";
    qDebug()<<"from thread id:"<<QThread::currentThreadId();
//    galil(GCmd(robot->g,"ST ABCDE"));
}


void UIinterface::enableRobotArm()
{
    qDebug()<<"enable robot arm";
    qDebug()<<"from thread id:"<<QThread::currentThreadId();
}

void UIinterface::disableRobotArm()
{
    qDebug()<<"disable robot arm";
    qDebug()<<"from thread id:"<<QThread::currentThreadId();
}

//Shut Down System
void UIinterface::onButton_PowerOff()
{
    SendInnerMsg(Module_Inner_E::Security, static_cast<int>(SecurityAction_E::PowerOff), "");
}

void UIinterface::SendInnerMsg(Module_Inner_E recever,int Action, QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Uiinterface;
    msgTemp.Recver = recever;
    msgTemp.Request.insert(Action,arg);
    m_messagePool.SendMessage(msgTemp);
}

void UIinterface::SendInnerMsg(Module_Inner_E recever,int Action, QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Uiinterface;
    msgTemp.Recver = recever;
    for(int i = 0; i < arglist.length(); i++)
    {
        msgTemp.Request.insert(Action, arglist[i]);
    }
    m_messagePool.SendMessage(msgTemp);
}

void UIinterface::dealWithMsg()//与robotcontrol部分通信
{
    Message_Inner_T msg;
    try {

        while(1){
            m_MsgGottenRWLock.lockForRead();
            if(m_MsgGottenQueue.isEmpty()==false){
                msg=m_MsgGottenQueue.dequeue();
            }else{
                 m_MsgGottenRWLock.unlock();
                 break;
            }
            m_MsgGottenRWLock.unlock();
            if(msg.Recver!=Module_Inner_E::Uiinterface && msg.Recver != Module_Inner_E::MultipleModules)
                return;

            QHash<int,QString>::ConstIterator i = msg.Request.constBegin();

            while(i != msg.Request.constEnd())
            {
                switch (i.key())
                {
                    case static_cast<int>(MultipleDevAction_E::RecvLightBoardIoState):
                    {
                        QStringList slist = i.value().split(":");
                        if(slist[1] == "1"){
                           onLightBoardStickChange(1);
                        }else if(slist[1] == "2"){
                           onLightBoardStickChange(2);
                        }else{
                           onLightBoardStickChange(0);
                        }
                        break;
                    }

                    case static_cast<int>(UIAction_E::RecvModuleStatus):
                    {
                        QStringList slist = i.value().split(":");
                        if(slist[0] == "Master")
                        {

                        }
                        else if(slist[0] == "RobotConrol")
                        {

                        }else if(slist[0] == "LiftArm")
                        {

                        }else if(slist[0] == "LightBoard")
                        {

                        }
                        break;
                    }

                    case static_cast<int>(UIAction_E::RecvMasterData):
                    {

                        QStringList cmdlist = i.value().split(";");
                        for(int j = 0;j < cmdlist.length(); j++)
                        {
                            QStringList slist = cmdlist[j].split(":");
                            if(slist[0] == "enableButton_L"){
                                auto stateTemp=slist[1].toUInt();
                                if(stateTemp == enableAction ||stateTemp == keepEnabling){
                                    SetHandle_L_En(true);
                                }else{
                                    SetHandle_L_En(false);
                                }
                            }else if(slist[0] == "enableButton_R"){
                                auto stateTemp = slist[1].toUInt();
                                if(stateTemp == enableAction ||stateTemp == keepEnabling){
                                    SetHandle_R_En(true);
                                }else{
                                    SetHandle_R_En(false);
                                }
                            }else if(slist[0] == "speedCur"){
                                auto speedLevelTemp = slist[1].toUInt();
                                setlevel(speedLevelTemp + 1);
                            }
                        }
                        break;
                    }

                    case static_cast<int>(UIAction_E::FinishCalibration):
                    {
                        QStringList cmdlist = i.value().split(";");
                        LOG(INFO) << "Get finish calibration signal from Robot Control: reset button";
                        resetButton_Calibration();
                        break;
                    }

                    case static_cast<int>(UIAction_E::RecvSlaveStatus):
                    {
                       QStringList cmdlist = i.value().split(";");
                        for(int j = 0;j < (cmdlist.length() - 1); j++)
                        {
                            QStringList slist = cmdlist[j].split(":");
                            SetAxisLimit(slist[0],slist[1]);
                        }
                        break;
                    }

                    case static_cast<int>(MultipleDevAction_E::RecvLiftingBootSta):
                    {
                        if(i.value() == "Ok")
                        {
                            SetLiftingStaimage(2);
                            SetLiftingcheckString("  升降柱连接成功");
                        }else{
                            SetLiftingStaimage(0);
                            SetLiftingcheckString("  升降柱连接失败");
                        }
                        break;
                    }
                    case static_cast<int>(MultipleDevAction_E::RecvHorizontalArmBootSta):
                    {
                        if(i.value()=="Ok")
                        {
                            SetArmStaimage(2);
                            SetArmcheckString("  支撑臂连接成功");
                        }else{
                            SetArmStaimage(0);
                            SetArmcheckString("  支撑臂连接失败");
                        }
                        break;
                    }
                    case static_cast<int>(MultipleDevAction_E::RecvLeftFinialBootSta):
                    {
                        if(i.value()=="Ok")
                        {
                            SetLeftfinialStaimage(2);
                            SetLeftfinialcheckString("  左执行末端连接成功");
                        }else{
                            SetLeftfinialStaimage(0);
                            SetLeftfinialcheckString("  左执行末端连接失败");
                        }
                        break;
                    }
                    case static_cast<int>(MultipleDevAction_E::RecvRightFinialBootSta):
                    {
                        if(i.value() == "Ok")
                        {
                            SetRightfinialStaimage(2);
                            SetRightfinialcheckString("  右执行末端连接成功");
                        }else{
                            SetRightfinialStaimage(0);
                            SetRightfinialcheckString("  右执行末端连接失败");
                        }
                        break;
                    }
                    case static_cast<int>(MultipleDevAction_E::RecvMasterBootSta):
                    {
                        if(i.value() == "Ok")
                        {
                            SetMainhandStaimage(2);
                            setMasterConsoleCheckString("  主手连接成功");
                        }else{
                            SetMainhandStaimage(0);
                            setMasterConsoleCheckString("  主手连接失败");
                        }
                        break;
                    }
                    case static_cast<int>(UIAction_E::RecvSystemBootSta):
                    {
                        if(i.value() == "Ok")
                        {
                            SetReserveButtonSta(2);
                            std::cout << "In UI Interface received system boot Status" << std::endl;
                        }else{
                            SetReserveButtonSta(0);
                        }
                        break;
                    }
                    default:break;
                }
                i++;
            }
        }
        } catch (QException e) {
            LOG(ERROR)<<"Uiinterface dealmsg err："<<e.what();
        }
}

void UIinterface::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}


