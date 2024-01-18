/**************************************************************************************
 *
 *   Copyright (c) Hilscher GmbH. All Rights Reserved.
 *
 **************************************************************************************

 Filename:
 $Workfile: spi_sample.c $
 Last Modification:
 $Author: sebastiand $
 $Modtime: 9.02.10 15:45 $
 $Revision: 3961 $

 Targets:
 Linux : yes

 Platform:

 Description:
 Example accessing netX device, connected via SPI, using the common cifX Driver API
 Based on the Linux cifX Driver using the cifX Toolkit Hardware Function Interface.

 Changes:

 Version   Date        Author   Description
 ----------------------------------------------------------------------------------
 1        01.07.2014  SD       - Initial Version

 **************************************************************************************/

#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cifxlinux.h"
#include <linux/spi/spidev.h>
#include <dirent.h>
#include "demo_funcs.h"

/*************************************************************************************/
/*** GLOBAL DEFINITIONS **************************************************************/
/*************************************************************************************/
#ifdef VERBOSE
#define DEBUG(x) printf(x)
#else
#define DEBUG(x)
#endif

/* alias for device under test */
#define CIFX_DEV "cifX0"

/* SPI sepcific configuration parameter */
#define SPI_IF_MODE          SPI_MODE_3
#define SPI_IF_UDELAY        0
#define SPI_IF_SPEED         100000
#define SPI_IF_BITS_PER_WORD 8

char szSPIInterface[261] = {0};

/*************************************************************************************/
/*** STRUCTURE DEFINITIONS ***********************************************************/
/*************************************************************************************/
/* states of the SPI interface */
typedef enum ESPI_IF_STATE_E {
  eNotInitialized,
  eInitialized,
} ESPI_IF_STATE;

/* Example structure containing information of the SPI interface (passed as user parameter -> see tSPISampleDev.userparam) */
struct SPI_PARAM_T {
  char          szName[256];  /* Name of the SPI interface (e.g. /dev/spidev1.0) */
  int           iSPIFD;       /* File desc of the SPI interface                  */
  void*         pvSerDPMLock; /* lock required to synchronize SPI access         */
  int           iError;       /* Error status of SPI interface                   */
  ESPI_IF_STATE eState;       /* Current interface status                        */
  /* The following parameter are related to the linux kernel 'spidev' interface  */
  uint32_t      ulUDelay;     /* SPI param:  If nonzero, how long to delay after the last bit transfer         */
  /*             before optionally deselecting the device before the next transfer */
  uint32_t      ulSpeed;      /* SPI param: see SPI_IOC_RD_MAX_SPEED_HZ, SPI_IOC_WR_MAX_SPEED_HZ    */
  uint8_t       bBits;        /* SPI param: see SPI_IOC_RD_BITS_PER_WORD, SPI_IOC_WR_BITS_PER_WORD */
  uint8_t       bMode;        /* SPI param: see SPI_IOC_RD_MODE, SPI_IOC_WR_MODE */
};

/* Internal structures required for read/write transactions */
/* SPI Read Transfer Structure */
struct SPI_RD_MSG_T {
  uint8_t  abSPIHeader[4];
  uint8_t  abData[1];
};
/* SPI Write Transfer Structure */
struct SPI_WR_MSG_T {
  uint8_t  abSPIHeader[3];
  uint8_t  abData[1];
};

void SPIHWIFDeInit( struct CIFX_DEVICE_T* ptDevice);

/*************************************************************************************/
/*** FUNCTION DEFINITIONS ************************************************************/
/*************************************************************************************/

/*****************************************************************************/
/*! Initializes SPI interface of device pointed by ptDevice.
 *   \param ptDevice  Pointer to the custom device                            */
