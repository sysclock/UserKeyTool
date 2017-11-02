#include <stdio.h>
#include "stm32f10x_it.h"	
#include "stm32_eval.h"
#include "header.h"
#include "Queue.h"

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
	U16 recv_data = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{ 
		USART_ClearITPendingBit(USART1,    USART_IT_RXNE);
		recv_data = USART_ReceiveData(USART1);
		if(!g_TryTimeEndFlag)  //判断试用是否到期
		{
//			USART_SendData(USART3,recv_data);   //用于级联
			//while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}
			//DealPCMsg(recv_data);	
			QueueIn(&g_strUartRxQueue,recv_data);
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
	U16 recv_data = 0;
//	unsigned int plen;
       /*中断接收*/
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		recv_data  = USART_ReceiveData(USART2);
		DealUart2Data(recv_data);
	}
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

// void USART3_IRQHandler(void)
// { 
// 	 /*中断接收*/
// // 	U16 recv_data = 0;
// // 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//判断是不是接收中断
// // 	{ 
// // 		USART_ClearITPendingBit(USART3,    USART_IT_RXNE);
// // 		recv_data = USART_ReceiveData(USART3);
// // 		if(!g_TryTimeEndFlag)  //判断试用是否到期
// // 		{	
// // 		
// // 			if(g_ComTypeFlag==Communication_Ethernet)  //当前为1号机器，接网线
// // 			{
// // 				QueueIn(&g_strUart3RxQueue,recv_data);
// // 			}
// // 			else

// // 			{
// // 				USART_SendData(USART1,recv_data);   //用于级联
// // 				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
// // 			}
// // 		}
// // 	} 

// }
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
/*void EXTI0_IRQHandler(void)
{
   int i;
 
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
  	  for(i=0;i<=8000000;i++)
  	  {
	  // GPIOC->ODR = 0xfffffC3f;	//
	  GPIOB->ODR = 0xfffffff3;
	  GPIOA->ODR = 0xfffffff3;				
      }
	  for(i=0;i<=2000000;i++)
	  {
      //GPIOC->ODR = 0xffffffff;  //
	  GPIOA->ODR = 0xffffffff;
	  GPIOB->ODR = 0xffffffff;
	  }
	  for(i=0;i<=1000000;i++)
      
	   
	// Clear the EXTI line 0 pending bit //
    EXTI_ClearITPendingBit(EXTI_Line0);
  }

}*/
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

    STM32_RtcTimeSnd();
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
	delay_ms(10);    //消抖	
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
void EXTI1_IRQHandler(void)
{
	delay_ms(10);    //消抖
#ifdef EC11_BUTTON
	if(BMC == 0x00)		//按键按下:低电平有效
	{ 	
		DealOkKey();  
	}
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位
#endif	
}

#ifdef TWO_EC11_BOARD
void EXTI0_IRQHandler(void)
{
	delay_ms(10);    //消抖	
	if(BMC2 == 0x00)		//按键按下:低电平有效
	{ 	
//		DealOkKey();  
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI1线路挂起位
}
#endif
#endif


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
