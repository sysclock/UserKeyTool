/*************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
//============================================================================*/
//
// MODULE:      ScreenWindowLayout.c
//
// USAGE:       This module make display single screen  windows.
//              
//
// DESIGNER:	HAN LONG
/************************************************************************************************************/

/***************************************************************************/
/*	I N C L U D E    F I L E S												*/
/***************************************************************************/
#include <string.h>
#include <stdio.h>
#include "ScreenWindowLayout.h"
#include "SignalWindowsCrop.h"
#ifndef PC_VC_DEBUG
//	#include "../System Code/lpc17xx.h"
	#include "../System Code/type.h"
	#include "../System Code/global.h"
	#include "../System Code/Timer.h"
//	#include "../MultiSignalBoard/InputBoardSiI9233.h"
	#include "../MultiSignalBoard/InputBoardTVP5158.h"
	#include "../MultiSignalBoard/InputBoardADV7604.h"
	#include "../MultiSignalBoard/InputBoardGV7601.h"
//	#include "../MultiSignalBoard/InputBoardIPDecoder.h"
	#include "../MultiSignalBoard/InputBoardGraphicFPGA.h"
//	#include "../MultiSignalBoard/OutputBoardGraphicFPGA.h"
#include "../MultiSignalBoard/OutputBoardSiI9136.h"
	#include "../Device/InputFPGA/InputFPGA.h"
//	#include "../Device/OutputFPGA/OutputFPGA.h"
	#include "../MultiSignalBoard/param.h"
#else
#endif

#define SCREEN_WINDOW_LAYOUT_DEBUG			0 //3
#define MAX_SCREEN_WIDTH								2048
#define MAX_SCREEN_HEIGHT								2048
#define CVBSVideoCaptureHWidth 							640
#define CVBSVideoCaptureHStartOffset 				8
#define CVBSVideoCaptureVHeight 						540

extern void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture);
/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/********************************************************************************/
ST_Screen_Rect gSt_ScreenRectInput[OUTPUT_BOARD_MAX] = {0}, *gStp_ScreenRectInput = NULL;
ST_ProcessorInputSignal gSt_ProcessorInputSignal;
ST_ProcessorInputSignal *gStp_ProcessorInputSignal = &(gSt_ProcessorInputSignal);
ST_ProcessorScaler gSt_ProcessorScaler;
ST_ProcessorScaler *gStp_ProcessorScaler = &(gSt_ProcessorScaler);
long	gW_ResolutionXStart = 0, gW_ResolutionYStart = 0, gW_ResolutionWidth = 0, gW_ResolutionHeight = 0;

/********************************************************************************/
/*	S T A T I C    V A R I A B L E S										*/
/********************************************************************************/
static ST_SignalSource gSt_StaticWindowLayoutSignalSource[MaxDiaplayWallWindowsNum * OUTPUT_BOARD_MAX] = {0};
static ST_SignalWindows gSt_StaticScreenRectInputSignalWindows[MaxDiaplayWallWindowsNum * OUTPUT_BOARD_MAX] = {0};
static ST_Screen_Rect_Output* gStp_ScreenRectOutput;
static XScalerAperture St_WindowLayoutAperture;
static BYTE g_CVBSSwitchPortType = TVP5158_NO_SIGNAL;
/****************************************************************************/
/*	G L O B A L    R O U T I N E S									 */
/****************************************************************************/
void ScreenWindowLayoutInit(BYTE B_Index);
void ScreenWindowLayoutInputSignalInit(void);
void ScreenWindowLayoutTask(void);
void ExecuteScreenWindowLayout(void);
int GetInputBoardSignalResolution(BYTE B_SlotIndex, BYTE B_SignalIndex, long *XStart, long *YStart, long *pResolutionWidth, long *pResolutionHeight);
BYTE GetBoardSlotType(BYTE B_SlotIndex);
int GraphicProcessorBoardPortSwitch(BYTE B_InputSlotIndex, BYTE B_InputPortIndex, BYTE B_OutputSlotIndex, BYTE B_OutputPortIndex);
/****************************************************************************/
/*	S T A T I C    R O U T I N E S									 */
/****************************************************************************/
static void ST_Screen_Rect_Init(ST_Screen_Rect* Stp_ScreenRect);
static void ScreenWindowLayoutVariableInit(BYTE B_Index);
static void ScreenWindowLayoutSortByLayer(ST_Screen_Rect* Stp_ScreenRectInput);
static int ScreenWindowLayoutCheckScreenRectInput(ST_Screen_Rect* Stp_ScreenRectInput);
static ST_Screen_Rect_Output* CombineSingleScreenYWindows(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static void CheckScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput, ST_Screen_Rect*	Stp_ScreenRectInput);
static void PrintScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static void SetScreenRectOutput(BYTE B_SlotIndex, BYTE B_OutputPortIndex, ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static int ScreenWindowLayoutRequestOneScaler(BYTE B_SlotIndex, ST_SignalWindows* Stp_SignalWindows);
static int ScreenWindowLayoutFindScalerIndex(BYTE B_SlotIndex, ST_SignalWindows* Stp_SignalWindows);
static int CheckSignalInScreenWindowLayout(unsigned char B_SignalSourceSlot, unsigned char B_SignalSource);
static int CheckSignalInScalerInput(unsigned char B_SignalSourceSlot, unsigned char B_SignalSource);
/*****************************************************************************/
/*	C O D E																										*/
/*****************************************************************************/
void ScreenWindowLayoutInit(BYTE B_Index)
{
	if(B_Index >= OUTPUT_BOARD_MAX)
		return;
//	ST_Screen_Rect_Init(gStp_ScreenRectInput);
	ScreenWindowLayoutVariableInit(B_Index);
	gStp_ScreenRectInput = &gSt_ScreenRectInput[B_Index];
}



void ScreenWindowLayoutInputSignalInit(void)
{
	BYTE i = 0;
	ST_InputSignal *Stp_TempInputSignal = NULL;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
	memset(gStp_ProcessorInputSignal, 0 ,sizeof(gSt_ProcessorInputSignal));
	gStp_ProcessorInputSignal = &(gSt_ProcessorInputSignal);
	gW_ResolutionXStart = 0;
	gW_ResolutionYStart = 0;
	gW_ResolutionWidth = 0;
	gW_ResolutionHeight = 0;
	
	for(i = 0; i < gSt_InputBoardADV7604.B_ActiveChip; i++)
	{
		Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceADV7604->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 0;
		Stp_TempInputSignal->B_SignalTYPE = 0;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
	}

	
	for(i = 0; i < gSt_InputBoardGV7601.B_ActiveChip; i++)
	{
		Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[i]);
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceGV7601->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 0;
		Stp_TempInputSignal->B_SignalTYPE = 0;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
	}

	
	for(i = 0; i < gSt_InputBoardTVP5158.B_ActiveChip; i++)
	{
		Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[i]);
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceTVP5158->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 0;
		Stp_TempInputSignal->B_SignalTYPE = 1;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceTVP5158->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 1;
		Stp_TempInputSignal->B_SignalTYPE = 1;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceTVP5158->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 2;
		Stp_TempInputSignal->B_SignalTYPE = 1;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->B_SlotIndex = Stp_TempDeviceTVP5158->B_BoardIndex;
		Stp_TempInputSignal->B_SignalSource = 3;
		Stp_TempInputSignal->B_SignalTYPE = 1;
		Stp_TempInputSignal->W_StdHSart = 1;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
	}
	memset(&gSt_ProcessorScaler, 0 ,sizeof(gSt_ProcessorScaler));
	gStp_ProcessorScaler = &(gSt_ProcessorScaler);
}






