/*************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
//============================================================================*/
//
// MODULE:      DisplayWallLayout.c
//
// USAGE:       This module make display wall single screen  windows
//              and send this windows to output board.
//
// DESIGNER:	HAN LONG
/************************************************************************************************************/

/***************************************************************************/
/*	I N C L U D E    F I L E S												*/
/***************************************************************************/
#include <string.h>
#include <stdio.h>
#include "DisplayWallLayout.h"
#include "SignalWindowsCrop.h"
#include "Processor.h"
#ifndef PC_VC_DEBUG
	#include "../System Code/lpc17xx.h"
	#include "../System Code/type.h"
	#include "../System Code/global.h"
	#include "../System Code/Timer.h"
	#include "../Device/SerialToParallel/STP_74HC595.h"
	#include "../Device/IIC_BusSwitch/PCA9546A.h"
	#include "../Device/OutputFPGA/OutputFPGA.h"
	#include "../GraphicProcessor Board/SwitchBoard.h"
	#include "../GraphicProcessor Board/InputBoardSiI9233.h"
	#include "../GraphicProcessor Board/InputBoardTVP5158.h"
	#include "../GraphicProcessor Board/InputBoardGraphicFPGA.h"
	#include "../GraphicProcessor Board/OutputBoardGraphicFPGA.h"
	#include "../Device/OutputFPGA/OutputFPGA.h"
	#include "../GraphicProcessor Board/param.h"
#else
#endif

#define DISPLAY_WALL_LAYOUT_DEBUG						1
#define CVBSVideoCaptureHWidth 							640
#define CVBSVideoCaptureHStartOffset 				8
#define CVBSVideoCaptureVHeight 						540

/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/********************************************************************************/
//extern volatile unsigned long 	gD_ADV7604TickCnt;

DisplayWallWindowLayoutStruct		gSt_DisplayWallWindowLayout;
DisplayWallWindowLayoutStruct*	gStp_DisplayWallWindowLayout = &gSt_DisplayWallWindowLayout;
/********************************************************************************/
/*	S T A T I C    V A R I A B L E S										*/
/********************************************************************************/

static ST_Screen_Rect gSt_ScreenRectInput;
static ST_SignalWindows gSt_StaticScreenRectInputSignalWindows[MaxDiaplayWallWindowsNum];
static ST_SignalSource gSt_StaticWindowLayoutSignalSource[MaxDiaplayWallWindowsNum];
static ST_Screen_Rect_Output* gStp_ScreenRectOutput;
static ST_Signal_Route_Path St_StaticSignalRoutePath;

/****************************************************************************/
/*	G L O B A L    R O U T I N E S									 */
/****************************************************************************/

ST_Screen_Rect_Output* DisplayWallMakeSreenWindows(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout);
int DisplayWallSendScreenWindows(ST_Screen_Rect_Output* Stp_ScreenRectOutput,
		int WallRow,
		int WallLine,
		long WallWidth,
		long WallHeight);
void DisplayWallWindowLayoutInit(int ModeIndex);
void DisplayWallWindowLayoutVariable(void);
void ExecuteDisplayWallWindowLayout(void);
void FindSignalSourceActivePos(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_SourceActivePos);
void LoadDisplayWallWindowLayout(void);
/****************************************************************************/
/*	S T A T I C    R O U T I N E S									 */
/****************************************************************************/
static DisplayWallWindowLayoutStruct* WindowLayoutSortByLayer(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout);
static void ST_Screen_Rect_Init(ST_Screen_Rect* Stp_ScreenRect);
static void WindowLayoutVariableInit(void);
static void CheckScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput, ST_Screen_Rect*	Stp_ScreenRectInput);
static void DisplayWallScalerAllocate(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout, ST_Screen_Rect*	Stp_ScreenRectInput);
static void SetScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput, int ScreenWidth, int ScreenHeight);
static void PrintScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static ST_Screen_Rect_Output* CombineSingleScreenYWindows(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static int DisplayWallSetCVBSVideoInputPos(int VideoSignalIndex, ST_SignalWindows* Stp_SignalWindows);
static int DisplayWallFindCVBSVideoInputPos(int VideoSignalIndex, ST_Rect *Stp_SourceActivePos);
static void SaveDisplayWallWindowLayout(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout);

/*****************************************************************************/
/*	C O D E																										*/
/*****************************************************************************/
void DisplayWallWindowLayoutVariable(void)
{
	gStp_DisplayWallWindowLayout = &gSt_DisplayWallWindowLayout;
	memset(gSt_StaticScreenRectInputSignalWindows, 0, sizeof(gSt_StaticScreenRectInputSignalWindows));
}

void DisplayWallWindowLayoutInit(int ModeIndex)
{
	DisplayWallWindowLayoutStruct*	Stp_TempDisplayWallWindowLayout;
	Stp_TempDisplayWallWindowLayout = &gSt_DisplayWallWindowLayout;
	Stp_TempDisplayWallWindowLayout->DisplayWallLine = 2;
	Stp_TempDisplayWallWindowLayout->DisplayWallRow = 2;
	Stp_TempDisplayWallWindowLayout->DisplayWallWidth = 2 * 1920;
	Stp_TempDisplayWallWindowLayout->DisplayWallHeight = 2 * 1080;
	Stp_TempDisplayWallWindowLayout->WindowsTotalNum = 0;

	if(ModeIndex == 0)
	{
		// 窗口1
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].SignalSource = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].Windows_Layer = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.top = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.left = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.bottom = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.right = 1024;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口2
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].SignalSource = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].Windows_Layer = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.top = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.left = 1024;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.bottom = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.right = 1024 * 2;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口3
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].SignalSource = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].Windows_Layer = 3;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.top = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.left = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.bottom = 768 * 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.right = 1024;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口4
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].SignalSource = 3;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].Windows_Layer = 4;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.top = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.left = 1024;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.bottom = 768 * 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.right = 1024 * 2;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口5
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].SignalSource = 4;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].Windows_Layer = 5;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.top = 300;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.left = 400;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.bottom = 1300;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.right = 1600;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
	}
	if(ModeIndex == 1)
	{
		// 窗口1
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].SignalSource = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].Windows_Layer = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.top = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.left = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.bottom = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.right = 1024;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口2
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].SignalSource = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].Windows_Layer = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.top = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.left = 1024;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.bottom = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.right = 1024 * 2;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口3
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].SignalSource = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].Windows_Layer = 3;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.top = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.left = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.bottom = 768 * 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.right = 1024;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口4
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].SignalSource = 3;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].Windows_Layer = 4;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.top = 768;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.left = 1024;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.bottom = 768 * 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[3].DisplayWindow.right = 1024 * 2;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口5
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].SignalSource = 4;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].Windows_Layer = 5;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.top = 300;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.left = 400;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.bottom = 1300;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[4].DisplayWindow.right = 1600;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
	}
	if(ModeIndex == 2)
	{
		// 窗口1
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].SignalSource = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].SignalSourceType = SIGNAL_SOURCE_HDMI;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].Windows_Layer = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.top = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.left = 0;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.bottom = 1080 * 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[0].DisplayWindow.right = 1920 * 2;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		// 窗口2
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].SignalSource = 1;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].SignalSourceType = SIGNAL_SOURCE_HDMI;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].Windows_Layer = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.top = 200;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.left = 500;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.bottom = 2000;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[1].DisplayWindow.right = 3000;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		/*
		// 窗口3
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].SignalSource = 2;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].SignalSourceType = SIGNAL_SOURCE_HDMI;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].Windows_Layer = 3;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.top = 300;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.left = 700;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.bottom = 1800;
		Stp_TempDisplayWallWindowLayout->DisplayWallWindow[2].DisplayWindow.right = 2500;
		Stp_TempDisplayWallWindowLayout->WindowsTotalNum++;
		*/
	}
	DisplayWallMakeSreenWindows(Stp_TempDisplayWallWindowLayout);
}

