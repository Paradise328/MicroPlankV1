cmake_minimum_required (VERSION 2.8)
project(cifx_examples)
set(PROJECT_VERSION, 2.0.0.0)

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS ${CMAKE_SOURCE_DIR})

option(PERMANENT "Enable permanent connection (marshaller server feature - see \"HIL_MARSHALLER_PERMANENT_CONNECTION\")" ON)
option(LINK_NETXTRANSPORT_TCP "Enable to link netxtransport. This requires the netxtransport library to be installed!" OFF)

if(NOT TCPSERVERDIR)
	set(TCPSERVERDIR "./")
endif()

if (LINK_NETXTRANSPORT_TCP)
	set(LIBRARY_BASE "netxtransport")
else (LINK_NETXTRANSPORT_TCP)
	set(LIBRARY_BASE "cifx")
endif (LINK_NETXTRANSPORT_TCP)

if(LIBRARY_HEADER OR LIBRARY_INC_LIB)
	if (LIBRARY_HEADER)
		set(LIBRARY_REQ_INCLUDE_DIRS  ${LIBRARY_HEADER})
	endif (LIBRARY_HEADER)
	if (LIBRARY_INC_LIB)
		set (LIBRARY_INC_LIB "-L${LIBRARY_INC_LIB}")
	endif (LIBRARY_INC_LIB)
	set(LIBRARY_REQ_LIBRARIES "-lpthread -lrt -l${LIBRARY_BASE}" ${LIBRARY_INC_LIB})
else(LIBRARY_HEADER OR LIBRARY_INC_LIB)
	include(FindPkgConfig)
	pkg_check_modules(LIBRARY_REQ_TCP REQUIRED ${LIBRARY_BASE})
endif(LIBRARY_HEADER OR LIBRARY_INC_LIB)

include_directories(${LIBRARY_REQ_TCP_INCLUDE_DIRS} ./Marshaller/cifXAPI/ /usr/local/include/cifx/)

add_executable(cifxtcpserver ${TCPSERVERDIR}TCPConnector.c ${TCPSERVERDIR}OS_Specific.c ${TCPSERVERDIR}TCPServer.c ${TCPSERVERDIR}cifXDownloadHook.c ${TCPSERVERDIR}Marshaller/CifXTransport.c       ${TCPSERVERDIR}Marshaller/HilMarshaller.c)
target_include_directories(cifxtcpserver BEFORE PUBLIC ${TCPSERVERDIR} ${TCPSERVERDIR}Marshaller ${TCPSERVERDIR}Marshaller/APIHeader/)

target_link_libraries (cifxtcpserver ${LIBRARY_REQ_TCP_LIBRARIES})

target_compile_options(cifxtcpserver PRIVATE -Wformat-overflow=0)

if (PERMANENT)
	target_compile_definitions(cifxtcpserver PUBLIC -DHIL_MARSHALLER_PERMANENT_CONNECTION)
endif (PERMANENT)

if (PERMANENT)
	target_compile_definitions(cifxtcpserver PUBLIC -DHIL_MARSHALLER_PERMANENT_CONNECTION)
endif (PERMANENT)
install(TARGETS cifxtcpserver DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

