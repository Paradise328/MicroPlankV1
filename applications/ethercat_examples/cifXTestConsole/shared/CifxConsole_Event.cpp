/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_Event.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    Event example implementation

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2014-08-26  Example reworked, callbacks splitted into separate
                functions, checked handshake and SYNC state settings
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

#include "Hil_DualPortMemory.h" /* Event mode definitions */

void* s_pvLock;


/*============================================= */
/* Locale definitions for notification handling */
/*============================================= */
/* COM state handling */
typedef struct COM_STATE_STRUCTtag
{
  CIFXHANDLE  hChannel;
  uint32_t    ulCOMState;
} COM_STATE_STRUCT;

COM_STATE_STRUCT tCOMState = {0};

/* Packet data handling */
typedef struct PACKET_DATA_STRUCTtag
{
  CIFXHANDLE    hChannel;
  bool          fSendMBXEmpty;
  bool          fRecvMBXFull;
  bool          fSendActive;
  CIFX_PACKET   tSendPkt;
  int32_t       lSendRet;
  CIFX_PACKET   tRecvPkt;
  int32_t       lRecvRet;
} PACKET_DATA_STRUCT;

PACKET_DATA_STRUCT tPacketData = {0};

/* I/O data handling */
typedef struct IO_DATA_STRUCTtag
{
  CIFXHANDLE  hChannel;
  uint8_t     bPDInHskMode;
  uint8_t     bPDInSource;
  bool        fReadDone;
  uint8_t     abReadBuffer[10];
  int32_t     lReadRet;
  uint8_t     bPDOutHskMode;
  uint8_t     bPDOutSource;
  bool        fWriteDone;
  uint8_t     abWriteBuffer[10];
  int32_t     lWriteRet;
} IO_DATA_STRUCT;
IO_DATA_STRUCT tIOData = {0};

/* SYNC data handling */
typedef struct SYNC_DATA_STRUCTtag
{
  CIFXHANDLE    hChannel;
  uint8_t       bSyncSource;
  uint8_t       bSyncHskMode;
  int32_t       lSyncRet;
  uint32_t      ulErrorCount;
  bool          fDeviceAck;
} SYNC_DATA_STRUCT;
SYNC_DATA_STRUCT tSyncData = {0};

/*****************************************************************************/
/*! HANDLE COM state
*
*   \param  ptCOMState  Pointer to ComState Structure
*   \return true if COM flag is set                                          */
/*****************************************************************************/
bool HandleCOMState( COM_STATE_STRUCT* ptCOMState)
{
  if( ptCOMState->ulCOMState)
    return true; /* COM flag is set */

  return false;  /* COM flag is not set */
}

