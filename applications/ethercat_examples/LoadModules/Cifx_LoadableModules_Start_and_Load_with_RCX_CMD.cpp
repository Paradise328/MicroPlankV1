/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: CifX_LoadableModules_Start_and_Load_with_RCX_CMD.cpp $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 1614 $

   Targets:
     Linux : yes

   Description:
     Example how to download modules, starting them using rcx commands

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        20.06.2011  SD       created (based on win32 example)


**************************************************************************************/

#include "cifXLoadModule.h"
#include "ModuleLoader_Public.h"
#include <unistd.h>

/* ------------------------------------------------- */
/*  Information for Module TEST */
extern char* g_tTestModule[1];
extern char  g_pszDeviceName[];
extern char* g_pszFirmwarePath;

/*****************************************************************************/
/*! Start a previuosly downloaded module
*   \param  hSysdevice      Handle to the System-Device
*   \param  ulChannelNumber Number of the channel to start
*   \param  pszModulName    Nmae of the module to start
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t StartModule(CIFXHANDLE hSysdevice, unsigned long ulChannelNumber, char* pszModuleName)
{
  int32_t     lRet        = CIFX_NO_ERROR;
  CIFX_PACKET tSendPacket = {{0}};
  CIFX_PACKET tRecvPacket = {{0}};

  /* ------------------------------ */
  /*  Download module file          */
  /* ------------------------------ */
  RCX_MODLOAD_LOAD_MODULE_REQ_DATA_T*  ptRCXModuleReq = NULL;
  
  /* ------------------------------ */
  /*  Create start packet           */
  /* ------------------------------ */
  tSendPacket.tHeader.ulSrc   = 0x12345678;
  tSendPacket.tHeader.ulDest  = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  tSendPacket.tHeader.ulCmd   = RCX_MODLOAD_CMD_LOAD_AND_RUN_MODULE_REQ;
  tSendPacket.tHeader.ulLen   = 0;
  tSendPacket.tHeader.ulState = 0;
  tSendPacket.tHeader.ulExt   = 0;
  
  /* Add request specific data to the packet data area */
  ptRCXModuleReq            = (RCX_MODLOAD_LOAD_MODULE_REQ_DATA_T*)&tSendPacket.abData[0];
  ptRCXModuleReq->ulChannel = ulChannelNumber;
  
  strcpy( (char*)(ptRCXModuleReq) + sizeof(ptRCXModuleReq->ulChannel), pszModuleName);

  /* Adjust packet length */
  tSendPacket.tHeader.ulLen = (unsigned long)(sizeof(ptRCXModuleReq->ulChannel) + strlen(pszModuleName) + 1);

  /* Send packet */
  lRet = xSysdevicePutPacket(hSysdevice, &tSendPacket, 100);

  if (CIFX_NO_ERROR != lRet)
  {
    printf("xSysdevicePutPacket() error 0x%08X\r\n", (unsigned int)lRet);
  } else
  {
    /* Get answer to the start command */
    lRet = xSysdeviceGetPacket(hSysdevice, sizeof(tRecvPacket), &tRecvPacket, 15000);
    if(CIFX_NO_ERROR != lRet)
    {
      printf("xSysdeviceGetPacket() error 0x%08X\r\n", (unsigned int)lRet);
    
    /* Check if got the correct answer back */
    } else if( 0 != (lRet = tRecvPacket.tHeader.ulState))
    {
      /* Error from the card */
      printf("rcX error on module start command, Error 0x%.8X\r\n", tRecvPacket.tHeader.ulState);          
    } else
    {
      /* Wait until module is running */
      sleep(1);
    }
  }
    
  return lRet;
}


