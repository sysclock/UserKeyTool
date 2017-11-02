#define __STP_74HC595_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      74HC595.c                                                   	*/
/*																			*/
/* USAGE:       This module contains functions for serial-in, 								*/
/*							serial or parallel-out shift register with output latches.		*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include "../../System Code/lpc17xx.h"
#include "../../System Code/ASCII.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "STP_74HC595.h"
#include <stdio.h>

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*  S T A T I C    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

void Init_74HC591(void);
void Set_74HC591(WORD W_Value);
void SelectDeviceI2CBus(BYTE B_BoardIndex, BYTE B_I2CBusIndex);
/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/* FUNCTION:    void 74HC595_Init(void)								*/
/*                                                                          */
/* USAGE:       74HC595 device initialization.						        */
/*                                                                          */
/* INPUT:       None                         								*/
/*                                                                          */
/* OUTPUT:      None														*/
/*                                                                          */
/****************************************************************************/
void Init_74HC591(void)
{
	//	Set P024(DS), P0.25(CLK), P0.26(LE) as Output, 
	LPC_GPIO0->FIODIR |= 0x07000000;
	LPC_GPIO0->FIOCLR |= 0x07000000;
	Set_74HC591(0xFFFFFFFF);
	printf("74HC591 init OK!\r\n");
}

/******************************************************************************
	SHIFT74LV595_Data :
	Low 24-bit valid, 

	Bit[0 - 2] : S0 - S2 of HC4051 

	Bit[3 - 23] ==> OE[0 - 20], OE of HC4051; 

	OE :
	OE0 - BIT3 	: I2C0_SCL	set 0 as selected
	OE2 - BIT5 	: I2C0_SDA	set 0 as selected
	OE4 - BIT7 	: I2C1_SCL	set 0 as selected
	OE6 - BIT9 	: I2C1_SDA
	OE8 - BIT11 : INT[x] // HDMI[x]IN_INT
	OE10 - BIT13 : HPD[x] : FPGA_DONE Detect
	OE12 - BIT15 : CPD_S[x]
	OE14 - BIT17 : CFG_FPGA_CS[x]
	OE16 - BIT19 : CFG_FPGA_PROG[x]

	18 		: RESET_74LV138_OE1
	19 		: RESET_74LV138_OE0
	20		: RESET_74LV138_OE2

******************************************************************************/
void Set_74HC591(WORD W_Value)
{
	uint32_t W_Count;
	W_Count = 0x800000;					// Initialize bit count
  while (W_Count)
	{
		if (W_Count & W_Value)		// Check data bit
			LPC_GPIO0->FIOSET = 0x01000000;			// Set 74hc595 DS (P0.24) = "1"
		else
			LPC_GPIO0->FIOCLR = 0x01000000;     // Clear 74hc595 DS (P0.24) = "0"
    Time_Wait();                 // Delay
		// Set CLK = 1
		LPC_GPIO0->FIOSET = 0x02000000;
    Time_Wait();                 // Delay
		// Clear CLK = 0
		LPC_GPIO0->FIOCLR = 0x02000000;
    W_Count = W_Count >> 1;     // Shift bit position to right
	}
	// Set LE = 1
	LPC_GPIO0->FIOSET = 0x04000000;
	Time_Wait();	  							//	Wait Cycle
	LPC_GPIO0->FIOCLR = 0x04000000;
	Time_Wait();	  							//	Wait Cycle
	Time_LongWait();
	Time_LongWait();
}

// B_BoardIndex: Board slot index 0~3 input board, 4~7 output board
// B_I2CBusIndex: 0--I2C0, 1--I2C1

void SelectDeviceI2CBus(BYTE B_BoardIndex, BYTE B_I2CBusIndex)
{
	uint32_t	x = 0;
	if(B_I2CBusIndex == 0)
	{
		//	Set S[2:0] = i2c_num, OE_N0 = 0, OE_N2 = 0, Others = 1
		x = B_BoardIndex + 0x00FFFFD0;  
		Set_74HC591(x);
		Time_LongWait();
		Time_LongWait();
	}
	else if(B_I2CBusIndex == 1)
	{
		//	Set S[2:0] = i2c_num, OE_N4 = 0, OE_N6 = 0, Others = 1
		x = B_BoardIndex + 0x00FFFD78;  
		Set_74HC591(x);
		Time_LongWait();
		Time_LongWait();
	}
	else
		return;
}
/****************************************************************************/
/*																			*/
/* FUNCTION:    STP_74HC591_DummyCall(void)                                 */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void STP_74HC591_DummyCall(void)
{
}
/********************************  END  *************************************/
