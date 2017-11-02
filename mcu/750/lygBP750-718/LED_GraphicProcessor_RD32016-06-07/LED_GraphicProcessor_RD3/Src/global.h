#ifndef __GLOBAL_H__
#define __GLOBAL_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      Global.h                                                    */
/*																			*/
/* USAGE:       Header file for global definition.							*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	D E F I N I T I O N														*/
/****************************************************************************/
typedef unsigned char   BYTE;
typedef BYTE			BOOL;
typedef unsigned int    WORD;
typedef unsigned long   DWORD;
typedef char   			SBYTE;
typedef int    			SWORD;
typedef long   			SDWORD;

typedef unsigned char  bool_t;	/* this type can be used in structures */
#ifndef FALSE
    #define FALSE   0
    #define TRUE    1
#endif

#define OFF			0
#define ON			1

#define OK			0
#define ERR			0xFFFF
 
#define BIT0		0x01
#define BIT1		0x02
#define BIT2		0x04
#define BIT3		0x08
#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80
#define BIT8		0x0100
#define BIT9		0x0200
#define BIT10		0x0400
#define BIT11		0x0800
#define BIT12		0x1000
#define BIT13		0x2000
#define BIT14		0x4000
#define BIT15		0x8000

#define BIT16		0x010000
#define BIT17		0x020000
#define BIT18		0x040000
#define BIT19		0x080000
#define BIT20		0x100000
#define BIT21		0x200000
#define BIT22		0x400000
#define BIT23		0x800000
#define BIT24		0x01000000
#define BIT25		0x02000000
#define BIT26		0x04000000
#define BIT27		0x08000000
#define BIT28		0x10000000
#define BIT29		0x20000000
#define BIT30		0x40000000
#define BIT31		0x80000000

#define USE_SIMULATED_I2C

#define BINARY_COMMAD_TAG 													0xDEFE
#define BINARY_COMMAD_FLAG_REV_START 								0
#define BINARY_COMMAD_FLAG_REV_TAG 									1
#define BINARY_COMMAD_FLAG_REV_DATA_LEN 						2
#define BINARY_COMMAD_FLAG_REV_DATA_TEXT 						3
#define BINARY_COMMAD_FLAG_REV_DATA_CHECK_SUM 			4

#define	INPUT_BOARD_MAX								5
#define	OUTPUT_BOARD_MAX							4
#define	INPUT_SIGNAL_PORT_MAX					5
#define	INPUT_BOARD_SCALER_MAX				4
#define	BOARD_OUTPUT_PORT_MAX					4
/****************************************************************************/
/*	C O M M O N    E R R O R												*/
/****************************************************************************/
	#define Time_Wait() __nop();__nop(); __nop(); __nop(); //__nop();__nop(); __nop(); __nop();
	#define Time_LongWait() Time_Wait(); Time_Wait(); Time_Wait(); Time_Wait();

/****************************************************************************/
/*  M A C R O   D E F I N I T I O N											*/
/****************************************************************************/

#endif 
