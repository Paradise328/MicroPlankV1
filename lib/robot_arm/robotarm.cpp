#include "cifxlinux.h"
#include "cifXEndianess.h"
#include "Hil_Packet.h"
#include "Hil_SystemCmd.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "robotarm.h"
#define CIFX_DEV "TCP0_cifX0"
#define CIFX_DEV_TEST "cifX0"
//#define DEBUG 0

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(a) (a=a)
#endif

int SendDataOffset=30;
int RecvDataOffset=22;

unsigned char abSendData[128];
unsigned char abRecvData[128];
CIFX_PACKET tSendPkt       = {{0}};
CIFX_PACKET tRecvPkt       = {{0}};

int brake_condition_now;
int torque_slope_now;

int flag_setSDO;
typedef struct SYNC_CALLBACK_DATAtag
{
  uint8_t bSyncHSMode;
  CIFXHANDLE hDevice;
} SYNC_CALLBACK_DATA;

/*****************************************************************************/
/*! Displays cifX error
*   \param lError     Error code                                             */
/*****************************************************************************/
void ShowError( int32_t lError )
{
  if( lError != CIFX_NO_ERROR)
  {
    char szError[1024] ={0};
    xDriverGetErrorDescription( lError,  szError, sizeof(szError));
    printf("Error: 0x%X, <%s>\n", (unsigned int)lError, szError);
  }
}

/*****************************************************************************/
/*! Displays a hex dump on the debug console (16 bytes per line)
*   \param pbData     Pointer to dump data
*   \param ulDataLen  Length of data dump                                    */
/*****************************************************************************/
void DumpData(unsigned char* pbData, unsigned long ulDataLen)
{
    unsigned long ulIdx;
#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  for(ulIdx = 0; ulIdx < ulDataLen; ++ulIdx)
  {
    if(0 == (ulIdx % 16))
      printf("\r\n");

    printf("%02X ", pbData[ulIdx]);
  }
  printf("\r\n");
}

