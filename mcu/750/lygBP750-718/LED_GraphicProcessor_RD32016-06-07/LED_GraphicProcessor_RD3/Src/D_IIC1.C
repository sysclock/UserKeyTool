#define __D_IIC1_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC1.c                                                   */
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

#include "stm32f1xx_hal.h"
#include "Timer.h"
#include "D_IIC1.H"

#define I2C1_EXTERN_DEBUG	0

#define I2C1_MCU_PORT			GPIOB
#define I2C1_SCL_PIN			GPIO_PIN_6
#define I2C1_SDA_PIN			GPIO_PIN_7

#define SetI2C1_SCL		HAL_GPIO_WritePin(I2C1_MCU_PORT, I2C1_SCL_PIN, GPIO_PIN_SET)					// Set SCL
#define ClrI2C1_SCL		HAL_GPIO_WritePin(I2C1_MCU_PORT, I2C1_SCL_PIN, GPIO_PIN_RESET)				// Clear SCL
#define CheckI2C1_SCL	HAL_GPIO_ReadPin(I2C1_MCU_PORT, I2C1_SCL_PIN)													// Check SCL
#define SetI2C1_SDA		HAL_GPIO_WritePin(I2C1_MCU_PORT, I2C1_SDA_PIN, GPIO_PIN_SET)					// Set SDA
#define ClrI2C1_SDA		HAL_GPIO_WritePin(I2C1_MCU_PORT, I2C1_SDA_PIN, GPIO_PIN_RESET)				// Clear SDA
#define CheckI2C1_SDA	HAL_GPIO_ReadPin(I2C1_MCU_PORT, I2C1_SDA_PIN)													// Check SDA


/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern volatile unsigned long gD_IICDelayTickCnt;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
#ifdef USE_SIMULATED_I2C
void I2C1_Init			(void);
void I2C1_IRQHandler		(void);				//i2c0 interrupt routine
uint8_t I2C1WriteByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count, uint8_t* pBuff);
uint8_t* I2C1ReadByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count);
#endif
/****************************************************************************/
/*  S T A T I C   F U N C T I O N    P R O T O T Y P E S                    */
/****************************************************************************/
#ifdef USE_SIMULATED_I2C
uint8_t	gB_I2C1ReceiveBuf[SIO_RxBufLen] = {0};
#endif

static void i2c_Init		(void);
static uint8_t i2c_Start      (uint8_t B_Device);
static void i2c_Stop       (void);
static uint8_t i2c_RcvAck		(void);
static uint8_t i2c_RcvByte	(void);
static uint8_t i2c_RcvByteAck	(void);
static uint8_t i2c_SendByte	(uint8_t B_Data);
static uint8_t i2c_SendData	(uint8_t B_Device, uint8_t B_SubAddr, uint8_t B_Data);
static uint8_t i2c_RcvData    (uint8_t B_Device, uint8_t B_SubAddr);
static uint8_t i2c_SendBufSeq	(uint8_t B_Device, uint8_t *Bp_DataTable);
static void i2c1_Wait(void);

/****************************************************************************/
/*  C O D E                                                                 */
/****************************************************************************/

