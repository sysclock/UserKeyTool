/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      gv7601.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip Input gv7601 */
/*																																			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "type.h"
#include "Timer.h"
#include "../Device/InputFPGA/InputFPGA.h"
#include "gv7601.h"

#define INPUT_GV7601_DEBUG					0
#define SIMULATED_GV7601_DEV				3
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
GV7601_GlobalStatus_t gSt_GV7601Status_t;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void GV7601_GSPI_Init(void);
int GV7601_Init(BYTE B_SPIIndex);
int GV7601_GetStatus(BYTE B_SPIIndex);
int GV7601_getVD_STD(BYTE B_SPIIndex, GV7601_VIDEO_STD *std);
WORD GV7601_ReadWord(BYTE B_SPI_Cs, WORD W_Addr, WORD* pReadData);
WORD GV7601_WriteWord(BYTE B_SPI_Cs, WORD W_Addr, WORD W_Data);

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void GV7601_GSPI_Init(void)
{
	/*
  LPC_PINCON->PINSEL0 &= ~(3UL<<30);          			// P0.15 cleared	GSPI_SCLK
	LPC_PINCON->PINSEL1 &= ~(3UL << 0);								// P0.16 cleared GSPI_CS
	LPC_PINCON->PINSEL1 &= ~(3UL << 2);								// P0.17 cleared GSPI_SDOUT
  LPC_PINCON->PINSEL1 &= ~((3UL<<4));								// P0.18 cleared GSPI_SDIN
	
	//	Set	P0.15 GSPI_SCLK As Output
	//	Set	P0.16 GSPI_SCLK As Output
	//	Set P0.17 GSPI_SDOUT As Input
	//	Set P0.18 GSPI_SDIN As Output
	LPC_GPIO0->FIODIR |= BIT15 | BIT16 | BIT18;
	LPC_GPIO0->FIODIR &= ~BIT17;											// P0.17 cleared
	LPC_GPIO0->FIOCLR = BIT15;												// Set all GSPI_SCLK pin to '0'
	LPC_GPIO0->FIOSET = BIT16;												// Set all GSPI_CS pin to '1'
	LPC_GPIO0->FIOSET = BIT18;												// Set all GSPI_SDIN pin to '1'
	*/
}

int GV7601_Init(BYTE B_SPIIndex)
{
	int ret = 0;
	ret = GV7601_WriteWord(BIT0 << B_SPIIndex, 0x00, 0xC0);
//	ret = GV7601_WriteWord(BIT0 << B_SPIIndex, 0x24, 0x06);
	ret = GV7601_WriteWord(BIT0 << B_SPIIndex, 0x6D, 0x3B);
	if(0 != ret)
		return -1;
	return 0;
}

