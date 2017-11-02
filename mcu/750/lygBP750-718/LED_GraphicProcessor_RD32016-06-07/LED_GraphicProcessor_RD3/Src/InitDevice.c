#define __INIT_DEVICE_C__
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
#include "InitDevice.h"
#include "tcp_echoserver.h"
#include "../MultiSignalBoard/param.h"
#include "../Device/ClockGeneration/SL38160.h"
#include "../Device/TVP5158/TVP5158.h"
#include "../Device/ADV7604Driver/adv7604.h"
//#include "../Device/IT66121/IT66121Task.h"
#include "../Device/InputFPGA/InputFPGA.h"

#include "../MultiSignalBoard/OutputBoardSiI9136.h"
#include "../MultiSignalBoard/InputBoardADV7604.h"
#include "../MultiSignalBoard/InputBoardGV7601.h"
#include "../MultiSignalBoard/InputBoardTVP5158.h"
#include "../MultiSignalBoard/InputBoardGraphicFPGA.h"

#include "../WindowsLayout/ScreenWindowLayout.h"
// #include "../OutputBoard/param.h"
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
	Serial1_Init();
	Serial4_Init();
	Serial5_Init();
	TimerInit();
	I2C1_Init();
	I2C2_Init();
	I2C3_Init();
	// Light On LED0
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	// Light off LED1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	// Light Off LED2
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	// FPGA program
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// FPGA_NRST
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	// Reset ADV7604 VIP0_NRST, VIP1_NRST
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	Delay10ms(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	#if 1
	// BUZZER Beep
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	Delay10ms(10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	#endif
	
	// Check FPGA Down
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) 
		Delay1ms(1);
	HAL_GPIO_WritePin(GPIOB, PB1_FPGA_NRST_Pin, GPIO_PIN_SET);
	Delay100ms(2);
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
	Init_SL38160();
	ParamInit();
	InitInputBoardGV7601();
	InitInputBoardADV7604();
	InitOutputBoardSiI9136();
	InitInputBoardGraphicFPGA();
	ScreenWindowLayoutInputSignalInit();
	/*
	Init_SL38160();
	InitInputBoardGV7601();
	while(1)
	InitInputBoardADV7604();
	InitInputBoardTVP5158();
	InitOutputBoardSiI9136();
	InitInputBoardGraphicFPGA();
	ScreenWindowLayoutInputSignalInit();
	*/
	BoardParamTCPInit();						// BoardParamTCPInit before ScreenWindowLayoutDisplayInit
	printf("Devices Init OK\r\n");
}

#if 0
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
			gB_EXTI0_SFP_LOS_Flag = 1;
		else
			gB_EXTI0_SFP_LOS_Flag = 2;
	}
}
#endif
/********************************  END  *************************************/

