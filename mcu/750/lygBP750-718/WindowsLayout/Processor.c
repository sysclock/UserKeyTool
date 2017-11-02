/*************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
//============================================================================*/
//
// MODULE:      Processor.c
//
// USAGE:       This module alloc processor scaler
//              and init Processor.
//
// DESIGNER:	HAN LONG
/************************************************************************************************************/

/***************************************************************************/
/*	I N C L U D E    F I L E S												*/
/***************************************************************************/
#include <string.h>
#include <stdio.h>
#include "DisplayWallLayout.h"
#include "Processor.h"
#include "SignalWindowsCrop.h"

#ifndef PC_VC_DEBUG
	#include "../System Code/lpc17xx.h"
	#include "../System Code/type.h"
	#include "../System Code/global.h"
	//#include "../System Code/Timer.h"
	#include "../Device/SerialToParallel/STP_74HC595.h"
	#include "../Device/IIC_BusSwitch/PCA9546A.h"
	#include "../GraphicProcessor Board/InputBoardSiI9233.h"
	#include "../GraphicProcessor Board/InputBoardTVP5158.h"
	#include "../GraphicProcessor Board/InputBoardGraphicFPGA.h"
	#include "../Device/InputFPGA/InputFPGA.h"
	#include "../GraphicProcessor Board/OutputBoardGraphicFPGA.h"
	#include "../Device/OutputFPGA/OutputFPGA.h"
	#include "../Device/M21121/M21121.h"
	#include "../GraphicProcessor Board/SwitchBoard.h"
	#include "../GraphicProcessor Board/param.h"
	#include "../TCPIP/TcpIpServerTask.h"
#endif

/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/********************************************************************************/
ST_Processor gSt_Processor;
ST_Processor *gStp_Processor = &(gSt_Processor);

/********************************************************************************/
/*	S T A T I C    V A R I A B L E S										*/
/********************************************************************************/
//static 	long Pixel_X[MAX_DIVISIVE_RECT_WINDOWS_NUM] = {0};
//static long Pixel_Y[MAX_DIVISIVE_RECT_WINDOWS_NUM] = {0};

/****************************************************************************/
/*	G L O B A L    R O U T I N E S									 */
/****************************************************************************/
void InitProcessor(void);
void ProcessorTask(void);
int AllocDisplayWindowScaler(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout, ST_SignalWindows*	Stp_SignalWindows);
void SetProcessorScaler(void);
void ClearProcessorScaler(void);
void PrintProcessorScalerMessage(void);
void PrintProcessorGroundMessage(void);
int FindBoardIndexBySignal(int SourceType, int SignalSource);
int ChangeSignalIndexToBoardSignalIndex(int SourceType, int SignalSource);
int FindDisplayScreen(ST_Rect *Stp_Rect);
int FindOutputBoardGroundInputByInputSignal(int InputBoardIndex, int InputIndex, int OutputBoardIndex, int GroundType);
ST_DisplayScaler* FindScalerByDisplayRect(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_InputRect, ST_Rect *Stp_OutputRect);
int SetProcessorDisplayScreen(int DisplayScreenIndex, int OutputPortIndex, int ScreenPosX, int ScreenPosY);
void SaveProcessorSet(void);
void LoadProcessorSet(void);
/****************************************************************************/
/*	S T A T I C    R O U T I N E S									 */
/****************************************************************************/
static int CheckRectWindowCover(ST_SignalWindows*	Stp_SignalWindows, DisplayWallWindowLayoutStruct *Stp_DisplayWallWindowLayout);
static int RequestOneScaler(ST_SignalWindows*	Stp_SignalWindows, int ScreenIndex);
static int CountBoardScalerUsed(int BoardIndex);
static int FindGTPIndexBySwitchOutputPort(int SwitchOutputPort);
static int FindOutputFPGAIndexBySwitchOutputPort(int SwitchOutputPort);
static int FindBoardScalerUsedBySignalWindows(int BoardIndex, ST_SignalWindows*	Stp_SignalWindows);
static DisplayWallWindowStruct* FindDisplayWallWindowBySignalWindows(ST_SignalWindows*	Stp_SignalWindows, DisplayWallWindowLayoutStruct *Stp_DisplayWallWindowLayout);
static int CountOutputBoardGroundUsed(int BoardIndex, int GroundType);
static int CheckSignalInDisplayWallWindowLayout(unsigned char B_SignalSource, unsigned char B_SignalSourceType);
/*****************************************************************************/
/*	C O D E																										*/
/*****************************************************************************/
void InitProcessor(void)
{
	int i = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
#ifndef PC_VC_DEBUG
	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	ST_InputSignal *Stp_TempInputSignal = NULL;
#endif
	gStp_Processor = &(gSt_Processor);
	memset(gStp_Processor, 0, sizeof(ST_Processor));
	printf("Sizeof gSt_Processor = %d\r\n", sizeof(ST_Processor));
	gStp_ProcessorInputSignal = &(gSt_ProcessorInputSignal);
	memset(gStp_ProcessorInputSignal, 0, sizeof(gSt_ProcessorInputSignal));
	printf("Sizeof gStp_ProcessorInputSignal = %d\r\n", sizeof(gSt_ProcessorInputSignal));
	gStp_Processor->B_ProcessorRow = 2;
	gStp_Processor->B_ProcessorLine = 2;
	gStp_Processor->ScreenWidth = 1920;
	gStp_Processor->ScreenHeight = 1080;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[0]);
	Stp_TempDisplayScreen->B_OutputPort = 0;
	Stp_TempDisplayScreen->B_FPGAIndex = 0;
	Stp_TempDisplayScreen->B_ScreenPosX = 0;
	Stp_TempDisplayScreen->B_ScreenPosY = 0;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[1]);
	Stp_TempDisplayScreen->B_OutputPort = 1;
	Stp_TempDisplayScreen->B_FPGAIndex = 0;
	Stp_TempDisplayScreen->B_ScreenPosX = 1;
	Stp_TempDisplayScreen->B_ScreenPosY = 0;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[2]);
	Stp_TempDisplayScreen->B_OutputPort = 2;
	Stp_TempDisplayScreen->B_FPGAIndex = 0;
	Stp_TempDisplayScreen->B_ScreenPosX = 0;
	Stp_TempDisplayScreen->B_ScreenPosY = 1;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[3]);
	Stp_TempDisplayScreen->B_OutputPort = 3;
	Stp_TempDisplayScreen->B_FPGAIndex = 0;
	Stp_TempDisplayScreen->B_ScreenPosX = 1;
	Stp_TempDisplayScreen->B_ScreenPosY = 1;
	/*
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[2]);
	Stp_TempDisplayScreen->B_OutputPort = 0;
	Stp_TempDisplayScreen->B_FPGAIndex = 1;
	Stp_TempDisplayScreen->B_ScreenPosX = 0;
	Stp_TempDisplayScreen->B_ScreenPosY = 1;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[3]);
	Stp_TempDisplayScreen->B_OutputPort = 1;
	Stp_TempDisplayScreen->B_FPGAIndex = 1;
	Stp_TempDisplayScreen->B_ScreenPosX = 1;
	Stp_TempDisplayScreen->B_ScreenPosY = 1;
	*/
	
#ifndef PC_VC_DEBUG
	gStp_Processor->B_ScalerTotalNum = 0;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_TVP5185)
			continue;
		else
			gStp_Processor->B_ScalerTotalNum += 4;
	}
	
	gStp_Processor->B_ScalerTotalNum += gSt_OutputBoarGraphicFPGA.B_ActiveChip * 4;
	gStp_Processor->B_DisplayScreenTotalNum = gSt_OutputBoarGraphicFPGA.B_ActiveChip * 4;
	gStp_Processor->B_ForegroundTotalNum = gSt_OutputBoarGraphicFPGA.B_ActiveChip * 4;
	gStp_Processor->B_ForegroundUsed = 0;
	gStp_Processor->B_BackgroundTotalNum = gSt_OutputBoarGraphicFPGA.B_ActiveChip * 4;
	gStp_Processor->B_BackgroundUsed = 0;
	gStp_ProcessorInputSignal->B_ActiveInputSignalNum = 0;
	for(i = 0; i < gSt_InputBoardSiI9233.B_ActiveChip; i++)
	{
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[gStp_ProcessorInputSignal->B_ActiveInputSignalNum]);
		Stp_TempInputSignal->SignalSource = i;
		Stp_TempInputSignal->SignalSourceType = SIGNAL_SOURCE_HDMI;
		Stp_TempInputSignal->W_StdWidth = 1024;
		Stp_TempInputSignal->W_StdHeight = 768;
		gStp_ProcessorInputSignal->B_ActiveInputSignalNum++;
	}
	gStp_ProcessorInputSignal->B_ActiveInputCVBSSignalNum = gSt_InputBoarTVP5158.B_ActiveChip * 4;
	for(i = 0; i < gStp_ProcessorInputSignal->B_ActiveInputCVBSSignalNum; i++)
		gStp_ProcessorInputSignal->B_InputCVBSSignalSource[i] = CVBS_NO_SIGNAL;
