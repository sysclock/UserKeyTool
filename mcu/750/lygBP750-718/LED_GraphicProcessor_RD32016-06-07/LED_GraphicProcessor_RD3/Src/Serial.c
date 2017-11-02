#define __SERIAL_C__
/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>

#include "Timer.h"
#include "Serial.h"
#include "../User Command/UserCommandPC_API.h"

/****************************************************************************/
/*	D E F I N I T I O N														*/
/****************************************************************************/
#define 		SERIAL_COMMAD_DEBUG			0
#define 		SERIAL1_PASSTHROUGH			0
//#define CR     0x0D
#define SERIAL_COMMAD_BUF_LENGTH     	128
#define SERIAL_TIMEOUT_DURATION				0xFFFF
#define SERIAL_PASSTHROUGH__LENGTH    32

#define SERIAL_SEND_CHECK_TIMES				200000
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

#if 1
	/* Private function prototypes -----------------------------------------------*/
	#ifdef __GNUC__
	/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
		 set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
	#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
	#endif /* __GNUC__ */
#endif
unsigned char gB_SerialMachineAddress = 0;

unsigned char	gB_SerialCommadIndex = 0;
unsigned char	gB_SerialCommadBuf[SERIAL_COMMAD_BUF_LENGTH];
unsigned char gB_Serial1RxData[2] = {0};
//unsigned char gB_Serial2RxData[2] = {0};
unsigned char gB_Serial4RxData[2] = {0};
unsigned char gB_Serial5RxData[2] = {0};
unsigned char gB_Serial1TxData[2] = {0};
unsigned char gB_Serial5TxData[2] = {0};

Squeue	gSqu_Serial1ReceiveBuf;
//Squeue	gSqu_Serial2ReceiveBuf;
#if(SERIAL4_USED == 1)
	Squeue	gSqu_Serial4ReceiveBuf;
	unsigned char	gB_Serial4CommadIndex = 0;
	unsigned char	gB_Serial4CommadBuf[SERIAL_COMMAD_BUF_LENGTH] = {0};
#endif
#if(SERIAL5_USED == 1)
	Squeue	gSqu_Serial5ReceiveBuf;
	unsigned char	gB_Serial5CommadIndex = 0;
	unsigned char	gB_Serial5CommadBuf[SERIAL_COMMAD_BUF_LENGTH] = {0};
#endif


/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

