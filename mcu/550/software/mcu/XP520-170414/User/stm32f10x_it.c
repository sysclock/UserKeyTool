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

extern U8 g_RevUart2Data[12];
extern U8 g_RevUartIdx;
extern U8 g_RevUart2Data_in[12];
extern U8 g_RevUart3Data[13];
extern U8 g_RevUart3Idx;
extern U8 g_RevUart3Data_in[12];
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
//	u8 dat;
	/*中断接收*/
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART1,    USART_IT_RXNE);
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
	U8 recv_data = 0;
	U8 i;
  /*中断接收*/
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
 		 recv_data  = USART_ReceiveData(USART2);
		USART_SendData(USART1, recv_data); 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
	
	}
		g_RevUart2Data[g_RevUartIdx]=recv_data;
		g_RevUartIdx++;
		if(g_RevUartIdx==1&&g_RevUart2Data[AA_IDX]!=0xAA)
				g_RevUartIdx=0;

			if(g_RevUartIdx==12)
			{
				if(g_RevUart2Data[EVENTID_IDX]==RETURN_EVENT&&g_RevUart2Data[PARA1_IDX]==0x06
				&&g_RevUart2Data[PARA2_IDX]==0x06&&g_RevUart2Data[PARA3_IDX]==0x06)
				{
					g_Fli32626HRevision=g_RevUart2Data[PARA4_IDX];
					g_Fli32626LRevision=g_RevUart2Data[PARA5_IDX];
					g_PoweronFlag=__TRUE;
					return;
				}
				if(g_RevUart2Data[EVENTID_IDX]==MAIN_ZOOM_EVENT||g_RevUart2Data[EVENTID_IDX]==SUB_ZOOM_EVENT)
				{
					if(g_RevUart2Data[PARA1_IDX]==0x07)
					{
						//chinesedisplay(0x88,"2507");
						for(i=0;i<CMDLENTH;i++)
							g_RevUart2Data_in[i]=g_RevUart2Data[i];
					}
				}
			}
		g_RevUartIdx%=12;
		g_ReceMsgFlag=__TRUE;
			
	
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
	// 	 /*中断接收*/
	U16 recv_data = 0;
	U8 i;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//判断是不是接收中断
	{
		USART_ClearITPendingBit(USART3,    USART_IT_RXNE);
		recv_data = USART_ReceiveData(USART3);
	}		 
		g_RevUart3Data[g_RevUart3Idx]=recv_data;
		g_RevUart3Idx++;
			
		if(g_RevUart3Idx==1&&g_RevUart3Data[AA_IDX]!=0xAA)
			g_RevUart3Idx=0;	
// 			else if(g_RevUart3Data[AA_IDX+1]==0xAA)
// 			{
// 				g_RevUart3Data[AA_IDX]=0xAA;
// 				g_RevUart3Idx=1;	
// 			}
		
			
		if(g_RevUart3Idx==12)
		{
			if(g_RevUart3Data[EVENTID_IDX]==RETURN_EVENT&&g_RevUart3Data[PARA1_IDX]==0x06
			&&g_RevUart3Data[PARA2_IDX]==0x06&&g_RevUart3Data[PARA3_IDX]==0x06)
			{
				g_550subHRevision=g_RevUart3Data[PARA4_IDX];
				g_550subLRevision=g_RevUart3Data[PARA5_IDX];
			}
			if((g_RevUart3Data[EVENTID_IDX]==MAIN_ZOOM_EVENT)  ||(g_RevUart3Data[EVENTID_IDX]==USB_STATE_EVENT))
			{
					{
						//chinesedisplay(0x88,"2507");
						for(i=0;i<CMDLENTH;i++)
							g_RevUart3Data_in[i]=g_RevUart3Data[i];
					}
			}
		}		
		

	g_RevUart3Idx %=12;	
	