int GV7601_GetStatus(BYTE B_SPIIndex)
{
	int ret = 0;
	WORD rx[4] = {0};
	GV7601_VIDEO_STD std = GV7601_STD_INVALID;
	std = GV7601_STD_INVALID;
	#if(SIMULATED_GV7601_DEV == 1)
		if(B_SPIIndex == 0)
		{
			gSt_GV7601Status_t.VideoState = GV7601_STD_1920X1080P_60;
			ret = InputFPGA_ReadVipStatus(B_SPIIndex, &rx[0], &rx[1], &rx[2], &rx[3]);
			gSt_GV7601Status_t.VideoRefresh = 60;
			gSt_GV7601Status_t.VideoHTotalSize = rx[0];
			gSt_GV7601Status_t.VideoVTotalSize = rx[1];
			gSt_GV7601Status_t.VideoHSize = rx[2];
			gSt_GV7601Status_t.VideoVSize = rx[3];
			gSt_GV7601Status_t.VideoInterlaced = ret;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 2)
		if((B_SPIIndex == 0) || (B_SPIIndex == 1))
		{
			gSt_GV7601Status_t.VideoState = GV7601_STD_1920X1080P_60;
			ret = InputFPGA_ReadVipStatus(B_SPIIndex, &rx[0], &rx[1], &rx[2], &rx[3]);
			gSt_GV7601Status_t.VideoRefresh = 60;
			gSt_GV7601Status_t.VideoHTotalSize = rx[0];
			gSt_GV7601Status_t.VideoVTotalSize = rx[1];
			gSt_GV7601Status_t.VideoHSize = rx[2];
			gSt_GV7601Status_t.VideoVSize = rx[3];
			gSt_GV7601Status_t.VideoInterlaced = ret;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 3)
		if((B_SPIIndex == 2) || (B_SPIIndex == 3) || (B_SPIIndex == 4))
		{
			gSt_GV7601Status_t.VideoState = GV7601_STD_1920X1080P_60;
			ret = InputFPGA_ReadVipStatus(B_SPIIndex, &rx[0], &rx[1], &rx[2], &rx[3]);
			gSt_GV7601Status_t.VideoRefresh = 60;
			gSt_GV7601Status_t.VideoHTotalSize = rx[0];
			gSt_GV7601Status_t.VideoVTotalSize = rx[1];
			gSt_GV7601Status_t.VideoHSize = rx[2];
			gSt_GV7601Status_t.VideoVSize = rx[3];
			gSt_GV7601Status_t.VideoInterlaced = ret;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 4)
		if((B_SPIIndex == 0) || (B_SPIIndex == 1) || (B_SPIIndex == 2) || (B_SPIIndex == 3))
		{
			gSt_GV7601Status_t.VideoState = GV7601_STD_1920X1080P_60;
			ret = InputFPGA_ReadVipStatus(B_SPIIndex, &rx[0], &rx[1], &rx[2], &rx[3]);
			gSt_GV7601Status_t.VideoRefresh = 60;
			gSt_GV7601Status_t.VideoHTotalSize = rx[0];
			gSt_GV7601Status_t.VideoVTotalSize = rx[1];
			gSt_GV7601Status_t.VideoHSize = rx[2];
			gSt_GV7601Status_t.VideoVSize = rx[3];
			gSt_GV7601Status_t.VideoInterlaced = ret;
			return 0;
		}
	#endif
	ret = GV7601_getVD_STD(B_SPIIndex, &std);
	if(0 != ret)
	{
		#if(INPUT_GV7601_DEBUG > 0)
		printf("GV7601_getVD_STD() fail.......................................\r\n");
		#endif
		return 0;
	}
	if(std != gSt_GV7601Status_t.VideoState)
		gSt_GV7601Status_t.VideoState = std;
	
//		printf("GV7601 VD_STD:\r\n");
		switch(std)
		{
			case GV7601_STD_720X480I_60Hz:
//				printf("    720 x 480i @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_720X576I_50Hz:
//				printf("    720 x 576i @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 50;
				break;
			case GV7601_STD_1280X720P_24:
//				printf("    1280 x 720p @ 24Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 240;
				break;
			case GV7601_STD_1280X720P_25:
//				printf("    1280 x 720p @ 25Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 25;
				break;
			case GV7601_STD_1280X720P_30:
//				printf("    1280 x 720p @ 30Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 30;
				break;
			case GV7601_STD_1280X720P_60 :
//				printf("    1280 x 720p @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_1280X720P_50 :
//				printf("    1280 x 720p @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 50;
				break;
			case GV7601_STD_1920X1080I_60:
//				printf("    1920 x 1080i @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_1920X1080I_50:
//				printf("    1920 x 1080i @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 50;
				break;
			case GV7601_STD_1920X1080P_30:
//				printf("    1920 x 1080p @ 29.97/30Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 30;
				break;
			case GV7601_STD_1920X1080P_25:
//				printf("    1920 x 1080p @ 25Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 25;
				break;
			case GV7601_STD_1920X1080P_24:
//				printf("    1920 x 1080p @ 24Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 24;
				break;
			case GV7601_STD_1920X1080P_60:
//				printf("    1920 x 1080p @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_1920X1080P_50:
//				printf("    1920 x 1080p @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 50;
				break;
			case GV7601_STD_1920X1035I_60:
//				printf("    1920 x 1035i @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_1920X1035I_50:
//				printf("    1920 x 1035i @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 50;
				break;
			case GV7601_STD_1920X1035P_60:
//				printf("    1920 x 1035p @ 59.94/60Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_1920X1035P_50:
//				printf("    1920 x 1035p @ 50Hz\r\n");
				gSt_GV7601Status_t.VideoRefresh = 60;
				break;
			case GV7601_STD_INVALID:
//				printf("Invalid video std\r\n");
				gSt_GV7601Status_t.VideoRefresh = 0;
				break;
			default:
//				printf("Invalid std val:%d\r\n",std);
				gSt_GV7601Status_t.VideoRefresh = 0;
				break;
		}
	
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x1F, rx);
	if(0 != ret)
		return -1;
	gSt_GV7601Status_t.VideoHSize = rx[0] & 0x3FFF;
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x22, rx);
	if(0 != ret)
		return -1;
	gSt_GV7601Status_t.VideoVSize = rx[0] & 0x7FF;
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x20, rx);
	if(0 != ret)
		return -1;
	gSt_GV7601Status_t.VideoHTotalSize = rx[0] & 0x3FFF;
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x21, rx);
	if(0 != ret)
		return -1;
	gSt_GV7601Status_t.VideoVTotalSize = rx[0] & 0x7FF;
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x22, rx);
	if(0 != ret)
		return -1;
	gSt_GV7601Status_t.VideoInterlaced = (rx[0] & BIT11) ? 1 : 0;
	return 0;
}

int GV7601_getVD_STD(BYTE B_SPIIndex, GV7601_VIDEO_STD *std)
{
	unsigned short i = 0, j = 0;
	int ret = 0;
	WORD rx[2];

	if(NULL == std)
	{
		#if(INPUT_GV7601_DEBUG > 0)
		printf("\r\nInvalid paramer(NULL Pointer)......\r\n");
		#endif
		return -1;
	}
	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 6, &rx[0]);
	if(0 != ret)
	{
		#if(INPUT_GV7601_DEBUG > 0)
		printf("\r\n gv7601 read std register fail......\r\n");
		#endif
		return -1;
	}
	i = rx[0];
	i = i >> 8;
	i &= 0x1f;

	ret = GV7601_ReadWord(BIT0 << B_SPIIndex, 0x25, &rx[1]);
	if(0 != ret)
	{
		#if(INPUT_GV7601_DEBUG > 0)
		printf("\r\n gv7601 read FORMAT_ID_861 register fail......\r\n");
		#endif
		return -1;
	}
	j = rx[1];
	if(rx[1] & BIT6)
		j = 0;
	else
		j &= 0x3F;
	
