/******************************************************************************
 Copyright   All rights reserved.
====
 [Module Name]: Main.c
 [Date]:        08-May-2012
 [Comment]:
   Main subroutines.
 [Reversion History]:
*******************************************************************************/

#include "header.h"	


/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

#ifndef NULL
#define NULL 0
#endif
  
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE 
{ 
	USART_SendData(USART1, (u8) ch); 
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
	return ch; 
} 
 
 
_InputSourceType g_InputBack=INPUTSOURCE_1DVI;

extern struct rtc_time systmtime;


extern void InitIT626X_Instance(void);
extern void InitIT626X(void);
extern void My_HDMITX_ChangeDisplayOption(void);
//extern void HDMITX_ChangeDisplayOption(HDMI_Video_Type VideoMode, HDMI_OutputColorMode OutputColorMode);
extern void HDMITX_DevLoopProc(void);


/*
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID
 */
void Get_ChipID(void)
{
//	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
	ChipUniqueID = *(__IO u32 *)(0X1FFFF7E8); // 低字节
}
//================ Common routines ==========================================

void NVIC_Configuration(void);
//===========================================================================

//***************************************main**********************************
char uart3_send[128]={0};
int main(void)
{ 	
	WORD W_Data = 0;

	NVIC_Configuration();
	delay_init(72);
	
	TIM3_Configuration();

///加一个定时器等待fpga初始化完成
//	for(W_Data = 0; W_Data < 3; W_Data++)
		Delay100ms(10);
	
	STM32_GPIO_Configuration();	   
	LCD_BK_H;	
	UartRxQueue_Init();
	
	initGlobalVar(); 
	LCDStart();
	Get_ChipID();
	
	STM32_USART_Config();
	

 	Read_EEPROM_Byte(E2ROM_DeviceID_Addr, &g_DeviceID);


#ifdef POWER_ON_PWD
	Read_EEPROM_U16(E2ROM_Log_Num_ADDR, &g_NumSaved);
#endif

	////DisplaySoftRevision(0x90);
	#ifdef DEBUG_STATE_PRINT
		PrintStateValue();
	#endif
	////DisMathineIDValue(ChipUniqueID,0x84);

	Serial_Init();
	ParamInit();
	
// // // 	DevicesInit();
// // // 	LoadUserLastMode(g_DisplayModeIndex);
// // // 	
// // // 	W_Data = CheckEquipmentIntegrity();
// // // 	
// // // 	if(W_Data == 0)
// // // 	{
// // // 		clearlcd();
// // // 		chinesedisplay(0x80,  "警告：               ");
// // // 		chinesedisplay(0x90,  "设备输入输出不完整   ");
// // // 		Delay100ms(30);
// // // 	}

	////int a=2,b=11,c;	
	////printf("a+b++/c++  = %d  \r\n", c=a+b++/a++);
	
	g_CurrentState=State_MainMenu_Not_Menu;
	STM32_RTC_Init();  
	

	Exit_EC11_Pin4_Init();

	ReturnMainWin();

		////
		if(IsHasSave(MODE1))
		{


		}
		else
		{
			#ifdef USESETUPWIZARD
			if(!CheckEEPRom())
			{	////进入主菜单之前的密码检查
				g_CurrentState=State_MainMenu_Not_Menu;
				clearlcd();
				chinesedisplay(0x80, g_EEPROMErrorStr);
				chinesedisplay(0x90, g_ContactTel1);
			}
			else
			{
				WizardSet();	
				g_WizadFlag=__TRUE;      
			}


				if(g_CurrentState==State_NewMode_Set)
				{

					Start_LED_Flashing(3);
					g_FlashLedKey[0]=OK_KEY;
					g_FlashLedKey[1]=ESC_KEY;
					g_FlashLedKey[2]=NUM2_KEY;

				}
				else if(Check_OK_Cancel_State())
					{

					Start_LED_Flashing(2);
					g_FlashLedKey[0]=OK_KEY;
					g_FlashLedKey[1]=ESC_KEY;

					}
				else
					{
					g_WizadFlag=__FALSE;
					}
				#endif	////USESETUPWIZARD
				
			}		
			TimeTick = 0;			
	while(1)
	{
		SerialCommandHandler();
		OutputBoardSiI9136Task();
		InputBoardADV7604Task();
		InputBoardGV7601Task();
		

		ScanKeypad();	//scan keypad		

		ScanEC11();

		
		#ifdef TRIAL_MACHINE
			ScanKeypadForSerial();
		#endif
		


		if (TimeTick >= 200)
		{
			TimeTick = 0;	
			
			ScreenWindowLayoutTask();
			
			W_Data = InputFPGA_ReadWord(0x03);
			W_Data ^= BIT11;
			InputFPGA_WriteWord(0x03, W_Data | BIT16);
		}




				


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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState) ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState) ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 		 

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState) ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 	
					 					 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState) ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
	
	
}



