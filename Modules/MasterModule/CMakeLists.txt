# 添加子目录
add_subdirectory(Viper_Lib)
# 创建主库
add_library(masterModule_lib  MasterConsole.cpp)

# 设置头文件目录
target_include_directories(masterModule_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# 添加 `masterModule_lib`库
target_link_libraries(masterModule_lib PUBLIC
				viper_lib
                atomic
                mathModule_lib)


