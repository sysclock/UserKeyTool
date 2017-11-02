#define __PARAM_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      Param.c                                                     */
/*																			*/
/* USAGE:       This module controls the system paramters that are stored	*/
/*              in EEprom device.                                     		*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <stdio.h>

#include "type.h"
#include "Serial.h"
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "lwip.h"
#include "tcp_echoserver.h"
#include "../Device/EEprom/AT24C256.h"
#include "../Device/EEprom/AT24C04.h"
#include "../User Command/UserCommandPC_API.h"
#include "param.h"

//#define EEPROM_AT24C04
#define EEPROM_AT24C256
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N S										*/
/****************************************************************************/
void	ParamInit		(void);
PARAM	ParamRead		(WORD W_ParamAddr);
void	ParamWrite		(WORD W_ParamAddr, PARAM W_Data);
void	ParamReset		(BYTE B_ResetAll);
PARAM	ParamDefault	(WORD W_ParamAddr);
WORD	ParamChecksum		(void);

void	BoardParamTCPInit (void);
/****************************************************************************/
/*	L O C A L    D E F I N I T I O N S										*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   C O N S T A N T 											*/
/****************************************************************************/

//
// NOTE1: defaults MUST follow exact order as specified in enumeration
// En_ParamAddr in Param.h
//
static const WORD Wa_NVRParamDefaults [MAX_NVR_PARAM] = {
	//
	// --- PARAMETERS IN NON-VOLATILE MEMORY ---
	//
	0,						// NVR_Mode
	0,						// NVR_CHECKSUM				(init. param checksum)
	1,			 			// NVR_VER_MAJ				(version major)
	0, 						// NVR_VER_MIN				(version minor)
	2,						// NVR_VER_REL				(release - RC#/REL#)

	0x01,													// NVR_SERIAL_ADDR
	192,													// IP Address MYIP_1
	168,													// IP Address MYIP_2
	1,														// IP Address MYIP_3
	100,													// IP Address MYIP_4
	255,													// IP Address SUBMASK_1
	255,													// IP Address SUBMASK_2
	255,													// IP Address SUBMASK_3
	0,														// IP Address SUBMASK_4
	192,													// IP Address Standard Gateway GWIP_1
	168,													// IP Address Standard Gateway GWIP_2
	0,														// IP Address Standard Gateway GWIP_3
	254,													// IP Address Standard Gateway GWIP_4
	8978,													// TCP port we want to listen to
	2,														// ADV7604Dev0 Select Port index
	2,														// ADV7604Dev1 Select Port index
	2,														// ADV7604Dev2 Select Port index
	2,														// ADV7604Dev3 Select Port index
	2,														// ADV7604Dev4 Select Port index
	2,														// ADV7604Dev5 Select Port index
	0,														// NVR_ADV7604_DEV0_EDID ADV7604Dev0 Select EDID index
	0,														// NVR_ADV7604_DEV1_EDID ADV7604Dev1 Select EDID index
	0,														// NVR_ADV7604_DEV2_EDID ADV7604Dev2 Select EDID index
	0,														// NVR_ADV7604_DEV3_EDID ADV7604Dev3 Select EDID index
	0,														// NVR_ADV7604_DEV4_EDID ADV7604Dev4 Select EDID index
	0,														// NVR_ADV7604_DEV5_EDID ADV7604Dev5 Select EDID index
	0,														// ADV7604DEV0 BRIGHTNESS
	128,													// ADV7604DEV0 CONTRAST
	128,													// ADV7604DEV0 SATURATION
	0,														// ADV7604DEV0 HUE
	0,														// ADV7604DEV1 BRIGHTNESS
	128,  												// ADV7604DEV1 CONTRAST  
	128,													// ADV7604DEV1 SATURATION
	0,       											// ADV7604DEV1 HUE
	0,														// ADV7604DEV2 BRIGHTNESS
	128,  												// ADV7604DEV2 CONTRAST  
	128,													// ADV7604DEV2 SATURATION
	0,       											// ADV7604DEV2 HUE
	0,														// ADV7604DEV3 BRIGHTNESS
	128,  												// ADV7604DEV3 CONTRAST  
	128,													// ADV7604DEV3 SATURATION
	0,       											// ADV7604DEV3 HUE
	0,														// ADV7604DEV4 BRIGHTNESS
	128,  												// ADV7604DEV4 CONTRAST  
	128,													// ADV7604DEV4 SATURATION
	0,       											// ADV7604DEV4 HUE
	0,														// ADV7604DEV5 BRIGHTNESS
	128,  												// ADV7604DEV5 CONTRAST  
	128,													// ADV7604DEV5 SATURATION
	0,       											// ADV7604DEV5 HUE
	0,														// Input 0 video switch 
	0,														// Input 1 video switch 
	0,														// Input 2 video switch 
	0,														// Input 3 video switch 
	0,														// Input 4 video switch 
	0,														// Input 5 video switch 
	0x80,								// TVP5158_DEV0_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV0_PORT0 CONTRAST
	0x80,								// TVP5158_DEV0_PORT0 SATURATION
	0x00,								// TVP5158_DEV0_PORT0 HUE
	0x80,								// TVP5158_DEV0_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV0_PORT1 CONTRAST
	0x80,								// TVP5158_DEV0_PORT1 SATURATION
	0x00,								// TVP5158_DEV0_PORT1 HUE
	0x80,								// TVP5158_DEV0_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV0_PORT2 CONTRAST
	0x80,								// TVP5158_DEV0_PORT2 SATURATION
	0x00,								// TVP5158_DEV0_PORT2 HUE
	0x80,								// TVP5158_DEV0_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV0_PORT3 CONTRAST
	0x80,								// TVP5158_DEV0_PORT3 SATURATION
	0x00,								// TVP5158_DEV0_PORT3 HUE
	0x80,								// TVP5158_DEV1_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV1_PORT0 CONTRAST
	0x80,								// TVP5158_DEV1_PORT0 SATURATION
	0x00,								// TVP5158_DEV1_PORT0 HUE
	0x80,								// TVP5158_DEV1_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV1_PORT1 CONTRAST
	0x80,								// TVP5158_DEV1_PORT1 SATURATION
	0x00,								// TVP5158_DEV1_PORT1 HUE
	0x80,								// TVP5158_DEV1_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV1_PORT2 CONTRAST
	0x80,								// TVP5158_DEV1_PORT2 SATURATION
	0x00,								// TVP5158_DEV1_PORT2 HUE
	0x80,								// TVP5158_DEV1_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV1_PORT3 CONTRAST
	0x80,								// TVP5158_DEV1_PORT3 SATURATION
	0x00,								// TVP5158_DEV1_PORT3 HUE
	0x80,								// TVP5158_DEV2_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV2_PORT0 CONTRAST  
	0x80,								// TVP5158_DEV2_PORT0 SATURATION
	0x00,								// TVP5158_DEV2_PORT0 HUE
	0x80,								// TVP5158_DEV2_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV2_PORT1 CONTRAST  
	0x80,								// TVP5158_DEV2_PORT1 SATURATION
	0x00,								// TVP5158_DEV2_PORT1 HUE
	0x80,								// TVP5158_DEV2_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV2_PORT2 CONTRAST  
	0x80,								// TVP5158_DEV2_PORT2 SATURATION
	0x00,								// TVP5158_DEV2_PORT2 HUE
	0x80,								// TVP5158_DEV2_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV2_PORT3 CONTRAST  
	0x80,								// TVP5158_DEV2_PORT3 SATURATION
	0x00,								// TVP5158_DEV2_PORT3 HUE
	0x80,								// TVP5158_DEV3_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV3_PORT0 CONTRAST
	0x80,								// TVP5158_DEV3_PORT0 SATURATION
	0x00,								// TVP5158_DEV3_PORT0 HUE
	0x80,								// TVP5158_DEV3_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV3_PORT1 CONTRAST
	0x80,								// TVP5158_DEV3_PORT1 SATURATION
	0x00,								// TVP5158_DEV3_PORT1 HUE
	0x80,								// TVP5158_DEV3_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV3_PORT2 CONTRAST
	0x80,								// TVP5158_DEV3_PORT2 SATURATION
	0x00,								// TVP5158_DEV3_PORT2 HUE
	0x80,								// TVP5158_DEV3_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV3_PORT3 CONTRAST
	0x80,								// TVP5158_DEV3_PORT3 SATURATION
	0x00,								// TVP5158_DEV3_PORT3 HUE
	0x80,								// TVP5158_DEV4_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV4_PORT0 CONTRAST
	0x80,								// TVP5158_DEV4_PORT0 SATURATION
	0x00,								// TVP5158_DEV4_PORT0 HUE
	0x80,								// TVP5158_DEV4_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV4_PORT1 CONTRAST
	0x80,								// TVP5158_DEV4_PORT1 SATURATION
	0x00,								// TVP5158_DEV4_PORT1 HUE
	0x80,								// TVP5158_DEV4_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV4_PORT2 CONTRAST
	0x80,								// TVP5158_DEV4_PORT2 SATURATION
	0x00,								// TVP5158_DEV4_PORT2 HUE
	0x80,								// TVP5158_DEV4_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV4_PORT3 CONTRAST
	0x80,								// TVP5158_DEV4_PORT3 SATURATION
	0x00,								// TVP5158_DEV4_PORT3 HUE
	0x80,								// TVP5158_DEV5_PORT0 BRIGHTNESS
	0x80,								// TVP5158_DEV5_PORT0 CONTRAST
	0x80,								// TVP5158_DEV5_PORT0 SATURATION
	0x00,								// TVP5158_DEV5_PORT0 HUE
	0x80,								// TVP5158_DEV5_PORT1 BRIGHTNESS
	0x80,								// TVP5158_DEV5_PORT1 CONTRAST
	0x80,								// TVP5158_DEV5_PORT1 SATURATION
	0x00,								// TVP5158_DEV5_PORT1 HUE
	0x80,								// TVP5158_DEV5_PORT2 BRIGHTNESS
	0x80,								// TVP5158_DEV5_PORT2 CONTRAST
	0x80,								// TVP5158_DEV5_PORT2 SATURATION
	0x00,								// TVP5158_DEV5_PORT2 HUE
	0x80,								// TVP5158_DEV5_PORT3 BRIGHTNESS
	0x80,								// TVP5158_DEV5_PORT3 CONTRAST
	0x80,								// TVP5158_DEV5_PORT3 SATURATION
	0x00,								// TVP5158_DEV5_PORT3 HUE
		
	3,														// ScreenTiming(Display Resolution Index 3--1920 x 1080@60)
	1920,													// ScreenWidth(Display Resolution)
	1080,													// ScreenHeight(Display Resolution)
	1,														// Screen Custom Resolution Enable
	6,														// Screen Custom Resolution Clock
	16,														// Screen Custom VModeIndex
	2200,													// Screen Custom Resolution Total Pixel
	1125,													// Screen Custom Resolution Total Line
	0,														// Screen Custom Resolution H Start
	0,														// Screen Custom Resolution V Start
	1920,													// Screen Custom Resolution Active Pixel
	1080,													// Screen Custom Resolution Active Line
	0,														// NVR_DISPLAY_MODE_INDEX
	4,					// NVR_SCALER_TOTAL_NUM,
	0,					// NVR_SCALER0_BOARD_INDEX,
	0,					// NVR_SCALER0_SCALER_INDEX,
	0,					// NVR_SCALER0_SOURCE_INDEX,
	1920,				// NVR_SCALER0_IN_HORI_SIZE,
	1080,				// NVR_SCALER0_IN_VERT_SIZE,
	0,					// NVR_SCALER0_IN_FIRST_PIXEL,
	1919,				// NVR_SCALER0_IN_LAST_PIXEL,
	0,					// NVR_SCALER0_IN_FIRST_LINE,
	1079,				// NVR_SCALER0_IN_LAST_LINE,
	1920,				// NVR_SCALER0_OUT_HORI_SIZE,
	1080,				// NVR_SCALER0_OUT_VERT_SIZE,
	0,					// NVR_SCALER1_BOARD_INDEX,
	1,					// NVR_SCALER1_SCALER_INDEX,
	0,					// NVR_SCALER1_SOURCE_INDEX,
	1920,				// NVR_SCALER1_IN_HORI_SIZE,
	1080,				// NVR_SCALER1_IN_VERT_SIZE,
	0,					// NVR_SCALER1_IN_FIRST_PIXEL,
	1919,				// NVR_SCALER1_IN_LAST_PIXEL,
	0,					// NVR_SCALER1_IN_FIRST_LINE,
	1079,				// NVR_SCALER1_IN_LAST_LINE,
	1920,				// NVR_SCALER1_OUT_HORI_SIZE,
	1080,				// NVR_SCALER1_OUT_VERT_SIZE,
	0,					// NVR_SCALER2_BOARD_INDEX,
	2,					// NVR_SCALER2_SCALER_INDEX,
	0,					// NVR_SCALER2_SOURCE_INDEX,
	1920,				// NVR_SCALER2_IN_HORI_SIZE,
	1080,				// NVR_SCALER2_IN_VERT_SIZE,
	0,					// NVR_SCALER2_IN_FIRST_PIXEL,
	1919,				// NVR_SCALER2_IN_LAST_PIXEL,
	0,					// NVR_SCALER2_IN_FIRST_LINE,
	1079,				// NVR_SCALER2_IN_LAST_LINE,
	1920,				// NVR_SCALER2_OUT_HORI_SIZE,
	1080,				// NVR_SCALER2_OUT_VERT_SIZE,
	0,					// NVR_SCALER3_BOARD_INDEX,
	3,					// NVR_SCALER3_SCALER_INDEX,
	0,					// NVR_SCALER3_SOURCE_INDEX,
	1920,				// NVR_SCALER3_IN_HORI_SIZE,
	1080,				// NVR_SCALER3_IN_VERT_SIZE,
	0,					// NVR_SCALER3_IN_FIRST_PIXEL,
	1919,				// NVR_SCALER3_IN_LAST_PIXEL,
	0,					// NVR_SCALER3_IN_FIRST_LINE,
	1079,				// NVR_SCALER3_IN_LAST_LINE,
	1920,				// NVR_SCALER3_OUT_HORI_SIZE,
	1080,				// NVR_SCALER3_OUT_VERT_SIZE,
	0,					// NVR_WINDOWS_TOTAL_NUM,
	0,					// NVR_WINDOWS_BOARD_INDEX,
	0,					// NVR_WINDOWS_BACKGROUND_INDEX,
	0,					// NVR_WINDOWS_SCREEN_WIDTH,
	0,					// NVR_WINDOWS_SCREEN_HEIGHT,
	0,					// NVR_WINDOW0_SIGNAL_SOURCE,
	0,					// NVR_WINDOW0_WINDOWS_LAYER,
	0,					// NVR_WINDOW0_IN_LEFT,
	0,					// NVR_WINDOW0_IN_TOP,
	0,					// NVR_WINDOW0_IN_RIGHT,
	0,					// NVR_WINDOW0_IN_BOTTOM,
	0,					// NVR_WINDOW0_DISPLAY_LEFT,
	0,					// NVR_WINDOW0_DISPLAY_TOP,
	0,					// NVR_WINDOW0_DISPLAY_RIGHT,
	0,					// NVR_WINDOW0_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW1_SIGNAL_SOURCE,
	0,					// NVR_WINDOW1_WINDOWS_LAYER,
	0,					// NVR_WINDOW1_IN_LEFT,
	0,					// NVR_WINDOW1_IN_TOP,
	0,					// NVR_WINDOW1_IN_RIGHT,
	0,					// NVR_WINDOW1_IN_BOTTOM,
	0,					// NVR_WINDOW1_DISPLAY_LEFT,
	0,					// NVR_WINDOW1_DISPLAY_TOP,
	0,					// NVR_WINDOW1_DISPLAY_RIGHT,
	0,					// NVR_WINDOW1_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW2_SIGNAL_SOURCE,
	0,					// NVR_WINDOW2_WINDOWS_LAYER,
	0,					// NVR_WINDOW2_IN_LEFT,
	0,					// NVR_WINDOW2_IN_TOP,
	0,					// NVR_WINDOW2_IN_RIGHT,
	0,					// NVR_WINDOW2_IN_BOTTOM,
	0,					// NVR_WINDOW2_DISPLAY_LEFT,
	0,					// NVR_WINDOW2_DISPLAY_TOP,
	0,					// NVR_WINDOW2_DISPLAY_RIGHT,
	0,					// NVR_WINDOW2_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW3_SIGNAL_SOURCE,
	0,					// NVR_WINDOW3_WINDOWS_LAYER,
	0,					// NVR_WINDOW3_IN_LEFT,
	0,					// NVR_WINDOW3_IN_TOP,
	0,					// NVR_WINDOW3_IN_RIGHT,
	0,					// NVR_WINDOW3_IN_BOTTOM,
	0,					// NVR_WINDOW3_DISPLAY_LEFT,
	0,					// NVR_WINDOW3_DISPLAY_TOP,
	0,					// NVR_WINDOW3_DISPLAY_RIGHT,
	0,					// NVR_WINDOW3_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW4_SIGNAL_SOURCE,
	0,					// NVR_WINDOW4_WINDOWS_LAYER,
	0,					// NVR_WINDOW4_IN_LEFT,
	0,					// NVR_WINDOW4_IN_TOP,
	0,					// NVR_WINDOW4_IN_RIGHT,
	0,					// NVR_WINDOW4_IN_BOTTOM,
	0,					// NVR_WINDOW4_DISPLAY_LEFT,
	0,					// NVR_WINDOW4_DISPLAY_TOP,
	0,					// NVR_WINDOW4_DISPLAY_RIGHT,
	0,					// NVR_WINDOW4_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW5_SIGNAL_SOURCE,
	0,					// NVR_WINDOW5_WINDOWS_LAYER,
	0,					// NVR_WINDOW5_IN_LEFT,
	0,					// NVR_WINDOW5_IN_TOP,
	0,					// NVR_WINDOW5_IN_RIGHT,
	0,					// NVR_WINDOW5_IN_BOTTOM,
	0,					// NVR_WINDOW5_DISPLAY_LEFT,
	0,					// NVR_WINDOW5_DISPLAY_TOP,
	0,					// NVR_WINDOW5_DISPLAY_RIGHT,
	0,					// NVR_WINDOW5_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW6_SIGNAL_SOURCE,
	0,					// NVR_WINDOW6_WINDOWS_LAYER,
	0,					// NVR_WINDOW6_IN_LEFT,
	0,					// NVR_WINDOW6_IN_TOP,
	0,					// NVR_WINDOW6_IN_RIGHT,
	0,					// NVR_WINDOW6_IN_BOTTOM,
	0,					// NVR_WINDOW6_DISPLAY_LEFT,
	0,					// NVR_WINDOW6_DISPLAY_TOP,
	0,					// NVR_WINDOW6_DISPLAY_RIGHT,
	0,					// NVR_WINDOW6_DISPLAY_BOTTOM,
	0,					// NVR_WINDOW7_SIGNAL_SOURCE,
	0,					// NVR_WINDOW7_WINDOWS_LAYER,
	0,					// NVR_WINDOW7_IN_LEFT,
	0,					// NVR_WINDOW7_IN_TOP,
	0,					// NVR_WINDOW7_IN_RIGHT,
	0,					// NVR_WINDOW7_IN_BOTTOM,
	0,					// NVR_WINDOW7_DISPLAY_LEFT,
	0,					// NVR_WINDOW7_DISPLAY_TOP,
	0,					// NVR_WINDOW7_DISPLAY_RIGHT,
	0,					// NVR_WINDOW7_DISPLAY_BOTTOM,
};

