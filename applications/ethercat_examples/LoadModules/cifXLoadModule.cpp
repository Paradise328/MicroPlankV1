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
      1        20.06.2011  SD       created (based on win32 example)


**************************************************************************************/

#include <limits.h>

#include "cifxlinux.h"
#include "OS_Includes.h"
#include "cifXLoadModule.h"


/* ============================================================================= */
/* Functions to test */
#define LOADABLE_MODULES_WITH_CIFX_API
#define LOADABLE_MODULES_WITH_RCX_CMD
#define TEST_LOAD_EXISTING_MODULES

#ifdef LOADABLE_MODULES_WITH_CIFX_API
  void TestLoadableModulesCIFXApi(void);
#endif
#ifdef LOADABLE_MODULES_WITH_RCX_CMD
  void TestLoadableModulesrcXPacketCmd( void);
#endif
#ifdef TEST_LOAD_EXISTING_MODULES
  bool TestLoadAndRunExistingModules( void);
#endif

/* ============================================================================= */
/*  Global information and definitions */
char g_pszDeviceName[] = {"cifX0"};

char* g_tTestModule[] = { (char*)"nX100pnm.nxo",
                                  NULL};

char* g_pszFirmwarePath = (char*)".";

/* ============================================================================= */
/* Command line parameters and functions */

/* Program command line parameters       */
#define COMMAND_HELP        "/?"
#define COMMAND_CARD        "/N:"

void SetTestDevice(char* szArgument, void* pvDest)
{
  int iDest;

  sscanf(szArgument, "%x", &iDest);
  sprintf( (char*)pvDest, "cifX%d", iDest);
}

/*****************************************************************************/
/*! Set String                                                               */
/*****************************************************************************/
void SetString(char* szArgument, void* pvDest)
{
  char* szDest = reinterpret_cast<char*>(pvDest);
  OS_Strncpy(szDest, szArgument, strlen(szArgument));
}

/*****************************************************************************/
/*! Set DWORD                                                                */
/*****************************************************************************/
void SetDWORD(char* szArgument, void* pvDest)
{
  uint32_t* pulDest = reinterpret_cast<uint32_t*>(pvDest);

  sscanf(szArgument, "%x", pulDest);
}

typedef void(*PFN_COMMAND)(char*, void*);

typedef struct tagCMDLINE_PARAMS
{
  char*       szCommandPrefix;
  PFN_COMMAND pfnCommand;
  void*       pvData;
} CMDLINE_PARAM;

/*****************************************************************************/
/*! Display program help
*   \param szArgument Currently not used
*   \param pvUser     Currently not used                                     */
/*****************************************************************************/
void ShowProgramHelp( char* szArgument, void* pvUser)
{
  printf("Load Module Test Program - Help\r\n");
  printf("  Options: \r\n");
  printf("  /N:<Number of board to test [0..n]>\r\n");
  printf("  /? - Show this help\r\n\n");
}

static CMDLINE_PARAM s_atCommands[] =
{
  {(char*)COMMAND_HELP,     ShowProgramHelp, NULL},
  {(char*)COMMAND_CARD,     SetTestDevice, g_pszDeviceName}
};

/*****************************************************************************/
/*! Evaluates program arguments and calls appropriate function
*   \param argc     Number of arguments in argv
*   \param argv     Pointer to buffer containing the arguments               */
/*****************************************************************************/
bool GetProgramParameters( int argc, char* argv[])
{
  for(int iArg = 1; iArg < argc; iArg++)
  {
    bool fFound = false;

    for(unsigned int iCommand = 0; iCommand < sizeof(s_atCommands) / sizeof(s_atCommands[0]); iCommand++)
    {

      if(OS_Strnicmp(argv[iArg],
                  s_atCommands[iCommand].szCommandPrefix,
                  strlen(s_atCommands[iCommand].szCommandPrefix)) == 0)
      {
        s_atCommands[iCommand].pfnCommand(argv[iArg] + strlen(s_atCommands[iCommand].szCommandPrefix),
                                          s_atCommands[iCommand].pvData);
        fFound = true;
        break;
      }
    }

    if(!fFound)
    {
      printf("Invalid option '%s'\r\n", argv[iArg]);
      return false;
    }
  }

  return true;
}

