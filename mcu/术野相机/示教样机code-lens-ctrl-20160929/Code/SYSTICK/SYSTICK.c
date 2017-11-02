
#include "stm32f10x.h"
#include "SYSTICK.h"


u8 zoomirq_flag=0;
u32 zoomirq_time=0;
extern u16 nTime;
/**************************************************************
 ** 函数名 :SYSTICK_Init
 ** 功能   :初始化SYSTICK，1us中断1次
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :SystemCoreClock/ 1000000：1us中断1次；SystemCoreClock/ 1000：1ms中断一次
***************************************************************/
void SYSTICK_Init(void)
{		
//	while (SysTick_Config(SystemCoreClock/ 1000000));
	while (SysTick_Config(SystemCoreClock/ 1000));
}
/**************************************************************
 ** 函数名 :delay_us
 ** 功能   :每次定时1us
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void delay_us(u16 nus)
{
	nTime=nus;
	while(nTime);
}
/**************************************************************
 ** 函数名 :delay_ms
 ** 功能   :每次定时1ms
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void delay_ms(u16 nms)
{
//	while(nms--)
//	delay_us(500);
	nTime=nms;
	while(nTime);
}
