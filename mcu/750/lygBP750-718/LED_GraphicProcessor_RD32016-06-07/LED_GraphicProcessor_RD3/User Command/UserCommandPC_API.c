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
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "type.h"
#include "Serial.h"
#include "ASCII.h"
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "D_IIC3.h"
#include "InitDevice.h"
#include "tcp_echoserver.h"

#include "../Device/InputFPGA/InputFPGA.h"
#include "../Device/GV7601/gv7601.h"
#include "../Device/ClockGeneration/SL38160.h"
#include "../MultiSignalBoard/param.h"
#include "../MultiSignalBoard/InputBoardADV7604.h"
#include "../MultiSignalBoard/InputBoardGV7601.h"
#include "../MultiSignalBoard/OutputBoardSiI9136.h"
#include "../MultiSignalBoard/InputBoardTVP5158.h"
#include "../MultiSignalBoard/InputBoardGraphicFPGA.h"
#include "../WindowsLayout/ScreenWindowLayout.h"
#include "UserCommandPC_API.h"

//#define CR     0x0D
#define TCP_REV_COMMAND_ECHO				0

#define USER_COMMAD_BUF_LENGTH     	128
#define USER_COMMAD_DEBUG						0
#define USER_COMMAD_SEND_STR_LEN		128
#define USER_COMMAD_SEND_STR1_LEN		64

#define DEBUG_LOAD_USER_LAST_MODE		0
#define SAVE_MESSAGE_TO_EEPROM			1
#define DISPLAY_MODE_INDEX_MAX			11
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
int ParseCommad(uint8_t B_CommandSource, uint8_t* pCommad);
BYTE ParseCommadByBrillViewType1(BYTE B_Data);
void ExcuteUserCommad(BYTE B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData);
void ExcuteUserBinaryCommad(BYTE B_CommandSource, unsigned short CommandData, WORD DataNum, unsigned short* pData);
void LoadUserLastMode(BYTE B_ModeIndex);
void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture);
BYTE CheckEquipmentIntegrity(void);
BYTE LoadUserKeyMode(BYTE B_Data);
void LCD_Display(BYTE B_XStart, BYTE B_YStart, char* pStr);

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
static BYTE	gB_IICBusIndex = 0, gB_SlotIndex = 0, gB_InputFPGASetWindowsLayoutFlag = 0;
static WORD	gW_DisplayScreenWindowsNum = 0;
static char DevStr[40] = {0}, OperationStr[40] = {0};
static WORD W_DataArry[USER_COMMAD_BUF_LENGTH >> 1] = {0};
static char UserCommadSendStr[USER_COMMAD_SEND_STR_LEN] = {0};
static char UserCommadSendStr1[USER_COMMAD_SEND_STR1_LEN] = {0};
static unsigned short W_BinaryCommadSendDataArry[USER_COMMAD_BUF_LENGTH] = {0};

static XScalerAperture St_UserCommadAperture;
static ST_SignalSource St_UserCommadSignalSource;
static ST_SignalWindows St_UserCommadSignalWin;

static BYTE gB_BrillViewType1PacketStartFlag = 0;
static BYTE gB_BrillViewType1PacketRecvFlag = 0;
static BYTE	gB_BrillViewType1CommadIndex=0;
#define BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH		64
static BYTE	gB_BrillViewType1CommadBuf[BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH];
//static BYTE	gB_LCD_DisplayBuf[USER_COMMAD_BUF_LENGTH];
static BYTE gB_KeyDisplayMode = 0;
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static BYTE ChangeBrillViewType1CommadToSelfCommad(BYTE* pCommad);
//static BYTE TCP_SendData	(BYTE *Bp_Buf, int W_Len);
static BYTE TCP_SendBinaryData(BYTE *Bp_Buf, int W_Len);

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

BYTE Serial_SendData(BYTE *Bp_Buf, int W_Len)
{
	printf("%s", Bp_Buf);
	return 1;
}

BYTE Serial_SendBinaryData(BYTE B_CommandSource, BYTE *Bp_Buf, int W_Len)
{
//	Serial1_SendData(Bp_Buf, W_Len);
	if(B_CommandSource == COMMAD_FROM_SERIAL1)
		Serial1_SendData(Bp_Buf, W_Len);
	if(B_CommandSource == COMMAD_FROM_SERIAL5)
		Serial5_SendData(Bp_Buf, W_Len);
	return 1;
}

