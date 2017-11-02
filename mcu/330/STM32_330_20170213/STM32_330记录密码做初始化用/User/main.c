/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: Main.c
 [Date]:        24-12-2015
 [Comment]:


 
   Main subroutines.
 [Reversion History]:
*******************************************************************************/
#include <stdio.h>

#include "stm32f10x.h"
#include "header.h"	 
#include "stm32_eval.h"
	
#include "spi_enc28j60.h"
#include "Queue.h"
#include "remote.h"



#ifndef NULL
	#define NULL 0
#endif
  
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

_InputSourceType g_InputBack=INPUTSOURCE_DVI;
extern struct rtc_time systmtime;


extern void InitIT626X_Instance(void);
extern void InitIT626X(void);
extern void My_HDMITX_ChangeDisplayOption(void);
//extern void HDMITX_ChangeDisplayOption(HDMI_Video_Type VideoMode, HDMI_OutputColorMode OutputColorMode);
extern void HDMITX_DevLoopProc(void);
BOOL CheckEEPRom(void);
void Get_ChipID(void);
void WizardOrRecoverMode(void);
void Read_EEPROM_UserCFG(void);
void NVIC_Configuration(void);
void optPeripInit(void);
void loadAutoAdjVGA(void);
void powerOnCheckPassWord(void);
void checkArrearMachineAndScanPad(void);
void checkArrearMachineAndScanPad(void);
void dealTryEnd(void);
void keyPeriphInit(void);




//***************************************main**********************************
int main(void)
{ 	
	
	Get_ChipID();

	initGlobalVar();
  	

	keyPeriphInit();
	

	Read_EEPROM_UserCFG();	
	

	WizardOrRecoverMode();		

	optPeripInit();


//20170213 lyg	

	g_UserTry_once_EndFlag=__FALSE;     //是否启动单次密码或者演示密码
	if(g_UserTry_show_onoff ||g_UserTry_once_onoff)  //单次或者演示开启，当前时间大于截止时间，表明授权已超时
	{
		g_UserTry_once_EndFlag=__TRUE;
	}
//	else if(g_UserTry_show_onoff && RTC_GetCounter()<(g_UserTry_OnceEndTime-g_UserTry_show_time*60))  //如果已经开启演示功能，当前的时间不应该小于演示截止时间减去演示时长 
//	{
//		g_UserTry_once_EndFlag=__TRUE;
//	}
//	else if(g_UserTry_once_onoff && RTC_GetCounter()<(g_UserTry_OnceEndTime-g_UserTry_once_time*3600))  //如果已经开启单次功能，当前的时间不应该小于单次截止时间减去单次时长
//	{
//		g_UserTry_once_EndFlag=__TRUE;
//	}
	while(1)
	{
	
	#ifdef POWER_ON_PWD
		if(!g_PowerOnPwdOk&&g_PowerOnPwdFlag)
		{
			ScanKeypadForPowerOnPwd();
		}
		else		
	#endif
		
		if(g_UserTry_once_EndFlag==__TRUE && (g_CurrentState!=State_UserTry_OnceEnd)&&(g_CurrentState!=State_UserTry_Once_ID_Time)) //如果授权已经结束，清除大屏显示，进行到输入密码页
		{
			g_CurrentState=State_UserTry_OnceEnd;
			g_MainInput_pre=g_MainInput;
			SetInputSourceSwitch(INPUTSOURCE_SVIDEO);  //变成蓝屏
			DealCurrentState(State_UserTry_OnceEnd);  
			chinesedisplay(0x93, g_UserTry_once_Tel);
			Date_Time[0]=systmtime.tm_year;				
			Date_Time[1]=systmtime.tm_mon;
			Date_Time[2]=systmtime.tm_mday;
			Date_Time[3]=systmtime.tm_hour;
		}	
		else 	if(!g_TryTimeEndFlag)
			checkArrearMachineAndScanPad();	
		else if(g_TryEndFlag)		
			dealTryEnd();		
	
		else
			ScanKeypadForSerial();		
		
	
	}

}


