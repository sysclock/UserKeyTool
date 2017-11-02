#define __D_IIC_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC.c	                                                */
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
#include "global.h"
#include "Timer.h"
#include "D_IIC.H"

#define IIC1_Debug	1
#define IIC2_Debug	1
#define IIC_TIMEOUT_DURATION_CNT				1000
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

/****************************************************************************/
/*  S T A T I C   F U N C T I O N    P R O T O T Y P E S                    */
/****************************************************************************/
#ifndef USE_SIMULATED_I2C
unsigned char	gB_I2C1SendBuf[SIO_RxBufLen + 2] = {0};
unsigned char	gB_I2C1ReceiveBuf[SIO_RxBufLen] = {0};
unsigned char	gB_I2C2SendBuf[SIO_RxBufLen + 2] = {0};
unsigned char	gB_I2C2ReceiveBuf[SIO_RxBufLen] = {0};
#endif
/****************************************************************************/
/*	G L O B A L    D A T A    T A B L E										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void I2C1_ExternInit(void);
extern void I2C2_ExternInit(void);

#ifndef USE_SIMULATED_I2C
void I2C1_Init(void);
unsigned char I2C1WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff);
unsigned char* I2C1ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count);
void I2C2_Init(void);
unsigned char I2C2WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff);
unsigned char* I2C2ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count);
#endif

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
#ifndef USE_SIMULATED_I2C
void I2C1_Init(void)
{
	memset((void*) gB_I2C1ReceiveBuf, 0, sizeof((void*) gB_I2C1ReceiveBuf));
	printf("I2C1 init OK!\r\n");
}
#endif

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    unsigned char I2C1WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr,					*/
/*									unsigned int W_Count, unsigned char* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write unsigned char data to slave through I2C1 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifndef USE_SIMULATED_I2C
unsigned char I2C1WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff)
{
	I2C1_ExternInit();
	if(B_MemAddrType == 1)
	{
		if(HAL_I2C_Mem_Write(&hi2c1, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_16BIT, pBuff, W_Count, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC1 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return 0;
		}
	}
	else
	{
		if(HAL_I2C_Mem_Write(&hi2c1, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_8BIT, pBuff, W_Count, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC1 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return 0;
		}
	}
	return 1;
}
#endif

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    unsigned char* I2C1ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr,					*/
/*									unsigned int W_Count)									*/
/*                                                                          		*/
/* USAGE:       read unsigned char data from slave through I2C1 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read unsigned char data											*/
/*              W_Count -	read unsigned char data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifndef USE_SIMULATED_I2C
unsigned char* I2C1ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count)
{
	if(W_Count > sizeof(gB_I2C1ReceiveBuf))
		return NULL;
	I2C1_ExternInit();
	if(B_MemAddrType == 1)
	{
		if(HAL_I2C_Mem_Read(&hi2c1, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_16BIT, gB_I2C1ReceiveBuf, W_Count + 1, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC1 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
	else
	{
		if(HAL_I2C_Mem_Read(&hi2c1, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_8BIT, gB_I2C1ReceiveBuf, W_Count + 1, IIC_TIMEOUT_DURATION_CNT))
		{
			#if(IIC1_Debug == 1)
			printf("IIC1 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
	return gB_I2C1ReceiveBuf;
}
#endif
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
/*				effect of this two-fold:  First, the intended initial I2C2	*/
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
#ifndef USE_SIMULATED_I2C
void I2C2_Init(void)
{
	memset((void*) gB_I2C2ReceiveBuf, 0, sizeof((void*) gB_I2C2ReceiveBuf));
	printf("I2C2 init OK!\r\n");
}
#endif

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    unsigned char I2C2WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr,					*/
/*									unsigned int W_Count, unsigned char* pBuff)						*/
/*                                                                          		*/
/* USAGE:       Write unsigned char data to slave through I2C2 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - TransferByte data										*/
/*              W_Count -	TransferByte data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifndef USE_SIMULATED_I2C
unsigned char I2C2WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff)
{
	I2C2_ExternInit();
	if(B_MemAddrType == 1)
	{
		if(HAL_I2C_Mem_Write(&hi2c2, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_16BIT, pBuff, W_Count, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC2 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return 0;
		}
	}
	else
	{
		if(HAL_I2C_Mem_Write(&hi2c2, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_8BIT, pBuff, W_Count, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC2 Write ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return 0;
		}
	}
	return 1;
}
#endif

/************************************************************************************/
/*                                                                          		*/
/* FUNCTION:    unsigned char* I2C2ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr,					*/
/*									unsigned int W_Count)									*/
/*                                                                          		*/
/* USAGE:       read unsigned char data from slave through I2C2 bus							*/
/*                                                                          		*/
/* INPUT:       B_I2CAddr - slave address		                          			*/
/*              W_MemAddr - read unsigned char data											*/
/*              W_Count -	read unsigned char data count									*/
/*                                                                          		*/
/* OUTPUT:      OK  	 - successful                                       		*/
/*              ERR_BUSY - no ACK                                           		*/
/*                                                                          		*/
/************************************************************************************/
#ifndef USE_SIMULATED_I2C
unsigned char* I2C2ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count)
{
	if(W_Count > sizeof(gB_I2C2ReceiveBuf))
		return NULL;
	I2C2_ExternInit();
	if(B_MemAddrType == 1)
	{
		if(HAL_I2C_Mem_Read(&hi2c2, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_16BIT, gB_I2C2ReceiveBuf, W_Count + 1, IIC_TIMEOUT_DURATION_CNT) != HAL_OK)
		{
			#if(IIC1_Debug == 1)
			printf("IIC2 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
	else
	{
		if(HAL_I2C_Mem_Read(&hi2c2, (uint16_t) B_I2CAddr, (uint16_t) W_MemAddr, I2C_MEMADD_SIZE_8BIT, gB_I2C2ReceiveBuf, W_Count + 1, IIC_TIMEOUT_DURATION_CNT))
		{
			#if(IIC1_Debug == 1)
			printf("IIC2 Read ERROR: SlaveAddr = 0x%02X, MemAddr = 0x%04X\r\n", B_I2CAddr, W_MemAddr);
			#endif
			return  NULL;
		}
	}
	return gB_I2C2ReceiveBuf;
}
#endif

/********************************  END  *************************************/
