cmake_minimum_required(VERSION 3.10.2)
project(LoggerModule)

set(CMAKE_CXX_STANDARD 17)

set(LOGGERMODULE_SOURCES
    src/easylogging++.cc
)

add_library(loggerModule_lib ${LOGGERMODULE_SOURCES})

target_include_directories(loggerModule_lib PUBLIC include)
