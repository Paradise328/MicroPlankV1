/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_PLCFunctions.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    Demonstrate the PLC-functions

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

/*****************************************************************************/
/*! Test PLC Functions
*
*   \param  hDriver    Driver handle
*   \param  szBoard    Board name                                            */
/*****************************************************************************/
void TestPLCFunctions( CIFXHANDLE hDriver, char* szBoard)
{
  unsigned char abBuffer[1000]  = {0};
  uint32_t      ulState         = 0;

  printf("\n--- Test PLC functions ---\r\n");

  int32_t lRet = CIFX_NO_ERROR;

  /* Open channel */
  CIFXHANDLE hDevice = NULL;
  lRet = xChannelOpen(hDriver, szBoard, 0, &hDevice);
  if(lRet != CIFX_NO_ERROR)
  {
    ShowError(lRet);
  } else
  {
    /* Start PLC functions */
    unsigned char*  pabDPMMemory          = NULL;
    uint32_t        ulAreaStartOffset     = 0;
    uint32_t        ulAreaSize            = 0;
    int32_t         lRetIN                = CIFX_NO_ERROR;
    int32_t         lRetOUT               = CIFX_NO_ERROR;

    /* Define the memory structures for Input data */
    PLC_MEMORY_INFORMATION tMemory = {0};
    tMemory.pvMemoryID            = NULL;                   /* Identification of the memory area */
    tMemory.ppvMemoryPtr          = (void**)&pabDPMMemory;  /* Memory pointer */
    tMemory.ulAreaDefinition      = CIFX_IO_INPUT_AREA;     /* Input/output area */
    tMemory.ulAreaNumber          = 0;                      /* Area number */
    tMemory.pulIOAreaStartOffset  = &ulAreaStartOffset;     /* Start offset of the requested channel */
    tMemory.pulIOAreaSize         = &ulAreaSize;            /* Memory size of the requested channel */


    /* Define the memory structures for Output data */
    unsigned char*  pabDPMMemory_OUT          = NULL;
    uint32_t        ulAreaStartOffset_OUT     = 0;
    uint32_t        ulAreaSize_OUT            = 0;

    PLC_MEMORY_INFORMATION tMemory_OUT = {0};
    tMemory_OUT.pvMemoryID            = NULL;                       /* Identification of the memory area */
    tMemory_OUT.ppvMemoryPtr          = (void**)&pabDPMMemory_OUT;  /* Memory pointer */
    tMemory_OUT.ulAreaDefinition      = CIFX_IO_OUTPUT_AREA;        /* Input/output area */
    tMemory_OUT.ulAreaNumber          = 0;                          /* Area number */
    tMemory_OUT.pulIOAreaStartOffset  = &ulAreaStartOffset_OUT;     /* Start offset of the requested channel */
    tMemory_OUT.pulIOAreaSize         = &ulAreaSize_OUT;            /* Memory size of the requested channel */

    /* Open a DPM memory pointer */
    if ( (CIFX_NO_ERROR != (lRetIN  = xChannelPLCMemoryPtr( hDevice, CIFX_MEM_PTR_OPEN, &tMemory)) )     ||
         (CIFX_NO_ERROR != (lRetOUT = xChannelPLCMemoryPtr( hDevice, CIFX_MEM_PTR_OPEN, &tMemory_OUT)))  )
    {
      /* Failed to get the memory mapping */
      ShowError( lRetIN);
      ShowError( lRetOUT);
    } else
    {
      uint32_t ulWaitBusCount = 100;

      printf( "Memory (Input Data)\n" \
              "-------------------\n" \
              "Memory ID      : 0x%p\n" \
              "Memory Pointer : 0x%p\n" \
              "Area Def       : 0x%08X\n" \
              "Area Number    : 0x%08X\n" \
              "Area Offset    : 0x%08X\n" \
              "Area Size      : 0x%08X\n",
              tMemory.pvMemoryID, pabDPMMemory,
              tMemory.ulAreaDefinition, tMemory.ulAreaNumber, ulAreaStartOffset,
              ulAreaSize);

      printf( "Memory (Output Data)\n" \
              "--------------------\n" \
              "Memory ID      : 0x%p\n" \
              "Memory Pointer : 0x%p\n" \
              "Area Def       : 0x%08X\n" \
              "Area Number    : 0x%08X\n" \
              "Area Offset    : 0x%08X\n" \
              "Area Size      : 0x%08X\n",
              tMemory_OUT.pvMemoryID, pabDPMMemory_OUT,
              tMemory_OUT.ulAreaDefinition, tMemory_OUT.ulAreaNumber, ulAreaStartOffset_OUT,
              ulAreaSize_OUT);

      /* Signal application is ready */
      lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_READY, &ulState, 100);
      if( CIFX_NO_ERROR != lRet)
      {
        ShowError(lRet);
      }

      /* Wait until BUS is up and running */
      printf("\r\nWait until BUS communication is available!\r\n");
      do
      {
        lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_ON, &ulState, 100);
        if( CIFX_NO_ERROR != lRet)
        {
          if( CIFX_DEV_NO_COM_FLAG != lRet)
          {
            ShowError(lRet);
            break;
          }
        } else if( 1 == ulState)
        {
          /* Bus is ON */
          printf("\r\nBUS is ON!\r\n");
          break;
        }
      } while ( --ulWaitBusCount > 0);

      if( 0 == ulWaitBusCount)
      {
        ShowError(lRet);
      }

      /*----------------------*/
      /* Start cyclic data IO */
      /*----------------------*/
      if( CIFX_NO_ERROR == lRet)
      {
        printf("\n Reading and writing INPUT/OUTPUT image. Press (Q) to stop... \r\n");
        do
        {
          /* We have a memory mapping, check if access to the DPM is allowed */
          uint32_t ulReadState  = 0;
          uint32_t ulWriteState = 0;

          /*----------------------------------------*/
          /* Check if we can access the INPUT image */
          /*----------------------------------------*/

          lRet = xChannelPLCIsReadReady  ( hDevice, 0, &ulReadState);
          if( CIFX_NO_ERROR != lRet)
          {
            ShowError( lRet);
          } else if( 1 == ulReadState)
          {
            /* It is allowed to read the image */
            /* Read 100 Bytes */
            OS_Memcpy( abBuffer, pabDPMMemory, sizeof(abBuffer));

            /* Activate transfer */
            lRet = xChannelPLCActivateRead  ( hDevice, 0);
            if( CIFX_NO_ERROR != lRet)
              ShowError( lRet);
          }

          /*-----------------------------------------*/
          /* Check if we can access the OUTPUT image */
          /*-----------------------------------------*/
          lRet = xChannelPLCIsWriteReady ( hDevice, 0, &ulWriteState);
          if( CIFX_NO_ERROR != lRet)
          {
            ShowError( lRet);
          } else if( 1 == ulWriteState)
          {
            /* It is allowed to write the image */
            pabDPMMemory_OUT[0]++;
            pabDPMMemory_OUT[1] = abBuffer[1];

            lRet = xChannelPLCActivateWrite  ( hDevice, 0);
            if( CIFX_NO_ERROR != lRet)
              ShowError( lRet);
          }
        }while(!OS_KbHit());
      }
    }

    lRet = xChannelBusState( hDevice, CIFX_BUS_STATE_OFF, &ulState, 100);
    if(CIFX_NO_ERROR != lRet)
    {
      ShowError(lRet);
    }

    lRet = xChannelHostState( hDevice, CIFX_HOST_STATE_NOT_READY, &ulState, 100);
    if(CIFX_NO_ERROR != lRet)
    {
      ShowError(lRet);
    }

    /* Return the DPM memory pointer */
    if ( NULL != pabDPMMemory)
    {
      lRet = xChannelPLCMemoryPtr( hDevice, CIFX_MEM_PTR_CLOSE, &tMemory);
      if(lRet != CIFX_NO_ERROR)
        /* Failed to return memory pointer */
        ShowError( lRet);
    }

    /* Return the DPM memory pointer */
    if ( NULL != pabDPMMemory_OUT)
    {
      lRet = xChannelPLCMemoryPtr( hDevice, CIFX_MEM_PTR_CLOSE, &tMemory_OUT);
      if(lRet != CIFX_NO_ERROR)
        /* Failed to return memory pointer */
        ShowError( lRet);
    }

    /* Close channel */
    if( hDevice != NULL) xChannelClose(hDevice);
  }

  printf("\n Test PLC functions done\r\n");
}
