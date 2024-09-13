add_library(omega_lib src/FdOmega.cpp)

link_directories(/home/chenxinbo/Documents/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/lib/release/lin-x86_64-gcc /usr/local/lib)

target_include_directories(omega_lib PUBLIC include /usr/local/include /home/chenxinbo/Documents/sdk-3.17.6-linux-x86_64-gcc/sdk-3.17.6/include)

target_link_libraries(omega_lib PUBLIC
		     -lusb-1.0 
		     -lpthread
             atomic
             loggerModule_lib
             mathModule_lib
             pthread
             dhd
             drd)

