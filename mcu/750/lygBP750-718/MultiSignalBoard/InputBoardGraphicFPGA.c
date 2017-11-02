#define __INPUT_BOARD_GRAPHIC_FPGA_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardGraphicFPGA.c                                      */
/*																			*/
/* USAGE:       This module contains functions for Input Board Graphic FPGA.	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
//#include "../System Code/lpc17xx.h"
#include "../System Code/type.h"
#include "../System Code/Serial.h"
#include "../System Code/global.h"
#include "../System Code/Timer.h"
#include "../MultiSignalBoard/BoardID.h"
#include "../Device/ClockGeneration/SL38160.h"
#include "Param.h"
#include "../Device/InputFPGA/InputFPGA.h"
#include "../Device/XilinxScaler/yuv422_scaler.h"
#include "InputBoardGraphicFPGA.h"
#include "WindowRectDef.h"

#define CVBSVideoCaptureWidth 					680
#define CVBSVideoCaptureHeight 					480
#define CVBSVideoDisplayHeight 					540
#define CVBSVideoCaptureLeftOffset 			20
#define CVBSVideoCaptureTopOffsetNTSC 	14
#define CVBSVideoCaptureTopOffsetPAL 		48
#define CVBSVideoCaptureTopOffset				14
#define CVBSVideoCaptureWidth1 					640
#define CVBSVideoCaptureHeight1 				490

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S																				*/
/****************************************************************************/
extern const ST_DEVICE_INPUT_GRAPHIC_FPGA_INTERFACE St_InputGraphicFPGAInterface[];
ST_INPUT_BOARD_GRAPHIC_FPGA gSt_InputBoarGraphicFPGA;
BYTE gB_CVBSSwitchEnable = 0;
BYTE gB_CVBSSwitchIndex = 0;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    																			*/
/****************************************************************************/
void InitInputBoardGraphicFPGA(void);
//void InputBoardGraphicFPGATask(void);
BYTE InputGraphicFPGACheckChipId(void);
BYTE InputGraphicFPGAChangeOutputTiming(BYTE B_TimingIndex);
BYTE InputGraphicFPGAGetSlotBoardType(BYTE B_SlotIndex);
BYTE InputGraphicFPGAGetSlotBoardBmgBaseType(BYTE B_SlotIndex);
BYTE InputGraphicFPGAFindGTPPortNum(BYTE B_FPGAIndex, BYTE B_GTPIndex);
void InputGraphicFPGAInitAllScaler(void);
int	InputGraphicFPGAClearScalerUse(BYTE B_SlotIndex);
int	InputGraphicFPGAClearOutputWindow(BYTE B_SlotIndex, BYTE B_OutputPortIndex);
void InputGraphicFPGASetCVBSCapturePos(int VideoSignalIndex, int VideoType);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S																					*/
/****************************************************************************/


