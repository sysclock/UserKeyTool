#include "header.h"

//extern struct rtc_time systmtime;
struct rtc_time systmtime;
struct rtc_time systmtimestart;


//-----------set chedule -------------------------------------
extern  U8 g_InputTime1[2];   //hour   min 
extern  U8 g_InputWeek1[7];
extern  U8 g_InputWeek1Idx;

extern  U8 g_InputTime2[2];
extern  U8 g_InputWeek2[7];
extern  U8 g_InputWeek2Idx;

extern  U8 g_InputTime3[2];
extern  U8 g_InputWeek3[7];
extern  U8 g_InputWeek3Idx;
//--------------------------------------------------
//显示之前已经设置的重复模式的星期
void DisplaySavedWeek1()
{
	U8 idx=0;
	U8 week[1];
	if(g_InputWeek1Idx!=0)
	{
		for(idx=0;idx<g_InputWeek1Idx;idx++)		
		{
			week[0]=g_InputWeek1[idx]+48;
			chinesedisplay(0x90+idx,week);
		}
		printStr(" _");
	}
}
void DisplaySavedWeek2()
{
	U8 idx=0;
	U8 week[1];
	if(g_InputWeek2Idx!=0)
	{		
		for(idx=0;idx<g_InputWeek2Idx;idx++)		
		{
			week[0]=g_InputWeek2[idx]+48;
			chinesedisplay(0x90+idx,week);
		}
		printStr(" _");
	}
}
void DisplaySavedWeek3()
{
	U8 idx=0;
	U8 week[1];
	if(g_InputWeek3Idx!=0)
	{	
		for(idx=0;idx<g_InputWeek3Idx;idx++)		
		{
			week[0]=g_InputWeek3[idx]+48;
			chinesedisplay(0x90+idx,week);
		}
		printStr(" _");
	}
}

void DisTwoNumValue(U16 wValue,U8 addr)
{
	U8 ch[3];
	U8 ch1,ch2,i=0;
	ch1=wValue/10%10;
	ch2=wValue%10;
	/*if(ch1!=0)
	{
		ch[i++]=ch1+48;
	}*/
	ch[i++]=(ch1==0)?' ':(ch1+48);
	ch[i++]=ch2+48;
	ch[i]=0;
	chinesedisplay(addr, ch);
}

void DisThreeNumValue(U16 wValue,U8 addr)
{
	U8 ch[4];
	U8 ch1,ch2;
	ch1=wValue/100%10;
	ch2=wValue/10%10;
	ch[0]=(ch1==0)?' ':(ch1+48);
	ch[1]=((ch2==0)&&(ch1==0))?' ':(ch2+48);
	ch[2]=(wValue%10+48);
	ch[3]=0;
	chinesedisplay(addr, ch);
}

void DisFourNumValue(U16 wValue,U8 addr)
{
	U8 ch[5];
	U8 ch1,ch2,ch3,i=0;
	ch1=wValue/1000%10;  //326548
	ch2=wValue/100%10;
	ch3=wValue/10%10;
	if(ch1!=0)
	{
		ch[i++]=ch1+48;
	}
	if((ch2!=0)||(ch1!=0))
	{

		ch[i++]=ch2+48;
	}
	if((ch3!=0)||(ch2!=0)||(ch1!=0))
	{

		ch[i++]=ch3+48;
	}
	ch[i++]=(wValue%10+48);
	ch[i]=0;
	/*
	ch[0]=(ch1==0)?' ':(ch1+48);
	ch[1]=((ch2==0)&&(ch1==0))?' ':(ch2+48);
	ch[2]=((ch3==0)&&(ch2==0)&&(ch1==0))?' ':(ch3+48);
	ch[3]=(wValue%10+48);
	ch[4]=0;*/
	chinesedisplay(addr, "    ");
	chinesedisplay(addr, ch);
}

void DisplayTime()
{	
  	U16 bak;
	bak   = systmtime.tm_year;				// 获取 年
	TimeSet[0]=bak;
	DisFourNumValue(bak,YEARADDR);
	bak   = systmtime.tm_mon;				// 获取 月
	TimeSet[1]=bak;
	DisTwoNumValue(bak,MONTHADDR);
	bak   = systmtime.tm_mday;				// 获取 日
	TimeSet[2]=bak;
	DisTwoNumValue(bak,DAYADDR);
}

