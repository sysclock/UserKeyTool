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
U8 g_SetInputScheFlag=INSRCINI;

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
//static U8 g_Week[2];
//static U8 g_WeekIdx=0;
//static U8 g_WeekOverFlag=0;


static U8 SaveMode[2]={0,0};          //mode ascii
//static U8 TwoMode[2]={10,10};          //two mode recall
//extern U16 TimeSet[6];




//     INPUTSOURCE_NONE,
//     INPUTSOURCE_CVBS,   // VIDEO1 - CVBS
//     INPUTSOURCE_CVBS2,   // VIDEO2 - CVBS2
//     INPUTSOURCE_CVBS3,   // VIDEO2 - CVBS2
//     INPUTSOURCE_CVBS4,   // VIDEO2 - CVBS2
//     INPUTSOURCE_DVI,    // PC - DVI
//     INPUTSOURCE_YPBPR,  // VIDEO - YPbPr1
//     INPUTSOURCE_VGA,    // PC - VGA
//     INPUTSOURCE_VGA2,    // PC - VGA
//     INPUTSOURCE_HDMI,    // PC - DVI
//     INPUTSOURCE_SDI,
// 	  INPUTSOURCE_USB,
//     INPUTSOURCE_DP,
//     INPUTSOURCE_NUM
_InputSourceType NumKeyToInputSrc(U8 cnt)    //key to input src
{
	switch(cnt)
	{
		case CVBS_KEY:
			return INPUTSOURCE_CVBS;
		case CVBS2_KEY:
			return INPUTSOURCE_CVBS2;
		case NUM6_KEY:
			return	INPUTSOURCE_CVBS3; 
		case NUM7_KEY:
			return INPUTSOURCE_VGA2; 
		case NUM8_KEY:
			return INPUTSOURCE_DVI2;
		case NUM9_KEY:
			return INPUTSOURCE_USB;
		case VGA_KEY:
			return INPUTSOURCE_VGA;
		case DVI_KEY:
			return INPUTSOURCE_DVI;
		case HDMI_KEY:
			return INPUTSOURCE_HDMI;		
		default:
			return INPUTSOURCE_NONE;
	}
}
 
_KeypadNum InputSrcToNumKey(_InputSourceType input)    // number to key
{	
	switch(input)
	{
		case INPUTSOURCE_CVBS:
			return CVBS_KEY;
		case INPUTSOURCE_CVBS2:
			return CVBS2_KEY;
	#if(MATHINE_TYPE==XP_520)
/*		case INPUTSOURCE_CVBS3:
			return CVBS3_KEY;
		case INPUTSOURCE_YPBPR:
			return YPBPR_KEY;
		case INPUTSOURCE_SDI:
			return SDI_KEY;	 */
	#endif
		case INPUTSOURCE_VGA:
			return VGA_KEY;
		case INPUTSOURCE_DVI:
			return DVI_KEY;
		case INPUTSOURCE_HDMI:
			return HDMI_KEY;
	}
	return KEY_NUM;
}

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

void DealSaveAndRecallNumber(U8 cnt)   //receive num key            user mode or save mode
{
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	SaveMode[0]=cnt+48;
	if(g_CurrentState==State_SavePara_Number)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x84, g_SingleOrDual+8);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x84, g_SingleOrDualEn+8);
	}
	else if(g_CurrentState==State_UserPara_Number)
	{
		clearlcd();
		NumKeySetUserMode();
	}	
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x90, g_Mode1Str+2);
	}
	else  if(g_LanguageFlag==ENGLISH)
	{	
		chinesedisplay(0x90, g_Mode1StrEn+2);
	}
	chinesedisplay(0x92, SaveMode);
}


void NumKeySetUserMode()
{
	
	//if save as mode 0-9

	SetUserMode(SaveMode[0]-48);
}


void NumKeySetSaveMode()
{
//	U8 SaveModeIdx;
	g_SaveModeValue=SaveMode[0]-48;
	
	//if save as mode 0-10

	SetSaveMode(g_SaveModeValue);
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
		
	}
	else
	{
		////
		if(Number1[0]=='5')////&&(Number1[1]=='5'||Number1[1]=='9'))
			{	
			g_MachineTypeXPStr[0]='B';
			g_MachineTypeXPStr[1]='P';
			g_MachineTypeXPStr[2]=Number1[0];
			g_MachineTypeXPStr[3]=Number1[1];
			g_MachineTypeXPStr[4]=Number1[2];
			g_MachineTypeXPStr[5]=Number1[3];
			}
		else////

			for(nTeleIdx=0;nTeleIdx<Number1Idx;nTeleIdx++)
				g_MachineTypeXPStr[nTeleIdx]=Number1[nTeleIdx];
		//g_MachineTypeXPStr[Number1Idx]=0;
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
	ReturnMainWin();
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
		case 2:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise2Str[nTeleIdx];
			break;
		case 3:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise3Str[nTeleIdx];
			break;
		case 4:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise4Str[nTeleIdx];
			break;
		case 5:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise5Str[nTeleIdx];
			break;
		case 6:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise6Str[nTeleIdx];
			break;
		case 7:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise7Str[nTeleIdx];
			break;
		case 8:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8Str[nTeleIdx];
			break;
		case 9:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise9Str[nTeleIdx];
			break;
		case 10:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise10Str[nTeleIdx];
			break;
		case 11:						
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise11Str[nTeleIdx];
			break;
		case 12:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise12Str[nTeleIdx];
			break;
		case 13:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise13Str[nTeleIdx];
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
		case 204:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise204Str[nTeleIdx];
			break;
		case 205:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise205Str[nTeleIdx];
			break;
		case 206:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise206Str[nTeleIdx];
			break;
		case 301:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise301Str[nTeleIdx];
			break;
		case 4958:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise4958Str[nTeleIdx];
			break;
		case 7075:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise7075Str[nTeleIdx];
			break;
		case 7076:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise7076Str[nTeleIdx];
			break;
		case 8476:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8476Str[nTeleIdx];
			break;
		default:
			break;
	}
	FlashWriteLogo();
}

