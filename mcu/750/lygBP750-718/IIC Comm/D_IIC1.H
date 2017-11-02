#ifndef __D_IIC1_H__
#define __D_IIC1_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      D_IIC1.h                                                    */
/*																			*/
/* USAGE:       Header file for module D_IIC1.c.                         	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    I N C L U D E	F I L E									*/
/****************************************************************************/
#include "../System Code/global.h"
#include "../System Code/type.h"

/****************************************************************************/
/*	D E F I N I T I O N														*/
/****************************************************************************/
#ifndef	TMC_BACK_BOARD_RD2
	#define TMC_BACK_BOARD_RD2
#endif

#ifdef	TMC_BACK_BOARD_RD1
	typedef enum
	{
		DVI0_0,	DVI0_1,	DVI0_2,	DVI0_3,	DVI0_4,	DVI0_5,	DVI0_6,
		DVI1_0,	DVI1_1,	DVI1_2,	DVI1_3,	DVI1_4,	DVI1_5,	DVI1_6,
		DVI2_0,	DVI2_1,	DVI2_2,	DVI2_3,	DVI2_4,	DVI2_5,	DVI2_6,
		DVI3_0,	DVI3_1,	DVI3_2,	DVI3_3,	DVI3_4,	DVI3_5,	DVI3_6,
		DVI4_0,	DVI4_1,	DVI4_2,	DVI4_3,	DVI4_4,	DVI4_5,	DVI4_6,
		DVI_MAX,
		EEROM0 = DVI_MAX,	EEROM1,	EEROM2,	EEROM3,	I2C1_DEVICE_MAX
	}I2C1_DEVICENAME;
#endif

#ifdef	TMC_BACK_BOARD_RD2
	typedef enum
	{
		DVI0_0,	DVI0_1,	DVI0_2,	DVI0_3,	DVI0_4,
		DVI1_0,	DVI1_1,	DVI1_2,	DVI1_3,	DVI1_4,
		DVI2_0,	DVI2_1,	DVI2_2,	DVI2_3,	DVI2_4,
		DVI3_0,	DVI3_1,	DVI3_2,	DVI3_3,	DVI3_4,
		DVI4_0,	DVI4_1,	DVI4_2,
		DVI_MAX,
		EEROM0 = DVI_MAX,	EEROM1,	EEROM2,	EEROM3,	I2C1_DEVICE_MAX
	}I2C1_DEVICENAME;
#endif
typedef enum
{
	EP6122, EP9134, EEROM, CHIP_TYPE_MAX
}I2C1_CHIP_TYPE;

typedef struct i2c1_Device 
{
	I2C1_DEVICENAME Name;
	I2C1_CHIP_TYPE ChipType;
	BYTE SelectAddr;
	BYTE I2CAddr;
} I2C1_DEVICE;

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern BYTE	gB_I2C1ReceiveBuf[SIO_RxBufLen];
extern const I2C1_DEVICE I2C1_DEVICETable[];
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void I2C1_Init			(void);
extern void I2C1_IRQHandler		(void);				//i2c1 interrupt routine
extern BYTE I2C1WriteByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count, BYTE* pBuff);
extern BYTE I2C1WriteByteEx(BYTE B_I2CPort, BYTE B_SlaveAddr);
extern BYTE* I2C1ReadByte(BYTE B_I2CAddr, WORD W_MemAddr, BYTE B_MemAddrType, int W_Count);

#endif