/****************************************************************************/
/*	S T A T I C    F U N C T I O N S										*/
/****************************************************************************/


/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* FUNCTION:	void ParamInit(void)										*/
/*																			*/
/* USAGE:		Initialize the system paramter in EEprom if require.		*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void ParamInit (void)
{
	WORD W_TempData = 0, W_TempData1 = 0;
	WORD W_Checksum = 0;				// defaults checksum
	//
	// Check eeprom parameters status
	//
	printf("Start Init System Param.\r\n");
//	ParamWrite(NVR_CHECKSUM, 0);		// write checksum
	W_Checksum = ParamChecksum() & 0xFFFF;
	W_TempData = ParamRead(NVR_CHECKSUM);
//	printf("NVR_CHECKSUM = 0x%04X\r\n", NVR_CHECKSUM);
//	printf("W_Checksum = 0x%04X, [NVR_CHECKSUM] = 0x%04X\r\n", W_Checksum, W_TempData);
	if (W_TempData != W_Checksum)		// checksum failure?
	{												// if yes,
		printf("Init system EEPROM. W_Checksum = 0x%04X, NVR_CHECKSUM = 0x%04X\r\n", W_Checksum, W_TempData);
		ParamReset(1);								// reset all parameters
		ParamWrite(NVR_CHECKSUM, W_Checksum);		// write checksum
		W_TempData1 = ParamRead(NVR_CHECKSUM);
		if(W_TempData1 != W_Checksum)
		{
			printf("%s Write Error, W_TempData1 = 0x%0X\r\n", __func__, W_TempData1);
			Delay10ms(5);
			ParamWrite(NVR_CHECKSUM, W_Checksum);
		}
	}

	gB_SerialMachineAddress = ParamRead(NVR_SERIAL_ADDR) & 0xFF;
	g_DisplayModeIndex  = ParamRead(NVR_DISPLAY_MODE_INDEX) & 0xFF;
	printf("NVR_SERIAL_ADDR: 0x%02X\r\n", gB_SerialMachineAddress);
	printf("Init System Param OK.\r\n");
}

void	BoardParamTCPInit (void)
{
	uint32_t W_TempData = 0, W_TempData1 = 0, W_TempData2 = 0, W_TempData3 = 0;
	W_TempData = ParamRead(NVR_TCP_MYIP_1) & 0xFF;
	W_TempData1 = ParamRead(NVR_TCP_MYIP_2) & 0xFF;
	//W_TempData = 192;
	//W_TempData1 = 168;
	W_TempData2 = ParamRead(NVR_TCP_MYIP_3) & 0xFF;
	W_TempData3 = ParamRead(NVR_TCP_MYIP_4) & 0xFF;
	// printf("IP: %d %d %d %d\r\n", W_TempData, W_TempData1, W_TempData2, W_TempData3);
	MX_LWIP_SetIPAddress(W_TempData, W_TempData1, W_TempData2, W_TempData3);
	W_TempData = ParamRead(NVR_TCP_LOCAL_PORT);
	// printf("TCP_LOCAL_PORT: %d\r\n", W_TempData);
	gW_TCPServerPort = W_TempData;
	/* tcp echo server Init */
	tcp_echoserver_init();
}

