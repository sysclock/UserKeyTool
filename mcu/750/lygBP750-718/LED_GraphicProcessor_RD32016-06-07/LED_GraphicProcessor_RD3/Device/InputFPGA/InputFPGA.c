#define __INPUT_FPGA_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputFPGA.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip Input FPGA */
/*																																			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ASCII.h"
#include "global.h"
#include "type.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "InputFPGA.h"
#include "EDID.h"

#include "../SiI9136/TypeDefs.h"
#include "../SiI9136/Constants.h"
#include "../SiI9136/VideoModeTable.h"
#include "../XilinxScaler/bin1_tap4.h"

#define INPUT_FPGA_DEBUG								0
#define INPUT_FPGA_COMPANY_NAME					0x4E484456
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
long g_TotalPixel = 2200, g_TotalLine = 1125, g_ScreenOutputWidth = 1920, g_ScreenOutputHeight = 1080, g_VFreq = 0;
extern const INPUT_FPGA_SCLER_DATA_OPERATE gSt_InputFPGAScalerInit[];
BYTE g_InputEdidData[128] = {0};
const BYTE g_StaticInput_EDID_13[128] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x41, 0x0C, 0x7B, 0xC0, 0x64, 0x06, 0x00, 0x00,
	0x17, 0x16, 0x01, 0x03, 0x80, 0x30, 0x1B, 0x78, 0x2A, 0x0A, 0x45, 0xA7, 0x54, 0x4A, 0x99, 0x25,
	0x11, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0, 0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x41, 0x55, 0x33,
	0x31, 0x32, 0x32, 0x33, 0x30, 0x30, 0x31, 0x36, 0x33, 0x36, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x48,
	0x44, 0x4D, 0x49, 0x2D, 0x53, 0x57, 0x49, 0x54, 0x43, 0x48, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
};
/*
const BYTE g_StaticInput_EDID[256] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x41, 0x0C, 0x7B, 0xC0, 0x64, 0x06, 0x00, 0x00,
	0x17, 0x16, 0x01, 0x03, 0x80, 0x30, 0x1B, 0x78, 0x2A, 0x0A, 0x45, 0xA7, 0x54, 0x4A, 0x99, 0x25,
	0x11, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0, 0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x41, 0x55, 0x33,
	0x31, 0x32, 0x32, 0x33, 0x30, 0x30, 0x31, 0x36, 0x33, 0x36, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x48,
	0x44, 0x4D, 0x49, 0x2D, 0x53, 0x57, 0x49, 0x54, 0x43, 0x48, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x4D,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
*/
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S										*/
/****************************************************************************/

BYTE InputFPGA_ReadByte	(WORD W_Addr);
BYTE InputFPGA_WriteByte	(WORD W_Addr, BYTE B_Data);
WORD InputFPGA_ReadWord	(WORD W_Addr);
BYTE InputFPGA_WriteWord	(WORD W_Addr, WORD W_Data);
BYTE* InputFPGA_ReadData	(WORD W_Addr, WORD W_Len);
BYTE InputFPGA_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
BYTE InputFPGA_ModifyWord(WORD W_Addr, WORD W_Mask, WORD W_Data);
BYTE InputFPGA_BitToggle(WORD W_Addr, WORD W_Mask);
void InputFPGA_SoftReset	(WORD W_Data);
BYTE InputFPGA_CheckChip(void);
BYTE InputFPGA_ReadVipStatus(BYTE B_VipIndex, WORD *HTotal, WORD *VTotal, WORD *pWidth, WORD *pHeight);
BYTE InputFPGASetBackgroundWindow(BYTE B_BackgroundIndex, BYTE B_WindowXIndex, BYTE B_WindowYIndex, BYTE BmgBaseType, ST_SignalWindows St_SignalWin);
BYTE InputFPGAClearBackgroundWindow(BYTE B_BackgroundIndex);
BYTE InputFPGASetOutputResolution(int VideoModeVIC);
BYTE InputFPGASetCustomOutputResolution(BYTE B_VModeIndex, WORD W_TotalPixel, WORD W_TotalLine, WORD W_HStart, WORD W_VStart, WORD W_ActivePixel, WORD W_ActiveLine);
BYTE* InputFPGA_SetCustomEdid(BYTE B_VModeIndex);
BYTE InputFPGASetVideoOutputEnalble(WORD W_Data);
BYTE IntputFPGASetVideoOutputDisalble(WORD W_Data);
BYTE InputFPGASetUpdata(void);
BYTE InputFPGA_ScalerDDRSwitch(BYTE B_ScalerIndex, BYTE B_Enable);
BYTE InputFPGA_ScalerDDRReadSwitch(BYTE B_ScalerIndex, BYTE B_Enable);
BYTE InputFPGA_ScalerDDRRead(BYTE B_ScalerIndex, BYTE B_Enable, WORD W_SourceHSize,WORD W_SourceVSize);
BYTE InputFPGA_ScalerSetup(BYTE B_ScalerIndex, BYTE B_Source, XScalerAperture *Stp_Aperture);
void InputFPGA_ScalerSetUpdata(BYTE B_ScalerIndex);
BYTE InputFPGA_ScalerSetNoSignalOutput(BYTE B_ScalerIndex, BYTE B_Enable);
void InputFPGA_ScalerInit(BYTE B_ScalerIndex);
void InputFPGA_ScalerInit1(BYTE B_ScalerIndex, BYTE B_Phases);
void InputFPGA_ScalerWrite(BYTE B_ScalerIndex, INPUT_FPGA_SCLER_DATA_OPERATE *Stp_InputFpgaScalerDataOp);
void InputFPGA_PrintScalerVar(BYTE B_ScalerIndex);
WORD InputFPGA_ScalerReadWord(BYTE B_ScalerIndex, WORD W_Addr);
void InputFPGA_ScalerSetAperture(BYTE B_ScalerIndex, XScalerAperture *Stp_Aperture);
void InputFPGA_ScalerSetCoefficient(BYTE B_ScalerIndex, BYTE B_Phases, WORD W_InputWidth, WORD W_InputHeight, WORD W_OutputWidth, WORD W_OutputHeight);
WORD InputFPGA_RGBToYUV(WORD W_RGBColor);
BYTE InputFPGA_SetTestPatternColor(BYTE B_OutputPort, BYTE B_Enable, WORD W_RGBColor);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
WORD InputFPGABackgroundWindowLength = 0, InputFPGABackgroundWindowDDrBase = 0, InputFPGABackgroundWindowBmgBase = 0;
INPUT_FPGA_SCLER_DATA_OPERATE gSt_InputFpgaScalerDataOp;
WORD InputFPGAScalerHorizontalBin[4], InputFPGAScalerVerticaltBin[4];
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

