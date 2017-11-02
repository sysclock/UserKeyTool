/*************************************************  
Copyright (C), 2013-2023, Bycox Tech. Co., Ltd.  
File name:
Author:  LYG  setup~      
Version:        
Date: 20170322 Setup  

Description:    
	// 用于详细说明此程序文件完成的主要功能，与其他模块                  
	// 或函数的接口，输出值、取值范围、含义及参数间的控                  
	// 制、顺序、独立或依赖等关系  Others:         
	// 其它内容的说明  

Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明    
	1. ....  

History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改 者及修改内容简述      
	1. Date:       Author:       Modification:    
	2. ...

*************************************************/


#include "header.h"	 


#ifndef NULL
	#define NULL 0
#endif

GPIO_InitTypeDef GPIO_InitStructure;
//ErrorStatus HSEStartUpStatus;
//u8 count=0;

//struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;

#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  					
   
//__IO uint32_t TimeDisplay = 0;
USART_InitTypeDef USART_InitStructure;
struct rtc_time systmtime;
//u8 const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void FirstStart(void);
void read_eeprom_para(void);
	
U8 LCD_ready=0;


/*
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID
 * 输入  ：无
 * 输出  ：无
 */
void Get_ChipID(void)
{
//	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0);  // 高字节
//	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC);  // 
	ChipUniqueID = *(__IO u32 *)(0X1FFFF7E8);     // 低字节
}


//Check storage device eeprom 24C256
BOOL CheckEEPRom()
{
	U8 testStr[10]={1,2,3,4,5,6,7,8,9,10};
	U8 ReadStr[10]={0};
	int i;
	I2C_WriteS_24C(E2ROM_Last_ADDR,testStr,10);	
	delay(5);
	I2C_ReadS_24C(E2ROM_Last_ADDR, ReadStr, 10);
	for(i=0;i<10;i++)
	{
		if(testStr[i]!=ReadStr[i])
			return __FALSE;
	}
	return __TRUE;
}


BOOL Erase_EEPROM()
{
	U8 data[E2ROM_Last_ADDR-E2ROM_LOCKTIME_BEGIN_ADDR];
	uint32_t i=0;
	for(;i<E2ROM_Last_ADDR-E2ROM_LOCKTIME_BEGIN_ADDR;i++)
	{
		data[i]=0xff;
	}
	m24xx_write(E2ROM_LOCKTIME_BEGIN_ADDR,data,i);
}




