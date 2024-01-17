/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: $
   Last Modification:
    $Author: sebastiand $
    $Modtime: $
    $Revision: 1034 $
   
   Targets:
     Linux        : yes
     
   Description:
   Linux OS Abstraction Layer implementation.
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         25.05.2010  SD       initial version
  
 
**************************************************************************************/

#include "OS_Includes.h"
#include <pthread.h>

pthread_mutex_t* g_ptMutex;

/*****************************************************************************/
/*! Memory allocation wrapper (standard malloc)
*     \param ulSize Size of block to allocate
*     \return NULL on failure                                                */
/*****************************************************************************/
void* OS_Malloc(uint32_t ulSize) {
  void *mem_ptr;
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  mem_ptr = malloc(ulSize);

  if( mem_ptr == NULL )
    perror("Memalloc failed");

  return mem_ptr;
}


/*****************************************************************************/
/*! Memory de-allocation wrapper (standard free)
*     \param pvMem  Block to free                                            */
/*****************************************************************************/
void OS_Free(void* pvMem) {
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  free(pvMem);
}


/*****************************************************************************/
/*! Compare strings case insensitive
*     \param pszBuf1  String buffer 1
*     \param pszBuf2  String buffer 2
*     \param ulLen    Maximum length to compare
*     \return 0 if strings are equal                                         */
/*****************************************************************************/
int OS_Strnicmp(const char* pszBuf1, const char* pszBuf2, uint32_t ulLen) {
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  return strncasecmp(pszBuf1, pszBuf2, ulLen);
}


/*****************************************************************************/
/*! Get Millisecond counter value (used for timeout handling)
*     \return Counter value with a resolution of 1ms                         */
/*****************************************************************************/
uint32_t OS_GetTickCount(void) {
  struct timespec ts_get_milli;
  unsigned int    msec_count;

#ifdef VERBOSE_2
  printf("%s() called\n", __FUNCTION__);
#endif
  if( clock_gettime( CLOCK_MONOTONIC, &ts_get_milli ) != 0 )
  {
    perror("gettime failed");
    return 0;
  }
  msec_count = ts_get_milli.tv_sec * 1000;
  msec_count += ts_get_milli.tv_nsec / 1000 / 1000;

  return msec_count;
}


/*****************************************************************************/
/*! Create Lock (Usually same as mutex, but does not support timed waiting)
*     \return Handle to created lock                                         */
/*****************************************************************************/
pthread_mutex_t* OS_Createlock(void) {
  pthread_mutexattr_t mta;
  int                 iRet;
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
 
  pthread_mutexattr_init(&mta);
  if( (iRet = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE)) != 0 )
  {
    fprintf( stderr, "Mutex set attr: %s\n", strerror(iRet));
    return NULL;
  }
  g_ptMutex = malloc( sizeof(pthread_mutex_t) );
  if( g_ptMutex == NULL )
  {
    perror("allocating memory for mutex");
    return NULL;
  }
  if( (iRet = pthread_mutex_init(g_ptMutex, &mta)) != 0 )
  {
    fprintf( stderr, "Mutex init: %s\n", strerror(iRet));
    goto err_out;
  }
  return g_ptMutex;

err_out:
  free(g_ptMutex);
  return NULL;
}


/*****************************************************************************/
/*! Acquire a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
int OS_Lock(void) {
  int iRet;
  
#ifdef VERBOSE_2
  printf("%s() called\n", __FUNCTION__);
#endif

 if( (iRet = pthread_mutex_lock(g_ptMutex)) != 0)
  {
    fprintf( stderr, "Mutex lock: %s\n", strerror(iRet));
  }

  return 1;
}


/*****************************************************************************/
/*! Release a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void OS_Unlock(int iLock) {
  int iRet;
  
#ifdef VERBOSE_2
  printf("%s() called\n", __FUNCTION__);
#endif

 if( (iRet = pthread_mutex_unlock(g_ptMutex)) != 0)
  {
    fprintf( stderr, "Mutex unlock: %s\n", strerror(iRet));
  }
}


/*****************************************************************************/
/*! Delete a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void OS_Deletelock(pthread_mutex_t* mutex) {
  int iRet;
  
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  if( (iRet = pthread_mutex_destroy(g_ptMutex)) != 0 )
    fprintf( stderr, "Delete lock: %s\n", strerror(iRet));

  free(g_ptMutex);
}