void check_Try_End_Time(U32 day)
{
	struct rtc_time systmtime1;
	to_tm(RTC_GetCounter()+day*86400, &systmtime1);
	g_TryEndDate[0]=systmtime1.tm_year;
	g_TryEndDate[1]=systmtime1.tm_mon;
	g_TryEndDate[2]=systmtime1.tm_mday;
}

void DisplayDaxieNum(U8 num,U8 addr)
{
	U8 weeknum[3]={0};
	switch(num)
	{
		case 0:
			weeknum[0]=0xC8;
			weeknum[1]=0xD5;
			break;
		case 1:
			weeknum[0]=0xD2;
			weeknum[1]=0xBB;
			break;
		case 2:
			weeknum[0]=0xB6;
			weeknum[1]=0xFE;
			break;
		case 3:
			weeknum[0]=0xC8;
			weeknum[1]=0xFD;
			break;
		case 4:
			weeknum[0]=0xCB;
			weeknum[1]=0xC4;
			break;
		case 5:
			weeknum[0]=0xCE;
			weeknum[1]=0xE5;
			break;
		case 6:
			weeknum[0]=0xC1;
			weeknum[1]=0xF9;
			break;
	}
	chinesedisplay( addr,weeknum);
}

void DisplayCurTime()
{	
  	U16 bak;
	
	bak   = systmtime.tm_sec;	
	chinesedisplay(SECONDADDR, "  ");					// 获取 秒钟
	DisTwoNumValue(bak,SECONDADDR);

	bak   = systmtime.tm_min;	// 获取 分钟				
	DisTwoNumValue(bak,MINADDR);
	
	bak   = systmtime.tm_hour;		// 获取 小时
	DisTwoNumValue(bak,HOURADDR);	
	
	bak   = systmtime.tm_wday;				// 获取 星期
	if(g_LanguageFlag==CHINESE)
	{
		DisplayDaxieNum(bak,WEEKADDR);
	}
	else  if(g_LanguageFlag==ENGLISH)
	{		
		switch(bak)
		{
			case 0:
				chinesedisplay( 0x98,"Sun.");
				break;
			case 1:
				chinesedisplay( 0x98,"Mon.");
				break;
			case 2:
				chinesedisplay( 0x98,"Tues");
				break;
			case 3:
				chinesedisplay( 0x98,"Wed.");
				break;
			case 4:
				chinesedisplay( 0x98,"Thur");
				break;
			case 5:
				chinesedisplay( 0x98,"Fri.");
				break;
			case 6:
				chinesedisplay( 0x98,"Sat.");
				break;
		}
	}
}


void CheckDeadline(U16 year,U8 mon,U8 day)
{
	if(year>g_TryEndDate[0]||(year==g_TryEndDate[0]&&mon>g_TryEndDate[1])
		||(year==g_TryEndDate[0]&&mon==g_TryEndDate[1])&&day>=g_TryEndDate[2])
		g_TryTimeEndFlag=__TRUE;
}

void CycleSwitchInputSrc(_InputSourceType CycleSrc)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	SwitchInputMethods(CycleSrc,g_ActiveImageValue,__TRUE);	
	clearLineFromAddr(0x90);
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x92, g_CycleScheduleStr+2);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x92, g_CycleScheduleStrEn+2);
}

void CheckCycleInput(U8 hour,U8 min)
{
	_InputSourceType input=INPUTSOURCE_NONE;
	if(g_MutiWindow==SINGLEIMAGE)
		input=g_MainInput;
	else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE)
	{
		if(g_ActiveImageValue==MAIN)
			input=g_MainInput;
		else
			input=g_SubInput;
	}
	else if(g_MutiWindow==SAMEPIPIMAGE)
	{
		if(g_NextPointImage==MAIN)
			input=g_SubInput;
		else
			input=g_MainInput;
	}
	if( g_CycleInput1==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			if(g_CycleInput2!=INPUTSOURCE_NONE)
			{
				CycleSwitchInputSrc(g_CycleInput2);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour2)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin2)%60;
			}
			else
			{
				g_StartCycleInputFlag=__FALSE;
			}
		}
	}
	else if( g_CycleInput2==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			if(g_CycleInput3!=INPUTSOURCE_NONE)
			{
				CycleSwitchInputSrc(g_CycleInput3);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour3)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin3)%60;
			}
			else
			{
				CycleSwitchInputSrc(g_CycleInput1);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour1)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin1)%60;
			}
		}
	}
	else if( g_CycleInput3==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			if(g_CycleInput4!=INPUTSOURCE_NONE)
			{
				CycleSwitchInputSrc(g_CycleInput4);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour4)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin4)%60;
			}
			else
			{
				CycleSwitchInputSrc(g_CycleInput1);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour1)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin1)%60;
			}
		}
	}
	else if( g_CycleInput4==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			if(g_CycleInput5!=INPUTSOURCE_NONE)
			{
				CycleSwitchInputSrc(g_CycleInput5);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour5)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin5)%60;
			}
			else
			{
				CycleSwitchInputSrc(g_CycleInput1);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour1)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin1)%60;
			}
		}
	}
	else if( g_CycleInput5==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			if(g_CycleInput6!=INPUTSOURCE_NONE)
			{
				CycleSwitchInputSrc(g_CycleInput6);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour6)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin6)%60;
			}
			else
			{
				CycleSwitchInputSrc(g_CycleInput1);
				g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour1)%24;
				g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin1)%60;
			}
		}
	}
	else if( g_CycleInput6==input)
	{
		if(g_CycleInputStartHour==hour&&g_CycleInputStartMin==min)
		{
			CycleSwitchInputSrc(g_CycleInput1);
			g_CycleInputStartHour=(g_CycleInputStartHour+g_CycleInputHour1)%24;
			g_CycleInputStartMin=(g_CycleInputStartMin+g_CycleInputMin1)%60;
		}
	}
}






