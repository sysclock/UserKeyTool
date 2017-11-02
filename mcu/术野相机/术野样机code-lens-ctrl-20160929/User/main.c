/**
  ******************************************************************************
  * �ļ����� ��main.c 
  * �� 	  �� ��V0.0
  * �� �� �� ��V3.5.0
  * ��    �� ��2012-12-06
  * ʵ��ƽ̨ ��С���
  * Ӳ������ ��TX-PA2
  *			   RX-PA3
  *	��	  �� ������2����˴�ӡwww.9mcu.com
							  M
							 �ھŵ�Ƭ��	
  *	�޸ļ�¼ ��		   	
  ******************************************************************************
  * By���ھŵ�Ƭ����̳ nibutaiguai
  ******************************************************************************
**/
#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "systick.h"
#include "wdg.h"
#include "stmflash.h"
#include "display.h"
#include "key.h"
#include "eeprom.h"

typedef enum
{
	PC_NONE,
	PC_ZOOM_ADD,
	PC_ZOOM_SUBTRACT,
	PC_FOCUS_ADD,
	PC_FOCUS_SUBTRACT,
	PC_POWER_ONOFF,
	PC_SCENE,
	PC_STOP
}_PC_COMMAND_Type;

_PC_COMMAND_Type PC_COMMAND=PC_NONE;
u8 TEXT_Buffer[20];
u8 power_onoff_flag=1;
u8 work_para[512];
u8 focus_mode_flag=0; 
u8 work_led_flag=0;
int i,j,k;
int kg_flag=0; 
void Delay(int time)
{
	for(i=0;i<time;i++)
		for(j=0;j<256;j++)
			for(k=0;k<256;k++);
		
}



void Deal_PC_Command(void);



int main(void)
{  		

 	u32 i=0;
	for(i=0;i<10;i++)work_para[i]=i;
	LED_GPIO_Config();
	KEY_GPIO_Config();
	SYSTICK_Init();
	USART2_Config(); 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	I2C_EE_Init(); //��ʼ��eeprom
	
	if(KEY7) //���zoom+���£��ͽ��г������õĻָ�
		factory_reset();

	
	if(CheckEEPRom())  //����Ƿ�,�������ʾ����
	{
		initGlobalVar();
	}
	
	
	USART1_Config(baudrate);
	
	send485_ON;
//	delay_ms(5);
//	USART1_Puts((char *)work_para,10);
	delay_ms(5);
	send485_OFF;
	if(format==2 ||format==3)
		GPIO_SetBits(GPIOB, GPIO_Pin_13);		
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);		
	turn_on_display();

	while(1)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();

		switch(Key_Value)
		{
			case MENU:
 				  Menu_Page();
			  break;			
			case SCENE:
				scene_num=(scene_num+1)%4;
				
				//�����һ������û�ж��壬ִ�к�������ĳ���
				while(scene_data[scene_num][1]==1)
				{
					scene_num=(scene_num+1)%4;
				}				
				if(scene_data[scene_num][1]==0)
				{
					
					//��ʾִ�еĳ�����scene n
					Display_Data_Command(0x0a,0x20,char_S,char_C,char_E,char_N,char_E,0x4a,display_num[scene_num],0x1b,0x1b,0x1b);
					scene_data_send(scene_num);
					Send_Command(title_display_on);
					delay_ms(2000);		
					//0x1b����ո�
					Display_Data_Command(0x0a,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					Zoom_Irq_Function();
				}
		
			  break;
			case FOCUS_ADD: 
				if(focus_mode==0 && focus_mode_flag==0)
				{
					focus_mode_set[4]=3;
					Send_Command(focus_mode_set);// �Զ��۽��ر�03  ����Ϊ02
					focus_mode_flag=1;
				}
				Send_Command(focusadd_command);
				delay_ms(200);
				Send_Command(focusstop_command);
				Zoom_Irq_Function();
			  break;
			case FOCUS_SUBTRACT:
				if(focus_mode==0 && focus_mode_flag==0)
				{
					focus_mode_set[4]=3;
					Send_Command(focus_mode_set);// �Զ��۽��ر�03  ����Ϊ02
					focus_mode_flag=1;
				}
				Send_Command(focussub_command);
				delay_ms(200);
				Send_Command(focusstop_command);
				Zoom_Irq_Function();
			  break;
			case ZOOM_ADD:			
				
				if(focus_mode_flag==1)
				{
					focus_mode_set[4]=2;
					Send_Command(focus_mode_set);// �Զ��۽��ر�03  ����Ϊ02
					focus_mode_flag=0;
				}
				do{
				Send_Command(zoomadd_command);
				delay_ms(200);
				}while(KEY7);
				Send_Command(zoomstop_command);		
				Zoom_Irq_Function();
				//���Ϊ�ֶ�����佹ģʽ��Ҫ����һ���Զ��Խ�	
				if( focus_mode==1)
					Send_Command(one_push_trigger);
			
			  break;
			case ZOOM_SUBTRACT:
				if(focus_mode_flag==1)
				{
					focus_mode_set[4]=2;
					Send_Command(focus_mode_set);// �Զ��۽��ر�03  ����Ϊ02
					focus_mode_flag=0;
				}
				do{
				Send_Command(zoomsub_command);
				delay_ms(200);
				}while(KEY8);
				Send_Command(zoomstop_command);
				Zoom_Irq_Function();
			  //���Ϊ�ֶ�����佹ģʽ��Ҫ����һ���Զ��Խ�
				if( focus_mode==1)
					Send_Command(one_push_trigger);
			  break;
			default :
			  break;
		} 
		if(Key2_time_count>1000)
		{
			
			if(Key2_num==1) //һ�ΰ���
			{
					wb_mode=0;
					wb_mode_set[4]=wb_mode;
					Send_Command(wb_mode_set);
			}
			else        //˫��������һ���ڵĶఴ��
			{
					MANU_WB_SET();
			}
			Key2_flag=0;
			Key2_time_count=0;
			Key2_num=0;
		}
		
		//��������˱䱶��ѯ����������
		if(zoomirq_time>3000)
		{
			Send_Command(title_clear);
			zoomirq_flag=0;
			zoomirq_time=0;
		}

		if(RS485_RX_FLAG)
		{
			Deal_PC_Command();
		}			
	}
}

