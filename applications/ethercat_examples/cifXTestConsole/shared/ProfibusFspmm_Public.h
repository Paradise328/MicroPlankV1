/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: ProfibusFspmm_Public.h $
   Last Modification:
    @version $Id: ProfibusFspmm_Public.h 2002 2010-08-18 15:13:54Z Robert $
   
   Targets:
    Win32/ANSI   : no
    Win32/Unicode: no (define _UNICODE)
    WinCE        : yes
    rcX          : yes
 
   Description:
     
    Profibus Fspmm
    public
   
   Changes:
 
     Version    Date        Author
        Description
     ----------------------------------------------------------------------------------
       19   03.04.09      MK
          - Add. PROFIBUS_FSPM_ACT_MAX_EXT_DIAG_LEN
       18    05.11.08     MK
          - Add. PROFIBUS_FSPMM_SET_OUTPUT_CNF_SIZE
       17    11.10.08     MK
          - Add command PROFIBUS_FSPMM_CMD_SLAVE_ACTIVATE_REQ to activate or deactivate
            a slave during runtime
          - New Data Status FLG_INPUT_DATA_STATUS_CLR & FLG_INPUT_DATA_STATUS_CLR 
            for Input data quality       
       13     11.09.07    MK
          - Wrong definition of PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_SIZE
          - Add. History list
       12     10.07.07    MK
          - Add defines PROFIBUS_FSPMM_CMD_RANGE_START / ... RANGE_END
       11     20.06.07    MK
          - Add some task internal commands   
       10     30.11.06    MK
          - Inital Version
        
**************************************************************************************/
#ifndef __PROFIBUS_FSPMM_PUBLIC_H
#define __PROFIBUS_FSPMM_PUBLIC_H

#include "ProfibusDl_BusParameter.h" /* Include the Bus Parameter definition */
#include "ProfibusFspm_Common.h" 
/***************************************************************************************/
/* Profibus Fspmm public, constants */

#define PROFIBUS_FSPMM_PROCESS_QUEUE_NAME       "FSPMM_QUE"


/***************************************************************************************/
/* Profibus Fspmm public commands */

#define  PROFIBUS_FSPMM_CMD_RANGE_START                0x00002200
                                                      
#define  PROFIBUS_FSPMM_CMD_INIT_REQ                   0x00002200
#define  PROFIBUS_FSPMM_CMD_INIT_CNF                   0x00002201
                                                      
#define  PROFIBUS_FSPMM_CMD_RESET_REQ                  0x00002202
#define  PROFIBUS_FSPMM_CMD_RESET_CNF                  0x00002203
                                                      
#define  PROFIBUS_FSPMM_CMD_ABORT_REQ                  0x00002204
#define  PROFIBUS_FSPMM_CMD_ABORT_CNF                  0x00002205
                                                      
#define  PROFIBUS_FSPMM_CMD_SET_MODE_REQ               0x00002206
#define  PROFIBUS_FSPMM_CMD_SET_MODE_CNF               0x00002207
                                                      
#define  PROFIBUS_FSPMM_CMD_GET_SLAVE_DIAG_REQ         0x0000220A
#define  PROFIBUS_FSPMM_CMD_GET_SLAVE_DIAG_CNF         0x0000220B
                                                      
#define  PROFIBUS_FSPMM_CMD_SET_OUTPUT_REQ             0x0000220C
#define  PROFIBUS_FSPMM_CMD_SET_OUTPUT_CNF             0x0000220D
                                                      
#define  PROFIBUS_FSPMM_CMD_GET_INPUT_REQ              0x0000220E
#define  PROFIBUS_FSPMM_CMD_GET_INPUT_CNF              0x0000220F
                                                      
#define  PROFIBUS_FSPMM_CMD_READ_REQ                   0x00002210
#define  PROFIBUS_FSPMM_CMD_READ_CNF                   0x00002211
                                                      
#define  PROFIBUS_FSPMM_CMD_WRITE_REQ                  0x00002212
#define  PROFIBUS_FSPMM_CMD_WRITE_CNF                  0x00002213
                                                      
