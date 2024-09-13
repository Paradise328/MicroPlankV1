cmake_minimum_required(VERSION 3.10.2)
project(MathModule)

set(MATH_UTILS_HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/include/MathUtils.h)

add_library(mathModule_lib INTERFACE)
target_sources(mathModule_lib INTERFACE ${MATH_UTILS_HEADERS})
target_include_directories(mathModule_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/include/eigen3)
