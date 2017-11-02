/**
  ******************************************************************************
  * @file    Project/Template/main.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    04/06/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "Init.h"
#include "Lcd.h"
#include "Key.h"
#include "MyUart.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */ 

int main(void)
{ u8 tempval; char Val[10];
    /* Setup STM32 system (clock, PLL and Flash configuration) */
    RCC_Configuration();
    /* Add your application code here */
    PeriphClkInit();					//系统外设时钟使能
    GPIO_Configuration();			//GPIO配置
    SysTick_Configuration();	    //系统滴答初始化
	TimeInit();								//定时器初始化
    NVIC_Configuration();			//中断初始化
	KEY_DIS;
	LcdInit();
	LCD_BK_ON;

        //DrawStr(40,24,"INIT4FAIL!");
        //while(1);
    UART_LED_OFF;
    //SysInfo.TypeModifyEn = TRUE;//使能修改信息标志
    //AT24xxWrite(FIX_ADD,0,1);	
    //DelayMs(10); 
    //Com_Config(9600);	
	ReadSysInfo();		
	if(!SysInfo.InitOK)				//读取配置失败
	{
		Com_Config(9600);		//串口初始化
		SysInfo.Addr = 1;
		SysInfo.BeepEn = TRUE;
                lcd_clr();
                LcdDisStr(1,3,"INIT FAIL!",10);
		//myprintf("INIT FAIL!");//
                DelayMs(400);
                lcd_clr();
                LcdDisStr(1,4,"RESET...",8);
                DelayMs(300);
                ResetSysInfo();
                DelayMs(200);
                ReadSysInfo();	
                if(SysInfo.InitOK)
                {
                  lcd_clr();
                  LcdDisStr(1,3,"RESET OK!",9);
                }
                else
                {
                  lcd_clr();
                  LcdDisStr(1,2,"RESET FAIL!",11);
                }
                DelayMs(200);
                //DrawStr(40,24,"INIT FAIL!");
                Uart.UartFlag = FALSE;
		while(!SysInfo.InitOK)
		{
		  if(Uart.UartFlag)
                  {
                    UART_LED_ON;
		    DisposalUart();
                    UART_LED_OFF;
                    Uart.RecvLen = 0;
                    Uart.UartFlag = FALSE;
                  }
		}
	}

    if(!DataInit())     //根据输入输出路数，设置最大预置组
    {
        lcd_clr();
        LcdDisStr(1,2,"MALLOC FAIL!",12);
        while(1);
    }
    //LcdClear();
	Com_Config(BaudArray[SysInfo.Baud]);					//串口初始化
        lcd_clr();
	DrawLogo();
	InitSwitch();			//初始化切换器
    IWDGInit();
	TickFlag = FALSE;
	BEEP_START;
          
 CycleInfo.CycleTiming = 0;//轮询 
 CycleInfo.CycleNum = 0;
 SysInfo.CycleEn = 0;//FALSE
 CycleIndex = 0;
  /* Infinite loop */
  while (1)
  {
    if(TickFlag)	//100ms时间到	
    {
	DisposalTick();
        IWDG_ReloadCounter();            
	TickFlag = FALSE;
    }
    
    if(Uart.UartFlag)	
    {
      UART_LED_ON;
      DisposalUart();
      UART_LED_OFF;
      Uart.RecvLen = 0;
      Uart.UartFlag = FALSE;
    }
    
    if(IRInfo.EndFlag)
    {
      if(LcdBkSec >= 300)
        LCD_BK_ON;
      DisposalIR();
      BEEP_START;
      IRInfo.EndFlag = FALSE;      
      LcdBkSec = 0;	
    }
    //UART_LED_ON;//    
    //DelayMs(200);
    if(ReadKey())
    {
      if(LcdBkSec >= 300)
        LCD_BK_ON;
      else
      {
        BEEP_START;                
        DisposeKey();
      }
      LcdBkSec = 0;
    }
    //UART_LED_OFF;
    //DelayMs(200);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
