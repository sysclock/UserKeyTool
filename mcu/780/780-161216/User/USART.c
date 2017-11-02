/****************************************

	USART2:182-1(Front,Switch);
	USART3:182-2(Split);
	USART4:182-3(Split);
	USART5:Color LCD;




***************************************/
#include "header.h"
#include "stm32f10x.h" 	 
//#include "stm32_eval.h"
#include <stdio.h>
#include "Queue.h"

//extern U8 g_LoadModeFlag;
//U8 g_QueueInCount=0;

U8 g_RevUart2Data[1024]={0};
U8 g_Read_Command_Flag=0;
U16 g_RevUart2Idx=0;
U8 g_RevUart2Data_in[20]={0};
U8 g_Rev3531_message[20]={0};
U8 g_Rev3531_message_Idx=0;

U8 g_RevUart3Data[12]={0};
U8 g_RevUart3Idx=0;
U8 g_RevUart3Data_in[12]={0};

U8 g_RevUart1Data[20]={0};
U8 g_RevUart1Idx=0;
static U8 chMainSwitchIdx=0,chSubSwitchIdx=0,chSamePIPIdx=0;

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

#ifdef STM32F103RB

void UART3_SendByte(U8 buf)
{	
	USART3->DR = buf;				// send data
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}  ; 	// waiting for completion
}







void UART4_SendBuf(U8 *buf, U32 number)   
{
 	U8 *s = NULL;
	U32 n = number;
    	
	s = buf;
	while (n-- > 0)
	{
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion
		UART4->DR = *s++;				// send data
	}
}







void bit(u8 b)
{
	if(b)
		IO_UART_TX_H; 
	else
		IO_UART_TX_L;
}

u8 UART4_SendByte(U8 buf)
{	



	u8 i=9;
	bit(0);  
	while(i--)  //数据位
	{
		delay_us(100);
		bit(buf&0x01);   //低位在前
		buf = buf>>1;
	}
	bit(1);  //释放总线

	return 0;
}

u8 UART4_RxByte()
{
	u8 i=9;
	u8 Output=0;

	if(!IO_UART_RX)
	{
		while(i--)
		{
			delay_us(100); //100 96
			Output >>=1;

			if(IO_UART_RX)
				Output|=0x80;   
			//位间间隔104us
		} 
	}
	return Output;
}
#endif


void UART2_SendByte(U8 buf)
{	
	USART2->DR = buf;	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion
}


void UART2_SendBuf(U8 *buf, U32 number)   
{		
	U8 *s = NULL;
	U32 n = number;
	U8 noxBuf;
 	s = buf;   
	while (n-- > 0)
	{	
		noxBuf=*s;
		USART2->DR = noxBuf;
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion	
		s++;
	}

}

void UART3_SendBuf(U8 *buf, U32 number)   
{
 	U8 *s = NULL;
	U32 n = number;
    	
	s = buf;
	while (n-- > 0)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}  ;	// waiting for completion
		USART3->DR = *s++;				// send data
	}
}




/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/

void USART_Config(void)
{ 	
	USART_InitTypeDef USART_InitStructure;				  
	USART_InitStructure.USART_BaudRate = 115200;//9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
//	STM_EVAL_COMInit(COM1, &USART_InitStructure);                                 //启动串口

	USART_Init(USART1, &USART_InitStructure);                                         //初始化
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//接收中断允许
	USART_Cmd(USART1, ENABLE);  




	USART_InitStructure.USART_BaudRate = 115200;//9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	
	USART_Init(USART2, &USART_InitStructure); 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE); 	
}


void USART3_Config(void)
{ 
	USART_InitTypeDef USART_InitStructure;	
	
//***********************************************************************************

	USART_InitStructure.USART_BaudRate =115200;//9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;

//***********************************************************************************

	USART_Init(USART3, &USART_InitStructure); 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
 
	USART_Cmd(USART3, ENABLE);  //启动串口
  	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	
}


