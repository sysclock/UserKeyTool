#ifndef __D_IIC_H__
#define __D_IIC_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC.h                                                    */
/*																			*/
/* USAGE:       Header file for module D_IIC.c.                         	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    I N C L U D E	F I L E									*/
/****************************************************************************/
#include "stm32f1xx_hal.h"

/****************************************************************************/
/*	D E F I N I T I O N														*/
/****************************************************************************/
#define SIO_RxBufLen 		256
#define I2C_DEV_MEMORY_ADDRESS_8BIT		0
#define I2C_DEV_MEMORY_ADDRESS_16BIT	1
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern unsigned char	gB_I2C1ReceiveBuf[SIO_RxBufLen];
extern unsigned char	gB_I2C2ReceiveBuf[SIO_RxBufLen];
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void I2C1_Init			(void);
extern unsigned char I2C1WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff);
extern unsigned char* I2C1ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count);
extern void I2C2_Init			(void);
extern unsigned char I2C2WriteByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count, unsigned char* pBuff);
extern unsigned char* I2C2ReadByte(unsigned char B_I2CAddr, unsigned int W_MemAddr, unsigned char B_MemAddrType, int W_Count);

#endif