/****************************************************************************/
/*	S T A T I C   R O U T I N E S																						*/
/****************************************************************************/
static void InputGraphicFPGAVideoOutputLayout(void);
static void InputGraphicFPGAVideoOutputLayoutByScaler(void);
/****************************************************************************/
/*	C O D E																																	*/
/****************************************************************************/
void InitInputBoardGraphicFPGA(void)
{
	int i = 0;
	BYTE B_Data = 0;
	WORD W_TempHStart = 0, W_TempVStart = 0;
	ST_SignalWindows St_TempSignalWin;
	ST_SignalSource St_TempSignalSource;
	ST_Rect St_TempRect;
	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	XScalerAperture St_TempAperture;
	memset(&gSt_InputBoarGraphicFPGA, 0, sizeof(gSt_InputBoarGraphicFPGA));
	B_Data = InputGraphicFPGACheckChipId();
	if(B_Data == 0xFF) 					// Unknown type FPGA chip
		return;
	Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[gSt_InputBoarGraphicFPGA.B_ActiveChip]);
	if(B_Data == INPUT_DEV_VDPA)					// VDPA Input FPGA
	{
		Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType = INPUT_DEV_VDPA;
		Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface = &(St_InputGraphicFPGAInterface[0]);
	}
	if(B_Data == 0x04)					// IP Decoder Input FPGA
	{
		Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType = INPUT_DEV_IP_DECODER;
		Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface = &(St_InputGraphicFPGAInterface[4]);
	}
	InputFPGA_SoftReset(0xF0 | BIT0);
	Stp_TempDeviceInputGraphicFPGA->B_BoardIndex = i;
	Stp_TempDeviceInputGraphicFPGA->B_I2CBusIndex = 1;
	Stp_TempDeviceInputGraphicFPGA->B_SubI2CBusIndex = 0x0F;
	gSt_InputBoarGraphicFPGA.B_ActiveChip++;
	InputFPGASetVideoOutputEnalble(0x00);
	memset(&St_TempSignalWin, 0, sizeof(St_TempSignalWin));
	St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
	St_TempRect.left = 0;
	St_TempRect.top = 0;
	St_TempRect.right = 1920;
	St_TempRect.bottom = 1080;
	if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_IP_DECODER)
	{
		memcpy(&(St_TempSignalWin.SignalInputWindow), &St_TempRect, sizeof(ST_Rect));
		memcpy(&(St_TempSignalWin.SignalDisplayWindow), &St_TempRect, sizeof(ST_Rect));
		for(B_Data = 0; B_Data < 2; B_Data++)
		{
			InputFPGA_ScalerInit(B_Data);
			Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[B_Data].Stp_SignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[B_Data]);
			memset(&St_TempAperture, 0, sizeof(St_TempAperture));
			St_TempAperture.InVertSize = 1080 / 2;						// < Vertical size of the input video
			St_TempAperture.InHoriSize = 1920 / 2;						// < Horizontal size of the input video
			St_TempAperture.InFirstLine = 0;							// < The first line index in the input video
			St_TempAperture.InLastLine = (1080 / 2) - 1;						// < The last line index in the input video
			St_TempAperture.InFirstPixel = 0;							// < The first pixel index in the input video
			St_TempAperture.InLastPixel = (1920 / 2) - 1;						// < The last pixel index in the input video
			St_TempAperture.OutVertSize = 1080;						// < Vertical size of the output video
			St_TempAperture.OutHoriSize = 1920;						// < Horizontal size of the output video
			/*
			InputFPGA_ScalerSetup(B_Data, B_Data << 1, &St_TempAperture);
			St_TempSignalWin.Stp_SignalSource->SignalSource = (B_Data << 1) + 1;				// Scaler Output as Source
			*/
			St_TempSignalWin.Stp_SignalSource->SignalSource = B_Data << 1;
			InputFPGASetBackgroundWindow(B_Data + 2, 0, 0, 1, St_TempSignalWin);
		}
	}
	else
	{
		memcpy(&(St_TempSignalWin.SignalInputWindow), &St_TempRect, sizeof(ST_Rect));
		memcpy(&(St_TempSignalWin.SignalDisplayWindow), &St_TempRect, sizeof(ST_Rect));
		for(B_Data = 0; B_Data < 4; B_Data++)
		{
			InputFPGA_ScalerInit1(B_Data, 16);
			Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler[B_Data].Stp_SignalSource = &(Stp_TempDeviceInputGraphicFPGA->St_ScalerSignalSource[B_Data]);
			memset(&St_TempAperture, 0, sizeof(St_TempAperture));
			St_TempAperture.InVertSize = 1080 / 2;						// < Vertical size of the input video
			St_TempAperture.InHoriSize = 1920 / 2;						// < Horizontal size of the input video
			St_TempAperture.InFirstLine = 0;							// < The first line index in the input video
			St_TempAperture.InLastLine = (1080 / 2) - 1;						// < The last line index in the input video
			St_TempAperture.InFirstPixel = 0;							// < The first pixel index in the input video
			St_TempAperture.InLastPixel = (1920 / 2) - 1;						// < The last pixel index in the input video
			St_TempAperture.OutVertSize = 1080;						// < Vertical size of the output video
			St_TempAperture.OutHoriSize = 1920;						// < Horizontal size of the output video
			/*
			InputFPGA_ScalerSetup(B_Data, B_Data << 1, &St_TempAperture);
			St_TempSignalWin.Stp_SignalSource->SignalSource = (B_Data << 1) + 1;				// Scaler Output as Source
			*/
			St_TempSignalWin.Stp_SignalSource->SignalSource = B_Data << 1;
//					InputFPGASetBackgroundWindow(B_Data, 0, 0, 0, St_TempSignalWin);
		}
	}
	#if 0
	InputFPGASetOutputResolution(16);
	#else
	B_Data = (BYTE) (ParamRead(NVR_CUSTOM_RES_ENABLE) & 0xFF);
	if(B_Data == 1)
	{
		B_Data = (BYTE) (ParamRead(NVR_CUSTOM_RES_CLK));
		printf("Board Output Clock %d\r\n", B_Data);
		InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x0F);
		BoardIICBusSwitch(7);
		Set_SL38160_CLK(DEVICEID_SL38160_17, 10, B_Data);		// AUDIO_CLK: pData[0], VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
		BoardIICBusSwitch(0);																// IIC Switch to default
		B_Data = (BYTE) (ParamRead(NVR_CUSTOM_VIDEO_MODE_INDEX));
		St_TempRect.left = ParamRead(NVR_CUSTOM_RES_TOTAL_PIXEL);
		St_TempRect.right = ParamRead(NVR_CUSTOM_RES_TOTAL_LINE);
		W_TempHStart =  ParamRead(NVR_CUSTOM_RES_H_START);
		W_TempVStart =  ParamRead(NVR_CUSTOM_RES_V_START);
		St_TempRect.top = ParamRead(NVR_CUSTOM_RES_ACTIVE_PIXEL);
		St_TempRect.bottom = ParamRead(NVR_CUSTOM_RES_ACTIVE_LINE);
		printf("CustomOutputRes: %d, %ld, %ld, %d, %d, %ld, %ld\r\n", B_Data, St_TempRect.left, St_TempRect.right, W_TempHStart, W_TempVStart, St_TempRect.top, St_TempRect.bottom);
		InputFPGASetCustomOutputResolution(B_Data, St_TempRect.left, St_TempRect.right, W_TempHStart, W_TempVStart, St_TempRect.top, St_TempRect.bottom);
		InputFPGASetVideoOutputEnalble(0x10F);
		InputFPGA_SoftReset(0xF0 | BIT0);
		InputFPGASetUpdata();
	}
	else
	{
		B_Data = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
		InputGraphicFPGAChangeOutputTiming(B_Data);
//	ParamWrite(NVR_SCREEN_WIDTH, g_ScreenOutputWidth);
//	ParamWrite(NVR_SCREEN_HEIGHT, g_ScreenOutputHeight);
	}
	#endif
	InputFPGASetVideoOutputEnalble(0x10F);
	#if 1
	W_TempHStart = InputFPGA_ReadWord(INPUT_FPGA_CHIP_ID);
  switch(W_TempHStart)
	{
    case INPUT_BOARD_VDPA:
    case INPUT_BOARD_VDPB:
    case INPUT_BOARD_VDPC:
      W_TempVStart = 0;
			gB_CVBSSwitchIndex = 0;
			gB_CVBSSwitchEnable = 0;
      break;
    case INPUT_BOARD_VDPD:
    case INPUT_BOARD_VDPE:
    case INPUT_BOARD_VDPF:
			W_TempVStart = ParamRead(NVR_INPUT0_VIDEO_SWITCH) & 0xFF;
			gB_CVBSSwitchIndex = W_TempVStart;
			gB_CVBSSwitchEnable = 1;
      break;
    default:
      W_TempVStart = 0;
			gB_CVBSSwitchIndex = 0;
			gB_CVBSSwitchEnable = 0;
			break;
	}
	if(gB_CVBSSwitchEnable == 1)
	{
		W_TempVStart <<= 8;
		W_TempVStart |= BIT0;
		InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_TempVStart);
	}
	
	W_TempHStart = InputFPGA_RGBToYUV(0x000000);
	InputFPGA_WriteWord(0x90, W_TempHStart);		// Set No signal scaler 0 color output
	InputFPGA_WriteWord(0x91, W_TempHStart);		// Set No signal scaler 1 color output
	InputFPGA_WriteWord(0x92, W_TempHStart);		// Set No signal scaler 2 color output
	InputFPGA_WriteWord(0x93, W_TempHStart);		// Set No signal scaler 3 color output