//	printf("gv7601 read std = 0x%02X, B_SPIIndex = 0x%02X\r\n", i, B_SPIIndex);
	switch(i)
	{
		case 0x16:
		case 0x17:
		case 0x19:
		case 0x1B:
			*std = GV7601_STD_720X480I_60Hz;
			break;
		case 0x18:
		case 0x1A:
			*std = GV7601_STD_720X576I_50Hz;
			break;
		case 0x08:
		case 0x09:
		case 0x28:
			*std = GV7601_STD_1280X720P_24;
			break;
		case 0x06:
		case 0x07:
		case 0x26:
			*std = GV7601_STD_1280X720P_25;
			break;
		case 0x02:
		case 0x03:
			*std = GV7601_STD_1280X720P_30;
			break;
		case 0x00:
		case 0x01:
		case 0x20:
			*std = GV7601_STD_1280X720P_60;
			break;
		case 0x04:
		case 0x05:
		case 0x24:
			*std = GV7601_STD_1280X720P_50;
			break;
		case 0x0A:
		case 0x2A:
			*std = GV7601_STD_1920X1080I_60;
			break;
		case 0x0C:
		case 0x14:
		case 0x2C:
			*std = GV7601_STD_1920X1080I_50;
			break;
		case 0x30:
		case 0x10:
		case 0x12:
			*std = GV7601_STD_1920X1080P_24;
			break;
		case 0x0B:
		case 0x2B:
			if(j == 16)
				*std = GV7601_STD_1920X1080P_60;
			else
				*std = GV7601_STD_1920X1080P_30;
			break;
		case 0x0D:
		case 0x0E:
		case 0x2D:
			if(j == 31)
				*std = GV7601_STD_1920X1080P_50;
			else
				*std = GV7601_STD_1920X1080P_25;
			break;
		case 0x15:
			*std = GV7601_STD_1920X1035I_60;
			break;
		default:
			*std = GV7601_STD_INVALID;
			break;
	}
	
	return 0;
}

WORD GV7601_WriteWord(BYTE B_SPI_Cs, WORD W_Addr, WORD W_Data)
{
	return 0;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE GV7601_ReadWord(BYTE B_SPI_Cs, WORD W_Addr, WORD* pReadData)							*/
/*                                                                          */
/* USAGE:       Read 4 bytes of data from GV7601.														*/
/*							Use FPGA Read GV7601 SPI Port      													*/
/*                                                                          */
/* INPUT:       W_Addr - GV7601 address (upper byte address)				*/
/*                                                                          */
/* OUTPUT:      GV7601 data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
WORD GV7601_ReadWord(BYTE B_SPI_Cs, WORD W_Addr, WORD* pReadData)
{
	#if(SIMULATED_GV7601_DEV == 1)
		if(B_SPI_Cs == BIT0)
		{
			if(W_Addr == 0)
				*pReadData = 0x00;
			else if(W_Addr == 1)
				*pReadData = 0x100;
			else if(W_Addr == 6)
				*pReadData = 0x0B << 8;
			else if(W_Addr == 0x25)
				*pReadData = 16;
			else
				*pReadData = 0;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 2)
		if((B_SPI_Cs == BIT0) || (B_SPI_Cs == BIT1))
		{
			if(W_Addr == 0)
				*pReadData = 0x00;
			else if(W_Addr == 1)
				*pReadData = 0x100;
			else if(W_Addr == 6)
				*pReadData = 0x0B << 8;
			else if(W_Addr == 0x25)
				*pReadData = 16;
			else
				*pReadData = 0;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 3)
		if((B_SPI_Cs == BIT2) || (B_SPI_Cs == BIT3) || (B_SPI_Cs == BIT4))
		{
			if(W_Addr == 0)
				*pReadData = 0x00;
			else if(W_Addr == 1)
				*pReadData = 0x100;
			else if(W_Addr == 6)
				*pReadData = 0x0B << 8;
			else if(W_Addr == 0x25)
				*pReadData = 16;
			else
				*pReadData = 0;
			return 0;
		}
	#endif
	#if(SIMULATED_GV7601_DEV == 4)
		if((B_SPI_Cs == BIT0) || (B_SPI_Cs == BIT1) || (B_SPI_Cs == BIT2) || (B_SPI_Cs == BIT3))
		{
			if(W_Addr == 0)
				*pReadData = 0x00;
			else if(W_Addr == 1)
				*pReadData = 0x100;
			else if(W_Addr == 6)
				*pReadData = 0x0B << 8;
			else if(W_Addr == 0x25)
				*pReadData = 16;
			else
				*pReadData = 0;
			return 0;
		}
	#endif
	*pReadData = 0;
	return 0;
}