BYTE InputFPGASetBackgroundWindow(BYTE B_BackgroundIndex, BYTE B_WindowXIndex, BYTE B_WindowYIndex, BYTE BmgBaseType, ST_SignalWindows St_SignalWin)
{
	WORD W_BackgroundWindowAddr = 0, W_RegAddress = 0, W_Data = 0;
	ST_Rect* Stp_TempRect = NULL;
	W_BackgroundWindowAddr = BACK_GROUND_WINDOW_BASE_ADDRESS
														+ B_BackgroundIndex * BACK_GROUND_WINDOW_OFFSET
														+ B_WindowYIndex * BACK_GROUND_WINDOW_Y_OFFSET;
	if(B_WindowXIndex >= BACK_GROUND_WINDO_X_MAX)
	{
		#if(INPUT_FPGA_DEBUG == 1)
		printf("Input FPGA Set Background Window WindowXIndex Overflow!\r\n");
		#endif
		return (BYTE) ERR;
	}
	if(B_WindowYIndex >= BACK_GROUND_WINDO_Y_MAX)
	{
		#if(INPUT_FPGA_DEBUG == 1)
		printf("Input FPGA Set Background Window WindowYIndex Overflow!\r\n");
		#endif
		return (BYTE) ERR;
	}
	Stp_TempRect = &(St_SignalWin.SignalInputWindow);
	Stp_TempRect->left >>= 1;
	Stp_TempRect->left <<= 1;
	Stp_TempRect->right >>= 1;
	Stp_TempRect->right <<= 1;
	Stp_TempRect = &(St_SignalWin.SignalDisplayWindow);
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGA ForegroundIndex = %d, XIndex = %d, YIndex = %d\r\n", B_BackgroundIndex, B_WindowXIndex, B_WindowYIndex);
	printf("Signal = %d, Top = %ld, Bottom = %ld, Left = %ld, Right = %ld\r\n",
						St_SignalWin.Stp_SignalSource->SignalSource,
						Stp_TempRect->top,
						Stp_TempRect->bottom,
						Stp_TempRect->left,
						Stp_TempRect->right);
	#endif
	Stp_TempRect->left >>= 1;
	Stp_TempRect->left <<= 1;
	Stp_TempRect->right >>= 1;
	Stp_TempRect->right <<= 1;
	if(B_WindowXIndex == 0)
	{
		W_RegAddress = W_BackgroundWindowAddr;
		InputFPGA_WriteWord(W_RegAddress, (WORD) Stp_TempRect->bottom);
		#if(INPUT_FPGA_DEBUG == 2)
		printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, (WORD) Stp_TempRect->bottom);
		#endif
	}
	if(St_SignalWin.Stp_SignalSource->SignalSource == 0xFF)
		return OK;
	W_RegAddress = W_BackgroundWindowAddr + B_WindowXIndex + 1;
	InputFPGA_WriteWord(W_RegAddress, (WORD) (Stp_TempRect->right - Stp_TempRect->left));
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, (WORD) (Stp_TempRect->right - Stp_TempRect->left));
	#endif
	W_RegAddress += BACK_GROUND_WINDOW_X_OFFSET;
	InputFPGA_WriteWord(W_RegAddress, (WORD) (Stp_TempRect->left));
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, (WORD) (Stp_TempRect->left));
	#endif

	W_RegAddress += BACK_GROUND_WINDOW_X_OFFSET;
//	Stp_TempRect = &(St_SignalWin.SignalDisplayWindow);
	Stp_TempRect = &(St_SignalWin.SignalInputWindow);
	W_Data = (WORD) ((St_SignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(W_RegAddress, W_Data);
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, W_Data);
	#endif
	if(BmgBaseType == 1)
		InputFPGABackgroundWindowBmgBase += ((InputFPGABackgroundWindowLength + InputFPGABackgroundWindowDDrBase) / 4) + 1;
	else
		InputFPGABackgroundWindowBmgBase += ((InputFPGABackgroundWindowLength + InputFPGABackgroundWindowDDrBase) / 8) + 1;
	switch(B_WindowXIndex)
	{
		case 1:
			W_RegAddress = W_BackgroundWindowAddr + 0x0D;
			InputFPGA_WriteWord(W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#if(INPUT_FPGA_DEBUG == 2)
			printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#endif
			break;
		case 2:
			W_RegAddress = W_BackgroundWindowAddr + 0x0E;
			InputFPGA_WriteWord(W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#if(INPUT_FPGA_DEBUG == 2)
			printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#endif
			break;
		case 3:
			W_RegAddress = W_BackgroundWindowAddr + 0x0F;
			InputFPGA_WriteWord(W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#if(INPUT_FPGA_DEBUG == 2)
			printf("InputFPGASetBackgroundWindow: [0x%08X] = 0x%08X\r\n", W_RegAddress, InputFPGABackgroundWindowBmgBase);
			#endif
			break;
		case 0:
		default:
			InputFPGABackgroundWindowBmgBase = 0;
			break;
	}
	Stp_TempRect = &(St_SignalWin.SignalInputWindow);
	if(BmgBaseType == 1)
		InputFPGABackgroundWindowDDrBase = Stp_TempRect->left & 0x03;
	else
		InputFPGABackgroundWindowDDrBase = Stp_TempRect->left & 0x07;
	Stp_TempRect = &(St_SignalWin.SignalDisplayWindow);
	InputFPGABackgroundWindowLength = Stp_TempRect->right - Stp_TempRect->left;
	return OK;
}

BYTE InputFPGAClearBackgroundWindow(BYTE B_BackgroundIndex)
{
//	BYTE i = 0;
//	WORD TempData = 0; 
	if(B_BackgroundIndex == 0x0F)
	{
		#if 0
		InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0xF0);
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x100);
		for(i = 0; i < 6; i++)
		{
			TempData = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
			if(TempData == 0x100)
				break;
			Delay1ms(5);
		}
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x10F);
		#else
		InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0xF0);
//		InputFPGASetUpdata();
		#endif
	}
	else if(B_BackgroundIndex < BOARD_OUTPUT_PORT_MAX)
	{
		InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, BIT0 << (B_BackgroundIndex + 4));
		#if 0
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x100);
		for(i = 0; i < 6; i++)
		{
			TempData = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
			if(TempData == 0x100)
				break;
			Delay1ms(5);
		}
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x10F);
		#else
//		InputFPGASetUpdata();
		#endif
	}
	else
	{
		#if(INPUT_FPGA_DEBUG == 2)
		printf("InputFPGAClearBackgroundWindow ERROR: B_BackgroundIndex = 0x%d\r\n", B_BackgroundIndex);
		#endif
		return 0xFF;
	}
	return 1;
}

BYTE InputFPGASetOutputResolution(int VideoModeVIC)
{
	int TempVModeInfoIndex = 0;
	WORD W_Data = 0;
	const VModeInfoType* Stp_TempVModeInfo = NULL;
	TempVModeInfoIndex = ConvertVIC_To_VM_IndexByParam(VideoModeVIC);
//	printf("InputFPGASetOutputResolution: VideoModeVIC = %d, TempVModeInfoIndex = %d\r\n", VideoModeVIC, TempVModeInfoIndex);
	Stp_TempVModeInfo = &(VModesTable[TempVModeInfoIndex]);
	W_Data = ((Stp_TempVModeInfo->Tag.Total.Lines - 1) << 16) + Stp_TempVModeInfo->Tag.Total.Pixels - 1;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_TOTAL, W_Data);
	W_Data = (Stp_TempVModeInfo->_656.HBit2HSync << 24)
						+ (Stp_TempVModeInfo->_656.HLength << 16)
						+ (Stp_TempVModeInfo->_656.VBit2VSync << 8)
						+ Stp_TempVModeInfo->_656.VBit2VSync + Stp_TempVModeInfo->_656.VLength;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_BLANK, W_Data);
	W_Data = Stp_TempVModeInfo->Pos.H + Stp_TempVModeInfo->_656.HBit2HSync;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_DE_START, W_Data);
	W_Data = Stp_TempVModeInfo->Res.V - 1;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_LINE, W_Data);
	g_TotalPixel = Stp_TempVModeInfo->Tag.Total.Pixels;
	g_TotalLine = Stp_TempVModeInfo->Tag.Total.Lines;
	g_ScreenOutputWidth = Stp_TempVModeInfo->Res.H;
	g_ScreenOutputHeight = Stp_TempVModeInfo->Res.V;
	g_VFreq = Stp_TempVModeInfo->Tag.VFreq;
	return OK;
}