#define  PROFIBUS_FSPMM_CMD_MODE_CHANGE_IND            0x00002214
#define  PROFIBUS_FSPMM_CMD_MODE_CHANGE_INDRET         0x00002215
                                                      
#define  PROFIBUS_FSPMM_CMD_NEW_SLAVE_DIAG_IND         0x00002216
#define  PROFIBUS_FSPMM_CMD_NEW_SLAVE_DIAG_INDRET      0x00002217
                                                      
#define  PROFIBUS_FSPMM_CMD_NEW_INPUT_IND              0x00002218
#define  PROFIBUS_FSPMM_CMD_NEW_INPUT_INDRET           0x00002219
                                                      
#define  PROFIBUS_FSPMM_CMD_ALARM_NOTIFICATION_IND     0x0000221A
#define  PROFIBUS_FSPMM_CMD_ALARM_NOTIFICATION_INDRET  0x0000221B
                                                      
#define  PROFIBUS_FSPMM_CMD_ALARM_ACK_REQ              0x0000221C
#define  PROFIBUS_FSPMM_CMD_ALARM_ACK_CNF              0x0000221D
                                                      
#define  PROFIBUS_FSPMM_CMD_DOWNLOAD_REQ               0x0000221E
#define  PROFIBUS_FSPMM_CMD_DOWNLOAD_CNF               0x0000221F
                                                      
#define  PROFIBUS_FSPMM_CMD_GLOBALCONTROL_REQ          0x00002220
#define  PROFIBUS_FSPMM_CMD_GLOBALCONTROL_CNF          0x00002221
                                                      
#define  PROFIBUS_FSPMM_CMD_FAULT_IND                  0x00002222
#define  PROFIBUS_FSPMM_CMD_FAULT_INDRET               0x00002223

#define  PROFIBUS_FSPMM_CMD_APP_REG_REQ                0x00002224
#define  PROFIBUS_FSPMM_CMD_APP_REG_CNF                0x00002225

#define  PROFIBUS_FSPMM_CMD_SLAVE_ACTIVATE_REQ         0x00002226
#define  PROFIBUS_FSPMM_CMD_SLAVE_ACTIVATE_CNF         0x00002227

#define  PROFIBUS_FSPMM_CMD_RANGE_END                  0x00002227 


/* Task internal commands */                                                      
#define  PROFIBUS_FSPMM_CMD_MMAC1_T1_EXPIRED_IND       0x000022F4
#define  PROFIBUS_FSPMM_CMD_MMAC1_T1_EXPIRED_XXX       0x000022F5
#define  PROFIBUS_FSPMM_CMD_MMAC1_T2_EXPIRED_IND       0x000022F6
#define  PROFIBUS_FSPMM_CMD_MMAC1_T2_EXPIRED_XXX       0x000022F7
#define  PROFIBUS_FSPMM_CMD_C1_TIM_EXPIRED_IND         0x000022F8         
#define  PROFIBUS_FSPMM_CMD_C1_TIM_EXPIRED_XXX         0x000022F9
#define  PROFIBUS_FSPMM_CMD_GLBL_CNTRL_TIM_IND         0x000022FA
#define  PROFIBUS_FSPMM_CMD_GLBL_CNTRL_TIM_XXX         0x000022FB
#define  PROFIBUS_FSPMM_CMD_MIN_SL_TIM_IND             0x000022FC
#define  PROFIBUS_FSPMM_CMD_MIN_SL_TIM_XXX             0x000022FD

/* Operation mode of the stack */
#define PROFIBUS_FSPMM_MODE_BUFFERED      0
#define PROFIBUS_FSPMM_MODE_BUSSYNCHRON   1

/* general master states */
#define USIF_OFFLINE 0x00
#define USIF_STOP    0x40
#define USIF_CLEAR   0x80
#define USIF_OPERATE 0xC0