unsigned char	gB_PassthroughSerialCommadBuf[SERIAL_COMMAD_BUF_LENGTH >> 1] = {0};
uint32_t gW_StaticSerialSendDataIndex = 0;
static uint32_t gW_StaticSerialSendData[SERIAL_PASSTHROUGH__LENGTH + 1] = {0};
/****************************************************************************/
/*  G L O B A L   F U N C T I O N												                    */
/****************************************************************************/
void Serial1_Init(void);
void Serial1CommandHandler(void);
void Serial4_Init(void);
void Serial4CommandHandler(void);
void Serial5_Init(void);
void Serial5CommandHandler(void);
unsigned char EnterSerialQueue(Squeue* Queue, unsigned char B_Data);
unsigned char Serial1_SendData(unsigned char *Bp_Buf, int W_Len);
//unsigned char Serial2_SendData(unsigned char *Bp_Buf, int W_Len);
unsigned char Serial4_SendData(unsigned char *Bp_Buf, int W_Len);
unsigned char Serial5_SendData(unsigned char *Bp_Buf, int W_Len);
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static void SerialCommadClear(void);
static unsigned char ParseSerialBinaryCommadBySelfType(uint8_t B_CommandSource, unsigned char B_Data);
static unsigned char ParseSerialCommadBySelfType(uint8_t B_CommandSource, unsigned char B_Data);
static void ClearQueue(Squeue* Queue);
static unsigned char IsEmptyQueue(Squeue* Queue);
static unsigned char EnterQueue(Squeue* Queue, unsigned char B_Data);
static unsigned char GetQueue(Squeue* Queue);
static unsigned char LengthQueue(Squeue* Queue);
static int sendchar1 (int ch);				// Write character to Serial Port
static int sendchar5 (int ch);				// Write character to Serial Port
/****************************************************************************/
/*																			*/
/* FUNCTION:	void serial1_Init											*/
/*																			*/
/* USAGE:		Initialize the serial1 inerface.							*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void Serial1_Init(void)
{
	ClearQueue(&gSqu_Serial1ReceiveBuf);
	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
	gB_SerialCommadIndex = 0;

//	HAL_UART_Transmit_IT(&huart1, gB_Serial1TxData, 1);
	// Enable uart RX
	HAL_UART_Receive_IT(&huart1, gB_Serial1RxData, 1);
	printf("Serial1 init OK!\r\n");
}

void Serial4_Init(void)
{
	#if(SERIAL4_USED == 1)
		ClearQueue(&gSqu_Serial4ReceiveBuf);
		memset(gB_Serial4CommadBuf, 0, sizeof(gB_Serial4CommadBuf));
		gB_Serial4CommadIndex = 0;
	#endif
	HAL_UART_Receive_IT(&huart4, gB_Serial4RxData, 1);
	printf("Serial4 init OK!\r\n");
}
/****************************************************************************/
/*																			*/
/* FUNCTION:	void serial6_Init											*/
/*																			*/
/* USAGE:		Initialize the serial1 inerface.							*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void Serial5_Init(void)
{
	#if(SERIAL5_USED == 1)
		ClearQueue(&gSqu_Serial5ReceiveBuf);
		memset(gB_Serial5CommadBuf, 0, sizeof(gB_Serial5CommadBuf));
		gB_Serial5CommadIndex = 0;
	#endif
	// Enable uart RX
	HAL_UART_Receive_IT(&huart5, gB_Serial5RxData, 1);
	printf("Serial5 init OK!\r\n");
}

void Serial1CommandHandler(void)
{
	unsigned char B_Temp = 0, B_SerialSelfCommandNum = 0, ret = 0;
	while(IsEmptyQueue(&gSqu_Serial1ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial1ReceiveBuf);
		ret = ParseSerialCommadBySelfType(COMMAD_FROM_SERIAL1, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		ret = ParseSerialBinaryCommadBySelfType(COMMAD_FROM_SERIAL1, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		#if 0
		ret = ParseLuanZongBinaryCommadPacket(COMMAD_FROM_SERIAL1, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		#endif
		if(B_SerialSelfCommandNum > 6)
			break;
	}
}

void Serial4CommandHandler(void)
{
	#if(SERIAL4_USED == 1)
	unsigned char B_Temp = 0, B_SerialSelfCommandNum = 0, ret = 0;
	while(IsEmptyQueue(&gSqu_Serial4ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial4ReceiveBuf);
		ret = ParseSerialCommadBySelfType(COMMAD_FROM_SERIAL4, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		ret = ParseSerialBinaryCommadBySelfType(COMMAD_FROM_SERIAL4, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		#if 0
			ret = ParseLuanZongBinaryCommadPacket(COMMAD_FROM_SERIAL4, B_Temp);
			if(ret == 1)
				B_SerialSelfCommandNum++;
		#endif
		if(B_SerialSelfCommandNum > 6)
			break;
	}
	#endif
}

void Serial5CommandHandler(void)
{
	#if(SERIAL5_USED == 1)
	unsigned char B_Temp = 0, B_SerialSelfCommandNum = 0, ret = 0;
	while(IsEmptyQueue(&gSqu_Serial5ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial5ReceiveBuf);
		ret = ParseSerialCommadBySelfType(COMMAD_FROM_SERIAL5, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		ret = ParseSerialBinaryCommadBySelfType(COMMAD_FROM_SERIAL5, B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		#if 0
			ret = ParseLuanZongBinaryCommadPacket(COMMAD_FROM_SERIAL5, B_Temp);
			if(ret == 1)
				B_SerialSelfCommandNum++;
		#endif
		if(B_SerialSelfCommandNum > 6)
			break;
	}
	#endif
}

void SerialCommadClear(void)
{
	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
	gB_SerialCommadIndex = 0;
}

void Serial4CommadClear(void)
{
	#if(SERIAL4_USED == 1)
	memset(gB_Serial4CommadBuf, 0, sizeof(gB_Serial4CommadBuf));
	gB_Serial4CommadIndex = 0;
	#endif
}

void Serial5CommadClear(void)
{
	#if(SERIAL5_USED == 1)
	memset(gB_Serial5CommadBuf, 0, sizeof(gB_Serial5CommadBuf));
	gB_Serial5CommadIndex = 0;
	#endif
}
unsigned char	gB_SerialBinaryNewDataFlag = 0;
unsigned short	gW_SerialBinaryNewData = 0;
unsigned char	gB_SerialBinaryCommadFlag = 0;
unsigned short	gW_SerialBinaryCommadLen = 0;
unsigned short	gW_SerialBinaryCommadCheckSum = 0;
unsigned short	gW_SerialBinaryCommadIndex = 0;
unsigned short	gW_SerialBinaryCommadBuf[SERIAL_COMMAD_BUF_LENGTH];

#if(SERIAL4_USED == 1)
unsigned char	gB_Serial4BinaryNewDataFlag = 0;
unsigned short	gW_Serial4BinaryNewData = 0;
unsigned char	gB_Serial4BinaryCommadFlag = 0;
unsigned short	gW_Serial4BinaryCommadLen = 0;
unsigned short	gW_Serial4BinaryCommadCheckSum = 0;
unsigned short	gW_Serial4BinaryCommadIndex = 0;
unsigned short	gW_Serial4BinaryCommadBuf[SERIAL_COMMAD_BUF_LENGTH];
#endif

#if(SERIAL5_USED == 1)
unsigned char	gB_Serial5BinaryNewDataFlag = 0;
unsigned short	gW_Serial5BinaryNewData = 0;
unsigned char	gB_Serial5BinaryCommadFlag = 0;
unsigned short	gW_Serial5BinaryCommadLen = 0;
unsigned short	gW_Serial5BinaryCommadCheckSum = 0;
unsigned short	gW_Serial5BinaryCommadIndex = 0;
unsigned short	gW_Serial5BinaryCommadBuf[SERIAL_COMMAD_BUF_LENGTH];
#endif

unsigned char ParseSerialBinaryCommadBySelfType(uint8_t B_CommandSource, unsigned char B_Data)
{
	if(B_CommandSource == COMMAD_FROM_SERIAL1)
	{
		if(gB_SerialBinaryNewDataFlag == 0)
		{
			gW_SerialBinaryNewData = B_Data;
			if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
			{
				gB_SerialBinaryNewDataFlag = 1;
				return 0;
			}
		}
		else if(gB_SerialBinaryNewDataFlag == 1)
		{
			if(B_Data == (unsigned char) (BINARY_COMMAD_TAG >> 8))
			{
				gB_SerialBinaryNewDataFlag = 2;
				if(gB_SerialBinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START)
				{
					gW_SerialBinaryCommadLen = 0;
					gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
					gW_SerialBinaryCommadCheckSum = 0;
					gW_SerialBinaryCommadIndex = 0;
					gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = BINARY_COMMAD_TAG;
					gW_SerialBinaryCommadIndex++;
					return 0;
				}
			}
			else if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
				gB_SerialBinaryNewDataFlag = 1;
			else
				gB_SerialBinaryNewDataFlag = 0;
			return 0;
		}
		else if(gB_SerialBinaryNewDataFlag == 2)
		{
			gW_SerialBinaryNewData = B_Data;
			gB_SerialBinaryNewDataFlag = 3;
			return 0;
		}
		else
		{
			gW_SerialBinaryNewData += (B_Data << 8);
			gB_SerialBinaryNewDataFlag = 2;
			if(gW_SerialBinaryCommadIndex >= (sizeof(gW_SerialBinaryCommadBuf) / sizeof(unsigned short)))
			{
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_SerialBinaryCommadIndex = 0;
				memset(gW_SerialBinaryCommadBuf, 0, sizeof(gW_SerialBinaryCommadBuf));
				gB_SerialBinaryNewDataFlag = 0;
				return 0;
			}
	//		printf("%04X ", gW_SerialBinaryNewData);
			if((gW_SerialBinaryNewData == BINARY_COMMAD_TAG) && (gB_SerialBinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START))
			{
				gW_SerialBinaryCommadLen = 0;
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
				gW_SerialBinaryCommadCheckSum = 0;
				gW_SerialBinaryCommadIndex = 0;
				gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = gW_SerialBinaryNewData;
				gW_SerialBinaryCommadIndex++;
				return 0;
			}
			if(gB_SerialBinaryCommadFlag == BINARY_COMMAD_FLAG_REV_TAG)
			{
				gW_SerialBinaryCommadLen = gW_SerialBinaryNewData;
				if(gW_SerialBinaryCommadLen > (sizeof(gW_SerialBinaryCommadBuf) / sizeof(unsigned short)))
					gW_SerialBinaryCommadLen = (sizeof(gW_SerialBinaryCommadBuf) / sizeof(unsigned short));
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_LEN;
				gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = gW_SerialBinaryNewData;
				gW_SerialBinaryCommadIndex++;
				gW_SerialBinaryCommadCheckSum += gW_SerialBinaryNewData;
				return 0;
			}
			if(gB_SerialBinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_LEN)
			{
				gW_SerialBinaryCommadLen--;
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_TEXT;
				gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = gW_SerialBinaryNewData;
				gW_SerialBinaryCommadIndex++;
				gW_SerialBinaryCommadCheckSum += gW_SerialBinaryNewData;
				return 0;
			}
			if((gW_SerialBinaryCommadLen > 0) &&(gB_SerialBinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_TEXT))
			{
				gW_SerialBinaryCommadLen--;
				gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = gW_SerialBinaryNewData;
				gW_SerialBinaryCommadIndex++;
				gW_SerialBinaryCommadCheckSum += gW_SerialBinaryNewData;
				return 0;
			}
			if(gW_SerialBinaryCommadLen == 0)
			{
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_CHECK_SUM;
				gW_SerialBinaryCommadBuf[gW_SerialBinaryCommadIndex] = gW_SerialBinaryNewData;
				gW_SerialBinaryCommadIndex++;
				gW_SerialBinaryCommadCheckSum += gW_SerialBinaryNewData;
				if(gW_SerialBinaryCommadCheckSum == 0)
				{
					ExcuteUserBinaryCommad(B_CommandSource, gW_SerialBinaryCommadBuf[2], gW_SerialBinaryCommadBuf[1] - 1, &gW_SerialBinaryCommadBuf[3]);
					gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
					gW_SerialBinaryCommadIndex = 0;
					memset(gW_SerialBinaryCommadBuf, 0, sizeof(gW_SerialBinaryCommadBuf));
					return 1;
				}
				gB_SerialBinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_SerialBinaryCommadIndex = 0;
				memset(gW_SerialBinaryCommadBuf, 0, sizeof(gW_SerialBinaryCommadBuf));
				return 0;
			}
		}
	}
	else if(B_CommandSource == COMMAD_FROM_SERIAL4)
	{
		#if(SERIAL4_USED == 1)
		if(gB_Serial4BinaryNewDataFlag == 0)
		{
			gW_Serial4BinaryNewData = B_Data;
			if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
			{
				gB_Serial4BinaryNewDataFlag = 1;
				return 0;
			}
		}
		else if(gB_Serial4BinaryNewDataFlag == 1)
		{
			if(B_Data == (unsigned char) (BINARY_COMMAD_TAG >> 8))
			{
				gB_Serial4BinaryNewDataFlag = 2;
				if(gB_Serial4BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START)
				{
					gW_Serial4BinaryCommadLen = 0;
					gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
					gW_Serial4BinaryCommadCheckSum = 0;
					gW_Serial4BinaryCommadIndex = 0;
					gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = BINARY_COMMAD_TAG;
					gW_Serial4BinaryCommadIndex++;
					return 0;
				}
			}
			else if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
				gB_Serial4BinaryNewDataFlag = 1;
			else
				gB_Serial4BinaryNewDataFlag = 0;
			return 0;
		}
		else if(gB_Serial4BinaryNewDataFlag == 2)
		{
			gW_Serial4BinaryNewData = B_Data;
			gB_Serial4BinaryNewDataFlag = 3;
			return 0;
		}
		else
		{
			gW_Serial4BinaryNewData += (B_Data << 8);
			gB_Serial4BinaryNewDataFlag = 2;
			if(gW_Serial4BinaryCommadIndex >= (sizeof(gW_Serial4BinaryCommadBuf) / sizeof(unsigned short)))
			{
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_Serial4BinaryCommadIndex = 0;
				memset(gW_Serial4BinaryCommadBuf, 0, sizeof(gW_Serial4BinaryCommadBuf));
				gB_Serial4BinaryNewDataFlag = 0;
				return 0;
			}
	//		printf("%04X ", gW_Serial4BinaryNewData);
			if((gW_Serial4BinaryNewData == BINARY_COMMAD_TAG) && (gB_Serial4BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START))
			{
				gW_Serial4BinaryCommadLen = 0;
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
				gW_Serial4BinaryCommadCheckSum = 0;
				gW_Serial4BinaryCommadIndex = 0;
				gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = gW_Serial4BinaryNewData;
				gW_Serial4BinaryCommadIndex++;
				return 0;
			}
			if(gB_Serial4BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_TAG)
			{
				gW_Serial4BinaryCommadLen = gW_Serial4BinaryNewData;
				if(gW_Serial4BinaryCommadLen > (sizeof(gW_Serial4BinaryCommadBuf) / sizeof(unsigned short)))
					gW_Serial4BinaryCommadLen = (sizeof(gW_Serial4BinaryCommadBuf) / sizeof(unsigned short));
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_LEN;
				gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = gW_Serial4BinaryNewData;
				gW_Serial4BinaryCommadIndex++;
				gW_Serial4BinaryCommadCheckSum += gW_Serial4BinaryNewData;
				return 0;
			}
			if(gB_Serial4BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_LEN)
			{
				gW_Serial4BinaryCommadLen--;
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_TEXT;
				gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = gW_Serial4BinaryNewData;
				gW_Serial4BinaryCommadIndex++;
				gW_Serial4BinaryCommadCheckSum += gW_Serial4BinaryNewData;
				return 0;
			}
			if((gW_Serial4BinaryCommadLen > 0) &&(gB_Serial4BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_TEXT))
			{
				gW_Serial4BinaryCommadLen--;
				gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = gW_Serial4BinaryNewData;
				gW_Serial4BinaryCommadIndex++;
				gW_Serial4BinaryCommadCheckSum += gW_Serial4BinaryNewData;
				return 0;
			}
			if(gW_Serial4BinaryCommadLen == 0)
			{
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_CHECK_SUM;
				gW_Serial4BinaryCommadBuf[gW_Serial4BinaryCommadIndex] = gW_Serial4BinaryNewData;
				gW_Serial4BinaryCommadIndex++;
				gW_Serial4BinaryCommadCheckSum += gW_Serial4BinaryNewData;
				if(gW_Serial4BinaryCommadCheckSum == 0)
				{
					ExcuteUserBinaryCommad(B_CommandSource, gW_Serial4BinaryCommadBuf[2], gW_Serial4BinaryCommadBuf[1] - 1, &gW_Serial4BinaryCommadBuf[3]);
					gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
					gW_Serial4BinaryCommadIndex = 0;
					memset(gW_Serial4BinaryCommadBuf, 0, sizeof(gW_Serial4BinaryCommadBuf));
					return 1;
				}
				gB_Serial4BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_Serial4BinaryCommadIndex = 0;
				memset(gW_Serial4BinaryCommadBuf, 0, sizeof(gW_Serial4BinaryCommadBuf));
				return 0;
			}
		}
		#endif
	}
	else if(B_CommandSource == COMMAD_FROM_SERIAL5)
	{
		#if(SERIAL5_USED == 1)
		if(gB_Serial5BinaryNewDataFlag == 0)
		{
			gW_Serial5BinaryNewData = B_Data;
			if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
			{
				gB_Serial5BinaryNewDataFlag = 1;
				return 0;
			}
		}
		else if(gB_Serial5BinaryNewDataFlag == 1)
		{
			if(B_Data == (unsigned char) (BINARY_COMMAD_TAG >> 8))
			{
				gB_Serial5BinaryNewDataFlag = 2;
				if(gB_Serial5BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START)
				{
					gW_Serial5BinaryCommadLen = 0;
					gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
					gW_Serial5BinaryCommadCheckSum = 0;
					gW_Serial5BinaryCommadIndex = 0;
					gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = BINARY_COMMAD_TAG;
					gW_Serial5BinaryCommadIndex++;
					return 0;
				}
			}
			else if(B_Data == (unsigned char) BINARY_COMMAD_TAG)
				gB_Serial5BinaryNewDataFlag = 1;
			else
				gB_Serial5BinaryNewDataFlag = 0;
			return 0;
		}
		else if(gB_Serial5BinaryNewDataFlag == 2)
		{
			gW_Serial5BinaryNewData = B_Data;
			gB_Serial5BinaryNewDataFlag = 3;
			return 0;
		}
		else
		{
			gW_Serial5BinaryNewData += (B_Data << 8);
			gB_Serial5BinaryNewDataFlag = 2;
			if(gW_Serial5BinaryCommadIndex >= (sizeof(gW_Serial5BinaryCommadBuf) / sizeof(unsigned short)))
			{
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_Serial5BinaryCommadIndex = 0;
				memset(gW_Serial5BinaryCommadBuf, 0, sizeof(gW_Serial5BinaryCommadBuf));
				gB_Serial5BinaryNewDataFlag = 0;
				return 0;
			}
	//		printf("%04X ", gW_Serial5BinaryNewData);
			if((gW_Serial5BinaryNewData == BINARY_COMMAD_TAG) && (gB_Serial5BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_START))
			{
				gW_Serial5BinaryCommadLen = 0;
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_TAG;
				gW_Serial5BinaryCommadCheckSum = 0;
				gW_Serial5BinaryCommadIndex = 0;
				gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = gW_Serial5BinaryNewData;
				gW_Serial5BinaryCommadIndex++;
				return 0;
			}
			if(gB_Serial5BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_TAG)
			{
				gW_Serial5BinaryCommadLen = gW_Serial5BinaryNewData;
				if(gW_Serial5BinaryCommadLen > (sizeof(gW_Serial5BinaryCommadBuf) / sizeof(unsigned short)))
					gW_Serial5BinaryCommadLen = (sizeof(gW_Serial5BinaryCommadBuf) / sizeof(unsigned short));
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_LEN;
				gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = gW_Serial5BinaryNewData;
				gW_Serial5BinaryCommadIndex++;
				gW_Serial5BinaryCommadCheckSum += gW_Serial5BinaryNewData;
				return 0;
			}
			if(gB_Serial5BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_LEN)
			{
				gW_Serial5BinaryCommadLen--;
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_TEXT;
				gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = gW_Serial5BinaryNewData;
				gW_Serial5BinaryCommadIndex++;
				gW_Serial5BinaryCommadCheckSum += gW_Serial5BinaryNewData;
				return 0;
			}
			if((gW_Serial5BinaryCommadLen > 0) &&(gB_Serial5BinaryCommadFlag == BINARY_COMMAD_FLAG_REV_DATA_TEXT))
			{
				gW_Serial5BinaryCommadLen--;
				gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = gW_Serial5BinaryNewData;
				gW_Serial5BinaryCommadIndex++;
				gW_Serial5BinaryCommadCheckSum += gW_Serial5BinaryNewData;
				return 0;
			}
			if(gW_Serial5BinaryCommadLen == 0)
			{
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_DATA_CHECK_SUM;
				gW_Serial5BinaryCommadBuf[gW_Serial5BinaryCommadIndex] = gW_Serial5BinaryNewData;
				gW_Serial5BinaryCommadIndex++;
				gW_Serial5BinaryCommadCheckSum += gW_Serial5BinaryNewData;
				if(gW_Serial5BinaryCommadCheckSum == 0)
				{
					ExcuteUserBinaryCommad(B_CommandSource, gW_Serial5BinaryCommadBuf[2], gW_Serial5BinaryCommadBuf[1] - 1, &gW_Serial5BinaryCommadBuf[3]);
					gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
					gW_Serial5BinaryCommadIndex = 0;
					memset(gW_Serial5BinaryCommadBuf, 0, sizeof(gW_Serial5BinaryCommadBuf));
					return 1;
				}
				gB_Serial5BinaryCommadFlag = BINARY_COMMAD_FLAG_REV_START;
				gW_Serial5BinaryCommadIndex = 0;
				memset(gW_Serial5BinaryCommadBuf, 0, sizeof(gW_Serial5BinaryCommadBuf));
				return 0;
			}
		}
		#endif
	}
	else
		return 0;
	return 0;
}

unsigned char ParseSerialCommadBySelfType(uint8_t B_CommandSource, unsigned char B_Data)
{
	unsigned char B_Temp = NULL;
	B_Temp = B_Data;
	if(B_CommandSource == COMMAD_FROM_SERIAL1)
	{
		if(gB_SerialCommadIndex >= SERIAL_COMMAD_BUF_LENGTH)
		{
			memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
			gB_SerialCommadIndex = 0;
			#if(SERIAL_COMMAD_DEBUG == 1)
			printf("Serial Command Length too long!\r\n");
			#endif
		}
		gB_SerialCommadBuf[gB_SerialCommadIndex] = B_Temp;
		gB_SerialCommadIndex++;
		if(B_Temp == '\n')
		{
			#if(SERIAL_COMMAD_DEBUG == 2)
			printf("SERIAL REV: %s\r\n", gB_SerialCommadBuf);
			#endif
			// ParseSerialCommad(gB_SerialCommadBuf);
			ParseCommad(B_CommandSource, gB_SerialCommadBuf);
			SerialCommadClear();
			return 1;
		}
	}
	else if(B_CommandSource == COMMAD_FROM_SERIAL4)
	{
		#if(SERIAL4_USED == 1)
		if(gB_Serial4CommadIndex >= SERIAL_COMMAD_BUF_LENGTH)
		{
			memset(gB_Serial4CommadBuf, 0, sizeof(gB_Serial4CommadBuf));
			gB_Serial4CommadIndex = 0;
			#if(SERIAL_COMMAD_DEBUG == 1)
			printf("Serial Command Length too long!\r\n");
			#endif
		}
		gB_Serial4CommadBuf[gB_Serial4CommadIndex] = B_Temp;
		gB_Serial4CommadIndex++;
		if(B_Temp == '\n')
		{
			#if(SERIAL_COMMAD_DEBUG == 2)
			printf("SERIAL REV: %s\r\n", gB_SerialCommadBuf);
			#endif
			// ParseSerialCommad(gB_SerialCommadBuf);
			ParseCommad(B_CommandSource, gB_Serial4CommadBuf);
			Serial4CommadClear();
			return 1;
		}
		#endif
	}
	else if(B_CommandSource == COMMAD_FROM_SERIAL5)
	{
		#if(SERIAL5_USED == 1)
		if(gB_Serial5CommadIndex >= SERIAL_COMMAD_BUF_LENGTH)
		{
			memset(gB_Serial5CommadBuf, 0, sizeof(gB_Serial5CommadBuf));
			gB_Serial5CommadIndex = 0;
			#if(SERIAL_COMMAD_DEBUG == 1)
			printf("Serial Command Length too long!\r\n");
			#endif
		}
		gB_Serial5CommadBuf[gB_Serial5CommadIndex] = B_Temp;
		gB_Serial5CommadIndex++;
		if(B_Temp == '\n')
		{
			#if(SERIAL_COMMAD_DEBUG == 2)
			printf("SERIAL REV: %s\r\n", gB_SerialCommadBuf);
			#endif
			// ParseSerialCommad(gB_SerialCommadBuf);
			ParseCommad(B_CommandSource, gB_Serial5CommadBuf);
			Serial5CommadClear();
			return 1;
		}
		#endif
	}
	else
		return 0;
	return 0;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	#if 1
	unsigned char flag = 0;
	if(UartHandle->Init.WordLength == UART_WORDLENGTH_9B)
	{
		if(UartHandle->Instance == USART1)
		{
			flag = EnterSerialQueue(&gSqu_Serial1ReceiveBuf, gB_Serial1RxData[0]);
			flag = EnterSerialQueue(&gSqu_Serial1ReceiveBuf, gB_Serial1RxData[1]);
		}
		if(UartHandle->Instance == UART5)
		{
			flag = EnterSerialQueue(&gSqu_Serial5ReceiveBuf, gB_Serial5RxData[0]);
			flag = EnterSerialQueue(&gSqu_Serial5ReceiveBuf, gB_Serial5RxData[1]);
		}
	}
	else
	{
		if(UartHandle->Instance == USART1)
			flag = EnterSerialQueue(&gSqu_Serial1ReceiveBuf, gB_Serial1RxData[0]);
		if(UartHandle->Instance == UART5)
			flag = EnterSerialQueue(&gSqu_Serial5ReceiveBuf, gB_Serial5RxData[0]);
	}
	if(flag == 0)
	{
		#if 0
		if(UartHandle->Instance == USART1)
			printf("Serial1 Rcvbuf full\r\n");
		if(UartHandle->Instance == USART5)
			printf("Serial5 Rcvbuf full\r\n");
		#endif
	}
	// Enable uart RX
	if(UartHandle->Instance == USART1)
		HAL_UART_Receive_IT(&huart1, gB_Serial1RxData, 1);
	if(UartHandle->Instance == UART5)
		HAL_UART_Receive_IT(&huart5, gB_Serial5RxData, 1);
	#endif
}

#if 1
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
	#if(DEBUG_SERIAL_INDEX == 1)
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
	(&huart1)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_TC);
	#endif
	#if(DEBUG_SERIAL_INDEX == 2)
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC);
	(&huart2)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);
	#endif
	#if(DEBUG_SERIAL_INDEX == 4)
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
	(&huart4)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_TC);
	#endif
	#if(DEBUG_SERIAL_INDEX == 5)
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC);
	(&huart5)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_TC);
	#endif

  return ch;
}
#endif
// implementation of putchar (also used by printf function to output data)
int sendchar1 (int ch)				// Write character to Serial Port
{
	/* Place your implementation of fputc here */
  /* e.g. write a character to the USART and Loop until the end of transmission */
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
	(&huart1)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_TC);
  return ch;
}

