/*********************************************************************************
 * 文件名  ：simple_server.c
 * 描述    ：web服务程序应用函数库
 *          
 * 硬件连接： ------------------------------------
 *           |PB1         ：ENC28J60-INT|
 *           |PA6-SPI1-MISO：ENC28J60-SO          |
 *           |PA7-SPI1-MOSI：ENC28J60-SI           |
 *           |PA5-SPI1-SCK ：ENC28J60-SCK        |
 *           |PA4-SPI1-NSS ：ENC28J60-CS          |
 *           |ENC28J60-RST (没用)                      |
 *            ------------------------------------
 *
**********************************************************************************/
#include <string.h>
#include "Queue.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"

#ifdef TCP_IP

#define BUFFER_SIZE 155             //网络数据包的大小

static unsigned char EthernetBuf[BUFFER_SIZE+1];
/* mac地址和ip地址在局域网内必须唯一，否则将与其他主机冲突，导致连接不成功 */
static unsigned char mymac[6] = {0x00,0x54,0x55,0x58,0x10,0x24};
static unsigned char g_StaticIPaddr[4] = {192,168,1,15};
/* tcp/www监听端口号，范围为:1-254 */
static unsigned short mywwwport =90; 

#ifdef ENC28J60INT

#ifdef STM32F103RB
void Exit_Enc28j60INT_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}

#else
void Exit_Enc28j60INT_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// Enable Button GPIO clock //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	// Configure Button pin as input floating //
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

	// Connect Button EXTI Line to Button GPIO Pin //
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	// Configure Button EXTI line //
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	// Enable and set Button EXTI Interrupt to the lowest priority //
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}
#endif

#endif

#endif

 
#ifdef EC11_BUTTON
void Exit_EC11_Pin4_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = BMC_PIN;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(BMC_GPIO, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}
#ifdef TWO_EC11_BOARD
void Exit_EC11U15_Pin4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = BMC2_PIN;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(BMC2_GPIO, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}
#endif
#endif