#else
	gStp_Processor->B_ScalerTotalNum = 8;
	gStp_Processor->B_DisplayScreenTotalNum = 8;
	gStp_Processor->B_ForegroundTotalNum = 8;
	gStp_Processor->B_ForegroundUsed = 0;
	gStp_Processor->B_BackgroundTotalNum = 8;
	gStp_Processor->B_BackgroundUsed = 0;
#endif
	for(i = 0; i < MaxScalerNum; i++)
	{
		gStp_Processor->St_DisplayScaler[i].B_BoardIndex = 0xFF;
		gStp_Processor->St_DisplayScaler[i].B_ScalerIndex = 0xFF;
		gStp_Processor->St_DisplayScaler[i].B_ScalerType = 0xFF;
	}
	printf("Init Processor OK.\r\n");
}

void ProcessorTask(void)
{
#ifndef PC_VC_DEBUG
	unsigned char B_SwitchFlag = 0, i = 0, j = 0, k = 0, B_RefreshDisplayWallWindowLayout = 0;
	ST_DEVICE_OUTPUT_GRAPHIC_FPGA *Stp_TempDeviceOutputGraphicFPGA = NULL;
	ST_InputSignal *Stp_TempInputSignal = NULL;
	long TempWidth = 0, TempHeight = 0;
//	const ST_DEVICE_OUTPUT_GRAPHIC_FPGA_INTERFACE* Stp_TempOutputFPGAInterface = NULL;
	for(i = SIG_OUT0; i < SIG_OUT_Max; i++)
		if(gW_SwitchBoardSwitchPort[i] != gW_OldSwitchBoardSwitchPort[i])
		{
			SwitchPortSignal(gW_SwitchBoardSwitchPort[i], i);
			B_SwitchFlag = 1;
		}
	if(B_SwitchFlag == 1)
	{
#ifdef GRAPHIC_PROCESSOR_RD1
		M21121_UpdataRegister(0);
#else
		M21121_UpdataRegister(0);
		M21121_UpdataRegister(1);
#endif
		B_SwitchFlag = 0;
	}
	for(i = SIG_OUT0; i < SIG_OUT_Max; i++)
		if(gW_SwitchBoardSwitchPort[i] != gW_OldSwitchBoardSwitchPort[i])
		{
			j = FindOutputFPGAIndexBySwitchOutputPort(i);
			if(j != ERR)
			{
				Stp_TempDeviceOutputGraphicFPGA = &(gSt_OutputBoarGraphicFPGA.St_DeviceOutputGraphicFPGA[j]);
				SelectDeviceI2CBus(Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex, Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex);
				if(Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex == 0)
				{
					if(Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex < 0x0F)
						PCA9546A_SelectBusPort(BIT0 << Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex);
				}
				j = FindGTPIndexBySwitchOutputPort(i);
				printf("ProcessorTask:GTPReset, BoardIndex = %d, GTPIndex = 0x%04X\r\n",Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex, j);
				if(j != ERR)
					OutputFPGA_GTPReset(j);
			}
			gW_OldSwitchBoardSwitchPort[i] = gW_SwitchBoardSwitchPort[i];
		}
	B_RefreshDisplayWallWindowLayout = 0;
	for(i = 0; i < gStp_ProcessorInputSignal->B_ActiveInputSignalNum; i++)
	{
		Stp_TempInputSignal = &(gStp_ProcessorInputSignal->St_InputSignalSource[i]);
		switch(Stp_TempInputSignal->SignalSourceType)
		{
			case SIGNAL_SOURCE_HDMI:
				TempWidth = 0;
				TempHeight = 0;
				InputBoardSiI9233GetSignalWidthHeight(Stp_TempInputSignal->SignalSource, &TempWidth, &TempHeight);
				if((Stp_TempInputSignal->W_StdWidth != TempWidth) || (Stp_TempInputSignal->W_StdHeight != TempHeight))
				{
//					printf("RefreshDisplayWallWindowLayout: TempWidth = %d, TempHeight = %d\r\n", TempWidth, TempHeight);
					j = CheckSignalInDisplayWallWindowLayout(Stp_TempInputSignal->SignalSource, Stp_TempInputSignal->SignalSourceType);
					if((j == 1) && (TempWidth != 0) && (TempHeight != 0))
						B_RefreshDisplayWallWindowLayout = 1;
					Stp_TempInputSignal->W_StdWidth = TempWidth;
					Stp_TempInputSignal->W_StdHeight = TempHeight;
				}
				break;
			case SIGNAL_SOURCE_DVI:
				// ADV7604 DVI Input
				break;
			case SIGNAL_SOURCE_VGA:
				//  ADV7604 VGA Input
				break;
			case SIGNAL_SOURCE_CVBS:
				// TVP5158 CVBS Input
				break;
			case SIGNAL_SOURCE_CVBS_2:
				break;
			case SIGNAL_SOURCE_SDI:
				break;
			case SIGNAL_SOURCE_NET:
				break;
			default:
				break;
		}
	}

	for(i = 0; i < gStp_ProcessorInputSignal->B_ActiveInputCVBSSignalNum; i++)
	{
		j = InputBoardTVP5158GetSignalType(i);
		if(j != gStp_ProcessorInputSignal->B_InputCVBSSignalSource[i])
		{
			if(j == CVBS_PAL)
				InputGraphicFPGASetCVBSCapturePos(i, 1);
			if(j == CVBS_NTSC)
				InputGraphicFPGASetCVBSCapturePos(i, 0);

			k = CheckSignalInDisplayWallWindowLayout(i, SIGNAL_SOURCE_CVBS);
			if((k == 1) && (j != CVBS_NO_SIGNAL) && (gStp_ProcessorInputSignal->B_InputCVBSSignalSource[i] == CVBS_NO_SIGNAL))
				B_RefreshDisplayWallWindowLayout = 1;
			k = CheckSignalInDisplayWallWindowLayout(i / 6, SIGNAL_SOURCE_CVBS_2);
			if((k == 1) && (j != CVBS_NO_SIGNAL) && (gStp_ProcessorInputSignal->B_InputCVBSSignalSource[i] == CVBS_NO_SIGNAL))
				B_RefreshDisplayWallWindowLayout = 1;

			gStp_ProcessorInputSignal->B_InputCVBSSignalSource[i] = j;
		}
	}
	if(B_RefreshDisplayWallWindowLayout == 1)
	{
/*
		printf("\r\nRefresh Display Wall Window Layout.\r\n");
		DisplayWallMakeSreenWindows(gStp_DisplayWallWindowLayout);
*/
		B_RefreshDisplayWallWindowLayout = 0;
	}
#endif
}

