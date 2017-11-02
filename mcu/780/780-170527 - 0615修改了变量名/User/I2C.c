/*
��������Ϊһ24C02
*/
#define _I2C_C_

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "header.h"
#include <stdio.h>

#define AT24C01A		//24C01A,I2Cʱ��������24C02һ��
//#define AT24C01		//24C01,I2C��ʱ�����ͨ���е㲻ͬ

#define EEPROM_ADDR		0xA0
#define I2C_PAGESIZE	32//8//4		//24C01/01Aҳ������4��

#ifndef NULL
#define NULL 0
#endif


#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT)) 
 
__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT;   

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
extern U8 g_initPIPSrc;

U8 genericEEPROMMemU8[EEPROMU8_SIZE] = {

//	DEF_DVI_CURRENT_SCENE,DEF_VGA_CURRENT_SCENE,
//	DEF_BRIGHTNESS,DEF_CONTRACT,DEF_SATURATION,
//	DEF_DVI_RESOLUTION,
//	DEF_DVI_FRAMERATE,
//	DEF_VGA_RESOLUTION,
//	DEF_VGA_FRAMERATE,
	9
	};


U8 EEPROMMemRegU8[EEPROMU8_SIZE]= {

//	DEF_DVI_CURRENT_SCENE,DEF_VGA_CURRENT_SCENE,
//	DEF_BRIGHTNESS,DEF_CONTRACT,DEF_SATURATION,
//	DEF_DVI_RESOLUTION,
//	DEF_DVI_FRAMERATE,
//	DEF_VGA_RESOLUTION,
//	DEF_VGA_FRAMERATE,
	9
	};

U16 genericEEPROMMemU16[EEPROMU16_SIZE] = {
//	DEF_SCREEN_TOTAL_HSIZE,DEF_SCREEN_TOTAL_VSIZE,
	2
	};

U16 EEPROMMemRegU16[EEPROMU16_SIZE]= {
//	DEF_SCREEN_TOTAL_HSIZE,DEF_SCREEN_TOTAL_VSIZE,
	2
	};
U8 InputEE[INPUT_SIZE]={
	0,0,0,0,0xff
};

#if(ENABLE_SOFTI2C)	
static void MyDelay_us(u8 n)
{
	u8 i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<8;j++)
			;
	}
}
#endif


#if(ENABLE_HARDI2C)	

void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //��ӳ��ʱ��	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   
	#ifdef STM32F103RB
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, DISABLE); 	   //��ֹ��ӳ��
	#else
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); 	   //��I2C1��ӳ�䵽  PB8 PB9
	#endif  

	// PB8,9 SCL and SDA //
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
	I2C_InitStructure.I2C_ClockSpeed = 100000;//100K�ٶ�

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	//����1�ֽ�1Ӧ��ģʽ//
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	//    printf("I2C_Configuration----\n\r");
}

////////////////IO Emulate End/////////////////////////////////////

//-***************************************************
//-**������:I2C_ReadS: 
//-**����:��ȡ24c04����ֽ�
//-������:24C02��256�ֽ�,8λ��ַ,A0-A2�̶�Ϊ0,��������ַΪEEPROM_ADDR
//-***************************************************-/
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no)
{
	if(no==0)
		return;
	//sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x80,"01");
	}
		
	//-*����1�ֽ�1Ӧ��ģʽ 
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	//- ������ʼλ 
	I2C_GenerateSTART(I2C1, ENABLE);
	
  	//sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x81,"02");
	}
#ifdef AT24C01A	
	//-����������ַ(д)
	I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);
  	//sEETimeout = sEE_FLAG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x82,"03");
	}

	//-���͵�ַ 
	//I2C_SendData(I2C1, addr);	 	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
	//sEETimeout = sEE_FLAG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//-�����ѷ��� 
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x83,"04");
	}
		
	//-��ʼλ 
	I2C_GenerateSTART(I2C1, ENABLE);
	//sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x84,"05");
	}

	//-������ 
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	//sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x85,"06");
	}

#else	
	//-����������ַ(��)24C01 
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Receiver);
	//sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		//if((sEETimeout--) == 0) 
			//return;
			//chinesedisplay(0x86,"07");
	}
#endif

	while (no)
	{
		if(no==1)
		{
	 		I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
			I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		}
	    	//sEETimeout = sEE_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) //- EV7  
		{
			//if((sEETimeout--) == 0) 
				//return;
				//chinesedisplay(0x87,"08");
		}
		//chinesedisplay(0x88,"OK");
		*pBuffer = I2C_ReceiveData(I2C1);
		pBuffer++;
		//- Decrement the read bytes counter  
		no--;
	}
	//�ٴ�����Ӧ��ģʽ
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