U16 GetLogAddr()
{
	U16 addr;
	addr=E2ROM_Log_Start_ADDR+5*g_NumSaved;
	
	g_NumSaved++;
	Write_EEPROM_U16(E2ROM_Log_Num_ADDR, g_NumSaved);
	
	if(addr+5>8191)
	{
		addr=E2ROM_Log_Start_ADDR;
		g_NumSaved=0;
		Write_EEPROM_U16(E2ROM_Log_Num_ADDR, g_NumSaved);
	}
	return addr;
}

void ClearLog()
{
	I2C_EraserS_24C(E2ROM_Log_Start_ADDR, 8192-E2ROM_Log_Start_ADDR);
	g_NumSaved=0;
	Write_EEPROM_U16(E2ROM_Log_Num_ADDR, g_NumSaved);
	g_LogAddr=E2ROM_Log_Start_ADDR;
}


void Clear_24C()
{
	I2C_EraserS_24C(6, 8192);

}


BOOL ReadLog()
{
	U8 cch[5];
	U16 num=0;
	
	printf("============================================ \r\n ");//
	printf("============================================ \r\n ");//
	printf("============================================ \r\n ");//
	printf("开始读出开机记录 \r\n ");//
		
	while(num<=(8192-E2ROM_Log_Start_ADDR)/5)
	{
		I2C_ReadS_24C(E2ROM_Log_Start_ADDR+5*num,cch,5);
		/*
		printf("cch[0]=%d ",cch[0]);
		printf("cch[1]=%d ",cch[1]);
		printf("cch[2]=%d ",cch[2]);
		printf("cch[3]=%d ",cch[3]);
		printf("cch[4]=%d ",cch[4]);*/
		if(cch[0]==0xFF&&cch[1]==0xFF&&cch[2]==0xFF)
			break;
		num++;
		//delay(1);
		printf("  \n 第%d条是: 从%d年%d月%d日%d:%d 开机，运行%d时%d分。\n",num,(cch[0]>>4&0x0F)+2013,cch[0]&0x0F,cch[1]>>3&0x1F,
			(cch[1]&0x07)<<2|cch[2]>>6,cch[2]&0x3F,(cch[3]<<8|cch[4])/60,(cch[3]<<8|cch[4])%60);
		//delay(1);
		
	}
	
	//printf("存储的开始地址为:%d\r\n",E2ROM_Log_Start_ADDR);
	//printf("存储的地址为:%d\r\n",g_LogAddr);
	//delay(1);
	printf("最小的开机记录时间:%d 分钟\r\n",g_howlongtosave);
	//delay(1);
	printf("存储的记录条数为:%d条 \r\n",g_NumSaved);//
	printf("读取完成 \r\n");//
	//printf("存储的是否记录:%d \r\n",g_EnableRecordLog);
	if(num==0)
		return __FALSE;
	return __TRUE;
	
}



//此函数5分钟执行一次
void SaveLog(U16 min)
{
	U8 cch[5];
	
	cch[0]=(systmtimestart.tm_year-2013)<<4|systmtimestart.tm_mon;
	cch[1]=systmtimestart.tm_mday<<3|systmtimestart.tm_hour>>2;
	cch[2]=systmtimestart.tm_hour<<6|systmtimestart.tm_min;
	cch[3]=(min)>>8;
	cch[4]=(min)&0xFF;
	I2C_WriteS_24C(g_LogAddr, cch, 5);

}


