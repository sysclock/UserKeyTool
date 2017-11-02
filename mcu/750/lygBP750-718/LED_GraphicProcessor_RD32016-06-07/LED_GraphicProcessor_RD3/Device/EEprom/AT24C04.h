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
#include "stm32f1xx_hal.h"
#include "global.h"

#define NVR_C04_DEV		0xA6			// Microchip AT24C04 EEPROM device address
#define NVR_C04_CACHE_LEN	8

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern uint8_t nvrC04_ReadByte	(uint32_t W_Addr);
extern uint8_t nvrC04_WriteByte	(uint32_t W_Addr, uint8_t B_Data);
extern uint32_t nvrC04_ReadWord	(uint32_t W_Addr);
extern uint8_t nvrC04_WriteWord	(uint32_t W_Addr, uint32_t W_Data);
extern uint8_t* nvrC04_ReadData	(uint32_t W_Addr, uint32_t W_Len);
extern uint8_t nvrC04_WriteData	(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len);

#endif
