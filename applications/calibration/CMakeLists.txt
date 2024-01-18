SET(CALIBRATION_TEST_SRC RobotProgram.cpp calibration_test.cpp )  
SET(CALIBRATION_TEST_INC RobotProgram.h)
include_directories(${CALIBRATION_TEST_INC})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

SET(GALILCONTROL_PREFIX ${CMAKE_SOURCE_DIR}/lib/GalilControl)
SET(GALILCONTROL_SRC ${GALILCONTROL_PREFIX}/GalilControl.cpp ${GALILCONTROL_PREFIX}/Calibration.cpp)
SET(GALILCONTROL_INC ${GALILCONTROL_PREFIX}/GalilControl.h )
include_directories(${GALILCONTROL_PREFIX})

SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)


add_executable(CalibrationTest  ${CALIBRATION_TEST_SRC} 
                                ${CALIBRATION_TEST_INC} 
                                ${GALILCONTROL_SRC}
                                ${GALILCONTROL_INC}
                              #  ${MAGNETIC_BOARD_SRC}
                              #  ${MAGNETIC_BOARD_INC}
                                 )

TARGET_LINK_LIBRARIES(CalibrationTest ${QtComponents} gclib gclibo pthread)

