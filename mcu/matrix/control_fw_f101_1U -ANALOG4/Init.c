/********************************************
【文 件 名】Init.c
【功能说明】实现STM32的所需功能的初始化
********************************************/
#include "Init.h"
#include "Lcd.h"
#include "AT24XX.h"
#include "Switch.h"
#include "Key.h"
#include "MyUart.h"
#include "stm32f10x_crc.h"
#include "stdlib.h"
#include "stm32f10x_wwdg.h"
#include "stm32f10x_flash.h"
//私有变量----------------------------------
Uart_T Uart;
vu8 TickFlag;
uc32 BaudArray[4]={4800,9600,38400,115200};
const char* pLogo[4]={" ","V","VGA","RGB"};
Sys_T SysInfo;
IR_T IRInfo;
/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】系统外设时钟使能
*******************************************************************************/
void PeriphClkInit()
{
  //RCC时钟使能
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
  //GPIOA时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  //GPIOB时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  //GPIOC时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
  //UART1时钟使能
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
  //UART2时钟使能
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);	
  //定时器2时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);   //uart
  //定时器3时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	//beep
  //定时器4时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);   //		
  //AFIO时钟使能
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);
}

/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】串口配置
*******************************************************************************/
void Com_Config(u32 Baud)
{
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = Baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_Init(USART2, &USART_InitStructure);
    
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);    //接收非空中断
  USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);    //接收非空中断
  
  // Enable USART1
  USART_Cmd(USART1, ENABLE);  
  USART_Cmd(USART2, ENABLE);   
}
/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】GPIO配置
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  AFIO->MAPR = 0x02000000;
  
  //GPIOA output
  GPIO_InitStructure.GPIO_Pin = UPDATE_PIN | DATE_PIN | KEY_EN_PIN | BEEP_PIN | LCD_CS_PIN | LCD_RST_PIN | UART_LED_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  //GPIOA input(RX0,RX1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3 | IR_INT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,IR_INT_SOURCE);
  
  //GPIOA AFF(TX0,TX1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  //GPIOB output
  GPIO_InitStructure.GPIO_Pin = LCD_RS_PIN | LCD_WR_PIN | AT_SDA_PIN | AT_SCK_PIN | 0xff00;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
				  
  //GPIOC output
  GPIO_InitStructure.GPIO_Pin = 0XFFFF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  //IR as extern interrupt
  EXTI_InitTypeDef Exit_InitStructure;
  Exit_InitStructure.EXTI_Line = IR_INT_PIN;
  Exit_InitStructure.EXTI_LineCmd = ENABLE;
  Exit_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  Exit_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&Exit_InitStructure);
}

void RCC_Configuration()
{
	ErrorStatus HSEStartUpStatus;
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);
		/* HCLK = SYSCLK */	//29.4912
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		/* PCLK2 = HCLK */	//29.4912
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		/* PCLK1 = HCLK/2 */ //14.7456
		RCC_PCLK1Config(RCC_HCLK_Div2);
		/* ADCCLK = PCLK2/4 */ //7.3728
		RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
		/* PLLCLK = 7.3728MHz * 4 = 29.4912 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
	
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
}
void IWDGInit(void)
{
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz  */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  /* Set counter reload value to 1250 */
  IWDG_SetReload(1250);
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void TimeInit()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;		
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
	TIM_TimeBaseStructure.TIM_Period = 2301;				//20ms中断
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                 //Uart
	
	TIM_TimeBaseStructure.TIM_Period = 25;//58;			    //0.5ms中断
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);			//BEEP
	
	TIM_TimeBaseStructure.TIM_Period = 2300;				//20ms超时		
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);			//IR
    
	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, 255, TIM_PSCReloadMode_Immediate); //256分频，等于115200
	TIM_PrescalerConfig(TIM3, 255, TIM_PSCReloadMode_Immediate); //256分频，等于115200
	TIM_PrescalerConfig(TIM4, 255, TIM_PSCReloadMode_Immediate); //256分频，等于115200	
	
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  // Set the Vector Table base location at 0x20000000
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH  
  // Set the Vector Table base location at 0x08000000 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //Enable the ExtI1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //Enable the ExtI2 Interrupt
  /*
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  */
  //Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //Enable the USART2 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  //Enable the Timx Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void SysTick_Configuration(void)
{
	/* Select HCLK/8 as SysTick clock source */ //29.4912M
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	/* SysTick interrupt each 100ms */
	if (SysTick_Config(2949120))
	{ 
		/* Capture error */ 
		while (1);
	}
	/* Set SysTick interrupt vector Preemption Priority to 1 */
	NVIC_SetPriority(SysTick_IRQn, 0x08);
}

static u8 CheckConfig()
{
	if((SysInfo.Addr == 0xff)||(SysInfo.Addr == 0))
		return FALSE;
	if(SysInfo.Baud > 3)
		return FALSE;
	return TRUE;
}

