/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   $Id: HilMarshaller.c 13309 2019-11-05 12:03:27Z AlexanderMinor $:

   Targets:
     Linux        : yes

   Description:
    Server handling

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    11.11.2019  Deprecated TLR definitions removed
    04.09.2019  Added new function xSysdeviceResetEx()
    09.05.2018  segfault when enumerating network adapters
    02.08.2017  added auto check for config file storage & removed
                singledir option USESINGLE_DIRECTORY
    10.07.2014  print server information
    30.04.2013  add support for firmware single directory file storage
                (see USESINGLE_DIRECTORY)
    16.11.2011  add file download support (HandleFileStorage())
                add optional program arguments
                - port number specification
                - select and initialize only specified card
    02.06.2010  initial version

**************************************************************************************/

#include <ifaddrs.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#include "TCPConnector.h"
#include "TCPServer.h"
#include "cifxlinux.h"
#include "cifXDownloadHook.h"
#include "MarshallerErrors.h"

/* marshaller handle */
void*            g_pvMarshaller = NULL;
/* flag for main loop */
int             g_fRunning      = 0;
/* flag to display once the network traffic */
int             g_fTrafficOnce  = 0;

unsigned short   g_usPortNumber = HIL_TRANSPORT_IP_PORT;

struct CIFX_LINUX_INIT  g_tInit = {0};

pthread_mutex_t* g_ptMutex;

typedef struct INIT_PARAM_Ttag
{
  int fUseSingleCard;
  int  iCardNumber;
  int  iPortNumber;

} INIT_PARAM_T;

INIT_PARAM_T g_tInitParam = {0};


/*****************************************************************************/
/*! Function to demonstrate the board/channel enumeration
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t EnumBoardDemo(void)
{
  CIFXHANDLE hDriver = NULL;
  long   lRet    = xDriverOpen(&hDriver);

  printf("---------- Available Cards ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    unsigned long     ulBoard    = 0;
    BOARD_INFORMATION tBoardInfo = {0};

    /* Iterate over all boards */
    while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
    {
      printf("%d.: %s\r\n", tBoardInfo.ulBoardID +1, tBoardInfo.abBoardName);
      if(strlen( (char*)tBoardInfo.abBoardAlias) != 0)
        printf("    Alias        : %s\r\n", tBoardInfo.abBoardAlias);

      printf("    DeviceNumber : %lu\r\n",(long unsigned int)tBoardInfo.tSystemInfo.ulDeviceNumber);
      printf("    SerialNumber : %lu\r\n",(long unsigned int)tBoardInfo.tSystemInfo.ulSerialNumber);

      unsigned long       ulChannel    = 0;
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      /* iterate over all channels on the current board */
      while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
      {
        printf("    - Channel %lu:\r\n", ulChannel);
        printf("      Firmware : %s\r\n", tChannelInfo.abFWName);
        printf("      Version  : %u.%u.%u build %u\r\n",
               tChannelInfo.usFWMajor,
               tChannelInfo.usFWMinor,
               tChannelInfo.usFWRevision,
               tChannelInfo.usFWBuild);

        ++ulChannel;
      }

      ++ulBoard;
      printf("----------------------------------------------------\r\n");
    }

    /* close previously opened driver */
    xDriverClose(hDriver);
  }

  return lRet;
}

/*****************************************************************************/
/*! Function display IP of the available network adapter                     */
/*****************************************************************************/
void DisplayIP(void)
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  getifaddrs(&ifAddrStruct);

  if (ifAddrStruct != NULL) {
    printf("Interface Name\t: IP\n");
    printf("---------------------------\n");
  }

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
  {
    /* if IP4 */
    if ( (ifa->ifa_addr != NULL) && (ifa->ifa_addr->sa_family == AF_INET) )
    {
      if (OS_Strnicmp("lo", ifa->ifa_name, strlen(ifa->ifa_name)))
      {
        tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

        printf("%s\t\t: %s\n", ifa->ifa_name, addressBuffer);
      }
    }
  }

  printf("\n");

  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
}