/*****************************************************************************/
/*! HANDLE Packet
*
*   \param  ptPacketData  Pointer to Packet Data Structure                   */
/*****************************************************************************/
void HandlePacket( PACKET_DATA_STRUCT* ptPacketData)
{
  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  /* Check if we have a receive packet */
  if ( ptPacketData->fRecvMBXFull)
  {
    ptPacketData->fRecvMBXFull = false;

    printf("Handle_Packet: xChannelGetPacket(), read a packet \n");
    ptPacketData->lRecvRet = xChannelGetPacket( ptPacketData->hChannel, sizeof(ptPacketData->tRecvPkt), &ptPacketData->tRecvPkt, 0);
    if (CIFX_NO_ERROR !=  ptPacketData->lRecvRet)
      /* Get packet error */
      printf("Handle_Packet: xChannelGetPacket(): Error: 0x%8x\n",  ptPacketData->lRecvRet);
    else
       ptPacketData->fSendActive = false;
  }

  /* Check if we can send something */
  if ( (false  == ptPacketData->fSendActive) &&
       (true   == ptPacketData->fSendMBXEmpty) )
  {
    /*------------------------------ */
    /* Create Demo Packet */
    /*------------------------------ */
    ptPacketData->tSendPkt.tHeader.ulDest  = 0;
    ptPacketData->tSendPkt.tHeader.ulSrc   = 0;
    ptPacketData->tSendPkt.tHeader.ulDestId= 0;
    ptPacketData->tSendPkt.tHeader.ulSrcId = 0;
    ptPacketData->tSendPkt.tHeader.ulLen   = 0;
    ptPacketData->tSendPkt.tHeader.ulId    = 0;
    ptPacketData->tSendPkt.tHeader.ulState = 0;
    ptPacketData->tSendPkt.tHeader.ulCmd   = 0;
    ptPacketData->tSendPkt.tHeader.ulExt   = 0;
    ptPacketData->tSendPkt.tHeader.ulRout  = 0;

    /* Send demo packet */
    printf("Handle_Packet: xChannelPutPacket(), write a packet \n");
    ptPacketData->lSendRet = xChannelPutPacket( ptPacketData->hChannel, &ptPacketData->tSendPkt, 0);

    if( CIFX_NO_ERROR != ptPacketData->lSendRet)
    {
      /* Put packet error */
      printf("Handle_Packet: xChannelPutPacket(): Error: 0x%8x\n", ptPacketData->lSendRet);
    } else
    {
      ptPacketData->fSendActive = true;
    }
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! HANDLE IO data
*
*   \param  ptIOData  Pointer to I/O Data Structure                          */
/*****************************************************************************/
void HandleIOData(IO_DATA_STRUCT* ptIOData)
{
  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  /*--------------------- */
  /* Handle INPUT data */
  /*--------------------- */
  switch ( ptIOData->bPDInHskMode)
  {
    case HIL_IO_MODE_UNCONTROLLED:
      /* Just read/write, there is no synchronisation with the hardware */
      printf("Handle_IOData: xChannelIORead()  - read INPUT data, HIL_IO_MODE_UNCONTROLLED\n");
      ptIOData->lReadRet = xChannelIORead(ptIOData->hChannel, 0, 0, 10,  &ptIOData->abReadBuffer[0], 0);
    break;

    case HIL_IO_MODE_BUFF_DEV_CTRL:
      if (ptIOData->fReadDone)
      {
        /* TEST: increment output */
        ptIOData->abWriteBuffer[1] = ptIOData->abReadBuffer[1];
      }
    break;

    case HIL_IO_MODE_BUFF_HST_CTRL:
    case HIL_IO_MODE_DEFAULT:
    default:
      if (ptIOData->fReadDone)
      {
        printf("Handle_IOData: xChannelIORead()  - read INPUT data, HIL_IO_MODE_BUFF_HST_CTRL\n");
        ptIOData->lReadRet = xChannelIORead(ptIOData->hChannel, 0, 0, 10, &ptIOData->abReadBuffer[0], 0);
      }
    break;
  }

  /*---------------------*/
  /* Handle OUTPUT data  */
  /*---------------------*/
  switch ( ptIOData->bPDOutHskMode)
  {
    case HIL_IO_MODE_UNCONTROLLED:
      /* Just read/write, there is no synchronisation with the hardware */
      printf("Handle_IOData: xChannelIOWrite() - write OUTPUT data, HIL_IO_MODE_UNCONTROLLED\n");
      ptIOData->lWriteRet = xChannelIOWrite(ptIOData->hChannel, 0, 0, 10, &ptIOData->abWriteBuffer[0], 0);

    break;

    case HIL_IO_MODE_BUFF_DEV_CTRL:
      if (ptIOData->fWriteDone)
      {
        /* TEST: increment output */
        ptIOData->abWriteBuffer[0] += 1;
      }
    break;

    case HIL_IO_MODE_BUFF_HST_CTRL:
    case HIL_IO_MODE_DEFAULT:
    default:
      if (ptIOData->fWriteDone)
      {
        printf("Handle_IOData: xChannelIOWrite() - write OUTPUT data,  HIL_IO_MODE_BUFF_HST_CTRL\n");

        /* TEST: increment output */
        ptIOData->abWriteBuffer[0] += 1;

        ptIOData->lWriteRet = xChannelIOWrite(ptIOData->hChannel, 0, 0, 10, &ptIOData->abWriteBuffer[0], 0);
      }
    break;
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! HANDLE Sync
*
*   \param  ptSyncData  Pointer to Sync Data Structure                       */
/*****************************************************************************/
void HandleSync( SYNC_DATA_STRUCT* ptSyncData)
{
  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  if( HIL_SYNC_MODE_OFF != ptSyncData->bSyncSource)
  {
    if( HIL_SYNC_MODE_HST_CTRL == ptSyncData->bSyncHskMode)
    {
      /* We have to send a CIFX_SYNC_SIGNAL_CMD */
      ptSyncData->lSyncRet = xChannelSyncState( ptSyncData->hChannel, CIFX_SYNC_SIGNAL_CMD, 0, &tSyncData.ulErrorCount);
      if(CIFX_NO_ERROR != ptSyncData->lSyncRet)
      {
        printf("Handle_Sync__: Error signaling the SYNC_CMD, lRet = 0x%08X\n", ptSyncData->lSyncRet);
      }
    }
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! Mailbox state callback
*
*  \param ulNotification  Notification type
*  \param ulDataLen       Unused
*  \param pvData          Unused
*  \param pvUser          User parameter                                     */
/*****************************************************************************/
void APIENTRY CLBK_MBXStateFnc (uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
  UNREFERENCED_PARAMETER( pvData);
  UNREFERENCED_PARAMETER( ulDataLen);

  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  if (NULL == pvUser)
  {
    printf("CLBK_MBXState: FAILED, pvUser pointer is NULL\n");
  } else
  {
    PACKET_DATA_STRUCT* ptPacketData = (PACKET_DATA_STRUCT*)pvUser;

    switch (ulNotification)
    {
      case CIFX_NOTIFY_RX_MBX_FULL:
        ptPacketData->fRecvMBXFull = true;
        printf("CLBK_MBXState: CIFX_NOTIFY_RX_MBX_FULL\n");
      break;

      case CIFX_NOTIFY_TX_MBX_EMPTY:
        ptPacketData->fSendMBXEmpty = true;
        printf("CLBK_MBXState: CIFX_NOTIFY_TX_MBX_EMPTY\n");
      break;

      default:
        printf("CLBK_MBXState: UNKNOWN Event, Event number %u\n", ulNotification);
      break;
    }
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! COM state callback
*
*  \param ulNotification  Notification type
*  \param ulDataLen       Unused
*  \param pvData          Unused
*  \param pvUser          Unused                                             */
/*****************************************************************************/
void APIENTRY CLBK_COMStateFnc (uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
  UNREFERENCED_PARAMETER( pvUser);
  UNREFERENCED_PARAMETER( pvData);
  UNREFERENCED_PARAMETER( ulDataLen);

  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  if ( (NULL == pvUser) ||
       (NULL == pvData) )
  {
    printf("CLBK_COMState: pvUser pointer is NULL\n");
  } else
  {
    COM_STATE_STRUCT*         ptComState           = (COM_STATE_STRUCT*)pvUser;
    CIFX_NOTIFY_COM_STATE_T*  ptCOMStateNotifyData = (CIFX_NOTIFY_COM_STATE_T*)pvData;

    switch (ulNotification)
    {
      case CIFX_NOTIFY_COM_STATE:
        ptComState->ulCOMState = ptCOMStateNotifyData->ulComState;
        printf("CLBK_COMState: CIFX_NOTIFY_COM_STATE, state = %u\n", ptComState->ulCOMState);
      break;

      default:
        printf("CLBK_COMState: UNKNOWN Event, Event number %u\n", ulNotification);
      break;
    }
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! IO data callback
*
*  \param ulNotification  Notification type
*  \param ulDataLen       Unused
*  \param pvData          Unused
*  \param pvUser          User parameter                                     */
/*****************************************************************************/
void APIENTRY CLBK_PDStateFnc (uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
  UNREFERENCED_PARAMETER( pvData);
  UNREFERENCED_PARAMETER( ulDataLen);

  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  if (NULL == pvUser)
  {
    printf("CLBK_PD_State: FAILED, pvUser pointer is NULL\n");
  } else
  {
    IO_DATA_STRUCT* ptIOData = (IO_DATA_STRUCT*)pvUser;

    switch (ulNotification)
    {
      case CIFX_NOTIFY_PD0_IN:
        switch (ptIOData->bPDInHskMode)
        {
          case HIL_IO_MODE_BUFF_HST_CTRL:
          case HIL_IO_MODE_DEFAULT:
            /* Buffered Host Controlled */
            ptIOData->fReadDone = true;
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_IN,  event read done\n");
          break;

          case HIL_IO_MODE_BUFF_DEV_CTRL:
            /* Buffered Device Controlled */
            ptIOData->lReadRet = xChannelIORead(ptIOData->hChannel, 0, 0, 10, &ptIOData->abReadBuffer[0], 0);
            ptIOData->fReadDone = true;
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_IN,  read INPUT done\n");
          break;

          default:
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_IN, UNKNOWN Event, number %u\n", ulNotification);
          break;
        }
      break;

      case CIFX_NOTIFY_PD0_OUT:
        switch (ptIOData->bPDOutHskMode)
        {
          case HIL_IO_MODE_BUFF_HST_CTRL:
          case HIL_IO_MODE_DEFAULT:
            /* Buffered Host Controlled */
            ptIOData->fWriteDone = true;
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_OUT, event write done\n");;
          break;

          case HIL_IO_MODE_BUFF_DEV_CTRL:
            /* Buffered Device Controlled */
            ptIOData->lWriteRet = xChannelIOWrite(ptIOData->hChannel, 0, 0, 10, &ptIOData->abWriteBuffer[0], 0);
            ptIOData->fWriteDone = true;
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_OUT, write OUTPUT done\n");;
          break;

          default:
            printf("CLBK_PD_State: CIFX_NOTIFY_PD0_OUT, UNKNOWN Event, Event number %u\n", ulNotification);
          break;
        }
      break;

      default:
        printf("CLBK_PD_State: UNKNOWN Event, number %u\n", ulNotification);
      break;
    } /* End notification */
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! IO data callback
*
*  \param ulNotification  Notification type (unused)
*  \param ulDataLen       Unused
*  \param pvData          Unused
*  \param pvUser          User parameter                                     */
/*****************************************************************************/
void APIENTRY CLBK_SyncStateFnc (uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
  UNREFERENCED_PARAMETER( pvData);
  UNREFERENCED_PARAMETER( ulDataLen);
  UNREFERENCED_PARAMETER( ulNotification);

  OS_EnterLock(s_pvLock);   /* Necessary for printf() */

  if (NULL == pvUser)
  {
    printf("CLBK_SYNCState: FAILED, pvUser pointer is NULL\n");
  } else
  {
    SYNC_DATA_STRUCT* ptSyncData = (SYNC_DATA_STRUCT*)pvUser;

    switch (ptSyncData->bSyncHskMode)
    {
      case HIL_SYNC_MODE_DEV_CTRL:

  /*    QueryPerformanceCounter( (LARGE_INTEGER*)&tPerfData[bPerfIdx].liCounterStart); */

        /* We have to send a CIFX_SYNC_ACKNOWLEDGE_CMD */
        printf("CLBK_SYCNState: Send CIFX_SYNC_ACKNOWLEDGE_CMD\n");
        ptSyncData->lSyncRet = xChannelSyncState( ptSyncData->hChannel, CIFX_SYNC_ACKNOWLEDGE_CMD, 0, &ptSyncData->ulErrorCount);
        if(CIFX_NO_ERROR != ptSyncData->lSyncRet)
        {
          printf("Error signaling the Sync, lRet = 0x%08x\n", ptSyncData->lSyncRet);
        }

        /* QueryPerformanceCounter( (LARGE_INTEGER*)&tPerfData[bPerfIdx].liCounterStop); */
        /* if(++bPerfIdx >= PERF_DATA_COUNT) */
        /*   bPerfIdx = 0; */
      break;

      case HIL_SYNC_MODE_HST_CTRL:
        /* This is a sync acknowledge event from the device */
        printf("CLBK_SYNCState: Received Device SYNC-ACK\n");
        ptSyncData->fDeviceAck = true;
      break;


      default:
        printf("CLBK_SYNCState: UNKNOWN Event, Event number %u\n", ulNotification);
      break;
    }
  }

  OS_LeaveLock(s_pvLock);
}

/*****************************************************************************/
/*! Initialize Event handling
*
*   \param  hChannel  Channel handle
*   \return false on error                                                   */
/*****************************************************************************/
bool InitializeEventHandling(CIFXHANDLE hChannel)
{
  int32_t                       lRet = CIFX_NO_ERROR;
  bool                          fRet = false;
  HIL_DPM_COMMON_STATUS_BLOCK_T tCommState = {0};

  printf("\r\n--- Initialize Event Handling ---\r\n");

  /*-----------------------------------------------------------------------------*/
  /* Read the COMMON_STATUS_BLOCK and check the configured synchronisation modes */
  /*-----------------------------------------------------------------------------*/
  if ( CIFX_NO_ERROR != (lRet = xChannelCommonStatusBlock( hChannel, CIFX_CMD_READ_DATA, 0, sizeof(tCommState), &tCommState)) )
  {
    /* Failed to read the common state information from the hardware */
    OS_EnterLock(s_pvLock);
    printf("InitializeEventHandling(): Failed to read the COMMON status block\n\r");
    ShowError( lRet);
    OS_LeaveLock(s_pvLock);
  } else
  {
    /*----------------------------------*/
    /* Display actual configuration     */
    /*----------------------------------*/
    OS_EnterLock(s_pvLock);

    printf("\n\r");
    printf("InitializeEventHandling(): Notification configuration\n\r");
    printf("COM           Event          = ON\n\r");
    printf("RX/TX Mailbox Event          = ON\n\r");
    printf("PD0_IN        Handshake Mode = 0x%.2x, Source = 0x%.2x\n\r", tCommState.bPDInHskMode,  tCommState.bPDInSource);
    printf("PD0_OUT       Handshake Mode = 0x%.2x, Source = 0x%.2x\n\r", tCommState.bPDOutHskMode, tCommState.bPDOutSource);
    printf("SYNC          Handshake Mode = 0x%.2x, Source = 0x%.2x\n\r", tCommState.bSyncHskMode,  tCommState.bSyncSource);
    printf("\n\r");

    OS_LeaveLock(s_pvLock);

    /*----------------------------------*/
    /* Register COM_STATE Notification  */
    /*----------------------------------*/
    tCOMState.hChannel    = hChannel;
    tCOMState.ulCOMState  = 0;
    if (CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_COM_STATE, CLBK_COMStateFnc,  &tCOMState)) )
    {
      /* Failed to register COM_STATE callbacks */
      /* Read driver error description */
      OS_EnterLock(s_pvLock);
      printf("InitializeEventHandling(): Failed to register CIFX_NOTIFY_COM_STATE\n\r");
      ShowError( lRet);
      OS_LeaveLock(s_pvLock);
    } else
    {
      fRet = true;
    }

    /*----------------------------------*/
    /* Register Packet data callback    */
    /*----------------------------------*/
    tPacketData.hChannel      = hChannel;
    tPacketData.fRecvMBXFull  = false;
    tPacketData.fSendMBXEmpty = true;
    tPacketData.fSendActive   = false;
    if( (CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_RX_MBX_FULL,   CLBK_MBXStateFnc, &tPacketData))) ||
        (CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_TX_MBX_EMPTY,  CLBK_MBXStateFnc, &tPacketData))) )
    {
      /* Failed to register mailbox callbacks */
      /* Read driver error description */
      OS_EnterLock(s_pvLock);
      printf("InitializeEventHandling(): Failed to register CIFX_NOTIFY_RX_MBX_FULL/CIFX_NOTIFY_TX_MBX_EMPTY\n\r");
      ShowError( lRet);
      OS_LeaveLock(s_pvLock);
    } else
    {
      fRet = true;
    }

    /*------------------------------------------*/
    /* Check INPUT Process data exchange mode   */
    /*------------------------------------------*/
    OS_Memset( &tIOData, 0, sizeof(tIOData));
    tIOData.hChannel = hChannel;
    tIOData.bPDInSource   = tCommState.bPDInSource;
    tIOData.bPDInHskMode  = tCommState.bPDInHskMode;
    tIOData.bPDOutSource  = tCommState.bPDOutSource;
    tIOData.bPDOutHskMode = tCommState.bPDOutHskMode;
    switch (tIOData.bPDInHskMode)
    {
      case HIL_IO_MODE_UNCONTROLLED:
        /* No Event handling */
      break;

      case HIL_IO_MODE_BUFF_DEV_CTRL:
      case HIL_IO_MODE_BUFF_HST_CTRL:
      case HIL_IO_MODE_DEFAULT:
      default:
        /* Store handshake mode and register event */
        tIOData.fReadDone     = false;
        if( CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_PD0_IN, CLBK_PDStateFnc, &tIOData)) )
        {
          /* Failed to register INPUT callbacks */
          /* Read driver error description */
          OS_EnterLock(s_pvLock);
          printf("InitializeEventHandling(): Failed to register CIFX_NOTIFY_PD0_IN\n\r");
          ShowError( lRet);
          OS_LeaveLock(s_pvLock);
        }  else
        {
          fRet = true;
        }
      break;
    }

    /*------------------------------------------*/
    /* Check OUTPUT Process data exchange mode  */
    /*------------------------------------------*/
    /* Use the same data structure like for input data,
     * because we only using one callback for both.
     */
    switch (tIOData.bPDOutHskMode)
    {
      case HIL_IO_MODE_UNCONTROLLED:
        /* No Event handling */
      break;

      case HIL_IO_MODE_BUFF_DEV_CTRL:
      case HIL_IO_MODE_BUFF_HST_CTRL:
      case HIL_IO_MODE_DEFAULT:
      default:
        /* Store handshake mode and register event */
        tIOData.fWriteDone = false;
        if( CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_PD0_OUT, CLBK_PDStateFnc, &tIOData)) )
        {
          /* Failed to register OUTPUT callbacks */
          /* Read driver error description */
          OS_EnterLock(s_pvLock);
          printf("InitializeEventHandling(): Failed to register CIFX_NOTIFY_PD0_OUT\n\r");
          ShowError( lRet);
          OS_LeaveLock(s_pvLock);
        } else
        {
          fRet = true;
        }
      break;
    }

    /*-------------------------------*/
    /* Check SYNC data exchange mode */
    /*-------------------------------*/
    OS_Memset( &tSyncData, 0, sizeof(tSyncData));

    tSyncData.hChannel      = hChannel;
    tSyncData.bSyncSource   = tCommState.bSyncSource;
    tSyncData.bSyncHskMode  = tCommState.bSyncHskMode;
    switch (tSyncData.bSyncHskMode)
    {
      case HIL_SYNC_MODE_HST_CTRL:
      case HIL_SYNC_MODE_DEV_CTRL:
        if( HIL_SYNC_SOURCE_OFF == tSyncData.bSyncSource)
        {
          /* No SYNC source specified */
          OS_EnterLock(s_pvLock);
          printf("InitializeEventHandling(): HIL_SYNC_SOURCE_OFF configured\n\r");
          OS_LeaveLock(s_pvLock);

        } else
        {
          if( CIFX_NO_ERROR != (lRet = xChannelRegisterNotification( hChannel, CIFX_NOTIFY_SYNC, CLBK_SyncStateFnc, &tSyncData)) )
          {
            /* Failed to register mailbox callbacks */
            /* Read driver error description */
            OS_EnterLock(s_pvLock);
            printf("InitializeEventHandling(): Failed to register CIFX_NOTIFY_SYNC\n\r");
            ShowError( lRet);
            OS_LeaveLock(s_pvLock);
          } else
          {
            fRet = true;
          }
        }
      break;

      case HIL_SYNC_MODE_OFF:
      default:
        /* No Sync defined */
        OS_EnterLock(s_pvLock);
        printf("InitializeEventHandling(): HIL_SYNC_MODE_OFF configured\n\r");
        OS_LeaveLock(s_pvLock);
      break;
    }
  }

  printf("\n\r");

  return fRet;
}

/*****************************************************************************/
/*! Uninitialize Event handling
*
*   \param  hChannel   Channel handle                                        */
/*****************************************************************************/
void UninitializeEventHandling(CIFXHANDLE hChannel)
{
  int32_t lRet = CIFX_NO_ERROR;

  printf("\r\n--- Uninitialize Event Handling ---\r\n");

  /*-------------------*/
  /* Unregister Events */
  /*-------------------*/
  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_RX_MBX_FULL);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_RX_MBX_FULL\n\r");
    ShowError(lRet);
  }

  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_TX_MBX_EMPTY);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_TX_MBX_EMPTY\n\r");
    ShowError(lRet);
  }

  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_PD0_IN);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_PD0_IN\n\r");
    ShowError(lRet);
  }

  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_PD0_OUT);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_PD0_OUT\n\r");
    ShowError(lRet);
  }

  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_SYNC);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_SYNC\n\r");
    ShowError(lRet);
  }

  lRet = xChannelUnregisterNotification( hChannel, CIFX_NOTIFY_COM_STATE);
  if( CIFX_NO_ERROR != lRet)
  {
    printf("xChannelUnregisterNotification() CIFX_NOTIFY_COM_STATE\n\r");
    ShowError(lRet);
  }

  printf("\n\r");
}

