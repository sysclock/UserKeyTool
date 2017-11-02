#ifndef __INPUT_BOARD_GV7601_H__
#define __INPUT_BOARD_GV7601_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardGV7601.h                                              */
/*																			*/
/* USAGE:       Header file for module InputBoardGV7601.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../Device/GV7601/gv7601.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#define			DEVICE_GV7601_MAX			5

typedef struct							// Structure for Input Board Chips GV7601
{
	BYTE B_BoardIndex;						// Board Index for GV7601
	BYTE B_I2CBusIndex;						// I2C Bus Index for GV7601
	BYTE B_SubI2CBusIndex;				// Sub I2C Bus Index for GV7601
	GV7601_GlobalStatus_t St_GV7601VideoStatus_t;
} ST_DEVICE_GV7601;

typedef struct							// Structure for Input Board Chips GV7601
{
	BYTE B_ActiveChip;						// Active Chip for GV7601
	ST_DEVICE_GV7601 St_DeviceGV7601[DEVICE_GV7601_MAX];
} ST_INPUT_BOARD_GV7601;

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern ST_INPUT_BOARD_GV7601 gSt_InputBoardGV7601;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void InitInputBoardGV7601(void);
extern void InputBoardGV7601Task(void);
//extern void InputBoardGV7601ProgramEDID(BYTE B_InputIndex);
extern int InputBoardGV7601GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight);
extern int InputBoardGV7601PrintSignalStd(BYTE B_InputIndex);
#endif