void keyPeriphInit()
{

	NVIC_Configuration();

	STM32_GPIO_Configuration();	


	LCD_BK_H;
	
	UartRxQueue_Init();

	Uart3RxQueue_Init();
	 

#if(ENABLE_HARDI2C)
	I2C_Configuration(); 	
#endif	

	STM32_USART_Config(); 
	STM32_RTC_Init(); 
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
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 		 
#ifdef STM32F103RB
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 	
#endif								 
					 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;//USART1_IRQn;		//USART1中断 ;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
	
	delay_init(72);
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
//	USART_SendData(/*EVAL_COM1*/USART3, (uint8_t) ch);
	USART_SendData(EVAL_COM1, (uint8_t) ch);
	/* Loop until the end of transmission */
	//while (USART_GetFlagStatus(/*EVAL_COM1*/USART3, USART_FLAG_TC) == RESET)
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
	{
	} 	
	return ch;
}



BOOL CheckEEPRom()
{
	U8 testStr[6]={1,2,3,4,5,6};
	U8 ReadStr[6]={0};
	int i;
	I2C_WriteS_24C(0,testStr,6);	
	delay(5);
	I2C_ReadS_24C(0, ReadStr, 6);
	for(i=0;i<6;i++)
	{
		if(testStr[i]!=ReadStr[i])
			return __FALSE;
	}
	return __TRUE;
}




void Get_ChipID(void)
{
//	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
//	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID = *(__IO u32 *)(0X1FFFF7E8); // 低字节
}



void WizardOrRecoverMode()
{

	if(IsHasSave(MODE1))
	{
		g_CurrentState=State_UserPara_RecallMode;
		clearlcd(); //清除LCD
		
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x80,  g_ExeModeStr);
			chinesedisplay(0x93,  g_waitStr);	
		}
		else if(g_LanguageFlag==ENGLISH)
		{
			chinesedisplay(0x80,  g_ExeModeStrEn);
			chinesedisplay(0x93,  g_waitStrEn);	
		}
			
		Updata_Execute(EEPROMMemRegU8,EEPROMMemRegU16);	

		ReturnMainWin();
		LightInputSrcLED();
	}
	else
	{
		if(!CheckEEPRom())
		{
			g_CurrentState=State_MainMenu_Not_Menu;
			clearlcd();
			chinesedisplay(0x80, g_EEPROMErrorStr);
			chinesedisplay(0x90, g_TechSupportTel1);
		}
		else
		{
			WizardSet();	
			g_WizadFlag=__TRUE;
		}
	}


	//
	Read_EEPROM_Byte(E2ROM_OutputHZ_ADDR,&g_OutputHzVal);
	if(g_OutputHzVal!=60)
	{
		delay(3);
		SetOutputHzValue();
	}


	//读取循环播放的设置
	Read_EEPROM_Byte(E2ROM_CycleFlag_ADDR,&g_StartCycleInputFlag);
	if(g_StartCycleInputFlag)
	{
		Read_EEPROM_InputCycle(E2ROM_CycleInput1_ADDR);
		if(g_CycleInput1!=INPUTSOURCE_NONE)
		{
			g_InputCycleIdx++;
			CycleSwitchInputSrc(g_CycleInput1);
			g_CycleInputStartHour=(systmtime.tm_hour+g_CycleInputHour1)%24;
			g_CycleInputStartMin=(systmtime.tm_min+g_CycleInputMin1)%60;
		}
		Read_EEPROM_InputCycle(E2ROM_CycleInput2_ADDR);
		if(g_CycleInput2!=INPUTSOURCE_NONE)
			g_InputCycleIdx++;
		Read_EEPROM_InputCycle(E2ROM_CycleInput3_ADDR);
		if(g_CycleInput3!=INPUTSOURCE_NONE)
			g_InputCycleIdx++;
		Read_EEPROM_InputCycle(E2ROM_CycleInput4_ADDR);
		if(g_CycleInput4!=INPUTSOURCE_NONE)
			g_InputCycleIdx++;
		Read_EEPROM_InputCycle(E2ROM_CycleInput5_ADDR);
		if(g_CycleInput5!=INPUTSOURCE_NONE)
			g_InputCycleIdx++;
		Read_EEPROM_InputCycle(E2ROM_CycleInput6_ADDR);
		if(g_CycleInput6!=INPUTSOURCE_NONE)
			g_InputCycleIdx++;
	}


	
	//读取时间计划值
	if(IsInputScheHasSave(MODE1))
		FromEEInitInputSche(MODE1);
	if(IsInputScheHasSave(MODE2))
		FromEEInitInputSche(MODE2);
	if(IsInputScheHasSave(MODE3))
		FromEEInitInputSche(MODE3);
       //把当前时间的赋值给锁键盘超时錶变量，初始化


	InitSwapFisrtInput();
	loadAutoAdjVGA();

}




