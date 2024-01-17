cmake_minimum_required(VERSION 2.8.9)

project(netx-spm)

set(LIB_MAJOR 1)
set(LIB_MINOR 0)
set(LIB_BUILD 0)
set(LIB_REVISION 0)
set(LIB_VERSION ${LIB_MAJOR}.${LIB_MINOR}.${LIB_REVISION})

# do not set to CMAKE_CURRENT_SOURCE_DIR since it may be called from driver build
set(src_dir "${CMAKE_CURRENT_LIST_DIR}")
set(inc_dir ${src_dir})

if(CIFX_HEADER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CIFX_HEADER}")
else(CIFX_HEADER)
	include(FindPkgConfig)
	pkg_check_modules(LIBCIFX REQUIRED cifx)
	include_directories(${LIBCIFX_INCLUDE_DIRS})
endif(CIFX_HEADER)

if(NOT PLUGINPATH)
	set(PLUGINPATH "/opt/cifx/plugins/")
endif(NOT PLUGINPATH)

# file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "${src_dir}/*.c")
include_directories(${inc_dir})

add_definitions(-D_GNU_SOURCE)

# link and create shared & static library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# install resources
install(TARGETS ${PROJECT_NAME} DESTINATION ${PLUGINPATH})
install(FILES ${src_dir}/config0 DESTINATION ${PLUGINPATH}/${PROJECT_NAME}/)