int AllocDisplayWindowScaler(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout, ST_SignalWindows*	Stp_SignalWindows)
{
/*	
	int Pixel_X_Num = 0;
	int Pixel_Y_Num = 0;
	int ScreenWidth = 0, ScreenHeight = 0, i = 0, j = 0, k = 0, m = 0, TempData = 0;
	long long	tl_ScaleValue = 0;
	ST_Rect *Stp_TempRect = NULL, St_TempRect;
	ST_SignalWindows	St_TempSignalWindows;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	ST_OutputBoardGround St_TempOutputBoardGround;
	Stp_TempRect = &(Stp_SignalWindows->SignalDisplayWindow);
	ScreenWidth = Stp_DisplayWallWindowLayout->DisplayWallWidth / Stp_DisplayWallWindowLayout->DisplayWallLine;
	ScreenHeight = Stp_DisplayWallWindowLayout->DisplayWallHeight / Stp_DisplayWallWindowLayout->DisplayWallRow;
	i = (Stp_TempRect->right / ScreenWidth) - (Stp_TempRect->left / ScreenWidth) + 2;
	if(i > (MAX_DIVISIVE_RECT_WINDOWS_NUM))
	{
		printf("CountDisplayWindowScaler:Pixel_X Memory Over Flow\r\n");
		return ERR;
	}
	i = (Stp_TempRect->bottom / ScreenHeight) - (Stp_TempRect->top / ScreenHeight) + 2;
	if(i > (MAX_DIVISIVE_RECT_WINDOWS_NUM))
	{
		printf("CountDisplayWindowScaler:Pixel_Y Memory Over Flow\r\n");
		return ERR;
	}
	Pixel_X[0] = Stp_TempRect->left;
	i = Stp_TempRect->left / ScreenWidth;
	j = (Stp_TempRect->right / ScreenWidth) - i;
	for(k = 1; k <= j; k++)
		Pixel_X[k] = (i + k) * ScreenWidth;
	Pixel_X[j + 1] = Stp_TempRect->right;
	Pixel_X_Num = BubSort(Pixel_X, j + 2);
	
	Pixel_Y[0] = Stp_TempRect->top;
	i = Stp_TempRect->top / ScreenHeight;
	j = (Stp_TempRect->bottom / ScreenHeight) - i;
	for(k = 1; k <= j; k++)
		Pixel_Y[k] = (i + k) * ScreenHeight;
	Pixel_Y[j + 1] = Stp_TempRect->bottom;
	Pixel_Y_Num = BubSort(Pixel_Y, j + 2);
//	k = (Pixel_X_Num - 1) * (Pixel_Y_Num - 1);
	k = 0;
	for(i = 0; i < Pixel_Y_Num - 1; i++)
		for(j = 0; j < Pixel_X_Num - 1; j++)
		{
			memcpy(&St_TempSignalWindows, Stp_SignalWindows, sizeof(St_TempSignalWindows));
			St_TempSignalWindows.SignalDisplayWindow.top = Pixel_Y[i];
			St_TempSignalWindows.SignalDisplayWindow.bottom = Pixel_Y[i + 1];
			St_TempSignalWindows.SignalDisplayWindow.left = Pixel_X[j];
			St_TempSignalWindows.SignalDisplayWindow.right = Pixel_X[j + 1];
			if(CheckRectWindowCover(&St_TempSignalWindows, Stp_DisplayWallWindowLayout) == 1)
				continue;
			else
			{
				if(Stp_SignalWindows->Stp_SignalSource->HozScale < SCALE_COEFFICIENT)
				{
					St_TempSignalWindows.SignalInputWindow.left = Stp_SignalWindows->SignalInputWindow.left;
					St_TempSignalWindows.SignalInputWindow.right = Stp_SignalWindows->SignalInputWindow.right;
					St_TempSignalWindows.SignalDisplayWindow.left = Stp_SignalWindows->SignalDisplayWindow.left;
					St_TempSignalWindows.SignalDisplayWindow.right = Stp_SignalWindows->SignalDisplayWindow.right;
				}
				else
				{
					if(St_TempSignalWindows.SignalDisplayWindow.left > Stp_SignalWindows->SignalDisplayWindow.left)
					{
						tl_ScaleValue = St_TempSignalWindows.SignalDisplayWindow.left - Stp_SignalWindows->SignalDisplayWindow.left;
						tl_ScaleValue *= SCALE_COEFFICIENT;
						tl_ScaleValue /= Stp_SignalWindows->Stp_SignalSource->HozScale;
						St_TempSignalWindows.SignalInputWindow.left = Stp_SignalWindows->SignalInputWindow.left + ((long) tl_ScaleValue);
					}
					else
						St_TempSignalWindows.SignalInputWindow.left = Stp_SignalWindows->SignalInputWindow.left;
					if(St_TempSignalWindows.SignalDisplayWindow.right < Stp_SignalWindows->SignalDisplayWindow.right)
					{
						tl_ScaleValue = St_TempSignalWindows.SignalDisplayWindow.right - St_TempSignalWindows.SignalDisplayWindow.left;
						tl_ScaleValue *= SCALE_COEFFICIENT;
						tl_ScaleValue /= Stp_SignalWindows->Stp_SignalSource->HozScale;
						St_TempSignalWindows.SignalInputWindow.right = St_TempSignalWindows.SignalInputWindow.left + ((long) tl_ScaleValue);
					}
					else
						St_TempSignalWindows.SignalInputWindow.right = Stp_SignalWindows->SignalInputWindow.right;
				}
				if(Stp_SignalWindows->Stp_SignalSource->VertScale < SCALE_COEFFICIENT)
				{
					St_TempSignalWindows.SignalInputWindow.top = Stp_SignalWindows->SignalInputWindow.top;
					St_TempSignalWindows.SignalInputWindow.bottom = Stp_SignalWindows->SignalInputWindow.bottom;
					St_TempSignalWindows.SignalDisplayWindow.top = Stp_SignalWindows->SignalDisplayWindow.top;
					St_TempSignalWindows.SignalDisplayWindow.bottom = Stp_SignalWindows->SignalDisplayWindow.bottom;
				}
				else
				{
					if(St_TempSignalWindows.SignalDisplayWindow.top > Stp_SignalWindows->SignalDisplayWindow.top)
					{
						tl_ScaleValue = St_TempSignalWindows.SignalDisplayWindow.top - Stp_SignalWindows->SignalDisplayWindow.top;
						tl_ScaleValue *= SCALE_COEFFICIENT;
						tl_ScaleValue /= Stp_SignalWindows->Stp_SignalSource->VertScale;
						St_TempSignalWindows.SignalInputWindow.top = Stp_SignalWindows->SignalInputWindow.top + ((long) tl_ScaleValue);
					}
					else
						St_TempSignalWindows.SignalInputWindow.top = Stp_SignalWindows->SignalInputWindow.top;
					if(St_TempSignalWindows.SignalDisplayWindow.bottom < Stp_SignalWindows->SignalDisplayWindow.bottom)
					{
						tl_ScaleValue =St_TempSignalWindows.SignalDisplayWindow.bottom - St_TempSignalWindows.SignalDisplayWindow.top;
						tl_ScaleValue *= SCALE_COEFFICIENT;
						tl_ScaleValue /= Stp_SignalWindows->Stp_SignalSource->VertScale;
						St_TempSignalWindows.SignalInputWindow.bottom = St_TempSignalWindows.SignalInputWindow.top + ((long) tl_ScaleValue);
					}
					else
						St_TempSignalWindows.SignalInputWindow.bottom = Stp_SignalWindows->SignalInputWindow.bottom;
				}
				St_TempRect.top = Pixel_Y[i];
				St_TempRect.bottom = Pixel_Y[i + 1];
				St_TempRect.left = Pixel_X[j];
				St_TempRect.right = Pixel_X[j + 1];
				m = FindDisplayScreen(&St_TempRect);
				if((Stp_SignalWindows->Stp_SignalSource->HozScale == SCALE_COEFFICIENT) && (Stp_SignalWindows->Stp_SignalSource->VertScale == SCALE_COEFFICIENT))
				{
					if(m < gStp_Processor->B_DisplayScreenTotalNum)
						Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[m]);
					else
						return ERR;
					Stp_TempSignalSource = Stp_SignalWindows->Stp_SignalSource;
					memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
					TempData = FindBoardIndexBySignal(Stp_TempSignalSource->SignalSourceType, Stp_TempSignalSource->SignalSource);
					if(TempData == 0xFF)
						continue;
					St_TempOutputBoardGround.B_InputBoardIndex = TempData;
					TempData = ChangeSignalIndexToBoardSignalIndex(Stp_TempSignalSource->SignalSourceType,Stp_TempSignalSource->SignalSource);
					St_TempOutputBoardGround.B_InputBoardOutputIndex = (TempData << 1) + 1;
					St_TempOutputBoardGround.B_OutputBoardIndex = Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;
					St_TempOutputBoardGround.B_ScalerIndex = 0xFF;
					TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData != ERR)
						continue;
					TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData < 4)
					{
						St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
						memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
						gStp_Processor->B_ForegroundUsed++;
						return 1;
					}
					else
						printf("OutputBoard %d, Foreground ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
					return 2;
				}
				else
				{
					Stp_TempSignalSource = Stp_SignalWindows->Stp_SignalSource;
					TempData = FindBoardIndexBySignal(Stp_TempSignalSource->SignalSourceType, Stp_TempSignalSource->SignalSource);
					if(TempData == 0xFF)
						continue;
					Stp_TempDisplayWallWindow = FindDisplayWallWindowBySignalWindows(&St_TempSignalWindows, Stp_DisplayWallWindowLayout);
					if(Stp_TempDisplayWallWindow)
					{
						TempData = Stp_TempDisplayWallWindow->DisplayWindow.right - Stp_TempDisplayWallWindow->DisplayWindow.left;
						FindSignalSourceActivePos(Stp_TempSignalSource, &St_TempRect);
						if(TempData <= gStp_Processor->ScreenWidth)
						{
							St_TempSignalWindows.SignalInputWindow.left = St_TempRect.left;
							St_TempSignalWindows.SignalInputWindow.right = St_TempRect.right;
							St_TempSignalWindows.SignalDisplayWindow.left = 0;
							St_TempSignalWindows.SignalDisplayWindow.right = TempData;
						}
						TempData = Stp_TempDisplayWallWindow->DisplayWindow.bottom - Stp_TempDisplayWallWindow->DisplayWindow.top;
						if(TempData <= gStp_Processor->ScreenHeight)
						{
							St_TempSignalWindows.SignalInputWindow.top = St_TempRect.top;
							St_TempSignalWindows.SignalInputWindow.bottom = St_TempRect.bottom;
							St_TempSignalWindows.SignalDisplayWindow.top = 0;
							St_TempSignalWindows.SignalDisplayWindow.bottom = TempData;
						}
					}
					if(RequestOneScaler(&St_TempSignalWindows, m) == ERR)
						printf("Request One Scaler Error\r\n");
					else
						k++;
				}
			}
		}
	return k;
*/
	return 0;
}