/*****************************************************************************/
/*! Function display optional arguments                                      */
/*****************************************************************************/
void DisplayHelp(void)
{
  printf("The cifXTCPServer application is a demo application for remote access of a cifX device under Linux!");
  printf("The application provides a file transfer mechanism, to be able to configure a device via Sycon.net\n");
  printf("from a Windows client.\n");
  printf("For custom purposes the application needs to be adapted!\n");
  printf("Available options:\n");
  printf("[-n <n>] initialize only a specific card specified by 'n'.\n");
  printf("[-p <n>] use port number specified by 'n'.\n");
  printf("[-d] display IP adress of the active adapter and return.\n");
  printf("[-a] display available cards and return.\n");
  printf("[-h] display this help.\n");

  printf("Example:\n");
  printf("cifXTCPServer -n 0 -p 51234\n");
}


/*****************************************************************************/
/*! Function evaluates input arguments                                       */
/*****************************************************************************/
int ValidateArgs(int argc, char* argv[])
{
  int fRet    = 1;
  int iArgCnt = 0;

  if (argc > 1)
  {
    /* ignore if help is not first param */
    if (0 == strcasecmp("-h", argv[1]))
    {
      DisplayHelp();
      return 0;
    }

    /* iterate over options check only -d & -a  */
    for (iArgCnt = 1; iArgCnt<argc; iArgCnt++)
    {
      if (0 == strcasecmp("-d", argv[iArgCnt]))
      {
        /* only display the active adapter and its IP */
        fRet = 0;

        /* display ip of the active adapter */
        DisplayIP();

      }else if (0 == strcasecmp("-a", argv[iArgCnt]))
      {
        /* only display all available cards */
        fRet = 0;

        /* struct to initialize toolkit */
        g_tInit.init_options        = CIFX_DRIVER_INIT_AUTOSCAN;
        g_tInit.iCardNumber         = 0;
        g_tInit.fEnableCardLocking  = 0;
        g_tInit.base_dir            = NULL;
        g_tInit.poll_interval       = 0;
        g_tInit.poll_StackSize      = 0;
        g_tInit.trace_level         = 255;
        g_tInit.user_card_cnt       = 0;

        /* First of all initialize toolkit */
        if (CIFX_NO_ERROR == cifXDriverInit(&g_tInit))
        {
          /* display ip of the active adapter */
          EnumBoardDemo();
        }
      }
    }
    /* if -d or -a option is used return */
    if (fRet == 0)
      return fRet;

    /* check options */
    for (iArgCnt = 1; ((iArgCnt<argc) && (fRet == 1)); iArgCnt++)
    {
      if (0 == strcasecmp( "-n", argv[iArgCnt]))
      {
        g_tInitParam.fUseSingleCard = 1;
        iArgCnt++;
        if ((iArgCnt) < argc)
        {
          g_tInitParam.iCardNumber = atoi( argv[iArgCnt]);
          printf("Select card: cifX%d!\n", g_tInitParam.iCardNumber);

        } else
        {
          fRet = 0;
        }
      }else if (0 == strcasecmp("-p", argv[iArgCnt]))
      {
        iArgCnt++;
        if ((iArgCnt) < argc)
        {
          g_usPortNumber = atoi( argv[iArgCnt]);
          printf("Use port number: %d!\n", g_usPortNumber);

        } else
        {
          fRet = 0;
        }
      }else if ((0 == strcasecmp("-d", argv[iArgCnt])) || (0 == strcasecmp("-a", argv[iArgCnt])) || (0 == strcasecmp("-h", argv[iArgCnt])))
      {
        /* already done */
      }else
      {
        fRet = 0;
      }
    }
    if (0 == fRet)
    {
      printf("Invalid argument!\n");
      DisplayHelp();
    }
  }

  return fRet;
}

/*****************************************************************************/
/*! Timer refreshing TCP traffic                                             */
/*****************************************************************************/
void TrafficTimer(void* dwUser)
{
  TCP_CONN_INTERNAL_T* ptTcpData = (TCP_CONN_INTERNAL_T*)dwUser;
  g_fTrafficOnce = 1;

  /* insert a handling to write down or display the traffic */

  printf("\nRX[Bytes]: %lu\nTX[Bytes]: %lu\n", ptTcpData->ulRxCount, ptTcpData->ulTxCount);
}

