#ifndef __SIGNALWINDOWSCROP_H
#define __SIGNALWINDOWSCROP_H
//******************************************************************************
//
//              Copyright (C) 2002.  TRIOLION INC. CONFIDENTIAL
//      All rights reserved.  No part of this program may be reproduced.
//
//     Triolion Inc.
//
//==============================================================================
//
// MODULE:      SignalWindowsCrop.h
//
// USAGE:       Header file for signale window crop and make definition
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
#include "../MultiSignalBoard/WindowRectDef.h"
#else
	#include "WindowRectDef.h"
#endif
/****************************************************************************/
//	G L O B A L    D E F I N I T I O N S
//***************************************************************************/
#define SCALE_COEFFICIENT	1000000
//#define MAX_RECT_WIDTH		900
//#define MAX_SIGNAL_WINDOWS								81
#define MAX_SIGNAL_WINDOWS								128
#define MAX_DIVISIVE_RECT_WINDOWS_NUM			64

//*****************************************************************************************/
//	G L O B A L    F U N C T I O N    P R O T O T Y P E S
//*****************************************************************************************/
extern void SignalWindowsCropInit(void);
extern ST_Screen_Rect_Output* MakeScreenRect(ST_Screen_Rect* Stp_ScreenRectInput,
										long XPosStart,
										long XPosEnd,
										long YPosStart,
										long YPosEnd);
extern int BubSort(long* LongArray, int len);
//***************************************************************************/
//	G L O B A L    V A R I A B L E S
//***************************************************************************/

#endif
