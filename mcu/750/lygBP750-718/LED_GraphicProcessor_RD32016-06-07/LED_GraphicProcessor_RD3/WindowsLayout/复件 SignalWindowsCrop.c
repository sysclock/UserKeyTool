#define __SIGNALWINDOWSCROP_C__
//*******************************************************************************************************************************
//
//              Copyright (C) 2002.  TRIOLION INC. CONFIDENTIAL
//      All rights reserved.  No part of this program may be reproduced.
//
//     Triolion Inc.
//
//==============================================================================
//
// MODULE:      SignalWindowsCrop.c
//
// USAGE:       This module contains functions for calculating and programming
//              the parametrs related to rect on screen.
//
//******************************************************************************************************************************

//***************************************************************************/
//	I N C L U D E    F I L E S
//***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SignalWindowsCrop.h"

#define MAX_SIGNAL_WINDOWS								32
#define MAX_DIVISIVE_RECT_WINDOWS_NUM			64

//****************************************************************************************/
//	G L O B A L    F U N C T I O N    P R O T O T Y P E S
//****************************************************************************************/
void SignalWindowsCropInit(void);
ST_Screen_Rect_Output* MakeScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd);

/***************************************************************************/
//	G L O B A L    V A R I A B L E S
//***************************************************************************/

//***************************************************************************/
//	S T A T I C    V A R I A B L E S
//***************************************************************************/
static ST_Screen_Rect	gSt_SignalWindowsCropScreenRectInput;
static ST_SignalWindows gSt_ScreenRectInputScreenSignalWindows[MaxRectWindowsNum];

static ST_Screen_Rect	gSt_WindowsOutputScreenRowRect[MaxRectWindowsNum];
static ST_SignalWindows* gStp_WindowsOutputScreenSignalWindows[MaxRectWindowsNum][MaxRectWindowsNum];
static ST_SignalWindows gSt_WindowsOutputScreenSignalWindows[MAX_SIGNAL_WINDOWS];

static ST_Screen_Rect_Output	gSt_ScreenRectWindowsOutput;
static ST_Screen_Rect_Output*	gStp_ScreenRectWindowsOutput = NULL;

//***************************************************************************/
//	S T A T I C    R O U T I N E S
//***************************************************************************/
//static void ST_Screen_Rect_Output_Board_Init(ST_Screen_Rect_Output_Board* Stp_ScreenRect);
static void	CheckScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd);

