/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: Uart.c
 [Date]:        08-May-2012
 [Comment]:
   Uart  file.
 [Reversion History]:
*******************************************************************************/
#include "header.h"
#include "stm32f10x.h" 	 
#include "stm32_eval.h"
#include <stdio.h>
#include "Queue.h"

#define _UART_C


static U8 RevPC_Data_Buf[MSG_LENGTH];
static U8 RevPC_Data_Idx=0;

#if(XP_TO_ID==XP_TO_6M16)
U8 g_RevUart2Data[4]={0};
U8 g_QueueInCount=0;
extern U8 g_LoadModeFlag;
//static U8 g_MstarID=0;
static U8 FourByte_IndexIn=0;
static U8 TwoByte_IndexIn=0;
#else
U8 g_RevUart2Data[12]={0};
U8 g_RevUartIdx=0;
static U8 chSamePIPIdx=0;
#endif

#ifdef HOT_BACKUP
static U8 chMainSwitchIdx=0,chSubSwitchIdx=0;
#endif

//extern struct rtc_time systmtime;


void SwitchInputMethods(_InputSourceType inputSrc,U8 image,BOOL delayFlag)
{	
#if(XP_TO_ID==XP_TO_FLI32626)
	if(g_SwitchEffectValue==FadeSwitchIdx||g_SwitchEffectValue==QuickSwitchIdx) 
	{
		if(g_NextPointImage==MAIN)
		{
			if(CheckDualSource(g_SubInput-1, inputSrc-1))
			{
				SetInputSource(inputSrc,MAIN);
				if(delayFlag)
					delay(4);
				DealSwapKey();
			}
			else if(CheckDualSource(g_MainInput-1, inputSrc-1))
			{
				g_CurrentState=State_MainMenu_Not_Menu;
				clearlcd();
				SetInputSource(inputSrc,SUB);
				DisplayLCDInputSrc(0x81,"      --> ");
			}
			else
			{
				LEDWarning(InputSrcToNumKey(inputSrc));
			}
		}
		else if(g_NextPointImage==SUB)
		{
			if(CheckDualSource(g_MainInput-1, inputSrc-1))
			{
				SetInputSource(inputSrc,SUB);
				if(delayFlag)
					delay(4);
				DealSwapKey();				
			}
			else if(CheckDualSource(g_SubInput-1, inputSrc-1))
			{
				g_CurrentState=State_MainMenu_Not_Menu;
				clearlcd();
				SetInputSource(inputSrc,MAIN);
				DisplayLCDInputSrc(0x81," <--      ");				
			}
			else//输入的信号(YPbPr)与主子画面都不能双通道
			{
				LEDWarning(InputSrcToNumKey(inputSrc));
			}
		}
	}
	else if(g_SwitchEffectValue==FuseSwitchIdx)
	{
		if(image==MAIN&&CheckDualSource(g_SubInput-1, inputSrc-1))
			SetInputSource(inputSrc,MAIN);
		else if(image==SUB&&CheckDualSource(g_MainInput-1, inputSrc-1))
			SetInputSource(inputSrc,SUB);
		else
			return;
		g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		DisplayLCDInputSrc(0x81," +  ");
		LCDDisplayEffectInfo();
	}
	else
#endif
	{
		//切换到备份源
		g_ActiveImageValue=image;
		SetInputSourceSwitch(inputSrc);
	}

}

#ifdef HOT_BACKUP
void SwitchBackup(U8 image)
{
	int i;
	_InputSourceType input=INPUTSOURCE_NONE,BackupInput=INPUTSOURCE_NONE;
	if(image==MAIN)
		input=g_MainInput;
	else if(image==SUB)
		input=g_SubInput;
	if(input!=INPUTSOURCE_NONE)
	{
		//获得用户设置信号的备份源
		for(i=0;i<8;i++)
		{
			if(input==g_HotBackupInput[i][0]&&g_HotBackupInput[i][1]!=INPUTSOURCE_NONE)
			{
				BackupInput=g_HotBackupInput[i][1];
			}
				
		}	
		//如果没有找到设置的备份，获得默认信号的备份源	
		if(BackupInput==INPUTSOURCE_NONE)
		{
			switch(input)
			{
				case INPUTSOURCE_CVBS:
					BackupInput=INPUTSOURCE_CVBS2;
					break;
				case INPUTSOURCE_CVBS2:
					BackupInput=INPUTSOURCE_CVBS3;
					break;
				case INPUTSOURCE_CVBS3:
					BackupInput=INPUTSOURCE_YPBPR;
					break;
				case INPUTSOURCE_YPBPR:
					BackupInput=INPUTSOURCE_VGA;
					break;
				case INPUTSOURCE_VGA:
					BackupInput=INPUTSOURCE_DVI;
					break;
				case INPUTSOURCE_DVI:
					BackupInput=INPUTSOURCE_HDMI;
					break;
				case INPUTSOURCE_HDMI:
					BackupInput=INPUTSOURCE_SDI;
					break;
			#if(XP_TO_ID==XP_TO_FLI32626)
				case INPUTSOURCE_SDI:
					BackupInput=INPUTSOURCE_CVBS;
					break;
			#endif
			}
		}
	#if(XP_TO_ID==XP_TO_FLI32626)
		//淡入淡出或无缝切换
		if(g_MutiWindow==SAMEPIPIMAGE&&chSamePIPIdx<8) 
		{			
			SwitchInputMethods(BackupInput,0,__FALSE);
			chSamePIPIdx++;			
		}
		//单画面或者双画面
		else
	#endif
		{
			if((g_MutiWindow==SINGLEIMAGE||g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE)
				&&((image==MAIN&&chMainSwitchIdx<8)||(image==SUB&&chSubSwitchIdx<8)))
			{
				SwitchInputMethods(BackupInput,image,__FALSE);
				//切换到备份源
				if(image==MAIN)
					chMainSwitchIdx++;
				else if(image==SUB)
					chSubSwitchIdx++;
			}
			else//所有的通道以检测一遍
			{
				clearlcd();
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80,g_AllChannelNoSYNCStr);
				else if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, "All Channel No Sync!");
				if(g_CurrentState==State_NewMode_Set)
					WizardSet();
				
			}
		}
	}
}