//	InputFPGA_WriteWord(0x94, 0x0F);					//
	InputFPGA_WriteWord(0x1420, W_TempHStart);
	InputFPGA_WriteWord(0x1421, W_TempHStart);
	InputFPGA_WriteWord(0x1422, W_TempHStart);
	InputFPGA_WriteWord(0x1423, W_TempHStart);
	InputFPGA_WriteWord(0x1424, W_TempHStart);		// Set No signal window color output 0
	InputFPGA_WriteWord(0x1425, W_TempHStart);		// Set No signal window color output 1
	InputFPGA_WriteWord(0x1426, W_TempHStart);		// Set No signal window color output 2
	InputFPGA_WriteWord(0x1427, W_TempHStart);		// Set No signal window color output 3
	#endif
	printf("\r\nBoard %d, Input FPGA Version: %08X\r\n", Stp_TempDeviceInputGraphicFPGA->B_BoardIndex, InputFPGA_ReadWord(INPUT_FPGA_CHIP_VER));
	printf("Board %d, Input FPGA Init OK\r\n", Stp_TempDeviceInputGraphicFPGA->B_BoardIndex);
}

void InputGraphicFPGAInitAllScaler(void)
{
	int i = 0;
	BYTE B_Data = 0;
	XScalerAperture St_TempAperture;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		for(B_Data = 0; B_Data < 4; B_Data++)
		{
			memset(&St_TempAperture, 0, sizeof(St_TempAperture));
			St_TempAperture.InVertSize = 1080 / 2;						// < Vertical size of the input video
			St_TempAperture.InHoriSize = 1920 / 2;						// < Horizontal size of the input video
			St_TempAperture.InFirstLine = 0;							// < The first line index in the input video
			St_TempAperture.InLastLine = (1080 / 2) - 1;						// < The last line index in the input video
			St_TempAperture.InFirstPixel = 0;							// < The first pixel index in the input video
			St_TempAperture.InLastPixel = (1920 / 2) - 1;						// < The last pixel index in the input video
			St_TempAperture.OutVertSize = 1080;						// < Vertical size of the output video
			St_TempAperture.OutHoriSize = 1920;						// < Horizontal size of the output video
			InputFPGA_ScalerSetup(B_Data, B_Data << 1, &St_TempAperture);
		}
	}
}

