#ifndef __OUTPUT_BOARD_SI_I9136_H__
#define __OUTPUT_BOARD_SI_I9136_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      OutputBoardSiI9136.h                                              */
/*																			*/
/* USAGE:       Header file for module OutputBoardSiI9136.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../Device/SiI9136/SiI9136Api.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#define			DEVICE_SI_I9136_MAX			32

typedef struct							// Structure for Input Board Chips SiI9136
{
	BYTE B_BoardIndex;						// Board Index for SiI9136
	BYTE B_I2CBusIndex;						// I2C Bus Index for SiI9136
	BYTE B_SubI2CBusIndex;				// Sub I2C Bus Index for SiI9136
	SiI9136_GlobalStatus_St St_SiI9136GlobalStatus_t;
} ST_DEVICE_SI_I9136;

typedef struct							// Structure for Input Board Chips SiI9136
{
	BYTE B_ActiveChip;						// Active Chip for SiI9136
	ST_DEVICE_SI_I9136 St_DeviceSiI9136[DEVICE_SI_I9136_MAX];
} ST_OUTPUT_BOARD_SI_I9136;

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern ST_OUTPUT_BOARD_SI_I9136 gSt_OutputBoarSiI9136;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void InitOutputBoardSiI9136(void);
extern void OutputBoardSiI9136Task(void);
#endif