BYTE InputFPGASetCustomOutputResolution(BYTE B_VModeIndex, WORD W_TotalPixel, WORD W_TotalLine, WORD W_HStart, WORD W_VStart, WORD W_ActivePixel, WORD W_ActiveLine)
{
//	BYTE HShift = 0, VShift = 0;
	int TempVModeInfoIndex = 0;
	WORD W_Data = 0;
	long long L_Data = 0;
	const VModeInfoType* Stp_TempVModeInfo = NULL;

	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0);
	#if 1		
	if(B_VModeIndex != 43)
		TempVModeInfoIndex = ConvertVIC_To_VM_IndexByParam(B_VModeIndex);
	else
		TempVModeInfoIndex = B_VModeIndex;
	#else
	TempVModeInfoIndex = B_VModeIndex;
	if(TempVModeInfoIndex > (PC_BASE + 50))
		TempVModeInfoIndex = 11; 
	#endif

	Stp_TempVModeInfo = &(VModesTable[TempVModeInfoIndex]);
	W_Data = ((W_TotalLine - 1) << 16) + W_TotalPixel - 1;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_TOTAL, W_Data);
	W_Data = ((Stp_TempVModeInfo->_656.HBit2HSync - W_HStart) << 24)
						+ (Stp_TempVModeInfo->_656.HLength << 16)
						+ ((Stp_TempVModeInfo->_656.VBit2VSync - W_VStart) << 8)
						+ (Stp_TempVModeInfo->_656.VBit2VSync - W_VStart) + Stp_TempVModeInfo->_656.VLength;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_BLANK, W_Data);
	W_Data = W_TotalPixel - W_ActivePixel;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_DE_START, W_Data);
	W_Data = W_ActiveLine - 1;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_LINE, W_Data);

	InputFPGASetVideoOutputEnalble(0x100);
	InputFPGA_SoftReset(0xF0 | BIT0);
	InputFPGASetUpdata();
	if(W_ActiveLine > 1080)
		InputFPGA_WriteWord(0x8B, 2273);
	else
		InputFPGA_WriteWord(0x8B, 1650);
	g_TotalPixel = W_TotalPixel;
	g_TotalLine = W_TotalLine;
	g_ScreenOutputWidth = W_ActivePixel;
	g_ScreenOutputHeight = W_ActiveLine;
	if(B_VModeIndex != 43)
		L_Data = Stp_TempVModeInfo->PixClk;
	else
		L_Data = 14850;
	L_Data *= 1000000;
	L_Data /= W_TotalPixel;
	L_Data /= W_TotalLine;
	g_VFreq = L_Data;
	return OK;
}

BYTE* InputFPGA_SetCustomEdid(BYTE B_VModeIndex)
{
	int TempVModeInfoIndex = 0, i = 0;
	WORD W_Data = 0;
	BYTE *pData = NULL;
	unsigned char B_CheckSum = 0;
	const VModeInfoType* Stp_TempVModeInfo = NULL;
	EDID_13* Stp_TempEdid13 = NULL;
	/*
	if(B_VModeIndex != 43)
		TempVModeInfoIndex = ConvertVIC_To_VM_IndexByParam(B_VModeIndex);
	else
		TempVModeInfoIndex = B_VModeIndex;
	*/
	TempVModeInfoIndex = B_VModeIndex;
	if(TempVModeInfoIndex > (PC_BASE + 50))
		TempVModeInfoIndex = 11; 
	Stp_TempVModeInfo = &(VModesTable[TempVModeInfoIndex]);
	memcpy(g_InputEdidData, g_StaticInput_EDID_13, 128);
	Stp_TempEdid13 = (EDID_13*) g_InputEdidData;

	pData = (BYTE*) &Stp_TempEdid13->Detail[0];
	// Pixel clock / 10,000. Stored LSB first. Example: 135MHz would be 13500 decimal, stored  as BCh, 34h 
	*pData = Stp_TempVModeInfo->PixClk;
	pData++;
	*pData = (Stp_TempVModeInfo->PixClk >> 8);
	pData++;
	// Horizontal Active (HA) Pixels, lower 8 bits of HA.
	*pData = Stp_TempVModeInfo->Res.H;
	pData++;
	
	// Horizontal Blanking
	W_Data = Stp_TempVModeInfo->Tag.Total.Pixels - Stp_TempVModeInfo->Res.H;
	// Horizontal Blanking (HBL) Pixels, lower 8 bits of HBL.
	*pData = W_Data;
	pData++;
	// Horizontal Active (HA): Upper nibble : upper 4 bits of HA.
	// Horizontal Blanking (HBL) Lower nibble : upper 4 bits of HBL.
	*pData = ((Stp_TempVModeInfo->Res.H >> 8) & 0x0F) << 4;
	*pData += (W_Data >> 8) & 0x0F;
	pData++;
	// Vertical Active (VA) Lines, lower 8 bits of VA
	*pData = Stp_TempVModeInfo->Res.V;
	pData++;
	
	// Vertical Blanking
	W_Data = Stp_TempVModeInfo->Tag.Total.Lines - Stp_TempVModeInfo->Res.V;
	// Vertical Blanking (VBL) Lines, lower 8 bits of VBL
	*pData = W_Data;
	pData++;
	// Vertical Active (VA): Upper nibble : upper 4 bits of VA.
	// Vertical Blanking (VBL):  Lower nibble upper 4 bits of VBL.
	*pData = ((Stp_TempVModeInfo->Res.V >> 8) & 0x0F) << 4;
	*pData += (W_Data >> 8) & 0x0F;
	pData++;
	
	// Horizontal Sync. Offset (HSO) Pixels , from blanking starts, lower 8 bits
	*pData = Stp_TempVModeInfo->_656.HLength;
	pData++;
	// Horizontal Sync Pulse Width (HSPW): Pixels, lower 8 bits of HSPW 
	*pData = Stp_TempVModeInfo->_656.HBit2HSync;
	pData++;
	// Vertical Sync Offset (VSO): Upper nibble : lines, lower 4 bits of VSO
	// Vertical Sync Pulse Width (VSPW): Lower nibble : lines, lower 4 bits of VSPW
	*pData = (Stp_TempVModeInfo->_656.VLength & 0x0F) << 4;
	*pData += Stp_TempVModeInfo->_656.VBit2VSync & 0x0F;
	pData++;
	// Horizontal Sync Offset (HSO): bits 7,6 : upper 2 bits of HSO 
	// Horizontal Sync Pulse Width (HSPW): bits 5,4 : upper 2 bits of HSPW
	// Vertical Sync Offset (VSO): bits 3,2 : upper 2 bits of VSO
	// Vertical Sync Pulse Width (VSPW): bits 1,0 : upper 2 bits of VSPW
	*pData = ((Stp_TempVModeInfo->_656.HLength >> 8) & 0x03) << 6;
	*pData = ((Stp_TempVModeInfo->_656.HBit2HSync >> 8) & 0x03) << 4;
	*pData = ((Stp_TempVModeInfo->_656.VLength >> 8) & 0x03) << 2;
	*pData = (Stp_TempVModeInfo->_656.VBit2VSync >> 8) & 0x03;
	pData++;
	// Horizontal Image Size (HIS). (dan wei) mm, lower 8 bits 
//	*pData = Stp_TempVModeInfo->Tag.Total.Pixels;
	pData++;
	// Vertical Image Size (VIS). (dan wei) mm, lower 8 bits 
//	*pData = Stp_TempVModeInfo->Tag.Total.Lines;
	pData++;
	// Horizontal Image Size (HIS). Upper nibble : upper 4 bits of HIS
	// Vertical Image Size (VIS). Lower nibble : upper 4 bits of VIS
//	*pData = ((Stp_TempVModeInfo->Tag.Total.Pixels >> 8) & 0x0F) << 4;
//	*pData += (Stp_TempVModeInfo->Tag.Total.Lines >> 8) & 0x0F;
	pData++;
	// Horizontal Border
	*pData = 0;
	pData++;
	// Vertical Border
	*pData = 0;
	pData++;

	// Flags. Interlace, Stereo, Horizontal polarity, Vertical polarity, Sync Configuration, etc.
	// Not Set, use ori
	for(i = 0; i < 127; i++)
		B_CheckSum += g_InputEdidData[i];
	g_InputEdidData[127] = (~B_CheckSum) + 1;
	
	return g_InputEdidData;
}

