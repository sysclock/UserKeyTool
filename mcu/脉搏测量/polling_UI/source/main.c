/*
天津普仁公司
脉感测量设备
主程序
设计:
20161201

*/


#include "board.h"
#include "fsl_uart.h"
#include "uart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "lcd_spi.h"
#include "flash_spi.h"
#include "key.h"
#include "rtc.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "fb.h"
#include "bat_adc.h"
#include "Queue.h"
#include "lowpower.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_smc.h"
uint32_t bat_num=6;
uint8_t bat_charge_flag=4;
uint8_t bat_num_flag=3;

//读处理器芯片ID
void read_chipID(void)
{
	RIDL = SIM->UIDL;   //MCU ID
  RIDML = SIM->UIDML;
  RIDMH = SIM->UIDMH;
  RIDH = SIM->UIDH;
}

void Deal_APP_comm(void);
void Deal_Auto_Measure(void);
void Deal_StandbyTime(void);
void Updata_main_page_info(void);



int main(void)
{
	uint32_t i,j,k;										 										 
		gpio_pin_config_t IOout_config = {
			kGPIO_DigitalOutput, 0,
		};
		//硬件管脚初始化
		BOARD_InitPins();

		//时钟初始化，源自官方
		BOARD_BootClockRUN();
		
		GPIO_PinInit(GPIOB, 22, &IOout_config);	//power-on	 pin		
		GPIO_PinInit(GPIOB, 21, &IOout_config);//vdda pin
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //拉高控制VDDA ；1u,无符号数1
		
		


		//PW1101
		GPIO_PinInit(GPIOD, 0, &IOout_config);
		GPIO_ClearPinsOutput(GPIOD, 1u);  //拉低PW1101的复位脚D的0号引脚		
		GPIO_PinInit(GPIOD, 0, &IOout_config);//重新配置
		GPIO_SetPinsOutput(GPIOD, 1u);  //拉高PW1101的复位脚

		
		Key_Config();  //按键的初始化
		
//		User_PIT_Init();  //配置定时器
		User_RTC_Init(); //进行RTC时钟的初使化
		
		Delay_S(3);  //延时3s开机
		
		//已经开机，把电池供电通道打开
		if(Power_OnOff_Flag==POWER_OFF)
		{
			Power_OnOff_Flag=POWER_ON;
			GPIO_SetPinsOutput(GPIOB, 1u << 22);				
		}	
		
		LCD_Init();			 //配置LCD所用的SPI口
		Display_full_screen(Page_fb_Data);  //显示开机界面
		Delay_S(2); 

		
		S25FL512S_Spi_Config();  
//		S25FL512S_Erase_Sector(0);		
//		S25FL512S_Erase_Sector(1); //测试用，擦除测量结果据区
//		S25FL512S_Erase_Sector(16); //测试用，擦除测量数据据区
//		S25FL512S_Erase_Sector(4);  //测试用，擦除测量数据据索引区


		Read_para_from_flash();  //从FLASH  中读取用户参数
		
		if(Ble_OnOff==1) //如果蓝牙为打开状态
		{
			GPIO_ClearPinsOutput(GPIOA, 1u << 5);	 
		}
			
		//外设初始化
		BLE_UART_Init();//进行BLE与主MCU通信串口的初使化
		USB_UART_Init(); //进行USB转串口H340的初使化
		PW1101_UART_Init();//进行PW1101通信
		PW1101_Data_RxQueue_Init();  //创建一个数据接收队列
		PW1101_Comm_RxQueue_Init();  //创建一个指令接收队列
		APP_Comm_RxQueue_Init();     //创建用于接收APP控制的队列
		Bat_ADC_Init();            //初始化ADC采集，此函数实现了电池和温度的ADC配置
		bat_num=read_bat_adc();	
		read_chipID();  //读取ID号


		//
		Find_measure_data_add_prt();//找到最后一次记录测量数据存放位置的索引，从而找到最后一次数据存放位置
		Find_Save_Index_Ptr(); //找到最后一条记录测量结果存放位置
	
		
		if(read_PW1101Information()==INTERACT_FAIL) //读取IC信息 如查读取错误，给提示
			g_CurrentStat=State_Main_Menu_interact_error;
		else		
			g_CurrentStat=State_Main_Menu; 	//当前状态等于主工作界面状态
		
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);//加载界面 
		i=0;

		
    while (1)
    {				
			if(Auto_Measure_OnOff==TURN_ON)  //如果已经打开后台测量
			{
				Deal_Auto_Measure();
			}
			
			if(StandbyTime_Count>StandbyTime)  //30秒待机定时，StandbyTime_Count的更新在rtc.RTC_Seconds_IRQHandler（）函数
			{
				Deal_StandbyTime();
			}
			
			if(g_CurrentStat==State_Main_Menu &&LCD_Display_OnOff!=Display_OFF)  //如果当前为主页面
			{
				Updata_main_page_info(); //进行数据更新
			}
			
			Scan_KeyValue();	 //进行按键扫描
					
			//判断APP有无数据需要处理，通过APP_Interact_Flag和APP_CRC_Error_Flag两个标识进行判断，
			//两个标识符在USB和BLE的串口中断中进行改变，如果有APP数据需要处理,则调用Deal_APP_comm函数
			if(APP_Interact_Flag ||APP_CRC_Error_Flag) 
			{
				Deal_APP_comm();  //处理APP发送的指令，包括从USB或者蓝牙传送来的数据
			}
			
   }
}







 //处理后台的自动测量；人一直带着手腕表
