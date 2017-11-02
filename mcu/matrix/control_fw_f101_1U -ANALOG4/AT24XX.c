#include "Init.h"
#include "AT24XX.h"

static u8 IICGetAck(void)
{
  	u8 Val;
	
	AT_SDA_INPUT;
	AT_SDA_HI;
	AT_SCK_HI;
	DelayUs(5);
	Val = AT_SDA_IS_HI;
	DelayUs(5);
	AT_SCK_LOW;
	DelayUs(10);
	AT_SDA_OUTPUT;
	return Val;
}

static void IICNAck(void)
{
	AT_SDA_HI;
	AT_SCK_HI;
	DelayUs(10);
	AT_SCK_LOW;
	DelayUs(10);
}

static void IICAck(void)
{
	AT_SDA_LOW;
	AT_SCK_HI;
	DelayUs(10);
	AT_SCK_LOW;
	DelayUs(10);
}

static void IICStart(void)
{
	AT_SDA_HI;
	AT_SCK_HI;
	DelayUs(5);
	AT_SDA_LOW;
	DelayUs(5);
	AT_SCK_LOW;
	DelayUs(10);
}

static void IICStop(void)
{
	AT_SDA_LOW;
	AT_SCK_HI;
	DelayUs(5);
	AT_SDA_HI;
	DelayUs(5);
	AT_SCK_LOW;
	DelayUs(10);
}

static void IICWriteByte(char c)
{
	int i;
	//Send Data
	for(i=0;i<8;i++)
	{
		if(c&0x80)
		  AT_SDA_HI;
		else
		  AT_SDA_LOW;
		
		AT_SCK_HI;
		DelayUs(10);
		AT_SCK_LOW;
		DelayUs(10);
		c <<= 1;
	}
}

static u8 IICReadByte(void)
{
	u8 Data=0;
	u8 i;
	AT_SDA_INPUT;	//ÊäÈë
	AT_SDA_HI;		//ÉÏÀ­
	for(i=0;i<8;i++)
	{
		AT_SCK_HI;
		DelayUs(5);	  
		Data <<= 1;
		if(AT_SDA_IS_HI)
		  Data |= 0x01;
		DelayUs(5);
		AT_SCK_LOW;
		DelayUs(10);
	}
	AT_SDA_OUTPUT;
	return Data;
}

u8 AT24xxWrite(u16 Addr,u8* pBuf,u8 Count)
{
  	u16 i;
	
	if((Addr >= FIX_ADD) && (SysInfo.TypeModifyEn == FALSE))
		return FALSE;
	
	if((Count == 0)||(Count > 16))
	  return FALSE;

	IICStart();
	IICWriteByte(0xa0+((Addr>>8)<<1));
	if(IICGetAck())
	{
		IICStop();
		return FALSE;
	}
	IICWriteByte(Addr&0xff);
	if(IICGetAck())
	{
		IICStop();
		return FALSE;
	}
	for(i=0;i<Count;i++)
	{
		IICWriteByte(*(pBuf+i));
		if(IICGetAck())
		{
			IICStop();
			return FALSE;
		}
	}
	IICStop();
	return TRUE;
}

u8 AT24xxRead(u16 Addr,u8* pBuf,u16 Count)
{
	u16 i = 0;
	if(Count == 0)
	  return FALSE;
	
	IICStart();
	IICWriteByte(0xa0+((Addr>>8)<<1));
	if(IICGetAck())
	{
		IICStop();
		return FALSE;
	}
	IICWriteByte(Addr&0xff);
	if(IICGetAck())
	{
		IICStop();
		return FALSE;
	}
	IICStart();
	IICWriteByte(0xa0+((Addr>>8)<<1)+1);
	if(IICGetAck())
	{
		IICStop();
		return FALSE;
	}
	
	if(Count > 1)
	{
		for(i=0;i<Count-1;i++)
		{
			*(pBuf+i) = IICReadByte();
			IICAck();
		}
	}
	*(pBuf+i) = IICReadByte();
	IICNAck();
	IICStop();
	
	return TRUE;
}
