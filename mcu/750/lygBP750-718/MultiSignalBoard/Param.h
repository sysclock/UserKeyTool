#ifndef __PARAM_H__
#define __PARAM_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      Param.h														*/
/*																			*/
/* USAGE:       Header file for module Param.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

#include "../User/I2C.h"
#include "../System Code/global.h"
#include "../System Code/type.h"
//#include "..\Device\EEprom\AT24C256.h"		// EEprom driver (Microchip 24LC256B)

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/


/*
#define	MDEV_MASK	0xC000					// Memory device mask
											// 00xx xxxx xxxx xxxx	- SDRAM
											// 01xx xxxx xxxx xxxx	- GM60xx
											// 10xx xxxx xxxx xxxx	- EEPROM
#define MEM_SRAM	0x0000					// Parameter address bitwise-OR mask
											// for data stored directly in
											// on-chip memory
#define MEM_GMREG	0x4000					// Parameter address bitwise-OR mask
											// for data stored directly in
											// gm60xx register space
#define MEM_EEPROM	0x8000					// Parameter address bitwise-OR mask
											// for data stored in EEprom memory
											// space
*/
#define	MDEV_MASK	0x0000					// Memory device mask
											// 00xx xxxx xxxx xxxx	- SDRAM
											// 01xx xxxx xxxx xxxx	- GM60xx
											// 10xx xxxx xxxx xxxx	- EEPROM
#define MEM_EEPROM	0x0000					// Parameter address bitwise-OR mask
#define MEM_EEPROM_OFFSET	((E2ROM_Last_ADDR >> 1) << 1)					// Parameter address bitwise-OR mask
											// for data stored in EEprom memory
											// space

/****************************************************************************/
/*	M A C R O S																*/
/****************************************************************************/

#define	InGMREG(ADDR)		(((ADDR) & MDEV_MASK) == MEM_GMREG)
#define	InNVR(ADDR)			(((ADDR) & MDEV_MASK) == MEM_EEPROM)
#define	InSRAM(ADDR)		(((ADDR) & MDEV_MASK) == MEM_SRAM)

/****************************************************************************/
/*	P A R A M E T E R   M A P P I N G										*/
/****************************************************************************/

typedef	SWORD			PARAM;

//
// --- PARAMETERS IN NON-VOLATILE MEMORY ---
//
enum En_ParamAddr
{
//	NVR_MODE=MEM_EEPROM + MEM_EEPROM_OFFSET,									// mode
	NVR_MODE=MEM_EEPROM,									// mode
	NVR_CHECKSUM,													// checksum must be at beginning
	NVR_START,														// Start of EEprom parameters
	NVR_VER_MAJ = NVR_START,							// firmware version major
	NVR_VER_MIN,													// firmware version minor
	NVR_VER_REL,													// (release - RC#/0x80+REL#)

	NVR_RELOAD_START,                   	// Starting point for reload values