/*****************************************************************************/
/*! Delete the given file
*   \param hSysdevice         Handle to the system device
*   \param ulChannelNumber    Channel number
*   \param pszFileName        Input file name
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t DeleteFiles( CIFXHANDLE hSysdevice, unsigned long ulChannelNumber, char* pszFileName)
{
  /* Create delete packet */
  CIFX_PACKET             tSendPacket   = {{0}};
  CIFX_PACKET             tRecvPacket   = {{0}};
  HIL_FILE_DELETE_REQ_T*  ptRequest     = (HIL_FILE_DELETE_REQ_T*)&tSendPacket;
  unsigned short          usFileNameLen = (unsigned short)strlen(pszFileName);
  int32_t                 lRet          = CIFX_NO_ERROR;

  /* Initialize the message */
  ptRequest->tHead.ulSrc              = 0x12345678;;
  ptRequest->tHead.ulDest             = HIL_PACKET_DEST_SYSTEM;
  ptRequest->tHead.ulCmd              = HIL_FILE_DELETE_REQ;
  ptRequest->tHead.ulExt              = HIL_PACKET_SEQ_NONE;
  ptRequest->tHead.ulLen              = (unsigned long)(sizeof(ptRequest->tData) + usFileNameLen + 1);

  /* Insert file data */  
  ptRequest->tData.ulChannelNo        = ulChannelNumber;
  ptRequest->tData.usFileNameLength   = (usFileNameLen + 1);

  /* Insert file name */
  strncpy( (char*)(ptRequest + 1), pszFileName, usFileNameLen);

  /* Send packet */
  lRet = xSysdevicePutPacket(hSysdevice, (CIFX_PACKET*)&tSendPacket, 100);
  if (CIFX_NO_ERROR != lRet)
  {
    ShowError(lRet);
  } else
  {
	/* Get answer to the start command */
    lRet = xSysdeviceGetPacket(hSysdevice, sizeof(tRecvPacket), &tRecvPacket, 1000);
    if( CIFX_NO_ERROR != lRet)
    {
      ShowError(lRet);
    } else
    {
      /* Check if got the correct answer back */
      if( 0 != (lRet = tRecvPacket.tHeader.ulState))
      {
        /* Error from the card */
        printf("rcX packet state, Error 0x%.8X\r\n", tRecvPacket.tHeader.ulState);
      }
    }
  }
  return lRet;
}

/*****************************************************************************/
/*! Remove channel files
*   \param hSysdevice         Handle to the system device
*   \param ulChannel          Channel number
*   \param szExceptFile       File extension to skip in deleteion
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int RemoveChannelFiles(CIFXHANDLE     hSysdevice,
                       unsigned long  ulChannel,
                       char*          szExceptFile)
{
  /* Try to find file with the extension *.nxm, *.nxf, *.mod and remove it */
  CIFX_DIRECTORYENTRY tDirectoryEntry = {0};
  int32_t             lRet            = CIFX_NO_ERROR;
  int                 fFindFirst      = 1;

  /* Search for all firmware files. If one is found. delete it an start with find first again, */
  /* because we can't store a directory list in here */ 
  do
  {
    if ( fFindFirst)
    {
      /* Search first file */
      if ( !(CIFX_NO_ERROR == (lRet = xSysdeviceFindFirstFile( hSysdevice, ulChannel, &tDirectoryEntry, NULL, NULL))) )
      {
        /* No more files, or error during find first */
        break;
      } else
      {
        /* Is this a valid file name */
        int iStrlen = (int)strlen(tDirectoryEntry.szFilename); 
        if( iStrlen >= CIFX_MIN_FILE_NAME_LENGTH)  /* At least x.abc */
        {
          if( !((NULL != szExceptFile)      &&
                (4 == strlen(szExceptFile)) &&
                (0 == strncmp( szExceptFile, &tDirectoryEntry.szFilename[iStrlen - 4], 4))) )
          {
            /* Delete file and continue with find first file again */
            DeleteFiles( hSysdevice, ulChannel, tDirectoryEntry.szFilename);
          }
        } else
        {
          /* Not a valid file, search next file */
          fFindFirst = 0;
        }
      }
    } else
    {
      /* Search for more files */
      if ( !(CIFX_NO_ERROR == (lRet = xSysdeviceFindNextFile( hSysdevice, ulChannel, &tDirectoryEntry, NULL, NULL))))
      {
        /* No more files, or error during find next */
        break;
      } else
      {
        /* Is this a valid file name */
        int iStrlen = (int)strlen(tDirectoryEntry.szFilename); 
        if( iStrlen >= CIFX_MIN_FILE_NAME_LENGTH)  /* At least x.abc */
        {
          /* If firmware file, delete it, else search until all files checked */
          if( !((NULL != szExceptFile)      &&
                (4 == strlen(szExceptFile)) &&
                (0 == strncmp( szExceptFile, &tDirectoryEntry.szFilename[iStrlen - 4], 4))) )
          {
            /* Delete the file and start with find first again */
            DeleteFiles( hSysdevice, ulChannel, tDirectoryEntry.szFilename);
            fFindFirst = 1;
          }
        }
      }
    }
  } while ( CIFX_NO_ERROR == lRet);

  return 1;
}


