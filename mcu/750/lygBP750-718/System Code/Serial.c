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
/*
#include "..\TMC Board\all.h"
#include "param.h"
#include "ASCII.h"
#include "../Device/EEprom/AT24C256.h"
#include "../Device/EEprom/AT24C02.h"
*/
#include <string.h>
#include <stdio.h>

#include "lpc17xx.h"
#include "ASCII.h"
#include "global.h"
#include "type.h"
#include "Timer.h"
#include "Serial.h"

#include "../User Command/UserCommandPC_API.h"
/****************************************************************************/
/*	D E F I N I T I O N														*/
/****************************************************************************/
#define 		SERIAL_COMMAD_DEBUG			0

//#define CR     0x0D
#define SERIAL_COMMAD_BUF_LENGTH     128

#define UART0_BPS     9600                                      		// 串口0通信波特率
#define UART1_BPS     9600                                       		// 串口1通信波特率  printf
#define UART3_BPS     9600                                      		// 串口3通信波特率
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

/*

static const char* BoardDevice[] =
{
	"DVI0_0",	"DVI0_1",	"DVI0_2",	"DVI0_3",	"DVI0_4",	"DVI0_5",	"DVI0_6",
	"DVI1_0",	"DVI1_1",	"DVI1_2",	"DVI1_3",	"DVI1_4",	"DVI1_5",	"DVI1_6",
	"DVI2_0",	"DVI2_1",	"DVI2_2",	"DVI2_3",	"DVI2_4",	"DVI2_5",	"DVI2_6",
	"DVI3_0",	"DVI3_1",	"DVI3_2",	"DVI3_3",	"DVI3_4",	"DVI3_5",	"DVI3_6",
	"DVI4_0",	"DVI4_1",	"DVI4_2",	"DVI4_3",	"DVI4_4",	"DVI4_5",	"DVI4_6",
	"EEROM0",	"EEROM1",	"EEROM2",	"EEROM3",	NULL
};
*/

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
BYTE gB_SerialMachineAddress = 0;

BYTE	gB_SerialCommadIndex=0;
BYTE	gB_SerialCommadBuf[SERIAL_COMMAD_BUF_LENGTH];

volatile uint32_t UART0Status = 0, UART0TxEmpty = 1;
volatile uint32_t UART1Status = 0, UART1TxEmpty = 1;
volatile uint32_t UART3Status = 0, UART3TxEmpty = 1;

Squeue	gSqu_Serial0ReceiveBuf;
Squeue	gSqu_Serial1ReceiveBuf;
Squeue	gSqu_Serial3ReceiveBuf;

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
#if(SERIAL_COMMAD_TYPE == 1)
static BYTE gB_PacketStartFlag = 0;
static BYTE gB_PacketRecvFlag = 0;
#endif

static BYTE gB_SerialBrillViewType1PacketStartFlag = 0;
static BYTE gB_SerialBrillViewType1PacketRecvFlag = 0;
static BYTE	gB_SerialBrillViewType1CommadIndex = 0;

#define SERIAL_BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH		64
static BYTE	gB_SerialBrillViewType1CommadBuf[SERIAL_BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH];

static char SerialDevStr[40] = {0}, SerialOperationStr[40] = {0};
static WORD W_SerialDataArry[SERIAL_COMMAD_BUF_LENGTH] = {0};
BYTE gB_KeyboardData = 0;

/****************************************************************************/
/*  G L O B A L   F U N C T I O N												                    */
/****************************************************************************/
void Serial0_Init(void);
void UART0_IRQHandler(void);				//Serial0 interrupt routine
void Serial1_Init(void);
void UART1_IRQHandler(void);				//Serial1 interrupt routine
void Serial3_Init(void);
void UART3_IRQHandler(void);				//Serial3 interrupt routine
BYTE* SerialHandler(void);
void SerialCommandHandler(void);
void SerialCommadClear(void);
BYTE Serial_SendData(BYTE *Bp_Buf, int W_Len);
BYTE Serial3_SendData(BYTE *Bp_Buf, int W_Len);
BYTE EnterSerialQueue(Squeue* Queue, BYTE B_Data);
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static BYTE ParseSerialCommadBySelfType(BYTE B_Data);
static BYTE ParseSerialCommadByBrillViewType1(BYTE B_Data);
static BYTE SerialChangeBrillViewType1CommadToSelfCommad(BYTE* pCommad);
static BYTE ParseKeyboardCommad(BYTE B_Data);

