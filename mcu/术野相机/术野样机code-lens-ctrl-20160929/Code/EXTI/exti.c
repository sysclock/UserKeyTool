
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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

  //GPIOA.10 中断线以及中断初始化配置   上升沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource10);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

 //GPIOA.9	  中断线以及中断初始化配置 下降沿触发 //KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource9);
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

 //GPIOA.8	  中断线以及中断初始化配置  下降沿触发	//KEY0
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键KEY2所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);

}

//外部中断0服务程序 
void EXTI15_10_IRQHandler(void)
{  
	 
//	delay_ms(10);//消抖
//	if(KEY3==1)	 	 //WK_UP按键
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
	EXTI_ClearITPendingBit(EXTI_Line10); //清除LINE0上的中断标志位 
}

#define Exti_From_Pin8 0x00000100
#define Exti_From_Pin9 0x00000200
//外部中断2服务程序
void EXTI9_5_IRQHandler(void)
{

	u32 Temp=0x00;
	Temp=EXTI->PR;
	switch(Temp)
  	{
    	case Exti_From_Pin8:
	  		//脉冲处理
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
			//报警脉冲 
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

	
