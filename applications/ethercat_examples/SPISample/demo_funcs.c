/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: spisample.c $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 9.02.10 15:45 $
    $Revision: 3961 $

   Targets:
     Linux : yes

   Platform:

   Description:
     Linux API demo functions

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        01.07.2014  SD       - Initial Version (based on cifxsample)

**************************************************************************************/

#include "cifxlinux.h"
#include "cifXEndianess.h"
#include "Hil_Packet.h"
#include "Hil_SystemCmd.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <pthread.h>

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(a) (a=a)
#endif

static int kbhit()
{
  struct termios oldt, newt;
  int    ch;
  int    oldf;
  int    iRet = 0;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    iRet = 1;
  }
  return iRet;
}

void* CreateLock(void) {
  pthread_mutexattr_t mta;
  pthread_mutex_t     *mutex;
  int                 iRet;

  pthread_mutexattr_init(&mta);
  if( (iRet = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE)) != 0 )
    return NULL;

  mutex = malloc( sizeof(pthread_mutex_t) );
  if( mutex == NULL )
    return NULL;

  if( (iRet = pthread_mutex_init(mutex, &mta)) != 0 )
    goto err_out;

  return mutex;

err_out:
  free(mutex);
  return NULL;
}

/*****************************************************************************/
/*! Acquire a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void EnterLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;
  int             iRet;

  if( (iRet = pthread_mutex_lock(mutex)) != 0)
  {
    fprintf( stderr, "Locking failed: %s\n", strerror(iRet));
  }
}

/*****************************************************************************/
/*! Release a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void LeaveLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;
  int             iRet;

  if( (iRet = pthread_mutex_unlock(mutex)) != 0)
  {
    fprintf( stderr, "Unlock failed: %s\n", strerror(iRet));
  }
}

/*****************************************************************************/
/*! Delete a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void DeleteLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;

  pthread_mutex_destroy(mutex);
  free(mutex);
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
/*! Displays a hex dump on the debug console (16 bytes per line)
*   \param pbData     Pointer to dump data
*   \param ulDataLen  Length of data dump                                    */
/*****************************************************************************/
void DumpData(unsigned char* pbData, unsigned long ulDataLen)
{
  unsigned long ulIdx;

  for(ulIdx = 0; ulIdx < ulDataLen; ++ulIdx)
  {
    if(0 == (ulIdx % 16))
      printf("\r\n");

    printf("%02X ", pbData[ulIdx]);
  }
  printf("\r\n");
}

