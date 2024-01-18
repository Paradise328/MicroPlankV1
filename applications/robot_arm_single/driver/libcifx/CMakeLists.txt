cmake_minimum_required(VERSION 2.8.9)

project(cifx)

set(DRV_MAJOR 2)
set(DRV_MINOR 1)
set(DRV_BUILD 0)
set(DRV_REVISION 1)
set(DRV_VERSION ${DRV_MAJOR}.${DRV_MINOR}.${DRV_REVISION})
set(DRVVERSION "\"V${DRV_VERSION}.${DRV_BUILD}\"")

set(DRVDIR "${CMAKE_CURRENT_SOURCE_DIR}/")

set(src_dir ${DRVDIR})
set(tk_dir "${DRVDIR}/Toolkit")
set(src_tk_dir "${tk_dir}/Source")
set(src_tap_dir "${DRVDIR}/netx_tap")

set(inc_dir ${src_dir})
set(inc_tk_dir ${src_tk_dir} ${tk_dir}/Common/cifXAPI/ ${tk_dir}/Common/HilscherDefinitions/)

set(dest_dir "${CMAKE_INSTALL_PREFIX}/lib/")
set(dest_inc_dir "${CMAKE_INSTALL_PREFIX}/include/cifx")

# file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "${src_dir}/*.c" "${src_tk_dir}/*.c")
include_directories(${inc_dir} ${inc_tk_dir} /usr/include/)

set(drv_libs "-lpthread -lrt")

add_definitions(-DVERSION_INFO="\"${DRVVERSION}\"")

option(DEBUG "Build Library debug messages" OFF)
option(DISABLE_PCI "Disable use of PCI" OFF)
option(TIME "Enable device time setting during start-up" OFF)
option(DMA "Compile driver with dma support" OFF)
option(NO_MINSLEEP "Disable minimum sleep time" OFF)
option(VIRTETH "Enables virtual ethernet interface support" OFF)
option(HWIF "Enables the toolkit's Hardware Function Interface (e.g. for SPI)" OFF)
option(PLUGIN "Enables support of device plugins and enables hardware function interface (see HWIF)" OFF)
option(SPM_PLUGIN "Build spm-plugin and enables support of device plugins (see PLUGIN)" OFF)

if(DEBUG)
	add_definitions(-DDEBUG)
endif(DEBUG)
if(DISABLE_PCI)
	add_definitions(-DCIFX_TOOLKIT_DISABLEPCI)
else(DISABLE_PCI)
	set(drv_libs "${drv_libs} -lpciaccess")
	set(REQUIRED_PACKAGES pciaccess)
	if (NOT PCIACCESS_LIBS)
		set(PCIACCESS_LIBS -lpciaccess)
	endif(NOT PCIACCESS_LIBS)
	if (PCIACCESS_CFLAGS)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PCIACCESS_CFLAGS}")
	endif (PCIACCESS_CFLAGS)
endif(DISABLE_PCI)
if(TIME)
	add_definitions(-DCIFX_TOOLKIT_TIME)
endif(TIME)
if(DMA)
	add_definitions(-DCIFX_TOOLKIT_DMA)
endif(DMA)
if(NO_MINSLEEP)
	add_definitions(-DNO_MIN_SLEEP)
endif(NO_MINSLEEP)
if(VIRTETH)
	add_definitions(-DCIFXETHERNET)
	include_directories(${src_tap_dir})
	file(GLOB NETXTAP_SOURCES ${src_tap_dir}/*.c)
	set_source_files_properties(${src_tk_dir}/cifXFunctions.c PROPERTIES COMPILE_FLAGS -DAPIRENAME)
endif(VIRTETH)
if(HWIF OR PLUGIN OR SPM_PLUGIN)
	add_definitions(-DCIFX_DRV_HWIF)
endif(HWIF OR PLUGIN OR SPM_PLUGIN)
if(PLUGIN OR SPM_PLUGIN)
	add_definitions(-DCIFX_PLUGIN_SUPPORT)
	set(PLUGIN_LIBS "-ldl")
	set(drv_libs "${drv_libs} ${PLUGIN_LIBS}")
endif(PLUGIN OR SPM_PLUGIN)

# check endianess
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
	message(STATUS "BIG_ENDIAN")
	add_definitions(-DCIFX_TOOLKIT_BIGENDIAN)
else(IS_BIG_ENDIAN)
	message(STATUS "LITTLE_ENDIAN")
endif(IS_BIG_ENDIAN)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${prefix})
set(libdir ${exec_prefix}/lib)
set(includedir ${prefix}/include)
# configure pkgconfig file
configure_file( ${DRVDIR}/cifx.pc.in
		${CMAKE_CURRENT_BINARY_DIR}/cifx.pc @ONLY)

#target_compile_options(${PROJECT_NAME} SHARED -Wformat-truncation=0)
#set_target_properties(

# link and create shared & static library
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${NETXTAP_SOURCES})
add_library(${PROJECT_NAME}_static STATIC ${SOURCES} ${NETXTAP_SOURCES})

target_link_libraries(${PROJECT_NAME} ${drv_libs})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${DRV_VERSION} SOVERSION ${DRV_MAJOR})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME cifx)

# removes warning about snprintf buffer truncation
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -Wformat-truncation=0)
set_target_properties(${PROJECT_NAME}_static PROPERTIES COMPILE_FLAGS -Wformat-truncation=0)

# install resources
install(TARGETS ${PROJECT_NAME} DESTINATION ${dest_dir})

# install header files
file(GLOB INSTALL_HEADERS
     "${DRVDIR}/Toolkit/Common/cifXAPI/*.h"
     "${DRVDIR}/Toolkit/Common/HilscherDefinitions/*.h"
     "${DRVDIR}/cifxlinux.h" "${DRVDIR}/Toolkit/Source/cifXEndianess.h")
install(FILES ${INSTALL_HEADERS} DESTINATION ${dest_inc_dir})

if (VIRTETH)
	install(FILES ${DRVDIR}/netx_tap/netx_tap.h DESTINATION ${dest_inc_dir})
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cifx.pc DESTINATION ${dest_dir}/pkgconfig)

if (SPM_PLUGIN)
	set (CIFX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/")
	include(${CMAKE_CURRENT_SOURCE_DIR}/../plugins/netx-spm/CMakeLists.txt)
endif(SPM_PLUGIN)
