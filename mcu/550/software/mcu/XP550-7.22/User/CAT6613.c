
#include "cat6613.h"
#include "header.h"
/*
#if (ENABLE_SOFTI2C)	

 static void MyDelay_us(u8 n)
{
	u8 i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<8;j++)
			;
	}
}

void I2C_Write_412(u8 addr,u8 dat1,u8 dat2)
{
	//起始位
	I2CStart();
	//发送器件地址(写)
  	I2CWriteByte( addr);
  	if( RESET == I2CSlaveAck() )  	
   		return;	 

       MyDelay_us(1);
	//写一个字节
  	I2CWriteByte(dat1);
  	if( RESET == I2CSlaveAck() )  	
   		return;
       MyDelay_us(1);

	I2CWriteByte(dat2);
  	if( RESET == I2CSlaveAck() )  	
   		return;
       MyDelay_us(1);

	I2CStop();
}
#endif
*/

unsigned char InitP412(void)
{
	u8 redata=0;
	u8 dat1=0x53;
	u8 dat2=0x53;
	redata= I2C_ByteWrite_ToDevice(P412_WADDR,dat1,dat2);
	return redata;
}

//关闭412的输出
unsigned char OUTPUT_HiZ_P412(void)
{
	u8 redata=0;
	u8 dat1=0x00;
	u8 dat2=0x00;
	redata= I2C_ByteWrite_ToDevice(P412_WADDR,dat1,dat2);
	return redata;
}

unsigned char InitCAT6613(void)
{
	u8 redata=0;
	U8 VenderID=0,DeviceID=0,Reg04=0,i=0;
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x0F,0);
	
	redata=I2C_ReadBuf_FromDevice(CAT6613ADDR,0x01, &VenderID, 1);
	if(VenderID!=0xCA)
		return 1;
	redata=I2C_ReadBuf_FromDevice(CAT6613ADDR,0x02, &DeviceID, 1);
	if(DeviceID!=0x13)
		return 1;
	redata=I2C_ReadBuf_FromDevice(CAT6613ADDR,0x03, &DeviceID, 1);
	if((DeviceID&0x0F)!=0x06)
		return 1;
	redata=I2C_ReadBuf_FromDevice(CAT6613ADDR,0x04, &Reg04, 1);
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x04,(Reg04|_BIT5));
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x04,0x05);
	
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x61,0x10);
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x61,0x00);
	for(i=0;i<17;i++)
		I2C_ByteWrite_ToDevice(CAT6613ADDR,(0xC0+i),0);
	
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x0F,1);
	redata=I2C_ByteWrite_ToDevice(CAT6613ADDR,0x58,0);
	return redata;
}


////应该还有其他函数啊，比如disableCAT6613():PowerDownCat6613
unsigned char disableCAT6613(void)
{
	
	return 0;
}

























