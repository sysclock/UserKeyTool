/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]:RTC.h
 [Date]:        26-march-2013
 [Comment]:
   LCD header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_

void LEDToggle(void);
void DisplaySavedWeek1(void);
void DisplaySavedWeek2(void);
void DisplaySavedWeek3(void);

void DisFourNumValue(U16 wValue,U8 addr);
void DisThreeNumValue(U16 wValue,U8 addr);
void DisTwoNumValue(U16 wValue,U8 addr);
//EventValue value; 
void STM32_RTC_Init(void);
void STM32_DisplayTime(void);
void STM32_RTC_Configuration(void);
void STM32_Time_Regulate(struct rtc_time *tm);
void STM32_Time_Adjust(void);
void STM32_RtcTimeSnd(void);
void CycleSwitchInputSrc(_InputSourceType CycleSrc);
void check_Try_End_Time(U32 day);
void DisplayDaxieNum(U8 num,U8 addr);
BOOL ReadLog(void);
U16 GetLogAddr(void);
void ClearLog(void);

void Clear_24C(void);

void SetCurrentTimeWithPassWordCheck(void);

#endif
