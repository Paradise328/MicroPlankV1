set(VIPER_SOURCES
    src/viper_queue.cpp
    src/viper_ui.cpp
    src/viper_usb.cpp
)
add_library(viper_lib  ${VIPER_SOURCES})

target_link_libraries(viper_lib PUBLIC
		     -lusb-1.0 
		     -lpthread
             atomic
             loggerModule_lib
             pthread
             )

target_include_directories(viper_lib PUBLIC include /usr/local/include)