void ScreenWindowLayoutTask(void)
{
	int i = 0, j = 0, n = 0, TempData1 = 0, TempData2 = 0;
	WORD W_TempData = 0;
	unsigned char B_RefreshScreenWindowLayout[OUTPUT_BOARD_MAX] = {0};
	unsigned char B_RefreshScalerConfig = 0, B_RefrshInputFPGASetUpdata = 0;
	long long TempllData = 0;
	ST_InputSignal *Stp_TempInputSignal = NULL;
	ST_ScalerType *Stp_TempScaler = NULL;
	memset(B_RefreshScreenWindowLayout, 0, sizeof(B_RefreshScreenWindowLayout));
	B_RefreshScalerConfig = 0;
	B_RefrshInputFPGASetUpdata = 0;
	
	// Check CVBS TYPE
	if(gB_CVBSSwitchEnable == 1)
	{
		j = InputBoardTVP5158GetSignalType(gB_CVBSSwitchIndex);
		W_TempData = gB_CVBSSwitchIndex;
		W_TempData <<= 8;
		if(j == TVP5158_PAL)
			InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_TempData | BIT0);
		else
			InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_TempData);
		
		if(j != g_CVBSSwitchPortType)
		{
			for(i = 0; i < gStp_ProcessorInputSignal->B_ActiveInputSignalNum; i++)
			{
				Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[i]);
				if((Stp_TempInputSignal->B_SlotIndex == 0) && (Stp_TempInputSignal->B_SignalSource == gB_CVBSSwitchIndex))
					Stp_TempInputSignal->W_StdWidth = 0;
			}
			for(i = 0; i < gStp_ProcessorScaler->B_ActiveScalerNum; i++)
			{
				Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
				if(Stp_TempScaler == NULL)
					continue;
				if(Stp_TempScaler->B_SignalSourceSlot == 0)
					Stp_TempScaler->B_Refresh = 1;
			}
			g_CVBSSwitchPortType = j;
		}
	}
	
	for(i = 0; i < gStp_ProcessorInputSignal->B_ActiveInputSignalNum; i++)
	{
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[i]);
		if((Stp_TempInputSignal->B_SlotIndex == 0) && (Stp_TempInputSignal->B_SignalSource == gB_CVBSSwitchIndex) && (gB_CVBSSwitchEnable == 1))
			W_TempData = gB_CVBSSwitchIndex;
		else
			W_TempData = Stp_TempInputSignal->B_SignalSource;
		if(GetInputBoardSignalResolution(Stp_TempInputSignal->B_SlotIndex, W_TempData, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight) == 1)
