#define __USER_COMMAND_PC_API_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      UserCommandPC_API.c                                         */
/*																			*/
/* USAGE:       This module contains functions for User Command PC API.		*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*												I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "../System Code/type.h"
#include "../System Code/Serial.h"
#include "../System Code/ASCII.h"
#include "../System Code/global.h"
#include "../System Code/Timer.h"
#include "../IIC Comm/D_IIC0.h"
#include "../IIC Comm/D_IIC1.h"
#include "../IIC Comm/D_IIC2.h"

#include "../Device/InputFPGA/InputFPGA.h"
#include "../Device/GV7601/gv7601.h"
#include "../Device/ClockGeneration/SL38160.h"
#include "../MultiSignalBoard/BoardID.h"
#include "../MultiSignalBoard/Init.h"
#include "../MultiSignalBoard/param.h"
#include "../MultiSignalBoard/InputBoardADV7604.h"
#include "../MultiSignalBoard/InputBoardGV7601.h"
#include "../MultiSignalBoard/OutputBoardSiI9136.h"
#include "../MultiSignalBoard/InputBoardTVP5158.h"
#include "../MultiSignalBoard/InputBoardGraphicFPGA.h"
#include "../WindowsLayout/ScreenWindowLayout.h"
#include "UserCommandPC_API.h"

#define _USERCOMMANDPC_API_C
//#define CR     0x0D
//#define USER_COMMAD_BUF_LENGTH     	128
#define USER_COMMAD_DEBUG						0
#define USER_COMMAD_SEND_STR_LEN		128
#define USER_COMMAD_SEND_STR1_LEN		64

#define DEBUG_LOAD_USER_LAST_MODE		0
#define SAVE_MESSAGE_TO_EEPROM			1
#define DISPLAY_MODE_INDEX_MAX			11
#define OUTPUT_BOARD_MAX 4
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

unsigned char g_DisplayModeIndex = 0;
unsigned char gB_TotalInputPort = 0, gB_TotalOutputPort = 0;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N 																					*/
/****************************************************************************/
void UserCommadTask(void);
void ParseTCPCommad(BYTE* pCommad);
void ParseSerialCommad(BYTE* pCommad);
BYTE ParseCommadByBrillViewType1(BYTE B_Data);
void ExcuteUserCommad(BYTE B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData);
void LoadUserLastMode(BYTE B_ModeIndex);
void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture);
BYTE CheckEquipmentIntegrity(void);
BYTE LoadUserKeyMode(BYTE B_Data);
void LCD_Display(BYTE B_XStart, BYTE B_YStart, char* pStr);

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
//#define SLOTBOARDINDEX 0;
static BYTE	gB_IICBusIndex = 0, gB_SlotIndex = 0, gB_InputFPGASetWindowsLayoutFlag = 0;
static WORD	gW_DisplayScreenWindowsNum = 0;
static WORD W_DataArry[USER_COMMAD_BUF_LENGTH] = {0};

extern BYTE gB_IICBusIndex, gB_SlotIndex , gB_InputFPGASetWindowsLayoutFlag ;
extern WORD gW_DisplayScreenWindowsNum ;
extern WORD W_DataArry[USER_COMMAD_BUF_LENGTH];
ST_SignalSource *Stp_TempSignalSource =  NULL;//Stp_TempSignalWindows
ST_SignalWindows *Stp_TempSignalWindows= NULL;

static XScalerAperture St_UserCommadAperture;
static char DevStr[40] = {0}, OperationStr[40] = {0};

static char UserCommadSendStr[USER_COMMAD_SEND_STR_LEN] = {0};
static char UserCommadSendStr1[USER_COMMAD_SEND_STR1_LEN] = {0};


static ST_SignalSource St_UserCommadSignalSource;
static ST_SignalWindows St_UserCommadSignalWin;

static BYTE gB_BrillViewType1PacketStartFlag = 0;
static BYTE gB_BrillViewType1PacketRecvFlag = 0;
static BYTE	gB_BrillViewType1CommadIndex=0;
#define BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH		64
static BYTE	gB_BrillViewType1CommadBuf[BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH];
static BYTE gB_KeyDisplayMode = 0;

//static BYTE	gB_LCD_DisplayBuf[USER_COMMAD_BUF_LENGTH];
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static BYTE ChangeBrillViewType1CommadToSelfCommad(BYTE* pCommad);
static BYTE TCP_SendData	(BYTE *Bp_Buf, int W_Len);

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
static BYTE IsNumeral(char ch)
{
	if(ch >= 0x30 && ch <= 0x39)
		return 1;
	else if(ch >= 0x41 && ch <= 0x46)
		return 1;
	else if(ch >= 0x61 && ch <= 0x66)
		return 1;
	else
		return 0;
}


////
BYTE Serial_SendData(BYTE *Bp_Buf, int W_Len)
{
	printf("%s", Bp_Buf);
	return 1;
}

////
void SendCmdData(BYTE B_CommandSource)
{
		if(B_CommandSource == 0)
			Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));//the function has no action ???
		else
			TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
}


//接收串口的命令，并解析转发给fpga 包括对邋邋FPGA的操作和对输入输出板的操作
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ExcuteUserCommad(BYTE B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData)
{
	WORD W_Data1 = 0;
	BYTE B_TempData = 0;
	int  i = 0, TempData1 = 0;
	WORD W_Data2 = 0;
	//ST_SignalWindows *Stp_TempSignalWindows = NULL;
	//ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_ScalerType *Stp_TempScaler = NULL;


//一 CMD:input FPGA
	if(strcmp(pDevice, "INPUT_FPGA") == 0)
	{
		//cmd1.1 input fpga read:
		if((strcmp(pOperation, "READ") == 0) && (DataNum >= 1))
		{
			W_Data1 = InputFPGA_ReadWord(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA READ %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], W_Data1);

			SendCmdData(B_CommandSource);
			#if(USER_COMMAD_DEBUG == 1)	//==0
				printf("Board %d, IIC %d, Input FPGA RD [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], W_Data1);
			#endif
			return;
		}

		//cmd1.2 input fpga write
		if((strcmp(pOperation, "WRITE") == 0) && (DataNum >= 2))
		{
			InputFPGA_WriteWord(pData[0], pData[1]);//(WORD W_Addr, WORD W_Data)
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA WRITE %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], pData[1]);

			SendCmdData(B_CommandSource);
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, Input FPGA WR [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1]);
			#endif
			return;
		}

		//cmd1.3 input fpga video output disable
		if((strcmp(pOperation, "VIDEO_OUTPUT_DISALBLE") == 0) && (DataNum >= 1))
		{
			if(pData[0] < 4)
				InputFPGASetVideoOutputDisalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputDisalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_DISALBLE ERROR!\r\n");

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA VIDEO_OUTPUT_DISALBLE %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
		
		}

		//cmd1.4 input fpga video output enable
		if((strcmp(pOperation, "VIDEO_OUTPUT_ENABLE") == 0) && (DataNum >= 1))
		{
			if(pData[0] < 4)
				InputFPGASetVideoOutputEnalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputEnalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_ENABLE ERROR!\r\n");
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA VIDEO_OUTPUT_ENABLE %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
				
			return;
		}


		//cmd1.5 input fpga clear output windows
		if((strcmp(pOperation, "CLEAR_OUTPUT_WIN") == 0) && (DataNum >= 1))
		{
			W_Data2 = pData[0];
			if(InputFPGAClearBackgroundWindow(pData[0]) == 0xFF)
				printf("INPUT_FPGA CLEAR_OUTPUT_WIN ERROR!\r\n");

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_OUTPUT_WIN %d\r\n", pData[0]);

			SendCmdData(B_CommandSource);
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
				#endif
				return;
		}


		//cmd1.6 input fpga set background window
		if((strcmp(pOperation, "SET_BACKGROUND_WINDOW") == 0) && (DataNum >= 13))
		{
			memset(&St_UserCommadSignalSource, 0, sizeof(St_UserCommadSignalSource));
			memset(&St_UserCommadSignalWin, 0, sizeof(St_UserCommadSignalWin));
			St_UserCommadSignalWin.Stp_SignalSource = &St_UserCommadSignalSource;
			St_UserCommadSignalSource.HozScale = 0;
			St_UserCommadSignalSource.VertScale = 0;
			St_UserCommadSignalSource.Windows_Layer = 1;
			St_UserCommadSignalSource.SignalSource = pData[3];
			St_UserCommadSignalSource.SignalSourceType = 0;
			St_UserCommadSignalSource.WindowsProperty = pData[4];
			St_UserCommadSignalWin.SignalInputWindow.left = pData[5];
			St_UserCommadSignalWin.SignalInputWindow.top = pData[6];
			St_UserCommadSignalWin.SignalInputWindow.right = pData[7];
			St_UserCommadSignalWin.SignalInputWindow.bottom = pData[8];
			St_UserCommadSignalWin.SignalDisplayWindow.left = pData[9];
			St_UserCommadSignalWin.SignalDisplayWindow.top = pData[10];
			St_UserCommadSignalWin.SignalDisplayWindow.right = pData[11];
			St_UserCommadSignalWin.SignalDisplayWindow.bottom = pData[12];
			B_TempData = InputGraphicFPGAGetSlotBoardBmgBaseType(gB_SlotIndex);
			if(InputFPGASetBackgroundWindow(pData[0], pData[1], pData[2], B_TempData, St_UserCommadSignalWin) == OK)
			{
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, InputFPGASetBackgroundWindow Ok!\r\n", gB_SlotIndex, gB_IICBusIndex);
			#endif
				;
			}
			else
				printf("Board %d, IIC %d, InputFPGASetBackgroundWindow ERROR!\r\n", gB_SlotIndex, gB_IICBusIndex);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_BACKGROUND_WINDOW\r\n");
			SendCmdData(B_CommandSource);
			return;
		}


		//cmd1.7 input fpga soft reset
		if((strcmp(pOperation, "SOFT_RESET") == 0) && (DataNum >= 1))
		{
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, pData[0]);
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0x00);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SOFT_RESET %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);

			return;
		}


		//cmd1.8 input fpga scaler setup
		if((strcmp(pOperation, "SCALER_SETUP") == 0) && (DataNum >= 11))
		{
			if(pData[0] > 3)
				return;
			memset(&St_UserCommadAperture, 0, sizeof(St_UserCommadAperture));
			St_UserCommadAperture.InHoriSize = pData[3];						// < Horizontal size of the input video
			St_UserCommadAperture.InVertSize = pData[4];						// < Vertical size of the input video
			St_UserCommadAperture.InFirstPixel = pData[5];							// < The first pixel index in the input video
			St_UserCommadAperture.InLastPixel = pData[6];						// < The last pixel index in the input video
			St_UserCommadAperture.InFirstLine = pData[7];							// < The first line index in the input video
			St_UserCommadAperture.InLastLine = pData[8];						// < The last line index in the input video
			St_UserCommadAperture.OutHoriSize = pData[9];						// < Horizontal size of the output video
			St_UserCommadAperture.OutVertSize = pData[10];						// < Vertical size of the output video
			i = pData[1]; 
			Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
			Stp_TempScaler->B_SignalSourceSlot = W_DataArry[2];
			Stp_TempScaler->InHoriSize = W_DataArry[3];
			Stp_TempScaler->InVertSize = W_DataArry[4];
			Stp_TempScaler->InFirstPixel = W_DataArry[5];
			Stp_TempScaler->InLastPixel = W_DataArry[6];
			Stp_TempScaler->InFirstLine = W_DataArry[7];
			Stp_TempScaler->InLastLine = W_DataArry[8];
			Stp_TempScaler->OutHoriSize = W_DataArry[9];
			Stp_TempScaler->OutVertSize = W_DataArry[10];

			if((W_DataArry[2] == 0) && (gB_CVBSSwitchEnable == 1))
				GetInputBoardSignalResolution(pData[2], gB_CVBSSwitchIndex, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
			else
				GetInputBoardSignalResolution(pData[2], 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
			W_Data1 = Stp_TempScaler->InLastPixel - Stp_TempScaler->InFirstPixel + 1;
			W_Data2 = Stp_TempScaler->InLastLine - Stp_TempScaler->InFirstLine + 1;
			if((pData[1] == 0) && (W_Data1 > gW_ResolutionWidth) && (W_Data2 > gW_ResolutionHeight))
			{
				gW_ResolutionWidth <<= 1;
				gW_ResolutionHeight <<= 1;
				Stp_TempScaler->B_SignalSource = 1;
			}
			else 
				Stp_TempScaler->B_SignalSource = 0;
			if((pData[2] < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 1);
			else
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 0);
			InputFPGA_ScalerSetup(pData[1], pData[2] << 1, &St_UserCommadAperture);
			
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SCALER_SETUP %d %d %d\r\n", pData[0], pData[1], pData[2]);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT BOARD SCALER SETUP OK!\r\n");
			#endif

			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
			for(i = 0; i < (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX); i++)
				ParamWrite(W_Data1 + i, pData[i]);
			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 = (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX)) + NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < 11; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif

			return;
		}


		//cmd1.9 input fpga scaler init
		if((strcmp(pOperation, "SCALER_INIT") == 0) && (DataNum >= 1))
		{
			if(pData[0] < 4)
				InputFPGA_ScalerInit(pData[0]);
		}

		//cmd1.10 input fpga scaler read all	
		if((strcmp(pOperation, "SCALER_READ_ALL") == 0) && (DataNum >= 1))
		{
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, Scaler %d, Read All\r\n", gB_SlotIndex, pData[0]);
			#endif
			InputFPGA_PrintScalerVar(pData[0]);
			return;
		}


		//cmd1.11 input fpga scaler read word
		if((strcmp(pOperation, "SCALER_READ_WORD") == 0) && (DataNum >= 2))
		{
			W_Data1 = InputFPGA_ScalerReadWord(pData[0], pData[1]);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SCALER_READ_WORD %d %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], pData[1], W_Data1);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
				printf("Slot %d, IIC %d, SCALER_READ %d, 0x%08X 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1], W_Data1);
			#endif
			return;
		}


		//cmd1.12 input fpga set key mode
		if(((strcmp(pOperation, "SET_KEY_MODE") == 0) || (strcmp(pOperation, "SKM") == 0)) && (DataNum >= 1))
		{
			if((pData[0] > 0) && (pData[0] < DISPLAY_MODE_INDEX_MAX))
				gB_KeyDisplayMode = pData[0];
			else
				gB_KeyDisplayMode = 0;

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			SendCmdData(B_CommandSource);
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			#endif
			return;
		}


		//cmd1.13 input fpga set test pattern collor
		if(((strcmp(pOperation, "SET_TEST_PATTERN_COLOR") == 0) || (strcmp(pOperation, "STPC") == 0)) && (DataNum > 2))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			TempData1 = InputFPGA_SetTestPatternColor(pData[0], pData[1], pData[2]);
			if(TempData1 == 0xFF)
				sprintf(UserCommadSendStr, "INPUT_FPGA SET_TEST_PATTERN_COLOR %d %d 0x%08X Error\r\n", pData[0], pData[1], pData[2]);
			else
				sprintf(UserCommadSendStr, "INPUT_FPGA SET_TEST_PATTERN_COLOR %d %d 0x%08X\r\n", pData[0], pData[1], pData[2]);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
			printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd11.14 input fpga load display mode
		if(((strcmp(pOperation, "LOAD_DISPLAY_MODE") == 0) || (strcmp(pOperation, "LDM") == 0)) && (DataNum >= 1))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			W_Data1 = pData[0];
			if((W_Data1 > 0) && (W_Data1 < DISPLAY_MODE_INDEX_MAX))
			{
				LoadUserLastMode(W_Data1);
				sprintf(UserCommadSendStr, "INPUT_FPGA LOAD_DISPLAY_MODE %d\r\n", W_Data1);
				#if(USER_COMMAD_DEBUG == 1)
				printf("INPUT_FPGA LOAD_DISPLAY_MODE %d\r\n", W_Data1);
				#endif
			}
			else
			{
				W_Data1 = 255;
				sprintf(UserCommadSendStr, "INPUT_FPGA LOAD_DISPLAY_MODE %d\r\n", W_Data1);
				#if(USER_COMMAD_DEBUG == 1)
				printf("INPUT_FPGA LOAD_DISPLAY_MODE %d\r\n", W_Data1);
				#endif
			}
			SendCmdData(B_CommandSource);
			
			return;
		}


		//cmd1.15 input fpga set windows layout
		if(((strcmp(pOperation, "SET_WINDOWS_LAYOUT") == 0) || (strcmp(pOperation, "SWL") == 0)) && (DataNum >= 4))
		{
			if(pData[1] >= OUTPUT_BOARD_MAX)
				return;
			ScreenWindowLayoutInit(pData[1]);

			gB_InputFPGASetWindowsLayoutFlag = 1;
			gStp_ScreenRectInput->ActiveRectWindowsNum = 0;
			gStp_ScreenRectInput->B_BoardSlotIndex = pData[0];
			gStp_ScreenRectInput->B_OutputScreenIndex = pData[1];
			gStp_ScreenRectInput->DisplayScreenWidth = pData[2];
			gStp_ScreenRectInput->DisplayScreenHeight = pData[3];
			gW_DisplayScreenWindowsNum = 0;
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_WINDOWS_LAYOUT %d %d %d %d\r\n",
																	gStp_ScreenRectInput->B_BoardSlotIndex,
																	gStp_ScreenRectInput->B_OutputScreenIndex,
																	gStp_ScreenRectInput->DisplayScreenWidth,
																	gStp_ScreenRectInput->DisplayScreenHeight);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT_FPGA SET_WINDOWS_LAYOUT: BoardSlotIndex = %d, OutputScreenIndex = %d, ScreenWidth = %d, ScreenHeight = %d\r\n",
																	gStp_ScreenRectInput->B_BoardSlotIndex,
																	gStp_ScreenRectInput->B_OutputScreenIndex,
																	gStp_ScreenRectInput->DisplayScreenWidth,
																	gStp_ScreenRectInput->DisplayScreenHeight);
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_Display0_WINDOWS_BOARD_INDEX + (gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
			for(i = 0; i < 4; i++)
				ParamWrite(W_Data1 + i, pData[i]);
			
			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < 4; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}


		//cmd1.16 input fpga add windows
		if(((strcmp(pOperation, "ADD_WINDOWS") == 0) || (strcmp(pOperation, "AW") == 0)) && (DataNum >= 10))
		{
			if(gB_InputFPGASetWindowsLayoutFlag == 0)
				return;
			if(gW_DisplayScreenWindowsNum >= MaxDiaplayWallWindowsNum)
			{
				gStp_ScreenRectInput->ActiveRectWindowsNum = gW_DisplayScreenWindowsNum;
				ExecuteScreenWindowLayout();
				return;
			}
			Stp_TempSignalWindows = gStp_ScreenRectInput->Stp_RectWindows[gW_DisplayScreenWindowsNum];
			Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
			if((Stp_TempSignalWindows == NULL) || (gW_DisplayScreenWindowsNum >= MaxDiaplayWallWindowsNum))
				return;
			/*
			GetInputBoardSignalResolution(pData[0], 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
			if((pData[0] < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
			{
				Stp_TempSignalSource->SignalSource = 8;
			}
			else
				Stp_TempSignalSource->SignalSource = pData[0];
			*/
			Stp_TempSignalSource->SignalSource = pData[0];
			Stp_TempSignalSource->SignalSourceSlot = pData[0];
			Stp_TempSignalSource->OriginalSignalSource = pData[0];
			Stp_TempSignalSource->Windows_Layer = pData[1];
			Stp_TempSignalWindows->SignalInputWindow.left = pData[2];
			Stp_TempSignalWindows->SignalInputWindow.top = pData[3];
			Stp_TempSignalWindows->SignalInputWindow.right = pData[4];
			Stp_TempSignalWindows->SignalInputWindow.bottom = pData[5];
			Stp_TempSignalWindows->SignalDisplayWindow.left = pData[6];
			Stp_TempSignalWindows->SignalDisplayWindow.top = pData[7];
			Stp_TempSignalWindows->SignalDisplayWindow.right = pData[8];
			Stp_TempSignalWindows->SignalDisplayWindow.bottom = pData[9];
			gW_DisplayScreenWindowsNum++;
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d\r\n",
																	Stp_TempSignalSource->SignalSource,
																	Stp_TempSignalSource->Windows_Layer,
																	Stp_TempSignalWindows->SignalInputWindow.left,
																	Stp_TempSignalWindows->SignalInputWindow.top,
																	Stp_TempSignalWindows->SignalInputWindow.right,
																	Stp_TempSignalWindows->SignalInputWindow.bottom,
																	Stp_TempSignalWindows->SignalDisplayWindow.left,
																	Stp_TempSignalWindows->SignalDisplayWindow.top,
																	Stp_TempSignalWindows->SignalDisplayWindow.right,
																	Stp_TempSignalWindows->SignalDisplayWindow.bottom);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
			printf("SET_WINDOWS_LAYOUT: Source = %d, Layer = %d, InLeft = %d, InTop = %d, InRight = %d, InBottom = %d, OutLeft = %d, OutTop = %d, OutRight = %d, OutBottom = %d\r\n",
																	Stp_TempSignalSource->SignalSource,
																	Stp_TempSignalSource->Windows_Layer,
																	Stp_TempSignalWindows->SignalInputWindow.left,
																	Stp_TempSignalWindows->SignalInputWindow.top,
																	Stp_TempSignalWindows->SignalInputWindow.right,
																	Stp_TempSignalWindows->SignalInputWindow.bottom,
																	Stp_TempSignalWindows->SignalDisplayWindow.left,
																	Stp_TempSignalWindows->SignalDisplayWindow.top,
																	Stp_TempSignalWindows->SignalDisplayWindow.right,
																	Stp_TempSignalWindows->SignalDisplayWindow.bottom);
			#endif

			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_Display0_WINDOW0_SIGNAL_SOURCE + (gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
			W_Data1 += ((gW_DisplayScreenWindowsNum - 1) * (NVR_Display0_WINDOW1_IN_LEFT - NVR_Display0_WINDOW0_IN_LEFT));
			for(i = 0; i < (NVR_Display0_WINDOW1_IN_LEFT - NVR_Display0_WINDOW0_IN_LEFT); i++)
				ParamWrite(W_Data1 + i, pData[i]);

			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < 10; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}

		//cmd1.17 input fpga add windows end
		if((strcmp(pOperation, "ADD_WINDOWS_END") == 0) || (strcmp(pOperation, "AWE") == 0))
		{
			if(gB_InputFPGASetWindowsLayoutFlag == 0)
				return;
			if(gW_DisplayScreenWindowsNum > 0)
			{
				#if(USER_COMMAD_DEBUG == 1)
				printf("INPUT_FPGA ADD_WINDOWS_END: gW_DisplayScreenWindowsNum = %d\r\n", gW_DisplayScreenWindowsNum);
				#endif
				
				gStp_ScreenRectInput->ActiveRectWindowsNum = gW_DisplayScreenWindowsNum;
				ExecuteScreenWindowLayout();
				gB_InputFPGASetWindowsLayoutFlag = 0;
				g_DisplayModeIndex = 0;
				ParamWrite (NVR_DISPLAY_MODE_INDEX, g_DisplayModeIndex);
				memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
				sprintf(UserCommadSendStr, "INPUT_FPGA ADD_WINDOWS_END\r\n");

				SendCmdData(B_CommandSource);
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM);
				ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
					ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				}