BOOL InputNumberOver(EventValue Val)
{
	U32 stemp;
	int nTeleIdx;
	switch(g_CurrentState)
	{
		case State_Input_YEAR:
			NumKeySetValue(Val,g_CurrentState);
			break;
		case State_Input_MONTH:
			NumKeySetMonth(Val,g_CurrentState);
			break;
		case State_Input_DOM:
			NumKeySetDate(Val,g_CurrentState);
			break;
		case State_Input_HOUR:
			NumKeySetHour(Val,g_CurrentState);
			break;
		case State_Input_MIN:
			NumKeySetMin(Val,g_CurrentState);
			break;
		case State_Input_SEC:
			NumKeySetSecond(Val,g_CurrentState);
			break;
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

		case State_HowLong_Save:
			if(Number1Idx!=0)
			{	
				g_howlongtosave=atoi(Number1,Number1Idx);
				Write_EEPROM_Byte(E2ROM_HowLongTime_ADDR,g_howlongtosave);
//				printf("g_howlongtosave=%d\n",g_howlongtosave);
				LcdDisplaySetOK_ReturnMain();
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
			
		case State_User_Input_TryDays:
			if(Number1Idx!=0)
			{	
				check_Try_End_Time(atoi(Number1,Number1Idx));
				Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);
				LcdDisplaySetOK_ReturnMain();
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		case State_User_Input_Logo:
			if(Number1Idx!=0)
			{	
				//chinesedisplay(0x90, "USE PC Set Logo!");
				
				stemp=atoi(Number1,Number1Idx);
				if(stemp>65535)
				{
					chinesedisplay(0x91, "_     ");
					chinesedisplay(0x98, g_OutRangeStr);
				}
				else
				{
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
				Write_EEPROM_Byte(E2ROM_MathineType_Num_ADDR, Number1Idx+1);
				m24xx_write(E2ROM_MathineType_ADDR,Number1, Number1Idx+2);
				Copy_Mathine_Type(Number1, Number1Idx+1);
				LcdDisplaySetOK_ReturnMain();
				//chinesedisplay(0x90,"Use PC Set Type!");
				Number1Idx=0;
				g_Number1OverFlag=0;
			}
			break;
		case State_User_Input_SerialNumber:
			if(CheckSerialNumber())
			{
				g_Deadline7DaysFlag=0xFF;
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
				ReturnMainWin();
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
					else if(g_InputOldSeriesForWhat==1)
						DealCurrentState(State_User_Input_TryDays);
					else if(g_InputOldSeriesForWhat==2)
					{		
						g_SetTimeFlag=Set_Try_Use_Idx;
						g_CurrentState=State_Input_YEAR;
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

BOOL IsSetupMode()
{
	switch(g_CurrentState)
	{
		case State_Input_YEAR:
		case State_Input_MONTH:
		case State_Input_DOM:
		case State_Input_HOUR:
		case State_Input_MIN:
		case State_Input_SEC:
			return __TRUE;
		default:
			return __FALSE;
	}
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

//void DealWeekNumber(U8 cnt,U8 addr)
//{	
//	if(!NumKeyToInt(&cnt))
//		return;
//	else if(cnt==8||cnt==9||cnt==0)
//		return;
//	if(g_WeekIdx<1&&g_WeekOverFlag==0)
//	{	
//		g_Week[g_WeekIdx++]=cnt+48;
//		g_Week[g_WeekIdx]=0;
//		chinesedisplay(addr, g_Week);
//	}
//	if(g_WeekIdx==1)
//	{
//		g_WeekOverFlag=1;
//	}
//}

//deal schedule week day for loop
void DealScheWeekNum(U8 cnt)
{	
	U8 week[1];
	U8 idx=0;
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	else if(cnt==8||cnt==9||cnt==0)
		return;
	week[0]=cnt+48;
	if(g_CurrentState==State_ScheduleInput1_InputWeek&&(g_InputWeek1Idx<7))
	{
		g_ScheduleInputMode1=MORESCHE;
		// check it is same number
		for(idx=0;idx<g_InputWeek1Idx;idx++)
		{
			if(cnt==g_InputWeek1[idx])
				return;
		}
		//---------------------
		g_InputWeek1[g_InputWeek1Idx]=cnt;
		chinesedisplay(0x90+g_InputWeek1Idx,week);
		
		g_InputWeek1Idx++;
		if(g_InputWeek1Idx<7)
			printStr(" _");
	}
	else if(g_CurrentState==State_ScheduleInput2_InputWeek&&(g_InputWeek2Idx<7))
	{
		g_ScheduleInputMode2=MORESCHE;
		// check it is same number
		for(idx=0;idx<g_InputWeek2Idx;idx++)
		{
			if(cnt==g_InputWeek2[idx])
				return;
		}
		//---------------------
		g_InputWeek2[g_InputWeek2Idx]=cnt;
		chinesedisplay(0x90+g_InputWeek2Idx,week);
		
		g_InputWeek2Idx++;
		if(g_InputWeek2Idx<7)
			printStr(" _");
	}
	else if(g_CurrentState==State_ScheduleInput3_InputWeek&&(g_InputWeek3Idx<7))
	{
		g_ScheduleInputMode3=MORESCHE;
		// check it is same number
		for(idx=0;idx<g_InputWeek3Idx;idx++)
		{
			if(cnt==g_InputWeek3[idx])
				return;
		}
		//---------------------
		g_InputWeek3[g_InputWeek3Idx]=cnt;
		chinesedisplay(0x90+g_InputWeek3Idx,week);
		
		g_InputWeek3Idx++;
		if(g_InputWeek3Idx<7)
			printStr(" _");
	}
}

void DealScreenNumber(U8 cnt,U8 addr,U8 num)
{
	int i;
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
	wcom(addr);
	for(i=0;i<num;i++)
	{
		dat(' ');
	}
	if(Number1Idx<num&&g_Number1OverFlag==0)
	{	
		Number1[Number1Idx++]=cnt+48;
		Number1[Number1Idx]=0;
		chinesedisplay(addr, Number1);
		//add next cusor
		if(Number1Idx<num)
			printStr("_");
			
	}
	if(Number1Idx==num)
	{
		g_Number1OverFlag=1;
		chinesedisplay(addr, Number1);    //after input four number display          can not delete
		if(g_CurrentState==State_User_Input_SerialNumber
			||g_CurrentState==State_UserPara_SetStaticIP_Value)
			InputNumberOver(KEY_UP);
	}

}

void InputFourNumReset()
{
	Number1Idx=0;
	g_Number1OverFlag=0;
}

void InputNumReset()
{
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

BOOL InputNumber()
{
	if(Number1Idx!=0)
		return __TRUE;
	return __FALSE;
}



//数字键输入值
BOOL NumKeySetValue(EventValue Val,State curState)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	
	value=atoi(Number1,Number1Idx);

	//声音音量最大为100
	if(g_CurrentState==State_Volume_Set&&value>100)
		value=100;

	else if((g_CurrentState==State_ScreenPara_Size_Value
		||g_CurrentState==State_ScalerPara_Size_Value
		||g_CurrentState==State_DualImage_Set_MainSize
		||g_CurrentState==State_DualImage_Set_SubSize
		)&&value==0)
		return __FALSE;
#ifdef TCP_IP
	else if(g_CurrentState==State_UserPara_SetStaticIP_Value&&value>255)
		value=255;
#endif

	SetCurrentStateValue(Val,curState,value);
	//set cusor
	
	if(curState==State_Input_YEAR)
	{
		chinesedisplay(YEARADDR, "    ");
		chinesedisplay(YEARADDR, Number1);		
		chinesedisplay(MONTHADDR, "_");
	}
	return __TRUE;
}


void NumKeySetMonth(EventValue Val,State curState)
{	
	U8 value;
	if(g_MonthIdx==0)
		return;
	value=atoi(g_Month,g_MonthIdx);
	if(value>12||value==0)
	{
		chinesedisplay(MONTHADDR, "_ ");
		g_MonthIdx=0;
		g_MonthOverFlag=0;
		return;
	}
	SetCurrentStateValue(Val,curState,value);
	//set cusor
	chinesedisplay(MONTHADDR, "  ");
	chinesedisplay(MONTHADDR, g_Month);		
	chinesedisplay(DAYADDR, "_");
}
void NumKeySetDate(EventValue Val,State curState)
{	
	U8 value;
	if(g_DateIdx==0)
		return;
	value=atoi(g_Date,g_DateIdx);
	if(value>31||value==0)
	{
	      chinesedisplay(DAYADDR, "_ ");
		g_DateIdx=0;
		g_DateOverFlag=0;
		return;
	}
	SetCurrentStateValue(Val,curState,value);
	//set cusor	
	chinesedisplay(DAYADDR, "  ");
	chinesedisplay(DAYADDR, g_Date);
	chinesedisplay(HOURADDR, "_");
}
void NumKeySetHour(EventValue Val,State curState)
{	
	U8 value;
	if(g_HourIdx==0)
		return;
	value=atoi(g_Hour,g_HourIdx);
	if(value>23)
	{
		chinesedisplay(HOURADDR, "_ ");
		g_HourIdx=0;
		g_HourOverFlag=0;
		return;
	}
	SetCurrentStateValue(Val,curState,value);
	
	chinesedisplay(HOURADDR, "  ");
	chinesedisplay(HOURADDR, g_Hour);
	chinesedisplay(MINADDR, "_");
}
//如果是设置当前时间，分钟两位满了之后会自动进入该函数，或者按OK键
//如果是设置时间计划，当按OK键才会执行该函数
void NumKeySetMin(EventValue Val,State curState)
{	
	U8 value;
	if(g_MinIdx==0)
		return;
	value=atoi(g_Min,g_MinIdx);
	if(value>59)
	{
		chinesedisplay(MINADDR, "_ ");
		g_MinIdx=0;
		g_MinOverFlag=0;
		return;
	}
	if(g_SetTimeFlag==Set_InputCycle_Idx)
	{
		if(TimeSet[3]==0&&value==0)
		{
			chinesedisplay(MINADDR, "_ ");
			g_MinIdx=0;
			g_MinOverFlag=0;
			return;
		}
	}
	SetCurrentStateValue(Val,curState,value);
	
	chinesedisplay(MINADDR, "  ");
	chinesedisplay(MINADDR, g_Min);
	
	if(g_SetTimeFlag==Set_Current_Time_Idx)
		chinesedisplay(SECONDADDR, "_");
	else if(g_SetTimeFlag==Set_Input_Schedule_Idx)
	{	
		clearlcd();
		if(g_InputScheduleModeValue==MODE1)
		{
			 g_CurrentState=State_ScheduleInput1_Once;
			 g_InputTime1[0]=TimeSet[3];  //hour
			 g_InputTime1[1]=TimeSet[4];  //min	
			 if(g_ScheduleInputMode1==ONCESCHE)
				chinesedisplay(0x90,"*");
			 else
			 	chinesedisplay(0x90,">");
		}
		else	if(g_InputScheduleModeValue==MODE2)
		{
			 g_CurrentState=State_ScheduleInput2_Once;
			 g_InputTime2[0]=TimeSet[3];  //hour
			 g_InputTime2[1]=TimeSet[4];  //min		
			 if(g_ScheduleInputMode2==ONCESCHE)
				chinesedisplay(0x90,"*");
			 else
			 	chinesedisplay(0x90,">"); 
		}
		else	if(g_InputScheduleModeValue==MODE3)
		{
			 g_CurrentState=State_ScheduleInput3_Once;
			 g_InputTime3[0]=TimeSet[3];  //hour
			 g_InputTime3[1]=TimeSet[4];  //min		
			 if(g_ScheduleInputMode3==ONCESCHE)
				chinesedisplay(0x90,"*");
			 else
			 	chinesedisplay(0x90,">"); 
		}	
//转入下一个状态，单一模式页面，用UP down键查看，OK选择
		chinesedisplay(0x80, stateData[g_CurrentState].string1th);  
		chinesedisplay(0x91, stateData[g_CurrentState].string2th);		
	}
	else if(g_SetTimeFlag==Set_Mode_Schedule_Idx)
	{
		
	}
	else if(g_SetTimeFlag==Set_InputCycle_Idx)
	{
		switch(g_InputCycleIdx)
		{
			case 1:
				g_CycleInputHour1=TimeSet[3];
			 	g_CycleInputMin1=TimeSet[4];  //min	
			 	
				g_StartCycleInputFlag=__TRUE;
				CycleSwitchInputSrc(g_CycleInput1);
				g_CycleInputStartHour=(systmtime.tm_hour+g_CycleInputHour1)%24;
				g_CycleInputStartMin=(systmtime.tm_min+g_CycleInputMin1)%60;
				Write_EEPROM_Byte(E2ROM_CycleFlag_ADDR,g_StartCycleInputFlag);
				Write_EEPROM_InputCycle(E2ROM_CycleInput1_ADDR,g_CycleInput1,g_CycleInputHour1,g_CycleInputMin1);
			
				break;
			case 2:
				g_CycleInputHour2=TimeSet[3];
			 	g_CycleInputMin2=TimeSet[4];  //min	
				Write_EEPROM_InputCycle(E2ROM_CycleInput2_ADDR,g_CycleInput2,g_CycleInputHour2,g_CycleInputMin2);
				break;
			case 3:
				g_CycleInputHour3=TimeSet[3];
			 	g_CycleInputMin3=TimeSet[4];  //min	
				Write_EEPROM_InputCycle(E2ROM_CycleInput3_ADDR,g_CycleInput3,g_CycleInputHour3,g_CycleInputMin3);
				break;
			case 4:
				g_CycleInputHour4=TimeSet[3];
			 	g_CycleInputMin4=TimeSet[4];  //min	
				Write_EEPROM_InputCycle(E2ROM_CycleInput4_ADDR,g_CycleInput4,g_CycleInputHour4,g_CycleInputMin4);
				break;
			case 5:
				g_CycleInputHour5=TimeSet[3];
			 	g_CycleInputMin5=TimeSet[4];  //min
				Write_EEPROM_InputCycle(E2ROM_CycleInput5_ADDR,g_CycleInput5,g_CycleInputHour5,g_CycleInputMin5);	
				break;
			case 6:
				g_CycleInputHour6=TimeSet[3];
			 	g_CycleInputMin6=TimeSet[4];  //min
				g_CurrentState=State_MainMenu_Menu;
				Write_EEPROM_InputCycle(E2ROM_CycleInput6_ADDR,g_CycleInput6,g_CycleInputHour6,g_CycleInputMin6);
				DealEscKey();
				return;
		}
		DealCurrentState(State_CycleInput_Continue);
	}
	
}
void NumKeySetSecond(EventValue Val,State curState)
{	
	U8 value;
	if(g_SecondIdx==0)
		return;
	value=atoi(g_Second,g_SecondIdx);
	if(value>59)
	{
		chinesedisplay(SECONDADDR, "_ ");
		g_SecondIdx=0;
		g_SecondOverFlag=0;
		return;
	}
	SetCurrentStateValue(Val,curState,value);
	
	chinesedisplay(SECONDADDR, "  ");
	chinesedisplay(SECONDADDR, g_Second);
//	chinesedisplay(WEEKADDR, "_");
}

//void NumKeySetWeek(EventValue Val,State curState)
//{	
//	U8 value;
//	if(g_WeekIdx==0)
//		return;
//	value=atoi(g_Week,g_WeekIdx);
//	
//	if(value==7)
//		value=0;
//	
//	SetCurrentStateValue(Val,curState,value);
//}

void SetCurrentTime()
{
//	U32 datas; 
//	U32 times;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
	chinesedisplay(0x80, g_DateStr);
	chinesedisplay(0x90, g_TimeStr);
	}
	else
	{
	chinesedisplay(0x80, g_DateStrEn);
	chinesedisplay(0x90, g_CurrentTimeStrEn);
	}	
	DisplayTime();
	
//	g_CurrentState=State_Input_SEC;
//	g_SecondIdx=1;
	g_CurrentState=State_Input_HOUR;
}

void CancelInputScheduleMode1()
{
	g_ScheduleInput1=INPUTSOURCE_NONE;
	g_InputTime1[0]=0xff;
	g_InputTime1[1]=0xff;
	SaveInput1Schedule();
}

void CancelInputScheduleMode2()
{
	g_ScheduleInput2=INPUTSOURCE_NONE;
	g_InputTime2[0]=0xff;
	g_InputTime2[1]=0xff;
	SaveInput2Schedule();
}

void CancelInputScheduleMode3()
{
	g_ScheduleInput3=INPUTSOURCE_NONE;
	g_InputTime3[0]=0xff;
	g_InputTime3[1]=0xff;
	SaveInput3Schedule();
}

BOOL CancelTimeSchedule(EventValue Val,State currentState,U16 wValue)
{
	//已取消
	U8 CancelOK[]={
		0xD2,0xD1,0xC8,0xA1,0xCF,0xFB,0
	};
	switch(g_CurrentState)
	{
		case State_TimeSchedule_CancelInput1:
			CancelInputScheduleMode1();
			break;
		case State_TimeSchedule_CancelInput2:
			CancelInputScheduleMode2();
			break;
		case State_TimeSchedule_CancelInput3:
			CancelInputScheduleMode3();
			break;
	#if(ENABLE_SCHEDULE_MODE)
		case State_TimeSchedule_CancelMode1:
			break;
		case State_TimeSchedule_CancelMode2:
			break;
		case State_TimeSchedule_CancelMode3:
			break;
	#endif
		case State_InputCycle_Cancel:
			g_InputCycleIdx=0;
			g_StartCycleInputFlag=__FALSE;
			initCycleInputPara();
			Write_EEPROM_Byte(E2ROM_CycleFlag_ADDR, g_StartCycleInputFlag);
			break;
		case State_HotBackup_Cancel:
			initHotBackupVariable(__FALSE);
			Write_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,g_EnableHotBackupFlag);
			Write_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,g_HotBackupIdx);
			Write_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
			break;
	}
	chinesedisplay(0x90, "*");
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x86, CancelOK);
	else  if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x86, "Cancel success");
	return __TRUE;
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

BOOL SetTimeSchedule(EventValue Val,State currentState,U16 wValue)
{
	if(Val==KEY_ENTER)
	{
		switch(currentState)
		{
			case State_TimeSchedule_Begin_Input1:
				g_SetTimeFlag=Set_Input_Schedule_Idx;
				g_InputScheduleModeValue=MODE1;
				//显示已经设置好的方案，如果已执行就不显示了
				if(g_ScheduleInput1!=INPUTSOURCE_NONE)
				{					
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
					DisplayMainInput(0x84,g_ScheduleInput1);					
					if(g_InputTime1[0]<24||g_InputTime1[1]<60)
					{
						DisTwoNumValue(g_InputTime1[0],HOURADDR);
						DisTwoNumValue( g_InputTime1[1],MINADDR);
					}
					g_CurrentState=State_Input_MIN;
					g_MinIdx=1;
				}
				else
				{
					DealCurrentState(currentState);
					chinesedisplay(0x84,"_");
				}
				break;
			case State_TimeSchedule_Begin_Input2:
				g_SetTimeFlag=Set_Input_Schedule_Idx;
				g_InputScheduleModeValue=MODE2;				
				//显示已经设置好的方案，如果已执行就不显示了
				if(g_ScheduleInput2!=INPUTSOURCE_NONE)
				{
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
					DisplayMainInput(0x84,g_ScheduleInput2);
					if(g_InputTime1[0]<24||g_InputTime1[1]<60)
					{
						DisTwoNumValue(g_InputTime2[0], HOURADDR);
						DisTwoNumValue( g_InputTime2[1],MINADDR);
					}
					g_CurrentState=State_Input_MIN;
					g_MinIdx=1;
				}
				else
				{
					DealCurrentState(currentState);
					chinesedisplay(0x84,"_");
				}
				break;
			case State_TimeSchedule_Begin_Input3:
				g_SetTimeFlag=Set_Input_Schedule_Idx;
				g_InputScheduleModeValue=MODE3;
				//显示已经设置好的方案，如果已执行就不显示了
				if(g_ScheduleInput3!=INPUTSOURCE_NONE)
				{
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
					DisplayMainInput(0x84,g_ScheduleInput3);
					if(g_InputTime1[0]<24||g_InputTime1[1]<60)
					{
						DisTwoNumValue(g_InputTime3[0],HOURADDR);
						DisTwoNumValue(g_InputTime3[1],MINADDR);
					}
					g_CurrentState=State_Input_MIN;
					g_MinIdx=1;
				}
				else
				{
					chinesedisplay(0x84,"_");
					DealCurrentState(currentState);
				}
				break;
		#if(ENABLE_SCHEDULE_MODE)
			case State_TimeSchedule_BeginMode1:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE1;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_UserMode+4);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_UserModeEn+4);
				printStr(":");
				break;
			case State_TimeSchedule_BeginMode2:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE2;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_UserMode+4);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_UserModeEn+4);
				printStr(":");
				break;
			case State_TimeSchedule_BeginMode3:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE3;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_UserMode+4);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_UserModeEn+4);
				printStr(":");
				break;
		#endif
			case State_InputCycle_Set:
			{
				U8 ch[4]={0};
				g_SetTimeFlag=Set_InputCycle_Idx;			
				clearlcd();
				switch(g_InputCycleIdx)
				{
					case 0:
						g_InputCycleIdx++;
						break;
					case 1:
						if(g_CycleInput1!=INPUTSOURCE_NONE)
							g_InputCycleIdx++;
						break;
					case 2:
						if(g_CycleInput2!=INPUTSOURCE_NONE)
							g_InputCycleIdx++;
						break;
					case 3:
						if(g_CycleInput3!=INPUTSOURCE_NONE)
							g_InputCycleIdx++;
						break;
					case 4:
						if(g_CycleInput4!=INPUTSOURCE_NONE)
							g_InputCycleIdx++;
						break;
					case 5:
						if(g_CycleInput5!=INPUTSOURCE_NONE)
							g_InputCycleIdx++;
						break;
				}	
				AddInputCycle();
				wcom(0x83);
				ch[0]=g_InputCycleIdx+48;
				ch[1]=':';
				ch[2]='_';
				printStr(ch);
			}
			break;
			case State_User_HotBackup:
				if(!g_SetBackupInputFlag)
				{
					if(g_HotBackupInput[g_HotBackupIdx][0]!=INPUTSOURCE_NONE)
					{
						g_SetBackupInputFlag=__TRUE;
						wcom(0x96);
						printStr("_");
						
					}
				}
				else
				{
					if(g_HotBackupInput[g_HotBackupIdx][1]!=INPUTSOURCE_NONE)
					{
						g_SetBackupInputFlag=__FALSE;
						g_HotBackupIdx++;
						Write_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,g_HotBackupIdx);
						DealCurrentState(State_User_HotBackup_Continue);
					}
				}
				break;
			default:
				return __FALSE;		
		}
	}
	return __TRUE;
}

