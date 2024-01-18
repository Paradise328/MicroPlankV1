/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   $Id: HilMarshaller.c 13309 2019-11-05 12:03:27Z AlexanderMinor $:

   Targets:
     Linux        : yes

   Description:
    TCP/IP connector for Hilscher marshaller package

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    11.11.2019  deprecated TLR definitions removed
    25.05.2009  initial version

**************************************************************************************/


#ifndef __TCPCONNECTOR__H
#define __TCPCONNECTOR__H

#include "OS_Includes.h"
#include "MarshallerInternal.h"
#include "CifXTransport.h"
#include "TCPServer.h"


#ifdef __cplusplus
  extern "C" {
#endif



uint32_t  InitMarshaller   ( void);
void      DeinitMarshaller ( void);
uint32_t  TCPConnectorInit (const HIL_MARSHALLER_CONNECTOR_PARAMS_T* ptParams, void* pvMarshaller);


#ifdef __cplusplus
  }
#endif

#endif /* __TCPCONNECTOR__H */