/*****************************************************************************/
int32_t SPIHWIFInit( struct CIFX_DEVICE_T* ptDevice)
{
  struct SPI_PARAM_T* ptSPIParam = (struct SPI_PARAM_T*)ptDevice->userparam;
  int                 iRet       = 0;

  DEBUG("++SPIHWIFInit\n");

  if (NULL == ptSPIParam) {
    fprintf( stderr, "SPIHWIFInit: Invalid initialization parameter for SPI device '%s'\n", szSPIInterface);
    DEBUG("--SPIHWIFInit\n");
    return CIFX_INVALID_PARAMETER;
  }
  strcpy( ptSPIParam->szName, szSPIInterface);
  ptSPIParam->bBits   = SPI_IF_BITS_PER_WORD;
  ptSPIParam->bMode   = SPI_IF_MODE;
  ptSPIParam->ulSpeed = SPI_IF_SPEED;

  if (0 > (ptSPIParam->iSPIFD = open( ptSPIParam->szName, O_RDWR))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to open SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));
    DEBUG("--SPIHWIFInit\n");
    return CIFX_FILE_OPEN_FAILED;

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_WR_MODE, &ptSPIParam->bMode))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_WR_MODE' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_RD_MODE, &ptSPIParam->bMode))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_RD_MODE' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_WR_BITS_PER_WORD, &ptSPIParam->bBits))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_WR_BITS_PER_WORD' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_RD_BITS_PER_WORD, &ptSPIParam->bBits))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_RD_BITS_PER_WORD' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_WR_MAX_SPEED_HZ, &ptSPIParam->ulSpeed))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_WR_MAX_SPEED_HZ' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else if (0 > (iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_RD_MAX_SPEED_HZ, &ptSPIParam->ulSpeed))) {
    ptSPIParam->iError = errno;
    fprintf( stderr, "SPIHWIFInit: Failed to set 'SPI_IOC_RD_MAX_SPEED_HZ' on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  } else {/* successfuly initialized SPI interface */
    /* create SPI access lock */
    if (NULL != (ptSPIParam->pvSerDPMLock = CreateLock())) {
      struct spi_ioc_transfer tSPITransfer = {0};
      uint8_t                 abData[10]   = {0};
      int                     iRetry       = 0;

      /* Do dummy read (required on netX51) */
      tSPITransfer.tx_buf = (unsigned long)abData;
      tSPITransfer.rx_buf = (unsigned long)abData;
      tSPITransfer.len    = 10;

      do
      {
        memset(abData, 0, sizeof(abData));
        abData[0] = 0x80; /* set read request */
        /* issue dummy transfer */
        if (0>(iRet = ioctl(ptSPIParam->iSPIFD, SPI_IOC_MESSAGE(1), &tSPITransfer))) {
          fprintf( stderr, "SPIHWIFInit: Failed to send message on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));
          ptSPIParam->iError = errno;
        } else {
          /* skip first result since this is not valid on netX51 */
          if (iRetry == 1) {
            /* check if DPM is in the correct status (serial DPM enabled and unlocked) */
            if (abData[0] != 0x11) {
              iRet = CIFX_FUNCTION_FAILED;
              fprintf( stderr, "SPIHWIFInit: Failed to read from serial DPM. Incorrect DPM status of SPI device '%s' (0x%X).\n", ptSPIParam->szName, abData[0]);
              fprintf( stderr, "SPIHWIFInit: Check the SPI connection and the serial DPM configuration of the device connected on '%s'.\n", ptSPIParam->szName);
            } else {
              ptSPIParam->ulUDelay = 0;
              ptSPIParam->eState   = eInitialized;
            }
            break;
          }
          iRetry++;
        }
      }while (0<=iRet);
    }
  }
  if (0>iRet)
    SPIHWIFDeInit( ptDevice);

  DEBUG("--SPIHWIFInit\n");

  if (0>iRet) {
    return CIFX_FUNCTION_FAILED;
  } else {
    ptSPIParam->iError = 0;
    return CIFX_NO_ERROR;
  }
}

/*****************************************************************************/
/*! De-initializes SPI interface of device pointed by ptDevice.
 *   \param ptDevice  Pointer to the custom device                            */