void ExcuteUserCommad(BYTE B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData)
{
	WORD W_Data1 = 0;
	BYTE B_TempData = 0;
	int  i = 0, TempData1 = 0;
	WORD W_Data2 = 0;
	ST_SignalWindows *Stp_TempSignalWindows = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_ScalerType *Stp_TempScaler = NULL;
	if((strcmp(pDevice, "INPUT_FPGA") == 0) || (strcmp(pDevice, "I_F") == 0))
	{
		if((strcmp(pOperation, "READ") == 0) && (DataNum >= 1))
		{
			W_Data1 = InputFPGA_ReadWord(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA READ %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], W_Data1);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, Input FPGA RD [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], W_Data1);
			#endif
			return;
		}
		if((strcmp(pOperation, "WRITE") == 0) && (DataNum >= 2))
		{
			InputFPGA_WriteWord(pData[0], pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA WRITE %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, Input FPGA WR [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1]);
			#endif
			return;
		}
		if((strcmp(pOperation, "VIDEO_OUTPUT_DISALBLE") == 0) && (DataNum >= 1))
		{
			if(pData[0] < OUTPUT_BOARD_MAX)
				InputFPGASetVideoOutputDisalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputDisalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_DISALBLE ERROR!\r\n");
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA VIDEO_OUTPUT_DISALBLE %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
		}
		if((strcmp(pOperation, "VIDEO_OUTPUT_ENABLE") == 0) && (DataNum >= 1))
		{
			if(pData[0] < OUTPUT_BOARD_MAX)
				InputFPGASetVideoOutputEnalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputEnalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_ENABLE ERROR!\r\n");
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA VIDEO_OUTPUT_ENABLE %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if((strcmp(pOperation, "CLEAR_OUTPUT_WIN") == 0) && (DataNum >= 1))
		{
			W_Data2 = pData[0];
			if(InputFPGAClearBackgroundWindow(pData[0]) == 0xFF)
				printf("INPUT_FPGA CLEAR_OUTPUT_WIN ERROR!\r\n");
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_OUTPUT_WIN %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
				#endif
				return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if((strcmp(pOperation, "SOFT_RESET") == 0) && (DataNum >= 1))
		{
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, pData[0]);
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0x00);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SOFT_RESET %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if((strcmp(pOperation, "SCALER_SETUP") == 0) && (DataNum >= 11))
		{
			if(pData[0] >= MaxOneInputBoardScalerNum)
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
			if((pData[2] < INPUT_SIGNAL_PORT_MAX) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 1);
			else
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 0);
			InputFPGA_ScalerSetup(pData[1], pData[2] << 1, &St_UserCommadAperture);
			
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SCALER_SETUP %d %d %d\r\n", pData[0], pData[1], pData[2]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(TCP_REV_COMMAND_ECHO == 1)
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT BOARD SCALER SETUP OK!\r\n");
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
			for(i = 0; i < (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX); i++)
				ParamWrite(W_Data1 + i, pData[i]);
			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 = (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START)) + NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 11; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
		if((strcmp(pOperation, "SCALER_INIT") == 0) && (DataNum >= 1))
		{
			if(pData[0] < MaxOneInputBoardScalerNum)
				InputFPGA_ScalerInit(pData[0]);
		}
		if((strcmp(pOperation, "SCALER_READ_ALL") == 0) && (DataNum >= 1))
		{
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, Scaler %d, Read All\r\n", gB_SlotIndex, pData[0]);
			#endif
			InputFPGA_PrintScalerVar(pData[0]);
			return;
		}
		if((strcmp(pOperation, "SCALER_READ_WORD") == 0) && (DataNum >= 2))
		{
			W_Data1 = InputFPGA_ScalerReadWord(pData[0], pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SCALER_READ_WORD %d %d 0x%08X 0x%08X\r\n", gB_SlotIndex, pData[0], pData[1], W_Data1);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Slot %d, IIC %d, SCALER_READ %d, 0x%08X 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if(((strcmp(pOperation, "SET_KEY_MODE") == 0) || (strcmp(pOperation, "SKM") == 0)) && (DataNum >= 1))
		{
			if((pData[0] > 0) && (pData[0] < DISPLAY_MODE_INDEX_MAX))
				gB_KeyDisplayMode = pData[0];
			else
				gB_KeyDisplayMode = 0;
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			#endif
			return;
		}
		if(((strcmp(pOperation, "SET_TEST_PATTERN_COLOR") == 0) || (strcmp(pOperation, "STPC") == 0)) && (DataNum > 2))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			TempData1 = InputFPGA_SetTestPatternColor(pData[0], pData[1], pData[2]);
			if(TempData1 == 0xFF)
				sprintf(UserCommadSendStr, "INPUT_FPGA SET_TEST_PATTERN_COLOR %d %d 0x%08X Error\r\n", pData[0], pData[1], pData[2]);
			else
				sprintf(UserCommadSendStr, "INPUT_FPGA SET_TEST_PATTERN_COLOR %d %d 0x%08X\r\n", pData[0], pData[1], pData[2]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
			printf("%s", UserCommadSendStr);
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if(((strcmp(pOperation, "SET_WINDOWS_LAYOUT") == 0) || (strcmp(pOperation, "SWL") == 0)) && (DataNum >= 4))
		{
			if(pData[1] >= MaxOneInputBoardDisplayPortNum)
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(TCP_REV_COMMAND_ECHO == 1)
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
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
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 4; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(TCP_REV_COMMAND_ECHO == 1)
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
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
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 10; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
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
				memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
				sprintf(UserCommadSendStr, "INPUT_FPGA ADD_WINDOWS_END\r\n");
				if(B_CommandSource == 0)
					Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				#if(TCP_REV_COMMAND_ECHO == 1)
					else
						TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				ParamWrite (NVR_DISPLAY_MODE_INDEX, g_DisplayModeIndex);
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM);
				ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					W_Data1 -= NVR_RELOAD_START;
					ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				}
//				gB_KeyboardData = 0;
				#endif
			}
			return;
		}
		if((strcmp(pOperation, "SET_UPDATA_SYSTEM_DISPLAY") == 0) || (strcmp(pOperation, "SUSD") == 0))
		{
			InputFPGASetUpdata();
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY\r\n");
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if(((strcmp(pOperation, "CLEAR_SCALER_USE") == 0) || (strcmp(pOperation, "CSUSE") == 0)) && (DataNum >= 1))
		{
			InputGraphicFPGAClearScalerUse(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_SCALER_USE %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(TCP_REV_COMMAND_ECHO == 1)
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
			return;
		}
		if(((strcmp(pOperation, "CLEAR_OUTPUT_WINDOW") == 0) || (strcmp(pOperation, "COWIN") == 0)) && (DataNum >= 2))
		{
			if((pData[0] >= DEVICE_INPUT_GRAPHIC_FPGA_MAX) || (pData[1] >= MaxOneInputBoardDisplayPortNum))
				return;
			W_Data2 = pData[1];
			InputGraphicFPGAClearOutputWindow(pData[0], pData[1]);
			if(W_Data2 < MaxOneInputBoardDisplayPortNum)
				gSt_ScreenRectInput[W_Data2].ActiveRectWindowsNum = 0;
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA CLEAR_OUTPUT_WINDOW %d %d\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(TCP_REV_COMMAND_ECHO == 1)
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					W_Data1 -= NVR_RELOAD_START;
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
	}
	if(strcmp(pDevice, "BOARD") == 0)
	{
		if((strcmp(pOperation, "PARAM_RESET") == 0) && (DataNum >= 1))
		{
			if(pData[0] == 1)
			{
				ParamWrite(NVR_CHECKSUM, 0);		// write checksum
				ParamInit();
				DevicesInit();
				LoadUserLastMode(0);
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_RESET %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}
		if((strcmp(pOperation, "PARAM_READ") == 0) && (DataNum >= 1))
		{
			W_Data1 = ParamRead(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_READ 0x%04X 0x%04X\r\n", pData[0], W_Data1);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}
		if((strcmp(pOperation, "PARAM_WRITE") == 0) && (DataNum >= 2))
		{
			ParamWrite(pData[0], pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD PARAM_WRITE 0x%04X 0x%04X\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}
		if((strcmp(pOperation, "DVI_HPD_SET") == 0) && (DataNum >= 2))
		{
			return;
		}
		if((strcmp(pOperation, "SET_CLK") == 0) && (DataNum >= 1))
		{
			/*
			SetBoardIDCLK(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SET_CLK %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			*/
			return;
		}
		if((strcmp(pOperation, "ADV7604_PROGRAM_EDID") == 0) && (DataNum > 1))
		{
			InputBoardADV7604ProgramEDID((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_PROGRAM_EDID %d %d\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_PROGRAM_EDID %d\r\n", pData[0]);
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_SELECT_INPUT_PORT") == 0) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputPort((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SELECT_INPUT_PORT %d %d\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_INPUT_PORT %d %d\r\n", pData[0], pData[1]);
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 = NVR_ADV7604_DEV0_PORT + pData[0];
					W_Data1 -= NVR_RELOAD_START;
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					ParamWrite(W_Data1, pData[1]);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_SELECT_TIMINGS") == 0) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputTiming((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SELECT_TIMINGS %d %d\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_TIMINGS %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_LOG_STATUS") == 0) && (DataNum > 0))
		{
			InputBoardADV7604PrintLogStatus((BYTE) (pData[0]));
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_LOG_STATUS %d\r\n", pData[0]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_LOG_STATUS %d\r\n", pData[0]);
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_SET_CTRL") == 0) && (DataNum > 2))
		{
			InputBoardADV7604SetCtrl((BYTE) (pData[0]), pData[1], pData[2]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SET_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 = NVR_ADV7604_DEV0_BRIGHTNESS + (pData[0] * 4) + pData[1];
					W_Data1 -= NVR_RELOAD_START;
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_GET_CTRL") == 0) && (DataNum >= 2))
		{
			W_Data1 = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (pData[0] * 4) + pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_GET_CTRL %d %d %d\r\n", pData[0], pData[1], W_Data1);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_GET_CTRL %d %d %d\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if((strcmp(pOperation, "ADV7604_SET_DV_CTRL") == 0) && (DataNum > 2))
		{
			InputBoardADV7604SetDVCtrl((BYTE) (pData[0]), pData[1], pData[2]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD ADV7604_SET_DV_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_DV_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}
		if((strcmp(pOperation, "TVP5158_SET_CTRL") == 0) && (DataNum >= 4))
		{
			if(pData[2] == 0)
			{
				InputBoardTVP5158SetLuminanceBrightness(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_BRIGHTNESS + (pData[0] * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 1)
			{
				InputBoardTVP5158SetLuminanceContrast(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_CONTRAST + (pData[0] * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 2)
			{
				InputBoardTVP5158SetLuminanceSaturation(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_SATURATION + (pData[0] * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 3)
			{
				InputBoardTVP5158SetLuminanceHue(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_HUE + (pData[0] * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_HUE - NVR_TVP5158_DEV0_PORT0_HUE));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD TVP5158_SET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[3]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_SET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[3]);
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_GET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], W_Data1);
			#endif
			return;
		}
		if((strcmp(pOperation, "GV7601_READ") == 0) && (DataNum > 0))
		{
			GV7601_GSPI_Init();
			GV7601_ReadWord(pData[0], pData[1], &W_Data1);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GV7601 %d READ [0x%04X = 0x%04X]\r\n", pData[0], pData[1], W_Data1);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d READ [0x%04X = 0x%04X]\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if((strcmp(pOperation, "GV7601_WRITE") == 0) && (DataNum > 0))
		{
			GV7601_GSPI_Init();
			GV7601_WriteWord(pData[0], pData[1], pData[2]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GV7601 %d WRITE [0x%04X = 0x%04X]\r\n", pData[0], pData[1], pData[2]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d WRITE [0x%04X = 0x%04X]\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if(((strcmp(pOperation, "READ_INPUT_SOURCE_RESOLUTION") == 0) || (strcmp(pOperation, "RISR") == 0)) && (DataNum > 1))
		{
			GetInputBoardSignalResolution(pData[0], pData[1], &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
//			InputBoardGV7601PrintSignalStd(pData[0]);
//			InputBoardADV7604PrintLogStatus(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD READ_INPUT_SOURCE_RESOLUTION %d %d %d %d %d %d\r\n", pData[0], pData[1], gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD READ_INPUT_SOURCE_RESOLUTION %d %d %d %d %d %d\r\n", pData[0], pData[1], gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight);
			#endif
			return;
		}
		if((strcmp(pOperation, "SET_INPUT_VIDEO_SWITCH") == 0) && (DataNum > 1))
		{
			if(gB_CVBSSwitchEnable == 1)
			{
				if(pData[0] <= 5)
				{
					W_Data1 = NVR_INPUT0_VIDEO_SWITCH + pData[0];
					#if(SAVE_MESSAGE_TO_EEPROM == 1)
						ParamWrite(W_Data1, (BYTE) pData[1]);
						if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
						{
							W_Data1 -= NVR_RELOAD_START;
							W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
							ParamWrite(W_Data1, pData[1]);
						}
					#endif
				}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD SET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], W_Data1);
			#endif
			return;
		}
		if(((strcmp(pOperation, "READ_SLOT_MESSAGE") == 0) || (strcmp(pOperation, "RSMG") == 0)) && (DataNum > 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			if(pData[0] < (OUTPUT_BOARD_MAX + INPUT_BOARD_MAX))
			{
				B_TempData = GetBoardSlotType(pData[0]);
				sprintf(UserCommadSendStr, "BOARD READ_SLOT_MESSAGE %d %d\r\n", pData[0], B_TempData);
				if(B_CommandSource == 0)
					Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				else
					TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			}
			else if(pData[0] == 0x0F)
			{
				for(i = 0; i < (OUTPUT_BOARD_MAX + INPUT_BOARD_MAX); i++)
				{
					memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
					B_TempData = GetBoardSlotType(i);
					sprintf(UserCommadSendStr, "BOARD READ_SLOT_MESSAGE %d %d\r\n", i, B_TempData);
					if(B_CommandSource == 0)
						Serial_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
					else
						TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
				}
			}
			else
				printf("BOARD READ_SLOT_MESSAGE ERROR!\r\n");
			return;
		}
		if((strcmp(pOperation, "SET_OUTPUT_CLK") == 0) && (DataNum >= 2))
		{
			Set_SL38160_CLK(DEVICEID_SL38160_17, pData[0], pData[1]);		// AUDIO_CLK: pData[0], VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
			ParamWrite(NVR_CUSTOM_RES_CLK, pData[1]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "INPUT_FPGA SET_OUTPUT_CLK %d %d\r\n", pData[0], pData[1]);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if((strcmp(pOperation, "GET_OUTPUT_RESOLUTION") == 0) || (strcmp(pOperation, "GOR") == 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GET_OUTPUT_RESOLUTION %d %d %d\r\n", g_ScreenOutputWidth, g_ScreenOutputHeight, g_VFreq);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
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
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
		if((strcmp(pOperation, "GET_OUTPUT_TIMING") == 0) || (strcmp(pOperation, "GOT") == 0))
		{
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD GET_OUTPUT_TIMING %d\r\n", B_TempData);
			if(B_CommandSource == COMMAD_FROM_SERIAL1)
				Serial1_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else if(B_CommandSource == COMMAD_FROM_SERIAL5)
				Serial5_SendData((uint8_t*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			return;
		}
	}
}

// Device def
#define DEVICE_INPUT_FPGA 																	0
#define DEVICE_BOARD 																				1
// Operation def
#define OPERATION_READ																			0
#define OPERATION_WRITE																			1
#define OPERATION_VIDEO_OUTPUT_DISALBLE                     2
#define OPERATION_VIDEO_OUTPUT_ENABLE                       3
#define OPERATION_CLEAR_OUTPUT_WIN                          4
#define OPERATION_SET_BACKGROUND_WINDOW                     5
#define OPERATION_SOFT_RESET                                6
#define OPERATION_SCALER_SETUP                              7
#define OPERATION_SCALER_INIT                               8
#define OPERATION_SCALER_READ_ALL														9
#define OPERATION_SCALER_READ_WORD                          10
#define OPERATION_SET_KEY_MODE                              11
#define OPERATION_SET_TEST_PATTERN_COLOR                    12
#define OPERATION_LOAD_DISPLAY_MODE                         13
#define OPERATION_SET_WINDOWS_LAYOUT                        14
#define OPERATION_ADD_WINDOWS                               15
#define OPERATION_ADD_WINDOWS_END                           16
#define OPERATION_SET_UPDATA_SYSTEM_DISPLAY                 17
#define OPERATION_CLEAR_SCALER_USE                          18
#define OPERATION_CLEAR_OUTPUT_WINDOW                       19

#define OPERATION_READ_ID                                   0
#define OPERATION_SLOT_RESET                                1
#define OPERATION_PARAM_RESET                               2
#define OPERATION_PARAM_READ                                3
#define OPERATION_PARAM_WRITE                               4
#define OPERATION_SELECT_SLOT                               5
#define OPERATION_DVI_HPD_SET                               6
#define OPERATION_SET_CLK                                   7
#define OPERATION_ADV7604_PROGRAM_EDID                      8
#define OPERATION_ADV7604_SELECT_INPUT_PORT                 9
#define OPERATION_ADV7604_SELECT_TIMINGS                    10
#define OPERATION_ADV7604_LOG_STATUS                        11
#define OPERATION_ADV7604_SET_CTRL                          12
#define OPERATION_ADV7604_GET_CTRL                          13
#define OPERATION_ADV7604_SET_DV_CTRL                       14
#define OPERATION_TVP5158_SET_CTRL                          15
#define OPERATION_TVP5158_GET_CTRL                          16
#define OPERATION_GV7601_READ                               17
#define OPERATION_GV7601_WRITE                              18
#define OPERATION_READ_MESSAGE                              19
#define OPERATION_READ_INPUT_SOURCE_RESOLUTION              20
#define OPERATION_SET_INPUT_VIDEO_SWITCH                    21
#define OPERATION_GET_INPUT_VIDEO_SWITCH                    22
#define OPERATION_READ_SLOT_MESSAGE                         23
#define OPERATION_SET_OUTPUT_CLK                            24
#define OPERATION_SET_CUSTOM_RESOLUTION                     25
#define OPERATION_GET_OUTPUT_RESOLUTION                     26
#define OPERATION_SET_OUTPUT_TIMING                         27
#define OPERATION_GET_OUTPUT_TIMING										      28

unsigned short CalculateCheckSum(WORD DataNum, unsigned short* pData)
{
	unsigned short W_TempData = 0;
	int i = 0;
	for(i = 0; i < DataNum; i++)
		W_TempData += pData[i];
	W_TempData = 0xFFFF - W_TempData;
	W_TempData++;
	return W_TempData;
}

void ExcuteUserBinaryCommad(BYTE B_CommandSource, unsigned short CommandData, WORD DataNum, unsigned short* pData)
{
	WORD W_Data1 = 0;
	BYTE B_TempData = 0;
	int  i = 0, TempData1 = 0;
	WORD W_Data2 = 0;
	ST_SignalWindows *Stp_TempSignalWindows = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_ScalerType *Stp_TempScaler = NULL;
//	printf("CommandData = 0x%04X, DataNum = 0x%04X\r\n", CommandData, DataNum);
	if((CommandData >> 8) == DEVICE_INPUT_FPGA)
	{
		if(((CommandData & 0xFF) == OPERATION_READ) && (DataNum >= 2))
		{
			W_Data2 = pData[1] << 16;
			W_Data2 += pData[0];
			W_Data1 = InputFPGA_ReadWord(W_Data2);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 6;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gB_SlotIndex;
			W_BinaryCommadSendDataArry[4] = pData[0];
			W_BinaryCommadSendDataArry[5] = pData[1];
			W_BinaryCommadSendDataArry[6] = W_Data1;
			W_BinaryCommadSendDataArry[7] = W_Data1 >> 16;
			W_BinaryCommadSendDataArry[8] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, Input FPGA RD [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_WRITE) && (DataNum >= 2))
		{
			W_Data1 = pData[1] << 16;
			W_Data1 += pData[0];
			W_Data2 = pData[2] << 16;
			W_Data2 += pData[3];
			InputFPGA_WriteWord(W_Data1, W_Data2);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 6;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gB_SlotIndex;
			W_BinaryCommadSendDataArry[4] = pData[0];
			W_BinaryCommadSendDataArry[5] = pData[1];
			W_BinaryCommadSendDataArry[6] = pData[2];
			W_BinaryCommadSendDataArry[7] = pData[3];
			W_BinaryCommadSendDataArry[8] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, IIC %d, Input FPGA WR [0x%08X] = 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_VIDEO_OUTPUT_DISALBLE) && (DataNum >= 1))
		{
			if(pData[0] < OUTPUT_BOARD_MAX)
				InputFPGASetVideoOutputDisalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputDisalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_DISALBLE ERROR!\r\n");
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
		}
		if(((CommandData & 0xFF) == OPERATION_VIDEO_OUTPUT_ENABLE) && (DataNum >= 1))
		{
			if(pData[0] < OUTPUT_BOARD_MAX)
				InputFPGASetVideoOutputEnalble(BIT0 << pData[0]);
			else if(pData[0] == 0x0F)
				InputFPGASetVideoOutputEnalble(0x0F);
			else
				printf("INPUT_FPGA VIDEO_OUTPUT_ENABLE ERROR!\r\n");
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_CLEAR_OUTPUT_WIN) && (DataNum >= 1))
		{
			W_Data2 = pData[0];
			if(InputFPGAClearBackgroundWindow(pData[0]) == 0xFF)
				printf("INPUT_FPGA CLEAR_OUTPUT_WIN ERROR!\r\n");
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					W_Data1 -= NVR_RELOAD_START;
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
				#endif
				return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_BACKGROUND_WINDOW) && (DataNum >= 13))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 1;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SOFT_RESET) && (DataNum >= 1))
		{
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, pData[0]);
			InputFPGA_WriteWord(INPUT_FPGA_SOFT_RESET, 0x00);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SCALER_SETUP) && (DataNum >= 11))
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
			if((pData[2] < INPUT_SIGNAL_PORT_MAX) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 1);
			else
				InputFPGA_ScalerSetNoSignalOutput(pData[1], 0);
			InputFPGA_ScalerSetup(pData[1], pData[2] << 1, &St_UserCommadAperture);
			
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT BOARD SCALER SETUP OK!\r\n");
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
			W_Data1 = NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
			for(i = 0; i < (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX); i++)
				ParamWrite(W_Data1 + i, pData[i]);
			if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
			{
				W_Data1 = (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START)) + NVR_SCALER0_BOARD_INDEX + (pData[1] * (NVR_SCALER1_BOARD_INDEX - NVR_SCALER0_BOARD_INDEX));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 11; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SCALER_INIT) && (DataNum >= 1))
		{
			if(pData[0] < INPUT_BOARD_SCALER_MAX)
				InputFPGA_ScalerInit(pData[0]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
		}
		if(((CommandData & 0xFF) == OPERATION_SCALER_READ_ALL) && (DataNum >= 1))
		{
			#if(USER_COMMAD_DEBUG == 1)
				printf("Board %d, Scaler %d, Read All\r\n", gB_SlotIndex, pData[0]);
			#endif
			InputFPGA_PrintScalerVar(pData[0]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SCALER_READ_WORD) && (DataNum >= 2))
		{
			W_Data1 = InputFPGA_ScalerReadWord(pData[0], pData[1]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 5;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gB_SlotIndex;
			W_BinaryCommadSendDataArry[4] = pData[0];
			W_BinaryCommadSendDataArry[5] = pData[1];
			W_BinaryCommadSendDataArry[6] = W_Data1;
			W_BinaryCommadSendDataArry[7] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("Slot %d, IIC %d, SCALER_READ %d, 0x%08X 0x%08X\r\n", gB_SlotIndex, gB_IICBusIndex, pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_KEY_MODE) && (DataNum >= 1))
		{
			if((pData[0] > 0) && (pData[0] < DISPLAY_MODE_INDEX_MAX))
				gB_KeyDisplayMode = pData[0];
			else
				gB_KeyDisplayMode = 0;
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gB_KeyDisplayMode;
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
			printf("INPUT_FPGA SET_KEY_MODE %d\r\n", gB_KeyDisplayMode);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_TEST_PATTERN_COLOR) && (DataNum > 2))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			TempData1 = InputFPGA_SetTestPatternColor(pData[0], pData[1], pData[2]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 6;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gB_SlotIndex;
			W_BinaryCommadSendDataArry[4] = pData[0];
			W_BinaryCommadSendDataArry[5] = pData[1];
			W_BinaryCommadSendDataArry[6] = pData[2];
			if(TempData1 == 0xFF)
				W_BinaryCommadSendDataArry[7] = 0xFF;
			else
				W_BinaryCommadSendDataArry[7] = 0;
			W_BinaryCommadSendDataArry[8] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_LOAD_DISPLAY_MODE) && (DataNum >= 1))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = W_Data1;
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_WINDOWS_LAYOUT) && (DataNum >= 4))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 5;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = gStp_ScreenRectInput->B_BoardSlotIndex;
			W_BinaryCommadSendDataArry[4] = gStp_ScreenRectInput->B_OutputScreenIndex;
			W_BinaryCommadSendDataArry[5] = gStp_ScreenRectInput->DisplayScreenWidth;
			W_BinaryCommadSendDataArry[6] = gStp_ScreenRectInput->DisplayScreenHeight;
			W_BinaryCommadSendDataArry[7] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
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
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 4; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADD_WINDOWS) && (DataNum >= 10))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 11;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = Stp_TempSignalSource->SignalSource;
			W_BinaryCommadSendDataArry[4] = Stp_TempSignalSource->Windows_Layer;
			W_BinaryCommadSendDataArry[5] = Stp_TempSignalWindows->SignalInputWindow.left;
			W_BinaryCommadSendDataArry[6] = Stp_TempSignalWindows->SignalInputWindow.top;
			W_BinaryCommadSendDataArry[7] = Stp_TempSignalWindows->SignalInputWindow.right;
			W_BinaryCommadSendDataArry[8] = Stp_TempSignalWindows->SignalInputWindow.bottom;
			W_BinaryCommadSendDataArry[9] = Stp_TempSignalWindows->SignalDisplayWindow.left;
			W_BinaryCommadSendDataArry[10] = Stp_TempSignalWindows->SignalDisplayWindow.top;
			W_BinaryCommadSendDataArry[11] = Stp_TempSignalWindows->SignalDisplayWindow.right;
			W_BinaryCommadSendDataArry[12] = Stp_TempSignalWindows->SignalDisplayWindow.bottom;
			W_BinaryCommadSendDataArry[13] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
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
				W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
				W_Data1 -= NVR_RELOAD_START;
				for(i = 0; i < 10; i++)
					ParamWrite(W_Data1 + i, pData[i]);
			}
			#endif
			return;
		}
		if((CommandData & 0xFF) == OPERATION_ADD_WINDOWS_END)
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
				memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
				W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
				W_BinaryCommadSendDataArry[1] = 1;
				W_BinaryCommadSendDataArry[2] = CommandData;
				W_BinaryCommadSendDataArry[3] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
				if(B_CommandSource != COMMAD_FROM_TCP)
					Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
				else
					TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				ParamWrite (NVR_DISPLAY_MODE_INDEX, g_DisplayModeIndex);
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + gStp_ScreenRectInput->B_OutputScreenIndex * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM);
				ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					W_Data1 -= NVR_RELOAD_START;
					ParamWrite(W_Data1, gW_DisplayScreenWindowsNum);
				}
//				gB_KeyboardData = 0;
				#endif
			}
			return;
		}
		if((CommandData & 0xFF) == OPERATION_SET_UPDATA_SYSTEM_DISPLAY)
		{
			InputFPGASetUpdata();
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 1;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_CLEAR_SCALER_USE) && (DataNum >= 1))
		{
			InputGraphicFPGAClearScalerUse(pData[0]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_CLEAR_OUTPUT_WINDOW) && (DataNum >= 2))
		{
			W_Data2 = pData[1];
			InputGraphicFPGAClearOutputWindow(pData[0], pData[1]);
			if(W_Data2 < OUTPUT_BOARD_MAX)
				gSt_ScreenRectInput[W_Data2].ActiveRectWindowsNum = 0;
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if(W_Data2 > 5)
					return;
				W_Data1 = NVR_Display0_WINDOWS_TOTAL_NUM + (W_Data2 * (NVR_Display1_WINDOWS_TOTAL_NUM - NVR_Display0_WINDOWS_TOTAL_NUM));
				ParamWrite(W_Data1, 0);
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					W_Data1 -= NVR_RELOAD_START;
					ParamWrite(W_Data1, 0);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
	}
	if((CommandData >> 8) == DEVICE_BOARD)
	{
		if(((CommandData & 0xFF) == OPERATION_PARAM_RESET) && (DataNum >= 1))
		{
			if(pData[0] == 1)
			{
				ParamWrite(NVR_CHECKSUM, 0);		// write checksum
				ParamInit();
				DevicesInit();
				LoadUserLastMode(0);
			}
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_PARAM_READ) && (DataNum >= 1))
		{
			W_Data1 = ParamRead(pData[0]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = W_Data1;
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_PARAM_WRITE) && (DataNum >= 2))
		{
			ParamWrite(pData[0], pData[1]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SELECT_SLOT) && (DataNum >= 1))
		{
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_DVI_HPD_SET) && (DataNum >= 2))
		{
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_CLK) && (DataNum >= 1))
		{
			/*
			SetBoardIDCLK(pData[0]);
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			sprintf(UserCommadSendStr, "BOARD SET_CLK %d\r\n", pData[0]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendBinaryData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#if(USER_COMMAD_DEBUG == 1)
				printf("%s", UserCommadSendStr);
			#endif
			*/
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_PROGRAM_EDID) && (DataNum > 1))
		{
			InputBoardADV7604ProgramEDID((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_PROGRAM_EDID %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_SELECT_INPUT_PORT) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputPort((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_INPUT_PORT %d %d\r\n", pData[0], pData[1]);
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 = NVR_ADV7604_DEV0_PORT + pData[0];
					W_Data1 -= NVR_RELOAD_START;
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					ParamWrite(W_Data1, pData[0]);
				}
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_SELECT_TIMINGS) && (DataNum > 1))
		{
			InputBoardADV7604SelectInputTiming((BYTE) (pData[0]), (BYTE) (pData[1]));
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SELECT_TIMINGS %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_LOG_STATUS) && (DataNum > 0))
		{
			InputBoardADV7604PrintLogStatus((BYTE) (pData[0]));
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_LOG_STATUS %d\r\n", pData[0]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_SET_CTRL) && (DataNum > 2))
		{
			InputBoardADV7604SetCtrl((BYTE) (pData[0]), pData[1], pData[2]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			#if(SAVE_MESSAGE_TO_EEPROM == 1)
				if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
				{
					W_Data1 = NVR_ADV7604_DEV0_BRIGHTNESS + (pData[0] * 4) + pData[1];
					W_Data1 -= NVR_RELOAD_START;
					W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
					ParamWrite(W_Data1, (BYTE) pData[2]);
				}
//				gB_KeyboardData = 0;
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_GET_CTRL) && (DataNum >= 2))
		{
			W_Data1 = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (pData[0] * 4) + pData[1]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = W_Data1;
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_GET_CTRL %d %d %d\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_ADV7604_SET_DV_CTRL) && (DataNum > 2))
		{
			InputBoardADV7604SetDVCtrl((BYTE) (pData[0]), pData[1], pData[2]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD ADV7604_SET_DV_CTRL %d %d %d\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_TVP5158_SET_CTRL) && (DataNum >= 4))
		{
			if(pData[2] == 0)
			{
				InputBoardTVP5158SetLuminanceBrightness(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_BRIGHTNESS + (pData[0] * (NVR_TVP5158_DEV1_PORT0_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_BRIGHTNESS - NVR_TVP5158_DEV0_PORT0_BRIGHTNESS));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 1)
			{
				InputBoardTVP5158SetLuminanceContrast(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_CONTRAST + (pData[0] * (NVR_TVP5158_DEV1_PORT0_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_CONTRAST - NVR_TVP5158_DEV0_PORT0_CONTRAST));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 2)
			{
				InputBoardTVP5158SetLuminanceSaturation(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_SATURATION + (pData[0] * (NVR_TVP5158_DEV1_PORT0_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_SATURATION - NVR_TVP5158_DEV0_PORT0_SATURATION));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			if(pData[2] == 3)
			{
				InputBoardTVP5158SetLuminanceHue(pData[0], pData[1], pData[3]);
				#if(SAVE_MESSAGE_TO_EEPROM == 1)
					W_Data1 = NVR_TVP5158_DEV0_PORT0_HUE + (pData[0] * (NVR_TVP5158_DEV1_PORT0_HUE - NVR_TVP5158_DEV0_PORT0_HUE)) + (pData[1] * (NVR_TVP5158_DEV0_PORT1_HUE - NVR_TVP5158_DEV0_PORT0_HUE));
					ParamWrite(W_Data1, (BYTE) pData[3]);
					if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
					{
						W_Data1 -= NVR_RELOAD_START;
						W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
						ParamWrite(W_Data1, (BYTE) pData[3]);
					}
				#endif
			}
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 5;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = pData[3];
			W_BinaryCommadSendDataArry[7] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_SET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], pData[3]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_TVP5158_GET_CTRL) && (DataNum >= 3))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 5;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = W_Data1;
			W_BinaryCommadSendDataArry[7] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD TVP5158_GET_CTRL %d %d %d %d\r\n", pData[0], pData[1], pData[2], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_GV7601_READ) && (DataNum > 0))
		{
			GV7601_GSPI_Init();
			GV7601_ReadWord(pData[0], pData[1], &W_Data1);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = W_Data1;
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d READ [0x%04X = 0x%04X]\r\n", pData[0], pData[1], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_GV7601_WRITE) && (DataNum > 0))
		{
			GV7601_GSPI_Init();
			GV7601_WriteWord(pData[0], pData[1], pData[2]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GV7601 %d WRITE [0x%04X = 0x%04X]\r\n", pData[0], pData[1], pData[2]);
			#endif
			return;
		}
		if((CommandData & 0xFF) == OPERATION_READ_MESSAGE)
		{
			#if 0
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
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			else
				TCP_SendBinaryData((BYTE*) UserCommadSendStr, strlen(UserCommadSendStr));
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_READ_INPUT_SOURCE_RESOLUTION) && (DataNum > 1))
		{
			GetInputBoardSignalResolution(pData[0], pData[1], &gW_ResolutionXStart, &gW_ResolutionYStart, &gW_ResolutionWidth, &gW_ResolutionHeight);
//			InputBoardGV7601PrintSignalStd(pData[0]);
//			InputBoardADV7604PrintLogStatus(pData[0]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 7;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = gW_ResolutionXStart;
			W_BinaryCommadSendDataArry[6] = gW_ResolutionYStart;
			W_BinaryCommadSendDataArry[7] = gW_ResolutionWidth;
			W_BinaryCommadSendDataArry[8] = gW_ResolutionHeight;
			W_BinaryCommadSendDataArry[9] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD READ_INPUT_SOURCE_RESOLUTION %d %d %d %d %d %d\r\n", pData[0], pData[1], gW_ResolutionXStart, gW_ResolutionYStart, gW_ResolutionWidth, gW_ResolutionHeight);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_INPUT_VIDEO_SWITCH) && (DataNum > 1))
		{
			if(gB_CVBSSwitchEnable == 1)
			{
				if(pData[0] <= 5)
				{
					W_Data1 = NVR_INPUT0_VIDEO_SWITCH + pData[0];
					#if(SAVE_MESSAGE_TO_EEPROM == 1)
						ParamWrite(W_Data1, (BYTE) pData[1]);
						if((gB_KeyDisplayMode > 0) && (gB_KeyDisplayMode < DISPLAY_MODE_INDEX_MAX))
						{
							W_Data1 -= NVR_RELOAD_START;
							W_Data1 += (MAX_NVR_PARAM + 1) + ((gB_KeyDisplayMode - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
							ParamWrite(W_Data1, pData[1]);
						}
					#endif
				}
				if(pData[0] == 0)
				{
					W_Data1 = InputFPGA_ReadWord(INPUT_FPGA_SYSTEM_CVBS_TYPE) & 0x0F;
					W_Data1 |= (pData[1] & 0x0F) << 8;
					InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_Data1);
					gB_CVBSSwitchIndex = pData[1];
				}
			}
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD SET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_GET_INPUT_VIDEO_SWITCH) && (DataNum >= 1))
		{
			if(pData[0] == 0)
			{
				W_Data1 = InputFPGA_ReadWord(INPUT_FPGA_SYSTEM_CVBS_TYPE);
				W_Data1 >>= 8;
				W_Data1 &= 0x0F;
			}
			else
				W_Data1 = 0xFF;
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = W_Data1;
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("BOARD GET_INPUT_VIDEO_SWITCH %d %d\r\n", pData[0], W_Data1);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_READ_SLOT_MESSAGE) && (DataNum > 0))
		{
			memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
			if(pData[0] < OUTPUT_BOARD_MAX)
			{
				B_TempData = GetBoardSlotType(pData[0]);
				memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
				W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
				W_BinaryCommadSendDataArry[1] = 3;
				W_BinaryCommadSendDataArry[2] = CommandData;
				W_BinaryCommadSendDataArry[3] = pData[0];
				W_BinaryCommadSendDataArry[4] = B_TempData;
				W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
				if(B_CommandSource != COMMAD_FROM_TCP)
					Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
				else
					TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			}
			else if(pData[0] == 0x0F)
			{
				for(i = 0; i < (OUTPUT_BOARD_MAX); i++)
				{
					memset(UserCommadSendStr, 0, sizeof(UserCommadSendStr));
					B_TempData = GetBoardSlotType(i);
					memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
					W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
					W_BinaryCommadSendDataArry[1] = 3;
					W_BinaryCommadSendDataArry[2] = CommandData;
					W_BinaryCommadSendDataArry[3] = i;
					W_BinaryCommadSendDataArry[4] = B_TempData;
					W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
					if(B_CommandSource == 0)
						Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
					else
						TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
				}
			}
			else
			{
				memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
				W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
				W_BinaryCommadSendDataArry[1] = 2;
				W_BinaryCommadSendDataArry[2] = CommandData;
				W_BinaryCommadSendDataArry[3] = pData[0];
				W_BinaryCommadSendDataArry[4] = 0xFF;
				W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
				if(B_CommandSource != COMMAD_FROM_TCP)
					Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
				else
					TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			}
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_OUTPUT_CLK) && (DataNum >= 2))
		{
			Set_SL38160_CLK(DEVICEID_SL38160_17, pData[0], pData[1]);		// AUDIO_CLK: pData[0], VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
			ParamWrite(NVR_CUSTOM_RES_CLK, pData[1]);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 3;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			#if(USER_COMMAD_DEBUG == 1)
				printf("INPUT_FPGA SET_OUTPUT_CLK %d %d\r\n", pData[0], pData[1]);
			#endif
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_CUSTOM_RESOLUTION) && (DataNum >= 7))
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
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 8;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = pData[0];
			W_BinaryCommadSendDataArry[4] = pData[1];
			W_BinaryCommadSendDataArry[5] = pData[2];
			W_BinaryCommadSendDataArry[6] = pData[3];
			W_BinaryCommadSendDataArry[7] = pData[4];
			W_BinaryCommadSendDataArry[8] = pData[5];
			W_BinaryCommadSendDataArry[9] = pData[6];
			W_BinaryCommadSendDataArry[10] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if((CommandData & 0xFF) == OPERATION_GET_OUTPUT_RESOLUTION)
		{
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 4;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = g_ScreenOutputWidth;
			W_BinaryCommadSendDataArry[4] = g_ScreenOutputHeight;
			W_BinaryCommadSendDataArry[5] = g_VFreq;
			W_BinaryCommadSendDataArry[6] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if(((CommandData & 0xFF) == OPERATION_SET_OUTPUT_TIMING) && (DataNum >= 1))
		{
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			if(B_TempData != ((unsigned char) pData[0]))
			{
				InputFPGAClearBackgroundWindow(0x0F);
				B_TempData = InputGraphicFPGAChangeOutputTiming((unsigned char) pData[0]);
				ParamWrite(NVR_SCREEN_TIMING, (unsigned char) pData[0]);
//				W_Data1 = ParamRead(NVR_SWITCH_INPUT_PORT) & 0xFFFF;
			}
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			if(B_TempData != 0xFF)
				W_BinaryCommadSendDataArry[3] = pData[0];
			else
				W_BinaryCommadSendDataArry[3] = 0xFF;
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
		if((CommandData & 0xFF) == OPERATION_GET_OUTPUT_TIMING)
		{
			B_TempData = (BYTE) (ParamRead(NVR_SCREEN_TIMING) & 0xFF);
			memset(W_BinaryCommadSendDataArry, 0, sizeof(W_BinaryCommadSendDataArry));
			W_BinaryCommadSendDataArry[0] = BINARY_COMMAD_TAG;
			W_BinaryCommadSendDataArry[1] = 2;
			W_BinaryCommadSendDataArry[2] = CommandData;
			W_BinaryCommadSendDataArry[3] = B_TempData;
			W_BinaryCommadSendDataArry[4] = CalculateCheckSum(W_BinaryCommadSendDataArry[1] + 1, &W_BinaryCommadSendDataArry[1]);
			if(B_CommandSource != COMMAD_FROM_TCP)
				Serial_SendBinaryData(B_CommandSource, (BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			else
				TCP_SendBinaryData((BYTE*) W_BinaryCommadSendDataArry, (W_BinaryCommadSendDataArry[1] + 3) * sizeof(unsigned short));
			return;
		}
	}
}

#if(SERIAL_COMMAD_TYPE == 1)
static void ParseSerialCommad(BYTE* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL;
	StrHead = (char*) pCommad;
	if(*StrHead != BRILL_VIEW_PACKET_START)
		return;
	StrHead = (char*) pCommad + 1;
	if(*StrHead != gB_SerialMachineAddress)
		return;
	StrHead = (char*) pCommad + 2;
	if(*StrHead == 0x01)
	{
		strcpy(DevStr, "INPUT_FPGA");
		strcpy(OperationStr, "SET_WINDOWS_LAYOUT");
		W_DataArry[0] = 1920;
		W_DataArry[1] = 1080;
		ExcuteUserCommad(1, DevStr, OperationStr, 2, W_DataArry);
		strcpy(DevStr, "INPUT_FPGA");
		strcpy(OperationStr, "ADD_WINDOWS");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		W_DataArry[1] = 1;
		W_DataArry[2] = 0;
		W_DataArry[3] = 0;
		W_DataArry[4] = 1920;
		W_DataArry[5] = 1080;
		ExcuteUserCommad(1, DevStr, OperationStr, 6, W_DataArry);
		strcpy(DevStr, "INPUT_FPGA");
		strcpy(OperationStr, "ADD_WINDOWS_END");
		W_DataArry[0] = 0;
		W_DataArry[1] = 0;
		ExcuteUserCommad(1, DevStr, OperationStr, 2, W_DataArry);
		return;
	}
	if(*StrHead == 0x02)
	{
		strcpy(DevStr, "ADV7604_DEV0");
		strcpy(OperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return;
	}
	if(*StrHead == 0x03)
	{
		strcpy(DevStr, "ADV7604_DEV1");
		strcpy(OperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_DataArry[0] = *pData0;
		ExcuteUserCommad(1, DevStr, OperationStr, 1, W_DataArry);
		return;
	}
}
#endif
#if(SERIAL_COMMAD_TYPE == 2)
static void ParseSerialCommad(BYTE* pCommad)
{
}
#endif
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
			if(DataCount >= (sizeof(W_DataArry) / sizeof(WORD)))
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
	ExcuteUserCommad(COMMAD_FROM_TCP, DevStr, OperationStr, DataCount, W_DataArry);
	#else
	ExcuteUserCommad(COMMAD_FROM_SERIAL1, DevStr, OperationStr, DataCount, W_DataArry);
	#endif
}
#endif

int ParseCommad(uint8_t B_CommandSource, uint8_t* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL, *pData1 = NULL;
	char DataFlag = 0;
	uint32_t i = 0, DataCount = 0, W_Data = 0;
	StrHead = (char*) pCommad;
	// Parse Device Name
	while((*StrHead == ' ') || (*StrHead == 0))
		StrHead++;
	i = 0;
	if((*StrHead == '\r') || (*StrHead == '\n'))
		return -1;
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
			#if(PARSE_COMMAD_DEBUG == 1)
			printf("\r\n1 Command Error.\r\n");
			#endif
			return -1;
		}
	}
	// Parse operation type
	while((*StrHead == ' ') || (*StrHead == 0))
		StrHead++;
	if((*StrHead == '\r') || (*StrHead == '\n'))
	{
		#if(PARSE_COMMAD_DEBUG == 1)
		printf("\r\n2 Command Error.\r\n");
		#endif
		return -1;
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
			#if(PARSE_COMMAD_DEBUG == 1)
			printf("\r\n3 Command Error.\r\n");
			#endif
			return -1;
		}
	}
	// Parse command data
	while((*StrHead == ' ') || (*StrHead == 0))
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
				#if(PARSE_COMMAD_DEBUG == 1)
				printf("\r\n4 Command Error.\r\n");
				#endif
				return -1;
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
						#if(PARSE_COMMAD_DEBUG == 1)
						printf("\r\n5 Command Error.\r\n");
						#endif
						return -1;
					}

				}
				else
				{
					W_Data *= 10;
					if((*StrHead) >= 0x30 && (*StrHead) <= 0x39)
						W_Data += (*StrHead) - 0x30;
					else
					{
						#if(PARSE_COMMAD_DEBUG == 1)
						printf("\r\n6 Command Error.\r\n");
						#endif
						return -1;
					}
				}
