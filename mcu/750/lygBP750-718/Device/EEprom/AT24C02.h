#ifndef __AT24C02_H__
#define __AT24C02_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C02.h                                                    */
/*																			*/
/* USAGE:       Header file for module 24AA02.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/

#define NVR_C02_CACHE_LEN	8			// Microchip AT24C02 EEPROM cache buffer length

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

BYTE nvrC02_ReadByte	(BYTE B_DeviceName, BYTE B_Addr);
BYTE nvrC02_WriteByte	(BYTE B_DeviceName, BYTE B_Addr, BYTE B_Data);
WORD nvrC02_ReadWord	(BYTE B_DeviceName, BYTE B_Addr);
BYTE nvrC02_WriteWord	(BYTE B_DeviceName, BYTE B_Addr, WORD W_Data);
BYTE* nvrC02_ReadData	(BYTE B_DeviceName, BYTE B_Addr, BYTE B_Len);
BYTE nvrC02_WriteData	(BYTE B_DeviceName, BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len);

#endif
