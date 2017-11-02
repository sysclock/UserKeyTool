/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main program body.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "header.h"	 
#include <stdio.h>	
#include "Queue.h"
#include "cat6613.h"
#include "spi_enc28j60.h"

#ifndef NULL
	#define NULL 0
#endif

GPIO_InitTypeDef GPIO_InitStructure;
//ErrorStatus HSEStartUpStatus;
//u8 count=0;

//struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
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


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{	
	char start_buf[100];
	U8 start_value=25;
//U16 HSize=0,VSize=0;
	U16 u8LogoID=1;
	U32 iCount=0;//,i=0;
//	int LedCount= 0x4000;
	BOOL PCWarnningFlag=__TRUE,KeyWarnningFlag=__FALSE,TryEndFlag=__TRUE;
//	U8 LedOnFlag=1;

	delay_init(72);

	NVIC_Configuration();
	GPIO_Configuration(); 

	UartRxQueue_Init();
#ifdef TCP_IP
	Uart3RxQueue_Init();
#endif

#ifdef STM32F103RB
	Uart4RxQueue_Init();
#endif

	g_PoweronInputSendFlag=__FALSE;
	initGlobalVar();
	
	#if(ENABLE_HARDI2C)
		I2C_Configuration(); 	
	#endif

	
	USART_Config();

	
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
	
	
	USART3_Config();
 	delay_ms(100);
	GpuSend("SEBL(100);\r\n");
	GpuSend("SPG(46);\r\n");//调用串口屏 启动菜单函数
	delay_ms(40);
/*********************************************************************************************/	


//			GpuSend("PIC(0,0,11);");/////++++++++++
//			GpuSend("\r\n");

	Read_EEPROM_U16(E2ROM_LogoID_ADDR, &u8LogoID);
	Copy_Logo(u8LogoID);
	
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

	Updata_Execute();
	while(start_value<190)
	{
		sprintf(start_buf,"CBOF(23,136,%d,138,0,15);\r\n",start_value);
		GpuSend(start_buf);
		start_value+=1;
		delay_ms(155);
	}
	if(!CheckEEPRom())
	{
		g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		chinesedisplay(0x80, g_EEPROMErrorStr);
		chinesedisplay(0x90, g_ContactTel1);
	}
	GpuSend("SPG(2);\r\n");//调用串口屏 启动菜单函数
	LCD_UI_NO = 2;
	g_CurrentState = State_MainMenu_Menu;
	DealCurrentState(g_CurrentState);/*************************************************/

	RTC_Init();  
	
#ifdef EC11_BUTTON
	Exit_EC11_Pin4_Init();
#endif
	FirstStart();
	delay(2);
	Calculation_total_point();
	input_RISR(); //读取输入分辨率		
	load_scenario_from_FPGA(g_Current_Scene);
	while(1)
	{ 	

		if(g_TryTimeEndFlag &&TryEndFlag)//用户到期标志
		{
			TryEndFlag=__FALSE;
			g_CurrentState=State_TryTime_End;
			DealCurrentState(g_CurrentState);
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





