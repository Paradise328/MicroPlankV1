/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Id: OS_Includes.h 1206 2010-04-15 11:59:09Z sebastiand $
   Last Modification:
    $Author: sebastiand $
    $Date: 2010-04-15 13:59:09 +0200 (Thu, 15 Apr 2010) $
    $Revision: 1206 $

   Targets:
     Linux        : yes

   Description:
    OS Dependent function declaration. These functions must be implemented to allow 
    abstraction from the operating system

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1        23.11.2011   SD       initial version
 

**************************************************************************************/
#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H

#define UINT32 uint32_t

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <netdb.h>
#include <unistd.h>

#ifndef NULL
  #define NULL  ((void*)0)
#endif

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(a) (a=a)
#endif

void*            OS_Malloc    (uint32_t ulSize);
void             OS_Free      (void* pvMem);
#define          OS_Memcpy    memcpy

void*            OS_CreateLock(void);
void             OS_EnterLock (void* pvLock);
void             OS_LeaveLock (void* pvLock);
void             OS_DeleteLock(void* pvLock);

int              OS_Strnicmp  (const char* pszBuf1, const char* pszBuf2, uint32_t ulLen);
#define          OS_Strncpy   strncpy
#define          OS_Strlen    strlen

void             OS_Sleep       (uint32_t ulSleepTimeMs);
uint32_t         OS_Gettickcount(void);

int              OS_KbHit    (void);
int              OS_GetChar  (void);

void*            OS_FileOpen (char* szFilename, uint32_t * pulFileSize);
void             OS_FileClose(void* pvFile);
uint32_t         OS_FileRead (void* pvFile, uint32_t ulOffset, uint32_t ulSize, void* pvBuffer);

int              OS_GetLastError(void);

#ifdef __cplusplus
}
#endif

#endif /* __OS_INCLUDES__H */