void ExecuteDisplayWallWindowLayout(void)
{
//	DisplayWallWindowLayoutStruct*	Stp_TempDisplayWallWindowLayout;
//	Stp_TempDisplayWallWindowLayout = gStp_DisplayWallWindowLayout;
	SaveDisplayWallWindowLayout(gStp_DisplayWallWindowLayout);
	DisplayWallMakeSreenWindows(gStp_DisplayWallWindowLayout);
}

ST_Screen_Rect_Output* DisplayWallMakeSreenWindows(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout)
{
	ST_Screen_Rect*	Stp_TempScreenRectInput = NULL;
	ST_SignalWindows* tSt_SignalWindows = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
#ifndef PC_VC_DEBUG
	ST_DEVICE_SI_I9233 *Stp_TempDeviceSiI9233 = NULL;
	GlobalStatus_t *Stp_TempSiI9233Status_t = NULL;
#endif
	int i = 0, j = 0, ScreenWidth = 0, ScreenHeight = 0;
//	int TempData = 0, TempData1 = 0;
	long long TempllData = 0;
	for(i = SIG_OUT0; i < SIG_OUT_Max; i++)
		gW_SwitchBoardSwitchPort[i] = i;
	InputGraphicFPGAInitAllScaler();
	OutputGraphicFPGAInitAllScaler();
	WindowLayoutSortByLayer(Stp_DisplayWallWindowLayout);
	ST_Screen_Rect_Init(&gSt_ScreenRectInput);
	WindowLayoutVariableInit();
	Stp_TempScreenRectInput = &gSt_ScreenRectInput;
	Stp_TempScreenRectInput->ActiveRectWindowsNum = 0;

	//  初始化窗口
	for(i = 0; i < Stp_DisplayWallWindowLayout->WindowsTotalNum; i++)
	{
		tSt_SignalWindows = Stp_TempScreenRectInput->Stp_RectWindows[Stp_TempScreenRectInput->ActiveRectWindowsNum];
		Stp_TempSignalSource = tSt_SignalWindows->Stp_SignalSource;
		memcpy(&tSt_SignalWindows->SignalDisplayWindow, &(Stp_DisplayWallWindowLayout->DisplayWallWindow[i].DisplayWindow), sizeof(ST_Rect));
		if((tSt_SignalWindows->SignalDisplayWindow.right <= 0) || (tSt_SignalWindows->SignalDisplayWindow.bottom <= 0))
			continue;
		switch(Stp_DisplayWallWindowLayout->DisplayWallWindow[i].SignalSourceType)
		{
			case SIGNAL_SOURCE_HDMI:
				// SiI9233 HDMI Input
				j = Stp_DisplayWallWindowLayout->DisplayWallWindow[i].SignalSource;
				#ifndef PC_VC_DEBUG
					if(j < gSt_InputBoardSiI9233.B_ActiveChip)
					{
						Stp_TempDeviceSiI9233 = &(gSt_InputBoardSiI9233.St_DeviceSiI9233[j]);
						Stp_TempSiI9233Status_t = &(Stp_TempDeviceSiI9233->St_SiI9233GlobalStatus_t);
						#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
						printf("Source = %d, VideoHSize = %d, VideoVSize = %d\r\n", j, Stp_TempSiI9233Status_t->VideoHSize, Stp_TempSiI9233Status_t->VideoVSize);
						#endif
						tSt_SignalWindows->SignalInputWindow.top = 0;
						tSt_SignalWindows->SignalInputWindow.left = 0;
						if(Stp_TempSiI9233Status_t->VideoHSize == 0)
							tSt_SignalWindows->SignalInputWindow.right = 1024;
						else
							tSt_SignalWindows->SignalInputWindow.right = Stp_TempSiI9233Status_t->VideoHSize;
						if(Stp_TempSiI9233Status_t->VideoVSize == 0)
							tSt_SignalWindows->SignalInputWindow.bottom = 768;
						else
							tSt_SignalWindows->SignalInputWindow.bottom = Stp_TempSiI9233Status_t->VideoVSize;
					}
					else
					{
						tSt_SignalWindows->SignalInputWindow.right = 1024;
						tSt_SignalWindows->SignalInputWindow.bottom = 768;
					}
				#else
					tSt_SignalWindows->SignalInputWindow.top = 0;
					tSt_SignalWindows->SignalInputWindow.left = 0;
					tSt_SignalWindows->SignalInputWindow.right = 1024;
					tSt_SignalWindows->SignalInputWindow.bottom = 768;
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
				DisplayWallSetCVBSVideoInputPos(Stp_DisplayWallWindowLayout->DisplayWallWindow[i].SignalSource, tSt_SignalWindows);
				break;
			case SIGNAL_SOURCE_CVBS_2:
				tSt_SignalWindows->SignalInputWindow.top = 0;
				tSt_SignalWindows->SignalInputWindow.left = 0;
//				tSt_SignalWindows->SignalInputWindow.right = 1920;
//				tSt_SignalWindows->SignalInputWindow.bottom = 1080;
				tSt_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
				tSt_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
				break;
			case SIGNAL_SOURCE_SDI:
				break;
			case SIGNAL_SOURCE_NET:
				break;
			default:
				tSt_SignalWindows->SignalInputWindow.top = 0;
				tSt_SignalWindows->SignalInputWindow.left = 0;
				tSt_SignalWindows->SignalInputWindow.right = 1920;
				tSt_SignalWindows->SignalInputWindow.bottom = 1080;
				break;
		}
				
		Stp_TempSignalSource->Windows_Layer = (unsigned char) (Stp_TempScreenRectInput->ActiveRectWindowsNum + 1);
		Stp_TempSignalSource->SignalSource = Stp_DisplayWallWindowLayout->DisplayWallWindow[i].SignalSource;
		Stp_TempSignalSource->SignalSourceType = Stp_DisplayWallWindowLayout->DisplayWallWindow[i].SignalSourceType;
		TempllData = tSt_SignalWindows->SignalDisplayWindow.right - tSt_SignalWindows->SignalDisplayWindow.left;
		TempllData *= SCALE_COEFFICIENT;
		if((tSt_SignalWindows->SignalInputWindow.right - tSt_SignalWindows->SignalInputWindow.left) == 0)
			printf("HozScale Div Error. TempllData: %lld, right = %d, left = %d\r\n", TempllData, tSt_SignalWindows->SignalInputWindow.right, tSt_SignalWindows->SignalInputWindow.left);
		TempllData /= tSt_SignalWindows->SignalInputWindow.right - tSt_SignalWindows->SignalInputWindow.left;
		Stp_TempSignalSource->HozScale = (long) TempllData;
		TempllData = tSt_SignalWindows->SignalDisplayWindow.bottom - tSt_SignalWindows->SignalDisplayWindow.top;
		TempllData *= SCALE_COEFFICIENT;
		if((tSt_SignalWindows->SignalInputWindow.bottom - tSt_SignalWindows->SignalInputWindow.top) == 0)
			printf("VertScale Div Error. TempllData: %lld, bottom = %d, top = %d\r\n", TempllData, tSt_SignalWindows->SignalInputWindow.bottom, tSt_SignalWindows->SignalInputWindow.top);
		TempllData /= tSt_SignalWindows->SignalInputWindow.bottom - tSt_SignalWindows->SignalInputWindow.top;
		Stp_TempSignalSource->VertScale = (long) TempllData;
		if(tSt_SignalWindows->SignalDisplayWindow.left < 0)
		{
			TempllData = tSt_SignalWindows->SignalDisplayWindow.left;
			TempllData *= SCALE_COEFFICIENT;
			TempllData /= Stp_TempSignalSource->HozScale;
			tSt_SignalWindows->SignalInputWindow.left += (long) TempllData;
			tSt_SignalWindows->SignalDisplayWindow.left = 0;
		}
		if(tSt_SignalWindows->SignalDisplayWindow.top < 0)
		{
			TempllData = tSt_SignalWindows->SignalDisplayWindow.left;
			TempllData *= SCALE_COEFFICIENT;
			TempllData /= Stp_TempSignalSource->HozScale;
			tSt_SignalWindows->SignalInputWindow.top += (long) TempllData;
			tSt_SignalWindows->SignalDisplayWindow.top = 0;
		}

		Stp_TempScreenRectInput->ActiveRectWindowsNum++;
	}

	if(Stp_TempScreenRectInput->ActiveRectWindowsNum > 0)
	{
		DisplayWallScalerAllocate(Stp_DisplayWallWindowLayout, Stp_TempScreenRectInput);
		SetProcessorScaler();
		ScreenWidth = Stp_DisplayWallWindowLayout->DisplayWallWidth / Stp_DisplayWallWindowLayout->DisplayWallLine;
		ScreenHeight = Stp_DisplayWallWindowLayout->DisplayWallHeight / Stp_DisplayWallWindowLayout->DisplayWallRow;
		for(i = 0; i < Stp_DisplayWallWindowLayout->DisplayWallRow; i++)
			for(j = 0; j < Stp_DisplayWallWindowLayout->DisplayWallLine; j++)
			{
				gStp_ScreenRectOutput = MakeScreenRect(Stp_TempScreenRectInput, j * ScreenWidth, (j + 1) * ScreenWidth,
																																				i * ScreenHeight, (i + 1) * ScreenHeight);
				if(gStp_ScreenRectOutput == NULL)
				{
					printf("\r\nERROR:Creat Windows Too Much .\r\n");
					return NULL;
				}
				CheckScreenRectOutput(gStp_ScreenRectOutput, Stp_TempScreenRectInput);
				CombineSingleScreenYWindows(gStp_ScreenRectOutput);
				#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
				printf("\r\nScreen PosX = %d, PosY = %d\r\n", j, i);
				PrintScreenRectOutput(gStp_ScreenRectOutput);
				#endif
				SetScreenRectOutput(gStp_ScreenRectOutput, ScreenWidth, ScreenHeight);
			}
		return gStp_ScreenRectOutput;
	}
	else
	{
		Stp_TempScreenRectInput->ActiveRectWindowsNum = 1;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->Stp_SignalSource->SignalSource = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->Stp_SignalSource->SignalSourceType = SIGNAL_SOURCE_HDMI;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->Stp_SignalSource->Windows_Layer = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->Stp_SignalSource->VertScale = SCALE_COEFFICIENT;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->Stp_SignalSource->HozScale = SCALE_COEFFICIENT;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalInputWindow.top = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalInputWindow.bottom = 1080;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalInputWindow.left = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalInputWindow.right = 1920;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalDisplayWindow.top = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalDisplayWindow.bottom = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalDisplayWindow.left = 0;
		Stp_TempScreenRectInput->Stp_RectWindows[0]->SignalDisplayWindow.right = 0;
		gStp_ScreenRectOutput = MakeScreenRect(Stp_TempScreenRectInput, 0, 1920, 0, 1080);
		return gStp_ScreenRectOutput;
	}
}