BOOL DetectNoSignalImage(U8 *IorII)
{
	U8 image=0;
#if(XP_TO_ID==XP_TO_FLI32626)
	if(!CheckProtocolStart(g_RevUart2Data)||!CheckSum(g_RevUart2Data))
		return __FALSE;	
	//获得没有信号的信号源
	if(g_RevUart2Data[EVENTID_IDX]==0x25)
	{
		if(g_RevUart2Data[PARA1_IDX]==0x09)
		{
			image=MAIN;
		}
		else if(g_RevUart2Data[PARA1_IDX]==0x0a)
		{
			chMainSwitchIdx=0;
			chSamePIPIdx=0;
			if(g_CurrentState==State_NewMode_Set)
				WizardSet();
			return __FALSE;
		}
	}
	else if(g_RevUart2Data[EVENTID_IDX]==0x26&&g_MutiWindow!=SINGLEIMAGE)
	{
		if(g_RevUart2Data[PARA1_IDX]==0x09)
		{
			image=SUB;
		}
		else if(g_RevUart2Data[PARA1_IDX]==0x0a)
		{
			chSubSwitchIdx=0;
			chSamePIPIdx=0;
			if(g_CurrentState==State_NewMode_Set)
				WizardSet();
			return __FALSE;
		}
	}
	else
		return __FALSE;
	if(g_MutiWindow==SAMEPIPIMAGE)
	{
		if(g_NextPointImage==image)
			return __FALSE;
	}
#else
	if(g_RevUart2Data[0]==0xFF&&g_RevUart2Data[2]==0)
	{
		if(g_RevUart2Data[1]==0x45&&g_RevUart2Data[3]==0xBC)
		{
			chMainSwitchIdx=0;
			if(g_CurrentState==State_NewMode_Set)
				WizardSet();
			return __FALSE;
		}
		else if(g_RevUart2Data[1]==0x46&&g_RevUart2Data[3]==0xBB)
		{
			image=MAIN;
		}
		else if(g_RevUart2Data[1]==0x55&&g_RevUart2Data[3]==0xAC)
		{
			chSubSwitchIdx=0;
			if(g_CurrentState==State_NewMode_Set)
				WizardSet();
			return __FALSE;
		}
		else if(g_RevUart2Data[1]==0x56&&g_RevUart2Data[3]==0xAB)
		{
			image=SUB;
		}
	}
#endif
	if(IorII)
	{
		*IorII=image;
	}
	return __TRUE;
}

void Detect_Current_Signal()
{
	if(g_EnableHotBackupFlag&&DetectNoSignalImage(&g_ActiveImageValue))
	{
		if(g_PoweronInputSendFlag)
			SwitchBackup(g_ActiveImageValue);
		else//表示32626开机默认的DVI 没信号
		{
			g_PoweronInputSendFlag=__TRUE;  
			if(g_MainInput==INPUTSOURCE_DVI)  //刚开机判断当前信号是不是DVI
				SwitchBackup(g_ActiveImageValue);
			//开机时DVI 无信号，但是开机是别的输入
		}
	#if(XP_TO_ID==XP_TO_6M16)
		g_FourByteFlag=__FALSE;
	#endif
	}
}
#endif

#ifdef XP360

////控制工控机的开机和关机的管脚B7
BOOL STM32_CTR_PC_OnOff(void)
{	

	GPIO_SetBits(PC_POWER_ON_GPIO,  PC_POWER_ON_PIN);//GPIO_Pin_7);
	delay_ms(2000);
	//启动工控机
	GPIO_ResetBits(PC_POWER_ON_GPIO, PC_POWER_ON_PIN);//GPIO_Pin_7);
	delay_ms(2000);////(1000)
	GPIO_SetBits(PC_POWER_ON_GPIO,  PC_POWER_ON_PIN);//GPIO_Pin_7);
	
	PC_POWER_ONOFF_LCD_DISPLAY();

	return 1;

}
#endif

void STM32_GPIO_Configuration(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //重映射时钟	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2
#ifdef STM32F103RB
		|RCC_APB1Periph_USART3
#endif
		,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
#ifdef STM32F103RB
		| RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE
#endif
		, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //将IO 重映射到PB3xxxxxxxxxxxxx
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);

#if(ENABLE_SOFTI2C)
	GPIO_InitStructure.GPIO_Pin = EEPROM_SCL_PIN|EEPROM_SDA_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 
#endif
	
	GPIO_InitStructure.GPIO_Pin =KB_SCK_PIN|LED_IN_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin =LED_SRCK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_SRCK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KB_PLOAD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KB_PLOAD_GPIO, &GPIO_InitStructure);
	
	// KEY IN  
	GPIO_InitStructure.GPIO_Pin = KB_SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KB_SDOUT_GPIO, &GPIO_InitStructure);   

#ifdef EC11_BUTTON
	GPIO_InitStructure.GPIO_Pin = BMA_PIN|BMB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BMA_GPIO, &GPIO_InitStructure);   
 #ifdef TWO_EC11_BOARD
	GPIO_InitStructure.GPIO_Pin = BMA2_PIN|BMB2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BMA2_GPIO, &GPIO_InitStructure);   
 #endif
#endif

	

	GPIO_InitStructure.GPIO_Pin =LED_LAMP_GREEN_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_LAMP_GREEN_GPIO, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = PC_AUD_SEL0_PIN|PC_AUD_SEL1_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PC_AUD_SEL0_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =VGA_SEL_H_PIN;   //VGA_SEL_H
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VGA_SEL_H_GPIO, &GPIO_InitStructure);

	
	
