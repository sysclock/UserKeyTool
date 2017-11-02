#define __D_IIC0_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC0.c                                                   */
/*																			*/
/* USAGE:       This module contains functions for general i2c0 protocol	*/
/*              Communication.                                              */
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*  I N C L U D E    F I L E S                                              */
/****************************************************************************/

#include <string.h>
#include <stdio.h>                		// prototype declarations for I/O functions

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "../User/i2c.h"
#include "../System Code/Timer.h"
#include "D_IIC0.H"

#define IIC0_Debug	0

#define SetI2C_SCL	GPIO_SetBits(GPIOB,EEPROM_SCL_PIN)								// Set SCL
#define ClrI2C_SCL	GPIO_ResetBits(GPIOB,EEPROM_SCL_PIN)							// Clear SCL
#define CheckI2C_SCL	GPIO_ReadInputDataBit(GPIOB, EEPROM_SCL_PIN)		// Check SCL
#define SetI2C_SDA	GPIO_SetBits(GPIOB,EEPROM_SDA_PIN)				// Set SDA
#define ClrI2C_SDA	GPIO_ResetBits(GPIOB,EEPROM_SDA_PIN)			// Clear SDA
#define CheckI2C_SDA	GPIO_ReadInputDataBit(GPIOB, EEPROM_SDA_PIN)		// Check SDA


/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern volatile unsigned long gD_IICDelayTickCnt;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void I2C0_Init			(void);
void I2C0_IRQHandler		(void);				//i2c0 interrupt routine
BYTE I2C0WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff);
BYTE* I2C0ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count);

/****************************************************************************/
/*  S T A T I C   F U N C T I O N    P R O T O T Y P E S                    */
/****************************************************************************/
BYTE	gB_I2C0ReceiveBuf[SIO_RxBufLen] = {0};
//BYTE	gB_I2C0SendBuf[SIO_RxBufLen] = {0};

static void i2c_Init		(void);
static BYTE i2c_Start      (BYTE B_Device);
static void i2c_Stop       (void);
static BYTE i2c_RcvAck		(void);
static BYTE i2c_RcvByte	(void);
static BYTE i2c_RcvByteAck	(void);
static BYTE i2c_SendByte	(BYTE B_Data);
static BYTE i2c_SendData	(BYTE B_Device, BYTE B_SubAddr, BYTE B_Data);
static BYTE i2c_RcvData    (BYTE B_Device, BYTE B_SubAddr);
static BYTE i2c_SendBufSeq	(BYTE B_Device, BYTE *Bp_DataTable);
static void i2c_Wait(void);

/****************************************************************************/
/*  C O D E                                                                 */
/****************************************************************************/

