#ifndef __INPUT_FPGA_H__
#define __INPUT_FPGA_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputFPGA.h                                                   */
/*																			*/
/* USAGE:       Header file for module InputFPGA.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../../MultiSignalBoard/WindowRectDef.h"
#include "../XilinxScaler/yuv422_scaler.h"

typedef struct
{
	unsigned long SCLER_WRITE_DATA;
	unsigned long SCLER_WRITE_CMD;
}	INPUT_FPGA_SCLER_DATA_OPERATE;

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
#define BACK_GROUND_WINDO_X_MAX							4
#define BACK_GROUND_WINDO_Y_MAX							9
#define INPUT_FPGA_DEV		0x8C														// Microchip Input FPGA device address
#define INPUT_FPGA_CHIP_ID			0x00											// Microchip Input FPGA ID
#define INPUT_FPGA_SOFT_RESET		0x01											// Microchip Soft Reset
#define INPUT_FPGA_CHIP_VER			0x02											// Microchip Input FPGA Version
#define INPUT_FPGA_CHIP_Check		0x04											// Microchip Input FPGA Chip check
#define INPUT_FPGA_SPI_CS_SET		0x05											// Microchip Input FPGA SPI CS SET

#define INPUT_FPGA_SET_UPDATA								0x10
#define BACK_GROUND_WINDOW_BASE_ADDRESS			0x1000				// Microchip Input FPGA mpi2ins_seg
#define BACK_GROUND_WINDOW_OFFSET						0x100
#define BACK_GROUND_WINDOW_X_OFFSET					0x04
#define BACK_GROUND_WINDOW_Y_OFFSET					0x10
#define VIP0_INPUT_INFO											0x20
#define VIP0_INPUT_INTERLACE								0x3F
#define INPUT_FPGA_SYSTEM_CVBS_TYPE					0x40						// 0x30
#define INPUT_FPGA_SCALER_IN_CVBS_CENTER_POS					0x41	// 0x31
#define INPUT_FPGA_SCALER_IN_FOUR_CVBS_SEL						0x42	// 0x32
#define INPUT_FPGA_SCALER_DDR_SOURCE				0x80
#define INPUT_FPGA_SCALER_DDR_READ					0x81
#define INPUT_FPGA_SCALER_DDR_SWITCH				0x8A
#define INPUT_FPGA_SCALER_MASK_ENABLE				0x94

#define	INPUT_FPGA_SPI_DATA_REG							0xA0
#define	INPUT_FPGA_SPI_COMMAND_REG					0xA1
#define	INPUT_FPGA_SPI_READ_DATA_REG				0xA2

#define	INPUT_FPGA_SCALER_DATA_REG					0x0104
#define	INPUT_FPGA_SCALER_COMMAND_REG				0x0105
#define	INPUT_FPGA_SCALER_READ_DATA_REG			0x0106
#define INPUT_FPGA_OUTPUT_TOTAL							0x1400
#define INPUT_FPGA_OUTPUT_BLANK							0x1401
#define INPUT_FPGA_OUTPUT_DE_START					0x1402
#define INPUT_FPGA_OUTPUT_LINE							0x1403
#define INPUT_FPGA_OUTPUT_CFG_SWITCH				0x1412
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern long g_TotalPixel, g_TotalLine, g_ScreenOutputWidth, g_ScreenOutputHeight, g_VFreq;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern BYTE InputFPGA_CheckChip(void);
extern void InputFPGA_SoftReset	(WORD W_Data);
extern BYTE InputFPGA_ReadVipStatus(BYTE B_VipIndex, WORD *HTotal, WORD *VTotal, WORD *pWidth, WORD *pHeight);
extern void InputFPGA_ScalerInit(BYTE B_ScalerIndex);
extern void InputFPGA_ScalerInit1(BYTE B_ScalerIndex, BYTE B_Phases);
extern BYTE InputFPGASetBackgroundWindow(BYTE B_BackgroundIndex, BYTE B_WindowXIndex, BYTE B_WindowYIndex, BYTE BmgBaseType, ST_SignalWindows St_SignalWin);
extern BYTE InputFPGAClearBackgroundWindow(BYTE B_BackgroundIndex);
extern BYTE InputFPGASetOutputResolution(int VideoModeVIC);
extern BYTE InputFPGASetCustomOutputResolution(BYTE B_VModeIndex, WORD W_TotalPixel, WORD W_TotalLine, WORD W_HStart, WORD W_VStart, WORD W_ActivePixel, WORD W_ActiveLine);
extern BYTE* InputFPGA_SetCustomEdid(BYTE B_VModeIndex);
extern BYTE InputFPGASetVideoOutputEnalble(WORD W_Data);
extern BYTE InputFPGASetVideoOutputDisalble(WORD W_Data);
extern BYTE InputFPGASetUpdata(void);
extern BYTE InputFPGA_ScalerDDRSwitch(BYTE B_ScalerIndex, BYTE B_Enable);
extern BYTE InputFPGA_ScalerDDRReadSwitch(BYTE B_ScalerIndex, BYTE B_Enable);
extern BYTE InputFPGA_ScalerDDRRead(BYTE B_ScalerIndex, BYTE B_Enable, WORD W_SourceHSize,WORD W_SourceVSize);
extern BYTE InputFPGA_ScalerSetup(BYTE B_ScalerIndex, BYTE B_Source, XScalerAperture *Stp_Aperture);
extern void InputFPGA_ScalerSetUpdata(BYTE B_ScalerIndex);
extern BYTE InputFPGA_ScalerSetNoSignalOutput(BYTE B_ScalerIndex, BYTE B_Enable);
extern BYTE InputFPGA_ReadByte	(WORD W_Addr);
extern BYTE InputFPGA_WriteByte	(WORD W_Addr, BYTE B_Data);
extern WORD InputFPGA_ReadWord	(WORD W_Addr);
extern BYTE InputFPGA_WriteWord	(WORD W_Addr, WORD W_Data);
extern BYTE InputFPGA_ModifyWord(WORD W_Addr, WORD W_Mask, WORD W_Data);
extern BYTE InputFPGA_BitToggle(WORD W_Addr, WORD W_Mask);
extern BYTE* InputFPGA_ReadData	(WORD W_Addr, WORD W_Len);
extern BYTE InputFPGA_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
extern void InputFPGA_PrintScalerVar(BYTE B_ScalerIndex);
extern WORD InputFPGA_ScalerReadWord(BYTE B_ScalerIndex, WORD W_Addr);
extern void InputFPGA_ScalerSetAperture(BYTE B_ScalerIndex, XScalerAperture *Stp_Aperture);
extern WORD InputFPGA_RGBToYUV(WORD W_RGBColor);
extern BYTE InputFPGA_SetTestPatternColor(BYTE B_OutputPort, BYTE B_Enable, WORD W_RGBColor);
#endif