#ifdef STM32F103RB
	GPIO_InitStructure.GPIO_Pin =DVI_SEL_PIN;   //DVI SEL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(DVI_SEL_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =HDMI_SEL_PIN;   //HDMI SEL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(HDMI_SEL_GPIO, &GPIO_InitStructure);
#endif
	
	
	GPIO_InitStructure.GPIO_Pin =VGA_SEL_PIN;   //VGA_SEL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VGA_SEL_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED_RCK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_RCK_GPIO, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_CS_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LCD_CLK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_CLK_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LCD_DAT_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_DAT_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_BK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_BK_GPIO, &GPIO_InitStructure);
	 //USART1     PA9 PA10 
	 //USART2     PA2 PA3            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_9;             //管脚9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //TX初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;            //管脚10     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //RX初始化
	
////#ifdef STM32F103RB	
	//USART3     PD8 PD9         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  
////#endif

////	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =PC_POWER_ON_PIN;////GPIO_Pin_5;//GPIO_Pin_7;   //启动工控机  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(PC_POWER_ON_GPIO, &GPIO_InitStructure);




}


void STM32_USART_Config(void)
{ 	
	USART_InitTypeDef USART_InitStructure;				  
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
//	STM_EVAL_COMInit(COM1, &USART_InitStructure);                                 //启动串口

	USART_Init(USART1, &USART_InitStructure);                                         //初始化
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//接收中断允许
	USART_Cmd(USART1, ENABLE);  
	
	USART_Init(USART2, &USART_InitStructure); 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE); 
#ifdef STM32F103RB
	USART_Init(USART3, &USART_InitStructure); 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);                                                     //启动串口
#endif
}



void UART1_SendByte(U8 buf)
{	
	USART1->DR = buf;				// send data
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}  ; 	// waiting for completion
}

void UART1_SendBuf(U8 *buf, U32 number)   
{
 	U8 *s = NULL;
	U32 n = number;
    	
	s = buf;
	while (n-- > 0)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}  ; 	// waiting for completion
		USART1->DR = *s++;				// send data
	}
}

void UART3_SendByte(U8 buf)
{	
	USART3->DR = buf;				// send data
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}  ; 	// waiting for completion
}

void UART3_SendBuf(U8 *buf, U32 number)   
{
#ifdef STM32F103RB
 	U8 *s = NULL;
	U32 n = number;
    	
	s = buf;
	while (n-- > 0)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion
		USART3->DR = *s++;				// send data
	}
#endif
}

void UART2_SendByte(U8 buf)
{	
	USART2->DR = buf;	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion
}

void UART2_SendBuf(U8 *buf, U32 number)   
{
#if(XP_TO_ID==XP_TO_6M16)
	U32 iCount=0x3000000;   //time out
#endif
	U8 *s = NULL;
	U32 n = number;
	U8 noxBuf;
 	s = buf;  
	while (n-- > 0)
	{	
		noxBuf=*s;//^g_MstarID;
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion	
		USART2->DR = noxBuf;	
	#if(XP_TO_ID==XP_TO_6M16)
		if(g_LoadModeFlag)
		{
			g_QueueInCount++;			// send data
			
			if(g_QueueInCount>FULLCOUNT)
			{
				g_QueueFullFlag=__TRUE;
				while(g_QueueFullFlag&&iCount>0)
				{
					iCount--;
				}
				g_QueueInCount=0;
				while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}  ;	// send overflow byte
				USART2->DR = noxBuf;				
				g_QueueInCount++;
			}
		}
	#endif
		s++;
	}
}


void DealUart2Data(U8 ucTmp)
{
#if(XP_TO_ID==XP_TO_6M16)
	if(((ucTmp&0xff)==0xff)&&(FourByte_IndexIn ==0) )
	{
		g_RevUart2Data[FourByte_IndexIn]=ucTmp;
		FourByte_IndexIn=1;
		
	}
	else if(FourByte_IndexIn!=0)
	{
		g_RevUart2Data[FourByte_IndexIn]=ucTmp;
		FourByte_IndexIn=(FourByte_IndexIn+1)%4;
		if(FourByte_IndexIn==0)
		{
			g_FourByteFlag=__TRUE;
		#ifdef HOT_BACKUP
			Detect_Current_Signal();
		#endif
		}
	}
	else if(((ucTmp&0xf0)==0xf0)&&(TwoByte_IndexIn ==0) )
	{
		g_RevUart2Data[TwoByte_IndexIn]=ucTmp;
		TwoByte_IndexIn=1;		
	}
	else if(TwoByte_IndexIn!=0)
	{
		g_RevUart2Data[TwoByte_IndexIn]=ucTmp;
		TwoByte_IndexIn=(TwoByte_IndexIn+1)%2;	
		if(TwoByte_IndexIn==0)
		{
			g_TwoByteFlag=__TRUE;
		}	
	}
	else if((ucTmp&0x80)==0x80)
	{
		if(ucTmp==0xE2)
		{		
			g_QueueFullFlag=__FALSE;
			return;
		}
		else if(ucTmp==0xE3)
		{
			g_PowerOnFlag=__TRUE;
//			g_MstarIDFlag=__TRUE;
			g_MainboardVersionFlag=__TRUE;
			return;
		}
//		else if(g_MstarIDFlag)
//		{
//			g_MstarIDFlag=__FALSE;
//			g_MstarID=ucTmp;
//			return;
//		}
		g_RevUart2Data[0]=ucTmp;
		g_OneByteFlag=__TRUE;
	}
	if(g_MainboardVersionFlag)
	{
		g_MainboardVersionFlag=__FALSE;
		g_MainboardHRevision=(ucTmp/10);
		g_MainboardLRevision=(ucTmp%10);
		return;
	}
#elif(XP_TO_ID==XP_TO_FLI32626)
	g_RevUart2Data[g_RevUartIdx]=ucTmp;
	g_RevUartIdx++;

	if(g_RevUartIdx==12)
	{
	#ifdef HOT_BACKUP
		Detect_Current_Signal();
	#endif
		if(!g_PowerOnFlag)
		{
			if(g_RevUart2Data[EVENTID_IDX]==RETURN_EVENT&&g_RevUart2Data[PARA1_IDX]==0x06
			&&g_RevUart2Data[PARA2_IDX]==0x06&&g_RevUart2Data[PARA3_IDX]==0x06)
			{
				g_MainboardHRevision=g_RevUart2Data[PARA4_IDX];
				g_MainboardLRevision=g_RevUart2Data[PARA5_IDX];
				g_PowerOnFlag=__TRUE;
				return;
			}
		}
	}
	g_RevUartIdx%=12;
	g_ReceMsgFlag=__TRUE;
#endif
}


