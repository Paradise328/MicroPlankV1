/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: OS_Includes.h $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 29.01.09 13:36 $
    $Revision: 13401 $

   Targets:
     Win32/ANSI   : no
     Win32/Unicode: no
     WinCE        : no
     Linux        : yes

   Description:
     Headerfile for specific operating system includes (linux only)

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         24.11.2008  MT       initial version

**************************************************************************************/

#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H

#ifdef CIFXETHERNET
#ifdef APIRENAME    /* NOTE: allow api renaming only when ethernet support is enabled */
#define xSysdeviceReset     xSysdeviceResetTK
#define xSysdeviceResetEx   xSysdeviceResetExTK
#define xSysdeviceBootstart xSysdeviceBootstartTK
#define xChannelReset       xChannelResetTK
#endif
#endif

#define APIENTRY

#ifndef NULL
  #define NULL 0
#endif

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(a) (void)(a)
#endif

#ifdef CIFX_DRV_HWIF
  #define CIFX_TOOLKIT_HWIF
#endif

#endif /* __OS_INCLUDES__H */