/* slave flags */
#define SL_FLAGS_RESERVED         0x01
#define SL_FLAGS_EXTRA_ALARM_SAP  0x02
#define SL_FLAGS_DPV1_DATA_TYPES  0x04
#define SL_FLAGS_DPV1_SUPPORTED   0x08
#define SL_FLAGS_PUBLISHER_ENABLE 0x10
#define SL_FLAGS_FAIL_SAFE        0x20
#define SL_FLAGS_NEW_PRM          0x40
#define SL_FLAGS_ACTIVE           0x80

/* Alarm Types */
#define MSAL1M_UPDATA_ALARM                 0x04  
#define MSAL1M_STATUS_ALARM                 0x08
#define MSAL1M_MANUFACTURER_SPECIFIC_ALARM  0x10
#define MSAL1M_DIAGNOSTIC_ALARM             0x20
#define MSAL1M_PROCESS_ALARM                0x40
#define MSAL1M_PULL_PLUG_ALARM              0x80

#define PROFIBUS_FSPMM_GLB_CLEAR    0x02
#define PROFIBUS_FSPMM_GLB_UNFREEZE 0x04
#define PROFIBUS_FSPMM_GLB_FREEZE   0x08
#define PROFIBUS_FSPMM_GLB_UNSYNC   0x10
#define PROFIBUS_FSPMM_GLB_SYNC     0x20


#define PROFIBUS_FSPM_ACT_MAX_DIAG_LEN      244 /* Maximum length of diag data */
#define PROFIBUS_FSPM_ACT_MAX_EXT_DIAG_LEN  238 /* Maximum length of extended diag data */
#define PROFIBUS_FSPM_MAX_IO_DATA_LEN       244 /* Maximum length of diag data */
#define PROFIBUS_FSPM_MAX_PCKT_LEN         2024 /* Maximum length of data for download data */ 

#define FSPMM_MAX_ALARM_LEN (64-4)


/* Common bus structures */
typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_DL_SL_PRM_DATA_Ttag {
  TLR_UINT16  usPrmLen;
  __PACKED_PRE struct __PACKED_POST
  {
      TLR_UINT8 bReserved         : 3;
      TLR_UINT8 bWD_On            : 1;
      TLR_UINT8 bFreeze_Req       : 1;
      TLR_UINT8 bSync_Req         : 1;
      TLR_UINT8 bUnlock_Req       : 1;
      TLR_UINT8 bLock_Req         : 1;
  } bStation_Status;
  
  TLR_UINT8 bWD_Fact_1;
  TLR_UINT8 bWD_Fact_2;
  TLR_UINT8 bMinTsdr;
  TLR_UINT16 usIdent_Number;
  __PACKED_PRE struct __PACKED_POST
  {
      TLR_UINT8 bGroup_1          : 1;
      TLR_UINT8 bGroup_2          : 1;
      TLR_UINT8 bGroup_3          : 1;
      TLR_UINT8 bGroup_4          : 1;
      TLR_UINT8 bGroup_5          : 1;
      TLR_UINT8 bGroup_6          : 1;
      TLR_UINT8 bGroup_7          : 1;
      TLR_UINT8 bGroup_8          : 1;
  } bGroup_Ident; 

  __PACKED_PRE union __PACKED_POST
  {
    PROFIBUS_FSPM_PARAMETER_DPV0_PRM_T tDpv0;     /* standard parameter , or */
    PROFIBUS_FSPM_PARAMETER_DPV1_PRM_T tDpv1Only; /* parameter data with DPV1 */
    PROFIBUS_FSPM_PARAMETER_DPV2_PRM_T tDpv1Dpv2; /* parameter data with DPV1,DPV2 */
  }un;

} PROFIBUS_DL_SL_PRM_DATA_T;

typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_DL_SL_PARA_SET_Ttag {
  TLR_UINT16  usSlave_Para_Len;
  __PACKED_PRE struct __PACKED_POST
  {
      TLR_UINT8 bReserved1        : 1;
      TLR_UINT8 bExtraAlarmSAP    : 1;
      TLR_UINT8 bDPV1DataTypes    : 1;
      TLR_UINT8 bDPV1Supported    : 1;
      TLR_UINT8 bPublisherEnable  : 1;
      TLR_UINT8 bFailSafe         : 1;
      TLR_UINT8 bNewPrm           : 1;
      TLR_UINT8 bActive           : 1;
  } bSlFlag;
  
  TLR_UINT8   bSlaveType;
  TLR_UINT8   bMaxDiagDataLen;
  TLR_UINT8   bMaxAlarmLen;
  TLR_UINT8   bMaxChannelDateLen;
  TLR_UINT8   bDiagUpdDelay;
  TLR_UINT8   bAlarmMode;
  __PACKED_PRE struct __PACKED_POST
  {
      TLR_UINT8 bNA_To_Abort      : 1;
      TLR_UINT8 bIgnore_AClr      : 1;
      TLR_UINT8 bReserved         : 6;
  } bAddSlFlag;
  TLR_UINT16  usC1Timeout;
  TLR_UINT8   bReserved[4];
  PROFIBUS_DL_SL_PRM_DATA_T tPrmData;
} PROFIBUS_DL_SL_PARA_SET_T;

typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPMM_DIAGNOSTIC_DATA_Ttag {
   
#define MSK_STATION_STATUS_1_NON_EXIST        0x01
#define MSK_STATION_STATUS_1_NOT_READY        0x02
#define MSK_STATION_STATUS_1_CFG_FAULT        0x04
#define MSK_STATION_STATUS_1_EXT_DIAG         0x08
#define MSK_STATION_STATUS_1_NOT_SUPPORTED    0x10
#define MSK_STATION_STATUS_1_INVALID_RESPONSE 0x20
#define MSK_STATION_STATUS_1_PRM_FAULT        0x40
#define MSK_STATION_STATUS_1_MASTER_LOCK      0x80

   __PACKED_PRE struct __PACKED_POST
   {
    TLR_UINT8 bStation_Non_Existent : 1; /* no response */
    TLR_UINT8 bStation_Not_Ready    : 1; /* station not ready */
    TLR_UINT8 bCfg_Fault            : 1; /* configuration fault */
    TLR_UINT8 bExt_Diag             : 1; /* extended diagnostic */

    TLR_UINT8 bNot_Supported        : 1; /* sync, freeze not supported */
    TLR_UINT8 bInvalid_Response     : 1; /* response faulty */
    TLR_UINT8 bPrm_Fault            : 1; /* parameters faulty */
    TLR_UINT8 bMaster_Lock          : 1; /* locked by a master */
  }  Stationstatus_1;

#define MSK_STATION_STATUS_2_PRM_REQ          0x01
#define MSK_STATION_STATUS_2_STAT_DIAG        0x02
#define MSK_STATION_STATUS_2_SLAVE_DEVICE     0x04
#define MSK_STATION_STATUS_2_WATCHDOG_ON      0x08
#define MSK_STATION_STATUS_2_FREEZE_MODE      0x10
#define MSK_STATION_STATUS_2_SYNC_MODE        0x20
#define MSK_STATION_STATUS_2_DEACTIVTED       0x80

  __PACKED_PRE struct __PACKED_POST 
  {
    TLR_UINT8 bPrm_Req              : 1; /* request new parameters */
    TLR_UINT8 bStat_Diag            : 1; /* static diagnostic */
    TLR_UINT8 bTrue                 : 1; /* set to 1 by a slave */
    TLR_UINT8 bWd_On                : 1; /* watchdog function on/off */

    TLR_UINT8 bFreeze_Mode          : 1; /* freeze mode active */
    TLR_UINT8 bSync_Mode            : 1; /* sync mode active */
    TLR_UINT8 bReserved             : 1; /* reserved */
    TLR_UINT8 bDeactivated          : 1; /* slave deactivated */
  }  Stationstatus_2;

#define MSK_STATION_STATUS_3_EXT_DIAG_OVERFLOW  0x80

  __PACKED_PRE struct __PACKED_POST 
  {
    TLR_UINT8 bReserved             : 7;
    TLR_UINT8 bExt_Diag_Overflow    : 1; /* ext. diagnostic overflow */
  }  Stationstatus_3;

  TLR_UINT8 bMaster_Add;               /* corresponding master address */
  TLR_UINT16 usIdent_Number;           /* ident number, motorola format */
  TLR_UINT8 abExt_Diag_Data[PROFIBUS_FSPM_ACT_MAX_EXT_DIAG_LEN];
                                       /* extended diagnostic field */
} PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T;


typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPMM_DATA_HEADER_Ttag {
  TLR_UINT16  usDataLen;
  TLR_UINT8   abData[PROFIBUS_FSPM_MAX_IO_DATA_LEN];
} PROFIBUS_FSPMM_DATA_HEADER_T;


/***************************************************************************************/
/* Profibus Fspmm public, packets */

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_PACK_1(PROFIBUS_FSPMM_PUBLIC)
#endif


/* packet data */
typedef struct PROFIBUS_FSPMM_PACKET_APP_REG_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_APP_REG_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_APP_REG_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_APP_REG_CNF_T;


typedef struct PROFIBUS_FSPMM_INIT_REQ_Ttag
{
  PROFIBUS_DL_BUS_PARAMETER_SET_T tBusParameter;
}PROFIBUS_FSPMM_INIT_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_INIT_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_INIT_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_INIT_REQ_T;

#define PROFIBUS_FSPMM_INIT_REQ_SIZE (sizeof(PROFIBUS_FSPMM_INIT_REQ_T) - sizeof(PROFIBUS_DL_BUS_PARAMETER_SET_T))

typedef struct PROFIBUS_FSPMM_PACKET_INIT_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_INIT_CNF_T;

#define DOWNLOAD_REMOTE_ADDRESS_LOCAL_MASTER   (0x000000FF)
typedef struct PROFIBUS_FSPMM_DOWNLOAD_REQ_Ttag
{
  TLR_UINT32 ulRemoteAddress;
  TLR_UINT32 ulAreaCode;
  TLR_UINT8  abData[PROFIBUS_FSPM_MAX_PCKT_LEN];
}PROFIBUS_FSPMM_DOWNLOAD_REQ_T;

typedef struct PROFIBUS_FSPMM_DOWNLOAD_CNF_Ttag
{
  TLR_UINT32 ulRemoteAddress;
  TLR_UINT32 ulAreaCode;
}PROFIBUS_FSPMM_DOWNLOAD_CNF_T;

typedef struct PROFIBUS_FSPMM_PACKET_DOWNLOAD_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_DOWNLOAD_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_DOWNLOAD_REQ_T;

#define PROFIBUS_FSPMM_DOWNLOAD_REQ_SIZE (sizeof(PROFIBUS_FSPMM_DOWNLOAD_REQ_T) - PROFIBUS_FSPM_MAX_PCKT_LEN)

typedef struct PROFIBUS_FSPMM_PACKET_DOWNLOAD_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_DOWNLOAD_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_DOWNLOAD_CNF_T;



typedef struct PROFIBUS_FSPMM_SET_MODE_REQ_Ttag
{
  TLR_UINT32 ulBusMode;
}PROFIBUS_FSPMM_SET_MODE_REQ_T;

#define PROFIBUS_FSPMM_SET_MODE_REQ_SIZE sizeof(PROFIBUS_FSPMM_SET_MODE_REQ_T)

typedef struct PROFIBUS_FSPMM_PACKET_SET_MODE_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SET_MODE_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_SET_MODE_REQ_T;

typedef struct PROFIBUS_FSPMM_SET_MODE_CNF_Ttag
{
  TLR_UINT32 ulBusMode;
}PROFIBUS_FSPMM_SET_MODE_CNF_T;

#define PROFIBUS_FSPMM_SET_MODE_CNF_SIZE sizeof(PROFIBUS_FSPMM_SET_MODE_CNF_T)

typedef struct PROFIBUS_FSPMM_PACKET_SET_MODE_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SET_MODE_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_SET_MODE_CNF_T;


typedef struct PROFIBUS_FSPMM_MODE_CHANGE_IND_Ttag
{
  TLR_UINT32 ulBusMode;
}PROFIBUS_FSPMM_MODE_CHANGE_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_MODE_CHANGE_IND_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_MODE_CHANGE_IND_T tData;
}PROFIBUS_FSPMM_PACKET_MODE_CHANGE_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_MODE_CHANGE_INDRET_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_MODE_CHANGE_INDRET_T;