static DisplayWallWindowLayoutStruct* WindowLayoutSortByLayer(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout)
{

	int i, j, flag;
	DisplayWallWindowStruct TempDisplayWallWindow;
	for(i = Stp_DisplayWallWindowLayout->WindowsTotalNum; i >= 2; i--)
	{
		flag = 0;
		for(j = 0; j <= i - 2; j++)
			if (Stp_DisplayWallWindowLayout->DisplayWallWindow[j].Windows_Layer > Stp_DisplayWallWindowLayout->DisplayWallWindow[j + 1].Windows_Layer)
			{
				//temp = LongArray[j];
				memcpy(&TempDisplayWallWindow, &Stp_DisplayWallWindowLayout->DisplayWallWindow[j], sizeof(DisplayWallWindowStruct));
				//LongArray[j] = LongArray[j + 1];
				memcpy(&Stp_DisplayWallWindowLayout->DisplayWallWindow[j], &Stp_DisplayWallWindowLayout->DisplayWallWindow[j + 1], sizeof(DisplayWallWindowStruct));
				//LongArray[j + 1] = temp;
				memcpy(&Stp_DisplayWallWindowLayout->DisplayWallWindow[j + 1], &TempDisplayWallWindow, sizeof(DisplayWallWindowStruct));
				flag = 1;
			}
		if (flag == 0) break;
	}
	return Stp_DisplayWallWindowLayout;
}

