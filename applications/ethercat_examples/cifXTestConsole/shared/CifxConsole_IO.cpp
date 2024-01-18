/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_IO.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    IO Area example implementation

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console tag V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

/*****************************************************************************/
/*! Read / Write IO data
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name
*   \param  ulWaitTimeout  Timeout [ms] when reading/writing I/OBuffer       */
/*****************************************************************************/
void ReadWriteIOData( CIFXHANDLE hDriver, char* szBoard, unsigned long ulWaitTimeout)
{
  int32_t       lRet                  = CIFX_NO_ERROR;
  unsigned char abReadIOBuffer[1024]  = {0};
  unsigned char abWriteIOBuffer[1024] = {0};

  printf("\n--- Read/Write I/O Data ---\r\n");

  /* Open channel */
  CIFXHANDLE hDevice = NULL;
  lRet = xChannelOpen( hDriver, szBoard, 0, &hDevice);
  if(lRet != CIFX_NO_ERROR)
  {
    /* Read driver error description */
    ShowError( lRet);
  } else
  {
    uint32_t ulState = 0;
    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_READ, &ulState, 0L);
    /* Read driver error description */
    ShowError( lRet);

    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_READY, &ulState, 1000L);
    ShowError( lRet);

    lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_ON, &ulState, 1000L);
    ShowError( lRet);

    /* Cyclic IO data transfer */
    printf("\n Wait for I/O cycles. Press (Q) to stop \r\n");
    do
    {
      /* Read Input data from al areas */
      if( (lRet = xChannelIORead( hDevice, 0, 0, 100, &abReadIOBuffer[0], ulWaitTimeout)) != CIFX_NO_ERROR)
      {
        /* Error reading Input data */
        printf("ReadWriteIOData(): Reading INPUT data failed, Error: 0x%X\n", lRet);
        ShowError( lRet);
      }

      /* Write Output to all areas */
      if( (lRet = xChannelIOWrite( hDevice, 0, 0, 100, &abWriteIOBuffer[0], ulWaitTimeout)) != CIFX_NO_ERROR)
      {
        /* Error writing Output data */
        printf("ReadWriteIOData(): Writing OUTPUT data failed, Error: 0x%X\r\n", lRet);
        ShowError( lRet);
      }

      printf("IN  byte 0,1 = 0x%02X, 0x%02X, OUT byte 0,1 = 0x%02X, 0x%02X\n", abReadIOBuffer[0], abReadIOBuffer[1], abWriteIOBuffer[0], abWriteIOBuffer[1]);

      abWriteIOBuffer[0] ++;
      abWriteIOBuffer[1] = abReadIOBuffer[1];
    }while(!OS_KbHit());

    /* Close channel */
    if( hDevice != NULL) xChannelClose(hDevice);
  }
}