BOOL DebugSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	value=atoi(Number1,Number1Idx);
	InputNumReset();
	if(currentState==State_DebugMode_Start)
	{
		switch(value)
		{
			case 1://设置当前时间
				g_SetTimeFlag=Set_Current_Time_Idx;
				SetCurrentTime();
				break;
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
					g_CurrentState=State_Input_YEAR;
					clearlcd();
					chinesedisplay(0x80, g_DeadlineFmtStr);
				}
					//DealCurrentState(State_Try_Use);
				break;
			case 5://查看机器ID
				DealCurrentState(State_MainMenu_Menu_IDInfo);
				DisMathineIDValue(ChipUniqueID,0x84);				
				break;
			case 6://开启同步切换双画面
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
				break;
				
			case 11:
				DealCurrentState(State_Input_PackagingName);
				break;
			case 12:
				DealCurrentState(State_Input_TestName);
				break;
			case 13:
				DealCurrentState(State_Input_SaleName);
				break;

			case 21:
				clearlcd();
				chinesedisplay(0x80, g_InputPackagingNameStr+6);
				if(g_Pack_ID/1000!=0)
					DisTwoNumValue(g_Pack_ID/1000, 0x85);
				DisFourNumValue(g_Pack_ID%1000, 0x86);
				chinesedisplay(0x90, g_PackagingName);
				break;
			case 22:
				clearlcd();
				chinesedisplay(0x80, g_InputTestNameStr+6);
				if(g_Test_ID/1000!=0)
					DisTwoNumValue(g_Test_ID/1000, 0x85);
				DisFourNumValue(g_Test_ID%1000, 0x86);
				chinesedisplay(0x90, g_TestName);
				break;
			case 23:
				clearlcd();
				chinesedisplay(0x80, g_InputSaleNameStr+6);
				if(g_Sale_ID/1000!=0)
					DisTwoNumValue(g_Sale_ID/1000, 0x85);
				DisFourNumValue(g_Sale_ID%1000, 0x86);
				chinesedisplay(0x90, g_SaleName);
				break;
				
			case 70://施工方设置公司logo
				DealCurrentState(State_User_Input_Logo);
				break;
			case 71://施工方设置技术支持电话
				DealCurrentState(State_User_Set_Tech_TelNumber);
				break;
			case 72://修改机器型号
				DealCurrentState(State_User_Input_MathineType);
				break;
				
			case 80://施工方设置试用密码
				g_InputOldSeriesForWhat=0;
				if(g_UserSetPasswordVal<1000000)
					DealCurrentState(State_User_Old_SerialNumber);
				else
					DealCurrentState(State_User_Set_SerialNumber);
				break;
			case 81://施工方设置试用天数
				g_InputOldSeriesForWhat=1;
				if(g_UserSetPasswordVal<1000000)
					DealCurrentState(State_User_Old_SerialNumber);
				else			
					DealCurrentState(State_User_Input_TryDays);
				break;
			case 82://施工方设置联系电话
				DealCurrentState(State_User_Set_TelNumber);
				break;
			case 83://输入序列号，解除试用
				DealCurrentState(State_User_Input_SerialNumber);
				break;



			//add 20160327
			case 86:
				#ifdef LOGWithoutKEY
					DealCurrentState(State_HowLong_Save);
				#else
					g_InputOldSeriesForWhat=4;//输入高级密码或用户密码，设置有效记录时间
					DealCurrentState(State_User_Old_SerialNumber);
				#endif				
				break;
				
			case 87:
				if(ReadLog())
				{
					clearlcd();
					chinesedisplay(0x80,g_LogHadReadOut);
					DisTwoNumValue(g_NumSaved,0x86);
				}
				else
				{
					clearlcd();
					chinesedisplay(0x80,g_noLog);
					
				}
				break;
			case 88:	
				//#ifdef LOGWithoutKEY
					//ClearLog();
					clearlcd(); //清除LCD
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x93,  g_waitStr);	
					else if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x93,  g_waitStrEn);	
					ClearLog();
					LcdDisplaySetOK_ReturnMain();					
				//#else
				//	g_InputOldSeriesForWhat=6;//输入高级密码或用户密码，清除记录
				//	DealCurrentState(State_User_Old_SerialNumber);	
				//#endif
				
				break;

			case 89://open log recording function
				g_EnableRecordLog=1;
				Write_EEPROM_Byte(E2ROM_EnableRecordLog_ADDR,g_EnableRecordLog);
				//chinesedisplay(0x80,"OPEN LOG FUNC");
				chinesedisplay(0x80,g_openLog);
				
				Read_EEPROM_Byte(E2ROM_EnableRecordLog_ADDR,&g_EnableRecordLog);
				//printf("   g_EnableRecordLog READ OUT:%d \n",g_EnableRecordLog);
				break;
			case 90://close log recording function
				g_EnableRecordLog=0;
				Write_EEPROM_Byte(E2ROM_EnableRecordLog_ADDR,g_EnableRecordLog);
				//chinesedisplay(0x80,"CLOSE LOG FUNC");
				chinesedisplay(0x80,g_closedLog);
				break;	


			case 91://close log recording function
				clearlcd();				
				if(g_EnableRecordLog)
					{
					chinesedisplay(0x80,"Log   ;");
					chinesedisplay(0x82,g_charOpen);
					}
				else
					{
					chinesedisplay(0x80,"Log   ;");
					chinesedisplay(0x82,g_charClose);
					}

				chinesedisplay(0x86,"Numb:");
				DisTwoNumValue(g_NumSaved,0x88);
				

				chinesedisplay(0x90,"Log After:  min");
				DisTwoNumValue(g_howlongtosave,0x95);
				break;


				
			default:
				return __FALSE;
		}
	}
	else if(currentState==State_LockPadTime_Set)
	{
		if(value>59)
			value=59;
		g_LockKeypadTime=value;
		DisCurWinSizeValue(value, 0x90, '*');
		Write_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR,value);
	}
	return __TRUE;
}
//diect user set up 



BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	value=atoi(Number1,Number1Idx);
	if(currentState==State_NewMode_Set_HSize)
	{/*
		if(value>1920)
		{
			AdviceForUser();
			delay(10);
			InputFourNumReset();
			clearlcd();
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x83, g_DirectSetStr);
				chinesedisplay(LCDADDR90, g_LEDHSizeStr);
			}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x83, g_DirectSetStrEn);
				chinesedisplay(LCDADDR90, g_LEDHSizeStrEn);
			}
			return __FALSE;
		}*/
		g_ScreenHSize=value;
		g_MainScalerHSize=value;
		if(g_MutiWindow==SAMEPIPIMAGE)
		{
			g_SubScalerHSize=value;
		}
	}
	else if(currentState==State_NewMode_Set_VSize)
	{/*
		if(value>1200)
		{
			AdviceForUser();
			delay(10);
			InputFourNumReset();
			clearlcd();
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x83, g_DirectSetStr);
				chinesedisplay(LCDADDR90, g_LEDVSizeStr);
			}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x83, g_DirectSetStrEn);
				chinesedisplay(LCDADDR90, g_LEDVSizeStrEn);
			}
			return __FALSE;
		}*/
		g_ScreenVSize=value;
		g_MainScalerVSize=value;
		if(g_MutiWindow==SAMEPIPIMAGE)
		{
		g_SubScalerVSize=value;
		}
	}
	InputNumReset();
	return __TRUE;
}

