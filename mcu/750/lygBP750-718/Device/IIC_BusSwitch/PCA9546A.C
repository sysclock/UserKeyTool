#define __PCA9546A_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      PCA9546A.c                                                   	*/
/*																			*/
/* USAGE:       This module contains functions for serial-in, 								*/
/*							serial or parallel-out shift register with output latches.		*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <stdio.h>

#include "../../System Code/lpc17xx.h"
#include "../../System Code/ASCII.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "../../System Code/Timer.h"
#include "../../IIC Comm/D_IIC0.h"
#include "PCA9546A.h"

#define PCA9546A_DECICE_ADDRESS				0xE8

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N							*/
/****************************************************************************/
void PCA9546A_SelectBusPort(BYTE B_Value);
/****************************************************************************/
/*  S T A T I C    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

/****************************************************************************/
/*																																					*/
/* FUNCTION:	PCA9546A_SelectBusPort(BYTE B_Value)													*/
/*																																					*/
/* USAGE:     Switch IIC bus																								*/
/*																																					*/
/* INPUT:		B_Value		0 All disabled																				*/
/*	BIT0 = 1 -- enable I2C0																									*/
/*	BIT1 = 1 -- enable I2C1																									*/
/*	BIT2 = 1 -- enable I2C2																									*/
/*	BIT3 = 1 -- enable I2C3																									*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void PCA9546A_SelectBusPort(BYTE B_Value)
{
	I2C0WriteByte(PCA9546A_DECICE_ADDRESS, 0, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Value);
	Delay1ms(1);
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    PCA9546A_DummyCall(void)                                 */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void PCA9546A_DummyCall(void)
{
}
/********************************  END  *************************************/
