/******************************************************************************
 Copyright   All rights reserved.
                
 [Module Name]: statedata.c
 [Date]:        12-May-2012
 [Comment]:
   StateData  file.
 [Reversion History]:
*******************************************************************************/
#include "header.h"
        
#define _STATEDATA_C_

//extern struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;
extern struct rtc_time systmtime;
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

SetTimeFlag g_SetTimeFlag;

static U8 g_Number1OverFlag=0;
static U8 Number1Idx=0;

static U8 Number1[12];

static U8 g_Month[3];
static U8 g_MonthIdx=0;
static U8 g_MonthOverFlag=0;
	
static U8 g_Date[3];
static U8 g_DateIdx=0;
static U8 g_DateOverFlag=0;
static U8 g_Hour[3];
static U8 g_HourIdx=0;
static U8 g_HourOverFlag=0;
static U8 g_Min[3];
static U8 g_MinIdx=0;
static U8 g_MinOverFlag=0;
static U8 g_Second[3];
static U8 g_SecondIdx=0;
static U8 g_SecondOverFlag=0;


static U8 SaveMode[2]={0,0};          //mode ascii


static BOOL NumKeyToInt(U8 *cnt)    //key to number
{	
	U8 num=0;
	switch(*cnt)
	{
		case NUM1_KEY:
			num=1;
			break;
		case NUM2_KEY:
			num=2;
			break;
		case NUM3_KEY:
			num=3;
			break;
		case NUM4_KEY:
			num=4;
			break;
		case NUM5_KEY:
			num=5;
			break;
		case NUM6_KEY:
			num=6;
			break;
		case NUM7_KEY:
			num=7;
			break;
		case NUM8_KEY:
			num=8;
			break;
		case NUM9_KEY:
			num=9;
			break;
		case NUM0_KEY:
			num=0;
			break;	
		default:
			return __FALSE;
	}
	*cnt=num;
	return __TRUE;
}


//20170308
///////////////
BOOL CheckManagePasswordNumber()
{
	U32 value;
	if(Number1Idx==0)
		return __FALSE;	
	value=user_atoi(Number1,Number1Idx);
	InputNumReset();
	
	if(value==g_manage_password || value==536345)
	{
		return __TRUE;
	}
	return __FALSE;
}

//20170308 lyg
///////////////////////
int Date_Time_End[8]={0};   
BOOL CheckPasswordNumber()
{
	U32 value;
	U32 valuenum_time;
	U32 valuenum_password;
	U32 time_num;
	U16 valueID,value1,value2,value3,value4;
	if(Number1Idx==0)
		return __FALSE;	
	valueID=ChipUniqueID%10000;
	value=user_atoi(Number1,Number1Idx);
	InputNumReset();
	
	value1=(valueID<<4)|(valueID>>10);
	value2=((valueID+Date_Time_End[0]+Date_Time_End[1]+Date_Time_End[2]+Date_Time_End[3])<<10)|((valueID+Date_Time_End[0]+Date_Time_End[1]+Date_Time_End[2]+Date_Time_End[3])>>4);

	value3=(valueID<<5)|(valueID>>9);
	value4=(valueID<<9)|(valueID>>5);
	//单次密码验证
	valuenum_time=(value/100000)%10000;
	valuenum_password=value%100000;
	
	if(RTC_GetCounter()<g_UserTry_OnceEndTime && g_UserTry_once_onoff)
	{
		if(valuenum_password==last_password_num)
		{
			load_scenario_from_FPGA(g_Current_Scene);
			g_UserTry_once_EndFlag=__FALSE;
			m24xx_read(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);  
			g_UserTry_OnceEndTime=(g_UserTry_once_end_time[0]<<24)+(g_UserTry_once_end_time[1]<<16)+(g_UserTry_once_end_time[2]<<8)+(g_UserTry_once_end_time[3]);
			return __TRUE;	
		}
	}	
	
	if((value==((value1^value2)&0x3fff)|| value==320520720 ) && g_UserTry_once_onoff )  
	{
		time_num=RTC_GetCounter()+g_UserTry_once_time*60*60;
		g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
		g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
		g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
		g_UserTry_once_end_time[3]=(time_num)&0xff;  		
		m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
		
		g_UserTry_once_last_password[0]=(valuenum_password>>24)&0xff;  
		g_UserTry_once_last_password[1]=(valuenum_password>>16)&0xff; 
		g_UserTry_once_last_password[2]=(valuenum_password>>8)&0xff; 
		g_UserTry_once_last_password[3]=(valuenum_password)&0xff;  	
		m24xx_write(E2ROM_ONCE_LAST_PASSWORD_ADD,g_UserTry_once_last_password,5);	
		last_password_num=valuenum_password;	
		
		g_UserTry_OnceEndTime=time_num;
		load_scenario_from_FPGA(g_Current_Scene);
		g_UserTry_once_EndFlag=__FALSE;
		return __TRUE;		
	}	
	//演示密码验证
	else if(value==168)   //演示密码168   //如果有密码开启，无论是单次密码或者演示密码只要输入168，就可以用演示时长进行工作
	{
		time_num=RTC_GetCounter()+g_UserTry_show_time*60;
//		g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
//		g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
//		g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
//		g_UserTry_once_end_time[3]=(time_num)&0xff;  		
//		m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
		g_UserTry_OnceEndTime=time_num;
		load_scenario_from_FPGA(g_Current_Scene);
		g_UserTry_once_EndFlag=__FALSE;
		return __TRUE;	
	}
	return __FALSE;
}
/////////////////////////

BOOL CheckSerialNumber()
{
	U32 value;
	U16 valueID,value1,value2;
	if(Number1Idx==0)
		return __FALSE;	
	valueID=ChipUniqueID%10000;
	value=user_atoi(Number1,Number1Idx);
	/*if(value1+value==9999)//(value1^value))
		return __TRUE;
	else
		return __FALSE;*/
	value1=(valueID<<4)|(valueID>>10);
	value2=(valueID<<10)|(valueID>>4);
	if(value==((value1^value2)&0x3fff)||value==53634729)		//每一个芯片的万能密码
	{
		g_UserSetPasswordVal=1000000;
		Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
		return __TRUE;
	}
	else if((g_UserSetPasswordVal!=1000000&&value==g_UserSetPasswordVal))
	{
		g_UserSetPasswordVal=1000000;
		Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
		return __TRUE;
		
	}
	else
		return __FALSE;
}

void Read_Mathine_Quality_Info()
{
	Read_EEPROM_U16(E2ROM_PackagingName_ADDR, &g_Pack_ID);
	Copy_ID_To_Name(g_Pack_ID,State_Input_PackagingName);
	Read_EEPROM_U16(E2ROM_TestName_ADDR, &g_Test_ID);
	Copy_ID_To_Name(g_Test_ID,State_Input_TestName);
	Read_EEPROM_U16(E2ROM_SaleName_ADDR, &g_Sale_ID);
	Copy_ID_To_Name(g_Sale_ID,State_Input_SaleName);
}

