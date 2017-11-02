/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: uart.h
 [Date]:        08-May-2012
 [Comment]:
   Uart header file.
 [Reversion History]:
*******************************************************************************/
#ifndef UART_H
#define UART_H

#include "common.h"

#ifdef _UART_C
  #define _UARTDEF_
#else
  #define _UARTDEF_ extern
#endif

//_UARTDEF_ U8 g_RevUart2Data[4];
//_UARTDEF_ U8 FourByte_IndexIn;
//_UARTDEF_ U8 TwoByte_IndexIn;


void STM32_USART_Config(void);  
void STM32_GPIO_Configuration(void);

extern char Usart3_Rev_Buf[128];
extern char Usart3_Rev_Idx;
 
void UART1_SendByte(U8 buf);
void UART1_SendBuf(U8 *buf, U32 n);

void UART2_SendBuf(U8 *buf, U32 n);
void UART2_SendByte(U8 buf);
void SwitchInputMethods(_InputSourceType inputSrc,U8 image,BOOL delayFlag);
void UART3_SendByte(U8 buf);
void UART3_SendBuf(U8 *buf, U32 n);
void DealUart2Data(U8 ucTmp);
#if 0
BOOL CheckSumFourByte(U8 ChCmd,U8 chPara,U8 chData1,U8 chData2);
#endif
void DealPC_QueueMsg(void);
  #ifdef TCP_IP
   void DealUSART3_QueueToEth(void);
  #endif
void STM32_PC_GPIO_Init(void);

extern void Serial_Init(void);
extern void SerialCommandHandler(void);

#endif
