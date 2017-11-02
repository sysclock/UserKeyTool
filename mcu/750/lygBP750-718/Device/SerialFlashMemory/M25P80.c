#define __M25P80_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C256.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip EEPROM 24LC16B */
/*				which has 2Kbyte of storage.								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include "../../System Code/lpc17xx.h"
#include "../../System Code/ASCII.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "../../System Code/Timer.h"
#include "../../SPI Comm/D_SPI.h"
#include "M25P80.h"
#include <string.h>
#include <stdio.h>

#define 		M25P80_Debug	1
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
WORD SFM_M25P80_ReadBufIndex;
BYTE SFM_M25P80_ReadBuf[SFM_M25P80_CACHE_LEN];

/****************************************************************************/
/*	G L O B A L    F U N C T I O N											*/
/****************************************************************************/

void SFM_M25P80_CE(BYTE B_SetData);
void SFM_M25P80Init(void);
void SFM_WriteEnable	(void);
void SFM_WriteDisable	(void);
BYTE* SFM_ReadIdentification	(void);
BYTE SFM_ReadStatusRegister	(void);
void SFM_WriteStatusRegister	(BYTE B_Data);
BYTE* SFM_ReadData	(WORD W_Addr, WORD W_Len);
BYTE* SFM_ReadDataFast	(WORD W_Addr, WORD W_Len);
BYTE SFM_PageWriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
BYTE SFM_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
BYTE SFM_SectorErase	(WORD W_Addr);
void SFM_BulkErase	(void);
void SFM_DeepPower_down	(void);
void SFM_ReleaseFromDeepPower_down	(void);

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void SFM_M25P80Init(void)
{
	// 设置M25P80的控制引脚 */
  LPC_GPIO1->FIODIR   |=  M25P80_SPI_CS;            // P1.21 is output
  LPC_GPIO1->FIOPIN   |=  M25P80_SPI_CS;            // set P1.21 high (SSEL inact.)
  LPC_PINCON->PINSEL3 &= ~(0<<10);            			// P1.21 SSEL (used as GPIO)
	SFM_M25P80_ReadBufIndex = 0;
	memset(SFM_M25P80_ReadBuf, 0, sizeof(SFM_M25P80_ReadBuf));
	printf("M25P80 init OK!\r\n");
}

void SFM_M25P80_CE(BYTE B_SetData)
{
  // Enable/Disable SPI Chip Select (drive it high or low).

  if (B_SetData)
    LPC_GPIO1->FIOPIN |= M25P80_SPI_CS;             // SSEL is GPIO, set to high.
  else
    LPC_GPIO1->FIOPIN &= ~M25P80_SPI_CS;            // SSEL is GPIO, set to low.
}

void SFM_WriteEnable	(void)
{
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_WREN);
	SFM_M25P80_CE(1);
}

void SFM_WriteDisable	(void)
{
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_WRDI);
	SFM_M25P80_CE(1);
}

BYTE SFM_SectorErase	(WORD W_Addr)
{
	BYTE	i = 0, B_Data = 0;
	WORD	W_TempAddr = 0;
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
	SFM_WriteEnable();
	W_TempAddr = W_Addr & 0xF0000;
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_SE);
	SPI_SendData(((W_TempAddr & 0xFFFFFF) >> 16));						// 发送地址信息:该地址为3个字节 
	SPI_SendData(((W_TempAddr & 0xFFFF) >> 8));
	SPI_SendData(W_TempAddr & 0xFF);
	SFM_M25P80_CE(1);
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
	return OK;
}

void SFM_BulkErase	(void)
{
	BYTE	i = 0, B_Data = 0;
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
	SFM_WriteEnable();
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_BE);
	SFM_M25P80_CE(1);
	i = 0;
	while(i < (SFM_M25P80_BUSY_TIME * 16))
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
		{
			printf("M25P80 Bulk Erase Complete!\r\n");
			break;
		}
	}
}

void SFM_DeepPower_down	(void)
{
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_DP);
	SFM_M25P80_CE(1);
}

void SFM_ReleaseFromDeepPower_down	(void)
{
	BYTE	i = 0, B_Data = 0;
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_RES);
	SFM_M25P80_CE(1);
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
}

