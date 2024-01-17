SET(TEST_OMEGA_SRC encoders.cpp)
SET(TEST_OMEGA_INC )

message("hello omega!")
SET(OMEGA_MASTER_PREFIX ${CMAKE_SOURCE_DIR}/lib/omega_master)
SET(OMEGA_MASTER_SRC )
SET(OMEGA_MASTER_INC ${OMEGA_MASTER_PREFIX}/dhdc.h ${OMEGA_MASTER_PREFIX}/drdc.h)
include_directories(${PERIPHERAL_DEVICE_PREFIX})



SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

#FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
#FIND_PACKAGE(Qt5Network REQUIRED)

#SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

# main program used to configure Console
add_executable (test_omega  ${TEST_OMEGA_SRC}
                            ${TEST_OMEGA_INC}
                            ${OMEGA_MASTER_SRC}
                            ${OMEGA_MASTER_INC})
# link against non cisst libraries and cisst components
#target_link_libraries (LiftColumn ${QtComponents} pthread)
