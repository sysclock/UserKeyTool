#ifndef SCREEN_WINDOW_LAYOUT_H_
#define SCREEN_WINDOW_LAYOUT_H_
/****************************************************************************************************************/
//
//				Copyright (C) 2000.  TRIOLION INC.
//		All rights reserved.  No part of this program may be reproduced.
//
/*===============================================================================*/
//
// MODULE:      ScreenWindowLayout.h
//
// USAGE:       Header file for module ScreenWindowLayout.c
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
	#include "../MultiSignalBoard/WindowRectDef.h"
#else
	#include "WindowRectDef.h"
#endif

#define	MAX_INPUT_SIGNAL						16
#define	MAX_SCALER_NUM						4

/*****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S						*/
/*****************************************************************************/
typedef struct
{
	unsigned char B_SlotIndex;
	unsigned char B_SignalSource;
	unsigned char B_SignalTYPE;
	unsigned int W_StdHSart;							// Standard HSart in pixel unit
	unsigned int W_StdVSart;							// Standard VSart in line unit
	unsigned int W_StdWidth;							// Standard width in pixel unit
	unsigned int W_StdHeight;							// Standard Height in line unit
}	ST_InputSignal;



typedef struct
{
	unsigned char B_ActiveInputSignalNum;
	ST_InputSignal St_InputSignalSource[MAX_INPUT_SIGNAL];
}	ST_ProcessorInputSignal;




typedef struct
{
	unsigned char B_SignalSource;
	unsigned char B_SignalSourceSlot;
	unsigned char B_Refresh;
	unsigned int InVertSize;
	unsigned int InHoriSize;
	unsigned int InFirstLine;
	unsigned int InLastLine;
	unsigned int InFirstPixel;
	unsigned int InLastPixel;
	unsigned int OutVertSize;
	unsigned int OutHoriSize;
}	ST_ScalerType;





typedef struct
{
	unsigned char B_ActiveScalerNum;
	ST_ScalerType St_Scaler[MAX_SCALER_NUM];
}	ST_ProcessorScaler;




/********************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/*******************************************************************************/
extern ST_Screen_Rect gSt_ScreenRectInput[OUTPUT_BOARD_MAX], *gStp_ScreenRectInput;
extern ST_ProcessorInputSignal gSt_ProcessorInputSignal;
extern ST_ProcessorInputSignal *gStp_ProcessorInputSignal;
extern ST_ProcessorScaler gSt_ProcessorScaler;
extern ST_ProcessorScaler *gStp_ProcessorScaler;
extern long	gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight;

/*****************************************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/*****************************************************************************************************/
extern void ScreenWindowLayoutInit(BYTE B_Index);
extern void ScreenWindowLayoutInputSignalInit(void);
extern void ScreenWindowLayoutTask(void);
extern void ExecuteScreenWindowLayout(void);
extern int GetInputBoardSignalResolution(BYTE B_SlotIndex, BYTE B_SignalIndex, long *XStart, long *YStart, long *pResolutionWidth, long *pResolutionHeight);
extern BYTE GetBoardSlotType(BYTE B_SlotIndex);
extern int GraphicProcessorBoardPortSwitch(BYTE B_InputSlotIndex, BYTE B_InputPortIndex, BYTE B_OutputSlotIndex, BYTE B_OutputPortIndex);


extern  char uart3_send[];
extern  char  Usart3_Rev_Buf[128];
extern  char Usart3_Rev_Idx;
#endif /*SCREEN_WINDOW_LAYOUT_H_*/
