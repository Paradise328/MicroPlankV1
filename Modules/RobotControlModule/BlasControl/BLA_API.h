#ifndef _BLA_API_H_
#define _BLA_API_H_

//#include "stm32f10x.h"
//#include "xil_types.h"
#include <cstdint>
#include "qglobal.h"

//#define  uint8_t quint8
//#define  uint16_t quint16
//#define  uint32_t quint32


#define SEND_FRAME_HEAD1  				0x55

#define SEND_FRAME_HEAD2  				0xAA


#define RCV_FRAME_HEAD1			  		0xAA
#define RCV_FRAME_HEAD2  				0x55


//Memory Control Table index address
#define MEMORY_HEAD_INDEX                            0  //2 Bytes
#define MEMORY_ID_INDEX                              0x06  //2 Bytes
#define MEMORY_BAUD_RATE_INDEX                       0x07  //2 Bytes, 0--19200 1--57600 2--115200 3--921600
#define MEMORY_CLEAR_ERROR_INDEX					 0x08  //2 Bytes
#define MEMORY_EMERGENCY_STOP_INDEX					 0x09  //2 Bytes
#define MEMORY_SAVE_PARAMETERS_INDEX				 0x0C  //2 Bytes
#define MEMORY_OVER_TEMPERATURE_PROTECTION_INDEX     0x0E  //2 Bytes, unit:
#define MEMORY_RECOVER_TEMPERATURE_INDEX             0x0F  //2 Bytes, unit:
#define MEMORY_OVER_CURRENT_PROTECTION_INDEX         0x10  //2 Bytes, 16384=3000mA
#define MEMORY_FORCE_CONTROL_DIRECTION_INDEX		 0x15  //2 Bytes, 0-push positive, 1-pull positive
#define MEMORY_WORK_MODE_INDEX						 0x20  //2 Bytes, 0-position mode, 1-force control mode
#define MEMORY_TARGET_FORCE_INDEX					 0x22  //2 Bytes, 16384=200N
#define MEMORY_TARGET_SPEED_INDEX					 0x23  //2 Bytes, 16384=23000RPM=12.2mm/s
#define MEMORY_TARGET_POSITION_INDEX				 0x24  //2 Bytes, 16384=10mm
#define MEMORY_ACTUAL_POSITION_INDEX				 0x26  //2 Bytes, 16384=10mm
#define MEMORY_ACTUAL_CURRENT_INDEX					 0x27  //2 Bytes, 16384=3000mA
#define MEMORY_ACTUAL_SPEED_INDEX					 0x28  //2 Bytes, 16384=23000RPM=12.2mm/s
#define MEMORY_ACTUAL_FORCE_INDEX					 0x29  //2 Bytes, 16384=200N
#define MEMORY_ERROR_CODE_INDEX					 	 0x2A  //2 Bytes, bit0: Stall, bit1: Overtemperature, bit2: Overcurrent, bit3: Motor malfunction, bit4: Flash error or unsaved, bit5: Drive malfunction, bit6: Encoder signal error, bit7: Current sampling circuit error, bit8: Overvoltage, bit9: Undervoltage
#define MEMORY_TEMPERATURE_INDEX					 0x2B  //2 Bytes, unit:



//Instruction mode
#define CMD_RD_STATUS								0x30  //Read drive status command
#define CMD_WR_REGISTER                             0x31  //Write drive register command
#define CMD_RD_REGISTER                             0x32  //Read drive register command



//Number of data
#define UART_RECV_LENGTH                            256   //data length
typedef struct
{
      int16_t       Current_ID;
      int16_t       Current_Position;
      int16_t       Current_Temperature;
      int16_t       Current_Current;
      int16_t       Current_Force;
      int16_t		Current_Speed;
      int16_t       Error_Code;
}Actuator_Status_TypeDef;

//typedef enum {FALSE = 0,TRUE = 1}bool;

uint8_t Read_BLA_Status(uint8_t ID,uint8_t array[]);
uint8_t Save_BLA_Data(uint8_t ID,uint8_t array[]);
uint8_t Change_BLA_ID(uint8_t ID,uint16_t val,uint8_t array[]);
uint8_t Change_BLA_Baud(uint8_t ID,uint16_t baud,uint8_t array[]);
uint8_t Clear_BLA_Error(uint8_t ID,uint8_t array[]);
uint8_t Stop_BLA(uint8_t ID,uint8_t array[]);
uint8_t Write_BLA_Position(uint8_t ID,int16_t val,uint8_t array[]);
uint8_t Write_BLA_Speed(uint8_t ID,int16_t val,uint8_t array[]);
uint8_t Write_BLA_Force(uint8_t ID,int16_t val,uint8_t array[]);
uint8_t Change_BLA_Mode(uint8_t ID,int16_t val,uint8_t array[]);

void Uart_Rece_Data(volatile uint8_t *strd,uint8_t val);
void Uart_Rece_Data_Parsing(void);
void UART_ADC_Moving(uint8_t Move_TempID);



#endif