void Deal_Auto_Measure(void) //处理后台的自动测量
{
	if(Auto_Measure_TimeInterval_Count>(60*AutoMeasurePara.Timeinterval)) // 自动测量的时间间隔单位为分钟，中断计数的单位为秒，所以乘以60
	{
		if(Auto_Measure_Total_Num_Count<=( AutoMeasurePara.Timelong*60/AutoMeasurePara.Timeinterval+1))  //测量次数少于自动定时时长和间隔推算出的时间
		{
			if(g_CurrentStat==State_Main_Menu ||g_CurrentStat==State_Main_Menu_Manual ||g_CurrentStat==State_Main_Menu_Auto||g_CurrentStat==State_Main_Menu_Record||g_CurrentStat==State_Main_Menu_Set)  //如果计时已到，且为可进入页面状态
			{
				Auto_Measure_TimeInterval_Count=0; //测量前清计数器
				
			//////////////////测试用///////////////////
				if(LCD_Display_OnOff==Display_OFF)
				{
					LCD_Transfer_Command(0xaf); //开显示				
				}
				auto_measure_going();  //显示后台测量中
				/////////////////////////////////////////
				if(pw1101_func_auto_measure()==MEASURE_SUCCESS)  //执行后台测量
				{
					Auto_Measure_Success_Count++;	
					Save_Measure_Result_To_Flash(RTC->TSR,measure_result_data);	 //如果成功，进行测量结果的存储
					alarm_function(); //进行报警限值的比较		
				}
				else
				{
					Auto_Measure_Fail_Count++;   //测量失败次数统计
				}				 
				Auto_Measure_Total_Num_Count++;	 //总的测量次数统计

				/////////////////////////////////
				if(LCD_Display_OnOff==Display_OFF)
				{
					LCD_Transfer_Command(0xae); //关显示				
				}
				First_in_Main_Menu=1;
				stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);//加载界面 
				/////////////////////////////////
			}
		}
	}
}



//===========================================================================================================================
//
void EnterSTOPMode()//单片机进入低功耗模式  吕鹏 2017-1-24
{
    /* unlock all VLP mode */
    SMC->PMPROT |= 0xFF;
    
    /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    //(enSleepOnExit)?(SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk):(SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk);
    
    /* WFI instruction will start entry into STOP mode */
    __ASM("WFI");
}



