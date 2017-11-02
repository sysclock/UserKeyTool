






#include "stm32f10x.h"
#include <stdio.h>
#include "remote.h"
#include "header.h"


void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	// TIM3 clock enable //	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


	// ---------------------------------------------------------------
	//TIM3CLK 即PCLK1=36MHz
	//TIM3CLK = 36 MHz, Prescaler = 7200, TIM3 counter clock = 5K,即改变一次为5K,周期就为10K
	//--------------------------------------------------------------- //
	// Time base configuration //
	TIM_TimeBaseStructure.TIM_Period = 10; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到10为100us
	TIM_TimeBaseStructure.TIM_Prescaler =(720-1); //设置用来作为TIMx时钟频率除数的预分频值  100Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//清除中断，
	
	// Enables the Update event for TIM3 //
	//TIM_UpdateDisableConfig(TIM3,ENABLE); 	//使能 TIM3 更新事件 

	// TIM IT enable //
	TIM_ITConfig(  //使能或者失能指定的TIM中断
	TIM3, 
	TIM_IT_Update   //TIM 中断源
	// |TIM_IT_Trigger   //TIM 触发中断源 
	,ENABLE  //使能
	);

	
	//TIM3 enable counter //
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}



// 4个bits用于分配pre-emption优先级和sub优先级。
// 前者为抢占优先级 后者为响应优先级，也称为副优先级
// 优先级数字越小代表的优先级越高
void Remote_Init(void)    			  
{
	GPIO_InitTypeDef GPIO_InitStructure;
    	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
		
	// Enable and set Button EXTI Interrupt to the lowest priority //
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//0 先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   


//01.EXTI_InitTypeDef EXTI_InitStructure;  
//03.// Configure EXTI Line3 to generate an interrupt on falling edge  
//04.EXTI_InitStructure.EXTI_Line = EXTI_Line3;  
//05.EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
//06.EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
//07.EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
//08.EXTI_Init(&EXTI_InitStructure);  
//09.GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4); 

	
	// Connect Button EXTI Line to Button GPIO Pin //
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	// Configure Button EXTI line //
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	//外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		
	//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		
	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
}

unsigned short  irtime=1;//红外用全局变量

unsigned char   irok=0;
unsigned char   IRcord[4];  //处理后的红外码，分别是 客户码，客户码，数据码，数据码反码
unsigned short   irdata[33]; //33个高低电平的时间数据
unsigned char  RmtCnt=0;	//按键按下的次数

extern U8 g_RorL;
extern U8 g_numvalue;
static u16  EC11Cnt=0;
static U8 g_preEC11Value=0;


//   100 us 定时器中断
void TIM3_IRQHandler(void)   //TIM3中断100us中断一次
{
	U8 cha=0;
	
	TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
	irtime++;	

	//#ifdef DEBUG		
	//		printf(" TIM3_IRQHandler irtime=:%x \r\n",irtime);
	//#endif	
	
#ifdef EC11_BUTTON
	EC11Cnt++;
	if(EC11Cnt==2000)//在固定时间变化的数
	{
		EC11Cnt=0;
		if(g_preEC11Value!=g_numvalue)
		{
			if(g_RorL==0)
			{
				cha=(g_preEC11Value>g_numvalue)?(g_preEC11Value-g_numvalue):(g_preEC11Value+255-g_numvalue);
				g_preEC11Value=g_numvalue;
				if(cha<3)
					g_StepValue=1;
				else if(cha<6)
					g_StepValue=10;
				else
					g_StepValue=100;
				if(g_LockKeyFlag==UNLOCKKEYPAD)
				{
					if( IsSetParaValueState())
                    	DealDownKey();
					else
						DealUpKey();
				}
				else if(g_LockKeyFlag==LOCKKEYPAD)
				{
					KeypadLockedPrompt();
				}
				
			}
			else
			{
				cha=(g_numvalue>g_preEC11Value)?(g_numvalue-g_preEC11Value):(g_numvalue+255-g_preEC11Value);
				g_preEC11Value=g_numvalue;
				if(cha<3)
					g_StepValue=1;
				else if(cha<6)
					g_StepValue=10;
				else
					g_StepValue=100;
				if(g_LockKeyFlag==UNLOCKKEYPAD)
				{
					if( IsSetParaValueState())
                    	DealUpKey();
					else
					   	DealDownKey();
				}
				else if(g_LockKeyFlag==LOCKKEYPAD)
				{
				KeypadLockedPrompt();
				}				
			}
		}
	}
#endif
	
}
//外部中断1服务程序
/*
void EXTI4_IRQHandler(void)
{	
	           //接收红外信号处理
	static unsigned char   startflag=1;//0;                //是否开始处理标志位
	static unsigned char  i=0; 
	
	
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0x00)		//按键按下:低电平有效
	{
		
		if(startflag)                         
		{

			if(irtime>100&&irtime<150)//引导码 TC9012的头码，9ms+4.5ms
			{
				i=0;
			}
			if(irtime<800)
			{
	//存储每个电平的持续时间，用于以后判断是0还是1
				irdata[i]=irtime;
			}
			else
			{
//连发码(由 9ms 低电平+2.5m 高电平+0.56ms 低电平+97.94ms 高电平组成)
				RmtCnt++;				
			}			
			
			irtime=0;
			i++;
			if(i==33)
			{
				irok=1;
				i=0;
				RmtCnt=0;				
			}			
			startflag=0;
		}
		else
		{
			irtime=0;
			startflag=1;
		}
	} 
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除EXTI1线路挂起位  

}
*/	