/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{	
	char start_buf[SEND_LEN];

	U8 start_value=25;

//	U16 u8LogoID=1;
	U32 iCount=0;//,i=0;

	BOOL PCWarnningFlag=__TRUE,KeyWarnningFlag=__FALSE,TryEndFlag=__TRUE;
//	U8 LedOnFlag=1;

	delay_init(72);

	NVIC_Configuration();
	GPIO_Configuration(); 

	UartRxQueue_Init();
	
//#ifdef TCP_IP
//	Uart3RxQueue_Init();
//#endif

#ifdef STM32F103RB
	Uart4RxQueue_Init();
#endif

	g_PoweronInputSendFlag=__FALSE;

	initGlobalVar();
	
	#if(ENABLE_HARDI2C)
		I2C_Configuration(); 	
	#endif

	
	USART_Config();
	
	read_eeprom_para();//从EEPROM中读取参数	

	//usart3 used?
	USART3_Config();

 	delay_ms(100);
	Read_EEPROM_U16(E2ROM_LogoID_ADDR, &gManufactoryID);
	
	GpuSend("SEBL(100);\r\n");//液晶背光最强
	
	GpuSend("SPG(46);\r\n");//调用串口屏 启动菜单函数:启动中... ...//

	if(gManufactoryID==1)
		GpuSend("PIC(0,0,9);;\r\n");//tuobei的开机图片PIC(0,0,9);
	else //if(==0)
		GpuSend("PIC(0,0,13);;\r\n");

	delay_ms(LCD_COMM_DELAY_MS);
	
/*********************************************************************************************/	

	Copy_Logo(gManufactoryID);
	
	Read_EEPROM_Language();

	Read_EEPROM_TryEndTime();


	//定时器3初始化
	TIM3_Configuration();
	//遥控板
	Remote_Init();

	GetMachineInfo();
//  	Send_MachineInfo();
	Get_ChipID();

//+++++++++++++++++++++++++++++++++++++++++++++++++++

	FlashReadType();
//+++++++++++++++++++++++++++++++++++++++++++++++++++
	//load and excute display scheme
	Updata_Execute();
	GpuSend("DS12(0,126,'启动中:',2);");
	delay_ms(60);

	//waitting mainboard enter working-status
	while(start_value<LOAD_TIME_LONG)
	{
		
		sprintf(start_buf,"CBOF(23,146,%d,158,0,2);\r\n",start_value+(220-LOAD_TIME_LONG));
		GpuSend(start_buf);
		start_value+=1;
		delay_ms(150);
	}
	
	if(!CheckEEPRom())
	{
		g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		chinesedisplay(0x80, g_EEPROMErrorStr);
		chinesedisplay(0x90, g_ContactTel1);
	}

	
	GpuSend("SPG(2);\r\n");//清除屏幕，2号屏布是空白
	LCD_UI_NO = 2;
	g_CurrentState = State_MainMenu_Menu;
	DealCurrentState(g_CurrentState);/*************************************************/

	RTC_Init();  

	
#ifdef EC11_BUTTON
	Exit_EC11_Pin4_Init();
#endif
	
	
	FirstStart();//first boot the machine
	delay(2);
	Calc_total_point();
	READ_INPUT_SOURCE_RESOLUTION(); //读取输入分辨率		
	
	g_UserTry_once_EndFlag=__FALSE;
	if((g_UserTry_show_onoff ||g_UserTry_once_onoff)&&RTC_GetCounter()>g_UserTry_OnceEndTime)
	{
		g_UserTry_once_EndFlag=__TRUE;
	}
	else
	{
		load_scenario_from_FPGA(g_Current_Scene);
	}


	while(1)
	{ 	

		if(g_TryTimeEndFlag &&TryEndFlag)//用户到期标志
		{
			TryEndFlag=__FALSE;
			g_CurrentState=State_TryTime_End;
			DealCurrentState(g_CurrentState);
		}
		if(g_UserTry_once_EndFlag==__TRUE) //如果授权已经结束，清除大屏显示，进行到输入密码页
		{
			DealCurrentState(State_UserTry_OnceEnd);  
		}
		
		
		
		if(!g_PCControlFlag)//上位机控制标志
		{
					
			ScanKeypad();	//scan keypad
			#ifdef EC11_BUTTON
			ScanEC11();
			#endif

			if(Check_OK_Cancel_State())
			{
				//LED_OK_Cancel_Flashing();
				Start_LED_Flashing(2);
				g_FlashLedKey[0]=OK_KEY;
				g_FlashLedKey[1]=ESC_KEY;
			}
		}			
// 			DealPC_QueueMsg();	
		
	}
		
}  




/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