static ST_Screen_Rect_Output*	DivisiveScreen(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd);
static void SignalWindowsCropVariableInit(void);
static void ST_Screen_Rect_Output_Init(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
static int OutputScreenSignalWindowsInit(int RowNum, int LineNum);
static int BubSort(long* LongArray, int len);
static int SetRectWindowCrop(ST_SignalWindows* Stp_RectWindows, ST_Screen_Rect* Stp_ScreenRectInput);
static int	CombineScreen(ST_Screen_Rect_Output* Stp_ScreenRectOutput);
//***************************************************************************/
//	C O D E
//***************************************************************************/

void SignalWindowsCropInit(void)
{
	int i = 0;
	gSt_SignalWindowsCropScreenRectInput.ActiveRectWindowsNum = 0;
	memset(gSt_ScreenRectInputScreenSignalWindows, 0, sizeof(gSt_ScreenRectInputScreenSignalWindows));
	for(i = 0; i < MaxRectWindowsNum; i++)
		gSt_SignalWindowsCropScreenRectInput.Stp_RectWindows[i] = &(gSt_ScreenRectInputScreenSignalWindows[i]);
	gStp_ScreenRectWindowsOutput = &gSt_ScreenRectWindowsOutput;
}

//*****************************************************************************************************************/
//
// FUNCTION:	MakeScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
//										long XPosStart,
//										long XPosEnd,
//										long YPosStart,
//										long YPosEnd)
//
// USAGE:		在指定区域中生成显示的矩形窗口
//
// INPUT:		Stp_ScreenRectInput	屏幕中的输入信号窗口
//					XPosStart 指定区域X起始
//					XPosEnd 指定区域X结束
//					YPosStart 指定区域Y起始
//					YPosEnd 指定区域Y结束
//
// OUTPUT:		显示墙分割后生成的显示矩形窗口
//
//*****************************************************************************************************************/
ST_Screen_Rect_Output* MakeScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd)
{
	ST_Screen_Rect_Output*	Stp_ScreenRectWindowsOutput = NULL;
	ST_Screen_Rect_Output_Init(&gSt_ScreenRectWindowsOutput);
	CheckScreenRect(Stp_ScreenRectInput, XPosStart, XPosEnd, YPosStart, YPosEnd);
	Stp_ScreenRectWindowsOutput = DivisiveScreen(&gSt_SignalWindowsCropScreenRectInput, XPosStart, XPosEnd, YPosStart, YPosEnd);
	if(Stp_ScreenRectWindowsOutput == NULL)
		return NULL;
	CombineScreen(Stp_ScreenRectWindowsOutput);
	return	Stp_ScreenRectWindowsOutput;
}
/*
static void ST_Screen_Rect_Output_Board_Init(ST_Screen_Rect_Output_Board* Stp_ScreenRect)
{
	int i = 0;
	Stp_ScreenRect->ActiveRectWindowsNum = 0;
	for(i = 0; i < (MaxRectWindowsNum * 4 + 1); i++)
		Stp_ScreenRect->Stp_RectWindows[i] = NULL;
}
*/
//*****************************************************************************************************************/
//
// FUNCTION:	CheckScreenRect(ST_Screen_Rect_Input* Stp_ScreenRectInput,
//										long XPosStart,
//										long XPosEnd,
//										long YPosStart,
//										long YPosEnd)
//
// USAGE:		检查窗口是否越界,将不越界的输入窗口重新计算后保存到本地变量 
//
// INPUT:		ST_Screen_Rect_Input* Stp_ScreenRectInput
//
// OUTPUT:		None
//
//*****************************************************************************************************************/
static void	CheckScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd)
{
	int i = 0;
	ST_SignalWindows *Stp_TempSignalWindows1 = NULL, *Stp_TempSignalWindows2 = NULL;
	long long	tl_ScaleValue = 0;
//	long TempData = 0;
	gSt_SignalWindowsCropScreenRectInput.ActiveRectWindowsNum = 0;
	for(i = 0; i < Stp_ScreenRectInput->ActiveRectWindowsNum; i++)
	{
		if(Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.top > YPosEnd)
			continue;
		if(Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.bottom < YPosStart)
			continue;
		if(Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.left > XPosEnd)
			continue;
		if(Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.right < XPosStart)
			continue;
		Stp_TempSignalWindows1 = gSt_SignalWindowsCropScreenRectInput.Stp_RectWindows[gSt_SignalWindowsCropScreenRectInput.ActiveRectWindowsNum];
		Stp_TempSignalWindows2 = Stp_ScreenRectInput->Stp_RectWindows[i];
		memcpy(Stp_TempSignalWindows1, Stp_TempSignalWindows2, sizeof(ST_SignalWindows));
		if(Stp_TempSignalWindows2->SignalDisplayWindow.left < XPosStart)
		{
			Stp_TempSignalWindows1->SignalDisplayWindow.left = XPosStart;
			tl_ScaleValue = Stp_TempSignalWindows1->SignalDisplayWindow.left - Stp_TempSignalWindows2->SignalDisplayWindow.left;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_TempSignalWindows2->HozScale;
			Stp_TempSignalWindows1->SignalInputWindow.left = Stp_TempSignalWindows2->SignalInputWindow.left + tl_ScaleValue;
		}
		if(Stp_TempSignalWindows2->SignalDisplayWindow.right > XPosEnd)
		{
			Stp_TempSignalWindows1->SignalDisplayWindow.right = XPosEnd;
			tl_ScaleValue = Stp_TempSignalWindows2->SignalDisplayWindow.right - Stp_TempSignalWindows1->SignalDisplayWindow.right;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_TempSignalWindows2->VertScale;
			Stp_TempSignalWindows1->SignalInputWindow.right = Stp_TempSignalWindows2->SignalInputWindow.right - tl_ScaleValue;
		}
		if(Stp_TempSignalWindows2->SignalDisplayWindow.top < YPosStart)
		{
			Stp_TempSignalWindows1->SignalDisplayWindow.top = YPosStart;
			tl_ScaleValue = Stp_TempSignalWindows1->SignalDisplayWindow.top - Stp_TempSignalWindows2->SignalDisplayWindow.top;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_TempSignalWindows2->VertScale;
			Stp_TempSignalWindows1->SignalInputWindow.top = Stp_TempSignalWindows2->SignalInputWindow.top + tl_ScaleValue;
		}
		if(Stp_TempSignalWindows2->SignalDisplayWindow.bottom > YPosEnd)
		{
			Stp_TempSignalWindows1->SignalDisplayWindow.bottom = YPosEnd;
			tl_ScaleValue = Stp_TempSignalWindows2->SignalDisplayWindow.bottom - Stp_TempSignalWindows1->SignalDisplayWindow.bottom;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_TempSignalWindows2->VertScale;
			Stp_TempSignalWindows1->SignalInputWindow.bottom = Stp_TempSignalWindows2->SignalInputWindow.bottom - tl_ScaleValue;
		}
		gSt_SignalWindowsCropScreenRectInput.ActiveRectWindowsNum++;
	}
}

