#ifndef __STP_74HC595_H__
#define __STP_74HC595_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      STP_74HC595.h                                                  */
/*																			*/
/* USAGE:       Header file for module STP_74HC595.c							*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/


/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void Init_74HC591(void);
extern void Set_74HC591(WORD W_Value);
extern void SelectDeviceI2CBus(BYTE B_BoardIndex, BYTE B_I2CBusIndex);
#endif
