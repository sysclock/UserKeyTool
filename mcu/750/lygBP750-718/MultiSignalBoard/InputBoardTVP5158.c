#define __INPUT_BOARD_TVP5158_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardTVP5158.c                                                      */
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
#include "BoardID.h"
#include "../MultiSignalBoard/Param.h"
#include "InputBoardTVP5158.h"

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern const ST_INPUT_BOARD_TVP5158_SIGNAL_INDEX St_InputBoardTVP5158SignalIndex[];
ST_INPUT_BOARD_TVP5158 gSt_InputBoardTVP5158;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void InitInputBoardTVP5158(void);
void InputBoardTVP5158Task(void);
BYTE InputBoardTVP5158GetIndexBySoltIndex(BYTE B_SoltIndex);
int InputBoardTVP5158SetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
int InputBoardTVP5158SetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
int InputBoardTVP5158SetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
int InputBoardTVP5158SetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
int InputBoardTVP5158GetSignalWidthHeight(BYTE B_InputIndex, long *pXStart, long *pYStart, long *pWidth, long *pHeight);
WORD InputBoardTVP5158GetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel);
WORD InputBoardTVP5158GetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel);
WORD InputBoardTVP5158GetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel);
WORD InputBoardTVP5158GetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
//static BYTE TVP5158_PollInterruptFlag = 0;
//static BYTE gB_TVP5158SignalType = TVP5158_NTSC;
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void InitInputBoardTVP5158(void)
{
	int i = 0, TempData = 0;
	BYTE B_Data = 0;
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
//	ST_TVP5158_CHANNEL_STATUS *Stp_TempTVP5158ChannelStatus = NULL;
	memset(&gSt_InputBoardTVP5158, 0, sizeof(gSt_InputBoardTVP5158));
	for(i = 0; i < INPUT_BOARD_MAX; i++)
	{
		BoardIICBusSwitch(i);
		B_Data = TVP5158_CheckChipId(TVP5158_DEV5);
		if(B_Data == OK)
		{
			Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[gSt_InputBoardTVP5158.B_ActiveChip]);
			Stp_TempDeviceTVP5158->B_BoardIndex = i;
			Stp_TempDeviceTVP5158->B_I2CBusIndex = 0;
			Stp_TempDeviceTVP5158->B_SubI2CBusIndex = TVP5158_DEV5;
			TVP5158_SoftReset(TVP5158_DEV5);
			TVP5158_SetOfmMode(TVP5158_DEV5, MCVIP_VIDEO_DECODER_MODE_4CH_D1, 0, 0, 0);
			gSt_InputBoardTVP5158.B_ActiveChip++;
			#if 1
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT0_BRIGHTNESS + (i * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)));
			TVP5158_SetLuminanceBrightness(TVP5158_DEV5, TempData, 0);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT0_CONTRAST + (i * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)));
			TVP5158_SetLuminanceContrast(TVP5158_DEV5, TempData, 0);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT0_SATURATION + (i * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)));
			TVP5158_SetLuminanceSaturation(TVP5158_DEV5, TempData, 0);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT0_HUE + (i * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)));
			TVP5158_SetLuminanceHue(TVP5158_DEV5, TempData, 0);
			
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT1_BRIGHTNESS + (i * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)));
			TVP5158_SetLuminanceBrightness(TVP5158_DEV5, TempData, 1);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT1_CONTRAST + (i * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)));
			TVP5158_SetLuminanceContrast(TVP5158_DEV5, TempData, 1);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT1_SATURATION + (i * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)));
			TVP5158_SetLuminanceSaturation(TVP5158_DEV5, TempData, 1);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT1_HUE + (i * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)));
			TVP5158_SetLuminanceHue(TVP5158_DEV5, TempData, 1);

			TempData = ParamRead(NVR_TVP5158_DEV0_PORT2_BRIGHTNESS + (i * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)));
			TVP5158_SetLuminanceBrightness(TVP5158_DEV5, TempData, 2);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT2_CONTRAST + (i * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)));
			TVP5158_SetLuminanceContrast(TVP5158_DEV5, TempData, 2);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT2_SATURATION + (i * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)));
			TVP5158_SetLuminanceSaturation(TVP5158_DEV5, TempData, 2);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT2_HUE + (i * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)));
			TVP5158_SetLuminanceHue(TVP5158_DEV5, TempData, 2);
			
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT3_BRIGHTNESS + (i * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)));
			TVP5158_SetLuminanceBrightness(TVP5158_DEV5, TempData, 3);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT3_CONTRAST + (i * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)));
			TVP5158_SetLuminanceContrast(TVP5158_DEV5, TempData, 3);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT3_SATURATION + (i * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)));
			TVP5158_SetLuminanceSaturation(TVP5158_DEV5, TempData, 3);
			TempData = ParamRead(NVR_TVP5158_DEV0_PORT3_HUE + (i * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)));
			TVP5158_SetLuminanceHue(TVP5158_DEV5, TempData, 3);
			#endif
			#if 0
			TempData = TVP5158_GetLuminanceBrightness(TVP5158_DEV5, 1);
			printf("Brightness = %d\r\n", TempData);
			TempData = TVP5158_GetLuminanceContrast(TVP5158_DEV5, 1);
			printf("Contrast = %d\r\n", TempData);
			TempData = TVP5158_GetLuminanceSaturation(TVP5158_DEV5, 1);
			printf("Saturation = %d\r\n", TempData);
			TempData = TVP5158_GetLuminanceHue(TVP5158_DEV5, 1);
			printf("Hue = %d\r\n", TempData);
			#endif
			printf("Board %d , Input %d , TVP5158 Init OK\r\n", Stp_TempDeviceTVP5158->B_BoardIndex, Stp_TempDeviceTVP5158->B_SubI2CBusIndex);
		}
	}
	printf("InitInputBoardTVP5158 OK! ActiveChip = %d\r\n", gSt_InputBoardTVP5158.B_ActiveChip);
}