void DealStartTime()
{
	static U32 startTime;
	U32 MinuteTime;
	static U8 once=0;
	static U8 day=0,hour=0,minute=0,second=0;
	second++;
	if(second==60)
	{
		second=0;
		minute++;
		if(minute==60)
		{
			minute=0;
			hour++;
			if(hour==24)
			{
				hour=0;
				day++;
				day%=100;
			}
		}
	}	
	if(g_CurrentState==State_MainMenu_Menu_StartInfo)
	{
		DisTwoNumValue(day,0x85);
		DisTwoNumValue(hour,0x90);			
		DisTwoNumValue(minute,0x92);		
		DisTwoNumValue(second,0x94);
	}

#ifdef POWER_ON_PWD
	//if(g_PowerOnPwdOk)
	if(g_EnableRecordLog)
	{//从输入开机密码后开始计时
		if(once==0)
		{
			once=1;
			startTime=RTC_GetCounter();
			to_tm(startTime, &systmtimestart);
		}
		MinuteTime=(RTC_GetCounter()-startTime)/60;
		
//		if(MinuteTime>=g_howlongtosave&&(MinuteTime-g_howlongtosave)%LogResolution_Min==0&&second==0&&MinuteTime<0x10000)   //因LogResolution_Min等于1，所以1分钟记录一次
		if(MinuteTime>=g_howlongtosave&& MinuteTime%g_howlongtosave==0 && second==0 && MinuteTime<0x10000)    //lyg 改为最小记录时间保存一次
		{
			if(once==1)
			{
				once=2;
				g_LogAddr=GetLogAddr();
				//printf("存储的地址为:%d\n",g_LogAddr);
				
			}
			SaveLog(MinuteTime);
		}
	}
#endif


/*	
#ifndef NOLOGO
	else if(g_AdvertisingFlag&&(second%3)==0&&!g_PCControlFlag)
	{
		static U8 AdvertiseIdx=0;
		clearLineFromAddr(0x80);
		if(AdvertiseIdx==0)
		{
			AdvertiseIdx=1;
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x80,g_BuyTeleStr);
			}
			else  if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,g_BuyTeleStrEn);
			}
		}
		else if(AdvertiseIdx==1)
		{
			AdvertiseIdx=0;
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x80,  g_WelcomeStr);
			}
			else  if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,  g_WelcomeStrEn);
			}
		}
	}
#endif	*/
}


/*
void CheckDeadline(U16 year,U8 mon,U8 day)
{
	if(year>g_TryEndDate[0]||(year==g_TryEndDate[0]&&mon>g_TryEndDate[1])
		||(year==g_TryEndDate[0]&&mon==g_TryEndDate[1])&&day>=g_TryEndDate[2])
		g_TryTimeEndFlag=__TRUE;
}
*/


/*
void LEDToggle()
{
	 LED_LAMP_GREEN_GPIO->ODR ^= LED_LAMP_GREEN_PIN;
}
*/









/*

void DealStartTime()
{
	static U8 day=0,hour=0,minute=0,second=0;
	second++;
	if(second==60)
	{
		second=0;
		minute++;
		if(minute==60)
		{
			minute=0;
			hour++;
			if(hour==24)
			{
				hour=0;
				day++;
				day%=100;
			}
		}
	}
	if(g_CurrentState==State_MainMenu_Menu_StartInfo)
	{
		DisTwoNumValue(day,0x85);
		DisTwoNumValue(hour,0x90);			
		DisTwoNumValue(minute,0x92);		
		DisTwoNumValue(second,0x94);
	}
}
*/

void More_LED_Flashing(U8 on)
{
	int i;
	U32 keynum=0;
	
	if(g_FlashLedFlag&&!g_PCControlFlag)
	{
		if(on)  //light on
		{
			for(i=0;i<g_FlashLedNum;i++)
			{
				keynum|=0x01<<g_FlashLedKey[i];
			}
			SetKeypadLED(~keynum);
		}
		else //if(LedCount==0)  //light off
		{	
			SetKeyLEDLight(KEY_NUM);
		}
	}
}