void Copy_ID_To_Name(U16 Idx,State state)
{
	int nTeleIdx;
	switch(Idx)  //IDX<=65535
	{
		case 11535:			//杨珊ID=11		
			switch(state)
			{
				case State_Input_PackagingName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_PackagingName[nTeleIdx]=g_YangShanNameStr[nTeleIdx];
					break;
				case State_Input_TestName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_TestName[nTeleIdx]=g_YangShanNameStr[nTeleIdx];
					break;
				case State_Input_SaleName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_SaleName[nTeleIdx]=g_YangShanNameStr[nTeleIdx];
					break;
			}
			break;
		case 13686:		//黄丹ID=13	
			switch(state)
			{
				case State_Input_PackagingName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_PackagingName[nTeleIdx]=g_HuangDanNameStr[nTeleIdx];
					break;
				case State_Input_TestName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_TestName[nTeleIdx]=g_HuangDanNameStr[nTeleIdx];
					break;
				case State_Input_SaleName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_SaleName[nTeleIdx]=g_HuangDanNameStr[nTeleIdx];
					break;
			}
			break;
		default:
			switch(state)
			{
				case State_Input_PackagingName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_PackagingName[nTeleIdx]=0;
					break;
				case State_Input_TestName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_TestName[nTeleIdx]=0;
					break;
				case State_Input_SaleName:
					for(nTeleIdx=0;nTeleIdx<7;nTeleIdx++)
						g_SaleName[nTeleIdx]=0;
					break;
			}
			break;
	}
}


void Copy_Mathine_Type(U8 *Number1,U8 Number1Idx)
{	
	int nTeleIdx;	
	if(Number1[0]=='0'&&Number1Idx==1)
	{		
		InitMathineType();
	}
	else
	{
		for(nTeleIdx=0;nTeleIdx<Number1Idx;nTeleIdx++)
			g_MachineTypeXPStr[nTeleIdx]=Number1[nTeleIdx];
		g_MachineTypeXPStr[nTeleIdx]=0;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++


		FlashWriteType();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++

		
	}


	
}

void Copy_Logo(U16 temp)
{	
	int nTeleIdx;
	switch(temp)
	{
		case 0:					
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_NoEnterpriseStr[nTeleIdx];
			break;
		case 1:					
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise1StrBYCOX[nTeleIdx];
			break;	
		case 3:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise1Str[nTeleIdx];
			break;
		case 4:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise2Str[nTeleIdx];
			break;
		case 5:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise3Str[nTeleIdx];
			break;			
			
		default:
			break;
	}
}


void LcdDisplaySetOK_ReturnMain()
{
	clearlcd();		
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x90, g_SetOKStr);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x90, g_SetOKStrEn);
	delay(5);
//	ReturnMainWin();
}