	NVR_SERIAL_ADDR = NVR_RELOAD_START,			// Serial Address
	NVR_ADV7604_DEV0_PORT,									// ADV7604Dev0 Select Port index
	NVR_ADV7604_DEV1_PORT,									// ADV7604Dev1 Select Port index
	NVR_ADV7604_DEV2_PORT,									// ADV7604Dev2 Select Port index
	NVR_ADV7604_DEV3_PORT,									// ADV7604Dev3 Select Port index
	NVR_ADV7604_DEV4_PORT,									// ADV7604Dev4 Select Port index
	NVR_ADV7604_DEV5_PORT,									// ADV7604Dev5 Select Port index
	NVR_ADV7604_DEV0_EDID,									// ADV7604Dev0 Select EDID index
	NVR_ADV7604_DEV1_EDID,									// ADV7604Dev1 Select EDID index
	NVR_ADV7604_DEV2_EDID,									// ADV7604Dev2 Select EDID index
	NVR_ADV7604_DEV3_EDID,									// ADV7604Dev3 Select EDID index
	NVR_ADV7604_DEV4_EDID,									// ADV7604Dev4 Select EDID index
	NVR_ADV7604_DEV5_EDID,									// ADV7604Dev5 Select EDID index
	NVR_ADV7604_DEV0_BRIGHTNESS,						// ADV7604DEV0 BRIGHTNESS
	NVR_ADV7604_DEV0_CONTRAST,							// ADV7604DEV0 CONTRAST
	NVR_ADV7604_DEV0_SATURATION,						// ADV7604DEV0 SATURATION
	NVR_ADV7604_DEV0_HUE,										// ADV7604DEV0 HUE
	NVR_ADV7604_DEV1_BRIGHTNESS,						// ADV7604DEV1 BRIGHTNESS
	NVR_ADV7604_DEV1_CONTRAST,  						// ADV7604DEV1 CONTRAST  
	NVR_ADV7604_DEV1_SATURATION,						// ADV7604DEV1 SATURATION
	NVR_ADV7604_DEV1_HUE,       						// ADV7604DEV1 HUE
	NVR_ADV7604_DEV2_BRIGHTNESS,						// ADV7604DEV2 BRIGHTNESS
	NVR_ADV7604_DEV2_CONTRAST,  						// ADV7604DEV2 CONTRAST  
	NVR_ADV7604_DEV2_SATURATION,						// ADV7604DEV2 SATURATION
	NVR_ADV7604_DEV2_HUE,       						// ADV7604DEV2 HUE
	NVR_ADV7604_DEV3_BRIGHTNESS,						// ADV7604DEV3 BRIGHTNESS
	NVR_ADV7604_DEV3_CONTRAST,  						// ADV7604DEV3 CONTRAST  
	NVR_ADV7604_DEV3_SATURATION,						// ADV7604DEV3 SATURATION
	NVR_ADV7604_DEV3_HUE,       						// ADV7604DEV3 HUE
	NVR_ADV7604_DEV4_BRIGHTNESS,						// ADV7604DEV4 BRIGHTNESS
	NVR_ADV7604_DEV4_CONTRAST,  						// ADV7604DEV4 CONTRAST  
	NVR_ADV7604_DEV4_SATURATION,						// ADV7604DEV4 SATURATION
	NVR_ADV7604_DEV4_HUE,       						// ADV7604DEV4 HUE
	NVR_ADV7604_DEV5_BRIGHTNESS,						// ADV7604DEV5 BRIGHTNESS
	NVR_ADV7604_DEV5_CONTRAST,  						// ADV7604DEV5 CONTRAST  
	NVR_ADV7604_DEV5_SATURATION,						// ADV7604DEV5 SATURATION
	NVR_ADV7604_DEV5_HUE,       						// ADV7604DEV5 HUE
	NVR_INPUT0_VIDEO_SWITCH,								// Input 0 video switch 
	NVR_INPUT1_VIDEO_SWITCH,								// Input 1 video switch 
	NVR_INPUT2_VIDEO_SWITCH,								// Input 2 video switch 
	NVR_INPUT3_VIDEO_SWITCH,								// Input 3 video switch 
	NVR_INPUT4_VIDEO_SWITCH,								// Input 4 video switch 
	NVR_INPUT5_VIDEO_SWITCH,								// Input 5 video switch 
	NVR_TVP5158_DEV0_PORT0_BRIGHTNESS,			// TVP5158_DEV0_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV0_PORT0_CONTRAST,				// TVP5158_DEV0_PORT0 CONTRAST
	NVR_TVP5158_DEV0_PORT0_SATURATION,			// TVP5158_DEV0_PORT0 SATURATION
	NVR_TVP5158_DEV0_PORT0_HUE,							// TVP5158_DEV0_PORT0 HUE
	NVR_TVP5158_DEV0_PORT1_BRIGHTNESS,			// TVP5158_DEV0_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV0_PORT1_CONTRAST,				// TVP5158_DEV0_PORT1 CONTRAST
	NVR_TVP5158_DEV0_PORT1_SATURATION,			// TVP5158_DEV0_PORT1 SATURATION
	NVR_TVP5158_DEV0_PORT1_HUE,							// TVP5158_DEV0_PORT1 HUE
	NVR_TVP5158_DEV0_PORT2_BRIGHTNESS,			// TVP5158_DEV0_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV0_PORT2_CONTRAST,				// TVP5158_DEV0_PORT2 CONTRAST
	NVR_TVP5158_DEV0_PORT2_SATURATION,			// TVP5158_DEV0_PORT2 SATURATION
	NVR_TVP5158_DEV0_PORT2_HUE,							// TVP5158_DEV0_PORT2 HUE
	NVR_TVP5158_DEV0_PORT3_BRIGHTNESS,			// TVP5158_DEV0_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV0_PORT3_CONTRAST,				// TVP5158_DEV0_PORT3 CONTRAST
	NVR_TVP5158_DEV0_PORT3_SATURATION,			// TVP5158_DEV0_PORT3 SATURATION
	NVR_TVP5158_DEV0_PORT3_HUE,							// TVP5158_DEV0_PORT3 HUE
	NVR_TVP5158_DEV1_PORT0_BRIGHTNESS,			// TVP5158_DEV1_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV1_PORT0_CONTRAST,				// TVP5158_DEV1_PORT0 CONTRAST
	NVR_TVP5158_DEV1_PORT0_SATURATION,			// TVP5158_DEV1_PORT0 SATURATION
	NVR_TVP5158_DEV1_PORT0_HUE,							// TVP5158_DEV1_PORT0 HUE
	NVR_TVP5158_DEV1_PORT1_BRIGHTNESS,			// TVP5158_DEV1_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV1_PORT1_CONTRAST,				// TVP5158_DEV1_PORT1 CONTRAST
	NVR_TVP5158_DEV1_PORT1_SATURATION,			// TVP5158_DEV1_PORT1 SATURATION
	NVR_TVP5158_DEV1_PORT1_HUE,							// TVP5158_DEV1_PORT1 HUE
	NVR_TVP5158_DEV1_PORT2_BRIGHTNESS,			// TVP5158_DEV1_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV1_PORT2_CONTRAST,				// TVP5158_DEV1_PORT2 CONTRAST
	NVR_TVP5158_DEV1_PORT2_SATURATION,			// TVP5158_DEV1_PORT2 SATURATION
	NVR_TVP5158_DEV1_PORT2_HUE,							// TVP5158_DEV1_PORT2 HUE
	NVR_TVP5158_DEV1_PORT3_BRIGHTNESS,			// TVP5158_DEV1_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV1_PORT3_CONTRAST,				// TVP5158_DEV1_PORT3 CONTRAST
	NVR_TVP5158_DEV1_PORT3_SATURATION,			// TVP5158_DEV1_PORT3 SATURATION
	NVR_TVP5158_DEV1_PORT3_HUE,							// TVP5158_DEV1_PORT3 HUE
	NVR_TVP5158_DEV2_PORT0_BRIGHTNESS,			// TVP5158_DEV2_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV2_PORT0_CONTRAST,				// TVP5158_DEV2_PORT0 CONTRAST
	NVR_TVP5158_DEV2_PORT0_SATURATION,			// TVP5158_DEV2_PORT0 SATURATION
	NVR_TVP5158_DEV2_PORT0_HUE,							// TVP5158_DEV2_PORT0 HUE
	NVR_TVP5158_DEV2_PORT1_BRIGHTNESS,			// TVP5158_DEV2_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV2_PORT1_CONTRAST,				// TVP5158_DEV2_PORT1 CONTRAST
	NVR_TVP5158_DEV2_PORT1_SATURATION,			// TVP5158_DEV2_PORT1 SATURATION
	NVR_TVP5158_DEV2_PORT1_HUE,							// TVP5158_DEV2_PORT1 HUE
	NVR_TVP5158_DEV2_PORT2_BRIGHTNESS,			// TVP5158_DEV2_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV2_PORT2_CONTRAST,				// TVP5158_DEV2_PORT2 CONTRAST
	NVR_TVP5158_DEV2_PORT2_SATURATION,			// TVP5158_DEV2_PORT2 SATURATION
	NVR_TVP5158_DEV2_PORT2_HUE,							// TVP5158_DEV2_PORT2 HUE
	NVR_TVP5158_DEV2_PORT3_BRIGHTNESS,			// TVP5158_DEV2_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV2_PORT3_CONTRAST,				// TVP5158_DEV2_PORT3 CONTRAST
	NVR_TVP5158_DEV2_PORT3_SATURATION,			// TVP5158_DEV2_PORT3 SATURATION
	NVR_TVP5158_DEV2_PORT3_HUE,							// TVP5158_DEV2_PORT3 HUE
	NVR_TVP5158_DEV3_PORT0_BRIGHTNESS,			// TVP5158_DEV3_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV3_PORT0_CONTRAST,				// TVP5158_DEV3_PORT0 CONTRAST  
	NVR_TVP5158_DEV3_PORT0_SATURATION,			// TVP5158_DEV3_PORT0 SATURATION
	NVR_TVP5158_DEV3_PORT0_HUE,							// TVP5158_DEV3_PORT0 HUE       
	NVR_TVP5158_DEV3_PORT1_BRIGHTNESS,			// TVP5158_DEV3_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV3_PORT1_CONTRAST,				// TVP5158_DEV3_PORT1 CONTRAST  
	NVR_TVP5158_DEV3_PORT1_SATURATION,			// TVP5158_DEV3_PORT1 SATURATION
	NVR_TVP5158_DEV3_PORT1_HUE,							// TVP5158_DEV3_PORT1 HUE       
	NVR_TVP5158_DEV3_PORT2_BRIGHTNESS,			// TVP5158_DEV3_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV3_PORT2_CONTRAST,				// TVP5158_DEV3_PORT2 CONTRAST  
	NVR_TVP5158_DEV3_PORT2_SATURATION,			// TVP5158_DEV3_PORT2 SATURATION
	NVR_TVP5158_DEV3_PORT2_HUE,							// TVP5158_DEV3_PORT2 HUE       
	NVR_TVP5158_DEV3_PORT3_BRIGHTNESS,			// TVP5158_DEV3_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV3_PORT3_CONTRAST,				// TVP5158_DEV3_PORT3 CONTRAST  
	NVR_TVP5158_DEV3_PORT3_SATURATION,			// TVP5158_DEV3_PORT3 SATURATION
	NVR_TVP5158_DEV3_PORT3_HUE,							// TVP5158_DEV3_PORT3 HUE
	NVR_TVP5158_DEV4_PORT0_BRIGHTNESS,			// TVP5158_DEV4_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV4_PORT0_CONTRAST,				// TVP5158_DEV4_PORT0 CONTRAST
	NVR_TVP5158_DEV4_PORT0_SATURATION,			// TVP5158_DEV4_PORT0 SATURATION
	NVR_TVP5158_DEV4_PORT0_HUE,							// TVP5158_DEV4_PORT0 HUE
	NVR_TVP5158_DEV4_PORT1_BRIGHTNESS,			// TVP5158_DEV4_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV4_PORT1_CONTRAST,				// TVP5158_DEV4_PORT1 CONTRAST
	NVR_TVP5158_DEV4_PORT1_SATURATION,			// TVP5158_DEV4_PORT1 SATURATION
	NVR_TVP5158_DEV4_PORT1_HUE,							// TVP5158_DEV4_PORT1 HUE
	NVR_TVP5158_DEV4_PORT2_BRIGHTNESS,			// TVP5158_DEV4_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV4_PORT2_CONTRAST,				// TVP5158_DEV4_PORT2 CONTRAST
	NVR_TVP5158_DEV4_PORT2_SATURATION,			// TVP5158_DEV4_PORT2 SATURATION
	NVR_TVP5158_DEV4_PORT2_HUE,							// TVP5158_DEV4_PORT2 HUE
	NVR_TVP5158_DEV4_PORT3_BRIGHTNESS,			// TVP5158_DEV4_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV4_PORT3_CONTRAST,				// TVP5158_DEV4_PORT3 CONTRAST
	NVR_TVP5158_DEV4_PORT3_SATURATION,			// TVP5158_DEV4_PORT3 SATURATION
	NVR_TVP5158_DEV4_PORT3_HUE,							// TVP5158_DEV4_PORT3 HUE
	NVR_TVP5158_DEV5_PORT0_BRIGHTNESS,			// TVP5158_DEV5_PORT0 BRIGHTNESS
	NVR_TVP5158_DEV5_PORT0_CONTRAST,				// TVP5158_DEV5_PORT0 CONTRAST  
	NVR_TVP5158_DEV5_PORT0_SATURATION,			// TVP5158_DEV5_PORT0 SATURATION
	NVR_TVP5158_DEV5_PORT0_HUE,							// TVP5158_DEV5_PORT0 HUE
	NVR_TVP5158_DEV5_PORT1_BRIGHTNESS,			// TVP5158_DEV5_PORT1 BRIGHTNESS
	NVR_TVP5158_DEV5_PORT1_CONTRAST,				// TVP5158_DEV5_PORT1 CONTRAST  
	NVR_TVP5158_DEV5_PORT1_SATURATION,			// TVP5158_DEV5_PORT1 SATURATION
	NVR_TVP5158_DEV5_PORT1_HUE,							// TVP5158_DEV5_PORT1 HUE
	NVR_TVP5158_DEV5_PORT2_BRIGHTNESS,			// TVP5158_DEV5_PORT2 BRIGHTNESS
	NVR_TVP5158_DEV5_PORT2_CONTRAST,				// TVP5158_DEV5_PORT2 CONTRAST  
	NVR_TVP5158_DEV5_PORT2_SATURATION,			// TVP5158_DEV5_PORT2 SATURATION
	NVR_TVP5158_DEV5_PORT2_HUE,							// TVP5158_DEV5_PORT2 HUE
	NVR_TVP5158_DEV5_PORT3_BRIGHTNESS,			// TVP5158_DEV5_PORT3 BRIGHTNESS
	NVR_TVP5158_DEV5_PORT3_CONTRAST,				// TVP5158_DEV5_PORT3 CONTRAST  
	NVR_TVP5158_DEV5_PORT3_SATURATION,			// TVP5158_DEV5_PORT3 SATURATION
	NVR_TVP5158_DEV5_PORT3_HUE,							// TVP5158_DEV5_PORT3 HUE
	NVR_SCREEN_TIMING,											// ScreenTiming(Display Resolution Index)
	NVR_SCREEN_WIDTH,												// ScreenWidth(Display Resolution)
	NVR_SCREEN_HEIGHT,											// ScreenHeight(Display Resolution)
	NVR_CUSTOM_RES_ENABLE,									// Screen Custom Resolution Enable
	NVR_CUSTOM_RES_CLK,											// Screen Custom Resolution Clock
	NVR_CUSTOM_VIDEO_MODE_INDEX,						// Screen Custom VModeIndex
	NVR_CUSTOM_RES_TOTAL_PIXEL,							// Screen Custom Resolution Total Pixel
	NVR_CUSTOM_RES_TOTAL_LINE,							// Screen Custom Resolution Total Line
	NVR_CUSTOM_RES_H_START,									// Screen Custom Resolution H Start
	NVR_CUSTOM_RES_V_START,									// Screen Custom Resolution V Start
	NVR_CUSTOM_RES_ACTIVE_PIXEL,						// Screen Custom Resolution Active Pixel
	NVR_CUSTOM_RES_ACTIVE_LINE,							// Screen Custom Resolution Active Line
	NVR_DISPLAY_MODE_INDEX,									// Display mode index
	NVR_SCALER_TOTAL_NUM,
	NVR_SCALER0_BOARD_INDEX,								// Scaler0 Param
	NVR_SCALER0_SCALER_INDEX,
	NVR_SCALER0_SOURCE_INDEX,
	NVR_SCALER0_IN_HORI_SIZE,
	NVR_SCALER0_IN_VERT_SIZE,
	NVR_SCALER0_IN_FIRST_PIXEL,
	NVR_SCALER0_IN_LAST_PIXEL,
	NVR_SCALER0_IN_FIRST_LINE,
	NVR_SCALER0_IN_LAST_LINE,
	NVR_SCALER0_OUT_HORI_SIZE,
	NVR_SCALER0_OUT_VERT_SIZE,
	NVR_SCALER1_BOARD_INDEX,								// SCALER1 PARAM
	NVR_SCALER1_SCALER_INDEX,
	NVR_SCALER1_SOURCE_INDEX,
	NVR_SCALER1_IN_HORI_SIZE,
	NVR_SCALER1_IN_VERT_SIZE,
	NVR_SCALER1_IN_FIRST_PIXEL,
	NVR_SCALER1_IN_LAST_PIXEL,
	NVR_SCALER1_IN_FIRST_LINE,
	NVR_SCALER1_IN_LAST_LINE,
	NVR_SCALER1_OUT_HORI_SIZE,
	NVR_SCALER1_OUT_VERT_SIZE,
	NVR_SCALER2_BOARD_INDEX,								// SCALER2 PARAM
	NVR_SCALER2_SCALER_INDEX,
	NVR_SCALER2_SOURCE_INDEX,
	NVR_SCALER2_IN_HORI_SIZE,
	NVR_SCALER2_IN_VERT_SIZE,
	NVR_SCALER2_IN_FIRST_PIXEL,
	NVR_SCALER2_IN_LAST_PIXEL,
	NVR_SCALER2_IN_FIRST_LINE,
	NVR_SCALER2_IN_LAST_LINE,
	NVR_SCALER2_OUT_HORI_SIZE,
	NVR_SCALER2_OUT_VERT_SIZE,
	NVR_SCALER3_BOARD_INDEX,								// SCALER3 PARAM
	NVR_SCALER3_SCALER_INDEX,
	NVR_SCALER3_SOURCE_INDEX,
	NVR_SCALER3_IN_HORI_SIZE,
	NVR_SCALER3_IN_VERT_SIZE,
	NVR_SCALER3_IN_FIRST_PIXEL,
	NVR_SCALER3_IN_LAST_PIXEL,
	NVR_SCALER3_IN_FIRST_LINE,
	NVR_SCALER3_IN_LAST_LINE,
	NVR_SCALER3_OUT_HORI_SIZE,
	NVR_SCALER3_OUT_VERT_SIZE,
	NVR_Display0_WINDOWS_TOTAL_NUM,
	NVR_Display0_WINDOWS_BOARD_INDEX,									// 
	NVR_Display0_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display0_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display0_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display0_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW0_IN_LEFT,											// 
	NVR_Display0_WINDOW0_IN_TOP,												// 
	NVR_Display0_WINDOW0_IN_RIGHT,											// 
	NVR_Display0_WINDOW0_IN_BOTTOM,										// 
	NVR_Display0_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW1_IN_LEFT,											// 
	NVR_Display0_WINDOW1_IN_TOP,												// 
	NVR_Display0_WINDOW1_IN_RIGHT,											// 
	NVR_Display0_WINDOW1_IN_BOTTOM,										// 
	NVR_Display0_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW2_IN_LEFT,											// 
	NVR_Display0_WINDOW2_IN_TOP,												// 
	NVR_Display0_WINDOW2_IN_RIGHT,											// 
	NVR_Display0_WINDOW2_IN_BOTTOM,										// 
	NVR_Display0_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW3_IN_LEFT,											// 
	NVR_Display0_WINDOW3_IN_TOP,												// 
	NVR_Display0_WINDOW3_IN_RIGHT,											// 
	NVR_Display0_WINDOW3_IN_BOTTOM,										// 
	NVR_Display0_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW4_IN_LEFT,											// 
	NVR_Display0_WINDOW4_IN_TOP,												// 
	NVR_Display0_WINDOW4_IN_RIGHT,											// 
	NVR_Display0_WINDOW4_IN_BOTTOM,										// 
	NVR_Display0_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW5_IN_LEFT,											// 
	NVR_Display0_WINDOW5_IN_TOP,												// 
	NVR_Display0_WINDOW5_IN_RIGHT,											// 
	NVR_Display0_WINDOW5_IN_BOTTOM,										// 
	NVR_Display0_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW6_IN_LEFT,											// 
	NVR_Display0_WINDOW6_IN_TOP,												// 
	NVR_Display0_WINDOW6_IN_RIGHT,											// 
	NVR_Display0_WINDOW6_IN_BOTTOM,										// 
	NVR_Display0_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display0_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display0_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display0_WINDOW7_IN_LEFT,											// 
	NVR_Display0_WINDOW7_IN_TOP,												// 
	NVR_Display0_WINDOW7_IN_RIGHT,											// 
	NVR_Display0_WINDOW7_IN_BOTTOM,										// 
	NVR_Display0_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display0_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display0_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display0_WINDOW7_DISPLAY_BOTTOM,								// 