//				gB_KeyboardData = 0;
				#endif
			}
			return;
		}



		//cmd1.18 input fpga set updata system display
		if((strcmp(pOperation, "SET_UPDATA_SYSTEM_DISPLAY") == 0) || (strcmp(pOperation, "SUSD") == 0))
		{
			InputFPGASetUpdata();
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY\r\n");
			SendCmdData(B_CommandSource);
			return;
		}



		//cmd1.19 input fpga clear scaler use
		if(((strcmp(pOperation, "CLEAR_SCALER_USE") == 0) || (strcmp(pOperation, "CSUSE") == 0)) && (DataNum >= 1))
		{
			InputGraphicFPGAClearScalerUse(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_SCALER_USE %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
			return;
		}


		//cmd1.20 input fpga clear output window
		if(((strcmp(pOperation, "CLEAR_OUTPUT_WINDOW") == 0) || (strcmp(pOperation, "COWIN") == 0)) && (DataNum >= 2))
		{
			W_Data2 = pData[1];
			InputGraphicFPGAClearOutputWindow(pData[0], pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_OUTPUT_WINDOW %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
	}

//=============二、对主板的操作=====
	if(strcmp(pDevice, "BOARD") == 0)
	{
		//cmd2.1 board read id 
		if((strcmp(pOperation, "READ_ID") == 0) && (DataNum >= 1))
		{
			B_TempData = ReadBoardID(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD READ_ID %d 0x%02X\r\n", pData[0], B_TempData);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.2 board slot reset
		if((strcmp(pOperation, "SLOT_RESET") == 0) && (DataNum >= 1))
		{
			BoardSlotReset(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SLOT_RESET %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.3 board param reset
		if((strcmp(pOperation, "PARAM_RESET") == 0) && (DataNum >= 1))
		{
			if(pData[0] == 1)
			{
				/*
				ParamWrite(NVR_CHECKSUM, 0);		// write checksum
				ParamInit();
				DevicesInit();
				LoadUserLastMode(0);
				*/
				//g_CurrentState = State_FactoryReset_OK;
				clearlcd();
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_waitStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_waitStrEn);
				if(ResetPara())
				{
					clearlcd();
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x80, g_ResetSuccessStr);
					else  if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x80, "Reset Success!");
					delay(5);
				}
				LightInputSrcLED();
				//g_CurrentState=State_MainMenu_Menu;
				delay(20);
				ReturnMainWin();
			
				
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_RESET %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.4 board param read
		if((strcmp(pOperation, "PARAM_READ") == 0) && (DataNum >= 1))
		{
			W_Data1 = ParamRead(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_READ 0x%04X 0x%04X\r\n", pData[0], W_Data1);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.5 board param write
		if((strcmp(pOperation, "PARAM_WRITE") == 0) && (DataNum >= 2))
		{
			ParamWrite(pData[0], pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_WRITE 0x%04X 0x%04X\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.6 board select slot
		if((strcmp(pOperation, "SELECT_SLOT") == 0) && (DataNum >= 1))
		{
			ReadBoardIDInit();
			SelectBoardSlot(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SELECT_SLOT %d\r\n", pData[0]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.7 board dvi hpd set
		if((strcmp(pOperation, "DVI_HPD_SET") == 0) && (DataNum >= 2))
		{
			W_Data1 = pData[0];
			BoardDVIHPDSetSwitch(W_Data1, 0);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD DVI_HPD_SET %d\r\n", W_Data1);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			InputFPGA_WriteWord(INPUT_FPGA_SPI_CS_SET, 1);						// set high (DVI_IN_HPD_CON )
			if((pData[1] > 50) || (pData[1] <= 0))
				Delay100ms(2);
			else
				Delay100ms(pData[1]);
			InputFPGA_WriteWord(INPUT_FPGA_SPI_CS_SET, 0);           	// set low (DVI_IN_HPD_CON )
			return;
		}


		//cmd2.8 board set clk
		if((strcmp(pOperation, "SET_CLK") == 0) && (DataNum >= 1))
		{
			/*
			SetBoardIDCLK(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SET_CLK %d\r\n", pData[0]);
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			*/
			return;
		}

	
		//cmd2.9 board adv7604 program edid
		if((strcmp(pOperation, "ADV7604_PROGRAM_EDID") == 0) && (DataNum > 1))
		{
			InputBoardADV7604ProgramEDID((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_PROGRAM_EDID %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_PROGRAM_EDID %d\r\n", pData[0]);
			#endif
			return;
		}


		//cmd2.10 board adv7604 select input port
		if((strcmp(pOperation, "ADV7604_SELECT_INPUT_PORT") == 0) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputPort((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SELECT_INPUT_PORT %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/	
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_INPUT_PORT %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}


		//cmd2.11 board 7604 select timing
		if((strcmp(pOperation, "ADV7604_SELECT_TIMINGS") == 0) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputTiming((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SELECT_TIMINGS %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_TIMINGS %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}

		//cmd2.12 board 7604 status
		if((strcmp(pOperation, "ADV7604_LOG_STATUS") == 0) && (DataNum > 0))
		{
			InputBoardADV7604PrintLogStatus((BYTE) (pData[0]));

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_LOG_STATUS %d\r\n", pData[0]);

			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_LOG_STATUS %d\r\n", pData[0]);
			#endif
			return;
		}


		//cmd2.13 board 7604 set ctrl
		if((strcmp(pOperation, "ADV7604_SET_CTRL") == 0) && (DataNum > 2))
		{
			InputBoardADV7604SetCtrl((BYTE) (pData[0]), pData[1], pData[2]);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SET_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);

			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}


		//cmd2.14 7604 get ctrl
		if((strcmp(pOperation, "ADV7604_GET_CTRL") == 0) && (DataNum >= 2))
		{
			W_Data1 = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (pData[0] * 4) + pData[1]);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_GET_CTRL %d %d %d\r\n", pData[0], pData[1], W_Data1);

			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_GET_CTRL %d %d %d\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}


		//cmd2.15 76 04 set dv ctrl
		if((strcmp(pOperation, "ADV7604_SET_DV_CTRL") == 0) && (DataNum > 2))
		{
			InputBoardADV7604SetDVCtrl((BYTE) (pData[0]), pData[1], pData[2]);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SET_DV_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);

			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_DV_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}


		//cmd2.16 5158 set ctrl
		if((strcmp(pOperation, "TVP5158_SET_CTRL") == 0) && (DataNum >= 4))
		{
			if(pData[2] == 0)
			{
				InputBoardTVP5158SetLuminanceBrightness(pData[0], pData[1], pData[3]);
				ParamWrite(NVR_TVP5158_DEV0_PORT0_BRIGHTNESS + (pData[0] * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)), (BYTE) pData[3]);
			}
			if(pData[2] == 1)
			{
				InputBoardTVP5158SetLuminanceContrast(pData[0], pData[1], pData[3]);
				ParamWrite(NVR_TVP5158_DEV0_PORT0_CONTRAST + (pData[0] * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)), (BYTE) pData[3]);
			}
			if(pData[2] == 2)
			{
				InputBoardTVP5158SetLuminanceSaturation(pData[0], pData[1], pData[3]);
				ParamWrite(NVR_TVP5158_DEV0_PORT0_SATURATION + (pData[0] * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)), (BYTE) pData[3]);
			}
			if(pData[2] == 3)
			{
				InputBoardTVP5158SetLuminanceHue(pData[0], pData[1], pData[3]);
				ParamWrite(NVR_TVP5158_DEV0_PORT0_HUE + (pData[0] * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_HUE - NVR_TVP5158_DEV0_PORT0_HUE)), (BYTE) pData[3]);
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD TVP5158_SET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[3]);

			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_SET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[2]);
			#endif
			return;
		}



		//cmd2.17 5158 get ctrl
		if((strcmp(pOperation, "TVP5158_GET_CTRL") == 0) && (DataNum >= 3))
		{
			#if 1
			if(pData[2] == 0)
				W_Data1 = ParamRead(NVR_TVP5158_DEV0_PORT0_BRIGHTNESS + (pData[0] * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)));
			else if(pData[2] == 1)
				W_Data1 = ParamRead(NVR_TVP5158_DEV0_PORT0_CONTRAST + (pData[0] * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)));
			else if(pData[2] == 2)
				W_Data1 = ParamRead(NVR_TVP5158_DEV0_PORT0_SATURATION + (pData[0] * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)));
			else if(pData[2] == 3)
				W_Data1 = ParamRead(NVR_TVP5158_DEV0_PORT0_HUE + (pData[0] * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_HUE - NVR_TVP5158_DEV0_PORT0_HUE)));
			else
				W_Data1 = 0xFFFF;
			#else
			if(pData[2] == 0)
				W_Data1 = InputBoardTVP5158GetLuminanceBrightness(pData[0], pData[1]);
			else if(pData[2] == 1)
				W_Data1 = InputBoardTVP5158GetLuminanceContrast(pData[0], pData[1]);
			else if(pData[2] == 2)
				W_Data1 = InputBoardTVP5158GetLuminanceSaturation(pData[0], pData[1]);
			else if(pData[2] == 3)
				W_Data1 = InputBoardTVP5158GetLuminanceHue(pData[0], pData[1]);
			else
				W_Data1 = 0xFFFF;
			#endif
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD TVP5158_GET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], W_Data1);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_GET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], W_Data1);
			#endif
			return;
		}


		//cmd2.18 7601 read
		if((strcmp(pOperation, "GV7601_READ") == 0) && (DataNum > 0))
		{
//			SPI_Init_16Bit();
			Board_CPU_SPI_BusSwitch(pData[0]);
			GV7601_GSPI_Init();
			GV7601_ReadWord(pData[0], pData[1], &W_Data1);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GV7601 %d READ [0x%04X = 0x%04X]\r\n", pData[0], pData[1], W_Data1);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d READ [0x%04X = 0x%04X]\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}


		//cmd2.19 7601 write
		if((strcmp(pOperation, "GV7601_WRITE") == 0) && (DataNum > 0))
		{
//			SPI_Init_16Bit();
			Board_CPU_SPI_BusSwitch(pData[0]);
			GV7601_GSPI_Init();
			GV7601_WriteWord(pData[0], pData[1], pData[2]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GV7601 %d WRITE [0x%04X = 0x%04X]\r\n", pData[0], pData[1], pData[2]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d WRITE [0x%04X = 0x%04X]\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}


		//cmd2.20 board read message
		if((strcmp(pOperation, "READ_MESSAGE") == 0) || (strcmp(pOperation, "RMG") == 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD READ_MESSAGE: ");

			if(gSt_InputBoardADV7604.B_ActiveChip > 0)
			{
				memset(UserCommadSendStr1, 0, sizeof(UserCommadSendStr1));
				sprintf(UserCommadSendStr1, "DVI_IN 0x%02X ", gSt_InputBoardADV7604.B_ActiveChip);
				if((strlen(UserCommadSendStr) + strlen(UserCommadSendStr1)) < sizeof(UserCommadSendStr))
					strcat(UserCommadSendStr, UserCommadSendStr1);
				else
					printf("ERROR: UserCommadSendStr Overflow!\r\n");
			}

			if(gSt_InputBoardGV7601.B_ActiveChip > 0)
			{
				memset(UserCommadSendStr1, 0, sizeof(UserCommadSendStr1));
				sprintf(UserCommadSendStr1, "SDI_IN 0x%02X ", gSt_InputBoardGV7601.B_ActiveChip);
				if((strlen(UserCommadSendStr) + strlen(UserCommadSendStr1)) < sizeof(UserCommadSendStr))
					strcat(UserCommadSendStr, UserCommadSendStr1);
				else
					printf("ERROR: UserCommadSendStr Overflow!\r\n");
			}
			#if 0
			if(gSt_InputBoarTVP5158.B_ActiveChip > 0)
			{
				memset(UserCommadSendStr1, 0, sizeof(UserCommadSendStr1));
				sprintf(UserCommadSendStr1, "VIDEO_IN 0x%02X ", gSt_InputBoarTVP5158.B_ActiveChip * 4);
				if((strlen(UserCommadSendStr) + strlen(UserCommadSendStr1)) < sizeof(UserCommadSendStr))
					strcat(UserCommadSendStr, UserCommadSendStr1);
				else
					printf("ERROR: UserCommadSendStr Overflow!\r\n");
			}
			#endif

			if(gSt_OutputBoarSiI9136.B_ActiveChip > 0)
			{
				memset(UserCommadSendStr1, 0, sizeof(UserCommadSendStr1));
				sprintf(UserCommadSendStr1, "HDMI_OUT 0x%02X ", gSt_OutputBoarSiI9136.B_ActiveChip);
				if((strlen(UserCommadSendStr) + strlen(UserCommadSendStr1)) < sizeof(UserCommadSendStr))
					strcat(UserCommadSendStr, UserCommadSendStr1);
				else
					printf("ERROR: UserCommadSendStr Overflow!\r\n");
			}

			if((strlen(UserCommadSendStr) + strlen("\r\n")) < sizeof(UserCommadSendStr))
				strcat(UserCommadSendStr, "\r\n");
			else
			{
				printf("ERROR: UserCommadSendStr Overflow!\r\n");
				return;
			}
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			return;
		}


		//cmd2.21 board read input source resolutions
		if(((strcmp(pOperation, "READ_INPUT_SOURCE_RESOLUTION") == 0) || (strcmp(pOperation, "RISR") == 0)) && (DataNum > 1))
		{
			GetInputBoardSignalResolution(pData[0], pData[1], &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
//			InputBoardGV7601PrintSignalStd(pData[0]);
//			InputBoardADV7604PrintLogStatus(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD READ_INPUT_SOURCE_RESOLUTION %d %d %d %d %d %d\r\n", pData[0], pData[1], gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight);
			SendCmdData(B_CommandSource);
			
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD READ_INPUT_SOURCE_RESOLUTION %d %d %d %d %d %d\r\n", pData[0], pData[1], gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight);
			#endif
			return;
		}


		//cmd2.22 board set input video switch
		if((strcmp(pOperation, "SET_INPUT_VIDEO_SWITCH") == 0) && (DataNum > 1))
		{
			if(gB_CVBSSwitchEnable == 1)
			{
				if(pData[0] <= 5)
					ParamWrite(NVR_INPUT0_VIDEO_SWITCH + pData[0], pData[1]);
				if(pData[0] == 0)
				{
					W_Data1 = InputFPGA_ReadWord(INPUT_FPGA_SYSTEM_CVBS_TYPE) & 0x0F;
					W_Data1 |= (pData[1] & 0x0F) << 8;
					InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_Data1);
					gB_CVBSSwitchIndex = pData[1];
				}
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD SET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}


		//cmd2.23 board get input video switch
		if((strcmp(pOperation, "GET_INPUT_VIDEO_SWITCH") == 0) && (DataNum >= 1))
		{
			if(pData[0] == 0)
			{
				W_Data1 = InputFPGA_ReadWord(INPUT_FPGA_SYSTEM_CVBS_TYPE);
				W_Data1 >>= 8;
				W_Data1 &= 0x0F;
			}
			else
				W_Data1 = 0xFF;
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], W_Data1);
			SendCmdData(B_CommandSource);
			/*
			if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], W_Data1);
			#endif
			return;
		}


		//cmd2.24 board read slot message 
		if(((strcmp(pOperation, "READ_SLOT_MESSAGE") == 0) || (strcmp(pOperation, "RSMG") == 0)) && (DataNum > 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			if(pData[0] < OUTPUT_BOARD_MAX)
			{
				B_TempData = GetBoardSlotType(pData[0]);
				sprintf(UserCommadSendStr, "BOARD READ_SLOT_MESSAGE %d %d\r\n", pData[0], B_TempData);
				SendCmdData(B_CommandSource);
				/*
				if(B_CommandSource == 0)
					Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				*/
			}
			else if(pData[0] == 0x0F)
			{
				for(i = 0; i < (OUTPUT_BOARD_MAX); i++)
				{
					memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
					B_TempData = GetBoardSlotType(i);
					sprintf(UserCommadSendStr, "BOARD READ_SLOT_MESSAGE %d %d\r\n", i, B_TempData);
					SendCmdData(B_CommandSource);
					/*
					if(B_CommandSource == 0)
						Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
					else
						TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
					*/
				}
			}
			else
				printf("BOARD READ_SLOT_MESSAGE ERROR!\r\n");
			return;
		}


		//cmd2.25 board set output clk
		if((strcmp(pOperation, "SET_OUTPUT_CLK") == 0) && (DataNum >= 2))
		{
			BoardIICBusSwitch(7);
			Set_SL38160_CLK(DEVICEID_SL38160_17, pData[0], pData[1]);		// AUDIO_CLK: pData[0], VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
			BoardIICBusSwitch(0);																				// IIC Switch to default
			ParamWrite(NVR_CUSTOM_RES_CLK, pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_OUTPUT_CLK %d %d\r\n", pData[0], pData[1]);
			SendCmdData(B_CommandSource);
			/*if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}


		//cmd2.26 board set custom resolution
		if(((strcmp(pOperation, "SET_CUSTOM_RESOLUTION") == 0) || (strcmp(pOperation, "SCR") == 0)) && (DataNum >= 7))
		{
			if((g_TotalPixel != pData[1]) || (g_TotalLine != pData[2]) || (g_ScreenOutputWidth != pData[5]) || (g_ScreenOutputHeight != pData[6]))
			{
				InputFPGAClearBackgroundWindow(0x0F);
//				InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x0F);
				InputFPGASetCustomOutputResolution(pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6]);
				InputFPGASetVideoOutputEnalble(0x10F);
				InputFPGA_SoftReset(0xF0 | BIT0);
//				InputFPGASetVideoOutputEnalble(0x10F);
				InputFPGASetUpdata();
				ParamWrite(NVR_CUSTOM_RES_ENABLE, 1);
				ParamWrite(NVR_CUSTOM_VIDEO_MODE_INDEX, pData[0]);
				ParamWrite(NVR_CUSTOM_RES_TOTAL_PIXEL, pData[1]);
				ParamWrite(NVR_CUSTOM_RES_TOTAL_LINE, pData[2]);
				ParamWrite(NVR_CUSTOM_RES_H_START, pData[3]);
				ParamWrite(NVR_CUSTOM_RES_V_START, pData[4]);
				ParamWrite(NVR_CUSTOM_RES_ACTIVE_PIXEL, pData[5]);
				ParamWrite(NVR_CUSTOM_RES_ACTIVE_LINE, pData[6]);
			}

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SCR %d %d %d %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6]);
			SendCmdData(B_CommandSource);
			/*if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			return;
		}


		//cmd2.27 board get output resolution
		if((strcmp(pOperation, "GET_OUTPUT_RESOLUTION") == 0) || (strcmp(pOperation, "GOR") == 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GET_OUTPUT_RESOLUTION %d %d %d\r\n", g_ScreenOutputWidth, g_ScreenOutputHeight, g_VFreq);

			SendCmdData(B_CommandSource);
			/*if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			return;
		}


		//cmd2.28 board set output timing
		if(((strcmp(pOperation, "SET_OUTPUT_TIMING") == 0) || (strcmp(pOperation, "SOT") == 0)) && (DataNum >= 1))
		{
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			if(B_TempData != ((unsigned char) pData[0]))
			{
				InputFPGAClearBackgroundWindow(0x0F);
				B_TempData = InputGraphicFPGAChangeOutputTiming((unsigned char) pData[0]);
				ParamWrite(NVR_SCREEN_TIMING, (unsigned char) pData[0]);
//				W_Data1 = ParamRead(NVR_SWITCH_INPUT_PORT) & 0xFFFF;
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			if(B_TempData != 0xFF)
				sprintf(UserCommadSendStr, "BOARD SOT %d OK\r\n", pData[0]);
			else
				sprintf(UserCommadSendStr, "BOARD SOT %d ERROR\r\n", pData[0]);

			SendCmdData(B_CommandSource);
			/*if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			return;
		}


		//cmd2.29 board get output timing
		if((strcmp(pOperation, "GET_OUTPUT_TIMING") == 0) || (strcmp(pOperation, "GOT") == 0))
		{
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);

			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GET_OUTPUT_TIMING %d\r\n", B_TempData);

			SendCmdData(B_CommandSource);
			/*if(B_CommandSource == 0)
				Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			*/
			return;
		}
	}
}

#if(SERIAL_COMMAD_TYPE == 0)
void ParseSerialCommad(BYTE* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL, *pData1 = NULL;
	char DataFlag = 0;
	WORD i = 0, DataCount = 0, W_Data = 0;
	StrHead = (char*) pCommad;

	// Parse Device Name
	while(*StrHead == ' ')
		StrHead++;

	i = 0;
	if((*StrHead == '\r') || (*StrHead == '\n'))
		return;

	memset(DevStr, 0, sizeof(DevStr));

	while((*StrHead != '\r') && (*StrHead != '\n') && (*StrHead != ' '))
	{
		if((*StrHead >= ASCII_a) && (*StrHead <= ASCII_z))
			DevStr[i] = *StrHead - 0x20;
		else
			DevStr[i] = *StrHead;

		StrHead++;
		i++;

		if(i >= sizeof(DevStr))
		{
			printf("\r\n1 Command Error.\r\n");
			return;
		}
	}


	// Parse operation type
	while(*StrHead == ' ')
		StrHead++;

	if((*StrHead == '\r') || (*StrHead == '\n'))
	{
		printf("\r\n2 Command Error.\r\n");
		return;
	}

	i = 0;
	memset(OperationStr, 0, sizeof(OperationStr));
	while((*StrHead != '\r') && (*StrHead != '\n') && (*StrHead != ' '))
	{
		if((*StrHead >= ASCII_a) && (*StrHead <= ASCII_z))
			OperationStr[i] = *StrHead - 0x20;
		else
			OperationStr[i] = *StrHead;

		StrHead++;
		i++;
		if(i >= sizeof(OperationStr))
		{
			printf("\r\n3 Command Error.\r\n");
			return;
		}
	}
	// Parse command data
	while(*StrHead == ' ')
	{
		StrHead++;
	}
	if((*StrHead == '\r') || (*StrHead == '\n'))
		DataCount = 0;
	else
	{
		memset(W_DataArry, 0, sizeof(W_DataArry));
		DataCount = 0;
		pData0 = StrHead;
		pData1 = StrHead + 1;
		if((*pData0 == '0') && ((*pData1 == 'X') || (*pData1 == 'x')))
		{
			DataFlag = 1;
			StrHead += 2;
		}
		else
			DataFlag = 0;
		while((*StrHead != '\r') && (*StrHead != '\n'))
		{
			W_Data = 0;
			if(!IsNumeral(*StrHead))
			{
				printf("\r\n4 Command Error.\r\n");
				return;
			}
			while(IsNumeral(*StrHead))
			{
				if(DataFlag == 1)
				{
					W_Data *= 16;
					if((*StrHead) >= 0x30 && (*StrHead) <= 0x39)
						W_Data += (*StrHead) - 0x30;
					else if((*StrHead) >= 0x41 && (*StrHead) <= 0x46)
						W_Data += (*StrHead) - 0x41 + 10;
					else if((*StrHead) >= 0x61 && (*StrHead) <= 0x66)
						W_Data += (*StrHead) - 0x61 + 10;
					else
					{
						printf("\r\n5 Command Error.\r\n");
						return;
					}

				}
				else
				{
					W_Data *= 10;
					if((*StrHead) >= 0x30 && (*StrHead) <= 0x39)
						W_Data += (*StrHead) - 0x30;
					else
					{
						printf("\r\n6 Command Error.\r\n");
						return;
					}
				}
//				B_Data += *StrHead - 0x30;
				StrHead++;
			}
			W_DataArry[DataCount] = W_Data;
			DataCount++;
			if(DataCount >= sizeof(W_DataArry))
			{
				printf("\r\n7 Command Error.\r\n");
				return;
			}
			while(*StrHead == ' ')
				StrHead++;
			pData0 = StrHead;
			pData1 = StrHead + 1;
			if((*pData0 == '0') && ((*pData1 == 'X') || (*pData1 == 'x')))
			{
				DataFlag = 1;
				StrHead += 2;
			}
			else
				DataFlag = 0;
		}
	}
/*
	printf("\r\n%s %s ", DevStr, OperationStr);
	for(i = 0; i < DataCount; i++)
		printf("%d ", W_DataArry[i]);
	printf("\r\nDataCount: %d\r\n", DataCount);
*/
	#ifdef USE_TCP_DEBUG_SERIAL
	ExcuteUserCommad(1, DevStr, OperationStr, DataCount, W_DataArry);

	#else	//串口
	ExcuteUserCommad(0, DevStr, OperationStr, DataCount, W_DataArry);
	#endif
}
#endif




void ParseTCPCommad(BYTE* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL, *pData1 = NULL;
	char DataFlag = 0;
	WORD i = 0, DataCount = 0, W_Data = 0;
	StrHead = (char*) pCommad;
	// Parse Device Name
	while(*StrHead == ' ')
		StrHead++;
	i = 0;
	if((*StrHead == '\r') || (*StrHead == '\n'))
		return;
	memset(DevStr, 0, sizeof(DevStr));
	while((*StrHead != '\r') && (*StrHead != '\n') && (*StrHead != ' '))
	{
		if((*StrHead >= ASCII_a) && (*StrHead <= ASCII_z))
			DevStr[i] = *StrHead - 0x20;
		else
			DevStr[i] = *StrHead;
		StrHead++;
		i++;
		if(i >= sizeof(DevStr))
		{
			printf("\r\n1 Command Error.\r\n");
			return;
		}
	}
	// Parse operation type
	while(*StrHead == ' ')
		StrHead++;
	if((*StrHead == '\r') || (*StrHead == '\n'))
	{
		printf("\r\n2 Command Error. %s\r\n", DevStr);
		for(i = 0; *StrHead == '\n'; i++, StrHead++)
			printf("0x%02X ", *StrHead);
		printf("\r\n");
		return;
	}
	i = 0;
	memset(OperationStr, 0, sizeof(OperationStr));
	while((*StrHead != '\r') && (*StrHead != '\n') && (*StrHead != ' '))
	{
		if((*StrHead >= ASCII_a) && (*StrHead <= ASCII_z))
			OperationStr[i] = *StrHead - 0x20;
		else
			OperationStr[i] = *StrHead;
		StrHead++;
		i++;
		if(i >= sizeof(OperationStr))
		{
			printf("\r\n3 Command Error.\r\n");
			return;
		}
	}
	// Parse command data
	while(*StrHead == ' ')
	{
		StrHead++;
	}
	if((*StrHead == '\r') || (*StrHead == '\n'))
		DataCount = 0;
	else
	{
		memset(W_DataArry, 0, sizeof(W_DataArry));
		DataCount = 0;
		pData0 = StrHead;
		pData1 = StrHead + 1;
		if((*pData0 == '0') && ((*pData1 == 'X') || (*pData1 == 'x')))
		{
			DataFlag = 1;
			StrHead += 2;
		}
		else
			DataFlag = 0;
		while((*StrHead != '\r') && (*StrHead != '\n'))
		{
			W_Data = 0;
			if(!IsNumeral(*StrHead))
			{
				printf("\r\n4 Command Error.\r\n");
				return;
			}
			while(IsNumeral(*StrHead))
			{
				if(DataFlag == 1)
				{
					W_Data *= 16;
					if((*StrHead) >= 0x30 && (*StrHead) <= 0x39)
						W_Data += (*StrHead) - 0x30;
					else if((*StrHead) >= 0x41 && (*StrHead) <= 0x46)
						W_Data += (*StrHead) - 0x41 + 10;
					else if((*StrHead) >= 0x61 && (*StrHead) <= 0x66)
						W_Data += (*StrHead) - 0x61 + 10;
					else
					{
						printf("\r\n5 Command Error.\r\n");
						return;
					}

				}
				else
				{
					W_Data *= 10;
					if((*StrHead) >= 0x30 && (*StrHead) <= 0x39)
						W_Data += (*StrHead) - 0x30;
					else
					{
						printf("\r\n6 Command Error.\r\n");
						return;
					}
				}
//				B_Data += *StrHead - 0x30;
				StrHead++;
			}
			W_DataArry[DataCount] = W_Data;
			DataCount++;
			if(DataCount >= sizeof(W_DataArry))
			{
				printf("\r\n7 Command Error.\r\n");
				return;
			}
			while(*StrHead == ' ')
				StrHead++;
			pData0 = StrHead;
			pData1 = StrHead + 1;
			if((*pData0 == '0') && ((*pData1 == 'X') || (*pData1 == 'x')))
			{
				DataFlag = 1;
				StrHead += 2;
			}
			else
				DataFlag = 0;
		}
	}
/*
	printf("\r\n%s %s ", DevStr, OperationStr);
	for(i = 0; i < DataCount; i++)
		printf("0x%08X ", W_DataArry[i]);
	printf("\r\nDataCount: %d\r\n", DataCount);
*/
	ExcuteUserCommad(1, DevStr, OperationStr, DataCount, W_DataArry);
}





BYTE ParseCommadByBrillViewType1(BYTE B_Data)
{
	BYTE B_Temp = NULL;
	B_Temp = B_Data;
	if(gB_BrillViewType1CommadIndex > BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH)
	{
		memset(gB_BrillViewType1CommadBuf, 0, sizeof(gB_BrillViewType1CommadBuf));
		gB_BrillViewType1CommadIndex = 0;
		printf("BV TCP Command Length too long!\r\n");
	}
	if(B_Temp == BRILL_VIEW_PACKET_START)
	{
		gB_BrillViewType1PacketStartFlag = 1;
		gB_BrillViewType1PacketRecvFlag = 0;
		return 0;
	}
	if((B_Temp == gB_SerialMachineAddress) && (gB_BrillViewType1PacketStartFlag == 1))
	{
		gB_BrillViewType1PacketRecvFlag = 1;
		gB_BrillViewType1PacketStartFlag = 0;
		memset(gB_BrillViewType1CommadBuf, 0, sizeof(gB_BrillViewType1CommadBuf));
		gB_BrillViewType1CommadIndex = 0;
		gB_BrillViewType1CommadIndex = 0;
		gB_BrillViewType1CommadBuf[gB_BrillViewType1CommadIndex] = BRILL_VIEW_PACKET_START;
		gB_BrillViewType1CommadIndex++;
		gB_BrillViewType1CommadBuf[gB_BrillViewType1CommadIndex] = B_Temp;
		gB_BrillViewType1CommadIndex++;
		return 0;
	}
	else
	{
		gB_BrillViewType1PacketStartFlag = 0;
		if(gB_BrillViewType1PacketRecvFlag == 0)
			gB_BrillViewType1CommadIndex = 0;
	}

	if(gB_BrillViewType1PacketRecvFlag == 1)
	{
		gB_BrillViewType1CommadBuf[gB_BrillViewType1CommadIndex] = B_Temp;
		gB_BrillViewType1CommadIndex++;
		if((gB_BrillViewType1CommadBuf[3] == (gB_BrillViewType1CommadIndex - 4)) && (gB_BrillViewType1CommadBuf[3] >= 1))
		{
			gB_BrillViewType1PacketRecvFlag = 0;
			if(ChangeBrillViewType1CommadToSelfCommad(gB_BrillViewType1CommadBuf) == 0)
				printf("Brill View Type1 Serial Commad Error!\r\n");
			memset(gB_BrillViewType1CommadBuf, 0, sizeof(gB_BrillViewType1CommadBuf));
			gB_BrillViewType1CommadIndex = 0;
			return 1;
		}
	}
	return 0;
}








static BYTE TCP_SendData	(BYTE *Bp_Buf, int W_Len)
{
	return (BYTE) OK;
}






static BYTE ChangeBrillViewType1CommadToSelfCommad(BYTE* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL;
	StrHead = (char*) pCommad;
	if((*StrHead) != BRILL_VIEW_PACKET_START)
		return 0;
	StrHead = (char*) pCommad + 1;
	if(*StrHead != gB_SerialMachineAddress)
		return 0;
	StrHead = (char*) pCommad + 2;
	if(*StrHead == 0x01)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SEL");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x20)
	{
		strcpy(DevStr, "ADV7604_DEV0");
		strcpy(OperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x21)
	{
		strcpy(DevStr, "ADV7604_DEV1");
		strcpy(OperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x22)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SET_OUTPUT_TIMING");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x23)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SET_IP_ADDRESS");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		pData0 = (char*) pCommad + 5;
		W_DataArry[1] = *pData0;
		pData0 = (char*) pCommad + 6;
		W_DataArry[2] = *pData0;
		pData0 = (char*) pCommad + 7;
		W_DataArry[3] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 4, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x24)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SET_TCP_LOCAL_PORT");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		W_DataArry[0] <<= 8;
		pData0 = (char*) pCommad + 5;
		W_DataArry[0] += *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x25)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SET_GATEWAY_IP");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		pData0 = (char*) pCommad + 5;
		W_DataArry[1] = *pData0;
		pData0 = (char*) pCommad + 6;
		W_DataArry[2] = *pData0;
		pData0 = (char*) pCommad + 7;
		W_DataArry[3] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 4, W_DataArry);
		return 1;
	}
	if(*StrHead == 0x26)
	{
		strcpy(DevStr, "SW");
		strcpy(OperationStr, "SET_SERIAL_ADDRESS");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return 1;
	}
	return 0;
}