//===========================================================================================================================
void PORTB_IRQHandler(void)//按键中断服务函数  吕鹏 2017-1-24
{   
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_SET_PIN);  //清中断标志位
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_RET_PIN);  //清中断标志位
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_UD_PIN);  //清中断标志位
		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<USB5_LINK_PIN);  //清中断标志位
	
        BOARD_BootClockRUN();//重新配置系统时钟
       // SMC->PMPROT= 0x00;
     
		 DisableIRQ(PORTA_IRQn);//关闭中断   
		 DisableIRQ(PORTB_IRQn);//关闭中断
        LCD_VCC_ON;//开启12.5高压  吕鹏 
		
		gpio_pin_config_t IOout_config = {
			kGPIO_DigitalOutput, 0,
		};
	
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //拉高控制VDDA
		GPIO_SetPinsOutput(GPIOD, 1u);  //拉高PW1101的复位脚
		
		LCD_Transfer_Command(0xaf); //开显示		
		
		LCD_Display_OnOff=Display_ON;
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}	
		bat_display=181;      //j显示标志，180表示3分钟  等于181，即进行电量的刷新			
		bat_charge_flag=3;
}

//===========================================================================================================================
void PORTA_IRQHandler(void)//按键中断服务函数  吕鹏 2017-1-24
{   
		GPIO_ClearPinsInterruptFlags(GPIOA, 1U<<KEY_POW_PIN);  //清中断标志位
	
    		BOARD_BootClockRUN();//重新配置系统时钟
       // SMC->PMPROT= 0x00;  (重新配置的时候，时钟没有配置到120MHZ模式，导致慢)
  
		 DisableIRQ(PORTA_IRQn);//关闭中断
		 DisableIRQ(PORTB_IRQn);//关闭中断
        LCD_VCC_ON;//开启12.5高压  吕鹏 
	
	
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //拉高控制VDDA		
		GPIO_SetPinsOutput(GPIOD, 1u);  //拉高PW1101的复位脚
		
				
		LCD_Transfer_Command(0xaf); //开显示
		LCD_Display_OnOff=Display_ON;
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
		bat_display=181;      //j显示标志，180表示3分钟  等于181，即进行电量的刷新
		bat_charge_flag=3;
}
//===========================================================================================================================

//当待机时间计时已到，进行此函数执行
void Deal_StandbyTime(void)    
{
	if(g_CurrentStat==State_Main_Menu ||g_CurrentStat==State_Main_Menu_Manual ||g_CurrentStat==State_Main_Menu_Auto||g_CurrentStat==State_Main_Menu_Record||g_CurrentStat==State_Main_Menu_Set)
	{
		LCD_Transfer_Command(0xae); //关显示
		LCD_Display_OnOff=Display_OFF;
		StandbyTime_Flag=0;					
		StandbyTime_Count=0;
        LCD_VCC_OFF;//关闭LCD屏的VCC 12.5V 高压   
		
//        PORT_SetPinInterruptConfig(PORTB,KEY_SET_PIN, kPORT_InterruptFallingEdge);//设置PTB2口为中断			
//				PORT_SetPinInterruptConfig(PORTB,KEY_RET_PIN, kPORT_InterruptFallingEdge);//设置PTB0口为中断    //RES
//				PORT_SetPinInterruptConfig(PORTB,KEY_UD_PIN, kPORT_InterruptFallingEdge);//设置PTB1口为中断    //UD
				PORT_SetPinInterruptConfig(PORTB,USB5_LINK_PIN, kPORT_InterruptRisingEdge);//设置PTB23口为中断   //USB_LINK
        	EnableIRQ(PORTB_IRQn);//开启中断       

		PORT_SetPinInterruptConfig(PORTA,KEY_POW_PIN, kPORT_InterruptFallingEdge);//设置PTA29口为中断   //POWER
		//按键的动作，通过中断唤醒，

		EnableIRQ(PORTA_IRQn);//开启中断    
        	EnterSTOPMode();  //muc低功耗模式  吕鹏 2017-1-24
		
		GPIO_ClearPinsOutput(GPIOB, 1u << 21);  //拉低控制VDDA
		GPIO_ClearPinsOutput(GPIOD, 1u);  //拉低PW1101的复位脚    
		
	}
}