void Read_EEPROM_UserCFG()
{
	U32 iCount=0;
	
	Read_EEPROM_Byte(E2ROM_BOTH_SWITCH_ADDR,&g_BothSwitchFlag);

	powerOnCheckPassWord();
	
	
	Read_EEPROM_Byte(E2ROM_LANGUAGE_BEGIN_ADDR, &g_LanguageFlag);
	Read_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR, &g_LockKeypadTime);
	Read_EEPROM_TryEndTime();
	Read_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, &g_TeleNumVal);//读取联系电话
	if(g_TeleNumVal!=0)
		Read_EEPROM_Telephone(E2ROM_Telephone_ADDR,g_ContactTel2,g_TeleNumVal+1);
	Read_EEPROM_Byte(E2ROM_Tech_TelephoneNum_ADDR, &g_TechTeleNumVal);//读取技术支持电话
	if(g_TechTeleNumVal!=0)
		Read_EEPROM_Telephone(E2ROM_Tech_Telephone_ADDR,g_TechSupportTel1,g_TechTeleNumVal+1);
	LCDStart();	

//	Read_EEPROM_U16(E2ROM_LogoID_ADDR, &u8LogoID); 
//	Copy_Logo(u8LogoID);

	//20170213 lyg
	Read_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR ,&g_UserTry_show_onoff);
	if(1) //非正常设置状态，正常设置状态0表示关闭，1表示打开
	{
		g_UserTry_show_onoff=0;
		Write_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR,g_UserTry_show_onoff);	
	}
	Read_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR ,&g_UserTry_show_time);
	if(1)  //演示时间最少10分钟，最多60分钟
	{
		g_UserTry_show_time=10;    
		Write_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR,g_UserTry_show_time);		     //默认演示时间为30分钟
	}
	Read_EEPROM_Byte(E2ROM_ONCE_ONOFF_ADDR ,&g_UserTry_once_onoff);
	if(1) //非正常设置状态，正常设置状态0表示关闭，1表示打开
	{
		g_UserTry_once_onoff=0;
		Write_EEPROM_Byte(E2ROM_ONCE_ONOFF_ADDR,g_UserTry_once_onoff);		
	}
	Read_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR ,&g_UserTry_once_time);
	if(1)  //单次时间最少1小时，最多24小时
	{	
		g_UserTry_once_time=8;                          //单次时间为8小时
		Write_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR,g_UserTry_once_time);	
	}	
	m24xx_read(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);  
	g_UserTry_OnceEndTime=(g_UserTry_once_end_time[0]<<24)+(g_UserTry_once_end_time[1]<<16)+(g_UserTry_once_end_time[2]<<8)+(g_UserTry_once_end_time[3]);

	m24xx_read(E2ROM_ONCE_MANAGE_PASSWORD_ADDR, g_UserTry_once_managepassword,5);
	g_manage_password=(g_UserTry_once_managepassword[0]<<24)+(g_UserTry_once_managepassword[1]<<16)+(g_UserTry_once_managepassword[2]<<8)+(g_UserTry_once_managepassword[3]);	
	if(1)
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
	if(0)
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
	
	
	
	PanelToScreenPara();

#ifdef HOT_BACKUP
	//读取热备份的设置
	Read_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,&g_EnableHotBackupFlag);
	Read_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,&g_HotBackupIdx);
	Read_EEPROM_Byte(E2ROM_Alpha_ADDR,&g_AlphaValue);
	Read_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
#endif

	Read_EEPROM_Byte(E2ROM_EnableRecordLog_ADDR,&g_EnableRecordLog);
	//printf("Init 6263! g_EnableRecordLog %d",g_EnableRecordLog);


	while(!g_PowerOnFlag&&iCount<0xC00000)   //wait 6M16 power on or time out 0xF00000
	{
		iCount++;
	}
	Read_EEPROM_PcPart();
	
#if(XP_ID==XP310|XP350)
	TIM3_Configuration();
	Remote_Init();
#endif

	Read_EEPROM_Byte(E2ROM_DeviceID_Addr, &g_DeviceID);

	Read_EEPROM_Byte(E2ROM_HowLongTime_ADDR,&g_howlongtosave);


	FlashReadLogo();
	FlashReadType();
	
