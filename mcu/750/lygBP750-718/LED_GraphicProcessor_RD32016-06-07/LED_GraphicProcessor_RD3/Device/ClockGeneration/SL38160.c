#define __SL38160_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      SL38160.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip SL38160 */
/*																																			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include <stdio.h>
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "SL38160.h"

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void Init_SL38160(void);
void Enable_SL38160(uint8_t B_DeviceID);
void PowerDown_SL38160(uint8_t B_DeviceID);
void Set_SL38160_CLK(uint8_t B_DeviceID, uint8_t audio_clk_num, uint8_t video_clk_num);
uint8_t SL38160_WriteByte	(uint8_t B_DeviceID, uint8_t B_Addr, uint8_t B_Data);
uint8_t I2C_WriteBlock_SL38160_HIGH126(uint8_t B_DeviceID, uint8_t reg128, uint8_t reg129);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/* FUNCTION:    void Init_SL38160(void)								*/
/*                                                                          */
/* USAGE:       SL38160 device initialization.						        */
/*                                                                          */
/* INPUT:       None                         								*/
/*                                                                          */
/* OUTPUT:      None														*/
/*                                                                          */
/****************************************************************************/
void Init_SL38160(void)
{
	#if 0
	while(1)
	{
			SL38160_WriteByte(DEVICEID_SL38160_18, 64, 0x08);					//	Disable REF-1
			Delay100ms(10);
	}
	#endif
//	PowerDown_SL38160(DEVICEID_SL38160_18);
//	printf("SL38160_18 Power Down!\r\n");
	
//	PowerDown_SL38160(DEVICEID_SL38160_17);
	Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 6);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