	NVR_Display1_WINDOWS_TOTAL_NUM,
	NVR_Display1_WINDOWS_BOARD_INDEX,									// 
	NVR_Display1_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display1_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display1_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display1_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW0_IN_LEFT,											// 
	NVR_Display1_WINDOW0_IN_TOP,												// 
	NVR_Display1_WINDOW0_IN_RIGHT,											// 
	NVR_Display1_WINDOW0_IN_BOTTOM,										// 
	NVR_Display1_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW1_IN_LEFT,											// 
	NVR_Display1_WINDOW1_IN_TOP,												// 
	NVR_Display1_WINDOW1_IN_RIGHT,											// 
	NVR_Display1_WINDOW1_IN_BOTTOM,										// 
	NVR_Display1_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW2_IN_LEFT,											// 
	NVR_Display1_WINDOW2_IN_TOP,												// 
	NVR_Display1_WINDOW2_IN_RIGHT,											// 
	NVR_Display1_WINDOW2_IN_BOTTOM,										// 
	NVR_Display1_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW3_IN_LEFT,											// 
	NVR_Display1_WINDOW3_IN_TOP,												// 
	NVR_Display1_WINDOW3_IN_RIGHT,											// 
	NVR_Display1_WINDOW3_IN_BOTTOM,										// 
	NVR_Display1_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW4_IN_LEFT,											// 
	NVR_Display1_WINDOW4_IN_TOP,												// 
	NVR_Display1_WINDOW4_IN_RIGHT,											// 
	NVR_Display1_WINDOW4_IN_BOTTOM,										// 
	NVR_Display1_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW5_IN_LEFT,											// 
	NVR_Display1_WINDOW5_IN_TOP,												// 
	NVR_Display1_WINDOW5_IN_RIGHT,											// 
	NVR_Display1_WINDOW5_IN_BOTTOM,										// 
	NVR_Display1_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW6_IN_LEFT,											// 
	NVR_Display1_WINDOW6_IN_TOP,												// 
	NVR_Display1_WINDOW6_IN_RIGHT,											// 
	NVR_Display1_WINDOW6_IN_BOTTOM,										// 
	NVR_Display1_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display1_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display1_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display1_WINDOW7_IN_LEFT,											// 
	NVR_Display1_WINDOW7_IN_TOP,												// 
	NVR_Display1_WINDOW7_IN_RIGHT,											// 
	NVR_Display1_WINDOW7_IN_BOTTOM,										// 
	NVR_Display1_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display1_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display1_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display1_WINDOW7_DISPLAY_BOTTOM,								// 

