/*
www.mcu123.net 

LPC236X AT 24C16演示程序
IIC演示程序

功能：读写AT24CXX系列IIC EEPROM
实现方式：非中断方式

2007.3.3创建

验证：已经测试	 

版本：v1.11

更新时间：2007.5.29  boy123

*/
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "header.h"
#include <stdio.h>


#ifndef NULL
#define NULL 0
#endif

#define _I2C_C_

#define AT24C01A		//24C01A,I2C时序和往后的24C02一样
//#define AT24C01		//24C01,I2C的时序和普通的有点不同

#define EEPROM_ADDR		0xA0
#define I2C_PAGESIZE	32		//24C01/01A页缓冲是4个
//-----------set chedule -------------------------------------
U8 g_InputTime1[2]={0xff,0xff};   //hour   min 
U8 g_InputWeek1[7]={0,0,0,0,0,0,0};
U8 g_InputWeek1Idx=0;

U8 g_InputTime2[2]={0xff,0xff};
U8 g_InputWeek2[7]={0,0,0,0,0,0,0};
U8 g_InputWeek2Idx=0;

U8 g_InputTime3[2]={0xff,0xff};
U8 g_InputWeek3[7]={0,0,0,0,0,0,0};
U8 g_InputWeek3Idx=0;
//-----------------------------------------------------------

// U16 wrbuf[10]; //write
//U16 rebuf[2];	//read

U8 genericEEPROMMemU8[EEPROMU8_SIZE] = {

	INDEX1024X768,SINGLEIMAGE,

	INIMAINSRC,INITPIPSRC,
	DEF_BRIGHTNESS,DEF_CONTRACT,DEF_SATURATION,
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
	DEF_MAIN_BRIGHTNESS_RED,DEF_MAIN_BRIGHTNESS_GREEN,
	DEF_MAIN_BRIGHTNESS_BLUE,DEF_MAIN_CONTRACT_RED,
	DEF_MAIN_CONTRACT_GREEN,DEF_MAIN_CONTRACT_BLUE,
	DEF_SUB_BRIGHTNESS_RED ,DEF_SUB_BRIGHTNESS_GREEN,
	DEF_SUB_BRIGHTNESS_BLUE,DEF_SUB_CONTRACT_RED,
	DEF_SUB_CONTRACT_GREEN ,DEF_SUB_CONTRACT_BLUE,
#endif
	0x14
	};//cs=0x14

U8 EEPROMMemRegU8[EEPROMU8_SIZE]= {

	INDEX1024X768,SINGLEIMAGE,

	INIMAINSRC,INITPIPSRC,
	DEF_BRIGHTNESS,DEF_CONTRACT,DEF_SATURATION,
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
	DEF_MAIN_BRIGHTNESS_RED,DEF_MAIN_BRIGHTNESS_GREEN,
	DEF_MAIN_BRIGHTNESS_BLUE,DEF_MAIN_CONTRACT_RED,
	DEF_MAIN_CONTRACT_GREEN,DEF_MAIN_CONTRACT_BLUE,
	DEF_SUB_BRIGHTNESS_RED ,DEF_SUB_BRIGHTNESS_GREEN,
	DEF_SUB_BRIGHTNESS_BLUE,DEF_SUB_CONTRACT_RED,
	DEF_SUB_CONTRACT_GREEN ,DEF_SUB_CONTRACT_BLUE,
#endif
	0x14
	};

U16 genericEEPROMMemU16[EEPROMU16_SIZE] = {
//#if(XP_TO_ID==XP_TO_6M16)
	DEF_SCREEN_HSIZE,DEF_SCREEN_VSIZE ,DEF_SCREEN_HPOS,DEF_SCREEN_VPOS,
	DEF_MAIN_SCALER_HSIZE,DEF_MAIN_SCALER_VSIZE ,
	DEF_MAIN_SCALER_HPOS,DEF_MAIN_SCALER_VPOS,
//#endif
	DEF_SUB_SCALER_HSIZE ,DEF_SUB_SCALER_VSIZE,
	DEF_SUB_SCALER_HPOS ,DEF_SUB_SCALER_VPOS ,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0x14
	};

U16 EEPROMMemRegU16[EEPROMU16_SIZE]= {
//#if(XP_TO_ID==XP_TO_6M16)
	DEF_SCREEN_HSIZE,DEF_SCREEN_VSIZE ,DEF_SCREEN_HPOS,DEF_SCREEN_VPOS,
	DEF_MAIN_SCALER_HSIZE,DEF_MAIN_SCALER_VSIZE ,
	DEF_MAIN_SCALER_HPOS,DEF_MAIN_SCALER_VPOS,
//#endif
	DEF_SUB_SCALER_HSIZE ,DEF_SUB_SCALER_VSIZE,
	DEF_SUB_SCALER_HPOS ,DEF_SUB_SCALER_VPOS ,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0x14
	};
U8 InputEE[INPUT_SIZE]={
	0,0,0,0,0xff
};

void My_Delay_us(u8 n)
{
	u8 i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<8;j++)
			;
	}
}



#if(ENABLE_HARDI2C)	

void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //重映射时钟	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    
	#ifdef STM32F103RB
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, DISABLE); 	   //禁止重映射
	#else
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); 	   //将I2C1重映射到	  PB8 PB9
	#endif

	// PB6,7 SCL and SDA //
	GPIO_InitStructure.GPIO_Pin =  EEPROM_SCL_PIN | EEPROM_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;//100K速度
    
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	//允许1字节1应答模式//
	I2C_AcknowledgeConfig(I2C1, ENABLE);

//    printf("I2C_Configuration----\n\r");
}

////////////////IO Emulate End/////////////////////////////////////

//-***************************************************
//-**函数名:I2C_ReadS: 
//-**功能:读取24c04多个字节
//-意事项:24C02是256字节,8位地址,A0-A2固定为0,从器件地址为EEPROM_ADDR
//-***************************************************-/
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no)
{
    if(no==0)
		return;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
	//-*允许1字节1应答模式 
	I2C_AcknowledgeConfig(I2C1, ENABLE);


	//- 发送起始位 
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//-EV5,主模式 

#ifdef AT24C01A	
    //-发送器件地址(写)
    I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//-发送地址 
	//I2C_SendData(I2C1, addr);	 	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//-数据已发送 
		
	//-起始位 
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	//-器件读 
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	
#else	
	//-发送器件地址(读)24C01 
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (no)
    {
		if(no==1)
		{
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位后要关闭应答的
    		I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
		}
	    
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); //- EV7  
	    *pBuffer = I2C_ReceiveData(I2C1);
	    pBuffer++;
	   //- Decrement the read bytes counter  
	    no--;
    }
	//再次允许应答模式
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

//-***************************************************
//-**函数名:I2C_Standby_24C
//-**功能:24C是否准备好再写入的判断
//-**注意事项:本函数可以理解为:判忙
//-***************************************************-//
void I2C_Standby_24C(void)      
{
  vu16 SR1_Tmp;
  do
  {
    //起始位
    I2C_GenerateSTART(I2C1, ENABLE);
    //读SR1
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    //器件地址(写)
    #ifdef AT24C01A
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	#else
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Transmitter);
	#endif
  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
 
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  //停止位   
  I2C_GenerateSTOP(I2C1, ENABLE);
}

