SET(LIFT_COLUMN_SRC main.cpp LiftColumn.cpp)
SET(LIFT_COLUMN_INC LiftColumn.h)
SET(LIFT_COLUMN_UI LiftColumn.ui )


SET(PERIPHERAL_DEVICE_PREFIX ${CMAKE_SOURCE_DIR}/lib/peripheral_device)
SET(PERIPHERAL_DEVICE_SRC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.cpp ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.cpp)
SET(PERIPHERAL_DEVICE_INC ${PERIPHERAL_DEVICE_PREFIX}/CRC16.h ${PERIPHERAL_DEVICE_PREFIX}/peripheral_device.h ${PERIPHERAL_DEVICE_PREFIX}/CommunicationProtol.h)
include_directories(${PERIPHERAL_DEVICE_PREFIX})

SET(LIFTING_PREFIX ${CMAKE_SOURCE_DIR}/lib/lift_column)
SET(LIFTING_SRC ${LIFTING_PREFIX}/lifting.cpp )
SET(LIFTING_INC ${LIFTING_PREFIX}/lifting.h )
include_directories(${LIFTING_PREFIX})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

# main program used to configure Console
add_executable (LiftColumn  ${LIFT_COLUMN_SRC} 
                            ${LIFT_COLUMN_INC} 
                            ${LIFTING_SRC} 
                            ${LIFTING_INC} 
                            ${PERIPHERAL_DEVICE_SRC}
                            ${PERIPHERAL_DEVICE_INC})
# link against non cisst libraries and cisst components
target_link_libraries (LiftColumn ${QtComponents} pthread)