void Deal_PC_Command(void)
{
	u8 i,sum=0;
	if(RS485_RX_BUF[1]==address)
	{
		if(RS485_RX_BUF[0]==0xff)  //��ʾpelco-D
		{
			for(i=1;i<6;i++)
			{
				sum+=RS485_RX_BUF[i];
			}
			if(sum==RS485_RX_BUF[6])
			{
				if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x20)
				{
					PC_COMMAND=PC_ZOOM_ADD;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x40)
				{
					PC_COMMAND=PC_ZOOM_SUBTRACT;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x80)
				{
					PC_COMMAND=PC_FOCUS_ADD;
				}
				else if(RS485_RX_BUF[2]==0x01&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_FOCUS_SUBTRACT;
				}
				else if(RS485_RX_BUF[2]==0x08&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_POWER_ONOFF;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x01)  
				{
					PC_COMMAND=PC_SCENE;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_STOP;
				}
			}
		}
		else if(RS485_RX_BUF[0]==0xa0&&RS485_RX_BUF[6]==0xaf) 
		{			
			for(i=1;i<6;i++)
			{
				sum+=RS485_RX_BUF[i];
			}
			if(sum==RS485_RX_BUF[7])
			{
				if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x20)
				{
					PC_COMMAND=PC_ZOOM_ADD;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x40)
				{
					PC_COMMAND=PC_ZOOM_SUBTRACT;
				}
				else if(RS485_RX_BUF[2]==0x01&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_FOCUS_ADD;
				}
				else if(RS485_RX_BUF[2]==0x02&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_FOCUS_SUBTRACT;
				}
				else if(RS485_RX_BUF[2]==0x10&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_POWER_ONOFF;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x01)
				{
					PC_COMMAND=PC_SCENE;
				}
				else if(RS485_RX_BUF[2]==0x00&&RS485_RX_BUF[3]==0x00)
				{
					PC_COMMAND=PC_STOP;
				}
			}
		}
		switch(PC_COMMAND)
		{		
			case PC_ZOOM_ADD:
				Send_Command(zoomadd_command);
				break;
			case PC_ZOOM_SUBTRACT:
				Send_Command(zoomsub_command);
				break;
			case PC_FOCUS_ADD:
				Send_Command(focusadd_command);	
				break;
			case PC_FOCUS_SUBTRACT:
				Send_Command(focussub_command);
				break;
			case PC_POWER_ONOFF:
				 if(power_onoff_flag)
					{
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
				break;
			case PC_SCENE:
					scene_num=(scene_num+1)%4;
					
					//�����һ������û�ж��壬ִ�к�������ĳ���
					while(scene_data[scene_num][1]==1)
					{
						scene_num=(scene_num+1)%4;
					}				
					if(scene_data[scene_num][1]==0)
					{
						
						//��ʾִ�еĳ�����scene n
						Display_Data_Command(0x0a,0x20,char_S,char_C,char_E,char_N,char_E,0x4a,display_num[scene_num],0x1b,0x1b,0x1b);
						scene_data_send(scene_num);
						Send_Command(title_display_on);
						delay_ms(2000);		
						//0x1b����ո�
						Display_Data_Command(0x0a,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					}
				break;
			case PC_STOP:
				Send_Command(focusstop_command);
				Send_Command(zoomstop_command);
					Zoom_Irq_Function();
					//���Ϊ�ֶ�����佹ģʽ��Ҫ����һ���Զ��Խ�
					if( focus_mode==1)
						Send_Command(one_push_trigger);
				break;
			default:
				break;
		}
	}
	for(i=0;i<RS485_RX_COUNT;i++)
		RS485_RX_BUF[i]=0;
	RS485_RX_COUNT=0;
	RS485_RX_FLAG=0;
}