U8 XOR_CheckSum(U8 *data)
{
	int i;
	U8 Checksum=0;
	for(i=1;i<8;i++)
	{
		Checksum^=data[i];
	}
	return Checksum;
}


void SendMsg_ChangeOne(U8 no,U8 value,_CommunicationType type)
{
	int i;
	U8 RevData[10];
	
	for(i=0;i<10;i++)
	{
		RevData[i]=RevPC_Data_Buf[i];
	}
	RevData[no]=value;
	RevData[8]=XOR_CheckSum(RevData);
	switch(type)
	{
		case Communication_USART1:
			UART1_SendBuf(RevData,MSG_LENGTH);
			break;
		case Communication_USART3:
			UART3_SendBuf(RevData,MSG_LENGTH);
			break;
	#ifdef TCP_IP
		case Communication_Ethernet:
			Ethernet_SendBuf(RevData,MSG_LENGTH);
			break;
	#endif
	}
}



void SendMsg_ReadWindowPara(U16 HValue,U16 VValue,_CommunicationType type)
{
	int i;
	U8 RevData[10];
	
	for(i=0;i<10;i++)
	{
		RevData[i]=RevPC_Data_Buf[i];
	}
	RevData[4]=HValue&0xff;
	RevData[5]=HValue>>8;
	RevData[6]=VValue&0xff;
	RevData[7]=VValue>>8;
	RevData[8]=XOR_CheckSum(RevData);
	switch(type)
	{
		case Communication_USART1:
			UART1_SendBuf(RevData,MSG_LENGTH);
			break;
//		case Communication_USART3:
//			UART3_SendBuf(RevData,MSG_LENGTH);
//			break;
	#ifdef TCP_IP
		case Communication_Ethernet:
			Ethernet_SendBuf(RevData,MSG_LENGTH);
			break;
	#endif
	}
}

void ReturnMsg_ChangeOne(U8 no,U8 value)
{
	if(g_ComTypeFlag==Communication_USART1)
		SendMsg_ChangeOne(no,value,Communication_USART1);
//	else if(g_ComTypeFlag==Communication_USART3)
//		SendMsg_ChangeOne(no,value,Communication_USART3);
#ifdef TCP_IP
	else if(g_ComTypeFlag==Communication_Ethernet)
		SendMsg_ChangeOne(no,value,Communication_Ethernet);
#endif
}

void ReturnMsg_ReadWindowPara(U16 HValue,U16 VValue)
{
	if(g_ComTypeFlag==Communication_USART1)
		SendMsg_ReadWindowPara(HValue,VValue,Communication_USART1);
//	else if(g_ComTypeFlag==Communication_USART3)
//		SendMsg_ReadWindowPara(HValue,VValue,Communication_USART3);
#ifdef TCP_IP
	else if(g_ComTypeFlag==Communication_Ethernet)
		SendMsg_ReadWindowPara(HValue,VValue,Communication_Ethernet);
#endif
}

