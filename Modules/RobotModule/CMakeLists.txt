set(ROBOTMODULE_SOURCES
    src/RobotKinematics.cpp
)
add_library(robotModule_lib ${ROBOTMODULE_SOURCES})

target_include_directories(robotModule_lib PUBLIC include /usr/local/include)

target_link_libraries(robotModule_lib PUBLIC 
                      pthread 
                      loggerModule_lib 
                      mathModule_lib)
