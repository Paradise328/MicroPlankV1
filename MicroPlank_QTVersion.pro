QT -= gui
QT += concurrent
QT += quick virtualkeyboard
QT += core gui network
QT += serialport
QT += testlib
QT += multimedia
QT += serialport

CONFIG -= app_bundle
CONFIG += c++17 console
#CONFIG +=resources_big
ETHERCAT_DRIVERS = $$files(*h,true)
DIRS = $$dirname(ETHERCAT_DRIVERS)
SORTEDDIRS = $$sorted(DIRS)

LIBS += -L"/home/a/Desktop/install_packages/cifx_linux/cifx/driver/my_build_folder/libcifx/" -lcifx
LIBS += -lX11 -lpthread -lrt -lstdc++ -lm
LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -latomic
LIBS+= -ldrd -ldhd -lusb-1.0



TEMPLATE = app
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib            # 根据安装位置调整路径
LIBS += -lruckig                    # 链接 Ruckig 库

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Modules/ForceSensorModule/ForceSensor.cpp \
        Modules/LoggerModule/easylogging++.cc \
        Modules/MasterModule/MasterConsole.cpp \
        Modules/MasterModule/MasterConsole_Lib/viper_transmitter.cpp \
        Modules/MasterModule/Omega_Lib/FdOmega.cpp \
        Modules/MasterModule/Viper_Lib/viper_queue.cpp \
        Modules/MasterModule/Viper_Lib/viper_ui.cpp \
        Modules/MasterModule/Viper_Lib/viper_usb.cpp \
        Modules/MotorDriverModule/MotorDriver.cpp \
        Modules/MsgModule/messagequeue.cpp \
        Modules/RobotControlModule/DomainControler.cpp \
        Modules/RobotControlModule/RobotControl.cpp \
        Modules/RobotControlModule/RobotGuidingArmControl.cpp \
        Modules/SecurityModule/security.cpp \
        Modules/UIModule/UIinterface.cpp \
        Modules/PeripheralDeviceModule/CRC16.cpp \
        Modules/PeripheralDeviceModule/peripheral_device.cpp \
        main.cpp \
        microPlank.cpp \
        Modules/RobotControlModule/BlasControl/BLA_API.cpp \
        Modules/RobotControlModule/BlasControl/communication.cpp \
        Modules/RobotControlModule/BlasControl/actuators_controler.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Modules/ForceSensorModule/ForceSensor.h \
    Modules/LoggerModule/easylogging++.h \
    Modules/MasterModule/MasterConsole.h \
    Modules/MasterModule/MasterConsole_Lib/viper_transmitter.h \
    Modules/MasterModule/Omega_Lib/FdOmega.h \
    Modules/MasterModule/Omega_Lib/dhdc.h \
    Modules/MasterModule/Omega_Lib/drdc.h \
    Modules/MasterModule/Viper_Lib/ViperInterface.h \
    Modules/MasterModule/Viper_Lib/libusb.h \
    Modules/MasterModule/Viper_Lib/viperInterface.h \
    Modules/MasterModule/Viper_Lib/viper_queue.h \
    Modules/MasterModule/Viper_Lib/viper_ui.h \
    Modules/MasterModule/Viper_Lib/viper_usb.h \
    Modules/MathModule/MathUtils.h \
    Modules/MathModule/lowpass_filter.h \
    Modules/MotorDriverModule/MotorDriver.h \
    Modules/MotorDriverModule/cifXEndianess.h \
    Modules/MsgModule/messagequeue.h \
    Modules/MsgModule/threadsafequeue.h \
    Modules/RobotControlModule/DomainControler.h \
    Modules/RobotControlModule/RobotControl.h \
    Modules/SecurityModule/security.h \
    Modules/SystemUtilsModule/SystemUtils.h \
    Modules/UIModule/UIinterface.h \
    Modules/PeripheralDeviceModule/CRC16.h \
    Modules/PeripheralDeviceModule/CommunicationProtol.h \
    Modules/PeripheralDeviceModule/peripheral_device.h \
    microPlank.h \
    Modules/RobotControlModule/BlasControl/BLA_API.h \
    Modules/RobotControlModule/BlasControl/communication.h \
    Modules/RobotControlModule/BlasControl/actuators_controler.h


DISTFILES += \
    Config/EndeffectorData.toml \
    Config/PDO_mapping.toml \
    Config/RobotData.toml \
    Config/commands \
    Config/logConf.conf \

RESOURCES += \
    Modules/UIModule/qrc/images.qrc \
    Modules/UIModule/qrc/qml.qrc \
    Modules/UIModule/qrc/translator.qrc

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/ -ldrd
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/ -ldrd
# else:unix: LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/ -ldrd

# INCLUDEPATH += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc
# DEPENDPATH += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc

# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/libdrd.a
# else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/debug/lin-x86_64-gcc/libdrd.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/drd.lib
# else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/drd.lib
# else:unix: PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/libdrd.a

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/ -ldhd
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/ -ldhd
# else:unix: LIBS += -L$$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/ -ldhd

# INCLUDEPATH += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc
# DEPENDPATH += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc

# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/libdhd.a
# else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/libdhd.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/dhd.lib
# else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/dhd.lib
# else:unix: PRE_TARGETDEPS += $$PWD/../../../下载/sdk-3.17.6/lib/release/lin-x86_64-gcc/libdhd.a


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/ -ldhd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/ -ldhd
else:unix: LIBS += -L$$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/ -ldhd

INCLUDEPATH += $$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc
DEPENDPATH += $$PWD/../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/ -ldrd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/ -ldrd
else:unix: LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/ -ldrd

INCLUDEPATH += $$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/include
DEPENDPATH += $$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-                           linux-x86_64-gcc/sdk-3.17.6/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/release/ -ldhd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/debug/ -ldhd
else:unix: LIBS += -L$$PWD/../../install_packages/force_dimension_driver/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc/ -ldhd

INCLUDEPATH += $$PWD/''
DEPENDPATH += $$PWD/''