//--*************************************************
//--**函数名:I2C_ByteWrite_24C
//--**功能:写一个字节
//--**注意事项:字写入同样需要调用忙判断
//--*************************************************
void I2C_ByteWrite_24C(u16 addr,u8 dat)
{
  // 起始位
  	I2C_GenerateSTART(I2C1, ENABLE);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
  	//-- 发送器件地址(写)
  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  //--发送地址
  	//I2C_SendData(I2C1, addr);	 	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	//-- 写一个字节
  	I2C_SendData(I2C1, dat); 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	// 停止位
  	I2C_GenerateSTOP(I2C1, ENABLE);
  
  	I2C_Standby_24C();
}

////--*************************************************
////--**函数名:I2C_PageWrite_24C
////--**功能:写入一页(以内)
////--**注意事项:此函数供群写入调用
////--*************************************************
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	//起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 

#ifdef AT24C01A
	//器件地址(写)
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	//写地址值
	//I2C_SendData(I2C1, addr);	  	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); 
#endif
	while(no--)  
	{
	  I2C_SendData(I2C1, *pBuffer); 
	  pBuffer++; 
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	//停止位
	I2C_GenerateSTOP(I2C1, ENABLE);
}

//--************************************************
////--函数名:I2C_WriteS_24C
////--功能:页写入24C
////--注意事项:24C02最多允许一次写入8个字节
////--***********************************************
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no)
{
	u8 temp;

	//1.先把页不对齐的部分写入
	temp=addr % I2C_PAGESIZE;
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		if(no>=temp)
		{
			I2C_PageWrite_24C(addr,pBuffer,  temp);
			no-=temp;
			addr+=temp;
			pBuffer+=temp;
		}
		else
		{
			I2C_PageWrite_24C(addr,pBuffer,  no);
			no=0;
		}
		I2C_Standby_24C();
	}
	//2.从页对齐开始写
	while(no)
	{
		if(no>=I2C_PAGESIZE)
		{
			I2C_PageWrite_24C(addr,pBuffer,  I2C_PAGESIZE);
			no-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
			I2C_Standby_24C();
		}
		else
		{
			I2C_PageWrite_24C(addr,pBuffer,  no);
			no=0;
			I2C_Standby_24C();
		}
	}
}
#else

////////////////-----------IO Emulate Start------------/////////////////////////////////////////

///--*************************************************************
//--*函数名称：I2CStart
//--*函数功能：I2C开始信号
//--*输入参数：
//--*输出参数：
//--*备           注：时钟线高时，数据线由高到低的跳变，表示I2C开始信号
//--**************************************************************
void I2CStart(void) 
{	
	EEPROM_SDA_1;
	My_Delay_us(1);
	EEPROM_SCL_1;
	My_Delay_us(1);
	EEPROM_SDA_0;
	My_Delay_us(1);
	EEPROM_SCL_0;
 }

///--*************************************************************
//--*函数名称：I2CStop
//--*函数功能：I2C停止信号
//--*输入参数：
//--*输出参数：
//--*备           注：时钟线高时，数据线由低到高的跳变，表示I2C停止信号
//--**************************************************************
void I2CStop(void) 
{
	EEPROM_SDA_0;
	My_Delay_us(1);
	EEPROM_SCL_1;
	My_Delay_us(1);
	EEPROM_SDA_1;
	My_Delay_us(1);
	EEPROM_SCL_0;
 }

///--*************************************************************
//--*函数名称：I2CSlaveAck
//--*函数功能：I2C从机设备应答查询
//--*输入参数：
//--*输出参数：
//--*备           注：
//--**************************************************************
unsigned char I2CSlaveAck( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;
  unsigned int TimeOut;
  unsigned char RetValue;

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //这里一定要设成输入上拉，否则不能读出数据
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
  GPIO_Init( GPIOB, &GPIO_InitStruct );

  EEPROM_SCL_1;
  TimeOut = 10000;
  while( TimeOut-- > 0 )
  {
    if( SET == EEPROM_SDA )
    {
      RetValue = RESET;
      break;
    }
    else
    {
      RetValue = SET;
    }
  }
  EEPROM_SCL_0;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init( GPIOB, &GPIO_InitStruct );
  
  return RetValue;
}

void I2CMasterAck(void)
{
  	EEPROM_SDA_0;
	My_Delay_us(1);
 	EEPROM_SCL_1;
	My_Delay_us(1);
  	EEPROM_SCL_0;
	My_Delay_us(1);
}



///--*************************************************************
//--*函数名称：I2CWriteByte
//--*函数功能：I2C写一字节数据
//--*输入参数：
//--*输出参数：
//--*备           注：
//--**************************************************************
void I2CWriteByte(U8 Data)
{
	U8 i;
	for(i=0; i<8; i++) 
	{
		if(Data&0x80)
			EEPROM_SDA_1;
		else
			EEPROM_SDA_0;
		Data<<=1;
		My_Delay_us(1);
		EEPROM_SCL_1;
		My_Delay_us(1);
		EEPROM_SCL_0;
		My_Delay_us(1);
	}
//	Waitack();
}
//--*************************************************************
//--*函数名称：I2CReadByte
//--*函数功能：I2C读一字节数据
//--*输入参数：
//--*输出参数：
//--*备           注：
//--**************************************************************
U8 I2CReadByte(void)
{
	U8 i,bytedata=0;
  	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //这里一定要设成输入上拉，否则不能读出数据
	GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStruct );
	
//	GPIOB->CRH &= 0xfffffff0;
//	GPIOB->CRH |= 0x00000004;  //PB8  浮空输入
	for(i=0; i<8; i++)
	{
		bytedata<<=1;
		EEPROM_SCL_1;
		My_Delay_us(1);
		bytedata|=EEPROM_SDA;
		EEPROM_SCL_0;
		My_Delay_us(1);
	}
	
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;      //PB8  推挽输出 50MHz	
  	GPIO_Init( GPIOB, &GPIO_InitStruct );
//	GPIOB->CRH &= 0xfffffff0;
//	GPIOB->CRH |= 0x00000001;  //PB8  推挽输出 10MHz	
	return bytedata;
}


////////////////IO Emulate End/////////////////////////////////////

///--***************************************************
//--**函数名:I2C_ReadS: 
//--**功能:读取
//--**注意事项:24C64是8K字节,16位地址,A0-A2固定为0,从器件地址为EEPROM_ADDR
//--***************************************************
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no)
{
       if(no==0)
		return;
	///--*起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	
	//发送地址	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//起始位
	I2CStart();
	
	//发送器件地址(读)24C64
  	I2CWriteByte( EEPROM_ADDR|0x01 );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	
    	while (no)
    	{	
	    	*pBuffer = I2CReadByte();
		//printf("%x  ",*pBuffer);
	    	pBuffer++;
		if(no!=1)
			I2CMasterAck();
	    	// Decrement the read bytes counter 
	    	no--;
    	}
    	I2CStop();			//发送停止位
}

///--*************************************************
//--**函数名:I2C_ByteWrite_24C
//--**功能:写一个字节
//--**注意事项:字写入同样需要调用忙判断
//--*************************************************
void I2C_ByteWrite_24C(u16 addr,u8 dat)
{
	//起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//发送地址	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;

       My_Delay_us(1);
	//写一个字节
  	I2CWriteByte(dat);
  	if( RESET == I2CSlaveAck() )  	
   		return;
       My_Delay_us(1);
	I2CStop();
}

