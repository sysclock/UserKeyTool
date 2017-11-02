/*
 *********************************************************************************************************
 * File:	i2c_io.c
 * Brief:	STM32F103x IO模拟I2C程序
 * Author:	Mars.zhu@Hotmail.com
 *
 * Description:
 *  IO模拟代码, CPU主频为72M
 *
 *	Rev:
 *		0.0		Mars.zhu -> 2009-6-18 10:31
 *				第一版本
 *		0.1		Mars.zhu -> 2009-9-17 10:33
 *				提供标准I2C函数, 并有24C02测试代码.
 *				不同的器件操作上可能有所不同, 比如W24C02写完一个字节需要延时!
 *********************************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

/** Includes -------------------------------------------------------------------------------------------*/
#include "option.h"
#include "ulib.h"
#include "driver.h"

#if	D_I2C_IO

/** Private macro --------------------------------------------------------------------------------------*/
#define	I2C_DELAY			200							// I2C延时
#define	I2C_ACK_TRIES		8

#define	I2C_SCL(x)			write_gpio_bit('B', GPB6_FOR_I2CSCK, x)
#define	I2C_SDA_W(x)		write_gpio_bit('B', GPB7_FOR_I2CSDA, x)
#define	I2C_SDA_R()			read_gpio_bit('B', GPB7_FOR_I2CSDA)

/*
 *********************************************************************************************************
 * Function Name: I2C_Delay
 * Description	: I2C延时程序.
 * Input		: None
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
static void
I2C_Delay(uint8 uc)
{
	uint8	i, j;

	for (i=0; i<uc; i++) {
		for (j=0; j<I2C_DELAY; j++);
	}
}

/*
 *********************************************************************************************************
 * Function Name: I2C_Start
 * Description	: I2C启动.
 * Input		: None
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
void
I2C_Start(void)
{
	I2C_SDA_W(1);
	I2C_SCL(1);				I2C_Delay(5);
	I2C_SDA_W(0);			I2C_Delay(5);
	I2C_SCL(0);
}

/*
 *********************************************************************************************************
 * Function Name: I2C_Stop
 * Description	: I2C停止.
 * Input		: None
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
void
I2C_Stop(void)
{
	I2C_SDA_W(0);
	I2C_SCL(0);
	I2C_SCL(1);				I2C_Delay(5);
	I2C_SDA_W(1);			I2C_Delay(5);
}

/*
 *********************************************************************************************************
 * Function Name: I2C_Ack
 * Description	: 发送应信号答.
 * Input		: None
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
void
I2C_Ack(void)
{
	I2C_SDA_W(0);
	I2C_SCL(1);				I2C_Delay(5);
	I2C_SCL(0);
}

/*
 *********************************************************************************************************
 * Function Name: I2C_NoAck
 * Description	: 发送无应答信号.
 * Input		: None
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
void
I2C_NoAck(void)
{
	I2C_SDA_W(1);
	I2C_SCL(1);		I2C_Delay(5);
	I2C_SCL(0);
}

/*
 *********************************************************************************************************
 * Function Name: I2C_WaitAck
 * Description	: 等待应答.
 * Input		: None
 * Output		: None
 * Return		: 0, 成功
 *********************************************************************************************************
 */
uint8
I2C_WaitAck(void)
{
	uint8 i;

	I2C_SDA_W(1);
	I2C_SCL(1);				I2C_Delay(5);

	for (i=0; i<I2C_ACK_TRIES; i++) {
		I2C_SDA_W(1);
		if (I2C_SDA_R() == 0) {
			i = 0;
			break;
		}
	}
	I2C_SDA_W(0);
	I2C_SCL(0);

	return i;
}

/*
 *********************************************************************************************************
 * Function Name: I2C_SendByte
 * Description	: I2C 发送8bit的数据.
 * Input		: -dat, 发送的数据
 * Output		: None
 * Return		: None
 *********************************************************************************************************
 */
