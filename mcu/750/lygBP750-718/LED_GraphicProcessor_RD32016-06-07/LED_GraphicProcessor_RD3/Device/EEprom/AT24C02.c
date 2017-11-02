#define __AT24C02_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2000.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      AT24C02.c                                                   */
/*																			*/
/* USAGE:       This module contains functions for Microchip EEPROM 24AA02. */
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#include "../../TMC Board/all.h"
#include "AT24C02.h"
#include <stdio.h>

//
// Include files requires
//

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

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
/* FUNCTION:    BYTE nvrC02_ReadByte(BYTE B_DeviceName, BYTE B_Addr)		*/
/*                                                                          */
/* USAGE:       Read a byte of data from EEPROM.                   			*/
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address                                     */
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
BYTE nvrC02_ReadByte(BYTE B_DeviceName, BYTE B_Addr)
{
	BYTE* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, 1)) == NULL)
		{
			printf("EEPROM%d read ERROR!\n", B_DeviceName);
			return 0xFF;
		}
		else
			return *pB_Data;
	}
    return (*pB_Data);							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC02_ReadWord(BYTE B_DeviceName, BYTE B_Addr)		*/
/*                                                                          */
/* USAGE:       Read 2 bytes of data from EEPROM.                  			*/
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address (upper byte address)				*/
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
WORD nvrC02_ReadWord(BYTE B_DeviceName, BYTE B_Addr)
{
	BYTE* pB_Data = NULL;
	WORD W_Data;
	if ((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, 1)) == NULL)
		{
			printf("EEPROM%d read ERROR!\n", B_DeviceName);
			return 0xFFFF;
		}
		else
		    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
	}
    W_Data  = (*pB_Data) << 8;  			// Read upper WORD of data