	NVR_Display2_WINDOWS_TOTAL_NUM,
	NVR_Display2_WINDOWS_BOARD_INDEX,									// 
	NVR_Display2_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display2_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display2_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display2_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW0_IN_LEFT,											// 
	NVR_Display2_WINDOW0_IN_TOP,												// 
	NVR_Display2_WINDOW0_IN_RIGHT,											// 
	NVR_Display2_WINDOW0_IN_BOTTOM,										// 
	NVR_Display2_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW1_IN_LEFT,											// 
	NVR_Display2_WINDOW1_IN_TOP,												// 
	NVR_Display2_WINDOW1_IN_RIGHT,											// 
	NVR_Display2_WINDOW1_IN_BOTTOM,										// 
	NVR_Display2_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW2_IN_LEFT,											// 
	NVR_Display2_WINDOW2_IN_TOP,												// 
	NVR_Display2_WINDOW2_IN_RIGHT,											// 
	NVR_Display2_WINDOW2_IN_BOTTOM,										// 
	NVR_Display2_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW3_IN_LEFT,											// 
	NVR_Display2_WINDOW3_IN_TOP,												// 
	NVR_Display2_WINDOW3_IN_RIGHT,											// 
	NVR_Display2_WINDOW3_IN_BOTTOM,										// 
	NVR_Display2_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW4_IN_LEFT,											// 
	NVR_Display2_WINDOW4_IN_TOP,												// 
	NVR_Display2_WINDOW4_IN_RIGHT,											// 
	NVR_Display2_WINDOW4_IN_BOTTOM,										// 
	NVR_Display2_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW5_IN_LEFT,											// 
	NVR_Display2_WINDOW5_IN_TOP,												// 
	NVR_Display2_WINDOW5_IN_RIGHT,											// 
	NVR_Display2_WINDOW5_IN_BOTTOM,										// 
	NVR_Display2_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW6_IN_LEFT,											// 
	NVR_Display2_WINDOW6_IN_TOP,												// 
	NVR_Display2_WINDOW6_IN_RIGHT,											// 
	NVR_Display2_WINDOW6_IN_BOTTOM,										// 
	NVR_Display2_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display2_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display2_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display2_WINDOW7_IN_LEFT,											// 
	NVR_Display2_WINDOW7_IN_TOP,												// 
	NVR_Display2_WINDOW7_IN_RIGHT,											// 
	NVR_Display2_WINDOW7_IN_BOTTOM,										// 
	NVR_Display2_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display2_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display2_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display2_WINDOW7_DISPLAY_BOTTOM,								// 