static void SaveDisplayWallWindowLayout(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout)
{
	WORD W_TempData = 0, W_TempData1 = 0;
	int i = 0;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	ST_Rect *Stp_TempRect = NULL;
	if(Stp_DisplayWallWindowLayout->WindowsTotalNum > MaxDiaplayWallWindowsNum)
		return;
	ParamWrite(NVR_DISPLAY_WALL_ROW, Stp_DisplayWallWindowLayout->DisplayWallRow);
	W_TempData1 = ParamRead(NVR_DISPLAY_WALL_ROW);
	if(W_TempData1 != Stp_DisplayWallWindowLayout->DisplayWallRow)
		ParamWrite(NVR_DISPLAY_WALL_ROW, Stp_DisplayWallWindowLayout->DisplayWallRow);
	ParamWrite(NVR_DISPLAY_WALL_LINE, Stp_DisplayWallWindowLayout->DisplayWallLine);
	W_TempData1 = ParamRead(NVR_DISPLAY_WALL_LINE);
	if(W_TempData1 != Stp_DisplayWallWindowLayout->DisplayWallLine)
		ParamWrite(NVR_DISPLAY_WALL_LINE, Stp_DisplayWallWindowLayout->DisplayWallLine);
	ParamWrite(NVR_DISPLAY_WALL_WIDTH, Stp_DisplayWallWindowLayout->DisplayWallWidth);
	W_TempData1 = ParamRead(NVR_DISPLAY_WALL_WIDTH);
	if(W_TempData1 != Stp_DisplayWallWindowLayout->DisplayWallWidth)
		ParamWrite(NVR_DISPLAY_WALL_WIDTH, Stp_DisplayWallWindowLayout->DisplayWallWidth);
	ParamWrite(NVR_DISPLAY_WALL_HEIGHT, Stp_DisplayWallWindowLayout->DisplayWallHeight);
	W_TempData1 = ParamRead(NVR_DISPLAY_WALL_HEIGHT);
	if(W_TempData1 != Stp_DisplayWallWindowLayout->DisplayWallHeight)
		ParamWrite(NVR_DISPLAY_WALL_HEIGHT, Stp_DisplayWallWindowLayout->DisplayWallHeight);
	ParamWrite(NVR_WINDOWS_TOTAL_NUM, Stp_DisplayWallWindowLayout->WindowsTotalNum);
	W_TempData1 = ParamRead(NVR_WINDOWS_TOTAL_NUM);
	if(W_TempData1 != Stp_DisplayWallWindowLayout->WindowsTotalNum)
		ParamWrite(NVR_WINDOWS_TOTAL_NUM, Stp_DisplayWallWindowLayout->WindowsTotalNum);
	W_TempData = NVR_WINDOW0_WINDOWS_LAYER;
	for(i = 0; i < Stp_DisplayWallWindowLayout->WindowsTotalNum; i++)
	{
		Stp_TempDisplayWallWindow = &(Stp_DisplayWallWindowLayout->DisplayWallWindow[i]);
		Stp_TempRect = &(Stp_TempDisplayWallWindow->DisplayWindow);
		ParamWrite(W_TempData, Stp_TempDisplayWallWindow->Windows_Layer);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayWallWindow->Windows_Layer)
			ParamWrite(W_TempData, Stp_TempDisplayWallWindow->Windows_Layer);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempDisplayWallWindow->SignalSource);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayWallWindow->SignalSource)
			ParamWrite(W_TempData, Stp_TempDisplayWallWindow->SignalSource);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempDisplayWallWindow->SignalSourceType);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempDisplayWallWindow->SignalSourceType)
			ParamWrite(W_TempData, Stp_TempDisplayWallWindow->SignalSourceType);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempRect->left);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempRect->left)
			ParamWrite(W_TempData, Stp_TempRect->left);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempRect->top);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempRect->top)
			ParamWrite(W_TempData, Stp_TempRect->top);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempRect->right);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempRect->right)
			ParamWrite(W_TempData, Stp_TempRect->right);
		W_TempData++;
		ParamWrite(W_TempData, Stp_TempRect->bottom);
		W_TempData1 = ParamRead(W_TempData);
		if(W_TempData1 != Stp_TempRect->bottom)
			ParamWrite(W_TempData, Stp_TempRect->bottom);
		W_TempData++;
	}
}

