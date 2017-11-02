#define __M21121_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      M21121.c                                                 		*/
/*																			*/
/* USAGE:       This module contains functions for Switcher M21121.					*/
/*							M21121 is high-speed crosspoint switches.										*/
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
#include "../../System Code/Timer.h"
#include "M21121.h"
#include <stdio.h>

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void M21121_Init(void);
BYTE M21121_SendData(BYTE B_Device, BYTE B_SubAddr, BYTE B_Data);
BYTE M21121_RcvData(BYTE B_Device, BYTE B_SubAddr);
void M21121_UpdataRegister(BYTE B_Device);
void M21121_SwitchPort(BYTE B_Device, WORD W_InputPort, WORD W_OutputPort);
void M21121_MuteInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);
void M21121_MuteOutputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);
void M21121_EqualizationInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);
/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void M21121_Init(void)
{
	//	Set	P2.2(CS_A), P2.3(CS_B), P2.4(SCLK), P2.5(SDI), use Software XSET; 
	//	Set P2.6(SDOA), P2.7(SDOB) ==> Input, 
	LPC_GPIO2->FIODIR |= 0x0000003C;
	LPC_GPIO2->FIOSET = 0x0000003C;		//	Set all output pin to '1'
	printf("\r\nM21121 A and B init OK!\r\n");
}

BYTE M21121_SendData(BYTE B_Device, BYTE B_SubAddr, BYTE B_Data)
{
	WORD W_Count = 0, W_Data = 0;
	W_Data = B_SubAddr;
	W_Data <<= 8;
	W_Data += B_Data;
	W_Data |= BIT17;
	LPC_GPIO2->FIOSET = 0x0000003C;					//	Set all output pin to '1'
	if(B_Device == 0)
		LPC_GPIO2->FIOCLR = 0x00000004;				//	M21121_Port = '0' ==> CS_A = '0'
	else
		LPC_GPIO2->FIOCLR = 0x00000008;				//	M21121_Port = '0' ==> CS_B = '0'
	W_Count = BIT17;												// Initialize bit count
  while (W_Count)
	{
		// Set CLK = 1
		LPC_GPIO2->FIOSET = 0x00000010;
		if (W_Count & W_Data)		// Check data bit
			LPC_GPIO2->FIOSET = 0x00000020;				// Set M21121 SDI (P2.5) = "1"
		else
			LPC_GPIO2->FIOCLR = 0x00000020;     	// Set M21121 SDI (P2.5) = "0"
		if(W_Count == BIT0)
		{
			if(B_Device == 0)
				LPC_GPIO2->FIOSET = 0x00000004;			//	M21121_Port = '0' ==> CS_A = '1'
			else
				LPC_GPIO2->FIOSET = 0x00000008;			//	M21121_Port = '1' ==> CS_B = '1'
			Time_Wait();                 // Delay
		}
//		printf("BIT 0x%08X, SDI = 0x%08X \r\n", W_Count, (W_Count & W_Data));
    Time_Wait();                 // Delay
		// Clear CLK = 0
		LPC_GPIO2->FIOCLR = 0x00000010;
    Time_Wait();                 // Delay
    W_Count = W_Count >> 1;     // Shift bit position to right
	}
	LPC_GPIO2->FIOSET = 0x0000003C;					//	Set all output pin to '1'
//	printf("\r\nM21121 %d Send, B_SubAddr = 0x%02X, B_Data = 0x%02X", B_Device, B_SubAddr, B_Data);
	return OK;
}