void I2C_PageEraser_24C(u16 addr,u8 no)
{
	//起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//发送地址	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;

       My_Delay_us(1);
	while(no--)  
	{
	  	I2CWriteByte(0xFF);	//printf("[%x]  ",*pBuffer);
	  	
  		if( RESET == I2CSlaveAck() )  	
   			return;
	}
	
	//停止位
	I2CStop();
}

//--*************************************************
//--**函数名:I2C_PageWrite_24C
//--**功能:写入一页(以内)
//--**注意事项:此函数供群写入调用
//--*************************************************
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no)
{
	//起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//发送地址	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;

       My_Delay_us(1);
	while(no--)  
	{
	  	I2CWriteByte(*pBuffer);	//printf("[%x]  ",*pBuffer);
	  	
	 	pBuffer++; 
  		if( RESET == I2CSlaveAck() )  	
   			return;
	}
	
	//停止位
	I2CStop();
}
///--*************************************************
//--**函数名:I2C_WriteS_24C
//--**功能:页写入24C
//--**注意事项:24C64最多允许一次写入32个字节
//--*************************************************
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no)
{
	u8 temp;

	//1.先把页不对齐的部分写入
	temp=addr % I2C_PAGESIZE;
	
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		delay1000(100);
		if(no>=temp)
		{
			I2C_PageWrite_24C(addr,pBuffer,  temp);
			no-=temp;
			addr+=temp;
			pBuffer+=temp;
		}
		else
		{
			I2C_PageWrite_24C(addr,pBuffer,  no);
			return;
		}
	}
	//2.从页对齐开始写
	while(no)
	{
		delay1000(100);
		if(no>=I2C_PAGESIZE)
		{
			I2C_PageWrite_24C(addr,pBuffer,  I2C_PAGESIZE);
			no-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
		}
		else
		{
			I2C_PageWrite_24C(addr,pBuffer,  no);
			no=0;
		}
	}
}

///--*************************************************
//--**函数名:I2C_WriteS_24C
//--**功能:页写入24C
//--**注意事项:24C64最多允许一次写入32个字节
//--*************************************************
void I2C_EraserS_24C(u16 addr, u16 no)
{
	u8 temp;

	//1.先把页不对齐的部分写入
	temp=addr % I2C_PAGESIZE;
	
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		delay1000(100);
		if(no>=temp)
		{
			I2C_PageEraser_24C(addr,temp);
			no-=temp;
			addr+=temp;
		}
		else
		{
			I2C_PageEraser_24C(addr,no);
			return;
		}
	}
	//2.从页对齐开始写
	while(no)
	{
		delay1000(100);
		if(no>=I2C_PAGESIZE)
		{
			I2C_PageEraser_24C(addr,I2C_PAGESIZE);
			no-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
		}
		else
		{
			I2C_PageEraser_24C(addr,no);
			no=0;
		}
	}
}

#if 0
unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no)
{
       if(no==0)
		return 1;
	///--*起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( eepaddr);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	//起始位
	I2CStart();
	
	//发送器件地址(读)24C64
  	I2CWriteByte( eepaddr|0x01 );
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	
    	while (no)
    	{	
	    	*pBuffer = I2CReadByte();
	    	pBuffer++;
		if(no!=1)
			I2CMasterAck();
	    	// Decrement the read bytes counter 
	    	no--;
    	}
    	I2CStop();			//发送停止位
    	return 0;
}

unsigned char  I2C_ByteWriteN_ToDevice(u8 eepaddr,u8 addr, u8 byteno, u8 *p_data)
{	
	unsigned char i;
	//起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( eepaddr);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
       My_Delay_us(1);

	for(i=0; i<byteno-1; i++)
	{
		I2CWriteByte(*p_data);
	  	if( RESET == I2CSlaveAck() )  	
	   		return 1;
		p_data++;
	}
	I2CWriteByte(*p_data);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	else
	{
		I2CStop();
		return 0;
	}
}

unsigned char I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat)
{		
	//起始位
	I2CStart();
	//发送器件地址(写)24C64
  	I2CWriteByte( eepaddr);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;

       My_Delay_us(1);
	//写一个字节
  	I2CWriteByte(dat);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
       My_Delay_us(1);
	I2CStop();
	return 0;
}
#endif
  
#endif

#ifdef HOT_BACKUP
void Write_EEPROM_HotBackupInput(int addr)
{
	U8 cch[17];
	int i=0,j=0,idx=0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<2;j++)
		{
			cch[idx++]=g_HotBackupInput[i][j];
		}
	}
	m24xx_write(addr,cch,17);
}

void Read_EEPROM_HotBackupInput(int addr)
{
	U8 cch[17];
	int rc;
	int i=0,j=0,idx=0;
	rc=m24xx_read(addr,cch,17);
	if(rc)
	{
		for(i=0;i<8;i++)
		{
			for(j=0;j<2;j++)
			{
				g_HotBackupInput[i][j]=(_InputSourceType)cch[idx++];
			}
		}
	}
	
}
#endif

#if(XP_TO_ID==XP_TO_FLI32626)
void Write_Eeprom_UserResolution(int addr)
{
	U8 cch[5];
	cch[0]=(UserResolutionHSize&0xff);
	cch[1]=(UserResolutionHSize>>8);
	cch[2]=(UserResolutionVSize&0xff);
	cch[3]=(UserResolutionVSize>>8);
	m24xx_write(addr,cch,5);
	
}

BOOL Read_Eeprom_UserResolution(int addr)
{
	U8 cch[5];
	int rc;
	rc=m24xx_read(addr,cch,5);
	if(rc)
	{
		UserResolutionHSize=(cch[1]<<8|cch[0]);
		UserResolutionVSize=(cch[3]<<8|cch[2]);
	}
	return rc;
}
//========================================================
void Read_EEPROM_ScreenScaler()
{

	U8 cch[5];
	int rc;
	rc=m24xx_read(E2ROM_SCREEN_H_SIZE,cch,5);
	if(rc)
	{
		g_ScreenHSize=(cch[0]<<8|cch[1]);
		if(g_ScreenHSize>g_PanelWidth)
			g_ScreenHSize=g_PanelWidth;
		g_MainScalerHSize=(cch[2]<<8|cch[3]);
		if(g_MainScalerHSize>g_ScreenHSize)
			g_MainScalerHSize=g_ScreenHSize;
	}
	rc=m24xx_read(E2ROM_SCREEN_V_SIZE,cch,5);
	if(rc)
	{
		g_ScreenVSize=(cch[0]<<8|cch[1]);
		if(g_ScreenVSize>g_PanelHeight)
			g_ScreenVSize=g_PanelHeight;
		g_MainScalerVSize=(cch[2]<<8|cch[3]);
		if(g_MainScalerVSize>g_ScreenVSize)
			g_MainScalerVSize=g_ScreenVSize;
	}
	rc=m24xx_read(E2ROM_SCREEN_H_POS,cch,5);
	if(rc)
	{
		g_ScreenHPos=(cch[0]<<8|cch[1]);
		if(g_ScreenHPos+g_ScreenHSize>g_PanelWidth)
			g_ScreenHPos=g_PanelWidth-g_ScreenHSize;
		g_MainScalerHPos=(cch[2]<<8|cch[3]);
		if(g_MainScalerHPos+g_MainScalerHSize>g_ScreenHPos+g_ScreenHSize)
			g_MainScalerHPos=g_ScreenHPos+g_ScreenHSize-g_MainScalerHSize;
	}
	rc=m24xx_read(E2ROM_SCREEN_V_POS,cch,5);
	if(rc)
	{
		g_ScreenVPos=(cch[0]<<8|cch[1]);
		if(g_ScreenVPos+g_ScreenVSize>g_PanelHeight)
			g_ScreenVPos=g_PanelWidth-g_ScreenVSize;
		g_MainScalerVPos=(cch[2]<<8|cch[3]);
		if(g_MainScalerVPos+g_MainScalerVSize>g_ScreenVPos+g_ScreenVSize)
			g_MainScalerVPos=g_ScreenVPos+g_ScreenVSize-g_MainScalerVSize;
	}
}
//-------------------------------------------------------------------------
void Write_EEPROM_ScreenScaler(int addr,U16 Screen,U16 Scaler)
{
	U8 cch[5];
	cch[0]=(Screen>>8);
	cch[1]=(Screen&0xff);
	cch[2]=(Scaler>>8);
	cch[3]=(Scaler&0xff);
	m24xx_write(addr,cch,5);
}
//-------------------------------------------------------------------------
#endif