#ifdef POWER_ON_PWD
	Read_EEPROM_U16(E2ROM_Log_Num_ADDR, &g_NumSaved);
#endif

	g_CurrentState=State_MainMenu_Not_Menu;

}



void optPeripInit()
{
#ifdef WIFI_MODULE
	InitEsp8266();
#endif
	
#ifdef TCP_IP
	Read_EEPROM_IP();
   #ifdef ENC28J60INT
       Exit_Enc28j60INT_Init();
   #endif
	SPI_Enc28j60_Init();
	Web_Server();
#endif

#ifdef EC11_BUTTON
	Exit_EC11_Pin4_Init();
#ifdef TWO_EC11_BOARD
	Exit_EC11U15_Pin4_Init();
#endif
#endif
}



void loadAutoAdjVGA()
{

	Read_EEPROM_Byte(E2ROM_AutoAdjustVGA_ADDR, &g_AutoAdjustVGA_Flag);
	if(g_AutoAdjustVGA_Flag)
	{
	 if(g_MainInput==INPUTSOURCE_VGA)
		{//当单画面时，当前通道已经为VGA时，再按此键可以自动调整VGA
		AutoAdjustVGA();								
		}					
	}
}



void powerOnCheckPassWord()
{
#ifdef POWER_ON_PWD	
	FlashReadPWD_Flag();
//	Read_EEPROM_Byte(E2ROM_PowerOnPwd_Flag_ADDR, &g_PowerOnPwdFlag);
	if(g_PowerOnPwdFlag)
	{
		g_InputBack=g_MainInput;
		//printf("g_InputBack=%d!",g_InputBack);
		SetInputSourceSwitch(INPUTSOURCE_SVIDEO);  //变成蓝屏
		DealCurrentState(State_MainMenu_Poweron_PWD);
	}

	if(g_PowerOnPwdFlag)
		Read_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);//读用户设置的密码g_UserSetPasswordVal
#endif
}



void checkArrearMachineAndScanPad()
{
		BOOL PCWarnningFlag=__TRUE,KeyWarnningFlag=__FALSE;//,TryEndFlag=__TRUE;	
		if(!g_PCControlFlag)
			{
				if(KeyWarnningFlag)
				{
					KeyWarnningFlag=__FALSE;
					PCWarnningFlag=__TRUE;
					g_CurrentState=State_MainMenu_Not_Menu;
					clearlcd();			 
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x90,  g_KeyWarnningStr);
					else if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x90,  "PC Disconnect!");
					delay(5);
					LightInputSrcLED();
					ReturnMainWin();
				}
				ScanKeypad();	//scan keypad
				
			#ifdef EC11_BUTTON
				ScanEC11();
			#endif

			
				if(g_CurrentState==State_NewMode_Set)
				{
				#ifndef TWO_EC11_BOARD
					Start_LED_Flashing(3);
					g_FlashLedKey[0]=OK_KEY;
					g_FlashLedKey[1]=ESC_KEY;
					g_FlashLedKey[2]=NUM2_KEY;
				#endif
				}
				else if(Check_OK_Cancel_State())
				{
				#ifndef TWO_EC11_BOARD
					Start_LED_Flashing(2);
					g_FlashLedKey[0]=OK_KEY;
					g_FlashLedKey[1]=ESC_KEY;
				#endif
				}
				else
				{
					g_WizadFlag=__FALSE;
				}

			}
			else
			{
				if(PCWarnningFlag)
				{
					g_WizadFlag=__FALSE;				
					g_FlashLedFlag=__FALSE;
				#ifdef INT_DELAY_PC_CTL
					g_PCSendFlag=__FALSE;
					g_OneSecondISRVal=0;
				#endif
					PCWarnningFlag=__FALSE;
					KeyWarnningFlag=__TRUE;
					LCDDisplayPCControl();
				}
			}
			DealPC_QueueMsg();
			DealUSART3_QueueToEth();

}

void dealTryEnd()
{
			
			g_TryEndFlag=__FALSE;
			SetInputSourceSwitch(INPUTSOURCE_SVIDEO);  //变成蓝屏
			clearlcd();
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x80,  g_TryOverStr);
				chinesedisplay(0x90,  g_PurchaseStr);
			}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,  "Please payment!");
				chinesedisplay(0x90,  "Tele:");
			}
			chinesedisplay(0x93,  g_ContactTel2);		
		}