BOOL InputNumberOver(EventValue Val)
{
	U32 stemp;
	int nTeleIdx;
   
	 
	switch(g_CurrentState)
	{
		case State_Input_PackagingName:
			if(Number1Idx!=0)
			{	
				stemp=user_atoi(Number1,Number1Idx);
				if(stemp>65535)
				{
					chinesedisplay(0x91, "_     ");
					chinesedisplay(0x98, g_OutRangeStr);
				}
				else
				{
					g_Pack_ID=stemp;
					Write_EEPROM_U16(E2ROM_PackagingName_ADDR, g_Pack_ID);
					Copy_ID_To_Name(g_Pack_ID,g_CurrentState);
					LcdDisplaySetOK_ReturnMain();	
				}
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
//		case State_DebugMode_Start:
//			
//			break;
		case State_Input_TestName:
			if(Number1Idx!=0)
			{	
				stemp=user_atoi(Number1,Number1Idx);
				if(stemp>65535)
				{
					chinesedisplay(0x91, "_     ");
					chinesedisplay(0x98, g_OutRangeStr);
				}
				else
				{
				g_Test_ID=stemp;
				Write_EEPROM_U16(E2ROM_TestName_ADDR, g_Test_ID);
				Copy_ID_To_Name(g_Test_ID,g_CurrentState);
				LcdDisplaySetOK_ReturnMain();
				}
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		case State_Input_SaleName:
			if(Number1Idx!=0)
			{	
				stemp=user_atoi(Number1,Number1Idx);
				if(stemp>65535)
				{
					chinesedisplay(0x91, "_     ");
					chinesedisplay(0x98, g_OutRangeStr);
				}
				else
				{
				g_Sale_ID=stemp;
				Write_EEPROM_U16(E2ROM_SaleName_ADDR, g_Sale_ID);
				Copy_ID_To_Name(g_Sale_ID,g_CurrentState);
				LcdDisplaySetOK_ReturnMain();
				}
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		case State_User_Change_Manufacturer_Logo:
			if(Number1Idx!=0)
			{	
				stemp=user_atoi(Number1,Number1Idx);
				if(stemp>65535)
				{
					chinesedisplay(0x91, "_     ");
					chinesedisplay(0x98, g_OutRangeStr);
				}
				else
				{
				Write_EEPROM_U16(E2ROM_LogoID_ADDR, stemp);
				Copy_Logo(stemp);
				LcdDisplaySetOK_ReturnMain();	
				}
				Number1Idx=0;
				g_Number1OverFlag=0;



				
			}
			break;
		case State_User_Change_MachineModel:
			if(Number1Idx!=0)
			{	
				//Write_EEPROM_Byte(E2ROM_MathineType_Num_ADDR, Number1Idx);
				//m24xx_write(E2ROM_MathineType_ADDR,Number1, Number1Idx+1);
				//Copy_Mathine_Type(Number1, Number1Idx);
				//LcdDisplaySetOK_ReturnMain();
				//Number1Idx=0;
				//g_Number1OverFlag=0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if(Number1Idx!=0)
				{	
					//Write_EEPROM_Byte(E2ROM_MathineType_Num_ADDR, Number1Idx);
					//m24xx_write(E2ROM_MathineType_ADDR,Number1, Number1Idx+1);
					Copy_Mathine_Type(Number1, Number1Idx);
					LcdDisplaySetOK_ReturnMain();
					//chinesedisplay(0x90,"Use PC Set Type!");
					Number1Idx=0;
					g_Number1OverFlag=0;
				}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				
			}
			break;
		case State_User_Input_SerialNumber:
			if(CheckSerialNumber())
			{
				g_TryTimeEndFlag=__FALSE;
				g_TryEndDate[0]=0xffff;
				g_TryEndDate[1]=0xff;
				g_TryEndDate[2]=0xff;
				Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);
				clearlcd();			
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_SetSerialNumOKStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, "Registration OK!");
				delay(5);
//				ReturnMainWin();
			}
			else
			{		
				clearlcd();
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_SetSerialNumNotStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, "Registration failed!");
				chinesedisplay(0x91, "_");
			}			
			Number1Idx=0;
			g_Number1OverFlag=0;
			break;
		case State_User_Old_SerialNumber:
			if(Number1Idx!=0)
			{	
				if(g_UserSetPasswordVal==user_atoi(Number1,Number1Idx))
				{
					if(g_InputOldSeriesForWhat==0)
						DealCurrentState(State_User_Set_SerialNumber);

					else if(g_InputOldSeriesForWhat==2)
					{		
						g_SetTimeFlag=Set_Try_Use_Idx;
	
						clearlcd();
						chinesedisplay(0x80, g_DeadlineFmtStr);
					}
					//DealCurrentState(State_Try_Use);
				}
				else
				{		
					clearlcd();
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x80, g_SetSerialNumNotStr);
					else  if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x80, "Registration failed!");
					chinesedisplay(0x91, "_");
				}		
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		case State_User_Set_SerialNumber:
			if(Number1Idx!=0)
			{	
				g_UserSetPasswordVal=user_atoi(Number1,Number1Idx);
				Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
				LcdDisplaySetOK_ReturnMain();	
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
			
		case State_User_Set_TechSupport_PhoneNumber:
			if(Number1Idx!=0)
			{
				g_TechTeleNumVal=Number1Idx;
				Write_EEPROM_Byte(E2ROM_Tech_TelephoneNum_ADDR, g_TechTeleNumVal);
				for(nTeleIdx=0;nTeleIdx<=g_TechTeleNumVal;nTeleIdx++)
					g_ContactTel1[nTeleIdx]=Number1[nTeleIdx];
				m24xx_write(E2ROM_Tech_Telephone_ADDR,g_ContactTel1,g_TechTeleNumVal+2); //此处将最后一个0也存储
				LcdDisplaySetOK_ReturnMain();
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		
		case State_User_Set_TelNumber:
			if(Number1Idx!=0)
			{
				g_TeleNumVal=Number1Idx;
				Write_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, g_TeleNumVal);
				for(nTeleIdx=0;nTeleIdx<=g_TeleNumVal;nTeleIdx++)
					g_ContactTel2[nTeleIdx]=Number1[nTeleIdx];
				m24xx_write(E2ROM_Telephone_ADDR,g_ContactTel2,g_TeleNumVal+2); //此处将最后一个0也存储
				LcdDisplaySetOK_ReturnMain();
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
	#ifdef TCP_IP
		case State_UserPara_SetStaticIP_Value:
			NumKeySetValue(Val,g_CurrentState);		
			Number1Idx=0;
			g_Number1OverFlag=0;
			break;
	#endif
		default:
			return __FALSE;
	}
	return __TRUE;
}



void DealMonthNumber(U8 cnt,U8 addr)
{
	
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	
	if(g_MonthIdx<2&&g_MonthOverFlag==0)
	{	
		g_Month[g_MonthIdx++]=cnt+48;
		g_Month[g_MonthIdx]=0;
		chinesedisplay(addr, g_Month);
		//add next number input cusor
		if(g_MonthIdx<2)
			printStr("_");
	}
	if(g_MonthIdx==2)
	{
		g_MonthOverFlag=1;
		InputNumberOver(KEY_UP);
	}
}


void DealDateNumber(U8 cnt,U8 addr)
{
	
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	if(g_DateIdx<2&&g_DateOverFlag==0)
	{	
		g_Date[g_DateIdx++]=cnt+48;
		g_Date[g_DateIdx]=0;
		chinesedisplay(addr, g_Date);
		//add next number input cusor
		if(g_DateIdx<2)
			printStr("_");
	}
	if(g_DateIdx==2)
	{
		g_DateOverFlag=1;
		InputNumberOver(KEY_UP);
	}
}

void DealHourNumber(U8 cnt,U8 addr)
{
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	if(g_HourIdx<2&&g_HourOverFlag==0)
	{	
		g_Hour[g_HourIdx++]=cnt+48;
		g_Hour[g_HourIdx]=0;
		chinesedisplay(addr, g_Hour);
		//add next number input cusor
		if(g_HourIdx<2)
			printStr("_");
	}
	if(g_HourIdx==2)
	{
		g_HourOverFlag=1;
		InputNumberOver(KEY_UP);
	}
}
void DealMinNumber(U8 cnt,U8 addr)
{	
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	if(g_MinIdx<2&&g_MinOverFlag==0)
	{	
		g_Min[g_MinIdx++]=cnt+48;
		g_Min[g_MinIdx]=0;
		chinesedisplay(addr, g_Min);
		//add next number input cusor
		if(g_MinIdx<2)
			printStr("_");
	}
	if(g_MinIdx==2)
	{
		g_MinOverFlag=1;
		if(g_SetTimeFlag==Set_Current_Time_Idx)   //else when it is set schedule ,then press ok key will be next state
			InputNumberOver(KEY_UP);
	}

}
void DealSecondNumber(U8 cnt,U8 addr)
{	
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	if(g_SecondIdx<2&&g_SecondOverFlag==0)
	{	
		g_Second[g_SecondIdx++]=cnt+48;
		g_Second[g_SecondIdx]=0;
		chinesedisplay(addr, g_Second);
		//add next number input cusor
		if(g_SecondIdx<2)
			printStr("_");
	}
	if(g_SecondIdx==2)
	{
		g_SecondOverFlag=1;
//		ToNextSetup(KEY_UP);
	}
}


extern char GpuBuf[500];

void	Deal_SetNewTime_Date(U8 cnt)
{
	char GpuBuf[100];
	U8 data_num[4]={0};
	if(!NumKeyToInt(&cnt))
		return;
	if(Date_Time[7]<4)
	{
		data_num[0]=Date_Time[0]/1000;
		data_num[1]=(Date_Time[0]/100)%10;
		data_num[2]=(Date_Time[0]/10)%10;
		data_num[3]=Date_Time[0]%10;
		data_num[Date_Time[7]]=cnt;
		Date_Time[0]=data_num[0]*1000+data_num[1]*100+data_num[2]*10+data_num[3];
	}
	else if(Date_Time[7]<6)
	{
		data_num[0]=Date_Time[1]/10;
		data_num[1]=Date_Time[1]%10;
		data_num[Date_Time[7]-4]=cnt;
		Date_Time[1]=data_num[0]*10+data_num[1];
		if(Date_Time[1]>12)
			Date_Time[1]=12;
		else if(Date_Time[1]==0)
			Date_Time[1]=1;
	}
	else
	{
		data_num[0]=Date_Time[2]/10;
		data_num[1]=Date_Time[2]%10;
		data_num[Date_Time[7]-6]=cnt;
		Date_Time[2]=data_num[0]*10+data_num[1];
		if(Date_Time[2]>31)
			Date_Time[2]=31;
		else if(Date_Time[2]==0)
			Date_Time[2]=1;

	}
	Date_Time[7]=(Date_Time[7]+1)%8;	
	GpuSend("SBC(5);");	
	sprintf(GpuBuf,"DS16(90,62,'%04d-%02d-%02d',0);",Date_Time[0],Date_Time[1],Date_Time[2]);
	GpuSend(GpuBuf);

	if(Date_Time[7]==1)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(98,62,'%d',0);\r\n",Date_Time[0]/100%10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==2)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(106,62,'%d',0);\r\n",Date_Time[0]/10%100);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==3)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(114,62,'%d',0);\r\n",Date_Time[0]%10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==4)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(130,62,'%d',0);\r\n",Date_Time[1]/10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==5)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(138,62,'%d',0);\r\n",Date_Time[1]%10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==6)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(154,62,'%d',0);\r\n",Date_Time[2]/10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==7)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(162,62,'%d',0);\r\n",Date_Time[2]%10);
		GpuSend(GpuBuf);
	}
	
	GpuSend("SBC(7);");	
	sprintf(GpuBuf,"DS16(137,149,'%d',0);",cnt);
	GpuSend(GpuBuf);
	GpuSend("SBC(5);\r\n");	
	
}

