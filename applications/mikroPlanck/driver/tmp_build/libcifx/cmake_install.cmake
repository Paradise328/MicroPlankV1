# Install script for directory: /home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libcifx.so.2.1.1"
      "$ENV{DESTDIR}/usr/local/lib/libcifx.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libcifx.so.2.1.1;/usr/local/lib/libcifx.so.2")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/libcifx.so.2.1.1"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/libcifx.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libcifx.so.2.1.1"
      "$ENV{DESTDIR}/usr/local/lib/libcifx.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/libcifx.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/libcifx.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/lib/libcifx.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libcifx.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/libcifx.so")
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/libcifx.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/libcifx.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/lib/libcifx.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/cifx/Hil_ApplicationCmd.h;/usr/local/include/cifx/Hil_BootParameter.h;/usr/local/include/cifx/Hil_CommandRange.h;/usr/local/include/cifx/Hil_Compiler.h;/usr/local/include/cifx/Hil_ComponentID.h;/usr/local/include/cifx/Hil_DeviceProductionData.h;/usr/local/include/cifx/Hil_DualPortMemory.h;/usr/local/include/cifx/Hil_FileHeaderNXS.h;/usr/local/include/cifx/Hil_FileHeaderV3.h;/usr/local/include/cifx/Hil_FirmwareIdent.h;/usr/local/include/cifx/Hil_GenericCommunicationInterface.h;/usr/local/include/cifx/Hil_Logbook.h;/usr/local/include/cifx/Hil_ModuleLoader.h;/usr/local/include/cifx/Hil_Packet.h;/usr/local/include/cifx/Hil_Results.h;/usr/local/include/cifx/Hil_SharedDefines.h;/usr/local/include/cifx/Hil_SystemCmd.h;/usr/local/include/cifx/Hil_Taglist.h;/usr/local/include/cifx/Hil_Types.h;/usr/local/include/cifx/User_Compiler.h;/usr/local/include/cifx/cifXErrors.h;/usr/local/include/cifx/cifXUser.h;/usr/local/include/cifx/netXAPI.h;/usr/local/include/cifx/cifXEndianess.h;/usr/local/include/cifx/cifxlinux.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include/cifx" TYPE FILE FILES
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_ApplicationCmd.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_BootParameter.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_CommandRange.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Compiler.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_ComponentID.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_DeviceProductionData.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_DualPortMemory.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_FileHeaderNXS.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_FileHeaderV3.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_FirmwareIdent.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_GenericCommunicationInterface.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Logbook.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_ModuleLoader.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Packet.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Results.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_SharedDefines.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_SystemCmd.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Taglist.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/Hil_Types.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/HilscherDefinitions/User_Compiler.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/cifXAPI/cifXErrors.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/cifXAPI/cifXUser.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Common/cifXAPI/netXAPI.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//Toolkit/Source/cifXEndianess.h"
    "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//cifxlinux.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/cifx/netx_tap.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include/cifx" TYPE FILE FILES "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/libcifx//netx_tap/netx_tap.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/pkgconfig/cifx.pc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib/pkgconfig" TYPE FILE FILES "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/cifx.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/cifx/plugins/netx-spm.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/opt/cifx/plugins" TYPE SHARED_LIBRARY FILES "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/netx-spm.so")
  if(EXISTS "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/opt/cifx/plugins/netx-spm.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/cifx/plugins/netx-spm/config0")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/opt/cifx/plugins/netx-spm" TYPE FILE FILES "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/plugins/netx-spm/config0")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/haosibo/Desktop/MicroSurgery-notGithub/testProject/lib/driver/tmp_build/libcifx/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