/*****************************************************************************/
/*! Displays cifX error
*   \param lError     Error code                                             */
/*****************************************************************************/
void ShowError( int32_t lError )
{
  if( lError != CIFX_NO_ERROR)
  {
    char szError[1024] ={0};
    xDriverGetErrorDescription( lError,  szError, sizeof(szError));
    printf("Error: 0x%X, <%s>\n", (unsigned int)lError, szError);
  }
}

/*****************************************************************************/
/*! Read base system version
 * \param hDriver       Handle to the driver
 * \param pszDeviceName Name of the device
 * \return CIFX_NO_ERROR on success                                          */
/*****************************************************************************/
int32_t ReadBaseSystemName( CIFXHANDLE hDriver, char* pszDeviceName)
{
  CIFXHANDLE hSysdevice = NULL;
  int32_t    lRet       = CIFX_NO_ERROR;

  lRet = xSysdeviceOpen( hDriver, pszDeviceName, &hSysdevice);
  if( CIFX_NO_ERROR != lRet)
  {
    ShowError(lRet);
  } else
  {
    HIL_FIRMWARE_IDENTIFY_REQ_T tSendPacket     = {{0}};
    CIFX_PACKET                 tRecvPacket     = {{0}};

    /* ------------------------------ */
    /*  Create start packet           */
    /* ------------------------------ */
    tSendPacket.tHead.ulSrc   = 0x12345678;
    tSendPacket.tHead.ulDest  = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    tSendPacket.tHead.ulCmd   = HIL_FIRMWARE_IDENTIFY_REQ;
    tSendPacket.tHead.ulLen   = sizeof(HIL_FIRMWARE_IDENTIFY_REQ_DATA_T);
    tSendPacket.tHead.ulSta   = 0;
    tSendPacket.tHead.ulExt   = 0;

    tSendPacket.tData.ulChannelId = HIL_PACKET_DEST_SYSTEM;

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
        } else
        {
          /* Display information */
          HIL_FIRMWARE_IDENTIFY_CNF_T* ptFwData = (HIL_FIRMWARE_IDENTIFY_CNF_T*)&tRecvPacket;

          /* Show information */
          printf("Base Firmware\r\n");
          printf(" Name        : %.*s\r\n",        (int)ptFwData->tData.tFirmwareIdentification.tFwName.bNameLength,
                                                   ptFwData->tData.tFirmwareIdentification.tFwName.abName);
          printf(" Version     : %d.%d.%d.%d\r\n", ptFwData->tData.tFirmwareIdentification.tFwVersion.usMajor,
                                                   ptFwData->tData.tFirmwareIdentification.tFwVersion.usMinor,
                                                   ptFwData->tData.tFirmwareIdentification.tFwVersion.usBuild,
                                                   ptFwData->tData.tFirmwareIdentification.tFwVersion.usRevision);
        }
      }
    }

    xSysdeviceClose( hSysdevice);
  }

  return lRet;
}

/*****************************************************************************/
/*! Read firmware name
 * \param hDriver         Handle to the driver
 * \param pszDeviceName   Name of the device
 * \param ulChannelNumber Number of channel
 * \return CIFX_NO_ERROR on success                                          */