//	printf("Read hi W_Addr = %04x, Data = %02x\n", W_Addr, *pB_Data);

	if ((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr + 1, 1)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr + 1, 1)) == NULL)
		{
			printf("EEPROM%d read ERROR!\n", B_DeviceName);
			return 0xFFFF;
		}
		else
		{
		    W_Data |= (*pB_Data);          			// Read lower WORD of data
			return W_Data;
		}
	}
    W_Data |= (*pB_Data);          			// Read lower WORD of data
	return W_Data;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC02_ReadData(BYTE B_DeviceName,						*/ 
/*									BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len)	*/
/*                                                                          */
/* USAGE:       Read EEPROM data and copy it into buffer.                   */
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address                                     */
/*              Bp_Buf - buffer use to store the returning EEPROM data      */
/*              B_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE* nvrC02_ReadData(BYTE B_DeviceName, BYTE B_Addr, BYTE B_Len)
{
	BYTE* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, B_Len)) == NULL)
	{
		Delay10ms(1);
		if((pB_Data = I2C1ReadByte(B_DeviceName, B_Addr, B_Len)) == NULL)
		{
			printf("EEPROM%d read ERROR!\n", B_DeviceName);
			return NULL;
		}
		else
			return pB_Data;
	}
    return pB_Data;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC02_WriteByte(BYTE B_DeviceName, 					*/
/*									BYTE B_Addr, BYTE B_Data)				*/
/*                                                                          */
/* USAGE:       Write a byte of data into EEPROM.                    		*/
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address                                     */
/*              B_Data - data write into EEPROM      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC02_WriteByte(BYTE B_DeviceName, BYTE B_Addr, BYTE B_Data)
{
	BYTE i = 0;
	while (I2C1WriteByte(B_DeviceName, B_Addr, 1, &B_Data) == ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("EEPROM%d write ERROR!\n", B_DeviceName);
			return (BYTE) ERR;
		}
	}
    return OK;							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC02_WriteWord(BYTE B_DeviceName,						*/
/*									BYTE B_Addr, BYTE W_Data)				*/
/*                                                                          */
/* USAGE:       Write 2 bytes of data into EEPROM.                    		*/
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address (upper byte address)				*/
/*              W_Data - data write into EEPROM      						*/
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC02_WriteWord(BYTE B_DeviceName, BYTE B_Addr, WORD W_Data)
{
	BYTE B_Data[2] = {0}, i = 0;
	B_Data[0] = (BYTE)(W_Data >> 8);
	B_Data[1] = (BYTE)(W_Data);
	i = 0;
	while (I2C1WriteByte(B_DeviceName, B_Addr, 1, B_Data) == ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("EEPROM%d write ERROR!\n", B_DeviceName);
			return (BYTE) ERR;
		}
	}
	if((((B_Addr + 1) % NVR_C02_CACHE_LEN)) == 0)
		Delay10ms(1);
	i = 1;
	while (I2C1WriteByte(B_DeviceName, B_Addr + 1, 1, &(B_Data[1])) == ERR)
	{
		Delay10ms(1);
		i++;
		if(i == 5)
		{
			printf("EEPROM%d write ERROR!\n", B_DeviceName);
			return (BYTE) ERR;
		}
	}
    return OK;							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    BYTE nvrC02_WriteData(BYTE B_DeviceName,					*/
/*									BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len)	*/
/*                                                                          */
/* USAGE:       Write the required data into EEPROM.                    	*/
/*                                                                          */
/* INPUT:       B_Addr - EEPROM address                                     */
/*              Bp_Buf - buffer use to store the EEPROM data      			*/
/*              B_Len  - number of byte to read                             */
/*                                                                          */
/* OUTPUT:      OK 		 - successful                                       */
/*              ERR_READ - error                                            */
/*                                                                          */
/****************************************************************************/
BYTE nvrC02_WriteData(BYTE B_DeviceName, BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len)
{
	BYTE *Bp_Data = NULL, i = 0, j = 0, k = 0, B_TempAddr;
	Bp_Data = Bp_Buf;
	B_TempAddr = B_Addr;
	k = NVR_C02_CACHE_LEN - (B_Addr % NVR_C02_CACHE_LEN);
	if(k >= B_Len)
	{
		j = 0;
		while(I2C1WriteByte(B_DeviceName, B_Addr, B_Len, Bp_Buf) == ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM%d write ERROR!\n", B_DeviceName);
				return (BYTE) ERR;
			}
		}
	}
	else
	{
//		printf("Write: W_TempAddr: 0x%02x, Len: %d\n",B_TempAddr, k);
		j = 0;
		while(I2C1WriteByte(B_DeviceName, B_TempAddr, k, Bp_Data) == ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM%d write ERROR!\n", B_DeviceName);
				return (BYTE) ERR;
			}
		}
		Delay10ms(1);
		Bp_Data += k;
		B_TempAddr += k;
		i = (B_Len - k) / NVR_C02_CACHE_LEN;
		j = 0;
		for(k = 0; k < i; k++)
		{
//			printf("Write: W_TempAddr: 0x%02x, Len: %d\n",B_TempAddr, NVR_C02_CACHE_LEN);
			while(I2C1WriteByte(B_DeviceName, B_TempAddr, NVR_C02_CACHE_LEN, Bp_Data) == ERR)
			{
				Delay10ms(1);
				j++;
				if(j == 5)
				{
					printf("EEPROM%d write ERROR!\n", B_DeviceName);
					return (BYTE) ERR;
				}
			}
			Bp_Data += NVR_C02_CACHE_LEN;
			B_TempAddr += NVR_C02_CACHE_LEN;
			Delay10ms(1);
		}
		k = NVR_C02_CACHE_LEN - (B_Addr % NVR_C02_CACHE_LEN);
		i = (B_Len - k) % NVR_C02_CACHE_LEN;
		j = 0;
//		printf("Write: W_TempAddr: 0x%02x, Len: %d\n",B_TempAddr, i);
		while(I2C1WriteByte(B_DeviceName, B_TempAddr, i, Bp_Data) == ERR)
		{
			Delay10ms(1);
			j++;
			if(j == 5)
			{
				printf("EEPROM%d write ERROR!\n", B_DeviceName);
				return (BYTE) ERR;
			}
		}
	}
    return OK;							// 
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    nvrC02_DummyCall(void)                                       */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void nvrC02_DummyCall(void)
{
	nvrC02_ReadWord	(I2C1_DEVICE_MAX, 0);
	nvrC02_WriteWord(I2C1_DEVICE_MAX, 0, 0);
	nvrC02_ReadData	(I2C1_DEVICE_MAX, 0, 0);
	nvrC02_WriteData(I2C1_DEVICE_MAX, 0, 0, 0);
}
/********************************  END  *************************************/