#ifdef STM32F103RB
void Exit_IOUartRx_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// Enable Button GPIO clock //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	// Configure Button pin as input floating //


	// Connect Button EXTI Line to Button GPIO Pin //
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	// Configure Button EXTI line //
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	// Enable and set Button EXTI Interrupt to the lowest priority //
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}
#endif
/*************************************************************************************************************/
/*************************************************************************************************************/
/*************************************************************************************************************/

void GPIO_Configuration(void)
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
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);



/***************************************************************************************/
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
/***************************************************************************************/



	//按键三根线GPIO初始化
	GPIO_InitStructure.GPIO_Pin = KB_SCK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KB_SCK_GPIO, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = KB_PLOAD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KB_PLOAD_GPIO, &GPIO_InitStructure);	
	// KEY IN  
	GPIO_InitStructure.GPIO_Pin = KB_SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KB_SDOUT_GPIO, &GPIO_InitStructure);  
	//按键LED 三根线GPIO 初始化
	GPIO_InitStructure.GPIO_Pin = LED_IN_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_IN_GPIO, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = LED_SRCK_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_SRCK_GPIO, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = LED_RCK_PIN|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_RCK_GPIO, &GPIO_InitStructure);
	//LCD 三根线GPIO 初始化
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
	//MUTE     GPIO 初始化
	GPIO_InitStructure.GPIO_Pin =MAIN_MUTE_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MAIN_MUTE_GPIO, &GPIO_InitStructure);
	//旋转编码器GPIO初始化
#ifdef EC11_BUTTON
	GPIO_InitStructure.GPIO_Pin = BMA_PIN|BMB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BMA_GPIO, &GPIO_InitStructure);   
#endif
	//声音切换GPIO初始化
	GPIO_InitStructure.GPIO_Pin = PC_AUD_SEL0_PIN|PC_AUD_SEL1_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PC_AUD_SEL0_GPIO, &GPIO_InitStructure);
	//PCB板LED闪烁灯GPIO初始化	
	GPIO_InitStructure.GPIO_Pin =LED_LAMP_GREEN_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_LAMP_GREEN_GPIO, &GPIO_InitStructure);
	//DVI选择GPIO初始化
#ifdef STM32F103RB
	GPIO_InitStructure.GPIO_Pin =PC_SEL_DVI_PIN;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PC_SEL_DVI_GPIO, &GPIO_InitStructure);
	PC_SEL_DVI_H;
#endif
	//HDMI选择GPIO初始化
	GPIO_InitStructure.GPIO_Pin =PC_SEL_HDMI_PIN;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PC_SEL_HDMI_GPIO, &GPIO_InitStructure);
/*	//VGA 切换GPIO初始化
	GPIO_InitStructure.GPIO_Pin =VGA_SEL_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VGA_SEL_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =VGA_SEL_H_PIN;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VGA_SEL_H_GPIO, &GPIO_InitStructure);
	*/
	/* Configure USART4 Tx (Pc.09) as alternate function push-pull */
  
	 //USART2           PA2 PA3            USART1     PA9 PA10 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_9;             //管脚9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //TX初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;            //管脚10     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //RX初始化
		
#ifdef STM32F103RB	
	//USART3             
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	  
	//USART4	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

/***************************************************************************************/
	//USART5	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态
/***************************************************************************************/

//	bit(1); 

	/* Configure USART4 Rx (Pc.8) as input floating */
/*	GPIO_InitStructure.GPIO_Pin = IO_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// GPIO_Mode_IPU;
	GPIO_Init(IO_UART_RX_GPIO, &GPIO_InitStructure);*/
	Exit_IOUartRx_Init() ;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////以下为处理上位机命令/////////////////////////////////////////////


static U8 RevPC_Data_Buf[MSG_LENGTH];
static U8 RevPC_Data_Idx=0;

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

	     
       #ifdef PC
		case Communication_USART1:
			UART1_SendBuf(RevData,MSG_LENGTH);
			break;
	   #endif


