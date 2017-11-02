#define __D_IIC2_C__
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

#include "../System Code/lpc17xx.h"
#include "../System Code/ASCII.h"
#include "../System Code/global.h"
#include "../System Code/type.h"
#include "../System Code/Timer.h"
#include "D_IIC2.H"

#define IIC2_Debug	0
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern volatile unsigned long gD_IICDelayTickCnt;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void I2C2_Init			(void);
void I2C2_IRQHandler		(void);				//i2c2 interrupt routine
BYTE I2C2WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff);
BYTE* I2C2ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count);

/****************************************************************************/
/*  S T A T I C   F U N C T I O N    P R O T O T Y P E S                    */
/****************************************************************************/
volatile BYTE	*I2C2Data;
volatile BYTE	I2C2Counter,	I2C2Address;
volatile BYTE	I2C2State;						// holds the status - LOWER_ADDRESS, DATA BYTE, etc.
volatile BYTE	I2C2Status;						// holds the state - I2C_ERROR, I2C_OK or I2C_BUSY
volatile BYTE	I2C2Operation;					// holds the operation - READ or WRITE
volatile BYTE	I2C2MemAddressType;			// holds the Memory Address Type - 0 8bit, 1 16bit.
volatile WORD	I2C2MemAddress;
BYTE	gB_I2C2ReceiveBuf[SIO_RxBufLen] = {0};

