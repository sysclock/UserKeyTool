#ifndef __INIT_H__
#define __INIT_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      Init.h                                                      */
/*																			*/
/* USAGE:       Header file for module Init.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/
#include "../../System Code/global.h"
#include "../../System Code/type.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void ReadBoardIDInit(void);
extern BYTE ReadBoardID(BYTE BoardSlot);
extern void SelectBoardSlot(BYTE B_Slot);
extern void SetBoardIDCLK(BYTE B_Data);
extern void BoardIICBusSwitch(BYTE B_Slot);
extern void Board_CPU_SPI_BusSwitch(BYTE B_Slot);
extern void BoardDVIHPDSetSwitch(BYTE B_Slot, BYTE B_HPDStatus);
extern void BoardSlotReset(BYTE B_Slot);

#endif