BYTE* SFM_ReadIdentification	(void)
{
	BYTE* pB_Data = NULL;
	BYTE B_Data = 0, i = 0;
	pB_Data = SFM_M25P80_ReadBuf;
	SFM_M25P80_ReadBufIndex = 0;
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_RDID);
	for(i = 0; i < 20; i++)
	{
		B_Data = SPI_SendData(0xFF);
		*pB_Data = B_Data;
		pB_Data++;
		SFM_M25P80_ReadBufIndex++;
		#if	(M25P80_Debug == 1)
			printf("M25P80_Debug REV DATA: i = %d 0x%02x\r\n", i, B_Data);
		#endif
	}
	SFM_M25P80_CE(1);
	return SFM_M25P80_ReadBuf;
}

BYTE SFM_ReadStatusRegister	(void)
{
	BYTE B_Data = 0;
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_RDSR);
	B_Data = SPI_SendData(0xFF);
	#if	(M25P80_Debug == 1)
		printf("SFM_M25P80_RDSR = 0x%02x\r\n", B_Data);
	#endif
	SFM_M25P80_CE(1);
	return B_Data;
}

void SFM_WriteStatusRegister	(BYTE B_Data)
{
	int	i = 0;
	BYTE B_TempData = 0;
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_TempData = SFM_ReadStatusRegister();
		if(B_TempData & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_WRSR);
	B_Data = SPI_SendData(B_Data);
	#if	(M25P80_Debug == 1)
		printf("SFM_M25P80_WRSR = 0x%02x\r\n", B_Data);
	#endif
	SFM_M25P80_CE(1);
}

BYTE* SFM_ReadData	(WORD W_Addr, WORD W_Len)
{
	BYTE* pB_Data = NULL;
	BYTE B_Data = 0;
	WORD i = 0;
	#if	(M25P80_Debug == 1)
		printf("M25P80_Debug Read %d DATA.\r\n", W_Len);
	#endif
	pB_Data = SFM_M25P80_ReadBuf;
	SFM_M25P80_ReadBufIndex = 0;
	i = 0;
	while(i < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_READ);
	SPI_SendData(((W_Addr & 0xFFFFFF) >> 16));						// 发送地址信息:该地址为3个字节 
	SPI_SendData(((W_Addr & 0xFFFF) >> 8));
	SPI_SendData(W_Addr & 0xFF);
	for(i = 0; i < W_Len; i++)
	{
		B_Data = SPI_SendData(0xFF);
		*pB_Data = B_Data;
		pB_Data++;
		SFM_M25P80_ReadBufIndex++;
		#if	(M25P80_Debug == 1)
			printf("M25P80_Debug REV DATA: W_Addr = %d  B_Data = %d\r\n", W_Addr + i, B_Data);
		#endif
	}
	SFM_M25P80_CE(1);
	return SFM_M25P80_ReadBuf;
}

