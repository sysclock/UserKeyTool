
#include "led.h"
#include "SYSTICK.h"
#include "wdg.h"

int exit_i,exit_j,exit_k;
void exit_Delay(int time)
{
	for(exit_i=0;exit_i<time;exit_i++)
		for(exit_j=0;exit_j<512;exit_j++)
			for(exit_k=0;exit_k<1024;exit_k++);
		
}
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

  //GPIOA.10 �ж����Լ��жϳ�ʼ������   �����ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource10);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

 //GPIOA.9	  �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource9);
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

 //GPIOA.8	  �ж����Լ��жϳ�ʼ������  �½��ش���	//KEY0
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);

}

//�ⲿ�ж�0������� 
void EXTI15_10_IRQHandler(void)
{  
	 
//	delay_ms(10);//����
//	if(KEY3==1)	 	 //WK_UP����
//	{				 
//		if(exit_i)
		FMQ_ON;
//		else
//		FMQ_OFF;
//
//		exit_i=1-exit_i;
//		IWDG_Feed();
	for(exit_i=0;exit_i<10;exit_i++)
		for(exit_j=0;exit_j<256;exit_j++)
			for(exit_k=0;exit_k<256;exit_k++);
//		delay_ms(1);
		FMQ_OFF;
//	}
	EXTI_ClearITPendingBit(EXTI_Line10); //���LINE0�ϵ��жϱ�־λ 
}

#define Exti_From_Pin8 0x00000100
#define Exti_From_Pin9 0x00000200
//�ⲿ�ж�2�������
void EXTI9_5_IRQHandler(void)
{

	u32 Temp=0x00;
	Temp=EXTI->PR;
	switch(Temp)
  	{
    	case Exti_From_Pin8:
	  		//���崦��
//			FMQ_ON;
			LED3_ON;
			for(exit_i=0;exit_i<3;exit_i++)
				for(exit_j=0;exit_j<256;exit_j++)
					for(exit_k=0;exit_k<256;exit_k++);
//			FMQ_OFF;
			LED3_OFF;
     		EXTI_ClearITPendingBit(EXTI_Line8);
     		break;
    	case Exti_From_Pin9:
			//�������� 
			FMQ_ON;
			for(exit_i=0;exit_i<50;exit_i++)
				for(exit_j=0;exit_j<256;exit_j++)
					for(exit_k=0;exit_k<256;exit_k++);
			FMQ_OFF;   
			EXTI_ClearITPendingBit(EXTI_Line9);	
			break;
		default:
			EXTI_ClearITPendingBit(EXTI_Line8);
			EXTI_ClearITPendingBit(EXTI_Line9);	
			break;
	}
}

	