//-***************************************************
//-**������:I2C_Standby_24C
//-**����:24C�Ƿ�׼������д����ж�
//-**ע������:�������������Ϊ:��æ
//-***************************************************-//
void I2C_Standby_24C(void)      
{
	vu16 SR1_Tmp;
	do
	{
		//��ʼλ
		I2C_GenerateSTART(I2C1, ENABLE);
		//��SR1
		SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
		//������ַ(д)
#ifdef AT24C01A
		I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
#else
		I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Transmitter);
#endif
	}while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));


	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	//ֹͣλ   
	I2C_GenerateSTOP(I2C1, ENABLE);
}

//--*************************************************
//--**������:I2C_ByteWrite_24C
//--**����:дһ���ֽ�
//--**ע������:��д��ͬ����Ҫ����æ�ж�
//--*************************************************
void I2C_ByteWrite_24C(u16 addr,u8 dat)
{
  // ��ʼλ
  	I2C_GenerateSTART(I2C1, ENABLE);
  	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((sEETimeout--) == 0) 
			return;
	} 

#ifdef AT24C01A
  	//-- ����������ַ(д)
  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}
  //--���͵�ַ
  	//I2C_SendData(I2C1, addr);	 	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}

#else	
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
  	sEETimeout = sEE_FLAG_TIMEOUT;
 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}
#endif

	//-- дһ���ֽ�
  	I2C_SendData(I2C1, dat); 
  	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}
	
  	// ֹͣλ
  	I2C_GenerateSTOP(I2C1, ENABLE);
  
  	I2C_Standby_24C();
}

////--*************************************************
////--**������:I2C_PageWrite_24C
////--**����:д��һҳ(����)
////--**ע������:�˺�����Ⱥд�����
////--*************************************************
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	//��ʼλ
	I2C_GenerateSTART(I2C1, ENABLE);
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((sEETimeout--) == 0) 
			return;
	}

#ifdef AT24C01A
	//������ַ(д)
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}
	
	//д��ֵַ
	//I2C_SendData(I2C1, addr);	  	  
	I2C_SendData(I2C1, ((addr>>8) & 0x1F));  
	I2C_SendData(I2C1, (addr& 0xFF));
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}

#else
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}
#endif
	while(no--)  
	{
		I2C_SendData(I2C1, *pBuffer); 
		pBuffer++; 
		sEETimeout = sEE_FLAG_TIMEOUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((sEETimeout--) == 0) 
				return;
		}
	}
	
	//ֹͣλ
	I2C_GenerateSTOP(I2C1, ENABLE);
}

//--************************************************
////--������:I2C_WriteS_24C
////--����:ҳд��24C
////--ע������:24C02�������һ��д��8���ֽ�
////--***********************************************
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no)
{
	u8 temp;

	//1.�Ȱ�ҳ������Ĳ���д��
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
	//2.��ҳ���뿪ʼд
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
	//chinesedisplay(0x90,"W-E2");
}

unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no)
{
	if(no==0)
		return;
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		if((sEETimeout--) == 0) 
			return;
	}
		
	//-*����1�ֽ�1Ӧ��ģʽ 
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	//- ������ʼλ 
	I2C_GenerateSTART(I2C1, ENABLE);
	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))//-EV5,��ģʽ
	{
		if((sEETimeout--) == 0) 
			return;
	} 

	//-����������ַ(д)
	I2C_Send7bitAddress(I2C1,  eepaddr, I2C_Direction_Transmitter);
	sEETimeout = sEE_FLAG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}

	//-���͵�ַ 
	I2C_SendData(I2C1, addr);
	sEETimeout = sEE_FLAG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//-�����ѷ���
	{
		if((sEETimeout--) == 0) 
			return;
	} 
		
	//-��ʼλ 
	I2C_GenerateSTART(I2C1, ENABLE);
	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((sEETimeout--) == 0) 
			return;
	}

	//-������ 
	I2C_Send7bitAddress(I2C1, eepaddr, I2C_Direction_Receiver);
	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}

	while (no)
	{
		if(no==1)
		{
	 		I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
			I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		}	    
		sEETimeout = sEE_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) //- EV7 
		{
			if((sEETimeout--) == 0) 
				return;
		} 
		*pBuffer = I2C_ReceiveData(I2C1);
		pBuffer++;
		//- Decrement the read bytes counter  
		no--;
	}
	//�ٴ�����Ӧ��ģʽ
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