WORD InputFPGA_RGBToYUV(WORD W_RGBColor)
{
	long long TempData = 0;
	int W_Data = 0;
	WORD W_Ret = 0, W_TempData = 0;
	BYTE B_TempR = 0, B_TempG = 0, B_TempB = 0, B_TempY = 0, B_TempU = 0, B_TempV = 0;
	B_TempR = (BYTE) (W_RGBColor >> 16);
	B_TempG = (BYTE) (W_RGBColor >> 8);
	B_TempB = (BYTE) (W_RGBColor);
	// Y = 0.257*R + 0.504*G + 0.098*B + 16
	TempData = 2570;
	TempData *= B_TempR;
	W_Data = TempData;
	TempData = 5040;
	TempData *= B_TempG;
	W_Data += TempData;
	TempData = 980;
	TempData *= B_TempB;
	W_Data += TempData;
	W_Data /= 10000;
	B_TempY = (BYTE) W_Data;
	B_TempY += 16;
	// U = -0.148*R - 0.291*G + 0.439*B + 128
	TempData = 1280000;
	W_Data = TempData;
	TempData = 1480;
	TempData *= B_TempR;
	W_Data -= TempData;
	TempData = 2910;
	TempData *= B_TempG;
	W_Data -= TempData;
	TempData = 4390;
	TempData *= B_TempB;
	W_Data += TempData;
	W_Data /= 10000;
	B_TempU = (BYTE) W_Data;
	// V = 0.439*R - 0.368*G - 0.071*B + 128
	TempData = 1280000;
	W_Data = TempData;
	TempData = 4390;
	TempData *= B_TempR;
	W_Data += TempData;
	TempData = 3680;
	TempData *= B_TempG;
	W_Data -= TempData;
	TempData = 710;
	TempData *= B_TempB;
	W_Data -= TempData;
	W_Data /= 10000;
	B_TempV = (BYTE) W_Data;
	
	W_TempData = B_TempU;
	W_TempData <<= 24;
	W_Ret += W_TempData;
	W_TempData = B_TempY;
	W_TempData <<= 16;
	W_Ret += W_TempData;
	W_TempData = B_TempV;
	W_TempData <<= 8;
	W_Ret += W_TempData;
	W_TempData = B_TempY;
	W_Ret += W_TempData;
//	printf("InputFPGA_RGBToYUV: W_RGBColor = 0x%08X, W_Ret = 0x%08X\r\n", W_RGBColor, W_Ret);
	return W_Ret;
}

