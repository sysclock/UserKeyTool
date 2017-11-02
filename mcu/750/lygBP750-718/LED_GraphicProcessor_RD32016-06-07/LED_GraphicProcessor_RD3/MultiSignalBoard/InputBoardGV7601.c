#define __INPUT_BOARD_GV7601_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardGV7601.c                                                      */
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

//#include "lpc17xx.h"
#include "type.h"
#include "Serial.h"
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "InputBoardGV7601.h"

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern volatile unsigned long gD_GV7601TickCnt;

ST_INPUT_BOARD_GV7601 gSt_InputBoardGV7601;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void InitInputBoardGV7601(void);
void InputBoardGV7601Task(void);
//void InputBoardGV7601ProgramEDID(BYTE B_InputIndex);
int InputBoardGV7601GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight);
int InputBoardGV7601PrintSignalStd(BYTE B_InputIndex);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
static BYTE GV7601_PollInterruptFlag = 0;
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void InitInputBoardGV7601(void)
{
	int i = 0, ret = 0;
	WORD W_Data1 = 0, W_Data2 = 0;
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	GV7601_GlobalStatus_t *Stp_TempGV7601Status_t = NULL;
	GV7601_GSPI_Init();
	GV7601_PollInterruptFlag = 1;
	memset(&gSt_InputBoardGV7601, 0, sizeof(gSt_InputBoardGV7601));
	for(i = 0; i < DEVICE_GV7601_MAX; i++)
	{
		ret = GV7601_ReadWord(BIT0 << i, 0, &W_Data1);
		if(0xFFFF == ret)
			continue;
		ret = GV7601_ReadWord(BIT0 << i, 1, &W_Data2);
		if(0xFFFF == ret)
			continue;
		if((W_Data1 == 0x00) && (W_Data2 == 0x100))
		{
			Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[gSt_InputBoardGV7601.B_ActiveChip]);
			Stp_TempDeviceGV7601->B_BoardIndex = i;
			Stp_TempDeviceGV7601->B_I2CBusIndex = 3;
			Stp_TempDeviceGV7601->B_SubI2CBusIndex = i;
			
			Stp_TempGV7601Status_t = &(Stp_TempDeviceGV7601->St_GV7601VideoStatus_t);
			Stp_TempGV7601Status_t->VideoState = GV7601_STD_INVALID;
			gSt_InputBoardGV7601.B_ActiveChip++;
			GV7601_Init(Stp_TempDeviceGV7601->B_SubI2CBusIndex);
			GV7601_GetStatus(Stp_TempDeviceGV7601->B_SubI2CBusIndex);
			memcpy(Stp_TempGV7601Status_t, &gSt_GV7601Status_t, sizeof(gSt_GV7601Status_t));
			printf("Board %d , Input %d , GV7601 Init OK\r\n", Stp_TempDeviceGV7601->B_BoardIndex, 0);
		}
	}
	printf("InitInputBoardGV7601 OK! ActiveChip = %d\r\n", gSt_InputBoardGV7601.B_ActiveChip);
	TimerStart(GV7601_POLLING_TMR, 100);
}

void InputBoardGV7601Task(void)
{
	int i = 0;
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	GV7601_GlobalStatus_t *Stp_TempGV7601Status_t = NULL;
	GV7601_GSPI_Init();
	if(GV7601_PollInterruptFlag == 1)
	{
		for(i = 0; i < gSt_InputBoardGV7601.B_ActiveChip; i++)
		{
			Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[i]);
			Stp_TempGV7601Status_t = &(Stp_TempDeviceGV7601->St_GV7601VideoStatus_t);
//		PCA9546A_SelectBusPort(BIT0 << Stp_TempDeviceGV7601->B_SubI2CBusIndex);
			GV7601_GetStatus(Stp_TempDeviceGV7601->B_SubI2CBusIndex);
			memcpy(Stp_TempGV7601Status_t, &gSt_GV7601Status_t, sizeof(gSt_GV7601Status_t));
		}
	}
//	GV7601_TimerTask();
	if (TimerCheck(GV7601_POLLING_TMR) == TIMER_TMO)
	{
		GV7601_PollInterruptFlag = 1;
		TimerStart(GV7601_POLLING_TMR, 100);
	}
	else
		GV7601_PollInterruptFlag = 0;
}