/****************************************************************************/
/*  C O D E                                                                 */
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* FUNCTION:	void i2c2_Init												*/
/*																			*/
/* USAGE:		Initialize the i2c2 bus.  On reset, it is prudent to send	*/
/*				stop bits to clear any data remaining to be placed on the	*/
/*				bus by other i2c2 devices.  In particular, if the system	*/
/*				is reset before ACK is sent from a slave device, the		*/
/*				device may issue an ACK on the first SCL pulse.  The		*/
/*				effect of this two-fold:  First, the intended initial i2c2	*/
/*				operation is rendered ineffective, which may cause failure 	*/
/*				of device initialization.  Second, the device issuing the	*/
/*				ACK believes that there is no break in the i2c2 command		*/
/*				(because it did not see a start/stop) and therefore an		*/
/*				unintended result occurs (ie. read failure or erroneous		*/
/*				write.)														*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void I2C2_Init(void)
{
	I2C2Status = I2C_BUSY;
	// initialize the IIC interface
  LPC_SC->PCONP |= (1 << 26);						// 打开I2C2功率控制 
  // set PIO0.10 and PIO0.11 to I2C2 SDA and SCL function to 01 on both SDA and SCL.
  LPC_PINCON->PINSEL0 &= ~((0x03<<20)|(0x03<<22));
  LPC_PINCON->PINSEL0 |= ((0x02<<20)|(0x02<<22));
  LPC_PINCON->PINMODE0 &= ~((0x03<<20)|(0x03<<22));
  LPC_PINCON->PINMODE0 |= ((0x02<<20)|(0x2<<22));				// No pull-up no pull-down
  LPC_PINCON->PINMODE_OD0 |= ((0x01<<10)|(0x1<<11));	
  // Clear flags
  LPC_I2C2->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    
  //--- Reset registers ---
  LPC_I2C2->SCLL   = I2SCLL_SCLL;					//Set bit rate Pclk/ (I2SCLH + I2SCLL) = 25M/(125 + 125) = 100Khz
  LPC_I2C2->SCLH   = I2SCLH_SCLH;

  // Install interrupt handler
  NVIC_EnableIRQ(I2C2_IRQn);

	I2C2Data = NULL;
	I2C2Counter = 0;
	I2C2Address = 0;
	I2C2MemAddress = 0;
	I2C2MemAddressType = 0;
	I2C2Status = I2C_OK;
	memset((void*) gB_I2C2ReceiveBuf, 0, sizeof((void*) gB_I2C2ReceiveBuf));

	LPC_I2C2->CONSET = I2CONSET_I2EN;
	printf("I2C2 init OK!\r\n");
}

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE I2C2WriteByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count, BYTE* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write Byte data to slave through i2c2 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE I2C2WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff)
{
	unsigned long 	D_TempData = 0;
	if (W_Count == 0)
		return (BYTE) ERR;
	while(I2C2Status == I2C_BUSY);
	I2C2Data = pBuff;
	I2C2Counter = W_Count;
	I2C2Address = B_I2CAddr;
	I2C2MemAddress = W_MemAddr;
	if(B_MemAddrType == I2C_DEV_MEMORY_ADDRESS_16BIT)
		I2C2MemAddressType = 1;
	else
		I2C2MemAddressType = 0;
	I2C2Status = I2C_BUSY;                      // I2C2 is now busy
	I2C2Operation = WRITE;                      // a write operation is being performed
	//Clear all I2C settings
	LPC_I2C2->CONCLR 	= I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
	//Enable the I2C interface and Start condition
	LPC_I2C2->CONSET 	= I2CONSET_I2EN | I2CONSET_STA;
//	while(I2C2Status == I2C_BUSY) ; 	        // wait until I2C is no longer busy
	
	gD_IICDelayTickCnt = 0;
	while(I2C2Status == I2C_BUSY)
	{
		D_TempData = gD_IICDelayTickCnt / 1000;
		if(D_TempData >= 5)
		{
			gD_IICDelayTickCnt = 0;
			LPC_I2C2->CONSET = I2CONSET_STO;	      // stop condition
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;       //
			I2C2Status = I2C_ERROR;
			printf("IIC2 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			break;
		}
	}
	
	if (I2C2Status == I2C_OK)
	{
//		printf("Write OK I2C2Status %d\n", I2C2Status);
		return OK;               	// if operation was successful then return read value
	}
	else
	{
//		printf("Write ERROR SlaveAddr = 0x%04x, MemAddr = 0x%04X\n", B_I2CAddr, W_MemAddr);
		return (BYTE) ERR;                             // return a constant value if there was an error
	}
}

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    BYTE* I2C2ReadByte(BYTE B_I2CAddr, WORD W_MemAddr,					*/
/*									WORD W_Count)									*/
/*                                                                          		*/
/* USAGE:       read Byte data from slave through i2c2 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read Byte data											*/
/*              W_Count -	read Byte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
BYTE* I2C2ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count)
{
	unsigned long 	D_TempData = 0;
	if (W_Count <= 0)
		return NULL;
	while(I2C2Status == I2C_BUSY);
	memset((void*) gB_I2C2ReceiveBuf, 0, sizeof((void*) gB_I2C2ReceiveBuf));
	I2C2Data = gB_I2C2ReceiveBuf;
	I2C2Counter = W_Count;
	I2C2Address = B_I2CAddr;
	I2C2MemAddress = W_MemAddr;
	if(B_MemAddrType == I2C_DEV_MEMORY_ADDRESS_16BIT)
		I2C2MemAddressType = 1;
	else
		I2C2MemAddressType = 0;
	I2C2Status = I2C_BUSY;                      // I2C is now busy
	I2C2Operation = READ;                      	// a write operation is being performed
	I2C2State = DUMMY_WRITE;                     // initially a dummy write is to be performed
	//Clear all I2C settings
	LPC_I2C2->CONCLR 	= I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;
	//Enable the I2C interface and Start condition
	LPC_I2C2->CONSET 	= I2CONSET_I2EN | I2CONSET_STA;
//	while(I2C2Status == I2C_BUSY) ;             // wait until I2C is no longer busy

	gD_IICDelayTickCnt = 0;
	while(I2C2Status == I2C_BUSY)
	{
		D_TempData = gD_IICDelayTickCnt / 1000;
		if(D_TempData >= 5)
		{
			gD_IICDelayTickCnt = 0;
			LPC_I2C2->CONSET = I2CONSET_STO;	      // stop condition
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;       //
			I2C2Status = I2C_ERROR;
			printf("IIC2 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			break;
		}
	}

	if (I2C2Status == I2C_OK)
	{
//		printf("Read OK I2C0Status %d\n", I2C0Status);
		return gB_I2C2ReceiveBuf;               // if operation was successful then return read value
	}
	else
	{
//		printf("Read Error I2C0Status %d\n", I2C0Status);
		return NULL;                             // return a constant value if there was an error
	}
}

void I2C2_IRQHandler		(void)						//I2C interrupt routine
{
  BYTE StatValue;
  // this handler deals with I2C0 master read and master write only
  StatValue = LPC_I2C2->STAT;
	switch (StatValue)								//Read result code and switch to next action
	{
		// Start and Send byte conditions
		
		case 0x08:									//Start bit
			if (I2C2Operation == READ && I2C2State != DUMMY_WRITE) // if performing a read operation and dummy write is completed then...
			{
				LPC_I2C2->DAT = I2C2Address | 0x01;      	// transmit EEPROM address + read command (SLA + R)
				#if	(IIC2_Debug == 1)
					printf("0x08 send slave add 0x%02X\r\n", LPC_I2C2->DAT);
				#endif
			}
			else
			{
				LPC_I2C2->DAT = I2C2Address & 0xFE;      	// transmit EEPROM address + write command (SLA + W)
																									// this situation will arise if no resonse was received from the EEPROM
																									// and another attempt is being made
				TimerStart(IIC2_10ms_TMR1, 1);
				#if	(IIC2_Debug == 1)
					printf("0x08 send slave add 0x%02X\r\n", LPC_I2C2->DAT);
				#endif
			}
			LPC_I2C2->CONSET = I2CONSET_AA;
			LPC_I2C2->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;  // clear start condition flag so another start condition is not requested
			break;
		
		// REPEATED START CONDITION TRANSMITTED
	    case 0x10:
			if (I2C2Operation == READ && I2C2State != DUMMY_WRITE) // if performing a read operation and dummy write is completed then...
			{
				LPC_I2C2->DAT = I2C2Address | 0x01;      						// transmit EEPROM address + read command (SLA + R)
				#if	(IIC2_Debug == 1)
					printf("0x10 send read I2C2Addr\r\n");
				#endif
			}
			else
			{
				LPC_I2C2->DAT = I2C2Address & 0xFE;      	// transmit EEPROM address + write command (SLA + W)
																									// this situation will arise if no resonse was received from the EEPROM
																									// and another attempt is being made
				#if	(IIC2_Debug == 1)
					printf("0x10 send write I2C2Add\r\n");
				#endif
			}
			LPC_I2C2->CONSET = I2CONSET_AA;
			LPC_I2C2->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;		// clear start condition flag so another start condition is not requested
			break;

		case 0x18:									//Slave address+W, ACK
			TimerStop(IIC2_10ms_TMR1);
			if(	I2C2MemAddressType == 1)
			{
				LPC_I2C2->DAT = I2C2MemAddress >> 8;				//Write Mem,ory start address to tx register
				I2C2State = LOWER_ADDRESS;	        	// next byte to be transmitted is lower part of address
				#if	(IIC2_Debug == 1)
					printf("0x18 send write MemAddr Hi 0x%02X\r\n", LPC_I2C2->DAT);
				#endif
				LPC_I2C2->CONSET = I2CONSET_AA;
				LPC_I2C2->CONCLR = I2CONCLR_SIC;     //
			}
			else
			{
					LPC_I2C2->DAT = I2C2MemAddress & 0xFF; 	// transmit the whole memory of address
					I2C2State = DATA_BYTE;              		// next time around transmit the data byte
					#if	(IIC2_Debug == 1)
						printf("0x28 send write MemAddr Low 0x%02X\r\n", LPC_I2C2->DAT);
					#endif
					LPC_I2C2->CONSET = I2CONSET_AA;
					LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
			}
			break;

		case (0x20):
			if(TimerCheck(IIC2_10ms_TMR1) == TIMER_TMO)
			{
				TimerStop(IIC2_10ms_TMR1);
				LPC_I2C2->CONSET = I2CONSET_STO;	      // stop condition
				LPC_I2C2->CONCLR 	= I2CONCLR_SIC;       //
				I2C2Status = I2C_ERROR;                 // status of operation is ERROR
			}
			else
			{
				LPC_I2C2->CONSET = I2CONSET_STA | I2CONSET_AA;		// Restart IIC
				LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                 //
				#if	(IIC2_Debug == 1)
					printf("0x20 send restart IIC 2 bus\r\n");
				#endif
			}
			break;

		case (0x28):
			switch	(I2C2State)
			{
				case LOWER_ADDRESS:                   		// next byte to transmit is lower part of address
					LPC_I2C2->DAT = I2C2MemAddress & 0xFF; 	// transmit lower 8 bits of address
					I2C2State = DATA_BYTE;              		// next time around transmit the data byte
					#if	(IIC2_Debug == 1)
						printf("0x28 send write MemAddr Low 0x%02X\r\n", LPC_I2C2->DAT);
					#endif
					LPC_I2C2->CONSET = I2CONSET_AA;
					LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
					break;
				case DATA_BYTE:                       // next byte to transmit is the data byte
					if (I2C2Operation == READ)          // if performing a read then the dummy write cycle
																							// ends here. no data byte is transmitted
					{
						LPC_I2C2->CONSET = I2CONCLR_STAC;
						LPC_I2C2->CONCLR 	= I2CONCLR_SIC;
						#if	(IIC2_Debug == 1)
							printf("0x28 send start, I2C2CONSET 0x%02X\r\n", LPC_I2C2->CONSET);
						#endif
					}
					else
					{
						if(I2C2Counter >= 1)					//Data sent, Ack
						{
							LPC_I2C2->DAT = *I2C2Data;			//Write data to tx register
							#if	(IIC2_Debug == 1)
								printf("0x28 data send 0x%02X, I2C2Data addr %d \r\n", LPC_I2C2->DAT, I2C2Data);
							#endif
							I2C2Counter--;
							I2C2Data++;
							LPC_I2C2->CONSET = I2CONSET_AA;
							LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
						}
						else
						{
							I2C2State = FINISHED;                  	//Signal end of I2C2C activity
							LPC_I2C2->CONSET = I2CONSET_STO;
							LPC_I2C2->CONCLR 	= I2CONCLR_SIC;      	//
							I2C2Status = I2C_OK;                		// status of operation is OK
							#if	(IIC2_Debug == 1)
								printf("0x28 send stop\r\n");
							#endif
						}
					}
				break;
				// finished writing byte
				case FINISHED:         		            	// data byte was successfully transmitted
					LPC_I2C2->CONSET = I2CONSET_STO;     	// request to transmit a stop condition
					LPC_I2C2->CONCLR 	= I2CONCLR_SIC;
					I2C2Status = I2C_OK;                	// status of operation is OK
					#if	(IIC2_Debug == 1)
						printf("0x28 send stop\r\n");
					#endif
				break;
			}
			break;

		// DATA BYTE OR UPPER ADDRESS OR LOWER ADDRESS TRANSMITTED - NO ACK RECEIVED
		case 0x30	:								//Data sent, NOT Ack
			LPC_I2C2->CONSET = I2CONSET_STO;
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
			I2C2Status = I2C_ERROR;                 // status of operation is ERROR
			#if	(IIC2_Debug == 1)
				printf("0x30 I2C 2_ERROR\r\n");
			#endif
			break;
		
		case 0x38	:								//
			LPC_I2C2->CONSET = I2CONSET_STA | I2CONSET_AA;
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
			#if	(IIC2_Debug == 1)
				printf("0x38 I2C2_Restart\r\n");
			#endif
			break;
		
		// SLAVE ADDRESS + READ TRANSMITTED - ACK RECEIVED
		case 0x40:
			LPC_I2C2->CONSET = I2CONSET_AA;
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;                    //
			#if	(IIC2_Debug == 1)
				printf("0x40\r\n");
			#endif
			break;
		
		// SLAVE ADDRESS + READ TRANSMITTED - NO ACK RECEIVED
		case 0x48 :									//Slave Address +R, Not Ack
			LPC_I2C2->CONSET = I2CONSET_STO;	      //stop condition
			LPC_I2C2->CONCLR 	= I2CONCLR_SIC;       //
			I2C2Status = I2C_ERROR;                 // status of operation is ERROR
			#if	(IIC2_Debug == 1)
				printf("0x48\r\n");
			#endif
			break;
		
		case 0x50 :								//Data Received, ACK
			*I2C2Data 	= LPC_I2C2->DAT;
			#if	(IIC2_Debug == 1)
				printf("0x50 data receive 0x%02X, I2C2Data addr %d\r\n", *I2C2Data, I2C2Data);
			#endif
			I2C2Counter--;
			I2C2Data++;
			if(I2C2Counter == 1)
				LPC_I2C2->CONCLR = I2CONCLR_SIC | I2CONCLR_AAC;
			else if(I2C2Counter == 0)
			{
				LPC_I2C2->CONSET = I2CONSET_STO;	      	//stop condition
				LPC_I2C2->CONCLR = I2CONCLR_SIC; 	      	//
				I2C2Status = I2C_OK;                   		//Signal end of I2C0C activity							
			}
			else
			{
				LPC_I2C2->CONSET = I2CONSET_AA;
				LPC_I2C2->CONCLR = I2CONCLR_SIC;
			}
			break;
		
		case (0x58):									//Data Received, Not Ack
			*I2C2Data 	= LPC_I2C2->DAT;
			#if	(IIC2_Debug == 1)
				printf("0x58 data receive 0x%02X, I2C2Data addr %d\r\n", *I2C2Data, I2C2Data);
			#endif
			LPC_I2C2->CONSET = I2CONSET_STO;	      	//stop condition
			LPC_I2C2->CONCLR = I2CONCLR_SIC; 	      	//
			I2C2Status = I2C_OK;                   		//Signal end of I2C2C activity							
			break;

		default :
			LPC_I2C2->CONSET = I2CONSET_STO;	      	//stop condition
			LPC_I2C2->CONCLR = I2CONCLR_SIC;  	     	//
			I2C2Status = I2C_ERROR;             	    // status of operation is ERROR
			#if	(IIC2_Debug == 1)
				printf("default\r\n");
			#endif
			break;
		
	}
	#if	(IIC2_Debug == 1)
		printf("exit irq\r\n");
	#endif
}
/********************************  END  *************************************/

