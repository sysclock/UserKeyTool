#define __D_IIC1_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC1.c	                                                */
/*																			*/
/* USAGE:       This module contains functions for general I2C1 protocol	*/
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

#include "../System Code/lpc17xx.h"
#include "../System Code/ASCII.h"
#include "../System Code/global.h"
#include "../System Code/type.h"
#include "../System Code/Timer.h"
#include "D_IIC1.H"

#define IIC1_Debug	0
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern volatile unsigned long gD_IICDelayTickCnt;

/****************************************************************************/
/*  S T A T I C   F U N C T I O N    P R O T O T Y P E S                    */
/****************************************************************************/
volatile BYTE	*I2C1Data;
volatile BYTE	I2C1Counter,	I2C1Address;
volatile BYTE	I2C1State;						// holds the status - LOWER_ADDRESS, DATA BYTE, etc.
volatile BYTE	I2C1Status;						// holds the state - I2C_ERROR, I2C_OK or I2C_BUSY
volatile BYTE	I2C1Operation;					// holds the operation - READ or WRITE
volatile BYTE	I2C1MemAddressType;			// holds the Memory Address Type - 0 8bit, 1 16bit.
volatile WORD	I2C1MemAddress;
BYTE	gB_I2C1ReceiveBuf[SIO_RxBufLen] = {0};

/****************************************************************************/
/*	G L O B A L    D A T A    T A B L E										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void I2C1_Init			(void);
void I2C1_IRQHandler		(void);				//i2c1 interrupt routine
BYTE I2C1WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff);
BYTE I2C1WriteByteEx(BYTE B_I2CPort, BYTE B_SlaveAddr);
BYTE* I2C1ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count);

/****************************************************************************/
/*  C O D E                                                                 */
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* FUNCTION:	void i2c1_Init												*/
/*																			*/
/* USAGE:		Initialize the i2c1 bus.  On reset, it is prudent to send	*/
/*				stop bits to clear any data remaining to be placed on the	*/
/*				bus by other i2c1 devices.  In particular, if the system	*/
/*				is reset before ACK is sent from a slave device, the		*/
/*				device may issue an ACK on the first SCL pulse.  The		*/
/*				effect of this two-fold:  First, the intended initial I2C1	*/
/*				operation is rendered ineffective, which may cause failure 	*/
/*				of device initialization.  Second, the device issuing the	*/
/*				ACK believes that there is no break in the i2c1 command		*/
/*				(because it did not see a start/stop) and therefore an		*/
/*				unintended result occurs (ie. read failure or erroneous		*/
/*				write.)														*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void I2C1_Init(void)
{
	I2C1Status = I2C_BUSY;
	// initialize the IIC interface
  LPC_SC->PCONP |= (1 << 19);						// 打开I2C1功率控制 
#if 1
  // set PIO0.0 and PIO0.1 to I2C1 SDA and SCL function to 11 on both SDA and SCL.
  LPC_PINCON->PINSEL0 &= ~((0x3<<0)|(0x3<<2));
  LPC_PINCON->PINSEL0 |= ((0x3<<0)|(0x3<<2));
  LPC_PINCON->PINMODE0 &= ~((0x3<<0)|(0x3<<2));
  LPC_PINCON->PINMODE0 |= ((0x2<<0)|(0x2<<2));					// No pull-up no pull-down
  LPC_PINCON->PINMODE_OD0 |= ((0x01<<0)|(0x1<<1));			// Open drain
#endif
#if 0
  // set PIO0.19 and PIO0.20 to I2C1 SDA and SCL function to 11 on both SDA and SCL.
  LPC_PINCON->PINSEL1 &= ~((0x3<<6)|(0x3<<8));
  LPC_PINCON->PINSEL1 |= ((0x3<<6)|(0x3<<8));
  LPC_PINCON->PINMODE1 &= ~((0x3<<6)|(0x3<<8));
  LPC_PINCON->PINMODE1 |= ((0x2<<6)|(0x2<<8));					// No pull-up no pull-down
  LPC_PINCON->PINMODE_OD0 |= ((0x1<<19)|(0x1<<20));	
#endif
  // Clear flags
  LPC_I2C1->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    
  //--- Reset registers ---
  LPC_I2C1->SCLL   = I2SCLL_SCLL;					//Set bit rate Pclk/ (I2SCLH + I2SCLL) = 25M/(125+125) = 100Khz
  LPC_I2C1->SCLH   = I2SCLH_SCLH;

  // Install interrupt handler
  NVIC_EnableIRQ(I2C1_IRQn);

	I2C1Data = NULL;
	I2C1Counter = 0;
	I2C1Address = 0;
	I2C1MemAddress = 0;
	I2C1MemAddressType = 0;
	I2C1Status = I2C_OK;
	memset((void*) gB_I2C1ReceiveBuf, 0, sizeof((void*) gB_I2C1ReceiveBuf));

	LPC_I2C1->CONSET = I2CONSET_I2EN;
	printf("I2C1 init OK!\r\n");
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    void I2C1_SelectDevice (BYTE B_Device)                      */
/*                                                                          */
/* USAGE:       Save the I2C device address and access this device from     */
/*              now on (support multiple device).                           */
/*                                                                          */
/* INPUT:       B_DeviceName - I2C device device name                       */
/*                                                                          */
/* OUTPUT:      None                                                        */
/*                                                                          */
/****************************************************************************/