/*****************************************************************************/
int32_t ReadFirmwareName( CIFXHANDLE hDriver, char* pszDeviceName, unsigned long ulChannelNumber)
{
  CIFXHANDLE hChannel = NULL;
  int32_t    lRet     = CIFX_NO_ERROR;

  lRet = xChannelOpen( hDriver, pszDeviceName, ulChannelNumber, &hChannel);
  if( CIFX_NO_ERROR != lRet)
  {
    ShowError(lRet);
  } else
  {
    HIL_FIRMWARE_IDENTIFY_REQ_T tSendPacket     = {{0}};
    CIFX_PACKET                 tRecvPacket     = {{0}};

    /* ------------------------------ */
    /*  Create start packet           */
    /* ------------------------------ */
    tSendPacket.tHead.ulSrc   = 0x12345678;
    tSendPacket.tHead.ulDest  = HIL_PACKET_DEST_DEFAULT_CHANNEL;
    tSendPacket.tHead.ulCmd   = HIL_FIRMWARE_IDENTIFY_REQ;
    tSendPacket.tHead.ulLen   = sizeof(HIL_FIRMWARE_IDENTIFY_REQ_DATA_T);
    tSendPacket.tHead.ulSta   = 0;
    tSendPacket.tHead.ulExt   = 0;

    tSendPacket.tData.ulChannelId = ulChannelNumber;

      /* Send packet */
      lRet = xChannelPutPacket(hChannel, (CIFX_PACKET*)&tSendPacket, 100);
      if (CIFX_NO_ERROR != lRet)
      {
          ShowError(lRet);
      } else
      {
        /* Get answer to the start command */
      lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 1000);
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
        } else
        {
          /* Display information */
          HIL_FIRMWARE_IDENTIFY_CNF_T* ptFwData = (HIL_FIRMWARE_IDENTIFY_CNF_T*)&tRecvPacket;

          /* Show information */
          printf("Channel %u Information\r\n", (unsigned int)ulChannelNumber);
          printf(" Firmware Name        : %.*s\r\n",         (int)ptFwData->tData.tFirmwareIdentification.tFwName.bNameLength,
                                                             ptFwData->tData.tFirmwareIdentification.tFwName.abName);
          printf(" Firmware Version     : %d.%d.%d.%d\r\n",  ptFwData->tData.tFirmwareIdentification.tFwVersion.usMajor,
                                                             ptFwData->tData.tFirmwareIdentification.tFwVersion.usMinor,
                                                             ptFwData->tData.tFirmwareIdentification.tFwVersion.usBuild,
                                                             ptFwData->tData.tFirmwareIdentification.tFwVersion.usRevision);
        }
      }
    }

    xChannelClose( hChannel);
  }

  return lRet;
}

/*****************************************************************************/
/*! Download File / Module
 * \param hDriver         Handle to the driver
 * \param ulChannelNumber Number of channel
 * \param pszPath         Path to the file to download
 * \param pszModulName    Name of the file to download
 * \param ulDownloadMode  Download-Mode
 * \return CIFX_NO_ERROR on success                                          */
/*****************************************************************************/
int32_t DownloadModule( CIFXHANDLE hDevice, unsigned long ulChannelNumber, char* pszPath, char* pszModuleName, unsigned long ulDownloadMode)
{
  int32_t lRet                     = CIFX_NO_ERROR;
  char    szFullFileName[PATH_MAX] = {0};

  /* ------------------------------ */
  /* Load a file                    */
  /* ------------------------------ */
  sprintf( szFullFileName, "%s/%s", pszPath, pszModuleName);

  uint32_t   dwFileSize;
  CIFXHANDLE hFile = OS_FileOpen( szFullFileName, &dwFileSize);

  if ( hFile == NULL)
  {
    /* Error opening the file */
    printf("DownloadModule(): File <%s> open error!\r\n", pszModuleName);
    lRet = CIFX_FILE_OPEN_FAILED;
  } else
  {
    unsigned char* pabFileData = new unsigned char[dwFileSize];

    uint32_t dwBytesRead = 0;
    if ( (dwBytesRead = OS_FileRead( hFile, 0, dwFileSize, pabFileData)) != dwFileSize)
    {
      /* Error opening the file */
      printf("DownloadModule(): Error reading file <%s>\r\n", pszModuleName);
      lRet = CIFX_FILE_READ_ERROR;
    } else
    {
      /* ------------------------------ */
      /*  Download module file          */
      /* ------------------------------ */
      /* For NXO, do a file download */
      lRet = xSysdeviceDownload( hDevice, ulChannelNumber, ulDownloadMode, pszModuleName, pabFileData, dwFileSize, NULL, NULL, NULL);
    }

    delete [] pabFileData;
    OS_FileClose(hFile);
  }
  return lRet;
}