//		GetInputBoardSignalResolution(Stp_TempInputSignal->B_SlotIndex, Stp_TempInputSignal->B_SignalSource, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
		{
			j = CheckSignalInScalerInput(Stp_TempInputSignal->B_SlotIndex , 0);
			if(j != -1)
			{
				Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[j]);
				TempData1 = Stp_TempScaler->InLastPixel - Stp_TempScaler->InFirstPixel + 1;
				TempData2 = Stp_TempScaler->InLastLine - Stp_TempScaler->InFirstLine + 1;
				if((Stp_TempScaler->B_SignalSource == 1) && (TempData1 > TVP5158CVBSVideoCaptureWidth) && (TempData2 > TVP5158CVBSVideoPALCaptureHeight))
				{
					gW_ResolutionWidth <<= 1;
					gW_ResolutionHeight <<= 1;
				}
			}
			
			if((Stp_TempInputSignal->W_StdHSart != gW_ResolutionXStart)
				|| (Stp_TempInputSignal->W_StdVSart != gW_ResolutionYStart)
				|| (Stp_TempInputSignal->W_StdWidth != gW_ResolutionWidth)
				|| (Stp_TempInputSignal->W_StdHeight != gW_ResolutionHeight)
				)
			{
				if((Stp_TempInputSignal->W_StdWidth > gW_ResolutionWidth - 5) && (Stp_TempInputSignal->W_StdWidth < gW_ResolutionWidth + 5)
						&& (Stp_TempInputSignal->W_StdHeight > gW_ResolutionHeight - 3) && (Stp_TempInputSignal->W_StdHeight < gW_ResolutionHeight + 3))
					continue;
				for(j = 0; j < gStp_ProcessorScaler->B_ActiveScalerNum; j++)
				{
					Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[j]);
					if(Stp_TempScaler == NULL)
						continue;
					if(Stp_TempScaler->B_SignalSourceSlot == Stp_TempInputSignal->B_SlotIndex)
					{
						B_RefreshScalerConfig = 1;
						if((Stp_TempScaler->InHoriSize != gW_ResolutionWidth) || (Stp_TempScaler->InVertSize != gW_ResolutionHeight))
						{
							ReadUserScalerParam(j, &St_WindowLayoutAperture);
							if((St_WindowLayoutAperture.InHoriSize == gW_ResolutionWidth) && (St_WindowLayoutAperture.InVertSize == gW_ResolutionHeight))
							{
								Stp_TempScaler->InHoriSize = gW_ResolutionWidth;
								Stp_TempScaler->InVertSize = gW_ResolutionHeight;
								Stp_TempScaler->InFirstPixel = St_WindowLayoutAperture.InFirstPixel;
								Stp_TempScaler->InFirstLine = St_WindowLayoutAperture.InFirstLine;
								Stp_TempScaler->InLastPixel = St_WindowLayoutAperture.InLastPixel;
								Stp_TempScaler->InLastLine = St_WindowLayoutAperture.InLastLine;
							}
							else
							{
								Stp_TempScaler->InHoriSize = gW_ResolutionWidth;
								Stp_TempScaler->InVertSize = gW_ResolutionHeight;
								TempllData = gW_ResolutionWidth;
								TempllData *= SCALE_COEFFICIENT;
								TempllData /= St_WindowLayoutAperture.InHoriSize;
								W_TempData = TempllData;
								TempllData = St_WindowLayoutAperture.InFirstPixel;
								TempllData *= W_TempData;
								TempllData /= SCALE_COEFFICIENT;
								Stp_TempScaler->InFirstPixel = TempllData;
								TempllData = St_WindowLayoutAperture.InLastPixel;
								TempllData *= W_TempData;
								TempllData /= SCALE_COEFFICIENT;
								Stp_TempScaler->InLastPixel = TempllData;
								
								TempllData = gW_ResolutionHeight;
								TempllData *= SCALE_COEFFICIENT;
								TempllData /= St_WindowLayoutAperture.InVertSize;
								W_TempData = TempllData;
								TempllData = St_WindowLayoutAperture.InFirstLine;
								TempllData *= W_TempData;
								TempllData /= SCALE_COEFFICIENT;
								Stp_TempScaler->InFirstLine = TempllData;
								TempllData = St_WindowLayoutAperture.InLastLine;
								TempllData *= W_TempData;
								TempllData /= SCALE_COEFFICIENT;
								Stp_TempScaler->InLastLine = TempllData;
							}
							Stp_TempScaler->B_Refresh = 1;
							#if(SCREEN_WINDOW_LAYOUT_DEBUG == 3)
							printf("Scaler %d Refresh, SignalSource = %d, InFirstPixel = %d, InLastPixel = %d, InFirstLine = %d, InLastLine = %d\r\n", j, Stp_TempInputSignal->B_SignalSource, Stp_TempScaler->InFirstPixel, Stp_TempScaler->InLastPixel, Stp_TempScaler->InFirstLine, Stp_TempScaler->InLastLine);
							#endif
						}
					}
				}
				Stp_TempInputSignal->W_StdHSart = gW_ResolutionXStart;
				Stp_TempInputSignal->W_StdVSart = gW_ResolutionYStart;
				Stp_TempInputSignal->W_StdWidth = gW_ResolutionWidth;
				Stp_TempInputSignal->W_StdHeight = gW_ResolutionHeight;
				
				for(n = 0; n < gSt_OutputBoarSiI9136.B_ActiveChip; n++)
				{
					gStp_ScreenRectInput = &gSt_ScreenRectInput[n];
					j = CheckSignalInScreenWindowLayout(Stp_TempInputSignal->B_SlotIndex, Stp_TempInputSignal->B_SignalSource);
	//				if((j == 1) && (gW_ResolutionWidth != 0) && (gW_ResolutionHeight != 0))
					if(j == 1)
						B_RefreshScreenWindowLayout[n] = 1;
				}
				#if(SCREEN_WINDOW_LAYOUT_DEBUG == 3)
				/*
				printf("W_StdHSart = %d, gW_ResolutionXStart = %d\r\n", Stp_TempInputSignal->W_StdHSart, gW_ResolutionXStart);
				printf("W_StdVSart = %d, gW_ResolutionYStart = %d\r\n", Stp_TempInputSignal->W_StdVSart, gW_ResolutionYStart);
				printf("W_StdWidth = %d, gW_ResolutionWidth = %d\r\n", Stp_TempInputSignal->W_StdWidth, gW_ResolutionWidth);
				printf("W_StdHeight = %d, gW_ResolutionWidth = %d\r\n", Stp_TempInputSignal->W_StdHeight, gW_ResolutionWidth);
				*/
				printf("SignalSource %d Change, i = %d, SignalpResolutionWidth = %d, SignalpResolutionHeight = %d\r\n", Stp_TempInputSignal->B_SlotIndex, i, gW_ResolutionWidth, gW_ResolutionHeight);
				#endif
			}
		}
	}
	for(n = 0; n < gSt_OutputBoarSiI9136.B_ActiveChip; n++)
	{
		if(B_RefreshScreenWindowLayout[n] == 1)
		{
			#if(SCREEN_WINDOW_LAYOUT_DEBUG == 3)
			printf("\r\nRefresh Screen Window %d Layout.\r\n", n);
			#endif
			gStp_ScreenRectInput = &gSt_ScreenRectInput[n];
			ExecuteScreenWindowLayout();
			B_RefreshScreenWindowLayout[n] = 0;
			B_RefrshInputFPGASetUpdata = 1;
		}
	}
	if(B_RefreshScalerConfig == 1)
	{
//		W_ScalerMask = 0;
		for(i = 0; i < gStp_ProcessorScaler->B_ActiveScalerNum; i++)
		{
			Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
			if((Stp_TempScaler->B_SignalSourceSlot < 16) && (Stp_TempScaler->B_Refresh == 1))
			{
				#if(SCREEN_WINDOW_LAYOUT_DEBUG == 3)
				printf("\r\nRefresh Scaler %d Config.\r\n", i);
				#endif
				if((Stp_TempScaler->B_SignalSourceSlot == 0) && (gB_CVBSSwitchEnable == 1))
					GetInputBoardSignalResolution(Stp_TempScaler->B_SignalSourceSlot, gB_CVBSSwitchIndex, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
				else
					GetInputBoardSignalResolution(Stp_TempScaler->B_SignalSourceSlot, Stp_TempScaler->B_SignalSource, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
				if((i < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				{
					InputFPGA_ScalerSetNoSignalOutput(i, 1);
//					W_ScalerMask |= (1 << i);
				}
				else
				{
					InputFPGA_ScalerSetNoSignalOutput(i, 0);
				}
				St_WindowLayoutAperture.InHoriSize = Stp_TempScaler->InHoriSize;
				St_WindowLayoutAperture.InVertSize = Stp_TempScaler->InVertSize;
				St_WindowLayoutAperture.InFirstPixel = Stp_TempScaler->InFirstPixel;
				St_WindowLayoutAperture.InLastPixel = Stp_TempScaler->InLastPixel;
				St_WindowLayoutAperture.InFirstLine = Stp_TempScaler->InFirstLine;
				St_WindowLayoutAperture.InLastLine = Stp_TempScaler->InLastLine;
				St_WindowLayoutAperture.OutHoriSize = Stp_TempScaler->OutHoriSize;
				St_WindowLayoutAperture.OutVertSize = Stp_TempScaler->OutVertSize;
				InputFPGA_ScalerSetup(i, Stp_TempScaler->B_SignalSourceSlot << 1, &St_WindowLayoutAperture);
				Stp_TempScaler->B_Refresh = 0;
			}
		}
		B_RefrshInputFPGASetUpdata = 1;
	}
	if(B_RefrshInputFPGASetUpdata == 1)
		InputFPGASetUpdata();
}

void ExecuteScreenWindowLayout(void)
{
	if((gStp_ScreenRectInput ->ActiveRectWindowsNum <= 0) || (gStp_ScreenRectInput ->ActiveRectWindowsNum >= MaxDiaplayWallWindowsNum))
		return;
	ScreenWindowLayoutSortByLayer(gStp_ScreenRectInput);
	if(ScreenWindowLayoutCheckScreenRectInput(gStp_ScreenRectInput) == -1)
		return;
	gStp_ScreenRectOutput = MakeScreenRect(gStp_ScreenRectInput, 0, gStp_ScreenRectInput->DisplayScreenWidth,
																															 0, gStp_ScreenRectInput->DisplayScreenHeight);
	if(gStp_ScreenRectOutput == NULL)
	{
		printf("\r\nERROR:Creat Windows Too Much .\r\n");
		return;
	}
	CheckScreenRectOutput(gStp_ScreenRectOutput, gStp_ScreenRectInput);
	CombineSingleScreenYWindows(gStp_ScreenRectOutput);
//	RemoveBackgroundWindows(gStp_ScreenRectOutput);
	#if(SCREEN_WINDOW_LAYOUT_DEBUG == 1)
	printf("\r\nBoardSlot = %d, OutputScreen = %d\r\n", gStp_ScreenRectInput->B_BoardSlotIndex, gStp_ScreenRectInput->B_OutputScreenIndex);
	PrintScreenRectOutput(gStp_ScreenRectOutput);
	#endif
	SetScreenRectOutput(gStp_ScreenRectInput->B_BoardSlotIndex, gStp_ScreenRectInput->B_OutputScreenIndex, gStp_ScreenRectOutput);
}




int mm=2;
int n=0,k=0;
int width=0;
int high=0;
int GetInputBoardSignalResolution(BYTE B_SlotIndex, BYTE B_SignalIndex, long *XStart, long *YStart, long *pResolutionWidth, long *pResolutionHeight)
{
	BYTE i = 0, j = 0, B_TempData = 0;
	n=0;
	mm=2;
	width=0;
	high=0;
	sprintf(uart3_send, "BOARD RISR %d %d\r\n", B_SlotIndex,B_SignalIndex);	
	UART3_SendBuf(uart3_send,strlen(uart3_send));	
	Usart3_Rev_Idx=0;
	Delay100ms(30);
// 	while(i<5)
// 	{
			if(Usart3_Rev_Idx>10&&Usart3_Rev_Buf[Usart3_Rev_Idx-2]==0x0D&&Usart3_Rev_Buf[Usart3_Rev_Idx-1]==0x0A)
			{	
				while(j<2)
				{
					n=mm-1;
					mm++;
					while(Usart3_Rev_Buf[Usart3_Rev_Idx-mm]>47&&Usart3_Rev_Buf[Usart3_Rev_Idx-mm]<58)
					{		
						mm++;
						
					}	
					j++;
				}
				mm--;
				for(k=mm;k>n+1;k--)
					width=(width*10)+((Usart3_Rev_Buf[Usart3_Rev_Idx-k])-48);
				for(k=n;k>2;k--)
					high=(high*10)+((Usart3_Rev_Buf[Usart3_Rev_Idx-k])-48);
				i=7;
				Usart3_Rev_Idx=0;
				*pResolutionWidth = width;
				*pResolutionHeight = high;
				return 0;
			}
// 		i++;	
// 		Delay100ms(2);
// 	}
	*XStart = 0;
	*YStart = 0;
	*pResolutionWidth = 0;
	*pResolutionHeight = 0;
	return -1;
}




BYTE GetBoardSlotType(BYTE B_SlotIndex)
{
	BYTE i = 0, j = 0;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
//	ST_DEVICE_SI_I9233 *Stp_TempDeviceSiI9233 = NULL;
//	GlobalStatus_t *Stp_TempSiI9233Status_t = NULL;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
//	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	ST_DEVICE_GV7601 *Stp_TempDeviceGV7601 = NULL;
	ST_DEVICE_TVP5158 *Stp_TempDeviceTVP5158 = NULL;
//	ST_DEVICE_IP_DECODER *Stp_TempDeviceIPDecoder = NULL;
	ST_DEVICE_SI_I9136 *Stp_TempDeviceSiI9136 = NULL;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
//		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			switch(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType)
			{
				case INPUT_DEV_VDPA:
					for(j = 0; j < gSt_InputBoardADV7604.B_ActiveChip; j++)
					{
						Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[j]);
						if(Stp_TempDeviceADV7604->B_BoardIndex == B_SlotIndex)
						{
							return INPUT_DEV_ADV7604;
						}
					}
					for(j = 0; j < gSt_InputBoardGV7601.B_ActiveChip; j++)
					{
						Stp_TempDeviceGV7601 = &(gSt_InputBoardGV7601.St_DeviceGV7601[j]);
						if(Stp_TempDeviceGV7601->B_BoardIndex == B_SlotIndex)
						{
							return INPUT_DEV_GV7601;
						}
					}
					for(j = 0; j < (gSt_InputBoardTVP5158.B_ActiveChip * 4); j++)
					{
						Stp_TempDeviceTVP5158 = &(gSt_InputBoardTVP5158.St_DeviceTVP5158[j / 4]);
						if(Stp_TempDeviceTVP5158->B_BoardIndex == B_SlotIndex)
						{
							return INPUT_DEV_TVP5185_FOUR;
						}
					}
					for(j = 0; j < gSt_OutputBoarSiI9136.B_ActiveChip; j++)
					{
						Stp_TempDeviceSiI9136 = &(gSt_OutputBoarSiI9136.St_DeviceSiI9136[j]);
						if(Stp_TempDeviceSiI9136->B_BoardIndex == B_SlotIndex)
						{
							return OUTPUT_DEV_SiI9136;
						}
					}
					break;
				#if 0
				case INPUT_DEV_IP_DECODER:
					for(j = 0; j < gSt_InputBoardIPDecoder.B_ActiveChip; j++)
					{
						Stp_TempDeviceIPDecoder = &(gSt_InputBoardIPDecoder.St_DeviceIPDecoder[j]);
						if((Stp_TempDeviceIPDecoder->B_BoardIndex == B_SlotIndex) && ((j % 2) == B_SignalIndex))
						{
							*XStart = 0;
							*YStart = 0;
							if(InputBoardIPDecoderGetSignalWidthHeight(j, pResolutionWidth, pResolutionHeight) == 0)
							{
								*pResolutionWidth = 0;
								*pResolutionHeight = 0;
								return -1;
							}
							return 1;
						}
					}
					break;
				#endif
				default:
					break;
			}
		}
	}
	return INPUT_DEV_MAX;
}