/*****************************************************************************/
/*! Event handling test
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name                                            */
/*****************************************************************************/
void TestEventHandling(CIFXHANDLE hDriver, char* szBoard)
{
  int32_t     lRet      = CIFX_NO_ERROR;
  CIFXHANDLE  hChannel  = NULL;

  printf("\n--- Test Event Handling ---\r\n");

  /* Create critical section for printf() handling */
  s_pvLock = OS_CreateLock();

  /* Open the communication channel */
  lRet = xChannelOpen(hDriver, szBoard, 0, &hChannel);
  if(lRet != CIFX_NO_ERROR)
  {
    /* Read driver error description */
    ShowError( lRet);
  } else
  {
    /* Set host ready */
    if( (lRet = xChannelHostState( hChannel, CIFX_HOST_STATE_READY, NULL, 2000L)) != CIFX_NO_ERROR)
    {
      /* Read driver error description */
      OS_EnterLock(s_pvLock);
      ShowError( lRet);
      OS_LeaveLock(s_pvLock);
    }

    /*---------------*/
    /* Wait for user */
    /*---------------*/
    OS_EnterLock(s_pvLock);

    printf("\n\r EVENT-Handling active! Press (Q) to stop... \r\n");

    OS_LeaveLock(s_pvLock);

    /* Initialize events */
    if (InitializeEventHandling( hChannel))
    {

      do
      {
        /* Check COM flags state */
        if (HandleCOMState(&tCOMState))
        {
          /* COM flag is set, at least ONE device is on the BUS */

          /* On MASTER devices check if all configured slaves are available
           * INPUT process data are valid for existing slaves
           */

        } else
        {
          /* COM flag is cleared, no device to exchange data available
           * INPUT data are NOT valid
           */
        }

        /* Handle packet data */
        HandlePacket(&tPacketData);

        /* Handle I/O data */
        HandleIOData(&tIOData);

        /* Handle SYNC data */
        HandleSync(&tSyncData);

        OS_Sleep(10);

      }while(!OS_KbHit());
    }

    /* Uninitialize events */
    UninitializeEventHandling( hChannel);

    /* Close channel */
    if( hChannel != NULL)
      xChannelClose(hChannel);
  }

  /* Delete critical section for printf() handling */
  OS_DeleteLock(s_pvLock);


  printf("\n Test done\r\n");
}