void SetProcessorScaler(void)
{
#ifndef PC_VC_DEBUG
	int i = 0, j = 0;
	ST_Rect *Stp_TempRect = NULL;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	ST_DEVICE_OUTPUT_GRAPHIC_FPGA *Stp_TempDeviceOutputGraphicFPGA = NULL;
	XScalerAperture St_TempAperture;
	ST_OutputBoardGround *Stp_TempOutputBoardGround = NULL;
	for(i = 0; i < gStp_Processor->B_ScalerUsed; i++)
	{
		Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[i]);
		memset(&St_TempAperture, 0, sizeof(St_TempAperture));
		Stp_TempRect = &(Stp_TempDisplayScaler->SignalInputWindow);
		St_TempAperture.InVertSize = Stp_TempRect->bottom - Stp_TempRect->top;
		St_TempAperture.InHoriSize = Stp_TempRect->right - Stp_TempRect->left;
		St_TempAperture.InFirstLine = Stp_TempRect->top;
		St_TempAperture.InLastLine = Stp_TempRect->bottom - 1;
		St_TempAperture.InFirstPixel = Stp_TempRect->left;
		St_TempAperture.InLastPixel = Stp_TempRect->right - 1;
		Stp_TempRect = &(Stp_TempDisplayScaler->SignalDisplayWindow);
		St_TempAperture.OutVertSize = Stp_TempRect->bottom - Stp_TempRect->top;
		St_TempAperture.OutHoriSize = Stp_TempRect->right - Stp_TempRect->left;
		if(Stp_TempDisplayScaler->B_ScalerType == INPUT_SCALER)
		{
			j = Stp_TempDisplayScaler->B_BoardIndex;
			Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[j]);
			SelectDeviceI2CBus(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex, Stp_TempDeviceInputGraphicFPGA->B_I2CBusIndex);
			if(Stp_TempDeviceInputGraphicFPGA->B_I2CBusIndex == 0)
			{
				if(Stp_TempDeviceInputGraphicFPGA->B_SubI2CBusIndex < 0x0F)
					PCA9546A_SelectBusPort(BIT0 << Stp_TempDeviceInputGraphicFPGA->B_SubI2CBusIndex);
			}
			j = ChangeSignalIndexToBoardSignalIndex(Stp_TempDisplayScaler->Stp_SignalSource->SignalSourceType, Stp_TempDisplayScaler->Stp_SignalSource->SignalSource);
			InputFPGA_ScalerSetup(Stp_TempDisplayScaler->B_ScalerIndex, j << 1, &St_TempAperture);
		}
		else if(Stp_TempDisplayScaler->B_ScalerType == OUTPUT_SCALER)
		{
			if(Stp_TempDisplayScaler->B_GroundType == 0)
				Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[Stp_TempDisplayScaler->B_GroundIndex]);
			else
				Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[Stp_TempDisplayScaler->B_GroundIndex]);
			j = Stp_TempDisplayScaler->B_BoardIndex - INPUT_BOARD_MAX;
			Stp_TempDeviceOutputGraphicFPGA = &(gSt_OutputBoarGraphicFPGA.St_DeviceOutputGraphicFPGA[j]);
			SelectDeviceI2CBus(Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex, Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex);
			if(Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex == 0)
			{
				if(Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex < 0x0F)
					PCA9546A_SelectBusPort(BIT0 << Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex);
			}
//			j = ChangeSignalIndexToBoardSignalIndex(Stp_TempDisplayScaler->Stp_SignalSource->SignalSourceType, Stp_TempDisplayScaler->Stp_SignalSource->SignalSource);
			j = Stp_TempOutputBoardGround->B_OutputBoardInputIndex;
			OutputFPGA_ScalerSetup(Stp_TempDisplayScaler->B_ScalerIndex, j << 1, &St_TempAperture);
		}
	}
#endif
}

void ClearProcessorScaler(void)
{
	gStp_Processor->B_ScalerUsed = 0;
	memset((gStp_Processor->St_DisplayScaler), 0, sizeof(gStp_Processor->St_DisplayScaler));
	gStp_Processor->B_ForegroundUsed = 0;
	memset((gStp_Processor->St_OutputBoardForeground), 0, sizeof(gStp_Processor->St_OutputBoardForeground));
	gStp_Processor->B_BackgroundUsed = 0;
	memset((gStp_Processor->St_OutputBoardBackground), 0, sizeof(gStp_Processor->St_OutputBoardBackground));
}

void PrintProcessorScalerMessage(void)
{
	int i = 0;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	printf("ProcessorScaler Total Used: %d\r\n",gStp_Processor->B_ScalerUsed);
	printf("ScalerIndex, ScalerType, BoardIndex, SignalSource, SourceType, Layer, GroundType, GroundIndex\r\n");
	printf("Display Window Rect( Top, Bottom, Left, Right ), Input Window Rect( Top, Bottom, Left, Right )\r\n");
	for(i = 0; i < gStp_Processor->B_ScalerUsed; i++)
	{
		Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[i]);
		printf("%d, %d, %d, %d, %d, %d, %d, %d\r\n", Stp_TempDisplayScaler->B_ScalerIndex,
			Stp_TempDisplayScaler->B_ScalerType,
			Stp_TempDisplayScaler->B_BoardIndex,
			Stp_TempDisplayScaler->Stp_SignalSource->SignalSource,
			Stp_TempDisplayScaler->Stp_SignalSource->SignalSourceType,
			Stp_TempDisplayScaler->Stp_SignalSource->Windows_Layer,
			Stp_TempDisplayScaler->B_GroundType,
			Stp_TempDisplayScaler->B_GroundIndex);
		printf("%d, %d, %d, %d, %d, %d, %d, %d\r\n", Stp_TempDisplayScaler->SignalDisplayWindow.top,
										Stp_TempDisplayScaler->SignalDisplayWindow.bottom,
										Stp_TempDisplayScaler->SignalDisplayWindow.left,
										Stp_TempDisplayScaler->SignalDisplayWindow.right,
										Stp_TempDisplayScaler->SignalInputWindow.top,
										Stp_TempDisplayScaler->SignalInputWindow.bottom,
										Stp_TempDisplayScaler->SignalInputWindow.left,
										Stp_TempDisplayScaler->SignalInputWindow.right);
	}
	PrintProcessorGroundMessage();
}

void PrintProcessorGroundMessage(void)
{
	int i = 0;
	ST_OutputBoardGround *Stp_TempOutputBoardGround = NULL;
	printf("Foreground Total Used: %d\r\n",gStp_Processor->B_ForegroundUsed);
	printf("ScalerIndex, OutputBoardIndex, OutputBoardInputIndex, InputBoardIndex, InputBoardOutputIndex\r\n");
	for(i = 0; i < gStp_Processor->B_ForegroundUsed; i++)
	{
		Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[i]);
		printf("%d, %d, %d, %d, %d\r\n", Stp_TempOutputBoardGround->B_ScalerIndex,
			Stp_TempOutputBoardGround->B_OutputBoardIndex,
			Stp_TempOutputBoardGround->B_OutputBoardInputIndex,
			Stp_TempOutputBoardGround->B_InputBoardIndex,
			Stp_TempOutputBoardGround->B_InputBoardOutputIndex);
	}
	printf("Background Total Used: %d\r\n",gStp_Processor->B_BackgroundUsed);
	printf("ScalerIndex, OutputBoardIndex, OutputBoardInputIndex, InputBoardIndex, InputBoardOutputIndex\r\n");
	for(i = 0; i < gStp_Processor->B_BackgroundUsed; i++)
	{
		Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[i]);
		printf("%d, %d, %d, %d, %d\r\n", Stp_TempOutputBoardGround->B_ScalerIndex,
			Stp_TempOutputBoardGround->B_OutputBoardIndex,
			Stp_TempOutputBoardGround->B_OutputBoardInputIndex,
			Stp_TempOutputBoardGround->B_InputBoardIndex,
			Stp_TempOutputBoardGround->B_InputBoardOutputIndex);
	}
}

