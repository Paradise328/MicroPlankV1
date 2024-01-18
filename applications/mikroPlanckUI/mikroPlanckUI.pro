#-------------------------------------------------
#
# Project created by QtCreator 2023-03-24T10:48:53
#
#-------------------------------------------------
QT += quick virtualkeyboard

QT += core gui network
QT += serialport

ETHERCAT_DRIVERS = $$files(*h,true)
DIRS = $$dirname(ETHERCAT_DRIVERS)
SORTEDDIRS = $$sorted(DIRS)

LIBS += -L"/driver/tmp_build/libcifx/" -lcifx
LIBS += -lpthread
LIBS += -lgclib -lgclibo -lX11 -lgclibo -lpthread -lrt -lstdc++ -lm

INCLUDEPATH += $$unique(SORTEDDIRS)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mikroPlanckUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../../lib/Viper_Kinematics/Handle_Control_Value.cpp \
    ../../lib/light_board/lightboard.cpp \
    ../../lib/master_handle_data/Handle.cpp \
    ../../lib/master_handle_data/viper_transmitter.cpp \
    EndEffector.cpp \
    Forcep_control_thread.cpp \
    Read_Handle_Data.cpp \
    StatusMenu.cpp \
    RobotArmPage.cpp \
    MikroPlanck.cpp \
    UIinterface.cpp \
    main.cpp \
    ../../lib/robot_arm/robotarm.cpp \
    ../../lib/lift_column/lifting.cpp \
    ../../lib/peripheral_device/peripheral_device.cpp \
    ../../lib/peripheral_device/CRC16.cpp \
    ../../lib/GalilControl/GalilControl.cpp \
    ../../lib/GalilControl/Calibration.cpp \
    qmllanguage.cpp


HEADERS += \
    ../../lib/Viper_Kinematics/Handle_Control_Value.h \
    ../../lib/light_board/lightboard.h \
    ../../lib/master_handle_data/CommunicationProtol.h \
    ../../lib/master_handle_data/Handle.h \
    ../../lib/master_handle_data/ViperInterface.h \
    ../../lib/master_handle_data/viper_transmitter.h \
    EndEffector.h \
    Forcep_control_thread.h \
    Read_Handle_Data.h \
    StatusMenu.h \
    RobotArmPage.h \
    MikroPlanck.h \
    ../../lib/robot_arm/robotarm.h \
    ../../lib/lift_column/lifting.h \
    ../../lib/peripheral_device/peripheral_device.h \
    ../../lib/peripheral_device/CRC16.h \
    ../../lib/peripheral_device/CommunicationProtol.h \
    UIinterface.h \
    main.h \
    ../../lib/GalilControl/GalilControl.h \
    qmllanguage.h

FORMS += \
    EndEffector.ui \
    StatusMenu.ui \
    RobotArmPage.ui \
    MikroPlanck.ui

DISTFILES += \
    ../../README.md \
    BasicComboBox.qml \
    CanvasWave.qml \
    InitPage.qml \
    IntroducePage.qml \
    MainPage.qml \
    RobotPage.qml \
    images/bg_add_win.png \
    images/bg_content.png \
    images/bg_initpage.png \
    images/bg_lock.png \
    images/bg_main.png \
    images/bg_mainpage_common.png \
    images/bg_mainpage_doc.png \
    images/bg_speed.png \
    images/bg_tab_win.png \
    images/bg_volume.png \
    images/bg_win_add_setting_para.png \
    images/btn_bg1.png \
    images/btn_bg2.png \
    images/btn_level.png \
    images/btn_level_hover.png \
    images/company_name.png \
    images/do_slider.png \
    images/do_slider2.png \
    images/down.png \
    images/freambg.png \
    images/icon_lock.png \
    images/icon_normal.png \
    images/icon_time.png \
    images/icon_unlock.png \
    images/icon_unlock1.png \
    images/icon_warning.png \
    images/initbg.png \
    images/isdown.png \
    images/isoppositedir.png \
    images/ispositivedir.png \
    images/isup.png \
    images/lock.png \
    images/logo.png \
    images/logo_name.png \
    images/logo_title.png \
    images/logo_title_bg.png \
    images/oppositedir.png \
    images/positivedir.png \
    images/shutdown.png \
    images/tab_btn_bg1.png \
    images/tab_btn_bg2.png \
    images/unlock.png \
    images/up.png \
    language_de.ts \
    language_en.ts \
    language_zh.ts \
    main.qml

RESOURCES += \
    images.qrc \
    qml.qrc \
    translator.qrc