/*******************************************************************************************************
** 函数名称:  RtcTimeSnd
** 函数功能:  被中断函数RTC_IRQHandler调用，每隔一秒调用一次。
** 入口参数:  无
** 出口参数:  无
*******************************************************************************************************/
void RtcTimeSnd(void)
{
	int IntYear;
	U8 szMon,szDay;	
	U8 WeekNum,HourNum,MinNum,SecNum;	
	U8 IsWeekOk1=0;
	U8 IsWeekOk2=0;
	U8 IsWeekOk3=0;
	U8 idx=0;
	U32 time_num=RTC_GetCounter(); //20170213 lyg
	to_tm(time_num, &systmtime);
	DealStartTime();
	if(g_CurrentState==State_MainMenu_Menu&&g_RTCErrorFlag==0)
		DisplayCurTime();
	WeekNum   = systmtime.tm_wday;				// 获取 星期	
	if(WeekNum==0)
		WeekNum=7;	
	HourNum   = systmtime.tm_hour;				// 获取 小时	
	MinNum   = systmtime.tm_min;				// 获取 分钟
	SecNum   = systmtime.tm_sec;				// 获取 秒钟

#ifndef EC11_BUTTON
	//LED Flashing
	if((SecNum%2)==0)
	{
		GPIO_ResetBits(LED_LAMP_GREEN_GPIO,LED_LAMP_GREEN_PIN);
		More_LED_Flashing(1);
	}
	else
	{
		GPIO_SetBits(LED_LAMP_GREEN_GPIO,LED_LAMP_GREEN_PIN);
		More_LED_Flashing(0);
	}
#endif
	
	IntYear = systmtime.tm_year;                       // 获取 年
	szMon = systmtime.tm_mon;				// 获取 月
	szDay = systmtime.tm_mday;				// 获取 日
  if(g_TryEndDate[2]!=0xff&&g_TryEndDate[1]!=0xff) 
  {
		CheckDeadline(IntYear,szMon,szDay);
		idx=Check_Deadline_Days(IntYear,szMon,szDay);
		if(idx<=7)
			g_Deadline7DaysFlag=idx;
  }
		
	//20170213 lyg
	if((g_UserTry_168Demo_onoff ||g_UserTry_once_onoff)&& (g_UserTry_once_EndFlag==__FALSE))  //如果演示密码或者单次密码打开
	{
		if(time_num>g_UserTry_OnceEndTime )
		g_UserTry_once_EndFlag=__TRUE;
	}	
			 
	if(SecNum==0&&g_StartCycleInputFlag)
		CheckCycleInput(HourNum,MinNum);
	else
	{
		//通道方案1
		if(g_ScheduleInputMode1==ONCESCHE&&g_ScheduleInput1!=INPUTSOURCE_NONE)
		{
			if(g_InputTime1[0]==HourNum&&g_InputTime1[1]==MinNum&&SecNum==0)
			{
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput1);
				LightInputSrcLED();
				CancelInputScheduleMode1();
			}
		}
		else if(g_ScheduleInputMode1==MORESCHE&&g_ScheduleInput1!=INPUTSOURCE_NONE)
		{
			for(idx=0;idx<g_InputWeek1Idx;idx++)
			{
				if(WeekNum==g_InputWeek1[idx])
				{
					IsWeekOk1=1;
					break;
				}
			}
			if(g_InputTime1[0]==HourNum&&g_InputTime1[1]==MinNum&&SecNum==0&&IsWeekOk1==1)
			{
				IsWeekOk1=0;
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput1);
				LightInputSrcLED();
			}
		}
		//通道方案2
		if(g_ScheduleInputMode2==ONCESCHE&&g_ScheduleInput2!=INPUTSOURCE_NONE)
		{
			if(g_InputTime2[0]==HourNum&&g_InputTime2[1]==MinNum&&SecNum==0)
			{
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput2);
				LightInputSrcLED();			
				CancelInputScheduleMode2();
			}
		}
		else if(g_ScheduleInputMode2==MORESCHE&&g_ScheduleInput2!=INPUTSOURCE_NONE)
		{
			for(idx=0;idx<g_InputWeek2Idx;idx++)
			{
				if(WeekNum==g_InputWeek2[idx])
				{
					IsWeekOk2=1;
					break;
				}
			}
			if(g_InputTime2[0]==HourNum&&g_InputTime2[1]==MinNum&&SecNum==0&&IsWeekOk2==1)
			{
				IsWeekOk2=0;
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput2);
				LightInputSrcLED();
			}
		}
		//通道方案3
		if(g_ScheduleInputMode3==ONCESCHE&&g_ScheduleInput3!=INPUTSOURCE_NONE)
		{
			if(g_InputTime3[0]==HourNum&&g_InputTime3[1]==MinNum&&SecNum==0)
			{
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput3);
				LightInputSrcLED();			
				CancelInputScheduleMode3();
			}
		}
		else if(g_ScheduleInputMode3==MORESCHE&&g_ScheduleInput3!=INPUTSOURCE_NONE)
		{
			for(idx=0;idx<g_InputWeek3Idx;idx++)
			{
				if(WeekNum==g_InputWeek3[idx])
				{
					IsWeekOk3=1;
					break;
				}
			}
			if(g_InputTime3[0]==HourNum&&g_InputTime3[1]==MinNum&&SecNum==0&&IsWeekOk3==1)
			{
				IsWeekOk3=0;
				g_ActiveImageValue=MAIN;
				SetInputSourceSwitch(g_ScheduleInput3);
				LightInputSrcLED();
			}
		}
	}
}