void LoadDisplayWallWindowLayout(void)
{
	WORD W_TempData = 0;
	int i = 0;
	DisplayWallWindowStruct *Stp_TempDisplayWallWindow = NULL;
	ST_Rect *Stp_TempRect = NULL;
	DisplayWallWindowLayoutStruct*	Stp_TempDisplayWallWindowLayout;
	Stp_TempDisplayWallWindowLayout = &gSt_DisplayWallWindowLayout;
	if(Stp_TempDisplayWallWindowLayout == NULL)
		return;
	Stp_TempDisplayWallWindowLayout->DisplayWallRow = ParamRead(NVR_DISPLAY_WALL_ROW);
	Stp_TempDisplayWallWindowLayout->DisplayWallLine = ParamRead(NVR_DISPLAY_WALL_LINE);
	Stp_TempDisplayWallWindowLayout->DisplayWallWidth = ParamRead(NVR_DISPLAY_WALL_WIDTH);
	Stp_TempDisplayWallWindowLayout->DisplayWallHeight = ParamRead(NVR_DISPLAY_WALL_HEIGHT);
	Stp_TempDisplayWallWindowLayout->WindowsTotalNum = ParamRead(NVR_WINDOWS_TOTAL_NUM);
	if(Stp_TempDisplayWallWindowLayout->WindowsTotalNum > MaxDiaplayWallWindowsNum)
		return;
	printf("LoadDisplayWallWindow: Row = %d, Line = %d, WallWidth = %d, WallHeight = %d, WindowsTotalNum = %d\r\n",
					Stp_TempDisplayWallWindowLayout->DisplayWallRow,
					Stp_TempDisplayWallWindowLayout->DisplayWallLine,
					Stp_TempDisplayWallWindowLayout->DisplayWallWidth,
					Stp_TempDisplayWallWindowLayout->DisplayWallHeight,
					Stp_TempDisplayWallWindowLayout->WindowsTotalNum);
	W_TempData = NVR_WINDOW0_WINDOWS_LAYER;
	for(i = 0; i < Stp_TempDisplayWallWindowLayout->WindowsTotalNum; i++)
	{
		Stp_TempDisplayWallWindow = &(Stp_TempDisplayWallWindowLayout->DisplayWallWindow[i]);
		if(Stp_TempDisplayWallWindow == NULL)
			return;
		Stp_TempRect = &(Stp_TempDisplayWallWindow->DisplayWindow);
		if(Stp_TempRect == NULL)
			return;
		Stp_TempDisplayWallWindow->Windows_Layer = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempDisplayWallWindow->SignalSource = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempDisplayWallWindow->SignalSourceType = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempRect->left = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempRect->top = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempRect->right = ParamRead(W_TempData);
		W_TempData++;
		Stp_TempRect->bottom = ParamRead(W_TempData);
		W_TempData++;
		printf("Layer = %d: Source = %d, SourceType = %d, left = %d, top = %d, right = %d, bottom = %d\r\n",
						Stp_TempDisplayWallWindow->Windows_Layer,
						Stp_TempDisplayWallWindow->SignalSource,
						Stp_TempDisplayWallWindow->SignalSourceType,
						Stp_TempRect->left,
						Stp_TempRect->top,
						Stp_TempRect->right,
						Stp_TempRect->bottom);
	}
//	if(Stp_TempDisplayWallWindowLayout->WindowsTotalNum > 0)
//		DisplayWallMakeSreenWindows(Stp_TempDisplayWallWindowLayout);
}

void FindSignalSourceActivePos(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_SourceActivePos)
{
	int j = 0;
#ifndef PC_VC_DEBUG
	ST_DEVICE_SI_I9233 *Stp_TempDeviceSiI9233 = NULL;
	GlobalStatus_t *Stp_TempSiI9233Status_t = NULL;
#endif
	switch(Stp_SignalSource->SignalSourceType)
	{
		case SIGNAL_SOURCE_HDMI:
			// SiI9233 HDMI Input
			j = Stp_SignalSource->SignalSource;
			#ifndef PC_VC_DEBUG
				if(j < gSt_InputBoardSiI9233.B_ActiveChip)
				{
					Stp_TempDeviceSiI9233 = &(gSt_InputBoardSiI9233.St_DeviceSiI9233[j]);
					Stp_TempSiI9233Status_t = &(Stp_TempDeviceSiI9233->St_SiI9233GlobalStatus_t);
					Stp_SourceActivePos->top = 0;
					Stp_SourceActivePos->left = 0;
					if(Stp_TempSiI9233Status_t->VideoHSize == 0)
						Stp_SourceActivePos->right = 1024;
					else
						Stp_SourceActivePos->right = Stp_TempSiI9233Status_t->VideoHSize;
					if(Stp_TempSiI9233Status_t->VideoVSize == 0)
						Stp_SourceActivePos->bottom = 768;
					else
						Stp_SourceActivePos->bottom = Stp_TempSiI9233Status_t->VideoVSize;
				}
				else
				{
					Stp_SourceActivePos->right = 1024;
					Stp_SourceActivePos->bottom = 768;
				}
			#else
				Stp_SourceActivePos->top = 0;
				Stp_SourceActivePos->left = 0;
				Stp_SourceActivePos->right = 1024;
				Stp_SourceActivePos->bottom = 768;
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
			DisplayWallFindCVBSVideoInputPos(Stp_SignalSource->SignalSource, Stp_SourceActivePos);
			break;
		case SIGNAL_SOURCE_CVBS_2:
				Stp_SourceActivePos->top = 0;
				Stp_SourceActivePos->left = 0;
				Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
				Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case SIGNAL_SOURCE_SDI:
			break;
		case SIGNAL_SOURCE_NET:
			break;
		default:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = 1920;
			Stp_SourceActivePos->bottom = 1080;
			break;
	}
}

static void ST_Screen_Rect_Init(ST_Screen_Rect* Stp_ScreenRect)
{
	int i = 0;
	Stp_ScreenRect->ActiveRectWindowsNum = 0;
	for(i = 0; i < (MaxDiaplayWallWindowsNum); i++)
		Stp_ScreenRect->Stp_RectWindows[i] = NULL;
}

