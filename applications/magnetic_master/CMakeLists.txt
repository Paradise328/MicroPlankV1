SET(MAGNETIC_MASTER_SRC magnetic_master.cpp )
SET(MAGNETIC_MASTER_INC magnetic_master.h)
include_directories(${MAGNETIC_MASTER_INC})

SET(PERIPHERAL_DEVICE_PREFIX ${CMAKE_SOURCE_DIR}/lib/peripheral_device)
SET(PERIPHERAL_DEVICE_SRC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.cpp ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.cpp)
SET(PERIPHERAL_DEVICE_INC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.h ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.h ${PERIPHERAL_DEVICE_PREFIX}/CommunicationProtol.h)
include_directories(${PERIPHERAL_DEVICE_PREFIX})

SET(MAGNETIC_BOARD_PREFIX ${CMAKE_SOURCE_DIR}/lib/magnetic_board)
SET(MAGNETIC_BOARD_SRC ${MAGNETIC_BOARD_PREFIX}/viper_transmitter.cpp )
SET(MAGNETIC_BOARD_INC ${MAGNETIC_BOARD_PREFIX}/viper_transmitter.h )
include_directories(${MAGNETIC_BOARD_PREFIX})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)


SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

add_executable(MagneticMaster  ${MAGNETIC_MASTER_SRC} 
                                ${MAGNETIC_MASTER_INC} 
                                ${PERIPHERAL_DEVICE_SRC}
                                ${PERIPHERAL_DEVICE_INC}
                                ${MAGNETIC_BOARD_SRC}
                                ${MAGNETIC_BOARD_INC}
                                 )

TARGET_LINK_LIBRARIES(MagneticMaster ${QtComponents} pthread)

