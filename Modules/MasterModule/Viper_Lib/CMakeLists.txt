add_library(viper_lib  
        src/viper_queue.cpp
        src/viper_ui.cpp
        src/viper_usb.cpp)

target_include_directories(viper_lib PUBLIC include /usr/local/include)

target_link_libraries(viper_lib PUBLIC
		     -lusb-1.0 
		     -lpthread
                      atomic
                      loggerModule_lib
                       pthread)

