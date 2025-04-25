#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_


#include <string.h>

#include "BLA_API.h"
#include<QDebug>
#include <QCoreApplication>

#define ACTUARTODATALENTH 12

#define SteadyDelay(time_ms)\
{\
    auto Delay_start = std::chrono::steady_clock::now();\
    auto Delay_end = Delay_start;\
    std::chrono::duration<double> Delay_time;\
      do{\
           Delay_end = std::chrono::steady_clock::now();\
           Delay_time =Delay_end-Delay_start;\
           if((Delay_time.count()*1000)>=time_ms)\
           {\
               break;\
           }\
           QCoreApplication::processEvents(QEventLoop::AllEvents,1);\
           std::this_thread::sleep_for(std::chrono::microseconds(500));\
      }while(1);\
}


typedef struct{
uint8_t Data_Actuator0[ACTUARTODATALENTH];
uint8_t Data_Actuator1[ACTUARTODATALENTH];
uint8_t Data_Actuator2[ACTUARTODATALENTH];
uint8_t Data_Actuator3[ACTUARTODATALENTH];
uint8_t Data_Actuator4[ACTUARTODATALENTH];
uint8_t Data_Actuator5[ACTUARTODATALENTH];
uint16_t CRC;
uint16_t LF;
}Com_Frame;

unsigned int BLS_Get_CRC16(unsigned char *ptr,  unsigned char len);
int Com_Frame_To_Array(uint8_t arr[],Com_Frame *cf);
int Array_To_Com_Frame(Com_Frame *cf,uint8_t arr[]);
uint8_t Array_To_BLA_Frame(uint8_t ID,uint8_t Datalen,uint8_t FrameBuff[],uint8_t Array[]);
uint8_t Frame_To_PC_Add_CRC_LF(uint8_t FrameBuff[]);



#endif