unsigned char  I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat)
{
  // ��ʼλ
  	I2C_GenerateSTART(I2C1, ENABLE);
  	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((sEETimeout--) == 0) 
			return;
	}   
	//�����豸��ַ
  	I2C_Send7bitAddress(I2C1, eepaddr, I2C_Direction_Transmitter);
	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}   
  //--���͵�ַ
  	I2C_SendData(I2C1, addr);
  	sEETimeout = sEE_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}   
	//-- дһ���ֽ�
  	I2C_SendData(I2C1, dat); 
	sEETimeout = sEE_FLAG_TIMEOUT;
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((sEETimeout--) == 0) 
			return;
	}   	
  	// ֹͣλ
  	I2C_GenerateSTOP(I2C1, ENABLE);  
  	I2C_Standby_24C();
}

#else

////////////////-----------IO Emulate Start------------/////////////////////////////////////////

///--*************************************************************
//--*�������ƣ�I2CStart
//--*�������ܣ�I2C��ʼ�ź�
//--*���������
//--*���������
//--*��           ע��ʱ���߸�ʱ���������ɸߵ��͵����䣬��ʾI2C��ʼ�ź�
//--**************************************************************
void I2CStart(void) 
{	
	EEPROM_SDA_1;
	MyDelay_us(1);
	EEPROM_SCL_1;
	MyDelay_us(1);
	EEPROM_SDA_0;
	MyDelay_us(1);
	EEPROM_SCL_0;
 }

///--*************************************************************
//--*�������ƣ�I2CStop
//--*�������ܣ�I2Cֹͣ�ź�
//--*���������
//--*���������
//--*��           ע��ʱ���߸�ʱ���������ɵ͵��ߵ����䣬��ʾI2Cֹͣ�ź�
//--**************************************************************
void I2CStop(void) 
{
	EEPROM_SDA_0;
	MyDelay_us(1);
	EEPROM_SCL_1;
	MyDelay_us(1);
	EEPROM_SDA_1;
	MyDelay_us(1);
	EEPROM_SCL_0;
 }

///--*************************************************************
//--*�������ƣ�I2CSlaveAck
//--*�������ܣ�I2C�ӻ��豸Ӧ���ѯ
//--*���������
//--*���������
//--*��           ע��
//--**************************************************************
unsigned char I2CSlaveAck( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	unsigned int TimeOut;
	unsigned char RetValue = RESET;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //����һ��Ҫ������������������ܶ�������
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
	MyDelay_us(1);
 	EEPROM_SCL_1;
	MyDelay_us(1);
  	EEPROM_SCL_0;
	MyDelay_us(1);
}



///--*************************************************************
//--*�������ƣ�I2CWriteByte
//--*�������ܣ�I2Cдһ�ֽ�����
//--*���������
//--*���������
//--*��           ע��
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
		MyDelay_us(1);
		EEPROM_SCL_1;
		MyDelay_us(1);
		EEPROM_SCL_0;
		MyDelay_us(1);
	}
//	Waitack();
}
//--*************************************************************
//--*�������ƣ�I2CReadByte
//--*�������ܣ�I2C��һ�ֽ�����
//--*���������
//--*���������
//--*��           ע��
//--**************************************************************
U8 I2CReadByte(void)
{
	U8 i,bytedata=0;
  	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //����һ��Ҫ������������������ܶ�������
	GPIO_InitStruct.GPIO_Pin = EEPROM_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStruct );
	
//	GPIOB->CRH &= 0xfffffff0;
//	GPIOB->CRH |= 0x00000004;  //PB8  ��������
	for(i=0; i<8; i++)
	{
		bytedata<<=1;
		EEPROM_SCL_1;
		MyDelay_us(1);
		bytedata|=EEPROM_SDA;
		EEPROM_SCL_0;
		MyDelay_us(1);
	}
	
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;      //PB8  ������� 50MHz	
  	GPIO_Init( GPIOB, &GPIO_InitStruct );
//	GPIOB->CRH &= 0xfffffff0;
//	GPIOB->CRH |= 0x00000001;  //PB8  ������� 10MHz	
	return bytedata;
}


////////////////IO Emulate End/////////////////////////////////////