	NVR_Display3_WINDOWS_TOTAL_NUM,
	NVR_Display3_WINDOWS_BOARD_INDEX,									// 
	NVR_Display3_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display3_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display3_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display3_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW0_IN_LEFT,											// 
	NVR_Display3_WINDOW0_IN_TOP,												// 
	NVR_Display3_WINDOW0_IN_RIGHT,											// 
	NVR_Display3_WINDOW0_IN_BOTTOM,										// 
	NVR_Display3_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW1_IN_LEFT,											// 
	NVR_Display3_WINDOW1_IN_TOP,												// 
	NVR_Display3_WINDOW1_IN_RIGHT,											// 
	NVR_Display3_WINDOW1_IN_BOTTOM,										// 
	NVR_Display3_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW2_IN_LEFT,											// 
	NVR_Display3_WINDOW2_IN_TOP,												// 
	NVR_Display3_WINDOW2_IN_RIGHT,											// 
	NVR_Display3_WINDOW2_IN_BOTTOM,										// 
	NVR_Display3_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW3_IN_LEFT,											// 
	NVR_Display3_WINDOW3_IN_TOP,												// 
	NVR_Display3_WINDOW3_IN_RIGHT,											// 
	NVR_Display3_WINDOW3_IN_BOTTOM,										// 
	NVR_Display3_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW4_IN_LEFT,											// 
	NVR_Display3_WINDOW4_IN_TOP,												// 
	NVR_Display3_WINDOW4_IN_RIGHT,											// 
	NVR_Display3_WINDOW4_IN_BOTTOM,										// 
	NVR_Display3_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW5_IN_LEFT,											// 
	NVR_Display3_WINDOW5_IN_TOP,												// 
	NVR_Display3_WINDOW5_IN_RIGHT,											// 
	NVR_Display3_WINDOW5_IN_BOTTOM,										// 
	NVR_Display3_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW6_IN_LEFT,											// 
	NVR_Display3_WINDOW6_IN_TOP,												// 
	NVR_Display3_WINDOW6_IN_RIGHT,											// 
	NVR_Display3_WINDOW6_IN_BOTTOM,										// 
	NVR_Display3_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display3_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display3_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display3_WINDOW7_IN_LEFT,											// 
	NVR_Display3_WINDOW7_IN_TOP,												// 
	NVR_Display3_WINDOW7_IN_RIGHT,											// 
	NVR_Display3_WINDOW7_IN_BOTTOM,										// 
	NVR_Display3_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display3_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display3_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display3_WINDOW7_DISPLAY_BOTTOM,								// 

