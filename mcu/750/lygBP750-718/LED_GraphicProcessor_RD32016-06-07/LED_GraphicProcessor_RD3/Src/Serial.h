#ifndef __SERIAL_H__
#define __SERIAL_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      SERIAL.h                                                    */
/*																			*/
/* USAGE:       Header file for module SERIAL.c.                         	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "stm32f1xx_hal.h"
#include "global.h"

#define DEBUG_SERIAL_INDEX								1
#define	Serial_RxBufLen										2048					// 3072,4096
// SERIAL_COMMAD_TYPE		0--zifu lei xing	1--Brill View 1		2--Brill View 2
#define SERIAL_COMMAD_TYPE								0
#define BRILL_VIEW_PACKET_START						0x96

#define 		SERIAL4_USED						0
#define 		SERIAL5_USED						1

typedef struct
{
	unsigned char Data[Serial_RxBufLen];
	volatile int Front, Rear;
	volatile char lock;
}Squeue;
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern unsigned char gB_SerialMachineAddress;
extern Squeue	gSqu_Serial1ReceiveBuf;
#if(SERIAL4_USED == 1)
	extern Squeue	gSqu_Serial4ReceiveBuf;
#endif
#if(SERIAL5_USED == 1)
	extern Squeue	gSqu_Serial5ReceiveBuf;
#endif

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void Serial1_Init(void);
extern void Serial4_Init(void);
extern void Serial5_Init(void);
extern void Serial1CommandHandler(void);
extern void Serial4CommandHandler(void);
extern void Serial5CommandHandler(void);
extern unsigned char Serial1_SendData(unsigned char *Bp_Buf, int W_Len);
extern unsigned char Serial4_SendData(unsigned char *Bp_Buf, int W_Len);
extern unsigned char Serial5_SendData(unsigned char *Bp_Buf, int W_Len);
extern unsigned char EnterSerialQueue(Squeue* Queue, unsigned char B_Data);
#endif