typedef struct PROFIBUS_FSPMM_NEW_SLAVE_DIAG_IND_Ttag
{
  TLR_UINT32 ulRemAdd;
}PROFIBUS_FSPMM_NEW_SLAVE_DIAG_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_NEW_SLAVE_DIAG_IND_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_NEW_SLAVE_DIAG_IND_T tData;
}PROFIBUS_FSPMM_PACKET_NEW_SLAVE_DIAG_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_NEW_SLAVE_DIAG_INDRET_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_NEW_SLAVE_DIAG_INDRET_T;


typedef struct PROFIBUS_FSPMM_NEW_INPUT_IND_Ttag
{
  TLR_UINT32 ulRemAdd;
}PROFIBUS_FSPMM_NEW_INPUT_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_NEW_INPUT_IND_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_NEW_INPUT_IND_T tData;
}PROFIBUS_FSPMM_PACKET_NEW_INPUT_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_NEW_INPUT_INDRET_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_NEW_INPUT_INDRET_T;


typedef struct PROFIBUS_FSPMM_SET_OUTPUT_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT8  abData[PROFIBUS_FSPM_MAX_IO_DATA_LEN];
}PROFIBUS_FSPMM_SET_OUTPUT_REQ_T;

#define PROFIBUS_FSPMM_SET_OUTPUT_REQ_SIZE (sizeof(PROFIBUS_FSPMM_SET_OUTPUT_REQ_T) - PROFIBUS_FSPM_MAX_IO_DATA_LEN)

typedef struct PROFIBUS_FSPMM_PACKET_SET_OUTPUT_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SET_OUTPUT_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_SET_OUTPUT_REQ_T;

typedef struct PROFIBUS_FSPMM_SET_OUTPUT_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
}PROFIBUS_FSPMM_SET_OUTPUT_CNF_T;

#define PROFIBUS_FSPMM_SET_OUTPUT_CNF_SIZE (sizeof(PROFIBUS_FSPMM_SET_OUTPUT_CNF_T))

typedef struct PROFIBUS_FSPMM_PACKET_SET_OUTPUT_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SET_OUTPUT_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_SET_OUTPUT_CNF_T;

typedef struct PROFIBUS_FSPMM_GET_INTPUT_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
}PROFIBUS_FSPMM_GET_INPUT_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_GET_INPUT_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_GET_INPUT_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_GET_INPUT_REQ_T;


#define MSK_INPUT_DATA_STATUS        (0x03000000)
#define FLG_INPUT_DATA_STATUS_CLR    (0x01000000)
#define FLG_INPUT_DATA_STATUS_NIL    (0x02000000)

typedef struct PROFIBUS_FSPMM_GET_INTPUT_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSlaveDiag;
  TLR_UINT8  abData[PROFIBUS_FSPM_MAX_IO_DATA_LEN];
}PROFIBUS_FSPMM_GET_INPUT_CNF_T;

#define PROFIBUS_FSPMM_GET_INPUT_REQ_SIZE (sizeof(PROFIBUS_FSPMM_GET_INPUT_REQ_T))
#define PROFIBUS_FSPMM_GET_INPUT_CNF_SIZE (sizeof(PROFIBUS_FSPMM_GET_INPUT_CNF_T) - PROFIBUS_FSPM_MAX_IO_DATA_LEN )


typedef struct PROFIBUS_FSPMM_PACKET_GET_INPUT_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_GET_INPUT_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_GET_INPUT_CNF_T;

typedef struct PROFIBUS_FSPMM_READ_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSlot;
  TLR_UINT32 ulIndex;
  TLR_UINT32 ulLength;
}PROFIBUS_FSPMM_READ_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_READ_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_READ_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_READ_REQ_T;

typedef struct PROFIBUS_FSPMM_READ_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSlot;
  TLR_UINT32 ulIndex;
  TLR_UINT8  abData[PROFIBUS_FSPM_MAX_IO_DATA_LEN];
}PROFIBUS_FSPMM_READ_CNF_T;