void read_eeprom_para(void)
{
	Read_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
	Read_EEPROM_TryEndTime();
	Read_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, &g_TeleNumVal);//读取试用结束联系电话
	if(g_TeleNumVal!=0)
		Read_EEPROM_Telephone(E2ROM_Telephone_ADDR,g_UserTry_Tel,g_TeleNumVal+1);
	
	Read_EEPROM_Byte(E2ROM_PassWordNum_ADDR, &g_PassWordNumVal);//读取试用密码
	if(g_PassWordNumVal!=0)
		Read_EEPROM_Telephone(E2ROM_PassWord_ADDR,g_UserTry_PassWord,g_PassWordNumVal+1);
	
	Read_EEPROM_Byte(E2ROM_Tech_TelephoneNum_ADDR, &g_TechTeleNumVal);//读取技术支持电话
	if(g_TechTeleNumVal!=0)
		Read_EEPROM_Telephone(E2ROM_Tech_Telephone_ADDR,g_ContactTel1,g_TechTeleNumVal+1);
	
	
	
	
	Read_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR ,&g_UserTry_show_onoff);
	if(g_UserTry_show_onoff>1) //非正常设置状态，正常设置状态0表示关闭，1表示打开
	{
		g_UserTry_show_onoff=0;
		Write_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR,g_UserTry_show_onoff);	
	}
	Read_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR ,&g_UserTry_show_time);
	if(g_UserTry_show_time>99 ||g_UserTry_show_time<10)  //演示时间最少10分钟，最多99分钟
	{
		g_UserTry_show_time=30;    
		Write_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR,g_UserTry_show_time);		     //默认演示时间为30分钟
	}
	Read_EEPROM_Byte(E2ROM_ONCE_ONOFF_ADDR ,&g_UserTry_once_onoff);
	if(g_UserTry_once_onoff>1) //非正常设置状态，正常设置状态0表示关闭，1表示打开
	{
		g_UserTry_once_onoff=0;
		Write_EEPROM_Byte(E2ROM_ONCE_ONOFF_ADDR,g_UserTry_once_onoff);		
	}
	Read_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR ,&g_UserTry_once_time);
	if(g_UserTry_once_time>24 ||g_UserTry_once_time<1)  //单次时间最少1小时，最多24小时
	{	
		g_UserTry_once_time=8;                          //默认单次时间为8小时
		Write_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR,g_UserTry_once_time);	
	}	
	m24xx_read(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);  
	g_UserTry_OnceEndTime=(g_UserTry_once_end_time[0]<<24)+(g_UserTry_once_end_time[1]<<16)+(g_UserTry_once_end_time[2]<<8)+(g_UserTry_once_end_time[3]<<24);

	
		m24xx_read(E2ROM_ONCE_MANAGE_PASSWORD_ADDR, g_UserTry_once_managepassword,5);
	g_manage_password=(g_UserTry_once_managepassword[0]<<24)+(g_UserTry_once_managepassword[1]<<16)+(g_UserTry_once_managepassword[2]<<8)+(g_UserTry_once_managepassword[3]);	
	if(g_manage_password>9999)
	{
		g_manage_password=9669;
		g_UserTry_once_managepassword[0]=(g_manage_password>>24)&0xff;  
		g_UserTry_once_managepassword[1]=(g_manage_password>>16)&0xff; 
		g_UserTry_once_managepassword[2]=(g_manage_password>>8)&0xff; 
		g_UserTry_once_managepassword[3]=(g_manage_password)&0xff; 
		m24xx_write(E2ROM_ONCE_MANAGE_PASSWORD_ADDR,g_UserTry_once_managepassword,5);			
	}
	
	m24xx_read(E2ROM_ONCE_LAST_PASSWORD_ADD, g_UserTry_once_last_password,5);
	last_password_num=(g_UserTry_once_last_password[0]<<24)+(g_UserTry_once_last_password[1]<<16)+(g_UserTry_once_last_password[2]<<8)+(g_UserTry_once_last_password[3]);	

	
	Read_EEPROM_Byte(E2ROM_ONCE_TelephoneNum_ADDR, &g_UserTry_once_TeleNumVal);//读取技术支持电话
	if(g_UserTry_once_TeleNumVal<13)
	{
		Read_EEPROM_Telephone(E2ROM_ONCE_Telephone_ADDR,g_UserTry_once_Tel,g_UserTry_once_TeleNumVal+1);
	}
	else
	{
		g_UserTry_once_TeleNumVal=10;
		Write_EEPROM_Byte(E2ROM_ONCE_TelephoneNum_ADDR,g_UserTry_once_TeleNumVal);	
		g_UserTry_once_Tel[0]='4';
		g_UserTry_once_Tel[1]='0';
		g_UserTry_once_Tel[2]='0';
		g_UserTry_once_Tel[3]='0';
		g_UserTry_once_Tel[4]='9';
		g_UserTry_once_Tel[5]='6';
		g_UserTry_once_Tel[6]='5';
		g_UserTry_once_Tel[7]='7';
		g_UserTry_once_Tel[8]='6';
		g_UserTry_once_Tel[9]='8';
		g_UserTry_once_Tel[10]='0';
		m24xx_write(E2ROM_ONCE_Telephone_ADDR,g_UserTry_once_Tel,g_UserTry_once_TeleNumVal+2); //此处将最后一个0也存储
	}
}
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	   

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 
	

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 	
 	

// 	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	      //中断响应优先级0	  
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
// 	NVIC_Init(&NVIC_InitStructure);	 


/**********************************************************************************************/
// 	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3 ;	      //中断响应优先级0	  
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
// 	NVIC_Init(&NVIC_InitStructure);	 
/**********************************************************************************************/

/*	  								 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 ;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
	*/	 							 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;//USART1_IRQn; //USART1中断 ;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	 //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	         //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		         //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);
	
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	} 	
	return ch;

}

//第一次开机，执行模式设置//
 void FirstStart(void)//+++++++++++++++++++++++++//
{
		Read_EEPROM_Byte(E2ROM_FirstStart_ADDR, &g_FirstStart_Flag);
		if(!g_FirstStart_Flag)
		{ 
			g_FirstStart_Flag=1;
			DealCurrentState(State_WizardSet_Welcome);	
		  Write_EEPROM_Byte(E2ROM_FirstStart_ADDR, g_FirstStart_Flag);			
		}
}