///--***************************************************
//--**������:I2C_ReadS: 
//--**����:��ȡ
//--**ע������:24C64��8K�ֽ�,16λ��ַ,A0-A2�̶�Ϊ0,��������ַΪEEPROM_ADDR
//--***************************************************
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no)
{
       if(no==0)
		return;
	///--*��ʼλ
	I2CStart();
	//����������ַ(д)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	
	//���͵�ַ	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//��ʼλ
	I2CStart();
	
	//����������ַ(��)24C64
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
    	I2CStop();			//����ֹͣλ
    	//chinesedisplay(0x90,"24c64R");
}

///--*************************************************
//--**������:I2C_ByteWrite_24C
//--**����:дһ���ֽ�
//--**ע������:��д��ͬ����Ҫ����æ�ж�
//--*************************************************
void I2C_ByteWrite_24C(u16 addr,u8 dat)
{
	//��ʼλ
	I2CStart();
	//����������ַ(д)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//���͵�ַ	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;

       MyDelay_us(1);
	//дһ���ֽ�
  	I2CWriteByte(dat);
  	if( RESET == I2CSlaveAck() )  	
   		return;
       MyDelay_us(1);
	I2CStop();
}

//--*************************************************
//--**������:I2C_PageWrite_24C
//--**����:д��һҳ(����)
//--**ע������:�˺�����Ⱥд�����
//--*************************************************
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no)
{
	//��ʼλ
	I2CStart();
	//����������ַ(д)24C64
  	I2CWriteByte( EEPROM_ADDR );
  	if( RESET == I2CSlaveAck() )  	
   		return;
	//���͵�ַ	 	  
	I2CWriteByte ((addr>>8) & 0x1F);  
  	if( RESET == I2CSlaveAck() )  	
   		return;
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return;

       MyDelay_us(1);
	while(no--)  
	{
	  	I2CWriteByte(*pBuffer);	//printf("[%x]  ",*pBuffer);
	  	
	 	pBuffer++; 
  		if( RESET == I2CSlaveAck() )  	
   			return;
	}
	
	//ֹͣλ
	I2CStop();
}
///--*************************************************
//--**������:I2C_WriteS_24C
//--**����:ҳд��24C
//--**ע������:24C64�������һ��д��32���ֽ�
//--*************************************************
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no)
{
	u8 temp;

	//1.�Ȱ�ҳ������Ĳ���д��
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
	//2.��ҳ���뿪ʼд
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
	//chinesedisplay(0x80,"24c64W");
}


unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no)
{
       if(no==0)
		return 1;
	///--*��ʼλ
	I2CStart();
	//����������ַ(д)24C64
  	I2CWriteByte( eepaddr);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	//��ʼλ
	I2CStart();
	
	//����������ַ(��)24C64
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
    	I2CStop();			//����ֹͣλ
    	return 0;
}

unsigned char I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat)
{
	//��ʼλ
	I2CStart();
	//����������ַ(д)24C64
  	I2CWriteByte( eepaddr);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
	
	I2CWriteByte (addr& 0xFF);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;

       MyDelay_us(1);
	//дһ���ֽ�
  	I2CWriteByte(dat);
  	if( RESET == I2CSlaveAck() )  	
   		return 1;
       MyDelay_us(1);
	I2CStop();
	return 0;
}

#endif


void Write_EEPROM_InputCycle(int addr,_InputSourceType Input,U8 Hour,U8 Min)
{	
	U8 cch[4];
	cch[0]=Input;
	cch[1]=Hour;
	cch[2]=Min;
	m24xx_write(addr,cch,4);
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


void Read_Screen_line_column_pixel(void)
{
	u8 i=0;
	for(i=0;i<8;i++)
	{
		Read_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6,&(gSplitScrRCPix[0][i]));
		Read_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6+3,&(gSplitScrRCPix[1][i]));
		g_screen_line_column_pixel[0][i]=gSplitScrRCPix[0][i];
		g_screen_line_column_pixel[1][i]=gSplitScrRCPix[1][i];
	}
	
}

void Write_Screen_line_column_pixel(void)
{
	u8 i=0;
	for(i=0;i<8;i++)
	{
		Write_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6,gSplitScrRCPix[0][i]);
		Write_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6+3,gSplitScrRCPix[1][i]);
	}
}

//read out user scheme
void Read_Scheme_IsSave(void)
{
	u8 i=0;
	for(i=0;i<9;i++)
	{
		Read_EEPROM_Byte(E2ROM_SCHEME_SAVE_ADDR+i*2,&scheme_save[i]);
	}
	
}