/*****************************************************************************/
/*! This function is called after each interval specified in the setitimer
 *  function (see InitMarshaller) used to install a timer.
 *    \param iSignal   Signal which caused the call                          */
/*****************************************************************************/
void MarshallerTimer(int iSignal)
{
  HilMarshallerTimer(g_pvMarshaller);
}

/*****************************************************************************/
/*! Wrapper for xSysdeviceOpen to track sysdevice access
*   \param hDriver      Driver handle
*   \param szBoard      Name of the board to open
*   \param phSysdevice  Returned handle to the System device area
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t APIENTRY xSysdeviceOpenWrap ( CIFXHANDLE  hDriver, char*   szBoard, CIFXHANDLE* phSysdevice)
{
  int32_t lRet  = CIFX_NO_ERROR;

  if (CIFX_NO_ERROR == (lRet = xSysdeviceOpen( hDriver, szBoard, phSysdevice)))
  {
    /* add here additional code for print out or display handling */

    printf("Open device \"%s\"\n", szBoard);

  }

  return lRet;
}

/*****************************************************************************/
/*! Wrapper for xSysdeviceClose to track sysdevice access
*   \param hSysdevice  Handle to the System device to close
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t APIENTRY xSysdeviceCloseWrap ( CIFXHANDLE  hSysdevice)
{
  int32_t lRet  = CIFX_NO_ERROR;

  if (CIFX_NO_ERROR == (lRet = xSysdeviceClose( hSysdevice)))
  {
    /* add here additional code for print out or display handling */
    printf("Close device\n");
  }

  return lRet;
}

/*****************************************************************************/
/*! Wrapper for xChannelOpen to track channel access
*   \param hDriver    Driver handle
*   \param szBoard    DOS Device Name of the Board to open
*   \param ulChannel  Channel number to open (0..n)
*   \param phChannel  Returned handle to the channel (Needed for all channel
*                     specific operations)
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t APIENTRY xChannelOpenWrap  ( CIFXHANDLE  hDriver,  char* szBoard, uint32_t ulChannel, CIFXHANDLE* phChannel)
{
  int32_t lRet  = CIFX_NO_ERROR;

  if (CIFX_NO_ERROR == (lRet = xChannelOpen( hDriver, szBoard, ulChannel, phChannel)))
  {
    /* add here additional code for print out or display handling */
    printf("Open channel %d of device \"%s\"", ulChannel, szBoard);

  }

  return lRet;
}

/*****************************************************************************/
/*! Wrapper for xChannelClose to track channel access
*   \param hChannel  Handle to the channel device to close
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t APIENTRY xChannelCloseWrap ( CIFXHANDLE  hChannel)
{
  int32_t lRet  = CIFX_NO_ERROR;

  if (CIFX_NO_ERROR == (lRet = xChannelClose(hChannel)))
  {
    /* add here additional code for print out or display handling */

    printf("Close channel\n");
  }

  return lRet;
}

/*****************************************************************************/
/*! Function for Marshaller Request                                          */
/*****************************************************************************/
void MarshallerRequest(void* pvMarshaller, void* pvUser)
{
  UNREFERENCED_PARAMETER(pvUser);
  HilMarshallerMain(pvMarshaller);
}

/*****************************************************************************/
/* Destroy marshallar and deinit driver
 * if SIGINT (ctrl +c)                                                       */
/*****************************************************************************/
void DeInitServer(int iSignal)
{
  DeinitMarshaller();
  cifXDriverDeinit();

  OS_DeleteLock(g_ptMutex);
  g_fRunning = 0;
}

