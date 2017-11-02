#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "pit.h"
#include "fsl_pit.h"
#include "MenuTbl.h"

#define PIT_LED_HANDLER PIT0_IRQHandler
#define PIT_IRQ_ID PIT0_IRQn
/* Get source clock for PIT driver */
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define LED_INIT() LED_RED_INIT(LOGIC_LED_ON)
#define LED_TOGGLE() LED_RED_TOGGLE()

uint32_t Delay_Time_Num=0;

uint32_t Pw1101_interact_timeout=0;
uint8_t  Pw1101_interact_flag=0;

uint8_t display_point_flag=0;


uint32_t Measure_time_flag=0;

uint32_t bat_display=0;

uint32_t Auto_Measure_TimeInterval_Count=0;
uint32_t Auto_Measure_Total_Num_Count=0; //进行测量次数统计，因可以设置测量时长和间隔时间，所以最大测量次数是一个可推导的值
uint32_t Auto_Measure_Success_Count=0;
uint32_t Auto_Measure_Fail_Count=0;
uint32_t StandbyTime_Count=0;
uint32_t StandbyTime_Flag=0;


//秒中断，此函数每秒进行一次，用来做一些超时检测用
void PIT_LED_HANDLER(void)
{ 	
	/* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	
		if(Delay_Time_Num) Delay_Time_Num--;    //延时函数用
		if(Pw1101_interact_flag)Pw1101_interact_timeout++;  //PW1101通信超时用
				
		if(display_point_flag) display_point_flag--;
		
		if(Measure_time_flag) Measure_time_flag--;
		bat_display++;
	
		if(Auto_Measure_OnOff==TURN_ON) Auto_Measure_TimeInterval_Count++; //如果当前自动测量状态为打开，为下一次进行计时
	
		if(StandbyTime_Flag) StandbyTime_Count++;   //待机定时器
	

}



//秒级延时
void Delay_S(uint32_t num)
{
	Delay_Time_Num=num;
	while(Delay_Time_Num);
}


//初始化
void User_PIT_Init(void)
{
		pit_config_t pitConfig;
		PIT_GetDefaultConfig(&pitConfig);

    /* Init pit module */
    PIT_Init(PIT, &pitConfig);

    /* Set timer period for channel 0 */
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(1000000U, PIT_SOURCE_CLOCK));

    /* Enable timer interrupts for channel 0 */
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(PIT_IRQ_ID);

    /* Start channel 0 */
    PIT_StartTimer(PIT, kPIT_Chnl_0);

}

