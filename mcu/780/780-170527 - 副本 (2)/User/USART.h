/******************************************************************************
			Copyright   All rights reserved.

[Date]:        		2015-1-22
[Comment]:			USART header file.
					Universal Synchronous Asynchronous Receiver/Transmitter
					通用同步和异步收发器
[Reversion History]:	NONE
*******************************************************************************/
#ifndef _USART_H_
#define _USART_H_

//typedef unsigned char      U8;
//typedef short              S16;
//typedef unsigned short     U16;
//typedef int                S32;
//typedef unsigned int       U32;
//typedef long long          S64;
//typedef unsigned long long U64;
//typedef unsigned char      BIT;
//typedef unsigned int       BOOL;



extern U8 g_Read_Command_Flag;
extern U8 g_RevUart2Data[1024];
extern U16 g_RevUart2Idx;


void USART_Config(void);  
void USART3_Config(void);
void GPIO_Configuration(void);
//void Delay(vu32 nCount);

U8 UART4_SendByte(U8 buf);
U8 UART4_RxByte(void);

void UART1_SendByte(U8 buf);
void UART1_SendBuf(U8 *buf, U32 n);

void UART2_SendByte(U8 buf);
void UART2_SendBuf(U8 *buf, U32 n);

//void uart2_sendbyte(U8 ch);
void UART3_SendByte(U8 buf);
void UART3_SendBuf(U8 *buf, U32 n);

//void UART4_SendByte(U8 buf);
void UART4_SendBuf(U8 *buf, U32 n);
//BOOL Check_Return_PowerOn(void);
void DealPC_QueueMsg(void);

#ifdef TCP_IP
	void DealUSART3_QueueToEth(void);
#endif

#ifdef STM32F103RB
	void DealUart4Buf(void);
#endif
//void GetMstarID(U8 buf);
//extern void __irq USART1_IRQHandler(void);

void GpuSend(char *buf1);

void UART5_SendByte(U8 buf);
void UART5_SendBuf(U8 *buf, U32 n);




#endif
