/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

  Filename:
    $Workfile: ModuleLoader_Public.h $
  Last Modification:
    $Author: Robert $
    $Modtime: 26.08.08 8:21 $
    $Revision: 2029 $

  Targets:
    Win32/ANSI   : no
    Win32/Unicode: no (define _UNICODE)
    WinCE        : no
    rcX          : yes

  Description:

    Module Loader API (will be part of RX_SYSTEM task)

  Changes:

  Version   Date        Author  Description
  ----------------------------------------------------------------------------------
  2         07.08.2008  sb      reduced query modules to contain file name only

  1         30.06.2007  MGr     Created

**************************************************************************************/
/**
 * @file ModuleLoader_Public.h
 * public definitions to access the rcX Module Loader.
 */
#ifndef __MODLOAD_PUBLIC_H
#define __MODLOAD_PUBLIC_H

/** queue name (RX_SYSTEM queue) */

/** Module Loader handles this many modules at most (TODO: tbd) */
#define MODLOAD_MAX_MODULES       10

/** Maximum file name length (TODO: tbd) */
#define MODLOAD_MAX_FILENAME      32

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
  #pragma PRAGMA_PACK_1(MODULELOADER_PUBLIC)
#endif

#ifdef CIFX_TOOLKIT
  #define __TKITPACKED_PRE   __PACKED_PRE
  #define __TKITPACKED_POST  __PACKED_POST
#else
  #define __TKITPACKED_PRE
  #define __TKITPACKED_POST
#endif


/****************************************************************************************
* Module Loader command codes  */

/* Start of the reserved area from 0x4B00 - 0x4BFF for MODLOAD commands */
#define MODLOAD_PACKET_COMMAND_START                  0x00004B00

/* Get a module list */
#define RCX_MODLOAD_CMD_QUERY_MODULES_REQ             (MODLOAD_PACKET_COMMAND_START + 0)
#define RCX_MODLOAD_CMD_QUERY_MODULES_CNF             (MODLOAD_PACKET_COMMAND_START + 1)

/* Run a module */
#define RCX_MODLOAD_CMD_RUN_MODULE_REQ                (MODLOAD_PACKET_COMMAND_START + 2)
#define RCX_MODLOAD_CMD_RUN_MODULE_CNF                (MODLOAD_PACKET_COMMAND_START + 3)

/* Load module from filesystem */
#define RCX_MODLOAD_CMD_LOAD_MODULE_REQ               (MODLOAD_PACKET_COMMAND_START + 4)
#define RCX_MODLOAD_CMD_LOAD_MODULE_CNF               (MODLOAD_PACKET_COMMAND_START + 5)

/* Load and run module from filesystem */
#define RCX_MODLOAD_CMD_LOAD_AND_RUN_MODULE_REQ       (MODLOAD_PACKET_COMMAND_START + 6)
#define RCX_MODLOAD_CMD_LOAD_AND_RUN_MODULE_CNF       (MODLOAD_PACKET_COMMAND_START + 7)

/* download module (will be done via RX_SYSTEM Download mechanism (Download type FILE_TYPE_MODULE)) */
/* download and run module (will be done via RX_SYSTEM Download mechanism (Download type FILE_TYPE_RUN_MODULE)) */


/****************************************************************************************
* Module Loader structs */

typedef __PACKED_PRE struct RCX_MODULE_VERSION_Ttag
{
  unsigned short                        usMajor;
  unsigned short                        usMinor;
  unsigned short                        usRevision;
  unsigned short                        usBuild;
} __PACKED_POST RCX_MODULE_VERSION_T;

typedef __PACKED_PRE struct RCX_MODULE_NAME_Ttag
{
  unsigned char                         bNameLength;
  unsigned char                         abName[15];
} __PACKED_POST RCX_MODULE_NAME_T;

typedef __PACKED_PRE struct RCX_MODULE_DATE_Ttag
{
  unsigned short                        usYear;
  unsigned char                         bMonth;
  unsigned char                         bDay;
} __PACKED_POST RCX_MODULE_DATE_T;

typedef __PACKED_PRE struct RCX_MODULE_ENTRY_Ttag
{
  RCX_MODULE_NAME_T                     tFileName;                    /* !< file name of module */
} __PACKED_POST RCX_MODULE_ENTRY_T;


#define RCX_MODLOAD_MAX_ENTRIES_PER_PACKET        (1024 / sizeof(RCX_MODULE_ENTRY_T))

/****************************************************************************************
 * Packet: RCX_MODLOAD_CMD_QUERY_MODULES_REQ/RCX_MODLOAD_CMD_QUERY_MODULES_CNF
 *
 * Query modules list
 *
 */

/**** Request Packet ****/

typedef __PACKED_PRE struct RCX_MODLOAD_QUERY_MODULES_REQ_DATA_Ttag
{
  uint32_t                 ulListType;
} __PACKED_POST RCX_MODLOAD_QUERY_MODULES_REQ_DATA_T;

/* List Types */
#define RCX_MODLOAD_QUERY_MODULES_REQ_LIST_TYPE_STORAGE       0x00000000   /* static storage, like CF file system */
#define RCX_MODLOAD_QUERY_MODULES_REQ_LIST_TYPE_LOADED        0x00000001   /* currently loaded modules */
#define RCX_MODLOAD_QUERY_MODULES_REQ_LIST_TYPE_RUNNING       0x00000002   /* currently running modules */


