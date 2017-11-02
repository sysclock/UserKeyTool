#define __AT24C04_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C04.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip EEPROM 24LC16B */
/*				which has 2Kbyte of storage.								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include "../../System Code/Timer.h"
#include "../../IIC Comm/D_IIC0.h"
//#include "../SerialToParallel/STP_74HC595.h"
#include "AT24C04.h"
#include <stdio.h>

//#define EEPROM_PORT_SET				0x6FFF
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

BYTE nvrC04_ReadByte	(WORD W_Addr);
BYTE nvrC04_WriteByte	(WORD W_Addr, BYTE B_Data);
WORD nvrC04_ReadWord	(WORD W_Addr);
BYTE nvrC04_WriteWord	(WORD W_Addr, WORD W_Data);
BYTE* nvrC04_ReadData	(WORD W_Addr, WORD W_Len);
BYTE nvrC04_WriteData	(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len);
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
/* FUNCTION:    BYTE nvrC04_Read(WORD W_Addr)   							*/
/*                                                                          */
/* USAGE:       Read a byte of data from EEPROM.                   			*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address                                     */
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
BYTE nvrC04_ReadByte(WORD W_Addr)
{
	BYTE* pB_Data = NULL;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	if ((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C04_DEV);
			return 0xFF;
		}
		else
		{
			return *pB_Data;
		}
	}
  return (*pB_Data);							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC04_ReadWord(WORD W_Addr)							*/
/*                                                                          */
/* USAGE:       Read 2 bytes of data from EEPROM.                  			*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address (upper byte address)				*/
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
WORD nvrC04_ReadWord(WORD W_Addr)
{
/*
	BYTE* pB_Data = NULL;
	WORD W_Data;
	if ((pB_Data = I2C0ReadByte(NVR_C04_DEV, W_Addr, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C0ReadByte(NVR_C04_DEV, W_Addr, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C04_DEV);
			return 0xFFFF;
		}
		else
		    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
	}
    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
//	printf("Read hi W_Addr = %04x, Data = %02x\r\n", W_Addr, *pB_Data);

	if ((pB_Data = I2C0ReadByte(NVR_C04_DEV, W_Addr + 1, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C0ReadByte(NVR_C04_DEV, W_Addr + 1, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C04_DEV);
			return 0xFFFF;
		}
		else
		{
		    W_Data |= (*pB_Data);          			// Read lower WORD of data
			return W_Data;
		}
	}
    W_Data |= (*pB_Data);          			// Read lower WORD of data
//	printf("Read low W_Addr = %04x, Data = %02x\r\n", W_Addr + 1, *pB_Data);
	return W_Data;
*/
	BYTE* pB_Data = NULL;
	WORD W_Data;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	if ((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 2)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 2)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C04_DEV);
			return 0xFFFF;
		}
		else
		{
		    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
			pB_Data++;
		    W_Data |= (*pB_Data);          			// Read lower WORD of data
			return W_Data;
		}
	}
    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
	pB_Data++;
    W_Data |= (*pB_Data);          			// Read lower WORD of data
//	printf("EEPROM READ 0x%04X = [0x%04X]\r\n", W_Addr, W_Data);
	return W_Data;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC04_ReadData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)*/
/*                                                                          */
/* USAGE:       Read EEPROM data and copy it into buffer.                   */
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address                                     */
/*              Bp_Buf - buffer use to store the returning EEPROM data      */
/*              W_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE* nvrC04_ReadData(WORD W_Addr, WORD W_Len)
{
	BYTE* pB_Data = NULL;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	if ((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, W_Len)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C0ReadByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, W_Len)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C04_DEV);
			return NULL;
		}
		else
		{
			return pB_Data;
		}
	}
  return pB_Data;
}
/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC04_WriteByte(WORD W_Addr, BYTE B_Data)			*/
/*                                                                          */
/* USAGE:       Write a byte of data into EEPROM.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address                                     */
/*              B_Data - data write into EEPROM      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC04_WriteByte(WORD W_Addr, BYTE B_Data)
{
	BYTE i = 0;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	i = 0;
	while (I2C0WriteByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
			return (BYTE) ERR;
		}
	}
  return OK;							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC04_WriteWord(WORD W_Addr, BYTE W_Data)			*/
