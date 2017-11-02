#ifndef __M25P80_H__
#define __M25P80_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      M25P80.h                                                   */
/*																			*/
/* USAGE:       Header file for module M25P80.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
#define M25P80_SPI_CS        (1 << 21)                                    // P1.21 口为M25P80的片选
																																					// 该脚为低时，M25P80被使能

//#define SFM_M25P80_DEV		0xA0			// Microchip AT24C256 EEPROM device address
#define SFM_M25P80_CACHE_LEN	256
#define SFM_M25P80_PAGE_SIZE	0x10000
#define SFM_M25P80_BUSY_TIME	10			// Wait Time for M25P80 When It Is Busy (100ms one SFM_M25P80_BUSY_TIME).

// Instruction set
#define SFM_M25P80_WREN						0x06
#define SFM_M25P80_WRDI						0x04
#define SFM_M25P80_RDID						0x9F
#define SFM_M25P80_RDSR						0x05
#define SFM_M25P80_WRSR						0x01
#define SFM_M25P80_READ						0x03
#define SFM_M25P80_FAST_READ			0x0B
#define SFM_M25P80_PP							0x02
#define SFM_M25P80_SE							0xD8
#define SFM_M25P80_BE							0xC7
#define SFM_M25P80_DP							0xB9
#define SFM_M25P80_RES						0xAB

// Macro defines for M25P80 Status Register
#define SFM_M25P80_WIP						(1 << 0)
#define SFM_M25P80_WEL						(1 << 1)
#define SFM_M25P80_SRWD						(1 << 7)
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void SFM_M25P80Init(void);
extern void SFM_WriteEnable	(void);
extern void SFM_WriteDisable	(void);
extern BYTE* SFM_ReadIdentification	(void);
extern BYTE SFM_ReadStatusRegister	(void);
extern void SFM_WriteStatusRegister	(BYTE B_Data);
extern BYTE* SFM_ReadData	(WORD W_Addr, WORD W_Len);
extern BYTE* SFM_ReadDataFast	(WORD W_Addr, WORD W_Len);
extern BYTE SFM_PageWriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
extern BYTE SFM_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
extern BYTE SFM_SectorErase	(WORD W_Addr);
extern void SFM_BulkErase	(void);
extern void SFM_DeepPower_down	(void);
extern void SFM_ReleaseFromDeepPower_down	(void);

#endif
