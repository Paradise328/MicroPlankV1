#-------------------------------------------------
#
# Project created by QtCreator 2023-03-09T17:35:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

ETHERCAT_DRIVERS = $$files(*h,true)
DIRS = $$dirname(ETHERCAT_DRIVERS)
SORTEDDIRS = $$sorted(DIRS)

LIBS += -L"/driver/tmp_build/libcifx/" -lcifx
LIBS += -lpthread
INCLUDEPATH += $$unique(SORTEDDIRS)

TARGET = armEthercat
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
    robot_arm_single.cpp \
    mainwindow.cpp \
    ../../lib/robot_arm/robotarm.cpp


HEADERS += \
    mainwindow.h \
    ../../lib/robot_arm/robotarm.h \
    robot_arm_single.h

FORMS += \
    mainwindow.ui
