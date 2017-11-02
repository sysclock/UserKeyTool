#ifndef __AT24C256_H__
#define __AT24C256_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C256.h                                                   */
/*																			*/
/* USAGE:       Header file for module 24AA02.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

#include "../../System Code/global.h"
#include "../../System Code/type.h"
/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/

#define NVR_C256_DEV		0xA0			// Microchip AT24C256 EEPROM device address
//#define NVR_C256_DEV		0xA2			// Microchip AT24C256 EEPROM device address
//#define NVR_C256_CACHE_LEN	128
#define NVR_C256_CACHE_LEN	32

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern BYTE nvrC256_ReadByte	(WORD W_Addr);
extern BYTE nvrC256_WriteByte	(WORD W_Addr, BYTE B_Data);
extern WORD nvrC256_ReadWord	(WORD W_Addr);
extern BYTE nvrC256_WriteWord	(WORD W_Addr, WORD W_Data);
extern BYTE* nvrC256_ReadData	(WORD W_Addr, WORD W_Len);
extern BYTE nvrC256_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);

#endif