static void ST_Screen_Rect_Init(ST_Screen_Rect* Stp_ScreenRect)
{
	int i = 0;
	Stp_ScreenRect->ActiveRectWindowsNum = 0;
	for(i = 0; i < (MaxDiaplayWallWindowsNum); i++)
		Stp_ScreenRect->Stp_RectWindows[i] = NULL;
}


static void ScreenWindowLayoutVariableInit(BYTE B_Index)
{
	int i = 0;
	if(B_Index >= OUTPUT_BOARD_MAX)
		return;
	gSt_ScreenRectInput[B_Index].ActiveRectWindowsNum = 0;
	for(i = 0; i < MaxDiaplayWallWindowsNum; i++)
	{
		gSt_ScreenRectInput[B_Index].Stp_RectWindows[i] = &(gSt_StaticScreenRectInputSignalWindows[(B_Index * MaxDiaplayWallWindowsNum) + i]);
		gSt_ScreenRectInput[B_Index].Stp_RectWindows[i]->Stp_SignalSource = &gSt_StaticWindowLayoutSignalSource[(B_Index * MaxDiaplayWallWindowsNum) + i];
		memset(&(gSt_ScreenRectInput[B_Index].Stp_RectWindows[i]->SignalInputWindow), 0, sizeof(ST_Rect));
		memset(&(gSt_ScreenRectInput[B_Index].Stp_RectWindows[i]->SignalDisplayWindow), 0, sizeof(ST_Rect));
		memset(gSt_ScreenRectInput[B_Index].Stp_RectWindows[i]->Stp_SignalSource, 0, sizeof(ST_SignalSource));
	}
}



static void ScreenWindowLayoutSortByLayer(ST_Screen_Rect* Stp_ScreenRectInput)
{
	int i, j, flag;
	ST_SignalWindows St_TempSignalWindows;
	for(i = Stp_ScreenRectInput->ActiveRectWindowsNum; i >= 2; i--)
	{
		flag = 0;
		for(j = 0; j <= i - 2; j++)
			if(Stp_ScreenRectInput->Stp_RectWindows[j]->Stp_SignalSource->Windows_Layer > Stp_ScreenRectInput->Stp_RectWindows[j + 1]->Stp_SignalSource->Windows_Layer)
			{
				//temp = LongArray[j];
				memcpy(&St_TempSignalWindows, Stp_ScreenRectInput->Stp_RectWindows[j], sizeof(ST_SignalWindows));
				memcpy(Stp_ScreenRectInput->Stp_RectWindows[j], Stp_ScreenRectInput->Stp_RectWindows[j + 1], sizeof(ST_SignalWindows));
				memcpy(Stp_ScreenRectInput->Stp_RectWindows[j + 1], &St_TempSignalWindows, sizeof(ST_SignalWindows));
				flag = 1;
			}
		if (flag == 0) break;
	}
}