void DealPC_Data()
{
	U8 device_ID=RevPC_Data_Buf[1];
	U8 cmd,byte3,byte4,byte5,byte6,byte7;
	cmd=(RevPC_Data_Buf[2]&0x7F);
	byte3=RevPC_Data_Buf[3];
	byte4=RevPC_Data_Buf[4];
	byte5=RevPC_Data_Buf[5];
	byte6=RevPC_Data_Buf[6];
	byte7=RevPC_Data_Buf[7];
	
	if(device_ID==0||device_ID==g_DeviceID)
	{
		switch(cmd)
		{
			case 0x01:
				switch(byte3)
				{
					case 0x04:
						g_PCSendFlag=__FALSE;
						g_PCControlFlag=__FALSE;
						break;
					case 0x01:
						#ifdef INT_DELAY_PC_CTL
							g_PCSendFlag=__TRUE;
						#endif
							g_PCControlFlag=__TRUE;
						break;
					case 0x02://VGA自动调整
						AutoAdjustVGA();
						break;
					case 0x03:
						g_DeviceID=byte4;
						Write_EEPROM_Byte(E2ROM_DeviceID_Addr, g_DeviceID);						
						SendMsg_ChangeOne(4,(g_DeviceID+1),Communication_USART3);  
						if((RevPC_Data_Buf[2]&0x80)==0x80)//需要返回数据,执行完后返回原数据
						{
							if(g_ComTypeFlag==Communication_USART1)
								UART1_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
						#ifdef TCP_IP
							else if(g_ComTypeFlag==Communication_Ethernet)
								Ethernet_SendBuf(RevPC_Data_Buf,MSG_LENGTH);
						#endif
						}
						return;
					default:
						return;
				}
				break;
			case 0x02:
				switch(byte3)
				{
					case 0x01:
						SetSingleImage();
						break;
					case 0x02:
						if(g_MutiWindow!=PIPIMAGE)
							SetPIPImage();
						else
							Deal_PIP_Pos(PIP_LT);							
						break;
					case 0x03:
						if(g_MutiWindow!=PIPIMAGE)
							SetPIPImage();
						Deal_PIP_Pos(PIP_RT);
						break;
					case 0x04:
						if(g_MutiWindow!=PIPIMAGE)
							SetPIPImage();
						Deal_PIP_Pos(PIP_MC);
						break;
					case 0x05:
						if(g_MutiWindow!=PIPIMAGE)
							SetPIPImage();
						Deal_PIP_Pos(PIP_LB);
						break;
					case 0x06:	
						if(g_MutiWindow!=PIPIMAGE)
							SetPIPImage();
						Deal_PIP_Pos(PIP_RB);
						break;
					case 0x07:
						SetPBPImage();					
						break;
					case 0x08:
						{
							_InputSourceType inputSwap;
							inputSwap=g_MainInput;
							SetInputSource(g_SubInput,MAIN);
							SetInputSource(inputSwap,SUB);
						}
						break;
					case 0x09:
						g_MainFreezeFlag=FREEZE;
						SetFreezeImage(MAIN,FREEZE);
						break;
					case 0x0A:
						g_MainFreezeFlag=UNFREEZE;
						SetFreezeImage(MAIN,UNFREEZE);
						break;
					case 0x0B:
						g_SubFreezeFlag=FREEZE;
						SetFreezeImage(SUB,FREEZE);
						break;
					case 0x0C:
						g_SubFreezeFlag=UNFREEZE;
						SetFreezeImage(SUB,UNFREEZE);
						break;
				#if(XP_TO_ID==XP_TO_FLI32626)
					case 0x0D:
						g_CurrentState=State_Fade_Switch_Mode;
						GetSwitchStateValue();
						break;
					case 0x0E:
						g_CurrentState=State_Quick_Switch_Mode;
						GetSwitchStateValue();
						break;
					case 0x0F:
						g_NextPointImage=SUB;
						SetWhichImageOnTop(MAIN);
						break;
					case 0x10:
						g_NextPointImage=MAIN;
						SetWhichImageOnTop(SUB);
						break;
					case 0x11:
						g_CurrentState=State_Fuse_Switch_Mode;
						GetSwitchStateValue();
						break;
					case 0x12:
						g_FadeTime=1;
						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
						break;
					case 0x13:
						g_FadeTime=2;
						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
						break;
					case 0x14:
						g_FadeTime=3;
						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
						break;				
				#endif
					default:
						return;
				}
				break;
			case 0x03:
				if(byte3==1)
					g_ActiveImageValue=MAIN;
				else if(byte3==2)
					g_ActiveImageValue=SUB;
				switch(byte4)
				{					
					case 1:
					SetInputSource(INPUTSOURCE_CVBS,g_ActiveImageValue);
					break;
					case 2:
					SetInputSource(INPUTSOURCE_CVBS2,g_ActiveImageValue);
					break;
					case 3:
					SetInputSource(INPUTSOURCE_CVBS3,g_ActiveImageValue);
					break;
					case 9:
					SetInputSource(INPUTSOURCE_YPBPR,g_ActiveImageValue);
					break;
					case 5:
					SetInputSource(INPUTSOURCE_VGA,g_ActiveImageValue);
					break;
					case 6:
					SetInputSource(INPUTSOURCE_DVI,g_ActiveImageValue);
					break;
				#if(ENABLE_HDMI)
					case 7:
					SetInputSource(INPUTSOURCE_HDMI,g_ActiveImageValue);
					break;
					case 8:	
					SetInputSource(INPUTSOURCE_HDMI2,g_ActiveImageValue);
					break;
				#endif
				
				#if(XP_TO_ID==XP_TO_FLI32626)
					case 9:	
					SetInputSource(INPUTSOURCE_SDI,g_ActiveImageValue);
					break;
				#endif
					case 10:	
					SetInputSource(INPUTSOURCE_VGA2,g_ActiveImageValue);
					break;
					case 11:	
					SetInputSource(INPUTSOURCE_DVI2,g_ActiveImageValue);
					break;
					default:
						return;

				}
				break;
			case 0x04:
				switch(byte3)
				{
					case 1:
					SetAudioPara(MUTE);
					break;
					case 2:
					g_ActiveAudioWin=MAIN;
					SetAudioPara(MAINVOL);
					break;
					case 3:
					g_ActiveAudioWin=SUB;
					SetAudioPara(SUBVOL);
					break;
					case 4:
					g_VolumeValue=byte4;
					SetVolumeValue(g_VolumeValue);
					break;
					default:
						return;
				}
				break;
			case 0x05:
				switch(byte3)
				{
					case 1:
					g_OutputFormatIndex=INDEX640X480;
					break;
					case 2:
					g_OutputFormatIndex=INDEX800X600;
					break;
					case 3:
					g_OutputFormatIndex=INDEX1024X768;
					break;
					case 4:
					g_OutputFormatIndex=INDEX1280X1024;
					break;
					case 5:
					g_OutputFormatIndex=INDEX1366X768;
					break;
					case 6:
					g_OutputFormatIndex=INDEX1440X900;
					break;
					case 7:
					g_OutputFormatIndex=INDEX1600X1200;
					break;
					case 8:
					g_OutputFormatIndex=INDEX1680X1050;
					break;
					case 9:
					g_OutputFormatIndex=INDEX1920X1080;
					break;
					case 10:
					g_OutputFormatIndex=INDEX1600X900;
					break;
					case 11:
					g_OutputFormatIndex=INDEX1280X720;
					break;
					case 12:
					g_OutputFormatIndex=INDEX1280X768;
					break;
					case 13:
					g_OutputFormatIndex=INDEX1280X800;
					break;
					case 14:
					g_OutputFormatIndex=INDEX1280X960;
					break;
				
				
					default:
						return;
				}
				SetOutputFormat(g_OutputFormatIndex);
				LoadOutputResolution();
				PanelToScreenPara();
				break;
			case 0x06:
				if(byte3==1)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ScreenPara_Pos_Value,(byte5<<8|byte4));
					SetCurrentStateValue(KEY_ENTER,State_ScreenPara_Pos_Value,0);
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ScreenPara_Pos_Value,(byte7<<8|byte6));
					SetCurrentStateValue(KEY_ENTER,State_ScreenPara_Pos_Value,0);
				}
				else if(byte3==2)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ScreenPara_Size_Value,(byte5<<8|byte4));
					SetCurrentStateValue(KEY_ENTER,State_ScreenPara_Size_Value,0);
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ScreenPara_Size_Value,(byte7<<8|byte6));
					SetCurrentStateValue(KEY_ENTER,State_ScreenPara_Size_Value,0);
				}
				else
					return;
				break;
			case 0x07:
				if((byte3&0x80)==0)
				{
					g_ActiveImageValue=MAIN;
				}
				else if((byte3&0x80)==0x80)
				{
					g_ActiveImageValue=SUB;
				}
				if((byte3&0x7F)==1)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ScalerPara_Pos_Value,(byte5<<8|byte4));
					SetCurrentStateValue(KEY_ENTER,State_ScalerPara_Pos_Value,0);
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ScalerPara_Pos_Value,(byte7<<8|byte6));
					SetCurrentStateValue(KEY_ENTER,State_ScalerPara_Pos_Value,0);
				}
				else if((byte3&0x7F)==2)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ScalerPara_Size_Value,(byte5<<8|byte4));
					SetCurrentStateValue(KEY_ENTER,State_ScalerPara_Size_Value,0);
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ScalerPara_Size_Value,(byte7<<8|byte6));
					SetCurrentStateValue(KEY_ENTER,State_ScalerPara_Size_Value,0);
				}
				else
					return;
				break;
			case 0x08:
				if((byte3&0x80)==0)
				{
					g_ActiveImageValue=MAIN;
				}
				else if((byte3&0x80)==0x80)
				{
					g_ActiveImageValue=SUB;
				}
				if((byte3&0x7F)==1)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ZoomPara_Pos_Value,(byte5<<8|byte4));
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ZoomPara_Pos_Value,(byte7<<8|byte6));
				}
				else if((byte3&0x7F)==2)
				{
					g_H_VFlag=__TRUE;
					SetCurrentStateValue(KEY_UP,State_ZoomPara_Size_Value,(byte5<<8|byte4));
					g_H_VFlag=__FALSE;
					SetCurrentStateValue(KEY_UP,State_ZoomPara_Size_Value,(byte7<<8|byte6));
				}
				else if((byte3&0x7F)==3)
				{
					DealFullOrPartKey();
				}
				else
					return;
				break;
			case 0x09:
				switch(byte3)
				{
					case 1:
					SetCurrentStateValue(KEY_UP,State_ColorPara_Brightness_Value,byte4);
					break;
					case 2:
					SetCurrentStateValue(KEY_UP,State_ColorPara_Contrast_Value,byte4);
					break;
				#if(XP_TO_ID==XP_TO_FLI32626)
					case 3:
					SetCurrentStateValue(KEY_UP,State_ColorPara_Saturation_Value,byte4);
					break;
				#endif
					default:
						return;
				}
				break;
			case 0x0A:
				switch(byte3)
				{
					case 1:
					TimeSet[0]=2000+(byte4&0x7f);	
					TimeSet[1]=((byte5&0x07)<<1)|(( byte4&0x80)>>7);
					TimeSet[2]= (byte5&0xf8)>>3;
					TimeSet[3]=(byte6&0x1f);	
					TimeSet[4]=((byte7&0x07)<<3)|(( byte6&0xE0)>>5);
					TimeSet[5]= (byte7&0xf8)>>2;
					STM32_Time_Adjust();
					break;
					case 2://不提示，直接覆盖
					if(!Save(byte4))//否则不返回数据，说明该模式没有保存
						return;
					break;
					case 3:
					if(IsHasSave(byte4))
						Updata_Execute(EEPROMMemRegU8,EEPROMMemRegU16);
					else  //否则不返回数据，说明该模式没有保存
						return;
					break;
					case 4:
					InputEE[INPUT_HOUR]=byte4;
					InputEE[INPUT_MIN]=byte5;
					InputEE[INPUT_SRC]=byte6;
					InputEE[INPUT_WEEK]=byte7;
					SaveInputScheduleToEEPROM(MODE1);
					FromEEInitInputSche(MODE1);  //将InputEE赋值给当前执行的各个模式
					break;
					case 5:
					InputEE[INPUT_HOUR]=byte4;
					InputEE[INPUT_MIN]=byte5;
					InputEE[INPUT_SRC]=byte6;
					InputEE[INPUT_WEEK]=byte7;
					SaveInputScheduleToEEPROM(MODE2);
					FromEEInitInputSche(MODE2);  //将InputEE赋值给当前执行的各个模式
					break;
					case 6:
					InputEE[INPUT_HOUR]=byte4;
					InputEE[INPUT_MIN]=byte5;
					InputEE[INPUT_SRC]=byte6;
					InputEE[INPUT_WEEK]=byte7;
					SaveInputScheduleToEEPROM(MODE3);
					FromEEInitInputSche(MODE3);  //将InputEE赋值给当前执行的各个模式
					break;
					case 7:
					CancelInputScheduleMode1();
					break;
					case 8:
					CancelInputScheduleMode2();
					break;
					case 9:
					CancelInputScheduleMode3();
					break;
					case 10:
					ResetPara();
					break;
					default:
						return;
				}
				break;
			case 0x0B:
				g_WelcomeStr[0]=byte3;
				g_WelcomeStr[1]=byte4;
				g_WelcomeStr[2]=byte5;
				g_WelcomeStr[3]=byte6;
				g_WelcomeStr[4]=byte7;
				break;
			case 0x0C:
				g_WelcomeStr[5]=byte3;
				g_WelcomeStr[6]=byte4;
				g_WelcomeStr[7]=byte5;
				g_WelcomeStr[8]=byte6;
				g_WelcomeStr[9]=byte7;
				break;
			case 0x0D:
				g_WelcomeStr[10]=byte3;
				g_WelcomeStr[11]=byte4;
				g_WelcomeStr[12]=byte5;
				g_WelcomeStr[13]=byte6;
				g_WelcomeStr[14]=byte7;
				break;
			case 0x0E:
				g_WelcomeStr[15]=byte3;
				g_WelcomeStr[16]=byte4;
				g_WelcomeStr[17]=byte5;
				g_WelcomeStr[18]=byte6;
				g_WelcomeStr[19]=byte7;
				g_WelcomeStr[20]=0;
				FlashWriteLogo();
				if(g_CurrentState==State_MainMenu_Menu)
				{
					clearLineFromAddr(0x80);
					chinesedisplay(0x80, g_WelcomeStr);
				}
				break;
			case 0x0F:
				g_MachineTypeXPStr[0]=byte3;
				g_MachineTypeXPStr[1]=byte4;
				g_MachineTypeXPStr[2]=byte5;
				g_MachineTypeXPStr[3]=byte6;
				g_MachineTypeXPStr[4]=byte7;
				break;
			case 0x10:
				g_MachineTypeXPStr[5]=byte3;
				g_MachineTypeXPStr[6]=byte4;
				g_MachineTypeXPStr[7]=byte5;
				g_MachineTypeXPStr[8]=byte6;
				g_MachineTypeXPStr[9]=byte7;
				break;
			case 0x11:
				g_MachineTypeXPStr[10]=byte3;
				g_MachineTypeXPStr[11]=byte4;
				g_MachineTypeXPStr[12]=byte5;
				g_MachineTypeXPStr[13]=byte6;
				g_MachineTypeXPStr[14]=byte7;
				break;
			case 0x12:
				g_MachineTypeXPStr[15]=byte3;
				g_MachineTypeXPStr[16]=byte4;
				g_MachineTypeXPStr[17]=byte5;
				g_MachineTypeXPStr[18]=byte6;
				g_MachineTypeXPStr[19]=byte7;
				g_MachineTypeXPStr[20]=0;
				FlashWriteType();
				break;
			case 0x13:
				g_TechTeleNumVal=byte3;
				Write_EEPROM_Byte(E2ROM_Tech_TelephoneNum_ADDR, g_TechTeleNumVal);
				g_TechSupportTel1[0]=byte4;
				g_TechSupportTel1[1]=byte5;
				g_TechSupportTel1[2]=byte6;
				g_TechSupportTel1[3]=byte7;
				break;
			case 0x14:
				g_TechSupportTel1[4]=byte3;
				g_TechSupportTel1[5]=byte4;
				g_TechSupportTel1[6]=byte5;
				g_TechSupportTel1[7]=byte6;
				g_TechSupportTel1[8]=byte7;
				break;
			case 0x15:
				g_TechSupportTel1[9]=byte3;
				g_TechSupportTel1[10]=byte4;
				g_TechSupportTel1[g_TechTeleNumVal]=0;
				m24xx_write(E2ROM_Tech_Telephone_ADDR,g_TechSupportTel1,g_TechTeleNumVal+2); //此处将最后一个0也存储
				break;
			case 0x16:
				g_TeleNumVal=byte3;
				Write_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, g_TeleNumVal);
				g_ContactTel2[0]=byte4;
				g_ContactTel2[1]=byte5;
				g_ContactTel2[2]=byte6;
				g_ContactTel2[3]=byte7;
				break;
			case 0x17:
				g_ContactTel2[4]=byte3;
				g_ContactTel2[5]=byte4;
				g_ContactTel2[6]=byte5;
				g_ContactTel2[7]=byte6;
				g_ContactTel2[8]=byte7;
				break;
			case 0x18:
				g_ContactTel2[9]=byte3;
				g_ContactTel2[10]=byte4;
				g_ContactTel2[g_TeleNumVal]=0;
				m24xx_write(E2ROM_Telephone_ADDR,g_ContactTel2,g_TeleNumVal+2); //此处将最后一个0也存储
				break;
			case 0x7F:
			{
				switch(byte3)
				{
					case 0x01:
					if(g_MutiWindow==SINGLEIMAGE)
						ReturnMsg_ChangeOne(4,1);
					else if(g_MutiWindow==PIPIMAGE)
						ReturnMsg_ChangeOne(4,2);
					else if(g_MutiWindow==PBPIMAGE)
						ReturnMsg_ChangeOne(4,3);
					break;
					case 0x02:
					case 0x03:
					{
						_InputSourceType input;
						if(byte3==2)
							input=g_MainInput;
						else
							input=g_SubInput;
						switch(input)
						{
							case INPUTSOURCE_CVBS:
								ReturnMsg_ChangeOne(4,1);
								break;
							case INPUTSOURCE_CVBS2:
								ReturnMsg_ChangeOne(4,2);
								break;
							case INPUTSOURCE_CVBS3:
								ReturnMsg_ChangeOne(4,3);
								break;
							case INPUTSOURCE_YPBPR:
								ReturnMsg_ChangeOne(4,4);
								break;
							case INPUTSOURCE_VGA:
								ReturnMsg_ChangeOne(4,5);
								break;
							case INPUTSOURCE_DVI:
								ReturnMsg_ChangeOne(4,6);
								break;
						//#if(ENABLE_HDMI)
							case INPUTSOURCE_HDMI:
								ReturnMsg_ChangeOne(4,7);
								break;
							case INPUTSOURCE_HDMI2:
								ReturnMsg_ChangeOne(4,8);
								break;
						//#endif
						
						#if(XP_TO_ID==XP_TO_FLI32626)
							case INPUTSOURCE_SDI:
								ReturnMsg_ChangeOne(4,9);
								break;	
						#endif
							case INPUTSOURCE_VGA2:
								ReturnMsg_ChangeOne(4,10);
								break;
							case INPUTSOURCE_DVI2:
								ReturnMsg_ChangeOne(4,INPUTSOURCE_DVI2);
								break;
						
							default:
								return;
						}
					}
					break;
					case 0x04:
					if(g_ActiveAudioWin==MAIN)
						ReturnMsg_ChangeOne(4,1);
					else if(g_ActiveAudioWin==SUB)
						ReturnMsg_ChangeOne(4,2);
					break;
					case 0x05:
					ReturnMsg_ChangeOne(4,g_VolumeValue);
					break;
					case 0x06:
					switch(g_OutputFormatIndex)
					{
						case INDEX640X480:
							ReturnMsg_ChangeOne(4,1);
							break;
						case INDEX800X600:
							ReturnMsg_ChangeOne(4,2);
							break;
						case INDEX1024X768:
							ReturnMsg_ChangeOne(4,3);
							break;
						case INDEX1280X1024:
							ReturnMsg_ChangeOne(4,4);
							break;
						case INDEX1366X768:
							ReturnMsg_ChangeOne(4,5);
							break;
						case INDEX1440X900:
							ReturnMsg_ChangeOne(4,6);
							break;
						case INDEX1600X1200:
							ReturnMsg_ChangeOne(4,7);
							break;
						case INDEX1680X1050:
							ReturnMsg_ChangeOne(4,8);
							break;
						case INDEX1920X1080:
							ReturnMsg_ChangeOne(4,9);
							break;
						case INDEX1600X900:
							ReturnMsg_ChangeOne(4,10);
							break;	
					}
					break;
					case 0x07:
					ReturnMsg_ReadWindowPara(g_ScreenHPos,g_ScreenVPos);
					break;
					case 0x08:
					ReturnMsg_ReadWindowPara(g_ScreenHSize,g_ScreenVSize);
					break;
					case 0x09:
					ReturnMsg_ReadWindowPara(g_MainScalerHPos,g_MainScalerVPos);
					break;
					case 0x0A:
					ReturnMsg_ReadWindowPara(g_MainScalerHSize,g_MainScalerVSize);
					break;
					case 0x0B:
					ReturnMsg_ReadWindowPara(g_SubScalerHPos,g_SubScalerVPos);
					break;
					case 0x0C:
					ReturnMsg_ReadWindowPara(g_SubScalerHSize,g_SubScalerVSize);
					break;
					case 0x0D:
					ReturnMsg_ReadWindowPara(g_MainZoomHPos,g_MainZoomVPos);
					break;
					case 0x0E:
					ReturnMsg_ReadWindowPara(g_MainZoomHSize,g_MainZoomVSize);
					break;
					case 0x0F:
					ReturnMsg_ReadWindowPara(g_SubZoomHPos,g_SubZoomVPos);
					break;
					case 0x10:
					ReturnMsg_ReadWindowPara(g_SubZoomHSize,g_SubZoomVSize);
					break;
					case 0x11:
					if(byte4==1)
						ReturnMsg_ChangeOne(5,g_Brightness_Value);
					else if(byte4==2)
						ReturnMsg_ChangeOne(5,g_Contrast_Value);
				#if(XP_TO_ID==XP_TO_FLI32626)
					else if(byte4==3)
						ReturnMsg_ChangeOne(5,g_Saturation_Value);
				#endif
					break;
					case 0x12:
						if(GetInputSrcFormat(MAIN,&g_InSrcHSize,&g_InSrcVSize))
							ReturnMsg_ReadWindowPara(g_InSrcHSize,g_InSrcVSize);
						break;
					case 0x13:
						if(GetInputSrcFormat(SUB,&g_InSrcHSize,&g_InSrcVSize))
							ReturnMsg_ReadWindowPara(g_InSrcHSize,g_InSrcVSize);
						break;						
					default:
						return;
				}
			}
			return;
			default:
				return;
		}
		if((RevPC_Data_Buf[2]&0x80)==0x80)//需要返回数据,执行完后返回原数据
		{
			if(g_ComTypeFlag==Communication_USART1)
				UART1_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
//			else if(g_ComTypeFlag==Communication_USART3)
//				UART3_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
		#ifdef TCP_IP
			else if(g_ComTypeFlag==Communication_Ethernet)
				Ethernet_SendBuf(RevPC_Data_Buf,MSG_LENGTH);
		#endif
		}
	}