static void ClearQueue(Squeue* Queue);
static BYTE IsEmptyQueue(Squeue* Queue);
static BYTE EnterQueue(Squeue* Queue, BYTE B_Data);
static BYTE GetQueue(Squeue* Queue);
static BYTE LengthQueue(Squeue* Queue);
//static void ParseSerialCommad(BYTE* pCommad);
//static void ExcuteSerialCommad(char* pDevice, char* pOperation, BYTE DataNum, BYTE* pData);
static int sendchar (int ch);

/****************************************************************************/
/*																			*/
/* FUNCTION:	void serial0_Init											*/
/*																			*/
/* USAGE:		Initialize the serial1 inerface.							*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void Serial0_Init(void)
{
	uint16_t Fdiv;
  uint32_t pclkdiv, pclk;
	
	// By default, the PCLKSELx value is zero, thus, the PCLK for
	// all the peripherals is 1/4 of the SystemFrequency. 
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |=  0x00000050;  // RxD0 is P0.3 and TxD0 is P0.2
  LPC_UART0->LCR = 0x83;		// 8 bits, no Parity, 1 Stop bit
	Fdiv = ( pclk / 16 ) / UART0_BPS ;	//baud rate
	LPC_UART0->DLM = Fdiv / 256;							
  LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		// DLAB = 0
  LPC_UART0->FCR = 0x07;		// Enable and reset TX and RX FIFO.

	ClearQueue(&gSqu_Serial0ReceiveBuf);
	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
	gB_SerialCommadIndex = 0;

	NVIC_EnableIRQ(UART0_IRQn);
	LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART0 interrupt
	printf("Serial0 init OK!\r\n");
}

/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:	None
** 
*****************************************************************************/
void UART0_IRQHandler (void) 
{
  BYTE IIRValue = 0, LSRValue = 0, B_Data = 0;
  BYTE Dummy = Dummy;
	
  IIRValue = LPC_UART0->IIR;
  IIRValue >>= 1;											// skip pending bit in IIR
  IIRValue &= 0x07;										// check bit 1~3, interrupt identification
  if(IIRValue == IIR_RLS)					// Receive Line Status
  {
		LSRValue = LPC_UART0->LSR;
		// Receive Line Status
		if( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			// There are errors or break interrupt
			// Read LSR will clear the interrupt
			UART0Status = LSRValue;
			Dummy = LPC_UART0->RBR;		// Dummy read on RX to clear interrupt, then bail out
			return;
		}
		if( LSRValue & LSR_RDR )		// Receive Data Ready
		{
			// If no error on RLS, normal ready, save into the data buffer.
			// Note: read RBR will clear the interrupt
			B_Data = LPC_UART0->RBR;
			if(gSqu_Serial0ReceiveBuf.Front != ((gSqu_Serial0ReceiveBuf.Rear + 1) % Serial_RxBufLen))
			{
				gSqu_Serial0ReceiveBuf.Rear = (gSqu_Serial0ReceiveBuf.Rear + 1) % Serial_RxBufLen;
				gSqu_Serial0ReceiveBuf.Data[gSqu_Serial0ReceiveBuf.Rear] = B_Data;
			}
		}
  }
  else if ( IIRValue == IIR_RDA )	// Receive Data Available
  {
		// Receive Data Available
		B_Data = LPC_UART0->RBR;
		if(gSqu_Serial0ReceiveBuf.Front != ((gSqu_Serial0ReceiveBuf.Rear + 1) % Serial_RxBufLen))
		{
			gSqu_Serial0ReceiveBuf.Rear = (gSqu_Serial0ReceiveBuf.Rear + 1) % Serial_RxBufLen;
			gSqu_Serial0ReceiveBuf.Data[gSqu_Serial0ReceiveBuf.Rear] = B_Data;
		}
//		sendchar(B_Data);
//		printf("Test3\r\n");
  }
  else if ( IIRValue == IIR_CTI )		// Character timeout indicator
  {
		// Character Time-out indicator
		UART0Status |= 0x100;					// Bit 9 as the CTI error
  }
  else if ( IIRValue == IIR_THRE )	// THRE, transmit holding register empty
  {
		// THRE interrupt
		LSRValue = LPC_UART0->LSR;		// Check status in the LSR to see if valid data in U0THR or not
		if ( LSRValue & LSR_THRE )
		{
			UART0TxEmpty = 1;
		}
		else
		{
			UART0TxEmpty = 0;
		}
  }
}

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
	uint16_t Fdiv;
  uint32_t pclkdiv, pclk;

	// By default, the PCLKSELx value is zero, thus, the PCLK for
	// all the peripherals is 1/4 of the SystemFrequency. 
	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}
	
	LPC_PINCON->PINSEL4 &= ~0x0000000F;
	LPC_PINCON->PINSEL4 |=  0x0000000A;				// Enable RxD1 P2.1, TxD1 P2.0
	LPC_UART1->LCR = 0x83;										// 8 bits, no Parity, 1 Stop bit
	Fdiv = (pclk / 16) / UART1_BPS ;				// baud rate
	LPC_UART1->DLM = Fdiv / 256;							
	LPC_UART1->DLL = Fdiv % 256;
	LPC_UART1->LCR = 0x03;										// DLAB = 0
	LPC_UART1->FCR = 0x07;										// Enable and reset TX and RX FIFO

	ClearQueue(&gSqu_Serial1ReceiveBuf);
