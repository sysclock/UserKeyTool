#ifndef __USER_COMMAND_PC_API_H__
#define __USER_COMMAND_PC_API_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      UserCommandPC_API.h                                              */
/*																			*/
/* USAGE:       Header file for module UserCommandPC_API.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#include "Serial.h"
#include "../Device/InputFPGA/InputFPGA.h"

#define COMMAD_FROM_SERIAL						0
#define COMMAD_FROM_TCP								1
#define COMMAD_FROM_SERIAL1						0
#define COMMAD_FROM_SERIAL4						4
#define COMMAD_FROM_SERIAL5						5

#define BINARY_COMMAD_TAG 													0xDEFE
#define BINARY_COMMAD_FLAG_REV_START 								0
#define BINARY_COMMAD_FLAG_REV_TAG 									1
#define BINARY_COMMAD_FLAG_REV_DATA_LEN 						2
#define BINARY_COMMAD_FLAG_REV_DATA_TEXT 						3
#define BINARY_COMMAD_FLAG_REV_DATA_CHECK_SUM 			4
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

extern unsigned char g_DisplayModeIndex;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void UserCommadTask(void);
extern void ParseTCPCommad(unsigned char* pCommad);
extern void ParseSerialCommad(unsigned char* pCommad);
extern int ParseCommad(uint8_t B_CommandSource, uint8_t* pCommad);
extern unsigned char ParseCommadByBrillViewType1(unsigned char B_Data);
extern void ExcuteUserCommad(unsigned char B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData);
void ExcuteUserBinaryCommad(BYTE B_CommandSource, unsigned short CommandData, WORD DataNum, unsigned short* pData);
extern void LoadUserLastMode(BYTE B_ModeIndex);
extern void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture);
extern BYTE CheckEquipmentIntegrity(void);
extern BYTE LoadUserKeyMode(BYTE B_Data);
extern void LCD_Display(BYTE B_XStart, BYTE B_YStart, char* pStr);
#endif
