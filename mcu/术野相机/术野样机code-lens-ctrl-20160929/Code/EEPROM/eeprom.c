
#include "eeprom.h"
#include "systick.h"

/*定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure*/ 



enum ENUM_TWI_REPLY {  TWI_NACK=0 ,TWI_ACK=1 };  
enum ENUM_TWI_BUS_STATE {  TWI_READY=0 ,TWI_BUS_BUSY=1 ,TWI_BUS_ERROR=2 };
void I2C_EE_Init(void) 
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// Configure I2C1 pins: SCL and SDA
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
} 

void TWI_delay(void)
{      
	uint8_t i=10; //i=10延时1.5us//这里可以优化速度 ，经测试最低到5还能写入
	while(i--);      
}

uint8_t TWI_Start(void)
{  
	SDAH; 
	SCLH; 
	TWI_delay();
	if(!SDAread)
		return TWI_BUS_BUSY; //SDA线为低电平则总线忙,退出  
	SDAL;  
	TWI_delay();
	if(SDAread)
		return TWI_BUS_ERROR; //SDA线为高电平则总线出错,退出
  SCLL;  
	TWI_delay();
	return TWI_READY;
}

void TWI_Stop(void)
{  
	SDAL;
	SCLL; 
  TWI_delay();
	SCLH;
	TWI_delay();  
  SDAH;  
	TWI_delay();
}

void TWI_Ack(void) 
{   
	SCLL;
	TWI_delay();
	SDAL;
	TWI_delay();
	SCLH;
	TWI_delay();
	SCLL; 
	TWI_delay();
}
void TWI_NoAck(void)
{   
	SCLL;
  TWI_delay();
  SDAH;
  TWI_delay();
  SCLH; 
	TWI_delay();
	SCLL; 
	TWI_delay(); 
}

uint8_t TWI_WaitAck(void)   //返回为:=1有ACK,=0无ACK 
{  
	SCLL; 
	TWI_delay();
	SDAH;
	TWI_delay(); 
	SCLH;
	TWI_delay();
	if(SDAread)
	{  
		SCLL;
		return 0;
	}  
	SCLL; 
	return 1;
}


void TWI_SendByte(uint8_t SendByte) //数据从高位到低位//
{      
	uint8_t i=8; 
	while(i--)
	{       
		SCLL; 
		TWI_delay();
		if(SendByte&0x80) 
			SDAH;   
		else    
			SDAL; 
		SendByte<<=1; 
		TWI_delay();  
		SCLH; 
		TWI_delay();  
	}     
	SCLL; 
}


uint8_t TWI_ReceiveByte(void)//数据从高位到低位//
{
	uint8_t i=8;
	uint8_t ReceiveByte=0;
	SDAH;
	while(i--)
	{			
		ReceiveByte<<=1;
		SCLL;
		TWI_delay();
		SCLH;
		TWI_delay();
		if(SDAread)
		{						
			ReceiveByte|=0x01;
		}	
	}
		SCLL;
		return ReceiveByte;
}

uint8_t TWI_WriteByte(uint8_t SendByte, uint32_t WriteAddress)
{       
	uint8_t i; 
	u32 ctrl;
	ctrl = (WriteAddress>>7) & 0x02; //-- 00000010
  ctrl |= 0xA0; //-- 1010xxxx
  i = TWI_Start();
	if(i) 
   return i;  
	TWI_SendByte( ctrl & 0xFE);//写器件地址  写入：地址最低位是0，读取：地址最低位是1 
	if(!TWI_WaitAck())  
	{     
		TWI_Stop(); 
		return 0; 
	} 
		
	TWI_SendByte(WriteAddress);   //设置起始地址 
	TWI_WaitAck(); 
	TWI_SendByte(SendByte); //写数据   
	TWI_WaitAck();     
	TWI_Stop();    //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	delay_ms(12);  //写入延时 12ms  写周期大于10ms即可
	return 3; 
}


uint8_t TWI_ReadByte( u8 * pBuffer ,u32 ReadAddress)
{        
	uint8_t i,temp; 
	u32 ctrl;
	ctrl = (ReadAddress>>7) & 0x02; //-- 00000010
  ctrl |= 0xA0; //-- 1010xxxx
	i = TWI_Start();
	if(i)   
		return i;  
	TWI_SendByte((ctrl & 0xFE));//写器件地址，先执行一次伪写操作  
   if(!TWI_WaitAck()) 
	{     
		TWI_Stop();  
		return 0; 
	} 
	
	TWI_SendByte(ReadAddress);   //设置起始地址  
	TWI_WaitAck();    
	TWI_Start();     
	TWI_SendByte((ctrl & 0xFE)|0x01);//写器件地址，先执行一次伪写操作   
	TWI_WaitAck();          //*pDat = TWI_ReceiveByte();  
	temp = TWI_ReceiveByte();   
	*pBuffer=temp;
	TWI_NoAck();          
	TWI_Stop();      
	return 0;//返回的如果是0，1，2则与错误代码相同了，再考虑一下
}


u8 CheckEEPRom()
{
	u8 testStr[6]={2,3,4,5,6,7};
	u8 ReadStr[6]={0};
	int i;
	for(i=0;i<6;i++)
	{
		TWI_WriteByte(testStr[i],i);	
		delay_ms(5);
		TWI_ReadByte(&(ReadStr[i]),i);
		
		if(testStr[i]!=ReadStr[i])
			return 0;
	}
	return 1;
}