//				B_Data += *StrHead - 0x30;
				StrHead++;
			}
			W_DataArry[DataCount] = W_Data;
			DataCount++;
			if(DataCount >= (sizeof(W_DataArry) / sizeof(uint32_t)))
			{
				#if(PARSE_COMMAD_DEBUG == 1)
				printf("\r\n7 Command Error.\r\n");
				#endif
				return -1;
			}
			while((*StrHead == ' ') || (*StrHead == 0))
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

	#if 0
	printf("\r\n%s %s ", DevStr, OperationStr);
	for(i = 0; i < DataCount; i++)
		printf("%d ", W_DataArry[i]);
	printf("\r\nDataCount: %d\r\n", DataCount);
	#endif

	#ifdef USE_TCP_DEBUG_SERIAL
	ExcuteUserCommad(COMMAD_FROM_TCP, DevStr, OperationStr, DataCount, W_DataArry);
	#else
	ExcuteUserCommad(B_CommandSource, DevStr, OperationStr, DataCount, W_DataArry);
	#endif
	return 1;
}

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
			if(DataCount >= (sizeof(W_DataArry) / sizeof(WORD)))
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
	ExcuteUserCommad(COMMAD_FROM_TCP, DevStr, OperationStr, DataCount, W_DataArry);
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

#if 0
static BYTE TCP_SendData(BYTE *Bp_Buf, int W_Len)
{
	return (BYTE) OK;
}
#endif