#if(MATHINE_TYPE==XP_530)
PortID GetAdv7441InputPort(U8 cnt)
{
	switch(cnt)
	{
	#if(INPUT_BOARD==ADV7441A_INPUT)
		case NUM6_KEY:
			return DEVICE_PORT_CVBS1;
		case NUM7_KEY:
			return DEVICE_PORT_CVBS2;
		case NUM8_KEY:
			return DEVICE_PORT_VGA;
		case NUM9_KEY:
			return DEVICE_PORT_DVI;
		case NUM0_KEY:
			return DEVICE_PORT_HDMI;
	#elif(INPUT_BOARD==SII9125_INPUT)
		case NUM9_KEY:
			return DEVICE_PORT_DVI;
		case NUM0_KEY:
			return DEVICE_PORT_HDMI;
	#endif
		default:
			return DEVICE_PORT_DVI;
			
	}
}
#endif

//-------------------------deal input source number----------------------------------
void DealInputNumber(U8 cnt,U8 addr)
{	
#if(MATHINE_TYPE==XP_530)
	PortID adv7441input=g_Adv7441CurrentInput;
#endif
	_InputSourceType input;
	input=NumKeyToInputSrc(cnt);	
	if(input==INPUTSOURCE_NONE)
		return;
#if(MATHINE_TYPE==XP_530)
	else if(input==INPUTSOURCE_SDI)
	{
		adv7441input=GetAdv7441InputPort(cnt);
	}
#endif
	switch(g_CurrentState)
	{
		case State_NewMode_Set_InputSource:
	#if(MATHINE_TYPE==XP_530)
			g_Adv7441CurrentInput=adv7441input;
			Write_EEPROM_Byte(E2ROM_ADV7441A_INPUT_ADDR, (U8)g_Adv7441CurrentInput);
	#endif
			g_MainInput=input;
			break;
		case State_TimeSchedule_Begin_Input1:
			g_ScheduleInput1=input;
			g_SetInputScheFlag=INSRCOVER;
			break;
		case State_TimeSchedule_Begin_Input2:
			g_ScheduleInput2=input;
			g_SetInputScheFlag=INSRCOVER;
			break;
		case State_TimeSchedule_Begin_Input3:
			g_SetInputScheFlag=INSRCOVER;
			g_ScheduleInput3=input;
			break;
		case State_InputCycle_Set:
			switch(g_InputCycleIdx)
			{
				case 1:
					g_CycleInput1=input;
					break;
				case 2:
					if(input==g_CycleInput1)
						return;
					g_CycleInput2=input;
					break;
				case 3:
					if(input==g_CycleInput1||input==g_CycleInput2)
						return;
					g_CycleInput3=input;
					break;
				case 4:
					if(input==g_CycleInput1||input==g_CycleInput2||input==g_CycleInput3)
						return;
					g_CycleInput4=input;
					break;
				case 5:
					if(input==g_CycleInput1||input==g_CycleInput2||input==g_CycleInput3||input==g_CycleInput4)
						return;
					g_CycleInput5=input;
					break;
				case 6:
					if(input==g_CycleInput1||input==g_CycleInput2||input==g_CycleInput3||input==g_CycleInput4||input==g_CycleInput5)
						return;
					g_CycleInput6=input;
					break;
			}
			g_SetInputScheFlag=INSRCOVER;
			break;
		case State_User_HotBackup:
			if(!g_SetBackupInputFlag)
			{
				int i;
				for(i=0;i<g_HotBackupIdx;i++)  //判断当前输入的信号源是否已设置备份，是就返回
				{
					if(input==g_HotBackupInput[i][0])
						return;
				}
				g_HotBackupInput[g_HotBackupIdx][0]=input;
			}
			else
			{				
				g_HotBackupInput[g_HotBackupIdx][1]=input;
			}
			break;
	}
	#ifndef EC11_BUTTON
	//开启提示灯 提示用户按OK
	Start_LED_Flashing(1);
	g_FlashLedKey[0]=OK_KEY;
	#endif
	chinesedisplay(addr, "      ");
	DisplayMainInput(addr,input);	
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

BOOL ParseAndSetMode()
{	
	U16 HSize=0,VSize=0;
	
	if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_VGA)
	{
		GetInputSrcFormat(MAIN,&HSize,&VSize);
		delay(3);
	}
	if(HSize!=0&&VSize!=0&&g_ScreenHSize<=HSize&&g_ScreenVSize<=VSize)
	{//输出源为电脑，并且大屏得点数小于电脑的分辨率
		if((HSize==640&&VSize==480)||(HSize==800&&VSize==600)||(HSize==1024&&VSize==768)||(HSize==1280&&VSize==1024)
			||(HSize==1366&&VSize==768)||(HSize==1440&&VSize==900)||(HSize==1600&&VSize==1200)||(HSize==1680&&VSize==1050)
			||(HSize==1920&&VSize==1080)||(HSize==1280&&VSize==720)||(HSize==1920&&VSize==1200)||(HSize==2048&&VSize==1152))
		{
			switch(HSize)
			{
				case 640:
				g_OutputFormatIndex=INDEX640X480;
					break;
				case 800:
				g_OutputFormatIndex=INDEX800X600;
					break;
				case 1024:
				g_OutputFormatIndex=INDEX1024X768;
					break;
				case 1280:
					if(VSize==720)
						g_OutputFormatIndex=INDEX1280X720;
					else if(VSize==1024)
						g_OutputFormatIndex=INDEX1280X1024;
					break;
				case 1366:
				g_OutputFormatIndex=INDEX1366X768;
					break;
				case 1440:
				g_OutputFormatIndex=INDEX1440X900;
					break;
				case 1600:
				g_OutputFormatIndex=INDEX1600X1200;
					break;
				case 1680:
				g_OutputFormatIndex=INDEX1680X1050;
					break;
				case 1920:
					if(VSize==1080)
						g_OutputFormatIndex=INDEX1920X1080;
					else if(VSize==1200)
						g_OutputFormatIndex=INDEX1920X1200;
					break;
				case 2048:
					g_OutputFormatIndex=INDEX2048x1152;
					break;
			}
		}
		else
		{
			g_OutputFormatIndex=0;
			UserResolutionHSize=HSize;
			UserResolutionVSize=VSize;
		}
	}
	else
	{
		if(g_ScreenHSize<=640&&g_ScreenVSize<=480)
		{
			g_OutputFormatIndex=INDEX640X480;
		}
		else if(g_ScreenHSize<=800&&g_ScreenVSize<=600)
		{
			g_OutputFormatIndex=INDEX800X600;
		}
		else if(g_ScreenHSize<=1024&&g_ScreenVSize<=768)
		{
			g_OutputFormatIndex=INDEX1024X768;
		}
		else if(g_ScreenHSize<=1280&&g_ScreenVSize<=720)
		{
			g_OutputFormatIndex=INDEX1280X720;
		}
		else if(g_ScreenHSize<=1280&&g_ScreenVSize<=1024)
		{
			g_OutputFormatIndex=INDEX1280X1024;
		}
		else if(g_ScreenHSize<=1366&&g_ScreenVSize<=768)
		{
			g_OutputFormatIndex=INDEX1366X768;
		}
		else if(g_ScreenHSize<=1440&&g_ScreenVSize<=900)
		{
			g_OutputFormatIndex=INDEX1440X900;
		}
		else if(g_ScreenHSize<=1600&&g_ScreenVSize<=1200)
		{
			g_OutputFormatIndex=INDEX1600X1200;
		}
		else if(g_ScreenHSize<=1680&&g_ScreenVSize<=1050)
		{
			g_OutputFormatIndex=INDEX1680X1050;
		}
		else if(g_ScreenHSize<=1920&&g_ScreenVSize<=1080)
		{
			g_OutputFormatIndex=INDEX1920X1080; 
		} 
		else if(g_ScreenHSize<=1920&&g_ScreenVSize<=1200)
		{
			g_OutputFormatIndex=INDEX1920X1200 ;
		}
		else if(g_ScreenHSize<=2048&&g_ScreenVSize<=1152)
		{
			g_OutputFormatIndex=INDEX2048x1152 ;
		}
		else
		{
			g_OutputFormatIndex=0;
			UserResolutionHSize=g_ScreenHSize;
			UserResolutionVSize=g_ScreenVSize;
		}
		//else
		//	return __FALSE;
	}
	SetOutputFormat(g_OutputFormatIndex);	
	//LoadOutputResolution();
	//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
	//if(g_OutputFormatIndex==0)
	//	Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
	
	if(!CheckDualSource( g_MainInput-1,g_SubInput-1))
	{
		g_SubInput=INPUTSOURCE_NUM;
	}
	if(g_MutiWindow==PBPIMAGE||g_MutiWindow==PIPIMAGE)
	{
		SetInputSource(g_SubInput, SUB);	
	}
	SetInputSource(g_MainInput, MAIN);
	
	SetScalerHVValue(HVSIZE,MAIN);
	if(g_MutiWindow==SAMEPIPIMAGE)
	{
		g_SubScalerHSize=g_ScreenHSize;
		g_SubScalerVSize=g_ScreenVSize;
		SetScalerHVValue(HVSIZE,SUB);
	}
	else if(g_MutiWindow==PBPIMAGE)
	{
		SetScalerHVValue(HVSIZE,SUB);
		g_SubScalerHPos=g_MainScalerHSize;
		g_SubScalerVPos=0;
		SetScalerHVValue(HVPOS,SUB);
	}
	Save(DEFAULT_MODE);
	return __TRUE;
}
/*
//SWAP user Mode
void ChangeUserMode()
{
	U8 UserModeIdx;
	UserModeIdx=SaveMode[0]-48;
	if(TwoMode[0]==UserModeIdx&&TwoMode[1]!=UserModeIdx&&TwoMode[1]<10)
	{
		DealSaveAndRecallNumber(IntToNumKey(TwoMode[1]));
	}
	else if(TwoMode[1]==UserModeIdx&&TwoMode[0]!=UserModeIdx&&TwoMode[0]<10)
	{
		DealSaveAndRecallNumber(IntToNumKey(TwoMode[0]));
	}
}
*/
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
		
}
void BackSpace()   //set current time backspace
{
	if(g_CurrentState==State_Input_MONTH)
	{
		chinesedisplay(MONTHADDR, "  ");	
		if(g_MonthIdx==0)
		{
			g_CurrentState=State_Input_YEAR;
			if(Number1Idx>0)
				Number1Idx--;
			g_Number1OverFlag=0;
			chinesedisplay(YEARADDR, "    ");	
			Number1[Number1Idx]=0;
			chinesedisplay(YEARADDR, Number1);
		}
		else if(g_MonthIdx==1)
		{
			if(g_MonthIdx>0)
				g_MonthIdx--;
			g_MonthOverFlag=0;
			g_Month[g_MonthIdx]=0;
			chinesedisplay(MONTHADDR, g_Month);
		}
		printStr("_");
	}
	else if(g_CurrentState==State_Input_DOM)
	{
		chinesedisplay(DAYADDR, "  ");	
		if(g_DateIdx==0)
		{
			g_CurrentState=State_Input_MONTH;
			if(g_MonthIdx>0)			
				g_MonthIdx--;
			g_MonthOverFlag=0;
			chinesedisplay(MONTHADDR, "  ");	
			g_Month[g_MonthIdx]=0;
			chinesedisplay(MONTHADDR, g_Month);
		}
		else if(g_DateIdx==1)
		{
			g_DateIdx--;
			g_DateOverFlag=0;
			g_Date[g_DateIdx]=0;
			chinesedisplay(DAYADDR, g_Date);
		}
		printStr("_");
	}
	else if(g_CurrentState==State_Input_HOUR)
	{
		chinesedisplay(HOURADDR, "  ");	
		if(g_HourIdx==0)  
		{	
			if(g_SetTimeFlag==Set_Current_Time_Idx)//if set current time can back space date
			{
				g_CurrentState=State_Input_DOM;
				if(g_DateIdx>0)
					g_DateIdx--;
				g_DateOverFlag=0;
				chinesedisplay(DAYADDR, "  ");	
				g_Date[g_DateIdx]=0;
				chinesedisplay(DAYADDR, g_Date);
				printStr("_");
			}
			else if(g_SetTimeFlag==Set_Input_Schedule_Idx)
			{
				if(g_InputScheduleModeValue==MODE1)
				{
					g_CurrentState=State_TimeSchedule_Begin_Input1;
					g_ScheduleInput1=INPUTSOURCE_NONE;
				}
				else if(g_InputScheduleModeValue==MODE2)
				{
					g_CurrentState=State_TimeSchedule_Begin_Input2;
					g_ScheduleInput2=INPUTSOURCE_NONE;
				}
				else if(g_InputScheduleModeValue==MODE3)
				{
					g_CurrentState=State_TimeSchedule_Begin_Input3;
					g_ScheduleInput3=INPUTSOURCE_NONE;
				}
				chinesedisplay(0x84, "            ");
				wcom(0x84);
				printStr("_");
			}
			else
			{
				chinesedisplay(HOURADDR, "_ ");
			}
		}
		else if(g_HourIdx==1)
		{
			g_HourIdx--;
			g_HourOverFlag=0;
			g_Hour[g_HourIdx]=0;
			chinesedisplay(HOURADDR, g_Hour);
			printStr("_");
		}
	}
	else if(g_CurrentState==State_Input_MIN)
	{
		chinesedisplay(MINADDR, "  ");	
		if(g_MinIdx==0)
		{
			g_CurrentState=State_Input_HOUR;
			if(g_HourIdx>0)
				g_HourIdx--;
			g_HourOverFlag=0;
			chinesedisplay(HOURADDR, "  ");	
			g_Hour[g_HourIdx]=0;
			chinesedisplay(HOURADDR, g_Hour);
		}
		else if(g_MinIdx>0)
		{
			if(g_MinIdx>0)
				g_MinIdx--;
			g_MinOverFlag=0;
			g_Min[g_MinIdx]=0;
			chinesedisplay(MINADDR, g_Min);
		}
		printStr("_");
	}
	else if(g_CurrentState==State_Input_SEC)
	{
		chinesedisplay(SECONDADDR, "  ");	
		if(g_SecondIdx==0)
		{
			g_CurrentState=State_Input_MIN;
			if(g_MinIdx>0)
				g_MinIdx--;
			g_MinOverFlag=0;
			chinesedisplay(MINADDR, "  ");	
			g_Min[g_MinIdx]=0;
			chinesedisplay(MINADDR, g_Min);
		}
		else if(g_SecondIdx>0)
		{
			g_SecondIdx--;
			g_SecondOverFlag=0;
			g_Second[g_SecondIdx]=0;
			chinesedisplay(SECONDADDR, g_Second);
		}
		printStr("_");
	}
}