BYTE InputGraphicFPGAChangeOutputTiming(BYTE B_TimingIndex)
{
#define VIC_BASE                0
#define HDMI_VIC_BASE           43
#define VIC_3D_BASE             47
#define PC_BASE                 61

	BYTE B_Data = 0;
	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0);
  switch(B_TimingIndex)
	{
    case 0:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 21);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 21 - 1024 x 768@60 	65.000MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE + 13);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 13, 1344, 806, 0, 0, 1024, 768);
			printf("Input FPGA Output Timing: 1024 x 768@60Hz\r\n");
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 21);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 13);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1344);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 806);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1024);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 768);
      break;
    case 1:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 16);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 16 - 800 x 600@60 	40.000MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE+9);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 9, 1056, 628, 0, 0, 800, 600);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 16);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 9);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1056);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 628);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 800);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 600);
			printf("Input FPGA Output Timing: 800 x 600@60Hz\r\n");
      break;
    case 2:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 4);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 4 - 1280 x 720@60 	74.25MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(4);
			B_Data = InputFPGASetCustomOutputResolution(4, 1650,  750, 0, 0, 1280, 720);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 4);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 4);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1650);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 750);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1280);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 720);
			printf("Input FPGA Output Timing: 1280 x 720@60Hz\r\n");
      break;
    case 3:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 1920 x 1080@60 	148.50MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(16);
			B_Data = InputFPGASetCustomOutputResolution(16, 2200, 1125, 0, 0, 1920, 1080);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 16);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 2200);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1125);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080);
			printf("Input FPGA Output Timing: 1920 x 1080@60Hz\r\n");
      break;
    case 4:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 30);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 30 - 1360 x 768@60 	85.500MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE+28);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 28, 1792, 795, 0, 0, 1360, 768);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 30);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 28);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1792);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 795);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1360);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 768);
			printf("Input FPGA Output Timing: 1366 x 768@60Hz\r\n");
      break;
    case 5:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 37);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 37 - 1400x1050@60 	121.750MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE+30);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 30, 1864, 1089, 0, 0, 1400, 1050);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 37);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 30);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1864);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1089);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1400);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1050);
			printf("Input FPGA Output Timing: 1400 x 1050@60Hz\r\n");
      break;
    case 6:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 40);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 40 - 1600x1200@60 	162.000MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE+18);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 18, 2160, 1250, 0, 0, 1600, 1200);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 40);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 18);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 2160);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1250);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1600);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1200);
			printf("Input FPGA Output Timing: 1600 x 1200@60Hz\r\n");
      break;
    case 7:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 46);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 48 - 1920x1200@60 	154.000MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(PC_BASE+41);
			B_Data = InputFPGASetCustomOutputResolution(PC_BASE + 41, 2080, 1235, 0, 0, 1920, 1200);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 46);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, PC_BASE + 41);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 2080);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1235);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1200);
			printf("Input FPGA Output Timing: 1920 x 1200@60Hz Reduced Blanking\r\n");
      break;
    case 100:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 3840 x 540@60 	148.50MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(16);
			B_Data = InputFPGASetCustomOutputResolution(16, 2200 * 2, 563, 0, 0, 1920 * 2, 1080 / 2);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 16);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 4400);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 563);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920 * 2);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080 / 2);
			printf("Input FPGA Output Timing: 3840 x 540@60Hz\r\n");
      break;
    case 101:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 960 x 2160@60 	148.50MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(16);
			B_Data = InputFPGASetCustomOutputResolution(16, 2200 / 2, 1125 * 2, 0, 0, 1920 / 2, 1080 * 2);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 16);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 1100);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 2250);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920 / 2);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080 * 2);
			printf("Input FPGA Output Timing: 960 x 2160@60Hz\r\n");
      break;
    case 102:
			BoardIICBusSwitch(7);
		#if 0
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 3840 x 1080@30 	148.50MHz
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(16);
			B_Data = InputFPGASetCustomOutputResolution(43, 2200 * 2, 1125, 0, 0, 1920 * 2, 1080);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 43);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 4400);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1125);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920 * 2);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080);
		#else
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 34);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 34---//1280x1024 	75 	135.000
			BoardIICBusSwitch(0);									// IIC Switch to default
			B_Data = InputFPGASetCustomOutputResolution(111, 3944, 1104, 0, 0, 3840, 1080);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 34);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 111);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 3944);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1104);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 3840);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080);
		#endif
			printf("Input FPGA Output Timing: 3840 x 1080@30Hz\r\n");
      break;
    case 103:
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 1920 x 2160@30 	148.50MHz
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			BoardIICBusSwitch(0);									// IIC Switch to default
//			B_Data = InputFPGASetOutputResolution(16);
			B_Data = InputFPGASetCustomOutputResolution(43, 2200, 1125 * 2, 0, 0, 1920, 1080 * 2);
			ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
			ParamWrite(NVR_CUSTOM_RES_CLK, 6);
			ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, 43);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, 2200);
			ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, 1125 * 2);
			ParamWrite(NVR_CUSTOM_RES_H_START, 0);
			ParamWrite(NVR_CUSTOM_RES_V_START, 0);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, 1920);
			ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, 1080 * 2);
			printf("Input FPGA Output Timing: 1920 x 2160@30Hz\r\n");
      break;
    default:
			B_Data = 0xFF;
			printf("Input Graphic FPGA Unsupported Output Timing, B_TimingIndex = 0x%d\r\n", B_TimingIndex);
			break;
	}
	InputFPGASetVideoOutputEnalble(0x100);
	InputFPGA_SoftReset(0xF0 | BIT0);
	InputFPGASetUpdata();
	return B_Data;
}

