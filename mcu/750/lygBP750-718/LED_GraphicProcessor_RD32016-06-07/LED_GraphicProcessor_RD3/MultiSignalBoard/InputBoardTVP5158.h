#ifndef __INPUT_BOARD_TVP5158_H__
#define __INPUT_BOARD_TVP5158_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardTVP5158.h                                              */
/*																			*/
/* USAGE:       Header file for module InputBoardTVP5158.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../Device/TVP5158/TVP5158.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#ifdef GRAPHIC_PROCESSOR_RD1
	#define			DEVICE_TVP5158_MAX			4
#else
	#define			DEVICE_TVP5158_MAX			1
#endif

#define TVP5158CVBSVideoCaptureWidth 								704
#define TVP5158CVBSVideoNTSCCaptureHeight 					474
#define TVP5158CVBSVideoPALCaptureHeight 						540

typedef enum
{
	CVBS_NTSC,
	CVBS_PAL,
	CVBS_NO_SIGNAL,
} CVBS_TYPE;

typedef struct							// Structure for Input Board Chips TVP5158
{
	BYTE B_BoardIndex;						// Board Index for TVP5158
	BYTE B_I2CBusIndex;						// I2C Bus Index for TVP5158
	BYTE B_SubI2CBusIndex;				// Sub I2C Bus Index for TVP5158
	ST_TVP5158_STATUS St_TVP5158Status;
} ST_DEVICE_TVP5158;

typedef struct							// Structure for Input Board Chips TVP5158
{
	BYTE B_ActiveChip;						// Active Chip for TVP5158
	ST_DEVICE_TVP5158 St_DeviceTVP5158[DEVICE_TVP5158_MAX];
} ST_INPUT_BOARD_TVP5158;

typedef struct							// Structure for Input Board Chips TVP5158
{
	BYTE B_ChipDevIndex;			// Chip index for TVP5158
	BYTE B_ChipDecoderIndex;	// Chip input decoder index for TVP5158
} ST_INPUT_BOARD_TVP5158_SIGNAL_INDEX;
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern ST_INPUT_BOARD_TVP5158 gSt_InputBoardTVP5158;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void InitInputBoardTVP5158(void);
extern void InputBoardTVP5158Task(void);
extern int InputBoardTVP5158GetSignalType(BYTE B_InputIndex);
extern int InputBoardTVP5158GetSignalWidthHeight(BYTE B_InputIndex, long *pXStart, long *pYStart, long *pWidth, long *pHeight);
extern int InputBoardTVP5158SetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
extern int InputBoardTVP5158SetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
extern int InputBoardTVP5158SetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
extern int InputBoardTVP5158SetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel, BYTE B_Value);
extern WORD InputBoardTVP5158GetLuminanceBrightness(BYTE B_InputSlotIndex, BYTE channel);
extern WORD InputBoardTVP5158GetLuminanceContrast(BYTE B_InputSlotIndex, BYTE channel);
extern WORD InputBoardTVP5158GetLuminanceSaturation(BYTE B_InputSlotIndex, BYTE channel);
extern WORD InputBoardTVP5158GetLuminanceHue(BYTE B_InputSlotIndex, BYTE channel);
#endif
