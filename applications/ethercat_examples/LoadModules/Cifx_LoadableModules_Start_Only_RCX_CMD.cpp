/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: Cifx_LoadableModules_Start_Only_RCX_CMD.cpp $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 1614 $

   Targets:
     Linux : yes

   Description:
     Example how to start a module using rcx commands

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        20.06.2011  SD       created (based on win32 example)


**************************************************************************************/

#include "cifxlinux.h"
#include "cifXLoadModule.h"

#define _MAX_FNAME 255
#define _MAX_EXT   255
#define _MAX_PATH  255

/* ------------------------------------------------- */
/* Information for Module TEST */
extern char*  tTestModule[];
extern char   g_pszDeviceName[];
extern char*  pszPathName;

/*****************************************************************************/
/*! Search a NXO file in the file system
*   \param hSysdevice  Handle to the System-Device
*   \param ulChannel   Number of channel, where to search for
*   \param pszFileName Name of the file to search for
*   \return !=0 on success                                                   */
/*****************************************************************************/
int SearchNXOFile( CIFXHANDLE hSysdevice, unsigned long ulChannel, char* pszFileName)
{
  CIFX_DIRECTORYENTRY   tDirectoryEntry = {0};            // File information structure
  int32_t               lRet            = CIFX_NO_ERROR;
  int                   fRet            = 0;
  int                   fFindFirst      = 1;

  /* Search existing modules in the file system */
  do
  {
    if ( fFindFirst)
    {
      /* Search first NXO file */
      if ( !(CIFX_NO_ERROR == (lRet = xSysdeviceFindFirstFile( hSysdevice, ulChannel, &tDirectoryEntry, NULL, NULL))) )
      {
        /* No more files, or error during find first */
        break;
      }
      fFindFirst = 0;
    } else
    {
      /* Search for more files */
      if ( !(CIFX_NO_ERROR == (lRet = xSysdeviceFindNextFile( hSysdevice, ulChannel, &tDirectoryEntry, NULL, NULL))))
      {
        /* No more files, or error during find next */
        break;
      }
    }

    /* Check filename */
    /* Is this a valid file name */
    //char  szFileName[_MAX_FNAME] = {0};
    //char  szExtension[_MAX_EXT]  = {0};
    char* pszExtension = NULL;

    //Tcl_SplitPath( &tDirectoryEntry.szFilename[0], NULL, NULL, &szFileName[0], &szExtension[0]);
    pszExtension = strrchr( &tDirectoryEntry.szFilename[0],'.');

    if( 0 != strlen(pszExtension))
    {
      if( 0 == strcmp( pszExtension, ".NXO"))
      {
        /* We have a valid extension, return the file name */
        strcpy( pszFileName, &tDirectoryEntry.szFilename[0]);

        fRet = 1;
        break;
      }
    }
  } while ( CIFX_NO_ERROR == lRet);

  return fRet;
}

/*****************************************************************************/
/*! Load and Run already existing Modules
*   \return !=0 on success                                                   */
/*****************************************************************************/
int TestLoadAndRunExistingModules( void)
{
  CIFXHANDLE      hDriver               = NULL;
  CIFXHANDLE      hSysdevice            = NULL;
  unsigned long   ulChannelNumber       = 0;
  char            szFileName[_MAX_PATH] = {0};
  int32_t         lRet                  = CIFX_NO_ERROR;
  int             fRet                  = 0;
  int             iIdx                  = 0;

  /* Search existing modules in the file system */
  printf("\n-----------------------------------------------\r\n");
  printf("--- Search and Start existing modules START ---\r\n");
  printf("-----------------------------------------------\r\n");

  /* -------------------------- */
  /*  Open the driver           */
  /* -------------------------- */
  lRet = xDriverOpen( &hDriver);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("Failed to open the driver, Error = 0x%08lx \n", (long unsigned int)lRet);
  } else
  {
    int fRestart = 0;
    /* --------------------------------------------------------------- */
    /*  Start with system reset, so we are sure no module is running   */
    /*  Only usable on FLASH based devices (not CIFX). Because on      */
    /*  CIFX cards, fiels are not loaded into the file system          */
    /* --------------------------------------------------------------- */

    /* Open the Sysdevice */
    lRet = xSysdeviceOpen( hDriver, g_pszDeviceName, &hSysdevice);
    if( CIFX_NO_ERROR != lRet)
    {
      printf("Failed to open the Sysdevice, Error = 0x%08lx \n", (long unsigned int)lRet);
    } else
    {
      /* Check if we have a FLASH based device */
      SYSTEM_CHANNEL_SYSTEM_STATUS_BLOCK tSystemInfo = {0};

      if ( CIFX_NO_ERROR != (lRet = xSysdeviceInfo( hSysdevice, CIFX_INFO_CMD_SYSTEM_STATUS_BLOCK, sizeof(tSystemInfo), &tSystemInfo)))
      {
        /* Failed to read system information */
        ShowError(lRet);

      } else if ( !(tSystemInfo.ulSystemStatus & HIL_SYS_STATUS_NXO_SUPPORTED))
      {
        /* No NXO supported by the firmare */
        printf("No NXOs supported by the firmware \n");

      } else if ( !(tSystemInfo.ulSystemStatus & HIL_SYS_STATUS_SYSVOLUME_FFS))
      {
        /* No Flash file system available, modules can't be searched by this function */
        printf("No Flash file system available, modules can't be searched by this function \n");

      } else
      {
        /* Start with a SYSTEMREST, this will load only the base OS firmware and no */
        /* none of the modules are running anymore                                  */
        if ( CIFX_NO_ERROR != (lRet = xSysdeviceReset( hSysdevice, 5000)))
        {
          /* Failed to reset device */
          ShowError(lRet);
        } else
        {
          /* Process all channels */
          for (iIdx = 0; iIdx < CIFX_MAX_NUMBER_OF_CHANNELS; iIdx++)
          {
            ulChannelNumber = iIdx;

            if ( SearchNXOFile( hSysdevice, ulChannelNumber, &szFileName[0]))
            {
              /* We found a module, try to start it */
              if ( CIFX_NO_ERROR != (lRet = StartModule( hSysdevice, ulChannelNumber, szFileName)))
              {
                ShowError(lRet);
              } else
              {
                fRestart = 1;
              }
            }
          }
        }
      }
    }

    /* Close the Sysdevice */
    xSysdeviceClose( hSysdevice);

    /* Restart the CIFX driver to re-read the channel information and the actual running firmware names */
    if ( fRestart)
      xDriverRestartDevice( hDriver, g_pszDeviceName, NULL);

  }

  if( NULL != hDriver)
    xDriverClose(hDriver);

  printf("\n--- Search and Start existing modules END ---\r\n\n");

  return fRet;
}