BYTE InputFPGA_SetTestPatternColor(BYTE B_OutputPort, BYTE B_Enable, WORD W_RGBColor)
{
	WORD W_TempData = 0;
	if(B_OutputPort == 0)
	{
		if(B_Enable == 1)
		{
			W_TempData = InputFPGA_RGBToYUV(W_RGBColor);
			InputFPGA_WriteWord(0x1430, W_TempData);
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData |= BIT0;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
		else
		{
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData &= ~BIT0;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
	}
	else if(B_OutputPort == 1)
	{
		if(B_Enable == 1)
		{
			W_TempData = InputFPGA_RGBToYUV(W_RGBColor);
			InputFPGA_WriteWord(0x1431, W_TempData);
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData |= BIT1;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
		else
		{
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData &= ~BIT1;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
	}
	else if(B_OutputPort == 2)
	{
		if(B_Enable == 1)
		{
			W_TempData = InputFPGA_RGBToYUV(W_RGBColor);
			InputFPGA_WriteWord(0x1432, W_TempData);
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData |= BIT2;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
		else
		{
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData &= ~BIT2;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
	}
	else if(B_OutputPort == 3)
	{
		if(B_Enable == 1)
		{
			W_TempData = InputFPGA_RGBToYUV(W_RGBColor);
			InputFPGA_WriteWord(0x1433, W_TempData);
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData |= BIT3;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
		else
		{
			W_TempData = InputFPGA_ReadWord(0x1434) & 0x0F;
			W_TempData &= ~BIT3;
			InputFPGA_WriteWord(0x1434, W_TempData);
		}
	}
	else
		return 0xFF;
	return 0;
}

BYTE InputFPGASetVideoOutputEnalble(WORD W_Data)
{
	WORD W_TempData = 0; //, W_Result = 0, i = 0;
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
	W_TempData |= (W_Data & 0x10F);
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGASetVideoOutputEnalble: W_Data = 0x%02X, W_TempData = 0x%02X\r\n", W_Data, W_TempData);
	#endif
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, W_TempData);
	/*
	W_Result = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
	i = 0;
	while(W_Result != W_TempData)
	{
		Delay10ms(1);
		if(i >= 5)
		{
			#if(INPUT_FPGA_DEBUG == 2)
			printf("Input FPGA Set Video Output Enalble Error. W_Result = 0x%08X, W_TempData = 0x%08X\r\n", W_Result, W_TempData);
			#endif
			return 0;
		}
		i++;
		W_Result = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
	}
	*/
	return 1;
}

BYTE InputFPGASetVideoOutputDisalble(WORD W_Data)
{
	WORD W_TempData = 0; //, W_Result = 0, i = 0;
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
	W_TempData ^= (W_Data & 0x10F);
	W_TempData &= 0x10F;
	#if(INPUT_FPGA_DEBUG == 2)
	printf("InputFPGASetVideoOutputDisalble: W_Data = 0x%02X, W_TempData = 0x%02X\r\n", W_Data, W_TempData);
	#endif
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, W_TempData);
	/*
	W_Result = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
	if(W_Result == 0xFFFFFFFF)
		return 0;
	i = 0;
	while(W_Result != W_TempData)
	{
		Delay10ms(1);
		if(i >= 1670)
		{
			printf("Input FPGA Set Video Output Disalble Error. W_Result = 0x%08X, W_TempData = 0x%08X\r\n", W_Result, W_TempData);
			return 0;
		}
		i++;
		W_Result = InputFPGA_ReadWord(INPUT_FPGA_OUTPUT_CFG_SWITCH);
		if(W_Result == 0xFFFFFFFF)
			return 0;
	}
	*/
	return 1;
}

BYTE InputFPGA_CheckChip(void)
{
	WORD W_TempData = 0, i = 0;
//	W_TempData = InputFPGA_ReadWord(0);
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_CHIP_Check);
	while(W_TempData != INPUT_FPGA_COMPANY_NAME)
	{
		Delay100ms(1);
		W_TempData = InputFPGA_ReadWord(INPUT_FPGA_CHIP_Check);
		i++;
		if(i >= 100)
			return 0;
	}
	return 1;
}

BYTE InputFPGA_ReadVipStatus(BYTE B_VipIndex, WORD *HTotal, WORD *VTotal, WORD *pWidth, WORD *pHeight)
{
	WORD W_TempData = 0, W_TempData1 = 0, i = 0, W_TempAdd = 0;
	W_TempAdd = VIP0_INPUT_INFO + (B_VipIndex << 2);
	W_TempData = InputFPGA_ReadWord(W_TempAdd + 2);
	Delay1ms(3);
	W_TempData1 = InputFPGA_ReadWord(W_TempAdd + 2);
	if(W_TempData1 != W_TempData)
	{
		W_TempData = InputFPGA_ReadWord(VIP0_INPUT_INTERLACE);
		W_TempData1 = InputFPGA_ReadWord(W_TempAdd);
		*HTotal = W_TempData1 & 0xFFFF;
		if(W_TempData & (BIT0 << B_VipIndex))
		{
			*VTotal = ((W_TempData1 >> 16) & 0xFFFF) << 1;
			i = 1;
		}
		else
		{
			*VTotal = (W_TempData1 >> 16) & 0xFFFF;
			i = 0;
		}
		W_TempData1 = InputFPGA_ReadWord(W_TempAdd + 1);
		*pWidth = W_TempData1 & 0xFFFF;
		if(W_TempData & (BIT0 << B_VipIndex))
			*pHeight = ((W_TempData1 >> 16) & 0xFFFF) << 1;
		else
			*pHeight = (W_TempData1 >> 16) & 0xFFFF;
	}
	else
	{
		*HTotal = 0;
		*VTotal = 0;
		*pWidth = 0;
		*pHeight = 0;
		i = 0;
	}
	return i;
}

void InputFPGA_SoftReset(WORD W_Data)
{
	InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, W_Data);
	Delay1ms(1);
	if(W_Data & BIT0)
		InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0x00);
}

BYTE InputFPGASetUpdata(void)
{
	InputFPGA_WriteWord(INPUT_FPGA_SET_UPDATA, BIT0);
	InputFPGA_WriteWord(INPUT_FPGA_SET_UPDATA, 0);
	return 1;
}

BYTE InputFPGA_ScalerDDRSwitch(BYTE B_ScalerIndex, BYTE B_Enable)
{
	WORD W_TempData = 0, i = 0;
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_SCALER_DDR_SWITCH);
	if(B_Enable)
		W_TempData |= BIT0 << B_ScalerIndex;
	else
		W_TempData &= ~(BIT0 << B_ScalerIndex);
	i = InputFPGA_WriteWord(INPUT_FPGA_SCALER_DDR_SWITCH, W_TempData);
	return i;
}

BYTE InputFPGA_ScalerDDRReadSwitch(BYTE B_ScalerIndex, BYTE B_Enable)
{
	WORD W_TempData = 0, W_TempAddr = 0, i = 0;
	W_TempAddr = INPUT_FPGA_SCALER_DDR_READ + (B_ScalerIndex << 1);
	W_TempData = InputFPGA_ReadWord(W_TempAddr);
	if(B_Enable)
		W_TempData |= BIT24;
	else
		W_TempData &= ~BIT24;
	i = InputFPGA_WriteWord(W_TempAddr, W_TempData);
	return i;
}

BYTE InputFPGA_ScalerDDRRead(BYTE B_ScalerIndex, BYTE B_Enable, WORD W_SourceHSize, WORD W_SourceVSize)
{
	WORD W_TempData = 0, W_TempAddr = 0, i = 0;
	W_TempAddr = INPUT_FPGA_SCALER_DDR_READ + (B_ScalerIndex << 1);
	W_TempData = 0;
	if(B_Enable)
		W_TempData |= BIT24;
	W_TempData += (W_SourceHSize << 12) + W_SourceVSize;
	i = InputFPGA_WriteWord(W_TempAddr, W_TempData);
	return i;
}

BYTE InputFPGA_ScalerSetup(BYTE B_ScalerIndex, BYTE B_Source, XScalerAperture *Stp_Aperture)
{
	WORD W_TempData = 0, W_TempAddr = 0, W_TempPixelNum = 0, W_TempLineNum = 0, W_TempInFirstPixel = 0, W_TempInFirstLine = 0;
	#if(INPUT_FPGA_DEBUG > 0)
	printf("Input Scaler Index 0x%02X Aperture:\r\n", B_ScalerIndex);
	printf("Input Scaler InputSource 0x%02X:\r\n", B_Source);
	printf("InVertSize = %d\r\n", Stp_Aperture->InVertSize);
	printf("InHoriSize = %d\r\n", Stp_Aperture->InHoriSize);
	printf("InFirstLine = %d\r\n", Stp_Aperture->InFirstLine);
	printf("InLastLine = %d\r\n", Stp_Aperture->InLastLine);
	printf("InFirstPixel = %d\r\n", Stp_Aperture->InFirstPixel);
	printf("InLastPixel = %d\r\n", Stp_Aperture->InLastPixel);
	printf("OutVertSize = %d\r\n", Stp_Aperture->OutVertSize);
	printf("OutHoriSize = %d\r\n", Stp_Aperture->OutHoriSize);
	#endif
	W_TempInFirstPixel = Stp_Aperture->InFirstPixel;
	W_TempInFirstLine = Stp_Aperture->InFirstLine;
	W_TempPixelNum = Stp_Aperture->InLastPixel - Stp_Aperture->InFirstPixel + 1;
	W_TempLineNum = Stp_Aperture->InLastLine - Stp_Aperture->InFirstLine + 1;
	if((W_TempPixelNum < 2) || (W_TempLineNum < 2))
		return 0xFF;
	#if 0
	InputFPGA_ScalerDDRSwitch(B_ScalerIndex, 0);
	#endif
	InputFPGA_ScalerDDRReadSwitch(B_ScalerIndex, 0);

//	reset_scaler_axi4_inp(B_ScalerIndex);
//	reset_scaler_inp(B_ScalerIndex);
	Stp_Aperture->InVertSize = W_TempLineNum;
	Stp_Aperture->InHoriSize = W_TempPixelNum;
	Stp_Aperture->InFirstLine = 0;
	Stp_Aperture->InLastLine = W_TempLineNum - 1;
	Stp_Aperture->InFirstPixel = 0;
	Stp_Aperture->InLastPixel = W_TempPixelNum - 1;

//	ScalerSetup_inp(B_ScalerIndex, Stp_Aperture);

	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = BIT0;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	
	InputFPGA_ScalerSetCoefficient(B_ScalerIndex, 16,
																	Stp_Aperture->InLastPixel - Stp_Aperture->InFirstPixel + 1,
																	Stp_Aperture->InLastLine - Stp_Aperture->InFirstLine + 1,
																	Stp_Aperture->OutHoriSize,
																	Stp_Aperture->OutVertSize);

	InputFPGA_ScalerSetAperture(B_ScalerIndex, Stp_Aperture);
	W_TempAddr = INPUT_FPGA_SCALER_DDR_SOURCE + (B_ScalerIndex << 1);
	W_TempData = (B_Source << 24) + (W_TempInFirstLine << 12) + (W_TempInFirstPixel >> 1);
	InputFPGA_WriteWord(W_TempAddr, W_TempData);
//	printf("InputFPGA_ScalerSetup: 0x%08X = 0x%08X\r\n", W_TempAddr, W_TempData);
/*
	InputFPGA_ScalerDDRRead(B_ScalerIndex,
													1,
													Stp_Aperture->InHoriSize,
													Stp_Aperture->InVertSize);
*/
	InputFPGA_ScalerDDRRead(B_ScalerIndex,
													0,
													W_TempPixelNum,
													W_TempLineNum);
	#if 0
	W_TempData = InputFPGA_ScalerDDRSwitch(B_ScalerIndex, 1);
	#endif
	W_TempData = InputFPGA_ReadWord(0x8B);
	if((Stp_Aperture->OutHoriSize > 1080) || (W_TempData >= 2273))
		W_TempData = InputFPGA_WriteWord(0x8B, 2273);
	else
		W_TempData = InputFPGA_WriteWord(0x8B, 1650);
	return W_TempData;
}

BYTE InputFPGA_ScalerSetNoSignalOutput(BYTE B_ScalerIndex, BYTE B_Enable)
{
	#if 1
	WORD W_TempData = 0, i = 0;
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_SCALER_MASK_ENABLE);
	if(B_Enable)
		W_TempData |= BIT0 << B_ScalerIndex;
	else
		W_TempData &= ~(BIT0 << B_ScalerIndex);
	i = InputFPGA_WriteWord(INPUT_FPGA_SCALER_MASK_ENABLE, W_TempData & 0x0F);
	return i;
	#else
	return 1;
	#endif
}

