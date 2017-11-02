#ifndef __INPUT_BOARD_GRAPHIC_FPGA_H__
#define __INPUT_BOARD_GRAPHIC_FPGA_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardGraphicFPGA.h                                              */
/*																			*/
/* USAGE:       Header file for module InputBoardGraphicFPGA.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "WindowRectDef.h"
/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#ifdef GRAPHIC_PROCESSOR_RD1
	#define			DEVICE_INPUT_GRAPHIC_FPGA_MAX			4
#else
	#define			DEVICE_INPUT_GRAPHIC_FPGA_MAX			8
#endif
#define	MaxOneInputBoardScalerNum				4
#define	MaxOneInputBoardWindowNum				32

#define			INPUT_BOARD_VDPA											0x56445041
#define			INPUT_BOARD_VDPB											0x56445042
#define			INPUT_BOARD_VDPC											0x56445043
#define			INPUT_BOARD_VDPD											0x56445044
#define			INPUT_BOARD_VDPE											0x56445045
#define			INPUT_BOARD_VDPF											0x56445046
//#define			INPUT_BOARD_VDPA											0x56444953
typedef enum
{
	/*
	INPUT_DEV_SiI9233,
	*/
	INPUT_DEV_TVP5185,
	INPUT_DEV_ADV7604,
	INPUT_DEV_GV7601,
	OUTPUT_DEV_SiI9136,
	INPUT_DEV_IP_DECODER,
	INPUT_DEV_VDPA,
	INPUT_DEV_TVP5185_FOUR,
	INPUT_DEV_MAX,
} INPUT_DEV_TYPE;

typedef struct												// Structure for Input Board Graphic FPGA Interface
{
	BYTE B_VideoInputNum;								// Input Interface Input Video Num
	BYTE B_VideoInputType;							// SiI9233(HDMI) , TVP5158(CVBS) etc.
	BYTE B_Background0GTP;
	BYTE B_Foreground0GTP;
	BYTE B_Background1GTP;
	BYTE B_Foreground1GTP;
	BYTE B_Background2GTP;
	BYTE B_Foreground2GTP;
	BYTE B_Background3GTP;
	BYTE B_Foreground3GTP;
} ST_DEVICE_INPUT_GRAPHIC_FPGA_INTERFACE;

typedef struct							// Structure for Input Board Chips Input Board Graphic FPGA
{
	BYTE B_BoardIndex;						// Board Index for Input Board Graphic FPGA
	BYTE B_I2CBusIndex;						// I2C Bus Index for Input Board Graphic FPGA
	BYTE B_SubI2CBusIndex;				// Sub I2C Bus Index for Input Board Graphic FPGA
	BYTE B_GraphicFPGAType;
	const ST_DEVICE_INPUT_GRAPHIC_FPGA_INTERFACE* Stp_InputFPGAInterface;
//	unsigned char B_ScalerTotalNum;
	unsigned char B_ScalerUsed;
	ST_DisplayScaler St_DisplayScaler[MaxOneInputBoardScalerNum];
	ST_SignalSource St_ScalerSignalSource[MaxOneInputBoardScalerNum];
//	ST_SignalWindows gSt_StaticScreenRectInputSignalWindows[MaxOneInputBoardWindowNum];
//	ST_SignalSource gSt_StaticWindowLayoutSignalSource[MaxOneInputBoardWindowNum];
} ST_DEVICE_INPUT_GRAPHIC_FPGA;

typedef struct							// Structure for Input Board Chips Input Board Graphic FPGA
{
	BYTE B_ActiveChip;						// Active Chip for Input Board Graphic FPGA
	ST_DEVICE_INPUT_GRAPHIC_FPGA St_DeviceInputGraphicFPGA[DEVICE_INPUT_GRAPHIC_FPGA_MAX];
} ST_INPUT_BOARD_GRAPHIC_FPGA;

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern ST_INPUT_BOARD_GRAPHIC_FPGA gSt_InputBoarGraphicFPGA;
extern BYTE gB_CVBSSwitchEnable;
extern BYTE gB_CVBSSwitchIndex;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void InitInputBoardGraphicFPGA(void);
//extern void InputBoardGraphicFPGATask(void);
extern BYTE InputGraphicFPGAChangeOutputTiming(BYTE B_TimingIndex);
extern BYTE InputGraphicFPGAGetSlotBoardType(BYTE B_SlotIndex);
extern BYTE InputGraphicFPGAGetSlotBoardBmgBaseType(BYTE B_SlotIndex);
extern BYTE InputGraphicFPGAFindGTPPortNum(BYTE B_FPGAIndex, BYTE B_GTPIndex);
extern int	InputGraphicFPGAClearScalerUse(BYTE B_SlotIndex);
extern int	InputGraphicFPGAClearOutputWindow(BYTE B_SlotIndex, BYTE B_OutputPortIndex);
extern void InputGraphicFPGAInitAllScaler(void);
extern void InputGraphicFPGASetCVBSCapturePos(int VideoSignalIndex, int VideoType);
#endif