BYTE InputGraphicFPGACheckChipId(void)
{
	BYTE B_Data = 0xFF;
	WORD W_Data = 0;
	W_Data = InputFPGA_ReadWord(INPUT_FPGA_CHIP_ID);
  switch(W_Data)
	{
    case INPUT_BOARD_VDPA:
    case INPUT_BOARD_VDPD:
      B_Data = INPUT_DEV_VDPA;
      break;
    case INPUT_BOARD_VDPB:
    case INPUT_BOARD_VDPE:
      B_Data = INPUT_DEV_VDPA;
      break;
    case INPUT_BOARD_VDPC:
    case INPUT_BOARD_VDPF:
      B_Data = INPUT_DEV_VDPA;
      break;
    default:
			B_Data = 0xFF;
			printf("Input Graphic FPGA Unsupported Chip, ID = 0x%08X\r\n", W_Data);
			break;
	}
	return B_Data;
}

BYTE InputGraphicFPGAGetSlotBoardType(BYTE B_SlotIndex)
{
	BYTE i = 0;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
			return Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType;
	}
	return 0xFF;
}

BYTE InputGraphicFPGAGetSlotBoardBmgBaseType(BYTE B_SlotIndex)
{
	BYTE i = 0;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			if(Stp_TempDeviceInputGraphicFPGA->B_GraphicFPGAType == INPUT_DEV_IP_DECODER)
				return 1;
			else
				return 0;
		}
	}
	return 0xFF;
}

BYTE InputGraphicFPGAFindGTPPortNum(BYTE B_FPGAIndex, BYTE B_GTPIndex)
{
	BYTE B_Data = 0xFF;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	if(B_FPGAIndex >= gSt_InputBoarGraphicFPGA.B_ActiveChip)
		return 0xFF;
	Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[B_FPGAIndex]);
	switch(B_GTPIndex)
	{
		case 0:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Background0GTP;
			break;
		case 1:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Foreground0GTP;
			break;
		case 2:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Background1GTP;
			break;
		case 3:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Foreground1GTP;
			break;
		case 4:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Background2GTP;
			break;
		case 5:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Foreground2GTP;
			break;
		case 6:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Background3GTP;
			break;
		case 7:
			B_Data = Stp_TempDeviceInputGraphicFPGA->Stp_InputFPGAInterface->B_Foreground3GTP;
			break;
		default:
			B_Data = 0xFF;
			printf("Input Graphic FPGA Can Not Find GTP PortNum! B_GTPIndex = 0x%d\r\n", B_GTPIndex);
			break;
	}
	return B_Data;
}

int	InputGraphicFPGAClearScalerUse(BYTE B_SlotIndex)
{
	BYTE i = 0;
	int TempData = -1;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(B_SlotIndex == 0x0F)
		{
			Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed = 0;
			memset(&(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler), 0, sizeof(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler));
			TempData = 1;
			continue;
		}
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			Stp_TempDeviceInputGraphicFPGA->B_ScalerUsed = 0;
			memset(&(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler), 0, sizeof(Stp_TempDeviceInputGraphicFPGA->St_DisplayScaler));
			return 1;
		}
	}
	return TempData;
}