static void WindowLayoutVariableInit(void)
{
	int i = 0;
	gSt_ScreenRectInput.ActiveRectWindowsNum = 0;
	memset(gSt_StaticScreenRectInputSignalWindows, 0, sizeof(gSt_StaticScreenRectInputSignalWindows));
	memset(gSt_StaticWindowLayoutSignalSource, 0, sizeof(gSt_StaticWindowLayoutSignalSource));
	for(i = 0; i < MaxDiaplayWallWindowsNum; i++)
	{
		gSt_ScreenRectInput.Stp_RectWindows[i] = &(gSt_StaticScreenRectInputSignalWindows[i]);
		(gSt_ScreenRectInput.Stp_RectWindows[i])->Stp_SignalSource = &(gSt_StaticWindowLayoutSignalSource[i]);
	}
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

static void DisplayWallScalerAllocate(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout, ST_Screen_Rect*	Stp_ScreenRectInput)
{
	int i = 0;
	ST_SignalWindows *Stp_RectWindows = NULL;
	ClearProcessorScaler();
	for(i = Stp_ScreenRectInput->ActiveRectWindowsNum; i > 0; i--)
	{
		Stp_RectWindows = Stp_ScreenRectInput->Stp_RectWindows[i - 1];
		AllocDisplayWindowScaler(Stp_DisplayWallWindowLayout, Stp_RectWindows);
	}
	#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
	PrintProcessorScalerMessage();
	#endif
}

static void SetScreenRectOutput(ST_Screen_Rect_Output* Stp_ScreenRectOutput, int ScreenWidth, int ScreenHeight)
{
	int i = 0, j = 0, k = 0, m = 0;
	int WindowXIndex = 0, WindowYIndex = 0;
	long long	tl_ScaleValue = 0;
	ST_Screen_Rect *Stp_TempScreenRowRect = NULL;
	ST_SignalWindows *Stp_OutputRectWindow = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_DisplayScaler *Stp_TempDisplayScaler = NULL;
	ST_DisplayScreen *Stp_TempDisplayScreen = NULL;
	ST_OutputBoardGround *Stp_TempOutputBoardGround = NULL;
	#ifndef PC_VC_DEBUG
	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	ST_DEVICE_OUTPUT_GRAPHIC_FPGA *Stp_TempDeviceOutputGraphicFPGA = NULL;
	ST_SignalWindows St_TempSignalWin;
	ST_SignalSource St_TempSignalSource;
	ST_Rect *Stp_RectSource = NULL, *Stp_RectDst = NULL;
	#endif
	if(Stp_ScreenRectOutput == NULL)
		return;
	if(Stp_ScreenRectOutput->ActiveRectWindowsTotalNum == 0)
		return;
	#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
	printf("RoutePath: Input(BoardIndex, InputPortIndex, OutputPortIndex, ScalerIndex), Output(BoardIndex, InputPortIndex, OutputPortIndex, ScalerIndex)\r\n");
	#endif
	WindowYIndex = 0;
	WindowXIndex = 0;
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
	{
		Stp_TempScreenRowRect = (Stp_ScreenRectOutput->St_ScreenRowRect[i]);
		WindowYIndex = i;
		for(j = 0; j < Stp_TempScreenRowRect->ActiveRectWindowsNum; j++)
		{
			Stp_OutputRectWindow = (Stp_TempScreenRowRect->Stp_RectWindows[j]);
			if(Stp_OutputRectWindow == NULL)
				continue;
			memset(&St_StaticSignalRoutePath, 0, sizeof(St_StaticSignalRoutePath));
			Stp_TempSignalSource = Stp_OutputRectWindow->Stp_SignalSource;
			Stp_TempDisplayScaler = FindScalerByDisplayRect(Stp_TempSignalSource, &(Stp_OutputRectWindow->SignalInputWindow), &(Stp_OutputRectWindow->SignalDisplayWindow));
			k = FindBoardIndexBySignal(Stp_TempSignalSource->SignalSourceType, Stp_TempSignalSource->SignalSource);
			St_StaticSignalRoutePath.B_InputBoardIndex = k;
			k = ChangeSignalIndexToBoardSignalIndex(Stp_TempSignalSource->SignalSourceType, Stp_TempSignalSource->SignalSource);
			St_StaticSignalRoutePath.B_InputBoardInputPortIndex = k;
			
			k = FindDisplayScreen(&(Stp_OutputRectWindow->SignalDisplayWindow));
			if((k == 0xFF) || (k >= gStp_Processor->B_DisplayScreenTotalNum))
			{
				printf("\r\nERROR: Can Not Find Display Screen,Check Output Board!\r\n");
				continue;
			}
			Stp_TempDisplayScreen = &(gStp_Processor->St_StaticDisplayScreen[k]);
			St_StaticSignalRoutePath.B_OutputBoardIndex = Stp_TempDisplayScreen->B_FPGAIndex;
			St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex = Stp_TempDisplayScreen->B_OutputPort;
			
			if(Stp_TempDisplayScaler == NULL)
			{
				St_StaticSignalRoutePath.B_InputBoardOutputPortIndex = (St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1) + 1;
				St_StaticSignalRoutePath.B_InputBoardScalerIndex = 0xFF;
				St_StaticSignalRoutePath.B_OutputBoardScalerIndex = 0xFF;
				k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																										St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																										St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																										0);
				#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
				printf("6 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																										St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																										St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
				#endif
				if(k == ERR)
				{
					k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																											1);
					#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
					printf("7 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
					#endif
					if(k == ERR)
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_InputBoardOutputPortIndex;
					else
					{
						Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[k]);
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1);
					}
				}
				else
				{
					Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[k]);
					St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1) + 1;
				}
			}
			else
			{
				if(Stp_TempDisplayScaler->B_ScalerType == INPUT_SCALER)
				{
//					St_StaticSignalRoutePath.B_InputBoardOutputPortIndex = St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1;
					St_StaticSignalRoutePath.B_InputBoardOutputPortIndex = Stp_TempDisplayScaler->B_ScalerIndex << 1;
					St_StaticSignalRoutePath.B_InputBoardScalerIndex = Stp_TempDisplayScaler->B_ScalerIndex;
					St_StaticSignalRoutePath.B_OutputBoardScalerIndex = 0xFF;
//					St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex << 1;
					#if 1
					k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																											0);
					#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
					printf("1 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
					#endif
					if(k == ERR)
					{
						k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																												St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																												St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																												1);
						#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
						printf("2 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																												St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																												St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
						#endif
						
						if(k == ERR)
							St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex << 1;
						else
						{
							Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[k]);
							St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1);
						}
					}
					else
					{
						Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[k]);
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1) + 1;
					}
					#endif
				}
				else if(Stp_TempDisplayScaler->B_ScalerType == OUTPUT_SCALER)
				{
					St_StaticSignalRoutePath.B_InputBoardOutputPortIndex = (St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1) + 1;
					St_StaticSignalRoutePath.B_InputBoardScalerIndex = 0xFF;
					St_StaticSignalRoutePath.B_OutputBoardScalerIndex = Stp_TempDisplayScaler->B_ScalerIndex;
					k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																											0);
					#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
					printf("3 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
					#endif
					if(k == ERR)
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_InputBoardOutputPortIndex;
					else
					{
						Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[k]);
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1) + 1;
					}
				}
				else
				{
					St_StaticSignalRoutePath.B_InputBoardOutputPortIndex = (St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1) + 1;
					St_StaticSignalRoutePath.B_InputBoardScalerIndex = 0xFF;
					St_StaticSignalRoutePath.B_OutputBoardScalerIndex = 0xFF;
//					St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_InputBoardOutputPortIndex;
					#if 1
					k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																											0);
					#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
					printf("4 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																											St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																											St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
					#endif
					if(k == ERR)
					{
						k = FindOutputBoardGroundInputByInputSignal(St_StaticSignalRoutePath.B_InputBoardIndex,
																												St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																												St_StaticSignalRoutePath.B_OutputBoardIndex + 4,
																												1);
						#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
						printf("5 OutputBoardGround, InputBoardIndex = %d, InputBoardOutputPortIndex = %d, OutputBoardIndex = %d, k = %d\r\n",St_StaticSignalRoutePath.B_InputBoardIndex,
																												St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
																												St_StaticSignalRoutePath.B_OutputBoardIndex + 4, k);
						#endif
						
						if(k == ERR)
							St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex << 1;
						else
						{
							Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardBackground[k]);
							St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1);
						}
					}
					else
					{
						Stp_TempOutputBoardGround = &(gStp_Processor->St_OutputBoardForeground[k]);
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex = (Stp_TempOutputBoardGround->B_OutputBoardInputIndex << 1) + 1;
					}
					#endif
				}
			}
			#if(DISPLAY_WALL_LAYOUT_DEBUG == 1)
			printf("%d, %d, %d, %d, %d, %d, %d, %d\r\n",
						St_StaticSignalRoutePath.B_InputBoardIndex,
						St_StaticSignalRoutePath.B_InputBoardInputPortIndex,
						St_StaticSignalRoutePath.B_InputBoardOutputPortIndex,
						St_StaticSignalRoutePath.B_InputBoardScalerIndex,
						St_StaticSignalRoutePath.B_OutputBoardIndex,
						St_StaticSignalRoutePath.B_OutputBoardInputPortIndex,
						St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex,
						St_StaticSignalRoutePath.B_OutputBoardScalerIndex);
			#endif
			#ifndef PC_VC_DEBUG
			Stp_TempDeviceOutputGraphicFPGA = &(gSt_OutputBoarGraphicFPGA.St_DeviceOutputGraphicFPGA[St_StaticSignalRoutePath.B_OutputBoardIndex]);
			Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[St_StaticSignalRoutePath.B_InputBoardIndex]);
			k = InputGraphicFPGAFindGTPPortNum(St_StaticSignalRoutePath.B_InputBoardIndex, St_StaticSignalRoutePath.B_InputBoardOutputPortIndex);
			if(k != 0xFF)
			{
				k += Stp_TempDeviceInputGraphicFPGA->B_BoardIndex * 8;
				m = OutputGraphicFPGAFindGTPPortNum(St_StaticSignalRoutePath.B_OutputBoardIndex, St_StaticSignalRoutePath.B_OutputBoardInputPortIndex);
				if(m != 0xFF)
				{
					m += (Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex - INPUT_BOARD_MAX) * 8;
					printf("Switch k = %d, To m = %d\r\n", k, m);
					gW_SwitchBoardSwitchPort[m] = k;
				}
			}
			memset(&St_TempSignalWin, 0, sizeof(St_TempSignalWin));
			memcpy(&St_TempSignalSource, Stp_TempSignalSource, sizeof(St_TempSignalSource));
			St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
			if((Stp_TempDisplayScaler != NULL) && (Stp_TempDisplayScaler->B_ScalerType == OUTPUT_SCALER))
				St_TempSignalSource.SignalSource = (Stp_TempDisplayScaler->B_ScalerIndex << 1) + 1;
			else if((Stp_TempDisplayScaler != NULL) && (Stp_TempDisplayScaler->B_ScalerType == INPUT_SCALER))
