/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: cifxlinux_internal.h $
   Last Modification:
    $Author: sebastiand $
    $Modtime: 4.03.09 12:59 $
    $Revision: 13208 $

   Targets:
     Linux        : yes

   Description:
    Linux driver specific internal structures

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     3        16.02.2009   MT       Added irq_priority settings from device.conf
     2        29.01.2009   MT       Added define CIFX_TOOLKIT_DISABLEPCI to allow
                                    disabling all links to libpciaccess
     1        21.11.2008   MT       initial version

**************************************************************************************/

#ifndef __CIFX_LINUX_INTERNAL__H
#define __CIFX_LINUX_INTERNAL__H

#include "cifxlinux.h"

#ifndef CIFX_TOOLKIT_DISABLEPCI
  #include "pciaccess.h"
#endif

#include <pthread.h>
#include <stdio.h>
#include "cifXToolkit.h"

extern void* g_eth_list_lock;

typedef struct CIFX_DEVICE_INTERNAL_Ttag
{
  struct CIFX_DEVICE_T* userdevice; /*!< Device description passed by user */

  int                   set_irq_prio;           /*!< Flag if custom IRQ priority should be set */
  int                   irq_prio;               /*!< Custom IRQ thread priority  */

  int                   set_irq_scheduler_algo; /*!< Use Custom scheduling algorithm for IRQ thread */
  int                   irq_scheduler_algo;     /*!< Scheduling algorithm to use for IRQ thread (only
                                                     valid if set_irq_scheduler_algo is set) */

  pthread_attr_t        irq_thread_attr;        /*!< Interrupt thread attributes */
  pthread_t             irq_thread;             /*!< Interrupt thread handle     */
  int                   irq_stop;               /*!< flag to signal IRQ handler to stop */

  int                   user_card;        /*!< !=0 if user specified card. This card will not be deleted on exit */
  FILE                  *log_file;        /*!< Handle to logfile if any */


#ifndef CIFX_TOOLKIT_DISABLEPCI
  struct pci_device     pci;              /*!< pci device information if it is a pci device */
#endif

  PDEVICEINSTANCE       devinstance;      /*!< Toolkit device instance */
  int                   eth_support;

} CIFX_DEVICE_INTERNAL_T, *PCIFX_DEVICE_INTERNAL_T;

#ifdef CIFXETHERNET
int USER_GetEthernet(PCIFX_DEVICE_INFORMATION ptDevInfo);
#endif

#endif /* __CIFX_LINUX_INTERNAL__H */