int InputBoardTVP5158GetSignalType(BYTE B_InputIndex)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	BYTE Ret = 0;
	if(B_InputIndex >= 16)
		return CVBS_NO_SIGNAL;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[B_InputIndex / 4]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
//	printf("GetSignalType: Source = %d, BoardIndex = %d, I2CBusIndex = %d\r\n", TempData, Stp_TempDeviceTVP5158->B_BoardIndex, Stp_TempDeviceTVP5158->B_I2CBusIndex);
//	printf("GetSignalType: ChipDevIndex = %d, DecoderIndex = %d\r\n", Stp_TempTVP5158SignalIndex->B_ChipDevIndex, Stp_TempTVP5158SignalIndex->B_ChipDecoderIndex);
	Ret = TVP5158_GetSignalType(TVP5158_DEV5, B_InputIndex % 4);
	return  Ret;
}

BYTE InputBoardTVP5158GetIndexBySoltIndex(BYTE B_SoltIndex)
{
	BYTE i = 0;
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	if(B_SoltIndex >= INPUT_BOARD_MAX)
		return 0xFF;
	for(i = 0; i < gSt_InputBoardTVP5158.B_ActiveChip; i++)
	{
		Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
		if(Stp_TempDeviceTVP5158->B_BoardIndex == B_SoltIndex)
			return i;
	}
	return 0xFF;
}

int InputBoardTVP5158SetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	BYTE Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_SetLuminanceBrightness(TVP5158_DEV5, B_Value, channel);
	return  Ret;
}

int InputBoardTVP5158SetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	BYTE Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_SetLuminanceContrast(TVP5158_DEV5, B_Value, channel);
	return  Ret;
}

int InputBoardTVP5158SetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	BYTE Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_SetLuminanceSaturation(TVP5158_DEV5, B_Value, channel);
	return  Ret;
}

int InputBoardTVP5158SetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	BYTE Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_SetLuminanceHue(TVP5158_DEV5, B_Value, channel);
	return  Ret;
}

WORD InputBoardTVP5158GetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	WORD Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_GetLuminanceBrightness(TVP5158_DEV5, channel);
	return  Ret;
}

WORD InputBoardTVP5158GetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	WORD Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_GetLuminanceContrast(TVP5158_DEV5, channel);
	return  Ret;
}

WORD InputBoardTVP5158GetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	WORD Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_GetLuminanceSaturation(TVP5158_DEV5, channel);
	return  Ret;
}