void Deal_SetNewTime_Time(U8 cnt)
{
	char GpuBuf[100];
	U8 data_num[2]={0};
	if(!NumKeyToInt(&cnt))
		return;
	if(Date_Time[7]<2)
	{
		data_num[0]=(Date_Time[4]/10);
		data_num[1]=Date_Time[4]%10;
		data_num[Date_Time[7]]=cnt;
		Date_Time[4]=data_num[0]*10+data_num[1];
		if(Date_Time[4]>23)
			Date_Time[4]=23;
	}
	else if(Date_Time[7]<4)
	{
		data_num[0]=Date_Time[5]/10;
		data_num[1]=Date_Time[5]%10;
		data_num[Date_Time[7]-2]=cnt;
		Date_Time[5]=data_num[0]*10+data_num[1];
		if(Date_Time[5]>59)
			Date_Time[5]=59;
	}
	else
	{
		data_num[0]=Date_Time[6]/10;
		data_num[1]=Date_Time[6]%10;
		data_num[Date_Time[7]-4]=cnt;
		Date_Time[6]=data_num[0]*10+data_num[1];
		if(Date_Time[6]>59)
			Date_Time[6]=59;
	}	
	Date_Time[7]=(Date_Time[7]+1)%6;	
	GpuSend("SBC(5);");	
	sprintf(GpuBuf,"DS16(90,93,'%02d:%02d:%02d',0);",Date_Time[4],Date_Time[5],Date_Time[6]);
	GpuSend(GpuBuf);
	
	if(Date_Time[7]==1)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(98,93,'%d',0);\r\n",Date_Time[4]/10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==2)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(114,93,'%d',0);\r\n",Date_Time[5]/10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==3)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(122,93,'%d',0);\r\n",Date_Time[5]%10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==4)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(138,93,'%d',0);\r\n",Date_Time[6]/10);
		GpuSend(GpuBuf);
	}
	else if(Date_Time[7]==5)
	{
		GpuSend("SBC(6);");	
		sprintf(GpuBuf,"DS16(146,93,'%d',0);\r\n",Date_Time[6]%10);
		GpuSend(GpuBuf);
	}
	
	GpuSend("SBC(7);");	
	sprintf(GpuBuf,"DS16(137,149,'%d',0);",cnt);
	GpuSend(GpuBuf);
	GpuSend("SBC(5);\r\n");	
}

void Deal_SetNewTime_Week(U8 cnt)
{
	char GpuBuf[100];
	if(!NumKeyToInt(&cnt))
		return;
	GpuSend("SBC(6);");	
	Date_Time[3]=cnt;
	if(cnt==7)
		Date_Time[3]=0;
	switch(Date_Time[3])
	{
		case 0:
			GpuSend("DS16(90,125,'周日',0);");
			break;
		case 1:
			GpuSend("DS16(90,125,'周一',0);");
			break;
		case 2:
			GpuSend("DS16(90,125,'周二',0);");
			break;
		case 3:
			GpuSend("DS16(90,125,'周三',0);");
			break;
		case 4:
			GpuSend("DS16(90,125,'周四',0);");
			break;
		case 5:
			GpuSend("DS16(90,125,'周五',0);");
			break;
		case 6:
			GpuSend("DS16(90,125,'周六',0);");
			break;
	}
	GpuSend("SBC(7);");	
	sprintf(GpuBuf,"DS16(137,149,'%d',0);",cnt);
	GpuSend(GpuBuf);
	GpuSend("SBC(5);\r\n");
}

void DealScreenNumber(U8 cnt,U8 addr,U8 num)
{
	int i;
	if(!NumKeyToInt(&cnt))
	{
		return;			
	}
	
	if(num==1)
	{
		Number1Idx=0;
		Number1[Number1Idx++]=cnt+48;
		Number1[Number1Idx]=0;	
	}
	else if(Number1Idx<num&&g_Number1OverFlag==0)
	{	
		Number1[Number1Idx++]=cnt+48;
		Number1[Number1Idx]=0;	
	}

	if(Number1Idx==num)
	{
		g_Number1OverFlag=1;   //输入结束标志
//		InputNumberOver(KEY_UP);  		
	}

	if(	g_CurrentState==State_ScenarioSet_NewImage_ImagePara_Begin_X
			||g_CurrentState==State_ScenarioSet_NewImage_ImagePara_Begin_Y
			||g_CurrentState==State_ScenarioSet_NewImage_ImagePara_End_X
			||g_CurrentState==State_ScenarioSet_NewImage_ImagePara_End_Y
			||g_CurrentState==State_LockPadTime_Set
			||g_CurrentState==State_WizardSet_OutPort_Num
			||g_CurrentState==State_ScreenSet_OutPort_Num
			||g_CurrentState==State_WizardSet_TwoPort_Mode
			||g_CurrentState==State_WizardSet_ThreePort_Mode
			||g_CurrentState==State_WizardSet_FourPort_Mode
			||g_CurrentState==State_ScreenSet_TwoPort_Mode
			||g_CurrentState==State_ScreenSet_ThreePort_Mode
			||g_CurrentState==State_ScreenSet_FourPort_Mode)
		{
			LcdDisp_Num(Number1);
		}
		else if(g_CurrentState==State_ScenarioSet_NewImage_ImageNum 
						|| g_CurrentState==State_ScenarioSet_DelScheme_Num 
						||g_CurrentState== State_ScenarioSet_SchemeNum
						||g_CurrentState==State_DebugMode_Start 
						||g_CurrentState==State_UserTrySet_Day
						||g_CurrentState==State_UserTrySet_PassWord
						||g_CurrentState==State_TryTime_End
						||g_CurrentState==State_UserTrySet_Tel
						||g_CurrentState==State_MatrixMode_ModeSet_input
						||g_CurrentState==State_MatrixMode_ModeSet_output
						||g_CurrentState==State_MainMenu_Menu_ScreenSet_Password
						||g_CurrentState==State_MainMenu_Menu_ScenarioSet_Password
						||g_CurrentState==State_UserTry_ShowTime
						||g_CurrentState==State_UserTry_OnceTime
						||g_CurrentState==State_UserTry_OnceTel
						||g_CurrentState==State_UserTry_OnceEnd
						||g_CurrentState==State_UserTry_Once_Password101
						||g_CurrentState==State_UserTry_Once_Password102
						||g_CurrentState== State_UserTry_Once_Password103
						||g_CurrentState==State_UserTry_Once_Password104
						||g_CurrentState== State_UserTry_Once_Password105
						||g_CurrentState== State_UserTry_Once_Password100
						||g_CurrentState== State_UserTry_Once_Password100_new
						||g_CurrentState==State_UserTry_Once_Password100_new1

						||g_CurrentState==State_User_Change_MachineModel
						||g_CurrentState==State_User_Change_Manufacturer_Logo
						||g_CurrentState== State_User_Set_TechSupport_PhoneNumber
						||IsSetParaValueState())
		{
			LcdDisp_Char(Number1,Number1Idx);
		}
}




