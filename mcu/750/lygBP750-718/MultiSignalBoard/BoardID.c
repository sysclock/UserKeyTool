#define __INIT_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      Init.c                                                      */
/*																			*/
/* USAGE:       This module contains functions for system initialization.	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
//#include "../System Code/lpc17xx.h"
#include "../System Code/type.h"
#include "../System Code/global.h"
#include "../System Code/Timer.h"
//#include "../SPI Comm/D_SPI.h"
#include "../Device/InputFPGA/InputFPGA.h"
#include "BoardID.h"

//#define USE_TUO_BEI_CPU_BOARD

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N							*/
/****************************************************************************/
void ReadBoardIDInit(void);
BYTE ReadBoardID(BYTE B_Slot);
void SetBoardIDCLK(BYTE B_Data);
void BoardIICBusSwitch(BYTE B_Slot);
void Board_CPU_SPI_BusSwitch(BYTE B_Slot);
void BoardDVIHPDSetSwitch(BYTE B_Slot, BYTE B_HPDStatus);
void BoardSlotReset(BYTE B_Slot);
/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void ReadBoardIDInit(void)
{
	#if 0
  LPC_PINCON->PINSEL0 &= ~(3UL<<30);          			// P0.15 cleared
  LPC_PINCON->PINSEL1 &= ~((3UL<<4));								// P0.18 cleared
	
	//	Set	P0.15 BID_CLK As Output
	//	Set P0.18 BID_DI As Input
	LPC_GPIO0->FIODIR |= BIT15;
	LPC_GPIO0->FIODIR &= ~BIT18;											// P0.18 cleared
	LPC_GPIO0->FIOSET = BIT15;												// Set all output pin to '1'
	#endif
}

BYTE ReadBoardID(BYTE B_Slot)
{
	#if 0
	WORD W_Count = 0, W_Data = 0, W_RevData = 0;

  LPC_PINCON->PINSEL0 &= ~(3UL<<30);          			// P0.15 cleared
  LPC_PINCON->PINSEL1 &= ~((3UL<<4));								// P0.18 cleared
	
	//	Set	P0.15 BID_CLK As Output
	//	Set P0.18 BID_DI As Input
	LPC_GPIO0->FIODIR |= BIT15;
	LPC_GPIO0->FIODIR &= ~BIT18;											// P0.18 cleared

	LPC_GPIO0->FIOSET = BIT15;												// Set all output pin to '1'

	BoardSlotReset(B_Slot);
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	W_Data |= BIT13;
	W_Data &= ~(BIT4 | BIT12 | BIT6 | BIT7);
	W_Data |= (B_Slot &  0x07);
//	W_Data &= 0xFFFF;
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
	
	W_RevData = 0;
	W_Count = BIT7;													// Initialize bit count
//	LPC_GPIO0->FIOSET = BIT15;
	Time_LongWait();                 						// Delay
  while (W_Count)
	{
		// Set CLK = 0
		LPC_GPIO0->FIOCLR = BIT15;
    Time_LongWait();                 // Delay
		if (LPC_GPIO0->FIOPIN & BIT18)			// Read data bit P0.18(SDOA)
		{
			W_RevData |= W_Count;      			// data bit = 1
//			printf("READ BIT 1\r\n");
		}
		else
		{			
			W_RevData &= (~W_Count);						// data bit = 0 
//			printf("READ BIT 0\r\n");
		}
		LPC_GPIO0->FIOSET = BIT15;
		// Clear CLK = 1
    Time_LongWait();                 // Delay
//		printf("BIT 0x%08X, SDI = 0x%08X \r\n", W_Count, (W_Count & W_Data));
    W_Count = W_Count >> 1;     // Shift bit position to right
	}
//	printf("BoardSlot %d , BID = 0x%02X\r\n", BoardSlot, W_RevData);
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	InputFPGA_WriteWord(0x03, W_Data);
	return (BYTE) W_RevData;
	#else
	return 0;
	#endif
}

void SelectBoardSlot(BYTE B_Slot)
{
	WORD W_Data = 0;
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	W_Data |= BIT13;
	W_Data &= ~(BIT4 | BIT12 | BIT6 | BIT7);
	W_Data |= (B_Slot &  0x07);
//	W_Data &= 0xFFFF;
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
}

void SetBoardIDCLK(BYTE B_Data)
{
	#if 0
	if(B_Data)
		LPC_GPIO0->FIOSET = BIT15;
	else
		LPC_GPIO0->FIOCLR = BIT15;
	#endif
}

void BoardIICBusSwitch(BYTE B_Slot)
{
	WORD W_Data = 0;
	W_Data = InputFPGA_ReadWord(0x03);
	if(W_Data == 0xFFFFFFFF)
		printf("BoardIICBusSwitch %d , Read Error\r\n", B_Slot);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	W_Data &= ~BIT3;
	W_Data |= (B_Slot &  0x07);
//	W_Data &= 0xFFFF;
//	printf("BoardIICBusSwitch %d , W_Data = 0x%04X\r\n", B_Slot, W_Data);
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
	Delay1ms(1);
}

void Board_CPU_SPI_BusSwitch(BYTE B_Slot)
{
	WORD W_Data = 0;
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	W_Data &= ~(BIT4 | BIT5 | BIT6 | BIT7 | BIT8);
	W_Data |= (B_Slot &  0x07);
//	W_Data &= 0xFFFF;
//	printf("BoardIICBusSwitch %d , W_Data = 0x%04X\r\n", B_Slot, W_Data);
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
}

void BoardDVIHPDSetSwitch(BYTE B_Slot, BYTE B_HPDStatus)
{
	WORD W_Data = 0;
	#ifdef USE_TUO_BEI_CPU_BOARD
		W_Data = InputFPGA_ReadWord(0x03);
		W_Data &= BIT11;
		W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
		if(B_HPDStatus)
			W_Data |= BIT12;									// N9 = 1
		else
		{
			W_Data &= ~(BIT12);								// N9 = 0 
			W_Data |= BIT13;									// N10 = 1 
		}
		W_Data |= (B_Slot &  0x07);
	#else
		W_Data = InputFPGA_ReadWord(0x03);
		W_Data &= BIT11;
		W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
		W_Data &= ~(BIT4 | BIT5);								// N1 = 0, N2 = 0
		W_Data |= (B_Slot &  0x07);
	#endif
//	W_Data &= 0xFFFF;
//	printf("BoardIICBusSwitch %d , W_Data = 0x%04X\r\n", B_Slot, W_Data);
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
}

void BoardSlotReset(BYTE B_Slot)
{
	WORD W_Data = 0;
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	W_Data &= ~BIT14;
	W_Data |= BIT15;
	W_Data |= (B_Slot &  0x07);
//	W_Data &= 0xFFFF;
	printf("BoardSlotReset %d , W_Data = 0x%04X\r\n", B_Slot, W_Data);
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
	Delay10ms(1);
	W_Data = InputFPGA_ReadWord(0x03);
	W_Data &= BIT11;
	W_Data |= ((~BIT11) & BOARD_SLOT_SELECT_DEF);
	InputFPGA_WriteWord(0x03, W_Data | BIT16);
	printf("BoardSlotReset %d , W_Data = 0x%04X\r\n", B_Slot, W_Data);
}

/********************************  END  *************************************/