int FindBoardIndexBySignal(int SourceType, int SignalSource)
{
	int i = 0, k = 0;
#ifndef PC_VC_DEBUG
	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
#endif
	switch(SourceType)
	{
		case SIGNAL_SOURCE_HDMI:
		#ifndef PC_VC_DEBUG
			k = 0;
			for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
			{
				Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
				if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_SiI9233)
				{
					k += 4;
					if((SignalSource < k) && (SignalSource >= (k - 4)))
						return i;
				}
			}
			i = 0xFF;
		#else
			i = SignalSource / 4;
		#endif
			break;
		case SIGNAL_SOURCE_DVI:
			// ADV7604 DVI Input
			break;
		case SIGNAL_SOURCE_VGA:
			//  ADV7604 VGA Input
			break;
		case SIGNAL_SOURCE_CVBS:
			// TVP5158 CVBS Input
		#ifndef PC_VC_DEBUG
			k = 0;
			for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
			{
				Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
				if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_TVP5185)
				{
					k += 24;
					if((SignalSource < k) && (SignalSource >= (k - 24)))
						return i;
				}
			}
			i = 0xFF;
		#else
			i = (SignalSource / 24) + 1;
		#endif
			break;
		case SIGNAL_SOURCE_CVBS_2:
		#ifndef PC_VC_DEBUG
			k = 0;
			for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
			{
				Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
				if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_TVP5185)
				{
					k += 4;
					if((SignalSource < k) && (SignalSource >= (k - 4)))
						return i;
				}
			}
			i = 0xFF;
		#else
			i = (SignalSource / 4) + 1;
		#endif
			break;
		case SIGNAL_SOURCE_SDI:
			i = 0xFF;
			break;
		case SIGNAL_SOURCE_NET:
			i = 0xFF;
			break;
		default:
			i = 0xFF;
			break;
	}
	return i;
}

int ChangeSignalIndexToBoardSignalIndex(int SourceType, int SignalSource)
{
	int i = 0;
	switch(SourceType)
	{
		case SIGNAL_SOURCE_HDMI:
			i = SignalSource % 4;
			break;
		case SIGNAL_SOURCE_DVI:
			// ADV7604 DVI Input
			break;
		case SIGNAL_SOURCE_VGA:
			//  ADV7604 VGA Input
			break;
		case SIGNAL_SOURCE_CVBS:
			// TVP5158 CVBS Input
			i = SignalSource % 24;
			i /= 6;
			break;
		case SIGNAL_SOURCE_CVBS_2:
			// TVP5158 CVBS Input
			i = SignalSource % 4;
			break;
		case SIGNAL_SOURCE_SDI:
			break;
		case SIGNAL_SOURCE_NET:
			break;
		default:
			i = 0xFF;
			break;
	}
	return i;
}

int FindDisplayScreen(ST_Rect *Stp_Rect)
{
	int i = 0, j = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	ST_Rect St_TempRect;
	j = gStp_Processor->B_ProcessorRow * gStp_Processor->B_ProcessorLine;
	for(i = 0; i < j; i++)
	{
		Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[i]);
		St_TempRect.left = Stp_TempDisplayScreen->B_ScreenPosX * gStp_Processor->ScreenWidth;
		St_TempRect.top = Stp_TempDisplayScreen->B_ScreenPosY * gStp_Processor->ScreenHeight;
		St_TempRect.right = (Stp_TempDisplayScreen->B_ScreenPosX + 1) * gStp_Processor->ScreenWidth;
		St_TempRect.bottom = (Stp_TempDisplayScreen->B_ScreenPosY + 1) * gStp_Processor->ScreenHeight;
		if(
			(Stp_Rect->right <= St_TempRect.right)
			&&
			(Stp_Rect->left >= St_TempRect.left)
			&&
			(Stp_Rect->bottom <= St_TempRect.bottom)
			&&
			(Stp_Rect->top >= St_TempRect.top)
		  )
			return i;
	}
	return 0xFF;
}

ST_DisplayScaler* FindScalerByDisplayRect(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_InputRect, ST_Rect *Stp_OutputRect)
{
	int i = 0, OutVertSizeSrc = 0, OutHoriSizeSrc = 0, OutVertSizeDst = 0, OutHoriSizeDst = 0;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_Rect *Stp_TempInputRect = NULL, *Stp_TempOutputRect = NULL;
	for(i = 0; i < gStp_Processor->B_ScalerUsed; i++)
	{
		Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[i]);
		Stp_TempSignalSource = Stp_TempDisplayScaler->Stp_SignalSource;
		Stp_TempInputRect = &(Stp_TempDisplayScaler->SignalInputWindow);
		Stp_TempOutputRect = &(Stp_TempDisplayScaler->SignalDisplayWindow);
		OutHoriSizeSrc = Stp_OutputRect->right - Stp_OutputRect->left;
		OutVertSizeSrc = Stp_OutputRect->bottom - Stp_OutputRect->top;
		OutHoriSizeDst = Stp_TempOutputRect->right - Stp_TempOutputRect->left;
		OutVertSizeDst = Stp_TempOutputRect->bottom - Stp_TempOutputRect->top;
		if((Stp_SignalSource->SignalSource == Stp_TempSignalSource->SignalSource)
				&&
				(Stp_SignalSource->SignalSourceType == Stp_TempSignalSource->SignalSourceType)
				&&
				(Stp_SignalSource->HozScale == Stp_TempSignalSource->HozScale)
				&&
				(Stp_SignalSource->VertScale == Stp_TempSignalSource->VertScale)
				&&
//				(Stp_SignalSource->Windows_Layer == Stp_TempSignalSource->Windows_Layer)
//				&&
				(Stp_InputRect->left >= Stp_TempInputRect->left)
				&&
				(Stp_InputRect->right <= Stp_TempInputRect->right)
				&&
				(Stp_InputRect->top >= Stp_TempInputRect->top)
				&&
				(Stp_InputRect->bottom <= Stp_TempInputRect->bottom)
				&&
				(OutHoriSizeSrc <= OutHoriSizeDst)
				&&
				(OutVertSizeSrc <= OutVertSizeDst)
			)
			return Stp_TempDisplayScaler;
	}
	return NULL;
}

int SetProcessorDisplayScreen(int DisplayScreenIndex, int OutputPortIndex, int ScreenPosX, int ScreenPosY)
{
	int i = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	if(DisplayScreenIndex >= MaxProcessorScreen)
		return -1;
	i = gStp_Processor->B_ProcessorRow * gStp_Processor->B_ProcessorLine;
	if(DisplayScreenIndex >= i)
		return -1;
	Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[DisplayScreenIndex]);
	i = gSt_OutputBoarGraphicFPGA.B_ActiveChip * 4;
	if(OutputPortIndex >= i)
		return -1;
	Stp_TempDisplayScreen->B_OutputPort = OutputPortIndex % 4;
	Stp_TempDisplayScreen->B_FPGAIndex = OutputPortIndex / 4;
	Stp_TempDisplayScreen->B_ScreenPosX = ScreenPosX;
	Stp_TempDisplayScreen->B_ScreenPosY = ScreenPosY;
	return 1;
}

void SaveProcessorSet(void)
{
	WORD W_TempData = 0, W_TempData1 = 0;
	int i = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	TCP_SaveIPAddress();
	TCP_SaveTCPLocalPort();
	TCP_SaveGateWayIPAddress();
	ParamWrite(NVR_PROCESSOR_ROW, gStp_Processor->B_ProcessorRow);
	W_TempData = ParamRead(NVR_PROCESSOR_ROW);
	if(W_TempData != gStp_Processor->B_ProcessorRow)
		ParamWrite(NVR_PROCESSOR_ROW, gStp_Processor->B_ProcessorRow);
	ParamWrite(NVR_PROCESSOR_LINE, gStp_Processor->B_ProcessorLine);
	W_TempData = ParamRead(NVR_PROCESSOR_LINE);
	if(W_TempData != gStp_Processor->B_ProcessorLine)
		ParamWrite(NVR_PROCESSOR_LINE, gStp_Processor->B_ProcessorLine);
	ParamWrite(NVR_SCREEN_WIDTH, gStp_Processor->ScreenWidth);
	W_TempData = ParamRead(NVR_SCREEN_WIDTH);
	if(W_TempData != gStp_Processor->ScreenWidth)
		ParamWrite(NVR_SCREEN_WIDTH, gStp_Processor->ScreenWidth);
	ParamWrite(NVR_SCREEN_HEIGHT, gStp_Processor->ScreenHeight);
	W_TempData = ParamRead(NVR_SCREEN_HEIGHT);
	if(W_TempData != gStp_Processor->ScreenHeight)
		ParamWrite(NVR_SCREEN_HEIGHT, gStp_Processor->ScreenHeight);
	printf("SaveProcessorSet: Row = %d, Line = %d, ScreenWidth = %d, ScreenHeight = %d\r\n",
					gStp_Processor->B_ProcessorRow,
					gStp_Processor->B_ProcessorLine,
					gStp_Processor->ScreenWidth,
					gStp_Processor->ScreenHeight);
	W_TempData = NVR_SCREEN0_OUTPUT_PORT;
	for(i = 0; i < gStp_Processor->B_ProcessorRow * gStp_Processor->B_ProcessorLine; i++)
	{
		Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[i]);
		ParamWrite(W_TempData, Stp_TempDisplayScreen->B_OutputPort);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayScreen->B_OutputPort)
			ParamWrite(W_TempData, Stp_TempDisplayScreen->B_OutputPort);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempDisplayScreen->B_FPGAIndex);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayScreen->B_FPGAIndex)
			ParamWrite(W_TempData, Stp_TempDisplayScreen->B_FPGAIndex);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempDisplayScreen->B_ScreenPosX);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayScreen->B_ScreenPosX)
			ParamWrite(W_TempData, Stp_TempDisplayScreen->B_ScreenPosX);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempDisplayScreen->B_ScreenPosY);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayScreen->B_ScreenPosY)
			ParamWrite(W_TempData, Stp_TempDisplayScreen->B_ScreenPosY);
		W_TempData++;
		printf("Screen = %d: OutputPort = %d, FPGAIndex = %d, ScreenPosX = %d, ScreenPosY = %d\r\n",
						i,
						Stp_TempDisplayScreen->B_OutputPort,
						Stp_TempDisplayScreen->B_FPGAIndex,
						Stp_TempDisplayScreen->B_ScreenPosX,
						Stp_TempDisplayScreen->B_ScreenPosY);
	}
}

