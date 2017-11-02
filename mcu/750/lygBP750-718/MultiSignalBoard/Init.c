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
#include "../System Code/Serial.h"
#include "../System Code/global.h"
#include "../System Code/Timer.h"
#include "../IIC Comm/D_IIC0.h"
#include "../IIC Comm/D_IIC1.h"
#include "../IIC Comm/D_IIC2.h"
#include "../Device/SerialFlashMemory/M25P80.h"
#include "../Device/ClockGeneration/SL38160.h"
#include "../Device/TVP5158/TVP5158.h"
#include "../Device/ADV7604Driver/adv7604.h"
#include "../Device/IT66121/IT66121Task.h"
#include "../Device/InputFPGA/InputFPGA.h"

#include "OutputBoardSiI9136.h"
#include "InputBoardADV7604.h"
#include "InputBoardGV7601.h"
#include "InputBoardTVP5158.h"
#include "InputBoardGraphicFPGA.h"

#include "../WindowsLayout/ScreenWindowLayout.h"

#if 0

#include "../WindowsLayout/ScreenWindowLayout.h"
#include "../WindowsLayout/SignalWindowsCrop.h"
#endif

extern void TCPClockHandler(void);
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
extern void SiI9233_TempTest(void);
void McuInit(void);
void DevicesInit(void);
/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* FUNCTION:	McuInit(void)												*/
/*																			*/
/* USAGE:     Initialization of MCU Control Registers						*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void McuInit(void)
{
	TimerInit();
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	DevicesInit(void)											*/
/*																			*/
/* USAGE:       Initialization of devices on board.							*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void DevicesInit(void)
{
	//printf("SoftVer: %s\r\n", GP_ALL_INPUT_FIRMWARE_VER);
	TimerInit();
	I2C0_Init();
	if(InputFPGA_CheckChip() == 0)
		printf("No FPGA Chip!\r\n");
	InputFPGA_WriteWord(0x03, BOARD_SLOT_SELECT_DEF | BIT16);
	Init_SL38160();
	InitInputBoardADV7604();
	InitInputBoardTVP5158();
	InitInputBoardGV7601();
	InitOutputBoardSiI9136();
	InitInputBoardGraphicFPGA();
	ScreenWindowLayoutInputSignalInit();

	#if 0
	BYTE B_Data = 0;
//	WORD W_Data = 0;
	ScreenWindowLayoutInit();
	SignalWindowsCropInit();
	SFM_M25P80Init();
	if(InputFPGA_CheckDone() == 0) Delay1ms(1);
	InitInputBoardGraphicFPGA();
	B_Data = TVP5158_CheckChipId(TVP5158_DEV5);
	if(B_Data == OK)
	{
		TVP5158_SoftReset(TVP5158_DEV5);
		TVP5158_SetOfmMode(TVP5158_DEV5, MCVIP_VIDEO_DECODER_MODE_4CH_D1, 0, 0, 0);
		printf("TVP5158 Init OK\r\n");
	}
	else
		printf("TVP5158 Init Error\r\n");
	ADV7604_InitDev();
	IT66121_InitDev();
//	W_Data = InputFPGA_ReadWord(INPUT_FPGA_CHIP_ID);
//	printf("W_Data = 0x%0X\r\n", W_Data);
//	printf("IP: %d %d %d %d\r\n", 192, 168, 0, 100);
//	TCP_SetIPAddress(192, 168, 0, 100);
	ScreenWindowLayoutInit();
	#endif
	printf("Devices Init OK\r\n");
}

#if 0
// SysTick interrupt happens every 1 ms
void SysTick_Handler (void)
{
//	int i = 0;
  TimeTick++;
	TCPClockTick++;
//	gD_SiI9233TickCnt++;
	gD_ADV7604TickCnt++;
	gD_GV7601TickCnt++;
	gD_IT66121TickCnt++;
	gD_TCPTickCnt++;
	gD_IICDelayTickCnt++;
/*
	for (i = 0; i < (INPUT_BOARD_MAX * 8); i++)							// 处理活动的1MS事件 
	{
		if ((gD_TimeTickCnt[i] != 0xFFFF) && (gD_TimeTickCnt[i] != 0))	// 
			gD_TimeTickCnt[i]--; 
	}
*/

  if (TCPClockTick >= 200)
	{
    TCPClockTick = 0;
//    LPC_GPIO1->FIOPIN ^= BIT22;
    TCPClockHandler();  
  }

}
#endif
/********************************  END  *************************************/

