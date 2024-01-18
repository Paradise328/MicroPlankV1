CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
project(GalilSlave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17")

set(GALILSLAVE_SOURCES 
    src/GalilSlave.cpp
)

add_library(galil_slave ${GALILSLAVE_SOURCES})
target_include_directories(galil_slave PUBLIC include)
target_link_libraries(galil_slave PUBLIC 
                          logger_lib 
                          slave_device)