void Write_EEPROM_U16(int addr,U16 value)
{	
	U8 cch[3];
	cch[0]=value&0xFF;
	cch[1]=value>>8;
	m24xx_write(addr,cch,3);
}

void Read_EEPROM_U16(int addr,U16 *ch)
{
	U8 cch[3];
	int rc;
	rc=m24xx_read(addr,cch,3);
	if(rc)
	{
		if(ch)
		{
			*ch=cch[1]<<8|cch[0];
		}
	}
}
/*
void Read_EEPROM_MathineType(void)
{
	U8 cch[11];
	int rc=0;
	U8 num=0;
	Read_EEPROM_Byte(E2ROM_MathineType_Num_ADDR, &num);
	if(num!=0)
		rc=m24xx_read(E2ROM_MathineType_ADDR,cch,num+1);
	if(rc)
	{
		Copy_Mathine_Type(cch,num);
	}
	else
		InitMathineType();
}
*/

BOOL Read_EEPROM_Byte(int addr,U8 *ch)
{
	U8 cch[2];
	int rc;
	rc=m24xx_read(addr,cch,2);
	if(rc)
	{
		if(ch)
		{
			*ch=cch[0];
		}
	}
	return rc;
}
//-------------------------------------------------------------------------

BOOL Read_EEPROM_Telephone(int addr,U8 *buf,U8 num)
{
	int i=0;
	U8 cch[12];
	int rc;
	rc=m24xx_read(addr,cch,num+1);
	if(rc)
	{
		for(i=0;i<num;i++)
			buf[i]=cch[i];
	}
	return rc;
}
//-------------------------------------------------------------------------
void Write_EEPROM_Byte(int addr,U8 ch)
{
	U8 cch[2];
	cch[0]=ch;
	m24xx_write(addr,cch,2);
}

void Write_EEPROM_UserPWD(int addr)
{
	U8 cch[5];
	cch[0]=g_UserSetPasswordVal&0xFF;
	cch[1]=g_UserSetPasswordVal>>8;
	cch[2]=g_UserSetPasswordVal>>16;
	cch[3]=g_UserSetPasswordVal>>24;
	m24xx_write(addr,cch,5);
}

BOOL Read_EEPROM_UserPWD(int addr)
{
	U8 cch[5];
	int rc;
	rc=m24xx_read(addr,cch,5);
	if(rc)
	{
		g_UserSetPasswordVal=cch[3]<<24|cch[2]<<16|cch[1]<<8|cch[0];
	}
	return rc;
}

void Write_EEPROM_PcPart()
{
	U8 cch[4];
	cch[0]=g_VGAFullOrPartValue;
	cch[1]=g_DVIFullOrPartValue;
	cch[2]=g_HDMIFullOrPartValue;
	m24xx_write(E2ROM_FULL_PART_ADDR,cch,4);
}

BOOL Read_EEPROM_PcPart()
{
	U8 cch[4];
	int rc;
	rc=m24xx_read(E2ROM_FULL_PART_ADDR,cch,4);
	if(rc)
	{
		g_VGAFullOrPartValue=cch[0];
		g_DVIFullOrPartValue=cch[1];
		g_HDMIFullOrPartValue=cch[2];
	}
	return rc;
}
//========================================================

void Write_EEPROM_InputCycle(int addr,_InputSourceType Input,U8 Hour,U8 Min)
{	
	U8 cch[4];
	cch[0]=Input;
	cch[1]=Hour;
	cch[2]=Min;
	m24xx_write(addr,cch,4);
}


void Read_EEPROM_InputCycle(int addr)
{
	U8 cch[4];
	int rc;
	rc=m24xx_read(addr,cch,4);
	if(rc)
	{
		switch(addr)
		{

			case E2ROM_CycleInput1_ADDR:
				g_CycleInput1=(_InputSourceType)cch[0];
				g_CycleInputHour1=cch[1];
				g_CycleInputMin1=cch[2];
				break;
			case E2ROM_CycleInput2_ADDR:
				g_CycleInput2=(_InputSourceType)cch[0];
				g_CycleInputHour2=cch[1];
				g_CycleInputMin2=cch[2];
				break;
			case E2ROM_CycleInput3_ADDR:
				g_CycleInput3=(_InputSourceType)cch[0];
				g_CycleInputHour3=cch[1];
				g_CycleInputMin3=cch[2];
				break;
			case E2ROM_CycleInput4_ADDR:
				g_CycleInput4=(_InputSourceType)cch[0];
				g_CycleInputHour4=cch[1];
				g_CycleInputMin4=cch[2];
				break;
			case E2ROM_CycleInput5_ADDR:
				g_CycleInput5=(_InputSourceType)cch[0];
				g_CycleInputHour5=cch[1];
				g_CycleInputMin5=cch[2];
				break;
			case E2ROM_CycleInput6_ADDR:
				g_CycleInput6=(_InputSourceType)cch[0];
				g_CycleInputHour6=cch[1];
				g_CycleInputMin6=cch[2];
				break;
				
		}
	}
}

//----------------------------------------------------------------------------
void m24xx_write(
//    int eeprom_type,    //-- EEPROM type
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
//    int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
    U8 * buf,         //-- Data src buf
    int num)            //-- Bytes to write qty
{
//   int page_size = 0;
//   int rc;
//   int b_to_wr;
   //=============================checksum===================
   U8 ii=0;
   char cs=0;
   for(;ii<num-1;ii++)
   {//printf("%x  ",buf[ii]);
   	cs+=buf[ii];
   }  
   buf[num-1]=(0xff-cs);//printf("%x  ",buf[num-1]);	
   //====================================================
   I2C_WriteS_24C(eeprom_addr,buf,num);
} 
//----------------------------------------------------------------------------
BOOL m24xx_read(
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  U8 * buf,         //-- Data dst buf
  int num)            //-- Bytes to read qty
{
	U8 ii=0;
	char cs=0;
	I2C_ReadS_24C(eeprom_addr,buf,num);
	//===================================check sum============

	for(;ii<num-1;ii++)
	{//printf("(%x)  ",buf[ii]);	
		cs+=buf[ii];  
	}
	//printf("(%x)  ",buf[num-1]);	
	if((buf[num-1]+cs)==0xff)
		return __TRUE;
	else
		return __FALSE;
}