//				St_TempSignalSource.SignalSource = St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1;
				St_TempSignalSource.SignalSource = (St_StaticSignalRoutePath.B_OutputBoardInputPortIndex >> 1) << 1;
			else if((Stp_TempDisplayScaler == NULL) && (St_TempSignalSource.Windows_Layer > 0))
			{
//				St_TempSignalSource.SignalSource = St_StaticSignalRoutePath.B_InputBoardInputPortIndex << 1;
				St_TempSignalSource.SignalSource = (St_StaticSignalRoutePath.B_OutputBoardInputPortIndex >> 1) << 1;
				St_TempSignalSource.SignalSource += St_StaticSignalRoutePath.B_OutputBoardInputPortIndex & BIT0;
			}
			else
				St_TempSignalSource.SignalSource = 0xFF;
//				memcpy(&(St_TempSignalWin.SignalDisplayWindow), &(Stp_OutputRectWindow->SignalDisplayWindow), sizeof(ST_Rect));
			Stp_RectSource =  &(Stp_OutputRectWindow->SignalDisplayWindow);
			Stp_RectDst = &(St_TempSignalWin.SignalDisplayWindow);
			Stp_RectDst->left = Stp_RectSource->left % ScreenWidth;
			Stp_RectDst->right = Stp_RectSource->right % ScreenWidth;
			if(Stp_RectDst->right == 0)
				Stp_RectDst->right = ScreenWidth;
			Stp_RectDst->top = Stp_RectSource->top % ScreenHeight;
			Stp_RectDst->bottom = Stp_RectSource->bottom % ScreenHeight;
			if(Stp_RectDst->bottom == 0)
				Stp_RectDst->bottom = ScreenHeight;
			if(Stp_TempDisplayScaler != NULL)
			{
				Stp_RectDst = &(St_TempSignalWin.SignalInputWindow);
				Stp_RectSource = &(Stp_OutputRectWindow->SignalInputWindow);
				tl_ScaleValue =  Stp_RectSource->left - Stp_TempDisplayScaler->SignalInputWindow.left;
				if(tl_ScaleValue < 0)
					tl_ScaleValue = 0 - tl_ScaleValue;
				tl_ScaleValue *= Stp_TempSignalSource->HozScale;
				tl_ScaleValue /= SCALE_COEFFICIENT;
				Stp_RectDst->left = (long) (tl_ScaleValue);

				if(Stp_RectSource->right == Stp_TempDisplayScaler->SignalInputWindow.right)
					Stp_RectDst->right = Stp_RectSource->right;
				else
				{
					tl_ScaleValue = Stp_RectSource->right - Stp_TempDisplayScaler->SignalInputWindow.right;
					if(tl_ScaleValue < 0)
						tl_ScaleValue = 0 - tl_ScaleValue;
					tl_ScaleValue *= Stp_TempSignalSource->HozScale;
					tl_ScaleValue /= SCALE_COEFFICIENT;
					Stp_RectDst->right = (long) (tl_ScaleValue);
				}

				tl_ScaleValue = Stp_RectSource->top - Stp_TempDisplayScaler->SignalInputWindow.top;
				if(tl_ScaleValue < 0)
					tl_ScaleValue = 0 - tl_ScaleValue;
				tl_ScaleValue *= Stp_TempSignalSource->VertScale;
				tl_ScaleValue /= SCALE_COEFFICIENT;
				Stp_RectDst->top = (long) (tl_ScaleValue);
				
				if(Stp_RectSource->bottom == Stp_TempDisplayScaler->SignalInputWindow.bottom)
					Stp_RectDst->bottom = Stp_RectSource->bottom;
				else
				{
					tl_ScaleValue = Stp_RectSource->bottom - Stp_TempDisplayScaler->SignalInputWindow.bottom;
					if(tl_ScaleValue < 0)
						tl_ScaleValue = 0 - tl_ScaleValue;
					tl_ScaleValue *= Stp_TempSignalSource->VertScale;
					tl_ScaleValue /= SCALE_COEFFICIENT;
					Stp_RectDst->bottom = (long) (tl_ScaleValue);
				}
			}
			else
				memcpy(&(St_TempSignalWin.SignalInputWindow), &(Stp_OutputRectWindow->SignalInputWindow), sizeof(ST_Rect));
			if(Stp_TempDeviceOutputGraphicFPGA)
			{
				SelectDeviceI2CBus(Stp_TempDeviceOutputGraphicFPGA->B_BoardIndex, Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex);
				if(Stp_TempDeviceOutputGraphicFPGA->B_I2CBusIndex == 0)
				{
					if(Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex < 0x0F)
						PCA9546A_SelectBusPort(BIT0 << Stp_TempDeviceOutputGraphicFPGA->B_SubI2CBusIndex);
				}
			}
			else
				continue;

			if((i == 0) && (j == 0))
			{
				OutputFPGASetVideoOutputDisalble(BIT0 << St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex);
				OutputFPGA_SoftReset(BIT0 << (St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex + 4));
			}
			if((St_StaticSignalRoutePath.B_InputBoardScalerIndex != 0xFF) && (Stp_TempDisplayScaler->B_GroundType == 1))
			{
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0xFF;
				OutputFPGASetBackgroundWindow(St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex, WindowXIndex, WindowYIndex, St_TempSignalWin);
			}
			else
			{
				OutputFPGASetBackgroundWindow(St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex, WindowXIndex, WindowYIndex, St_TempSignalWin);
				if(St_TempSignalWin.Stp_SignalSource->Windows_Layer > 0)
					WindowXIndex++;
			}

			#endif
		}
		WindowXIndex = 0;
	}
	OutputFPGASetVideoOutputEnalble(BIT0 << St_StaticSignalRoutePath.B_OutputBoardOutputPortIndex);
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

	printf("Output Screen Rect Total Num: %d\r\n", Stp_ScreenRectOutput->ActiveRectWindowsTotalNum);
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