//	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
//	gB_SerialCommadIndex = 0;

	NVIC_EnableIRQ(UART1_IRQn);
	LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART1 interrupt
	printf("Serial1 init OK!\r\n");
}

/*****************************************************************************
** Function name:		UART1_IRQHandler
**
** Descriptions:		UART1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART1_IRQHandler (void) 
{
  BYTE IIRValue = 0, LSRValue = 0, B_Data = 0;
  BYTE Dummy = Dummy;
	
  IIRValue = LPC_UART1->IIR;
    
  IIRValue >>= 1;											// skip pending bit in IIR
  IIRValue &= 0x07;										// check bit 1~3, interrupt identification
  if(IIRValue == IIR_RLS)					// Receive Line Status
  {
		LSRValue = LPC_UART1->LSR;
		// Receive Line Status
		if( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			// There are errors or break interrupt
			// Read LSR will clear the interrupt
			UART1Status = LSRValue;
			Dummy = LPC_UART1->RBR;		// Dummy read on RX to clear interrupt, then bail out
			return;
		}
		if( LSRValue & LSR_RDR )		// Receive Data Ready
		{
			// If no error on RLS, normal ready, save into the data buffer.
			// Note: read RBR will clear the interrupt
			B_Data = LPC_UART1->RBR;
			if(gSqu_Serial1ReceiveBuf.Front != ((gSqu_Serial1ReceiveBuf.Rear + 1) % Serial_RxBufLen))
			{
				gSqu_Serial1ReceiveBuf.Rear = (gSqu_Serial1ReceiveBuf.Rear + 1) % Serial_RxBufLen;
				gSqu_Serial1ReceiveBuf.Data[gSqu_Serial1ReceiveBuf.Rear] = B_Data;
			}
		}
  }
  else if ( IIRValue == IIR_RDA )	// Receive Data Available
  {
		// Receive Data Available
		B_Data = LPC_UART1->RBR;
		if(gSqu_Serial1ReceiveBuf.Front != ((gSqu_Serial1ReceiveBuf.Rear + 1) % Serial_RxBufLen))
		{
			gSqu_Serial1ReceiveBuf.Rear = (gSqu_Serial1ReceiveBuf.Rear + 1) % Serial_RxBufLen;
			gSqu_Serial1ReceiveBuf.Data[gSqu_Serial1ReceiveBuf.Rear] = B_Data;
		}
  }
  else if ( IIRValue == IIR_CTI )		// Character timeout indicator
  {
		// Character Time-out indicator
		UART1Status |= 0x100;					// Bit 9 as the CTI error
  }
  else if ( IIRValue == IIR_THRE )	// THRE, transmit holding register empty
  {
		// THRE interrupt
		LSRValue = LPC_UART1->LSR;		// Check status in the LSR to see if valid data in U0THR or not
		if ( LSRValue & LSR_THRE )
		{
			UART1TxEmpty = 1;
		}
		else
		{
			UART1TxEmpty = 0;
		}
  }
}

/****************************************************************************/
/*																			*/
/* FUNCTION:	void serial3_Init											*/
/*																			*/
/* USAGE:		Initialize the serial3 inerface.							*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
void Serial3_Init(void)
{
	uint16_t Fdiv;
  uint32_t pclkdiv, pclk;
	
	LPC_SC->PCONP |= (1 << 25);						// 打开UART3功率控制 
	// By default, the PCLKSELx value is zero, thus, the PCLK for
	// all the peripherals is 1/4 of the SystemFrequency. 
	pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}
	
	LPC_PINCON->PINSEL0 &= ~((0x3<<0)|(0x3<<2));
	LPC_PINCON->PINSEL0 |= ((0x2<<0)|(0x2<<2));				// Enable RxD3 P2.1, TxD3 P2.0
	LPC_UART3->LCR = 0x83;										// 8 bits, no Parity, 1 Stop bit
	Fdiv = (pclk / 16) / UART3_BPS ;				// baud rate
	LPC_UART3->DLM = Fdiv / 256;							
	LPC_UART3->DLL = Fdiv % 256;
	LPC_UART3->LCR = 0x03;										// DLAB = 0
	LPC_UART3->FCR = 0x07;										// Enable and reset TX and RX FIFO

	ClearQueue(&gSqu_Serial3ReceiveBuf);
//	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
//	gB_SerialCommadIndex = 0;

	NVIC_EnableIRQ(UART3_IRQn);
	LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART3 interrupt
	printf("Serial3 init OK!\r\n");
}

/*****************************************************************************
** Function name:		UART3_IRQHandler
**
** Descriptions:		UART1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART3_IRQHandler (void) 
{
  BYTE IIRValue = 0, LSRValue = 0, B_Data = 0;
  BYTE Dummy = Dummy;
	
  IIRValue = LPC_UART3->IIR;
    
  IIRValue >>= 1;											// skip pending bit in IIR
  IIRValue &= 0x07;										// check bit 1~3, interrupt identification
  if(IIRValue == IIR_RLS)					// Receive Line Status
  {
		LSRValue = LPC_UART3->LSR;
		// Receive Line Status
		if( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			// There are errors or break interrupt
			// Read LSR will clear the interrupt
			UART3Status = LSRValue;
			Dummy = LPC_UART3->RBR;		// Dummy read on RX to clear interrupt, then bail out
			return;
		}
		if( LSRValue & LSR_RDR )		// Receive Data Ready
		{
			// If no error on RLS, normal ready, save into the data buffer.
			// Note: read RBR will clear the interrupt
			B_Data = LPC_UART3->RBR;
			if(gSqu_Serial3ReceiveBuf.Front != ((gSqu_Serial3ReceiveBuf.Rear + 1) % Serial_RxBufLen))
			{
				gSqu_Serial3ReceiveBuf.Rear = (gSqu_Serial3ReceiveBuf.Rear + 1) % Serial_RxBufLen;
				gSqu_Serial3ReceiveBuf.Data[gSqu_Serial3ReceiveBuf.Rear] = B_Data;
			}
		}
  }
  else if ( IIRValue == IIR_RDA )	// Receive Data Available
  {
		// Receive Data Available
		B_Data = LPC_UART3->RBR;
		if(gSqu_Serial3ReceiveBuf.Front != ((gSqu_Serial3ReceiveBuf.Rear + 1) % Serial_RxBufLen))
		{
			gSqu_Serial3ReceiveBuf.Rear = (gSqu_Serial3ReceiveBuf.Rear + 1) % Serial_RxBufLen;
			gSqu_Serial3ReceiveBuf.Data[gSqu_Serial3ReceiveBuf.Rear] = B_Data;
		}
  }
  else if ( IIRValue == IIR_CTI )		// Character timeout indicator
  {
		// Character Time-out indicator
		UART3Status |= 0x100;					// Bit 9 as the CTI error
  }
  else if ( IIRValue == IIR_THRE )	// THRE, transmit holding register empty
  {
		// THRE interrupt
		LSRValue = LPC_UART3->LSR;		// Check status in the LSR to see if valid data in U3THR or not
		if ( LSRValue & LSR_THRE )
		{
			UART3TxEmpty = 1;
		}
		else
		{
			UART3TxEmpty = 0;
		}
  }
}


// implementation of putchar (also used by printf function to output data)
int sendchar (int ch)				// Write character to Serial Port
{
/*
#if(DEBUG_OUT_UART_NUM == 1)
	while (!(LPC_UART0->LSR & 0x20)) ;
  return (LPC_UART0->THR = ch);
#else
	while (!(LPC_UART1->LSR & 0x20)) ;
  return (LPC_UART1->THR = ch);
#endif
*/
#if(DEBUG_OUT_UART_NUM == 1)
	while (!(LPC_UART1->LSR & 0x20)) ;
  return (LPC_UART1->THR = ch);