void
I2C_SendByte(uint8 dat)
{
	uint8 i;

	for (i=0; i<8; i++) {
		I2C_SCL(0);			I2C_Delay(1);
		if (dat & __BIT(7))	I2C_SDA_W(1);
		else				I2C_SDA_W(0);
		dat <<= 1;
		I2C_Delay(5);
		I2C_SCL(1);			I2C_Delay(2);
	}
    I2C_SCL(0);
}

/*
 *********************************************************************************************************
 * Function Name: I2C_ReceiveByte
 * Description	: I2C 接收8bit的数据.
 * Input		: None
 * Output		: None
 * Return		: 接收到的数据
 *********************************************************************************************************
 */
uint8
I2C_ReceiveByte(void)
{
	uint8 i, dat=0;

	I2C_SDA_W(1);
	for (i=0; i<8; i++) {
		dat <<= 1;
		I2C_SCL(0);
		I2C_SDA_W(1);
		I2C_SCL(1);		I2C_Delay(1);
		if (I2C_SDA_R()) {
			dat	|= 0x01;
		}
	}
	I2C_SCL(0);
	I2C_SDA_W(0);

	return dat;
}

/** EEPROM test ----------------------------------------------------------------------------------------*/

/** EEPROM macro ---------------------------------------------------------------------------------------*/
#define	D_W2402_ADDR		0xA0
#define	D_W2402_PSIZE		8							// 24C02页大小为8Byte

/*
 *********************************************************************************************************
 * Function Name: EE_WriteByte
 * Description	: EEPROM 写一个字节的数据.
 * Input		: -addr,地址
 *				  -dat,	数据
 * Output		: None
 * Return		: 0, 成功
 *********************************************************************************************************
 */
uint8
EE_WriteByte(uint8 addr, uint8 dat)
{
	I2C_Start();
	I2C_SendByte(D_W2402_ADDR);							// 1. 器件地址
	I2C_WaitAck();
	I2C_SendByte(addr);									// 2. 数据地址
	I2C_WaitAck();
	I2C_SendByte(dat);
	I2C_WaitAck();
	I2C_Stop();
	return 0;
}

/*
 *********************************************************************************************************
 * Function Name: EE_ReadByte
 * Description	: EEPROM 读一个字节的数据.
 * Input		: -addr,地址
 * Output		: None
 * Return		: 接收到的数据
 *********************************************************************************************************
 */
uint8
EE_ReadByte(uint8 addr)
{
	uint8 dat;

	I2C_Start();
	I2C_SendByte(D_W2402_ADDR);							// 1. 器件地址
	I2C_WaitAck();
	I2C_SendByte(addr);									// 2. 数据地址
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(D_W2402_ADDR|__BIT(0));				// 3. 读取命令
	I2C_WaitAck();
	dat	= I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop();
	return dat;
}

/*
 *********************************************************************************************************
 * Function Name: EE_WritePage
 * Description	: EEPROM 按页写操作函数.
 * Input		: -addr,器件地址
 *				  -buf,	数据指针
 *				  -len, 数据长度, 不大于一页(D_W2402_PSIZE)
 * Output		: None
 * Return		: 0, 成功
 *********************************************************************************************************
 */
uint8
EE_WritePage(uint8 addr, uint8 *buf, uint8 len)
{
	uint8	i;

	if (len > D_W2402_PSIZE) {
		len = D_W2402_PSIZE;
	}
	I2C_Start();
	I2C_SendByte(D_W2402_ADDR);							// 1. 器件地址
	I2C_WaitAck();
	I2C_SendByte(addr);									// 2. 数据地址
	I2C_WaitAck();

	for (i=0; i<len; i++) {								// 3. 数据
		I2C_SendByte(buf[i]);
		I2C_WaitAck();
	}
	I2C_Stop();

	return 0;
}

/*
 *********************************************************************************************************
 * Function Name: EE_Read
 * Description	: EEPROM 序列读操作函数.
 * Input		: -addr,器件地址
 *				  -buf,	数据指针
 *				  -len, 数据长度(无长度限制)
 * Output		: None
 * Return		: 0, 成功
 *********************************************************************************************************
 */