BOOL Read_EEPROM_TryEndTime()
{
	U8 cch[TRY_END_SIZE];
	int rc;
	rc=m24xx_read(E2ROM_TRYTIME_END_ADDR,cch,TRY_END_SIZE);
	if(rc&&cch[TRY_END_MON]>0&&cch[TRY_END_MON]<13&&cch[TRY_END_DAY]<32&&cch[TRY_END_DAY]>0)
	{	
		g_TryEndDate[0]=cch[TRY_END_YEAR_HIGH]<<8|cch[TRY_END_YEAR_LOW];
		g_TryEndDate[1]=cch[TRY_END_MON];
		g_TryEndDate[2]=cch[TRY_END_DAY];
	}
	return rc;
}

//-------------------------------------------------------------------------
void Write_EEPROM_TryEndDate(int addr,U16 *ch)
{
	U8 cch[TRY_END_SIZE];
	cch[TRY_END_YEAR_LOW]=ch[0]&0xff;
	cch[TRY_END_YEAR_HIGH]=ch[0]>>8;
	cch[TRY_END_MON]=ch[1]&0xff;
	cch[TRY_END_DAY]=ch[2]&0xff;
	m24xx_write(addr,cch,TRY_END_SIZE);
}
//----------------------------------------------------------------------------
void m24xx_writeU16(
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
    U16 * buf,         //-- Data src buf
    int num)            //-- Bytes to write qty
{
	U8 i=0,j=0;
	U8 Low_buf[100];
	for(;i<num;i++)
	{
		//Low_buf[i++] = (unsigned char)((*buf)&0xff);		
		//Low_buf[i] = (unsigned char)((*buf++)>>8);
		Low_buf[j++] = (unsigned char)((*buf)&0xff);	
		Low_buf[j++] = (unsigned char)((*buf++)>>8);
		
	}
	m24xx_write(eeprom_addr,Low_buf,num*2);
}

//----------------------------------------------------------------------------
BOOL m24xx_readU16(
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  U16 * buf,         //-- Data dst buf
  int num)            //-- Bytes to read qty
{
	U8 i=0,j=0;	
	U8 Low_buf[100];
	char lowbyte;
//	U16 cs;
	if(!m24xx_read(eeprom_addr,Low_buf,num*2))
		return __FALSE;
	for(;i<num;i++)
	{
		lowbyte=Low_buf[j++];
		*buf++ = (U16)((Low_buf[j++])<<8)|lowbyte;	
	}
	return __TRUE;
}