/*****************************************************************************/
void SPIHWIFDeInit( struct CIFX_DEVICE_T* ptDevice)
{
  struct SPI_PARAM_T* ptSPIParam = (struct SPI_PARAM_T*)ptDevice->userparam;

  DEBUG("++SPIHWIFDeInit\n");
  if (ptSPIParam->pvSerDPMLock) {
    DeleteLock(ptSPIParam->pvSerDPMLock);
    ptSPIParam->pvSerDPMLock = NULL;
  }
  close(ptSPIParam->iSPIFD);

  ptSPIParam->eState = eNotInitialized;
  DEBUG("--SPIHWIFDeInit\n");
}

/*****************************************************************************/
/*! Helper function, transfers SPI message.
 *   \param ptSPIParam Pointer to interface specific parameter
 *   \param pbData     Pointer to SPI message
 *   \param pbData     length of SPI message
 *   \param fCSChange                                                         */
/*****************************************************************************/
static void SPITransferMessage(struct SPI_PARAM_T* ptSPIParam, uint8_t *pbData, uint32_t ulLen, uint8_t fCSChange)
{
  int                     iRet         = 0;
  struct spi_ioc_transfer tSPITransfer = {0};

  DEBUG("++SPITransferMessage\n");

  tSPITransfer.tx_buf    = (uint64_t)pbData;
  tSPITransfer.rx_buf    = (uint64_t)pbData;
  tSPITransfer.len       = ulLen;
  tSPITransfer.cs_change = 0;

  if (0>(iRet = ioctl( ptSPIParam->iSPIFD, SPI_IOC_MESSAGE(1), &tSPITransfer)))
    fprintf( stderr, "SPITransferMessage: Failed to transfer message on SPI device '%s' - '%s'.\n", ptSPIParam->szName, strerror(errno));

  DEBUG("--SPITransferMessage\n");
}

/*****************************************************************************/
/*! Helper function, reading data via SPI interface.
 *  Translate read request into SPI read message.
 *   \param ptSPIParam Pointer to interface specific parameter
 *   \param ulDpmAddr Address offset in DPM to read data from
 *   \param pbData    Pointer to Buffer to store read data
 *   \param ulLen     Number of bytes to read                                 */
/*****************************************************************************/
static void SPIReadChunk( struct SPI_PARAM_T* ptSPIParam, uint32_t ulDpmAddr, uint8_t *pbData, uint32_t ulLen)
{
  struct SPI_RD_MSG_T* ptSPIRDMsg = (struct SPI_RD_MSG_T*)calloc(ulLen+4, sizeof(uint8_t));

  DEBUG("++SPIReadChunk\n");

  /* prepare SPI message */
  ptSPIRDMsg->abSPIHeader[0] = 0x80 + (uint8_t)((ulDpmAddr >> 16) & 0x0F);
  ptSPIRDMsg->abSPIHeader[1] = (uint8_t)((ulDpmAddr >> 8) & 0xFF);
  ptSPIRDMsg->abSPIHeader[2] = (uint8_t)((ulDpmAddr) & 0xFF);
  ptSPIRDMsg->abSPIHeader[3] = 0x00;

  /* transfer message */
  SPITransferMessage( ptSPIParam, (uint8_t*)ptSPIRDMsg, ulLen+4, 0);

  /* return read data */
  memcpy( pbData, ptSPIRDMsg->abData, ulLen);

  free( ptSPIRDMsg);

  DEBUG("--SPIReadChunk\n");
}

/*****************************************************************************/
/*! Helper function, writing data via SPI interface
 *  Translate write request into SPI write message
 *   \param ptSPIParam Pointer to interface specific parameter
 *   \param ulDpmAddr  Offset in DPM where data to write to
 *   \param pbData     Pointer to Buffer pointing to write data
 *   \param ulLen      Number of bytes to write                               */