static BYTE TCP_SendBinaryData(BYTE *Bp_Buf, int W_Len)
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
		case 'G':
			if((gB_TotalInputPort == 4) && (gB_TotalOutputPort == 2))
				B_Result = 1;
			gB_TotalInputPort = 4;
			gB_TotalOutputPort = 2;
			break;
		case 'B':
		case 'E':
		case 'H':
			if((gB_TotalInputPort == 3) && (gB_TotalOutputPort == 3))
				B_Result = 1;
			gB_TotalInputPort = 3;
			gB_TotalOutputPort = 3;
			break;
		case 'C':
		case 'F':
		case 'I':
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
	Stp_WindowLayoutAperture->InFirstPixel = ParamRead(W_Data + 5);							// < The first pixel index in the input video
	Stp_WindowLayoutAperture->InLastPixel = ParamRead(W_Data + 6);						// < The last pixel index in the input video
	Stp_WindowLayoutAperture->InFirstLine = ParamRead(W_Data + 7);							// < The first line index in the input video
	Stp_WindowLayoutAperture->InLastLine = ParamRead(W_Data + 8);						// < The last line index in the input video
	Stp_WindowLayoutAperture->OutHoriSize = ParamRead(W_Data + 9);						// < Horizontal size of the output video
	Stp_WindowLayoutAperture->OutVertSize = ParamRead(W_Data + 10);						// < Vertical size of the output video
}

