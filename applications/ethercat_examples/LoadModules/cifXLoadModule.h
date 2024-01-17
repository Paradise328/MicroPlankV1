/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: cifxlinuxsample.c $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 1614 $

   Targets:
     Linux : yes

   Description:
     Linux I/O test application using modules

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        20.06.2011  SD       created


**************************************************************************************/


/*****************************************************************************/
/*! \addtogroup CIFX_DRIVER_TESTI cifX Driver API TEST PROGRAM               */
/*! \{                                                                       */
/*****************************************************************************/

/* prevent multiple inclusion */
#ifndef __CIFx_TEST_CONSOLE_H
#define __CIFx_TEST_CONSOLE_H

#ifdef __cplusplus
  extern "C" {
#endif  /* _cplusplus */


#define HANDLE CIFXHANDLE

#include <string.h>

#define __PACKED_PRE
#define __PACKED_POST __attribute__((__packed__))

#include <stdio.h>
#include "cifXUser.h"
#include "cifXErrors.h"

#include "Hil_Packet.h"
#include "Hil_SystemCmd.h"


/***************************************************************************
* Global Functions
***************************************************************************/

void    ShowError( int32_t lError);
int32_t StartModule(CIFXHANDLE hSysdevice, unsigned long ulChannelNumber, char* pszModuleName);
int32_t ReadBaseSystemName( CIFXHANDLE hDriver, char* pszDeviceName);
int32_t ReadFirmwareName( CIFXHANDLE hDriver, char* pszDeviceName, unsigned long ulChannelNumber);
int32_t DownloadModule( HANDLE hDevice, unsigned long ulChannelNumber, char* pszPath, char* pszModuleName, unsigned long ulDownloadMode);


#ifdef __cplusplus
}
#endif

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/

#endif  /* __CIFxUSER_H */