//***************************************************************************************************************/
//
// FUNCTION:	DivisiveScreen(ST_Screen_Rect_Input* Stp_ScreenRectInput,
//										long XPosStart,
//										long XPosEnd,
//										long YPosStart,
//										long YPosEnd)
//
// USAGE:		在指定区域中将屏幕窗口划分为平面矩形窗口
//					XPosStart 指定区域X起始
//					XPosEnd 指定区域X结束
//					YPosStart 指定区域Y起始
//					YPosEnd 指定区域Y结束
//
// INPUT:		ST_Screen_Rect_Input* Stp_ScreenRectInput
//
// OUTPUT:		ST_Screen_Rect_Output*
//
//
//****************************************************************************************************************/
static ST_Screen_Rect_Output*	DivisiveScreen(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd)
{
	long Pixel_X[MAX_DIVISIVE_RECT_WINDOWS_NUM] = {0};
	long Pixel_Y[MAX_DIVISIVE_RECT_WINDOWS_NUM] = {0};
	int Pixel_X_Num = 0;
	int Pixel_Y_Num = 0;

	int i = 0, j = 0;
	ST_Screen_Rect*	Stp_ScreenRowRect = NULL;
	ST_SignalWindows* Stp_RectWindow = NULL;
	gStp_ScreenRectWindowsOutput = &gSt_ScreenRectWindowsOutput;
	SignalWindowsCropVariableInit();
	Pixel_X_Num = 0;
	Pixel_Y_Num = 0;
	i = Stp_ScreenRectInput->ActiveRectWindowsNum * 2 + 2;
	if(i > (MaxRectWindowsNum * 2 + 17 * 2))
	{
		printf("Pixel_X Memory Over Flow\r\n");
		return NULL;
	}
	i = Stp_ScreenRectInput->ActiveRectWindowsNum * 2 + 2;
	if(i > (MaxRectWindowsNum * 2 + 17 * 2))
	{
		printf("Pixel_Y Memory Over Flow\r\n");
		return NULL;
	}
	Pixel_X[0] = XPosStart;
	Pixel_Y[0] = YPosStart;
	for(i = 0; i < Stp_ScreenRectInput->ActiveRectWindowsNum; i++)
	{
		Pixel_X[i * 2 + 1] = Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.left;
		Pixel_X[i * 2 + 2] = Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.right;
		Pixel_Y[i * 2 + 1] = Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.top;
		Pixel_Y[i * 2 + 2] = Stp_ScreenRectInput->Stp_RectWindows[i]->SignalDisplayWindow.bottom;
	}
	Pixel_X[(Stp_ScreenRectInput->ActiveRectWindowsNum + 1) * 2 - 1] = XPosEnd;
	Pixel_Y[(Stp_ScreenRectInput->ActiveRectWindowsNum + 1) * 2 - 1] = YPosEnd;
	Pixel_X_Num = BubSort(Pixel_X, (Stp_ScreenRectInput->ActiveRectWindowsNum + 1) * 2);
	Pixel_Y_Num = BubSort(Pixel_Y, (Stp_ScreenRectInput->ActiveRectWindowsNum + 1) * 2);
	if(Pixel_Y_Num > MaxRectWindowsNum)
	{
		printf("Windows Row Memory Over Flow\r\n");
		return NULL;
	}
	if(OutputScreenSignalWindowsInit(Pixel_Y_Num - 1, Pixel_X_Num - 1) == -1)
	{
		printf("Output Screen Signal Windows Over Flow\r\n");
		return NULL;
	}
	gStp_ScreenRectWindowsOutput->ActiveRectWindowsRow = Pixel_Y_Num - 1;
	gStp_ScreenRectWindowsOutput->ActiveRectWindowsTotalNum = 0;
	for(i = 0; i < Pixel_Y_Num - 1; i++)
	{
		Stp_ScreenRowRect = gStp_ScreenRectWindowsOutput->St_ScreenRowRect[i];
		Stp_ScreenRowRect->ActiveRectWindowsNum = 0;
		for(j = 0; j < Pixel_X_Num - 1; j++)
		{
			//Stp_RectWindow = (ST_SignalWindows*) malloc(sizeof(ST_SignalWindows));
			if(Stp_ScreenRowRect->ActiveRectWindowsNum >= MaxRectWindowsNum)
			{
				printf("Windows Line Memory Over Flow\r\n");
				return NULL;
			}
			Stp_RectWindow = Stp_ScreenRowRect->Stp_RectWindows[Stp_ScreenRowRect->ActiveRectWindowsNum];
			Stp_RectWindow->SignalDisplayWindow.top = Pixel_Y[i];
			Stp_RectWindow->SignalDisplayWindow.bottom = Pixel_Y[i + 1];
			Stp_RectWindow->SignalDisplayWindow.left = Pixel_X[j];
			Stp_RectWindow->SignalDisplayWindow.right = Pixel_X[j + 1];
			if(SetRectWindowCrop(Stp_RectWindow, Stp_ScreenRectInput) == 0)
			{
				Stp_RectWindow->SignalInputWindow.left = Stp_RectWindow->SignalDisplayWindow.left;
				Stp_RectWindow->SignalInputWindow.top = Stp_RectWindow->SignalDisplayWindow.top;
				Stp_RectWindow->SignalInputWindow.right = Stp_RectWindow->SignalDisplayWindow.right;
				Stp_RectWindow->SignalInputWindow.bottom = Stp_RectWindow->SignalDisplayWindow.bottom;
				Stp_RectWindow->SignalInputOriWidth = XPosEnd - XPosStart;
				Stp_RectWindow->SignalInputOriHeight = YPosEnd - YPosStart;
				Stp_RectWindow->SignalSource = 0;
				Stp_RectWindow->Windows_Layer = 0;
				Stp_RectWindow->HozScale = SCALE_COEFFICIENT;
				Stp_RectWindow->VertScale = SCALE_COEFFICIENT;
			}
			Stp_ScreenRowRect->ActiveRectWindowsNum++;
			gStp_ScreenRectWindowsOutput->ActiveRectWindowsTotalNum++;
		}
	}
	return gStp_ScreenRectWindowsOutput;
}

