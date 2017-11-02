#ifndef _RTC_H_
#define _RTC_H_

#include "stdint.h"
#include "clock_config.h"
#include "fsl_rtc.h"

#define BLE_COMM 1
#define USB_COMM 2

extern rtc_datetime_t date;
extern rtc_datetime_t date_set;

extern char GMT_num;
extern char GMT_num_set;

extern uint8_t minute_updata_flag;

void User_RTC_Init(void);
uint8_t User_Set_time(void);
uint8_t User_Set_UTC(void);
void BOARD_SetRtcClockSource(void);
void Display_current_time(void);
void Updata_time(uint8_t);

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

void Delay_S(uint32_t num);

#endif