/****************************************************************************/
/*																			*/
/* ROUTINE:		PARAM ParamRead (WORD W_ParamAddr)							*/
/*																			*/
/* USAGE:		This function retrieves data regarding parameter specified	*/
/*				at W_ParamAddr.												*/
/*																			*/
/* INPUT:		W_ParamAddr - word-aligned address to read from				*/
/*																			*/
/* OUTPUT:		data from non-volatile memory at some function of			*/
/*				W_ParamAddr.												*/
/*																			*/
/****************************************************************************/
PARAM ParamRead (WORD W_ParamAddr)
{
	PARAM W_TempData = 0;
	if (InNVR(W_ParamAddr))							// in non-volatile memory?
	{
		#ifdef EEPROM_AT24C04
		W_TempData = (nvrC04_ReadWord					// yes, read from eeprom
				(((W_ParamAddr + MEM_EEPROM_OFFSET) & ~MDEV_MASK)*2));	//
		#endif
		#ifdef EEPROM_AT24C256
		W_TempData = (nvrC256_ReadWord					// yes, read from eeprom
				(((W_ParamAddr + MEM_EEPROM_OFFSET) & ~MDEV_MASK)*2));	//
		#endif
	}
//	printf("%s 0x%04X = [0x%04X]\r\n", __func__, W_ParamAddr, W_TempData);
	return W_TempData;										// return param address
}

