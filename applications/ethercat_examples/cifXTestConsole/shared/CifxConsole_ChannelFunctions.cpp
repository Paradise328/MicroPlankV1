/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_ChannelFunctions.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    CifX API Channel functions test

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

/*****************************************************************************/
/*! Event Callback
*
*  \param ulNotification
*  \param ulDataLen
*  \param pvData
*  \param pvUser                                                             */
/*****************************************************************************/
void APIENTRY EventCallback (uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
  UNREFERENCED_PARAMETER( pvUser);
  UNREFERENCED_PARAMETER( pvData);
  UNREFERENCED_PARAMETER( ulDataLen);

  switch (ulNotification)
  {
    case CIFX_NOTIFY_RX_MBX_FULL:
    break;

    case CIFX_NOTIFY_TX_MBX_EMPTY:
    break;

    case CIFX_NOTIFY_PD0_IN:
    break;

    case CIFX_NOTIFY_PD0_OUT:
    break;

    case CIFX_NOTIFY_PD1_IN:
    break;

    case CIFX_NOTIFY_PD1_OUT:
    break;

    default:
    break;
  }
}

/*****************************************************************************/
/*! Test Channel Functions
*
*   \param  hDriver  Driver handle
*   \param  szBoard  Board name                                              */
/*****************************************************************************/
void TestChannelFunctions( CIFXHANDLE hDriver, char* szBoard)
{
  int32_t lRet = CIFX_NO_ERROR;

  printf("\n--- Test Channel Functions ---\r\n");

  /* Open channel */
  CIFXHANDLE hDevice = NULL;
  lRet = xChannelOpen(hDriver, szBoard, 0, &hDevice);
  if(lRet != CIFX_NO_ERROR)
  {
    /* Read driver error description */
    ShowError( lRet);
  } else
  {
    uint32_t ulState = 0;

    /*=======================================================================================*/
    printf( "\n--- Test xChannelConfigLock() ---\r\n");

    /* Get lock configuration state */
    printf( "Test xChannelConfigLock(), CIFX_CONFIGURATION_GETLOCKSTATE \r\n");
    lRet = xChannelConfigLock( hDevice, CIFX_CONFIGURATION_GETLOCKSTATE, &ulState, 0);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Configuration Lock state = 0x%.8X \r\n", ulState);

    /* Lock configuration state */
    printf( "Test xChannelConfigLock(), CIFX_CONFIGURATION_LOCK \r\n");
    lRet = xChannelConfigLock( hDevice, CIFX_CONFIGURATION_LOCK, &ulState, 100);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Configuration Lock state = 0x%.8X\r\n", ulState);

    /* Unlock configuration state */
    printf( "Test xChannelConfigLock(), CIFX_CONFIGURATION_UNLOCK \r\n");
    lRet = xChannelConfigLock( hDevice, CIFX_CONFIGURATION_UNLOCK, &ulState, 100);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Configuration Lock state = 0x%.8X \r\n", ulState);

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelHostState() ---\r\n");

    /* Read host state */
    printf( "Test xChannelHostState(), CIFX_HOST_STATE_READ \r\n");
    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_READ, &ulState, 0L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Host State = 0x%.8X \r\n", ulState);

    /* Clear host state */
    printf( "Test xChannelHostState(), CIFX_HOST_STATE_NOT_READY \r\n");
    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_NOT_READY, &ulState, 1000L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Host State = 0x%.8X \r\n", ulState);

    /* Set host state */
    printf( "Test xChannelHostState(), CIFX_HOST_STATE_READY \r\n");
    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_READY, &ulState, 1000L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "Host State = 0x%.8X \r\n", ulState);

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelBusState() ---\r\n");

    /* Get BUS state */
    printf( "Test xChannelBusState(), CIFX_BUS_STATE_GETSTATE \r\n");
    lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_GETSTATE, &ulState, 1000L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "BUS State = 0x%.8X \r\n", ulState);

    /* BUS OFF */
    printf( "Test xChannelBusState(), CIFX_BUS_STATE_OFF \r\n");
    lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_OFF, &ulState, 1000L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "BUS State = 0x%.8X \r\n", ulState);

    /* BUS ON */
    /* Timeout value larger 1s required for automatic test with Profibus CB-AB32 Slave */
    printf( "Test xChannelBusState(), CIFX_BUS_STATE_ON \r\n");
    lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_ON, &ulState, 5000L);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "BUS State = 0x%.8X \r\n", ulState);

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelDMAState() ---\r\n");

    /* Get DMA state */
    printf( "Test xChannelDMAState(), CIFX_DMA_STATE_GETSTATE \r\n");
    lRet = xChannelDMAState( hDevice, CIFX_DMA_STATE_GETSTATE, &ulState);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "DMA State = 0x%.8X \r\n", ulState);

    /* DMA ON */
    printf( "Test xChannelDMAState(), CIFX_DMA_STATE_ON \r\n");
    lRet = xChannelDMAState( hDevice, CIFX_DMA_STATE_ON, &ulState);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "DMA State = 0x%.8X \r\n", ulState);

    /* DMA OFF */
    printf( "Test xChannelDMAState(), CIFX_DMA_STATE_OFF \r\n");
    lRet = xChannelDMAState( hDevice, CIFX_DMA_STATE_OFF, &ulState);
    if (CIFX_NO_ERROR != lRet)
      ShowError( lRet);
    else
      printf( "DMA State = 0x%.8X \r\n", ulState);

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelInfo() ---\r\n");

    {
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      /* Read Channel info */
      printf( "Test xChannelInfo(), CIFX_BUS_STATE_GETSTATE \r\n");
      lRet = xChannelInfo( hDevice, sizeof(tChannelInfo), (void*)&tChannelInfo);
      ShowError( lRet);
    }

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelControlBlock() ---\r\n");

    {
      unsigned char abBuffer[100] = {0};

      /* Read control block */
      printf("Test xChannelControlBlock(),  CIFX_CMD_READ_DATA \r\n");
      lRet = xChannelControlBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);

      /* Write control block */
      printf("Test xChannelControlBlock(), CIFX_CMD_WRITE_DATA \r\n");
      lRet = xChannelControlBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);
    }

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelCommonStatusBlock() ---\r\n");
    {
      unsigned char abBuffer[100] = {0};

      /* Read common status block */
      printf("Test xChannelCommonStatusBlock(), CIFX_CMD_READ_DATA \r\n");
      lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);

      /* Write common status block */
      printf("Test xChannelCommonStatusBlock(), CIFX_CMD_WRITE_DATA \r\n");
      lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);

    }
    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelExtendedStatusBlock() ---\r\n");
    {
      unsigned char abBuffer[100] = {0};

      /* Read extended status block */
      printf("Test xChannelExtendedStatusBlock(), CIFX_CMD_READ_DATA \r\n");
      lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);

      /* Write extended status block */
      printf("Test xChannelExtendedStatusBlock(), CIFX_CMD_WRITE_DATA \r\n");
      lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);
    }
    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    /*
    printf( "\n--- Test xChannelUserBlock() ---\r\n");
    {
      unsigned char abBuffer[100] = {0};

      // Read user block
      printf("Test xChannelUserBlock(), CIFX_CMD_READ_DATA \r\n");
      lRet = xChannelUserBlock( hDevice, 0, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);

      // Write user block
      printf("Test xChannelUserBlock(), CIFX_CMD_WRITE_DATA \r\n");
      lRet = xChannelUserBlock( hDevice, 0, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);
      ShowError(lRet);
    }
    printf( "--- Test done \r\n");
    */

    /*=======================================================================================*/

    printf( "\n--- Test xChannelRegisterNotification() / xChannelUnregisterNotification ---\r\n");

    /* Register notifications */
    printf("Test xChannelRegisterNotification(), CIFX_NOTIFY_RX_MBX_FULL \r\n");
    lRet = xChannelRegisterNotification  ( hDevice, CIFX_NOTIFY_RX_MBX_FULL, EventCallback, NULL);
    ShowError(lRet);

    printf("Test xChannelRegisterNotification(), CIFX_NOTIFY_TX_MBX_EMPTY \r\n");
    lRet = xChannelRegisterNotification  ( hDevice, CIFX_NOTIFY_TX_MBX_EMPTY, EventCallback, NULL);
    ShowError(lRet);

    printf("Test xChannelRegisterNotification(), CIFX_NOTIFY_PD0_IN \r\n");
    lRet = xChannelRegisterNotification  ( hDevice, CIFX_NOTIFY_PD0_IN, EventCallback, NULL);
    ShowError(lRet);

    printf("Test xChannelRegisterNotification(), CIFX_NOTIFY_PD0_OUT \r\n");
    lRet = xChannelRegisterNotification  ( hDevice, CIFX_NOTIFY_PD0_OUT, EventCallback, NULL);
    ShowError(lRet);

    printf("Test xChannelRegisterNotification(), CIFX_NOTIFY_SYNC \r\n");
    lRet = xChannelRegisterNotification  ( hDevice, CIFX_NOTIFY_SYNC, EventCallback, NULL);
    ShowError(lRet);


    /* Unregister notifications */
    printf("Test xChannelUnregisterNotification(), CIFX_NOTIFY_RX_MBX_FULL \r\n");
    lRet = xChannelUnregisterNotification  ( hDevice, CIFX_NOTIFY_RX_MBX_FULL);
    ShowError(lRet);

    printf("Test xChannelUnregisterNotification(), CIFX_NOTIFY_TX_MBX_EMPTY \r\n");
    lRet = xChannelUnregisterNotification  ( hDevice, CIFX_NOTIFY_TX_MBX_EMPTY);
    ShowError(lRet);

    printf("Test xChannelUnregisterNotification(), CIFX_NOTIFY_PD0_IN \r\n");
    lRet = xChannelUnregisterNotification  ( hDevice, CIFX_NOTIFY_PD0_IN);
    ShowError(lRet);

    printf("Test xChannelUnregisterNotification(), CIFX_NOTIFY_PD0_OUT \r\n");
    lRet = xChannelUnregisterNotification  ( hDevice, CIFX_NOTIFY_PD0_OUT);
    ShowError(lRet);

    printf("Test xChannelUnregisterNotification(), CIFX_NOTIFY_SYNC \r\n");
    lRet = xChannelUnregisterNotification  ( hDevice, CIFX_NOTIFY_SYNC);
    ShowError(lRet);

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    printf( "\n--- Test xChannelSyncState() ---\r\n");

    {
      uint32_t ulErrorCount = 0;

      printf("Test xChannelSyncState(), CIFX_SYNC_SIGNAL_CMD \r\n");
      lRet = xChannelSyncState( hDevice, CIFX_SYNC_SIGNAL_CMD, 0, &ulErrorCount);
      ShowError(lRet);

      printf("Test xChannelSyncState(), CIFX_SYNC_ACKNOWLEDGE_CMD \r\n");
      lRet = xChannelSyncState( hDevice, CIFX_SYNC_ACKNOWLEDGE_CMD, 0, &ulErrorCount);
      ShowError(lRet);

      printf("Test xChannelSyncState(), CIFX_SYNC_WAIT_CMD \r\n");
      lRet = xChannelSyncState( hDevice, CIFX_SYNC_WAIT_CMD, 0, &ulErrorCount);
      ShowError(lRet);
    }

    printf( "--- Test done \r\n");

    /*=======================================================================================*/

    /* Close channel */
    if( hDevice != NULL) xChannelClose(hDevice);
  }
}
