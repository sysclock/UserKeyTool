/**
  ******************************************************************************
  * �ļ���    led.c 
  * ����      ��ʼ��LED�˿�  
  * ��汾    V3.5.0
  * ����      2012-11-13
  * ʵ��ƽ̨  С���
  * Ӳ������  LED1-PC0
  			  LED2-PC1
			  LED3-PC2
			  LED4-PC3 	
  ******************************************************************************
  * By���ھŵ�Ƭ����̳ nibutaiguai
  ****************************************************************************** 
  */ 
#include "led.h"

void LED_GPIO_Config(void)
{							 						 
	/*����һ��GPIO_InitTypeDef ���͵Ľṹ�壬���ֽ�GPIO_InitStructure*/ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	/*����GPIO������ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	/*ѡ��Ҫ�õ�GPIO����*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_8 ;
								
	/*��������ģʽ*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 

	/*���������ٶ�*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_2|GPIO_Pin_13;
								
	/*��������ģʽ*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 

	/*���������ٶ�*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_15);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);	
	
}