#else
	while (!(LPC_UART0->LSR & 0x20)) ;
  return (LPC_UART0->THR = ch);
#endif
}

BYTE Serial_SendData(BYTE *Bp_Buf, int W_Len)
{
	int i = 0;
	if(W_Len <= 0)
		return (BYTE) ERR;
	for(i = 0; i < W_Len; i++, Bp_Buf++)
		sendchar (*Bp_Buf);
	return (BYTE) OK;
}

int UART3SendChar (int ch)				// Write character to Serial Port
{
	while (!(LPC_UART3->LSR & 0x20)) ;
  return (LPC_UART3->THR = ch);
}

BYTE Serial3_SendData(BYTE *Bp_Buf, int W_Len)
{
	int i = 0;
	if(W_Len <= 0)
		return (BYTE) ERR;
	for(i = 0; i < W_Len; i++, Bp_Buf++)
		UART3SendChar (*Bp_Buf);
	return (BYTE) OK;
}
/****************************************************************************/
/*																			*/
/* FUNCTION:	BYTE* SerialHandler(void)									*/
/*																			*/
/* USAGE:		Check for any incoming debug packet from UART port. If the  */
/*				define for WIN_DEBUG is set, this function will be called   */
/* 				continuously in Main.c										*/
/*																			*/
/* INPUT:		None														*/
/*																			*/
/* OUTPUT:		None														*/
/*																			*/
/****************************************************************************/
#if(SERIAL_COMMAD_TYPE == 1)
BYTE* SerialHandler(void)
{
	BYTE B_Temp = NULL;
#if(DEBUG_OUT_UART_NUM == 1)
//	U1IER = 0x00;						// Disable RBR interrupt
	if(IsEmptyQueue(&gSqu_Serial0ReceiveBuf))
		return NULL;
	B_Temp = GetQueue(&gSqu_Serial0ReceiveBuf);
//	U1IER = 0x01;						// Enable RBR interrupt
#else
	if(IsEmptyQueue(&gSqu_Serial1ReceiveBuf))
		return NULL;
	B_Temp = GetQueue(&gSqu_Serial1ReceiveBuf);
#endif
	if(gB_SerialCommadIndex > SERIAL_COMMAD_BUF_LENGTH)
	{
		memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
		gB_SerialCommadIndex = 0;
		printf("Serial Command Length too long!\r\n");
	}
	if(B_Temp == BRILL_VIEW_PACKET_START)
	{
		gB_PacketStartFlag = 1;
		gB_PacketRecvFlag = 0;
		return NULL;
	}
	if((B_Temp == gB_SerialMachineAddress) && (gB_PacketStartFlag == 1))
	{
		gB_PacketRecvFlag = 1;
		gB_PacketStartFlag = 0;
		SerialCommadClear();
		gB_SerialCommadIndex = 0;
		gB_SerialCommadBuf[gB_SerialCommadIndex] = BRILL_VIEW_PACKET_START;
		gB_SerialCommadIndex++;
		gB_SerialCommadBuf[gB_SerialCommadIndex] = B_Temp;
		gB_SerialCommadIndex++;
		return NULL;
	}
	else
	{
		gB_PacketStartFlag = 0;
		if(gB_PacketRecvFlag == 0)
			gB_SerialCommadIndex = 0;
	}

	if(gB_PacketRecvFlag == 1)
	{
		gB_SerialCommadBuf[gB_SerialCommadIndex] = B_Temp;
		gB_SerialCommadIndex++;
		if((gB_SerialCommadBuf[3] == (gB_SerialCommadIndex - 4)) && (gB_SerialCommadBuf[3] > 1))
		{
			gB_PacketRecvFlag = 0;
			return gB_SerialCommadBuf;
		}
	}
	return NULL;
}
#else
BYTE* SerialHandler(void)
{
	BYTE B_Temp = NULL;
#if(DEBUG_OUT_UART_NUM == 1)
//	U1IER = 0x00;						// Disable RBR interrupt
	if(IsEmptyQueue(&gSqu_Serial0ReceiveBuf))
		return NULL;
	B_Temp = GetQueue(&gSqu_Serial0ReceiveBuf);
//	U1IER = 0x01;						// Enable RBR interrupt
#else
	if(IsEmptyQueue(&gSqu_Serial1ReceiveBuf))
		return NULL;
	B_Temp = GetQueue(&gSqu_Serial1ReceiveBuf);
#endif
	if((B_Temp >= ASCII_BLK) && (B_Temp < ASCII_DEL))
	{	//可显示字符（除了空格，因为不考虑一次输入多个参数，所以不需要空格） 
		if(gB_SerialCommadIndex > SERIAL_COMMAD_BUF_LENGTH)
		{	//超过缓冲区长度，后续的字符都忽略掉 
			return NULL;
		}
		else
		{
//		putchar(B_Temp);
//		sendchar(B_Temp);
		}
	}
	else if(B_Temp == ASCII_CR)
	{	//一个字符串输入结束 
		printf("\r\n");
	}
	else if(B_Temp == ASCII_BS)
	{	//输入退格键，撤销错误输入 
		if(gB_SerialCommadIndex > 0)
		{
			//tC = ASCII_BS;
		#if(SERIAL_COMMAD_DEBUG == 1)
			printf("%c %c",ASCII_BS,ASCII_BS);
		#endif
			gB_SerialCommadIndex--;
			return NULL;
		}
		else
		{	//没有字符可以删除 
			return NULL;
		}
	}
	else 
	{	//其它不可显示字符,忽略 
		return NULL;
	}
	gB_SerialCommadBuf[gB_SerialCommadIndex] = B_Temp;
	gB_SerialCommadIndex++;
	if(gB_SerialCommadIndex > SERIAL_COMMAD_BUF_LENGTH)
	{
		memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
		gB_SerialCommadIndex = 0;
		printf("Serial Command Length too long!\r\n");
	}
	if(B_Temp == '\n')
	{
	#if(SERIAL_COMMAD_DEBUG == 1)
		printf("SERIAL REV: %s\r\n", gB_SerialCommadBuf);
	#endif
		return gB_SerialCommadBuf;
	}
	return NULL;
}
#endif