void Write_Scheme_IsSave(void)
{
	u8 i=0;
	for(i=0;i<9;i++)
	{
		Write_EEPROM_Byte(E2ROM_SCHEME_SAVE_ADDR+i*2,0);
		scheme_save[i]=0;
	}
}



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

//-------------------------------------------------------------------------
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
//========================================================
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
//========================================================


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


BOOL Read_EEPROM_Language()
{
	U8 cch[2];
	int rc;

	//read language
	rc=m24xx_read(E2ROM_LANGUAGE_BEGIN_ADDR,cch,2);
	if(rc)
		g_LanguageFlag=cch[0];
	//read lock time
	rc=m24xx_read(E2ROM_LOCKTIME_BEGIN_ADDR,cch,2);
	if(rc&&cch[0]<60)
		g_LockKeypadTime=cch[0];
	//read output format
	
	return rc;
}

BOOL Read_EEPROM_TryEndTime()
{
	U8 cch[TRY_END_SIZE+1];
	int rc;
	rc=m24xx_read(E2ROM_TRYTIME_END_ADDR,cch,TRY_END_SIZE+1);
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
	U8 cch[TRY_END_SIZE+1];
	cch[TRY_END_YEAR_LOW]=ch[0]&0xff;
	cch[TRY_END_YEAR_HIGH]=ch[0]>>8;
	cch[TRY_END_MON]=ch[1]&0xff;
	cch[TRY_END_DAY]=ch[2]&0xff;
	m24xx_write(addr,cch,TRY_END_SIZE+1);
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




//load display scheme
BOOL Updata_Execute()
{
	Read_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,&g_Current_Scene);
	if(g_Current_Scene>9 ||g_Current_Scene<1)
		g_Current_Scene=1;
//	load_scenario_from_FPGA(g_Current_Scene);
//	Read_EEPROM_Byte(E2ROM_BRIGHTNESS_ADDR,&g_BrightnessValue);
//	Read_EEPROM_Byte(E2ROM_CONTRAST_ADDR,&g_ContrastValue);
//	Read_EEPROM_Byte(E2ROM_SATURATION_ADDR,&g_SaturationValue);
	
//	Read_EEPROM_Byte(E2ROM_DVI_RESOLUTION_ADDR,&g_DVI_Resolution_Para);
//	Read_EEPROM_Byte(E2ROM_DVI_FRAMERATE_ADDR,&g_DVI_FrameRate_Para);
//	Read_EEPROM_Byte(E2ROM_VGA_RESOLUTON_ADDR,&g_VGA_Resolution_Para);
//	Read_EEPROM_Byte(E2ROM_VGA_FRAMERATE_ADDR,&g_VGA_FrameRate_Para);
	
	Read_EEPROM_Byte(E2ROM_SCREEN_LINE_ADDR,&g_stitching_screen_line);	//��ȡ�ж�����
	Read_EEPROM_Byte(E2ROM_SCREEN_COL_ADDR,&g_stitching_screen_column);	//��ȡ�ж�����		
	gScrRow=g_stitching_screen_line;
	gScrCol=g_stitching_screen_column;
	Read_Screen_line_column_pixel(); //��ȡ�������и��е���ʾ����		 
	Read_Scheme_IsSave();
	return __TRUE;
}

BOOL reset()
{	
	u8 i=0;
	for(i=0;i<8;i++)
	{
		Write_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6,1920);
		Write_EEPROM_U16(E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+i*6+3,1080);		
	}
	
	Write_Scheme_IsSave();
	Write_EEPROM_Byte(E2ROM_SCREEN_LINE_ADDR,0);
	Write_EEPROM_Byte(E2ROM_SCREEN_COL_ADDR,0);	
	
	Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,DEF_CURRENT_SCENE);
	Write_EEPROM_Byte(E2ROM_BRIGHTNESS_ADDR,DEF_BRIGHTNESS);
	Write_EEPROM_Byte(E2ROM_CONTRAST_ADDR,DEF_CONTRAST);
	Write_EEPROM_Byte(E2ROM_SATURATION_ADDR,DEF_SATURATION);
	Write_EEPROM_Byte(E2ROM_VOLUME_ADDR,DEF_VOLUME);
	
	g_FirstStart_Flag=0;
	Write_EEPROM_Byte(E2ROM_FirstStart_ADDR, g_FirstStart_Flag);	
	
	/////////////////////////////////////////////////////////
	
	
	/////////////////////////////////////////////////////////

	return __TRUE;
}


void GetMachineInfo(void)
{
	Read_EEPROM_Byte(E2ROM_DeviceID_Addr, &g_DeviceID);
}