static int DisplayWallSetCVBSVideoInputPos(int VideoSignalIndex, ST_SignalWindows* Stp_SignalWindows)
{
	BYTE B_Data = 0;
	B_Data = gSt_InputBoarTVP5158.B_ActiveChip * 4;
	if(VideoSignalIndex > B_Data)
	{
		Stp_SignalWindows->SignalInputWindow.top = 0;
		Stp_SignalWindows->SignalInputWindow.left = 0;
		Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
		Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
		return -1;
	}
	B_Data = VideoSignalIndex % 24;
	switch(B_Data)
	{
		case 0:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 1:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 2:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 3:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 4:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 5:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 6:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 7:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 8:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 9:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 10:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 11:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 12:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 13:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 14:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 15:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 16:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 17:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 18:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 19:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 20:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
		case 21:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 22:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SignalWindows->SignalInputWindow.right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 23:
			Stp_SignalWindows->SignalInputWindow.top = CVBSVideoCaptureVHeight;
			Stp_SignalWindows->SignalInputWindow.left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth * 3;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight * 2;
			break;
		default:
			Stp_SignalWindows->SignalInputWindow.top = 0;
			Stp_SignalWindows->SignalInputWindow.left = 0;
			Stp_SignalWindows->SignalInputWindow.right = CVBSVideoCaptureHWidth;
			Stp_SignalWindows->SignalInputWindow.bottom = CVBSVideoCaptureVHeight;
			break;
	}
	return 1;
}

static int DisplayWallFindCVBSVideoInputPos(int VideoSignalIndex, ST_Rect *Stp_SourceActivePos)
{
//	#define CVBSVideoCaptureHWidth 640
//	#define CVBSVideoCaptureVHeight 490
	BYTE B_Data = 0;
	B_Data = gSt_InputBoarTVP5158.B_ActiveChip * 4;
	if(VideoSignalIndex > B_Data)
	{
		Stp_SourceActivePos->top = 0;
		Stp_SourceActivePos->left = 0;
		Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
		Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
		return -1;
	}
	B_Data = VideoSignalIndex % 24;
	switch(B_Data)
	{
		case 0:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 1:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 2:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 3:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 4:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 5:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 6:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 7:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 8:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 9:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 10:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 11:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 12:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 13:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 14:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 15:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 16:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 17:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 18:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 19:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 20:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
		case 21:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 22:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset);
			Stp_SourceActivePos->right = (CVBSVideoCaptureHWidth * 2) + CVBSVideoCaptureHStartOffset;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		case 23:
			Stp_SourceActivePos->top = CVBSVideoCaptureVHeight;
			Stp_SourceActivePos->left = (CVBSVideoCaptureHWidth + CVBSVideoCaptureHStartOffset) * 2;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth * 3;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight * 2;
			break;
		default:
			Stp_SourceActivePos->top = 0;
			Stp_SourceActivePos->left = 0;
			Stp_SourceActivePos->right = CVBSVideoCaptureHWidth;
			Stp_SourceActivePos->bottom = CVBSVideoCaptureVHeight;
			break;
	}
	return 1;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    DisplayWallLayout_DummyCall(void)                            */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*							being called.												*/
/*																			*/
/****************************************************************************/
void DisplayWallLayout_DummyCall(void)
{
	WindowLayoutSortByLayer(NULL);
	ST_Screen_Rect_Init(NULL);
	WindowLayoutVariableInit();
	CheckScreenRectOutput(NULL, NULL);
	PrintScreenRectOutput(NULL);
	CombineSingleScreenYWindows(NULL);
	DisplayWallSetCVBSVideoInputPos(0, NULL);
	SaveDisplayWallWindowLayout(NULL);
}
/********************************  END  *************************************/
