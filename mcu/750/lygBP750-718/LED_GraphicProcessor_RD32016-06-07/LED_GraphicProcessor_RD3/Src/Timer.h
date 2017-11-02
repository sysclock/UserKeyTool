#ifndef __TIMER_H__
#define __TIMER_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:		Timer.h														*/
/*																			*/
/* USAGE:		Header file for module Timer.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"

#define PRE_SCALE_1MS			10		// Reload value for 1ms timer interrupt
#define PRE_SCALE_10MS		100		// Reload value for 10ms timer interrupt
#define PRE_SCALE_100MS		1000		// Reload value for 100ms timer interrupt

#define TIMER_OK			0		// Timer ruuning or stop
#define TIMER_TMO			1		// Timer timeout

//
// Define all application timers
//
enum
{									// ***** 1ms timer
	DELAY_1ms_TMR,					// (1) Delay timer
//	SiI_9233_AUDIO_TMR,			// SiI9233 Audio timer
//	SiI_9233_VIDEO_TMR,			// SiI9233 Video timer
	SiI_9233_POLLING_TMR,			// (2) SiI9233 Polling timer
	SiI_9136_POLLING_TMR,			// (3) SiI9136 Polling timer
	ADV7604_POLLING_TMR,			// (4) ADV7604 Polling timer
	GV7601_POLLING_TMR,				// (5) GV7601 Polling timer
									// ***** 10ms timer
	DELAY_10ms_TMR,					// (1) Delay timer
	IIC0_10ms_TMR1,					// (2) IIC0 bus delay timer
	IIC1_10ms_TMR1,					// (3) IIC1 bus delay timer
	IIC2_10ms_TMR1,					// (4) IIC1 bus delay timer
	IIC3_10ms_TMR1,					// (5) IIC1 bus delay timer
	SPI_10ms_TMR1,					// (6) IIC1 bus delay timer
									// ***** 100ms timer
	DELAY_100ms_TMR,				// (1) Delay timer
	SI_9187_100ms_TMR,			// (2) SI9187 Delay timer
	IT_66121_CEC_100ms_TMR,	// (3) It66121 CEC timer
	MAX_TMR							// Max number of timer

};

//
// Note : The number of timer need to match with the ones in "TIMER.INC"
//
#define NUM_1ms_TMR			5		// Number of 1ms timer
#define NUM_10ms_TMR		6		// Number of 10ms timer
#define NUM_100ms_TMR		3		// Number of 100ms timer

extern volatile unsigned char	gB_Count1ms;								// 1ms prescale counter
extern volatile unsigned char gB_Count10ms;								// 10ms prescale counter
extern volatile unsigned short gB_Count100ms;			// 100ms prescale counter
extern volatile unsigned short gBa_TmrBuf[MAX_TMR];				// Timer array

extern volatile unsigned long TimeTick;
extern volatile unsigned long KeyboardScanTick;
extern volatile unsigned long TCPClockTick;
extern volatile unsigned long gD_IICDelayTickCnt;
//extern volatile unsigned short gD_TimeTickCnt[INPUT_BOARD_MAX * 8];
//extern volatile unsigned short gD_SiI9233TickCnt;
extern volatile unsigned long 	gD_ADV7604TickCnt;
extern volatile unsigned long 	gD_GV7601TickCnt;
extern volatile unsigned long 	gD_IT66121TickCnt;
extern volatile unsigned long 	gD_TCPTickCnt;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void TimerInit		(void);
extern void TimerStart		(unsigned char B_Timer, unsigned short B_Value);
extern void TimerStop		(unsigned char B_Timer);
extern unsigned char TimerCheck		(unsigned char B_Timer);
extern void Delay1ms		(unsigned short B_Value);
extern void Delay10ms		(unsigned short B_Value);
extern void Delay100ms		(unsigned short B_Value);
extern void TIMER1_IRQHandler(void);

#endif