const I2C1_DEVICE* I2C1_SelectDevice (BYTE B_DeviceName)
{
	/*
  const I2C1_DEVICE* Stp_DEVICE = I2C1_DEVICETable;
	BYTE B_Count = 0;
	while (Stp_DEVICE->Name != I2C1_DEVICE_MAX)
	{
		if (Stp_DEVICE->Name == B_DeviceName)
		{
			IOSET0 = (WORD) 1 << 31;			// Disable 4051,I2CS_EN = 1;
			//printf("IOSET0 0x%x\n", IOPIN0);
		    // 
		    // Set I2C_SEL
			//
		    B_Count = 0x04;						// Initialize bit count
			if (B_Count & Stp_DEVICE->SelectAddr)		// Check data bit and set I2C_SEL2
				IOSET0 = 1 << 26;
			else
				IOCLR0 = 1 << 26;
			B_Count >>= 1;
			if (B_Count & Stp_DEVICE->SelectAddr)		// Check data bit and set I2C_SEL1
				IOSET0 = 1 << 25;
			else
				IOCLR0 = 1 << 25;
			B_Count >>= 1;
			if (B_Count & Stp_DEVICE->SelectAddr)		// Check data bit and set I2C_SEL0
				IOSET0 = 1 << 23;
			else
				IOCLR0 = 1 << 23;
			IOCLR0 = (WORD) 1 << 31;			// Enable 4051,I2CS_EN = 0;
			return Stp_DEVICE;
		}
		Stp_DEVICE++;
	}
	*/
	return NULL;
}

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE I2C1WriteByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count, BYTE* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write Byte data to slave through I2C1 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE I2C1WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff)
{
	unsigned long 	D_TempData = 0;
	if (W_Count == 0)
		return (BYTE) ERR;
	while(I2C1Status == I2C_BUSY);
	//memcpy(gB_I2C1SendBuf, pBuff, W_Count);
	//I2C1Data = gB_I2C1SendBuf;
//	Stp_DEVICE = I2C1_SelectDevice(B_DeviceName);
	I2C1Data = pBuff;
	I2C1Counter = W_Count;
	I2C1Address = B_I2CAddr;
//	printf("Stp_DEVICE->I2CAddr 0x%x\n", Stp_DEVICE->I2CAddr);
	I2C1MemAddress = W_MemAddr;
	if(B_MemAddrType == I2C_DEV_MEMORY_ADDRESS_16BIT)
		I2C1MemAddressType = 1;
	else
		I2C1MemAddressType = 0;
	I2C1Status = I2C_BUSY;                      // I2C is now busy
	I2C1Operation = WRITE;                      // a write operation is being performed
	//Clear all I2C settings
	LPC_I2C1->CONCLR 	= I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
	//Enable the I2C interface and Start condition
	LPC_I2C1->CONSET 	= I2CONSET_I2EN | I2CONSET_STA;
//	while(I2C1Status == I2C_BUSY) ; 	        // wait until I2C is no longer busy
	
	gD_IICDelayTickCnt = 0;
	while(I2C1Status == I2C_BUSY)
	{
		D_TempData = gD_IICDelayTickCnt / 1000;
		if(D_TempData >= 5)
		{
			gD_IICDelayTickCnt = 0;
			LPC_I2C1->CONSET = I2CONSET_STO;	      // stop condition
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
			I2C1Status = I2C_ERROR;
			printf("IIC1 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			break;
		}
	}
	
	if (I2C1Status == I2C_OK)
	{
		return OK;               	// if operation was successful then return read value
	}
	else
	{
		return (BYTE) ERR;                             // return a constant value if there was an error
	}
}

BYTE I2C1WriteByteEx(BYTE B_I2CPort, BYTE B_SlaveAddr)
{
	unsigned long 	D_TempData = 0;
	BYTE i = 0x08;
	I2C1Data = &i;
	I2C1Counter = 1;
	I2C1Address = B_SlaveAddr;
//	printf("Stp_DEVICE->I2CAddr 0x%x\n", Stp_DEVICE->I2CAddr);
	I2C1MemAddress = 0x07;
	I2C1MemAddressType = 0;
	I2C1Status = I2C_BUSY;                      // I2C is now busy
	I2C1Operation = WRITE;                      // a write operation is being performed
	//Clear all I2C settings
	LPC_I2C1->CONCLR 	= I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
	//Enable the I2C interface and Start condition
	LPC_I2C1->CONSET 	= I2CONSET_I2EN | I2CONSET_STA;
//	while(I2C1Status == I2C_BUSY) ; 	        // wait until I2C is no longer busy
	
	gD_IICDelayTickCnt = 0;
	while(I2C1Status == I2C_BUSY)
	{
		D_TempData = gD_IICDelayTickCnt / 1000;
		if(D_TempData >= 5)
		{
			gD_IICDelayTickCnt = 0;
			LPC_I2C1->CONSET = I2CONSET_STO;	      // stop condition
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
			I2C1Status = I2C_ERROR;
			printf("IIC1 Write ERROR: SlaveAddr = 0x%02X\r\n", B_SlaveAddr);
			break;
		}
	}
	
	if (I2C1Status == I2C_OK)
	{
		return OK;               	// if operation was successful then return read value
	}
	else
	{
		return (BYTE) ERR;                             // return a constant value if there was an error
	}
}

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE* I2C1ReadByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count)									*/
/*                                                                          		*/
/* USAGE:       read Byte data from slave through I2C1 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read Byte data											*/
/*              W_Count -	read Byte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE* I2C1ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count)
{
	unsigned long 	D_TempData = 0;
	if (W_Count <= 0)
		return NULL;
	while(I2C1Status == I2C_BUSY);
	memset((void*) gB_I2C1ReceiveBuf, 0, sizeof((void*) gB_I2C1ReceiveBuf));
	I2C1Data = gB_I2C1ReceiveBuf;
	I2C1Counter = W_Count;
	I2C1Address = B_I2CAddr;
	I2C1MemAddress = W_MemAddr;
	if(B_MemAddrType == I2C_DEV_MEMORY_ADDRESS_16BIT)
		I2C1MemAddressType = 1;
	else
		I2C1MemAddressType = 0;
	I2C1Status = I2C_BUSY;                      // I2C is now busy
	I2C1Operation = READ;                      	// a write operation is being performed
	I2C1State = DUMMY_WRITE;                     // initially a dummy write is to be performed
	//Clear all I2C settings
	LPC_I2C1->CONCLR 	= I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
	//Enable the I2C interface and Start condition
	LPC_I2C1->CONSET 	= I2CONSET_I2EN | I2CONSET_STA;
//	while(I2C1Status == I2C_BUSY) ;             // wait until I2C is no longer busy

	gD_IICDelayTickCnt = 0;
	while(I2C1Status == I2C_BUSY)
	{
		D_TempData = gD_IICDelayTickCnt / 1000;
		if(D_TempData >= 5)
		{
			gD_IICDelayTickCnt = 0;
			LPC_I2C1->CONSET = I2CONSET_STO;	      // stop condition
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
			I2C1Status = I2C_ERROR;
			printf("IIC1 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			break;
		}
	}

	if (I2C1Status == I2C_OK)
	{
		return gB_I2C1ReceiveBuf;               // if operation was successful then return read value
	}
	else
	{
		return NULL;                             // return a constant value if there was an error
	}
}

void I2C1_IRQHandler		(void)						//I2C 1 interrupt routine
{
  BYTE StatValue;
  // this handler deals with I2C0 master read and master write only
  StatValue = LPC_I2C1->STAT;
	switch (StatValue)								//Read result code and switch to next action
	{
		// Start and Send byte conditions
		
		case 0x08:									//Start bit
			if (I2C1Operation == READ && I2C1State != DUMMY_WRITE) // if performing a read operation and dummy write is completed then...
			{
				LPC_I2C1->DAT = I2C1Address | 0x01;      	// transmit EEPROM address + read command (SLA + R)
				#if	(IIC1_Debug == 1)
					printf("0x08 send slave add 0x%02X\r\n", LPC_I2C1->DAT);
				#endif
			}
			else
			{
				LPC_I2C1->DAT = I2C1Address & 0xFE;      	// transmit EEPROM address + write command (SLA + W)
																									// this situation will arise if no resonse was received from the EEPROM
																									// and another attempt is being made
				TimerStart(IIC1_10ms_TMR1, 1);
				#if	(IIC0_Debug == 1)
					printf("0x08 send slave add 0x%02X\r\n", LPC_I2C1->DAT);
				#endif
			}
			LPC_I2C1->CONSET = I2CONSET_AA;
			LPC_I2C1->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;  // clear start condition flag so another start condition is not requested
			break;
		
		// REPEATED START CONDITION TRANSMITTED
	    case 0x10:
			if (I2C1Operation == READ && I2C1State != DUMMY_WRITE) // if performing a read operation and dummy write is completed then...
			{
				LPC_I2C1->DAT = I2C1Address | 0x01;      						// transmit EEPROM address + read command (SLA + R)
				#if	(IIC1_Debug == 1)
					printf("0x10 send read I2C1Addr\r\n");
				#endif
			}
			else
			{
				LPC_I2C1->DAT = I2C1Address & 0xFE;      	// transmit EEPROM address + write command (SLA + W)
																									// this situation will arise if no resonse was received from the EEPROM
																									// and another attempt is being made
				#if	(IIC1_Debug == 1)
					printf("0x10 send write I2C1Add\r\n");
				#endif
			}
			LPC_I2C1->CONSET = I2CONSET_AA;
			LPC_I2C1->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;		// clear start condition flag so another start condition is not requested
			break;

		case 0x18:									//Slave address+W, ACK
			TimerStop(IIC1_10ms_TMR1);
			if(	I2C1MemAddressType == 1)
			{
				LPC_I2C1->DAT = I2C1MemAddress >> 8;				//Write Memory start address to tx register
				I2C1State = LOWER_ADDRESS;	        	// next byte to be transmitted is lower part of address
				#if	(IIC1_Debug == 1)
					printf("0x18 send write MemAddr Hi 0x%02X\r\n", LPC_I2C1->DAT);
				#endif
				LPC_I2C1->CONSET = I2CONSET_AA;
				LPC_I2C1->CONCLR = I2CONCLR_SIC;     //
			}
			else
			{
					LPC_I2C1->DAT = I2C1MemAddress & 0xFF; 	// transmit memory of address
					I2C1State = DATA_BYTE;              		// next time around transmit the data byte
					#if	(IIC1_Debug == 1)
						printf("0x28 send write MemAddr Low 0x%02X\r\n", LPC_I2C1->DAT);
					#endif
					LPC_I2C1->CONSET = I2CONSET_AA;
					LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
			}
			break;

		case (0x20):
			#if 0
				TimerStop(IIC1_10ms_TMR1);
				LPC_I2C1->CONSET = I2CONSET_STO;	      // stop condition
				LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
				I2C1Status = I2C_ERROR;                 // status of operation is ERROR
				break;
			#endif
			if(TimerCheck(IIC1_10ms_TMR1) == TIMER_TMO)
			{
				TimerStop(IIC1_10ms_TMR1);
				LPC_I2C1->CONSET = I2CONSET_STO;	      // stop condition
				LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
				I2C1Status = I2C_ERROR;                 // status of operation is ERROR
			}
			else
			{
				LPC_I2C1->CONSET = I2CONSET_STA | I2CONSET_AA;		// Restart IIC
				LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                 //
				#if	(IIC1_Debug == 1)
					printf("0x20 send restart IIC 1 bus\r\n");
				#endif
			}
			break;

		case (0x28):
			switch	(I2C1State)
			{
				case LOWER_ADDRESS:                   		// next byte to transmit is lower part of address
					LPC_I2C1->DAT = I2C1MemAddress & 0xFF; 	// transmit lower 8 bits of address
					I2C1State = DATA_BYTE;              		// next time around transmit the data byte
					#if	(IIC1_Debug == 1)
						printf("0x28 send write MemAddr Low 0x%02X\r\n", LPC_I2C1->DAT);
					#endif
					LPC_I2C1->CONSET = I2CONSET_AA;
					LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
					break;
				case DATA_BYTE:                       // next byte to transmit is the data byte
					if (I2C1Operation == READ)          // if performing a read then the dummy write cycle
																							// ends here. no data byte is transmitted
					{
						LPC_I2C1->CONSET = I2CONCLR_STAC;
						LPC_I2C1->CONCLR 	= I2CONCLR_SIC;
						#if	(IIC1_Debug == 1)
							printf("0x28 send start, I2C1CONSET 0x%02X\r\n", LPC_I2C1->CONSET);
						#endif
					}
					else
					{
						if(I2C1Counter >= 1)					//Data sent, Ack
						{
							LPC_I2C1->DAT = *I2C1Data;			//Write data to tx register
							#if	(IIC1_Debug == 1)
								printf("0x28 data send 0x%02X, I2C1Data addr %d \r\n", LPC_I2C1->DAT, I2C1Data);
							#endif
							I2C1Counter--;
							I2C1Data++;
							LPC_I2C1->CONSET = I2CONSET_AA;
							LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
						}
						else
						{
							I2C1State = FINISHED;                  	//Signal end of I2C1C activity
							LPC_I2C1->CONSET = I2CONSET_STO;
							LPC_I2C1->CONCLR 	= I2CONCLR_SIC;      	//
							I2C1Status = I2C_OK;                		// status of operation is OK
							#if	(IIC1_Debug == 1)
								printf("0x28 send stop\r\n");
							#endif
						}
					}
				break;
				// finished writing byte
				case FINISHED:         		            	// data byte was successfully transmitted
					LPC_I2C1->CONSET = I2CONSET_STO;     	// request to transmit a stop condition
					LPC_I2C1->CONCLR 	= I2CONCLR_SIC;
					I2C1Status = I2C_OK;                	// status of operation is OK
					#if	(IIC1_Debug == 1)
						printf("0x28 send stop\r\n");
					#endif
				break;
			}
			break;

		// DATA BYTE OR UPPER ADDRESS OR LOWER ADDRESS TRANSMITTED - NO ACK RECEIVED
		case 0x30	:								//Data sent, NOT Ack
			LPC_I2C1->CONSET = I2CONSET_STO;
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
			I2C1Status = I2C_ERROR;                 // status of operation is ERROR
			#if	(IIC1_Debug == 1)
				printf("0x30 I2C_ERROR\r\n");
			#endif
			break;
		
		case 0x38	:								//
			LPC_I2C1->CONSET = I2CONSET_STA | I2CONSET_AA;
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
			I2C1Status = I2C_ERROR;                 // status of operation is ERROR
			#if	(IIC1_Debug == 1)
				printf("0x38 I2C_Restart\r\n");
			#endif
			break;
		
		// SLAVE ADDRESS + READ TRANSMITTED - ACK RECEIVED
		case 0x40:
			LPC_I2C1->CONSET = I2CONSET_AA;
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;                    //
			#if	(IIC1_Debug == 1)
				printf("0x40\r\n");
			#endif
			break;
		
		// SLAVE ADDRESS + READ TRANSMITTED - NO ACK RECEIVED
		case 0x48 :									//Slave Address +R, Not Ack
			LPC_I2C1->CONSET = I2CONSET_STO;	      //stop condition
			LPC_I2C1->CONCLR 	= I2CONCLR_SIC;       //
			I2C1Status = I2C_ERROR;                 // status of operation is ERROR
			#if	(IIC1_Debug == 1)
				printf("0x48\r\n");
			#endif
			break;
		
		case 0x50 :								//Data Received, ACK
			*I2C1Data 	= LPC_I2C1->DAT;
			#if	(IIC1_Debug == 1)
				printf("0x50 data receive 0x%02X, I2C1Data addr %d\r\n", *I2C1Data, I2C1Data);
			#endif
			I2C1Counter--;
			I2C1Data++;
			if(I2C1Counter == 1)
				LPC_I2C1->CONCLR = I2CONCLR_SIC | I2CONCLR_AAC;
			else if(I2C1Counter == 0)
			{
				LPC_I2C1->CONSET = I2CONSET_STO;	      	//stop condition
				LPC_I2C1->CONCLR = I2CONCLR_SIC; 	      	//
				I2C1Status = I2C_OK;                   		//Signal end of I2C0C activity							
			}
			else
			{
				LPC_I2C1->CONSET = I2CONSET_AA;
				LPC_I2C1->CONCLR = I2CONCLR_SIC;
			}
			break;
		
		case (0x58):									//Data Received, Not Ack
			*I2C1Data 	= LPC_I2C1->DAT;
			#if	(IIC1_Debug == 1)
				printf("0x58 data receive 0x%02X, I2C1Data addr %d\r\n", *I2C1Data, I2C1Data);
			#endif
			LPC_I2C1->CONSET = I2CONSET_STO;	      	//stop condition
			LPC_I2C1->CONCLR = I2CONCLR_SIC; 	      	//
			I2C1Status = I2C_OK;                   		//Signal end of I2C1C activity							
			break;

		default :
			LPC_I2C1->CONSET = I2CONSET_STO;	      	//stop condition
			LPC_I2C1->CONCLR = I2CONCLR_SIC;  	     	//
			I2C1Status = I2C_ERROR;             	    // status of operation is ERROR
			#if	(IIC1_Debug == 1)
				printf("I2C default\r\n");
			#endif
			break;
		
	}
	#if	(IIC1_Debug == 1)
		printf("exit I2C 1 irq\r\n");
	#endif
}
/********************************  END  *************************************/