void SetDefSysInfo(u8 Type,u8 InputNum,u8 OutputNum,u8* pLogo,u8* pInfo)
{
  u8 i;
  unsigned short tmp[16];
	SysInfo.Addr = 1;
	AT24xxWrite(FIX_ADD,&SysInfo.Addr,1);	
    DelayMs(10);
	IWDG_ReloadCounter();
	SysInfo.Baud = 1;
	AT24xxWrite(FIX_BAUD,&SysInfo.Baud,1);		
    DelayMs(10);    
	IWDG_ReloadCounter();
    SysInfo.Type = Type;
	AT24xxWrite(FIX_TYPE,&SysInfo.Type,1);
    DelayMs(10);    
	IWDG_ReloadCounter();
	SysInfo.InputNum = InputNum;
	AT24xxWrite(FIX_INPUT,&SysInfo.InputNum,1);		
    DelayMs(10);    
	IWDG_ReloadCounter();
	SysInfo.OutputNum = OutputNum;
	AT24xxWrite(FIX_OUTPUT,&SysInfo.OutputNum,1);		
    DelayMs(10);    
	IWDG_ReloadCounter();
	memcpy(SysInfo.Logo,pLogo,strlen((char const*)pLogo)+1);
	AT24xxWrite(FIX_LOGO,SysInfo.Logo,16);		
    DelayMs(10);    
	IWDG_ReloadCounter();
	memcpy(SysInfo.Info,pInfo,strlen((char const*)pInfo)+1);
	AT24xxWrite(FIX_INFO,SysInfo.Info,16);			
    DelayMs(10);  
	IWDG_ReloadCounter();
	AT24xxWrite(FIX_BEEP,&SysInfo.BeepEn,1);
    DelayMs(10);      
    //初始化EEPROM---END
    //-----write config to flash---Begin
    FLASH_Unlock();
    FLASH_ErasePage(SYS_INFO_BAK_ADDR);
	//sprintf( cp, "len:%d", len);
	//USART1_Puts(cp);
    tmp[0] = SysInfo.Baud; 
    tmp[0] <<=8;
    tmp[0] |= SysInfo.Addr;
    FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR,tmp[0]);
    tmp[0] = SysInfo.BeepEn; 
    tmp[0] <<=8;
    tmp[0] |= SysInfo.Type;
    FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR+2,tmp[0]);
    tmp[0] = SysInfo.OutputNum; 
    tmp[0] <<=8;
    tmp[0] |= SysInfo.InputNum;
    FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR+4,tmp[0]);
    
    memset(tmp,0,32);
    memcpy(tmp,pLogo,strlen((char const*)pLogo)+1);
    memcpy(tmp+8,pInfo,strlen((char const*)pInfo)+1);
    for(i =0;i<16;i++)
    {
      FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR+6+i*2,tmp[i]);
    }
    FLASH_Lock();
        
    //-----write config to flash---END    
    DataInit();
    
    
    for(i=0;i<SysInfo.OutputNum;i++)
      InOut_V[i] = 0x80|i;
    
    if(SysInfo.Type & TypeAudio)
    {
      for(i=0;i<SysInfo.OutputNum;i++)
        InOut_A[i] = 0x80|i;
    }
    
    for(i=0;i<=SysInfo.PresetCount;i++)
	{
      SaveEnv(i);
	  IWDG_ReloadCounter();
	}
}
void ResetSysInfo(void)
{
    //u8 i;
    unsigned char *addr;
    
    addr = (unsigned char *)SYS_INFO_BAK_ADDR;
    SysInfo.Addr = *addr;
    
    SysInfo.TypeModifyEn = TRUE;//使能修改信息标志
    AT24xxWrite(FIX_ADD,&SysInfo.Addr,1);	
    DelayMs(10);
    IWDG_ReloadCounter();
    SysInfo.Baud = *(addr+1);
    AT24xxWrite(FIX_BAUD,&SysInfo.Baud,1);		
    DelayMs(10);    
    IWDG_ReloadCounter();
    SysInfo.Type = *(addr+2);
    AT24xxWrite(FIX_TYPE,&SysInfo.Type,1);
    DelayMs(10);    
    IWDG_ReloadCounter();
    SysInfo.InputNum = *(addr+4);
    AT24xxWrite(FIX_INPUT,&SysInfo.InputNum,1);		
    DelayMs(10);    
    IWDG_ReloadCounter();
    SysInfo.OutputNum = *(addr+5);
    AT24xxWrite(FIX_OUTPUT,&SysInfo.OutputNum,1);		
    DelayMs(10);    
    IWDG_ReloadCounter();
    memcpy(SysInfo.Logo,addr+6,16);
    AT24xxWrite(FIX_LOGO,SysInfo.Logo,16);		
    DelayMs(10);    
    IWDG_ReloadCounter();
    memcpy(SysInfo.Info,addr+22,16);
    AT24xxWrite(FIX_INFO,SysInfo.Info,16);			
    DelayMs(10);  
    IWDG_ReloadCounter();
    SysInfo.BeepEn = *(addr+3);
    AT24xxWrite(FIX_BEEP,&SysInfo.BeepEn,1);
    DelayMs(10);      
    //初始化EEPROM---END    
    //-----write config to flash---END    
    /*DataInit();
    
    
    for(i=0;i<SysInfo.OutputNum;i++)
      InOut_V[i] = 0x80|i;
    
    if(SysInfo.Type & TypeAudio)
    {
      for(i=0;i<SysInfo.OutputNum;i++)
        InOut_A[i] = 0x80|i;
    }
    
    for(i=0;i<=SysInfo.PresetCount;i++)
	{
      SaveEnv(i);
	  IWDG_ReloadCounter();
  }*/
  SysInfo.TypeModifyEn = FALSE;
  //SysInfo.InitOK = TRUE;
}
void ReadSysInfo(void)
{
	AT24xxRead(FIX_ADD,&SysInfo.Addr,1);		
	AT24xxRead(FIX_BAUD,&SysInfo.Baud,1);		
	AT24xxRead(FIX_TYPE,&SysInfo.Type,1);		
	AT24xxRead(FIX_INPUT,&SysInfo.InputNum,1);		
	AT24xxRead(FIX_OUTPUT,&SysInfo.OutputNum,1);	
       	
	AT24xxRead(FIX_LOGO,SysInfo.Logo,16);	
	AT24xxRead(FIX_INFO,SysInfo.Info,16);		
	AT24xxRead(FIX_BEEP,&SysInfo.BeepEn,1);	
        
       // USART1->DR = SysInfo.Addr;	  DelayMs(50);
        //USART1->DR = SysInfo.Baud;	  DelayMs(50);
       // USART1->DR = SysInfo.Type;	  DelayMs(50);
       // USART1->DR = SysInfo.InputNum;	  DelayMs(50);
       // USART1->DR = SysInfo.OutputNum;	  DelayMs(50);
        
	//myprintf("A:%x,B:%x,T:%x,I:%x,O:%x,B:%x",SysInfo.Addr,SysInfo.Baud,SysInfo.Type,SysInfo.InputNum,\
        //  SysInfo.OutputNum,SysInfo.BeepEn);
        //myprintf("%x",SysInfo.Addr);
        //  DelayMs(1000);  
        /*  FlashWriteStr();
            int *addr;
        addr = (int *)SYS_INFO_BAK_ADDR;
        
        myprintf("%x %x",*addr,*(addr+2));
        DelayMs(10);  
          while(1);*/
	SysInfo.InitOK = CheckConfig();
}
/*****************************
遥控器编码芯片型号为：HT6222
*****************************/
void IR_ISR(void)
{
  u16 Count;
  
  IR_STOP;
  Count = TIM_GetCounter(TIM4);
  if(Count > 460)     
  {
    IRInfo.HeadFlag = TRUE;
    IRInfo.Index = 0;
    IRInfo.Data = 0;
  }
  else if(Count > 230)  
  {
    IRInfo.Data |= (1<<IRInfo.Index);
    IRInfo.Index++;
  }
  else if(Count > 115)
  {
    IRInfo.Index++;
  }
  else
    IRInfo.Index = 0;
  IR_START;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

void DelayMs(int ms)
{
  int i,j;
  for (i=0; i<ms; i++)
    for(j=0;j<6000;j++);   //1ms
} 

void DelayUs(int us)
{
  int i,j;
  for (i=0; i<us; i++)
    for(j=0;j<6;j++);   //1us
}

bool DataInit()
{
  u16 Count = 0x7C0;
  
  if(SysInfo.Type&0x0F)
  {
    if(InOut_V)
      free(InOut_V);
    InOut_V = malloc(SysInfo.OutputNum);
    if(!InOut_V)
      return FALSE;
  }
  if(SysInfo.Type&0xF0)
  {
    if(InOut_A)
      free(InOut_A);
    InOut_A = malloc(SysInfo.OutputNum);
    if(!InOut_A)
      return FALSE;
  }
  if((SysInfo.Type&0xF0)&&(SysInfo.Type&0x0F))      //音视频都有
    Count >>= 1;
  
  Count/=SysInfo.OutputNum;
  if(Count > 30)
    SysInfo.PresetCount = 30;
  else if(Count > 20)
    SysInfo.PresetCount = 20;
  else if(Count > 10)
    SysInfo.PresetCount = 10;
  else
    SysInfo.PresetCount = Count-1;
  
  SysInfo.AStartAddr = SysInfo.OutputNum*(SysInfo.PresetCount+1);

  return TRUE;
}


void FlashWriteStr(void)
{
	//char cp[12];
	//u8 s = 0;
	FLASH_Unlock();
	FLASH_ErasePage(SYS_INFO_BAK_ADDR);
	//sprintf( cp, "len:%d", len);
	//USART1_Puts(cp);
        FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR,0x2211);
        FLASH_ProgramHalfWord(SYS_INFO_BAK_ADDR+2,0x4433);
        FLASH_Lock();
}
