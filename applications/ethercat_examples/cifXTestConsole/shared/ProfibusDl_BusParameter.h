/***************************************************************************************

  Copyright (c) Hilscher GmbH. All Rights Reserved.

 ***************************************************************************************

  Filename:
    $Workfile: ProfibusDl_BusParameter.h $ $Revision: 2002 $

  Last Modification:
    $Author: Robert $
    $Modtime: 23.07.08 11:51 $

  Responsible:
    Armin Beck (AB)

  Targets:

  Description:
    ProfibusDL Definitions of the Bus Parameter Set 

  Functions:


  ----------------------------------------------------------------------------
  Todo:
  ----------------------------------------------------------------------------

  Changes:

  Revision  Date          Author        Task Version
    Description
  --------------------------------------------------------------------------------------
  2         23.07.2008    UJ            moved __PACKED_POST to correct position
  1         11.11.2004    AB            1.000
    Created

***************************************************************************************/

#ifndef __PROFIBUS_DL_BUS_PARAMETER_H
 #define __PROFIBUS_DL_BUS_PARAMETER_H

  #define PROFIBUS_DL_DATA_RATE_96      0
  #define PROFIBUS_DL_DATA_RATE_19_2    1
  #define PROFIBUS_DL_DATA_RATE_93_75   2
  #define PROFIBUS_DL_DATA_RATE_187_5   3
  #define PROFIBUS_DL_DATA_RATE_500     4
  #define PROFIBUS_DL_DATA_RATE_1500    6
  #define PROFIBUS_DL_DATA_RATE_3000    7
  #define PROFIBUS_DL_DATA_RATE_6000    8
  #define PROFIBUS_DL_DATA_RATE_12000   9
  #define PROFIBUS_DL_DATA_RATE_31_25  10
  #define PROFIBUS_DL_DATA_RATE_45_45  11
  #define PROFIBUS_DL_DATA_RATE_AUTO   15 


  #define PROFIBUS_DL_PB_FLAG_ISO_MODE_MSK       0x18
  #define PROFIBUS_DL_PB_FLAG_ISOM_SYNC          0x20
  #define PROFIBUS_DL_PB_FLAG_ISOM_FREEZE        0x40
  #define PROFIBUS_DL_PB_FLAG_ERROR_ACTION_FLAG  0x80

  #define BIG_ENDIAN      0
  #define LITTLE_ENDIAN   1

  typedef __PACKED_PRE struct PROFIBUS_DL_BUS_PARAMETER_SET_Ttag {
     TLR_UINT16 usBus_Para_Len; /* Contains the length of the Bus_Para inclusive the field Bus_Para_Len itself */
     TLR_UINT8  bDL_Add; /* Contains the own address of the PROFIBUS Device */
     TLR_UINT8  bData_rate; /* Contains the Transmission speed */
     TLR_UINT16 usTSL; /* slot-time */
     TLR_UINT16 usMin_TSDR; /* min. station delay responder */
     TLR_UINT16 usMax_TSDR; /* max. station delay responder */
     TLR_UINT8  bTQUI; /* quite time */
     TLR_UINT8  bTSET; /* setup time */
     TLR_UINT32 ulTTR; /* target rotation time */
     TLR_UINT8  bG; /* Gap update factor */
     TLR_UINT8  bHSA; /* Highest station address */
     TLR_UINT8  bMax_Retry_Limit; /* retries if error occurs */
     TLR_UINT8  bBp_Flag;
     TLR_UINT16 usMin_Slave_Interval; /* Minimum Slave Interval Time */
     TLR_UINT16 usPoll_Timeout; /* Class2 Poll timeout */
     TLR_UINT16 usData_Control_Time; /* Data Control Time */
     /* START changed hp 08.12.2009 */
     /*TLR_UINT8  bAlarm_Max; *//* Maximum Alarms */
     TLR_UINT8  bMasterSetting; /* 0 == big Endian, 1 == little Endian (swapping should be done)*/
     /* END changed hp 08.12.2009 */
     TLR_UINT8  bMax_User_Global_Control; /* Maximum allowed parallel active USER Global Control Commands */
     TLR_UINT8  abReserved[4]; /* 4 reserved Octets */
     TLR_UINT16 usMaster_User_Data_Len; /* Contains the length of the USER Master data */
     TLR_UINT8  abMaster_Class2_Name[32]; /* Name of the Master */
     TLR_UINT8  abMaster_User_Data[32]; /* USER specific Parameter data */
     TLR_UINT32 ulTCL; /* Isochronous cycle time */
     TLR_UINT8  bMax_TSH; /* Maximum Shift Time */
  } __PACKED_POST PROFIBUS_DL_BUS_PARAMETER_SET_T;
  
#endif  