typedef struct PROFIBUS_FSPMM_PACKET_READ_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_READ_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_READ_CNF_T;

#define PROFIBUS_FSPMM_READ_REQ_SIZE (sizeof(PROFIBUS_FSPMM_READ_REQ_T))
#define PROFIBUS_FSPMM_READ_CNF_SIZE (sizeof(PROFIBUS_FSPMM_READ_CNF_T) - PROFIBUS_FSPM_MAX_IO_DATA_LEN )

typedef struct PROFIBUS_FSPMM_WRITE_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSlot;
  TLR_UINT32 ulIndex;
  TLR_UINT8  abData[PROFIBUS_FSPM_MAX_IO_DATA_LEN];
}PROFIBUS_FSPMM_WRITE_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_WRITE_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_WRITE_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_WRITE_REQ_T;

typedef struct PROFIBUS_FSPMM_WRITE_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSlot;
  TLR_UINT32 ulIndex;
}PROFIBUS_FSPMM_WRITE_CNF_T;

typedef struct PROFIBUS_FSPMM_PACKET_WRITE_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_WRITE_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_WRITE_CNF_T;

#define PROFIBUS_FSPMM_WRITE_REQ_SIZE (sizeof(PROFIBUS_FSPMM_WRITE_REQ_T) - PROFIBUS_FSPM_MAX_IO_DATA_LEN)
#define PROFIBUS_FSPMM_WRITE_CNF_SIZE (sizeof(PROFIBUS_FSPMM_WRITE_CNF_T))


#define PROFIBUS_FSPMM_GET_DIAG_FLAG_PEEK   0x00000001L 
#define PROFIBUS_FSPMM_GET_DIAG_FLAG_QUERY  0x00000002L 
typedef struct PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulFlags;
}PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_REQ_T;

#define PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_SIZE (sizeof(PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_T))
typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
  PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T  tDiagnostic;
}PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T;

#define PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_SIZE (sizeof(PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T) - sizeof(PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T) )

typedef struct PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_CNF_T;


typedef struct PROFIBUS_FSPMM_ALARM_NOTIFICATION_IND_Ttag
{
  TLR_UINT32 ulRem_Adr;
  TLR_UINT32 ulAlarm_Type;
  TLR_UINT32 ulSlot_Number;
  TLR_UINT32 ulSpecifier;
  TLR_UINT8 abDiagnostic_User_Data[FSPMM_MAX_ALARM_LEN];
}PROFIBUS_FSPMM_ALARM_NOTIFICATION_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_ALARM_NOTIFICATIONG_IND_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_ALARM_NOTIFICATION_IND_T tData;
}PROFIBUS_FSPMM_PACKET_ALARM_NOTIFICATION_IND_T;

#define PROFIBUS_FSPMM_ALARM_NOTIFICATION_IND_SIZE (sizeof(PROFIBUS_FSPMM_ALARM_NOTIFICATION_IND_T)-FSPMM_MAX_ALARM_LEN)

typedef struct PROFIBUS_FSPMM_PACKET_ALARM_NOTIFICATION_INDRET_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_ALARM_NOTIFICATION_INDRET_T;


typedef struct PROFIBUS_FSPMM_ALARM_ACK_REQ_Ttag
{
  TLR_UINT32 ulRem_Adr;
  TLR_UINT32 ulAlarm_Type;            
  TLR_UINT32 ulSlot_Number;
  TLR_UINT32 ulSpecifier;
  TLR_UINT32 ulFunction;
}PROFIBUS_FSPMM_ALARM_ACK_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_ALARM_ACK_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_ALARM_ACK_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_ALARM_ACK_REQ_T;

#define PROFIBUS_FSPMM_ALARM_ACK_REQ_SIZE (sizeof(PROFIBUS_FSPMM_ALARM_ACK_REQ_T))

