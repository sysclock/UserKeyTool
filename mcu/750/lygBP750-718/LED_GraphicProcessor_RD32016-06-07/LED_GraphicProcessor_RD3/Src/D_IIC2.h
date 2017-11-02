#ifndef __D_IIC2_H__
#define __D_IIC2_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_i2c1.h                                                     */
/*																			*/
/* USAGE:       Header file for module D_i2c0.c.                         	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#define I2C2_FAST_MODE_PLUS      0

#include "stm32f1xx_hal.h"
#include "global.h"

#define SIO_RxBufLen 		256
#define I2C_DEV_MEMORY_ADDRESS_8BIT		0
#define I2C_DEV_MEMORY_ADDRESS_16BIT	1

#ifdef USE_SIMULATED_I2C
extern uint8_t	gB_I2C2ReceiveBuf[SIO_RxBufLen];
#endif
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
#ifdef USE_SIMULATED_I2C
extern void I2C2_Init			(void);
extern void I2C2_IRQHandler		(void);				//i2c0 interrupt routine
extern uint8_t I2C2WriteByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count, uint8_t* pBuff);
extern uint8_t* I2C2ReadByte(uint8_t B_I2CAddr, uint32_t W_MemAddr, uint8_t B_MemAddrType, int W_Count);
#endif
#endif
