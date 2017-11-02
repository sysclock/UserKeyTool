/******************** (C) COPYRIGHT 2012 BYCOX*******************
* File Name          : main.c
* Author             : huozhangyi
				liyonggang
				yangjinlin
				
* Version            : V2.1.1

* Date               : 12/18/2013 found

* Description        :
				Video Processor BP550/520 Human-Machine Interface 
				Main program body.
*main Function:
				1,key scan
				2,lcd display
				3,uart communication with main dsp
				



*******************************************************************************/
#include "header.h"	 
#include <stdio.h>	
#include "Queue.h"


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
   
//__IO uint32_t TimeDisplay = 0;
USART_InitTypeDef USART_InitStructure;
//struct rtc_time systmtime;
extern struct rtc_time systmtime;

//u8 const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Long_Delay_with_Flicker_OutVideo_after_VideoStable(void);
void Read_EEPROM_Launch(void);
char	OutDevicesInit(void);
void	WizardOrRecoverMode(void);

void	LoadUserMode(void);
BOOL CheckEEPRom(void);
void LED_Indicate_at_Wizard(void);
void checkArrearMachineAndScanPad(void);
void dealTryEnd(void);

/*
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID
 */
void Get_ChipID(void)
{
	ChipUniqueID = *(__IO u32 *)(0X1FFFF7E8); // 低字节
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
//	U16 u8LogoID=1;
//	int LedCount= 0x4000;
//	BOOL PCWarnningFlag=__TRUE,KeyWarnningFlag=__FALSE;//TryEndFlag=__TRUE;
//	U16 HSize=0,VSize=0;	
//	U8 image=MAIN;
	
	delay_init(72);
	NVIC_Configuration();
	GPIO_Configuration(); 
		
	UartRxQueue_Init();
	
	#ifdef TCP_IP
		Uart3RxQueue_Init();
	#endif

	#if(ENABLE_HARDI2C)
		I2C_Configuration(); 	
	#endif
	
	

	OUTPUT_HiZ_P412();
 	disableCAT6613();
	
	
//#ifdef STM32F103RB
//	Uart4RxQueue_Init();
//#endif

  	LCD_BK_H;
	
	#ifdef USE_INFRARED_IR
		TIM3_Configuration();
		Remote_Init();
	#endif

	g_PoweronInputSendFlag=__FALSE;
	
	initGlobalVar();		
	
	USART_Config();
	
	Read_EEPROM_Launch();

	printf("\r\n===============================\r\n");
	printf("@Begin of Main g_DVIFullOrPartValue=%d\r\n",g_DVIFullOrPartValue);
	
	FlashReadLogo();	
		
	//向导设置或调用，调用模式前等待图像稳定
	WizardOrRecoverMode();
	
	//输出芯片稳定
	OutDevicesInit();
	

	Read_EEPROM_Byte(E2ROM_VOLUME_ADDR, &g_VolumeValue);
	if(g_VolumeValue!=100)
		SetVolumeValue(g_VolumeValue);

	RTC_Init();  
	
	LoadUserMode();
		
	InitSwapFisrtInput();	
	Get_ChipID();
//	upDateFullOrPart();
//	ReturnMainWinDontChangeState();
//	g_CurrentState=State_MainMenu_Menu;
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
#endif

 	Read_EEPROM_MathineType();


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

		if(g_UserTry_once_EndFlag==__TRUE && (g_CurrentState!=State_UserTry_OnceEnd)&&(g_CurrentState!=State_UserTry_Once_ID_Time)) //如果授权已经结束，清除大屏显示，进行到输入密码页
		{
			g_CurrentState=State_UserTry_OnceEnd;
			g_MainInput_pre=g_MainInput;
			g_SubInput_pre=g_SubInput;
			
			SetInputSource(INPUTSOURCE_CVBS4,MAIN);  //CVB4没有用
			if(g_MutiWindow!=SINGLEIMAGE)
				SetInputSource(INPUTSOURCE_CVBS4,SUB);  //CVB4没有用
			
			DealCurrentState(State_UserTry_OnceEnd);  
			chinesedisplay(0x93, g_UserTry_once_Tel);
			Date_Time[0]=systmtime.tm_year;				
			Date_Time[1]=systmtime.tm_mon;
			Date_Time[2]=systmtime.tm_mday;
			Date_Time[3]=systmtime.tm_hour;
		}	
		else if(!g_TryTimeEndFlag)
			checkArrearMachineAndScanPad();	
		
		else if(g_TryEndFlag)
			dealTryEnd();		

		else
			ScanKeypadForSerial();
	}
		
}  