void LoadProcessorSet(void)
{
	WORD W_TempData = 0;
	int i = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	gStp_Processor->B_ProcessorRow = ParamRead(NVR_PROCESSOR_ROW);
	gStp_Processor->B_ProcessorLine = ParamRead(NVR_PROCESSOR_LINE);
	gStp_Processor->ScreenWidth = ParamRead(NVR_SCREEN_WIDTH);
	gStp_Processor->ScreenHeight = ParamRead(NVR_SCREEN_HEIGHT);
	printf("LoadProcessorSet: Row = %d, Line = %d, ScreenWidth = %d, ScreenHeight = %d\r\n",
					gStp_Processor->B_ProcessorRow,
					gStp_Processor->B_ProcessorLine,
					gStp_Processor->ScreenWidth,
					gStp_Processor->ScreenHeight);
	W_TempData = NVR_SCREEN0_OUTPUT_PORT;
	for(i = 0; i < gStp_Processor->B_ProcessorRow * gStp_Processor->B_ProcessorLine; i++)
	{
		if(i >= MaxProcessorScreen)
			return;
		Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[i]);
		if(i < gStp_Processor->B_DisplayScreenTotalNum)
		{
			Stp_TempDisplayScreen->B_OutputPort = ParamRead(W_TempData);
			W_TempData++;
			Stp_TempDisplayScreen->B_FPGAIndex = ParamRead(W_TempData);
			W_TempData++;
			Stp_TempDisplayScreen->B_ScreenPosX = ParamRead(W_TempData);
			W_TempData++;
			Stp_TempDisplayScreen->B_ScreenPosY = ParamRead(W_TempData);
			W_TempData++;
		}
		else
		{
			Stp_TempDisplayScreen->B_OutputPort = 0xFF;
			Stp_TempDisplayScreen->B_FPGAIndex = 0xFF;
			Stp_TempDisplayScreen->B_ScreenPosX = 0xFF;
			Stp_TempDisplayScreen->B_ScreenPosY = 0xFF;
		}
		printf("Screen = %d: OutputPort = %d, FPGAIndex = %d, ScreenPosX = %d, ScreenPosY = %d\r\n",
						i,
						Stp_TempDisplayScreen->B_OutputPort,
						Stp_TempDisplayScreen->B_FPGAIndex,
						Stp_TempDisplayScreen->B_ScreenPosX,
						Stp_TempDisplayScreen->B_ScreenPosY);
	}
}

static int CountBoardScalerUsed(int BoardIndex)
{
	int i = 0, k = 0;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	for(i = 0; i < gStp_Processor->B_ScalerUsed; i++)
	{
		Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[i]);
		if(Stp_TempDisplayScaler->B_BoardIndex == BoardIndex)
			k++;
	}
	return k;
}

static int FindBoardScalerUsedBySignalWindows(int BoardIndex, ST_SignalWindows*	Stp_SignalWindows)
{
	int i = 0, OutVertSizeSrc = 0, OutHoriSizeSrc = 0, OutVertSizeDst = 0, OutHoriSizeDst = 0;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_SignalSource *Stp_TempSignalSourceSrc = NULL, *Stp_TempSignalSourceDst = NULL;
	ST_Rect *Stp_TempSignalInputWindowSrc = NULL, *Stp_TempSignalInputWindowDst = NULL;
	ST_Rect *Stp_TempSignalDisplayWindowSrc = NULL, *Stp_TempSignalDisplayWindowDst = NULL;
	for(i = 0; i < gStp_Processor->B_ScalerUsed; i++)
	{
		Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[i]);
		Stp_TempSignalSourceSrc = Stp_TempDisplayScaler->Stp_SignalSource;
		Stp_TempSignalInputWindowSrc = &(Stp_TempDisplayScaler->SignalInputWindow);
		Stp_TempSignalDisplayWindowSrc = &(Stp_TempDisplayScaler->SignalDisplayWindow);
		OutHoriSizeSrc = Stp_TempSignalDisplayWindowSrc->right - Stp_TempSignalDisplayWindowSrc->left;
		OutVertSizeSrc = Stp_TempSignalDisplayWindowSrc->bottom - Stp_TempSignalDisplayWindowSrc->top;
		Stp_TempSignalSourceDst = Stp_SignalWindows->Stp_SignalSource;
		Stp_TempSignalInputWindowDst = &(Stp_SignalWindows->SignalInputWindow);
		Stp_TempSignalDisplayWindowDst = &(Stp_SignalWindows->SignalDisplayWindow);
		OutHoriSizeDst = Stp_TempSignalDisplayWindowDst->right - Stp_TempSignalDisplayWindowDst->left;
		OutVertSizeDst = Stp_TempSignalDisplayWindowDst->bottom - Stp_TempSignalDisplayWindowDst->top;
		if((Stp_TempDisplayScaler->B_BoardIndex == BoardIndex)
				&& (Stp_TempSignalSourceSrc->SignalSource == Stp_TempSignalSourceDst->SignalSource)
				&& (Stp_TempSignalSourceSrc->SignalSourceType == Stp_TempSignalSourceDst->SignalSourceType)
				&& (Stp_TempSignalInputWindowSrc->left == Stp_TempSignalInputWindowDst->left)
				&& (Stp_TempSignalInputWindowSrc->top == Stp_TempSignalInputWindowDst->top)
				&& (Stp_TempSignalInputWindowSrc->right == Stp_TempSignalInputWindowDst->right)
				&& (Stp_TempSignalInputWindowSrc->bottom == Stp_TempSignalInputWindowDst->bottom)
				&& (OutHoriSizeSrc == OutHoriSizeDst)
				&& (OutVertSizeSrc == OutVertSizeDst)
			)
			return i;
	}
	return ERR;
}

