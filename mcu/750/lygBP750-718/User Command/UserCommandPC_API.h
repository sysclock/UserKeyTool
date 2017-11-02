
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.			*/
/*																			*/
/*=========================================================*/
/*																			*/
/* MODULE:      UserCommandPC_API.h                                              			*/
/*																			*/
/* USAGE:       Header file for module UserCommandPC_API.c							*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/


#ifndef _USERCOMMANDPC_API_H_
#define _USERCOMMANDPC_API_H_


#ifdef _USERCOMMANDPC_API_C
  #define _USERCOMMANDPC_APIDEF_
#else
  #define _USERCOMMANDPC_APIDEF_ extern
#endif


#define BOARD_ID 0	
#define ZERO	0


/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#include "../System Code/Serial.h"
#include "../Device/InputFPGA/InputFPGA.h"

#define USER_COMMAD_BUF_LENGTH     	128
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S											*/
/****************************************************************************/
extern BYTE gB_IICBusIndex, gB_SlotIndex, gB_InputFPGASetWindowsLayoutFlag;

extern WORD gW_DisplayScreenWindowsNum;
extern WORD W_DataArry[USER_COMMAD_BUF_LENGTH];
extern XScalerAperture St_UserCommadAperture;


extern unsigned char g_DisplayModeIndex;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void UserCommadTask(void);
extern void ParseTCPCommad(unsigned char* pCommad);
extern void ParseSerialCommad(unsigned char* pCommad);
extern unsigned char ParseCommadByBrillViewType1(unsigned char B_Data);
extern void ExcuteUserCommad(unsigned char B_CommandSource, char* pDevice, char* pOperation, WORD DataNum, WORD* pData);
extern void LoadUserLastMode(BYTE B_ModeIndex);
extern void ReadUserScalerParam(BYTE B_ScalerIndex, XScalerAperture *Stp_WindowLayoutAperture);
extern BYTE CheckEquipmentIntegrity(void);
extern BYTE LoadUserKeyMode(BYTE B_Data);
extern void LCD_Display(BYTE B_XStart, BYTE B_YStart, char* pStr);










void KeyPad_SetupWizard_to_FPGA(char iMode);
void KeyBoardCommandHandler(void);
void CMD_INPUTFPGA_CLEAR_SCALER_USE(void);
void CMD_INPUTFPGA_SCALER_SETUP(WORD* pData);
void CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(WORD outPort);
void CMD_INPUTFPGA_SET_WINDOW_LAYOUT(WORD* pData);
void CMD_INPUTFPGA_ADD_WINDOW(WORD* pData);
void CMD_INPUTFPGA_ADD_WINDOW_END(void);
void CMD_BOARD_RMG(void);
void CMD_BOARD_RISR(BYTE Slot_ID);


void INPUT_FPGA_SET_UPDATA_SYSTEM_DISPLAY(void);
void CMD_INPUTFPGA_SET_KEY_MODE(char iMode);

void Read_EEPROM_WizardPara(void);
void Write_EEPROM_WizardPara(void);

#endif