/*                                                                          */
/* USAGE:       Write 2 bytes of data into EEPROM.                    		*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address (upper byte address)				*/
/*              W_Data - data write into EEPROM      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC04_WriteWord(WORD W_Addr, WORD W_Data)
{
	BYTE B_Data[2] = {0}, i = 0;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	B_Data[0] = (BYTE)(W_Data >> 8);
	B_Data[1] = (BYTE)(W_Data);

//	printf("%s Write W_Addr = %04x, Data = %02x\r\n", __func__, W_Addr, B_Data[0]);
//	printf("%s Write W_Addr = 0x%04x, Data = 0x%0X\r\n", __func__, W_Addr, W_Data);
//	printf("EEPROM WRITE 0x%04X = [0x%04X]\r\n", W_Addr, W_Data);
	if((((W_Addr + 1) % NVR_C04_CACHE_LEN)) == 0)
	{
		i = 0;
		while (I2C0WriteByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &(B_Data[0])) == (BYTE) ERR)
		{
			Delay10ms(1);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
		Delay10ms(1);
		i = 0;
		while (I2C0WriteByte(NVR_C04_DEV | B_Page, W_Addr + 1, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &(B_Data[1])) == (BYTE) ERR)
		{
			Delay10ms(1);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
	  return OK;							// 
	}
	else
	{
		i = 0;
		while (I2C0WriteByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 2, B_Data) == (BYTE) ERR)
		{
			Delay10ms(1);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
		return OK;
	}
//	printf("Write low W_Addr = %04x, Data = %02x\r\n", W_Addr + 1, B_Data[1]);
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC04_WriteData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)	*/
/*                                                                          */
/* USAGE:       Write the required data into EEPROM.                    	*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address                                     */
/*              Bp_Buf - buffer use to store the EEPROM data      			*/
/*              W_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC04_WriteData(WORD W_Addr, BYTE *Bp_Buf, WORD W_Len)
{
	WORD i = 0, j = 0, k = 0, W_TempAddr;
	BYTE	*Bp_Data = NULL;
	BYTE B_Page = 0;
	B_Page = (W_Addr / 256) << 1;
	Bp_Data = Bp_Buf;
	W_TempAddr = W_Addr;
	k = NVR_C04_CACHE_LEN - (W_Addr % NVR_C04_CACHE_LEN);
	if(k >= W_Len)
	{
		j = 0;
		while(I2C0WriteByte(NVR_C04_DEV | B_Page, W_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, W_Len, Bp_Buf) == (BYTE) ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
	}
	else
	{
//		printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
		j = 0;
		while(I2C0WriteByte(NVR_C04_DEV | B_Page, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_8BIT, k, Bp_Data) == (BYTE) ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
		Delay10ms(1);
		Bp_Data += k;
		W_TempAddr += k;
		i = (W_Len - k) / NVR_C04_CACHE_LEN;
		for(k = 0; k < i; k++)
		{
//			printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
			j = 0;
			while(I2C0WriteByte(NVR_C04_DEV | B_Page, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_8BIT, NVR_C04_CACHE_LEN, Bp_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				j++;
				if(j == 5)
				{
					printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
					return (BYTE) ERR;
				}
			}
			Bp_Data += NVR_C04_CACHE_LEN;
			W_TempAddr += NVR_C04_CACHE_LEN;
			Delay10ms(1);
		}
		j = 0;
		k = NVR_C04_CACHE_LEN - (W_Addr % NVR_C04_CACHE_LEN);
		i = (W_Len - k) % NVR_C04_CACHE_LEN;
//		printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
		while(I2C0WriteByte(NVR_C04_DEV | B_Page, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_8BIT, i, Bp_Data) == (BYTE) ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C04_DEV);
				return (BYTE) ERR;
			}
		}
	}
  return OK;							// 
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    nvrC04_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void nvrC04_DummyCall(void)
{
	nvrC04_ReadByte	(0);
	nvrC04_WriteByte	(0, 0);
	nvrC04_ReadWord	(0);
	nvrC04_WriteWord	(0, 0);
	nvrC04_ReadData	(0, 0);
	nvrC04_WriteData	(0, 0, 0);
}
/********************************  END  *************************************/