/*****************************************************************************/
static void SPIWriteChunk( struct SPI_PARAM_T* ptSPIParam, uint32_t ulDpmAddr, uint8_t *pbData, uint32_t ulLen)
{
  struct SPI_WR_MSG_T* ptSPIWRMsg = (struct SPI_WR_MSG_T*)calloc(ulLen+3, sizeof(uint8_t));

  DEBUG("++SPIWriteChunk\n");

  /* prepare SPI message */
  ptSPIWRMsg->abSPIHeader[0] = (uint8_t)((ulDpmAddr >> 16) & 0x0F);
  ptSPIWRMsg->abSPIHeader[1] = (uint8_t)((ulDpmAddr >> 8) & 0xFF);
  ptSPIWRMsg->abSPIHeader[2] = (uint8_t)((ulDpmAddr) & 0xFF);

  memcpy( ptSPIWRMsg->abData, pbData, ulLen);

  /* transfer message */
  SPITransferMessage( ptSPIParam, (uint8_t*)ptSPIWRMsg, ulLen+3, 0);

  free( ptSPIWRMsg);

  DEBUG("--SPIWriteChunk\n");
}

/*****************************************************************************/
/*! Read a number of bytes via the custom hardware interface function
 *   \param ptDevice  Pointer to the custom device
 *   \param pvDpmAddr Address offset in DPM to read data from
 *   \param pvDst     Buffer to store read data
 *   \param ulLen     Number of bytes to read                                 */
/*****************************************************************************/
void* SPIHWIFRead( struct CIFX_DEVICE_T* ptDevice, void* pvDpmAddr, void* pvDst, uint32_t ulLen)
{
  struct SPI_PARAM_T* ptSPIParam = (struct SPI_PARAM_T*)ptDevice->userparam;

  DEBUG("++SPIHWIFRead\n");

  /* check if interface is correctly configured */
  if ((NULL != ptSPIParam) && (ptSPIParam->eState == eInitialized)) {
    /* enter SPI access lock */
    EnterLock(ptSPIParam->pvSerDPMLock);
    /* read data from DPM */
    SPIReadChunk( ptSPIParam, (uint32_t)(intptr_t)pvDpmAddr, (uint8_t*)pvDst, ulLen);
    LeaveLock(ptSPIParam->pvSerDPMLock);
  }
  DEBUG("--SPIHWIFRead\n");
  return pvDst;
}

/*****************************************************************************/
/*! Read a number of bytes via the custom hardware interface function
 *   \param ptDevice  Pointer to the custom device
 *   \param pvDpmAddr Address offset in DPM to write data to
 *   \param pvSrc     Buffer to data to be written
 *   \param ulLen     Number of bytes to writ                                 */
/*****************************************************************************/
void* SPIHWIFWrite( struct CIFX_DEVICE_T* ptDevice, void* pvDpmAddr, void* pvSrc, uint32_t ulLen)
{
  struct SPI_PARAM_T* ptSPIParam = (struct SPI_PARAM_T*)ptDevice->userparam;

  DEBUG("++SPIHWIFWrite\n");

  /* check if interface is correctly configured */
  if ((NULL != ptSPIParam) && (ptSPIParam->eState == eInitialized)) {
    /* enter SPI access lock */
    EnterLock(ptSPIParam->pvSerDPMLock);
    /* write data to DPM */
    SPIWriteChunk( ptSPIParam, (uint32_t)(intptr_t)pvDpmAddr, (uint8_t*)pvSrc, ulLen);
    LeaveLock(ptSPIParam->pvSerDPMLock);
  }
  DEBUG("--SPIHWIFWrite\n");
  return pvDpmAddr;
}

int GetUserParam(int argc, char* argv[])
{
  int iRet = -1;

  if (argc>1) {
    if (0 == strncmp(argv[1],"/dev/",5)) {
      strcpy(szSPIInterface, argv[1]);
      iRet = 0;
    } else {
      printf("Usage:\n");
      printf("spisample [spi device]\n");
      printf("\nOptions:\n");
      printf("spi device - Device file of interface where netX device is connected to\n");
      printf("\nExamples:\n");
      printf("Without parameter, - application scans '/sys/class/spidev' and runs the demo on the first found interface.\n");
      printf("./spisample\n");
      printf("Run demo application on specific interface e.g. '/dev/spidev1.2'\n");
      printf("./spisample /dev/spidev1.2\n\n");
      return -1;
    }
  } else {
    struct dirent*  entry;
    DIR*            dir = opendir ("/sys/class/spidev");

    if (NULL != dir) {
      while ((entry = readdir(dir)) != NULL) {
        if ((0 == strncmp(entry->d_name,"spidev",6))) {
          sprintf(szSPIInterface,"/dev/%s", entry->d_name);
          iRet = 0;
          break;
        }
      }
      if (iRet != 0) {
        printf("\nFailed to scan for spi device based on 'spidev' interface!\n");
        printf("Make sure the system provides a SPI interface under '/dev/spidev/' or specify an alternative interface!\n");
        printf("Aborting demo application!\n");
      }

    } else {
      printf("\nThere is no spi device to access!\n");
      printf("Make sure the system provides a SPI interface under '/dev/spidev/' or specify an alternative interface!\n");
      printf("Aborting demo application!\n");
      iRet = -1;
    }
  }
  if (0 == iRet)
    printf("Demo application estimates netX device connected at SPI interface '%s'...\n\n", szSPIInterface);

  return iRet;
}

