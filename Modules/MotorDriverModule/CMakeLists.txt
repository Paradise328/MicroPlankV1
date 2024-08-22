set(ROBOTMODULE_SOURCES
    src/MotorDriver.cpp
)
add_library(motorDriverModule_lib ${ROBOTMODULE_SOURCES})

target_include_directories(motorDriverModule_lib PUBLIC include /usr/local/include)

target_link_libraries(motorDriverModule_lib PUBLIC 
                      pthread 
                      loggerModule_lib 
                      mathModule_lib
                      /usr/lib/x86_64-linux-gnu/libatomic.so.1
                      /usr/local/lib/libcifx.so.2)
