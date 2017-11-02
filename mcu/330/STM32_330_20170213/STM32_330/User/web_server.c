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

int Web_Server(void)
{  
	mymac[0]=(ChipUniqueID%10000)&0xff;
	/* 初始化 enc28j60 的MAC地址(物理地址),这个函数必须要调用一次 */
	enc28j60Init(mymac);
	
	/* PHY LED 配置,LED用来指示通信的状态 */	
	enc28j60PhyWrite(PHLCON,0x476);	
	
	/* 将enc28j60第三引脚的时钟输出改为：from 6.25MHz to 12.5MHz(本例程该引脚NC,没用到) */	
	//enc28j60clkout(2);    
  
	/* 初始化以太网 IP 层 */
	init_ip_arp_udp_tcp(mymac,g_StaticIPaddr,mywwwport);

	return (0);	 	
}

void Write_EEPROM_IP(void)
{
	int i;
	U8 cch[5];
	for(i=0;i<4;i++)
	{
		cch[i]=g_StaticIPaddr[i];
	}
	m24xx_write(E2ROM_IP_ADDR,cch,5);
}
BOOL Read_EEPROM_IP(void)
{
	int i;
	U8 cch[5];
	int rc;
	rc=m24xx_read(E2ROM_IP_ADDR,cch,5);
	if(rc)
	{
		for(i=0;i<4;i++)
		{
			g_StaticIPaddr[i]=cch[i];
		}
	}
	return rc;
}

void DisMathineStaticIP()
{
	chinesedisplay(0x90, "IP:");
	DisThreeNumValue(g_StaticIPaddr[0],0x92);
	dat('.');
	DisThreeNumValue(g_StaticIPaddr[1],0x94);
	dat('.');
	DisThreeNumValue(g_StaticIPaddr[2],0x96);
	dat('.');
	DisThreeNumValue(g_StaticIPaddr[3],0x98);	
}

void SetStaticIPAddress(U8 wValue)
{
	switch(g_StaticIPIdx)
	{
		case IP1_IDX:
			DisThreeNumValue(wValue,0x90);
			dat('.');dat('_');
			g_StaticIPaddr[0]=wValue;
			g_StaticIPIdx=IP2_IDX;
			break;
		case IP2_IDX:
			DisThreeNumValue(wValue,0x92);
			dat('.');dat('_');
			g_StaticIPaddr[1]=wValue;
			g_StaticIPIdx=IP3_IDX;
			break;
		case IP3_IDX:
			DisThreeNumValue(wValue,0x94);
			dat('.');dat('_');
			g_StaticIPaddr[2]=wValue;
			g_StaticIPIdx=IP4_IDX;
			break;
		case IP4_IDX:
			DisThreeNumValue(wValue,0x96);
			g_StaticIPaddr[3]=wValue;
			g_StaticIPIdx=IP1_IDX;
			Web_Server();
			Write_EEPROM_IP();
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x82, g_SaveOKStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x82, g_SaveOKStrEn);
			delay(5);
			ReturnMainWin();
			break;
	}
}

void ReceiveEthernetPacket()
{ 
	unsigned int plen, i = 0;
	unsigned int dat_p;	
	plen = enc28j60PacketReceive(BUFFER_SIZE, EthernetBuf);
	if(plen==0)
	{
		return;
	}	     
	if(eth_type_is_arp_and_my_ip(EthernetBuf,plen))
	{
		make_arp_answer_from_request(EthernetBuf);          
		return;
	}
	// check if ip packets are for us:			
	if(eth_type_is_ip_and_my_ip(EthernetBuf,plen)==0) 
	{
		return;
	}      

	if(EthernetBuf[IP_PROTO_P]==IP_PROTO_ICMP_V && EthernetBuf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
	{
	// a ping packet, let's send pong  DOS 下的 ping 命令包		 
		make_echo_reply_from_request(EthernetBuf, plen);          
		return;
	}

	//-----------------tcp port www start, compare only the lower byte-----------------------------------//
	if (EthernetBuf[IP_PROTO_P]==IP_PROTO_TCP_V&&EthernetBuf[TCP_DST_PORT_H_P]==mywwwport>>8&&EthernetBuf[TCP_DST_PORT_L_P]==(mywwwport&0xff))
	{
		if (EthernetBuf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
		{
			make_tcp_synack_from_syn(EthernetBuf);
			// make_tcp_synack_from_syn does already send the syn,ack
			return;
		}
		if (EthernetBuf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
		{
			init_len_info(EthernetBuf); // init some data structures
			// we can possibly have no data, just ack:
			dat_p=get_tcp_data_pointer();
			if (dat_p==0)
			{
				if (EthernetBuf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
				{
					// finack, answer with ack
					make_tcp_ack_from_any(EthernetBuf);
				}
				// just an ack with no data, wait for next packet
				return;
			}			  
			if(!g_TryTimeEndFlag)  //判断试用是否到期
			{
				for(i=0;i<plen-dat_p;i++)
				{
					QueueIn(&g_strUartRxQueue,EthernetBuf[dat_p+i]);
				}
			}
			make_tcp_ack_from_any(EthernetBuf);       // send ack for http get
		}		  
	}	
	g_ComTypeFlag=Communication_Ethernet;
}

void Ethernet_SendBuf(U8 *data, U32 number)
{
	unsigned short num=0;
	//unsigned int plen;
	//为以太网返回数据
	//plen=fill_tcp_data_p(EthernetBuf,0,data); 
	//make_tcp_ack_with_data(EthernetBuf,plen); // send data
	make_PcEth(EthernetBuf);
	make_PcIp(EthernetBuf);
	make_PcPort(EthernetBuf);
	make_PcSeqAndSeqack(EthernetBuf,number);
	
	fill_tcp_data_pp(EthernetBuf,number,data);
	make_tcp_ack_with_data(EthernetBuf,number); // send data
	num=(EthernetBuf[0x12]<<8|EthernetBuf[0x13])+1;EthernetBuf[0x12]=num>>8;EthernetBuf[0x13]=num&0xff;	
}

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

