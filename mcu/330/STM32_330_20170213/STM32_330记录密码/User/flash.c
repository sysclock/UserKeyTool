#include "stm32f10x_flash.h"
#include "header.h"

volatile FLASH_Status FLASHStatus;

void FlashEraserPage(u32 startaddr,u32 endaddr)
{
	FLASHStatus = FLASH_COMPLETE; 	
	
	FLASH_Unlock();								//Unlock the Flash Program Erase controller
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  // Clear All pending flags	

	// Erase the FLASH pages 
	FLASHStatus = FLASH_ErasePage(startaddr);
}

void FlashWriteLogo()
{
	int i=0;
	u32 Address = 0x0;
	u8 datatoflash[22],sum=0;

	FlashEraserPage(LOGO_STARTADDR,LOGO_ENDADDR);

	for(i=0;i<21;i++)
	{

		datatoflash[i]=g_WelcomeStr[i];
		sum+=g_WelcomeStr[i];
	}
	datatoflash[21]=sum;
	
	Address = LOGO_STARTADDR;
	i=0;
	while((Address < LOGO_STARTADDR+22) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramHalfWord(Address, datatoflash[i+1]<<8|datatoflash[i]);
		Address = Address + 2;	
		i=i+2;
	}
	
}

void FlashReadLogo()
{
	int i=0;
	u8 dataout[21],sumout=0;
	u8 *p=(u8*)LOGO_STARTADDR;
	   
	while( p<( u8 *)LOGO_STARTADDR+21) 
	{
		dataout[i]=*p++;
		sumout+=dataout[i++];
 	}
	if(sumout==*p)
	{
		for(i=0;i<21;i++)
		{
			g_WelcomeStr[i]=dataout[i];
			g_WelcomeStrEn[i]=dataout[i];
		}
	}
}


void FlashWriteType()
{
	int i=0;
	u32 Address = 0x0;
	u8 datatoflash[22],sum=0;

	FlashEraserPage(TYPE_STARTADDR,TYPE_ENDADDR);

	for(i=0;i<21;i++)
	{
		datatoflash[i]=g_MachineTypeXPStr[i];
		sum+=g_MachineTypeXPStr[i];
	}
	datatoflash[21]=sum;
	
	Address = TYPE_STARTADDR;
	i=0;
	while((Address < TYPE_STARTADDR+22) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramHalfWord(Address, datatoflash[i+1]<<8|datatoflash[i]);
		Address = Address + 2;	
		i=i+2;
	}
	
}

void FlashReadType()
{
	int i=0;
	u8 dataout[21],sumout=0;
	u8 *p=(u8*)TYPE_STARTADDR;
	   
	while( p<( u8 *)TYPE_STARTADDR+21) 
	{
		dataout[i]=*p++;
		sumout+=dataout[i++];
 	}
	if(sumout==*p)
	{
		for(i=0;i<21;i++)
		{
			g_MachineTypeXPStr[i]=dataout[i];
		}
	}
}



void FlashWritePWD_Flag()
{
	int i=0;
	u32 Address = 0x0;
	u8 datatoflash[4],sum=0;

	FlashEraserPage(PWDFLAG_STARTADDR,PWDFLAG_ENDADDR);
	
	datatoflash[0]=g_PowerOnPwdFlag;
	datatoflash[1]=g_PowerOnPwdFlag;
	datatoflash[2]=g_PowerOnPwdFlag;
	datatoflash[3]=datatoflash[0]+datatoflash[1]+datatoflash[2];
	
	Address = PWDFLAG_STARTADDR;
	i=0;
	while((Address < PWDFLAG_STARTADDR+4) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramHalfWord(Address, datatoflash[i+1]<<8|datatoflash[i]);
		Address = Address + 2;	
		i=i+2;
	}
	
}

void FlashReadPWD_Flag()
{
	int i=0;
	u8 dataout[3],sumout=0;
	u8 *p=(u8*)PWDFLAG_STARTADDR;
	   
	while( p<( u8 *)PWDFLAG_STARTADDR+3) 
	{
		dataout[i]=*p++;
		sumout+=dataout[i++];
 	}
	if(sumout==*p)
	{
		g_PowerOnPwdFlag=dataout[0];
	}
}








void assert_failed(u8* file, u32 line)
{ 
	while(1)
	{
		
	}
}