//	Set_SL38160_CLK(DEVICEID_SL38160_17, 10, 4);		// AUDIO_CLK 10--49.1520, VIDEO_CLK 6 - 148.50MHz, 5 - 148.35MHz, 4 - 74.25MHz
	printf("SL38160_17 init OK!\r\n");
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t SL38160_WriteByte(uint8_t B_DeviceID, WORD W_Addr, uint8_t B_Data)			*/
/*                                                                          */
/* USAGE:       Write a byte of data into SL38160.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - InputFPGA address                                     */
/*              B_Data - data write into SL38160      						*/
/*							B_DeviceID - 	SL38160-17 : DeviceID = 0xD2						*/
/*														SL38160-18 : DeviceID = 0xD4						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
uint8_t SL38160_WriteByte	(uint8_t B_DeviceID, uint8_t B_Addr, uint8_t B_Data)
{
	uint8_t i = 0;
	i = 0;
	while (I2C1WriteByte(B_DeviceID, B_Addr | 0x80, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (uint8_t) ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("SL38160 0x%02X write ERROR!\r\n", B_DeviceID);
			return (uint8_t) ERR;
		}
	}
	Delay1ms(1);
	return OK;							// 
}

uint8_t I2C_WriteBlock_SL38160_HIGH126(uint8_t B_DeviceID, uint8_t reg128, uint8_t reg129)
{
	uint8_t B_Buf[9], i = 0;
	B_Buf[0] = 0x26;
	B_Buf[1] = 0x7D;
	B_Buf[2] = reg128;
	B_Buf[3] = reg129;
	B_Buf[4] = 0x53;
	B_Buf[5] = 0xC0;
	B_Buf[6] = 0x10;
	B_Buf[7] = 0x55;
	B_Buf[8] = 0x7B;
	i = 0;
	while (I2C1WriteByte(B_DeviceID, 0xFE, I2C_DEV_MEMORY_ADDRESS_8BIT, 9, B_Buf) == (uint8_t) ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("SL38160 0x%02X write HIGH 126 ERROR!\r\n", B_DeviceID);
			return (uint8_t) ERR;
		}
	}
	Delay1ms(1);
	return OK;							// 
}

void Enable_SL38160(uint8_t B_DeviceID)
{
	//	Enable output
	SL38160_WriteByte(B_DeviceID, 64, 0x04);
	SL38160_WriteByte(B_DeviceID, 117, 0x00);

	if((B_DeviceID == DEVICEID_SL38160_17) || (B_DeviceID == DEVICEID_SL38160_23))
	{
		SL38160_WriteByte(B_DeviceID, 86, 0x04);
		SL38160_WriteByte(B_DeviceID, 96, 0x04);
	}

	//	Power Up
	SL38160_WriteByte(B_DeviceID, 29, 0x16);
}

void PowerDown_SL38160(uint8_t B_DeviceID)
{
	//	disable output
	SL38160_WriteByte(B_DeviceID, 64, 0x08);					//	Disable REF-1
	SL38160_WriteByte(B_DeviceID, 117, 0x04);					//	Disable VCLK

	if((B_DeviceID == DEVICEID_SL38160_17) || (B_DeviceID == DEVICEID_SL38160_23))
	{
		SL38160_WriteByte(B_DeviceID, 86, 0x08);	//	Disable ACLK
		SL38160_WriteByte(B_DeviceID, 96, 0x08);	//	Disable REF-2
	}

	//	Power Down
	SL38160_WriteByte(B_DeviceID, 29, 0x36);
}

void Set_SL38160_CLK(uint8_t B_DeviceID, uint8_t audio_clk_num, uint8_t video_clk_num)
{
	uint8_t		x, i, j;

	//	Set Audio CLK
	for(i = 0; i < 4; i++)
	{
		x = MCLK_Regs[audio_clk_num][i];
		SL38160_WriteByte(B_DeviceID, 40 + i, x);
	}
	//	An additional Byte outside 32d to 63d
	SL38160_WriteByte(B_DeviceID, 65, 0x00);

	if	(video_clk_num < 7)
	{	//	Video CLK
		for ( i = 0; i < 3; i++ )
		{
			x = VCLK_Regs[video_clk_num][i];
			SL38160_WriteByte(B_DeviceID, 17 + i, x);
		}
	
		SL38160_WriteByte(B_DeviceID, 22, VCLK_Regs[video_clk_num][3]);
	
		for ( i = 0; i < 4; i++ )
		{
			x = VCLK_Regs[video_clk_num][i + 4];
			SL38160_WriteByte(B_DeviceID, 48 + i, x);
		}
		//	An additional Byte outside 32d to 63d
		SL38160_WriteByte(B_DeviceID, 65, 0x00);
	
		x = VCLK_Regs[video_clk_num][8];
	}
	else
	{	// Montior CLK
		j = video_clk_num - 7;

		for ( i = 0; i < 3; i++ )
		{
			x = PCLK_Regs[j][i];
			SL38160_WriteByte(B_DeviceID, 17 + i, x);
		}
	
		x = PCLK_Regs[j][3];
		SL38160_WriteByte(B_DeviceID, 22, x);

		for ( i = 0; i < 3; i++ )
		{
			x = PCLK_Regs[j][i + 4];
			SL38160_WriteByte(B_DeviceID, 48 + i, x);
		}
		x = PCLK_Regs_Last5[0];
		SL38160_WriteByte(B_DeviceID, 51, x);
	
		//	An additional Byte outside 32d to 63d
		SL38160_WriteByte(B_DeviceID, 65, 0x00);
	
		x = PCLK_Regs_Last5[1];
		SL38160_WriteByte(B_DeviceID, 68, x);
		x = PCLK_Regs_Last5[2];
		SL38160_WriteByte(B_DeviceID, 69, x);
		x = PCLK_Regs_Last5[3];
		SL38160_WriteByte(B_DeviceID, 114, x);

		x = PCLK_Regs_Last5[4];
	}

	I2C_WriteBlock_SL38160_HIGH126(B_DeviceID, x, 0x81);
}
/****************************************************************************/
/*																			*/
/* FUNCTION:    InputFPGA_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void SL38160_DummyCall(void)
{
	SL38160_WriteByte	(0, 0, 0);
}
/********************************  END  *************************************/