int	InputGraphicFPGAClearOutputWindow(BYTE B_SlotIndex, BYTE B_OutputPortIndex)
{
	BYTE i = 0;
	int TempData = -1;
 	ST_DEVICE_INPUT_GRAPHIC_FPGA *Stp_TempDeviceInputGraphicFPGA = NULL;
	for(i = 0; i < gSt_InputBoarGraphicFPGA.B_ActiveChip; i++)
	{
		Stp_TempDeviceInputGraphicFPGA = &(gSt_InputBoarGraphicFPGA.St_DeviceInputGraphicFPGA[i]);
		if(B_SlotIndex == 0x0F)
		{
			if(InputFPGAClearBackgroundWindow(B_OutputPortIndex) == 0xFF)
			{
				printf("InputGraphicFPGAClearOutputWindow ERROR!\r\n");
				return 0xFF;
			}
			TempData = 1;
			continue;
		}
		if(Stp_TempDeviceInputGraphicFPGA->B_BoardIndex == B_SlotIndex)
		{
			if(InputFPGAClearBackgroundWindow(B_OutputPortIndex) == 0xFF)
			{
				printf("InputGraphicFPGAClearOutputWindow ERROR!\r\n");
				return 0xFF;
			}
			return 1;
		}
	}
	return TempData;
}

void InputGraphicFPGASetCVBSCapturePos(int VideoSignalIndex, int VideoType)
{
	
}

