#include "stm32f10x_flash.h"
#include "flash.h"
#include "Strings.h"

u8 *p=(u8 *)STARTADDR;  
u32 EraseCounter = 0x0, Address = 0x0;
vu32 NbrOfPage = 0;
volatile FLASH_Status FLASHStatus;
ErrorStatus HSEStartUpStatus;


//*************************************************************************

void FlashEraserPage(u32 startaddr,u32 endaddr)
{
	FLASHStatus = FLASH_COMPLETE; 	
	
	FLASH_Unlock();								//Unlock the Flash Program Erase controller
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  // Clear All pending flags	

	// Erase the FLASH pages 
	FLASHStatus = FLASH_ErasePage(startaddr);
}



//*************************************************************************

void FlashTest()
{

	int i=0;
	u16 Data1=0x1112;

	FLASHStatus = FLASH_COMPLETE; 	

	NbrOfPage = (ENDADDR - STARTADDR) >> 10;	// Define the number of page to be erased
	
	FLASH_Unlock();								//Unlock the Flash Program Erase controller
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  // Clear All pending flags	

	// Erase the FLASH pages 
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(STARTADDR + (PAGESIZE * EraseCounter));
	} 
	Address = STARTADDR;
	while((Address < ENDADDR) && (FLASHStatus == FLASH_COMPLETE))
	{
		//FLASHStatus = FLASH_ProgramWord(Address, Data);
		//Address = Address + 4;
		FLASHStatus = FLASH_ProgramHalfWord(Address, Data1);
		Address = Address + 2;	
	}
  	   
	while( p<( u8 *)ENDADDR) 
	{
		USART_SendData(USART1, *p+48);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET)
		{
		}
	    p ++;
		for (i=0;i<30;i++);
 	}	  
}
 
void assert_failed(u8* file, u32 line)
{ 
  while(1)
  {
  }
}

//*************************************************************************

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




//*************************************************************************






//*************************************************************************




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






//*************************************************************************

