unsigned char Serial1_SendData(unsigned char *Bp_Buf, int W_Len)
{
	int i = 0;
	if(W_Len <= 0)
		return (unsigned char) 0xFFFF;
	for(i = 0; i < W_Len; i++, Bp_Buf++)
		sendchar1(*Bp_Buf);
	return (unsigned char) 0;
}

unsigned char Serial4_SendData(unsigned char *Bp_Buf, int W_Len)
{
	uint32_t tmp = 0, i = 0;
	if(W_Len <= 0)
		return (unsigned char) 0xFFFF;
	for(i = 0; i < W_Len; i++, Bp_Buf++)
	{
		tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
		(&huart4)->Instance->DR = ((*Bp_Buf) & (uint8_t)0xFF);
		tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
		while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
		{
			tmp = __HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC);
			i++;
		}
		__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_TC);
	}
	return (unsigned char) 0;
}

int sendchar5 (int ch)				// Write character to Serial Port
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
	uint32_t tmp = 0, i = 0;
	tmp = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC);
	(&huart5)->Instance->DR = (ch & (uint8_t)0xFF);
	tmp = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC);
	while((tmp == RESET) && (i < SERIAL_SEND_CHECK_TIMES))
	{
		tmp = __HAL_UART_GET_FLAG(&huart5, UART_IT_TC);
		i++;
	}
	__HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_TC);
  return ch;
}