//外部中断1服务程序
//红外的波形触发32次，i才能达到33；然后把数据移入ircord ?//===2017,Jun 01 ; 15:49:57  ;BY YANG@BYCOX.COM===
//波形的全周期是108ms
//	NEC格式的特征：
//	1：使用38 kHz载波频率；0.026ms ;21x0.0263=0.56ms
//	2：引导码间隔是9 ms + 4.5 ms
//	3：使用16位客户代码
//	4：使用8位数据代码和8位取反的数据代码
//	NEC 协议通过脉冲串之间的时间间隔来实现信号的调制（英文简写PPM）。
//	逻辑"0"是由0.56ms的38KHZ载波和0.560ms的无 载波间隔组成；
//	逻辑"1"是由0.56ms的38KHZ载波和1.68ms的无载波间隔组成；结束位是0.56ms的38K载波。
void EXTI15_10_IRQHandler(void)
{	
	 //接收红外信号处理
	static unsigned char   startflag=1;//0;                //是否开始处理标志位
	static unsigned char  i=0; 

		
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0x00)		//按键按下:低电平有效
	{	
		
		if(startflag)                         
		{

			if(irtime>100&&irtime<150)//引导码 TC9012的头码，9ms Low + 4.5ms High
			{
				i=0;
			}

			//存储每个电平的持续时间，用于以后判断是0还是1
			if(irtime<800)//800)	//80ms
			{
				irdata[i]=irtime;
			}
			else
			{
			//连发码(由 9ms 低电平+2.5m 高电平+0.56ms 低电平+97.94ms 高电平组成)
				RmtCnt++;				
			}	
			
			//#ifdef DEBUG		
			//	printf("EXTI15_10_IRQHandler  i=%d;  irtime=%d ,RmtCnt= %d \r\n",i,irtime,RmtCnt); 
			//#endif 晶日盛

			
			irtime=0;
			i++;
			
			
			if(i==33)
			{
				irok=1;		//IROK 唯一置高的地方
				i=0;
				RmtCnt=0;		
			
			}
			
			startflag=0;
		}
		else  //(startflag==0)       
		{
			irtime=0;
			startflag=1;
		}

	} 
	EXTI_ClearITPendingBit(EXTI_Line11);  //清除EXTI1线路挂起位  

}




/******************************************************************/
/*                    红外解码函数处理                            		   */
/******************************************************************/
void Ircordpro(void)//红外码值处理函数
{ 
	unsigned char i, j, k;
	unsigned char value; 
	unsigned short cord;

	k=1;
	for(i=0;i<4;i++)      //处理4个字节
	{
		for(j=1;j<=8;j++) //处理1个字节8位
		{
			cord=irdata[k];
			
			//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差	  
			if(cord>18)
			{
				//逻辑 1 应该是 560us 低+1680us 高，
				value=value|0x01;//0x80;
			}
			else 
			{
				//逻辑 0 应该是 560us 低+560us 高
				value=value&0xFE;
			}
			
			if(j<8)
			{
				value=value<<1;//>>1;
			}
			
			k++;
		}
		
		IRcord[i]=value;	

		#ifdef DEBUG_IR		
			printf("IRcord[%d]=:0x%x \r\n",i,IRcord[i]);	
		#endif		

		value=0;     
	} 	 
	
    
}




//这个函数有时候进不来，导致遥控器失效
//IR ok目前出了问题;IR ok在中断函数EXTI15_10_IRQHandler中赋值
//操作遥控器的时候确实进入了中断函数，但是其中的变量i达不到33，导致irok无法置高
//先等待中断irok,然后处理串行数据Ircordpro()
unsigned char Remote_Scan(void)
{
	//printf("enter Remote_Scan func,irok=%d \r\n",irok);
	if(irok)                        //如果接收好了进行红外处理;给遥控器指令赋值
	{   
		
		irok=0;
		delay_ms(500);

		//遥控器窜行数据处理
		Ircordpro();
		
		if((IRcord[0]==(u8)~IRcord[1])&&IRcord[0]==REMOTE_ID)
		{
			if(IRcord[2]==(u8)~IRcord[3])
			{				
				return 1;
			}
		}
	}


	
	return 0;
}

