//======================================
BYTE CheckEquipmentIntegrity(void)
{
	WORD W_Data = 0;
	BYTE B_Result = 0;
	W_Data = InputFPGA_ReadWord(INPUT_FPGA_CHIP_ID) & 0xFF;
	gB_TotalInputPort = gSt_InputBoardADV7604.B_ActiveChip + gSt_InputBoardGV7601.B_ActiveChip + gSt_InputBoardTVP5158.B_ActiveChip;
	gB_TotalOutputPort = gSt_OutputBoarSiI9136.B_ActiveChip;
	switch(W_Data)
	{
		case 'A':
		case 'D':
			if((gB_TotalInputPort == 4) && (gB_TotalOutputPort == 2))
				B_Result = 1;
			gB_TotalInputPort = 4;
			gB_TotalOutputPort = 2;
			break;
		case 'B':
		case 'E':
			if((gB_TotalInputPort == 3) && (gB_TotalOutputPort == 3))
				B_Result = 1;
			gB_TotalInputPort = 3;
			gB_TotalOutputPort = 3;
			break;
		case 'C':
		case 'F':
			if((gB_TotalInputPort == 2) && (gB_TotalOutputPort == 4))
				B_Result = 1;
			gB_TotalInputPort = 2;
			gB_TotalOutputPort = 4;
			break;
		default:
			B_Result = 0;
			break;
	}
	return B_Result;
}




