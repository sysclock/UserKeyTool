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
#include "global.h"

#define	Serial_RxBufLen	1024
// SERIAL_COMMAD_TYPE		0--zifu lei xing	1--Brill View 1		2--Brill View 2
#define SERIAL_COMMAD_TYPE			0
#define BRILL_VIEW_PACKET_START						0x96

typedef struct
{
	BYTE Data[Serial_RxBufLen];
	volatile int Front, Rear;
}Squeue;
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern BYTE gB_SerialMachineAddress;
extern Squeue	gSqu_Serial0ReceiveBuf;
extern Squeue	gSqu_Serial1ReceiveBuf;
extern Squeue	gSqu_Serial3ReceiveBuf;
extern BYTE gB_KeyboardData;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void Serial0_Init(void);
extern void UART0_IRQHandler(void);				//Serial1 interrupt routine
extern void Serial1_Init(void);
extern void UART1_IRQHandler(void);				//Serial1 interrupt routine
extern void Serial3_Init(void);
extern void UART3_IRQHandler(void);				//Serial3 interrupt routine
extern BYTE* SerialHandler(void);
extern void SerialCommandHandler(void);
extern void SerialCommadClear(void);
extern BYTE Serial_SendData(BYTE *Bp_Buf, int W_Len);
extern BYTE Serial3_SendData(BYTE *Bp_Buf, int W_Len);
extern BYTE EnterSerialQueue(Squeue* Queue, BYTE B_Data);
#endif