/*****************************************************************************/
/*! Test loadable modules                                                    */
/*****************************************************************************/
void TestLoadableModulesrcXPacketCmd( void)
{
  CIFXHANDLE hDriver      = NULL;
  CIFXHANDLE hSysdevice   = NULL;
  int32_t    lRet         = CIFX_NO_ERROR;
  
  /* Download parameter */
  char*          pszModuleName  = NULL;;
  unsigned long  lChannelNumber = 0;
  int            iIdx;

  printf("\n----------------------------------------------\r\n");
  printf("---------- Test netX Loadable Modules --------\r\n");  
  printf("----------------------------------------------\r\n");
  
  /* -------------------------- */
  /*  Open the driver           */
  /* -------------------------- */
  lRet = xDriverOpen( &hDriver);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("Failed to open the driver, Error = 0x%08X \n", (unsigned int)lRet);
  } else
  {
    /* --------------------------------------- */
    /*  Read the base module firmware version  */
    /* --------------------------------------- */
    ReadBaseSystemName( hDriver, g_pszDeviceName);

    /* --------------------------------------- */
    /*  Process all defined modules            */
    /* --------------------------------------- */
    for ( iIdx = 0; NULL != (pszModuleName = g_tTestModule[iIdx]); iIdx++)
    {
      lChannelNumber = iIdx;
      printf("\n--> Testing Module: %s on Channel: %lu \r\n", pszModuleName, lChannelNumber);

      /* -------------------------------------------------------------------- */
      /* Delete all Modules in the given channel, except the 2nd stage loader */
      /* -------------------------------------------------------------------- */
      /* Open the Sysdevice */
      lRet = xSysdeviceOpen( hDriver, g_pszDeviceName, &hSysdevice);
      if( CIFX_NO_ERROR != lRet)
      {
        printf("Failed to open the Sysdevice, Error = 0x%.8lx \n", (long)lRet);
      } else
      {
    	/* -------------------------------------------------------------------- */
        /* Remove already existing files                                        */
        /* Skip the NXF because this is our Base OS Module                      */
        /* -------------------------------------------------------------------- */
        RemoveChannelFiles( hSysdevice, lChannelNumber, (char*)".NXF");
      
        /* ---------------------------------------------------------------------- */
        /* Download the module into the file system by just doing a file download */
        /* ---------------------------------------------------------------------- */
        lRet = DownloadModule( hSysdevice, lChannelNumber, g_pszFirmwarePath, pszModuleName, DOWNLOAD_MODE_FILE);
        if( CIFX_NO_ERROR != lRet)
        {
          /* Read driver error description and display error */
          ShowError( lRet);
        } else
        {
          printf("Module download was successfull!\n\n");
          printf("--> Try to start firmware module...\n");
          /* -------------------------------------------------------------------- */
          /* Start the downloaded module                                          */
          /* -------------------------------------------------------------------- */
          lRet = StartModule( hSysdevice, lChannelNumber, pszModuleName);
          if( 0 != lRet)
          {
            /* Error starting module */
            ShowError( lRet);
          } else
          {
            printf("--> Try to read firmware information...\n");
        	/* -------------------------------------------------------------------- */
            /* Module successfully started                                          */
        	/* -------------------------------------------------------------------- */
            ReadFirmwareName( hDriver, g_pszDeviceName, lChannelNumber);
          }
        }
        
        if( CIFX_NO_ERROR == lRet)
        {
          /* -------------------------------------------------------------------- */
          /* Check if module is working                                           */
          /* -------------------------------------------------------------------- */
          CIFXHANDLE hChannel = NULL;
          
          /* Check Channel functions */
          lRet = xChannelOpen(hDriver, g_pszDeviceName, lChannelNumber, &hChannel);
          if( CIFX_NO_ERROR != lRet)
          {
            /* Error starting module */
            ShowError( lRet);
          } else
          {
            uint32_t ulState = 0;
            
            lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_NOT_READY, &ulState, 1000);
            if( CIFX_NO_ERROR != lRet)
            {
              /* Error starting module */
              ShowError( lRet);
            }
            
            lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, &ulState, 1000);
            if( CIFX_NO_ERROR != lRet)
            {
              /* Error starting module */
              ShowError( lRet);
            }
            /* Close communication channel */
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


