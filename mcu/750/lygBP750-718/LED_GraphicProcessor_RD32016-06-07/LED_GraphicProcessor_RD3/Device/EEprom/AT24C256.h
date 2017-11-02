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
#include "stm32f1xx_hal.h"
#include "global.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/

#define NVR_C256_DEV		0xA6			// Microchip AT24C256 EEPROM device address
//#define NVR_C256_DEV		0xA2			// Microchip AT24C256 EEPROM device address
//#define NVR_C256_CACHE_LEN	128
#define NVR_C256_CACHE_LEN	32

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern uint8_t nvrC256_ReadByte	(uint32_t W_Addr);
extern uint8_t nvrC256_WriteByte	(uint32_t W_Addr, uint8_t B_Data);
extern uint32_t nvrC256_ReadWord	(uint32_t W_Addr);
extern uint8_t nvrC256_WriteWord	(uint32_t W_Addr, uint32_t W_Data);
extern uint8_t* nvrC256_ReadData	(uint32_t W_Addr, uint32_t W_Len);
extern uint8_t nvrC256_WriteData	(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len);

#endif