/*****************************************************************************/
/*! File storage callback (uses NX-API)                                      */
/*****************************************************************************/
static int32_t HandleFileStorage(BOARD_INFORMATION* ptBoardInfo,
                                 char* pszFileName, uint32_t ulFileSize,
                                 uint8_t* pabFileData, uint32_t ulChannel,
                                 uint32_t ulDownloadMode, void* pvUser)
{
  int32_t                 lRet   = CIFX_FUNCTION_FAILED;
  struct CIFX_LINUX_INIT* ptInit = (struct CIFX_LINUX_INIT*)(pvUser);
  char                    abBaseDir[FILENAME_MAX];
  char                    abFileDir[FILENAME_MAX];
  char                    abFileName[FILENAME_MAX];
  FILE*                   iFd;

  if (NULL != ptInit->base_dir)
  {
    OS_STRNCPY( abBaseDir, ptInit->base_dir, sizeof(ptInit->base_dir)/sizeof(ptInit->base_dir[0]));
  }else
  {
    sprintf( abBaseDir, "/opt/cifx/deviceconfig");
  }
  /* check if configuration files are stored using slot number */
  if (ptBoardInfo->tSystemInfo.bDevIdNumber)
  {
    sprintf( abFileDir, "%s/Slot_%d/channel%d", abBaseDir, ptBoardInfo->tSystemInfo.bDevIdNumber, ulChannel);
  } else
  {
    struct stat tFWDirStatus;

    sprintf( abFileDir, "%s/%d/%d/channel%d", abBaseDir, ptBoardInfo->tSystemInfo.ulDeviceNumber, ptBoardInfo->tSystemInfo.ulSerialNumber, ulChannel);

    if ( (stat(abFileDir, &tFWDirStatus) != 0) || (S_ISDIR(tFWDirStatus.st_mode) == 0) ) {
      /* in case directory does not exist try to accesss single directory storage */
      sprintf( abFileDir, "%s/FW/channel%d/", abBaseDir, ulChannel);
    }
  }
  sprintf( abFileName, "%s/%s", abFileDir, pszFileName);

  printf("Store file: %s\n", abFileName);
  if ( NULL != (iFd = fopen( abFileName, "w+")))
  {
    int iFileSize;
    if ( 0 < (iFileSize = fwrite( pabFileData, 1, ulFileSize, iFd)))
    {
      if ((((unsigned int)iFileSize) == ulFileSize))
        lRet = CIFX_NO_ERROR;
    } else
    {
      printf("File storing failed %d\n", iFileSize);
    }
    fclose( iFd);
  } else
  {
    printf("File open failed (error=%d)!\n",errno);
    printf("Please create directory in case path does not exist!\n");
    lRet = CIFX_FILE_OPEN_FAILED;
  }
  if (( DOWNLOAD_MODE_FIRMWARE == ulDownloadMode) && (lRet == CIFX_NO_ERROR))
  {
    /* if download succeeded restart device */
    xDriverRestartDevice ( NULL, ptBoardInfo->abBoardName, NULL);
  }

  return lRet;
}