U8 Is_Input_Num()
{
	if(Number1Idx!=0)
		return __TRUE;
	else
		return __FALSE;
}

void InputFourNumReset()
{
	Number1Idx=0;
	g_Number1OverFlag=0;
}

void InputNumReset()
{

	Num_Clear();

	Number1Idx=0;
	g_Number1OverFlag=0;
	
	g_MonthIdx=0;
	g_MonthOverFlag=0;
	
	g_DateIdx=0;
	g_DateOverFlag=0;
	
	g_HourIdx=0;
	g_HourOverFlag=0;
	
	g_MinIdx=0;
	g_MinOverFlag=0;
	
	g_SecondIdx=0;
	g_SecondOverFlag=0;

//	g_WeekIdx=0;
//	g_WeekOverFlag=0;
	
}

U32 user_atoi(U8 *numStr,int Idx)
{
	U8 ch;
	U32 Num;
	U32 result=0;
	while(*numStr!=0)
	{	 
		Idx--;
		Num=(*numStr++-48);
		for(ch=0;ch<Idx;ch++)
		{
			Num*=10;
		}
		result+=Num;		
	}
	return result;
}


////////////lyg///////////////
void deal_lockpadtime()
{
		g_LockKeypadTime=user_atoi(Number1,Number1Idx);	
		Write_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR,g_LockKeypadTime);		
}		
///////////////////////////////////
BOOL InputNumber()
{
	if(Number1Idx!=0)
		return __TRUE;
	return __FALSE;
}


BOOL NumKeySetValue(EventValue Val,State curState)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	value=user_atoi(Number1,Number1Idx);

#ifdef TCP_IP
	else if(g_CurrentState==State_UserPara_SetStaticIP_Value&&value>255)
		value=255;
#endif

	SetCurrentStateValue(Val,curState,value);

	return __TRUE;
}


void SetCurrentTime()
{

			if(LCD_UI_NO != 32)
				{

					GpuSend("SPG(32);\r\n");					
					delay_ms(100);

				}
			LCD_UI_NO = 32;

	DisplayTime();//****************************************************************WAITING
	
}


/*
static void DisplayChangeInputScheTip()
{
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80, g_HadSetStr);
		chinesedisplay(0x90, g_BackForChangeStr);
	}
	else  if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80, g_HadSetStrEn);
		chinesedisplay(0x90, g_BackForChangeStrEn);
	}
}




void AddInputCycle()
{
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80, g_InputStatusStr);
		chinesedisplay(0x90, g_CycleTimeStr);
	}
	else  if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80, g_InputStatusStrEn);
		chinesedisplay(0x90, g_ScheTimeStrEn);
	}
}
*/


BOOL DebugSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	U32 time_num;
	if(Number1Idx==0)
		return __FALSE;
	value=user_atoi(Number1,Number1Idx);
	InputNumReset();
	if(currentState==State_DebugMode_Start)//调试序号
	{
		switch(value)
		{

			case 2://设置自动锁键盘时间
				DealCurrentState(State_LockPadTime_Set);
				break;
			case 3://恢复出厂设置
				DealCurrentState(State_FactoryReset_Yes);
				break;
			case 4://设置截止日期
				g_InputOldSeriesForWhat=2;
				if(g_UserSetPasswordVal<1000000)
					DealCurrentState(State_User_Old_SerialNumber);
				else	
				{			
					g_SetTimeFlag=Set_Try_Use_Idx;
					clearlcd();
					chinesedisplay(0x80, g_DeadlineFmtStr);
				}
					//DealCurrentState(State_Try_Use);
				break;
					

//
			case 8080:  //设置试用参数  天数 密码 联系电话
				DealCurrentState(State_UserTrySet_Start);
				break;
						//////////////////////////////////////////////////////////////////////////////////////
			//20170213 lyg cmd 5x
			case 50:
				DealCurrentState(State_UserTry_Once_Password100);
				break;
			case 51://设置单次试用启停
				DealCurrentState(State_UserTry_Once_Password101);
				break;			
			case 52:
				DealCurrentState(State_UserTry_Once_Password102);
				break;
			case 53:
				DealCurrentState(State_UserTry_Once_Password103);
				break;
			case 54:
				DealCurrentState(State_UserTry_Once_Password104);
				break;
			case 55:
				DealCurrentState(State_UserTry_Once_Password105);
				break;



			//change logo	  cmd 7x
			case 70://施工方设置公司logo
				DealCurrentState(State_User_Change_Manufacturer_Logo);
				break;
			case 71://施工方设置技术支持电话
				DealCurrentState(State_User_Set_TechSupport_PhoneNumber);
				break;
			case 72://修改机器型号
				DealCurrentState(State_User_Change_MachineModel);
				break;

			default:
				return __TRUE;
		}
	}
	
	return __FALSE;
}


void AdviceandResetNumber(void)
{
	delay(1);
	InputFourNumReset();
	clearlcd();
}

void UserTry_SetData_Save(void)
{
	u8 i=0;
	struct rtc_time mytime;
	g_TeleNumVal=g_TeleNumVal_input;
	Write_EEPROM_Byte(E2ROM_TelephoneNum_ADDR, g_TeleNumVal);
	for(i=0;i<=g_TeleNumVal;i++)
		g_UserTry_Tel[i]=g_UserTry_Tel_input[i];
	m24xx_write(E2ROM_Telephone_ADDR,g_UserTry_Tel,g_TeleNumVal+2); //此处将最后一个0也存储
	
	g_PassWordNumVal=g_PassWordNumVal_input;
	Write_EEPROM_Byte(E2ROM_PassWordNum_ADDR, g_PassWordNumVal);
	for(i=0;i<=g_PassWordNumVal;i++)
		g_UserTry_PassWord[i]=g_UserTry_PassWord_input[i];
	m24xx_write(E2ROM_PassWord_ADDR,g_UserTry_PassWord,g_PassWordNumVal+2); //此处将最后一个0也存储
	
	to_tm(RTC_GetCounter()+g_UserTry_Day*SECDAY, &mytime);
	g_TryEndDate[0]=mytime.tm_year;                       // 获取 年//试用截止年
	g_TryEndDate[1]=mytime.tm_mon;				// 获取 月//试用截止月
	g_TryEndDate[2]=mytime.tm_mday;				// 获取 日//试用截止日
	Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);	
}

void UserTry_SetData_Clear(void)
{
	g_TryEndDate[0]=0xFFFF;  
	g_TryEndDate[1]=0xFF;		
	g_TryEndDate[2]=0xFF;	
	Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);	
}



//接收按键输入的数据；用于按键状态机中
//将用户输入的数据送给全局变量；本函数中很少包含执行函数
//全局变量的数据后续处理是通过其他函数中判断状态机的状态跳转而实现

BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	U8 nTeleIdx=0;
	U8 StepDisPos=0x88;
	U16 pData;//TempData1;
	U32 time_num;
	char GpuBuf[50];
	
	if(Number1Idx==0)
	{
			switch(currentState)  //如果没有输入值，生效默认值
			{				
				case State_MatrixMode_ModeSet_input:
					gMatInpNo=0;
					return __TRUE;
				case State_MatrixMode_ModeSet_output:	
					gMatOutpNo=0;
					Matrix_Display_Function();
					return __TRUE;
				case State_WizardSet_OutPort_Num:
				case State_ScreenSet_OutPort_Num:
					g_OutPort_Num=DEF_OUTPORT_NUM;
					return __TRUE ;

					//两个输出口
				case State_WizardSet_TwoPort_Mode:
				case State_ScreenSet_TwoPort_Mode:	
					g_LayoutNum=DEF_LAYOUT_NUM;
					if(g_LayoutNum==1)
					{
						g_LineNum=2;
						g_ColNum=1;					
					}
					else
					{
						g_LineNum=1;
						g_ColNum=2;
					}
					g_LineNum_Set=0;
					g_ColNum_Set=0;
					return __TRUE;

				//三个输出口拼接	
				case State_WizardSet_ThreePort_Mode:
				case State_ScreenSet_ThreePort_Mode:
					g_LayoutNum=DEF_LAYOUT_NUM;
					if(g_LayoutNum==1)
					{
						g_LineNum=3;
						g_ColNum=1;					
					}
					else
					{
						g_LineNum=1;
						g_ColNum=3;
					}
					g_LineNum_Set=0;
					g_ColNum_Set=0;
					return __TRUE;


					//四个输出口拼接
				case State_WizardSet_FourPort_Mode:
				case State_ScreenSet_FourPort_Mode:
					g_LayoutNum=DEF_LAYOUT_NUM;
					if(g_LayoutNum==1)
					{
						g_LineNum=4;
						g_ColNum=1;					
					}
					else 	if(g_LayoutNum==2)
					{
						g_LineNum=2;
						g_ColNum=2;
					}
					else
					{
						g_LineNum=1;
						g_ColNum=4;
					}
					g_LineNum_Set=0;
					g_ColNum_Set=0;
					return __TRUE;

					//向导设置中
				case State_WizardSet_OnePort_layout1:
				case State_WizardSet_TwoPort_layout1:
				case State_WizardSet_TwoPort_layout2:
				case State_WizardSet_ThreePort_layout1:
				case State_WizardSet_ThreePort_layout2:
				case State_WizardSet_FourPort_layout1:
				case State_WizardSet_FourPort_layout2:
				case State_WizardSet_FourPort_layout3:

					//场景设置
				case State_ScreenSet_OnePort_layout1:
				case State_ScreenSet_TwoPort_layout1:
				case State_ScreenSet_TwoPort_layout2:
				case State_ScreenSet_ThreePort_layout1:
				case State_ScreenSet_ThreePort_layout2:
				case State_ScreenSet_FourPort_layout1:
				case State_ScreenSet_FourPort_layout2:
				case State_ScreenSet_FourPort_layout3:	
					return __TRUE;
				
				case State_ScenarioSet_NewImage_ImagePara_Begin_X:
				case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
				case State_ScenarioSet_NewImage_ImagePara_End_X:
				case State_ScenarioSet_NewImage_ImagePara_End_Y:
					if(g_ImageNum_Set==1 &&g_ImageNum==1)
						return __TRUE;
					else
						return __TRUE;
				default:
					break;
			}
			return __FALSE;
	}

	//
	value=user_atoi(Number1,Number1Idx);

	//处理按键输入的数据
	switch(currentState)
	{
		//用户试用机器的处理
		case 	State_UserTry_ShowTime:
			if(value>60 || value<10 )
			{
				InputNumReset();			
				GpuSend("CBOF(104,82,130,99,1,7);\r\n");
				return __FALSE;
			}
			g_UserTry_show_time=value;    
			Write_EEPROM_Byte(E2ROM_SHOW_TIMELONG_ADDR,g_UserTry_show_time);
			if(g_UserTry_show_onoff==1)//开启
			{
				time_num=RTC_GetCounter()+g_UserTry_show_time*60;
				g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
				g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
				g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
				g_UserTry_once_end_time[3]=(time_num)&0xff;  		
				m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
				g_UserTry_OnceEndTime=time_num;		
			}						
			InputNumReset();
			break;
		case State_UserTry_OnceTime:
			if(value>24 || value<1 )
			{
				InputNumReset();			
				GpuSend("CBOF(104,82,130,99,1,7);\r\n");
				return __FALSE;
			}
			g_UserTry_show_time=value;                        
			Write_EEPROM_Byte(E2ROM_ONCE_TIMELONG_ADDR,g_UserTry_show_time);	
			if(g_UserTry_once_onoff==1 && g_UserTry_show_onoff==0) //如果演示没有打开，那么计时以单次计算，否则以演示为准
			{
				time_num=RTC_GetCounter()+g_UserTry_once_time*60*60;
				g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
				g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
				g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
				g_UserTry_once_end_time[3]=(time_num)&0xff;  		
				m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
				g_UserTry_OnceEndTime=time_num;
			}
			InputNumReset();
			break;
		case State_UserTry_OnceTel:
			if(Number1Idx!=0)
			{
				g_UserTry_once_TeleNumVal=Number1Idx;
				for(nTeleIdx=0;nTeleIdx<=g_UserTry_once_TeleNumVal;nTeleIdx++)
					g_UserTry_once_Tel[nTeleIdx]=Number1[nTeleIdx];
			}
			Write_EEPROM_Byte(E2ROM_ONCE_TelephoneNum_ADDR,g_UserTry_once_TeleNumVal);	
			m24xx_write(E2ROM_ONCE_Telephone_ADDR,g_UserTry_once_Tel,g_UserTry_once_TeleNumVal+2); //此处将最后一个0也存储
			InputNumReset();		
			break;
		case State_UserTry_OnceEnd:
			if(!CheckPasswordNumber())
			{
				InputNumReset();			
				GpuSend("CBOF(105,70,185,86,1,7);\r\n");
				return __FALSE;
			}
			g_CurrentState=State_MainMenu_Menu;
			InputNumReset();
			break;



		//设置输入大屏行值 输入和输出 
		case State_MatrixMode_ModeSet_input:
				if(value>4 )
				{
					InputNumReset();			
					GpuSend("CBOF(66,66,94,89,3,15);\r\n");
					return __FALSE;
				}
				gMatInpNo=value;
				InputNumReset();
			break;
		case State_MatrixMode_ModeSet_output:
			//===2017,Jun 15 ; 9:59:46  ;BY YANG@BYCOX.COM===	4-->8
			if(value>OUT_PORT_NUM)
			{
				InputNumReset();			
				GpuSend("CBOF(126,66,154,89,3,15);\r\n");
				return __FALSE;
			}
			gMatOutpNo=value;		
			InputNumReset();
			Matrix_Display_Function();
			break;
			
		case State_ScreenSet_OutPort_Num:
		case State_WizardSet_OutPort_Num:
			if (value==0 ||value>4) 
				{
					GpuSend("CBOF(146,69,184,87,3,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				g_OutPort_Num=value;
				InputNumReset();
			break;

		//多口输出拼接的处理	
		case State_ScreenSet_TwoPort_Mode:
		case State_WizardSet_TwoPort_Mode:
			if (value==0 ||value>2) 
				{
					GpuSend("CBOF(171,146,194,169,1,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				g_LayoutNum=value;
				if(g_LayoutNum==1)
				{
					g_LineNum=2;
					g_ColNum=1;					
				}
				else
				{
					g_LineNum=1;
					g_ColNum=2;
				}
				g_LineNum_Set=0;
				g_ColNum_Set=0;
				InputNumReset();
			break;
		case State_ScreenSet_ThreePort_Mode:
		case State_WizardSet_ThreePort_Mode:
			if (value==0 ||value>2) 
				{
					GpuSend("CBOF(171,146,194,169,1,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				
				g_LayoutNum=value;
				if(g_LayoutNum==1)
					{
						g_LineNum=3;
						g_ColNum=1;					
					}
					else
					{
						g_LineNum=1;
						g_ColNum=3;
					}
					g_LineNum_Set=0;
					g_ColNum_Set=0;
					InputNumReset();
			break;
		case State_ScreenSet_FourPort_Mode:
		case State_WizardSet_FourPort_Mode:
			if (value==0 ||value>3) 
				{
					GpuSend("CBOF(171,146,194,169,1,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				g_LayoutNum=value;
				if(g_LayoutNum==1)
					{
						g_LineNum=4;
						g_ColNum=1;					
					}
				else 	if(g_LayoutNum==2)
					{
						g_LineNum=2;
						g_ColNum=2;
					}
					else
					{
						g_LineNum=1;
						g_ColNum=4;
					}
					g_LineNum_Set=0;
					g_ColNum_Set=0;
					InputNumReset();
			break;


				//	
		case State_WizardSet_TwoPort_layout1:
		case State_WizardSet_TwoPort_layout2:
		case State_WizardSet_ThreePort_layout1:
		case State_WizardSet_ThreePort_layout2:
		case State_WizardSet_FourPort_layout1:
		case State_WizardSet_FourPort_layout2:
		case State_WizardSet_FourPort_layout3:
		case State_ScreenSet_OnePort_layout1:
		case State_ScreenSet_TwoPort_layout1:
		case State_ScreenSet_TwoPort_layout2:
		case State_ScreenSet_ThreePort_layout1:
		case State_ScreenSet_ThreePort_layout2:
		case State_ScreenSet_FourPort_layout1:
		case State_ScreenSet_FourPort_layout2:
		case State_ScreenSet_FourPort_layout3:
		{
			if(g_LineNum_Set<=g_LineNum)
			{
				if(value==0 ||value>1080)
				{
					GpuSend("CBOF(131,126,204,149,1,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				gUserInScrRCPix[1][g_LineNum_Set-1]=value;
				InputNumReset();
			}
			else
			{
				if(value==0 ||value>1920)
				{
					GpuSend("CBOF(131,126,204,149,1,15);\r\n");
					InputNumReset();
					return __FALSE;
				}
				gUserInScrRCPix[0][g_ColNum_Set-1]=value;
				InputNumReset();
			}
			break;
		}

		case State_ScenarioSet_NewImage_ImageNum:
		{
			if(value>8 ||value==0)
			{
				InputNumReset();
				GpuSend("CBOF(146,81,184,99,3,15);\r\n");	
				return __FALSE;
			}
			g_ImageNum=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			g_ImageNum_Set=1;
			InputNumReset();
			break;
		}
		
		case State_ScenarioSet_DelScheme_Num:
		case State_ScenarioSet_SchemeNum:
		{
			if(value>9 ||value==0)
			{
				InputNumReset();
				GpuSend("CBOF(131,81,159,99,3,15);\r\n");	
				return __FALSE;
			}
			g_SchemeNum=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}

		//场景设置，新画面，画面参数PSX,PSY.PEX,PEY
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		{
			if(value>gScrTotX)
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(68,63,106,82,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			gInImgPara[0]=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		{
			if(value>gScrTotY)
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(170,63,208,82,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			gInImgPara[1]=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		{
			if(value>gScrTotX )
			{
				InputNumReset();
				GpuSend("SBC(7);CBOF(68,93,106,112,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			gInImgPara[2]=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		{
			if(value>gScrTotY )
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(170,93,208,102,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			gInImgPara[3]=value;  //gInImgPara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}


		//用户试用设置
		case State_UserTrySet_Tel:
		{	
			if(Number1Idx!=0)
			{
				g_TeleNumVal_input=Number1Idx;
				for(nTeleIdx=0;nTeleIdx<=g_TeleNumVal_input;nTeleIdx++)
					g_UserTry_Tel_input[nTeleIdx]=Number1[nTeleIdx];
			}
			
			
			//当在解锁电话输入上按了确认键，要执行一下存储函数
			UserTry_SetData_Save();
			InputNumReset();		
			break;	
		}
		case State_UserTrySet_PassWord:
		{
			if(Number1Idx!=0)
			{
				g_PassWordNumVal_input=Number1Idx;
				for(nTeleIdx=0;nTeleIdx<=g_TeleNumVal;nTeleIdx++)
					g_UserTry_PassWord_input[nTeleIdx]=Number1[nTeleIdx];
			}
			InputNumReset();
			break;	
		}
		
		case State_TryTime_End:
		{
			if(Number1Idx!=0)
			{
				g_PassWordNumVal_input=Number1Idx;
				for(nTeleIdx=0;nTeleIdx<=g_TeleNumVal;nTeleIdx++)
					g_UserTry_PassWord_input[nTeleIdx]=Number1[nTeleIdx];
			}
			Input_PassWord_Check();
			InputNumReset();
			break;	
		}
		
		case State_UserTrySet_Day:
			g_UserTry_Day=value;
			InputNumReset();
			break;
		case State_MainMenu_Menu_ScreenSet_Password:
		case State_MainMenu_Menu_ScenarioSet_Password:
			if(value==168)
			{
				g_Set_First_Flag=0;
				InputNumReset();
				break;
			}
			else
			{
				InputNumReset();
				GpuSend("SBC(7);CBOF(145,80,185,100,3,15);CBOX(145,80,185,100,3,0);DS12(40,105,'错误,请重新输入',1);\r\n");
				return __FALSE;
			}

		
		//20170308 处理数据管理密码的状态
		case 	State_UserTry_Once_Password101:
		case State_UserTry_Once_Password102:
		case State_UserTry_Once_Password103:
		case State_UserTry_Once_Password104:
		case State_UserTry_Once_Password105:	
		case State_UserTry_Once_Password100:		
		{
			if(!CheckManagePasswordNumber())  //如果输入错误
			{
				InputNumReset();			
				GpuSend("SBC(7);CBOF(125,80,200,100,3,15);CBOX(125,80,200,100,3,0);DS12(40,115,'错误,请重新输入',1);\r\n");
				return __FALSE;
			}
			
			if(g_CurrentState==State_UserTry_Once_Password101)
			{
				if(g_UserTry_once_onoff)
				{
					g_UserTry_once_onoff=0; //关闭		
					
					GpuSend("CBOF(5,40,213,170,7,7);CBOX(5,40,213,170,7,0);SBC(15);CBOF(50,80,165,120,7,15);CBOX(50,80,165,120,7,0);DS16(60,90,'单次密码关闭',0);\r\n");		
					delay_ms(20);
				}
				else
				{
					g_UserTry_once_onoff=1; //开启
					
					GpuSend("CBOF(5,40,213,170,7,7);CBOX(5,40,213,170,7,0);SBC(15);CBOF(40,70,175,130,7,15);CBOX(40,70,175,130,7,0);DS16(60,80,'单次密码开启',0);");		
					sprintf(GpuBuf,"DS16(52,103,'单次时长%2d小时',0);\r\n",g_UserTry_once_time);
					GpuSend(GpuBuf);
					delay_ms(20);
								
					if(g_UserTry_show_onoff==0) //如果演示没有打开，那么计时以单次计算，否则以演示为准
					{
						time_num=RTC_GetCounter()+g_UserTry_once_time*60*60;
						g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
						g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
						g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
						g_UserTry_once_end_time[3]=(time_num)&0xff;  		
						m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
						g_UserTry_OnceEndTime=time_num;
					}
				}
				Write_EEPROM_Byte(E2ROM_ONCE_ONOFF_ADDR,g_UserTry_once_onoff);	
			}
			else if(g_CurrentState==State_UserTry_Once_Password102)
			{
				if(g_UserTry_show_onoff)
				{
					g_UserTry_show_onoff=0; //关闭
					GpuSend("CBOF(5,40,213,170,7,7);CBOX(5,40,213,170,7,0);SBC(15);CBOF(50,80,165,120,7,15);CBOX(50,80,165,120,7,0);DS16(60,90,'演示密码关闭',0);\r\n");		
					delay_ms(20);
				}
				else
				{
					g_UserTry_show_onoff=1; //开启
					GpuSend("CBOF(5,40,213,170,7,7);CBOX(5,40,213,170,7,0);SBC(15);CBOF(40,70,175,130,7,15);CBOX(40,70,175,130,7,0);DS16(60,80,'演示密码开启',0);");		
					sprintf(GpuBuf,"DS16(52,103,'演示时长%2d分钟',0);\r\n",g_UserTry_show_time);
					GpuSend(GpuBuf);
					delay_ms(20);
					
					time_num=RTC_GetCounter()+g_UserTry_show_time*60;
					g_UserTry_once_end_time[0]=(time_num>>24)&0xff;  
					g_UserTry_once_end_time[1]=(time_num>>16)&0xff; 
					g_UserTry_once_end_time[2]=(time_num>>8)&0xff; 
					g_UserTry_once_end_time[3]=(time_num)&0xff;  		
					m24xx_write(E2ROM_ONCE_END_ADDR,g_UserTry_once_end_time,5);	
					g_UserTry_OnceEndTime=time_num;
				}
				Write_EEPROM_Byte(E2ROM_SHOW_ONOFF_ADDR,g_UserTry_show_onoff);	
			}
			break;
		}
		case State_UserTry_Once_Password100_new:
		{
			g_manage_password_frist=value;
			break;
		}
		case State_UserTry_Once_Password100_new1:
		{
			if(g_manage_password_frist==value)
			{
				g_manage_password=value;
				g_UserTry_once_managepassword[0]=(g_manage_password>>24)&0xff;  
				g_UserTry_once_managepassword[1]=(g_manage_password>>16)&0xff; 
				g_UserTry_once_managepassword[2]=(g_manage_password>>8)&0xff; 
				g_UserTry_once_managepassword[3]=(g_manage_password)&0xff; 
				m24xx_write(E2ROM_ONCE_MANAGE_PASSWORD_ADDR,g_UserTry_once_managepassword,5);				
				g_manage_password=value;
				
				GpuSend("CBOF(5,40,213,170,7,7);CBOX(5,40,213,170,7,0);SBC(15);CBOF(50,80,165,120,7,15);CBOX(50,80,165,120,7,0);DS16(60,90,'密码修改成功',0);\r\n");		
				delay_ms(20);
				g_CurrentState=State_DebugMode_Start;
			}
			else
			{
				InputNumReset();			
				GpuSend("SBC(7);CBOF(125,97,200,117,3,7);DS12(40,135,'错误,请重新输入',1);\r\n");
				return __FALSE;
			}
			break;
		}
		
	} 
		//InputFourNumReset();
		InputDateTimeReset();
	return __TRUE;

}



//-----??????????s------------------------
void DealInputNumber(U8 cnt)
{	

	U8 input=0;
	NumKeyToInt(&cnt);	
	input=cnt;
	//if(input==0||input>4)
	if(input==0||input>=(MAX_OUT_PORTS+1))
		return;
//	g_Image1Scr=input;
	
	switch(g_CurrentState)
	{
		case State_AdvanceSet_CardTest_Port:
			g_CardTest_Port=input;
			OutPort_TestPatternColor();
			break;
		case State_AdvanceSet_CardTest_Color:
			g_CardTest_Color=input;
			//===2017,Jun 14 ; 11:4:40  ;BY YANG@BYCOX.COM===
			//if(input>0&&input<4)
				OutPort_TestPatternColor();
			break;
		case State_ScenarioSet_NewImage_ImagePara_Source:
		case State_WizardSet_InputSource_Set:
			g_Image_Scr=input;
			break;

	}
	LcdDisp_Follow_Keyboard(g_CurrentState);
}




void BackSpaceNumber(U8 addr,U8 num)
{	
	int i=0;
	if(Number1Idx>0)
		Number1Idx--;
	g_Number1OverFlag=0;
	wcom(addr);
	for(i=0;i<num;i++)
		dat(' ');
	//chinesedisplay(addr, "    ");	
	Number1[Number1Idx]=0;
	chinesedisplay(addr, Number1);
	printStr("_");

	if(g_CurrentState==State_LockPadTime_Set||IsSetParaValueState())
	{
		LcdDisp_Num(Number1);
	}
		
}

void InputDateTimeReset(void)
{


Num_Clear();


	Number1Idx=0;
	g_Number1OverFlag=0;
	
	g_MonthIdx=0;
	g_MonthOverFlag=0;
	
	g_DateIdx=0;
	g_DateOverFlag=0;
	
	g_HourIdx=0;
	g_HourOverFlag=0;
	
	g_MinIdx=0;
	g_MinOverFlag=0;
	
	g_SecondIdx=0;
	g_SecondOverFlag=0;	
}



void Num_Clear(void)
{
	Number1[1]=0;
	Number1[2]=0;
	Number1[3]=0;
	Number1[4]=0;
	Number1[5]=0;
}

