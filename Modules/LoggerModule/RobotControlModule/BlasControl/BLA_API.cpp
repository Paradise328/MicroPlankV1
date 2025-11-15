/****************************************Copyright (c)****************************************************
** 
**                                      
**
**                                   
**
**-----------------------------------------File Info------------------------------------------------------
** File name: API.c
** Version:   V1.00
** Description: 
** API for micro brushless linear actuator
*********************************************************************************************************/
#include "BLA_API.h"


volatile uint8_t   UART_Cmd_RT_ID = 0;
volatile uint16_t  UART_RX_Num = 0;


volatile uint8_t   UART_Recv_Buf[UART_RECV_LENGTH]={0};
volatile uint8_t   UART_Send_Buf[UART_RECV_LENGTH]={0};


Actuator_Status_TypeDef Actuator_Status;    //Define receive data Structure
/*********************************************************************************************************
** Function name:       void Read_BLA_Status(uint8_t ID)      
** Descriptions:        Read drive status      
** input parameters:    Input ID of the drive
** output parameters:   Output command array
** Returned value:      None
*********************************************************************************************************/

uint8_t Read_BLA_Status(uint8_t ID,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t  checksum = 0;
    uint8_t send_num = 8;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_RD_STATUS;
    array[5] = 0;
    array[6] = 0;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:      void Save_BLA_Data(uint8_t ID)    
** Descriptions:       Save flash data to drive
** input parameters:   Input ID of the drive
** output parameters:  Output command array
** Returned value:     None
*********************************************************************************************************/

uint8_t Save_BLA_Data(uint8_t ID,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_SAVE_PARAMETERS_INDEX & 0xFF;
    array[6] = (MEMORY_SAVE_PARAMETERS_INDEX >> 8) & 0xFF;
    array[7] = 1;
    array[8] = 0;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:       void Change_BLA_ID(uint8_t ID,uint16_t val)
** Descriptions:        Change ID of the current drive
** input parameters:    ID:ID of the current drive
                        val:Set ID
** output parameters:   Output command array
** Returned value:      None 
*********************************************************************************************************/

uint8_t Change_BLA_ID(uint8_t ID,uint16_t val,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_ID_INDEX & 0xFF;
    array[6] = (MEMORY_ID_INDEX >> 8) & 0xFF;
    array[7] = val & 0xFF;
    array[8] = (val >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:       void Change_BLA_Baud(uint8_t ID,uint16_t baud)  
** Descriptions:        Change baud of the drive 
** input parameters:    ID:ID of the current drive
                        val:Representative number of baud rate,0--19200 1--57600 2--115200 3--921600
** output parameters:   Output command array
** Returned value:      None
*********************************************************************************************************/

uint8_t Change_BLA_Baud(uint8_t ID,uint16_t baud,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_BAUD_RATE_INDEX & 0xFF;
    array[6] = (MEMORY_BAUD_RATE_INDEX >> 8) & 0xFF;
    array[7] = baud & 0xFF;
    array[8] = (baud >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:       void Clear_BLA_Error(uint8_t ID)
** Descriptions:        Clear drive error
** input parameters:    Input ID of the drive
** output parameters:   Output command array
** Returned value:      None
*********************************************************************************************************/

uint8_t Clear_BLA_Error(uint8_t ID,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_CLEAR_ERROR_INDEX & 0xFF;
    array[6] = (MEMORY_CLEAR_ERROR_INDEX >> 8) & 0xFF;
    array[7] = 1;
    array[8] = 0;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:        void Stop_BLA(uint8_t ID)
** Descriptions:         Forced to stop exercise
** input parameters:     Input ID of the drive
** output parameters:    Output command array
** Returned value:       None
*********************************************************************************************************/

uint8_t Stop_BLA(uint8_t ID,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_EMERGENCY_STOP_INDEX & 0xFF;
    array[6] = (MEMORY_EMERGENCY_STOP_INDEX >> 8) & 0xFF;
    array[7] = 1;
    array[8] = 0;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:      void Write_BLA_Position(uint8_t ID,int16_t val) 
** Descriptions:       Write target location to drive
** input parameters:   ID:ID of the current drive
                       val: target location of the drive
** output parameters:  Output command array
** Returned value:     None
*********************************************************************************************************/

uint8_t Write_BLA_Position(uint8_t ID,int16_t val,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_TARGET_POSITION_INDEX & 0xFF;
    array[6] = (MEMORY_TARGET_POSITION_INDEX >> 8) & 0xFF;
    array[7] = val & 0xFF;
    array[8] = (val >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:       void Write_BLA_Speed(uint8_t ID,int16_t val)
** Descriptions:        Write target speed to drive
** input parameters:    ID:ID of the current drive
                        val: target speed of the drive
** output parameters:   Output command array
** Returned value:      None
*********************************************************************************************************/

uint8_t Write_BLA_Speed(uint8_t ID,int16_t val,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_TARGET_SPEED_INDEX & 0xFF;
    array[6] = (MEMORY_TARGET_SPEED_INDEX >> 8) & 0xFF;
    array[7] = val & 0xFF;
    array[8] = (val >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:      void Write_BLA_Force(uint8_t ID,int16_t val) 
** Descriptions:       Write target force to drive
** input parameters:   ID:ID of the current drive
                       val: target force of the drive
** output parameters:  Output command array 
** Returned value:     None
*********************************************************************************************************/

uint8_t Write_BLA_Force(uint8_t ID,int16_t val,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_TARGET_FORCE_INDEX & 0xFF;
    array[6] = (MEMORY_TARGET_FORCE_INDEX >> 8) & 0xFF;
    array[7] = val & 0xFF;
    array[8] = (val >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:      void Change_BLA_Mode(uint8_t ID,int16_t val) 
** Descriptions:       Change drive work mode
** input parameters:   ID:ID of the current drive
                       val: target work mode of the drive, 0-Position Mode, 4-Force Control Mode
** output parameters:  Output command array 
** Returned value:     None
*********************************************************************************************************/

uint8_t Change_BLA_Mode(uint8_t ID,int16_t val,uint8_t array[])
{
	uint8_t  i = 0;
	uint8_t checksum = 0;
    uint8_t send_num = 10;
    array[0] = SEND_FRAME_HEAD1;
    array[1] = SEND_FRAME_HEAD2;
    array[2] = send_num - 5;
    array[3] = ID;
    array[4] = CMD_WR_REGISTER;
    array[5] = MEMORY_WORK_MODE_INDEX & 0xFF;
    array[6] = (MEMORY_WORK_MODE_INDEX >> 8) & 0xFF;
    array[7] = val & 0xFF;
    array[8] = (val >> 8) & 0xFF;
	for(i=2;i<(send_num-1);i++)
	{
        checksum += array[i];
	}
    array[send_num-1] = checksum & 0xFF;
    return send_num;
}

/*********************************************************************************************************
** Function name:     void Uart_Rece_Data(volatile uint8_t *strd,uint8_t val)  
** Descriptions:      Sort the data received by the serial port 
** input parameters:  strd:Storage array of serial port received data
                      val:Length of serial port received data
** output parameters: Storage array
** Returned value:    None  
*********************************************************************************************************/

void Uart_Rece_Data(volatile uint8_t *strd,uint8_t val)
{
	int16_t k = 0;	
	UART_RX_Num = val; 
	if(UART_RX_Num != 0)
	{
		for(k=0;k<UART_RX_Num;k++)
		{
			UART_Recv_Buf[k] = strd[k];
		}
    	Uart_Rece_Data_Parsing();		
	}
}

/*********************************************************************************************************
** Function name:       void Uart_Rece_Data_Parsing()
** Descriptions:        Parse drive data
** input parameters:    None
** output parameters:   Drive data
** Returned value:      None
*********************************************************************************************************/

void Uart_Rece_Data_Parsing()
{
	uint8_t  CheckSum = 0;
	uint8_t  RXData_USART = 0;
	uint8_t  RX_Cnt_UART = 0;
	uint8_t  Rx_Data_UARTCOM = 0;
	uint16_t i = 0,j = 0;
    uint16_t UART_Rx_Data_Lens = 0;
	for(j=0;j<UART_RX_Num;j++)
	{
		RXData_USART = UART_Recv_Buf[j];
		Rx_Data_UARTCOM = RXData_USART;
		if(RX_Cnt_UART == 0)
		{
			if(Rx_Data_UARTCOM == RCV_FRAME_HEAD1)
			{
				UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
				RX_Cnt_UART++;	
			}			
		}
		else if(RX_Cnt_UART == 1)
		{
			if(Rx_Data_UARTCOM == RCV_FRAME_HEAD2)
			{
				UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
				RX_Cnt_UART++;	
			}	
			else if(Rx_Data_UARTCOM == RCV_FRAME_HEAD1)
			{
				RX_Cnt_UART = 0;
				UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
				RX_Cnt_UART++;	
			}
		}
		else if(RX_Cnt_UART == 2)
		{
			UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
			UART_Rx_Data_Lens = Rx_Data_UARTCOM;
			RX_Cnt_UART++;				
		}	
		else if(RX_Cnt_UART == 3)
		{
		  	UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
			RX_Cnt_UART++;	
		}			
		else if(RX_Cnt_UART == (UART_Rx_Data_Lens+4))
		{
			UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
			RX_Cnt_UART = 0;
			CheckSum = 0;
			for(i=2;i<(UART_Rx_Data_Lens+4);i++)
			{
				CheckSum += UART_Recv_Buf[i];
			}
			if(CheckSum == UART_Recv_Buf[UART_Rx_Data_Lens+4])
			{
				UART_Cmd_RT_ID = UART_Recv_Buf[3]; 
				if(UART_Recv_Buf[4] == CMD_RD_STATUS || UART_Recv_Buf[4] == CMD_WR_REGISTER)
				{
					Actuator_Status.Current_ID = UART_Recv_Buf[3];                                      //Drv ID act
					Actuator_Status.Current_Position = (UART_Recv_Buf[7]&0xFF) + (UART_Recv_Buf[8]<<8); //Drv pos act
					Actuator_Status.Current_Current = (UART_Recv_Buf[9]&0xFF) + (UART_Recv_Buf[10]<<8); //Drv current act
					Actuator_Status.Current_Force = (UART_Recv_Buf[11]&0xFF) + (UART_Recv_Buf[12]<<8);  //Drv force act
					Actuator_Status.Current_Speed = (UART_Recv_Buf[13]&0xFF) + (UART_Recv_Buf[14]<<8);  //Drv speed act
					Actuator_Status.Error_Code = (UART_Recv_Buf[15]&0xFF) + (UART_Recv_Buf[16]<<8);  	//Drv error code
					Actuator_Status.Current_Temperature = (UART_Recv_Buf[17]&0xFF) + (UART_Recv_Buf[18]<<8);  //Drv temperature
			  }				
		  	}
	  	}
		else
		{
			UART_Recv_Buf[RX_Cnt_UART] = Rx_Data_UARTCOM;
			RX_Cnt_UART++;
		}
	}
}