// 当W_Len > SFM_M25P80_CACHE_LEN 时，依次写 SFM_M25P80_CACHE_LEN 个数据，最后写入剩余数据 ，
// 如果先写入剩余数据 ，再写 SFM_M25P80_CACHE_LEN 个数时，读出来的数据就会有问题，原因没有找到。
BYTE SFM_PageWriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)
{
	BYTE* pB_Data = NULL;
	BYTE B_TempData = 0, B_Data = 0;
	WORD W_TempAddr = 0;
	int i = 0, k = 0, j = 0, m = 0;
	#if	(M25P80_Debug == 1)
		printf("M25P80_Debug Page Write %d DATA.\r\n", W_Len);
	#endif
	k = W_Len % SFM_M25P80_CACHE_LEN;
	j = W_Len / SFM_M25P80_CACHE_LEN;
	B_TempData = SFM_ReadStatusRegister();
	SFM_WriteStatusRegister(0);												// 清SRWD位，使Flash芯片全区可写

	W_TempAddr = W_Addr;
	pB_Data = Bp_Buf;
	for(i = 0; i < j; i++)
	{
		m = 0;
		while(m < SFM_M25P80_BUSY_TIME)
		{
			B_Data = SFM_ReadStatusRegister();
			if(B_Data & SFM_M25P80_WIP)
			{
				#if	(M25P80_Debug == 1)
					printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
				#endif
				Delay100ms(1);
			}
			else
				break;
		}

		SFM_WriteEnable();
		SFM_M25P80_CE(0);
		SPI_SendData(SFM_M25P80_PP);
		SPI_SendData(((W_TempAddr & 0xFFFFFF) >> 16));						// 发送地址信息:该地址为3个字节 
		SPI_SendData(((W_TempAddr & 0xFFFF) >> 8));
		SPI_SendData(W_TempAddr & 0xFF);
		for(k = 0; k < SFM_M25P80_CACHE_LEN; k++)
		{
			SPI_SendData(*pB_Data);
			#if	(M25P80_Debug == 1)
				printf("M25P80_Debug WRITE DATA: W_Addr = %d  B_Data = %d\r\n", W_TempAddr + k, *pB_Data);
			#endif
			pB_Data++;
		}
		SFM_M25P80_CE(1);
//		SFM_WriteDisable();
		W_TempAddr += SFM_M25P80_CACHE_LEN;
	}
	m = 0;
	while(m < SFM_M25P80_BUSY_TIME)
	{
		B_Data = SFM_ReadStatusRegister();
		if(B_Data & SFM_M25P80_WIP)
		{
			#if	(M25P80_Debug == 1)
				printf("M25P80 The Memory is Busy, Delay 100 ms\r\n");
			#endif
			Delay100ms(1);
		}
		else
			break;
	}

	k = W_Len % SFM_M25P80_CACHE_LEN;
	SFM_WriteEnable();
	SFM_M25P80_CE(0);
	SPI_SendData(SFM_M25P80_PP);
	SPI_SendData(((W_TempAddr & 0xFFFFFF) >> 16));						// 发送地址信息:该地址为3个字节 
	SPI_SendData(((W_TempAddr & 0xFFFF) >> 8));
	SPI_SendData(W_TempAddr & 0xFF);
	for(i = 0; i < k; i++)
	{
		SPI_SendData(*pB_Data);
		#if	(M25P80_Debug == 1)
			printf("M25P80_Debug WRITE DATA: W_Addr = %d  B_Data = %d\r\n", W_Addr + i, *pB_Data);
		#endif
		pB_Data++;
	}
	SFM_M25P80_CE(1);
	SFM_WriteStatusRegister(B_TempData);
	return OK;							// 
}

BYTE SFM_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)
{
	BYTE* pB_Data = NULL;
	WORD W_TempAddr = 0;
	int i = 0, k = 0, j = 0;

	#if	(M25P80_Debug == 1)
		printf("M25P80_Debug Write %d DATA.\r\n", W_Len);
	#endif

	pB_Data = Bp_Buf;
	k = SFM_M25P80_PAGE_SIZE - (W_Addr % SFM_M25P80_PAGE_SIZE);
	if(k >= W_Len)
		SFM_PageWriteData	(W_Addr, pB_Data, W_Len);
	else
	{
		SFM_PageWriteData	(W_Addr, pB_Data, k);
		j = (W_Len - k) / SFM_M25P80_PAGE_SIZE;
		i = (W_Len - k) % SFM_M25P80_PAGE_SIZE;
		W_TempAddr = W_Addr + k;
		pB_Data += k;
		for(k = 0; k < j; k++)
		{
			SFM_PageWriteData	(W_TempAddr, pB_Data, SFM_M25P80_PAGE_SIZE);
			W_TempAddr += SFM_M25P80_PAGE_SIZE;
			pB_Data += SFM_M25P80_PAGE_SIZE;
		}
		SFM_PageWriteData	(W_TempAddr, pB_Data, i);
	}
	return OK;							// 
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    M25P80_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void M25P80_DummyCall(void)
{
/*
	nvrC256_ReadByte	(0);
	nvrC256_WriteByte	(0, 0);
	nvrC256_ReadWord	(0);
	nvrC256_WriteWord	(0, 0);
	nvrC256_ReadData	(0, 0);
	nvrC256_WriteData	(0, 0, 0);
*/
}
/********************************  END  *************************************/