void LoadUserLastMode(BYTE B_ModeIndex)
{
	WORD W_Data = 0, W_Data2 = 0, W_ModeBaseAdd = 0, W_BaseAdd = 0;
	BYTE i = 0, j = 0;
	ST_SignalWindows *Stp_TempSignalWindows = NULL;
	ST_SignalSource *Stp_TempSignalSource = NULL;
	ST_ScalerType *Stp_TempScaler = NULL;
	if(B_ModeIndex > 0)
	{
		W_ModeBaseAdd = (MAX_NVR_PARAM + 1) + ((B_ModeIndex - 1) * (MAX_NVR_PARAM - NVR_RELOAD_START));
		W_ModeBaseAdd -= NVR_RELOAD_START;
	}
	else
		W_ModeBaseAdd = 0;
//	printf("LoadUserLastMode Start: W_ModeBaseAdd = 0x%04X\r\n", W_ModeBaseAdd);
	W_Data = ParamRead(NVR_DISPLAY_MODE_INDEX);
	if(W_Data != B_ModeIndex)
		ParamWrite (NVR_DISPLAY_MODE_INDEX, B_ModeIndex);
//	InputFPGA_WriteWord(INPUT_FPGA_SCALER_MASK_ENABLE, 0x0F);
	InputFPGAClearBackgroundWindow(0x0F);
	// Switch CVBS Port
	if(gB_CVBSSwitchEnable == 1)
	{
		W_BaseAdd = W_ModeBaseAdd + NVR_INPUT0_VIDEO_SWITCH;
		W_Data = ParamRead(W_BaseAdd);
		W_Data2 = InputFPGA_ReadWord(INPUT_FPGA_SYSTEM_CVBS_TYPE) & 0x0F;
		W_Data2 |= (W_Data & 0x0F) << 8;
		InputFPGA_WriteWord(INPUT_FPGA_SYSTEM_CVBS_TYPE, W_Data2);
		gB_CVBSSwitchIndex = W_Data;
	}
	// Switch ADV7604 Input Port
	W_BaseAdd = W_ModeBaseAdd + NVR_ADV7604_DEV0_PORT;
	for(j = 0; j < INPUT_BOARD_MAX; j++)
	{
		W_Data = ParamRead(W_BaseAdd + j);
		InputBoardADV7604SelectInputPort(j, (BYTE) (W_Data));
	}
	
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
				W_DataArry[10] = ParamRead(W_Data + 10);
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
	for(i = 0; i < MaxOneInputBoardScalerNum; i++)
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
			if((W_DataArry[2] < INPUT_SIGNAL_PORT_MAX) && (gW_ResolutionWidth == 0) && (gW_ResolutionHeight == 0))
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

/********************************  END  *************************************/