BYTE M21121_RcvData(BYTE B_Device, BYTE B_SubAddr)
{
	WORD W_Count = 0, W_Data = 0, W_RevData = 0;
//	BYTE B_Data = 0;
	W_Data = B_SubAddr;
	W_Data <<= 9;
//	W_Data += B_Data;
	W_Data |= BIT18 | BIT17;
	LPC_GPIO2->FIOSET = 0x0000003C;						//	Set all output pin to '1'
	if(B_Device == 0)
		LPC_GPIO2->FIOCLR = 0x00000004;					//	M21121_Port = '0' ==> CS_A = '0'
	else
		LPC_GPIO2->FIOCLR = 0x00000008;					//	M21121_Port = '1' ==> CS_B = '0'
	W_RevData = 0;
	W_Count = BIT18;													// Initialize bit count
  while (W_Count)
	{
		// Set CLK = 1
		LPC_GPIO2->FIOSET = 0x00000010;
		if (W_Count & W_Data)										// Check data bit
			LPC_GPIO2->FIOSET = 0x00000020;				// Set M21121 SDI (P2.5) = "1"
		else
			LPC_GPIO2->FIOCLR = 0x00000020;     	// Set M21121 SDI (P2.5) = "0"

		if(W_Count == BIT0)
		{
			if(B_Device == 0)
				LPC_GPIO2->FIOSET = 0x00000004;			//	M21121_Port = '0' ==> CS_A = '1'
			else
				LPC_GPIO2->FIOSET = 0x00000008;			//	M21121_Port = '1' ==> CS_B = '1'
			Time_Wait();                 // Delay
		}

		if(W_Count <= BIT7)
		{
//			Time_Wait();                 // Delay
			if(B_Device == 0)
			{
        if (LPC_GPIO2->FIOPIN & 0x00000040)			// Read data bit P2.6(SDOA)
            W_RevData |= W_Count;      					// data bit = 1 
        else    
            W_RevData &= (~W_Count);						// data bit = 0 
			}
			else
			{
        if (LPC_GPIO2->FIOPIN & 0x00000080)			// Read data bit P2.7(SDOB)
            W_RevData |= W_Count;      					// data bit = 1 
        else    
            W_RevData &= (~W_Count);						// data bit = 0 
			}
//			printf("BIT 0x%08X, SDO = 0x%08X \r\n", W_Count, (LPC_GPIO2->FIOPIN & 0x00000040));
		}
    Time_Wait();                 // Delay
		// Clear CLK = 0
		LPC_GPIO2->FIOCLR = 0x00000010;
    Time_Wait();                 // Delay
//		printf("BIT 0x%08X, SDI = 0x%08X \r\n", W_Count, (W_Count & W_Data));
    W_Count = W_Count >> 1;     // Shift bit position to right
	}
	LPC_GPIO2->FIOSET = 0x0000003C;				//	Set all output pin to '1'
//	printf("W_RevData = 0x%08X\r\n", W_RevData);
//	printf("\r\nM21121 %d Rcv, B_SubAddr = 0x%02X, B_Data = 0x%02X", B_Device, B_SubAddr, W_RevData);
	return (BYTE) W_RevData;
}

void M21121_UpdataRegister(BYTE B_Device)
{
	M21121_SendData(B_Device, 0xE8, 0x01);
}

void M21121_SwitchPort(BYTE B_Device, WORD W_InputPort, WORD W_OutputPort)
{
	WORD W_TempData = 0;
	if((W_OutputPort > M21121_OUTPUT_PORT_MAX) || (W_InputPort > M21121_INPUT_PORT_MAX))
		return;
	W_TempData = 1 + (W_InputPort << 1);
	M21121_SendData(B_Device, W_OutputPort, W_TempData);
}

// M21121_MuteInputPort	enable  input port or mute input port
// W_Port: input port index
// B_Data: 1--Mute port, 0 -- enable port
void M21121_MuteInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data)
{
	BYTE B_TempData = 0;
	WORD W_TempAddr = 0;
	if(W_Port > M21121_INPUT_PORT_MAX)
		return;
	W_TempAddr = 0x81 + (W_Port << 1);
	B_TempData = M21121_RcvData(B_Device, W_TempAddr);
	if(B_Data == 1)
		B_TempData = B_TempData & 0x05;
	else
		B_TempData = B_TempData | BIT1;
	B_TempData &= BIT0 | BIT1 | BIT2;
	M21121_SendData(B_Device, W_TempAddr, B_TempData);
}

// M21121_MuteOutputPort	enable output port or mute output port
// W_Port: output port index
// B_Data: 1--Mute port, 0 -- enable port
void M21121_MuteOutputPort(BYTE B_Device, WORD W_Port, BYTE B_Data)
{
	BYTE B_TempData = 0;
	WORD W_TempAddr = 0;
	if(W_Port > M21121_OUTPUT_PORT_MAX)
		return;
	W_TempAddr = 0x80 + W_Port;
	B_TempData = M21121_RcvData(B_Device, W_TempAddr);
	if(B_Data == 1)
		B_TempData = B_TempData & 0x06;
	else
		B_TempData = B_TempData | BIT0;
	if(W_Port & BIT0)
		B_TempData &= BIT0 | BIT1 | BIT2;
	else
		B_TempData &= BIT0;
	M21121_SendData(B_Device, W_TempAddr, B_TempData);
}

// M21121_EqualizationInputPort	Equalize input port or bypass input port
// W_Port: input port index
// B_Data: 1--Equalize port, 0 -- bypass port
void M21121_EqualizationInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data)
{
	BYTE B_TempData = 0;
	WORD W_TempAddr = 0;
	if(W_Port > M21121_INPUT_PORT_MAX)
		return;
	W_TempAddr = 0x81 + (W_Port << 1);
	B_TempData = M21121_RcvData(B_Device, W_TempAddr);
	if(B_Data == 1)
		B_TempData = B_TempData & 0x03;
	else
		B_TempData = B_TempData | BIT2;
	B_TempData &= BIT0 | BIT1 | BIT2;
	M21121_SendData(B_Device, W_TempAddr, B_TempData);
}