static int RequestOneScaler(ST_SignalWindows*	Stp_SignalWindows, int ScreenIndex)
{
	int i = 0, j = 0, TempData = 0;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_OutputBoardGround St_TempOutputBoardGround;
	i = ScreenIndex;
	if(i < gStp_Processor->B_DisplayScreenTotalNum)
		Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[i]);
	else
		return ERR;
	Stp_TempSignalSource = Stp_SignalWindows->Stp_SignalSource;
	if((Stp_TempSignalSource->SignalSourceType == SIGNAL_SOURCE_CVBS) || (Stp_TempSignalSource->SignalSourceType == SIGNAL_SOURCE_CVBS_2))
	{
		i = FindBoardScalerUsedBySignalWindows(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX, Stp_SignalWindows);
		if(i != ERR)
			return 1;
		i = CountBoardScalerUsed(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX);
		if(i < MaxScalerOneBoard)
		{
			Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[gStp_Processor->B_ScalerUsed]);
			Stp_TempDisplayScaler->Stp_SignalSource = Stp_SignalWindows->Stp_SignalSource;
			Stp_TempDisplayScaler->B_ScalerIndex = i;
			Stp_TempDisplayScaler->B_ScalerType = OUTPUT_SCALER;
			Stp_TempDisplayScaler->B_BoardIndex = Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;
			memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
			memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));

			memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
			TempData = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
			St_TempOutputBoardGround.B_InputBoardIndex = TempData;
			TempData = ChangeSignalIndexToBoardSignalIndex(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
			St_TempOutputBoardGround.B_InputBoardOutputIndex = (TempData << 1) + 1;
			St_TempOutputBoardGround.B_OutputBoardIndex = Stp_TempDisplayScaler->B_BoardIndex;
			St_TempOutputBoardGround.B_ScalerIndex = gStp_Processor->B_ScalerUsed;
			TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
			if(TempData == ERR)
			{
				TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
				if(TempData < 4)
				{
					St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
					memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
					gStp_Processor->B_ForegroundUsed++;
				}
				else
					printf("OutputBoard %d, Foreground ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
			}

			Stp_TempDisplayScaler->B_GroundType = 0;
			Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
			gStp_Processor->B_ScalerUsed++;

			return 1;
		}
		else
			return ERR;
	}
	else
	{
		i = FindBoardScalerUsedBySignalWindows(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX, Stp_SignalWindows);
		if(i != ERR)
			return 1;
		i = FindBoardScalerUsedBySignalWindows(Stp_TempDisplayScreen->B_FPGAIndex, Stp_SignalWindows);
		if(i != ERR)
			return 1;
		i = CountOutputBoardGroundUsed(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX, 0);
		if(i < 4)
		{
			j = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
			i = CountBoardScalerUsed(j);
			if(i < MaxScalerOneBoard)
			{
				Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[gStp_Processor->B_ScalerUsed]);
				Stp_TempDisplayScaler->Stp_SignalSource = Stp_SignalWindows->Stp_SignalSource;
				Stp_TempDisplayScaler->B_ScalerIndex = i;
				Stp_TempDisplayScaler->B_ScalerType = INPUT_SCALER;
				Stp_TempDisplayScaler->B_BoardIndex = j;
				memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
				TempData = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
				St_TempOutputBoardGround.B_InputBoardIndex = TempData;
				TempData = ChangeSignalIndexToBoardSignalIndex(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
				St_TempOutputBoardGround.B_InputBoardOutputIndex = (Stp_TempDisplayScaler->B_ScalerIndex << 1);
				St_TempOutputBoardGround.B_OutputBoardIndex =Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;
				St_TempOutputBoardGround.B_ScalerIndex = gStp_Processor->B_ScalerUsed;
				TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
				if(TempData == ERR)
				{
					TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData < 4)
					{
						St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
						memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
						gStp_Processor->B_ForegroundUsed++;
						Stp_TempDisplayScaler->B_GroundType = 0;
						Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
					}
					else
					{
						TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 1);
						if(TempData < 4)
						{
							St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
							memcpy(&(gStp_Processor->St_OutputBoardBackground[gStp_Processor->B_BackgroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
							gStp_Processor->B_BackgroundUsed++;
							Stp_TempDisplayScaler->B_GroundType = 1;
							Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
						}
						else
							printf("OutputBoard %d, Background ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
					}
				}

//				Stp_TempDisplayScaler->B_GroundType = 1;
//				Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
				memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
				memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
				gStp_Processor->B_ScalerUsed++;
				return 1;
			}
			else
			{
				i = CountBoardScalerUsed(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX);
				if(i < MaxScalerOneBoard)
				{
					Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[gStp_Processor->B_ScalerUsed]);
					Stp_TempDisplayScaler->Stp_SignalSource = Stp_SignalWindows->Stp_SignalSource;
					Stp_TempDisplayScaler->B_ScalerIndex = i;
					Stp_TempDisplayScaler->B_ScalerType = OUTPUT_SCALER;
					Stp_TempDisplayScaler->B_BoardIndex = Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;

					memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
					TempData = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
					St_TempOutputBoardGround.B_InputBoardIndex = TempData;
					TempData = ChangeSignalIndexToBoardSignalIndex(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
					St_TempOutputBoardGround.B_InputBoardOutputIndex = (TempData << 1) + 1;
					St_TempOutputBoardGround.B_OutputBoardIndex = Stp_TempDisplayScaler->B_BoardIndex;
					St_TempOutputBoardGround.B_ScalerIndex = gStp_Processor->B_ScalerUsed;
					TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData == ERR)
					{
						TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
						if(TempData < 4)
						{
							St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
							memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
							gStp_Processor->B_ForegroundUsed++;
							Stp_TempDisplayScaler->B_GroundType = 0;
							Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
						}
						else
						{
							TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 1);
							if(TempData < 4)
							{
								St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
								memcpy(&(gStp_Processor->St_OutputBoardBackground[gStp_Processor->B_BackgroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
								gStp_Processor->B_BackgroundUsed++;
								Stp_TempDisplayScaler->B_GroundType = 1;
								Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
							}
							else
								printf("OutputBoard %d, Background ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
						}
					}

//					Stp_TempDisplayScaler->B_GroundType = 0;
//					Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
					memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
					memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
					gStp_Processor->B_ScalerUsed++;

					return 1;
				}
			}
			
		}
		else
		{
			i = CountBoardScalerUsed(Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX);
			if(i < MaxScalerOneBoard)
			{
				Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[gStp_Processor->B_ScalerUsed]);
				Stp_TempDisplayScaler->Stp_SignalSource = Stp_SignalWindows->Stp_SignalSource;
				Stp_TempDisplayScaler->B_ScalerIndex = i;
				Stp_TempDisplayScaler->B_ScalerType = OUTPUT_SCALER;
				Stp_TempDisplayScaler->B_BoardIndex = Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;

				memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
				TempData = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
				St_TempOutputBoardGround.B_InputBoardIndex = TempData;
				TempData = ChangeSignalIndexToBoardSignalIndex(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
				St_TempOutputBoardGround.B_InputBoardOutputIndex = (TempData << 1) + 1;
				St_TempOutputBoardGround.B_OutputBoardIndex = Stp_TempDisplayScaler->B_BoardIndex;
				St_TempOutputBoardGround.B_ScalerIndex = gStp_Processor->B_ScalerUsed;
				TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
				if(TempData == ERR)
				{
					TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData < 4)
					{
						St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
						memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
						gStp_Processor->B_ForegroundUsed++;
						Stp_TempDisplayScaler->B_GroundType = 0;
						Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
					}
					else
					{
						TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 1);
						if(TempData < 4)
						{
							St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
							memcpy(&(gStp_Processor->St_OutputBoardBackground[gStp_Processor->B_BackgroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
							gStp_Processor->B_BackgroundUsed++;
							Stp_TempDisplayScaler->B_GroundType = 1;
							Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
						}
						else
							printf("OutputBoard %d, Background ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
					}
				}

//				Stp_TempDisplayScaler->B_GroundType = 0;
//				Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
				memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
				memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
				gStp_Processor->B_ScalerUsed++;

				return 1;
			}
			else
			{
				j = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
				i = CountBoardScalerUsed(j);
				if(i < MaxScalerOneBoard)
				{
					Stp_TempDisplayScaler = &(gStp_Processor->St_DisplayScaler[gStp_Processor->B_ScalerUsed]);
					Stp_TempDisplayScaler->Stp_SignalSource = Stp_SignalWindows->Stp_SignalSource;
					Stp_TempDisplayScaler->B_ScalerIndex = i;
					Stp_TempDisplayScaler->B_ScalerType = INPUT_SCALER;
					Stp_TempDisplayScaler->B_BoardIndex = j;
					memset(&St_TempOutputBoardGround, 0, sizeof(St_TempOutputBoardGround));
					TempData = FindBoardIndexBySignal(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
					St_TempOutputBoardGround.B_InputBoardIndex = TempData;
					TempData = ChangeSignalIndexToBoardSignalIndex(Stp_SignalWindows->Stp_SignalSource->SignalSourceType, Stp_SignalWindows->Stp_SignalSource->SignalSource);
					St_TempOutputBoardGround.B_InputBoardOutputIndex = (Stp_TempDisplayScaler->B_ScalerIndex << 1);
					St_TempOutputBoardGround.B_OutputBoardIndex =Stp_TempDisplayScreen->B_FPGAIndex + INPUT_BOARD_MAX;
					St_TempOutputBoardGround.B_ScalerIndex = gStp_Processor->B_ScalerUsed;
					TempData = FindOutputBoardGroundInputByInputSignal(St_TempOutputBoardGround.B_InputBoardIndex, St_TempOutputBoardGround.B_InputBoardOutputIndex, St_TempOutputBoardGround.B_OutputBoardIndex, 0);
					if(TempData == ERR)
					{
						TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 0);
						if(TempData < 4)
						{
							St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
							memcpy(&(gStp_Processor->St_OutputBoardForeground[gStp_Processor->B_ForegroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
							gStp_Processor->B_ForegroundUsed++;
							Stp_TempDisplayScaler->B_GroundType = 0;
							Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_ForegroundUsed - 1;
						}
						else
						{
							TempData = CountOutputBoardGroundUsed(St_TempOutputBoardGround.B_OutputBoardIndex, 1);
							if(TempData < 4)
							{
								St_TempOutputBoardGround.B_OutputBoardInputIndex = TempData;
								memcpy(&(gStp_Processor->St_OutputBoardBackground[gStp_Processor->B_BackgroundUsed]), &St_TempOutputBoardGround, sizeof(St_TempOutputBoardGround));
								gStp_Processor->B_BackgroundUsed++;
								Stp_TempDisplayScaler->B_GroundType = 1;
								Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
							}
							else
								printf("OutputBoard %d, Background ERROR!\r\n", St_TempOutputBoardGround.B_OutputBoardIndex);
						}
					}

//					Stp_TempDisplayScaler->B_GroundType = 1;
//					Stp_TempDisplayScaler->B_GroundIndex = gStp_Processor->B_BackgroundUsed - 1;
					memcpy(&(Stp_TempDisplayScaler->SignalInputWindow), &(Stp_SignalWindows->SignalInputWindow), sizeof(ST_Rect));
					memcpy(&(Stp_TempDisplayScaler->SignalDisplayWindow), &(Stp_SignalWindows->SignalDisplayWindow), sizeof(ST_Rect));
					gStp_Processor->B_ScalerUsed++;

					return 1;
				}
			}
		}
	}
	return ERR;
}

static int CheckRectWindowCover(ST_SignalWindows*	Stp_SignalWindows, DisplayWallWindowLayoutStruct *Stp_DisplayWallWindowLayout)
{
	int i = 0;
	ST_Rect *Stp_TempRectSource = NULL, *Stp_TempRectDest = NULL;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	Stp_TempRectSource = &(Stp_SignalWindows->SignalDisplayWindow);
	for(i = Stp_DisplayWallWindowLayout->WindowsTotalNum; i > 0; i--)
	{
		Stp_TempDisplayWallWindow = &(Stp_DisplayWallWindowLayout->DisplayWallWindow[i - 1]);
		Stp_TempRectDest = &(Stp_TempDisplayWallWindow->DisplayWindow);
		if(
			(Stp_SignalWindows->Stp_SignalSource->Windows_Layer < Stp_TempDisplayWallWindow->Windows_Layer)
			&&
			(Stp_TempRectSource->right <= Stp_TempRectDest->right)
			&&
			(Stp_TempRectSource->left >= Stp_TempRectDest->left)
			&&
			(Stp_TempRectSource->bottom <= Stp_TempRectDest->bottom)
			&&
			(Stp_TempRectSource->top >= Stp_TempRectDest->top)
		  )
			return 1;
	}
	return 0;
}

static DisplayWallWindowStruct* FindDisplayWallWindowBySignalWindows(ST_SignalWindows*	Stp_SignalWindows, DisplayWallWindowLayoutStruct *Stp_DisplayWallWindowLayout)
{
	int i = 0;
	ST_Rect *Stp_TempRectSource = NULL, *Stp_TempRectDest = NULL;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	Stp_TempRectSource = &(Stp_SignalWindows->SignalDisplayWindow);
	for(i = Stp_DisplayWallWindowLayout->WindowsTotalNum; i > 0; i--)
	{
		Stp_TempDisplayWallWindow = &(Stp_DisplayWallWindowLayout->DisplayWallWindow[i - 1]);
		Stp_TempRectDest = &(Stp_TempDisplayWallWindow->DisplayWindow);
		if(
			(Stp_SignalWindows->Stp_SignalSource->Windows_Layer == Stp_TempDisplayWallWindow->Windows_Layer)
			&&
			(Stp_SignalWindows->Stp_SignalSource->SignalSource == Stp_TempDisplayWallWindow->SignalSource)
			&&
			(Stp_SignalWindows->Stp_SignalSource->SignalSourceType == Stp_TempDisplayWallWindow->SignalSourceType)
			&&
			(Stp_TempRectSource->right <= Stp_TempRectDest->right)
			&&
			(Stp_TempRectSource->left >= Stp_TempRectDest->left)
			&&
			(Stp_TempRectSource->bottom <= Stp_TempRectDest->bottom)
			&&
			(Stp_TempRectSource->top >= Stp_TempRectDest->top)
		  )
			return Stp_TempDisplayWallWindow;
	}
	return NULL;
}

static int FindGTPIndexBySwitchOutputPort(int SwitchOutputPort)
{
#ifndef PC_VC_DEBUG
	int i = 0, j = 0, k = 0;
	ST_DEVICE_OUTPUT_GRAPHIC_FPGA *Stp_TempDeviceOutputGraphicFPGA = NULL;
	for(i = 0; i < gSt_OutputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceOutputGraphicFPGA = &(gSt_OutputBoarGraphicFPGA.St_DeviceOutputGraphicFPGA[i]);
		j = (Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex - 4) * 8;
		if((SwitchOutputPort >= j) && ((SwitchOutputPort < (j + 8))))
		{
			j = SwitchOutputPort % 8;
			switch(j)
			{
				case 1:
					// Background3
					k = BIT7;
					break;
				case 3:
					// Background2
					k = BIT6;
					break;
				case 5:
					// Background1
					k = BIT5;
					break;
				case 7:
					// Background0
					k = BIT4;
					break;
				case 0:
					// Foreground3
					k = BIT3;
					break;
				case 2:
					// Foreground2
					k = BIT2;
					break;
				case 4:
					// Foreground1
					k = BIT1;
					break;
				case 6:
					// Foreground0
					k = BIT0;
					break;
				default:
					k = ERR;
					break;
				}
			return k;
		}
	}
#endif
	return ERR;
}

static int FindOutputFPGAIndexBySwitchOutputPort(int SwitchOutputPort)
{
#ifndef PC_VC_DEBUG
	int i = 0, j = 0;
	ST_DEVICE_OUTPUT_GRAPHIC_FPGA *Stp_TempDeviceOutputGraphicFPGA = NULL;
	for(i = 0; i < gSt_OutputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceOutputGraphicFPGA = &(gSt_OutputBoarGraphicFPGA.St_DeviceOutputGraphicFPGA[i]);
		j = (Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex - 4) * 8;
		if((SwitchOutputPort >= j) && ((SwitchOutputPort < (j + 8))))
			return i;
	}
#endif
	return ERR;
}

int FindOutputBoardGroundInputByInputSignal(int InputBoardIndex, int InputIndex, int OutputBoardIndex, int GroundType)
{
	int i = 0;
	ST_OutputBoardGround *Stp_TempOutputBoardGround = NULL;
	if(GroundType == 0)
	{
//		printf("B_ForegroundUsed = %d\r\n", gStp_Processor->B_ForegroundUsed);
		for(i = 0; i < gStp_Processor->B_ForegroundUsed; i++)
		{
			Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[i]);
#if 0
			printf("OutputBoardIndex = %d, InputBoardIndex = %d, InputBoardOutputIndex = %d\r\n", Stp_TempOutputBoardGround->B_OutputBoardIndex,
			Stp_TempOutputBoardGround->B_InputBoardIndex,
			Stp_TempOutputBoardGround->B_InputBoardOutputIndex);
#endif
			if((Stp_TempOutputBoardGround->B_OutputBoardIndex == OutputBoardIndex)
					&& (Stp_TempOutputBoardGround->B_InputBoardIndex == InputBoardIndex)
					&&(Stp_TempOutputBoardGround->B_InputBoardOutputIndex == InputIndex))
			return i;
		}
	}
	else
	{
		for(i = 0; i < gStp_Processor->B_BackgroundUsed; i++)
		{
			Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[i]);
			if((Stp_TempOutputBoardGround->B_OutputBoardIndex == OutputBoardIndex)
					&& (Stp_TempOutputBoardGround->B_InputBoardIndex == InputBoardIndex)
					&&(Stp_TempOutputBoardGround->B_InputBoardOutputIndex == InputIndex))
			return i;
		}
	}
	return ERR;
}

static int CountOutputBoardGroundUsed(int BoardIndex, int GroundType)
{
	int i = 0, k = 0;
	ST_OutputBoardGround *Stp_TempOutputBoardGround = NULL;
	k = 0;
	if(GroundType == 0)
	{
		for(i = 0; i < gStp_Processor->B_ForegroundUsed; i++)
		{
			Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[i]);
			if(Stp_TempOutputBoardGround->B_OutputBoardIndex == BoardIndex)
				k++;
		}
	}
	else
	{
		for(i = 0; i < gStp_Processor->B_BackgroundUsed; i++)
		{
			Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[i]);
			if(Stp_TempOutputBoardGround->B_OutputBoardIndex == BoardIndex)
				k++;
		}
	}
	return k;
}

static int CheckSignalInDisplayWallWindowLayout(unsigned char B_SignalSource, unsigned char B_SignalSourceType)
{
	/*
	int i = 0;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	for(i = 0; i < gStp_DisplayWallWindowLayout->WindowsTotalNum; i++)
	{
		Stp_TempDisplayWallWindow = &(gStp_DisplayWallWindowLayout->DisplayWallWindow[i]);
		if((Stp_TempDisplayWallWindow->SignalSource == B_SignalSource) && (Stp_TempDisplayWallWindow->SignalSourceType == B_SignalSourceType))
			return 1;
	}
	*/
	return 0;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    Processor_DummyCall(void)                                    */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void Processor_DummyCall(void)
{
	CheckRectWindowCover(NULL, NULL);
	RequestOneScaler(NULL, 0);
	FindDisplayWallWindowBySignalWindows(NULL, NULL);
}
/********************************  END  *************************************/