//===========================用户模式===================================================================
//===========================
BOOL FindE2PROMAddr(int *addr1,int *addr2,U8 Mode)
{
	int eepromaddr1,eepromaddr2;
	switch(Mode)
	{
		case MODE1:
			eepromaddr1=E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=EEPROMU8_SIZE+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE2:
			eepromaddr1=(EEPROMU8_SIZE+EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(2*EEPROMU8_SIZE+EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE3:
			eepromaddr1=(2*EEPROMU8_SIZE+2*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(3*EEPROMU8_SIZE+2*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE4:
			eepromaddr1=(3*EEPROMU8_SIZE+3*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(4*EEPROMU8_SIZE+3*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE5:
			eepromaddr1=(4*EEPROMU8_SIZE+4*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(5*EEPROMU8_SIZE+4*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE6:
			eepromaddr1=(5*EEPROMU8_SIZE+5*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(6*EEPROMU8_SIZE+5*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE7:
			eepromaddr1=(6*EEPROMU8_SIZE+6*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(7*EEPROMU8_SIZE+6*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE8:
			eepromaddr1=(7*EEPROMU8_SIZE+7*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(8*EEPROMU8_SIZE+7*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE9:
			eepromaddr1=(8*EEPROMU8_SIZE+8*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(9*EEPROMU8_SIZE+8*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		case MODE0:
			eepromaddr1=(9*EEPROMU8_SIZE+9*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			eepromaddr2=(10*EEPROMU8_SIZE+9*EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR;
			break;
		default:
			return __FALSE;
	}
       if(addr1)
	     *addr1 = eepromaddr1;
      if(addr2)
	      *addr2 = eepromaddr2;
	  return __TRUE;

}


BOOL FindInputScheE2PROMAddr(int *addr,U8 Mode)
{
	int eepromaddr=E2ROM_INPUT_SCHEDULE_ADDR;
	switch(Mode)
	{
		case MODE1:
			eepromaddr+=0;
			break;
		case MODE2:
			eepromaddr+=INPUT_SIZE;
			break;
		case MODE3:
			eepromaddr+=2*INPUT_SIZE;   //815   byte  24C64= 8192 byte
			break;
		default:
			return __FALSE;
	}
       if(addr)
	     *addr = eepromaddr;
	return __TRUE;
}
BOOL Save(U8 Mode)
{
	int eepromaddr1,eepromaddr2;

	if(FindE2PROMAddr(&eepromaddr1,&eepromaddr2,Mode))
	{
		EEPROMMemRegU8[OUTPUT_FORMAT]=g_OutputFormatIndex;	

		EEPROMMemRegU8[MUTI_SINGLE]=g_MutiWindow;
		EEPROMMemRegU8[MAIN_INPUT_SOURCE]=g_MainInput;
		EEPROMMemRegU8[BRIGHTNESS_IDX]=g_Brightness_Value;
		EEPROMMemRegU8[CONTRAST_IDX]=g_Contrast_Value;
		EEPROMMemRegU8[SATURATION_IDX]=g_Saturation_Value;

#if(STM32_CONTROL_MCU==MSTAR_6M16GS)	
		EEPROMMemRegU8[MAIN_BRIGHTNESS_R]=g_BrightnessR_Value[0];
		EEPROMMemRegU8[MAIN_BRIGHTNESS_G]=g_BrightnessG_Value[0];
		EEPROMMemRegU8[MAIN_BRIGHTNESS_B]=g_BrightnessB_Value[0];
		EEPROMMemRegU8[MAIN_CONTRACT_R]=g_ContrastR_Value[0];
		EEPROMMemRegU8[MAIN_CONTRACT_G]=g_ContrastG_Value[0];
		EEPROMMemRegU8[MAIN_CONTRACT_B]=g_ContrastB_Value[0];
#endif

//#if(XP_TO_ID==XP_TO_6M16)
		EEPROMMemRegU16[SCREEN_HSIZE]=g_ScreenHSize;
		EEPROMMemRegU16[SCREEN_VSIZE]=g_ScreenVSize;
		EEPROMMemRegU16[SCREEN_HPOS]=g_ScreenHPos;
		EEPROMMemRegU16[SCREEN_VPOS]=g_ScreenVPos;
		EEPROMMemRegU16[MAIN_SCALER_HSIZE]=g_MainScalerHSize;
		EEPROMMemRegU16[MAIN_SCALER_VSIZE]=g_MainScalerVSize;
		EEPROMMemRegU16[MAIN_SCALER_HPOS]=g_MainScalerHPos;
		EEPROMMemRegU16[MAIN_SCALER_VPOS]=g_MainScalerVPos;
//#endif
		EEPROMMemRegU16[MAIN_VGAZOOM_HRATIO]=g_MainVGAZoomHSize;
		EEPROMMemRegU16[MAIN_VGAZOOM_VRATIO]=g_MainVGAZoomVSize;
		EEPROMMemRegU16[MAIN_DVIZOOM_HRATIO]=g_MainDVIZoomHSize;
		EEPROMMemRegU16[MAIN_DVIZOOM_VRATIO]=g_MainDVIZoomVSize;
		EEPROMMemRegU16[MAIN_HDMIZOOM_HRATIO]=g_MainHDMIZoomHSize;
		EEPROMMemRegU16[MAIN_HDMIZOOM_VRATIO]=g_MainHDMIZoomVSize;
		EEPROMMemRegU16[MAIN_OTHERZOOM_HRATIO]=g_MainZoomHSize;
		EEPROMMemRegU16[MAIN_OTHERZOOM_VRATIO]=g_MainZoomVSize;
		EEPROMMemRegU16[MAIN_VGAZOOM_HPOS]=g_MainVGAZoomHPos;
		EEPROMMemRegU16[MAIN_VGAZOOM_VPOS]=g_MainVGAZoomVPos;
		EEPROMMemRegU16[MAIN_DVIZOOM_HPOS]=g_MainDVIZoomHPos;
		EEPROMMemRegU16[MAIN_DVIZOOM_VPOS]=g_MainDVIZoomVPos;
		EEPROMMemRegU16[MAIN_HDMIZOOM_HPOS]=g_MainHDMIZoomHPos;
		EEPROMMemRegU16[MAIN_HDMIZOOM_VPOS]=g_MainHDMIZoomVPos;
		EEPROMMemRegU16[MAIN_OTHERZOOM_HPOS]=g_MainZoomHPos;
		EEPROMMemRegU16[MAIN_OTHERZOOM_VPOS]=g_MainZoomVPos;

		if(g_MutiWindow!=SINGLEIMAGE)
		{
			EEPROMMemRegU8[SUB_INPUT_SOURCE]=g_SubInput;
			
			EEPROMMemRegU16[SUB_SCALER_HSIZE]=g_SubScalerHSize;
			EEPROMMemRegU16[SUB_SCALER_VSIZE]=g_SubScalerVSize;
			EEPROMMemRegU16[SUB_SCALER_HPOS]=g_SubScalerHPos;
			EEPROMMemRegU16[SUB_SCALER_VPOS]=g_SubScalerVPos;
			
			EEPROMMemRegU16[SUB_VGAZOOM_HRATIO]=g_SubVGAZoomHSize;
			EEPROMMemRegU16[SUB_VGAZOOM_VRATIO]=g_SubVGAZoomVSize;
			EEPROMMemRegU16[SUB_DVIZOOM_HRATIO]=g_SubDVIZoomHSize;
			EEPROMMemRegU16[SUB_DVIZOOM_VRATIO]=g_SubDVIZoomVSize;
			EEPROMMemRegU16[SUB_HDMIZOOM_HRATIO]=g_SubHDMIZoomHSize;
			EEPROMMemRegU16[SUB_HDMIZOOM_VRATIO]=g_SubHDMIZoomVSize;
			EEPROMMemRegU16[SUB_OTHERZOOM_HRATIO]=g_SubZoomHSize;
			EEPROMMemRegU16[SUB_OTHERZOOM_VRATIO]=g_SubZoomVSize;
			
			EEPROMMemRegU16[SUB_VGAZOOM_HPOS]=g_SubVGAZoomHPos;
			EEPROMMemRegU16[SUB_VGAZOOM_VPOS]=g_SubVGAZoomVPos;
			EEPROMMemRegU16[SUB_DVIZOOM_HPOS]=g_SubDVIZoomHPos;
			EEPROMMemRegU16[SUB_DVIZOOM_VPOS]=g_SubDVIZoomVPos;
			EEPROMMemRegU16[SUB_HDMIZOOM_HPOS]=g_SubHDMIZoomHPos;
			EEPROMMemRegU16[SUB_HDMIZOOM_VPOS]=g_SubHDMIZoomVPos;
			EEPROMMemRegU16[SUB_OTHERZOOM_HPOS]=g_SubZoomHPos;
			EEPROMMemRegU16[SUB_OTHERZOOM_VPOS]=g_SubZoomVPos;
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)	
			EEPROMMemRegU8[SUB_BRIGHTNESS_R]=g_BrightnessR_Value[1];
			EEPROMMemRegU8[SUB_BRIGHTNESS_G]=g_BrightnessG_Value[1];
			EEPROMMemRegU8[SUB_BRIGHTNESS_B]=g_BrightnessB_Value[1];
			EEPROMMemRegU8[SUB_CONTRACT_R]=g_ContrastR_Value[1];
			EEPROMMemRegU8[SUB_CONTRACT_G]=g_ContrastG_Value[1];
			EEPROMMemRegU8[SUB_CONTRACT_B]=g_ContrastB_Value[1];
#endif
		}
		m24xx_write(eepromaddr1,EEPROMMemRegU8,EEPROMU8_SIZE);
		m24xx_writeU16(eepromaddr2,EEPROMMemRegU16,EEPROMU16_SIZE);
		return __TRUE;
	}
	else
		return __FALSE;
}

void LoadOutputResolution()
{
	switch(g_OutputFormatIndex)
	{
	case INDEX640X480:
		g_PanelWidth=640;
		g_PanelHeight=480;
		break;
	case INDEX800X600:
		g_PanelWidth=800;
		g_PanelHeight=600;
		break;
	case INDEX1024X768:
		g_PanelWidth=1024;
		g_PanelHeight=768;
		break;
	case INDEX1280X1024:
		g_PanelWidth=1280;
		g_PanelHeight=1024;
		break;
	case INDEX1366X768:
		g_PanelWidth=1366;
		g_PanelHeight=768;
		break;
	case INDEX1440X900:
		g_PanelWidth=1440;
		g_PanelHeight=900;
		break;
	case INDEX1600X900:
		g_PanelWidth=1600;
		g_PanelHeight=900;
		break;
	case INDEX1600X1200:
		g_PanelWidth=1600;
		g_PanelHeight=1200;
		break;
	case INDEX1680X1050:
		g_PanelWidth=1680;
		g_PanelHeight=1050;
		break;
	case INDEX1920X1080:
		g_PanelWidth=1920;
		g_PanelHeight=1080;
		break;
	case INDEX1280X720:
		g_PanelWidth=1280;
		g_PanelHeight=720;
		break;
	case INDEX1280X768:
		g_PanelWidth=1280;
		g_PanelHeight=768;
		break;
	case INDEX1280X800:
		g_PanelWidth=1280;
		g_PanelHeight=800;
		break;
	case INDEX1280X960:
		g_PanelWidth=1280;
		g_PanelHeight=960;
		break;
	case INDEX1400X1050:
		g_PanelWidth=1400;
		g_PanelHeight=1050;
		break;
#if(XP_TO_ID==XP_TO_FLI32626)
	case 0:
		g_PanelWidth=UserResolutionHSize;
		g_PanelHeight=UserResolutionVSize;
		break;
	case INDEX1920X1200:
		g_PanelWidth=1920;
		g_PanelHeight=1200;
		break;
	case INDEX2048x1152:		
		g_PanelWidth=2048;
		g_PanelHeight=1152;
		break;
#endif
	}	
}

BOOL Updata_Execute(U8 *U8EEPROMMem,U16 *U16EEPROMMem)
{
	g_OutputFormatIndex=U8EEPROMMem[OUTPUT_FORMAT];
	LoadOutputResolution();

#if(XP_TO_ID==XP_TO_FLI32626)
	g_SwitchEffectValue=(SrcSwitchType)U8EEPROMMem[SWITCH_MODE];
	g_NextPointImage=U8EEPROMMem[POINT_NEXT_IMAGE];
#endif
	g_MutiWindow=U8EEPROMMem[MUTI_SINGLE];
	g_MainInput=(_InputSourceType)U8EEPROMMem[MAIN_INPUT_SOURCE];
	
	g_Brightness_Value=U8EEPROMMem[BRIGHTNESS_IDX];
	g_Contrast_Value=U8EEPROMMem[CONTRAST_IDX];
	g_Saturation_Value=U8EEPROMMem[SATURATION_IDX];
	
	g_MainVGAZoomHSize=U16EEPROMMem[MAIN_VGAZOOM_HRATIO];
	g_MainVGAZoomVSize=U16EEPROMMem[MAIN_VGAZOOM_VRATIO];
	g_MainDVIZoomHSize=U16EEPROMMem[MAIN_DVIZOOM_HRATIO];
	g_MainDVIZoomVSize=U16EEPROMMem[MAIN_DVIZOOM_VRATIO];
	g_MainHDMIZoomHSize=U16EEPROMMem[MAIN_HDMIZOOM_HRATIO];
	g_MainHDMIZoomVSize=U16EEPROMMem[MAIN_HDMIZOOM_VRATIO];
	g_MainZoomHSize=U16EEPROMMem[MAIN_OTHERZOOM_HRATIO];
	g_MainZoomVSize=U16EEPROMMem[MAIN_OTHERZOOM_VRATIO];
	
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
	g_BrightnessR_Value[0]=U8EEPROMMem[MAIN_BRIGHTNESS_R];
	g_BrightnessG_Value[0]=U8EEPROMMem[MAIN_BRIGHTNESS_G];
	g_BrightnessB_Value[0]=U8EEPROMMem[MAIN_BRIGHTNESS_B];
	g_ContrastR_Value[0]=U8EEPROMMem[MAIN_CONTRACT_R];
	g_ContrastG_Value[0]=U8EEPROMMem[MAIN_CONTRACT_G];
	g_ContrastB_Value[0]=U8EEPROMMem[MAIN_CONTRACT_B];
	
#endif

//#if(XP_TO_ID==XP_TO_6M16)	
	g_ScreenHSize=U16EEPROMMem[SCREEN_HSIZE];
	g_ScreenVSize=U16EEPROMMem[SCREEN_VSIZE];
	g_ScreenHPos=U16EEPROMMem[SCREEN_HPOS];
	g_ScreenVPos=U16EEPROMMem[SCREEN_VPOS];
	
	g_MainScalerHSize=U16EEPROMMem[MAIN_SCALER_HSIZE];
	g_MainScalerVSize=U16EEPROMMem[MAIN_SCALER_VSIZE];
	g_MainScalerHPos=U16EEPROMMem[MAIN_SCALER_HPOS];
	g_MainScalerVPos=U16EEPROMMem[MAIN_SCALER_VPOS];
//#endif	
	
	g_MainVGAZoomHPos=U16EEPROMMem[MAIN_VGAZOOM_HPOS];
	g_MainVGAZoomVPos=U16EEPROMMem[MAIN_VGAZOOM_VPOS];
	g_MainDVIZoomHPos=U16EEPROMMem[MAIN_DVIZOOM_HPOS];
	g_MainDVIZoomVPos=U16EEPROMMem[MAIN_DVIZOOM_VPOS];
	g_MainHDMIZoomHPos=U16EEPROMMem[MAIN_HDMIZOOM_HPOS];
	g_MainHDMIZoomVPos=U16EEPROMMem[MAIN_HDMIZOOM_VPOS];
	g_MainZoomHPos=U16EEPROMMem[MAIN_OTHERZOOM_HPOS];
	g_MainZoomVPos=U16EEPROMMem[MAIN_OTHERZOOM_VPOS];
	
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		g_SubInput=(_InputSourceType)U8EEPROMMem[SUB_INPUT_SOURCE];
		g_SubScalerHSize=U16EEPROMMem[SUB_SCALER_HSIZE];
		g_SubScalerVSize=U16EEPROMMem[SUB_SCALER_VSIZE];
		g_SubScalerHPos=U16EEPROMMem[SUB_SCALER_HPOS];
		g_SubScalerVPos=U16EEPROMMem[SUB_SCALER_VPOS];
		g_SubVGAZoomHSize=U16EEPROMMem[SUB_VGAZOOM_HRATIO];
		g_SubVGAZoomVSize=U16EEPROMMem[SUB_VGAZOOM_VRATIO];
		g_SubDVIZoomHSize=U16EEPROMMem[SUB_DVIZOOM_HRATIO];
		g_SubDVIZoomVSize=U16EEPROMMem[SUB_DVIZOOM_VRATIO];
		g_SubHDMIZoomHSize=U16EEPROMMem[SUB_HDMIZOOM_HRATIO];
		g_SubHDMIZoomVSize=U16EEPROMMem[SUB_HDMIZOOM_VRATIO];
		g_SubZoomHSize=U16EEPROMMem[SUB_OTHERZOOM_HRATIO];
		g_SubZoomVSize=U16EEPROMMem[SUB_OTHERZOOM_VRATIO];
		g_SubVGAZoomHPos=U16EEPROMMem[SUB_VGAZOOM_HPOS];
		g_SubVGAZoomVPos=U16EEPROMMem[SUB_VGAZOOM_VPOS];
		g_SubDVIZoomHPos=U16EEPROMMem[SUB_DVIZOOM_HPOS];
		g_SubDVIZoomVPos=U16EEPROMMem[SUB_DVIZOOM_VPOS];
		g_SubHDMIZoomHPos=U16EEPROMMem[SUB_HDMIZOOM_HPOS];
		g_SubHDMIZoomVPos=U16EEPROMMem[SUB_HDMIZOOM_VPOS];
		g_SubZoomHPos=U16EEPROMMem[SUB_OTHERZOOM_HPOS];
		g_SubZoomVPos=U16EEPROMMem[SUB_OTHERZOOM_VPOS];
	#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
		g_BrightnessR_Value[1]=U8EEPROMMem[SUB_BRIGHTNESS_R];
		g_BrightnessG_Value[1]=U8EEPROMMem[SUB_BRIGHTNESS_G];
		g_BrightnessB_Value[1]=U8EEPROMMem[SUB_BRIGHTNESS_B];
		g_ContrastR_Value[1]=U8EEPROMMem[SUB_CONTRACT_R];
		g_ContrastG_Value[1]=U8EEPROMMem[SUB_CONTRACT_G];
		g_ContrastB_Value[1]=U8EEPROMMem[SUB_CONTRACT_B];
	#endif
	}
	if(g_CurrentState==State_UserPara_Number)
	{
		 DisplayLCDInputSrc(0x94,"+");
	}
	LoadModeSet();
	return __TRUE;
}

BOOL reset()
{	
	m24xx_write(E2ROM_SAVEMODE_BEGIN_ADDR,genericEEPROMMemU8,EEPROMU8_SIZE);
		
	m24xx_writeU16(EEPROMU8_SIZE+E2ROM_SAVEMODE_BEGIN_ADDR,genericEEPROMMemU16,EEPROMU16_SIZE);
		
	return __TRUE;
}
//if the mode has save return true,else return false
BOOL IsHasSave(U8 Mode)
{
	U8 i;
	int eepromaddr1,eepromaddr2;
//	U8 ch[3];
	if(!FindE2PROMAddr(&eepromaddr1,&eepromaddr2,Mode))
		return __FALSE;
	if(m24xx_read(eepromaddr1,EEPROMMemRegU8,EEPROMU8_SIZE)==__FALSE)
		return __FALSE;
	if(m24xx_readU16(eepromaddr2,EEPROMMemRegU16,EEPROMU16_SIZE)==__FALSE)
		return __FALSE;
	for(i=0;i<EEPROMU8_SIZE-1;i++)
	{
		if(EEPROMMemRegU8[i]!=genericEEPROMMemU8[i])
			return __TRUE;	
	}
	for(i=0;i<EEPROMU16_SIZE-1;i++)
	{
		if(EEPROMMemRegU16[i]!=genericEEPROMMemU16[i])
			return __TRUE;
	}
	return __FALSE;
}
//===========================
//===========================用户模式===================================================================


//-------------------------------------时间计划=------------------------
//-------------------------------------------------
void SaveInputScheduleToEEPROM(U8 MODE)
{
	int addr;
	if(FindInputScheE2PROMAddr(&addr, MODE))
		m24xx_write(addr,InputEE,INPUT_SIZE);
}

void SaveInput1Schedule()
{
	InputEE[INPUT_HOUR]=g_InputTime1[0];
	InputEE[INPUT_MIN]=g_InputTime1[1];
	InputEE[INPUT_SRC]=g_ScheduleInput1;
	InputEE[INPUT_WEEK]=RepeatWeekToByte(g_ScheduleInputMode1,g_InputWeek1);
	SaveInputScheduleToEEPROM(MODE1);
}

void SaveInput2Schedule()
{
	InputEE[INPUT_HOUR]=g_InputTime2[0];
	InputEE[INPUT_MIN]=g_InputTime2[1];
	InputEE[INPUT_SRC]=g_ScheduleInput2;
	InputEE[INPUT_WEEK]=RepeatWeekToByte(g_ScheduleInputMode2,g_InputWeek2);
	SaveInputScheduleToEEPROM(MODE2);
}

void SaveInput3Schedule()
{
	InputEE[INPUT_HOUR]=g_InputTime3[0];
	InputEE[INPUT_MIN]=g_InputTime3[1];
	InputEE[INPUT_SRC]=g_ScheduleInput3;
	InputEE[INPUT_WEEK]=RepeatWeekToByte(g_ScheduleInputMode3,g_InputWeek3);
	SaveInputScheduleToEEPROM(MODE3);
}

BOOL IsInputScheHasSave(U8 Mode)
{
	int addr;
	if(!FindInputScheE2PROMAddr(&addr, Mode))
		return __FALSE;
	if(m24xx_read(addr,InputEE,INPUT_SIZE)==__FALSE)
		return __FALSE;
	return __TRUE;
}
//该函数主要是从一个字节获得周几
//return mode 
U8 ByteToRepeatWeek(U8 OneB,U8 Mode)
{
	U8 idx=0;
	if((OneB&_BIT0)==0)
		return 0;
	else
	{
		switch(Mode)
		{
			case MODE1:
				if((OneB&_BIT1)==_BIT1)
					g_InputWeek1[idx++]=1;
				if((OneB&_BIT2)==_BIT2)
					g_InputWeek1[idx++]=2;
				if(OneB&_BIT3==_BIT3)
					g_InputWeek1[idx++]=3;
				if((OneB&_BIT4)==_BIT4)
					g_InputWeek1[idx++]=4;
				if((OneB&_BIT5)==_BIT5)
					g_InputWeek1[idx++]=5;
				if((OneB&_BIT6)==_BIT6)
					g_InputWeek1[idx++]=6;
				if((OneB&_BIT7)==_BIT7)
					g_InputWeek1[idx++]=7;
				g_InputWeek1Idx=idx;
				break;
			case MODE2:
				if((OneB&_BIT1)==_BIT1)
					g_InputWeek2[idx++]=1;
				if((OneB&_BIT2)==_BIT2)
					g_InputWeek2[idx++]=2;
				if(OneB&_BIT3==_BIT3)
					g_InputWeek2[idx++]=3;
				if((OneB&_BIT4)==_BIT4)
					g_InputWeek2[idx++]=4;
				if((OneB&_BIT5)==_BIT5)
					g_InputWeek2[idx++]=5;
				if((OneB&_BIT6)==_BIT6)
					g_InputWeek2[idx++]=6;
				if((OneB&_BIT7)==_BIT7)
					g_InputWeek2[idx++]=7;
				g_InputWeek2Idx=idx;
				break;
			case MODE3:
				if((OneB&_BIT1)==_BIT1)
					g_InputWeek3[idx++]=1;
				if((OneB&_BIT2)==_BIT2)
					g_InputWeek3[idx++]=2;
				if(OneB&_BIT3==_BIT3)
					g_InputWeek3[idx++]=3;
				if((OneB&_BIT4)==_BIT4)
					g_InputWeek3[idx++]=4;
				if((OneB&_BIT5)==_BIT5)
					g_InputWeek3[idx++]=5;
				if((OneB&_BIT6)==_BIT6)
					g_InputWeek3[idx++]=6;
				if((OneB&_BIT7)==_BIT7)
					g_InputWeek3[idx++]=7;
				g_InputWeek3Idx=idx;
				break;
		}
		return 1;
	}
}
//将时间计划的重复模式或执行一次，用一个字节来表示
//具体表示如下
//bit0=0     执行一次
//bit0=1    表示是重复执行
//bit1-bit7   如果为1表示周几存在
//例如bit4=1                 表示周四存在
//return one byte
U8 RepeatWeekToByte(U8 mode,U8 *week)
{
	U8 ch=0;
	U8 idx=0;
	if(mode==MORESCHE)
		ch|=_BIT0;
	else
		ch&=~_BIT0;
	for(idx=0;idx<7;idx++)
	{
		if(*week==1)
			ch|=_BIT1;
		else if(*week==2)
			ch|=_BIT2;
		else if(*week==3)
			ch|=_BIT3;
		else if(*week==4)
			ch|=_BIT4;
		else if(*week==5)
			ch|=_BIT5;
		else if(*week==6)
			ch|=_BIT6;
		else if(*week==7)
			ch|=_BIT7;
		else if(*week==0)
			break;
		week++;			
	}
	return ch;
}
//从EEPROM 读取通道时间计划
void FromEEInitInputSche(U8 Mode)	
{
	U8 chWeek;
	chWeek=InputEE[INPUT_WEEK];
	switch(Mode)
	{
		case MODE1:
			g_InputTime1[0]=InputEE[INPUT_HOUR];
			g_InputTime1[1]=InputEE[INPUT_MIN];
			g_ScheduleInput1=(_InputSourceType)InputEE[INPUT_SRC];
			
			//DisFourNumValue( chWeek,0x80);
			g_ScheduleInputMode1=ByteToRepeatWeek(chWeek,MODE1);
			//DisFourNumValue( g_ScheduleInputMode1,0x82);
			break;
		case MODE2:
			g_InputTime2[0]=InputEE[INPUT_HOUR];
			g_InputTime2[1]=InputEE[INPUT_MIN];
			g_ScheduleInput2=(_InputSourceType)InputEE[INPUT_SRC];
			g_ScheduleInputMode2=ByteToRepeatWeek(chWeek,MODE2);
			break;
		case MODE3:
			g_InputTime3[0]=InputEE[INPUT_HOUR];
			g_InputTime3[1]=InputEE[INPUT_MIN];
			g_ScheduleInput3=(_InputSourceType)InputEE[INPUT_SRC];
			g_ScheduleInputMode3=ByteToRepeatWeek(chWeek,MODE3);
			break;
	}
}
//-------------------------------------------------
//-------------------------------------时间计划=------------------------