//*******************************************************************************************************/
//
// FUNCTION:	SetRectWindowCrop(ST_SignalWindows* Stp_RectWindows,
//								ST_Screen_Rect* Stp_ScreenRectInput)
//
// USAGE:		设置显示窗口相对于输入的裁剪位置、信号源、所在层等信息
//
// INPUT:		ST_SignalWindows	矩形窗口
//				Stp_ScreenRectInput	每层窗口列表
//
// OUTPUT:		是否成功
//
//*****************************************************************************************************/
static int SetRectWindowCrop(ST_SignalWindows* Stp_RectWindows, ST_Screen_Rect* Stp_ScreenRectInput)
{
	int i = 0;
	ST_PIXEL	t_Pixel;
	ST_Rect St_TempRect;
	long long	tl_ScaleValue = 0;
	t_Pixel.X = ((Stp_RectWindows->SignalDisplayWindow.left + Stp_RectWindows->SignalDisplayWindow.right) << 2) / 2;
	t_Pixel.Y = ((Stp_RectWindows->SignalDisplayWindow.top + Stp_RectWindows->SignalDisplayWindow.bottom) << 2) / 2;
	for(i = Stp_ScreenRectInput->ActiveRectWindowsNum; i > 0; i--)
	{
		St_TempRect.top = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.top << 2;
		St_TempRect.bottom = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.bottom << 2;
		St_TempRect.left = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.left << 2;
		St_TempRect.right = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.right << 2;
		if(
			(t_Pixel.X <= St_TempRect.right)
			&&
			(t_Pixel.X >= St_TempRect.left)
			&&
			(t_Pixel.Y <= St_TempRect.bottom)
			&&
			(t_Pixel.Y >= St_TempRect.top)
		  )
		{
			tl_ScaleValue = Stp_RectWindows->SignalDisplayWindow.left - Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.left;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_ScreenRectInput->Stp_RectWindows[i - 1]->HozScale;
			Stp_RectWindows->SignalInputWindow.left = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputWindow.left
														+ tl_ScaleValue
														- Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOffsetX;

			tl_ScaleValue = Stp_RectWindows->SignalDisplayWindow.right - Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.left;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_ScreenRectInput->Stp_RectWindows[i - 1]->HozScale;
			Stp_RectWindows->SignalInputWindow.right = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputWindow.left
														+ tl_ScaleValue
														-Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOffsetX;

			tl_ScaleValue = Stp_RectWindows->SignalDisplayWindow.top - Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.top;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_ScreenRectInput->Stp_RectWindows[i - 1]->VertScale;
			Stp_RectWindows->SignalInputWindow.top = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputWindow.top
														+ tl_ScaleValue
														- Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOffsetY;

			tl_ScaleValue = Stp_RectWindows->SignalDisplayWindow.bottom - Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalDisplayWindow.top;
			tl_ScaleValue *= SCALE_COEFFICIENT;
			tl_ScaleValue /= Stp_ScreenRectInput->Stp_RectWindows[i - 1]->VertScale;
			Stp_RectWindows->SignalInputWindow.bottom = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputWindow.top
														+ tl_ScaleValue
														- Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOffsetY;

			Stp_RectWindows->SignalInputOriWidth = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOriWidth;
			Stp_RectWindows->SignalInputOriHeight = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalInputOriHeight;
			Stp_RectWindows->HozScale = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->HozScale;
			Stp_RectWindows->VertScale = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->VertScale;
			Stp_RectWindows->SignalSource = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->SignalSource;
			Stp_RectWindows->Windows_Layer = Stp_ScreenRectInput->Stp_RectWindows[i - 1]->Windows_Layer;
			return 1;
		}
	}
	return 0;
}

