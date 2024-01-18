/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: ProfibusFspm_Common.h $
   Last Modification:
    @version $Id: ProfibusFspm_Common.h 2002 2010-08-18 15:13:54Z Robert $
   
   Targets:
    Win32/ANSI   : no
    Win32/Unicode: no (define _UNICODE)
    WinCE        : yes
    rcX          : yes
 
   Description:
     
    Profibus FSPM
    common structures for FSPM
      
   Changes:
 
     Version    Date        Author   Description
     ----------------------------------------------------------------------------------

      1        11.11.2004  AB       1.000
        Created
        
**************************************************************************************/
#ifndef __PROFIBUS_FSPM_COMMON_H
#define __PROFIBUS_FSPM_COMMON_H


/****************************************************************************************
 * Profibus Fspms, constants
 * 
 */

  #define PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE 238          /* Maximum supported diagnostic data size, exlusive 6 standard bytes */
  
  #define PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE 237      /* Maximum supported user parameter data size */
  
  #define PROFIBUS_FSPM_MAX_DLPDU_SIZE 246
  
  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_REQ_PDU 0x41
  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_RES_PDU 0x41
  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_NRS_PDU 0xC1

  #define PROFIBUS_FSPM_APDU_START_SEQ_REQ_PDU 0x42
  #define PROFIBUS_FSPM_APDU_START_SEQ_RES_PDU 0x42
  #define PROFIBUS_FSPM_APDU_START_SEQ_NRS_PDU 0xC2

  #define PROFIBUS_FSPM_APDU_DOWNLOAD_REQ_PDU 0x43
  #define PROFIBUS_FSPM_APDU_DOWNLOAD_RES_PDU 0x43
  #define PROFIBUS_FSPM_APDU_DOWNLOAD_NRS_PDU 0xC3

  #define PROFIBUS_FSPM_APDU_UPLOAD_REQ_PDU 0x44
  #define PROFIBUS_FSPM_APDU_UPLOAD_RES_PDU 0x44
  #define PROFIBUS_FSPM_APDU_UPLOAD_NRS_PDU 0xC4

  #define PROFIBUS_FSPM_APDU_END_SEQ_REQ_PDU 0x45
  #define PROFIBUS_FSPM_APDU_END_SEQ_RES_PDU 0x45
  #define PROFIBUS_FSPM_APDU_END_SEQ_NRS_PDU 0xC5

  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_REQ_PDU 0x46
  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_RES_PDU 0x46
  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_NRS_PDU 0xC6

  #define PROFIBUS_FSPM_APDU_ACT_PARAM_REQ_PDU 0x47
  #define PROFIBUS_FSPM_APDU_ACT_PARAM_RES_PDU 0x47
  #define PROFIBUS_FSPM_APDU_ACT_PARAM_NRS_PDU 0xC7

  #define PROFIBUS_FSPM_APDU_IDLE_REQ_PDU 0x48
  #define PROFIBUS_FSPM_APDU_IDLE_RES_PDU 0x48
  #define PROFIBUS_FSPM_APDU_IDLE_NRS_PDU 0xC8

  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_REQ_PDU 0x51
  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_RES_PDU 0x51
  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_NRS_PDU 0xD1

  #define PROFIBUS_FSPM_APDU_RM_REQ_PDU 0x56
  #define PROFIBUS_FSPM_APDU_RM_RES_PDU 0x56
  #define PROFIBUS_FSPM_APDU_RM_NRS_PDU 0xD6

  #define PROFIBUS_FSPM_APDU_INITIATE_REQ_PDU 0x57
  #define PROFIBUS_FSPM_APDU_INITIATE_RES_PDU 0x57
  #define PROFIBUS_FSPM_APDU_INITIATE_NRS_PDU 0xD7

  #define PROFIBUS_FSPM_APDU_ABORT_REQ_PDU 0x58
  #define PROFIBUS_FSPM_APDU_ABORT_RES_PDU 0x58
  #define PROFIBUS_FSPM_APDU_ABORT_NRS_PDU 0xD8

  #define PROFIBUS_FSPM_APDU_ALARM_ACK_REQ_PDU 0x5C
  #define PROFIBUS_FSPM_APDU_ALARM_ACK_RES_PDU 0x5C
  #define PROFIBUS_FSPM_APDU_ALARM_ACK_NRS_PDU 0xDC
  
  #define PROFIBUS_FSPM_APDU_READ_REQ_PDU 0x5E
  #define PROFIBUS_FSPM_APDU_READ_RES_PDU 0x5E
  #define PROFIBUS_FSPM_APDU_READ_NRS_PDU 0xDE

  #define PROFIBUS_FSPM_APDU_WRITE_REQ_PDU 0x5F
  #define PROFIBUS_FSPM_APDU_WRITE_RES_PDU 0x5F
  #define PROFIBUS_FSPM_APDU_WRITE_NRS_PDU 0xDF