static int ScreenWindowLayoutCheckScreenRectInput(ST_Screen_Rect* Stp_ScreenRectInput)
{
	int i;
	ST_SignalWindows *Stp_TempSignalWindows;
	ST_SignalSource *Stp_TempSignalSource;
	long long TempllData = 0;
	if(Stp_ScreenRectInput == NULL)
		return -1;
	for(i = 0; i < Stp_ScreenRectInput->ActiveRectWindowsNum; i++)
	{
		Stp_TempSignalWindows = Stp_ScreenRectInput->Stp_RectWindows[i];
		if(Stp_TempSignalWindows == NULL)
			return -1;
		Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
		if(Stp_TempSignalSource == NULL)
			return -1;
		if((Stp_TempSignalSource->OriginalSignalSource == 0) && (gB_CVBSSwitchEnable == 1))
			GetInputBoardSignalResolution(Stp_TempSignalSource->OriginalSignalSource, gB_CVBSSwitchIndex, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
		else
			GetInputBoardSignalResolution(Stp_TempSignalSource->OriginalSignalSource, 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
		if(((Stp_TempSignalSource->SignalSource < 4) || (Stp_TempSignalSource->SignalSource == 8)) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
			Stp_TempSignalSource->SignalSource = 8;
		else if((Stp_TempSignalSource->SignalSource == 8) && (gW_ResolutionWidth != 0) && (gW_ResolutionHeight != 0))
			Stp_TempSignalSource->SignalSource = Stp_TempSignalSource->OriginalSignalSource;
		else
			Stp_TempSignalSource->SignalSource = Stp_TempSignalSource->OriginalSignalSource;
		if(Stp_TempSignalSource->SignalSource > 8)
		{
//			Stp_TempSignalSource->SignalSource = 1;
			Stp_TempSignalSource->WindowsProperty |= BIT0;
		}
		else if (Stp_TempSignalSource->SignalSource == 8)
			Stp_TempSignalSource->WindowsProperty |= BIT1;
		else
			Stp_TempSignalSource->WindowsProperty &= ~(BIT0 | BIT1);
		if(Stp_TempSignalWindows->SignalDisplayWindow.left < 0)
			Stp_TempSignalWindows->SignalDisplayWindow.left = 0;
		if(Stp_TempSignalWindows->SignalDisplayWindow.top < 0)
			Stp_TempSignalWindows->SignalDisplayWindow.top = 0;
		if(Stp_TempSignalWindows->SignalDisplayWindow.right > Stp_ScreenRectInput->DisplayScreenWidth)
			Stp_TempSignalWindows->SignalDisplayWindow.right = Stp_ScreenRectInput->DisplayScreenWidth;
		if(Stp_TempSignalWindows->SignalDisplayWindow.bottom > Stp_ScreenRectInput->DisplayScreenHeight)
			Stp_TempSignalWindows->SignalDisplayWindow.bottom = Stp_ScreenRectInput->DisplayScreenHeight;
		if(Stp_TempSignalWindows->SignalDisplayWindow.right < Stp_TempSignalWindows->SignalDisplayWindow.left)
			return -1;
		if(Stp_TempSignalWindows->SignalDisplayWindow.bottom < Stp_TempSignalWindows->SignalDisplayWindow.top)
			return -1;
		TempllData = Stp_TempSignalWindows->SignalDisplayWindow.right - Stp_TempSignalWindows->SignalDisplayWindow.left;
		TempllData *= SCALE_COEFFICIENT;
		if((Stp_TempSignalWindows->SignalInputWindow.right - Stp_TempSignalWindows->SignalInputWindow.left) == 0)
		{
			#if(SCREEN_WINDOW_LAYOUT_DEBUG > 0)
			printf("HozScale Div Error. TempllData: %lld, right = %d, left = %d\r\n", TempllData, Stp_TempSignalWindows->SignalInputWindow.right, Stp_TempSignalWindows->SignalInputWindow.left);
			#endif
			return -1;
		}
		TempllData /= Stp_TempSignalWindows->SignalInputWindow.right - Stp_TempSignalWindows->SignalInputWindow.left;
		Stp_TempSignalSource->HozScale = (long) TempllData;
		TempllData = Stp_TempSignalWindows->SignalDisplayWindow.bottom - Stp_TempSignalWindows->SignalDisplayWindow.top;
		TempllData *= SCALE_COEFFICIENT;
		if((Stp_TempSignalWindows->SignalInputWindow.bottom - Stp_TempSignalWindows->SignalInputWindow.top) == 0)
		{
			#if(SCREEN_WINDOW_LAYOUT_DEBUG > 0)
			printf("VertScale Div Error. TempllData: %lld, bottom = %d, top = %d\r\n", TempllData, Stp_TempSignalWindows->SignalInputWindow.bottom, Stp_TempSignalWindows->SignalInputWindow.top);
			#endif
			return -1;
		}
		TempllData /= Stp_TempSignalWindows->SignalInputWindow.bottom - Stp_TempSignalWindows->SignalInputWindow.top;
		Stp_TempSignalSource->VertScale = (long) TempllData;
		
		if((Stp_TempSignalSource->HozScale != SCALE_COEFFICIENT) || (Stp_TempSignalSource->VertScale != SCALE_COEFFICIENT))
		{
		}
	}
	#if(SCREEN_WINDOW_LAYOUT_DEBUG == 2)
	printf("ScreenRectInput: BoardSlotIndex %d, OutputScreenIndex %d, ScreenWidth %d, ScreenHeight %d, ActiveRectWindows %d\r\n",
																	Stp_ScreenRectInput->B_BoardSlotIndex,
																	Stp_ScreenRectInput->B_OutputScreenIndex,
																	Stp_ScreenRectInput->DisplayScreenWidth,
																	Stp_ScreenRectInput->DisplayScreenHeight,
																	Stp_ScreenRectInput->ActiveRectWindowsNum);
	printf("ScreenRectInput: SignalSource, WindowsProperty, Windows_Layer, InLeft, InTop, InRight, InBottom, OutLeft, OutTop, OutRight, OutBottom\r\n");
	for(i = 0; i < Stp_ScreenRectInput->ActiveRectWindowsNum; i++)
	{
		Stp_TempSignalWindows = Stp_ScreenRectInput->Stp_RectWindows[i];
		if(Stp_TempSignalWindows == NULL)
			return -1;
		Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
		if(Stp_TempSignalSource == NULL)
			return -1;
		printf("%4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d\r\n",
								Stp_TempSignalSource->SignalSource,
								Stp_TempSignalSource->WindowsProperty,
								Stp_TempSignalSource->Windows_Layer,
								Stp_TempSignalWindows->SignalInputWindow.left,
								Stp_TempSignalWindows->SignalInputWindow.top,
								Stp_TempSignalWindows->SignalInputWindow.right,
								Stp_TempSignalWindows->SignalInputWindow.bottom,
								Stp_TempSignalWindows->SignalDisplayWindow.left,
								Stp_TempSignalWindows->SignalDisplayWindow.top,
								Stp_TempSignalWindows->SignalDisplayWindow.right,
								Stp_TempSignalWindows->SignalDisplayWindow.bottom);
	}
	#endif
	return 1;
}

static void CheckScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput, ST_Screen_Rect*	Stp_ScreenRectInput)
{
	int i = 0, j = 0, k = 0;
	ST_Screen_Rect *St_TempScreenRowRect = NULL;
	ST_SignalWindows *Stp_OutputRectWindow = NULL;
	ST_SignalSource *Stp_OutputRectSignalSource = NULL;
	ST_SignalWindows *Stp_ScreenRectInputWindow = NULL;
	ST_SignalSource *Stp_ScreenRectInputSignalSource = NULL;
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
	{
		St_TempScreenRowRect = (Stp_ScreenRectOutput->St_ScreenRowRect[i]);
		if(St_TempScreenRowRect == NULL)
			return;
		for(j = 0; j < St_TempScreenRowRect->ActiveRectWindowsNum; j++)
		{
			Stp_OutputRectWindow = (St_TempScreenRowRect->Stp_RectWindows[j]);
			if(Stp_OutputRectWindow == NULL)
				break;
			Stp_OutputRectSignalSource = Stp_OutputRectWindow->Stp_SignalSource;
			for(k = 0; k < Stp_ScreenRectInput->ActiveRectWindowsNum; k++)
			{
				Stp_ScreenRectInputWindow = (Stp_ScreenRectInput->Stp_RectWindows[k]);
				Stp_ScreenRectInputSignalSource = Stp_ScreenRectInputWindow->Stp_SignalSource;
				if((Stp_ScreenRectInputSignalSource->Windows_Layer == Stp_OutputRectSignalSource->Windows_Layer)
					&& (Stp_ScreenRectInputSignalSource->SignalSource == Stp_OutputRectSignalSource->SignalSource)
					&& (Stp_ScreenRectInputSignalSource->WindowsProperty == Stp_OutputRectSignalSource->WindowsProperty)
					&& (Stp_ScreenRectInputSignalSource->SignalSourceType == Stp_OutputRectSignalSource->SignalSourceType))
				{
					if(Stp_ScreenRectInputWindow->SignalDisplayWindow.right == Stp_OutputRectWindow->SignalDisplayWindow.right)
						Stp_OutputRectWindow->SignalInputWindow.right = Stp_ScreenRectInputWindow->SignalInputWindow.right;
					if(Stp_ScreenRectInputWindow->SignalDisplayWindow.bottom == Stp_OutputRectWindow->SignalDisplayWindow.bottom)
						Stp_OutputRectWindow->SignalInputWindow.bottom = Stp_ScreenRectInputWindow->SignalInputWindow.bottom;
				}
			}
		}
	}
}

static ST_Screen_Rect_Output* CombineSingleScreenYWindows(ST_Screen_Rect_Output* Stp_ScreenRectOutput)
{
	int i = 0, j = 0, CombineOK = 0;
	ST_SignalWindows*	Stp_RectWindows = NULL;
	ST_SignalWindows*	Stp_RectWindowsIndex = NULL;
	ST_Screen_Rect *Stp_TempScreenRectOutputRow = NULL, *Stp_TempScreenRectOutputRowIndex = NULL;
	i = Stp_ScreenRectOutput->ActiveRectWindowsRow  - 1;
	while(i > 0)
	{
		Stp_TempScreenRectOutputRow = Stp_ScreenRectOutput->St_ScreenRowRect[i - 1];
		Stp_TempScreenRectOutputRowIndex = Stp_ScreenRectOutput->St_ScreenRowRect[i];
		// 比较两行矩形块是否需要合并
		CombineOK = 0;
		if(Stp_TempScreenRectOutputRow->ActiveRectWindowsNum == Stp_TempScreenRectOutputRowIndex->ActiveRectWindowsNum)
		{
			for(j = 0; j < Stp_TempScreenRectOutputRow->ActiveRectWindowsNum; j++)
			{
				Stp_RectWindows = Stp_TempScreenRectOutputRow->Stp_RectWindows[j];
				Stp_RectWindowsIndex = Stp_TempScreenRectOutputRowIndex->Stp_RectWindows[j];
				if(Stp_RectWindows && Stp_RectWindowsIndex
						&& (Stp_RectWindowsIndex->Stp_SignalSource->Windows_Layer == Stp_RectWindows->Stp_SignalSource->Windows_Layer)
						&& (Stp_RectWindowsIndex->Stp_SignalSource->SignalSource == Stp_RectWindows->Stp_SignalSource->SignalSource)
						&& (Stp_RectWindowsIndex->Stp_SignalSource->WindowsProperty == Stp_RectWindows->Stp_SignalSource->WindowsProperty)
						&& (Stp_RectWindowsIndex->Stp_SignalSource->SignalSourceType == Stp_RectWindows->Stp_SignalSource->SignalSourceType)
						&& (Stp_RectWindowsIndex->SignalDisplayWindow.left == Stp_RectWindows->SignalDisplayWindow.left)
						&& (Stp_RectWindowsIndex->SignalDisplayWindow.right == Stp_RectWindows->SignalDisplayWindow.right)
						&& (Stp_RectWindowsIndex->SignalDisplayWindow.top == Stp_RectWindows->SignalDisplayWindow.bottom)
						&& (Stp_RectWindowsIndex->SignalInputWindow.top == Stp_RectWindows->SignalInputWindow.bottom))
					CombineOK = 1;
				else
				{
					CombineOK = 0;
					break;
				}
			}
			if(CombineOK == 1)
			{
				for(j = 0; j < Stp_TempScreenRectOutputRow->ActiveRectWindowsNum; j++)
				{
					Stp_RectWindows = Stp_TempScreenRectOutputRow->Stp_RectWindows[j];
					Stp_RectWindowsIndex = Stp_TempScreenRectOutputRowIndex->Stp_RectWindows[j];
					Stp_RectWindows->SignalDisplayWindow.bottom = Stp_RectWindowsIndex->SignalDisplayWindow.bottom;
					Stp_RectWindows->SignalInputWindow.bottom = Stp_RectWindowsIndex->SignalInputWindow.bottom;
				}
				for(j = i; j < (Stp_ScreenRectOutput->ActiveRectWindowsRow - 1); j++)
					Stp_ScreenRectOutput->St_ScreenRowRect[j] = Stp_ScreenRectOutput->St_ScreenRowRect[j + 1];
				// 将后面的矩形窗口前移，覆盖合并的窗口
				Stp_ScreenRectOutput->ActiveRectWindowsRow--;
				Stp_ScreenRectOutput->ActiveRectWindowsTotalNum -= Stp_TempScreenRectOutputRow->ActiveRectWindowsNum;
				i = Stp_ScreenRectOutput->ActiveRectWindowsRow  - 1;
			}
			else
				i--;
		}
		else
			i--;
	}
	return Stp_ScreenRectOutput;
}

static void PrintScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput)
{
	int i = 0, j = 0;
	ST_Screen_Rect *St_TempScreenRowRect = NULL;
	ST_SignalWindows *Stp_OutputRectWindow = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	if(Stp_ScreenRectOutput == NULL)
		return;
	if(Stp_ScreenRectOutput->ActiveRectWindowsTotalNum == 0)
		return;

	printf("Output Screen Rect Total Num: %d, Total Row: %d\r\n", Stp_ScreenRectOutput->ActiveRectWindowsTotalNum, Stp_ScreenRectOutput->ActiveRectWindowsRow);
	printf("Display Window Rect( Top, Bottom, Left, Right ), Input Window Rect( Top, Bottom, Left, Right ), Layer:\r\n");
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
	{
		St_TempScreenRowRect = (Stp_ScreenRectOutput->St_ScreenRowRect[i]);
		for(j = 0; j < St_TempScreenRowRect->ActiveRectWindowsNum; j++)
		{
			Stp_OutputRectWindow = (St_TempScreenRowRect->Stp_RectWindows[j]);
			if(Stp_OutputRectWindow == NULL)
				continue;
			Stp_TempSignalSource = Stp_OutputRectWindow->Stp_SignalSource;
			printf("%6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d,\r\n", Stp_OutputRectWindow->SignalDisplayWindow.top,
											Stp_OutputRectWindow->SignalDisplayWindow.bottom,
											Stp_OutputRectWindow->SignalDisplayWindow.left,
											Stp_OutputRectWindow->SignalDisplayWindow.right,
											Stp_OutputRectWindow->SignalInputWindow.top,
											Stp_OutputRectWindow->SignalInputWindow.bottom,
											Stp_OutputRectWindow->SignalInputWindow.left,
											Stp_OutputRectWindow->SignalInputWindow.right,
											Stp_TempSignalSource->Windows_Layer);
		}
	}
}

static void SetScreenRectOutput(BYTE B_SlotIndex, BYTE B_OutputPortIndex, ST_Screen_Rect_Output* Stp_ScreenRectOutput)
{
	int i = 0, j = 0, TempData = 0;
	int WindowXIndex = 0, WindowYIndex = 0;
	ST_SignalWindows St_TempSignalWin;
	ST_SignalSource St_TempSignalSource;
	ST_Screen_Rect *St_TempScreenRowRect = NULL;
	ST_SignalWindows *Stp_OutputRectWindow = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	if(Stp_ScreenRectOutput == NULL)
		return;
	if(Stp_ScreenRectOutput->ActiveRectWindowsTotalNum == 0)
		return;
	WindowYIndex = 0;
	WindowXIndex = 0;
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
	{
		St_TempScreenRowRect = (Stp_ScreenRectOutput->St_ScreenRowRect[i]);
		if(St_TempScreenRowRect == NULL)
			continue;
		WindowYIndex = i;
		for(j = 0; j < St_TempScreenRowRect->ActiveRectWindowsNum; j++)
		{
			Stp_OutputRectWindow = (St_TempScreenRowRect->Stp_RectWindows[j]);
			if(Stp_OutputRectWindow == NULL)
				continue;
			Stp_TempSignalSource = Stp_OutputRectWindow->Stp_SignalSource;
			memcpy(&St_TempSignalWin, Stp_OutputRectWindow, sizeof(St_TempSignalWin));
			memcpy(&St_TempSignalSource, Stp_TempSignalSource, sizeof(St_TempSignalSource));
			St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
			if(St_TempSignalWin.Stp_SignalSource->WindowsProperty & BIT0)
				Stp_TempSignalSource->Windows_Layer = 0;
			if((i == 0) && (j == 0))
			{
				if(B_SlotIndex > 3)
				{
//					OutputFPGASetVideoOutputDisalble(BIT0 << B_OutputPortIndex);
//					OutputFPGA_SoftReset(BIT0 << (B_OutputPortIndex + 4));
				}
				else
				{
//					InputFPGASetVideoOutputDisalble(BIT0 << B_OutputPortIndex);
					InputFPGA_SoftReset(BIT0 << (B_OutputPortIndex + 4));
				}
			}
			if(Stp_TempSignalSource->SignalSource < 4)
				TempData = Stp_TempSignalSource->SignalSource << 1;
			else if((Stp_TempSignalSource->SignalSource >= 4) && (Stp_TempSignalSource->SignalSource < 8))
				TempData = ((Stp_TempSignalSource->SignalSource - 4) << 1) + 1;
			else
				TempData = Stp_TempSignalSource->SignalSource;
			if(Stp_TempSignalSource->Windows_Layer > 0)
				St_TempSignalSource.SignalSource = TempData;
			else
				St_TempSignalSource.SignalSource = 0xFF;
			if(St_TempSignalWin.Stp_SignalSource->WindowsProperty & BIT1)
				St_TempSignalSource.SignalSource = 0x20;
			if(B_SlotIndex > 3)
			{
//				TempData = OutputGraphicFPGAGetSlotBoardBmgBaseType(B_SlotIndex);
//				OutputFPGASetBackgroundWindow(B_OutputPortIndex, WindowXIndex, WindowYIndex, TempData, St_TempSignalWin);
			}
			else
			{
				TempData = InputGraphicFPGAGetSlotBoardBmgBaseType(B_SlotIndex);
				InputFPGASetBackgroundWindow(B_OutputPortIndex, WindowXIndex, WindowYIndex, TempData, St_TempSignalWin);
			}
			if(Stp_TempSignalSource->Windows_Layer > 0)
				WindowXIndex++;
		}
		WindowXIndex = 0;
	}
	#if 0
	if(B_SlotIndex <= 3)
	{
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x100);
		#if 0
		for(i = 0; i < 6; i++)
		{
			TempData = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
			if(TempData == 0x100)
				break;
			Delay1ms(5);
		}
		#endif
//		InputFPGA_ScalerDDRReadSwitch(0, 1);
		InputFPGA_ScalerSetUpdata(0);
//		InputFPGA_ScalerDDRReadSwitch(1, 1);
		InputFPGA_ScalerSetUpdata(1);
//		InputFPGA_ScalerDDRReadSwitch(2, 1);
		InputFPGA_ScalerSetUpdata(2);
//		InputFPGA_ScalerDDRReadSwitch(3, 1);
		InputFPGA_ScalerSetUpdata(3);
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x10F);
	}
	#endif
}