void InputFPGA_PrintScalerVar(BYTE B_ScalerIndex)
{
	WORD W_TempData = 0, W_TempAddr = 0;
	W_TempAddr = 0x000;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x004;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x008;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x00C;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x010;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x100;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x104;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x108;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x10C;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x110;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x114;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x118;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x11C;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x120;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x124;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x128;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x12C;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x130;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x134;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x138;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
	W_TempAddr = 0x13C;
	W_TempData = InputFPGA_ScalerReadWord(B_ScalerIndex, W_TempAddr);
	printf("[0x%04X] = 0x%08X\r\n", W_TempAddr, W_TempData);
}

WORD InputFPGA_ScalerReadWord(BYTE B_ScalerIndex, WORD W_Addr)
{
	WORD W_TempData = 0, W_TempAddr = 0, i = 0;
	W_TempAddr = ((W_Addr & 0x01FF) << 8) + ((B_ScalerIndex & 0x03) << 2) + 0xF1;
	InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, W_TempAddr);
	i = 0;
	W_TempData = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
	while(((W_TempData & BIT25) == 0) && (i < 100))
	{
		Delay1ms(1);
		W_TempData = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
		i++;
	}
	if((W_TempData & BIT25) == 0)
	{
		#if(INPUT_FPGA_DEBUG == 1)
		printf("Scaler %d Read Word Error!\r\n", B_ScalerIndex);
		#endif
		return	ERR;
	}
	else
	{
		W_TempData = InputFPGA_ReadWord(INPUT_FPGA_SCALER_READ_DATA_REG);
		#if(INPUT_FPGA_DEBUG == 1)
		printf("Scaler %d Read [0x%04X] = 0x%08X\r\n", B_ScalerIndex, W_Addr, W_TempData); 
		#endif
		return W_TempData;
	}
}

void InputFPGA_ScalerSetAperture(BYTE B_ScalerIndex, XScalerAperture *Stp_Aperture)
{
	long long	tl_ScaleValue = 0;
//	INPUT_FPGA_SCLER_DATA_OPERATE gSt_InputFpgaScalerDataOp;
	#if 0
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	#endif
	tl_ScaleValue = 1 << 20;
	tl_ScaleValue *= Stp_Aperture->InLastPixel + 1 - Stp_Aperture->InFirstPixel;
	tl_ScaleValue /= Stp_Aperture->OutHoriSize;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (unsigned long) tl_ScaleValue;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x100;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	tl_ScaleValue = 1 << 20;
	tl_ScaleValue *= Stp_Aperture->InLastLine + 1 - Stp_Aperture->InFirstLine;
	tl_ScaleValue /= Stp_Aperture->OutVertSize;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (unsigned long) tl_ScaleValue;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x104;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (Stp_Aperture->InHoriSize & 0x1FFF) + ((Stp_Aperture->InVertSize & 0x1FFF) << 16);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x108;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (Stp_Aperture->InFirstPixel & 0x1FFF) + ((Stp_Aperture->InLastPixel & 0x1FFF) << 16);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x10C;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (Stp_Aperture->InFirstLine & 0x1FFF) + ((Stp_Aperture->InLastLine & 0x1FFF) << 16);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x110;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = (Stp_Aperture->OutHoriSize & 0x1FFF) + ((Stp_Aperture->OutVertSize & 0x1FFF) << 16);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x114;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);

	#if 0
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0x02;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	#endif
	#if 0
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0x03;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	#endif
}

void InputFPGA_ScalerSetUpdata(BYTE B_ScalerIndex)
{
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0x03;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
}

void InputFPGA_ScalerInit(BYTE B_ScalerIndex)
{
	WORD W_TempData = 0, i = 0, j = 0;
	const INPUT_FPGA_SCLER_DATA_OPERATE *Stp_TempInputFpgaScalerDataOp = NULL;
	InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, BIT20 << B_ScalerIndex);
//	Delay1ms(5);
	InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, 0);
//	Delay1ms(5);
	Stp_TempInputFpgaScalerDataOp = &(gSt_InputFPGAScalerInit[0]);
	while(Stp_TempInputFpgaScalerDataOp != NULL)
	{
		if((Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_CMD == 0x00FFFF) && (Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_DATA == 0x00FFFF))
			return;
		if((Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_CMD == 0) && (Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_DATA == 0x80000000))
			Delay1ms(5);
		if((Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_CMD == 0) && (Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_DATA == 0))
			Delay1ms(5);
		InputFPGA_WriteWord(INPUT_FPGA_SCALER_DATA_REG, Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_DATA);
		W_TempData = 0xF3 + (B_ScalerIndex * 4) + ((Stp_TempInputFpgaScalerDataOp->SCLER_WRITE_CMD & 0xFFF) << 8);
		InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, W_TempData);
		i = 0;
		j = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
		while(((j & BIT24) == 0) && (i < 100))
		{
			Delay1ms(1);
			j = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
			i++;
		}
		Stp_TempInputFpgaScalerDataOp++;
	}
}

void InputFPGA_ScalerInit1(BYTE B_ScalerIndex, BYTE B_Phases)
{
	/*
	int i = 0, j = 0;
	printf("const short XScaler_Coefficients[17][540] =\r\n");
	printf("{\r\n");
	for(i = 0; i < 17; i++)
	{
		printf("		{\r\n");
		for(j = 0; j < 540; j += 4)
		{
			printf("		%d, %d, %d, %d,\r\n", XScaler_Coefficients[i][j], XScaler_Coefficients[i][j + 1], XScaler_Coefficients[i][j + 2], XScaler_Coefficients[i][j + 3]);
		}
		printf("		},\r\n");
	}
	printf("};\r\n");
	while(1)	;
	*/
	InputFPGAScalerHorizontalBin[B_ScalerIndex] = 0;
	InputFPGAScalerVerticaltBin[B_ScalerIndex] = 0;
	InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, BIT20 << B_ScalerIndex);
//	Delay1ms(5);
	InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, 0);
//	Delay1ms(5);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0x80000000;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	Delay1ms(5);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	Delay1ms(5);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000118;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0x00000404;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x00011C;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
//	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000130;
//	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
//	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	InputFPGA_ScalerSetCoefficient(B_ScalerIndex, B_Phases, 1920, 1080, 1920, 1080);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000120;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000124;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000128;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x00012C;
	gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
	InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
}

WORD InputFPGA_GetCoefficientsStartIndex(BYTE B_Phases)
{
	WORD W_TempData = 0;
	switch(B_Phases)
	{
		case 2:
			W_TempData = (XScaler_TriangularNumber(1) - 1) * 4;
			break;
		case 3:
			W_TempData = (XScaler_TriangularNumber(2) - 1) * 4;
			break;
		case 4:
			W_TempData = (XScaler_TriangularNumber(3) - 1) * 4;
			break;
		case 5:
			W_TempData = (XScaler_TriangularNumber(4) - 1) * 4;
			break;
		case 6:
			W_TempData = (XScaler_TriangularNumber(5) - 1) * 4;
			break;
		case 7:
			W_TempData = (XScaler_TriangularNumber(6) - 1) * 4;
			break;
		case 8:
			W_TempData = (XScaler_TriangularNumber(7) - 1) * 4;
			break;
		case 9:
			W_TempData = (XScaler_TriangularNumber(8) - 1) * 4;
			break;
		case 10:
			W_TempData = (XScaler_TriangularNumber(9) - 1) * 4;
			break;
		case 11:
			W_TempData = (XScaler_TriangularNumber(10) - 1) * 4;
			break;
		case 12:
			W_TempData = (XScaler_TriangularNumber(11) - 1) * 4;
			break;
		case 13:
			W_TempData = (XScaler_TriangularNumber(12) - 1) * 4;
			break;
		case 14:
			W_TempData = (XScaler_TriangularNumber(13) - 1) * 4;
			break;
		case 15:
			W_TempData = (XScaler_TriangularNumber(14) - 1) * 4;
			break;
		case 16:
			W_TempData = (XScaler_TriangularNumber(15) - 1) * 4;
			break;
		default:
			W_TempData = (XScaler_TriangularNumber(4) - 1) * 4;
			break;
	}
	return W_TempData;
}