/****************************************************************************/
/*																			*/
/* ROUTINE:		void ParamWrite (WORD W_ParamAddr, PARAM W_Data)			*/
/*																			*/
/* USAGE:		This routine stores the word W_Data into non-volatile		*/
/*				or volatile memory at address W_ParamAddr.					*/
/*																			*/
/* INPUT:		W_ParamAddr - address of parameter to write					*/
/*				W_Data - data to write										*/
/*																			*/
/* OUTPUT:		none														*/
/*																			*/
/****************************************************************************/
void ParamWrite (WORD W_ParamAddr, PARAM W_Data)
{
//	PARAM W_TempData = 0;
	if (InNVR(W_ParamAddr))							// in non-volatile memory?
	{		
		#ifdef EEPROM_AT24C04
		nvrC04_WriteWord							// yes, write to eeprom
			(((W_ParamAddr + MEM_EEPROM_OFFSET) & ~MDEV_MASK)*2, W_Data & 0xFFFF);	//
		#endif
		#ifdef EEPROM_AT24C256
		nvrC256_WriteWord							// yes, write to eeprom
			(((W_ParamAddr + MEM_EEPROM_OFFSET) & ~MDEV_MASK)*2, W_Data & 0xFFFF);	//
		#endif
		Delay1ms(2);
	}
//	printf("%s 0x%04X = [0x%04X]\r\n", __func__, W_ParamAddr, W_Data);
	#if 0
	W_TempData = ParamRead(W_ParamAddr);
	if(W_TempData != W_Data)
	{
		printf("%s Error! 0x%04X = [0x%04X]\r\n", __func__, W_ParamAddr, W_Data);
		Delay10ms(1);
		#ifdef EEPROM_AT24C04
		nvrC04_WriteWord							// yes, write to eeprom
			(((W_ParamAddr + MEM_EEPROM_OFFSET)&~MDEV_MASK)*2, W_Data);	//
		#endif
		#ifdef EEPROM_AT24C256
		nvrC256_WriteWord							// yes, write to eeprom
			(((W_ParamAddr + MEM_EEPROM_OFFSET)&~MDEV_MASK)*2, W_Data);	//
		#endif
	}
	#endif
	return;											// invalid param address
}

