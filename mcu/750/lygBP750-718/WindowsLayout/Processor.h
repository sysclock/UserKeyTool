#ifndef PROCESSOR_SCALER_H_
#define PROCESSOR_SCALER_H_
/****************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
/*===============================================================================*/
//
// MODULE:      ProcessorScaler.h
//
// USAGE:       Header file for module ProcessorScaler.c
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

#define	MaxProcessorLine						12
#define	MaxProcessorRow							10
#define	MaxProcessorScreen					16
#define	MaxScalerOneBoard						4
#define	MAX_INPUT_SIGNAL						16
#define	MAX_INPUT_CVBS_SIGNAL				96
/*****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S						*/
/*****************************************************************************/

typedef enum
{
	INPUT_SCALER,
	OUTPUT_SCALER,
	NO_SCALER_USE,
} SCALER_TYPE;

typedef struct
{
	unsigned char B_OutputPort;
	unsigned char B_FPGAIndex;
	unsigned char B_ScreenPosX;
	unsigned char B_ScreenPosY;
}	ST_DisplayScreen;

typedef struct
{
	unsigned char SignalSource;
	unsigned char SignalSourceType;
	unsigned int W_StdWidth;							// Standard width in pixel unit
	unsigned int W_StdHeight;							// Standard Height in line unit
}	ST_InputSignal;

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
	unsigned char B_ScalerIndex;
	unsigned char B_OutputBoardIndex;
	unsigned char B_OutputBoardInputIndex;
	unsigned char B_InputBoardIndex;
	unsigned char B_InputBoardOutputIndex;
}	ST_OutputBoardGround;

typedef struct
{
	unsigned char B_ProcessorRow;
	unsigned char B_ProcessorLine;
	long ScreenWidth;
	long ScreenHeight;
	unsigned char B_DisplayScreenTotalNum;
	ST_DisplayScreen St_StaticDisplayScreen[MaxProcessorScreen];
	unsigned char B_ScalerTotalNum;
	unsigned char B_ScalerUsed;
	ST_DisplayScaler St_DisplayScaler[MaxScalerNum];
	unsigned char B_ForegroundTotalNum;
	unsigned char B_ForegroundUsed;
	ST_OutputBoardGround St_OutputBoardForeground[OUTPUT_BOARD_MAX * 4];
	unsigned char B_BackgroundTotalNum;
	unsigned char B_BackgroundUsed;
	ST_OutputBoardGround St_OutputBoardBackground[OUTPUT_BOARD_MAX * 4];
	/*
	unsigned char B_ActiveInputSignalNum;
	ST_InputSignal St_InputSignalSource[MAX_INPUT_SIGNAL];
	unsigned char B_ActiveInputCVBSSignalNum;
	unsigned char B_InputCVBSSignalSource[MAX_INPUT_CVBS_SIGNAL];
	*/
}	ST_Processor;

typedef struct
{
	unsigned char B_ActiveInputSignalNum;
	ST_InputSignal St_InputSignalSource[MAX_INPUT_SIGNAL];
	unsigned char B_ActiveInputCVBSSignalNum;
	unsigned char B_InputCVBSSignalSource[MAX_INPUT_CVBS_SIGNAL];
}	ST_ProcessorInputSignal;

/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/*******************************************************************************/
extern ST_Processor gSt_Processor;
extern ST_Processor *gStp_Processor;
extern ST_ProcessorInputSignal gSt_ProcessorInputSignal;
extern ST_ProcessorInputSignal *gStp_ProcessorInputSignal;

/*****************************************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/*****************************************************************************************************/
extern void InitProcessor(void);
extern void ProcessorTask(void);
//extern int AllocDisplayWindowScaler(DisplayWallWindowLayoutStruct*	Stp_DisplayWallWindowLayout, ST_SignalWindows*	Stp_SignalWindows);
extern void SetProcessorScaler(void);
extern void ClearProcessorScaler(void);
extern void PrintProcessorScalerMessage(void);
extern void PrintProcessorGroundMessage(void);
extern int FindBoardIndexBySignal(int SourceType, int SignalSource);
extern int ChangeSignalIndexToBoardSignalIndex(int SourceType, int SignalSource);
extern int FindDisplayScreen(ST_Rect *Stp_Rect);
extern ST_DisplayScaler* FindScalerByDisplayRect(ST_SignalSource *Stp_SignalSource, ST_Rect *Stp_InputRect, ST_Rect *Stp_OutputRect);
extern int FindOutputBoardGroundInputByInputSignal(int InputBoardIndex, int InputIndex, int OutputBoardIndex, int GroundType);
extern int SetProcessorDisplayScreen(int DisplayScreenIndex, int OutputPortIndex, int ScreenPosX, int ScreenPosY);
extern void SaveProcessorSet(void);
extern void LoadProcessorSet(void);

#endif /*PROCESSOR_SCALER_H_*/
