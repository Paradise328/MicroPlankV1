/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_Watchdog.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    Starts watchdog and display WatchDogValue

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

/*****************************************************************************/
/*! Test watchdog
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name                                            */
/*****************************************************************************/
void TestWatchdog( CIFXHANDLE hDriver, char* szBoard)
{
  printf("\n--- Test Watchdog ---\r\n");

  int32_t lRet = CIFX_NO_ERROR;

  /* Open channel */
  CIFXHANDLE hDevice = NULL;
  lRet = xChannelOpen(hDriver, szBoard, 0, &hDevice);
  if(lRet != CIFX_NO_ERROR)
  {
    ShowError(lRet);
  } else
  {
    uint32_t ulWatchdogValue = 0;

    lRet = xChannelWatchdog( hDevice, CIFX_WATCHDOG_START, &ulWatchdogValue);
    ShowError(lRet);

    if ( lRet == CIFX_NO_ERROR)
    {

      printf("\n Display WatchDogValue. Press (Q) to stop... \r\n");
      do
      {
        OS_Sleep(10);

        lRet = xChannelWatchdog( hDevice, CIFX_WATCHDOG_START, &ulWatchdogValue);
        ShowError(lRet);

        /* Display Watchdog value */
        printf("--> Watchdog Value = %d\n", ulWatchdogValue);
      }while(!OS_KbHit());

      lRet = xChannelWatchdog( hDevice, CIFX_WATCHDOG_STOP, &ulWatchdogValue);
      ShowError(lRet);
    }

    /* Stop Watchdog checking */
    printf("Watchdog test done\r\n");
  }

  /* Close channel */
  if( hDevice != NULL) xChannelClose(hDevice);
}