//检查欠款机器和扫描键盘-密码
BOOL PCWarnningFlag=__TRUE,KeyWarnningFlag=__FALSE;
void checkArrearMachineAndScanPad()
{
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
					delay(3);
					g_CurrentState=State_MainMenu_Menu;
					LightInputSrcLED();
					DealEscKey();	
				}
				ScanKeypad();	//scan keypad

			#ifdef EC11_BUTTON
				ScanEC11();
			#endif

				LED_Indicate_at_Wizard();

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
				//g_PCControlFlag=__FALSE;
				//for(i=0;i<20;i++)
				//	delay_ms(1500);	
				//delay_ms(1);
			}		
			DealPC_QueueMsg();

			#ifdef TCP_IP
				DealUSART3_QueueToEth();
	   		   #ifndef ENC28J60INT
				ReceiveEthernetPacket();
			   #endif
			#endif

}


//trial is expire
void dealTryEnd()
{
			g_TryEndFlag=__FALSE;
			SetInputSourceSwitch(INPUTSOURCE_DP);  //变成蓝屏
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	   

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState) ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	 

/*	  								 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		//RTC_IRQn;//USART1_IRQn;		//USART1中断 ;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
	*/	 							 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;//USART1_IRQn;		//USART1中断 ;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	     //中断占先等级0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      //中断响应优先级0	  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		     //打开中断	
	NVIC_Init(&NVIC_InitStructure);	  
}







/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Long_Delay_with_Flicker_OutVideo_after_VideoStable(void)
{
	unsigned short const DELAY_MS = 500;
	unsigned char i;
	clearlcd();
	chinesedisplay(0x80,  g_VideoComing);
	for(i=0;i<0x0a;i++)
	{
		chinesedisplay(0x90+i,  "->");		
			delay_ms(DELAY_MS);
		chinesedisplay(0x90+i,  "-->");		
			delay_ms(DELAY_MS);
	}
		//ReturnMainWin();
		
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Read_EEPROM_Launch(void)
{
			U32 iCount=0;//,i=0;
	
			Read_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
			Read_EEPROM_TryEndTime();
			Read_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, &g_TeleNumVal);//读取联系电话
			if(g_TeleNumVal!=0)
				Read_EEPROM_Telephone(E2ROM_Telephone_ADDR,g_ContactTel2,g_TeleNumVal+1);
			Read_EEPROM_Byte(E2ROM_Tech_TelephoneNum_ADDR, &g_TechTeleNumVal);//读取技术支持电话
			if(g_TechTeleNumVal!=0)
				Read_EEPROM_Telephone(E2ROM_Tech_Telephone_ADDR,g_ContactTel1,g_TechTeleNumVal+1);	
			LCDStart(); 
			


			Read_EEPROM_Language();
			//Read_EEPROM_Byte(E2ROM_OutputHZ_ADDR,&g_OutputHzVal);
			//Read_EEPROM_ScreenScaler();
			Read_EEPROM_TryEndTime();
			Read_EEPROM_Byte(E2ROM_BOTH_SWITCH_ADDR,&g_BothSwitchFlag);
			Read_EEPROM_Byte(E2ROM_ADC_CABLIBRATION_ADDR,&g_HadAdcCalibrationFlag);
			//读取热备份的设置
			Read_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,&g_EnableHotBackupFlag);
			Read_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,&g_HotBackupIdx);
			Read_EEPROM_Byte(E2ROM_Alpha_ADDR,&g_AlphaValue);
			Read_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);


			Read_EEPROM_Byte(E2ROM_EnableRecordLog_ADDR,&g_EnableRecordLog);
			
			while(iCount<0xc00000)   //wait 6M16 power on or time out
			{
				iCount++;
				if(g_PoweronFlag)
					break;
			}
			Read_EEPROM_PcPart();
			//DisplayOneInt(0x88,g_VGAFullOrPartValue);
			
			Read_EEPROM_Byte(E2ROM_DeviceID_Addr, &g_DeviceID);
		#if(MATHINE_TYPE==XP_530)
			Read_EEPROM_Byte(E2ROM_ADV7441A_INPUT_ADDR, &g_Adv7441CurrentInput);
		#endif


		#ifdef POWER_ON_PWD
			Read_EEPROM_Byte(E2ROM_HowLongTime_ADDR,&g_howlongtosave);
			Read_EEPROM_U16(E2ROM_Log_Num_ADDR, &g_NumSaved);
		#endif
			
	
			
	//20170213 lyg
	Read_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR ,&g_UserTry_show_onoff);
	if(g_UserTry_show_onoff>1) //非正常设置状态，正常设置状态0表示关闭，1表示打开
	{
		g_UserTry_show_onoff=0;
		Write_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR,g_UserTry_show_onoff);	
	}
	Read_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR ,&g_UserTry_show_time);
	if(g_UserTry_show_time>60 ||g_UserTry_show_time<10)  //演示时间最少10分钟，最多60分钟
	{
		g_UserTry_show_time=15;    
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
		g_UserTry_once_time=8;                          //单次时间为8小时
		Write_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR,g_UserTry_once_time);	
	}	
	m24xx_read(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);  
	g_UserTry_OnceEndTime=(g_UserTry_once_end_time[0]<<24)+(g_UserTry_once_end_time[1]<<16)+(g_UserTry_once_end_time[2]<<8)+(g_UserTry_once_end_time[3]);

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
		/*g_UserTry_once_Tel[0]='4';
		g_UserTry_once_Tel[1]='0';
		g_UserTry_once_Tel[2]='0';
		g_UserTry_once_Tel[3]='0';
		g_UserTry_once_Tel[4]='0';
		g_UserTry_once_Tel[5]='0';
		g_UserTry_once_Tel[6]='3';
		g_UserTry_once_Tel[7]='0';
		g_UserTry_once_Tel[8]='8';
		g_UserTry_once_Tel[9]='3';
		g_UserTry_once_Tel[10]=' ';
		*/
		strcpy(g_UserTry_once_Tel,"4000003083");
		//g_UserTry_once_Tel[]={'4','0','0' ,'0','0','0' ,'3','0','8','3',' ',' ','\0'};//only used for char array initial
		m24xx_write(E2ROM_ONCE_Telephone_ADDR,g_UserTry_once_Tel,g_UserTry_once_TeleNumVal+2); //此处将最后一个0也存储
	}
	
}


