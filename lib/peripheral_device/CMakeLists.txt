SET(LIB_PERIPHERAL_SRC peripheral_device.cpp CRC16.cpp)
SET(LIB_PERIPHERAL_INC peripheral_device.h CRC16.h CommunicationProtol.h)

SET(CMAKe_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

SET(QtComponents Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)


ADD_LIBRARY(peripheral_static STATIC ${LIB_PERIPHERAL_SRC} ${LIB_PERIPHERAL_INC} )
SET_TARGET_PROPERTIES(peripheral_static PROPERTIES OUTPUT_NAME "peripheral_device")
TARGET_LINK_LIBRARIES(peripheral_static PUBLIC ${QtComponents})

ADD_LIBRARY(peripheral_device SHARED ${LIB_PERIPHERAL_SRC} ${LIB_PERIPHERAL_INC})
SET_TARGET_PROPERTIES(peripheral_device PROPERTIES OUTPUT_NAME "peripheral_device")
TARGET_LINK_LIBRARIES(peripheral_device PUBLIC ${QtComponents})