int InputBoardGV7601GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight)
{
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	GV7601_GlobalStatus_t *Stp_TempGV7601Status_t = NULL;
	if(B_InputIndex >= gSt_InputBoardGV7601.B_ActiveChip)
	{
		*pWidth = 1024;
		*pHeight = 768;
		return 0;
	}
	Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[B_InputIndex]);
	Stp_TempGV7601Status_t = &(Stp_TempDeviceGV7601->St_GV7601VideoStatus_t);
	*pWidth = Stp_TempGV7601Status_t->VideoHSize;
	*pHeight = Stp_TempGV7601Status_t->VideoVSize;
	return 1;
}

int InputBoardGV7601PrintSignalStd(BYTE B_InputIndex)
{
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	GV7601_GlobalStatus_t *Stp_TempGV7601Status_t = NULL;
	if(B_InputIndex >= gSt_InputBoardGV7601.B_ActiveChip)
	{
		printf("InputBoardGV7601: No This Device, DeviceIndex = %d\r\n", B_InputIndex);
		return GV7601_STD_INVALID;
	}
	printf("InputBoardGV7601 VD_STD: DeviceIndex = %d\r\n", B_InputIndex);
	Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[B_InputIndex]);
	Stp_TempGV7601Status_t = &(Stp_TempDeviceGV7601->St_GV7601VideoStatus_t);
	switch(Stp_TempGV7601Status_t->VideoState)
	{
		case GV7601_STD_720X480I_60Hz:
			printf("    720 x 480i @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_720X576I_50Hz:
			printf("    720 x 576i @ 50Hz\r\n");
			break;
		case GV7601_STD_1280X720P_24:
			printf("    1280 x 720p @ 24Hz\r\n");
			break;
		case GV7601_STD_1280X720P_25:
			printf("    1280 x 720p @ 25Hz\r\n");
			break;
		case GV7601_STD_1280X720P_30:
			printf("    1280 x 720p @ 30Hz\r\n");
			break;
		case GV7601_STD_1280X720P_60 :
			printf("    1280 x 720p @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_1280X720P_50 :
			printf("    1280 x 720p @ 50Hz\r\n");
			break;
		case GV7601_STD_1920X1080I_60:
			printf("    1920 x 1080i @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_1920X1080I_50:
			printf("    1920 x 1080i @ 50Hz\r\n");
			break;
		case GV7601_STD_1920X1080P_30:
			printf("    1920 x 1080p @ 29.97/30Hz\r\n");
			break;
		case GV7601_STD_1920X1080P_25:
			printf("    1920 x 1080p @ 25Hz\r\n");
			break;
		case GV7601_STD_1920X1080P_24:
			printf("    1920 x 1080p @ 24Hz\r\n");
			break;
		case GV7601_STD_1920X1080P_60:
			printf("    1920 x 1080p @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_1920X1080P_50:
			printf("    1920 x 1080p @ 50Hz\r\n");
			break;
		case GV7601_STD_1920X1035I_60:
			printf("    1920 x 1035i @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_1920X1035I_50:
			printf("    1920 x 1035i @ 50Hz\r\n");
			break;
		case GV7601_STD_1920X1035P_60:
			printf("    1920 x 1035p @ 59.94/60Hz\r\n");
			break;
		case GV7601_STD_1920X1035P_50:
			printf("    1920 x 1035p @ 50Hz\r\n");
			break;
		case GV7601_STD_INVALID:
			printf("Invalid video std\r\n");
			break;
		default:
			printf("Invalid std val:%d\r\n",Stp_TempGV7601Status_t->VideoState);
			break;
	}
	if(Stp_TempGV7601Status_t->VideoState == GV7601_STD_INVALID)
		return GV7601_STD_INVALID;
	printf("VD_STD: Total Words Per Line = %d\r\n", Stp_TempGV7601Status_t->VideoHTotalSize);
	printf("VD_STD: Words Per Active Line = %d\r\n", Stp_TempGV7601Status_t->VideoHSize);
	printf("VD_STD: Total Lines Per Frame = %d\r\n", Stp_TempGV7601Status_t->VideoVTotalSize);
	printf("VD_STD: Active lines per frame = %d\r\n", Stp_TempGV7601Status_t->VideoVSize);
	printf("VD_STD: %s\r\n", (Stp_TempGV7601Status_t->VideoInterlaced == 1) ? "Interlaced" : "Progressive");
	return Stp_TempGV7601Status_t->VideoState;
}

/********************************  END  *************************************/
