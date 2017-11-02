/**
  ******************************************************************************
  * 文件名    key.c 
  * 功能      初始化KEY端口 
  * 日期      20160802
  * 硬件连接  菜单-PB9	   KEY1
  			确认-PA15	   KEY2
			  场景-PB3	   KEY3
			  电源-PA11	   KEY4
			  FOCUS+  -PB7	 KEY5
			  FOCUS-  -PB8	 KEY6
			  ZOOM+	  -PB5	 KEY7
			  ZOOM-   -PB6	 KEY8
  ****************************************************************************** 
  */ 
#include "key.h"
#include "systick.h"
#include "display.h"
#include "led.h"
unsigned char Key_Value;
unsigned char Key2_flag=0;
unsigned int Key2_time_count=0;
unsigned char Key2_num=0;
unsigned char Key1_flag=0;
unsigned int Key1_time_count=0;
unsigned char MENU_KEY_FLAG=0;

void KEY_GPIO_Config(void)
{	
	/*定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure*/ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*开启GPIO的外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	/*选择要用的GPIO引脚*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	
	/*设置引脚模式*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 

	/*调用库函数，初始化GPIO*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/*选择要用的GPIO引脚*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_15;
	
	/*设置引脚模式*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 

	/*调用库函数，初始化GPIO*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
							
}

u8 Scan_Key_Value()
{
	if(MENU_KEY_FLAG==0)
	{
		if(KEY1 ||KEY2)
		{
			delay_ms(300);
			if(KEY1 &&KEY2)
			{
				delay_ms(1000);
				if(KEY1&&KEY2)
				{
					MENU_KEY_FLAG=1;
					return MENU;
				}
				
			}
		}
	}
	
	if(KEY1)//KEY1按下
	{
		delay_ms(200);	 //延时消抖
		Key1_flag=1;
		while(KEY1&&Key1_time_count<1000);
		if(Key1_time_count<1000) //短按键
		{
			Key1_flag=0;
			Key1_time_count=0;
			return ENTER;
		}
//		else    //大于1秒按键
//		{
//			Key1_flag=0;
//			Key1_time_count=0;
//			return MENU;
//		}
	}
	else
	if(KEY2)//KEY2按下
	{
		delay_ms(100);	 //延时消抖
		Key2_flag=1;
		while(KEY2&&Key2_time_count<1000);
		if(Key2_time_count<1000) //短按键
		{
			Key2_num++;
		}
		else    //大于1秒按键
		{
			Key2_flag=0;
			Key2_time_count=0;
			//返回长按键
		}
//			return KEY2_VALUE;
	}
 	else
 	if(KEY3)//KEY3按下
 	{
 		delay_ms(1000);	 //延时消抖
 		if(KEY3)
 		{
 			return KEY3_VALUE;
 		}	
 	}
	else
	if(KEY4)//KEY4按下
	{
		delay_ms(3000);	 //延时消抖
		if(KEY4)
		{
			if(power_onoff_flag)
			{
				Send_Command(title_clear);
				//执行关机
				Send_Command(power_off);
				delay_ms(50);
				Send_Command(power_off);
				power_onoff_flag=0;
				power_led_off;
			}
			else
			{
				//执行开机
				 delay_ms(50);
				Send_Command(power_on);//开机命令
			//如果指令被执行，进行开机
//				  	turn_on_display();
				power_onoff_flag=1;
				power_led_on;
			}
			return 4;
		}	
	}
	else
	if(KEY5)//KEY5按下
	{
		delay_ms(100);	 //延时消抖
		if(KEY5)
		{
			return KEY5_VALUE;
		}	
	}
	else
	if(KEY6)//KEY6按下
	{
		delay_ms(100);	 //延时消抖
		if(KEY6)
		{
			return KEY6_VALUE;
		}	
	}
	else
	if(KEY7)//KEY7按下
	{
		delay_ms(100);	 //延时消抖
		if(KEY7)
		{
			return KEY7_VALUE;
		}	
	}
	else
	if(KEY8)//KEY8按下
	{
		delay_ms(100);	 //延时消抖
		if(KEY8)
		{
			return KEY8_VALUE;
		}	
	}
	else	//无键按下
		return 0;
		
	return 0;	
}