//u8 ERR_FLAG=0;iic configure mulit-time
char	OutDevicesInit(void)
{
		U8 iic_comm_err=0;
		U8 i=0; 
		U8 const DELAY_nMS=50;
		//iic_comm_err = InitCAT6613();

		do
		{
			delay_ms(DELAY_nMS);
			iic_comm_err = InitCAT6613();
			i++;
			if(iic_comm_err==0)
				break;
			if(i>IIC_CONFIG_COMM_TRY_TIME)
				{
				clearlcd();
				chinesedisplay(0x80, "6613 comm err");
				chinesedisplay(0x90, "connect factory");
				break;
				}
			
		}
		while(iic_comm_err==1);//配置成果

		
		iic_comm_err=0;
		i=0;
		do
		{	delay_ms(DELAY_nMS);
			iic_comm_err = InitP412();
			i++;
			if(iic_comm_err==0)
				break;
			
			if(i>IIC_CONFIG_COMM_TRY_TIME)
				{
				clearlcd();
				chinesedisplay(0x80, "412 comm err");
				chinesedisplay(0x90, "connect factory");
				break;
				}
			
		}
		while(iic_comm_err==1);//配置成果

	return 1;	
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



void	WizardOrRecoverMode(void)
{
//	U16 HSize=0,VSize=0;	
//	U8 image=MAIN;
	if(IsHasSave(DEFAULT_MODE))
	{
		g_CurrentState=State_MainMenu_Menu;
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

		
		Updata_Execute((U8*)EEPROMMemRegU8,(U16*)EEPROMMemRegU16);


		//等待图像稳定
		Long_Delay_with_Flicker_OutVideo_after_VideoStable();

		upDateFullOrPart();
		//DisplayOneInt(0x89,g_VGAFullOrPartValue);

		
		ReturnMainWinDontChangeState();
		g_CurrentState=State_MainMenu_Menu;
	}
	else
	{
					
			if(!CheckEEPRom())//在没有设置向导前，检查一次EEPROM
			{
				g_CurrentState=State_MainMenu_Not_Menu;
				clearlcd();
				chinesedisplay(0x80, g_EEPROMErrorStr);
				chinesedisplay(0x90, g_ContactTel1);
			}
			else
			{
				WizardSet();
				g_WizadFlag=__TRUE;
				g_powerOnNoChangePartofFullFlag=FALSE;
			}
		
	}

}



void LoadUserMode(void)
{
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
	
}





void LED_Indicate_at_Wizard()
	{

		if(g_CurrentState==State_NewMode_Set)
				{
				//#ifndef EC11_BUTTON
					Start_LED_Flashing(3);
					g_FlashLedKey[0]=OK_KEY;
					g_FlashLedKey[1]=ESC_KEY;
					g_FlashLedKey[2]=NUM2_KEY;
				//#endif
				}
			else if(Check_OK_Cancel_State())
				{
					#ifndef EC11_BUTTON
						//LED_OK_Cancel_Flashing();
						Start_LED_Flashing(2);
						g_FlashLedKey[0]=OK_KEY;
						g_FlashLedKey[1]=ESC_KEY;
					#endif
				}
			else
				{
					g_WizadFlag=__FALSE;
				}

			//				else if(g_OKCancelLedFlashingFlag)
//				{
//					g_OKCancelLedFlashingFlag=__FALSE;
//					g_FlashLedFlag=__FALSE;
//				}
				
				//if(g_FlashLedFlag)
				//	More_LED_Flashing();
	}

/*
BOOL IsScreenChanged()
{
	if(g_MainScalerHSize!=g_PanelWidth||g_MainScalerVSize!=g_PanelHeight)
	{
		return __TRUE;
	}
	else
		return __FALSE;
}
*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
