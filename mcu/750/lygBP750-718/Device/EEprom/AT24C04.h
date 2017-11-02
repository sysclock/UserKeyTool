#ifndef __AT24C04_H__
#define __AT24C04_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C04.h                                                   */
/*																			*/
/* USAGE:       Header file for module AT24C04.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
#include "../../System Code/global.h"
#include "../../System Code/type.h"

#define NVR_C04_DEV		0xA0			// Microchip AT24C04 EEPROM device address
#define NVR_C04_CACHE_LEN	8

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern BYTE nvrC04_ReadByte	(WORD W_Addr);
extern BYTE nvrC04_WriteByte	(WORD W_Addr, BYTE B_Data);
extern WORD nvrC04_ReadWord	(WORD W_Addr);
extern BYTE nvrC04_WriteWord	(WORD W_Addr, WORD W_Data);
extern BYTE* nvrC04_ReadData	(WORD W_Addr, WORD W_Len);
extern BYTE nvrC04_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);

#endif
