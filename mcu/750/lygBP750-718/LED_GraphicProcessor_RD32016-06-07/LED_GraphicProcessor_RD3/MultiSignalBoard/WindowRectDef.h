#ifndef __WINDOWRECTDEF_H
#define __WINDOWRECTDEF_H
//******************************************************************************
//
//              Copyright (C) 2002.  TRIOLION INC. CONFIDENTIAL
//      All rights reserved.  No part of this program may be reproduced.
//
//     Triolion Inc.
//
//==============================================================================
//
// MODULE:      WindowRectDef.h
//
// USAGE:       Header file for windows rect definition
//
//******************************************************************************

//******************************************************************************
//  I N C L U D E    F I L E S
//******************************************************************************
/*
#ifndef PC_VC_DEBUG
	#define	PC_VC_DEBUG
#endif
*/
#ifndef PC_VC_DEBUG
#include "global.h"
#else
#define	ERR 0xFFF
#define	INPUT_BOARD_MAX 4
#endif

//***************************************************************************
//	G L O B A L    D E F I N I T I O N S
//**************************************************************************
#define	MaxRectWindowsNum						16
#define	MaxDiaplayWallWindowsNum		8					//16
#define	MaxScalerNum								32

typedef struct
{
	long X;
	long Y;
}	ST_PIXEL;

typedef struct
{
	long left;
	long top;
	long right;
	long bottom;
}	ST_Rect;

typedef struct
{
	long left;
	long top;
	long right;
	long bottom;
	int	Windows_Layer;
}	ST_Rect_Windows;

typedef struct
{
	long HozScale;
	long VertScale;
	unsigned char	Windows_Layer;
	unsigned char SignalSource;
	unsigned char OriginalSignalSource;
	unsigned char SignalSourceType;
	unsigned char	WindowsProperty;
	unsigned char SignalSourceSlot;
}	ST_SignalSource;

typedef struct
{
	ST_SignalSource *Stp_SignalSource;
	ST_Rect SignalInputWindow;
	ST_Rect SignalDisplayWindow;
}	ST_SignalWindows;

typedef struct
{
	unsigned char B_BoardSlotIndex;
	unsigned char B_OutputScreenIndex;
	long DisplayScreenWidth;
	long DisplayScreenHeight;
	int	ActiveRectWindowsNum;
	ST_SignalWindows*	Stp_RectWindows[MaxDiaplayWallWindowsNum];
}	ST_Screen_Rect;
/*
typedef struct
{
	int	ActiveRectWindowsNum;
	ST_SignalWindows*	Stp_RectWindows[MaxRectWindowsNum];
}	ST_Screen_Rect_Output_Board;
*/
typedef struct
{
	unsigned char B_BoardSlotIndex;
	unsigned char B_OutputScreenIndex;
	long DisplayScreenWidth;
	long DisplayScreenHeight;
	int	ActiveRectWindowsNum;
	ST_SignalWindows	Stp_RectWindows[MaxRectWindowsNum];
}	ST_Static_Screen_Rect;

typedef struct
{
	int	ActiveRectWindowsTotalNum;
	int ActiveRectWindowsRow;
	ST_Screen_Rect*	St_ScreenRowRect[MaxRectWindowsNum];
}	ST_Screen_Rect_Output;

typedef struct
{
	ST_SignalSource *Stp_SignalSource;
	unsigned char B_ScalerIndex;
	unsigned char B_ScalerType;
	unsigned char B_BoardIndex;
	unsigned char B_GroundIndex;
	unsigned char B_GroundType;
	ST_Rect SignalInputWindow;
	ST_Rect SignalDisplayWindow;
}	ST_DisplayScaler;

typedef struct
{
	unsigned char	B_InputBoardIndex;
	unsigned char	B_InputBoardInputPortIndex;
	unsigned char	B_InputBoardOutputPortIndex;
	unsigned char	B_InputBoardScalerIndex;
	unsigned char	B_OutputBoardIndex;
	unsigned char	B_OutputBoardInputPortIndex;
	unsigned char	B_OutputBoardOutputPortIndex;
	unsigned char	B_OutputBoardScalerIndex;
}	ST_Signal_Route_Path;
#endif