unsigned char Serial5_SendData(unsigned char *Bp_Buf, int W_Len)
{
	int i = 0;
	if(W_Len <= 0)
		return (unsigned char) 0xFFFF;
	for(i = 0; i < W_Len; i++, Bp_Buf++)
		sendchar5 (*Bp_Buf);
	return (unsigned char) 0;
}

unsigned char EnterSerialQueue(Squeue* Queue, unsigned char B_Data)
{
	Queue->lock = 1;
	if(Queue->Front == ((Queue->Rear + 1) % Serial_RxBufLen))
	{
		Queue->lock = 0;
		return 0;
	}
	else
	{
		Queue->Rear = (Queue->Rear + 1) % Serial_RxBufLen;
		Queue->Data[Queue->Rear] = B_Data;
		Queue->lock = 0;
		return 1;
	}
}

static void ClearQueue(Squeue* Queue)
{
	Queue->lock = 1;
	Queue->Front = 0;
	Queue->Rear = 0;
	Queue->lock = 0;
}

static unsigned char IsEmptyQueue(Squeue* Queue)
{
	if(	Queue->lock == 1)
		return 1;
	if (Queue->Front == Queue->Rear)
		return 1;
	else
		return 0;
}

static unsigned char EnterQueue(Squeue* Queue, unsigned char B_Data)
{
	Queue->lock = 1;
	if(Queue->Front == ((Queue->Rear + 1) % Serial_RxBufLen))
	{
		Queue->lock = 0;
		return 0;
	}
	else
	{
		Queue->Rear = (Queue->Rear + 1) % Serial_RxBufLen;
		Queue->Data[Queue->Rear] = B_Data;
		Queue->lock = 0;
		return 1;
	}
}

