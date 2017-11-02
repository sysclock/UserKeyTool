/**
  ******************************************************************************
  * 文件名称 ：main.c 
  * 版 	  本 ：V0.0
  * 库 版 本 ：V3.5.0
  * 日    期 ：2012-12-06
  * 实验平台 ：小苗板
  * 硬件连接 ：TX-PA2
  *			   RX-PA3
  *	现	  象 ：串口2输出端打印www.9mcu.com
							  M
							 第九单片机	
  *	修改记录 ：		   	
  ******************************************************************************
  * By：第九单片机论坛 nibutaiguai
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	I2C_EE_Init(); //初始化eeprom
	
	if(KEY7) //如果zoom+按下，就进行出厂设置的恢复
		factory_reset();

	
	if(CheckEEPRom())  //检查是否坏,返回真表示可用
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
				
				//如果下一个场景没有定义，执行后续定义的场景
				while(scene_data[scene_num][1]==1)
				{
					scene_num=(scene_num+1)%4;
				}				
				if(scene_data[scene_num][1]==0)
				{
					
					//显示执行的场景　scene n
					Display_Data_Command(0x0a,0x20,char_S,char_C,char_E,char_N,char_E,0x4a,display_num[scene_num],0x1b,0x1b,0x1b);
					scene_data_send(scene_num);
					Send_Command(title_display_on);
					delay_ms(2000);		
					//0x1b代表空格
					Display_Data_Command(0x0a,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					Zoom_Irq_Function();
				}
		
			  break;
			case FOCUS_ADD: 
				if(focus_mode==0 && focus_mode_flag==0)
				{
					focus_mode_set[4]=3;
					Send_Command(focus_mode_set);// 自动聚焦关闭03  开启为02
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
					Send_Command(focus_mode_set);// 自动聚焦关闭03  开启为02
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
					Send_Command(focus_mode_set);// 自动聚焦关闭03  开启为02
					focus_mode_flag=0;
				}
				do{
				Send_Command(zoomadd_command);
				delay_ms(200);
				}while(KEY7);
				Send_Command(zoomstop_command);		
				Zoom_Irq_Function();
				//如果为手动数码变焦模式，要进行一次自动对焦	
				if( focus_mode==1)
					Send_Command(one_push_trigger);
			
			  break;
			case ZOOM_SUBTRACT:
				if(focus_mode_flag==1)
				{
					focus_mode_set[4]=2;
					Send_Command(focus_mode_set);// 自动聚焦关闭03  开启为02
					focus_mode_flag=0;
				}
				do{
				Send_Command(zoomsub_command);
				delay_ms(200);
				}while(KEY8);
				Send_Command(zoomstop_command);
				Zoom_Irq_Function();
			  //如果为手动数码变焦模式，要进行一次自动对焦
				if( focus_mode==1)
					Send_Command(one_push_trigger);
			  break;
			default :
			  break;
		} 
		if(Key2_time_count>1000)
		{
			
			if(Key2_num==1) //一次按键
			{
					wb_mode=0;
					wb_mode_set[4]=wb_mode;
					Send_Command(wb_mode_set);
			}
			else        //双按键或者一秒内的多按键
			{
					MANU_WB_SET();
			}
			Key2_flag=0;
			Key2_time_count=0;
			Key2_num=0;
		}
		
		//如果进行了变倍查询，三秒后清除
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
		if(RS485_RX_BUF[0]==0xff)  //表示pelco-D
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
				break;
			case PC_SCENE:
					scene_num=(scene_num+1)%4;
					
					//如果下一个场景没有定义，执行后续定义的场景
					while(scene_data[scene_num][1]==1)
					{
						scene_num=(scene_num+1)%4;
					}				
					if(scene_data[scene_num][1]==0)
					{
						
						//显示执行的场景　scene n
						Display_Data_Command(0x0a,0x20,char_S,char_C,char_E,char_N,char_E,0x4a,display_num[scene_num],0x1b,0x1b,0x1b);
						scene_data_send(scene_num);
						Send_Command(title_display_on);
						delay_ms(2000);		
						//0x1b代表空格
						Display_Data_Command(0x0a,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					}
				break;
			case PC_STOP:
				Send_Command(focusstop_command);
				Send_Command(zoomstop_command);
					Zoom_Irq_Function();
					//如果为手动数码变焦模式，要进行一次自动对焦
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

