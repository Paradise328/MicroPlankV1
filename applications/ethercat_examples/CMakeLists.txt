
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS ${CMAKE_SOURCE_DIR})
option(LINK_NETXTRANSPORT "Enable linking of the netxtransport library. This requires the netxtransport library to be installed!" OFF)

if (LINK_NETXTRANSPORT)
        set(LIBRARY_BASE "netxtransport")
else (LINK_NETXTRANSPORT)
        set(LIBRARY_BASE "cifx")
endif (LINK_NETXTRANSPORT)

message(STATUS ${LIBRARY_BASE})

if(LIBRARY_HEADER OR LIBRARY_INC_LIB)
        set(LIBRARY_REQ_INCLUDE_DIRS  ${LIBRARY_HEADER})
        set(LIBRARY_REQ_LIBRARIES "-pthread -lrt -l${LIBRARY_BASE} -L${LIBRARY_INC_LIB}")
else(LIBRARY_HEADER OR LIBRARY_INC_LIB)
        include(FindPkgConfig)
        pkg_check_modules(LIBRARY_REQ REQUIRED ${LIBRARY_BASE})
endif(LIBRARY_HEADER OR LIBRARY_INC_LIB)



include_directories(${LIBRARY_REQ_INCLUDE_DIRS})

add_executable(cifxsample cifxsample/cifxlinuxsample.c)
target_include_directories(cifxsample BEFORE PUBLIC cifxsample/)
target_link_libraries (cifxsample ${LIBRARY_REQ_LIBRARIES})
#install(TARGETS cifxsample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)