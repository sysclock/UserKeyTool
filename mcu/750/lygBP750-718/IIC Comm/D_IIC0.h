#ifndef __D_IIC0_H__
#define __D_IIC0_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_i2c0.h                                                     */
/*																			*/
/* USAGE:       Header file for module D_i2c0.c.                         	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#define I2C0_FAST_MODE_PLUS      0

#include "../System Code/global.h"
#include "../System Code/type.h"

extern BYTE	gB_I2C0ReceiveBuf[SIO_RxBufLen];
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void I2C0_Init			(void);
extern void I2C0_IRQHandler		(void);				//i2c0 interrupt routine
extern BYTE I2C0WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff);
extern BYTE* I2C0ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count);
#endif