//***************************************************************************/
//
// FUNCTION:	BubSort(long* LongArray, int len)
//
// USAGE:		数组排序
//
// INPUT:		LongArray	数组指针
//							len			数组个数
//
// OUTPUT:		排序后有效数据个数
//
//***************************************************************************/
static int BubSort(long* LongArray, int len)
{
	int i, j, flag;
	long	temp;
	for(i = len; i >= 2; i--)
	{
		flag = 0;
		for(j = 0; j <= i - 2; j++)
			if (LongArray[j] > LongArray[j + 1])
			{
				temp = LongArray[j];
				LongArray[j] = LongArray[j + 1];
				LongArray[j + 1] = temp;
				flag = 1;
			}
		if (flag == 0) break;
	}

	flag = len;
	for(i = len - 1; i > 0; i--)
	{
		if(LongArray[i] == LongArray[i - 1])
		{
			for(j = i; j < len; j++)
				LongArray[j - 1] = LongArray[j];
			flag--;
		}
	}
	return flag;
}

static void SignalWindowsCropVariableInit(void)
{
	int i = 0, j = 0;
	memset(&gSt_ScreenRectWindowsOutput, 0, sizeof(gSt_ScreenRectWindowsOutput));
	memset(&gSt_WindowsOutputScreenRowRect, 0, sizeof(gSt_WindowsOutputScreenRowRect));
//	memset(&St_WindowsOutputScreenSignalWindows, 0, sizeof(St_WindowsOutputScreenSignalWindows));
	for(i = 0; i < MaxRectWindowsNum; i++)
	{
		gSt_ScreenRectWindowsOutput.St_ScreenRowRect[i] = &gSt_WindowsOutputScreenRowRect[i];
		for(j = 0; j < MaxRectWindowsNum; j++)
			gSt_WindowsOutputScreenRowRect[i].Stp_RectWindows[j] = gStp_WindowsOutputScreenSignalWindows[i][j];
	}
}

static void ST_Screen_Rect_Output_Init(ST_Screen_Rect_Output* Stp_ScreenRectOutput)
{
	int i = 0;
	Stp_ScreenRectOutput->ActiveRectWindowsRow = 0;
	Stp_ScreenRectOutput->ActiveRectWindowsTotalNum = 0;
	for(i = 0; i < (MaxRectWindowsNum * 2 + 1); i++)
		Stp_ScreenRectOutput->St_ScreenRowRect[i] = NULL;
}

static int OutputScreenSignalWindowsInit(int RowNum, int LineNum)
{
	int i = 0, j = 0, k = 0;
	k = RowNum * LineNum;
	if(k > MAX_SIGNAL_WINDOWS)
		return -1;
	for(i = 0; i < RowNum; i++)
	{
		for(j = 0; j < LineNum; j++)
		{
			k = i * RowNum + j;
			gStp_WindowsOutputScreenSignalWindows[i][j] = &(gSt_WindowsOutputScreenSignalWindows[k]);
		}
	}
	return 1;
}

