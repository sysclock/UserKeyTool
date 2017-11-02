#ifndef __M21121_H__
#define __M21121_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      M21121.h                                                   */
/*																			*/
/* USAGE:       Header file for module M21121.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
#define M21121_INPUT_PORT_MAX 33
#define M21121_OUTPUT_PORT_MAX 33

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern void M21121_Init(void);
extern BYTE M21121_SendData(BYTE B_Device, BYTE B_SubAddr, BYTE B_Data);
extern BYTE M21121_RcvData(BYTE B_Device, BYTE B_SubAddr);
extern void M21121_UpdataRegister(BYTE B_Device);
extern void M21121_SwitchPort(BYTE B_Device, WORD W_InputPort, WORD W_OutputPort);
extern void M21121_MuteInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);
extern void M21121_MuteOutputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);
extern void M21121_EqualizationInputPort(BYTE B_Device, WORD W_Port, BYTE B_Data);

#endif