void BackScheWeekNum()
{
	if(g_CurrentState==State_ScheduleInput1_InputWeek)
	{
		g_InputWeek1Idx--;
		g_InputWeek1[g_InputWeek1Idx]=0;
		chinesedisplay(0x90+g_InputWeek1Idx,"_");
		chinesedisplay(0x90+g_InputWeek1Idx+1," ");
	}
	else if(g_CurrentState==State_ScheduleInput2_InputWeek)
	{
		g_InputWeek2Idx--;
		g_InputWeek2[g_InputWeek2Idx]=0;
		chinesedisplay(0x90+g_InputWeek2Idx,"_");
		chinesedisplay(0x90+g_InputWeek2Idx+1," ");
	}
	else if(g_CurrentState==State_ScheduleInput3_InputWeek)
	{
		g_InputWeek3Idx--;
		g_InputWeek3[g_InputWeek3Idx]=0;
		chinesedisplay(0x90+g_InputWeek3Idx,"_");
		chinesedisplay(0x90+g_InputWeek3Idx+1," ");
	}

}

void ClearScheWeekNum()
{
	U8 i=0;
	for(i=0;i<7;i++)
	{
		g_InputWeek1[i]=0;
		g_InputWeek2[i]=0;
		g_InputWeek3[i]=0;
	}
	g_InputWeek1Idx=0;
	g_InputWeek2Idx=0;
	g_InputWeek3Idx=0;	
}