/*****************************************************************************/
/*! Main entry function
 *   \return 0                                                                */
/*****************************************************************************/
int main(int argc, char* argv[])
{
  struct CIFX_DEVICE_T   tSPISampleDev = {0};
  struct SPI_PARAM_T     tSPIParam     = {{0}};
  struct CIFX_LINUX_INIT tDriverInit   = {0};
  uint32_t               lRet          = 0;

  if (-1 == GetUserParam(argc,argv))
    return 0;

  /* NOTE: Since the driver can only autodetect PCI devices, all other devices need to be published   */
  /*       by the user. Therefore also the custom device, connected via SPI, need to be specified by  */
  /*       the 'CIFX_DEVICE_T' structure and passed to the driver. In case of successfull driver      */
  /*       initializataion the custom device, is accessible via the cifX API.                         */
  /*       For more information of the parameter CIFX_DEVICE_T, CIFX_LINUX_INIT and the generic       */
  /*       driver initialization refer to the cifX Linux Driver documentation.                        */
  /*                                                                                                  */
  /*       Custom device integration and access consists of three steps:                              */
  /*       1. Custom device definition via CIFX_DEVICE_T                                              */
  /*       2. Driver initialitation (pass custom device(s) to the driver)                             */
  /*       3. Access via the standard cifX Driver API                                                 */

  /***********************************************************/
  /*** 1. SPI(/CUSTOM) DEVICE DEFINITION VIA CIFX_DEVICE_T ***/
  /***********************************************************/
  /* example configuration of a SPI device */
  tSPISampleDev.dpm         = NULL;    /*!< set to 'NULL' since the SPI device is not memory mapped */
  tSPISampleDev.dpmaddr     = 0x0;     /*!< set to '0x00' since there is no direct address to the physical DPM */
  tSPISampleDev.dpmlen      = 0x10000; /*!< set to length of dpm (depends on the device) */
  /* since device is not a uio device and no pci card invalidate all parameter */
  tSPISampleDev.uio_num     = -1;      /*!< set to '-1' since it is no 'uio-devices' */
  tSPISampleDev.uio_fd      = -1;      /*!< set to '-1' since it is no 'uio-devices' */
  tSPISampleDev.pci_card    = 0;       /*!< set to 0 since it is no pci card */
  tSPISampleDev.force_ram   = 0;       /*!< Force usage of RAM instead of flash. Card will always be reset and all
  files are downloaded again (0 => autodetect) */
  /* in this example we are not interrested in any configuration state, so set parameter to NULL */
  tSPISampleDev.notify      = NULL; /*!< Function to call, after the card has passed several stages (usually needed on RAM based
  devices, that change DPM configuration during initialization) */
  /* append any custom parameter */
  tSPISampleDev.userparam   = (void*)&tSPIParam; /*!< Use this parameter to pass SPI interface specific information */
  /* there is no extra memory */
  tSPISampleDev.extmem      = NULL;   /*!< virtual pointer to extended memory  */
  tSPISampleDev.extmemaddr  = 0x00;   /*!< physical address to extended memory */
  tSPISampleDev.extmemlen   = 0;      /*!< Length of extended memory in bytes  */
  /* NOTE: The following parameter are SPI specific */
  /* hardware access functions, required to read or write to the hardware */
  tSPISampleDev.hwif_init   = SPIHWIFInit;   /*!< Function initializes SPI hw-function interface (need to be implemented) */
  tSPISampleDev.hwif_deinit = SPIHWIFDeInit; /*!< Function de-initializes SPI hw-function interface (need to be implemented) */
  tSPISampleDev.hwif_read   = SPIHWIFRead;   /*!< Function realize DPM read access via SPI transfers (need to be implemented) */
  tSPISampleDev.hwif_write  = SPIHWIFWrite;  /*!< Function realize DPM write access via SPI transfers (need to be implemented) */

  /***********************************************************/
  /*** 2. GENERIC DRIVER INITIALIZATION                    ***/
  /***********************************************************/
  /* setup the standard driver initializaion structure (for detailed information refer to the linux driver documentation) */
  tDriverInit.init_options       = CIFX_DRIVER_INIT_NOSCAN;/* no scan required, we only want to initialize the above specified spi device 'tSPISampleDev' */
  tDriverInit.iCardNumber        = 0;
  tDriverInit.fEnableCardLocking = 0;
  tDriverInit.base_dir           = NULL;
  tDriverInit.poll_interval      = 0;
  tDriverInit.poll_StackSize     = 0;
  tDriverInit.trace_level        = 255;
  tDriverInit.user_card_cnt      = 1;
  tDriverInit.user_cards         = &tSPISampleDev; /* the previously initialized SPI device structure */

  if (tDriverInit.trace_level)
    printf("For detailed information of the standard initialization procedure refer to the driver's log file (default: /opt/cifx/cifX[x].log)\n");

  printf("Information and errors related to the custom SPI interface, will be printed to console and stderr!\n\n");

  printf("Start cifX driver initialization (cifXDriverInit())...\n");
  /* initialize driver */
  lRet = cifXDriverInit(&tDriverInit);

  if(CIFX_NO_ERROR == lRet)
  {
    /* NOTE: cifXDriverInit() may not return an error in case of an initialization failure of a particular device.   */
    /*       If the device initialzation fails the device is not under toolkit control and so not accessible via the */
    /*       cifX API (-> the device will not be enumerated). Since we have access to the SPI initialization state   */
    /*       we can skip the enumeration process in case of an SPI specific error.                                   */
    /* check if SPI device is in an operational state */
    if ((tSPIParam.iError) || (tSPIParam.eState == eNotInitialized)) {
      printf("\nFailed to add custom SPI device to cifX driver!");

      if (tSPIParam.iError)
        printf(" Failed to initialize SPI interface! %s.\n", strerror(tSPIParam.iError));
      else if (tSPIParam.eState == eNotInitialized)
        printf("\nSPI interface not in an initialized state! Refer to the driver's log file for more information!\n");

      printf("\nAborting demo application!\n");
    } else {
      /***********************************************************/
      /*** 3. DEMO APPLICATION (USING THE CIFX API)            ***/
      /***********************************************************/
      printf("\nSuccessfully initialized the driver. Start demo...\n");
      /* Display version of the driver and the cifXToolkit */
      DisplayDriverInformation();

      /* Demonstrate the board/channel enumeration */
      EnumBoardDemo();

      /* Demonstrate system channel functionality */
      SysdeviceDemo( CIFX_DEV);

      /* Demonstrate communication channel functionality */
      ChannelDemo( CIFX_DEV);

      /* Demonstrate control/status block functionality */
      BlockDemo( CIFX_DEV);

      /* Demonstrate host/bus state functions */
      StateDemo ( CIFX_DEV);
    }
    /***********************************************************/
    /*** 4. DE-INITIALIZATION                                ***/
    /***********************************************************/
    cifXDriverDeinit();

  } else {
    printf("\nThe driver initialization failed!\n");
    printf("For detailed information refer to the driver's log file (default: /opt/cifx/cifX[x].log) or the previous messages!\n");
    printf("In case of errors relating to the SPI interface, see the previous messages or run demo in 'VERBOSE' mode!\n");
  }
  return 0;
}