static void InputGraphicFPGAVideoOutputLayoutByScaler(void)
{
	BYTE B_Data = 0;
	ST_SignalWindows St_TempSignalWin;
	ST_SignalSource St_TempSignalSource;
	XScalerAperture St_TempAperture;
	WORD W_TempData = 0;
	ST_Rect* Stp_TempRect = NULL;
	memset(&St_TempSignalWin, 0, sizeof(St_TempSignalWin));
	memset(&St_TempSignalSource, 0, sizeof(St_TempSignalSource));
	St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
	Stp_TempRect = &(St_TempSignalWin.SignalInputWindow);
	InputFPGA_WriteWord(0x01, 0xF0);
	InputFPGA_WriteWord(0x1413, 0);
	InputFPGA_WriteWord(0x1414, 0);
	// Scaler0 Set
	InputFPGA_WriteWord(0x1500, CVBSVideoCaptureHeight); 
	InputFPGA_WriteWord(0x1510, CVBSVideoCaptureHeight * 2); 
	// Signal 0
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1509, W_TempData); 
	// Signal 1
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x150A, W_TempData); 
	// Signal 2
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x150B, W_TempData); 
	// Signal 3
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1519, W_TempData);
	// Signal 4
	St_TempSignalWin.Stp_SignalSource->SignalSource = 1;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x151A, W_TempData);
	// Signal 5
	St_TempSignalWin.Stp_SignalSource->SignalSource = 1;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x151B, W_TempData);
	memset(&St_TempAperture, 0, sizeof(St_TempAperture));
	St_TempAperture.InVertSize = CVBSVideoCaptureHeight * 2;					// < Vertical size of the input video
	St_TempAperture.InHoriSize = CVBSVideoCaptureWidth * 3;						// < Horizontal size of the input video
	St_TempAperture.InFirstLine = 0;																	// < The first line index in the input video
	St_TempAperture.InLastLine = St_TempAperture.InVertSize - 1;			// < The last line index in the input video
	St_TempAperture.InFirstPixel = 0;																	// < The first pixel index in the input video
	St_TempAperture.InLastPixel = St_TempAperture.InHoriSize - 1;			// < The last pixel index in the input video
	St_TempAperture.OutVertSize = 1080;						// < Vertical size of the output video
	St_TempAperture.OutHoriSize = 1920;						// < Horizontal size of the output video
	InputFPGA_ScalerSetAperture(0, &St_TempAperture);
	// Scaler1 Set
	InputFPGA_WriteWord(0x1600, CVBSVideoCaptureHeight); 
	InputFPGA_WriteWord(0x1610, CVBSVideoCaptureHeight * 2); 
	// Signal 6
	St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1609, W_TempData);
	// Signal 7
	St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x160A, W_TempData);
	// Signal 8
	St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x160B, W_TempData);
	// Signal 9
	St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1619, W_TempData);
	// Signal 10
	St_TempSignalWin.Stp_SignalSource->SignalSource = 3;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x161A, W_TempData);
	// Signal 11
	St_TempSignalWin.Stp_SignalSource->SignalSource = 3;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x161B, W_TempData);
	InputFPGA_ScalerSetAperture(1, &St_TempAperture);
	// Scaler2 Set
	InputFPGA_WriteWord(0x1700, CVBSVideoCaptureHeight); 
	InputFPGA_WriteWord(0x1710, CVBSVideoCaptureHeight * 2); 
	// Signal 12
	St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1709, W_TempData);
	// Signal 13
	St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x170A, W_TempData);
	// Signal 14
	St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x170B, W_TempData);
	// Signal 15
	St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1719, W_TempData);
	// Signal 16
	St_TempSignalWin.Stp_SignalSource->SignalSource = 5;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x171A, W_TempData);
	// Signal 17
	St_TempSignalWin.Stp_SignalSource->SignalSource = 5;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x171B, W_TempData);
	InputFPGA_ScalerSetAperture(2, &St_TempAperture);
	// Scaler3 Set
	InputFPGA_WriteWord(0x1800, CVBSVideoCaptureHeight); 
	InputFPGA_WriteWord(0x1810, CVBSVideoCaptureHeight * 2); 
	// Signal 18
	St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1809, W_TempData);
	// Signal 19
	St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x180A, W_TempData);
	// Signal 20
	St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x180B, W_TempData);
	// Signal 21
	St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
	Stp_TempRect->top = 1024 + CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x1819, W_TempData);
	// Signal 22
	St_TempSignalWin.Stp_SignalSource->SignalSource = 7;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = 1024 + CVBSVideoCaptureLeftOffset;
	W_TempData = (WORD) ((St_TempSignalWin.Stp_SignalSource->SignalSource << 24) + (Stp_TempRect->top << 12) + Stp_TempRect->left);
	InputFPGA_WriteWord(0x181A, W_TempData);
	// Signal 23
	St_TempSignalWin.Stp_SignalSource->SignalSource = 7;
	Stp_TempRect->top = CVBSVideoCaptureTopOffset;
	Stp_TempRect->left = CVBSVideoCaptureLeftOffset;
	InputFPGA_WriteWord(0x181B, W_TempData);
	InputFPGA_ScalerSetAperture(3, &St_TempAperture);
	InputFPGA_WriteWord(0x1413, 0x0F);
	InputFPGA_WriteWord(0x1414, 0x0F);
	memset(&St_TempSignalWin, 0, sizeof(St_TempSignalWin));
	memset(&St_TempSignalSource, 0, sizeof(St_TempSignalSource));
	St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
	/*
	St_TempSignalWin.SignalInputWindow.top = 0;
	St_TempSignalWin.SignalInputWindow.left = 0;
//	St_TempSignalWin.SignalInputWindow.left = 8;
	St_TempSignalWin.SignalInputWindow.right = 1920;
	St_TempSignalWin.SignalInputWindow.bottom = 1080;
	St_TempSignalWin.SignalDisplayWindow.top = 0;
	St_TempSignalWin.SignalDisplayWindow.left = 0;
	St_TempSignalWin.SignalDisplayWindow.right = 1920;
	St_TempSignalWin.SignalDisplayWindow.bottom = 1080;
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
	InputFPGASetBackgroundWindow(0, 0, 0, St_TempSignalWin);
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
	InputFPGASetBackgroundWindow(1, 0, 0, St_TempSignalWin);
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
	InputFPGASetBackgroundWindow(2, 0, 0, St_TempSignalWin);
	St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
	InputFPGASetBackgroundWindow(3, 0, 0, St_TempSignalWin);
	*/

	for(B_Data = 0; B_Data < 24; B_Data++)
	{
		
		switch(B_Data)
		{
			case 0:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(0, 0, 0, 0, St_TempSignalWin);
				break;
			case 1:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = (CVBSVideoCaptureWidth1 * 2);
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(0, 1, 0, 0, St_TempSignalWin);
				break;
			case 2:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(0, 2, 0, 0, St_TempSignalWin);
				break;
			case 3:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(0, 0, 1, 0, St_TempSignalWin);
				break;
			case 4:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(0, 1, 1, 0, St_TempSignalWin);
				break;
			case 5:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x08;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(0, 2, 1, 0, St_TempSignalWin);
				break;
			case 6:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(1, 0, 0, 0, St_TempSignalWin);
				break;
			case 7:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(1, 1, 0, 0, St_TempSignalWin);
				break;
			case 8:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(1, 2, 0, 0, St_TempSignalWin);
				break;
			case 9:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(1, 0, 1, 0, St_TempSignalWin);
				break;
			case 10:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(1, 1, 1, 0, St_TempSignalWin);
				break;
			case 11:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0A;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(1, 2, 1, 0, St_TempSignalWin);
				break;
			case 12:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(2, 0, 0, 0, St_TempSignalWin);
				break;
			case 13:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(2, 1, 0, 0, St_TempSignalWin);
				break;
			case 14:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(2, 2, 0, 0, St_TempSignalWin);
				break;
			case 15:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(2, 0, 1, 0, St_TempSignalWin);
				break;
			case 16:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(2, 1, 1, 0, St_TempSignalWin);
				break;
			case 17:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0C;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(2, 2, 1, 0, St_TempSignalWin);
				break;
			case 18:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(3, 0, 0, 0, St_TempSignalWin);
				break;
			case 19:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(3, 1, 0, 0, St_TempSignalWin);
				break;
			case 20:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight;
				InputFPGASetBackgroundWindow(3, 2, 0, 0, St_TempSignalWin);
				break;
			case 21:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(3, 0, 1, 0, St_TempSignalWin);
				break;
			case 22:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(3, 1, 1, 0, St_TempSignalWin);
				break;
			case 23:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0x0E;
				St_TempSignalWin.SignalInputWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalInputWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalInputWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalInputWindow.bottom = CVBSVideoDisplayHeight * 2;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoDisplayHeight;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoDisplayHeight * 2;
				InputFPGASetBackgroundWindow(3, 2, 1, 0, St_TempSignalWin);
				break;
			default:
				break;
		}

	}
}

