cmake_minimum_required(VERSION 3.10.2)
project(MicroPlank)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/lib)
# add_subdirectory(lib)
add_subdirectory(Modules)


# 创建主库
add_library(microPlank_Lib  microPlank.cpp)
# 设置头文件目录
target_include_directories(microPlank_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 添加 `microPlank_Lib`库
target_link_libraries(microPlank_Lib PUBLIC
						loggerModule_lib
						mathModule_lib
						robotModule_lib
						masterModule_lib
						motorDriverModule_lib)

add_executable(MicroplankExecutable ${PROJECT_SOURCE_DIR}/main.cpp)

target_link_libraries(MicroplankExecutable PUBLIC
						microPlank_Lib
		     )

