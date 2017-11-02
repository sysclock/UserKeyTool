#define __AT24C256_C__
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

#include <stdio.h>
#include "Timer.h"
#include "D_IIC.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "D_IIC3.h"
#include "AT24C256.h"

//#define EEPROM_PORT_SET				0x6FFF
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

uint8_t nvrC256_ReadByte	(uint32_t W_Addr);
uint8_t nvrC256_WriteByte	(uint32_t W_Addr, uint8_t B_Data);
uint32_t nvrC256_ReadWord	(uint32_t W_Addr);
uint8_t nvrC256_WriteWord	(uint32_t W_Addr, uint32_t W_Data);
uint8_t* nvrC256_ReadData	(uint32_t W_Addr, uint32_t W_Len);
uint8_t nvrC256_WriteData	(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len);
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
/* FUNCTION:    uint8_t nvrC256_Read(uint32_t W_Addr)   							*/
/*                                                                          */
/* USAGE:       Read a byte of data from EEPROM.                   			*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address                                     */
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
uint8_t nvrC256_ReadByte(uint32_t W_Addr)
{
	uint8_t* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1)) == NULL)
	{
		Delay1ms(5);
		if((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C256_DEV);
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
/* FUNCTION:    uint8_t nvrC256_ReadWord(uint32_t W_Addr)							*/
/*                                                                          */
/* USAGE:       Read 2 bytes of data from EEPROM.                  			*/
/*                                                                          */
/* INPUT:       W_Addr - EEPROM address (upper byte address)				*/
/*                                                                          */
/* OUTPUT:      EEPROM data (note:assume successful)						*/
/*                                                                          */
/****************************************************************************/
uint32_t nvrC256_ReadWord(uint32_t W_Addr)
{
/*
	uint8_t* pB_Data = NULL;
	uint32_t W_Data;
	if ((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, 1)) == NULL)
	{
		Delay1ms(5);
		if((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C256_DEV);
			return 0xFFFF;
		}
		else
		    W_Data  = (*pB_Data) << 8;  			// Read upper uint32_t of data
	}
    W_Data  = (*pB_Data) << 8;  			// Read upper uint32_t of data
//	printf("Read hi W_Addr = %04x, Data = %02x\r\n", W_Addr, *pB_Data);

	if ((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr + 1, 1)) == NULL)
	{
		Delay1ms(5);
		if((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr + 1, 1)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C256_DEV);
			return 0xFFFF;
		}
		else
		{
		    W_Data |= (*pB_Data);          			// Read lower uint32_t of data
			return W_Data;
		}
	}
    W_Data |= (*pB_Data);          			// Read lower uint32_t of data
//	printf("Read low W_Addr = %04x, Data = %02x\r\n", W_Addr + 1, *pB_Data);
	return W_Data;
*/
	uint8_t* pB_Data = NULL;
	uint32_t W_Data;
	if ((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 2)) == NULL)
	{
		Delay1ms(5);
		if((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 2)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C256_DEV);
			return 0xFFFF;
		}
		else
		{
		    W_Data  = (*pB_Data) << 8;  			// Read upper uint32_t of data
			pB_Data++;
		    W_Data |= (*pB_Data);          			// Read lower uint32_t of data
			return W_Data;
		}
	}
	W_Data  = (*pB_Data) << 8;  			// Read upper uint32_t of data
	pB_Data++;
	W_Data |= (*pB_Data);          			// Read lower uint32_t of data
//	printf("EEPROM READ 0x%04X = [0x%04X]\r\n", W_Addr, W_Data);
	return W_Data;
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t nvrC256_ReadData(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len)*/
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
uint8_t* nvrC256_ReadData(uint32_t W_Addr, uint32_t W_Len)
{
	uint8_t* pB_Data = NULL;
	if ((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len)) == NULL)
	{
		Delay1ms(5);
		if((pB_Data = I2C1ReadByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len)) == NULL)
		{
			printf("EEPROM 0x%02X read ERROR!\r\n", NVR_C256_DEV);
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
/* FUNCTION:    uint8_t nvrC256_WriteByte(uint32_t W_Addr, uint8_t B_Data)			*/
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
uint8_t nvrC256_WriteByte(uint32_t W_Addr, uint8_t B_Data)
{
	uint8_t i = 0;
	i = 0;
	while (I2C1WriteByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1, &B_Data) == (uint8_t) ERR)
	{
		Delay1ms(5);
		i++;
		if(i == 5)
		{
			printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
			return (uint8_t) ERR;
		}
	}
  return OK;							// 
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t nvrC256_WriteWord(uint32_t W_Addr, uint8_t W_Data)			*/
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
uint8_t nvrC256_WriteWord(uint32_t W_Addr, uint32_t W_Data)
{
	uint8_t B_Data[2] = {0}, i = 0;
	B_Data[0] = (uint8_t)(W_Data >> 8);
	B_Data[1] = (uint8_t)(W_Data);

//	printf("%s Write W_Addr = %04x, Data = %02x\r\n", __func__, W_Addr, B_Data[0]);
//	printf("%s Write W_Addr = 0x%04x, Data = 0x%0X\r\n", __func__, W_Addr, W_Data);
//	printf("EEPROM WRITE 0x%04X = [0x%04X]\r\n", W_Addr, W_Data);
	if((((W_Addr + 1) % NVR_C256_CACHE_LEN)) == 0)
	{
		i = 0;
		while (I2C1WriteByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 1, &(B_Data[0])) == (uint8_t) ERR)
		{
			Delay1ms(5);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
		Delay1ms(5);
		i = 0;
		while (I2C1WriteByte(NVR_C256_DEV, W_Addr + 1, I2C_DEV_MEMORY_ADDRESS_16BIT, 1, &(B_Data[1])) == (uint8_t) ERR)
		{
			Delay1ms(5);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
	  return OK;							// 
	}
	else
	{
		i = 0;
		while (I2C1WriteByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, 2, B_Data) == (uint8_t) ERR)
		{
			Delay1ms(5);
			i++;
			if(i == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
		return OK;
	}
//	printf("Write low W_Addr = %04x, Data = %02x\r\n", W_Addr + 1, B_Data[1]);
}

/****************************************************************************/
/*                                                                          */
/* FUNCTION:    uint8_t nvrC256_WriteData(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len)	*/
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
uint8_t nvrC256_WriteData(uint32_t W_Addr, uint8_t *Bp_Buf, uint32_t W_Len)
{
	uint32_t i = 0, j = 0, k = 0, W_TempAddr;
	uint8_t	*Bp_Data = NULL;
	Bp_Data = Bp_Buf;
	W_TempAddr = W_Addr;
	k = NVR_C256_CACHE_LEN - (W_Addr % NVR_C256_CACHE_LEN);
	if(k >= W_Len)
	{
		j = 0;
		while(I2C1WriteByte(NVR_C256_DEV, W_Addr, I2C_DEV_MEMORY_ADDRESS_16BIT, W_Len, Bp_Buf) == (uint8_t) ERR)
		{
			Delay1ms(5);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
	}
	else
	{
//		printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
		j = 0;
		while(I2C1WriteByte(NVR_C256_DEV, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_16BIT, k, Bp_Data) == (uint8_t) ERR)
		{
			Delay1ms(5);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
		Delay1ms(5);
		Bp_Data += k;
		W_TempAddr += k;
		i = (W_Len - k) / NVR_C256_CACHE_LEN;
		for(k = 0; k < i; k++)
		{
//			printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
			j = 0;
			while(I2C1WriteByte(NVR_C256_DEV, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_16BIT, NVR_C256_CACHE_LEN, Bp_Data) == (uint8_t) ERR)
			{
				Delay1ms(5);
				j++;
				if(j == 5)
				{
					printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
					return (uint8_t) ERR;
				}
			}
			Bp_Data += NVR_C256_CACHE_LEN;
			W_TempAddr += NVR_C256_CACHE_LEN;
			Delay1ms(5);
		}
		j = 0;
		k = NVR_C256_CACHE_LEN - (W_Addr % NVR_C256_CACHE_LEN);
		i = (W_Len - k) % NVR_C256_CACHE_LEN;
//		printf("Write: Bp_Data: 0x%04x, W_TempAddr: 0x%02x\r\n", Bp_Data, W_TempAddr);
		while(I2C1WriteByte(NVR_C256_DEV, W_TempAddr, I2C_DEV_MEMORY_ADDRESS_16BIT, i, Bp_Data) == (uint8_t) ERR)
		{
			Delay1ms(5);
			j++;
			if(j == 5)
			{
				printf("EEPROM 0x%02X write ERROR!\r\n", NVR_C256_DEV);
				return (uint8_t) ERR;
			}
		}
	}
  return OK;							// 
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    nvrC256_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void nvrC256_DummyCall(void)
{
	nvrC256_ReadByte	(0);
	nvrC256_WriteByte	(0, 0);
	nvrC256_ReadWord	(0);
	nvrC256_WriteWord	(0, 0);
	nvrC256_ReadData	(0, 0);
	nvrC256_WriteData	(0, 0, 0);
}
/********************************  END  *************************************/

