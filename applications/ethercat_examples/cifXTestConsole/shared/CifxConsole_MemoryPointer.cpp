/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: CifxConsole_MemoryPointer.cpp 14823 2018-10-25 13:22:59Z LuisContreras $:

  Description:
    Memory pointer example implementation

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2020-01-16  Perform test on the selected board
    2018-10-19  Ported from cifXTest_Console V1.0.6.0
    2010-07-21  initial version

**************************************************************************************/
#include "OS_Includes.h"
#include "CifXConsole.h"

/*****************************************************************************/
/*! Test memory pointer
*
*   \param  hDriver    Driver handle                                         */
/*****************************************************************************/
void TestMemoryPointer( CIFXHANDLE hDriver, char* szBoard)
{
  uint32_t ulFoundBoard = 0;
  uint32_t ulBoardIdx   = 0;
  BOARD_INFORMATION tBoardInfo = {0};

  int32_t lRet = CIFX_NO_ERROR;

  printf("\n--- Test Memory Pointer ---\r\n");

  /* Search for the matching Board number (memory pointer API uses board number instead of string) */
  while(CIFX_NO_ERROR == lRet)
  {
    lRet = xDriverEnumBoards(hDriver, ulBoardIdx, sizeof(tBoardInfo), &tBoardInfo);

    if((CIFX_NO_ERROR == lRet) &&
       (OS_Strlen(szBoard) == OS_Strlen(tBoardInfo.abBoardName)) &&
       (0 == OS_Strnicmp(szBoard, tBoardInfo.abBoardName, OS_Strlen(szBoard))) )
    {
      ulFoundBoard = ulBoardIdx;
      break;
    }
    ulBoardIdx++;
  }

  if(CIFX_NO_ERROR != lRet)
  {
    printf("No matching board found (%s)\r\n", szBoard);
    ShowError( lRet);
  }
  else
  {
    unsigned char abBuffer[100] = {0};
    uint32_t        ulMemoryID            = 0;
    unsigned char*  pabDPMMemory          = NULL;
    uint32_t        ulMemorySize          = 0;
    uint32_t        ulChannelStartOffset  = 0;
    uint32_t        ulChannelSize         = 0;

    MEMORY_INFORMATION tMemory = {0};
    tMemory.pvMemoryID            = &ulMemoryID;            /* Identification of the memory area */
    tMemory.ppvMemoryPtr          = (void**)&pabDPMMemory;  /* Memory pointer */
    tMemory.pulMemorySize         = &ulMemorySize;          /* Complete size of the mapped memory */
    tMemory.ulChannel             = CIFX_NO_CHANNEL;        /* Requested channel number */
    tMemory.pulChannelStartOffset = &ulChannelStartOffset;  /* Start offset of the requested channel */
    tMemory.pulChannelSize        = &ulChannelSize;         /* Memory size of the requested channel */

    /* Open a DPM memory pointer */
    lRet = xDriverMemoryPointer( hDriver, ulFoundBoard, CIFX_MEM_PTR_OPEN, &tMemory);
    if(lRet != CIFX_NO_ERROR)
    {
      /* Failed to get the memory mapping */
      ShowError( lRet);
    } else
    {
        printf( "Memory ID      : %p\n" \
                "Memory Pointer : %p\n" \
                "Memory Size    : 0x%08X\n" \
                "Channel        : 0x%08X\n" \
                "Channel Offset : 0x%08X\n" \
                "Channel Size   : 0x%08X\n",
                tMemory.pvMemoryID, pabDPMMemory,
                ulMemorySize, tMemory.ulChannel, ulChannelStartOffset,
                ulChannelSize);

      /* We have a memory mapping */
      /* Read 100 Bytes */
      OS_Memcpy( abBuffer, pabDPMMemory, sizeof(abBuffer));
      printf("DPM content:\r\n");
      printf("0x0000: %d (%c)\r\n", abBuffer[0], abBuffer[0]);
      printf("0x0001: %d (%c)\r\n", abBuffer[1], abBuffer[1]);
      printf("0x0002: %d (%c)\r\n", abBuffer[2], abBuffer[2]);
      printf("0x0003: %d (%c)\r\n", abBuffer[3], abBuffer[3]);
      OS_Memcpy( pabDPMMemory, abBuffer, sizeof(abBuffer));
    }

    /* Return the DPM memory pointer */
    lRet = xDriverMemoryPointer( hDriver, 0, CIFX_MEM_PTR_CLOSE, &tMemory);
    ShowError( lRet);
  }

  /* Test done */
  printf("\n Memory Pointer test done\r\n");
}