static void InputGraphicFPGAVideoOutputLayout(void)
{
	BYTE B_Data = 0;
	ST_SignalWindows St_TempSignalWin;
	ST_SignalSource St_TempSignalSource;
	
	memset(&St_TempSignalWin, 0, sizeof(St_TempSignalWin));
	St_TempSignalWin.Stp_SignalSource = &St_TempSignalSource;
	for(B_Data = 0; B_Data < 24; B_Data++)
	{
		switch(B_Data)
		{
			case 0:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 0, 0, 0, St_TempSignalWin);
				break;
			case 1:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 1, 0, 0, St_TempSignalWin);
				break;
			case 2:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 2, 0, 0, St_TempSignalWin);
				break;
			case 3:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 0;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 0, 1, 0, St_TempSignalWin);
				break;
			case 4:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 1;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 1, 1, 0, St_TempSignalWin);
				break;
			case 5:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 1;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(0, 2, 1, 0, St_TempSignalWin);
				break;
			case 6:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 0, 0, 0, St_TempSignalWin);
				break;
			case 7:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 1, 0, 0, St_TempSignalWin);
				break;
			case 8:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 2, 0, 0, St_TempSignalWin);
				break;
			case 9:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 2;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 0, 1, 0, St_TempSignalWin);
				break;
			case 10:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 3;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 1, 1, 0, St_TempSignalWin);
				break;
			case 11:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 3;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(1, 2, 1, 0, St_TempSignalWin);
				break;
			case 12:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 0, 0, 0, St_TempSignalWin);
				break;
			case 13:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 1, 0, 0, St_TempSignalWin);
				break;
			case 14:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 2, 0, 0, St_TempSignalWin);
				break;
			case 15:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 4;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 0, 1, 0, St_TempSignalWin);
				break;
			case 16:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 5;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 1, 1, 0, St_TempSignalWin);
				break;
			case 17:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 5;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(2, 2, 1, 0, St_TempSignalWin);
				break;
			case 18:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 0, 0, 0, St_TempSignalWin);
				break;
			case 19:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 1, 0, 0, St_TempSignalWin);
				break;
			case 20:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = 0;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 2, 0, 0, St_TempSignalWin);
				break;
			case 21:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 6;
				St_TempSignalWin.SignalInputWindow.top = 1024;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 2000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = 0;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 0, 1, 0, St_TempSignalWin);
				break;
			case 22:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 7;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 1024;
				St_TempSignalWin.SignalInputWindow.right = 2000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 1, 1, 0, St_TempSignalWin);
				break;
			case 23:
				St_TempSignalWin.Stp_SignalSource->SignalSource = 7;
				St_TempSignalWin.SignalInputWindow.top = 0;
				St_TempSignalWin.SignalInputWindow.left = 0;
				St_TempSignalWin.SignalInputWindow.right = 1000;
				St_TempSignalWin.SignalInputWindow.bottom = 1000;
				St_TempSignalWin.SignalDisplayWindow.top = CVBSVideoCaptureHeight1;
				St_TempSignalWin.SignalDisplayWindow.left = CVBSVideoCaptureWidth1 * 2;
				St_TempSignalWin.SignalDisplayWindow.right = CVBSVideoCaptureWidth1 * 3;
				St_TempSignalWin.SignalDisplayWindow.bottom = CVBSVideoCaptureHeight1 * 2;
				St_TempSignalWin.SignalInputWindow.left += 40;
				St_TempSignalWin.SignalInputWindow.top += 14;
				InputFPGASetBackgroundWindow(3, 2, 1, 0, St_TempSignalWin);
				break;
			default:
				break;
		}
	}
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    InputGraphicFPGA_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void InputGraphicFPGA_DummyCall(void)
{
	InputGraphicFPGAVideoOutputLayout();
	InputGraphicFPGAVideoOutputLayoutByScaler();
}
/********************************  END  *************************************/
