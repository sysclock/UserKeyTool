/**
  ******************************************************************************
  * 文件名称 ：USART.c 
  * 功    能 ：初始化USART1
  * 版 	  本 ：V0.0
  * 库 版 本 ：V3.5.0
  *	修改记录 ：		   	

  *******************************************************************************/ 

/*包涵的头文件-----------------------------------------------*/ 
#include "USART.h"
/*变量名称---------------------------------------------------*/
u8 U2data;

u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_RX_COUNT=0;       //接收状态标记

u8 RS485_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 RS485_RX_COUNT=0;       //接收状态标记

u8 RS485_RX_FLAG=0;

/**************************************************************
 ** 函数名称 :USART1_Config
 ** 功能说明 :USART1初始化（GPIO、NVIC、USART1）
 ** 输入参数 :无
 ** 输出参数 :无
 ** 返回参数 :无
 ** 注    意 :USART1时钟--APB2
***************************************************************/
void USART1_Config(u8 baudrate_num)
{	
	/*1、声明结构体：GPIO、NVIC、USART*/
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
	/*2、开时钟：使能GPIO时钟(GPIOA)，使能USART时钟(USART2)*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_DeInit(USART1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*3、配置TX：PA2-TX复用推挽输出*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*4、配置RX：A3-RX浮空输入*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	         //组1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 //USART1全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 //使能USART2通道
	NVIC_Init(&NVIC_InitStructure);	

	USART_InitStructure.USART_BaudRate =(1<<baudrate_num)*2400;                     //波特率115200         
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    //字长9bit:带奇偶校验时字长为9bit,无奇偶校验时字长为8bit										
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	       //停止位2位									
	USART_InitStructure.USART_Parity = USART_Parity_No;		       //偶校验							
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发使能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制							
  USART_Init(USART1, &USART_InitStructure);   //初始化外设 USART2   
  USART_Cmd(USART1, ENABLE);					//使能USART外设
	
	/*7、使能USART2接收中断*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//使能接收中断
}



/**************************************************************
 ** 函数名称 :USART2_Config
 ** 功能说明 :USART2初始化（GPIO、NVIC、USART2）
 ** 输入参数 :无
 ** 输出参数 :无
 ** 返回参数 :无
 ** 注    意 :USART2时钟--APB1
***************************************************************/
void USART2_Config(void)
{	
	/*1、声明结构体：GPIO、NVIC、USART*/
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
		
	/*2、开时钟：使能GPIO时钟(GPIOA)，使能USART时钟(USART2)*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);

	/*3、配置TX：PA2-TX复用推挽输出*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*4、配置RX：A3-RX浮空输入*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*6、配置NVIC参数：
	@	组1
	@ 	USART2全局中断
	@	抢占优先级0
	@ 	响应优先级0
	@	使能USART2通道
	@	初始化NVIC
 	*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	         //组1
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		 //USART2全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 //使能USART2通道
	NVIC_Init(&NVIC_InitStructure);	


	/*6、配置USART参数：
	@	波特率-----115200
	@	数据字长---9
	@	停止位-----2
	@	校验-------偶校验
	@	模式-------收发
	@	硬件流控制-无
	@	初始化USART2
	@	使能USART2
	*/
	USART_InitStructure.USART_BaudRate =9600;                     //波特率115200         
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    //字长9bit:带奇偶校验时字长为9bit,无奇偶校验时字长为8bit										
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	       //停止位2位									
	USART_InitStructure.USART_Parity = USART_Parity_No;		       //偶校验							
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发使能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制							
  USART_Init(USART2, &USART_InitStructure);   //初始化外设 USART2   
  USART_Cmd(USART2, ENABLE);					//使能USART外设
	
	/*7、使能USART2接收中断*/
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能接收中断
}
/**************************************************************
 ** 函数名 :fputc
 ** 功能   :重定向c库函数printf到USART
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :由printf调用
***************************************************************/
int fputc(int ch, FILE *f)
{
   /* 将Printf内容发往串口 */
   USART_SendData(USART2, (unsigned char) ch);
   /* 等待发送结束 */
   while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
   return (ch);
}
/**************************************************************
 ** 函数名 :fgetc
 ** 功能   :重定向c库函数scanf到USART
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :由scanf调用
***************************************************************/
int fgetc(FILE *f)
{
	/* 等待串口2输入数据 */
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART2);
}
/**************************************************************
 ** 函数名 :USART2_Putc
 ** 功能   :将USART2_Putc（）内容打印到串口
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void USART2_Putc(unsigned char c)
{
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
}
/**************************************************************
 ** 函数名 :USART1_Puts
 ** 功能   :将USART1_Puts（）内容打印到串口
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void USART1_Puts(char * str,u32 num)
{
    u32 i;
	for(i=0;i<num;i++)
    {
        USART_SendData(USART1, *str++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}
/**************************************************************
 ** 函数名 :USART2_Puts
 ** 功能   :将USART2_Puts（）内容打印到串口
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void USART2_Puts(char * str,u32 num)
{
    u32 i;
	for(i=0;i<num;i++)
    {
        USART_SendData(USART2, *str++);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}
/**************************************************************
 ** 函数名 :USART2_IRQHandler
 ** 功能   :中断函数，将串口接收到的函数直接从串口打印出来
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void USART2_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{	 
 		USART_ClearITPendingBit(USART2,    USART_IT_RXNE);
		res =USART_ReceiveData(USART2); 	//读取接收到的数据
		if(USART_RX_COUNT<30)
		{
			USART_RX_BUF[USART_RX_COUNT]=res;		//记录接收到的值
			USART_RX_COUNT++;						//接收数据增加1 
		} 
		else
		  USART_RX_COUNT=0;
	}  
}

/**************************************************************
 ** 函数名 :USART1_IRQHandler
 ** 功能   :中断函数，将串口接收到的函数直接从串口打印出来
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void USART1_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收到数据
	{	 
 		USART_ClearITPendingBit(USART1,    USART_IT_RXNE);
		if(RS485_RX_FLAG==0)
		{
				res =USART_ReceiveData(USART1); 	//if读取接收到的数据		
				RS485_RX_BUF[RS485_RX_COUNT]=res;	
				RS485_RX_COUNT++;
				if(RS485_RX_COUNT==1 && res!=0xff && res!=0xa0)
				{
					RS485_RX_COUNT=0;
				}
				if(( RS485_RX_COUNT==7 &&RS485_RX_BUF[0]==0xff)||( RS485_RX_COUNT==8 &&RS485_RX_BUF[0]==0xa0))
				{
					RS485_RX_FLAG=1;
				}
				if(RS485_RX_BUF[0]==0xff)RS485_RX_COUNT%=7;
				if(RS485_RX_BUF[0]==0xa0)RS485_RX_COUNT%=8;
		}
	}  
}