/*****************************************************************************/
/*! Function to demonstrate the board/channel enumeration
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t EnumBoard(void)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("----------------------------------------------\r\n");
  printf("---------- Board/Channel enumeration ---------\r\n");
  printf("----------------------------------------------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    unsigned long     ulBoard    = atoi( &g_pszDeviceName[4]);
    BOARD_INFORMATION tBoardInfo = {0};

    if (CIFX_NO_ERROR == (lRet = xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo)))
    {
      printf("Found Board %s\r\n", tBoardInfo.abBoardName);

      if(strlen( (char*)tBoardInfo.abBoardAlias) != 0)
        printf(" Alias        : %s\r\n", tBoardInfo.abBoardAlias);

      printf(" DeviceNumber : %u\r\n", tBoardInfo.tSystemInfo.ulDeviceNumber);
      printf(" SerialNumber : %u\r\n", tBoardInfo.tSystemInfo.ulSerialNumber);
      printf(" Board ID     : %u\r\n", tBoardInfo.ulBoardID);
      printf(" System Error : 0x%08X\r\n", tBoardInfo.ulSystemError);
      printf(" Channels     : %u\r\n", tBoardInfo.ulChannelCnt);
      printf(" DPM Size     : %u\r\n", tBoardInfo.ulDpmTotalSize);

      unsigned long       ulChannel    = 0;
      CHANNEL_INFORMATION tChannelInfo = {{0}};

       /* iterate over all channels on the current board */
      while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
      {
        printf(" - Channel %u:\r\n", (unsigned int)ulChannel);
        printf("    Firmware : %s\r\n", tChannelInfo.abFWName);
        printf("    Version  : %u.%u.%u build %u\r\n",
               tChannelInfo.usFWMajor,
               tChannelInfo.usFWMinor,
               tChannelInfo.usFWRevision,
               tChannelInfo.usFWBuild);

         printf("    Date     : %02u/%02u/%04u\r\n",
               tChannelInfo.bFWMonth,
               tChannelInfo.bFWDay,
               tChannelInfo.usFWYear);

         ++ulChannel;
      }
      printf("\n");
    } else
    {
      printf("Specified board could not be found! Error 0x%08X\n", lRet);
    }

    /* close previously opened driver */
    xDriverClose(hDriver);
  }

  printf("----------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Main cifXLoadModules - Demonstration of the firmware module handling     */
/*****************************************************************************/
int main(int argc, char* argv[])
{
  DRIVER_INFORMATION     tDriverInfo = {{0}};
  struct CIFX_LINUX_INIT init        = {0};
  int32_t                lRet        = CIFX_NO_ERROR;
  CIFXHANDLE             hDriver;
  int                    fBoardFound = 0;

  if ((argc >= 2) && (0 == OS_Strnicmp( argv[1], "/?", strlen("/?"))))
  {
    ShowProgramHelp( NULL ,NULL);
    return 0;
  }

  printf("********************************************************************************\r\n");
  printf("*** cifX Driver Module Test Program \r\n");
  printf("********************************************************************************\r\n\n");

  /* ------------------------------------------ */
  /*  Get program parameter                     */
  /* ------------------------------------------ */
  if ( GetProgramParameters( argc, argv) == 0)
  {
    ShowProgramHelp( NULL ,NULL);
    return 0;
  }

  /* driver initialization structure */
  init.init_options        = CIFX_DRIVER_INIT_AUTOSCAN, /* scan for all available devices                    */
  init.iCardNumber         = 0,                         /* ignored because of the init options               */
  init.fEnableCardLocking  = 0,                         /* deny simultaneous access from another application */
  init.base_dir            = NULL,
  init.poll_interval       = 0,
  init.poll_StackSize      = 0, /* set to 0 to used default */
  init.trace_level         = 255,
  init.user_card_cnt       = 0,
  init.user_cards          = NULL,

  /* First of all initialize toolkit */
  lRet = cifXDriverInit(&init);
  if (lRet != CIFX_NO_ERROR)
  {
    printf( "Error initializing toolkit! Error: 0x%08X\n", lRet);
    return 0;
  }

  /* open driver */
  lRet = xDriverOpen(&hDriver);
  if( lRet != CIFX_NO_ERROR)
  {
    /* Read driver error description */
    ShowError( lRet);
  } else
  {
    unsigned long ulBoardIdx  = 0;
    int32_t       lBoardRet   = CIFX_NO_ERROR;
    /* ------------------------------------------ */
    /*  Read driver information                   */
    /* ------------------------------------------ */
    xDriverGetInformation(NULL, sizeof(tDriverInfo), &tDriverInfo);

    printf("\nDriver Version\r\n");
    printf(" --> Name: %.32s \r\n", tDriverInfo.abDriverVersion);
    printf("\r\n");

    /* ------------------------------------------ */
    /*  Find out how many boards are installed    */
    /* ------------------------------------------ */
  printf("Available boards:\n\n");
    while(lBoardRet == CIFX_NO_ERROR)
    {
      /* Enumerate the boards */
      BOARD_INFORMATION tBoardInfo = {0};
      lBoardRet = xDriverEnumBoards(NULL, ulBoardIdx, sizeof(tBoardInfo), &tBoardInfo);

      if(lBoardRet != CIFX_NO_ERROR)
      {
        /* No more boards */
        break;
      }

      /* print found board */
      printf("Board %u Information\r\n", (unsigned int)ulBoardIdx);
      printf(" Name : %.16s\r\n",        tBoardInfo.abBoardName);
      printf(" Alias: %s\r\n",           tBoardInfo.abBoardAlias);
      printf(" DevNr: %u\r\n",           tBoardInfo.tSystemInfo.ulDeviceNumber);
      printf(" SN   : %u\r\n",           tBoardInfo.tSystemInfo.ulSerialNumber);
      printf("\r\n");

      fBoardFound = 1;

      ++ulBoardIdx;    /* get next board */
    }
  }
  /* close driver */
  xDriverClose( hDriver);

  printf("Tests run on %s!!!\n\n", g_pszDeviceName);

  /*--------------------------------- */
  /* Test loadable modules            */
  /*--------------------------------- */
#ifdef LOADABLE_MODULES_WITH_CIFX_API
  if( fBoardFound == true)
  {
    char bUserInput;
    int  fSkip = 1;

    /* Wait for user */
    printf("\n\r Press 'Enter' to start loading module to %s, using the cifX API! Press 'Q' to skip...\r\n", g_pszDeviceName);
    if ( (bUserInput = getchar()) != 'Q')
        fSkip = 0;

    /* read rest of line */
    if (bUserInput != '\n')
      while(fgetc(stdin) != '\n'){};

    if (fSkip == 0)
    {
        /* Test a firmware download */
      EnumBoard();
      TestLoadableModulesCIFXApi();
      EnumBoard();
      }
  }
#endif

#ifdef LOADABLE_MODULES_WITH_RCX_CMD
  if( fBoardFound == true)
  {
    char bUserInput;
    int  fSkip = 1;

    /* Wait for user */
    printf("\n\r Press 'Enter' to start loading module to %s, using the rcX commands! Press 'Q' to skip...\r\n", g_pszDeviceName);
    if ( (bUserInput = getchar()) != 'Q')
      fSkip = 0;

    /* read rest of line */
    if (bUserInput != '\n')
      while(fgetc(stdin) != '\n'){};

    if (fSkip == 0)
    {
      /* Test a firmware download */
      EnumBoard();
      TestLoadableModulesrcXPacketCmd();
      EnumBoard();
    }
  }
#endif

#ifdef TEST_LOAD_EXISTING_MODULES
  if( fBoardFound == true)
  {
    char bUserInput;
    int  fSkip = 1;

    /* Wait for user */
    printf("\n\r Press 'Enter' to start modules on %s! Press 'Q' to skip...\r\n", g_pszDeviceName);
    if ( (bUserInput = getchar()) != 'Q')
      fSkip = 0;

    /* read rest of line */
    if (bUserInput != '\n')
      while(fgetc(stdin) != '\n'){};

    if (fSkip == 0)
    {
      EnumBoard();
      TestLoadAndRunExistingModules();
      EnumBoard();
    }
  }
#endif

  printf("********************************************************************************\r\n");

  /* Wait for user */
  printf("\n\r Press 'Enter' to close program!!!!!\r\n");
  getchar();

  /* deinitialize driver */
  cifXDriverDeinit();

  return 0;
}
