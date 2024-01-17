/*******************************************************************************
 *
 *   Copyright (c) Hilscher GmbH. All Rights Reserved.
 *
 *******************************************************************************

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
 -------------------------------------------------------------------------------
 3        04.04.2019  SD       - added support for externals control of cs_change
 2        13.07.2015  FM       - Reworked for use of the libsdpm library
 1        01.07.2014  SD       - Initial Version

 ******************************************************************************/

#include <malloc.h>
#include <linux/spi/spidev.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cifxlinux.h"
#include "libsdpm.h"

#define PARSER_BUFFER_SIZE 256

#define DEVICE_NAME       "Device="
#define DEVICE_SPEED      "Speed="
#define DEVICE_MODE       "Mode="
#define DEVICE_CSCHANGE   "CSChange="
#define DEVICE_CHUNK_SIZE "ChunkSize="
#define DEVICE_IRQ        "irq="
#define SPI_DEVICE        "spidev"


static int GetDeviceConfigString(const char* szFile, const char* szKey, char** szValue)
{
  int   ret = 0;
  FILE* fd  = fopen(szFile, "r");

  if(NULL != fd)
  {
    /* File is open */
    char* buffer = malloc(PARSER_BUFFER_SIZE);

    /* Read file line by line */
    while(NULL != fgets(buffer, PARSER_BUFFER_SIZE, fd))
    {
      char* key;

      /* '#' marks a comment line in the device.conf file */
      if(buffer[0] == '#')
        continue;

      /* Search for key in the input buffer */
      key = strcasestr(buffer, szKey);

      if(NULL != key)
      {
        /* We've found the key */
        int   allocsize  = strlen(key + strlen(szKey)) + 1;
        int   valuelen;
        char* tempstring = (char*)malloc(allocsize);

        strcpy(tempstring, key + strlen(szKey));
        valuelen = strlen(tempstring);

        /* strip all trailing whitespaces */
        while( (tempstring[valuelen - 1] == '\n') ||
               (tempstring[valuelen - 1] == '\r') ||
               (tempstring[valuelen - 1] == ' '))
        {
          tempstring[valuelen - 1] = '\0';
          --valuelen;
        }

        *szValue = tempstring;
        ret = 1;
        break;
      }
    }

    free(buffer);
    fclose(fd);
  }

  return ret;
}

int GetDeviceName(char** name, char* szFile)
{
  return GetDeviceConfigString( szFile, DEVICE_NAME, name);
}

int GetSPISpeed(uint32_t* speed, char* szFile)
{
  char* szspeed = NULL;
  int ret = 0;

  if (GetDeviceConfigString( szFile, DEVICE_SPEED, &szspeed)) {
    sscanf(szspeed,"%d",speed);
    free(szspeed);
    ret = 1;
  }
  return ret;
}

int GetSPIMode(int* mode, char* szFile)
{
  char* szmode = NULL;
  int   ret    = 0;
  if (GetDeviceConfigString( szFile, DEVICE_MODE, &szmode)) {
    sscanf(szmode, "%d", mode);
    free(szmode);
    ret = 1;
  }
  return ret;
}

int GetSPICSChange(uint8_t* pbCSChange, char* szFile)
{
  char* string = NULL;
  int   ret    = 0;
  int   number = 0;

  if (pbCSChange == NULL)
    return 0;

  *pbCSChange = 0;
  ret = 1;
  if (GetDeviceConfigString( szFile, DEVICE_CSCHANGE, &string)) {
    sscanf(string, "%d", &number);
    if (number != 0)
      *pbCSChange = 1;
    free(string);
  }
  return ret;
}


int GetIRQFile(char* irq, char* szFile)
{
  int   ret    = 0;
  char* szirq  = NULL;
  if (GetDeviceConfigString( szFile, DEVICE_IRQ, &szirq)) {
    sprintf(irq, "%s", szirq);
    free(szirq);
    ret = 1;
  }
  return ret;
}

int CheckIsSPIDevice(char* szFile)
{
  char* name = NULL;
  int   ret  = 0;
  if (GetDeviceConfigString( szFile, DEVICE_NAME, &name)) {
    if (0 == strncmp(name, SPI_DEVICE, strlen(SPI_DEVICE))) {
      ret = 1;
    }
    free(name);
  }
  return ret;
}

int GetSPIChunkSize(uint32_t* size, char* szFile)
{
  char* szsize = NULL;
  int   ret    = 0;
  *size = 0;
  if (GetDeviceConfigString( szFile, DEVICE_CHUNK_SIZE, &szsize)) {
    sscanf(szsize, "%d", size);
    free(szsize);
    ret = 1;
  }
  return ret;
}

int file_exist (char *filename)
{
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}

void find_config_path(char* base_path)
{
  if (file_exist ("./config0"))
    sprintf(base_path,"./");
  else
    sprintf(base_path,"/opt/cifx/plugins/netx-spm/");
}

uint32_t cifx_device_count(void)
{
  char config[256] = {0};
  char base_path[256] = {0};
  uint32_t spi_dev = 0;

  /* read all configs or parser error or device is not a spidev device */
  find_config_path(base_path);
  sprintf(config, "%sconfig0", base_path);
  while ((file_exist(config)) && (CheckIsSPIDevice(config))) {
    sprintf(config, "%sconfig%d",base_path,++spi_dev);
  }
  return spi_dev;
}

struct CIFX_DEVICE_T* cifx_alloc_device(uint32_t num)
{
  char*    devicename     = NULL;
  char     dev[254]       = {0};
  char     config[254]    = {0};
  char     base_path[254] = {0};
  uint32_t speed          = 0;
  int      mode           = 0;
  uint32_t size           = 0;
  char     irq_file[254]  = {0};
  char*    irq            = NULL;
  uint8_t  cs_change      = 0;


  find_config_path(base_path);
  sprintf(config, "%sconfig%d", base_path, num);
  if (0 == CheckIsSPIDevice(config)) {
    return NULL;
  } else if (0 == GetDeviceName( &devicename, config)) {
    return NULL;
  } else if (0 == GetSPISpeed( &speed, config)) {
    free(devicename);
    return NULL;
  } else if (0 == GetSPIMode( &mode, config)) {
    free(devicename);
    return NULL;
  } else {
    if (0 != GetIRQFile( irq_file, config))
      irq = irq_file;

    GetSPICSChange( &cs_change, config);
    GetSPIChunkSize( &size, config);
    sprintf(dev, "/dev/%s",devicename);
    free(devicename);
    return SDPMInit(dev,   /* device to use */
                    mode,  /* SPI mode */
                    8,     /* number of bits */
                    speed, /* frequency */
                    irq,   /* interrupt */
                    size,
                    cs_change);
  }
}

void cifx_free_device(struct CIFX_DEVICE_T* device)
{
  SDPMDeInit(device);
}