typedef __TKITPACKED_PRE struct RCX_MODLOAD_QUERY_MODULES_REQ_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
  /* packet data */
  RCX_MODLOAD_QUERY_MODULES_REQ_DATA_T          tData;
} __TKITPACKED_POST RCX_MODLOAD_QUERY_MODULES_REQ_T;



/**** Confirmation Packet ****/

typedef __PACKED_PRE struct RCX_MODLOAD_QUERY_MODULES_CNF_DATA_Ttag
{
  RCX_MODULE_ENTRY_T                atModules[RCX_MODLOAD_MAX_ENTRIES_PER_PACKET];
} __PACKED_POST RCX_MODLOAD_QUERY_MODULES_CNF_DATA_T;


typedef __TKITPACKED_PRE struct RCX_MODLOAD_QUERY_MODULES_CNF_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
  /* packet data */
  RCX_MODLOAD_QUERY_MODULES_CNF_DATA_T          tData;
} __TKITPACKED_POST RCX_MODLOAD_QUERY_MODULES_CNF_T;


/****************************************************************************************
 * Packet: RCX_MODLOAD_CMD_RUN_MODULE_REQ/RCX_MODLOAD_CMD_RUN_MODULE_CNF
 *
 * Run a module which has been loaded
 */


/**** Request Packet ****/

typedef __PACKED_PRE struct RCX_MODLOAD_RUN_MODULE_REQ_DATA_Ttag
{
  /* channel parameter for modules supporting multi-instantiation */
  uint32_t                                    ulChannel;
  /* module name follows with NUL termination */
} __PACKED_POST RCX_MODLOAD_RUN_MODULE_REQ_DATA_T;


typedef __TKITPACKED_PRE struct RCX_MODLOAD_RUN_MODULE_REQ_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
  /* packet data */
  RCX_MODLOAD_RUN_MODULE_REQ_DATA_T             tData;
} __TKITPACKED_POST RCX_MODLOAD_RUN_MODULE_REQ_T;



/**** Confirmation Packet ****/

typedef __TKITPACKED_PRE struct RCX_MODLOAD_RUN_MODULE_CNF_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
} __TKITPACKED_POST RCX_MODLOAD_RUN_MODULE_CNF_T;

/****************************************************************************************
 * Packet: RCX_MODLOAD_CMD_LOAD_MODULE_REQ/RCX_MODLOAD_CMD_LOAD_MODULE_CNF
 *
 * Load a module from file system
 */


/**** Request Packet ****/

typedef __PACKED_PRE struct RCX_MODLOAD_LOAD_MODULE_REQ_DATA_Ttag
{
  /* channel parameter for modules supporting multi-instantiation */
  uint32_t                                    ulChannel;
  /* file name (including path) follows with NUL termination */
  /* XXX mgr: dynamic lenght structs are evil - why not use RCX_MODULE_NAME_T? */
} __PACKED_POST RCX_MODLOAD_LOAD_MODULE_REQ_DATA_T;


typedef __TKITPACKED_PRE struct RCX_MODLOAD_LOAD_MODULE_REQ_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
  /* packet data */
  RCX_MODLOAD_LOAD_MODULE_REQ_DATA_T            tData;
} __TKITPACKED_POST RCX_MODLOAD_LOAD_MODULE_REQ_T;



/**** Confirmation Packet ****/

typedef __TKITPACKED_PRE struct RCX_MODLOAD_LOAD_MODULE_CNF_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
} __TKITPACKED_POST RCX_MODLOAD_LOAD_MODULE_CNF_T;


/****************************************************************************************
 * Packet: RCX_MODLOAD_CMD_LOAD_AND_RUN_MODULE_REQ/RCX_MODLOAD_CMD_LOAD_AND_RUN_MODULE_CNF
 *
 * Run a loaded module
 */


/**** Request Packet ****/

typedef __PACKED_PRE struct RCX_MODLOAD_LOAD_AND_RUN_MODULE_REQ_DATA_Ttag
{
  /* channel parameter for modules supporting multi-instantiation.
   * if it doen't, this value has to match the actual channel supported
   * by the module
   */
  uint32_t                                    ulChannel;
  /* file name (including path) follows with NUL termination */
  /* XXX mgr: dynamic lenght structs are evil - why not use RCX_MODULE_NAME_T? */
} __PACKED_POST RCX_MODLOAD_LOAD_AND_RUN_MODULE_REQ_DATA_T;


typedef __TKITPACKED_PRE struct RCX_MODLOAD_LOAD_AND_RUN_MODULE_REQ_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
  /* packet data */
  RCX_MODLOAD_LOAD_AND_RUN_MODULE_REQ_DATA_T    tData;
} __TKITPACKED_POST RCX_MODLOAD_LOAD_AND_RUN_MODULE_REQ_T;



/**** Confirmation Packet ****/

typedef __TKITPACKED_PRE struct RCX_MODLOAD_LOAD_AND_RUN_MODULE_CNF_Ttag
{
  /* packet header */
  HIL_PACKET_HEADER_T                           tHead;
} __TKITPACKED_POST RCX_MODLOAD_LOAD_AND_RUN_MODULE_CNF_T;


/*************************************************************************************/

/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(MODULELOADER_PUBLIC)
#endif

#undef __TKITPACKED_PRE
#undef __TKITPACKED_POST

#endif /* #ifndef __MODLOAD_PUBLIC_H */