static int ScreenWindowLayoutRequestOneScaler(BYTE B_SlotIndex, ST_SignalWindows* Stp_SignalWindows)
{
	int i = 0, j = 0, CombineFlag = 0;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_SignalSource *Stp_ScalerSignalSource = NULL;
	long long TempllData = 0;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			if(Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed >= MaxOneInputBoardScalerNum)
				return 0xFF;
			j = ScreenWindowLayoutFindScalerIndex(B_SlotIndex, Stp_SignalWindows);
			if(j == 0xFF)
			{
				Stp_TempDisplayScaler = &(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed]);
				Stp_ScalerSignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed]);
				memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
				memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
				memcpy(Stp_ScalerSignalSource, Stp_SignalWindows->Stp_SignalSource, sizeof(ST_SignalSource));
				Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed++;
				return (Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed - 1);
			}
			else
			{
				Stp_TempDisplayScaler = &(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[j]);
				Stp_ScalerSignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[j]);
				CombineFlag = 0;
				if(Stp_TempDisplayScaler->SignalInputWindow.right <= Stp_SignalWindows->SignalInputWindow.left)
				{
					TempllData = Stp_SignalWindows->SignalInputWindow.right - Stp_TempDisplayScaler->SignalInputWindow.left;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->HozScale;
					if((TempllData < MAX_SCREEN_WIDTH) && (TempllData > 0))
						CombineFlag = 1;
					else
						CombineFlag = 0;
				}
				if(Stp_TempDisplayScaler->SignalInputWindow.left >= Stp_SignalWindows->SignalInputWindow.right)
				{
					TempllData = Stp_TempDisplayScaler->SignalInputWindow.right - Stp_SignalWindows->SignalInputWindow.left;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->HozScale;
					if((TempllData < MAX_SCREEN_WIDTH) && (TempllData > 0))
						CombineFlag = 1;
					else
						CombineFlag = 0;
				}
				if(CombineFlag == 1)
				{
					if(Stp_TempDisplayScaler->SignalInputWindow.left > Stp_SignalWindows->SignalInputWindow.left)
						Stp_TempDisplayScaler->SignalInputWindow.left = Stp_SignalWindows->SignalInputWindow.left;
					if(Stp_TempDisplayScaler->SignalInputWindow.right < Stp_SignalWindows->SignalInputWindow.right)
						Stp_TempDisplayScaler->SignalInputWindow.right = Stp_SignalWindows->SignalInputWindow.right;
				}
				CombineFlag = 0;
				if(Stp_TempDisplayScaler->SignalInputWindow.bottom <= Stp_SignalWindows->SignalInputWindow.top)
				{
					TempllData = Stp_SignalWindows->SignalInputWindow.bottom - Stp_TempDisplayScaler->SignalInputWindow.top;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->VertScale;
					if((TempllData < MAX_SCREEN_HEIGHT) && (TempllData > 0))
						CombineFlag = 1;
					else
						CombineFlag = 0;
				}
				if(Stp_TempDisplayScaler->SignalInputWindow.top >= Stp_SignalWindows->SignalInputWindow.bottom)
				{
					TempllData = Stp_TempDisplayScaler->SignalInputWindow.bottom - Stp_SignalWindows->SignalInputWindow.top;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->VertScale;
					if((TempllData < MAX_SCREEN_HEIGHT) && (TempllData > 0))
						CombineFlag = 1;
					else
						CombineFlag = 0;
				}
				if(CombineFlag == 1)
				{
					if(Stp_TempDisplayScaler->SignalInputWindow.top > Stp_SignalWindows->SignalInputWindow.top)
						Stp_TempDisplayScaler->SignalInputWindow.top = Stp_SignalWindows->SignalInputWindow.top;
					if(Stp_TempDisplayScaler->SignalInputWindow.bottom < Stp_SignalWindows->SignalInputWindow.bottom)
						Stp_TempDisplayScaler->SignalInputWindow.bottom = Stp_SignalWindows->SignalInputWindow.bottom;
					TempllData = Stp_TempDisplayScaler->SignalInputWindow.right - Stp_TempDisplayScaler->SignalInputWindow.left;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->HozScale;
					Stp_TempDisplayScaler->SignalDisplayWindow.right = (long) TempllData;
					Stp_TempDisplayScaler->SignalDisplayWindow.left = 0;
					TempllData = Stp_TempDisplayScaler->SignalInputWindow.bottom - Stp_TempDisplayScaler->SignalInputWindow.top;
					TempllData *= SCALE_COEFFICIENT;
					TempllData /= Stp_ScalerSignalSource->VertScale;
					Stp_TempDisplayScaler->SignalDisplayWindow.bottom = (long) TempllData;
					Stp_TempDisplayScaler->SignalDisplayWindow.top = 0;
				}
				if( (Stp_SignalWindows->SignalInputWindow.top >= Stp_TempDisplayScaler->SignalInputWindow.top)
						&& (Stp_SignalWindows->SignalInputWindow.left >= Stp_TempDisplayScaler->SignalInputWindow.left)
						&& (Stp_SignalWindows->SignalInputWindow.right <= Stp_TempDisplayScaler->SignalInputWindow.right)
						&& (Stp_SignalWindows->SignalInputWindow.bottom <= Stp_TempDisplayScaler->SignalInputWindow.bottom)
					)
					return j;
				else
				{
					Stp_TempDisplayScaler = &(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed]);
					Stp_ScalerSignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed]);
					memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
					memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
					memcpy(Stp_ScalerSignalSource, Stp_SignalWindows->Stp_SignalSource, sizeof(ST_SignalSource));
					Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed++;
					return (Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed - 1);
				}
			}
			
		}
	}
	return 0xFF;
}

