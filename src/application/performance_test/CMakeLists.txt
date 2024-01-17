SET(PERFORMANCE_TEST_SRC PerformanceTestProgram.cpp performance_test.cpp)  
SET(PERFORMANCE_TEST_INC PerformanceTestProgram.h)
include_directories(${CALIBRATION_TEST_INC})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

#SET(PERFORMANCE_TEST_PREFIX ${CMAKE_SOURCE_DIR}/lib/GalilControl)
#SET(PERFORMANCE_TEST_SRC ${GALILCONTROL_PREFIX}/GalilControl.cpp ${GALILCONTROL_PREFIX}/Calibration.cpp)
#SET(PERFORMANCE_TEST_INC ${GALILCONTROL_PREFIX}/GalilControl.h )
#include_directories(${GALILCONTROL_PREFIX})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

add_executable(PerformanceTest  ${PERFORMANCE_TEST_SRC} 
                                ${PERFORMANCE_TEST_INC} 
                                 )

target_link_libraries(PerformanceTest PUBLIC 
                          logger_lib 
                          slave_device
                          galil_slave
                          gclib 
                          gclibo
                          )