WORD InputBoardTVP5158GetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel)
{
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	WORD Ret = 0, i = 0;
	i = InputBoardTVP5158GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return i;
	Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
	BoardIICBusSwitch(Stp_TempDeviceTVP5158->B_BoardIndex);
	Ret = TVP5158_GetLuminanceHue(TVP5158_DEV5, channel);
	return  Ret;
}

#if 0
int InputBoardTVP5158GetSignalWidthHeight(BYTE B_InputIndex, long *pXStart, long *pYStart, long *pWidth, long *pHeight)
{
	BYTE B_TempData = 0;
	if(B_InputIndex >= (gSt_InputBoardTVP5158.B_ActiveChip * 4))
	{
		*pXStart = 0;
		*pYStart = 0;
		*pWidth = TVP5158CVBSVideoCaptureWidth;
		*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
		return 0;
	}
	#if 0
	B_TempData = InputBoardTVP5158GetSignalType(0);
	if((B_TempData == TVP5158_NO_SIGNAL) || ((B_TempData == TVP5158_NTSC)))
		gB_TVP5158SignalType = TVP5158_NTSC;
	else
		gB_TVP5158SignalType = TVP5158_PAL;
	#endif
	B_TempData = InputBoardTVP5158GetSignalType(B_InputIndex);
	switch(B_InputIndex % 4)
	{
		case 0:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				#if 0
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = 0;
				*pHeight = 0;
				return 0;
				#else
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
				#endif
			}
			break;
		case 1:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = 0;
				*pWidth = 0;
				*pHeight = 0;
				return 0;
			}
			break;
		case 2:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = 0;
				*pYStart = TVP5158CVBSVideoNTSCCaptureHeight;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = 0;
				*pYStart = TVP5158CVBSVideoPALCaptureHeight;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				*pXStart = 0;
				*pYStart = TVP5158CVBSVideoPALCaptureHeight;
				*pWidth = 0;
				*pHeight = 0;
				return 0;
			}
			break;
		case 3:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = TVP5158CVBSVideoNTSCCaptureHeight;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = TVP5158CVBSVideoPALCaptureHeight;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				*pXStart = TVP5158CVBSVideoCaptureWidth;
				*pYStart = TVP5158CVBSVideoPALCaptureHeight;
				*pWidth = 0;
				*pHeight = 0;
				return 0;
			}
			break;
			default:
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
				break;
	}
	return 1;
}

#else
int InputBoardTVP5158GetSignalWidthHeight(BYTE B_InputIndex, long *pXStart, long *pYStart, long *pWidth, long *pHeight)
{
	BYTE B_TempData = 0;
	if(B_InputIndex >= (gSt_InputBoardTVP5158.B_ActiveChip * 4))
	{
		*pXStart = 0;
		*pYStart = 0;
		*pWidth = TVP5158CVBSVideoCaptureWidth;
		*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
		return 0;
	}
	#if 0
	B_TempData = InputBoardTVP5158GetSignalType(0);
	if((B_TempData == TVP5158_NO_SIGNAL) || ((B_TempData == TVP5158_NTSC)))
		gB_TVP5158SignalType = TVP5158_NTSC;
	else
		gB_TVP5158SignalType = TVP5158_PAL;
	#endif
	B_TempData = InputBoardTVP5158GetSignalType(B_InputIndex);
	switch(B_InputIndex % 4)
	{
		case 0:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				#if 1
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = 0;
				*pHeight = 0;
//				return 0;
				#else
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
				#endif
			}
			break;
		case 1:
		case 2:
		case 3:
			if(B_TempData == TVP5158_NTSC)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			}
			else if(B_TempData == TVP5158_PAL)
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = TVP5158CVBSVideoCaptureWidth;
				*pHeight = TVP5158CVBSVideoPALCaptureHeight;
			}
			else
			{
				*pXStart = 0;
				*pYStart = 0;
				*pWidth = 0;
				*pHeight = 0;
//				return 0;
			}
			break;
		default:
			*pXStart = 0;
			*pYStart = 0;
			*pWidth = TVP5158CVBSVideoCaptureWidth;
			*pHeight = TVP5158CVBSVideoNTSCCaptureHeight;
			break;
	}
	return 1;
}

#endif
void InputBoarTVP5158Task(void)
{

}

/********************************  END  *************************************/
