#ifndef _PIT_H_
#define _PIT_H_

#include "stdint.h"


#define INTERACT_TIMEOUT 5

#define MEASURE_TIMEOUT  60

extern uint32_t Pw1101_interact_timeout;
extern uint8_t  Pw1101_interact_flag;

extern uint32_t Measure_time_flag;

extern uint32_t Auto_Measure_TimeInterval_Count;
extern uint32_t Auto_Measure_Total_Num_Count;
extern uint32_t Auto_Measure_Success_Count;
extern uint32_t Auto_Measure_Fail_Count;

extern uint32_t StandbyTime_Count; //做待机时间统计
extern uint32_t StandbyTime_Flag;

extern uint8_t display_point_flag;

extern uint32_t bat_display;

void User_PIT_Init(void);
void Delay_S(uint32_t num);

#endif