	NVR_Display4_WINDOWS_TOTAL_NUM,
	NVR_Display4_WINDOWS_BOARD_INDEX,									// 
	NVR_Display4_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display4_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display4_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display4_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW0_IN_LEFT,											// 
	NVR_Display4_WINDOW0_IN_TOP,												// 
	NVR_Display4_WINDOW0_IN_RIGHT,											// 
	NVR_Display4_WINDOW0_IN_BOTTOM,										// 
	NVR_Display4_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW1_IN_LEFT,											// 
	NVR_Display4_WINDOW1_IN_TOP,												// 
	NVR_Display4_WINDOW1_IN_RIGHT,											// 
	NVR_Display4_WINDOW1_IN_BOTTOM,										// 
	NVR_Display4_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW2_IN_LEFT,											// 
	NVR_Display4_WINDOW2_IN_TOP,												// 
	NVR_Display4_WINDOW2_IN_RIGHT,											// 
	NVR_Display4_WINDOW2_IN_BOTTOM,										// 
	NVR_Display4_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW3_IN_LEFT,											// 
	NVR_Display4_WINDOW3_IN_TOP,												// 
	NVR_Display4_WINDOW3_IN_RIGHT,											// 
	NVR_Display4_WINDOW3_IN_BOTTOM,										// 
	NVR_Display4_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW4_IN_LEFT,											// 
	NVR_Display4_WINDOW4_IN_TOP,												// 
	NVR_Display4_WINDOW4_IN_RIGHT,											// 
	NVR_Display4_WINDOW4_IN_BOTTOM,										// 
	NVR_Display4_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW5_IN_LEFT,											// 
	NVR_Display4_WINDOW5_IN_TOP,												// 
	NVR_Display4_WINDOW5_IN_RIGHT,											// 
	NVR_Display4_WINDOW5_IN_BOTTOM,										// 
	NVR_Display4_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW6_IN_LEFT,											// 
	NVR_Display4_WINDOW6_IN_TOP,												// 
	NVR_Display4_WINDOW6_IN_RIGHT,											// 
	NVR_Display4_WINDOW6_IN_BOTTOM,										// 
	NVR_Display4_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display4_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display4_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display4_WINDOW7_IN_LEFT,											// 
	NVR_Display4_WINDOW7_IN_TOP,												// 
	NVR_Display4_WINDOW7_IN_RIGHT,											// 
	NVR_Display4_WINDOW7_IN_BOTTOM,										// 
	NVR_Display4_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display4_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display4_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display4_WINDOW7_DISPLAY_BOTTOM,								// 