void InputFPGA_ScalerSetCoefficient(BYTE B_ScalerIndex, BYTE B_Phases, WORD W_InputWidth, WORD W_InputHeight, WORD W_OutputWidth, WORD W_OutputHeight)
{
	BYTE B_HorizontalBin = 0, B_VerticaltBin = 0, k = 0;
	WORD W_TempData = 0, W_Addr = 0, W_StartIndex = 0;
	short ShortData = 0;
	W_TempData = (W_OutputWidth * 160) / W_InputWidth;
	if(W_TempData >= 160)
		B_HorizontalBin = 1;
	else if((W_TempData % 10) >= 5)
		B_HorizontalBin = (W_TempData / 10) + 1;
	else
		B_HorizontalBin = W_TempData / 10;
	W_TempData = (W_OutputHeight * 160) / W_InputHeight;
	if(W_TempData >= 160)
		B_VerticaltBin = 1;
	else if((W_TempData % 10) >= 5)
		B_VerticaltBin = (W_TempData / 10) + 1;
	else
		B_VerticaltBin = W_TempData / 10;
	if((B_HorizontalBin > 0) && (B_HorizontalBin <= 17) && (B_VerticaltBin > 0) && (B_VerticaltBin <= 17))
	{
		if((B_HorizontalBin == InputFPGAScalerHorizontalBin[B_ScalerIndex]) && (B_VerticaltBin == InputFPGAScalerVerticaltBin[B_ScalerIndex]))
			return;
		InputFPGAScalerHorizontalBin[B_ScalerIndex] = B_HorizontalBin;
		InputFPGAScalerVerticaltBin[B_ScalerIndex] = B_VerticaltBin;
		B_HorizontalBin--;
		B_VerticaltBin--;
		W_StartIndex = InputFPGA_GetCoefficientsStartIndex(B_Phases);
		#if(INPUT_FPGA_DEBUG == 3)
		printf("W_StartIndex = %d, B_HorizontalBin = %d, B_VerticaltBin = %d\r\n", W_StartIndex, B_HorizontalBin, B_VerticaltBin);
		#endif
		gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x000130;
		gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = 0;
		InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
		gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD = 0x134;
		for(k = 0; k < 32; k++)
		{
			if(k < (B_Phases << 1))
			{
				ShortData = XScaler_Coefficients[B_HorizontalBin][W_StartIndex];
				W_TempData = (unsigned short) (ShortData);
				#if(INPUT_FPGA_DEBUG == 3)
				printf("W_StartIndex = %d, ShortData = %d, W_TempData = 0x%08X\r\n", W_StartIndex, ShortData, W_TempData);
				#endif
				W_StartIndex++;
				ShortData = XScaler_Coefficients[B_HorizontalBin][W_StartIndex];
				W_Addr = (unsigned short) (ShortData);
				W_Addr <<= 16;
				W_TempData += W_Addr;
				#if(INPUT_FPGA_DEBUG == 3)
				printf("W_StartIndex = %d, ShortData = %d, W_TempData = 0x%08X\r\n", W_StartIndex, ShortData, W_TempData);
				#endif
				W_StartIndex++;
			}
			else
				W_TempData = 0;
			gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = W_TempData;
			InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);
			#if(INPUT_FPGA_DEBUG == 5)
			printf("Set Coefficient : [0x%08X] = 0x%08X\r\n", (WORD) (gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD), (WORD) (gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA));
			#endif
		}
		W_StartIndex = InputFPGA_GetCoefficientsStartIndex(B_Phases);
		#if(INPUT_FPGA_DEBUG == 4)
		printf("W_StartIndex = %d, ShortData = %d, B_HorizontalBin = %d, B_VerticaltBin = %d\r\n", W_StartIndex, ShortData, B_HorizontalBin, B_VerticaltBin);
		#endif
		for(k = 0; k < 32; k++)
		{
			if(k < (B_Phases << 1))
			{
				ShortData = XScaler_Coefficients[B_VerticaltBin][W_StartIndex];
				W_TempData = (unsigned short) (ShortData);
//				W_TempData <<= 16;
				#if(INPUT_FPGA_DEBUG == 4)
				printf("W_StartIndex = %d, ShortData = %d, W_TempData = 0x%08X\r\n", W_StartIndex, ShortData, W_TempData);
				#endif
				W_StartIndex++;
				ShortData = XScaler_Coefficients[B_VerticaltBin][W_StartIndex];
				W_Addr = (unsigned short) (ShortData);
				W_Addr <<= 16;
				W_TempData += W_Addr;
				#if(INPUT_FPGA_DEBUG == 4)
				printf("W_StartIndex = %d, ShortData = %d, W_TempData = 0x%08X\r\n", W_StartIndex, ShortData, W_TempData);
				#endif
				W_StartIndex++;
			}
			else
				W_TempData = 0;
			gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA = W_TempData;
			InputFPGA_ScalerWrite(B_ScalerIndex, &gSt_InputFpgaScalerDataOp);

			#if(INPUT_FPGA_DEBUG == 5)
			printf("Set Coefficient : [0x%08X] = 0x%08X\r\n", (WORD) (gSt_InputFpgaScalerDataOp.SCLER_WRITE_CMD), (WORD) (gSt_InputFpgaScalerDataOp.SCLER_WRITE_DATA));
			#endif
		}
	}
}