/*****************************************************************************/
/*! Initialization the Marshallar                                         */
/*****************************************************************************/
uint32_t InitMarshaller(void)
{
  HIL_MARSHALLER_PARAMS_T           tParams        = {{0}};
  HIL_MARSHALLER_CONNECTOR_PARAMS_T tTCPConnector  = {0};

  tTCPConnector.pfnConnectorInit = TCPConnectorInit;
  tTCPConnector.pvConfigData     = NULL;
  tTCPConnector.ulDataBufferCnt  = 1;
  tTCPConnector.ulDataBufferSize = 6000;
  tTCPConnector.ulTimeout        = 1000;

  TRANSPORT_LAYER_CONFIG_T          tCifXTransport = {0};
  CIFX_TRANSPORT_CONFIG             tCifXConfig    = {{0}};

  tCifXConfig.tDRVFunctions.pfnxDriverOpen                 = xDriverOpen;
  tCifXConfig.tDRVFunctions.pfnxDriverClose                = xDriverClose;
  tCifXConfig.tDRVFunctions.pfnxDriverGetInformation       = xDriverGetInformation;
  tCifXConfig.tDRVFunctions.pfnxDriverGetErrorDescription  = xDriverGetErrorDescription;
  tCifXConfig.tDRVFunctions.pfnxDriverEnumBoards           = xDriverEnumBoards;
  tCifXConfig.tDRVFunctions.pfnxDriverEnumChannels         = xDriverEnumChannels;
  tCifXConfig.tDRVFunctions.pfnxDriverMemoryPointer        = xDriverMemoryPointer;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceOpen              = xSysdeviceOpenWrap;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceClose             = xSysdeviceCloseWrap;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceReset             = xSysdeviceReset;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceResetEx           = xSysdeviceResetEx;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceGetMBXState       = xSysdeviceGetMBXState;
  tCifXConfig.tDRVFunctions.pfnxSysdevicePutPacket         = xSysdevicePutPacket;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceGetPacket         = xSysdeviceGetPacket;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceDownload          = xSysdeviceDownload;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceInfo              = xSysdeviceInfo;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceFindFirstFile     = xSysdeviceFindFirstFile;
  tCifXConfig.tDRVFunctions.pfnxSysdeviceFindNextFile      = xSysdeviceFindNextFile;
  tCifXConfig.tDRVFunctions.pfnxChannelOpen                = xChannelOpenWrap;
  tCifXConfig.tDRVFunctions.pfnxChannelClose               = xChannelCloseWrap;
  tCifXConfig.tDRVFunctions.pfnxChannelDownload            = xChannelDownload;
  tCifXConfig.tDRVFunctions.pfnxChannelGetMBXState         = xChannelGetMBXState;
  tCifXConfig.tDRVFunctions.pfnxChannelPutPacket           = xChannelPutPacket;
  tCifXConfig.tDRVFunctions.pfnxChannelGetPacket           = xChannelGetPacket;
  tCifXConfig.tDRVFunctions.pfnxChannelGetSendPacket       = xChannelGetSendPacket;
  tCifXConfig.tDRVFunctions.pfnxChannelConfigLock          = xChannelConfigLock;
  tCifXConfig.tDRVFunctions.pfnxChannelReset               = xChannelReset;
  tCifXConfig.tDRVFunctions.pfnxChannelInfo                = xChannelInfo;
  tCifXConfig.tDRVFunctions.pfnxChannelWatchdog            = xChannelWatchdog;
  tCifXConfig.tDRVFunctions.pfnxChannelHostState           = xChannelHostState;
  tCifXConfig.tDRVFunctions.pfnxChannelBusState            = xChannelBusState;
  tCifXConfig.tDRVFunctions.pfnxChannelIORead              = xChannelIORead;
  tCifXConfig.tDRVFunctions.pfnxChannelIOWrite             = xChannelIOWrite;
  tCifXConfig.tDRVFunctions.pfnxChannelIOReadSendData      = xChannelIOReadSendData;
  tCifXConfig.tDRVFunctions.pfnxChannelControlBlock        = xChannelControlBlock;
  tCifXConfig.tDRVFunctions.pfnxChannelCommonStatusBlock   = xChannelCommonStatusBlock;
  tCifXConfig.tDRVFunctions.pfnxChannelExtendedStatusBlock = xChannelExtendedStatusBlock;
  tCifXConfig.tDRVFunctions.pfnxChannelPLCMemoryPtr        = xChannelPLCMemoryPtr;
  tCifXConfig.tDRVFunctions.pfnxChannelPLCIsReadReady      = xChannelPLCIsReadReady;
  tCifXConfig.tDRVFunctions.pfnxChannelPLCIsWriteReady     = xChannelPLCIsWriteReady;
  tCifXConfig.tDRVFunctions.pfnxChannelPLCActivateWrite    = xChannelPLCActivateWrite;
  tCifXConfig.tDRVFunctions.pfnxChannelPLCActivateRead     = xChannelPLCActivateRead;
  tCifXConfig.tDRVFunctions.pfnxChannelFindFirstFile       = xChannelFindFirstFile;
  tCifXConfig.tDRVFunctions.pfnxChannelFindNextFile        = xChannelFindNextFile;

  /* Install download hook */
  xDownloadHook_Install(&tCifXConfig.tDRVFunctions, HandleFileStorage, &g_tInit);

  tCifXTransport.pfnInit  = cifXTransportInit;
  tCifXTransport.pvConfig = &tCifXConfig;

  tParams.ulMaxConnectors = 1;
  tParams.atTransports    = &tCifXTransport;
  tParams.ulTransportCnt  = 1;

  tParams.ptConnectors    = &tTCPConnector;
  tParams.ulConnectorCnt  = 1;

  uint32_t eRet = HilMarshallerStart(&tParams, &g_pvMarshaller, MarshallerRequest, 0);

  if ( MARSHALLER_NO_ERROR == eRet)
  {
    /* interval = 10ms */
    struct itimerval pITimerVal;
    pITimerVal.it_value.tv_sec     = 0;
    pITimerVal.it_value.tv_usec    = 10000;
    pITimerVal.it_interval.tv_sec  = 0;
    pITimerVal.it_interval.tv_usec = 10000;

    setitimer(ITIMER_REAL,&pITimerVal,NULL);

    struct sigaction tSigTimer;

    sigemptyset(&tSigTimer.sa_mask);

    /* timer event for timeout */
    tSigTimer.sa_handler = MarshallerTimer;
    tSigTimer.sa_flags = 0;

    sigaction(SIGALRM,&tSigTimer,NULL);
  }

  return eRet;
}