void SerialCommadClear(void)
{
	memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
	gB_SerialCommadIndex = 0;
	#if(SERIAL_COMMAD_DEBUG == 1)
		printf("SerialCommadClear\r\n");
	#endif
}

void SerialCommandHandler(void)
{
	BYTE B_Temp = 0, B_SerialSelfCommandNum = 0, B_SerialBrillViewCommandNum = 0, ret = 0;
#if(DEBUG_OUT_UART_NUM == 1)
	/*
	while(IsEmptyQueue(&gSqu_Serial0ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial0ReceiveBuf);
	*/
	while(IsEmptyQueue(&gSqu_Serial1ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial1ReceiveBuf);
#else
	/*
	while(IsEmptyQueue(&gSqu_Serial1ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial1ReceiveBuf);
	*/
	while(IsEmptyQueue(&gSqu_Serial0ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial0ReceiveBuf);
#endif
		ret = ParseSerialCommadBySelfType(B_Temp);
		if(ret == 1)
			B_SerialSelfCommandNum++;
		ret = ParseSerialCommadByBrillViewType1(B_Temp);
		if(ret == 1)
			B_SerialBrillViewCommandNum++;
		if((B_SerialSelfCommandNum > 6) || (B_SerialBrillViewCommandNum > 6))
			break;
	}

	while(IsEmptyQueue(&gSqu_Serial3ReceiveBuf) == 0)
	{
		B_Temp = GetQueue(&gSqu_Serial3ReceiveBuf);
		ParseKeyboardCommad(B_Temp);
//		printf("S3 0x%02X\r\n", B_Temp);
	}

}

static BYTE ParseSerialCommadBySelfType(BYTE B_Data)
{
	BYTE B_Temp = NULL;
	B_Temp = B_Data;
	/*
	if((B_Temp >= ASCII_BLK) && (B_Temp < ASCII_DEL))
	{	//可显示字符（除了空格，因为不考虑一次输入多个参数，所以不需要空格） 
		if(gB_SerialCommadIndex > SERIAL_COMMAD_BUF_LENGTH)
		{	//超过缓冲区长度，后续的字符都忽略掉 
			return 0;
		}
		else
		{
//		putchar(B_Temp);
//		sendchar(B_Temp);
		}
	}
	else if(B_Temp == '\n')
	{	//一个字符串输入结束 
		printf("\r\n");
	}
	else if(B_Temp == ASCII_BS)
	{	//输入退格键，撤销错误输入 
		if(gB_SerialCommadIndex > 0)
		{
			//tC = ASCII_BS;
		#if(SERIAL_COMMAD_DEBUG == 1)
			printf("%c %c",ASCII_BS,ASCII_BS);
		#endif
			gB_SerialCommadIndex--;
			return 0;
		}
		else
		{	//没有字符可以删除 
			return 0;
		}
	}
	else 
	{	//其它不可显示字符,忽略 
		return 0;
	}
	*/
	gB_SerialCommadBuf[gB_SerialCommadIndex] = B_Temp;
	gB_SerialCommadIndex++;
	if(gB_SerialCommadIndex > SERIAL_COMMAD_BUF_LENGTH)
	{
		memset(gB_SerialCommadBuf, 0, sizeof(gB_SerialCommadBuf));
		gB_SerialCommadIndex = 0;
		printf("Serial Command Length too long!\r\n");
	}
	if(B_Temp == '\n')
	{
	#if(SERIAL_COMMAD_DEBUG == 1)
		printf("SERIAL REV: %s\r\n", gB_SerialCommadBuf);
	#endif
		ParseSerialCommad(gB_SerialCommadBuf);
		SerialCommadClear();
		return 1;
	}
	return 0;
}

static BYTE ParseSerialCommadByBrillViewType1(BYTE B_Data)
{
	BYTE B_Temp = NULL;
	B_Temp = B_Data;
	if(gB_SerialBrillViewType1CommadIndex > SERIAL_BRILL_VIEW_TYPE1_COMMAD_BUF_LENGTH)
	{
		memset(gB_SerialBrillViewType1CommadBuf, 0, sizeof(gB_SerialBrillViewType1CommadBuf));
		gB_SerialBrillViewType1CommadIndex = 0;
		printf("BV Serial Command Length too long!\r\n");
	}
	if(B_Temp == BRILL_VIEW_PACKET_START)
	{
		gB_SerialBrillViewType1PacketStartFlag = 1;
		gB_SerialBrillViewType1PacketRecvFlag = 0;
		return 0;
	}
	if((B_Temp == gB_SerialMachineAddress) && (gB_SerialBrillViewType1PacketStartFlag == 1))
	{
		gB_SerialBrillViewType1PacketRecvFlag = 1;
		gB_SerialBrillViewType1PacketStartFlag = 0;
		memset(gB_SerialBrillViewType1CommadBuf, 0, sizeof(gB_SerialBrillViewType1CommadBuf));
		gB_SerialBrillViewType1CommadIndex = 0;
		gB_SerialBrillViewType1CommadIndex = 0;
		gB_SerialBrillViewType1CommadBuf[gB_SerialBrillViewType1CommadIndex] = BRILL_VIEW_PACKET_START;
		gB_SerialBrillViewType1CommadIndex++;
		gB_SerialBrillViewType1CommadBuf[gB_SerialBrillViewType1CommadIndex] = B_Temp;
		gB_SerialBrillViewType1CommadIndex++;
		return 0;
	}
	else
	{
		gB_SerialBrillViewType1PacketStartFlag = 0;
		if(gB_SerialBrillViewType1PacketRecvFlag == 0)
			gB_SerialBrillViewType1CommadIndex = 0;
	}

	if(gB_SerialBrillViewType1PacketRecvFlag == 1)
	{
		gB_SerialBrillViewType1CommadBuf[gB_SerialBrillViewType1CommadIndex] = B_Temp;
		gB_SerialBrillViewType1CommadIndex++;
		if((gB_SerialBrillViewType1CommadBuf[3] == (gB_SerialBrillViewType1CommadIndex - 4)) && (gB_SerialBrillViewType1CommadBuf[3] >= 1))
		{
			gB_SerialBrillViewType1PacketRecvFlag = 0;
			SerialChangeBrillViewType1CommadToSelfCommad(gB_SerialBrillViewType1CommadBuf);
			memset(gB_SerialBrillViewType1CommadBuf, 0, sizeof(gB_SerialBrillViewType1CommadBuf));
			gB_SerialBrillViewType1CommadIndex = 0;
			return 1;
		}
	}
	return 0;
}

static BYTE ParseKeyboardCommad(BYTE B_Data)
{
	BYTE B_TempData = 0;
	if((gB_KeyboardData == B_Data) || (B_Data == 0xF5) || (B_Data == 0xF0))
		return 1;
//	printf("ParseKeyboardCommad: B_Data = %d, gB_KeyboardData = %d\r\n", B_Data, gB_KeyboardData);
	gB_KeyboardData = B_Data;
	switch (gB_KeyboardData)
	{
	  case 0x02:
	  case 0x03:
	  case 0x04:
	  case 0x05:
	  case 0x06:
	  case 0x07:
	  case 0x08:
		B_TempData = LoadUserKeyMode(gB_KeyboardData - 2);
		break;
	  case 0x12:
	  case 0x13:
	  case 0x14:
	  case 0x15:
	  case 0x16:
	  case 0x17:
	  case 0x18:
		B_TempData = LoadUserKeyMode(gB_KeyboardData - 11);
		break;
	  case 0x21:
	  case 0x22:
	  case 0x23:
		B_TempData = LoadUserKeyMode(gB_KeyboardData - 19);
		break;
	  case 0x25:
	  case 0x26:
	  case 0x27:
		B_TempData = LoadUserKeyMode(gB_KeyboardData - 20);
		break;
	  default:
		B_TempData = 0;
		break;
	}
	return B_TempData;
}

static BYTE SerialChangeBrillViewType1CommadToSelfCommad(BYTE* pCommad)
{
	char *StrHead = NULL, *pData0 = NULL;
	StrHead = (char*)pCommad;
	if(*StrHead != (char) BRILL_VIEW_PACKET_START)
		return 0;
	StrHead = (char*)pCommad + 1;
	if(*StrHead != gB_SerialMachineAddress)
		return 0;
	StrHead = (char*)pCommad + 2;
	if(*StrHead == 0x01)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SEL");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x20)
	{
		strcpy(SerialDevStr, "ADV7604_DEV0");
		strcpy(SerialOperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x21)
	{
		strcpy(SerialDevStr, "ADV7604_DEV1");
		strcpy(SerialOperationStr, "SET_ROUTING");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x22)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SET_OUTPUT_TIMING");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x23)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SET_IP_ADDRESS");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		pData0 = (char*) pCommad + 5;
		W_SerialDataArry[1] = *pData0;
		pData0 = (char*) pCommad + 6;
		W_SerialDataArry[2] = *pData0;
		pData0 = (char*) pCommad + 7;
		W_SerialDataArry[3] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 4, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x24)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SET_TCP_LOCAL_PORT");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		W_SerialDataArry[0] <<= 8;
		pData0 = (char*) pCommad + 5;
		W_SerialDataArry[0] += *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x25)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SET_GATEWAY_IP");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		pData0 = (char*) pCommad + 5;
		W_SerialDataArry[1] = *pData0;
		pData0 = (char*) pCommad + 6;
		W_SerialDataArry[2] = *pData0;
		pData0 = (char*) pCommad + 7;
		W_SerialDataArry[3] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 4, W_SerialDataArry);
		return 1;
	}
	if(*StrHead == 0x26)
	{
		strcpy(SerialDevStr, "SW");
		strcpy(SerialOperationStr, "SET_SERIAL_ADDRESS");
		pData0 = (char*) pCommad + 4;
		W_SerialDataArry[0] = *pData0;
		ExcuteUserCommad(0, SerialDevStr, SerialOperationStr, 1, W_SerialDataArry);
		return 1;
	}
	return 0;
}


