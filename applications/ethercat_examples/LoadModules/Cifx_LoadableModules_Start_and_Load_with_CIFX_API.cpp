/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: CifX_LoadableModules_Start_and_Load_with_CIFX_API.cpp $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 1614 $

   Targets:
     Linux : yes

   Description:
     Example how to start a module using the cifX API

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        20.06.2011  SD       created (based on win32 example)


**************************************************************************************/

#include "cifxlinux.h"
#include "cifXLoadModule.h"

/* ------------------------------------------------- */
/*  Information for Module TEST */
extern char*  g_tTestModule[];
extern char   g_pszDeviceName[];
extern char*  g_pszFirmwarePath;


/*****************************************************************************/
/*! Test loadable modules                                                    */
/*****************************************************************************/
void TestLoadableModulesCIFXApi( void)
{
  CIFXHANDLE hDriver    = NULL;
  CIFXHANDLE hSysdevice = NULL;
  int32_t    lRet       = CIFX_NO_ERROR;
  
  /* Download parameter */
  char*          pszModuleName  = NULL;
  unsigned long  lChannelNumber = 0;

  printf("\n----------------------------------------------\r\n");
  printf("---------- Test netX Loadable Modules --------\r\n");  
  printf("----------------------------------------------\r\n");
  
  /* Open the driver */
  lRet = xDriverOpen( &hDriver);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("\nFailed to open the driver, Error = 0x%8X \n", (unsigned int)lRet);
  } else
  {
    printf("\n--> Read Base-System Firmware...\n\n");
    /* Read the base module firmware version */
    ReadBaseSystemName( hDriver, g_pszDeviceName);
    
    /* Process all defined modules */
    for ( int iIdx = 0; NULL != (pszModuleName = g_tTestModule[iIdx]); iIdx++)
    {
      lChannelNumber = iIdx;
      printf("\n--> Testing Module: %s on Channel: %u \r\n", pszModuleName, (unsigned int)lChannelNumber);
    
      /* Open the Sysdevice */
      lRet = xSysdeviceOpen( hDriver, g_pszDeviceName, &hSysdevice);
      if( CIFX_NO_ERROR != lRet)
      {
        printf("Failed to open the Sysdevice, Error = 0x%.8X \n", lRet);
      } else
      {
        /* Download the module */
        lRet = DownloadModule( hSysdevice, lChannelNumber, g_pszFirmwarePath, pszModuleName, DOWNLOAD_MODE_MODULE);
        if( CIFX_NO_ERROR != lRet)
        {
          /* Read driver error description and display error */
          ShowError( lRet);
        } else
        {
          printf("Module download was successfull!\n\n");
          printf("--> Try to read firmware information...\n");
          /* Module successfully started */
          ReadFirmwareName( hDriver, g_pszDeviceName, lChannelNumber);
        }

        if( CIFX_NO_ERROR == lRet)
        {
          CIFXHANDLE hChannel = NULL;

          /* Check Channel functions */
          lRet = xChannelOpen(hDriver, g_pszDeviceName, lChannelNumber, &hChannel);
          if( 0 != lRet)
          {
            /* Error starting module */
            ShowError( lRet);
          } else
          {
            uint32_t ulState = 0;

            lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_NOT_READY, &ulState, 1000);
            if( 0 != lRet)
            {
              /* Error starting module */
              ShowError( lRet);
            }

            lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, &ulState, 1000);
            if( 0 != lRet)
            {
              /* Error starting module */
              ShowError( lRet);
            }
            xChannelClose(hChannel);
          }
        }
        /* Close the Sysdevice */
        xSysdeviceClose( hSysdevice);
      }
    }
  }
  
  if( NULL != hDriver)
    xDriverClose(hDriver);
  
  printf("\n--- Test Loadable Modules END ----------------\r\n\n");      
}

