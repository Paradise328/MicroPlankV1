SET(VIPER_SINGLE_SRC viper_single.cpp )
SET(VIPER_SINGLE_INC viper_single.h)
include_directories(${VIPER_SINGLE_INC})

SET(VIPER_DEVICE_PREFIX ${CMAKE_SOURCE_DIR}/lib/viper_device)
SET(VIPER_DEVICE_SRC ${VIPER_DEVICE_PREFIX}/viper_device.cpp )
SET(VIPER_DEVICE_INC ${VIPER_DEVICE_PREFIX}/viper_device.h ${VIPER_DEVICE_PREFIX}/ViperInterface.h )
include_directories(${VIPER_DEVICE_PREFIX}/)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5 COMPONENTS SerialBus Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network )
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

add_executable(ViperSingle  ${VIPER_SINGLE_SRC} 
                                ${VIPER_SINGLE_INC} 
                                ${PERIPHERAL_DEVICE_SRC}
                                ${PERIPHERAL_DEVICE_INC}
                                ${VIPER_DEVICE_SRC}
                                ${VIPER_DEVICE_INC}
                                 )
qt5_use_modules(ViperSingle Core SerialPort)
TARGET_LINK_LIBRARIES(ViperSingle ${QtComponents} )