// 		case Communication_USART3:
// 			UART3_SendBuf(RevData,MSG_LENGTH);
// 			break;
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
	#ifdef PC
		case Communication_USART1:
			UART1_SendBuf(RevData,MSG_LENGTH);
			break;
	#endif
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
#ifdef TCP_IP
	else if(g_ComTypeFlag==Communication_Ethernet)
		SendMsg_ChangeOne(no,value,Communication_Ethernet);
#endif
}

void ReturnMsg_ReadWindowPara(U16 HValue,U16 VValue)
{
	if(g_ComTypeFlag==Communication_USART1)
		SendMsg_ReadWindowPara(HValue,VValue,Communication_USART1);
#ifdef TCP_IP
	else if(g_ComTypeFlag==Communication_Ethernet)
		SendMsg_ReadWindowPara(HValue,VValue,Communication_Ethernet);
#endif
}


//USART-1 App software
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

  #ifdef PC
	#ifdef EN_DEBUG_MSG
	printf("Receive Command is Byte4: %x \r\n",RevPC_Data_Buf[4]);
	#endif
	#endif
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
					case 0x03:
						g_DeviceID=byte4;
						Write_EEPROM_Byte(E2ROM_DeviceID_Addr, g_DeviceID);						
						SendMsg_ChangeOne(4,(g_DeviceID+1),Communication_USART3);  
						if((RevPC_Data_Buf[2]&0x80)==0x80)//需要返回数据,执行完后返回原数据
						{
						 #ifdef PC
							if(g_ComTypeFlag==Communication_USART1)
								UART1_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
					
						#endif
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
//						SetSingleImage();
						break;
					case 0x02:
//						if(g_MutiWindow!=PIPIMAGE)
//							SetPIPImage();
//						else
//							Deal_PIP_Pos(PIP_LT);							
						break;
					case 0x03:
//						if(g_MutiWindow!=PIPIMAGE)
//							SetPIPImage();
//						Deal_PIP_Pos(PIP_RT);
						break;
					case 0x04:
//						if(g_MutiWindow!=PIPIMAGE)
//							SetPIPImage();
//						Deal_PIP_Pos(PIP_MC);
						break;
					case 0x05:
//						if(g_MutiWindow!=PIPIMAGE)
//							SetPIPImage();
//						Deal_PIP_Pos(PIP_LB);
						break;
					case 0x06:	
//						if(g_MutiWindow!=PIPIMAGE)
//							SetPIPImage();
//						Deal_PIP_Pos(PIP_RB);
						break;
					case 0x07:
//						SetPBPImage();					
						break;
					case 0x08:
//						{
//							_InputSourceType inputSwap;
//							inputSwap=g_MainInput;
//							SetInputSource(g_SubInput,MAIN);
//							SetInputSource(inputSwap,SUB);
//						}
						break;
					case 0x09:
//						g_MainFreezeFlag=FREEZE;
//						SetFreezeImage(MAIN,FREEZE);
//						break;
//					case 0x0A:
//						g_MainFreezeFlag=UNFREEZE;
//						SetFreezeImage(MAIN,UNFREEZE);
//						break;
//					case 0x0B:
//						g_SubFreezeFlag=FREEZE;
//						SetFreezeImage(SUB,FREEZE);
//						break;
//					case 0x0C:
//						g_SubFreezeFlag=UNFREEZE;
//						SetFreezeImage(SUB,UNFREEZE);
//						break;
					case 0x0D:
//						g_CurrentState=State_Fade_Switch_Mode;
//						SetHVValueToSplitSoCwithSwitchState();
						break;
					case 0x0E:
//						g_CurrentState=State_Quick_Switch_Mode;
//						SetHVValueToSplitSoCwithSwitchState();
						break;
//					case 0x0F:
//						g_NextPointImage=SUB;
//						SetWhichImageOnTop(MAIN);
//						break;
//					case 0x10:
//						g_NextPointImage=MAIN;
//						SetWhichImageOnTop(SUB);
//						break;
//					case 0x11:
//						g_CurrentState=State_Fuse_Switch_Mode;
//						SetHVValueToSplitSoCwithSwitchState();
//						break;
//					case 0x12:
//						g_FadeTime=1;
//						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
//						break;
//					case 0x13:
//						g_FadeTime=2;
//						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
//						break;
//					case 0x14:
//						g_FadeTime=3;
//						Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
//						break;	
					default:
						return;
				}
				break;
			case 0x03:
