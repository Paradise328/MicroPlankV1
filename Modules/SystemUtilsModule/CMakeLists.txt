project(SystemUtilsModule)

set(SYSTEM_UTILS_HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/include/SystemUtils.h)

add_library(systemUtilsModule_lib INTERFACE)
target_sources(systemUtilsModule_lib INTERFACE ${SYSTEM_UTILS_HEADERS})
target_include_directories(systemUtilsModule_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/include/eigen3)