//====================================================================
void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture)
{
	WORD W_Data = 0,W_ModeBaseAdd = 0;
	if(g_DisplayModeIndex > 0)
		W_ModeBaseAdd = (MAX_NVR_PARAM + 1) + ((g_DisplayModeIndex - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
	else
		W_ModeBaseAdd = 0;
	W_Data = W_ModeBaseAdd + NVR_SCALER0_BOARD_INDEX + (B_ScalerIndex * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
	memset(Stp_WindowLayoutAperture, 0, sizeof(XScalerAperture));
	Stp_WindowLayoutAperture->InHoriSize = ParamRead(W_Data + 3);						// < Horizontal size of the input video
	Stp_WindowLayoutAperture->InVertSize = ParamRead(W_Data + 4);						// < Vertical size of the input video
	Stp_WindowLayoutAperture->InFirstPixel = ParamRead(W_Data + 5);						// < The first pixel index in the input video
	Stp_WindowLayoutAperture->InLastPixel = ParamRead(W_Data + 6);						// < The last pixel index in the input video
	Stp_WindowLayoutAperture->InFirstLine = ParamRead(W_Data + 7);						// < The first line index in the input video
	Stp_WindowLayoutAperture->InLastLine = ParamRead(W_Data + 8);						// < The last line index in the input video
	Stp_WindowLayoutAperture->OutHoriSize = ParamRead(W_Data + 9);						// < Horizontal size of the output video
	Stp_WindowLayoutAperture->OutVertSize = ParamRead(W_Data + 10);						// < Vertical size of the output video
}















//================================================================
void LoadUserLastMode(BYTE B_ModeIndex)
{
	WORD W_Data = 0, W_Data2 = 0, W_ModeBaseAdd = 0, W_BaseAdd = 0;
	BYTE i = 0, j = 0;
	ST_SignalWindows 	*Stp_TempSignalWindows 	= NULL;
	ST_SignalSource 	*Stp_TempSignalSource 		= NULL;
	ST_ScalerType 	*Stp_TempScaler 			= NULL;

	if(B_ModeIndex > 0)
		W_ModeBaseAdd = (MAX_NVR_PARAM + 1) + ((B_ModeIndex - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
	else
		W_ModeBaseAdd = 0;
//	printf("LoadUserLastMode Start: W_ModeBaseAdd = 0x%04X\r\n", W_ModeBaseAdd);

	W_Data = ParamRead(NVR_DISPLAY_MODE_INDEX);
	if(W_Data != B_ModeIndex)
		ParamWrite (NVR_DISPLAY_MODE_INDEX, B_ModeIndex);
//	InputFPGA_WriteWord(INPUT_FPGA_SCALER_MASK_ENABLE, 0x0F);

       InputFPGAClearBackgroundWindow(0x0F);
	gStp_ProcessorScaler->B_ActiveScalerNum = MAX_SCALER_NUM;
	
	for(j = 0; j < gSt_OutputBoarSiI9136.B_ActiveChip; j++)
	{
		W_BaseAdd = W_ModeBaseAdd + (j * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
		i = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_BACKGROUND_INDEX);
		if(i >= OUTPUT_BOARD_MAX)
			continue;
		
		ScreenWindowLayoutInit(i);
		W_Data = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_TOTAL_NUM);

		if(W_Data > 8)
			continue;

		if(W_Data > 0)
		{
			gStp_ScreenRectInput->ActiveRectWindowsNum = W_Data;
			if(W_Data >= MaxDiaplayWallWindowsNum)
			{
				gStp_ScreenRectInput->ActiveRectWindowsNum = 0;
				return;
			}
			gStp_ScreenRectInput->B_BoardSlotIndex = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_BOARD_INDEX);
			gStp_ScreenRectInput->B_OutputScreenIndex = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_BACKGROUND_INDEX);
			gStp_ScreenRectInput->DisplayScreenWidth = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_SCREEN_WIDTH);
			gStp_ScreenRectInput->DisplayScreenHeight = ParamRead(W_BaseAdd + NVR_Display0_WINDOWS_SCREEN_HEIGHT);
			if((gStp_ScreenRectInput->DisplayScreenWidth == 0) || (gStp_ScreenRectInput->DisplayScreenHeight == 0))
				continue;
			gW_DisplayScreenWindowsNum = W_Data;
			
			#if(DEBUG_LOAD_USER_LAST_MODE == 1)
			printf("INPUT_FPGA SET_WINDOWS_LAYOUT %d %d %d %d\r\n",
																		gStp_ScreenRectInput->B_BoardSlotIndex,
																		gStp_ScreenRectInput->B_OutputScreenIndex,
																		gStp_ScreenRectInput->DisplayScreenWidth,
																		gStp_ScreenRectInput->DisplayScreenHeight);
			#endif


			
			for(i = 0; i < gW_DisplayScreenWindowsNum; i++)
			{
				Stp_TempSignalWindows = gStp_ScreenRectInput->Stp_RectWindows[i];
				Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
				if((Stp_TempSignalWindows == NULL) || (gW_DisplayScreenWindowsNum >= MaxDiaplayWallWindowsNum))
					return;
				W_Data = W_BaseAdd + NVR_Display0_WINDOW0_SIGNAL_SOURCE + (i * 10);
				W_DataArry[0] = ParamRead(W_Data);
				W_DataArry[1] = ParamRead(W_Data + 1);
				W_DataArry[2] = ParamRead(W_Data + 2);
				W_DataArry[3] = ParamRead(W_Data + 3);
				W_DataArry[4] = ParamRead(W_Data + 4);
				W_DataArry[5] = ParamRead(W_Data + 5);
				W_DataArry[6] = ParamRead(W_Data + 6);
				W_DataArry[7] = ParamRead(W_Data + 7);
				W_DataArry[8] = ParamRead(W_Data + 8);
				W_DataArry[9] = ParamRead(W_Data + 9);
				W_DataArry[10]= ParamRead(W_Data + 10);
				/*
				GetInputBoardSignalResolution(W_DataArry[0], 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
				if((W_DataArry[0] < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				{
					Stp_TempSignalSource->SignalSource = 8;
				}
				else
					Stp_TempSignalSource->SignalSource = W_DataArry[0];
				*/
				Stp_TempSignalSource->SignalSource = W_DataArry[0];
				Stp_TempSignalSource->SignalSourceSlot = W_DataArry[0];
				Stp_TempSignalSource->OriginalSignalSource = W_DataArry[0];
				Stp_TempSignalSource->Windows_Layer = W_DataArry[1];
				Stp_TempSignalWindows->SignalInputWindow.left = W_DataArry[2];
				Stp_TempSignalWindows->SignalInputWindow.top = W_DataArry[3];
				Stp_TempSignalWindows->SignalInputWindow.right = W_DataArry[4];
				Stp_TempSignalWindows->SignalInputWindow.bottom = W_DataArry[5];
				Stp_TempSignalWindows->SignalDisplayWindow.left = W_DataArry[6];
				Stp_TempSignalWindows->SignalDisplayWindow.top = W_DataArry[7];
				Stp_TempSignalWindows->SignalDisplayWindow.right = W_DataArry[8];
				Stp_TempSignalWindows->SignalDisplayWindow.bottom = W_DataArry[9];

				
				#if( DEBUG_LOAD_USER_LAST_MODE == 1)
				printf("INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d\r\n",
																		Stp_TempSignalSource->SignalSource,
																		Stp_TempSignalSource->Windows_Layer,
																		Stp_TempSignalWindows->SignalInputWindow.left,
																		Stp_TempSignalWindows->SignalInputWindow.top,
																		Stp_TempSignalWindows->SignalInputWindow.right,
																		Stp_TempSignalWindows->SignalInputWindow.bottom,
																		Stp_TempSignalWindows->SignalDisplayWindow.left,
																		Stp_TempSignalWindows->SignalDisplayWindow.top,
																		Stp_TempSignalWindows->SignalDisplayWindow.right,
																		Stp_TempSignalWindows->SignalDisplayWindow.bottom);
			#endif
			}
			ExecuteScreenWindowLayout();
		}
//		else
//			InputFPGAClearBackgroundWindow(j);
	}
//	InputFPGA_WriteWord(INPUT_FPGA_OUTPUT_CFG_SWITCH, 0x10F);
	for(i = 0; i < 4; i++)
	{
		W_Data = W_ModeBaseAdd + NVR_SCALER0_BOARD_INDEX + (i * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
		W_DataArry[0] = ParamRead(W_Data);
		W_DataArry[1] = ParamRead(W_Data + 1);
		W_DataArry[2] = ParamRead(W_Data + 2);
		W_DataArry[3] = ParamRead(W_Data + 3);
		W_DataArry[4] = ParamRead(W_Data + 4);
		W_DataArry[5] = ParamRead(W_Data + 5);
		W_DataArry[6] = ParamRead(W_Data + 6);
		W_DataArry[7] = ParamRead(W_Data + 7);
		W_DataArry[8] = ParamRead(W_Data + 8);
		W_DataArry[9] = ParamRead(W_Data + 9);
		W_DataArry[10] = ParamRead(W_Data + 10);
		memset(&St_UserCommadAperture, 0, sizeof(St_UserCommadAperture));
		St_UserCommadAperture.InHoriSize = W_DataArry[3];						// < Horizontal size of the input video
		St_UserCommadAperture.InVertSize = W_DataArry[4];						// < Vertical size of the input video
		St_UserCommadAperture.InFirstPixel = W_DataArry[5];							// < The first pixel index in the input video
		St_UserCommadAperture.InLastPixel = W_DataArry[6];						// < The last pixel index in the input video
		St_UserCommadAperture.InFirstLine = W_DataArry[7];							// < The first line index in the input video
		St_UserCommadAperture.InLastLine = W_DataArry[8];						// < The last line index in the input video
		St_UserCommadAperture.OutHoriSize = W_DataArry[9];						// < Horizontal size of the output video
		St_UserCommadAperture.OutVertSize = W_DataArry[10];						// < Vertical size of the output video
		Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
		Stp_TempScaler->B_SignalSourceSlot = W_DataArry[2];
		
		Stp_TempScaler->InHoriSize = W_DataArry[3];
		Stp_TempScaler->InVertSize = W_DataArry[4];
		Stp_TempScaler->InFirstPixel = W_DataArry[5];
		Stp_TempScaler->InLastPixel = W_DataArry[6];
		Stp_TempScaler->InFirstLine = W_DataArry[7];
		Stp_TempScaler->InLastLine = W_DataArry[8];
		Stp_TempScaler->OutHoriSize = W_DataArry[9];
		Stp_TempScaler->OutVertSize = W_DataArry[10];

		
		#if( DEBUG_LOAD_USER_LAST_MODE == 1)
		printf("INPUT_FPGA SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",
																W_DataArry[0], W_DataArry[1], W_DataArry[2],
																W_DataArry[3], W_DataArry[4], W_DataArry[5],
																W_DataArry[6], W_DataArry[7], W_DataArry[8],
																W_DataArry[9], W_DataArry[10]);
		#endif


		
		if((W_DataArry[0] < 16) && (W_DataArry[1] < 32))
		{
			if((W_DataArry[2] == 0) && (gB_CVBSSwitchEnable == 1))
				GetInputBoardSignalResolution(W_DataArry[2], gB_CVBSSwitchIndex, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
			else
				GetInputBoardSignalResolution(W_DataArry[2], 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
			W_Data = Stp_TempScaler->InLastPixel - Stp_TempScaler->InFirstPixel + 1;
			W_Data2 = Stp_TempScaler->InLastLine - Stp_TempScaler->InFirstLine + 1;

			if((W_DataArry[1] == 0) && (W_Data > gW_ResolutionWidth) && (W_Data2 > gW_ResolutionHeight) && (gW_ResolutionWidth > 0) && (gW_ResolutionHeight > 0))
			{
				gW_ResolutionWidth <<= 1;
				gW_ResolutionHeight <<= 1;
				Stp_TempScaler->B_SignalSource = 1;
			}
			else
				Stp_TempScaler->B_SignalSource = 0;
			
			if((W_DataArry[2] < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				InputFPGA_ScalerSetNoSignalOutput(W_DataArry[1], 1);
			else
				InputFPGA_ScalerSetNoSignalOutput(W_DataArry[1], 0);

			InputFPGA_ScalerSetup(W_DataArry[1], W_DataArry[2] << 1, &St_UserCommadAperture);
		}
	}
	InputFPGASetUpdata();
	
	#if( DEBUG_LOAD_USER_LAST_MODE == 1)
		printf("BOARD LOAD_USER_LAST_MODE\r\n");
	#endif
}






//======================================================================================

WORD W_DataArry[USER_COMMAD_BUF_LENGTH];
XScalerAperture St_UserCommadAperture;
ST_SignalSource *Stp_TempSignalSource;//Stp_TempSignalWindows
ST_SignalWindows *Stp_TempSignalWindows;


void KeyPad_SetupWizard_to_FPGA(char iMode)
{
	char i;
	char NumberOfOutputCard=0;
	char Slot_ID = 0;
	char scalerNo=0;
//	const char scalerOutNoAdd = 4;
	char outPort=0;

	
	U32 Scaler0_InHLast=0,	Scaler0_InVLast=0;
	U32 Scaler1_InHLast=0,	Scaler1_InVLast=0;
	U32 Scaler2_InHLast=0,	Scaler2_InVLast=0;
	U32 Scaler3_InHLast=0,	Scaler3_InVLast=0;

	U32 Scaler0_OutHSize=0,	Scaler0_OutVSize=0;
	U32 Scaler1_OutHSize=0,	Scaler1_OutVSize=0;
	U32 Scaler2_OutHSize=0,	Scaler2_OutVSize=0;
	U32 Scaler3_OutHSize=0,	Scaler3_OutVSize=0;

	U32 Scaler0_InHPos=0,	Scaler0_InVPos=0;
	U32 Scaler1_InHPos=0,	Scaler1_InVPos=0;
	U32 Scaler2_InHPos=0,	Scaler2_InVPos=0;
	U32 Scaler3_InHPos=0,	Scaler3_InVPos=0;
	
	
	WORD W_WizardDataArry[12] = {0};
	WORD W_WizardDataArry_T[12] = {0};
	if((iMode<=0)||(iMode>DISPLAY_MODE_INDEX_MAX))
		return;

	//INPUT_FPGA SET_KEY_MODE 1
	CMD_INPUTFPGA_SET_KEY_MODE(iMode);
	
	CMD_INPUTFPGA_CLEAR_SCALER_USE();
	CMD_BOARD_RMG();
	NumberOfOutputCard=gSt_OutputBoarSiI9136.B_ActiveChip;	
	switch(g_CardNumber)
	{
		case 1:
			if(g_ImageNumber==1)//1x1
			{
						
					
					W_WizardDataArry[0]=0;	//Slot 0
					W_WizardDataArry[1]=0;	//scaler 0
					switch(g_Image1Scr)
						{
						case INPUTSOURCE_1DVI:
						case INPUTSOURCE_1VGA:								
								Slot_ID=0;
								break;
						case INPUTSOURCE_2DVI:
						case INPUTSOURCE_2VGA:
								Slot_ID=1;
								break;	
						case INPUTSOURCE_3DVI:
						case INPUTSOURCE_3VGA:
								Slot_ID=2;
								break;
						case INPUTSOURCE_4DVI:
						case INPUTSOURCE_4VGA:
								Slot_ID=3;
								break;		
						}
					CMD_BOARD_RISR(Slot_ID);	
					W_WizardDataArry[2]=Slot_ID;
					W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;
					W_WizardDataArry[5]=0;
					W_WizardDataArry[6]=gW_ResolutionWidth-1;
					W_WizardDataArry[7]=0;
					W_WizardDataArry[8]=gW_ResolutionHeight-1;
					
					W_WizardDataArry[9]=g_ScreenTotalHSize;
					W_WizardDataArry[10]=g_ScreenTotalVSize;
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					
					
					
					#ifdef DEBUG_STATE_MSG
					printf("g_Image1Scr: %d,     gW_ResolutionWidth: %d,     gW_ResolutionHeight: %d,	     g_ScreenTotalHSize: %d,      g_ScreenTotalVSize: %d,      g_Card1HSize: %d,        g_Card1VSize: %d;\r\n"
									,g_Image1Scr,	gW_ResolutionWidth,		gW_ResolutionHeight,
									g_ScreenTotalHSize,	g_ScreenTotalVSize,	g_Card1HSize,g_Card1VSize
									);
					#endif
					
					for(i=0;i<NumberOfOutputCard;i++)
					{
						//CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(i);	//output port					
					
						//视频开窗
						W_WizardDataArry_T[0]=Slot_ID;	//Slot 0
						W_WizardDataArry_T[1]=i;	//outport no. 0
						W_WizardDataArry_T[2]=g_ScreenTotalHSize;
						W_WizardDataArry_T[3]=g_ScreenTotalVSize;
												
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("SET WINDOWS, BOARD=:%d,   OUTPORT=:%d,     WIDTH=:%d,     HEIGHT=:%d\r\n",	
									W_WizardDataArry_T[0],W_WizardDataArry_T[1],
									W_WizardDataArry_T[2],W_WizardDataArry_T[3]);
						#endif
						W_WizardDataArry_T[0]=4;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=gW_ResolutionWidth;	//InRight
						W_WizardDataArry_T[5]=gW_ResolutionHeight;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card1HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card1VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);	
						CMD_INPUTFPGA_ADD_WINDOW_END();	

						//还要细究，为什么只有1号口能输出数据???   
						INPUT_FPGA_SET_UPDATA_SYSTEM_DISPLAY();
					}
				}	
				#ifdef DEBUG_STATE_MSG
				printf("1x1,Card Number: %d, Image Number: %d\r\n",g_CardNumber,g_ImageNumber);
				#endif
				//else if(g_ImageNumber==2)//2x1
				//{

				//}
			break;	
		

				
		
		case 2://card number
			if(g_ImageNumber==1)//1x2
				{	

					switch(g_Image1Scr)
						{
						case INPUTSOURCE_1DVI:
						case INPUTSOURCE_1VGA:								
								Slot_ID=0;
								break;
						case INPUTSOURCE_2DVI:
						case INPUTSOURCE_2VGA:
								Slot_ID=1;
								break;	
						case INPUTSOURCE_3DVI:
						case INPUTSOURCE_3VGA:
								Slot_ID=2;
								break;
						case INPUTSOURCE_4DVI:
						case INPUTSOURCE_4VGA:
								Slot_ID=3;
								break;		
						}
					CMD_BOARD_RISR(Slot_ID);
					switch(g_CardLayout)
						{
						case 0: //左右拼接							
							Scaler0_InHLast =(gW_ResolutionWidth*g_Card1HSize)/g_ScreenTotalHSize;	
							Scaler0_InVLast = gW_ResolutionHeight;
							Scaler1_InHLast = gW_ResolutionWidth-Scaler0_InHLast;
							Scaler1_InVLast = gW_ResolutionHeight;
							
							Scaler0_OutVSize=g_Card1VSize;
							#ifdef DEBUG_STATE_MSG
							printf("cardLayout 0leftright, g_Card1HSize=:%d,    g_ScreenTotalHSize=:%d,    gW_ResolutionWidth=:%d,    Scaler0_InHLast=:%d \r\n",
									g_Card1HSize,g_ScreenTotalHSize,	gW_ResolutionWidth,Scaler0_InHLast
									);				
							#endif
							break;
						case 1://上下拼接
							Scaler0_InHLast = gW_ResolutionWidth;
							Scaler0_InVLast = gW_ResolutionHeight;
							Scaler0_OutVSize=g_Card1VSize+g_Card2VSize;
							break;
						}
					#ifdef DEBUG_STATE_MSG
					printf("gW_ResolutionWidth=:%d,    gW_ResolutionHeight=:%d,    g_Card1HSize=:%d,    g_Card1VSize=:%d,    g_ScreenTotalHSize=:%d,    g_ScreenTotalVSize=:%d \r\n",
							gW_ResolutionWidth,gW_ResolutionHeight,g_Card1HSize,g_Card1VSize,
							g_ScreenTotalHSize,g_ScreenTotalVSize
							);

					printf("Scaler0_InHLast=:%d,    Scaler0_InVLast=:%d,    Scaler1_InHLast=:%d,    Scaler1_InVLast=:%d \r\n",
							Scaler0_InHLast,Scaler0_InVLast,Scaler1_InHLast,Scaler1_InVLast
							);
					#endif

				//设置缩放器-0
					W_WizardDataArry[0]=BOARD_ID;	//Slot 0
					scalerNo = 0;
					W_WizardDataArry[1]=scalerNo;	//scaler 0					
					W_WizardDataArry[2]=Slot_ID;
					W_WizardDataArry[3]=gW_ResolutionWidth;//输入信号的长read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;//输入信号的高
					W_WizardDataArry[5]=0;
					W_WizardDataArry[6]=Scaler0_InHLast-1;
					W_WizardDataArry[7]=0;
					W_WizardDataArry[8]=Scaler0_InVLast-1;					
					W_WizardDataArry[9]=g_Card1HSize;
					//if(g_CardLayout==1)//上下拼接
					W_WizardDataArry[10]=Scaler0_OutVSize;//g_Card1VSize+g_Card2VSize;
					//else
					//	W_WizardDataArry[10]=g_Card1VSize;
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					#ifdef DEBUG_STATE_MSG
					printf("0: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
							);
					#endif
					
					//设置缩放器-1  上下拼接的时候只需要一个缩放器
					if(g_CardLayout==0)
						{
							W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
							scalerNo = 1;
							W_WizardDataArry[1]=scalerNo;	//scaler 1
							W_WizardDataArry[2]=Slot_ID;//SLOT ID
							W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
							W_WizardDataArry[4]=gW_ResolutionHeight;
							W_WizardDataArry[5]=Scaler0_InHLast;
							W_WizardDataArry[6]=gW_ResolutionWidth-1;
							W_WizardDataArry[7]=0;
							W_WizardDataArry[8]=Scaler1_InVLast-1;					
							W_WizardDataArry[9]=g_Card2HSize;
							W_WizardDataArry[10]=g_Card2VSize;
							
							CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
							#ifdef DEBUG_STATE_MSG
							printf("1: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
									W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
									W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
									W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
									);
							#endif
						}

					//视频开窗-0
						//CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(0);	//output port					
						
						W_WizardDataArry_T[0]=BOARD_ID;	//Slot 0
						outPort=0;	
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card1HSize;
						W_WizardDataArry_T[3]=g_Card1VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);						
						printf("0: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]
							);
						
						scalerNo = 4;
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card1HSize;	//InRight
						W_WizardDataArry_T[5]=g_Card1VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card1HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card1VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("0: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d  \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	
						
						
					//视频开窗-1
						//CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(1);	//output port					
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=1;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card2HSize;
						W_WizardDataArry_T[3]=g_Card2VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]
							);
						#endif
						if(g_CardLayout==1)
							scalerNo=4;
						else
							scalerNo=5;
						
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==1)
								W_WizardDataArry_T[3]=g_Card1VSize;
							else
								W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card2HSize;	//InRight
						if(g_CardLayout==1)
							W_WizardDataArry_T[5]=W_WizardDataArry_T[3]+g_Card2VSize;
							else
								W_WizardDataArry_T[5]=g_Card2VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card2HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card2VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();					
					}
				#ifdef DEBUG_STATE_MSG
				printf("1x2,Card Number: %d, Image Number: %d\r\n",g_CardNumber,g_ImageNumber);
				#endif
				break;
		case 3:	
				if(g_ImageNumber==1)//1x3
				{	
					CMD_BOARD_RISR(Slot_ID);
					switch(g_CardLayout)
						{
						case 0: //水平拼接	
							//B4
							Scaler0_InVLast = gW_ResolutionHeight;
							Scaler1_InVLast = gW_ResolutionHeight;
							Scaler2_InVLast = gW_ResolutionHeight;
						
							//B5 //B6
							Scaler0_InHPos=0;	
							Scaler0_InHLast =(gW_ResolutionWidth*g_Card1HSize)/g_ScreenTotalHSize;	

							Scaler1_InHPos=Scaler0_InHLast;
							Scaler1_InHLast =(gW_ResolutionWidth*g_Card2HSize)/g_ScreenTotalHSize+Scaler0_InHLast;

							Scaler2_InHPos=Scaler1_InHLast;
							Scaler2_InHLast =gW_ResolutionWidth;	
							
							#ifdef DEBUG_STATE_MSG		
						printf("1x3 h split:  SCALER_PARAME:\r\n Scaler0_InHLast=%d,Scaler1_InHLast=%d,Scaler2_InHLast=%d,Scaler3_InHLast=%d,Scaler0_InHPos=%d,Scaler1_InHPos=%d,Scaler2_InHPos=%d,Scaler3_InHPos=%d\r\n",
							Scaler0_InHLast,Scaler1_InHLast,Scaler2_InHLast,Scaler3_InHLast,
							Scaler0_InHPos,Scaler1_InHPos, Scaler2_InHPos,Scaler3_InHPos	
							);
							#endif
						
							//B7
							Scaler0_InVPos=0;
							Scaler1_InVPos=0;
							Scaler2_InVPos=0;
							Scaler3_InVPos=0;
							//B9
							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;
							Scaler2_OutHSize=g_Card3HSize;
							Scaler3_OutHSize=g_Card4HSize;
							//B10
							Scaler0_OutVSize=g_Card1VSize;
							Scaler1_OutVSize=g_Card2VSize;
							Scaler2_OutVSize=g_Card3VSize;
							Scaler3_OutVSize=g_Card4VSize;
							
							break;
						case 1://上下三拼接;只是需要2个缩放器						
							//B5
							Scaler0_InHPos=0;
							Scaler1_InHPos=0;
							//B6
							Scaler0_InHLast = gW_ResolutionWidth;														
							Scaler1_InHLast = gW_ResolutionWidth;	
							
							//B7//B8
							Scaler0_InVPos=0;							
							Scaler0_InVLast =(gW_ResolutionHeight*(g_Card1VSize+g_Card2VSize))/g_ScreenTotalVSize;
							Scaler1_InVPos=Scaler0_InVLast;
							Scaler1_InVLast = gW_ResolutionHeight;

							//B9
							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;
							//B10
							Scaler0_OutVSize=g_Card1VSize+g_Card2VSize;
							Scaler1_OutVSize=g_Card3VSize;
							break;
							
						case 2://田字拼接;只是需要2个缩放器
							//B5//B6
							Scaler0_InHPos=0;													
							Scaler0_InHLast =(gW_ResolutionWidth*g_Card1HSize)/g_ScreenTotalHSize;
							Scaler1_InHPos=Scaler0_InHLast;	
							Scaler1_InHLast = gW_ResolutionWidth;		
							//B7//B8
							Scaler0_InVPos=0;							
							Scaler0_InVLast =gW_ResolutionHeight;//*g_Card1VSize)/g_ScreenTotalVSize;
							Scaler1_InVPos=0;//Scaler0_InVLast;
							Scaler1_InVLast = gW_ResolutionHeight;	
							

							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;

							Scaler0_OutVSize=g_Card1VSize+g_Card3VSize;
							Scaler1_OutVSize=g_Card2VSize+g_Card4VSize;
							break;	
							
						}
					
				switch(g_Image1Scr)
						{
						case INPUTSOURCE_1DVI:
						case INPUTSOURCE_1VGA:								
								Slot_ID=0;
								break;
						case INPUTSOURCE_2DVI:
						case INPUTSOURCE_2VGA:
								Slot_ID=1;
								break;	
						case INPUTSOURCE_3DVI:
						case INPUTSOURCE_3VGA:
								Slot_ID=2;
								break;
						case INPUTSOURCE_4DVI:
						case INPUTSOURCE_4VGA:
								Slot_ID=3;
								break;		
						}


				

				//设置缩放器-0
					W_WizardDataArry[0]=BOARD_ID;	
					scalerNo = 0;
					W_WizardDataArry[1]=scalerNo;	//scaler 0												
					W_WizardDataArry[2]=Slot_ID;
					W_WizardDataArry[3]=gW_ResolutionWidth;//输入信号的长read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;//输入信号的高
					W_WizardDataArry[5]=Scaler0_InHPos;//0;
					W_WizardDataArry[6]=Scaler0_InHLast-1;						
					W_WizardDataArry[7]=Scaler0_InVPos;
					W_WizardDataArry[8]=Scaler0_InVLast-1;					
					W_WizardDataArry[9]=Scaler0_OutHSize;
					W_WizardDataArry[10]=Scaler0_OutVSize;
						
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					#ifdef DEBUG_STATE_MSG
					printf("0: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
							);
					#endif
					
					//设置缩放器-1
					W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
					scalerNo = 1;
					W_WizardDataArry[1]=scalerNo;	//scaler 1
					W_WizardDataArry[2]=Slot_ID;//SLOT ID
					W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;
					W_WizardDataArry[5]=Scaler1_InHPos;//Scaler0_InHLast;				
					W_WizardDataArry[6]=Scaler1_InHLast-1;						
					W_WizardDataArry[7]=Scaler1_InVPos;
					W_WizardDataArry[8]=Scaler1_InVLast-1;					
					W_WizardDataArry[9]=Scaler1_OutHSize;
					W_WizardDataArry[10]=Scaler1_OutVSize;
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					#ifdef DEBUG_STATE_MSG
					printf("1: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
							);
					#endif

					if(g_CardLayout==0)//只有左右拼接的时候才用第三四缩放器
						{
							//设置缩放器-2
							W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
							scalerNo = 2;
							W_WizardDataArry[1]=scalerNo;	//scaler 1
							W_WizardDataArry[2]=Slot_ID;//SLOT ID
							W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
							W_WizardDataArry[4]=gW_ResolutionHeight;
							W_WizardDataArry[5]=Scaler2_InHPos;//Scaler0_InHLast+Scaler1_InHLast;
							W_WizardDataArry[6]=Scaler2_InHLast-1;
							W_WizardDataArry[7]=Scaler2_InVPos;
							W_WizardDataArry[8]=Scaler2_InVLast-1;					
							W_WizardDataArry[9]=Scaler2_OutHSize;
							W_WizardDataArry[10]=Scaler2_OutVSize;//g_Card3VSize;
							
							CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
							#ifdef DEBUG_STATE_MSG
							printf("2: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
									W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
									W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
									W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
									);
							#endif
						}
										

					//视频开窗-0						
						W_WizardDataArry_T[0]=BOARD_ID;	//Slot 0
						outPort=0;	
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card1HSize;
						W_WizardDataArry_T[3]=g_Card1VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);						
						printf("0: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]	);
						
						scalerNo = 4;
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card1HSize;	//InRight
						W_WizardDataArry_T[5]=g_Card1VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card1HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card1VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("0: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d  \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	
						
						
					//视频开窗-1										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=1;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card2HSize;
						W_WizardDataArry_T[3]=g_Card2VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]	);
						#endif
						if(g_CardLayout==1)
							scalerNo=4;
						else 
							scalerNo=5;						
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==1)
								W_WizardDataArry_T[3]=g_Card1VSize;
							else
								W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card2HSize;	//InRight
						if(g_CardLayout==1)
								W_WizardDataArry_T[5]=g_Card1VSize+g_Card2VSize;
							else
								W_WizardDataArry_T[5]=g_Card2VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card2HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card2VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();		


						//视频开窗-2										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=2;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card3HSize;
						W_WizardDataArry_T[3]=g_Card3VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("2: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]);
						#endif
						if(g_CardLayout==0)
							scalerNo=6;
						else if(g_CardLayout==1)
							scalerNo=5;
						else
							scalerNo=4;
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==2)
								W_WizardDataArry_T[3]=g_Card1VSize;
							else
								W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card3HSize;	//InRight
						if(g_CardLayout==2)
								W_WizardDataArry_T[5]=g_Card1VSize+g_Card3VSize;
							else
								W_WizardDataArry_T[5]=g_Card3VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card3HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card3VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("2: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	


						//视频开窗-3										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=3;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card4HSize;
						W_WizardDataArry_T[3]=g_Card4VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("3: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]);
						#endif
						if(g_CardLayout==0)
							scalerNo=7;
						else 
							scalerNo=5;
						
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==0)
								W_WizardDataArry_T[3]=ZERO;	//InTop
							else
								W_WizardDataArry_T[3]=g_Card2VSize;	
							
						W_WizardDataArry_T[4]=g_Card4HSize;	//InRight
						if(g_CardLayout==0)
								W_WizardDataArry_T[5]=g_Card4VSize;	//Inbottom
							else
								W_WizardDataArry_T[5]=g_Card2VSize+g_Card4VSize;	
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card4HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card4VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("3: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	
					}
				printf("1x4,Card Number: %d, Image Number: %d\r\n",g_CardNumber,g_ImageNumber);	

			
			break;
			
		case 4:
			if(g_ImageNumber==1)//1x4
				{	
					CMD_BOARD_RISR(Slot_ID);
					switch(g_CardLayout)
						{
						case 0: //水平拼接	
							//B4
							Scaler0_InVLast = gW_ResolutionHeight;
							Scaler1_InVLast = gW_ResolutionHeight;
							Scaler2_InVLast = gW_ResolutionHeight;
							Scaler3_InVLast = gW_ResolutionHeight;
							//B5 //B6
							Scaler0_InHPos=0;	
							Scaler0_InHLast =(gW_ResolutionWidth*g_Card1HSize)/g_ScreenTotalHSize;	

							Scaler1_InHPos=Scaler0_InHLast;
							Scaler1_InHLast =(gW_ResolutionWidth*g_Card2HSize)/g_ScreenTotalHSize+Scaler0_InHLast;

							Scaler2_InHPos=Scaler1_InHLast;
							Scaler2_InHLast =(gW_ResolutionWidth*g_Card3HSize)/g_ScreenTotalHSize+Scaler1_InHLast;	
							
							Scaler3_InHPos=Scaler2_InHLast;												
							Scaler3_InHLast = gW_ResolutionWidth;
						#ifdef DEBUG_STATE_MSG
						printf("1x4 h split:  SCALER_PARAME:\r\n Scaler0_InHLast=%d,Scaler1_InHLast=%d,Scaler2_InHLast=%d,Scaler3_InHLast=%d,Scaler0_InHPos=%d,Scaler1_InHPos=%d,Scaler2_InHPos=%d,Scaler3_InHPos=%d\r\n",
							Scaler0_InHLast,Scaler1_InHLast,Scaler2_InHLast,Scaler3_InHLast,
							Scaler0_InHPos,Scaler1_InHPos, Scaler2_InHPos,Scaler3_InHPos	
							);
						#endif
							//B7
							Scaler0_InVPos=0;
							Scaler1_InVPos=0;
							Scaler2_InVPos=0;
							Scaler3_InVPos=0;
							//B9
							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;
							Scaler2_OutHSize=g_Card3HSize;
							Scaler3_OutHSize=g_Card4HSize;
							//B10
							Scaler0_OutVSize=g_Card1VSize;
							Scaler1_OutVSize=g_Card2VSize;
							Scaler2_OutVSize=g_Card3VSize;
							Scaler3_OutVSize=g_Card4VSize;
							
							break;
						case 1://上下四拼接;只是需要2个缩放器						
							//B5
							Scaler0_InHPos=0;
							Scaler1_InHPos=0;
							//B6
							Scaler0_InHLast = gW_ResolutionWidth;														
							Scaler1_InHLast = gW_ResolutionWidth;	
							
							//B7//B8
							Scaler0_InVPos=0;							
							Scaler0_InVLast =(gW_ResolutionHeight*(g_Card1VSize+g_Card3VSize))/g_ScreenTotalVSize;
							Scaler1_InVPos=Scaler0_InVLast;
							Scaler1_InVLast = gW_ResolutionHeight;

							//B9
							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;
							//B10
							Scaler0_OutVSize=g_Card1VSize+g_Card3VSize;
							Scaler1_OutVSize=g_Card2VSize+g_Card4VSize;
							break;
							
						case 2://田字拼接;只是需要2个缩放器
							//B5//B6
							Scaler0_InHPos=0;													
							Scaler0_InHLast =(gW_ResolutionWidth*g_Card1HSize)/g_ScreenTotalHSize;
							Scaler1_InHPos=Scaler0_InHLast;	
							Scaler1_InHLast = gW_ResolutionWidth;		
							//B7//B8
							Scaler0_InVPos=0;							
							Scaler0_InVLast =gW_ResolutionHeight;//*g_Card1VSize)/g_ScreenTotalVSize;
							Scaler1_InVPos=0;//Scaler0_InVLast;
							Scaler1_InVLast = gW_ResolutionHeight;	
							

							Scaler0_OutHSize=g_Card1HSize;
							Scaler1_OutHSize=g_Card2HSize;

							Scaler0_OutVSize=g_Card1VSize+g_Card3VSize;
							Scaler1_OutVSize=g_Card2VSize+g_Card4VSize;
							break;	
							
						}
					
				switch(g_Image1Scr)
						{
						case INPUTSOURCE_1DVI:
						case INPUTSOURCE_1VGA:								
								Slot_ID=0;
								break;
						case INPUTSOURCE_2DVI:
						case INPUTSOURCE_2VGA:
								Slot_ID=1;
								break;	
						case INPUTSOURCE_3DVI:
						case INPUTSOURCE_3VGA:
								Slot_ID=2;
								break;
						case INPUTSOURCE_4DVI:
						case INPUTSOURCE_4VGA:
								Slot_ID=3;
								break;		
						}


				

				//设置缩放器-0
					W_WizardDataArry[0]=BOARD_ID;	
					scalerNo = 0;
					W_WizardDataArry[1]=scalerNo;	//scaler 0												
					W_WizardDataArry[2]=Slot_ID;
					W_WizardDataArry[3]=gW_ResolutionWidth;//输入信号的长read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;//输入信号的高
					W_WizardDataArry[5]=Scaler0_InHPos;//0;
					W_WizardDataArry[6]=Scaler0_InHLast-1;						
					W_WizardDataArry[7]=Scaler0_InVPos;
					W_WizardDataArry[8]=Scaler0_InVLast-1;					
					W_WizardDataArry[9]=Scaler0_OutHSize;
					W_WizardDataArry[10]=Scaler0_OutVSize;
						
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					#ifdef DEBUG_STATE_MSG
					printf("0: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
							);
					#endif
					
					//设置缩放器-1
					W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
					scalerNo = 1;
					W_WizardDataArry[1]=scalerNo;	//scaler 1
					W_WizardDataArry[2]=Slot_ID;//SLOT ID
					W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
					W_WizardDataArry[4]=gW_ResolutionHeight;
					W_WizardDataArry[5]=Scaler1_InHPos;//Scaler0_InHLast;				
					W_WizardDataArry[6]=Scaler1_InHLast-1;						
					W_WizardDataArry[7]=Scaler1_InVPos;
					W_WizardDataArry[8]=Scaler1_InVLast-1;					
					W_WizardDataArry[9]=Scaler1_OutHSize;
					W_WizardDataArry[10]=Scaler1_OutVSize;
					
					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
					#ifdef DEBUG_STATE_MSG
					printf("1: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
							);
					#endif

					if(g_CardLayout==0)//只有左右拼接的时候才用第三四缩放器
						{
							//设置缩放器-2
							W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
							scalerNo = 2;
							W_WizardDataArry[1]=scalerNo;	//scaler 1
							W_WizardDataArry[2]=Slot_ID;//SLOT ID
							W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
							W_WizardDataArry[4]=gW_ResolutionHeight;
							W_WizardDataArry[5]=Scaler2_InHPos;//Scaler0_InHLast+Scaler1_InHLast;
							W_WizardDataArry[6]=Scaler2_InHLast-1;
							W_WizardDataArry[7]=Scaler2_InVPos;
							W_WizardDataArry[8]=Scaler2_InVLast-1;					
							W_WizardDataArry[9]=Scaler2_OutHSize;
							W_WizardDataArry[10]=Scaler2_OutVSize;//g_Card3VSize;
							
							CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
							#ifdef DEBUG_STATE_MSG
							printf("2: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
									W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
									W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
									W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
									);
							#endif

							//设置缩放器-4
							W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
							scalerNo = 3;
							W_WizardDataArry[1]=scalerNo;	//scaler 1
							W_WizardDataArry[2]=Slot_ID;//SLOT ID
							W_WizardDataArry[3]=gW_ResolutionWidth;//read inBoard resolution first
							W_WizardDataArry[4]=gW_ResolutionHeight;
							W_WizardDataArry[5]=Scaler3_InHPos;//Scaler0_InHLast+Scaler1_InHLast+Scaler2_InHLast;
							W_WizardDataArry[6]=Scaler3_InHLast-1;
							W_WizardDataArry[7]=Scaler3_InVPos;
							W_WizardDataArry[8]=Scaler3_InVLast-1;					
							W_WizardDataArry[9]=Scaler3_OutHSize;
							W_WizardDataArry[10]=Scaler3_OutVSize;//g_Card4VSize;
							
							CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
							#ifdef DEBUG_STATE_MSG
							printf("3: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
									W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
									W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
									W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
									);
							#endif
						}

			

					//视频开窗-0						
						W_WizardDataArry_T[0]=BOARD_ID;	//Slot 0
						outPort=0;	
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card1HSize;
						W_WizardDataArry_T[3]=g_Card1VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);						
						printf("0: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]	);
						
						scalerNo = 4;
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card1HSize;	//InRight
						W_WizardDataArry_T[5]=g_Card1VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card1HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card1VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("0: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d  \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	
						
						
					//视频开窗-1										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=1;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card2HSize;
						W_WizardDataArry_T[3]=g_Card2VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]	);
						#endif
						if(g_CardLayout==1)
							scalerNo=4;
						else 
							scalerNo=5;						
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==1)
								W_WizardDataArry_T[3]=g_Card1VSize;
							else
								W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card2HSize;	//InRight
						if(g_CardLayout==1)
								W_WizardDataArry_T[5]=g_Card1VSize+g_Card2VSize;
							else
								W_WizardDataArry_T[5]=g_Card2VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card2HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card2VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("1: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();		


						//视频开窗-2										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=2;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card3HSize;
						W_WizardDataArry_T[3]=g_Card3VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("2: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]);
						#endif
						if(g_CardLayout==0)
							scalerNo=6;
						else if(g_CardLayout==1)
							scalerNo=5;
						else
							scalerNo=4;
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==2)
								W_WizardDataArry_T[3]=g_Card1VSize;
							else
								W_WizardDataArry_T[3]=ZERO;	//InTop
						W_WizardDataArry_T[4]=g_Card3HSize;	//InRight
						if(g_CardLayout==2)
								W_WizardDataArry_T[5]=g_Card1VSize+g_Card3VSize;
							else
								W_WizardDataArry_T[5]=g_Card3VSize;	//Inbottom
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card3HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card3VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("2: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	


						//视频开窗-3										
						W_WizardDataArry_T[0]=BOARD_ID;
						outPort=3;
						W_WizardDataArry_T[1]=outPort;	//outport no. 0
						W_WizardDataArry_T[2]=g_Card4HSize;
						W_WizardDataArry_T[3]=g_Card4VSize;						
						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("3: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]);
						#endif
						if(g_CardLayout==0)
							scalerNo=7;
						else 
							scalerNo=5;
						
						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
						W_WizardDataArry_T[1]=1;	//image Layer						
						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
						if(g_CardLayout==0)
								W_WizardDataArry_T[3]=ZERO;	//InTop
							else
								W_WizardDataArry_T[3]=g_Card2VSize;	
							
						W_WizardDataArry_T[4]=g_Card4HSize;	//InRight
						if(g_CardLayout==0)
								W_WizardDataArry_T[5]=g_Card4VSize;	//Inbottom
							else
								W_WizardDataArry_T[5]=g_Card2VSize+g_Card4VSize;	
						W_WizardDataArry_T[6]=ZERO;	//DispLeft
						W_WizardDataArry_T[7]=ZERO;	//DispTop
						W_WizardDataArry_T[8]=g_Card4HSize;	//DispRight
						W_WizardDataArry_T[9]=g_Card4VSize;	//DispBottom
						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
						#ifdef DEBUG_STATE_MSG
						printf("3: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
							);
						#endif
						CMD_INPUTFPGA_ADD_WINDOW_END();	
					}
				printf("1x4,Card Number: %d, Image Number: %d\r\n",g_CardNumber,g_ImageNumber);	

			
			break;
		
		
		}
	
	printf("INPUT_FPGA_SET_UPDATA_SYSTEM_DISPLAY\r\n");
	INPUT_FPGA_SET_UPDATA_SYSTEM_DISPLAY();
	CMD_INPUTFPGA_SET_KEY_MODE(0);
	
	

}




void CMD_INPUTFPGA_CLEAR_SCALER_USE()
	{
			//char B_CommandSource = 0;			
			InputGraphicFPGAClearScalerUse(0);
			//memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			//SendCmdData(B_CommandSource);		
	}
	
	
void CMD_INPUTFPGA_SCALER_SETUP(WORD* pData)
	{
//			char B_CommandSource = 0;	
			WORD W_Data1 = 0;
			WORD W_Data2 = 0;			
			int  i = 0;//, TempData1 = 0;
//			BYTE B_TempData = 0;
			//ST_SignalWindows *Stp_TempSignalWindows = NULL;
			//ST_SignalSource *Stp_TempSignalSource = NULL;
			ST_ScalerType *Stp_TempScaler = NULL;

			if(pData[0] > 3)
				return;
			memset(&St_UserCommadAperture, 0, sizeof(St_UserCommadAperture));
			St_UserCommadAperture.InHoriSize = pData[3];						// < Horizontal size of the input video
			St_UserCommadAperture.InVertSize = pData[4];						// < Vertical size of the input video
			St_UserCommadAperture.InFirstPixel = pData[5];							// < The first pixel index in the input video
			St_UserCommadAperture.InLastPixel = pData[6];						// < The last pixel index in the input video
			St_UserCommadAperture.InFirstLine = pData[7];							// < The first line index in the input video
			St_UserCommadAperture.InLastLine = pData[8];						// < The last line index in the input video
			St_UserCommadAperture.OutHoriSize = pData[9];						// < Horizontal size of the output video
			St_UserCommadAperture.OutVertSize = pData[10];						// < Vertical size of the output video
			
			i = pData[1];
			Stp_TempScaler = &(gStp_ProcessorScaler->St_Scaler[i]);
			Stp_TempScaler->B_SignalSourceSlot = W_DataArry[2];
			Stp_TempScaler->InHoriSize = W_DataArry[3];
			Stp_TempScaler->InVertSize = W_DataArry[4];
			Stp_TempScaler->InFirstPixel = W_DataArry[5];
			Stp_TempScaler->InLastPixel = W_DataArry[6];
			Stp_TempScaler->InFirstLine = W_DataArry[7];
			Stp_TempScaler->InLastLine = W_DataArry[8];
			Stp_TempScaler->OutHoriSize = W_DataArry[9];
			Stp_TempScaler->OutVertSize = W_DataArry[10];

			if((W_DataArry[2] == 0) && (gB_CVBSSwitchEnable == 1))
				{
				GetInputBoardSignalResolution(pData[2], gB_CVBSSwitchIndex, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
				
				}	
			else
				{
				GetInputBoardSignalResolution(pData[2], 0, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
				//printf("NOT (W_DataArry[2] == 0) && (gB_CVBSSwitchEnable == 1)");
				}
			W_Data1 = Stp_TempScaler->InLastPixel - Stp_TempScaler->InFirstPixel + 1;
			W_Data2 = Stp_TempScaler->InLastLine - Stp_TempScaler->InFirstLine + 1;
			
			if((pData[1] == 0) && (W_Data1 > gW_ResolutionWidth) && (W_Data2 > gW_ResolutionHeight))
			{
				gW_ResolutionWidth <<= 1;
				gW_ResolutionHeight <<= 1;
				Stp_TempScaler->B_SignalSource = 1;
			}
			else
				Stp_TempScaler->B_SignalSource = 0;
			
			if((pData[2] < 4) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				{
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 1);
				//printf("(gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0)");
				}
			else
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 0);
			
			InputFPGA_ScalerSetup(pData[1], pData[2] << 1, &St_UserCommadAperture);
			
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				W_Data1 = NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX); i++)
					ParamWrite(W_Data1 + i, pData[i]);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 = (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX)) + NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
					for(i = 0; i < 11; i++)
						ParamWrite(W_Data1 + i, pData[i]);
				}
			#endif		
		
	}	


void CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(WORD outPort)
	{
			
			//WORD W_Data1 = 0;
			//WORD W_Data2 = 0;
			
			InputGraphicFPGAClearOutputWindow(0, outPort);
		
	}	
	
void CMD_INPUTFPGA_SET_WINDOW_LAYOUT(WORD* pData)
{
		WORD W_Data1 = 0;
//		BYTE B_TempData = 0;
		int  i = 0;
		//int TempData1 = 0;
		//WORD W_Data2 = 0;
	

		if(pData[1] >= OUTPUT_BOARD_MAX)
				return;
			ScreenWindowLayoutInit(pData[1]);

			gB_InputFPGASetWindowsLayoutFlag = 1;
			gStp_ScreenRectInput->ActiveRectWindowsNum = 0;
			gStp_ScreenRectInput->B_BoardSlotIndex = pData[0];
			gStp_ScreenRectInput->B_OutputScreenIndex = pData[1];
			gStp_ScreenRectInput->DisplayScreenWidth = pData[2];
			gStp_ScreenRectInput->DisplayScreenHeight = pData[3];
			gW_DisplayScreenWindowsNum = 0;
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_Display0_WINDOWS_BOARD_INDEX + (gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
			for(i = 0; i < 4; i++)
				ParamWrite(W_Data1 + i, pData[i]);
			
			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < 4; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
	
	}
	
	
void CMD_INPUTFPGA_ADD_WINDOW(WORD* pData)
	{
			WORD W_Data1 = 0;
//			WORD W_Data2 = 0;			
			int  i = 0; // TempData1 = 0;
			
			if(gB_InputFPGASetWindowsLayoutFlag == 0)
				return;
			
			if(gW_DisplayScreenWindowsNum >= MaxDiaplayWallWindowsNum)
			{
				gStp_ScreenRectInput->ActiveRectWindowsNum = gW_DisplayScreenWindowsNum;
				ExecuteScreenWindowLayout();
				return;
			}
			Stp_TempSignalWindows = gStp_ScreenRectInput->Stp_RectWindows[gW_DisplayScreenWindowsNum];
			Stp_TempSignalSource = Stp_TempSignalWindows->Stp_SignalSource;
			if((Stp_TempSignalWindows == NULL) || (gW_DisplayScreenWindowsNum >= MaxDiaplayWallWindowsNum))
				return;
			
			Stp_TempSignalSource->SignalSource = pData[0];
			Stp_TempSignalSource->SignalSourceSlot = pData[0];
			Stp_TempSignalSource->OriginalSignalSource = pData[0];
			Stp_TempSignalSource->Windows_Layer = pData[1];
			Stp_TempSignalWindows->SignalInputWindow.left = pData[2];
			Stp_TempSignalWindows->SignalInputWindow.top = pData[3];
			Stp_TempSignalWindows->SignalInputWindow.right = pData[4];
			Stp_TempSignalWindows->SignalInputWindow.bottom = pData[5];
			Stp_TempSignalWindows->SignalDisplayWindow.left = pData[6];
			Stp_TempSignalWindows->SignalDisplayWindow.top = pData[7];
			Stp_TempSignalWindows->SignalDisplayWindow.right = pData[8];
			Stp_TempSignalWindows->SignalDisplayWindow.bottom = pData[9];
			gW_DisplayScreenWindowsNum++;
			
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_Display0_WINDOW0_SIGNAL_SOURCE + (gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
			W_Data1 += ((gW_DisplayScreenWindowsNum - 1) * (NVR_Display0_WINDOW1_IN_LEFT - NVR_Display0_WINDOW0_IN_LEFT));
			for(i = 0; i < (NVR_Display0_WINDOW1_IN_LEFT - NVR_Display0_WINDOW0_IN_LEFT); i++)
				ParamWrite(W_Data1 + i, pData[i]);

			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
				for(i = 0; i < 10; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		
		}
		
		
void CMD_INPUTFPGA_ADD_WINDOW_END()
	{

		WORD W_Data1 = 0;
//	BYTE B_TempData = 0;
//		int  i = 0;
		//int TempData1 = 0;
//		WORD W_Data2 = 0;

		if(gB_InputFPGASetWindowsLayoutFlag == 0)
				return;
			if(gW_DisplayScreenWindowsNum > 0)
			{
				#if(USER_COMMAD_DEBUG == 1)
					printf("INPUT_FPGA ADD_WINDOWS_END: gW_DisplayScreenWindowsNum = %d\r\n", gW_DisplayScreenWindowsNum);
				#endif
				
				gStp_ScreenRectInput->ActiveRectWindowsNum = gW_DisplayScreenWindowsNum;
				ExecuteScreenWindowLayout();
				gB_InputFPGASetWindowsLayoutFlag = 0;
				g_DisplayModeIndex = 0;
				ParamWrite (NVR_DISPLAY_MODE_INDEX, g_DisplayModeIndex);
				
			
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM);
				ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_SCALER0_BOARD_INDEX));
					ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				}
//				gB_KeyboardData = 0;
				#endif
			}
			return;
	
	}
	
	