//				if(byte3==1)
//					g_ActiveImageValue=MAIN;
//				else if(byte3==2)
//					g_ActiveImageValue=SUB;
				
				switch(byte4)
				{					
//					case 0x01:
//					SetInputSource(INPUTSOURCE_CVBS,g_ActiveImageValue);
//					delay(1);
//					chinesedisplay(0x81, "CVBS1");
//					break;
//					case 0x02:
//					SetInputSource(INPUTSOURCE_CVBS3,g_ActiveImageValue);
//					delay(1);
//					chinesedisplay(0x81, "CVBS2");
//					break;
//					case 0x03:
//					SetInputSource(INPUTSOURCE_CVBS2,g_ActiveImageValue);
//					delay(1);
//					chinesedisplay(0x81, "CVBS3");
//					break;
//				
//					case 0x05:
//					SetInputSource(INPUTSOURCE_VGA,g_ActiveImageValue);
//					delay(1);
//					chinesedisplay(0x81, "VGA  ");
//					#ifdef PC
//					#ifdef EN_DEBUG_MSG
//						printf("Swithc VGA !\r\n");
//					#endif
//					#endif
//					break;
					case 0x06:
//					SetInputSource(INPUTSOURCE_DVI,g_ActiveImageValue);
//					delay(1);
//					 #ifdef PC
//					#ifdef EN_DEBUG_MSG
//						printf("SWITCH DVI1!\r\n");
//					#endif
//					#endif
//					chinesedisplay(0x81, "DVI1 ");
					break;

					case 0x0b://yjl 150928
//					SetInputSource(INPUTSOURCE_DVI2,g_ActiveImageValue);
//					delay(1);
//					#ifdef PC
//					#ifdef EN_DEBUG_MSG
//						printf("SWITCH DVI2!\r\n");
//					#endif
//					#endif
//					chinesedisplay(0x81, "DVI2 ");								
					break;

//					case 0x0c://yjl 150928
//					//SetInputSource(INPUTSOURCE_USB,g_ActiveImageValue);
//					//printf(" USB1-START!\r\n");
////					SetInputSourceSwitch(INPUTSOURCE_USB);
//					delay(1);
//					#ifdef EN_DEBUG_MSG
//						printf("USB1!\r\n");
//					#endif
//					chinesedisplay(0x81, "USB1 ");	
//					SetKeypadLED(0x01<<USB_KEY);					
//					break;

					case 0x0d://yjl 150928
					//printf(" USB2-START!\r\n");
//					delay(1);
//					chinesedisplay(0x81, "USB2 ");	
//					#ifdef EN_DEBUG_MSG
//						printf("USB2!\r\n");
//					#endif
					//SetInputSource(INPUTSOURCE_USB2,g_ActiveImageValue);
//					SetInputSourceSwitch(INPUTSOURCE_USB2);	
//					SetKeypadLED(0x01<<USB2_KEY);						
					break;
					
					
					case 0x07:
//					SetInputSource(INPUTSOURCE_HDMI,g_ActiveImageValue);
//					delay(1);
//					chinesedisplay(0x81, "HDMI ");
					break;
					//case 8:	
					//SetInputSource(INPUTSOURCE_HDMI2,g_ActiveImageValue);
					//break;				
					
					default:
						return;
				}
				break;
				
			case 0x04:
				
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
					g_OutputFormatIndex=INDEX1920X1200;
					break;
					case 11:	
					g_OutputFormatIndex=INDEX2048x1152;
					break;
					case 12:	
					g_OutputFormatIndex=INDEX1280X720;
					break;
					case 13:	