// 	 /*中断接收*/
// 	U16 recv_data = 0;
// 	U8 i;
// 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//判断是不是接收中断
// 	{
// 		USART_ClearITPendingBit(USART3,    USART_IT_RXNE);
// 		recv_data = USART_ReceiveData(USART3);
// 	}		 
// 		if( g_RevUart3Idx<12)
// 		{
// 	
// 			g_RevUart3Data[g_RevUart3Idx]=recv_data;
// 			g_RevUart3Idx++;
// 			
// 			if(g_RevUart3Idx==1&&g_RevUart3Data[AA_IDX]!=0xAA)
// 				g_RevUart3Idx=0;	
// // 			else if(g_RevUart3Data[AA_IDX+1]==0xAA)
// // 			{
// // 				g_RevUart3Data[AA_IDX]=0xAA;
// // 				g_RevUart3Idx=1;	
// // 			}
// 			
// 				
// 			if(g_RevUart3Idx==12)
// 			{
// 				if(g_RevUart3Data[EVENTID_IDX]==RETURN_EVENT&&g_RevUart3Data[PARA1_IDX]==0x06
// 				&&g_RevUart3Data[PARA2_IDX]==0x06&&g_RevUart3Data[PARA3_IDX]==0x06)
// 				{
// 					g_550subHRevision=g_RevUart3Data[PARA4_IDX];
// 					g_550subLRevision=g_RevUart3Data[PARA5_IDX];
// 				}
// 			}		
// 			if(g_RevUart3Data[EVENTID_IDX]==MAIN_ZOOM_EVENT &&g_RevUart3Idx==12)
// 			{
// 				;
// 			}
// 			else
// 			{
// 				g_RevUart3Idx %=12;	
// 			}
// 		}	
		
	
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

void EXTI0_IRQHandler(void)
{
	delay_ms(100);    //消抖	
#ifdef EC11_BUTTON
	if(BMC == 0x00)		//按键按下:低电平有效
	{ 	
		DealOkKey();  
	}
#endif
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI1线路挂起位
}

void EXTI1_IRQHandler(void)
{
	delay_ms(100);    //消抖	
#ifdef EC11_BUTTON
	if(BMC == 0x00)		//按键按下:低电平有效
	{ 	
		DealOkKey();  
	}
#endif
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位
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
	  
	  GPIOB->ODR = 0xfffffff3;
	  GPIOA->ODR = 0xfffffff3;				
      }
	  for(i=0;i<=2000000;i++)
	  {
      
	  GPIOA->ODR = 0xffffffff;
	  GPIOB->ODR = 0xffffffff;
	  }
	  for(i=0;i<=1000000;i++)
      
	   
	
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

	RtcTimeSnd();
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
/*
#ifdef EC11_BUTTON
	if(BMC == 0x00)		//按键按下:低电平有效
	{ 	
		DealOkKey();  
	}
	EXTI_ClearITPendingBit(EXTI_Line6);  //清除EXTI1线路挂起位
#endif	
*/
}
//外部中断PC12服务程序

//u8 uart4_buf[10];
/*
void EXTI15_10_IRQHandler(void)
{	
	u8 rxdata;
	
	if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0x00)		//按键按下:低电平有效
	{ 
		EXTI->IMR &= ~(1<<12);   //屏蔽外部中断
		rxdata=UART4_RxByte();	
		
		UART1_SendByte(rxdata);
		//UART4_SendByte(rxdata);
		//QueueIn(&g_strUart4RxQueue,rxdata);
		EXTI->IMR |= (1<<12);   //屏蔽外部中断
	} 
	EXTI_ClearITPendingBit(EXTI_Line12);  //清除EXTI1线路挂起位   
}
*/

#else
//外部中断1服务程序
void EXTI15_10_IRQHandler(void)
{	
	delay_ms(10);    //消抖	
#ifdef TCP_IP
   #ifdef ENC28J60INT
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0x00)		//按键按下:低电平有效
	{ 
		ReceiveEthernetPacket();
	} 
	EXTI_ClearITPendingBit(EXTI_Line11);  //清除EXTI1线路挂起位    
   #endif
#endif
}
#endif


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
