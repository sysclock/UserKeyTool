#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

#define USART_REC_LEN  			30  	//定义最大接收字节数 200

#define send485_ON GPIO_SetBits(GPIOA, GPIO_Pin_12);	
#define send485_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_12);	

	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8 USART_RX_COUNT;   

extern u8 RS485_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u8 RS485_RX_COUNT;       //接收状态标记

extern u8 RS485_RX_FLAG;

void USART2_Config(void);	
void USART1_Config(u8 );
int fputc(int ch, FILE *f);
void USART2_Putc(u8 c);
void USART2_Puts(char * str,u32 num);
void USART1_Puts(char * str,u32 num);
void USART2_IRQHandler(void);
void USART1_IRQHandler(void);
#endif
