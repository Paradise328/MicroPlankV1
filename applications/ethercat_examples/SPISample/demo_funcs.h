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
      1        01.07.2014  SD       - Initial Version

**************************************************************************************/

#ifndef _demo_H__
#define _demo_H__

/* helper functions */
void*  CreateLock(void);
void   EnterLock(void* pvLock);
void   LeaveLock(void* pvLock);
void   DeleteLock(void* pvLock);

/* demo functions */
void    DisplayDriverInformation(void);
int32_t EnumBoardDemo(void);
int32_t SysdeviceDemo(char* szDevice);
int32_t ChannelDemo(char* szDevice);
int32_t BlockDemo(char* szDevice);
int32_t StateDemo(char* szDevice);

#endif //_demo_H__
