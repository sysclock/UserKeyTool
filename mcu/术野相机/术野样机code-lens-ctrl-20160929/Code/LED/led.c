/**
  ******************************************************************************
  * 文件名    led.c 
  * 功能      初始化LED端口  
  * 库版本    V3.5.0
  * 日期      2012-11-13
  * 实验平台  小苗板
  * 硬件连接  LED1-PC0
  			  LED2-PC1
			  LED3-PC2
			  LED4-PC3 	
  ******************************************************************************
  * By：第九单片机论坛 nibutaiguai
  ****************************************************************************** 
  */ 
#include "led.h"

void LED_GPIO_Config(void)
{							 						 
	/*定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure*/ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	/*开启GPIO的外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	/*选择要用的GPIO引脚*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_8 ;
								
	/*设置引脚模式*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 

	/*设置引脚速度*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
	/*调用库函数，初始化GPIO*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_2|GPIO_Pin_13;
								
	/*设置引脚模式*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 

	/*设置引脚速度*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
	/*调用库函数，初始化GPIO*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_15);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);	
	
}