static unsigned char GetQueue(Squeue* Queue)
{
	if(	Queue->lock == 1)
		return NULL;
	if(IsEmptyQueue(Queue))
		return NULL;
	else
	{
		Queue->Front = (Queue->Front + 1) % Serial_RxBufLen;
		return Queue->Data[Queue->Front];
	}
}

static unsigned char LengthQueue(Squeue* Queue)
{
	int i = (Queue->Rear - Queue->Front + Serial_RxBufLen) % Serial_RxBufLen;
	return i;
}

static unsigned char IsNumeral(char ch)
{
	if(ch >= 0x30 && ch <= 0x39)
		return 1;
	else
		return 0;
}


/****************************************************************************/
/*																			*/
/* FUNCTION:    Serial_DummyCall(void)                                      */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*							being called.												*/
/*																			*/
/****************************************************************************/
void Serial_DummyCall(void)
{
	EnterQueue(NULL, 0);
	LengthQueue(NULL);
	GetQueue(NULL);
	sendchar1(0);
	sendchar5(0);
	IsNumeral(0);
	ParseSerialBinaryCommadBySelfType(0, 0);
	ParseSerialCommadBySelfType(0, 0);
	memset(gW_StaticSerialSendData, 0, sizeof(gW_StaticSerialSendData));
}
/********************************  END  *************************************/
