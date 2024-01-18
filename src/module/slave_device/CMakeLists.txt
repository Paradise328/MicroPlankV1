CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
project(SlaveDevice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17")

set(SLAVEDEVICE_SOURCES 
    src/SlaveDevice.cpp
)

add_library(slave_device ${SLAVEDEVICE_SOURCES})
target_include_directories(slave_device PUBLIC include)
target_link_libraries(slave_device PUBLIC 
                          logger_lib )