/****************************************************************************************
* Profibus Fspm, common structures */

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_PACK_1(PROFIBUS_FSPM_COMMON)
#endif

  /* APDU structure */
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_APDU_INITIATE_REQ_Ttag {
    TLR_UINT8 bFunction_Num;
    TLR_UINT8 bReserved[3];
    TLR_UINT16 usSend_Timeout;
    TLR_UINT16 usFeatures_Supported;
    TLR_UINT16 usProfile_Features_Supported;
    TLR_UINT16 usProfile_Ident_Number;
    TLR_UINT8  bAdd_Addr_Param[PROFIBUS_FSPM_MAX_DLPDU_SIZE - 10];
  } PROFIBUS_FSPM_APDU_INITIATE_REQ_T;

  /* APDU structure */
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_APDU_INITIATE_RES_Ttag {
    TLR_UINT8 bFunction_Num;
    TLR_UINT8 bMax_Len_Data;
    TLR_UINT16 usFeatures_Supported;
    TLR_UINT16 usProfile_Features_Supported;
    TLR_UINT16 usProfile_Ident_Number;
    TLR_UINT8  bAdd_Addr_Param[PROFIBUS_FSPM_MAX_DLPDU_SIZE - 10];
  } PROFIBUS_FSPM_APDU_INITIATE_RES_T;


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_ADD_ADDR_PARAM_Ttag {
    TLR_UINT8 bS_Type;
    TLR_UINT8 bS_Len;
    TLR_UINT8 bD_Type;
    TLR_UINT8 bD_Len;
    TLR_UINT8 bAddr_Data[1];  /* len is defined by S_len or D_len !! */
  } PROFIBUS_FSPM_ADD_ADDR_PARAM_T;
 
 
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV0_PRM_Ttag {
    TLR_UINT8 abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE];  /* user parameter data without DPV1-bytes */
  } PROFIBUS_FSPM_PARAMETER_DPV0_PRM_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_Ttag {
    __PACKED_PRE struct __PACKED_POST
    {
      TLR_UINT8 bReserved1        : 2;
      TLR_UINT8 bWD_Base_1ms      : 1;
      TLR_UINT8 bReserved2        : 2;
      TLR_UINT8 bPublisher_Enable : 1;
      TLR_UINT8 bFail_Safe        : 1;
      TLR_UINT8 bDPV1_Enable      : 1;
    } DPV1_Status_1;
  
    __PACKED_PRE struct __PACKED_POST
    {
      TLR_UINT8 bRun_On_Cfg_Fault         : 1;
      TLR_UINT8 bReserved1                : 1;
      TLR_UINT8 bEnable_Update_Alarm      : 1;
      TLR_UINT8 bEnable_Status_Alarm      : 1;
      TLR_UINT8 bEnable_Manufacture_Alarm : 1;
      TLR_UINT8 bEnable_Diagnostic_Alarm  : 1;
      TLR_UINT8 bEnable_Process_Alarm     : 1;
      TLR_UINT8 bEnable_Pull_plug_Alarm   : 1;
    } DPV1_Status_2;
  
    __PACKED_PRE struct __PACKED_POST
    {
      TLR_UINT8 bAlarm_Mode   : 3;
      TLR_UINT8 bPrm_Structure: 1;
      TLR_UINT8 bIsoM_Req     : 1;
      TLR_UINT8 bReserved1    : 2;
      TLR_UINT8 bPrmCmd       : 1;
    } DPV1_Status_3;
  } PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV1_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T tDpv1;
    TLR_UINT8     abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE-sizeof(PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T)];
  } PROFIBUS_FSPM_PARAMETER_DPV1_PRM_T;
  
  
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_PRM_CMD             0x02
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_DXB_LINKTABLE       0x03
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_ISOM_PARAMETER      0x04
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_DXB_SUBSCRIBERTABLE 0x07


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_Ttag {
    TLR_UINT8     bStructure_Length;
    TLR_UINT8     bStructure_Type;
    TLR_UINT8     bSlot_Number;
    TLR_UINT8     bReserved;
  } PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T tDpv2Header;
    TLR_UINT8  bVersion;
    TLR_UINT32 bTBase_Dp;
    TLR_UINT16 bTDp;
    TLR_UINT8  bTMapc;
    TLR_UINT32 bTBase_Io;
    TLR_UINT16 bTI;
    TLR_UINT16 bTO;
    TLR_UINT32 bTDx;
    TLR_UINT16 bTPll_W;
    TLR_UINT16 bTPll_D;
  } PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_Ttag {
    TLR_UINT8 bPublisher_Addr;
    TLR_UINT8 bPublisher_Length;
    TLR_UINT8 bSample_Offset;
    TLR_UINT8 bSample_Length;
  } PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T tDpv2Header;
    TLR_UINT8 bVersion;
    PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_T tDbxLinkFilter;
  } PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T tDpv1;
    PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T tDpv2Iso;
    TLR_UINT8 abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE-
                             sizeof(PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T)-
                             sizeof(PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T)];
  } PROFIBUS_FSPM_PARAMETER_DPV2_PRM_T;
  
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_PARAMETER_DATA_Ttag {
    __PACKED_PRE struct __PACKED_POST
    {
      TLR_UINT8 bReserved     : 3;
      TLR_UINT8 bWD_On        : 1;
      TLR_UINT8 bFreeze_Req   : 1;
      TLR_UINT8 bSync_Req     : 1;
      TLR_UINT8 bUnLock_Req   : 1;
      TLR_UINT8 bLock_Req     : 1;
    } Station_Status;             /* status of supported functions */
    TLR_UINT8  bWD_Fact_1;     /* watchdog factor 1 */
    TLR_UINT8  bWD_Fact_2;     /* watchdog factor 2 */
    TLR_UINT8  bMin_Tsdr;      /* min. station delay reponder */
    TLR_UINT16 usIdent_Number; /* ident number of the station, motorola format */
    TLR_UINT8  bGroup_Ident;   /* configures group number */
  
    __PACKED_PRE union __PACKED_POST
    {
      PROFIBUS_FSPM_PARAMETER_DPV0_PRM_T tDpv0;     /* standard parameter , or */
      PROFIBUS_FSPM_PARAMETER_DPV1_PRM_T tDpv1Only; /* parameter data with DPV1 */
      PROFIBUS_FSPM_PARAMETER_DPV2_PRM_T tDpv1Dpv2; /* parameter data with DPV1,DPV2 */
    }un;
  } PROFIBUS_FSPM_PARAMETER_DATA_T;

   

  #define PROFIBUS_FSPM_DIAGNOSTIC_MASTER_ADD_INVALID 0xFF

  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_FSPM_DIAGNOSTIC_DATA_Ttag {
   
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
  
    __PACKED_PRE struct __PACKED_POST 
    {
      TLR_UINT8 bReserved             : 7;
      TLR_UINT8 bExt_Diag_Overflow    : 1; /* ext. diagnostic overflow */
    }  Stationstatus_3;
  
    TLR_UINT8 bMaster_Add;               /* corresponding master address */
    TLR_UINT16 usIdent_Number;           /* ident number, motorola format */
    TLR_UINT8 abExt_Diag_Data[PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE];
                                         /* extended diagnostic field */
  } PROFIBUS_FSPM_DIAGNOSTIC_DATA_T;
  
  #define PROFIBUS_FSPM_DIAGNOSTIC_DATA_SIZE (sizeof(PROFIBUS_FSPM_DIAGNOSTIC_DATA_T) - PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE)

/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(PROFIBUS_FSPM_COMMON)
#endif


/***************************************************************************************/
#endif /* #ifndef __PROFIBUS_FSPM_COMMON_H */
