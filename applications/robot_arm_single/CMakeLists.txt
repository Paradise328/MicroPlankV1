SET(ROBOT_ARM_SINGLE_SRC mainwindow.cpp robot_arm_single.cpp)
SET(ROBOT_ARM_SINGLE_INC mainwindow.h  robot_arm_single.h)

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
message(${CMAKE_BINARY_DIR}/bin/RobotArmSingle)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)
add_executable(RobotArmSingle  ${ROBOT_ARM_SINGLE_SRC} 
                                ${ROBOT_ARM_SINGLE_INC} 
                                ${ROBOT_ARM_FUNC_SRC}
                                ${ROBOT_ARM_FUNC_INC}
                                )

TARGET_LINK_LIBRARIES(RobotArmSingle ${QtComponents} ${LIBRARY_REQ_LIBRARIES})