/****************************************************************************/
/*																			*/
/* ROUTINE:		PARAM GetParamDefault (WORD W_ParamAddr)					*/
/*																			*/
/* USAGE:		This function returns the default parameter value (on		*/
/*				startup) at W_ParamAddr.									*/
/*																			*/
/* INPUT:		W_ParamAddr - address of parameter							*/
/*																			*/
/* OUTPUT:		data from Wa_NVRParamDefaults indexed by W_ParamAddr.		*/
/*																			*/
/****************************************************************************/
PARAM ParamDefault (WORD W_ParamAddr)
{
	if (InNVR(W_ParamAddr))							// in non-volatile memory?
		return (PARAM) Wa_NVRParamDefaults			// yes, read from table
					[W_ParamAddr&~MDEV_MASK];		//
													// return zero for all
	return 0;										// other devices
}

/****************************************************************************/
/*																			*/
/* ROUTINE:		void ParamReset (bit Bt_ResetAll)							*/
/*																			*/
/* USAGE:		This routine reloads defaults for all "non-critical"		*/
/*				hardware parameters.										*/
/*																			*/
/* INPUT:		Bt_ResetAll - '1' to reset all parameters.					*/
/*							  '0' to exclude timing, colorspace and DCLK	*/
/*							  setting										*/
/*																			*/
/* OUTPUT:		none														*/
/*																			*/
/****************************************************************************/
void ParamReset (BYTE B_ResetAll)
{
	WORD i = NVR_RELOAD_START, j = 0;			// default start location
//	WORD W_Data = 0;

	if (B_ResetAll)							// reset all case start location
		i = NVR_START;						//
	j = i - NVR_MODE;

	printf("%s Write Start,i = %d\r\n", __func__, i);

	do										// loop start
	{
		ParamWrite (i, ParamDefault(j));		// write default
		j++;
	} while (++i < MAX_NVR_PARAM);			// loop end

	#if 1
	j = MAX_NVR_PARAM + 1 + ((MAX_NVR_PARAM - NVR_SCALER_TOTAL_NUM) * 10);
	for(i = MAX_NVR_PARAM + 1; i < j; i++)
		ParamWrite (i, 0xFFFF);
	#endif
	printf("%s END\r\n", __func__);
}

/****************************************************************************/
/*																			*/
/* ROUTINE:		WORD ParamChecksum (void)									*/
/*																			*/
/* USAGE:		Calculates checksum for DEFAULT parameters settings.		*/
/*																			*/
/* INPUT:		none														*/
/*																			*/
/* OUTPUT:		two's complement single byte checksum value					*/
/*																			*/
/****************************************************************************/
WORD ParamChecksum (void)
{
	WORD i;											// loop iterator
	WORD W_Checksum = 0;							// checksum

	for (i = NVR_START; i < MAX_NVR_PARAM; i++)	// compute checksum
		W_Checksum += ParamDefault(i - NVR_MODE);				// of all default params

	return (~W_Checksum) + 1;							// return 2's complement
}

/********************************  END  ************************************/