uint8
EE_Read(uint16 addr, uint8 *buf, uint16 len)
{
	uint16	i;

	I2C_Start();
	I2C_SendByte(D_W2402_ADDR);							// 1. 器件地址
	I2C_WaitAck();
	I2C_SendByte(addr);									// 2. 数据地址
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(D_W2402_ADDR|__BIT(0));				// 3. 读器件
	I2C_WaitAck();

	for (i=0; i<len; i++) {
		buf[i] = I2C_ReceiveByte();
		if (i != (len-1)) {
			I2C_Ack();
		} else {
			I2C_NoAck();
		}
	}
	I2C_Stop();
	return 0;
}

#if 1
void
EE_test(void)
{
	uint16	i;
	uint8	buf[256];

	for (i=0; i<256; i++) {
		buf[i] = i+0x80;
	}
#if 1
	/*
	 *********************************************************************************************************
	 * 字节写
	 *********************************************************************************************************
	 */
	Uart_Printf("Byte write:\n");
	for (i=0; i<256; i++) {
		EE_WriteByte(i, buf[i]);
		Delay(1);
	}

#if 1
	/*
	 *********************************************************************************************************
	 * 字节读
	 *********************************************************************************************************
	 */
	Uart_Printf("Byte read:\n");
	for (i=0; i<256; i++) {
		buf[i] = 0xFF;
	}
	for (i=0; i<256; i++) {
		buf[i] = EE_ReadByte(i);
	}
	for (i=0; i<256; i++) {
		Uart_Printf(" %02X", buf[i]);
		if (((i+1)%16)==0) {
			Uart_Printf("\n");
		}
	}
	Uart_Printf("\n");
#endif

#if 1
	/*
	 *********************************************************************************************************
	 * 字符串读
	 *********************************************************************************************************
	 */
	Uart_Printf("String read:\n");
	for (i=0; i<256; i++) {
		buf[i] = 0xFF;
	}
	EE_Read(0, buf, 256);
	for (i=0; i<256; i++) {
		Uart_Printf(" %02X", buf[i]);
		if (((i+1)%16)==0) {
			Uart_Printf("\n");
		}
	}
	Uart_Printf("\n");
#endif
#endif

#if 1
	/*
	 *********************************************************************************************************
	 * 字符串写
	 *********************************************************************************************************
	 */
	Uart_Printf("String write:\n");
	for (i=0; i<256; i++) {
		buf[i] = i+0x00;
	}
	for (i=0; i<256; i+=D_W2402_PSIZE) {
		EE_WritePage(i, buf+i, D_W2402_PSIZE);
		Delay(1);
	}

#if 1
	/*
	 *********************************************************************************************************
	 * 字节读
	 *********************************************************************************************************
	 */
	Uart_Printf("Byte read:\n");
	for (i=0; i<256; i++) {
		buf[i] = 0xFF;
	}
	for (i=0; i<256; i++) {
		buf[i] = EE_ReadByte(i);
	}
	for (i=0; i<256; i++) {
		Uart_Printf(" %02X", buf[i]);
		if (((i+1)%16)==0) {
			Uart_Printf("\n");
		}
	}
	Uart_Printf("\n");
#endif

#if 1
	/*
	 *********************************************************************************************************
	 * 字符串读
	 *********************************************************************************************************
	 */
	Uart_Printf("String read:\n");
	for (i=0; i<256; i++) {
		buf[i] = 0xFF;
	}
	EE_Read(0, buf, 256);
	for (i=0; i<256; i++) {
		Uart_Printf(" %02X", buf[i]);
		if (((i+1)%16)==0) {
			Uart_Printf("\n");
		}
	}
	Uart_Printf("\n");
#endif
#endif

	/*
	 *********************************************************************************************************
	 * Test Over
	 *********************************************************************************************************
	 */
//	EnableInterrupt();
	return;
}
#endif

#endif	/** D_I2C_IO*/

#ifdef __cplusplus
}
#endif