/*
int getkey (void)
{                     			// Read character from Serial Port

  while (!(U1LSR & 0x01));

  return (U1RBR);
}
*/

BYTE EnterSerialQueue(Squeue* Queue, BYTE B_Data)
{
	if(Queue->Front == ((Queue->Rear + 1) % Serial_RxBufLen))
		return 0;
	else
	{
		Queue->Rear = (Queue->Rear + 1) % Serial_RxBufLen;
		Queue->Data[Queue->Rear] = B_Data;
		return 1;
	}
}

static void ClearQueue(Squeue* Queue)
{
	Queue->Front = 0;
	Queue->Rear = 0;
}

static BYTE IsEmptyQueue(Squeue* Queue)
{
	if (Queue->Front == Queue->Rear)
		return 1;
	else
		return 0;
}

static BYTE EnterQueue(Squeue* Queue, BYTE B_Data)
{
	if(Queue->Front == ((Queue->Rear + 1) % Serial_RxBufLen))
		return 0;
	else
	{
		Queue->Rear = (Queue->Rear + 1) % Serial_RxBufLen;
		Queue->Data[Queue->Rear] = B_Data;
		return 1;
	}
}

static BYTE GetQueue(Squeue* Queue)
{
	if(IsEmptyQueue(Queue))
		return NULL;
	else
	{
		Queue->Front = (Queue->Front + 1) % Serial_RxBufLen;
		return Queue->Data[Queue->Front];
	}
}

static BYTE LengthQueue(Squeue* Queue)
{
	int i = (Queue->Rear - Queue->Front + Serial_RxBufLen) % Serial_RxBufLen;
	return i;
}

static BYTE IsNumeral(char ch)
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
//	ParseSerialCommad(NULL);
	ParseKeyboardCommad(0);
	sendchar(0);
	IsNumeral(0);
}
/********************************  END  *************************************/