/*****************************************************************************/
/*! Dumps a rcX packet to debug console
*   \param ptPacket Pointer to packed being dumped                           */
/*****************************************************************************/
void DumpPacket(CIFX_PACKET* ptPacket)
{
#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  printf("Dest   : 0x%08lX      ID   : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDest),  (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulId));
  printf("Src    : 0x%08lX      Sta  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrc),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulState));
  printf("DestID : 0x%08lX      Cmd  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulDestId),(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulCmd));
  printf("SrcID  : 0x%08lX      Ext  : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulSrcId), (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulExt));
  printf("Len    : 0x%08lX      Rout : 0x%08lX\r\n",(long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulLen),   (long unsigned int)HOST_TO_LE32(ptPacket->tHeader.ulRout));

  printf("Data:");
  DumpData(ptPacket->abData, HOST_TO_LE32(ptPacket->tHeader.ulLen));
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t ChannelDemo()
{
#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  printf("---------- Communication Channel demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hChannel = NULL;
    lRet = xChannelOpen(hDriver, CIFX_DEV_TEST, 0, &hChannel);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening Channel!");

    } else
    {
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      /* Channel successfully opened, so query basic information */
      if( CIFX_NO_ERROR != (lRet = xChannelInfo(hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("Communication Channel Info:\r\n");
        printf("Device Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulDeviceNumber);
        printf("Serial Number    : %lu\r\n",(long unsigned int)tChannelInfo.ulSerialNumber);
        printf("Firmware         : %s\r\n", tChannelInfo.abFWName);
        printf("FW Version       : %u.%u.%u build %u\r\n",
                tChannelInfo.usFWMajor,
                tChannelInfo.usFWMinor,
                tChannelInfo.usFWRevision,
                tChannelInfo.usFWBuild);
        printf("FW Date          : %02u/%02u/%04u\r\n",
                tChannelInfo.bFWMonth,
                tChannelInfo.bFWDay,
                tChannelInfo.usFWYear);

        printf("Mailbox Size     : %lu\r\n",(long unsigned int)tChannelInfo.ulMailboxSize);
      }
      uint32_t    ulSendPktCount = 0;
      uint32_t    ulRecvPktCount = 0;


      printf("\nStart put/get packet Demo!\n");

      /* Read Security EEPROM zone 1*/
      xChannelGetMBXState( hChannel, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
      printf("Channel Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
             ulSendPktCount, ulRecvPktCount);

      /* Do a basic Packet Transfer */
      if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(hChannel, &tSendPkt, 10)))
      {
        printf("Error sending packet to device (0x%X)!\r\n", lRet);
      } else
      {
        printf("Send Packet:\r\n");
        DumpPacket(&tSendPkt);

        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(hChannel, sizeof(tRecvPkt), &tRecvPkt, 20)) )
        {
          printf("Error getting packet from device!\r\n");
        } else
        {
          printf("Received Packet:\r\n");
          DumpPacket(&tRecvPkt);
        }
      }
      sleep(1);

      printf("\nStart read/write IO-Data!\n");

      /* Read and write I/O data (32Bytes). Output data will be incremented each cyle */
    //  unsigned char abSendData[32] = {0};
    //  unsigned char abRecvData[32] = {0};
      unsigned long ulCycles       = 0;
      unsigned long ulState;

      if(CIFX_NO_ERROR != (lRet = xChannelBusState(hChannel, CIFX_BUS_STATE_ON,(uint32_t*) &ulState, 10000)))
      {
         printf("Error setting Bus state lRet = 0x%08X!\r\n",(unsigned int)lRet);
         xChannelClose(hChannel);
         xDriverClose(hDriver);
         return lRet;
      }
     // printf("IO Demo running <Type Your Command>:\n");
      while(1)
      {
        ++ulCycles;
        usleep(1 * 1000); /* Wait 1 ms so we can see the counter on the LEDs */
        if(CIFX_NO_ERROR != (lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
        {
          printf("Error reading IO Data area!\r\n");
          break;
        } else {
#ifdef DEBUG
          printf("------\n");
          printf("IO Read Data:");
          DumpData(abRecvData, sizeof(abRecvData));


#endif
          if(flag_setSDO){
            printf("in\n");
            printf("\nStart put/get packet Demo!\n");

            /* Read Security EEPROM zone 1*/
            xChannelGetMBXState( hChannel, (uint32_t*)&ulRecvPktCount, (uint32_t*)&ulSendPktCount);
            printf("Channel Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
            ulSendPktCount, ulRecvPktCount);

            /* Do a basic Packet Transfer */
            if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(hChannel, &tSendPkt, 10)))
            {
             printf("Error sending packet to device (0x%X)!\r\n", lRet);
            } else
            {
             printf("Send Packet:\r\n");
             DumpPacket(&tSendPkt);

             if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(hChannel, sizeof(tRecvPkt), &tRecvPkt, 20)) )
             {
               printf("Error getting packet from device!\r\n");
             } else
             {
               printf("Received Packet:\r\n");
               DumpPacket(&tRecvPkt);
             }
             flag_setSDO=0;
            }

        }
       //   memcpy(abSendData, abRecvData, sizeof(abRecvData));

          /* On a CB-AB32 we will echo the input 8 buttons if one is pressed, otherwise a counter
             will be placed on output */
        //  if(abRecvData[0] == 0)
        //    abSendData[0] = (unsigned char)ulCycles;

          if(CIFX_NO_ERROR != (lRet = xChannelIOWrite(hChannel, 0, 0, sizeof(abSendData), abSendData, 10)))
          {
            printf("Error writing to IO Data area!\r\n");
            break;
          } else
          {
#ifdef DEBUG
            printf("======\n");
            printf(" IOWrite Data:");
            DumpData(abSendData, sizeof(abSendData));
            printf("------\n");
#endif
          }
        }
      }
      printf("IODemo ended. Total cycles %lu\n", ulCycles);

      if(CIFX_NO_ERROR != (lRet = xChannelBusState(hChannel, CIFX_BUS_STATE_OFF, (uint32_t*)&ulState, 10000)))
      {
        printf("Error setting Bus state lRet = 0x%08X!\r\n",(unsigned int)lRet);
      }


      xChannelClose(hChannel);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (unsigned int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;

}


// A normal C function that is executed as a thread when its name is specified in pthread_create()
void *cifxThread(void *vargp)
{
    printf("Thread for cifx starts! \n");
    struct CIFX_LINUX_INIT init;
    init.init_options        = CIFX_DRIVER_INIT_AUTOSCAN;
    init.iCardNumber         = 0;
    init.fEnableCardLocking  = 0;
    init.base_dir            = NULL;
    init.poll_interval       = 0;
    init.poll_StackSize      = 0;
    init.trace_level         = 255;
    init.user_card_cnt       = 0;
    init.user_cards          = NULL;

#ifdef DEBUG
    printf("%s() called\n", __FUNCTION__);
#endif
  // First of all initialize toolkit
  int32_t lRet = cifXDriverInit(&init);
  if(CIFX_NO_ERROR == lRet)
  {
    // Demonstrate communication channel functionality
    ChannelDemo();
  }

  cifXDriverDeinit();
  return NULL;
}
// display abSendData
void print_abSendData(){
    printf("IOSend Data:");
    DumpData(abSendData, sizeof(abSendData));
}
// display abRecvData
void print_abRecvData(){
    printf("IOReceive Data:");
    DumpData(abRecvData, sizeof(abRecvData));
}
void print_tSendSDOData(){
    printf("tsend sdo Data:");
    DumpPacket(&tSendPkt);
}
void print_tRecvSDOData(){
    printf("trecv sdo Data:");
    DumpPacket(&tRecvPkt);
}
// display error_code uint
unsigned int get_error_code(int joint){
    unsigned int error_code = abRecvData[11+RecvDataOffset*(joint-1)]*256+abRecvData[10+RecvDataOffset*(joint-1)];
    return error_code;
}

// display actual Position
long int get_position(int joint){
    long int position = 0;
    if(abRecvData[9+RecvDataOffset*(joint-1)]>127){
      position=-((((255-abRecvData[9+RecvDataOffset*(joint-1)])*256+(255-abRecvData[8+RecvDataOffset*(joint-1)]))*256+(255-abRecvData[7+RecvDataOffset*(joint-1)]))*256+255-abRecvData[6+RecvDataOffset*(joint-1)]);
    }else{
      position=((abRecvData[9+RecvDataOffset*(joint-1)]*256+abRecvData[8+RecvDataOffset*(joint-1)])*256+abRecvData[7+RecvDataOffset*(joint-1)])*256+abRecvData[6+RecvDataOffset*(joint-1)];
    }
    return position;
}

// display status_word
unsigned int get_status_word(int joint){
    unsigned int status_word = abRecvData[1+RecvDataOffset*(joint-1)]*256+abRecvData[0+RecvDataOffset*(joint-1)];
    return status_word;
}

// display digital_input
unsigned long int get_digital_input(int joint){
    unsigned long int di = abRecvData[4+RecvDataOffset*(joint-1)];
    return di;
}
void sync_target_position(int joint){
    abSendData[2+SendDataOffset*(joint-1)]=abRecvData[6+RecvDataOffset*(joint-1)];
    abSendData[3+SendDataOffset*(joint-1)]=abRecvData[7+RecvDataOffset*(joint-1)];
    abSendData[4+SendDataOffset*(joint-1)]=abRecvData[8+RecvDataOffset*(joint-1)];
    abSendData[5+SendDataOffset*(joint-1)]=abRecvData[9+RecvDataOffset*(joint-1)];
}

// display actual velocity
long int get_velocity(int joint){
    long int velocity = 0;
    if(abRecvData[15+RecvDataOffset*(joint-1)]>127){
      velocity=-((((255-abRecvData[15+RecvDataOffset*(joint-1)])*256+(255-abRecvData[14+RecvDataOffset*(joint-1)]))*256+(255-abRecvData[13+RecvDataOffset*(joint-1)]))*256+255-abRecvData[12+RecvDataOffset*(joint-1)]);
    }else{
      velocity=((abRecvData[15+RecvDataOffset*(joint-1)]*256+abRecvData[14+RecvDataOffset*(joint-1)])*256+abRecvData[13+RecvDataOffset*(joint-1)])*256+abRecvData[12+RecvDataOffset*(joint-1)];
    }
    return velocity;
}

// display actual current: int
int get_current(int joint){
    int current = abRecvData[19+RecvDataOffset*(joint-1)]*256+abRecvData[18+RecvDataOffset*(joint-1)];
    return current;
}

// display mode of operation sint
short int get_mode(int joint){
    short int mode_display = abRecvData[20+RecvDataOffset*(joint-1)];
    return mode_display;
}
// display actual torque: int
int get_torque(int joint){
    int torque = abRecvData[17+RecvDataOffset*(joint-1)]*256+abRecvData[16+RecvDataOffset*(joint-1)];
    if(abRecvData[17+RecvDataOffset*(joint-1)]>127){
      torque = -((255-abRecvData[17+RecvDataOffset*(joint-1)])*256+(255-abRecvData[16+RecvDataOffset*(joint-1)]));
    }else{
      torque = abRecvData[17+RecvDataOffset*(joint-1)]*256+abRecvData[16+RecvDataOffset*(joint-1)];
    }
    return torque;
}
int update_control_mode(int joint,short int control_mode){
    abSendData[16+SendDataOffset*(joint-1)]=control_mode%256;

}

int init_joint(int joint){
  short int control_mode = 3;
  unsigned long int target_acceleration=2000000;
  unsigned long int target_deceleration=2000000;
  unsigned long int max_velocity=8000;
  unsigned int control_word=128;

 // printf("set control mode to %d\n",control_mode);
  abSendData[16+SendDataOffset*(joint-1)]=control_mode%256;

 // printf("set acceleration to %d\n",target_acceleration);
  abSendData[22+SendDataOffset*(joint-1)]=target_acceleration%(256);
  abSendData[23+SendDataOffset*(joint-1)]=target_acceleration/256%256;
  abSendData[24+SendDataOffset*(joint-1)]=target_acceleration/(256*256)%256;
  abSendData[25+SendDataOffset*(joint-1)]=target_acceleration/(256*256*256)%256;

 // printf("set decceleration to %d\n",target_deceleration);
  abSendData[26+SendDataOffset*(joint-1)]=target_deceleration%(256);
  abSendData[27+SendDataOffset*(joint-1)]=target_deceleration/256%256;
  abSendData[28+SendDataOffset*(joint-1)]=target_deceleration/(256*256)%256;
  abSendData[29+SendDataOffset*(joint-1)]=target_deceleration/(256*256*256)%256;

 // printf("set max velocity to %d\n",max_velocity);
  abSendData[18+SendDataOffset*(joint-1)]=max_velocity%(256);
  abSendData[19+SendDataOffset*(joint-1)]=max_velocity/256%256;
  abSendData[20+SendDataOffset*(joint-1)]=max_velocity/(256*256)%256;
  abSendData[21+SendDataOffset*(joint-1)]=max_velocity/(256*256*256)%256;

  control_word = 128;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;

  usleep(50*1000);
  control_word = 6;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;

  usleep(50*1000);
  control_word = 7;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;


  return 1;
}
int init_torque(int joint){
  short int control_mode = 1;
  unsigned int control_word=128;

 // printf("set control mode to %d\n",control_mode);
  abSendData[16+SendDataOffset*(joint-1)]=control_mode%256;

  update_torque(joint,0);
  control_word = 128;
//  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;

  usleep(50*1000);
  control_word = 6;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;

  usleep(50*1000);
  control_word = 7;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;

  return 1;
}
int torque_mode(int joint){
    unsigned int control_word=15;
    //printf("set control word to %d\n",control_word);
    abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
    abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;
    return 1;
}
int toggle_servo(int joint){
    if((abRecvData[0+SendDataOffset*(joint-1)]/8)%2==1){
        abSendData[0+SendDataOffset*(joint-1)]-=8;
    }else{
        abSendData[0+SendDataOffset*(joint-1)]+=8;
    }
}
int toggle_control_mode(int joint){
    if(get_mode(joint)==4){
        update_control_mode(joint,1);
    }else if(get_mode(joint)==1){
        update_control_mode(joint,4);
    }
}

int disable_joint(int joint){
  unsigned int control_word = 0;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;
  return 1;
}
int enable_joint(int joint){
  unsigned int control_word = 15;
  //printf("set control word to %d\n",control_word);
  abSendData[0+SendDataOffset*(joint-1)]=control_word%(256);
  abSendData[1+SendDataOffset*(joint-1)]=control_word/256%256;
  return 1;
}
int update_torque(int joint,long int target_torque){
    int p1,p2;
    int target_torque_abs=target_torque;
    if(target_torque<0){
        target_torque_abs=-target_torque;
    }

    p1 = target_torque_abs%256;
    p2 = target_torque_abs/256%256;
    if(target_torque<0){
        p1=255-p1;
        p2=255-p2;
    }
    abSendData[14+SendDataOffset*(joint-1)]=p1;
    abSendData[15+SendDataOffset*(joint-1)]=p2;
}

int update_speed(int joint,long int target_velocity){

  int p1,p2,p3,p4;
  long int target_velocity_abs=target_velocity;

  if(target_velocity<0){
      target_velocity_abs=-target_velocity;
  }

  p1 = target_velocity_abs%256;
  p2 = target_velocity_abs/256%256;
  p3 = target_velocity_abs/256/256%256;
  p4 = target_velocity_abs/256/256/256%256;

  if(target_velocity<0){
      p1=255-p1;
      p2=255-p2;
      p3=255-p3;
      p4=255-p4;
  }

  abSendData[10+SendDataOffset*(joint-1)]=p1;
  abSendData[11+SendDataOffset*(joint-1)]=p2;
  abSendData[12+SendDataOffset*(joint-1)]=p3;
  abSendData[13+SendDataOffset*(joint-1)]=p4;

  return 1;
}

int set_SDO_6087(int joint, int val){
    tSendPkt.tHeader.ulDest=0x00000020;
    tSendPkt.tHeader.ulCmd=0x00009A00;
    tSendPkt.tHeader.ulLen=0x00000016;
    //tSendPkt.tHeader.ulId
    tSendPkt.abData[0]=joint-1;
    tSendPkt.abData[1]=0x01;
    tSendPkt.abData[2]=0x00;
    tSendPkt.abData[3]=0x00;
    tSendPkt.abData[4]=0x00;
    tSendPkt.abData[5]=0x00;
    tSendPkt.abData[6]=0x87;
    tSendPkt.abData[7]=0x60;
    tSendPkt.abData[8]=0x00;
    tSendPkt.abData[9]=0x00;
    tSendPkt.abData[0x0A]=0x04;
    tSendPkt.abData[0x0B]=0x00;
    tSendPkt.abData[0x0C]=0x00;
    tSendPkt.abData[0x0D]=0x00;
    tSendPkt.abData[0x0E]=0xE8;
    tSendPkt.abData[0x0F]=0x03;
    tSendPkt.abData[0x10]=0x00;
    tSendPkt.abData[0x11]=0x00;
    tSendPkt.abData[0x12]=val%256;
    tSendPkt.abData[0x13]=val/256;
    tSendPkt.abData[0x14]=0x00;
    tSendPkt.abData[0x15]=0x00;
    flag_setSDO=1;
    usleep(1000*10);
}
int read_SDO_6087(int joint){
    flag_setSDO=1;
    usleep(1000*10);


    /*
     printf("set brake condition(sdo):");
          tSendPkt.tHeader.ulCmd=0x00009A00;
          tSendPkt.tHeader.ulLen=0x00000016;
          tSendPkt.abData[0]=joint[0]-'1';
          tSendPkt.abData[1]=0x01;
          tSendPkt.abData[6]=0x02;
          tSendPkt.abData[7]=0x46;
          tSendPkt.abData[8]=0x00;
          tSendPkt.abData[0x0A]=0x04;
          tSendPkt.abData[0x0B]=0x00;
          tSendPkt.abData[0x12]=val[0]-'0';
          tSendPkt.abData[0x13]=0x00;
          tSendPkt.abData[0x14]=0x00;
          tSendPkt.abData[0x15]=0x00;
          flag_setSDO=1;
          usleep(1000*10);
          printf("4602 set: %02X %02X %02X %02X\n",tRecvPkt.abData[0x12],tRecvPkt.abData[0x13],tRecvPkt.abData[0x14],tRecvPkt.abData[0x15]);
          printf("4602 return val: %d\n",tRecvPkt.abData[0x12]);
          */

}
int read_brake_4602(int joint){
    tSendPkt.tHeader.ulCmd=0x00009A02;
    tSendPkt.tHeader.ulLen=0x00000012;
    tSendPkt.abData[0]=joint-1;
    tSendPkt.abData[1]=0x01;
    tSendPkt.abData[6]=0x02;
    tSendPkt.abData[7]=0x46;
    tSendPkt.abData[8]=0x00;
    tSendPkt.abData[14]=0xE8;
    tSendPkt.abData[15]=0x03;
    tSendPkt.abData[16]=0x00;
    tSendPkt.abData[17]=0x00;
    flag_setSDO=1;
    usleep(1000*10);
    return tRecvPkt.abData[18]+(tRecvPkt.abData[19]+(tRecvPkt.abData[20]+tRecvPkt.abData[21]*256)*256)*256;
}
int set_brake_4602(int joint, int val){
    tSendPkt.tHeader.ulDest=0x00000020;
    tSendPkt.tHeader.ulCmd=0x00009A00;
    tSendPkt.tHeader.ulLen=0x00000016;
    //tSendPkt.tHeader.ulId
    tSendPkt.abData[0]=joint-1;
    tSendPkt.abData[1]=0x01;
    tSendPkt.abData[2]=0x00;
    tSendPkt.abData[3]=0x00;
    tSendPkt.abData[4]=0x00;
    tSendPkt.abData[5]=0x00;
    tSendPkt.abData[6]=0x02;
    tSendPkt.abData[7]=0x46;
    tSendPkt.abData[8]=0x00;
    tSendPkt.abData[9]=0x00;
    tSendPkt.abData[0x0A]=0x04;
    tSendPkt.abData[0x0B]=0x00;
    tSendPkt.abData[0x0C]=0x00;
    tSendPkt.abData[0x0D]=0x00;
    tSendPkt.abData[0x0E]=0xE8;
    tSendPkt.abData[0x0F]=0x03;
    tSendPkt.abData[0x10]=0x00;
    tSendPkt.abData[0x11]=0x00;
    tSendPkt.abData[0x12]=val;
    tSendPkt.abData[0x13]=0x00;
    tSendPkt.abData[0x14]=0x00;
    tSendPkt.abData[0x15]=0x00;
    flag_setSDO=1;
    usleep(1000*10);
}

void *robotarmThread(void *p){
    cifxThread(NULL);
    sleep(2);
    return NULL;
}
