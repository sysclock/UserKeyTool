/**
  ******************************************************************************
  * �ļ���    key.c 
  * ����      ��ʼ��KEY�˿� 
  * ����      20160802
  * Ӳ������  �˵�-PB9	   KEY1
  			ȷ��-PA15	   KEY2
			  ����-PB3	   KEY3
			  ��Դ-PA11	   KEY4
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
	/*����һ��GPIO_InitTypeDef ���͵Ľṹ�壬���ֽ�GPIO_InitStructure*/ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*����GPIO������ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	/*ѡ��Ҫ�õ�GPIO����*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	
	/*��������ģʽ*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 

	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/*ѡ��Ҫ�õ�GPIO����*/			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_15;
	
	/*��������ģʽ*/				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 

	/*���ÿ⺯������ʼ��GPIO*/
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
	
	if(KEY1)//KEY1����
	{
		delay_ms(200);	 //��ʱ����
		Key1_flag=1;
		while(KEY1&&Key1_time_count<1000);
		if(Key1_time_count<1000) //�̰���
		{
			Key1_flag=0;
			Key1_time_count=0;
			return ENTER;
		}
//		else    //����1�밴��
//		{
//			Key1_flag=0;
//			Key1_time_count=0;
//			return MENU;
//		}
	}
	else
	if(KEY2)//KEY2����
	{
		delay_ms(100);	 //��ʱ����
		Key2_flag=1;
		while(KEY2&&Key2_time_count<1000);
		if(Key2_time_count<1000) //�̰���
		{
			Key2_num++;
		}
		else    //����1�밴��
		{
			Key2_flag=0;
			Key2_time_count=0;
			//���س�����
		}
//			return KEY2_VALUE;
	}
 	else
 	if(KEY3)//KEY3����
 	{
 		delay_ms(1000);	 //��ʱ����
 		if(KEY3)
 		{
 			return KEY3_VALUE;
 		}	
 	}
	else
	if(KEY4)//KEY4����
	{
		delay_ms(3000);	 //��ʱ����
		if(KEY4)
		{
			if(power_onoff_flag)
			{
				Send_Command(title_clear);
				//ִ�йػ�
				Send_Command(power_off);
				delay_ms(50);
				Send_Command(power_off);
				power_onoff_flag=0;
				power_led_off;
			}
			else
			{
				//ִ�п���
				 delay_ms(50);
				Send_Command(power_on);//��������
			//���ָ�ִ�У����п���
//				  	turn_on_display();
				power_onoff_flag=1;
				power_led_on;
			}
			return 4;
		}	
	}
	else
	if(KEY5)//KEY5����
	{
		delay_ms(100);	 //��ʱ����
		if(KEY5)
		{
			return KEY5_VALUE;
		}	
	}
	else
	if(KEY6)//KEY6����
	{
		delay_ms(100);	 //��ʱ����
		if(KEY6)
		{
			return KEY6_VALUE;
		}	
	}
	else
	if(KEY7)//KEY7����
	{
		delay_ms(100);	 //��ʱ����
		if(KEY7)
		{
			return KEY7_VALUE;
		}	
	}
	else
	if(KEY8)//KEY8����
	{
		delay_ms(100);	 //��ʱ����
		if(KEY8)
		{
			return KEY8_VALUE;
		}	
	}
	else	//�޼�����
		return 0;
		
	return 0;	
}