//更新信息
void Updata_main_page_info(void)
{
	unsigned int i=0;
	unsigned char data[5];
	Display_current_time();	
				
	if(GPIO_ReadPinInput(USB5_LINK_GPIO,USB5_LINK_PIN)) //如果已经接入充电线
	{
		if(GPIO_ReadPinInput(BAT_FULL_GPIO,BAT_FULL_PIN) &&bat_charge_flag!=1)
		{
			bat_charge_flag=1;
			Display_bat_bmp(1);//满电
		}
		else if(bat_charge_flag!=2 && bat_charge_flag!=1)
		{
			bat_charge_flag=2;
			Display_bat_bmp(4);//充电
		}
		bat_num_flag=3;   //为防止电量显示抖动，做一个不可逆标志，电量只有再充电时可进行逆增加
		bat_display=0;
		First_in_Main_Menu=0;
	}
	else if(bat_charge_flag!=3)
	{
		bat_charge_flag=3;
		bat_display=181;      //j显示标志，180表示3分钟  等于181，即进行电量的刷新
		First_in_Main_Menu=1;
	}
	
	if(bat_display>30 || First_in_Main_Menu) //3分钟更新一下电池状态
	{	
		bat_num=read_bat_adc();	
		if(bat_num>3576  &&  bat_num_flag>2)  //约大于4.0V
		{
			Display_bat_bmp(1);//满电
			bat_num_flag=3;   //显示的是电量满
		}
		else if(bat_num>3258  && bat_num_flag>1) //大于3.7V
		{
			Display_bat_bmp(2);//中电
			bat_num_flag=2;   //显示的中电
		}
		else if(bat_num>2921  && bat_num_flag>0) //大于3.3V
		{
			Display_bat_bmp(3);//低电
			bat_num_flag=1;   //显示的中电
		}
		else if(bat_num>2844) //大于3.2V
		{
			//进行提示 	
			Display_full_screen(Page_LowPower_Data);
			Delay_S(3);
			minute_updata_flag=61;
			clear_screen();
			bat_num_flag=0; 
			Display_bat_bmp(5);//低电
		}
		else
		{
			//关机
			//USB转串口的两个IO要配置成普通IO才可以，否则无法关机
			PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAsGpio);            /*设置为普通io口 */  //吕鹏  2017-1月24日                   
			PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAsGpio);            /*设置为普通io口 */
			
			
			Power_OnOff_Flag=POWER_OFF;
			GPIO_ClearPinsOutput(GPIOB, 1u << 22);	
		}
		bat_display=0;
		First_in_Main_Menu=0;
	}
}



//App通信
void Deal_APP_comm(void)
{
		if(APP_Interact_Flag) //如果为真，表示APP队列中有数据
		{
		 if(read_comm_from_USBqueue()) //USB收到数据
		 {
			 if(USB_comm_cache[1]==0x50) //如果当前指令为
			 {					 
				 Updata_time(USB_COMM);  //更新时间
				 reply_APP(APP_RX_OK,USB_COMM); //进行回复
			 }
			 else if(USB_comm_cache[1]==0x51) //上传数据
			 {
				 Upload_data(USB_COMM);
			 }
			 else if(USB_comm_cache[1]==0x52) //标识所以示标识的数据为已读（实际相当于清空数据，下次同步数据时不再上传）
			 {
				 Clear_data(USB_COMM);
			 }
		 }
		 else if(read_comm_from_BLEqueue())  //蓝牙收到数据
		 {
				if(BLE_comm_cache[1]==0x50) //如果当前指令为
			 {
				 Updata_time(BLE_COMM);
				 reply_APP(APP_RX_OK,BLE_COMM);   //更新完时间进行APP应答
			 }
			 else if(BLE_comm_cache[1]==0x51) //上传数据
			 {
				 Upload_data(BLE_COMM);
			 }
			 else if(BLE_comm_cache[1]==0x52) //标识所以示标识的数据为已读（实际相当于清空数据，下次同步数据时不再上传）
			 {
				 Clear_data(BLE_COMM);   //清除数据
			 }
		 }
		 else
		 {
			 APP_Interact_Flag=0;  //清空数据
		 }
		}
		if(APP_CRC_Error_Flag)  //收到检验错误的数据
		{
			reply_APP(APP_RX_ERROR,APP_CRC_Error_Flag);
		 APP_CRC_Error_Flag=0;
		}	

		if(StandbyTime!=0)
		{
			StandbyTime_Count=0;  //计时清空
			StandbyTime_Flag=1;   //打开计时标志
		}			
}