	NVR_Display5_WINDOWS_TOTAL_NUM,
	NVR_Display5_WINDOWS_BOARD_INDEX,									// 
	NVR_Display5_WINDOWS_BACKGROUND_INDEX,							// 
	NVR_Display5_WINDOWS_SCREEN_WIDTH,									// 
	NVR_Display5_WINDOWS_SCREEN_HEIGHT,								// 
	NVR_Display5_WINDOW0_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW0_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW0_IN_LEFT,											// 
	NVR_Display5_WINDOW0_IN_TOP,												// 
	NVR_Display5_WINDOW0_IN_RIGHT,											// 
	NVR_Display5_WINDOW0_IN_BOTTOM,										// 
	NVR_Display5_WINDOW0_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW0_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW0_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW0_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW1_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW1_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW1_IN_LEFT,											// 
	NVR_Display5_WINDOW1_IN_TOP,												// 
	NVR_Display5_WINDOW1_IN_RIGHT,											// 
	NVR_Display5_WINDOW1_IN_BOTTOM,										// 
	NVR_Display5_WINDOW1_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW1_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW1_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW1_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW2_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW2_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW2_IN_LEFT,											// 
	NVR_Display5_WINDOW2_IN_TOP,												// 
	NVR_Display5_WINDOW2_IN_RIGHT,											// 
	NVR_Display5_WINDOW2_IN_BOTTOM,										// 
	NVR_Display5_WINDOW2_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW2_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW2_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW2_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW3_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW3_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW3_IN_LEFT,											// 
	NVR_Display5_WINDOW3_IN_TOP,												// 
	NVR_Display5_WINDOW3_IN_RIGHT,											// 
	NVR_Display5_WINDOW3_IN_BOTTOM,										// 
	NVR_Display5_WINDOW3_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW3_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW3_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW3_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW4_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW4_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW4_IN_LEFT,											// 
	NVR_Display5_WINDOW4_IN_TOP,												// 
	NVR_Display5_WINDOW4_IN_RIGHT,											// 
	NVR_Display5_WINDOW4_IN_BOTTOM,										// 
	NVR_Display5_WINDOW4_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW4_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW4_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW4_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW5_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW5_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW5_IN_LEFT,											// 
	NVR_Display5_WINDOW5_IN_TOP,												// 
	NVR_Display5_WINDOW5_IN_RIGHT,											// 
	NVR_Display5_WINDOW5_IN_BOTTOM,										// 
	NVR_Display5_WINDOW5_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW5_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW5_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW5_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW6_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW6_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW6_IN_LEFT,											// 
	NVR_Display5_WINDOW6_IN_TOP,												// 
	NVR_Display5_WINDOW6_IN_RIGHT,											// 
	NVR_Display5_WINDOW6_IN_BOTTOM,										// 
	NVR_Display5_WINDOW6_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW6_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW6_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW6_DISPLAY_BOTTOM,								// 
	NVR_Display5_WINDOW7_SIGNAL_SOURCE,								// 
	NVR_Display5_WINDOW7_WINDOWS_LAYER,								// 
	NVR_Display5_WINDOW7_IN_LEFT,											// 
	NVR_Display5_WINDOW7_IN_TOP,												// 
	NVR_Display5_WINDOW7_IN_RIGHT,											// 
	NVR_Display5_WINDOW7_IN_BOTTOM,										// 
	NVR_Display5_WINDOW7_DISPLAY_LEFT,									// 
	NVR_Display5_WINDOW7_DISPLAY_TOP,									// 
	NVR_Display5_WINDOW7_DISPLAY_RIGHT,								// 
	NVR_Display5_WINDOW7_DISPLAY_BOTTOM,								// 
	MAX_NVR_PARAM
};

/****************************************************************************/
/*	P A R A M E T E R   O P T I O N S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

extern void		ParamInit		(void);
extern PARAM	ParamRead		(WORD W_ParamAddr);
extern void		ParamWrite		(WORD W_ParamAddr, PARAM W_Data);
extern void		ParamReset		(BYTE B_ResetAll);
extern PARAM	ParamDefault	(WORD W_ParamAddr);
extern WORD	ParamChecksum		(void);
#endif