//将时间计划的重复模式或执行一次，用一个字节来表示
//具体表示如下
//bit0=0     执行一次
//bit0=1    表示是重复执行
//bit1-bit7   如果为1表示周几存在
//例如bit4=1                 表示周四存在
//return one byte
U8 RepeatWeekToByte(U8 mode,U8 *week)
{
	U8 ch=0;
	U8 idx=0;
	if(mode==MORESCHE)
		ch|=_BIT0;
	else
		ch&=~_BIT0;
	for(idx=0;idx<7;idx++)
	{
		if(*week==1)
			ch|=_BIT1;
		else if(*week==2)
			ch|=_BIT2;
		else if(*week==3)
			ch|=_BIT3;
		else if(*week==4)
			ch|=_BIT4;
		else if(*week==5)
			ch|=_BIT5;
		else if(*week==6)
			ch|=_BIT6;
		else if(*week==7)
			ch|=_BIT7;
		else if(*week==0)
			break;
		week++;			
	}
	return ch;
}
	
void RTC_Init(void)	 
{
 	//set RTC 
	if (BKP_ReadBackupRegister(BKP_DR1)!= 0xA1A2)		  //  RTC not yet configured
	{// Backup data register value is not correct or not yet programmed (when the first time the program is executed) 
		RTC_Configuration();  // RTC Configuration 	
		if(g_TryEndDate[1]!=0xff&&g_TryEndDate[2]!=0xff)  //当设置为试用机器，不可以更改时间
			g_TryTimeEndFlag=__TRUE;
		Time_Adjust();		 // Adjust time  
		BKP_WriteBackupRegister(BKP_DR1, 0xA1A2);
	}
	else	 // No need to configure RTC
	{
//		/* Check if the Power On Reset flag is set */
//		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//		{
//		  printf("\r\n\n Power On Reset occurred....");
//		}
//		/* Check if the Pin Reset flag is set */
//		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//		{
//		  printf("\r\n\n External Reset occurred....");
//		}
//		printf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();		
		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE); 		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	} 	
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	  
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);   	
	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper	functionality must be disabled */
	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);   	
	/* Clear reset flags */
	RCC_ClearFlag();		
	/* Display time in infinite loop */
//	printf("\r\n\n WWW.ARMJISHU.COM  Display time in infinite loop....\r\n"); 
//	Time_Show();
	RTC_ClearITPendingBit(RTC_IT_SEC);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
  
void RTC_Configuration(void)
{
	int i=0;
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
	BKP_DeInit();
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	  {
	  	i++;
		if(i>0x80000000)
		{
			g_RTCErrorFlag=1;
			for(i=0;i<21;i++)
			{
				g_CurrentTimeStr[i]=g_WelcomeStr[i];
			}
			return;
		}
	  }	
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
void Time_Regulate(struct rtc_time *tm)
{
	tm->tm_year = TimeSet[0];
	tm->tm_mon = TimeSet[1];
	tm->tm_mday = TimeSet[2];
	tm->tm_hour = TimeSet[3];
	tm->tm_min = TimeSet[4];
	tm->tm_sec = TimeSet[5];
}
/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
	if(g_TryEndDate[2]!=0xff&&g_TryEndDate[1]!=0xff)  //当设置为试用机器，不可以更改时间
		return;
		//20170213 lyg
	if(g_UserTry_168Demo_onoff ||g_UserTry_once_onoff)  //如果演示密码或者单次密码打开，不可以更改时间
		return;
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Get time entred by the user on the hyperterminal */
	Time_Regulate(&systmtime);
	/* Get wday */
	GregorianDay(&systmtime);
	/* Change the current time */
	RTC_SetCounter(mktimev(&systmtime));
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}					
