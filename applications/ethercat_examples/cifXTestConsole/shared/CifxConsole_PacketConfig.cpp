/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_PacketConfig.cpp 12751 2019-03-01 10:42:25Z AlexanderMinor $:

  Description:
    Example for a PROFIBUS master packet configuration

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/

#include "OS_Includes.h"
#include "CifXConsole.h"

#include "Hil_ApplicationCmd.h" /* HIL_SET_WATCHDOG_TIME_REQ */

#pragma pack(1)
#include "TLR_Types.h" /* type definitions in Profibus stack headers */
#include "ProfibusFspmm_Public.h"
#pragma pack()

/*****************************************************************************/
/*! Test Packet configuration for a PROFIBUS Master
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name                                            */
/*****************************************************************************/
bool RunPacketConfiguration( CIFXHANDLE hDriver, char* szBoard)
{
  /* --- Download configuration --- */
  unsigned char abWatchDogTime[] = {0x20, 0x00, 0x00, 0x00,
                                    0x01, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x04, 0x00, 0x00, 0x00,
                                    0x0A, 0x00, 0x00, 0x00,
                                    0x11, 0x00, 0x00, 0x00,
                                    0x04, 0x2F, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    /* Data */
                                    0xF4, 0x01, 0x00, 0x00,
                                    };



  unsigned char ab1Packet[] = {0x20, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x24, 0x22, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};

#if 0
  /* Slave Configuration Packet: */
  unsigned char ab2Packet[] = {0x20, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x3A, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x1E, 0x22, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
     /*Data:*/
                               0xFF, 0x00, 0x00, 0x00,
                               0x0A, 0x00, 0x00, 0x00,
                               0x0A,
                               0x31, 0x00, 0xA8, 0x03, 0x4B, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x10, 0x00, 0x88, 0x01, 0x64, 0x0B, 0x09, 0x6B, 0x00, 0x80, 0x00, 0x00, 0x50, 0x04, 0x81, 0x00,
                               0x07, 0x00, 0xC2, 0x00, 0x00, 0xBF, 0x81, 0x08, 0x00, 0x01, 0x01, 0x00, 0x80, 0x00, 0x80, 0x02,
                               0x00};
#endif /* 0 */

  /* CBAB_32 Slave */
  unsigned char ab2Packet[] = { 0x20, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x35, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x1E, 0x22, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,

                                0xFF, 0x00, 0x00, 0x00,
                                0x02, 0x00, 0x00, 0x00,
                                0x02,
                                0x2c, 0x00, 0x80, 0x00, 0xf4, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x0e, 0x00, 0xf8, 0x14, 0x01, 0x0b, 0x75, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
                                0x21, 0x11, 0x08, 0x00, 0x01, 0x01, 0x00, 0x80, 0x00, 0x80, 0x02, 0x00};


#if 0
  /* Master Configuration Packet: */
  /* Header: */
  unsigned char ab3Packet[] = {0x20, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x67, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x22, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,

  /*Data:*/                   0x67, 0x00, 0x00, 0x06, 0x90, 0x01, 0x0B, 0x00,
                              0x96, 0x00, 0x00, 0x01, 0x61, 0x11, 0x00, 0x00, 0x0A, 0x7D, 0x02, 0x00, 0x64, 0x00, 0x0A, 0x00,
                              0x78, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif /* 0 */
  unsigned char ab3Packet[] = {  /* 1,5 MB */
                                0x20, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x68, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x22, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00,

  /* DATA */                    0x43, 0x00, 0x00, 0x06, 0x2c, 0x01, 0x0b, 0x00, 0x96, 0x00, 0x00, 0x01, 0xAB, 0x09, 0x00, 0x00,
                                0x0A, 0x05 /*HSA*/, 0x01, 0x00, 0x06,/* <-MinSlaveInterval */ 0x00, 0x0A, 0x00, 0x78, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
                                0x02, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34,
                                0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
                                0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30,
                                0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


  int32_t       lRet            = CIFX_NO_ERROR;
  CIFXHANDLE    hChannel        = NULL;
  uint32_t      ulState         = 0L;
  CIFX_PACKET*  ptSendPacket    = NULL;
  CIFX_PACKET   tRecvPacket     = {{0}};

  uint32_t      ulWDTrigger     = 0;

  uint8_t       abReadData[100]    = {0};
  uint8_t       abReadDataOld[100] = {0};

  /*------------------------------*/
  lRet = xChannelOpen ( hDriver,  szBoard, 0, &hChannel);

  if(lRet != CIFX_NO_ERROR)
  {
    /* Error opening a channel */
    ShowError( lRet);
  } else
  {

    printf("==> Start packet configuration\n");

    lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, &ulState, 0L);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("02 ");
      ShowError( lRet);
    }

    lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READ, &ulState, 0L);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("02 ");
      ShowError( lRet);
    }

    lRet = xChannelReset( hChannel, CIFX_CHANNELINIT, 5000L);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("04 ");
      ShowError( lRet);
    }

    OS_Sleep(1000);

    lRet = xChannelWatchdog( hChannel, CIFX_WATCHDOG_STOP, &ulWDTrigger);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("05 ");
      ShowError( lRet);
    }

    /* BUS ON */
    lRet = xChannelBusState( hChannel, CIFX_BUS_STATE_ON, &ulState, 500L);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("06 ");
      ShowError( lRet);
    }

    /*--------------------  Set Watchdog Time  ------------------------------*/
    {
      HIL_SET_WATCHDOG_TIME_REQ_T* ptWDReq = (HIL_SET_WATCHDOG_TIME_REQ_T*)&abWatchDogTime[0];

      lRet = xChannelPutPacket(hChannel, (CIFX_PACKET*)ptWDReq, 100L);
      if(lRet != CIFX_NO_ERROR)
      {
        /* Read driver error description */
        printf("07 ");
        ShowError( lRet);
      }

      lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 100L);
      if((lRet != CIFX_NO_ERROR) ||
        ((lRet = tRecvPacket.tHeader.ulState) != CIFX_NO_ERROR))
      {
        /* Read driver error description */
        printf("08 ");
        ShowError( lRet);
      }
    }

    /*--------------------  START Configuration ------------------------------*/
    /*
    uint32_t  ulSendPktCount  = 0;
    uint32_t  ulRecvPktCount  = 0;
    lRet = xChannelGetMBXState( hChannel, &ulRecvPktCount, &ulSendPktCount);
    if(lRet != CIFX_NO_ERROR)
    {
      // Read driver error description
      printf("06 ");
      ShowError( lRet);
    }else
    {
      while ( ulRecvPktCount > 0)
      {
         xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 100L);
        --ulRecvPktCount;
      }
    }
    */

    /* Download Configuration */
    ptSendPacket = (CIFX_PACKET*)&ab1Packet[0];
    lRet = xChannelPutPacket(hChannel, ptSendPacket, 1000);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("07 ");
      ShowError( lRet);
    }

    lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 1000);
    if((lRet != CIFX_NO_ERROR) ||
       ((lRet = tRecvPacket.tHeader.ulState) != CIFX_NO_ERROR))
    {
      /* Read driver error description */
      printf("08 ");
      ShowError( lRet);
    }

    /* Download Slave parameter */
    ptSendPacket = (CIFX_PACKET*)&ab2Packet[0];

    /*PROFIBUS_FSPMM_DOWNLOAD_REQ_T* ptPBSlaveParam = (PROFIBUS_FSPMM_DOWNLOAD_REQ_T*)&ptSendPacket->abData[0];*/

    lRet = xChannelPutPacket(hChannel, ptSendPacket, 1000);

    lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 1000);
    if((lRet != CIFX_NO_ERROR) ||
       ((lRet = tRecvPacket.tHeader.ulState) != CIFX_NO_ERROR))
    {
      /* Read driver error description */
      printf("09 ");
      ShowError( lRet);
    }

    /* Download BUS-Parameter */
    ptSendPacket = (CIFX_PACKET*)&ab3Packet[0];

    /*PROFIBUS_DL_BUS_PARAMETER_SET_T* ptPBBusParam = (PROFIBUS_DL_BUS_PARAMETER_SET_T*)&ptSendPacket->abData[0];*/

    lRet = xChannelPutPacket(hChannel, ptSendPacket, 1000);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("10 ");
      ShowError( lRet);
    }

    lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 10);
    if((lRet != CIFX_NO_ERROR) ||
       ((lRet = tRecvPacket.tHeader.ulState) != CIFX_NO_ERROR))
    {
      /* Read driver error description */
      printf("11 ");
      ShowError( lRet);
    }

    /*--------------------  END Configuration ------------------------------*/

    /*
    lRet = xChannelConfigLock( hChannel, CIFX_CONFIGURATION_LOCK, &ulState, 0L);
    if(lRet != CIFX_NO_ERROR)
    {
      // Read driver error description
      printf("12 ");
      ShowError( lRet);
    }

    lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, &ulState, 0L);
    if(lRet != CIFX_NO_ERROR)
    {
      // Read driver error description
      printf("13 ");
      ShowError( lRet);
    }
    */


    /* BUS ON */
    lRet = xChannelBusState( hChannel, CIFX_BUS_STATE_ON, &ulState, 5000L);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      printf("14 ");
      ShowError( lRet);
    }

    /*--------------------  Set Watchdog Time  ------------------------------*/
    {
      HIL_SET_WATCHDOG_TIME_REQ_T* ptWDReq = (HIL_SET_WATCHDOG_TIME_REQ_T*)&abWatchDogTime[0];

      lRet = xChannelPutPacket(hChannel, (CIFX_PACKET*)ptWDReq, 100L);
      if(lRet != CIFX_NO_ERROR)
      {
        /* Read driver error description */
        printf("07 ");
        ShowError( lRet);
      }

      lRet = xChannelGetPacket(hChannel, sizeof(tRecvPacket), &tRecvPacket, 100L);
      if((lRet != CIFX_NO_ERROR) ||
        ((lRet = tRecvPacket.tHeader.ulState) != CIFX_NO_ERROR))
      {
        /* Read driver error description */
        printf("08 ");
        ShowError( lRet);
      }
    }
    /*--------------------  Set Watchdog Time END ---------------------------*/

    do
    {
      lRet = xChannelIORead( hChannel, 0, 0, 10, &abReadData[0], 1L);
      if(lRet != CIFX_NO_ERROR)
      {
        /* Read IO data failed */
        printf("ReadWriteIOData(): Reading INPUT data failed, ");
        ShowError( lRet);
        break;
      }

      /* Write Output to all areas */
      if( (lRet = xChannelIOWrite( hChannel, 0, 0, 10, &abReadData[0], 1L)) != CIFX_NO_ERROR)
      {
        /* Error writing Output data */
        printf("ReadWriteIOData(): Writing OUTPUT data failed, ");
        ShowError( lRet);
      }

      if ( (abReadData[0] != abReadDataOld[0]) ||
           (abReadData[1] != abReadDataOld[1])   )
      {
        printf("IN  byte 0,1 = 0x%02X, 0x%02X\n", abReadData[0], abReadData[1]);
      }

      abReadDataOld[0] = abReadData[0];
      abReadDataOld[1] = abReadData[1];

    } while (!OS_KbHit());

    /* END */
    printf("==> New Configuration Cycle\n");
    printf("--- Test Done --------------------\n");

    /* Close channel */
    xChannelClose( hChannel);
  }

  return true;
}

/*****************************************************************************/
/*! Test packet based configuration
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name                                            */
/*****************************************************************************/
void TestPacketConfig( CIFXHANDLE hDriver, char* szBoard)
{
  printf("\n--- Test Packet Configuration ---\r\n");

  /* Test configuration download */
  RunPacketConfiguration( hDriver, szBoard);
}
