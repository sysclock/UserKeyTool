/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    04/06/2009
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
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Init.h"
#include "MyUart.h"
#include "Key.h"

/** @addtogroup Template_Project
  * @{
  */

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
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
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
  * @retval : None
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
  * @retval : None
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
  * @retval : None
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
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
	TickFlag = TRUE;
        CycleDelay ++;
        if(TipInfo.TipType == CycleOn)
        {
          if(CycleDelay == CycleInfo.CycleTiming*10)
          {            
            CycleDelay = 0;
            LoadEnv(CycleIndex+1);            
            BEEP_START;
            CycleIndex++;
            if(CycleIndex ==CycleInfo.CycleNum)
            {
              CycleIndex = 0;
            }
          }
        }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

void EXTI15_10_IRQHandler(void)
{
	/* Generate SysTick exception */
	//SCB->ICSR |= 0x04000000;
	/* Clear EXTI Line0 pending bit */
    if(EXTI_GetITStatus(EXTI_Line11) == SET)
    {
      IR_ISR();
	  EXTI_ClearITPendingBit(EXTI_Line11);
    }
    else
    {
      EXTI_ClearITPendingBit(EXTI_Line10);
      EXTI_ClearITPendingBit(EXTI_Line12);
      EXTI_ClearITPendingBit(EXTI_Line13);
      EXTI_ClearITPendingBit(EXTI_Line14);
      EXTI_ClearITPendingBit(EXTI_Line15);      
    }
    
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART1_RXD_IT();
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
      USART2_RXD_IT(); 
	  USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
        UART_STOP;
        Uart.RecvBuf[Uart.RecvLen] = 0;
        Uart.UartFlag = TRUE;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM3_IRQHandler(void)
{
	static int i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		if(i%2)
			BEEP_ON;
		else
			BEEP_OFF;		
		if(++i > 300)
		{
			i = 0;
			BEEP_OFF;
			BEEP_STOP;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if((IRInfo.Index == 32)&&(IRInfo.HeadFlag))
          IRInfo.EndFlag = TRUE;
        IR_STOP;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval : None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
