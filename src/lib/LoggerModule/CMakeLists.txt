CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
project(LoggerModule)

set(CMAKE_CXX_STANDARD 17)

set(LOGGERMODULE_SOURCES 
    src/easylogging++.cc
)

add_library(logger_lib ${LOGGERMODULE_SOURCES})
target_include_directories(logger_lib PUBLIC include)