static int ScreenWindowLayoutFindScalerIndex(BYTE B_SlotIndex, ST_SignalWindows* Stp_SignalWindows)
{
	int i = 0, j = 0;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
//	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_SignalSource *Stp_ScalerSignalSource = NULL;
	if(Stp_SignalWindows->Stp_SignalSource->SignalSource > 3)
		return 0xFF;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			if(Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed <= 0)
				return 0xFF;
			for(j = 0; j < Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed; j++)
			{
//				Stp_TempDisplayScaler = &(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[i]);
				Stp_ScalerSignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[i]);
				if((Stp_ScalerSignalSource->SignalSource == Stp_SignalWindows->Stp_SignalSource->SignalSource)
					&& (Stp_ScalerSignalSource->HozScale == Stp_SignalWindows->Stp_SignalSource->HozScale)
					&& (Stp_ScalerSignalSource->VertScale == Stp_SignalWindows->Stp_SignalSource->VertScale)
					)
					return j;
			}
		}
	}
	return 0xFF;
}

static int CheckSignalInScreenWindowLayout(unsigned char B_SignalSourceSlot, unsigned char B_SignalSource)
{
	int i = 0;
	ST_SignalWindows *Stp_TempSignalWindows;
	ST_SignalSource *Stp_TempSignalSource;
//	ST_ScalerType *Stp_TempScaler = NULL;
	if(gStp_ScreenRectInput == NULL)
		return -1;
	for(i = 0; i < gStp_ScreenRectInput->ActiveRectWindowsNum; i++)
	{
		Stp_TempSignalWindows = gStp_ScreenRectInput->Stp_RectWindows[i];
		if(Stp_TempSignalWindows == NULL)
			return -1;
		Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
		if(Stp_TempSignalSource == NULL)
			return -1;
//		printf("Stp_TempSignalSource: SignalSourceSlot = %d, SignalSource = %d\r\n", Stp_TempSignalSource->SignalSourceSlot, Stp_TempSignalSource->SignalSource);
//		if((Stp_TempSignalSource->SignalSource == B_SignalSource) && (Stp_TempSignalSource->SignalSourceSlot == B_SignalSourceSlot))
		if(Stp_TempSignalSource->SignalSourceSlot == B_SignalSourceSlot)
			return 1;
	}
	/*
	for(i = 0; i < gStp_ProcessorScaler->B_ActiveScalerNum; i++)
	{
		Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
		if(Stp_TempScaler == NULL)
			return -1;
		if((Stp_TempScaler->B_SignalSource == B_SignalSource) && (Stp_TempScaler->B_SignalSourceSlot == B_SignalSourceSlot))
			return 2;
	}
	*/
	return 0;
}

