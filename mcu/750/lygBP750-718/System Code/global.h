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

//typedef bit				BOOL;
typedef unsigned char   BYTE;
typedef BYTE			BOOL;
typedef unsigned int    WORD;
typedef unsigned long   DWORD;
typedef char   			SBYTE;
typedef int    			SWORD;
typedef long   			SDWORD;
/*
typedef enum
{
	false   = 0,
	true    = 1
} bool_t;
*/
typedef unsigned char  bool_t;	/* this type can be used in structures */

typedef struct			// Structure for WORD		
{
	BYTE B_Upper;
	BYTE B_Lower;
} ST_WORD;

typedef union			// Union for WORD with upper and lower
{
	WORD    W_Data;
	ST_WORD	St_Data;
} UNION_WORD;

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

// Define NULL
#ifndef NULL
	#define NULL ((void*) 0)
#endif

#define SIO_RxBufLen 256
#define	DEBUG_OUT_UART_NUM		0
#define	GP_ALL_INPUT_RD				2

#define	BOARD_SLOT_SELECT_DEF						0xFFF8
#define	GRAPHIC_PROCESSOR_RD1		// 3U, 4 input board, 4 output board
#ifdef GRAPHIC_PROCESSOR_RD1
	#define	INPUT_BOARD_MAX				4
	#define	OUTPUT_BOARD_MAX			6
#else
	#define	INPUT_BOARD_MAX				8
	#define	OUTPUT_BOARD_MAX			8
#endif
//#define FCCLK									100000000
//#define FPCLK                 (FCCLK / 4)     //  外设时钟频率,FCCLK的1/2、1/4

/****************************************************************************/
/*	C O M M O N    E R R O R												*/
/****************************************************************************/

/*
enum
{                                   // Error status
    ERR_SEND  = 0xF9,               // Error in sending data
    ERR_RCV   = 0xFA,               // Error in receiving data
    ERR_TMO   = 0xFB,               // Timeout error
    ERR_PARAM = 0xFC,               // Error in input parameters
    ERR_READ  = 0xFD,               // Error in reading from EEPROM
    ERR_WRITE = 0xFE,	            // Error in writing to EEPROM
    ERR_BUSY  = 0xFF                // Device busy
};
*/
#define I2C_ERROR 1                           // i2c_state holds I2C_ERROR or I2C_OK after reading
#define I2C_OK 2                              // or writing a byte to indicate the result of the operation
#define I2C_BUSY 0                            // i2c_state holds I2C_BUSY during a read or write operation
#define I2C_DEV_MEMORY_ADDRESS_8BIT		0
#define I2C_DEV_MEMORY_ADDRESS_16BIT	1

#define I2CONSET_I2EN       (0x1<<6)  // I2C Control Set Register
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  // I2C Control clear Register
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2SCLH_SCLH					125  				// I2C SCL Duty Cycle High Reg
#define I2SCLL_SCLL					125  				// I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH			62  				// Fast Plus I2C SCL Duty Cycle High Reg
#define I2SCLL_HS_SCLL			63					// Fast Plus I2C SCL Duty Cycle Low Reg

#define KEIL
#ifdef KEIL

//
//	The video decoder needs 20ns delay between clearing or setting 
//	SCL and SDA. Since the processor clock speed is 100 Mhz and a NOP
//	will take 1 cycle to execute, we will need 4 NOP calls (~ 40ns).
//

//
// _nop()_ is the Keil equivalent call to NOP in assembly
//
	#define Time_Wait() __nop();__nop(); __nop(); __nop(); //__nop();__nop(); __nop(); __nop();
	#define Time_LongWait() Time_Wait(); Time_Wait(); Time_Wait(); Time_Wait();
#else			// Non-Keil compiler implementation
    void Time_Wait(void)
    {
		// noop equivalent should be called here, or implement as above with
		// 3 NOP equivalent calls. A 3 execution cycle delay is required.
    }
#endif

enum i2c_states
{
	LOWER_ADDRESS,
	DATA_BYTE,
	FINISHED,
	DUMMY_WRITE
}; // states during an I2C operation

enum i2c_operations
{
	READ,
	WRITE
};          // possible I2C operations are reading or writing

/****************************************************************************/
/*  M A C R O   D E F I N I T I O N											*/
/****************************************************************************/
typedef enum
{
	gmd_MODE_GRAPHICS 		= 0,			// Mode is Graphics
	gmd_MODE_SDVIDEO 			= 1,			// Mode is SD Video
	gmd_MODE_HDVIDEO   		= 2,			// Mode is HD Video
	gmd_MODE_EDVIDEO  		= 3,			// Mode is Extended Definition Video (480p/576p/etc)
	gmd_MODE_INTERLACED 	= BIT2,			// Mode is Interlaced
	gmd_MODE_NEG_HSYNC 		= BIT3,			// Mode HSync is Negative
	gmd_MODE_NEG_VSYNC 		= BIT4,			// Mode VSync is Negative
	gmd_MODE_SOG 						= BIT5,			// Mode is SOG or Embedded sync
	gmd_MODE_CSYNC 				= BIT6,			// Mode is Composite Sync
	gmd_MODE_OVERLAPPED720 	= BIT7,			// Non-resolved 720 Mode
	gmd_MODE_OVERLAPPED640		= BIT8,			// Non-resolved 640 Mode
	gmd_MODE_ESTIMATED 		= BIT9,			// Mode is Estimated
	gmd_MODE_OUT_RANGE 		= BIT10,		// Signal is Out of Range
	gmd_MODE_SUB_SAMPLE 	= BIT11,		// Sub-sample input mode
	gmd_MODE_UNKNOWN     = BIT12,		// Mode not found in Rom table
	gmd_MODE_VALID					= BIT13,		// Mode is Stable and Valid
	gmd_MODE_COMPLETE			= BIT14,		// Mode has been configured
	gmd_MODE_EMBEDDED_VIDEO	= BIT15		// Mode is a Video Image in a Graphic signal
} gmt_MODE_FLAGS;
/*
//
// General bit access macro
//
#define SetBit(VAR,BIT)         ( VAR |= BIT )
#define ClrBit(VAR,BIT)         ( VAR &= ~BIT )
#define CheckBit(VAR,BIT)       ( VAR | BIT )
#define RetBit(VAR,BIT)         (( VAR & BIT ) != 0 )
//
// Format table header for register loading
// 1st & 2nd bytes = Regitser start address
// 3rd & 4th bytes = Number of register data to load
//
#define TableHeader(START,END)	START>>8,START,(END-START+1)>>8,(END-START+1)
*/

#endif 