//	if(device_ID==0||device_ID!=g_DeviceID)//将命令发送到下一台机器
//	{
//		UART3_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
//	}
}



void Find_CMD(U8 ucTmp)
{	
	int i,CheckSum=0;
	if(RevPC_Data_Idx ==0)
	{
		if(ucTmp==0xAA)//起始位为0xAA,放入接受数组
		{
			RevPC_Data_Buf[RevPC_Data_Idx++]=ucTmp;
		}			
	}
	else if(RevPC_Data_Idx ==9)
	{
		if(ucTmp==0x0D)
		{
			RevPC_Data_Buf[RevPC_Data_Idx]=ucTmp;
			
			for(i=1;i<8;i++)
			{
				CheckSum^=RevPC_Data_Buf[i];
			}
			if(RevPC_Data_Buf[8]==CheckSum)
				DealPC_Data();
		}
		RevPC_Data_Idx=0;
	}
	else
	{
		RevPC_Data_Buf[RevPC_Data_Idx++]=ucTmp;
	}
}

void DealPC_QueueMsg()
{
	U8 ucTmp=0;
	if(QueueOut(&g_strUartRxQueue,&ucTmp)!=QueueEmpty)
	{
		Find_CMD(ucTmp);
	}
}

void DealUSART3_QueueToEth()
{
	U8 ucTmp=0;
	
	if(QueueOut(&g_strUart3RxQueue,&ucTmp)!=QueueEmpty)
	{
		
		Find_CMD(ucTmp);
	}
}