void CMD_BOARD_RMG()
{
			char NumberOfInputCard=0;			
			char NumberOfOutputCard=0;
			NumberOfInputCard = gSt_InputBoardADV7604.B_ActiveChip +gSt_InputBoardGV7601.B_ActiveChip;// +gSt_InputBoarTVP5158.B_ActiveChip ;
			NumberOfOutputCard= gSt_OutputBoarSiI9136.B_ActiveChip;		
				
	}	
	
	


void CMD_BOARD_RISR(BYTE Slot_ID)
{
	
	GetInputBoardSignalResolution(BOARD_ID, Slot_ID, &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
	#ifdef DEBUG_STATE_MSG
	printf("gW_ResolutionWidth: %d,gW_ResolutionHeight: %d\r\n"
									,gW_ResolutionWidth,
									gW_ResolutionHeight
									);	
	#endif
}
	


void INPUT_FPGA_SET_UPDATA_SYSTEM_DISPLAY()
{
//cmd1.18 input fpga set updata system display
		//if((strcmp(pOperation, "SET_UPDATA_SYSTEM_DISPLAY") == 0) || (strcmp(pOperation, "SUSD") == 0))
		//{
			InputFPGASetUpdata();
			//memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			//sprintf(UserCommadSendStr, "INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY\r\n");
			//SendCmdData(B_CommandSource);
		//	return;
		//}

}



		//cmd1.12 input fpga set key mode
void CMD_INPUTFPGA_SET_KEY_MODE(char iMode)
		//if(((strcmp(pOperation, "SET_KEY_MODE") == 0) || (strcmp(pOperation, "SKM") == 0)) && (DataNum >= 1))
		{
			
			if((iMode > 0) && (iMode < DISPLAY_MODE_INDEX_MAX))
				gB_KeyDisplayMode =iMode;
			else
				gB_KeyDisplayMode = 0;

			#ifdef DEBUG_STATE_MSG
				printf("INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			#endif
			
			return;
		}

void Write_EEPROM_WizardPara()
{
	Write_EEPROM_U16(E2_ADDR_CardNumber ,   g_CardNumber    ); 
	Write_EEPROM_U16(E2_ADDR_CardLayout ,   g_CardLayout    ); 
	Write_EEPROM_U16(E2_ADDR_Card1HSize ,   g_Card1HSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card1VSize ,   g_Card1VSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card2HSize ,   g_Card2HSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card2VSize ,   g_Card2VSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card3HSize ,   g_Card3HSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card3VSize ,   g_Card3VSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card4HSize ,   g_Card4HSize    ); 
	Write_EEPROM_U16(E2_ADDR_Card4VSize ,   g_Card4VSize    ); 
	Write_EEPROM_U16(E2_ADDR_ImageNumber,   g_ImageNumber    ); 
	Write_EEPROM_U16(E2_ADDR_ImageLayout,   g_ImageLayout    ); 
	Write_EEPROM_U16(E2_ADDR_Image1Hsize,   g_Image1Hsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image1Vsize,   g_Image1Vsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image1Hpos ,   g_Image1Hpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image1Vpos ,   g_Image1Vpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image2Hsize,   g_Image2Hsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image2Vsize,   g_Image2Vsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image2Hpos ,   g_Image2Hpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image2Vpos ,   g_Image2Vpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image3Hsize,   g_Image3Hsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image3Vsize,   g_Image3Vsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image3Hpos ,   g_Image3Hpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image3Vpos ,   g_Image3Vpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image4Hsize,   g_Image4Hsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image4Vsize,   g_Image4Vsize    ); 
	Write_EEPROM_U16(E2_ADDR_Image4Hpos ,   g_Image4Hpos    ); 
	Write_EEPROM_U16(E2_ADDR_Image4Vpos ,   g_Image4Vpos    ); 
	I2C_ByteWrite_24C(E2_ADDR_Image1Scr  ,   g_Image1Scr     ); 
	I2C_ByteWrite_24C(E2_ADDR_Image2Scr  ,   g_Image2Scr     ); 
	I2C_ByteWrite_24C(E2_ADDR_Image3Scr  ,   g_Image3Scr     ); 
	I2C_ByteWrite_24C(E2_ADDR_Image4Scr  ,   g_Image4Scr     ); 

	#ifdef DEBUG_STATE_MSG
		printf("Write_EEPROM_WizardPara,Succeed!\r\n");
	#endif


}


void Read_EEPROM_WizardPara()
{
	Read_EEPROM_U16(E2_ADDR_CardNumber , &g_CardNumber    );      
	Read_EEPROM_U16(E2_ADDR_CardLayout ,   &g_CardLayout    );      
	Read_EEPROM_U16(E2_ADDR_Card1HSize ,   &g_Card1HSize    );      
	Read_EEPROM_U16(E2_ADDR_Card1VSize ,   &g_Card1VSize    );      
	Read_EEPROM_U16(E2_ADDR_Card2HSize ,   &g_Card2HSize    );      
	Read_EEPROM_U16(E2_ADDR_Card2VSize ,   &g_Card2VSize    );      
	Read_EEPROM_U16(E2_ADDR_Card3HSize ,   &g_Card3HSize    );      
	Read_EEPROM_U16(E2_ADDR_Card3VSize ,   &g_Card3VSize    );      
	Read_EEPROM_U16(E2_ADDR_Card4HSize ,   &g_Card4HSize    );      
	Read_EEPROM_U16(E2_ADDR_Card4VSize ,   &g_Card4VSize    );      
	Read_EEPROM_U16(E2_ADDR_ImageNumber,   &g_ImageNumber    );     
	Read_EEPROM_U16(E2_ADDR_ImageLayout,   &g_ImageLayout    );     
	Read_EEPROM_U16(E2_ADDR_Image1Hsize,   &g_Image1Hsize    );     
	Read_EEPROM_U16(E2_ADDR_Image1Vsize,   &g_Image1Vsize    );     
	Read_EEPROM_U16(E2_ADDR_Image1Hpos ,   &g_Image1Hpos    );      
	Read_EEPROM_U16(E2_ADDR_Image1Vpos ,   &g_Image1Vpos    );      
	Read_EEPROM_U16(E2_ADDR_Image2Hsize,   &g_Image2Hsize    );     
	Read_EEPROM_U16(E2_ADDR_Image2Vsize,   &g_Image2Vsize    );     
	Read_EEPROM_U16(E2_ADDR_Image2Hpos ,   &g_Image2Hpos    );      
	Read_EEPROM_U16(E2_ADDR_Image2Vpos ,   &g_Image2Vpos    );      
	Read_EEPROM_U16(E2_ADDR_Image3Hsize,   &g_Image3Hsize    );     
	Read_EEPROM_U16(E2_ADDR_Image3Vsize,   &g_Image3Vsize    );     
	Read_EEPROM_U16(E2_ADDR_Image3Hpos ,   &g_Image3Hpos    );      
	Read_EEPROM_U16(E2_ADDR_Image3Vpos ,   &g_Image3Vpos    );      
	Read_EEPROM_U16(E2_ADDR_Image4Hsize,   &g_Image4Hsize    );     
	Read_EEPROM_U16(E2_ADDR_Image4Vsize,   &g_Image4Vsize    );     
	Read_EEPROM_U16(E2_ADDR_Image4Hpos ,   &g_Image4Hpos    );      
	Read_EEPROM_U16(E2_ADDR_Image4Vpos ,   &g_Image4Vpos    );      
	Read_EEPROM_Byte(E2_ADDR_Image1Scr  ,   &g_Image1Scr     );      
	Read_EEPROM_Byte(E2_ADDR_Image2Scr  ,   &g_Image2Scr     );      
	Read_EEPROM_Byte(E2_ADDR_Image3Scr  ,   &g_Image3Scr     );      
	Read_EEPROM_Byte(E2_ADDR_Image4Scr  ,   &g_Image4Scr     );      

	#ifdef DEBUG_STATE_MSG
		printf("Read_EEPROM_WizardPara,Succeed!\r\n");
	#endif

}






/********************************  END  *************************************/