typedef struct PROFIBUS_FSPMM_ALARM_ACK_CNF_Ttag
{
  TLR_UINT32 ulRem_Adr;
  TLR_UINT32 ulAlarm_Type;
  TLR_UINT32 ulSlot_Number;
  TLR_UINT32 ulSpecifier;
  TLR_UINT32 ulFunction;
}PROFIBUS_FSPMM_ALARM_ACK_CNF_T;

typedef struct PROFIBUS_FSPMM_PACKET_ALARM_ACK_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_ALARM_ACK_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_ALARM_ACK_CNF_T;

#define PROFIBUS_FSPMM_ALARM_ACK_CNF_SIZE (sizeof(PROFIBUS_FSPMM_ALARM_ACK_CNF_T))



typedef struct PROFIBUS_FSPMM_GLOBALCONTROL_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulSyncCommand;
  TLR_UINT32 ulFreezeCommand;
  TLR_UINT32 ulGroupSelect;
}PROFIBUS_FSPMM_GLOBALCONTROL_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_GLOBALCONTROL_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_GLOBALCONTROL_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_GLOBALCONTROL_REQ_T;

#define PROFIBUS_FSPMM_GLOBALCONTROL_REQ_SIZE sizeof(PROFIBUS_FSPMM_GLOBALCONTROL_REQ_T)


typedef struct PROFIBUS_FSPMM_PACKET_GLOBALCONTROL_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_GLOBALCONTROL_CNF_T;

#define PROFIBUS_FSPMM_GLOBALCONTROL_CNF_SIZE 0


typedef struct PROFIBUS_FSPMM_FAULT_IND_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulFault;
}PROFIBUS_FSPMM_FAULT_IND_T;

typedef struct PROFIBUS_FSPMM_PACKET_FAULT_IND_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_FAULT_IND_T tData;
}PROFIBUS_FSPMM_PACKET_FAULT_IND_T;

#define PROFIBUS_FSPMM_FAULT_IND_SIZE (sizeof(PROFIBUS_FSPMM_FAULT_IND_T))

typedef struct PROFIBUS_FSPMM_PACKET_FAULT_INDRET_Ttag
{
  TLR_PACKET_HEADER_T tHead;
}PROFIBUS_FSPMM_PACKET_FAULT_INDRET_T;

#define PROFIBUS_FSPMM_FAULT_INDRET_SIZE 0


#define PROFIBUS_FSPMM_SLAVE_ACTIVATE    (0x00000001)
#define PROFIBUS_FSPMM_SLAVE_DEACTIVATE  (0x00000002)

typedef struct PROFIBUS_FSPMM_SLAVE_ACTIVATE_REQ_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulActivate;
}PROFIBUS_FSPMM_SLAVE_ACTIVATE_REQ_T;

typedef struct PROFIBUS_FSPMM_PACKET_SLAVE_ACTIVATE_REQ_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SLAVE_ACTIVATE_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_SLAVE_ACTIVATE_REQ_T;


typedef struct PROFIBUS_FSPMM_SLAVE_ACTIVATE_CNF_Ttag
{
  TLR_UINT32 ulRemAdd;
  TLR_UINT32 ulActivate;
}PROFIBUS_FSPMM_SLAVE_ACTIVATE_CNF_T;

typedef struct PROFIBUS_FSPMM_PACKET_SLAVE_ACTIVATE_CNF_Ttag
{
  TLR_PACKET_HEADER_T tHead;
  PROFIBUS_FSPMM_SLAVE_ACTIVATE_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_SLAVE_ACTIVATE_CNF_T;

#define PROFIBUS_FSPMM_SLAVE_ACTIVATE_REQ_SIZE (sizeof(PROFIBUS_FSPMM_SLAVE_ACTIVATE_REQ_T))
#define PROFIBUS_FSPMM_SLAVE_ACTIVATE_CNF_SIZE (sizeof(PROFIBUS_FSPMM_SLAVE_ACTIVATE_CNF_T))


/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(PROFIBUS_FSPMM_PUBLIC)
#endif



/***************************************************************************************/
/* Profibus Fspmm public, prototypes */



/***************************************************************************************/
#endif /* #ifndef __PROFIBUS_FSPMM_PUBLIC_H */
