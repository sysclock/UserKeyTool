/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: LCD.h
 [Date]:        09-May-2012
 [Comment]:
   LCD header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _USART_H_
#define _USART_H_

BOOL DetectInputSignal(U8 *IorII);
void SwitchBackup(U8 image);

void USART_Config(void);  
void GPIO_Configuration(void);
//void Delay(vu32 nCount);

//u8 UART4_SendByte(U8 buf);
//u8 UART4_RxByte(void);

void UART1_SendByte(U8 buf);
void UART1_SendBuf(U8 *buf, U32 n);

void UART2_SendBuf(U8 *buf, U32 n);
void UART4_SendBuf(U8 *buf, U32 n);
void UART2_SendByte(U8 buf);
void SwitchInputMethods(_InputSourceType inputSrc,U8 image,BOOL delayFlag);
//void uart2_sendbyte(U8 ch);
void UART3_SendByte(U8 buf);
void UART3_SendBuf(U8 *buf, U32 n);
//BOOL Check_Return_PowerOn(void);
BOOL Check_Return_Empty(void);
void DealPC_QueueMsg(void);

 #ifdef TCP_IP
   void DealUSART3_QueueToEth(void);
 #endif

//#ifdef STM32F103RB
//void DealUart4Buf(void);
//#endif
//void GetMstarID(U8 buf);
//extern void __irq USART1_IRQHandler(void);

#endif