void i2c_Wait(void)
{
	BYTE i = 0, j = 0;
	for(i = 0; i< 5; i++)
	{
		for(j=0;j<8;j++)
			;
	}
}
/****************************************************************************/
/*																			*/
/* FUNCTION:	void i2c0_Init												*/
/*																			*/
/* USAGE:		Initialize the i2c0 bus.  On reset, it is prudent to send	*/
/*				stop bits to clear any data remaining to be placed on the	*/
/*				bus by other i2c0 devices.  In particular, if the system	*/
/*				is reset before ACK is sent from a slave device, the		*/
/*				device may issue an ACK on the first SCL pulse.  The		*/
/*				effect of this two-fold:  First, the intended initial i2c0	*/
/*				operation is rendered ineffective, which may cause failure 	*/
/*				of device initialization.  Second, the device issuing the	*/
/*				ACK believes that there is no break in the i2c0 command		*/
/*				(because it did not see a start/stop) and therefore an		*/
/*				unintended result occurs (ie. read failure or erroneous		*/
/*				write.)														*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void I2C0_Init(void)
{
/*
	I2C0Status = I2C_BUSY;
	// initialize the IIC interface
  LPC_SC->PCONP |= (1 << 7);						// 打开I2C0功率控制 
  // set PIO0.27 and PIO0.28 to I2C0 SDA and SCL function to 01 on both SDA and SCL.
  LPC_PINCON->PINSEL1 &= ~((0x03<<22)|(0x03<<24));
  LPC_PINCON->PINSEL1 |= ((0x01<<22)|(0x01<<24));	
  // Clear flags
  LPC_I2C0->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    
  //--- Reset registers ---
#if I2C0_FAST_MODE_PLUS == 1
  LPC_PINCON->I2CPADCFG |= ((0x1<<0)|(0x1<<2));
  LPC_I2C0->SCLL   = I2SCLL_HS_SCLL;			//Set bit rate Pclk/ (I2SCLH + I2SCLL) = 25M/(25+25) = 500Khz
  LPC_I2C0->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_PINCON->I2CPADCFG &= ~((0x1<<0)|(0x1<<2));
  LPC_I2C0->SCLL   = I2SCLL_SCLL << 1;					//Set bit rate Pclk/ (I2SCLH + I2SCLL) = 25M/(125+125) = 100Khz
  LPC_I2C0->SCLH   = I2SCLH_SCLH << 1;
#endif

  // Install interrupt handler
  NVIC_EnableIRQ(I2C0_IRQn);

	I2C0Data = NULL;
	I2C0Counter = 0;
	I2C0Address = 0;
	I2C0MemAddress = 0;
	I2C0MemAddressType = 0;
	I2C0Status = I2C_OK;
	memset((void*) gB_I2C0ReceiveBuf, 0, sizeof((void*) gB_I2C0ReceiveBuf));

	LPC_I2C0->CONSET = I2CONSET_I2EN;
	printf("I2C0 init OK!\r\n");
*/
//	i2c_Init();
	memset(gB_I2C0ReceiveBuf, 0, sizeof(gB_I2C0ReceiveBuf));
}

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE I2C0WriteByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count, BYTE* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write Byte data to slave through i2c0 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              (BYTE) ERR - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE I2C0WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff)
{
	BYTE *pBuffer = pBuff, i = 0;
	if (W_Count == 0)
		return (BYTE) ERR;
	while(i2c_Start(B_I2CAddr))           		// i2c Start
	{
		i++;
		Delay1ms(1);
		if(i > 5)
		{
			i2c_Stop();								// i2c STOP
			#if(IIC0_Debug == 1)
			printf("IIC0 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  (BYTE) ERR;
		}
	}

 	if(B_MemAddrType == 1)
	{
		i2c_SendByte((BYTE) (W_MemAddr >> 8) & 0xFF);	       			// Data address
	}
	i2c_SendByte((BYTE) W_MemAddr);	       			// Data address
	while(W_Count)
	{
		i2c_SendByte((BYTE)(*pBuffer));  			// Write lower WORD of data
		pBuffer++;
		// Decrement the read bytes counter 
		W_Count--;
	}
	i2c_Stop();								// i2c STOP
	//
	// Wait till write completed
	//
	while(i2c_Start(B_I2CAddr));      		// i2c device address
	i2c_Stop();								// i2c STOP
    return (OK);
}
/*
BYTE I2C0WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff)
{
	BYTE* pBuffer = pBuff;
	if (W_Count == 0)
		return (BYTE) ERR;
	
	//起始位
	I2CStart();
	//-- 发送器件地址(写)
	I2CWriteByte(B_I2CAddr);
	if( RESET == I2CSlaveAck() )  	
	{
		#if(IIC0_Debug == 1)
		printf("IIC0 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
		#endif
		I2CStop();
		return  (BYTE) ERR;
	}

	//--发送地址
	if(B_MemAddrType == 1)
	{
		I2CWriteByte((W_MemAddr >> 8) & 0xFF);  
		if( RESET == I2CSlaveAck() )  	
		{
			#if(IIC0_Debug == 1)
			printf("IIC0 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			I2CStop();
			return  (BYTE) ERR;
		}
	}
	I2CWriteByte (W_MemAddr & 0xFF);
	if( RESET == I2CSlaveAck() )  	
	{
		#if(IIC0_Debug == 1)
		printf("IIC0 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
		#endif
		I2CStop();
		return  (BYTE) ERR;
	}
	while(W_Count--)  
	{
	  I2CWriteByte(*pBuffer); 
	  pBuffer++; 
		if( RESET == I2CSlaveAck() )  	
		{
			#if(IIC0_Debug == 1)
			printf("IIC0 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			I2CStop();
			return  (BYTE) ERR;
		}
	}
	
	//停止位
	I2CStop();
	return OK;
}
*/
/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE* I2C0ReadByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count)									*/
/*                                                                          		*/
/* USAGE:       read Byte data from slave through i2c0 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read Byte data											*/
/*              W_Count -	read Byte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              (BYTE) ERR - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE* I2C0ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count)
{
	BYTE *pBuffer = NULL, i = 0;
	memset(gB_I2C0ReceiveBuf, 0, sizeof(gB_I2C0ReceiveBuf));
	pBuffer = gB_I2C0ReceiveBuf;
	if (W_Count <= 0)
		return NULL;
	while(i2c_Start(B_I2CAddr))          		// i2c Start
	{
		i++;
		Delay1ms(1);
		if(i > 5)
		{
			i2c_Stop();								// i2c STOP
			#if(IIC0_Debug == 1)
			printf("IIC0 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
 	if(B_MemAddrType == 1)
	{
		i2c_SendByte((BYTE) (W_MemAddr >> 8) & 0xFF);	       			// Data address
	}
	i2c_SendByte((BYTE) W_MemAddr);	       			// Data address
	i2c_Start(B_I2CAddr | BIT0);   				// i2c Start
	while(W_Count)
	{	
		if(W_Count == 1)
			*pBuffer = i2c_RcvByte();
		else
			*pBuffer = i2c_RcvByteAck();
		pBuffer++;
		// Decrement the read bytes counter 
		W_Count--;
	}
  i2c_Stop();                         		// i2c STOP
	return gB_I2C0ReceiveBuf;
}
/*
BYTE* I2C0ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count)
{
	BYTE* pBuffer = NULL;
	memset((void*) gB_I2C0ReceiveBuf, 0, sizeof((void*) gB_I2C0ReceiveBuf));
	pBuffer = gB_I2C0ReceiveBuf;
	if (W_Count <= 0)
		return NULL;
	I2CStart();
	//发送器件地址(写)
  I2CWriteByte( B_I2CAddr );
  if( RESET == I2CSlaveAck() )  	
	{
		#if(IIC0_Debug == 1)
		printf("IIC0 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
		#endif
		I2CStop();
		return  NULL;
	}
	//发送地址	 	  
	if(B_MemAddrType == 1)
	{
		I2CWriteByte ((W_MemAddr >> 8) & 0xFF);  
		if( RESET == I2CSlaveAck() )  	
		{
			#if(IIC0_Debug == 1)
			printf("IIC0 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			I2CStop();
			return  NULL;
		}
	}
	I2CWriteByte (W_MemAddr & 0xFF);
	if( RESET == I2CSlaveAck() )  	
	{
		#if(IIC0_Debug == 1)
		printf("IIC0 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
		#endif
		I2CStop();
		return  NULL;
	}
	//起始位
	I2CStart();
	//发送器件地址(读)
	I2CWriteByte( B_I2CAddr | 0x01 );
	if( RESET == I2CSlaveAck() )  	
	{
		#if(IIC0_Debug == 1)
		printf("IIC0 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
		#endif
		I2CStop();
		return  NULL;
	}
	while(W_Count)
	{	
		*pBuffer = I2CReadByte();
		pBuffer++;
		if(W_Count != 1)
		I2CMasterAck();
		// Decrement the read bytes counter 
		W_Count--;
	}
	I2CStop();			//发送停止位

	return gB_I2C0ReceiveBuf;               // if operation was successful then return read value
	
}

*/
void I2C0_IRQHandler		(void)						//I2C interrupt routine
{
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	void i2c_Init												*/
/*																			*/
/* USAGE:		Initialize the i2c bus.  On reset, it is prudent to send	*/
/*				stop bits to clear any data remaining to be placed on the	*/
/*				bus by other i2c devices.  In particular, if the system		*/
/*				is reset before ACK is sent from a slave device, the		*/
/*				device may issue an ACK on the first SCL pulse.  The		*/
/*				effect of this two-fold:  First, the intended initial i2c	*/
/*				operation is rendered ineffective, which may cause failure 	*/
/*				of device initialization.  Second, the device issuing the	*/
/*				ACK believes that there is no break in the i2c command		*/
/*				(because it did not see a start/stop) and therefore an		*/
/*				unintended result occurs (ie. read failure or erroneous		*/
/*				write.)														*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void i2c_Init(void)
{
	// first clear SCL to complete slave device ACK, if it was in the
	// process of doing so.
	//
	ClrI2C_SCL;                    // Clear SCL
	i2c_Wait();                     // Delay

	// now send a complete i2c stop
	//
	ClrI2C_SDA;                    // Clear SDA
	i2c_Wait();                     // Delay
	SetI2C_SCL;                    // Set SCL
	i2c_Wait();                     // Delay
	SetI2C_SDA;                    // Set SDA
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    BYTE i2c_Start(BYTE B_Device)								*/
/*																			*/
/* USAGE:       Issue I2C START condition and slave device address.			*/
/*              Check device acknowledge after sending.						*/
/*              Generates STOP if no ACK received.							*/
/*																			*/
/* INPUT:       B_Device = Slave address device								*/
/*																			*/
/* OUTPUT:      OK 		 - successful										*/
/*              (BYTE) ERR - no ACK from device								*/
/*																			*/
/****************************************************************************/
BYTE i2c_Start(BYTE B_Device) 
{
    BYTE B_Count;

    //
    // Generate I2C START
    //
		TimerStart		(IIC0_10ms_TMR1, 2);
    SetI2C_SCL;                    	// Set SCL
    while(!(CheckI2C_SCL))        	// Wait until bus is released by slave
		{
			if (TimerCheck(IIC0_10ms_TMR1) == TIMER_TMO)
			{
				printf("1 Set I2C SCL Error\r\n");
				TimerStop(IIC0_10ms_TMR1);
        i2c_Stop();                 // Issue I2C STOP
        return ((BYTE) ERR);
			}
		}
    SetI2C_SDA;                    	// Set SDA
    i2c_Wait();                     // Delay
    ClrI2C_SDA;                    	// Clear SDA
    i2c_Wait();                     // Delay
    ClrI2C_SCL;                    	// Clear SCL

    // 
    // Send slave device address
	//
    B_Count = 0x80;					// Initialize bit count
    while (B_Count)
    {
		if (B_Count & B_Device)		// Check data bit
            SetI2C_SDA;            // Set SDA
        else
            ClrI2C_SDA;            // Clear SDA
        i2c_Wait();                 // Delay
        SetI2C_SCL;                // Set SCL
        i2c_Wait();                 // Delay
        ClrI2C_SCL;                // Clear SCL
        B_Count = B_Count >> 1;     // Shift bit position to right
    }
    
	//
    // Check I2C ACK
 	//
		TimerStop(IIC0_10ms_TMR1);
    if (i2c_RcvAck())				// ACK seen ?
    {								// No
        i2c_Stop();                 // Issue I2C STOP
        return ((BYTE) ERR);
    }
    else							// Yes
        return (OK);				// I2c START OK
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	void i2c_Stop(void)											*/
/*																			*/
/* USAGE:		Issue I2C STOP condition.									*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void i2c_Stop(void) 
{
    ClrI2C_SDA;                    // Clear SDA
    i2c_Wait();                     // Delay
    SetI2C_SCL;                    // Set SCL
    i2c_Wait();                     // Delay
    SetI2C_SDA;                    // Set SDA
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	BYTE i2c_RcvAck(void)										*/
/*																			*/
/* USAGE:		Read SDA line for slave acknowledgement.					*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		0 = data ACK												*/
/* 				1 = no ACK													*/
/*																			*/
/****************************************************************************/
BYTE i2c_RcvAck(void) 
{
    BYTE B_Ack;
		GPIO_InitTypeDef GPIO_InitStruct;

    SetI2C_SDA;                    // Set SDA 

		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
		GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    i2c_Wait();                     // Delay 
    SetI2C_SCL;                    // Set SCL 
    i2c_Wait();                     // Delay 
    B_Ack = CheckI2C_SDA;          // Check SDA line 
    ClrI2C_SCL;                    // Clear SCL 
	
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOB, &GPIO_InitStruct );
		return (B_Ack);                 // Return SDA status 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE i2c_RcvByte(void)                                     	*/
/*                                                                          */
/* USAGE:       Receive a byte of data from slave in I2C bus.               */
/*                                                                          */
/* INPUT:		None														*/
/*                                                                          */
/* OUTPUT:      data received                                               */
/*                                                                          */
/****************************************************************************/
BYTE i2c_RcvByte(void)
{
    BYTE B_Data, B_Count;
		GPIO_InitTypeDef GPIO_InitStruct;

    B_Data = 0;              		// Init data receive 
    B_Count = 0x80;                 // Setup bit Count 
    SetI2C_SDA;                    // Set SDA 
 
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
		GPIO_Init( GPIOB, &GPIO_InitStruct );
	
		while (B_Count)
    {    
        SetI2C_SCL;             	  // Set SCL 
				TimerStart		(IIC0_10ms_TMR1, 1);
        while(!(CheckI2C_SCL))	    // Wait until bus is released by slave 
				{
					if (TimerCheck(IIC0_10ms_TMR1) == TIMER_TMO)
					{
						printf("2 Set I2C SCL Error\r\n");
						TimerStop(IIC0_10ms_TMR1);
						i2c_Stop();                 // Issue I2C STOP
						return ((BYTE) ERR);
					}
				}
        if (CheckI2C_SDA)						// Read data bit
            B_Data |= B_Count;      // data bit = 1 
        else    
            B_Data &= (~B_Count);		// data bit = 0 
        B_Count = B_Count >> 1;			// Shift bit position to right 
        i2c_Wait();                 // Delay 
        ClrI2C_SCL;                	// Clear SCL 
        i2c_Wait();                 // Delay 
    }
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOB, &GPIO_InitStruct );
		TimerStop(IIC0_10ms_TMR1);
    return (B_Data);    			// Return received data
} 

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE i2c_RcvWithAck()                                      	*/
/*                                                                          */
/* USAGE:       Receive a byte of data from slave in I2C bus				*/
/*              and generate ACK.                                           */
/*                                                                          */
/* INPUT:		None														*/
/*                                                                          */
/* OUTPUT:      data received                                        		*/
/*                                                                          */
/****************************************************************************/
BYTE i2c_RcvByteAck(void) 
{
    BYTE B_Data, B_Count;
		GPIO_InitTypeDef GPIO_InitStruct;

	//
	// Receive data from i2c bus
	//
    B_Data = 0;              		// Init data receive 
    B_Count = 0x80;                 // Setup bit Count 
    SetI2C_SDA;                    // Set SDA 

		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
		GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    while (B_Count)
    {    
        SetI2C_SCL;                // Set SCL 
				TimerStart		(IIC0_10ms_TMR1, 1);
        while(!(CheckI2C_SCL))	    // Wait until bus is released by slave 
				{
					if (TimerCheck(IIC0_10ms_TMR1) == TIMER_TMO)
					{
						printf("3 Set I2C SCL Error\r\n");
						TimerStop(IIC0_10ms_TMR1);
						i2c_Stop();                 // Issue I2C STOP
						return ((BYTE) ERR);
					}
				}
        if (CheckI2C_SDA)			// Read data bit
            B_Data |= B_Count;      // data bit = 1 
        else    
            B_Data &= (~B_Count);	// data bit = 0 
        B_Count = B_Count >> 1;		// Shift bit position to right 
        i2c_Wait();                 // Delay 
        ClrI2C_SCL;                // Clear SCL 
        i2c_Wait();                 // Delay 
    }
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOB, &GPIO_InitStruct );

	//
	// Send an ACK
	//
    ClrI2C_SDA;                    // Send ACK to slave
    SetI2C_SCL;                    // Set SCL
    i2c_Wait();                     // Delay
    ClrI2C_SCL;                    // Clear SCL
		TimerStop(IIC0_10ms_TMR1);
    return (B_Data);    			// Return received data
} 

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE i2c_SendByte(BYTE B_data)								*/
/*                                                                          */
/* USAGE:       Send a data byte to slave through I2C bus and wait for     	*/
/*              ACK from slave.                                             */
/*                                                                          */
/* INPUT:       B_Data - data write to i2c bus                          	*/
/*                                                                          */
/* OUTPUT:      OK  	 - successful                                       */
/*              (BYTE) ERR - no ACK                                           */
/*                                                                          */
/****************************************************************************/
BYTE i2c_SendByte(BYTE B_Data) 
{
    BYTE B_Count;

	//
	// Setup i2c bus line
	// 
    B_Count = 0x80;					// Initialize bit count
    if (B_Count & B_Data)           // Check data bit
        SetI2C_SDA;                // Set SDA 
    else
        ClrI2C_SDA;                // Clear SDA 
    i2c_Wait();                     // Delay 
    SetI2C_SCL;                    // Set SCL 
		TimerStart		(IIC0_10ms_TMR1, 1);
    while(!(CheckI2C_SCL))	        // Wait until bus is released by slave 
		{
			if (TimerCheck(IIC0_10ms_TMR1) == TIMER_TMO)
			{
				printf("3 Set I2C SCL Error\r\n");
				TimerStop(IIC0_10ms_TMR1);
        i2c_Stop();                 // Issue I2C STOP
        return ((BYTE) ERR);
			}
		}
    i2c_Wait();                     // Delay 
    ClrI2C_SCL;                    // Clear SCL 

	//
	// Shift 8 data bit out
	//
    B_Count = B_Count >> 1;			// Shift bit position to right 
    while (B_Count)
    {
        if (B_Count & B_Data)       // Check data bit 
            SetI2C_SDA;            // Set SDA 
        else
            ClrI2C_SDA;            // Clear SDA 
        i2c_Wait();                 // Delay 
        SetI2C_SCL;                // Set SCL 
        i2c_Wait();                 // Delay 
        ClrI2C_SCL;                // Clear SCL 
        B_Count = B_Count >> 1;		// Shift bit position to right 
    }
    
	//
    // Check i2c ACK
	//
		TimerStop(IIC0_10ms_TMR1);
    if (i2c_RcvAck())            	// ACK seen ? 
    {								// No
        i2c_Stop();                	// Issue I2C STOP 
        return ((BYTE) ERR);		
    }
    else							// Yes
        return (OK);				// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE i2c_SendData(BYTE B_Device, BYTE SubAddr, BYTE B_Data)	*/
/*                                                                          */
/* USAGE:       Write a byte of data to a selected device through I2C bus   */
/*                                                                          */
/* INPUT:       B_Device  - Slave device address                            */
/*              B_SubAddr - Register address inside the device				*/
/*              B_Data    - Data write to device                    		*/
/*                                                                          */
/* OUTPUT:      OK       - successful                                       */
/*              (BYTE) ERR - no ACK                                           */
/*                                                                          */
/****************************************************************************/
BYTE i2c_SendData(BYTE B_Device, BYTE B_SubAddr, BYTE B_Data) 
{
    if (i2c_Start(B_Device))					// i2c START, device ready ?
        return ((BYTE) ERR);						// Return if No ACK

    i2c_SendByte(B_SubAddr);		 			// Register address
    i2c_SendByte(B_Data);		 				// Register data
    i2c_Stop();                  				// I2C STOP
    return (OK);                    			// Return OK
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE i2c_RcvData(BYTE B_Device, BYTE B_SubAddr)				*/
/*                                                                          */
/* USAGE:       Read One byte data from selected device through I2C bus     */
/*                                                                          */
/* INPUT:       B_Device  - Slave device address                            */
/*              B_SubAddr - Register address inside the device              */
/*                                                                          */
/* OUTPUT:      Read data                                                   */
/*                                                                          */
/****************************************************************************/
BYTE i2c_RcvData(BYTE B_Device, BYTE B_SubAddr) 
{
	BYTE B_Data;

    if (i2c_Start(B_Device))					// i2c START, device ready ?
        return ((BYTE) ERR);						// Return if No ACK

    i2c_SendByte(B_SubAddr);					// Send i2c Sub-Addr to the device 
    if (i2c_Start(B_Device | BIT0))				// i2c Start for read 
        return ((BYTE) ERR);						// Return if No ACK

    B_Data = i2c_RcvByte();						// Read one byte 
    i2c_Stop();									// Send i2c STOP
    return (B_Data);							// Return read byte 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    void i2c_SendBufSeq(BYTE B_Device, BYTE *Bp_DataTable)		*/
/*                                                                          */
/* USAGE:       Write data from table into I2C device registers.			*/
/* 				register data need to be in sequential order.           	*/
/*                                                                          */
/* INPUT:		B_Device     = slave device address							*/
/*				Bp_DataTable = pointer to table contains the register table	*/
/* 				1st & 2nd bytes - Register address start					*/
/*				3th & 4th bytes - Table register length						*/
/*				5th byte and forward - register data						*/
/*                                                                          */
/* OUTPUT:      None                                                        */
/*                                                                          */
/* NOTE:		It assume the register address and register length is BYTE.	*/
/*                                                                          */
/****************************************************************************/
BYTE i2c_SendBufSeq(BYTE B_Device, BYTE *Bp_DataTable)
{
	BYTE B_Length;

	if (i2c_Start(B_Device))					// I2C Start
        return ((BYTE) ERR);						// Return if No ACK
												//
	B_Length = Bp_DataTable[3];					// Number of byte to load
    i2c_SendByte(Bp_DataTable[1]);	 			// Register address
	Bp_DataTable += 4;							// Skip header data
	while (B_Length--)
	{
	    i2c_SendByte(*Bp_DataTable++);			// Register data
	}
    i2c_Stop();		                  			// I2C STOP
    return (OK);                    			// Return OK
}
void i2c_DummyCall(void)
{
	i2c_Init();
	i2c_Start(0);
	i2c_Stop();
	i2c_RcvAck();
	i2c_RcvByte();
	i2c_RcvByteAck();
	i2c_SendByte(0);
	i2c_SendData(0, 0, 0);
	i2c_RcvData(0, 0);
	i2c_SendBufSeq(0, NULL);
	i2c_Wait();
}

/********************************  END  *************************************/