//					g_OutputFormatIndex=0;
//					UserResolutionHSize=byte5<<8|byte4;
//					UserResolutionVSize=byte7<<8|byte6;
//					Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
					break;
				
					default:
						return;
				}
//				SetOutputResolution(g_OutputFormatIndex);
//				LoadOutputResolution();
//				PanelToScreenPara();
				break;
			case 0x07:
\
				break;
			case 0x08:
				break;
			case 0x09:
				switch(byte3)
				{
		
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
					Time_Adjust();
					break;
					case 2://不提示，直接覆盖
//					if(!Save(byte4))//否则不返回数据，说明该模式没有保存
						return;
					break;
					case 3:
//					if(IsHasSave(byte4))
//						Updata_Execute(EEPROMMemRegU8,EEPROMMemRegU16);
//					else  //否则不返回数据，说明该模式没有保存
						return;
					break;
					case 4:
//					InputEE[INPUT_HOUR]=byte4;
//					InputEE[INPUT_MIN]=byte5;
//					InputEE[INPUT_SRC]=byte6;
//					InputEE[INPUT_WEEK]=byte7;
//					SaveInputScheduleToEEPROM(MODE1);
//					FromEEInitInputSche(MODE1);  //将InputEE赋值给当前执行的各个模式
					break;
					case 5:
//					InputEE[INPUT_HOUR]=byte4;
//					InputEE[INPUT_MIN]=byte5;
//					InputEE[INPUT_SRC]=byte6;
//					InputEE[INPUT_WEEK]=byte7;
//					SaveInputScheduleToEEPROM(MODE2);
//					FromEEInitInputSche(MODE2);  //将InputEE赋值给当前执行的各个模式
					break;
					case 6:
//					InputEE[INPUT_HOUR]=byte4;
//					InputEE[INPUT_MIN]=byte5;
//					InputEE[INPUT_SRC]=byte6;
//					InputEE[INPUT_WEEK]=byte7;
//					SaveInputScheduleToEEPROM(MODE3);
//					FromEEInitInputSche(MODE3);  //将InputEE赋值给当前执行的各个模式
					break;
					case 7:
//					CancelInputScheduleMode1();
					break;
					case 8:
//					CancelInputScheduleMode2();
					break;
					case 9:
//					CancelInputScheduleMode3();
					break;
					case 10:
					ResetPara_Total();
					break;
					default:
						return;
				}
				break;
			case 0x7F:
			{
				switch(byte3)
				{
					case 0x01:
//					if(g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
//						ReturnMsg_ChangeOne(4,1);
//					else if(g_MutiWindow==PIPIMAGE)
//						ReturnMsg_ChangeOne(4,2);
//					else if(g_MutiWindow==PBPIMAGE)
//						ReturnMsg_ChangeOne(4,3);
					break;
					case 0x02:
					case 0x03:
//					{
//						_InputSourceType input;
//						if(byte3==2)
//							input=g_MainInput;
//						else
//							input=g_SubInput;
//						switch(input)
//						{
//							case INPUTSOURCE_CVBS:
//								ReturnMsg_ChangeOne(4,1);
//								break;
//							case INPUTSOURCE_CVBS2:
//								ReturnMsg_ChangeOne(4,2);
//								break;
//							case INPUTSOURCE_CVBS3:
//								ReturnMsg_ChangeOne(4,3);
//								break;
//							case INPUTSOURCE_YPBPR:
//								ReturnMsg_ChangeOne(4,4);
//								break;
//							case INPUTSOURCE_VGA:
//								ReturnMsg_ChangeOne(4,5);
//								break;
//							case INPUTSOURCE_DVI:
//								ReturnMsg_ChangeOne(4,6);
//								break;
//							case INPUTSOURCE_HDMI:
//								ReturnMsg_ChangeOne(4,7);
//								break;
//							//case INPUTSOURCE_HDMI2:
//							//	ReturnMsg_ChangeOne(4,8);
//							//	break;
//						
//							case INPUTSOURCE_SDI:
//								ReturnMsg_ChangeOne(4,9);
//								break;	
//							default:
//								return;
//						}
//					}
					break;
					case 0x04:
//					if(g_ActiveAudioWin==MAIN)
//						ReturnMsg_ChangeOne(4,1);
//					else if(g_ActiveAudioWin==SUB)
//						ReturnMsg_ChangeOne(4,2);
					break;
					case 0x05:
//					ReturnMsg_ChangeOne(4,g_VolumeValue);
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
						case INDEX1920X1200:
							ReturnMsg_ChangeOne(4,10);
							break;
						case INDEX2048x1152:
							ReturnMsg_ChangeOne(4,11);
							break;
						case INDEX1280X720:
							ReturnMsg_ChangeOne(4,12);
							break;	
						case 0:
//							RevPC_Data_Buf[3]|=0x80;
//							ReturnMsg_ReadWindowPara(UserResolutionHSize, UserResolutionVSize);
							break;	
					}
					break;
					case 0x07:
//					ReturnMsg_ReadWindowPara(g_ScreenHPos,g_ScreenVPos);
					break;
					case 0x08:
//					ReturnMsg_ReadWindowPara(g_ScreenHSize,g_ScreenVSize);
					break;
//					case 0x09:
//					ReturnMsg_ReadWindowPara(g_MainScalerHPos,g_MainScalerVPos);
//					break;
//					case 0x0A:
//					ReturnMsg_ReadWindowPara(g_MainScalerHSize,g_MainScalerVSize);
//					break;
//					case 0x0B:
//					ReturnMsg_ReadWindowPara(g_SubScalerHPos,g_SubScalerVPos);
//					break;
//					case 0x0C:
//					ReturnMsg_ReadWindowPara(g_SubScalerHSize,g_SubScalerVSize);
//					break;
//					case 0x0D:
//					ReturnMsg_ReadWindowPara(g_MainZoomHPos,g_MainZoomVPos);
//					break;
//					case 0x0E:
//					ReturnMsg_ReadWindowPara(g_MainZoomHSize,g_MainZoomVSize);
//					break;
//					case 0x0F:
//					ReturnMsg_ReadWindowPara(g_SubZoomHPos,g_SubZoomVPos);
//					break;
//					case 0x10:
//					ReturnMsg_ReadWindowPara(g_SubZoomHSize,g_SubZoomVSize);
//					break;
					case 0x11:
//					if(byte4==1)
//						ReturnMsg_ChangeOne(5,g_BrightnessValue);
//					else if(byte4==2)
//						ReturnMsg_ChangeOne(5,g_ContrastValue);
//					else if(byte4==3)
//						ReturnMsg_ChangeOne(5,g_SaturationValue);
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
		#ifdef TCP_IP
			else if(g_ComTypeFlag==Communication_Ethernet)
				Ethernet_SendBuf(RevPC_Data_Buf,MSG_LENGTH);
		#endif
		}
	}
// 	if(device_ID==0||device_ID!=g_DeviceID)//将命令发送到下一台机器
// 	{
// 		UART3_SendBuf(RevPC_Data_Buf, MSG_LENGTH);
// 	}
}


//Usart1
void DealPC_QueueMsg()
{
	int i,CheckSum=0;
	U8 ucTmp=0;
	if(QueueOut(&g_strUartRxQueue,&ucTmp)!=QueueEmpty)
	{
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
}


void GpuSend(char *buf1)
{  u8 i=0;
   while (1)
   {  if (buf1[i]!='\0')
     { 
				USART_SendData(USART3, buf1[i]);  //发送一个byte到串口
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}; //等待发送结束

        i++;
     }
     else return;
   }
}