/*****************************************************************************/
/*! Dumps a rcX packet to debug console
*   \param ptPacket Pointer to packed being dumped                           */
/*****************************************************************************/
void DumpPacket(CIFX_PACKET* ptPacket)
{
  printf("Dest   : 0x%08lX      ID   : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDest),  (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulId));
  printf("Src    : 0x%08lX      Sta  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrc),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulState));
  printf("DestID : 0x%08lX      Cmd  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDestId),(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulCmd));
  printf("SrcID  : 0x%08lX      Ext  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrcId), (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulExt));
  printf("Len    : 0x%08lX      Rout : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulLen),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulRout));

  printf("Data:");
  DumpData(ptPacket->abData, HOST_TO_LE32(ptPacket->tHeader.ulLen));
}


/*****************************************************************************/
/*! Function to display driver information
 *   \param  hDriver  Handle to cifX driver
 *   \param  ptVTable Pointer to cifX API function table
 *   \return CIFX_NO_ERROR on success                                        */
/*****************************************************************************/
void DisplayDriverInformation (void)
{
  int32_t            lRet             = CIFX_NO_ERROR;
  DRIVER_INFORMATION tDriverInfo      = {{0}};
  char               szDrvVersion[32] = "";
  CIFXHANDLE         hDriver          = NULL;

  if (CIFX_NO_ERROR == (lRet = xDriverOpen(&hDriver)))
  {
    printf("\n---------- Display Driver Version ----------\n");
    if( CIFX_NO_ERROR != (lRet = xDriverGetInformation(NULL, sizeof(tDriverInfo), &tDriverInfo)) )
      ShowError( lRet);
    else if ( CIFX_NO_ERROR != (lRet = cifXGetDriverVersion( sizeof(szDrvVersion)/sizeof(*szDrvVersion), szDrvVersion)))
      ShowError( lRet);
    else
      printf("Driver Version: %s, based on %.32s \n\n", szDrvVersion, tDriverInfo.abDriverVersion);

    /* close previously opened driver */
    xDriverClose(hDriver);

  }

  printf(" State = 0x%08X\r\n", (unsigned int)lRet);
  printf("----------------------------------------------------\r\n");
}


/*****************************************************************************/
/*! Function to demonstrate the board/channel enumeration
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t EnumBoardDemo( void)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("---------- Board/Channel enumeration demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    unsigned long     ulBoard    = 0;
    BOARD_INFORMATION tBoardInfo = {0};

    /* Iterate over all boards */
    while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
    {
      printf("Found Board %s\r\n", tBoardInfo.abBoardName);
      if(strlen( (char*)tBoardInfo.abBoardAlias) != 0)
        printf(" Alias        : %s\r\n", tBoardInfo.abBoardAlias);

      printf(" DeviceNumber : %lu\r\n",(long unsigned int)tBoardInfo.tSystemInfo.ulDeviceNumber);
      printf(" SerialNumber : %lu\r\n",(long unsigned int)tBoardInfo.tSystemInfo.ulSerialNumber);
      printf(" Board ID     : %lu\r\n",(long unsigned int)tBoardInfo.ulBoardID);
      printf(" System Error : 0x%08lX\r\n",(long unsigned int)tBoardInfo.ulSystemError);
      printf(" Channels     : %lu\r\n",(long unsigned int)tBoardInfo.ulChannelCnt);
      printf(" DPM Size     : %lu\r\n",(long unsigned int)tBoardInfo.ulDpmTotalSize);

      unsigned long       ulChannel    = 0;
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      /* iterate over all channels on the current board */
      while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
      {
        printf(" - Channel %lu:\r\n", ulChannel);
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

        printf("  Device Nr. : %lu\r\n",(long unsigned int)tChannelInfo.ulDeviceNumber);
        printf("  Serial Nr. : %lu\r\n",(long unsigned int)tChannelInfo.ulSerialNumber);
        printf("  netX Flags : 0x%08X\r\n", tChannelInfo.ulNetxFlags);
        printf("  Host Flags : 0x%08X\r\n", tChannelInfo.ulHostFlags);
        printf("  Host COS   : 0x%08X\r\n", tChannelInfo.ulHostCOSFlags);
        printf("  Device COS : 0x%08X\r\n", tChannelInfo.ulDeviceCOSFlags);

        ++ulChannel;
      }

      ++ulBoard;
    }

    /* close previously opened driver */
    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (unsigned int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate system channel functionality (PacketTransfer)
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t SysdeviceDemo( char* szDevice)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("---------- System Device handling demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hSys = NULL;
    lRet = xSysdeviceOpen(hDriver, szDevice, &hSys);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening SystemDevice!\r\n");

    } else
    {
      SYSTEM_CHANNEL_SYSTEM_INFO_BLOCK    tSysInfoBlock = {{0}};
      SYSTEM_CHANNEL_SYSTEM_INFORMATION   tSysInfo      = {0};
      SYSTEM_CHANNEL_SYSTEM_CONTROL_BLOCK tControlBlock = {0};
      SYSTEM_CHANNEL_SYSTEM_STATUS_BLOCK  tStatusBlock  = {0};

      /* System channel successfully opened, try to read the System Info Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_INFO_BLOCK, sizeof(tSysInfoBlock), &tSysInfoBlock)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("System Channel Info Block:\r\n");
        printf("==========================\r\n");
        printf("DPM Cookie       : %.4s\r\n",(char*)tSysInfoBlock.abCookie);
        printf("DPM Size         : %lu\r\n",(long unsigned int)tSysInfoBlock.ulDpmTotalSize);
        printf("Device Number    : %lu\r\n",(long unsigned int)tSysInfoBlock.ulDeviceNumber);
        printf("Serial Number    : %lu\r\n",(long unsigned int)tSysInfoBlock.ulSerialNumber);
        printf("HW Options       : 0x%04X 0x%04X 0x%04X 0x%04X\r\n",
               tSysInfoBlock.ausHwOptions[0], tSysInfoBlock.ausHwOptions[1],
               tSysInfoBlock.ausHwOptions[2], tSysInfoBlock.ausHwOptions[3]);
        printf("Manufacturer     : %u\r\n", tSysInfoBlock.usManufacturer);
        printf("Production Date  : %u\r\n", tSysInfoBlock.usProductionDate);
        printf("Device Class     : %u\r\n", tSysInfoBlock.usDeviceClass);
        printf("HW Revision      : %u\r\n", tSysInfoBlock.bHwRevision);
        printf("HW Compatibility : %u\r\n", tSysInfoBlock.bHwCompatibility);

        printf("License Flags 1  : 0x%08X\r\n", tSysInfoBlock.ulLicenseFlags1);
        printf("License Flags 2  : 0x%08X\r\n", tSysInfoBlock.ulLicenseFlags2);
        printf("LicenseID        : 0x%04X\r\n", tSysInfoBlock.usNetxLicenseID);
        printf("LicenseFlags     : 0x%04X\r\n", tSysInfoBlock.usNetxLicenseFlags);
        printf("==========================\r\n");
      }

      /* Try to read the System Information */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_INFORMATION, sizeof(tSysInfo), &tSysInfo)))
      {
        printf("Error querying system information\r\n");
      } else
      {
        printf("System Information:\r\n");
        printf("===================\r\n");
        printf("System Error     : 0x%08X\r\n", tSysInfo.ulSystemError);
        printf("DPM Size         : %lu\r\n",    (long unsigned int)tSysInfo.ulDpmTotalSize);
        printf("Mailbox size     : %lu\r\n",    (long unsigned int)tSysInfo.ulMBXSize);
        printf("Device Number    : %lu\r\n",    (long unsigned int)tSysInfo.ulDeviceNumber);
        printf("Serial Number    : %lu\r\n",    (long unsigned int)tSysInfo.ulSerialNumber);
        printf("Open Count       : %lu\r\n",    (long unsigned int)tSysInfo.ulOpenCnt);
        printf("===================\r\n");
      }

      /* Try to read the System Control Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_CONTROL_BLOCK, sizeof(tControlBlock), &tControlBlock)))
      {
        printf("Error querying system control block\r\n");
      } else
      {
        printf("System Control Block:\r\n");
        printf("=====================\r\n");
        printf("Command COS      : 0x%08X\r\n", tControlBlock.ulSystemCommandCOS);
        printf("System Control   : 0x%08X\r\n", tControlBlock.ulSystemControl);
        printf("=====================\r\n");
      }

      printf("Waiting 2s to let cifX card calculate CPU load!\r\n");
      sleep(2);

      /* Try to read the System Status Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_STATUS_BLOCK, sizeof(tStatusBlock), &tStatusBlock)))
      {
        printf("Error querying system status block\r\n");
      } else
      {
        printf("System Status Block:\r\n");
        printf("====================\r\n");
        printf("System COS       : 0x%08X\r\n", tStatusBlock.ulSystemCOS);
        printf("System Status    : 0x%08X\r\n", tStatusBlock.ulSystemStatus);
        printf("System Error     : 0x%08X\r\n", tStatusBlock.ulSystemError);
        printf("Time since start : %lu\r\n",    (long unsigned int)tStatusBlock.ulTimeSinceStart);
        printf("CPU Load [%%]     : %.2f\r\n",  (float)tStatusBlock.usCpuLoad / 100);
        printf("====================\r\n");
      }

      unsigned long ulSendPktCount = 0;
      unsigned long ulRecvPktCount = 0;

      printf("\r\n");
      printf("Trying to read Security Eeprom:\r\n");
      printf("===============================\r\n");


      /* Read Security EEPROM zone 1*/
      xSysdeviceGetMBXState(hSys, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
      printf("System Mailbox State: MaxSend = %lu, Pending Receive = %lu\r\n",
             ulSendPktCount, ulRecvPktCount);

      HIL_SECURITY_EEPROM_READ_REQ_T tCryptoRead    = {{0}};
      HIL_SECURITY_EEPROM_READ_CNF_T tCryptoReadCnf = {{0}};

      tCryptoRead.tHead.ulDest   = HOST_TO_LE32(HIL_PACKET_DEST_SYSTEM);
      tCryptoRead.tHead.ulLen    = HOST_TO_LE32(sizeof(tCryptoRead.tData));
      tCryptoRead.tHead.ulCmd    = HOST_TO_LE32(HIL_SECURITY_EEPROM_READ_REQ);
      tCryptoRead.tData.ulZoneId = HOST_TO_LE32(1);

      if(CIFX_NO_ERROR != (lRet = xSysdevicePutPacket(hSys, (CIFX_PACKET*)&tCryptoRead, 10)))
      {
        printf("Error sending packet to device!\r\n");
      } else
      {
        printf("Send Packet (Read Crypto Flash Zone 1):\r\n");
        DumpPacket((CIFX_PACKET*)&tCryptoRead);

        xSysdeviceGetMBXState(hSys, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
        printf("System Mailbox State: MaxSend = %lu, Pending Receive = %lu\r\n",
              ulSendPktCount, ulRecvPktCount);

        if(CIFX_NO_ERROR != (lRet = xSysdeviceGetPacket(hSys, sizeof(tCryptoReadCnf), (CIFX_PACKET*)&tCryptoReadCnf, 20)) )
        {
          printf("Error getting packet from device! (lRet=0x%08X)\r\n",(unsigned int)lRet);
        } else
        {
          printf("Received Packet (Read Crypto Flash Zone 1):\r\n");
          DumpPacket((CIFX_PACKET*)&tCryptoReadCnf);

          xSysdeviceGetMBXState(hSys, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
          printf("System Mailbox State: MaxSend = %lu, Pending Receive = %lu\r\n",
                ulSendPktCount, ulRecvPktCount);
        }
      }

      printf("===============================\r\n");
      printf("\r\n");

      xSysdeviceClose(hSys);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (unsigned int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t ChannelDemo( char* szDevice)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("---------- Communication Channel demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hChannel = NULL;
    lRet = xChannelOpen(NULL, szDevice, 0, &hChannel);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening Channel!");

    } else
    {
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      /* Channel successfully opened, so query basic information */
      if( CIFX_NO_ERROR != (lRet = xChannelInfo(hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("Communication Channel Info:\r\n");
        printf("Device Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulDeviceNumber);
        printf("Serial Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulSerialNumber);
        printf("Firmware         : %s\r\n", tChannelInfo.abFWName);
        printf("FW Version       : %u.%u.%u build %u\r\n",
                tChannelInfo.usFWMajor,
                tChannelInfo.usFWMinor,
                tChannelInfo.usFWRevision,
                tChannelInfo.usFWBuild);
        printf("FW Date          : %02u/%02u/%04u\r\n",
                tChannelInfo.bFWMonth,
                tChannelInfo.bFWDay,
                tChannelInfo.usFWYear);

        printf("Mailbox Size     : %lu\r\n",(long unsigned int)tChannelInfo.ulMailboxSize);
      }

      CIFX_PACKET tSendPkt = {{0}};
      CIFX_PACKET tRecvPkt = {{0}};

      printf("\nStart put/get packet Demo!\n");

      /* Do a basic Packet Transfer */
      if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(hChannel, &tSendPkt, 10)))
      {
        printf("Error sending packet to device!\r\n");
      } else
      {
        printf("Send Packet:\r\n");
        DumpPacket(&tSendPkt);

        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(hChannel, sizeof(tRecvPkt), &tRecvPkt, 20)) )
        {
          printf("Error getting packet from device!\r\n");
        } else
        {
          printf("Received Packet:\r\n");
          DumpPacket(&tRecvPkt);
        }
      }

      sleep(1);

      printf("\nStart read/write IO-Data!\n");

      /* Read and write I/O data (32Bytes). Output data will be incremented each cyle */
      unsigned char abSendData[32] = {0};
      unsigned char abRecvData[32] = {0};
      unsigned long ulCycles       = 0;
      unsigned long ulState;

      if(CIFX_NO_ERROR != (lRet = xChannelBusState(hChannel, CIFX_BUS_STATE_ON,(uint32_t*) &ulState, 10000)))
      {
        printf("Error setting Bus state lRet = 0x%08X!\r\n",(unsigned int)lRet);

         xChannelClose(hChannel);
         xDriverClose(hDriver);
         return lRet;
      }

      printf("IO Demo running <Hit any key to abort>:\n");

      while(!kbhit())
      {
        ++ulCycles;

        usleep(1 * 1000); /* Wait 1 ms so we can see the counter on the LEDs */
        if(CIFX_NO_ERROR != (lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
        {
          printf("Error reading IO Data area!\r\n");
          break;
        } else
        {
          printf("IORead Data:");
          DumpData(abRecvData, sizeof(abRecvData));
          memcpy(abSendData, abRecvData, sizeof(abRecvData));

          /* On a CB-AB32 we will echo the input 8 buttons if one is pressed, otherwise a counter
             will be placed on output */
          if(abRecvData[0] == 0)
            abSendData[0] = (unsigned char)ulCycles;

          if(CIFX_NO_ERROR != (lRet = xChannelIOWrite(hChannel, 0, 0, sizeof(abSendData), abSendData, 10)))
          {
            printf("Error writing to IO Data area!\r\n");
            break;
          } else
          {
            printf("IOWrite Data:");
            DumpData(abSendData, sizeof(abSendData));
          }
        }
      }
      printf("IODemo ended. Total cycles %lu\n", ulCycles);

      if(CIFX_NO_ERROR != (lRet = xChannelBusState(hChannel, CIFX_BUS_STATE_OFF, (uint32_t*)&ulState, 10000)))
      {
        printf("Error setting Bus state lRet = 0x%08X!\r\n",(unsigned int)lRet);
      }


      xChannelClose(hChannel);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (unsigned int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;

}

/*****************************************************************************/
/*! Function to demonstrate control/status block functionality
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t BlockDemo( char* szDevice)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("\n--- Read / Write Block Information ---\r\n");

  /* Open channel */
  if(CIFX_NO_ERROR == lRet)
  {
    CIFXHANDLE hDevice = NULL;
    lRet = xChannelOpen(NULL, szDevice, 0, &hDevice);
    if(lRet != CIFX_NO_ERROR)
    {
      printf("Error opening Channel!\r\n");
    } else
    {
      unsigned char abBuffer[4] = {0};

      /* Read / Write control block */
      printf("Read CONTROL Block \r\n");
      memset( abBuffer, 0, sizeof(abBuffer));
      lRet = xChannelControlBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

      DumpData(abBuffer, 4);

      printf("Write CONTROL Block \r\n");
      lRet = xChannelControlBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

      printf("Read COMMON Status Block \r\n");
      memset( abBuffer, 0, sizeof(abBuffer));
      lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

      DumpData(abBuffer, 4);

      printf("Write COMMON Status Block \r\n");
      lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

      /* this is expected to fail, as this block must not be written by Host */
      if(CIFX_NO_ERROR != lRet)
        printf("Error writing to common status block. lRet = 0x%08X\r\n", (unsigned int)lRet);

      printf("Read EXTENDED Status Block \r\n");
      memset( abBuffer, 0, sizeof(abBuffer));
      lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
      DumpData(abBuffer, 4);

      printf("Write EXTENDED Status Block \r\n");
      lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

      /* this is expected to fail, as this block must not be written by Host */
      if(CIFX_NO_ERROR != lRet)
        printf("Error writing to extended status block. lRet = 0x%08X\r\n", (unsigned int)lRet);

      xChannelClose(hDevice);
    }
    xDriverClose(hDriver);
  }
  return lRet;
}


/*****************************************************************************/
/*! Function to demonstrate bus / host state
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t StateDemo (char* szDevice)
{
  CIFXHANDLE    hDriver  = NULL;
  CIFXHANDLE    hChannel = NULL;
  int32_t       lRet     = xDriverOpen(&hDriver);

  printf("\n--- Set Bus / Host State Demo ---\n");

  if (CIFX_NO_ERROR == lRet)
  {
    /* Open channel */
    if( CIFX_NO_ERROR != (lRet = xChannelOpen(hDriver, szDevice, 0, &hChannel)))
    {
      printf("Error opening Channel!\n");

    } else
    {
      uint32_t ulState  = 0;

      /* Set Host ready */
      lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_HOST_STATE_READY, state %lu, error 0x%08X !\n", (long unsigned int)ulState, (unsigned int)lRet);
      else
        printf("Set CIFX_HOST_STATE_READY!\n");

      /* Get Host state */
      lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READ, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_HOST_STATE_READ, state %lu, error 0x%08X !\n", (long unsigned int)ulState, (unsigned int)lRet);
      else
        printf("CIFX_HOST_STATE_READ, state %lu !\n", (long unsigned int)ulState);

      /* Set Bus state on */
      lRet = xChannelBusState  (hChannel, CIFX_BUS_STATE_ON, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_BUS_STATE_ON, state %lu, error 0x%08X !\n", (long unsigned int)ulState, (unsigned int)lRet);
      else
        printf("CIFX_BUS_STATE_ON, state %lul !\n", (long unsigned int)ulState);

      /* Set Bus state off */
      lRet = xChannelBusState  (hChannel, CIFX_BUS_STATE_OFF, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_BUS_STATE_OFF, state %lu, error 0x%08X !\n", (long unsigned int)ulState, (unsigned int)lRet);
      else
        printf("CIFX_BUS_STATE_OFF, state %lu !\n", (long unsigned int)ulState);

      /* Set Host not ready */
      lRet = xChannelHostState(hChannel, CIFX_HOST_STATE_NOT_READY, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_HOST_STATE_NOT_READY, state %lu, error 0x%08X !\n", (long unsigned int)ulState,(unsigned int)lRet);
      else
        printf("Set CIFX_HOST_STATE_NOT_READY!\n");

      /* Get Host state */
      lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READ, &ulState, 1000);
      if (CIFX_NO_ERROR != lRet)
        printf("Error CIFX_HOST_STATE_READ, state %lu, error 0x%08X !\n", (long unsigned int)ulState, (unsigned int)lRet);
      else
        printf("CIFX_HOST_STATE_READ, state %lu !\n", (long unsigned int)ulState);

      xChannelClose(hChannel);
    }
    xDriverClose(hDriver);
  }
  printf(" State = 0x%08X\n", (unsigned int)lRet);
  printf("----------------------------------------------------\n");

  return lRet;
}
