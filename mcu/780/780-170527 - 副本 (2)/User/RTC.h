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

void RTC_Init(void);

void DisplaySavedWeek1(void);
void DisplaySavedWeek2(void);
void DisplaySavedWeek3(void);
 /* Private function prototypes -----------------------------------------------*/
void RTC_Configuration(void);
void Time_Regulate(struct rtc_time *tm);



void Time_Adjust(void);
void Set_Current_Time(void);
void DisFourNumValue(U16 wValue,U8 addr);
void DisThreeNumValue(U16 wValue,U8 addr);
void DisTwoNumValue(U16 wValue,U8 addr);
U8 RepeatWeekToByte(U8 mode,U8 *week);
void DisplayTime(void);
void RtcTimeSnd(void);
void check_Try_End_Time(U32 day);
void DisplayDaxieNum(U8 num,U8 addr);
#endif
