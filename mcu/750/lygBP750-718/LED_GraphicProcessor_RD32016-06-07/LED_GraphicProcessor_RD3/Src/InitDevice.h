#ifndef __INIT_DEVICE_H__
#define __INIT_DEVICE_H__
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

#include "stm32f1xx_hal.h"
#include "global.h"
#include "Timer.h"
#include "Serial.h"
#include "D_IIC.H"
#include "D_IIC1.H"
#include "D_IIC2.H"
#include "D_IIC3.H"
//#include "../User Command/UserCommandPC_API.h"

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
//extern unsigned char	gB_BoardType;
//extern volatile unsigned char gB_EXTI0_SFP_LOS_Flag;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void McuInit	(void);
extern void DevicesInit(void);

#endif
