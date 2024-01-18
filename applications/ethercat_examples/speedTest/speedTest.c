
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

#define CIFX_DEV "TCP0_cifX0"
#define CIFX_DEV_TEST "cifX0"
#define DEBUG 1

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(a) (a=a)
#endif

unsigned char abSendData[64] ;
unsigned char abRecvData[64] ;
CIFX_PACKET tSendPkt       = {{0}};
CIFX_PACKET tRecvPkt       = {{0}};

typedef struct SYNC_CALLBACK_DATAtag
{
  uint8_t bSyncHSMode;
  CIFXHANDLE hDevice;
} SYNC_CALLBACK_DATA;


int flag_setSDO=0;
static int kbhit()
{
  struct termios oldt, newt;
  int    ch;
  int    oldf;
  int    iRet = 0;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    iRet = 1;
  }
  return iRet;
}


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
  printf("Data is on above\n");
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

      printf("IO Demo running <Type Your Command>:\n");

      while(1)
      {
        ++ulCycles;
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
          }
          while(1);
        }
        usleep(1 * 1000); /* Wait 1 ms so we can see the counter on the LEDs */
        if(CIFX_NO_ERROR != (lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
        {
          printf("Error reading IO Data area!\r\n");
          break;
        } else
        {
#ifdef DEBUG
    //      printf("------\n");
    //      printf("IORead Data:");
    //      DumpData(abRecvData, sizeof(abRecvData));
#endif
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
     //       printf("======\n");
      //      printf("IOWrite Data:");
      //      DumpData(abSendData, sizeof(abSendData));
      //      printf("------\n");
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

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void *cifxThread(void *vargp)
{
	printf("Thread for cifx starts! \n");

  struct CIFX_LINUX_INIT init =
    {
      .init_options        = CIFX_DRIVER_INIT_AUTOSCAN,
      .iCardNumber         = 0,
      .fEnableCardLocking  = 0,
      .base_dir            = NULL,
      .poll_interval       = 0,
      .poll_StackSize      = 0,   /* set to 0 to use default */
      .trace_level         = 255,
      .user_card_cnt       = 0,
      .user_cards          = NULL,
    };
	
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

void parseCmd(char cmd[], char val[]){
  

  if(strcmp(val,"?")==0){
    if(strcmp(cmd,"receive")==0){
        printf("IORead Data:");
        DumpData(abRecvData, sizeof(abRecvData));
    }else if(strcmp(cmd,"send")==0){
        printf("IOSend Data:");
        DumpData(abSendData, sizeof(abSendData));
    }else if(strcmp(cmd,"status")==0){
        printf("status word: %02X %02X\n",abRecvData[1],abRecvData[0]);
    }else if(strcmp(cmd,"di")==0){
        printf("digital input: %02X %02X %02X %02X\n",abRecvData[5],abRecvData[4],abRecvData[3],abRecvData[2]);
    }else if(strcmp(cmd,"position")==0){
        printf("position actual: %02X %02X %02X %02X\n",abRecvData[9],abRecvData[8],abRecvData[7],abRecvData[6]);
    }else if(strcmp(cmd,"error")==0){
        printf("error code: %02X %02X\n",abRecvData[11],abRecvData[10]);
    }else if(strcmp(cmd,"velocity")==0){
        printf("velocity actual: %02X %02X %02X %02X\n",abRecvData[15],abRecvData[14],abRecvData[13],abRecvData[12]);
    }else if(strcmp(cmd,"torque")==0){
        printf("torque actual: %02X %02X\n",abRecvData[17],abRecvData[16]);
    }else if(strcmp(cmd,"current")==0){
        printf("current actual: %02X %02X\n",abRecvData[19],abRecvData[18]);
    }else if(strcmp(cmd,"mode")==0){
        printf("modes of operation display: %02X\n",abRecvData[20]);
    }else if(strcmp(cmd,"send")==0){
        printf("send data dump: ");
        DumpData(abSendData,sizeof(abSendData));
    }else if(strcmp(cmd,"sdo")==0){
      printf("set sdo");
      tSendPkt.tHeader.ulDest=0x00000020;
      tSendPkt.tHeader.ulCmd=0x00009A02;
      tSendPkt.tHeader.ulLen=0x00000012;
      tSendPkt.abData[0]=0x00;
      tSendPkt.abData[1]=0x01;
      tSendPkt.abData[2]=0x00;
      tSendPkt.abData[3]=0x00;
      tSendPkt.abData[4]=0x00;
      tSendPkt.abData[5]=0x00;
      tSendPkt.abData[6]=0x00;
      tSendPkt.abData[7]=0x10;
      tSendPkt.abData[8]=0x00;
      tSendPkt.abData[9]=0x00;
      tSendPkt.abData[10]=0xE8;
      tSendPkt.abData[11]=0x03;
      tSendPkt.abData[12]=0x00;
      tSendPkt.abData[13]=0x00;
      tSendPkt.abData[14]=0x0A;
      tSendPkt.abData[15]=0x00;
      tSendPkt.abData[16]=0x00;
      tSendPkt.abData[17]=0x00;
      flag_setSDO=1;
    }
    else{
      printf("wrong cmd!\n");
    }
  }else{
    //set control mode to RxPDO --uint
    if(strcmp(cmd,"control")==0){
        printf("set control word to %s\n",val);
        abSendData[0]=atoi(val)%(256);
        abSendData[1]=atoi(val)/256%256;
        printf("control word: %02X %02X\n",abSendData[0],abSendData[1]);
    }

    //set target position to RxPDO --dint
    else if(strcmp(cmd,"position")==0){
        printf("set target position to %s\n",val);
        int target_position = atoi(val);
        int p1,p2,p3,p4,target_position_abs=target_position;
        if(target_position<0){target_position_abs=-target_position;}
        p1 = target_position_abs%256;
        p2 = target_position_abs/256%256;
        p3 = target_position_abs/256/256%256;
        p4 = target_position_abs/256/256/256%256;
        if(target_position<0){
            p1=255-p1;
            p2=255-p2;
            p3=255-p3;
            p4=255-p4;
        }
        abSendData[2]=p1;
        abSendData[3]=p2;
        abSendData[4]=p3;
        abSendData[5]=p4;
        printf("target position: %02X %02X %02X %02X\n",abSendData[2],abSendData[3],abSendData[4],abSendData[5]);
    }
    // set digital oputputs to RxPDO udint----not finish
    else if(strcmp(cmd,"do")==0){
        printf("set digital output to %s\n",val);
        abSendData[22]=atoi(val)%256;
        printf("digital output: %02X %02X %02X %02X\n",abSendData[6],abSendData[7],abSendData[8],abSendData[9]);
    }
    // set target velocity to RxPDO dint
    else if(strcmp(cmd,"v")==0){
        printf("set target velocity to %s\n",val);
        int target_velocity = atoi(val);
        int p1,p2,p3,p4,target_velocity_abs=target_velocity;
        if(target_velocity<0){target_velocity_abs=-target_velocity;}
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
        abSendData[10]=p1;
        abSendData[11]=p2;
        abSendData[12]=p3;
        abSendData[13]=p4;
        printf("target velocity: %02X %02X %02X %02X\n",abSendData[10],abSendData[11],abSendData[12],abSendData[13]);
    }
    // set target torque to RxPDO int
    else if(strcmp(cmd,"torque")==0){
        printf("set target torque to %s\n",val);
        int target_torque = atoi(val);
        int p1,p2,p3,p4,target_torque_abs=target_torque;
        if(target_torque<0){target_torque_abs=-target_torque;}
        p1 = target_torque_abs%256;
        p2 = target_torque_abs/256%256;
        if(target_torque<0){
            p1=255-p1;
            p2=255-p2;
        }
        abSendData[14]=p1;
        abSendData[15]=p2;
        printf("target torque: %02X %02X\n",abSendData[14],abSendData[15]);
    }

    //set control mode to RxPDO sint
    else if(strcmp(cmd,"mode")==0){
        printf("set control mode to %s\n",val);
        abSendData[16]=atoi(val)%256;
        printf("control mode: %02X\n",abSendData[16]);
    }

    //set max velocity to RxPDO
    else if(strcmp(cmd,"maxv")==0){
        printf("set max velocity to %s\n",val);
        int max_velocity = atoi(val);
        abSendData[18]=max_velocity%(256);
        abSendData[19]=max_velocity/256%256;
        abSendData[20]=max_velocity/(256*256)%256;
        abSendData[21]=max_velocity/(256*256*256)%256;
        printf("max velocity: %02X %02X %02X %02X\n",abSendData[18],abSendData[19],abSendData[20],abSendData[21]);
    }

    //set acceleration to RxPDO
    else if(strcmp(cmd,"ac")==0){
        printf("set acceleration to %s\n",val);
        int target_acceleration = atoi(val);
        abSendData[22]=target_acceleration%(256);
        abSendData[23]=target_acceleration/256%256;
        abSendData[24]=target_acceleration/(256*256)%256;
        abSendData[25]=target_acceleration/(256*256*256)%256;
        printf("deicimal target_acceleration=%d\n",target_acceleration);
        printf("heximal target_acceleration: %02X %02X %02X %02X\n",abSendData[22],abSendData[23],abSendData[24],abSendData[25]);
    }

    //set deceleration to RxPDO
    else if(strcmp(cmd,"dc")==0){
        printf("set decceleration to %s\n",val);
        int target_deceleration = atoi(val);
        abSendData[26]=target_deceleration%(256);
        abSendData[27]=target_deceleration/256%256;
        abSendData[28]=target_deceleration/(256*256)%256;
        abSendData[29]=target_deceleration/(256*256*256)%256;
        printf("deicimal target_deceleration=%d\n",target_deceleration);
        printf("heximal target_deceleration: %02X %02X %02X %02X\n",abSendData[26],abSendData[27],abSendData[28],abSendData[29]);
    
    }
  }
}
int main(){

 //   while(1){}
    pthread_t thread_id;
    char input[100],cmd[30],val[30];


	  pthread_create(&thread_id, NULL, cifxThread, NULL);
  
    while(1){
        printf( "input a cmd :");
        scanf("%s %s", cmd, val);
        printf("\ncmd is: %s; val is %s\n", cmd, val);
        
        parseCmd(cmd,val);
    }
    
    pthread_join(thread_id, NULL);
    exit(0);
    /*            
    while(1){
        int control_word;
        int target_position;
        int digital_outputs;
        int target_velocityint;
        int target_torque;
        int velocity_offset;
        int modes_of_operation;

        

    }
    */
    return 1;
}