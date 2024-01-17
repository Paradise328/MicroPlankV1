SET(ROBOT_LIFT_SRC RobotArmPage.cpp StatusMenu.cpp EndEffector.cpp MikroPlanck.cpp main.cpp)
SET(ROBOT_LIFT_INC RobotArmPage.h StatusMenu.h EndEffector.h MikroPlanck.h )

SET(PERIPHERAL_DEVICE_PREFIX ${CMAKE_SOURCE_DIR}/lib/peripheral_device)
SET(PERIPHERAL_DEVICE_SRC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.cpp ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.cpp)
SET(PERIPHERAL_DEVICE_INC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.h ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.h ${PERIPHERAL_DEVICE_PREFIX}/CommunicationProtol.h)
include_directories(${PERIPHERAL_DEVICE_PREFIX})

SET(LIFTING_PREFIX ${CMAKE_SOURCE_DIR}/lib/lift_column)
SET(LIFTING_SRC ${LIFTING_PREFIX}/lifting.cpp )
SET(LIFTING_INC ${LIFTING_PREFIX}/lifting.h )
include_directories(${LIFTING_PREFIX})

SET(ROBOT_ARM_FUNC_PREFIX ${CMAKE_SOURCE_DIR}/lib/robot_arm)
SET(ROBOT_ARM_FUNC_SRC ${ROBOT_ARM_FUNC_PREFIX}/robotarm.cpp)
SET(ROBOT_ARM_FUNC_INC ${ROBOT_ARM_FUNC_PREFIX}/robotarm.h )
include_directories(${ROBOT_ARM_FUNC_PREFIX})


option(LINK_NETXTRANSPORT "Enable linking of the netxtransport library. This requires the netxtransport library to be installed!" OFF)

if (LINK_NETXTRANSPORT)
        set(LIBRARY_BASE "netxtransport")
else (LINK_NETXTRANSPORT)
        set(LIBRARY_BASE "cifx")
endif (LINK_NETXTRANSPORT)

message(STATUS ${LIBRARY_BASE})

if(LIBRARY_HEADER OR LIBRARY_INC_LIB)
        set(LIBRARY_REQ_INCLUDE_DIRS  ${LIBRARY_HEADER})
        set(LIBRARY_REQ_LIBRARIES "-pthread -lrt -l${LIBRARY_BASE} -L${LIBRARY_INC_LIB}")
else(LIBRARY_HEADER OR LIBRARY_INC_LIB)
        include(FindPkgConfig)
        pkg_check_modules(LIBRARY_REQ REQUIRED ${LIBRARY_BASE})
endif(LIBRARY_HEADER OR LIBRARY_INC_LIB)

include_directories(${LIBRARY_REQ_INCLUDE_DIRS})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

add_executable(ArmAndLift   ${ROBOT_LIFT_SRC} 
                            ${ROBOT_LIFT_INC} 
                            ${ROBOT_ARM_FUNC_SRC}
                            ${ROBOT_ARM_FUNC_INC}
                            ${LIFTING_SRC} 
                            ${LIFTING_INC} 
                            ${PERIPHERAL_DEVICE_SRC}
                            ${PERIPHERAL_DEVICE_INC}
                            
                            )

TARGET_LINK_LIBRARIES(ArmAndLift ${QtComponents} ${LIBRARY_REQ_LIBRARIES})