/*****************************************************************************/
/* Destroy the Marshallar */
/*****************************************************************************/
void DeinitMarshaller()
{
  setitimer(ITIMER_REAL,NULL,NULL);

  if(NULL != g_pvMarshaller)
  {
    printf("\nWaiting for all process to end...\n");
    HilMarshallerStop(g_pvMarshaller);
  }
}

/*****************************************************************************/
/* Displays information of the server (cards under control and IP)           */
/*****************************************************************************/
void PrintServerInformation(void)
{
  CIFXHANDLE hDriver = NULL;
  long       lRet    = xDriverOpen(&hDriver);

  printf("\n******************************************************************************\n");
  printf("The following card(s) are now accessible via TCP/IP remote connection:\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    unsigned long     ulBoard    = 0;
    BOARD_INFORMATION tBoardInfo = {0};
    /* Iterate over all boards */
    while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
    {
      printf("%d.: %s\r\n", tBoardInfo.ulBoardID +1, tBoardInfo.abBoardName);
      ++ulBoard;
    }
    xDriverClose(hDriver);
  }
  printf("\nThe server is reachable under:\n");
  DisplayIP();
  printf("******************************************************************************\n");
}


int main( int argc, char* argv[])
{
  long             lRet = CIFX_NO_ERROR;
  struct sigaction tSigTerm;

  if (0 == ValidateArgs(argc, argv))
    return 0;

  sigemptyset(&tSigTerm.sa_mask);
  tSigTerm.sa_handler = DeInitServer;
  tSigTerm.sa_flags = 0;

  /* catch "ctrl + c" signal */
  sigaction(SIGINT,&tSigTerm,NULL);

  /* set to default values */
  g_tInit.init_options        = CIFX_DRIVER_INIT_AUTOSCAN;
  g_tInit.iCardNumber         = 0;
  g_tInit.fEnableCardLocking  = 0;
  g_tInit.base_dir            = NULL;
  g_tInit.poll_interval       = 0;
  g_tInit.poll_StackSize      = 0;
  g_tInit.trace_level         = 255;
  g_tInit.user_card_cnt       = 0;

  if (g_tInitParam.fUseSingleCard)
  {
    /* initialize toolkit to scan only for the specified card */
    g_tInit.init_options   = CIFX_DRIVER_INIT_CARDNUMBER;
    g_tInit.iCardNumber    = g_tInitParam.iCardNumber;
  }

  printf("cifXDriverInit...\n");
  /* First of all initialize toolkit */
  lRet = cifXDriverInit(&g_tInit);

  g_ptMutex = OS_CreateLock();

  if(CIFX_NO_ERROR == lRet)
  {
      if (MARSHALLER_NO_ERROR == (lRet = InitMarshaller()))
      {
            printf("Press ctrl+'c' to quit!\n");
            g_fRunning = 1;

            /* display the current server settings */
            PrintServerInformation();
          } else
      {
        printf("Marshaller initialization failed!\n");
      }
  } else
  {
    printf("CifXDriver initialization failed!\n");
  }

  if (MARSHALLER_NO_ERROR != lRet) kill(0,SIGINT);

  /* main loop */
  while( g_fRunning ){
    usleep(250000);
  };

  return 0;
}
