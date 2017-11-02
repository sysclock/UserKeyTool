/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"	
//#include "stm32_eval.h"
#include "header.h"
#include "Queue.h"
#include "remote.h"

extern U8 g_RevUart2Data[1024];
extern U16 g_RevUart2Idx;
extern U8 g_RevUart2Data_in[20];
extern U8 g_Rev3531_message[20];
extern U8 g_Rev3531_message_Idx;

extern U8 g_RevUart3Data[12];
extern U8 g_RevUart3Idx;
extern U8 g_RevUart3Data_in[12];


extern U8 g_RevUart1Data[12];
extern U8 g_RevUart1Idx;
extern U8 g_RevUart1Data_in[12];
extern U8 LCD_ready;

U8 g_MESSAGE_FLAG=0;
U32 g_MESSAGE_DisplayTime_Count=0;
U8 g_DISCONNECT_FLAG=0;
U8 g_disconncet_which=0;

U8 g_RECONNECT_FLAG=0;
U8 g_reconncet_which=0;
U16 g_message_resolution_H=0;
U16 g_message_resolution_V=0;
U8  g_message_framrate=0;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
//extern __IO uint32_t TimeDisplay;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}
 
 /**********************************************************************
* 名    称：USART1_IRQHandler()
* 功    能：USART1中断
* 入口参数：
* 出口参数：
* 全局变量：
-----------------------------------------------------------------------
* 说明：
***********************************************************************/
void USART1_IRQHandler(void)
{ 
	U8 recv_data = 0;
	U8 i;

  /*中断接收*/
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		dat = USART_ReceiveData(USART1);
		if(!g_TryTimeEndFlag)  //判断试用是否到期
		{
			QueueIn(&g_strUartRxQueue,USART_ReceiveData(USART1));
			g_ComTypeFlag=Communication_USART1;
		}
	}

}  
/**********************************************************************
* 名    称：USART2_IRQHandler()
* 功    能：USART2中断
* 入口参数：
* 出口参数：
* 全局变量：
-----------------------------------------------------------------------
* 说明：
***********************************************************************/
			
void USART2_IRQHandler(void)
{  
//	U8 recv_data = 0;
//	U8 i;
  /*中断接收*/
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		if(g_Read_Command_Flag)
		{
			g_RevUart2Data[g_RevUart2Idx]=USART_ReceiveData(USART2);
			g_RevUart2Idx++;
			g_RevUart2Idx %=1024;
		}
	}	
//	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}
/**********************************************************************
* 名    称：USART3_IRQHandler()
* 功    能：USART2中断
* 入口参数：
* 出口参数：
* 全局变量：
-----------------------------------------------------------------------
* 说明：
***********************************************************************/
void USART3_IRQHandler(void)
{ 
	 /*中断接收*/
	U16 recv_data = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		recv_data = USART_ReceiveData(USART3);
	}	 
}


void UART4_IRQHandler(void)
{ 
	 /*中断接收*/
	U16 recv_data = 0;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);

/**********************************************************************/
		QueueIn(&g_strUart3RxQueue,4);
	}	 
// 	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET){}  ;	
}



void UART5_IRQHandler(void)
{ 
	 /*中断接收*/

}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
   int i;
 
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
  	  for(i=0;i<=8000000;i++)
  	  {
	  /* GPIOC->ODR = 0xfffffC3f;	*/
	  GPIOB->ODR = 0xfffffff3;
	  GPIOA->ODR = 0xfffffff3;				
      }
	  for(i=0;i<=2000000;i++)
	  {
      /*GPIOC->ODR = 0xffffffff;  */
	  GPIOA->ODR = 0xffffffff;
	  GPIOB->ODR = 0xffffffff;
	  }
	  for(i=0;i<=1000000;i++)
      
	   
	/* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }

}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
  
/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
  	#ifdef INT_DELAY_PC_CTL
		g_OneSecondISRVal=(g_OneSecondISRVal+1)%31;
		if(g_OneSecondISRVal==0)
		{//当30秒到之后，判断30秒之前是否发送保持连接命令
			if(!g_PCSendFlag)  //没有发送
				g_PCControlFlag=__FALSE;
			else//已经发送
				g_PCSendFlag=__FALSE;
		}
	#endif
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

		RtcTimeSnd();
		
		if(LCD_BACK_NUM>0)
		{
			LCD_BACK_NUM_TIME++;
			if(LCD_BACK_NUM_TIME>1800)
			{						
					LCD_BACK_NUM=0;
					Send_LCD_BACK(LCD_BACK_NUM);
			}
		}
		
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
    /* Reset RTC Counter when Time is 23:59:59 */
    if (RTC_GetCounter() == 0x00015180)
    {
      RTC_SetCounter(0x0);
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
    }
  }
}


#ifdef STM32F103RB
//外部中断1服务程序
	void EXTI9_5_IRQHandler(void)
	{
		delay_ms(100);    //消抖	
	#ifdef TCP_IP
		#ifdef ENC28J60INT
			if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0x00)		//按键按下:低电平有效
			{ 			
				ReceiveEthernetPacket();	
			}
			EXTI_ClearITPendingBit(EXTI_Line5);  //清除EXTI1线路挂起位  
   		#endif
	#endif
	}
	//外部中断PC12服务程序

	void EXTI1_IRQHandler(void)
	{
	#ifdef EC11_BUTTON
        delay_ms(100);
		if(BMC == 0x00&&g_LockKeyFlag==UNLOCKKEYPAD)		//按键按下:低电平有效
		{ 	
			DealOkKey();  
		}
		EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位
	#endif	
	}

#endif



/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