//****************************************************************************************************************/
//
// FUNCTION:	CombineScreen(ST_Screen_Rect_Output* Stp_ScreenRectOutput)
//
// USAGE:		将相同信号源的矩形合并
//
// INPUT:		ST_Screen_Rect_Output* Stp_ScreenRectOutput
//
// OUTPUT:		int	0	未能成功合并
//										1	成功合并
//
//***************************************************************************************************************/
static int	CombineScreen(ST_Screen_Rect_Output* Stp_ScreenRectOutput)
{
	int i = 0, j = 0, k = 0, m = 0;
	ST_Screen_Rect*	Stp_ScreenRowRect = NULL;
	ST_SignalWindows*	Stp_RectWindows = NULL;
	ST_SignalWindows*	Stp_RectWindowsIndex = NULL;
//	long long	tl_ScaleValue = 0;
//	int ScreenWidth = gl_WallWidth / gl_WallLine;
	int RectWindowsNum = 0;
	m = m;
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
	{
		Stp_ScreenRowRect = Stp_ScreenRectOutput->St_ScreenRowRect[i];
		for(j = 0; j < Stp_ScreenRowRect->ActiveRectWindowsNum - 1; j++)
		{
			Stp_RectWindows = Stp_ScreenRowRect->Stp_RectWindows[j];
			Stp_RectWindowsIndex = Stp_ScreenRowRect->Stp_RectWindows[j + 1];
			RectWindowsNum = Stp_ScreenRowRect->ActiveRectWindowsNum;
			while((Stp_RectWindowsIndex->Windows_Layer == Stp_RectWindows->Windows_Layer)
					&& (Stp_RectWindowsIndex->SignalSource == Stp_RectWindows->SignalSource))
			{
				Stp_RectWindows->SignalDisplayWindow.right = Stp_RectWindowsIndex->SignalDisplayWindow.right;
				Stp_RectWindows->SignalInputWindow.right = Stp_RectWindowsIndex->SignalInputWindow.right;
				Stp_RectWindows->SignalRectDivFlag = Stp_RectWindowsIndex->SignalRectDivFlag;
				for(k = j + 1; k < RectWindowsNum - 1; k++)
					Stp_ScreenRowRect->Stp_RectWindows[k] = Stp_ScreenRowRect->Stp_RectWindows[k + 1];
				k = RectWindowsNum - 1;
				//free(Stp_RectWindowsIndex);
				Stp_ScreenRowRect->Stp_RectWindows[k] = NULL;
				RectWindowsNum--;
				Stp_RectWindows = Stp_ScreenRowRect->Stp_RectWindows[j];
				if((j == RectWindowsNum - 1) || (RectWindowsNum == 1))
					break;
				Stp_RectWindowsIndex = Stp_ScreenRowRect->Stp_RectWindows[j + 1];
			}
			Stp_ScreenRowRect->ActiveRectWindowsNum = RectWindowsNum;
		}
	}
	Stp_ScreenRectOutput->ActiveRectWindowsTotalNum = 0;
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
		Stp_ScreenRectOutput->ActiveRectWindowsTotalNum += Stp_ScreenRectOutput->St_ScreenRowRect[i]->ActiveRectWindowsNum;

	printf("\n\rScreen Rect Total Num: %d\n\r", Stp_ScreenRectOutput->ActiveRectWindowsTotalNum);
	printf("Display Window Rect( Top, Bottom, Left, Right ) Input Window Rect( Top, Bottom, Left, Right ) Layer:\n\r");
	for(i = 0; i < Stp_ScreenRectOutput->ActiveRectWindowsRow; i++)
		for(j = 0; j < Stp_ScreenRectOutput->St_ScreenRowRect[i]->ActiveRectWindowsNum; j++)
			printf("%d, %d, %d, %d, %d, %d, %d, %d, %d,\n", Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalDisplayWindow.top,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalDisplayWindow.bottom,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalDisplayWindow.left,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalDisplayWindow.right,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalInputWindow.top,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalInputWindow.bottom,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalInputWindow.left,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->SignalInputWindow.right,
											Stp_ScreenRectOutput->St_ScreenRowRect[i]->Stp_RectWindows[j]->Windows_Layer);

	return 1;
}
