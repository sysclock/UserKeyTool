#ifndef __INPUT_BOARD_ADV7604_H__
#define __INPUT_BOARD_ADV7604_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardADV7604.h                                              */
/*																			*/
/* USAGE:       Header file for module InputBoardADV7604.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../Device/ADV7604Driver/adv7604.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#define			DEVICE_ADV7604_MAX			2

typedef struct							// Structure for Input Board Chips ADV7604
{
	BYTE B_BoardIndex;						// Board Index for ADV7604
	BYTE B_I2CBusIndex;						// I2C Bus Index for ADV7604
	BYTE B_SubI2CBusIndex;				// Sub I2C Bus Index for ADV7604
	struct adv7604_state St_ADV7604GlobalStatus_t;
} ST_DEVICE_ADV7604;

typedef struct							// Structure for Input Board Chips ADV7604
{
	BYTE B_ActiveChip;						// Active Chip for ADV7604
	ST_DEVICE_ADV7604 St_DeviceADV7604[DEVICE_ADV7604_MAX];
} ST_INPUT_BOARD_ADV7604;

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern ST_INPUT_BOARD_ADV7604 gSt_InputBoardADV7604;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void InitInputBoardADV7604(void);
extern void InputBoardADV7604Task(void);
extern BYTE InputBoardADV7604GetIndexBySoltIndex(BYTE B_SoltIndex);
extern void InputBoardADV7604ProgramEDID(BYTE B_InputSlotIndex, unsigned char EdidMode);
extern void InputBoardADV7604SelectInputPort(BYTE B_InputSlotIndex, BYTE InputPortIndex);
extern void InputBoardADV7604SelectInputTiming(BYTE B_InputSlotIndex, BYTE InputTimingIndex);
extern void InputBoardADV7604PrintLogStatus(BYTE B_InputSlotIndex);
extern int InputBoardADV7604GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight);
extern void InputBoardADV7604SetCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal);
extern void InputBoardADV7604SetDVCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal);
#endif
