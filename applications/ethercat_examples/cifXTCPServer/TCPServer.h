/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   $Id: HilMarshaller.c 13309 2019-11-05 12:03:27Z AlexanderMinor $:

   Targets:
     Linux        : yes

   Description:
    Server handling

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    11.11.2019  deprecated TLR definitions removed
    02.06.2010  initial version

**************************************************************************************/


#ifndef __TCPSERVER__H
#define __TCPSERVER__H

#ifdef __cplusplus
  extern "C" {
#endif


/*****************************************************************************/
/*! Internal UART connector data                                             */
/*****************************************************************************/
typedef struct TCP_CONN_INTERNAL_Ttag
{
uint32_t   ulConnectorIdx;
void*      pvMarshaller;

int        fRunning;

SOCKET     hListen;
pthread_t  hServerThread;

SOCKET        hClient;
pthread_t     hClientThread;
unsigned long ulRxCount;
unsigned long ulTxCount;

} TCP_CONN_INTERNAL_T;



void        TrafficTimer                (void* dwUser);
void        MarshallerTimer             (int iSignal);
int32_t     APIENTRY xSysdeviceOpenWrap (CIFXHANDLE  hDriver, char*   szBoard, CIFXHANDLE* phSysdevice);
int32_t     APIENTRY xSysdeviceOpenWrap (CIFXHANDLE  hDriver, char*   szBoard, CIFXHANDLE* phSysdevice);
int32_t     APIENTRY xChannelOpenWrap   (CIFXHANDLE  hDriver,  char* szBoard, uint32_t ulChannel, CIFXHANDLE* phChannel);
int32_t     APIENTRY xChannelCloseWrap  (CIFXHANDLE  hChannel);
void        MarshallerRequest           (void* pvMarshaller, void* pvUser);
uint32_t    InitMarshaller              (void);
void        DeinitMarshaller            (void);

#ifdef __cplusplus
  }
#endif

#endif /* __TCPSERVER__H */









