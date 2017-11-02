#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

#define USART_REC_LEN  			30  	//�����������ֽ��� 200

#define send485_ON GPIO_SetBits(GPIOA, GPIO_Pin_12);	
#define send485_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_12);	

	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_COUNT;   

extern u8 RS485_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u8 RS485_RX_COUNT;       //����״̬���

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
