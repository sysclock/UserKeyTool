#define __TIMER_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:		Timer.c														*/
/*																			*/
/* USAGE:		This module is used to process all timer values.			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include <stdio.h>
#include "ASCII.h"
#include "global.h"
#include "type.h"
#include "Timer.h"
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

volatile BYTE	gB_Count1ms;								// 1ms prescale counter
volatile BYTE gB_Count10ms;								// 10ms prescale counter
volatile unsigned short gB_Count100ms;		// 100ms prescale counter
volatile BYTE gBa_TmrBuf[MAX_TMR];				// Timer array

volatile unsigned long TimeTick  = 0;
volatile unsigned long TCPClockTick = 0;
volatile unsigned long gD_IICDelayTickCnt = 0;
//volatile unsigned short gD_TimeTickCnt[INPUT_BOARD_MAX * 8]  = {0xFFFF};
//volatile unsigned short gD_SiI9233TickCnt = 0;
volatile unsigned long 	gD_ADV7604TickCnt = 0;
volatile unsigned long 	gD_GV7601TickCnt = 0;
volatile unsigned long 	gD_IT66121TickCnt = 0;
volatile unsigned long 	gD_TCPTickCnt = 0;
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

void TimerInit		(void);
void TimerStart		(BYTE B_Timer, BYTE B_Value);
void TimerStop		(BYTE B_Timer);
BYTE TimerCheck		(BYTE B_Timer);
void Delay1ms		(BYTE B_Value);
void Delay10ms		(BYTE B_Value);
void Delay100ms		(BYTE B_Value);

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* FUNCTION:	TimerInit(void)												*/
/*																			*/
/* USAGE:		Initialize timer1 pre-scaler value and application timers.	*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void TimerInit(void) 
{
	BYTE B_Index;

	gB_Count1ms		= PRE_SCALE_1MS;		// Timer #1 prescale counters
	gB_Count10ms	= PRE_SCALE_10MS;		//
	gB_Count100ms	= PRE_SCALE_100MS;		//

	for (B_Index = 0; B_Index < MAX_TMR; B_Index++)
		gBa_TmrBuf[B_Index] = 0xFF;			// Initialize timer values
	printf("Timer 1 init OK!\r\n");
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	TimerStart(BYTE B_Timer, BYTE B_value)						*/
/*																			*/
/* USAGE:		Start timer based on the timer number and load value into	*/
/*				gBa_TmrBuf[] accordingly.									*/
/*																			*/
/* INPUT:		B_Timer - timer number										*/
/*				B_value - timer value to load								*/
/*						  (1 - 254) and the value 0xFF is reserved.			*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void TimerStart(BYTE B_Timer, BYTE B_value) 
{
	//
	// Error checking
	//
	if (B_Timer >= MAX_TMR)				// Within buffer size
		return;							// No, exit
	if (B_value == 0xFF)				// Over limit
		B_value--;						// Take the max
	//
	// Reload timer value into buffer
	//
//	T1TCR = 0x00000000;					// Disable timer 1 interrupt
	gBa_TmrBuf[B_Timer] = B_value;		// Load value into timer
//	T1TCR = 0x00000001;					// Enable timer 1 interrupt
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	TimerStop(BYTE B_Timer)										*/
/*																			*/
/* USAGE:		Stop timer based on the timer number and 0xFF is loaded.	*/
/*																			*/
/* INPUT:		B_Timer - timer number										*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void TimerStop(BYTE B_Timer) 
{
	//
	// Error checking
	//
	if (B_Timer >= MAX_TMR)				// Within buffer size
		return;							// No, exit
	//
	// Set timer value into buffer to indicate STOP
	//
//	T1TCR = 0x00000000;					// Disable timer 1 interrupt
	gBa_TmrBuf[B_Timer] = 0xFF;			// Load value into timer
//	T1TCR = 0x00000001;					// Enable timer 1 interrupt
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	TimerCheck(BYTE B_Timer)									*/
/*																			*/
/* USAGE:		Check timer based on the timer number and the value in 		*/
/*				timer buffer.												*/
/*																			*/
/* INPUT:		B_Timer - timer number										*/
/*																			*/
/* OUTPUT:		0 = TIMER_OK (runing or aborted)							*/
/*				1 = TIMER_TMO (timer timeout)								*/
/*																			*/
/****************************************************************************/
BYTE TimerCheck(BYTE B_Timer) 
{
	//
	// Error checking
	//
	if (B_Timer >= MAX_TMR)				// Within buffer size
		return (TIMER_OK);				// No, exit
	//
	// Set timer value into buffer to indicate STOP
	//
//	T1TCR = 0x00000000;					// Disable timer 1 interrupt
	if (gBa_TmrBuf[B_Timer] == 0)		// Timer expired ?
	{									// Yes
		gBa_TmrBuf[B_Timer] = 0xFF;		// Auto clear the flag	
//		T1TCR = 0x00000001;				// Enable timer 1 interrupt
		return (TIMER_TMO);				// Timer expired
	}
	else
	{
//		T1TCR = 0x00000001;				// Enable timer 1 interrupt
		return (TIMER_OK);				// Timer running or aborted
	}
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	Delay1ms(BYTE B_Value)										*/
/*																			*/
/* USAGE:		Create a wait loop in 1ms unit.								*/
/*																			*/
/* INPUT:		B_Value - time to wait										*/
/*						  (1 - 254) and the value 0xFF is reserved.			*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/

void Delay1ms(BYTE B_Value) 
{
	TimerStart(DELAY_1ms_TMR, B_Value);				// Start timer
	while (TimerCheck(DELAY_1ms_TMR)==TIMER_OK);	// Wait till it expired
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	Delay10ms(BYTE B_Value)										*/
/*																			*/
/* USAGE:		Create a wait loop in 10ms unit.							*/
/*																			*/
/* INPUT:		B_Value - time to wait										*/
/*						  (1 - 254) and the value 0xFF is reserved.			*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/

void Delay10ms(BYTE B_Value) 
{
	TimerStart(DELAY_10ms_TMR, B_Value);			// Start timer
	while (TimerCheck(DELAY_10ms_TMR)==TIMER_OK);	// Wait till it expired
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	Delay100ms(BYTE B_Value)									*/
/*																			*/
/* USAGE:		Create a wait loop in 100ms unit.							*/
/*																			*/
/* INPUT:		B_Value - time to wait										*/
/*						  (1 - 254) and the value 0xFF is reserved.			*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void Delay100ms(BYTE B_Value) 
{
	TimerStart(DELAY_100ms_TMR, B_Value);			// Start timer
	while (TimerCheck(DELAY_100ms_TMR)==TIMER_OK);	// Wait till it expired
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	TimerInterrupt	interrupt 3									*/
/*																			*/
/* USAGE:		系统定时器使用的定时器1的中断处理函数						*/
/*																			*/
/* INPUT:		None														*/
/*						  													*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void TIMER1_IRQHandler(void)
{
}
/********************************  END  *************************************/