void InputFPGA_ScalerWrite(BYTE B_ScalerIndex, INPUT_FPGA_SCLER_DATA_OPERATE *Stp_InputFpgaScalerDataOp)
{
	WORD W_TempData = 0, i = 0, j = 0;
	if(Stp_InputFpgaScalerDataOp != NULL)
	{
		InputFPGA_WriteWord(INPUT_FPGA_SCALER_DATA_REG, Stp_InputFpgaScalerDataOp->SCLER_WRITE_DATA);
		W_TempData = 0xF3 + (B_ScalerIndex * 4) + (Stp_InputFpgaScalerDataOp->SCLER_WRITE_CMD << 8);
		InputFPGA_WriteWord(INPUT_FPGA_SCALER_COMMAND_REG, W_TempData);
		i = 0;
		j = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
		while(((j & BIT24) == 0) && (i < 100))
		{
			Delay1ms(1);
			j = InputFPGA_ReadWord(INPUT_FPGA_SCALER_COMMAND_REG);
			i++;
		}
		#if(INPUT_FPGA_DEBUG == 7)
		printf("ScalerWrite: [0x%08X] = 0x%08X, i = %d\r\n", (WORD) (Stp_InputFpgaScalerDataOp->SCLER_WRITE_CMD), (WORD) (Stp_InputFpgaScalerDataOp->SCLER_WRITE_DATA), i);
		#endif
	}
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_Read(WORD W_Addr)   							*/
/*                                                                          */
/* USAGE:       Read a byte of data from InputFPGA.                   			*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address                                     */
/*                                                                          */
/* OUTPUT:      InputFPGA data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
BYTE InputFPGA_ReadByte(WORD W_Addr)
{
	BYTE* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1)) == NULL)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA 0x%02X read ERROR!\r\n", INPUT_FPGA_DEV);
			#endif
			return 0xFF;
		}
		else
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA Read [0x%08X] = 0x%02X\r\n", W_Addr, *pB_Data);
			#endif
			return *pB_Data;
		}
	}
	#if(INPUT_FPGA_DEBUG == 1)
	printf("Input FPGA Read [0x%08X] = 0x%02X\r\n", W_Addr, *pB_Data);
	#endif
 	Delay1ms(1);
  return (*pB_Data);							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_ReadWord(WORD W_Addr)							*/
/*                                                                          */
/* USAGE:       Read 4 bytes of data from InputFPGA.                  			*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address (upper byte address)				*/
/*                                                                          */
/* OUTPUT:      InputFPGA data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
WORD InputFPGA_ReadWord(WORD W_Addr)
{
	BYTE* pB_Data = NULL;
	WORD W_Data;
	if ((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 4)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 4)) == NULL)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA 0x%02X read ERROR! W_Addr = 0x%04X\r\n", INPUT_FPGA_DEV, W_Addr);
			#endif
			return 0xFFFFFFFF;
		}
		else
		{
			W_Data = (*pB_Data) << 24;  			// Read upper 24 WORD of data
			pB_Data++;
			W_Data |= ((*pB_Data) << 16);  			// Read upper 16 - 23 WORD of data
			pB_Data++;
			W_Data |= ((*pB_Data) << 8);  			// Read upper 8 - 15 WORD of data
			pB_Data++;
			W_Data |= (*pB_Data);          			// Read lower WORD of data
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA Read [0x%08X] = 0x%08X\r\n", W_Addr, W_Data);
			#endif
			return W_Data;
		}
	}
	W_Data = (*pB_Data) << 24;  			// Read upper 24 WORD of data
	pB_Data++;
	W_Data |= ((*pB_Data) << 16);  			// Read upper 16 - 23 WORD of data
	pB_Data++;
	W_Data |= ((*pB_Data) << 8);  			// Read upper 8 - 15 WORD of data
	pB_Data++;
	W_Data |= (*pB_Data);          			// Read lower WORD of data
	#if(INPUT_FPGA_DEBUG == 1)
	printf("INPUT_FPGA READ 0x%08X 0x%08X\r\n", W_Addr, W_Data);
	#endif
//	if(W_Addr == 0x1412)
//		printf("INPUT_FPGA READ 0x%08X 0x%08X\r\n", W_Addr, W_Data);
	Delay1ms(1);
	return W_Data;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_ReadData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)*/
/*                                                                          */
/* USAGE:       Read InputFPGA data and copy it into buffer.                   */
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address                                     */
/*              Bp_Buf - buffer use to store the returning InputFPGA data      */
/*              W_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE* InputFPGA_ReadData(WORD W_Addr, WORD W_Len)
{
	BYTE* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len)) == NULL)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA 0x%02X read ERROR!\r\n", INPUT_FPGA_DEV);
			#endif
			return NULL;
		}
		else
			return pB_Data;
	}
	Delay1ms(1);
  return pB_Data;
}
/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_WriteByte(WORD W_Addr, BYTE B_Data)			*/
/*                                                                          */
/* USAGE:       Write a byte of data into InputFPGA.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address                                     */
/*              B_Data - data write into InputFPGA      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE InputFPGA_WriteByte(WORD W_Addr, BYTE B_Data)
{
	BYTE i = 0;
	i = 0;
	while (I2C1WriteByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1, &B_Data) == ((BYTE) ERR))
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("Input FPGA 0x%02X write [0x%08X] = 0x%02X ERROR!\r\n", INPUT_FPGA_DEV, W_Addr, B_Data);
			#endif
			return (BYTE) ERR;
		}
	}
	#if(INPUT_FPGA_DEBUG == 1)
	printf("Input FPGA Write [0x%08X] = 0x%02X\r\n", W_Addr, B_Data);
	#endif
	Delay1ms(1);
	return OK;							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_WriteWord(WORD W_Addr, BYTE W_Data)			*/
/*                                                                          */
/* USAGE:       Write 2 bytes of data into InputFPGA.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address (upper byte address)				*/
/*              W_Data - data write into InputFPGA      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE InputFPGA_WriteWord(WORD W_Addr, WORD W_Data)
{
	BYTE B_Data[4] = {0}, i = 0;
	B_Data[0] = (BYTE)(W_Data >> 24);
	B_Data[1] = (BYTE)(W_Data >> 16);
	B_Data[2] = (BYTE)(W_Data >> 8);
	B_Data[3] = (BYTE)(W_Data);

	i = 0;
	while (I2C1WriteByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 4, B_Data) == ((BYTE) ERR))
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("INPUT_FPGA 0x%02X write [0x%08X] = 0x%08X ERROR!\r\n", INPUT_FPGA_DEV, W_Addr, W_Data);
			#endif
			return (BYTE) ERR;
		}
	}
	#if(INPUT_FPGA_DEBUG == 1)
	printf("INPUT_FPGA WRITE 0x%08X 0x%08X\r\n", W_Addr, W_Data);
	#endif
//	if(W_Addr == 0x1412)
//		printf("INPUT_FPGA WRITE 0x%08X 0x%08X\r\n", W_Addr, W_Data);
	Delay1ms(1);
	return OK;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_ModifyWord(WORD W_Addr, WORD W_Mask, WORD W_Data)			*/
/*                                                                          */
/* USAGE:       Modify 4 bytes of data into InputFPGA.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address (upper byte address)				*/
/*							W_Mask																								*/
/*              W_Data - data write into InputFPGA      						*/
/*                                                                          */
/*              All bits specified in the mask are set in the register according to the value specified.	*/
/*              A mask of 0x00000000 does not change any bits.																									*/
/*              A mask of 0xFFFFFFFF is the same a writing a WORD - all bits are set to the value given.			*/
/*              When only some buts in the mask are set, only those bits are changed to the values given.	*/
/*																																								*/
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE InputFPGA_ModifyWord(WORD W_Addr, WORD W_Mask, WORD W_Data)
{
	WORD W_TempData = 0;
	W_TempData = InputFPGA_ReadWord(W_Addr);
	W_TempData &= (~W_Mask);        //first clear all bits in mask
	W_TempData |= (W_Mask & W_Data); //then set bits from value
	return InputFPGA_WriteWord(W_Addr, W_TempData);
}

//------------------------------------------------------------------------------
// Function: InputFPGA_BitToggle
// Description: Toggle a bit or bits in a register
//
//              All bits specified in the mask are first set and then cleared in the register.
//              This is a common operation for toggling a bit manually.
//------------------------------------------------------------------------------
BYTE InputFPGA_BitToggle(WORD W_Addr, WORD W_Mask)
{
	WORD W_TempData = 0;
	W_TempData = InputFPGA_ReadWord(W_Addr);
  W_TempData |=  W_Mask;  										//first set the bits in mask
	InputFPGA_WriteWord(W_Addr, W_TempData);		//write register with bits set
	W_TempData &= ~W_Mask;  										//then clear the bits in mask
	return InputFPGA_WriteWord(W_Addr, W_TempData);	//write register with bits clear
}
/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE InputFPGA_WriteData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)	*/
/*                                                                          */
/* USAGE:       Write the required data into InputFPGA.                    	*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address                                     */
/*              Bp_Buf - buffer use to store the InputFPGA data      			*/
/*              W_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE InputFPGA_WriteData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)
{
	BYTE i = 0;
	while (I2C1WriteByte(INPUT_FPGA_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len, Bp_Buf) == ((BYTE) ERR))
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			#if(INPUT_FPGA_DEBUG == 1)
			printf("InputFPGA 0x%02X write ERROR!\r\n", INPUT_FPGA_DEV);
			#endif
			return (BYTE) ERR;
		}
	}
	Delay1ms(1);
	return OK;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    InputFPGA_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void InputFPGA_DummyCall(void)
{
	InputFPGA_ReadByte	(0);
	InputFPGA_WriteByte	(0, 0);
	InputFPGA_ReadWord	(0);
	InputFPGA_WriteWord	(0, 0);
	InputFPGA_ReadData	(0, 0);
	InputFPGA_WriteData	(0, 0, 0);
}
/********************************  END  *************************************/

