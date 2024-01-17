#-------------------------------------------------
#
# Project created by QtCreator 2023-03-24T10:48:53
#
#-------------------------------------------------

QT += core gui network
QT += serialport

ETHERCAT_DRIVERS = $$files(*h,true)
DIRS = $$dirname(ETHERCAT_DRIVERS)
SORTEDDIRS = $$sorted(DIRS)

LIBS += -L"/driver/tmp_build/libcifx/" -lcifx
LIBS += -lpthread
LIBS += -lgclib -lgclibo -lX11 -lgclibo -lpthread -lrt -lstdc++ -lm

INCLUDEPATH += $$unique(SORTEDDIRS)
INCLUDEPATH += /usr/include/eigen3
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mikroPlanck
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
    main.cpp \
    ../../lib/robot_arm/robotarm.cpp \
    ../../lib/lift_column/lifting.cpp \
    ../../lib/peripheral_device/peripheral_device.cpp \
    ../../lib/peripheral_device/CRC16.cpp \
    ../../lib/GalilControl/GalilControl.cpp \
    ../../lib/GalilControl/Calibration.cpp


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
    main.h \
    ../../lib/GalilControl/GalilControl.h

FORMS += \
    EndEffector.ui \
    StatusMenu.ui \
    RobotArmPage.ui \
    MikroPlanck.ui

DISTFILES += \
    ../../README.md

