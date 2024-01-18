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
     1         23.11.2011  SD       initial version
  
 
**************************************************************************************/

#include "OS_Includes.h"

#define __USE_UNIX98
#include <pthread.h>

#include <errno.h>
#include <poll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h> /* for PTHREAD_STACK_MIN */
#include <stdlib.h>

#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP

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
/*! Create Lock (Usually same as mutex, but does not support timed waiting)
*     \return Handle to created lock                                         */
/*****************************************************************************/
void* OS_CreateLock(void) {
  pthread_mutexattr_t mta;
  pthread_mutex_t     *mutex;
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
  mutex = malloc( sizeof(pthread_mutex_t) );
  if( mutex == NULL )
  {
    perror("allocating memory for mutex");
    return NULL;
  }
  if( (iRet = pthread_mutex_init(mutex, &mta)) != 0 )
  {
    fprintf( stderr, "Mutex init: %s\n", strerror(iRet));
    goto err_out;
  }
  return mutex;

err_out:
  free(mutex);
  return NULL;
}

/*****************************************************************************/
/*! Acquire a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void OS_EnterLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;
  int             iRet;
  
#ifdef VERBOSE_2
  printf("%s() called\n", __FUNCTION__);
#endif

  if( (iRet = pthread_mutex_lock(mutex)) != 0)
  {
    fprintf( stderr, "Mutex lock: %s\n", strerror(iRet));
  }
}

/*****************************************************************************/
/*! Release a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void OS_LeaveLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;
  int             iRet;

#ifdef VERBOSE_2
  printf("%s() called\n", __FUNCTION__);
#endif

  if( (iRet = pthread_mutex_unlock(mutex)) != 0)
  {
    fprintf( stderr, "Mutex unlock: %s\n", strerror(iRet));
  }
}

/*****************************************************************************/
/*! Delete a lock
*     \param pvLock Handle to lock                                           */
/*****************************************************************************/
void OS_DeleteLock(void* pvLock) {
  pthread_mutex_t *mutex = (pthread_mutex_t *) pvLock;
  int             iRet;
  
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  
  if( (iRet = pthread_mutex_destroy(mutex)) != 0 )
    fprintf( stderr, "Delete lock: %s\n", strerror(iRet));

  free(mutex);
}


/*****************************************************************************/
/*! Get Millisecond counter value (used for timeout handling)
*     \return Counter value with a resolution of 1ms                         */
/*****************************************************************************/
uint32_t OS_Gettickcount(void) {
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
/*! Sleep for the given time
*     \param ulSleepTimeMs Time in ms to sleep (0 will sleep for 50us)       */
/*****************************************************************************/
void OS_Sleep(uint32_t ulSleepTimeMs) {
  struct timespec sleeptime;
  struct timespec RemainingTime;
  struct timespec *pRemainingTime = &RemainingTime;
  int    iRet;
  int    iTmpErrno;

  if(ulSleepTimeMs == 0)
  {
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 50000; // 50 usecs
  } else 
  {
    sleeptime.tv_sec = ulSleepTimeMs / 1000;
    ulSleepTimeMs -= sleeptime.tv_sec * 1000;
    sleeptime.tv_nsec = ulSleepTimeMs * 1000 * 1000;
  }

  iTmpErrno = errno;
  errno = 0;
  while((iRet = nanosleep(&sleeptime, pRemainingTime)))
  {
    if ((errno == EINTR) && (pRemainingTime != NULL) ) 
    {  
      sleeptime.tv_sec  = RemainingTime.tv_sec;
      sleeptime.tv_nsec = RemainingTime.tv_nsec;
    } else
    {
      perror("OS_Sleep failed");
    } 
  }
  errno = iTmpErrno; 
}

/*****************************************************************************/
/*! Open file for reading
*     \param szFilename   File to open (including path)
*     \param pulFileSize  Returned size of the file in bytes
*     \return Handle to the file, NULL on failure                            */
/*****************************************************************************/
void* OS_FileOpen(char* szFilename, uint32_t * pulFileSize) {
  int         fd;
  struct stat buf;
#ifdef VERBOSE
  printf("%s(%s) called\n", __FUNCTION__, szFilename);
#endif
  fd = open(szFilename, O_RDONLY);
  if( fd == -1 )
  {
    perror("open file");
    return NULL;
  }

  if( fstat(fd, &buf) != 0 )
  {
    perror("fstat failed");
    return NULL;
  }

  *pulFileSize = buf.st_size;
#ifdef VERBOSE
  printf("opened: %s (%lu bytes)\n", szFilename, *pulFileSize);
#endif

  return fdopen(fd, "r");
}

/*****************************************************************************/
/*! Read data from file
*     \param pvFile    Handle to the file (acquired by OS_FileOpen)
*     \param ulOffset  Offset to read from
*     \param ulSize    Size to read
*     \param pvBuffer  Buffer to read data into
*     \return number of bytes read                                           */
/*****************************************************************************/
uint32_t OS_FileRead(void* pvFile, uint32_t ulOffset,
                          uint32_t ulSize, void* pvBuffer) {
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  return fread(pvBuffer, 1, ulSize, pvFile);
}

/*****************************************************************************/
/*! Close open file
*     \param pvFile    Handle to the file (acquired by OS_FileOpen)          */
/*****************************************************************************/
void OS_FileClose(void* pvFile) {
#ifdef VERBOSE
  printf("%s() called\n", __FUNCTION__);
#endif
  if( fclose(pvFile) != 0 )
    perror("FileClose failed");
}

int OS_GetLastError(void)
{
  return errno;
}

int ReadLine(void)
{
  char bLine[100];
  fgets( bLine, 100, stdin);
  return bLine[0];
}

/*****************************************************************************/
/*! Reads character from stdin
*  blocks until character is read and return character as int                */
/*****************************************************************************/
int OS_GetChar(void)
{
  int iChar;
  while(0== (iChar = OS_KbHit())){}
  
  /* return character */
  return iChar;
}

/*****************************************************************************/
/*! Checks if key is pressed
*  return character as int                                                   */
/*****************************************************************************/
int OS_KbHit(void)
{
  struct pollfd fds;
  fds.fd      = 0;//stdin
  fds.events  = POLLIN;
  fds.revents = 0;
  
  if (0>=poll( &fds, 1, 1))
  {
    return 0;
  } else
  {
    return ReadLine();
  }
}

