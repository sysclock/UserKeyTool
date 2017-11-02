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
#include "spi_enc28j60.h"

#ifdef TCP_IP

/*
 * 函数名：SPI1_Init
 * 描述  ：ENC28J60 SPI 接口初始化
 * 输入  ：无 
 * 输出  ：无
 * 返回  ：无
 */																						  
void SPI_Enc28j60_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* 使能 SPI1 时钟 */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------enc28j60 通信I/O初始化----------------
	 * PA5-SPI1-SCK :ENC28J60_SCK
	 * PA6-SPI1-MISO:ENC28J60_SO
	 * PA7-SPI1-MOSI:ENC28J60_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------enc28j60 控制I/O初始化----------------*/
	/* PA4-SPI1-NSS:ENC28J60_CS */ 											 // 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // 推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);										 // 先把片选拉高，真正用的时候再拉低

	/* PB13:ENC28J60_INT */						// 中断引脚没用到

	/* PE1:ENC28J60_RST*/				      // 复位似乎不用也可以
 

	/* SPI1 配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/* 使能 SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
}

static __IO uint32_t  sSPITimeout = 0x10000;   

/*
 * 函数名：SPI1_ReadWrite
 * 描述  ：SPI1读写一字节数据
 * 输入  ： 
 * 输出  ：
 * 返回  ：
 */
unsigned char	SPI1_ReadWrite(unsigned char writedat)
{
	sSPITimeout = 0x10000; 
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((sSPITimeout--) == 0) 
			return 0;
	}
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, writedat);
	
	sSPITimeout = 0x10000; 
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((sSPITimeout--) == 0) 
			return 0;
	}
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
#endif