static int CheckSignalInScalerInput(unsigned char B_SignalSourceSlot, unsigned char B_SignalSource)
{
	int i = 0;
	ST_ScalerType *Stp_TempScaler = NULL;
	for(i = 0; i < gStp_ProcessorScaler->B_ActiveScalerNum; i++)
	{
		Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
		if(Stp_TempScaler == NULL)
			return -1;
		if((Stp_TempScaler->B_SignalSource == B_SignalSource) && (Stp_TempScaler->B_SignalSourceSlot == B_SignalSourceSlot))
			return i;
	}
	return -1;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    ScreenWindowLayout_DummyCall(void)                            */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*							being called.												*/
/*																			*/
/****************************************************************************/
void ScreenWindowLayout_DummyCall(void)
{
	ST_Screen_Rect_Init(NULL);
	ScreenWindowLayoutVariableInit(0);
	ScreenWindowLayoutCheckScreenRectInput(NULL);
	CombineSingleScreenYWindows(NULL);
	CheckScreenRectOutput(NULL, NULL);
	PrintScreenRectOutput(NULL);
	SetScreenRectOutput(0, 0, NULL);
	ScreenWindowLayoutRequestOneScaler(0, NULL);
	CheckSignalInScreenWindowLayout(0, 0);
	CheckSignalInScalerInput(0, 0);
}
/********************************  END  *************************************/
