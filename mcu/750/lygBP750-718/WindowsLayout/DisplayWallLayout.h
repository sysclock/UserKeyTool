#ifndef DISPLAY_WALL_LAYOUT_H_
#define DISPLAY_WALL_LAYOUT_H_
/****************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
/*===============================================================================*/
//
// MODULE:      DisplayWallLayout.h
//
// USAGE:       Header file for module DisplayWallLayout.c
//
// DESIGNER:	HAN LONG
//
/****************************************************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
/*
#ifndef PC_VC_DEBUG
	#define	PC_VC_DEBUG
#endif
*/
#ifndef PC_VC_DEBUG
	#include "../System Code/global.h"
	#include "../GraphicProcessor Board/WindowRectDef.h"
#else
	#include "WindowRectDef.h"
#endif

/*****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S						*/
/*****************************************************************************/

enum SIGNAL_SOURCE_TYPE
{
	SIGNAL_SOURCE_DVI,
	SIGNAL_SOURCE_HDMI,
	SIGNAL_SOURCE_CVBS,
	SIGNAL_SOURCE_CVBS_2,
	SIGNAL_SOURCE_SDI,
	SIGNAL_SOURCE_VGA,
	SIGNAL_SOURCE_NET,
	MAX_SIGNAL_SOURCE_TYPE
};

typedef struct
{
	unsigned char	Windows_Layer;
	unsigned char SignalSource;
	unsigned char SignalSourceType;
	unsigned char	WindowsProperty;
	ST_Rect DisplayWindow;
} DisplayWallWindowStruct;

typedef struct
{
	long DisplayWallRow;
	long DisplayWallLine;
	long DisplayWallWidth;
	long DisplayWallHeight;
	long WindowsTotalNum;
	DisplayWallWindowStruct DisplayWallWindow[MaxDiaplayWallWindowsNum];
} DisplayWallWindowLayoutStruct;

/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/*******************************************************************************/

extern DisplayWallWindowLayoutStruct*	gStp_DisplayWallWindowLayout;

/*****************************************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/*****************************************************************************************************/

extern ST_Screen_Rect_Output* DisplayWallMakeSreenWindows(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout);
extern int DisplayWallSendScreenWindows(ST_Screen_Rect_Output* Stp_ScreenRectOutput,
								int WallRow,
								int WallLine,
								long WallWidth,
								long WallHeight);
extern void DisplayWallWindowLayoutInit(int ModeIndex);
extern void DisplayWallWindowLayoutVariable(void);
extern void ExecuteDisplayWallWindowLayout(void);
extern int RefreshDisplayWallWindowsLayout(DisplayWallWindowLayoutStruct* Stp_DisplayWallWindowLayout, int SignalChangeFlag);
extern void FindSignalSourceActivePos(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_SourceActivePos);
extern void LoadDisplayWallWindowLayout(void);
#endif /*DISPLAY_WALL_LAYOUT_H_*/
