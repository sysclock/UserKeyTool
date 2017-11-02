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


BOOL CheckSerialNumber()
{
	U16 value,valueID,value1,value2;
	if(Number1Idx==0)
		return __FALSE;	
	valueID=ChipUniqueID%10000;
	value=atoi(Number1,Number1Idx);
	/*if(value1+value==9999)//(value1^value))
		return __TRUE;
	else
		return __FALSE;*/
	value1=(valueID<<4)|(valueID>>10);
	value2=(valueID<<10)|(valueID>>4);
	if(value==((value1^value2)&0x3fff))
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
		case 201:					
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise201Str[nTeleIdx];
			break;
		case 202:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise202Str[nTeleIdx];
			break;
		case 203:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise203Str[nTeleIdx];
			break;
		case 301:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise301Str[nTeleIdx];
			break;
		case 8721:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8721Str[nTeleIdx];
			break;
		case 8722:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8722Str[nTeleIdx];
			break;
		case 8723:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8723Str[nTeleIdx];
			break;
		case 8725:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8725Str[nTeleIdx];
			break;
		case 8726:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8726Str[nTeleIdx];
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
				stemp=atoi(Number1,Number1Idx);
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
				stemp=atoi(Number1,Number1Idx);
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
				stemp=atoi(Number1,Number1Idx);
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
		case State_User_Input_Logo:
			if(Number1Idx!=0)
			{	
				stemp=atoi(Number1,Number1Idx);
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
		case State_User_Input_MathineType:
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
				if(g_UserSetPasswordVal==atoi(Number1,Number1Idx))
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
				g_UserSetPasswordVal=atoi(Number1,Number1Idx);
				Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
				LcdDisplaySetOK_ReturnMain();	
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
			
		case State_User_Set_Tech_TelNumber:
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
			ColorDisplay_Num(Number1);
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
						||IsSetParaValueState())
		{
			ColorDisplay_Char(Number1,Number1Idx);
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

U16 atoi(U8 *numStr,int Idx)
{
	U8 ch;
	U16 Num;
	U16 result=0;
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
		g_LockKeypadTime=atoi(Number1,Number1Idx);	
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
	value=atoi(Number1,Number1Idx);

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



BOOL DebugSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	value=atoi(Number1,Number1Idx);
	InputNumReset();
	if(currentState==State_DebugMode_Start)//调试序号
	{
		switch(value)
		{
			case 8080:  //设置试用参数  天数 密码 联系电话
				DealCurrentState(State_UserTrySet_Start);
				break;
//			case 1://设置当前时间
//				g_SetTimeFlag=Set_Current_Time_Idx;
//				SetCurrentTime();
//				break;
			case 2://设置自动锁键盘时间
				DealCurrentState(State_LockPadTime_Set);
				break;
			case 3://恢复出厂设置
				DealCurrentState(State_FactoryReset_Yes);
				break;
//			case 4://设置截止日期
//				g_InputOldSeriesForWhat=2;
//				if(g_UserSetPasswordVal<1000000)
//					DealCurrentState(State_User_Old_SerialNumber);
//				else	
//				{			
//					g_SetTimeFlag=Set_Try_Use_Idx;
//					clearlcd();
//					chinesedisplay(0x80, g_DeadlineFmtStr);
//				}
//					//DealCurrentState(State_Try_Use);
//				break;
			case 5://查看机器ID
//				DealCurrentState(State_MainMenu_Menu_IDInfo);
//				DisMathineIDValue(ChipUniqueID,0x84);				
//				break;
			/*case 6://开启同步切换双画面
				clearlcd();
				g_BothSwitchFlag=__TRUE;
				Write_EEPROM_Byte(E2ROM_BOTH_SWITCH_ADDR,g_BothSwitchFlag);
				if(g_LanguageFlag==CHINESE)
				{
					chinesedisplay(0x80,g_BothSwithStr);
					chinesedisplay(0x87,g_StartStr);
				}
				else
				{
					chinesedisplay(0x80, g_BothSwithStrEn);
					chinesedisplay(0x90,g_StartStrEn);
				}
				break;
			case 7://取消同步切换双画面
				clearlcd();
				g_BothSwitchFlag=__FALSE;
				Write_EEPROM_Byte(E2ROM_BOTH_SWITCH_ADDR,g_BothSwitchFlag);
				if(g_LanguageFlag==CHINESE)
				{
					chinesedisplay(0x80,g_BothSwithStr);
					chinesedisplay(0x87,g_CancelStr);
				}
				else
				{
					chinesedisplay(0x80, g_BothSwithStrEn);
					chinesedisplay(0x90,g_CancelStrEn);
				}
				break;*/
//			case 8://USB升级
////				clearlcd();
////				DealUsbUpgrade();
//				break;
				
//			case 11:
//				DealCurrentState(State_Input_PackagingName);
//				break;
//			case 12:
//				DealCurrentState(State_Input_TestName);
//				break;
//			case 13:
//				DealCurrentState(State_Input_SaleName);
//				break;

//			case 21://组装员
//				clearlcd();
//				chinesedisplay(0x80, g_InputPackagingNameStr+6);
//				if(g_Pack_ID/1000!=0)
//					DisTwoNumValue(g_Pack_ID/1000, 0x85);
//				DisFourNumValue(g_Pack_ID%1000, 0x86);
//				chinesedisplay(0x90, g_PackagingName);
//				break;
//			case 22://测试员
//				clearlcd();
//				chinesedisplay(0x80, g_InputTestNameStr+6);
//				if(g_Test_ID/1000!=0)
//					DisTwoNumValue(g_Test_ID/1000, 0x85);
//				DisFourNumValue(g_Test_ID%1000, 0x86);
//				chinesedisplay(0x90, g_TestName);
//				break;
//			case 23://销售员
//				clearlcd();
//				chinesedisplay(0x80, g_InputSaleNameStr+6);
//				if(g_Sale_ID/1000!=0)
//					DisTwoNumValue(g_Sale_ID/1000, 0x85);
//				DisFourNumValue(g_Sale_ID%1000, 0x86);
//				chinesedisplay(0x90, g_SaleName);
//				break;
//				
//			case 70://施工方设置公司logo
//				DealCurrentState(State_User_Input_Logo);
//				break;
//			case 71://施工方设置技术支持电话
//				DealCurrentState(State_User_Set_Tech_TelNumber);
//				break;
//			case 72://修改机器型号
//				DealCurrentState(State_User_Input_MathineType);
//				break;				
//			case 80://施工方设置试用密码
//				g_InputOldSeriesForWhat=0;
//				if(g_UserSetPasswordVal<1000000)
//					DealCurrentState(State_User_Old_SerialNumber);
//				else
//					DealCurrentState(State_User_Set_SerialNumber);
//				break;
//			case 81://施工方设置试用天数
//				g_InputOldSeriesForWhat=1;
//				if(g_UserSetPasswordVal<1000000)
//					DealCurrentState(State_User_Old_SerialNumber);
//				break;
//			case 82://施工方设置联系电话
//				DealCurrentState(State_User_Set_TelNumber);
//				break;
//			case 83://输入序列号，解除试用
//				DealCurrentState(State_User_Input_SerialNumber);
//				break;
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

BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	U8 nTeleIdx=0;
	U8 StepDisPos=0x88;
	U16 pData;//TempData1;
	char GpuBuf[50];
	
	if(Number1Idx==0)
	{
			switch(currentState)  //如果没有输入值，生效默认值
			{				
				case State_MatrixMode_ModeSet_input:
					d_matrix_inputsource_num=0;
					return __TRUE;
				case State_MatrixMode_ModeSet_output:	
					d_matrix_outputsource_num=0;
					Matrix_Display_Function();
					return __TRUE;
				case State_WizardSet_OutPort_Num:
				case State_ScreenSet_OutPort_Num:
					g_OutPort_Num=DEF_OUTPORT_NUM;
					return __TRUE ;
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
				case State_WizardSet_OnePort_layout1:
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
	value=atoi(Number1,Number1Idx);

	switch(currentState)
	{
		//设置输入大屏行值
		case State_MatrixMode_ModeSet_input:
				if(value>4 )
				{
					InputNumReset();			
					GpuSend("CBOF(66,66,94,89,3,15);\r\n");
					return __FALSE;
				}
				d_matrix_inputsource_num=value;
				InputNumReset();
			break;
		case State_MatrixMode_ModeSet_output:
			if(value>4)
			{
				InputNumReset();			
				GpuSend("CBOF(126,66,154,89,3,15);\r\n");
				return __FALSE;
			}
			d_matrix_outputsource_num=value;		
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
				g_screen_line_column_pixel_input[1][g_LineNum_Set-1]=value;
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
				g_screen_line_column_pixel_input[0][g_ColNum_Set-1]=value;
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
			g_ImageNum=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
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
			g_SchemeNum=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		{
			if(value>d_screen_total_X)
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(68,63,106,82,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			g_input_imagepara[0]=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		{
			if(value>d_screen_total_Y)
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(170,63,208,82,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			g_input_imagepara[1]=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		{
			if(value>d_screen_total_X )
			{
				InputNumReset();
				GpuSend("SBC(7);CBOF(68,93,106,112,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			g_input_imagepara[2]=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		{
			if(value>d_screen_total_Y )
			{
				AdviceandResetNumber();
				InputNumReset();
				GpuSend("SBC(7);CBOF(170,93,208,102,1,7);DS12(10,130,'错误:参数超出范围',1);DS12(10,145,'     重新输入参数',1);\r\n");
				return __FALSE;
			}
			g_input_imagepara[3]=value;  //g_input_imagepara 在 display.c中做的定义，元素7表示画面号
			InputNumReset();
			break;
		}
		case State_UserTrySet_Tel:
		{	
			if(Number1Idx!=0)
			{
				g_TeleNumVal_input=Number1Idx;
				for(nTeleIdx=0;nTeleIdx<=g_TeleNumVal;nTeleIdx++)
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
		
	}
		//InputFourNumReset();
		InputDateTimeReset();
	return __TRUE;

}


void CardTest_Send(void)
{
	u8 i;
	if(g_CardTest_Port>0&&g_CardTest_Port<9)
	{
		switch(g_CardTest_Color)
		{
			case 1:
				sprintf(d_780command_send_buf,"O_F STPC %d %d 1 0xFF0000\r\n",(g_CardTest_Port-1)/4+3,(g_CardTest_Port-1)%4);
				command_send(d_780command_send_buf);
				break;
			case 2:
				sprintf(d_780command_send_buf,"O_F STPC %d %d 1 0x00FF00\r\n",(g_CardTest_Port-1)/4+3,(g_CardTest_Port-1)%4);
				command_send(d_780command_send_buf);
				break;
			case 3:
				sprintf(d_780command_send_buf,"O_F STPC %d %d 1 0x0000FF\r\n",(g_CardTest_Port-1)/4+3,(g_CardTest_Port-1)%4);
				command_send(d_780command_send_buf);
				break;
			case 4:
				for(i=0;i<4;i++)
				{
					sprintf(d_780command_send_buf,"O_F STPC %d %d 0 0x000000\r\n",(i)/4+3,(i)%4);
					command_send(d_780command_send_buf);
				}	
				break;
			default:	
				break;
		}

	}
}


//--------------------------------------
void DealInputNumber(U8 cnt)
{	

	U8 input=0;
	NumKeyToInt(&cnt);	
	input=cnt;
	if(input==0||input>4)
		return;
//	g_Image1Scr=input;
	
	switch(g_CurrentState)
	{
		case State_AdvanceSet_CardTest_Port:
			g_CardTest_Port=input;
			CardTest_Send();
			break;
		case State_AdvanceSet_CardTest_Color:
			g_CardTest_Color=input;
			if(input>0&&input<4)
				CardTest_Send();
			break;
		case State_ScenarioSet_NewImage_ImagePara_Source:
		case State_WizardSet_InputSource_Set:
			g_Image_Scr=input;
			break;

	}
	lcdselection(g_CurrentState);
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
		ColorDisplay_Num(Number1);
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