void i2c1_Wait(void)
{
	uint8_t i = 0, j = 0;
	for(i = 0; i< 5; i++)
	{
		for(j = 0; j < 8; j++)
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
#ifdef USE_SIMULATED_I2C
void I2C1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = I2C1_SCL_PIN|I2C1_SDA_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
	memset(gB_I2C1ReceiveBuf, 0, sizeof(gB_I2C1ReceiveBuf));
	printf("I2C1 init OK!\r\n");
}
#endif
/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    uint8_t I2C1WriteByte(uint8_t B_I2CAddr, uint32_t W_MemAddr,					*/
/*									uint32_t W_Count, uint8_t* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write Byte data to slave through i2c0 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              (uint8_t) ERR - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifdef USE_SIMULATED_I2C
uint8_t I2C1WriteByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count, uint8_t* pBuff)
{
	uint8_t *pBuffer = pBuff, i = 0;
	if (W_Count == 0)
		return (uint8_t) ERR;
	while(i2c_Start(B_I2CAddr))           		// i2c Start
	{
		i++;
		Delay1ms(1);
		if(i > 5)
		{
			i2c_Stop();								// i2c STOP
			#if(I2C1_EXTERN_DEBUG == 1)
			printf("IIC1 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  (uint8_t) ERR;
		}
	}

 	if(B_MemAddrType == 1)
	{
		i2c_SendByte((uint8_t) (W_MemAddr >> 8) & 0xFF);	       			// Data address
	}
	i2c_SendByte((uint8_t) W_MemAddr);	       			// Data address
	while(W_Count)
	{
		i2c_SendByte((uint8_t)(*pBuffer));  			// Write lower uint32_t of data
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
#endif

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    uint8_t* I2C1ReadByte(uint8_t B_I2CAddr, uint32_t W_MemAddr,					*/
/*									uint32_t W_Count)									*/
/*                                                                          		*/
/* USAGE:       read Byte data from slave through i2c0 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read Byte data											*/
/*              W_Count -	read Byte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              (uint8_t) ERR - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifdef USE_SIMULATED_I2C
uint8_t* I2C1ReadByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count)
{
	uint8_t *pBuffer = NULL, i = 0;
	memset(gB_I2C1ReceiveBuf, 0, sizeof(gB_I2C1ReceiveBuf));
	pBuffer = gB_I2C1ReceiveBuf;
	if (W_Count <= 0)
		return NULL;
	while(i2c_Start(B_I2CAddr))          		// i2c Start
	{
		i++;
		Delay1ms(1);
		if(i > 5)
		{
			i2c_Stop();								// i2c STOP
			#if(I2C1_EXTERN_DEBUG == 1)
			printf("IIC1 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
 	if(B_MemAddrType == 1)
	{
		i2c_SendByte((uint8_t) (W_MemAddr >> 8) & 0xFF);	       			// Data address
	}
	i2c_SendByte((uint8_t) W_MemAddr);	       			// Data address
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
	return gB_I2C1ReceiveBuf;
}
#endif

#ifdef USE_SIMULATED_I2C
void I2C1_IRQHandler		(void)						//I2C interrupt routine
{
}
#endif
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
static void i2c_Init(void)
{
	// first clear SCL to complete slave device ACK, if it was in the
	// process of doing so.
	//
	ClrI2C1_SCL;                    // Clear SCL
	i2c1_Wait();                     // Delay

	// now send a complete i2c stop
	//
	ClrI2C1_SDA;                    // Clear SDA
	i2c1_Wait();                     // Delay
	SetI2C1_SCL;                    // Set SCL
	i2c1_Wait();                     // Delay
	SetI2C1_SDA;                    // Set SDA
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    uint8_t i2c_Start(uint8_t B_Device)								*/
/*																			*/
/* USAGE:       Issue I2C START condition and slave device address.			*/
/*              Check device acknowledge after sending.						*/
/*              Generates STOP if no ACK received.							*/
/*																			*/
/* INPUT:       B_Device = Slave address device								*/
/*																			*/
/* OUTPUT:      OK 		 - successful										*/
/*              (uint8_t) ERR - no ACK from device								*/
/*																			*/
/****************************************************************************/
static uint8_t i2c_Start(uint8_t B_Device) 
{
    uint8_t B_Count;

    //
    // Generate I2C START
    //
		TimerStart		(IIC1_10ms_TMR1, 2);
    SetI2C1_SCL;                    	// Set SCL
    while(!(CheckI2C1_SCL))        	// Wait until bus is released by slave
		{
			if (TimerCheck(IIC1_10ms_TMR1) == TIMER_TMO)
			{
				printf("1 Set I2C SCL Error\r\n");
				TimerStop(IIC1_10ms_TMR1);
        i2c_Stop();                 // Issue I2C STOP
        return ((uint8_t) ERR);
			}
		}
    SetI2C1_SDA;                    	// Set SDA
    i2c1_Wait();                     // Delay
    ClrI2C1_SDA;                    	// Clear SDA
    i2c1_Wait();                     // Delay
    ClrI2C1_SCL;                    	// Clear SCL

    // 
    // Send slave device address
	//
    B_Count = 0x80;					// Initialize bit count
    while (B_Count)
    {
		if (B_Count & B_Device)		// Check data bit
            SetI2C1_SDA;            // Set SDA
        else
            ClrI2C1_SDA;            // Clear SDA
        i2c1_Wait();                 // Delay
        SetI2C1_SCL;                // Set SCL
        i2c1_Wait();                 // Delay
        ClrI2C1_SCL;                // Clear SCL
        B_Count = B_Count >> 1;     // Shift bit position to right
    }
    
	//
    // Check I2C ACK
 	//
		TimerStop(IIC1_10ms_TMR1);
    if (i2c_RcvAck())				// ACK seen ?
    {								// No
        i2c_Stop();                 // Issue I2C STOP
        return ((uint8_t) ERR);
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
static void i2c_Stop(void) 
{
    ClrI2C1_SDA;                    // Clear SDA
    i2c1_Wait();                     // Delay
    SetI2C1_SCL;                    // Set SCL
    i2c1_Wait();                     // Delay
    SetI2C1_SDA;                    // Set SDA
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	uint8_t i2c_RcvAck(void)										*/
/*																			*/
/* USAGE:		Read SDA line for slave acknowledgement.					*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		0 = data ACK												*/
/* 				1 = no ACK													*/
/*																			*/
/****************************************************************************/
static uint8_t i2c_RcvAck(void) 
{
    uint8_t B_Ack;
		GPIO_InitTypeDef GPIO_InitStruct;

    SetI2C1_SDA;                    // Set SDA 

		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pin = I2C1_SDA_PIN;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
	
    i2c1_Wait();                     // Delay 
    SetI2C1_SCL;                    // Set SCL 
    i2c1_Wait();                     // Delay 
    B_Ack = CheckI2C1_SDA;          // Check SDA line 
    ClrI2C1_SCL;                    // Clear SCL 
	
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
		return (B_Ack);                 // Return SDA status 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t i2c_RcvByte(void)                                     	*/
/*                                                                          */
/* USAGE:       Receive a byte of data from slave in I2C bus.               */
/*                                                                          */
/* INPUT:		None														*/
/*                                                                          */
/* OUTPUT:      data received                                               */
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_RcvByte(void)
{
    uint8_t B_Data, B_Count;
		GPIO_InitTypeDef GPIO_InitStruct;

    B_Data = 0;              		// Init data receive 
    B_Count = 0x80;                 // Setup bit Count 
    SetI2C1_SDA;                    // Set SDA 
 
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pin = I2C1_SDA_PIN;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
		while (B_Count)
    {    
        SetI2C1_SCL;             	  // Set SCL 
				TimerStart		(IIC1_10ms_TMR1, 1);
        while(!(CheckI2C1_SCL))	    // Wait until bus is released by slave 
				{
					if (TimerCheck(IIC1_10ms_TMR1) == TIMER_TMO)
					{
						printf("2 Set I2C SCL Error\r\n");
						TimerStop(IIC1_10ms_TMR1);
						i2c_Stop();                 // Issue I2C STOP
						return ((uint8_t) ERR);
					}
				}
        if (CheckI2C1_SDA)						// Read data bit
            B_Data |= B_Count;      // data bit = 1 
        else    
            B_Data &= (~B_Count);		// data bit = 0 
        B_Count = B_Count >> 1;			// Shift bit position to right 
        i2c1_Wait();                 // Delay 
        ClrI2C1_SCL;                	// Clear SCL 
        i2c1_Wait();                 // Delay 
    }
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
		TimerStop(IIC1_10ms_TMR1);
    return (B_Data);    			// Return received data
} 

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t i2c_RcvWithAck()                                      	*/
/*                                                                          */
/* USAGE:       Receive a byte of data from slave in I2C bus				*/
/*              and generate ACK.                                           */
/*                                                                          */
/* INPUT:		None														*/
/*                                                                          */
/* OUTPUT:      data received                                        		*/
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_RcvByteAck(void) 
{
    uint8_t B_Data, B_Count;
		GPIO_InitTypeDef GPIO_InitStruct;

	//
	// Receive data from i2c bus
	//
    B_Data = 0;              		// Init data receive 
    B_Count = 0x80;                 // Setup bit Count 
    SetI2C1_SDA;                    // Set SDA 

		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  //这里一定要设成输入上拉，否则不能读出数据
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pin = I2C1_SDA_PIN;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);
    while (B_Count)
    {    
        SetI2C1_SCL;                // Set SCL 
				TimerStart		(IIC1_10ms_TMR1, 1);
        while(!(CheckI2C1_SCL))	    // Wait until bus is released by slave 
				{
					if (TimerCheck(IIC1_10ms_TMR1) == TIMER_TMO)
					{
						printf("3 Set I2C SCL Error\r\n");
						TimerStop(IIC1_10ms_TMR1);
						i2c_Stop();                 // Issue I2C STOP
						return ((uint8_t) ERR);
					}
				}
        if (CheckI2C1_SDA)			// Read data bit
            B_Data |= B_Count;      // data bit = 1 
        else    
            B_Data &= (~B_Count);	// data bit = 0 
        B_Count = B_Count >> 1;		// Shift bit position to right 
        i2c1_Wait();                 // Delay 
        ClrI2C1_SCL;                // Clear SCL 
        i2c1_Wait();                 // Delay 
    }
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(I2C1_MCU_PORT, &GPIO_InitStruct);

	//
	// Send an ACK
	//
    ClrI2C1_SDA;                    // Send ACK to slave
    SetI2C1_SCL;                    // Set SCL
    i2c1_Wait();                     // Delay
    ClrI2C1_SCL;                    // Clear SCL
		TimerStop(IIC1_10ms_TMR1);
    return (B_Data);    			// Return received data
} 

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t i2c_SendByte(uint8_t B_data)								*/
/*                                                                          */
/* USAGE:       Send a data byte to slave through I2C bus and wait for     	*/
/*              ACK from slave.                                             */
/*                                                                          */
/* INPUT:       B_Data - data write to i2c bus                          	*/
/*                                                                          */
/* OUTPUT:      OK  	 - successful                                       */
/*              (uint8_t) ERR - no ACK                                           */
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_SendByte(uint8_t B_Data) 
{
    uint8_t B_Count;

	//
	// Setup i2c bus line
	// 
    B_Count = 0x80;					// Initialize bit count
    if (B_Count & B_Data)           // Check data bit
        SetI2C1_SDA;                // Set SDA 
    else
        ClrI2C1_SDA;                // Clear SDA 
    i2c1_Wait();                     // Delay 
    SetI2C1_SCL;                    // Set SCL 
		TimerStart		(IIC1_10ms_TMR1, 1);
    while(!(CheckI2C1_SCL))	        // Wait until bus is released by slave 
		{
			if (TimerCheck(IIC1_10ms_TMR1) == TIMER_TMO)
			{
				printf("3 Set I2C SCL Error\r\n");
				TimerStop(IIC1_10ms_TMR1);
        i2c_Stop();                 // Issue I2C STOP
        return ((uint8_t) ERR);
			}
		}
    i2c1_Wait();                     // Delay 
    ClrI2C1_SCL;                    // Clear SCL 

	//
	// Shift 8 data bit out
	//
    B_Count = B_Count >> 1;			// Shift bit position to right 
    while (B_Count)
    {
        if (B_Count & B_Data)       // Check data bit 
            SetI2C1_SDA;            // Set SDA 
        else
            ClrI2C1_SDA;            // Clear SDA 
        i2c1_Wait();                 // Delay 
        SetI2C1_SCL;                // Set SCL 
        i2c1_Wait();                 // Delay 
        ClrI2C1_SCL;                // Clear SCL 
        B_Count = B_Count >> 1;		// Shift bit position to right 
    }
    
	//
    // Check i2c ACK
	//
		TimerStop(IIC1_10ms_TMR1);
    if (i2c_RcvAck())            	// ACK seen ? 
    {								// No
        i2c_Stop();                	// Issue I2C STOP 
        return ((uint8_t) ERR);		
    }
    else							// Yes
        return (OK);				// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t i2c_SendData(uint8_t B_Device, uint8_t SubAddr, uint8_t B_Data)	*/
/*                                                                          */
/* USAGE:       Write a byte of data to a selected device through I2C bus   */
/*                                                                          */
/* INPUT:       B_Device  - Slave device address                            */
/*              B_SubAddr - Register address inside the device				*/
/*              B_Data    - Data write to device                    		*/
/*                                                                          */
/* OUTPUT:      OK       - successful                                       */
/*              (uint8_t) ERR - no ACK                                           */
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_SendData(uint8_t B_Device, uint8_t B_SubAddr, uint8_t B_Data) 
{
    if (i2c_Start(B_Device))					// i2c START, device ready ?
        return ((uint8_t) ERR);						// Return if No ACK

    i2c_SendByte(B_SubAddr);		 			// Register address
    i2c_SendByte(B_Data);		 				// Register data
    i2c_Stop();                  				// I2C STOP
    return (OK);                    			// Return OK
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t i2c_RcvData(uint8_t B_Device, uint8_t B_SubAddr)				*/
/*                                                                          */
/* USAGE:       Read One byte data from selected device through I2C bus     */
/*                                                                          */
/* INPUT:       B_Device  - Slave device address                            */
/*              B_SubAddr - Register address inside the device              */
/*                                                                          */
/* OUTPUT:      Read data                                                   */
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_RcvData(uint8_t B_Device, uint8_t B_SubAddr) 
{
	uint8_t B_Data;

    if (i2c_Start(B_Device))					// i2c START, device ready ?
        return ((uint8_t) ERR);						// Return if No ACK

    i2c_SendByte(B_SubAddr);					// Send i2c Sub-Addr to the device 
    if (i2c_Start(B_Device | BIT0))				// i2c Start for read 
        return ((uint8_t) ERR);						// Return if No ACK

    B_Data = i2c_RcvByte();						// Read one byte 
    i2c_Stop();									// Send i2c STOP
    return (B_Data);							// Return read byte 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    void i2c_SendBufSeq(uint8_t B_Device, uint8_t *Bp_DataTable)		*/
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
/* NOTE:		It assume the register address and register length is uint8_t.	*/
/*                                                                          */
/****************************************************************************/
static uint8_t i2c_SendBufSeq(uint8_t B_Device, uint8_t *Bp_DataTable)
{
	uint8_t B_Length;

	if (i2c_Start(B_Device))					// I2C Start
        return ((uint8_t) ERR);						// Return if No ACK
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
	i2c1_Wait();
}

/********************************  END  *************************************/

