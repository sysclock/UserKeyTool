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
#ifdef DEBUG_STATE_MSG
//char *dirs[] = { "North", "East", };
char* state_string[]={
	"State_MainMenu_Menu",		//0
	"State_MainMenu_Menu_StartInfo",	
	"State_MainMenu_Menu_VersionInfo",
	"State_MainMenu_Menu_IDInfo",	
	"State_MainMenu_Menu_InOutput",
	"State_MainMenu_Menu_In",//5
	"State_MainMenu_Menu_Current_Date",
	"State_MainMenu_Menu_Logo",
#ifdef POWER_ON_PWD
	"State_MainMenu_Poweron_PassWord",
#endif
	"State_MainMenu_Not_Menu",	
	//Full or Part
	"State_FullOrPart_Full",//10	
	"State_FullOrPart_Part",	
	//multi window
	"State_MultiWindow_Single",
	"State_MultiWindow_PBP",               
	"State_MultiWindow_PIP",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_MultiWindow_SAMEPIP",//15
#endif
	//Main or Sub
	"State_ActiveImage_Main",
	"State_ActiveImage_Sub",
	
	//
	"State_SavePara_Number",
	"State_LoadUserPara_Number",
	//screen parameter
	"State_MainMenu_Menu_ScreenPara",    //20
	"State_ScreenPara_Size",
	"State_ScreenPara_Size_Value",
	"State_ScreenPara_Pos",                    
	"State_ScreenPara_Pos_Value",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_ScreenPara_UserResolution",                     //25
	"State_ScreenPara_UserResolution_Value",
#endif
	//scaler parameter
	"State_MainMenu_Menu_ScalerPara",
	"State_ScalerPara_Size",
	"State_ScalerPara_Size_Value",
	"State_ScalerPara_Pos",//30
	"State_ScalerPara_Pos_Value",
	//Zoom parameter
	"State_MainMenu_Menu_ZoomPara",
	"State_ZoomPara_Size",
	"State_ZoomPara_Size_Value",
	"State_ZoomPara_Pos",//35
	"State_ZoomPara_Pos_Value",
	"State_ZoomPara_Input",
	"State_ZoomPara_Input_Format",
	//Color parameter
	"State_MainMenu_Menu_ColorPara",
	"State_ColorPara_Brightness",//40
	"State_ColorPara_Brightness_Value",           
	"State_ColorPara_Contrast",
	"State_ColorPara_Contrast_Value",

#if(XP_TO_ID==XP_TO_FLI32626)
	"State_ColorPara_Saturation",
	"State_ColorPara_Saturation_Value",//45
	"State_ColorPara_Reset",
	"State_ColorPara_Reset_Check",
	"State_ColorPara_ADC",
	"State_ColorPara_ADC_Calibration",
#endif
	//Advance parameter
	"State_MainMenu_Menu_AdvancePara",//50
	//屏幕分辨率
	"State_AdvancePara_OutputFormat",
	"State_OutputFormat_640x480",
	"State_OutputFormat_800x600",                      
	"State_OutputFormat_1024x768",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_OutputFormat_1280x720",//55
#endif
	"State_OutputFormat_1280x720",
	"State_OutputFormat_1280x768",
	"State_OutputFormat_1280x800",
	"State_OutputFormat_1280x960",
	"State_OutputFormat_1280x1024",//60
	"State_OutputFormat_1366x768",
	"State_OutputFormat_1400x1050",
	"State_OutputFormat_1440x900",
	"State_OutputFormat_1600x900",              
	"State_OutputFormat_1600x1200",      //65       
	"State_OutputFormat_1680x1050",
	"State_OutputFormat_1920x1080",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_OutputFormat_1920x1200",
	"State_OutputFormat_2048x1152",
#endif
	//刷新频率
	"State_AdvancePara_OutputHz",//70
	"State_OutputHz_30Hz",
	"State_OutputHz_50Hz",
	"State_OutputHz_60Hz",
	"State_OutputHz_NumHz",
	"State_FreezeImage_Check",//75
	
	"State_AdvancePara_Audio",
	"State_Audio_Main",
	"State_Audio_Sub",
	"State_AdvancePara_LockPadTime",
	"State_LockPadTime_Set",//80
	//Factory Reset parameter
	"State_AdvancePara_FactoryReset",
	"State_FactoryReset_Yes",
	"State_FactoryReset_OK",
	"State_AdvancePara_Wizard",
	//Language parameter
	"State_AdvancePara_Language",//85
	"State_Language_English",
	"State_Language_Chinese",


////六、用户设置
	"State6_MainMenu_Menu_UserPara",
	"State_UserPara_SaveMode",
	"State_UserPara_RecallMode",//90
	"State63_UserPara_SetCurrentTime",
	"State_UserPara_SetFadeTime",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_UserPara_SetFadeTime_500",
	"State_UserPara_SetFadeTime_1000",
	"State_UserPara_SetFadeTime_1500",//95
#endif
	"State_UserPara_SetHotBackup",
	"State_HotBackup_Set",
	"State_HotBackup_Cancel",
	"State_HotBackup_Check",
	"State_User_HotBackup",//100
	"State_User_HotBackup_Continue",
	"State_UserPara_SetStaticIP",
	"State_UserPara_SetStaticIP_Value",	
	"State_SavePara_Mode_check",
	
	//七、Time schedule
	"State_MainMenu_TimeSchedule",//105
	"State_TimeSchedule_InputSource",
	"State_TimeSchedule_Input1",
	"State_TimeSchedule_BeginInput1",
	"State_TimeSchedule_Begin_Input1",
	"State_ScheduleInput1_Once",//110
	"State_ScheduleInput1_EveryDay",
	"State_ScheduleInput1_InputWeek",
	"State_TimeSchedule_CancelInput1",
	"State_TimeSchedule_Input2",
	"State_TimeSchedule_BeginInput2",//115
	"State_TimeSchedule_Begin_Input2",
	"State_ScheduleInput2_Once",
	"State_ScheduleInput2_EveryDay",
	"State_ScheduleInput2_InputWeek",
	"State_TimeSchedule_CancelInput2",//120
	"State_TimeSchedule_Input3",
	"State_TimeSchedule_BeginInput3",
	"State_TimeSchedule_Begin_Input3",
	"State_ScheduleInput3_Once",
	"State_ScheduleInput3_EveryDay",//125
	"State_ScheduleInput3_InputWeek",
	"State_TimeSchedule_CancelInput3",

#if(ENABLE_SCHEDULE_MODE)
	"State_TimeSchedule_UserMode",
	"State_TimeSchedule_Mode1",
	"State_TimeSchedule_BeginMode1",//130
	"State_TimeSchedule_CancelMode1",
	"State_TimeSchedule_Mode2",
	"State_TimeSchedule_BeginMode2",
	"State_TimeSchedule_CancelMode2",
	"State_TimeSchedule_Mode3",//135
	"State_TimeSchedule_BeginMode3",
	"State_TimeSchedule_CancelMode3",
#endif
	"State_TimeSchedule_InputCycle",
	"State_InputCycle_Set",
	"State_InputCycle_Cancel",//140
	"State_CycleInput_Continue",
	"State_TimeSchedule_CancelAll",
	"State_CancelAll_Yes",
	//TechSuppor parameter
	"State_MainMenu_Menu_TechSupport",
	"State83_TechSuppor_TestPattern",//145
	//State831_TechSuppor_TestPattern_Page",
	"State_TechSuppor_TestPatternR",
	"State_TechSuppor_TestPatternG",
	"State_TechSuppor_TestPatternB",
	"State_TechSuppor_DisableTestPattern",
	
	"State81_TechSuppor_ContactInfo",//150
	"State811_TechSuppor_ContactInfo_Page",	
	"State82_TechSuppor_VersionInfo",
	"State82_TechSuppor_VersionInfo_Page",
	//setup mode
	"State_MainMenu_Menu_DebugMode",
	"State_DebugMode_Check",//155
	"State_DebugMode_Start",
	
	//
	"State_NewMode_Set",
	"State_NewMode_Set_HTotalSize",
	"State_NewMode_Set_VTotalSize",

////  33 new state
"State_NewMode_Set_CardNumber",//160
"State_NewMode_Set_CardLayout",
"State_NewMode_Set_Card1HSize",
"State_NewMode_Set_Card1VSize",
"State_NewMode_Set_Card2HSize",
"State_NewMode_Set_Card2VSize",//165
"State_NewMode_Set_Card3HSize",
"State_NewMode_Set_Card3VSize",
"State_NewMode_Set_Card4HSize",
"State_NewMode_Set_Card4VSize",
"State_NewMode_Set_ImageNumber",//170
"State_NewMode_Set_ImageLayout",
"State_NewMode_Set_Image1Hsize",
"State_NewMode_Set_Image1Vsize",
"State_NewMode_Set_Image1Hpos",
"State_NewMode_Set_Image1Vpos",//175
"State_NewMode_Set_Image1Scr",
"State_NewMode_Set_Image2Hsize",
"State_NewMode_Set_Image2Vsize",
"State_NewMode_Set_Image2Hpos",
"State_NewMode_Set_Image2Vpos",//180
"State_NewMode_Set_Image2Scr",
"State_NewMode_Set_Image3Hsize",
"State_NewMode_Set_Image3Vsize",
"State_NewMode_Set_Image3Hpos",
"State_NewMode_Set_Image3Vpos",//185
"State_NewMode_Set_Image3Scr",
"State_NewMode_Set_Image4Hsize",
"State_NewMode_Set_Image4Vsize",
"State_NewMode_Set_Image4Hpos",
"State_NewMode_Set_Image4Vpos",//190
"State_NewMode_Set_Image4Scr",
"State_NewMode_Set_SaveGuide",


////
	"State_NewMode_Set_InputSource",
	"State_DualImage_Set_MainSize",
	"State_DualImage_Set_SubSize",//195
////设置时间
	"State_Input_YEAR",
	"State_Input_MONTH",
	"State_Input_DAY",
	"State_Input_HOUR",
	"State_Input_MIN",//200
	"State_Input_SECOND",
	"State_Volume_Set",
	"State_Lock_Key",
#if(XP_TO_ID==XP_TO_FLI32626)
	"State_Fade_Switch_Mode",
	"State_Quick_Switch_Mode",//205
	"State_Fuse_Switch_Mode",
	"State_Fuse_Switch_Value",
#endif
//	State_Try_Use,
	"State_User_Input_SerialNumber",
	"State_User_Old_SerialNumber",
	"State_User_Set_SerialNumber",//210
	"State_User_Set_TelNumber",
	"State_User_Set_Tech_TelNumber",
	"State_User_Input_TryDays",
	"State_User_Input_Try7Days",

	"State_User_Input_Logo",//215
	"State_Input_PackagingName",
	"State_Input_TestName",
	"State_Input_SaleName",
	"State_User_Input_MathineType",
	"State_HowLong_Save"//220
};
#endif


SetTimeFlag g_SetTimeFlag=Set_Time_Nothing;
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

static U8 SaveMode[2]={0,0};          //mode ascii
//static U8 TwoMode[2]={10,10};          //two mode recall




_InputSourceType NumKeyToInputSrc(U8 cnt)    //key to input src
{
	switch(cnt)
	{
 	
		case CVBS_KEY:
			return INPUTSOURCE_1DVI;
		case CVBS2_KEY:
			return INPUTSOURCE_2DVI;
		case CVBS3_KEY:
			return INPUTSOURCE_3DVI;
		case VGA_KEY:
			return INPUTSOURCE_4DVI;	

		case DVI_KEY:
			return INPUTSOURCE_1VGA;	
		case DVI2_KEY:
			return INPUTSOURCE_2VGA;
		case HDMI_KEY:
			return INPUTSOURCE_3VGA;
		case HDMI2_KEY:
			return INPUTSOURCE_4VGA;

		case VGA2_KEY:
			return INPUTSOURCE_1CVBS;
		case HDMI3_KEY:
			return INPUTSOURCE_2CVBS;	
	
		default:
			return INPUTSOURCE_NONE;
			
	}
}

U8 NumKeyToInputMode(U8 cnt)
{
	switch(cnt)
	{
		case CVBS_KEY:
			return 1;
		case CVBS2_KEY:
			return 2;
		case CVBS3_KEY:
			return 3;
//		case YPBPR_KEY:
//			return INPUTSOURCE_YPBPR;
		case VGA_KEY:
			return 4;	
		case VGA2_KEY:
			return 5;	
		case DVI_KEY:
			return 6;	
		case DVI2_KEY:
			return 7;	
		case HDMI_KEY:
			return 8;	
		case NUM9_KEY:
			return 9;	
		case NUM0_KEY:
			return 10;	
		default:
			return 0xFF;
	}
}

_KeypadNum InputSrcToNumKey(_InputSourceType input)    // number to key
{	
	switch(input)
	{
		/* ////
		case INPUTSOURCE_CVBS:
			return CVBS_KEY;
		case INPUTSOURCE_CVBS2:
			return CVBS2_KEY;
		case INPUTSOURCE_CVBS3:
			return CVBS3_KEY;
//		case INPUTSOURCE_YPBPR:
//			return YPBPR_KEY;
		case INPUTSOURCE_VGA:
			return VGA_KEY;
		case INPUTSOURCE_DVI:
			return DVI_KEY;
	#if(ENABLE_HDMI)
		case INPUTSOURCE_HDMI:
			return HDMI_KEY;
		#if(XP_ID==XP350)
		case INPUTSOURCE_HDMI2:
			return HDMI2_KEY;
		#endif
	#endif
	
	#if(XP_TO_ID==XP_TO_FLI32626)
		case INPUTSOURCE_SDI:
			return SDI_KEY;
	#endif
	*/ ////
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
//	static U8 modeIndex=0;	
		
	if(!NumKeyToInt(&cnt))
	{
		LEDWarning((_KeypadNum)cnt);
		return;			
	}
//	TwoMode[modeIndex++%2]=cnt;
	#ifdef DEBUG_STATE_MSG
		printf("function  DealSaveAndRecallNumber,Line492  iMode= %d;g_CurrentState=%d\r\n",cnt,g_CurrentState);
	#endif
	SaveMode[0]=cnt+48;

	switch(g_CurrentState)
		{

		case State_SavePara_Number:
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x84, g_SingleOrDual+8);
					else  if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x84, g_SingleOrDualEn+8);
					printf("function  DealSaveAndRecallNumber,State_SavePara_Number");
					break;
					
		case State_LoadUserPara_Number:
					clearlcd();
					printf("function  DealSaveAndRecallNumber,State_LoadUserPara_Number");
					NumKeySetUserMode();
					break;	
		

	}


	
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x90, g_ModeStr);
	}
	else  if(g_LanguageFlag==ENGLISH)
	{	
		chinesedisplay(0x90, g_ModeStrEn);
	}

	chinesedisplay(0x92, SaveMode);
}


void NumKeySetUserMode()
{
	
	//if save as mode 0-9

	//SetUserMode(SaveMode[0]-48);
	#ifdef DEBUG_STATE_MSG
	printf("NumKeySetUserMode:\r\n");
	#endif
	Read_EEPROM_WizardPara();
	
	KeyPad_SetupWizard_to_FPGA(SaveMode[0]-48);
}


void NumKeySetSaveMode()
{
	g_SaveModeValue=SaveMode[0]-48;	
	//if save as mode 0-9
	SetSaveMode(g_SaveModeValue);
	printf("NumKeySetSaveMode:\r\n");
	KeyPad_SetupWizard_to_FPGA(g_SaveModeValue);
}


////asc to integ
U32 atoi(U8 *numStr,int Idx)
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

extern _InputSourceType g_InputBack;

BOOL CheckPowerOnPWDNumber()
{
	U32 value;
	U16 valueID,value1,value2,value3;
	if(Number1Idx==0)
		return __FALSE;	
	valueID=ChipUniqueID%10000;
	value=atoi(Number1,Number1Idx);
	
	value1=(valueID<<4)|(valueID>>10);
	value2=((valueID+systmtime.tm_year+systmtime.tm_mon+systmtime.tm_mday)<<10)|((valueID+systmtime.tm_year+systmtime.tm_mon+systmtime.tm_mday)>>4);
	value3=(valueID<<10)|(valueID>>4);
	if(value==((value1^value2)&0x3fff))
	{
		//SetInputSourceSwitch(g_InputBack);
		return __TRUE;
		
	}
	else if(value==((value1^value3)&0x3fff))
	{
		//SetInputSourceSwitch(g_InputBack);
		return __TRUE;
		
	}
	else
		return __FALSE;
	
}

BOOL CheckSerialNumber()
{
	U32 value;
	U16 valueID,value1,value2;
	if(Number1Idx==0)
		return __FALSE;	
	valueID=ChipUniqueID%10000;
	value=atoi(Number1,Number1Idx);

	value1=(valueID<<4)|(valueID>>10);
	value2=(valueID<<10)|(valueID>>4);
	if(value==((value1^value2)&0x3fff))
	{//输入为高级密码
		g_UserSetPasswordVal=1000000;   //清除用户密码
		Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
		return __TRUE;
		
	}
	else if((g_UserSetPasswordVal!=1000000&&value==g_UserSetPasswordVal))
	{
		//g_UserSetPasswordVal=1000000;  //清除密码
		//Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
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
		//InitMathineType();
	}
	else
	{
		for(nTeleIdx=0;nTeleIdx<Number1Idx;nTeleIdx++)
			g_MachineTypeXPStr[nTeleIdx]=Number1[nTeleIdx];
		//g_MachineTypeXPStr[nTeleIdx]=0;
	}
}





void Copy_Logo(U16 temp)
{	
	int nTeleIdx;
	switch(temp)
	{
			/*
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x80,  g_TryOverStr);
				chinesedisplay(0x90,  g_PurchaseStr);
			}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,  "Please payment!");
				chinesedisplay(0x90,  "Tele:");
			}

			*/	


		case 0:					
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_NoEnterpriseStr[nTeleIdx];
			break;
		case 1:					
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise1StrBYCOX[nTeleIdx];
			break;	
		/*
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
			if(g_LanguageFlag==CHINESE)
				{
					for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
						g_WelcomeStr[nTeleIdx]=g_Enterprise301Str[nTeleIdx];
					break;
				}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,  "Huidu Technology");
				//chinesedisplay(0x90,  "Tele:");
			}

			
			if(g_LanguageFlag==CHINESE)
			{
				chinesedisplay(0x80,  g_TryOverStr);
				chinesedisplay(0x90,  g_PurchaseStr);
			}
			else if(g_LanguageFlag==ENGLISH)
			{
				chinesedisplay(0x80,  "Please payment!");
				chinesedisplay(0x90,  "Tele:");
			}

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
		case 8724:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8724Str[nTeleIdx];
			break;
		case 8725:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8725Str[nTeleIdx];
			break;
		case 8726:				
			for(nTeleIdx=0;nTeleIdx<21;nTeleIdx++)
				g_WelcomeStr[nTeleIdx]=g_Enterprise8726Str[nTeleIdx];
			break;
		*/
			
		default:
			break;
	}
	FlashWriteLogo();
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




BOOL InputNumberOver(EventValue Val)
{
//	U8 temp;
	U32 stemp;
	int nTeleIdx;
	switch(g_CurrentState)
	{
		case State_Input_YEAR:
			NumKeySetValue(Val,g_CurrentState);
			return __TRUE;
		case State_Input_MONTH:
			NumKeySetMonth(Val,g_CurrentState);
			return __TRUE;
		case State_Input_DAY:
			NumKeySetDate(Val,g_CurrentState);
			return __TRUE;
		case State_Input_HOUR:
			NumKeySetHour(Val,g_CurrentState);
			return __TRUE;
		case State_Input_MIN:
			NumKeySetMin(Val,g_CurrentState);
			return __TRUE;
		case State_Input_SECOND:
			NumKeySetSecond(Val,g_CurrentState);
			return __TRUE;
		
		#ifdef REC_WORKER_ID
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
				#ifdef DEBUG_STATE_MSG
					printf("func InputNumberOver Debug CMD:70;State_Input_PackagingName State= %d  \r\n", State_Input_PackagingName);
				#endif
				InputFourNumReset();
				//Number1Idx=0;
				//g_Number1OverFlag=0;
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
				InputFourNumReset();
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
				InputFourNumReset();
			}
			break;
		#endif
		
		case State_HowLong_Save:
			if(Number1Idx!=0)
			{	
				g_howlongtosave=atoi(Number1,Number1Idx);
				Write_EEPROM_Byte(E2ROM_HowLongTime_ADDR,g_howlongtosave);
//				printf("g_howlongtosave=%d\n",g_howlongtosave);
				LcdDisplaySetOK_ReturnMain();
				InputFourNumReset();
			}
			break;



		case State_User_Input_TryDays:
			if(Number1Idx!=0)
			{	
				check_Try_End_Time(atoi(Number1,Number1Idx));
				Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);
				LcdDisplaySetOK_ReturnMain();
				InputFourNumReset();
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
				////chinesedisplay(0x90, "USE PC Set Logo!");
				InputFourNumReset();
				#ifdef DEBUG_DEBUGCMD
					printf("func InputNumberOver Debug CMD:70;State_User_Input_Logo State= %d  \r\n", State_User_Input_Logo);
				#endif
				
			}
			break;


			
		case State_User_Input_MathineType:
			if(Number1Idx!=0)
			{	/*
				Write_EEPROM_Byte(E2ROM_MathineType_Num_ADDR, Number1Idx);
				m24xx_write(E2ROM_MathineType_ADDR,Number1, Number1Idx+1);
				Copy_Mathine_Type(Number1, Number1Idx);
				LcdDisplaySetOK_ReturnMain();*/
				chinesedisplay(0x90,"Use PC Set Type!");
				InputFourNumReset();
				//Number1Idx=0;
				//g_Number1OverFlag=0;
			}
			break;

		case State_MainMenu_Poweron_PassWord:
			if(CheckPowerOnPWDNumber())
			{
				g_PowerOnPwdOk=__TRUE;
				clearlcd();			
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_SetSerialNumOKStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, "Password OK!");
//				printf("g_InputBack=%d!",g_InputBack);
				SetInputSourceSwitch(g_InputBack);
				delay(5);
				ReturnMainWin();
			}
			else
			{		
				clearlcd();
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_SetSerialNumNotStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, "Error,Input again!");
				chinesedisplay(0x91, "_");
			}			
			InputFourNumReset();
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
			InputFourNumReset();
			break;

		case State_User_Old_SerialNumber:
			if(Number1Idx!=0)
			{	
				//if(g_UserSetPasswordVal==atoi(Number1,Number1Idx))
				if(CheckSerialNumber())
				{
					switch(g_InputOldSeriesForWhat)
					{
					case 0://if(g_InputOldSeriesForWhat==0)//输入用户密码或高级密码，设置新用户密码
						DealCurrentState(State_User_Set_SerialNumber);
						break;
					case 1://else if(g_InputOldSeriesForWhat==1)//输入高级密码或用户密码，设置试用天数
						DealCurrentState(State_User_Input_TryDays);
						break;
					case 2://else if(g_InputOldSeriesForWhat==2)//输入高级密码或用户密码，设置截止日期
						g_SetTimeFlag=Set_Try_Use_Idx;
						g_CurrentState=State_Input_YEAR;
						clearlcd();
						chinesedisplay(0x80, g_DeadlineFmtStr);
						break;
					case 3://else if(g_InputOldSeriesForWhat==3)//输入高级密码或用户密码，取消开机密码
						g_CurrentState=State_MainMenu_Not_Menu;
						clearlcd();
						chinesedisplay(0x80, "Cancel Password OK!");
						g_PowerOnPwdFlag=__FALSE;
						Write_EEPROM_Byte(E2ROM_PowerOnPwd_Flag_ADDR, g_PowerOnPwdFlag);
						break;
					case 4://else if(g_InputOldSeriesForWhat==4)//输入高级密码或用户密码，设置有效记录时间
						DealCurrentState(State_HowLong_Save);
						break;
					case 5://else if(g_InputOldSeriesForWhat==5)//输入高级密码或用户密码，设置当前时间
						g_SetTimeFlag=Set_Current_Time_Idx;
						SetCurrentTime();
						break;

					case 6:						
						clearlcd(); //清除LCD
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x93,  g_waitStr);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x93,  g_waitStrEn);	
						ClearLog();
						LcdDisplaySetOK_ReturnMain();	
						break;

					
					}
				}
				else	//序列号检查不通过
				{		
					clearlcd();
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x80, g_SetSerialNumNotStr);
					else  if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x80, "Registration failed!");
					chinesedisplay(0x91, "_");
				}
				InputFourNumReset();
			}
			break;

		////用户设置密码
		case State_User_Set_SerialNumber:
			if(Number1Idx!=0)
			{	
				g_UserSetPasswordVal=atoi(Number1,Number1Idx);
				Write_EEPROM_UserPWD(E2ROM_UserPWD_ADDR);
				LcdDisplaySetOK_ReturnMain();	
				InputFourNumReset();
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
				InputFourNumReset();
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
				InputFourNumReset();
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
		return;
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
		if(g_SetTimeFlag==Set_Current_Time_Idx)   //when it is set schedule ,then press ok key will be next state
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
		InputNumberOver(KEY_UP);
	}
}


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
	if(!NumKeyToInt(&cnt))//如果不是数字键，闪灯
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
		if(/*g_CurrentState!=State_User_Set_SerialNumber
			&&g_CurrentState!=State_User_Old_SerialNumber
			&&g_CurrentState!=State_User_Set_TelNumber
			&&g_CurrentState!=State_User_Set_Tech_TelNumber
			&&g_CurrentState!=State_User_Input_TryDays
			&&g_CurrentState!=State_User_Input_Logo*/
			g_CurrentState==State_User_Input_SerialNumber
			||g_CurrentState==State_UserPara_SetStaticIP_Value
			)


			//处理数据，存储数据
			InputNumberOver(KEY_UP);
	}

}




void InputFourNumReset()
{
	Number1Idx=0;
	g_Number1OverFlag=0;
}



////InputNumReset
void InputDateTimeReset()
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
}


BOOL NumKeySetValue(EventValue Val,State curState)
{	
	U16 value;
	if(Number1Idx==0)
		return __FALSE;
	
	value=atoi(Number1,Number1Idx);
	if(g_CurrentState==State_Volume_Set&&value>100)
		value=100;	
	else if((g_CurrentState==State_ScreenPara_Size_Value
		||g_CurrentState==State_ScalerPara_Size_Value
		||g_CurrentState==State_DualImage_Set_MainSize
		||g_CurrentState==State_DualImage_Set_SubSize)
		&&value==0)
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
	if(value==0||value>12)
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
	if(value==0||value>31)
	{
		chinesedisplay(DAYADDR, "_ ");
		g_DateIdx=0;
		g_DateOverFlag=0;
		return;
	}
	SetCurrentStateValue(Val,curState,value);
	//set cusor
	if(g_SetTimeFlag!=Set_Try_Use_Idx)  //如果不是设置截止日期
	{
		chinesedisplay(DAYADDR, "  ");
		chinesedisplay(DAYADDR, g_Date);
		chinesedisplay(HOURADDR, "_"); //提示用户继续输入时间
	}
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



//如果是设置当前时间，小时两位满了之后会自动进入该函数并且DealSecondNumber
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
	//如果设置循环播放的时分均为0，则返回
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
				//g_CurrentState=State_MainMenu_Menu;
				Write_EEPROM_InputCycle(E2ROM_CycleInput6_ADDR,g_CycleInput6,g_CycleInputHour6,g_CycleInputMin6);
				ReturnMainWin();
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
	chinesedisplay(WEEKADDR, "_");
}



void SetCurrentTime()
{
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

	STM32_DisplayTime();
//	g_CurrentState=State_Input_SEC;
//	g_WeekIdx=1;
	
	g_CurrentState=State_Input_YEAR;//State_Input_HOUR;

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
	const U8 CancelOK[]={
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
/*#ifdef HOT_BACKUP
		case State_HotBackup_Cancel:
			initHotBackupVariable(__FALSE);
			Write_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,g_EnableHotBackupFlag);
			Write_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,g_HotBackupIdx);
			Write_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
			break;
#endif*/
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
				if(g_ScheduleInput1!=INPUTSOURCE_NONE)
				{
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
				//显示已经设置好的方案，如果已执行就不显示了
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
				if(g_ScheduleInput2!=INPUTSOURCE_NONE)
				{
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
				//显示已经设置好的方案，如果已执行就不显示了
					DisplayMainInput(0x84,g_ScheduleInput2);
					if(g_InputTime2[0]<24||g_InputTime2[1]<60)
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
				if(g_ScheduleInput3!=INPUTSOURCE_NONE)
				{
					DisplayChangeInputScheTip();
					delay(5);
					DealCurrentState(currentState);
				//显示已经设置好的方案，如果已执行就不显示了
					DisplayMainInput(0x84,g_ScheduleInput3);
					if(g_InputTime3[0]<24||g_InputTime3[1]<60)
					{
						DisTwoNumValue(g_InputTime3[0],HOURADDR);
						DisTwoNumValue(g_InputTime3[1],MINADDR);
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


				
#if(ENABLE_SCHEDULE_MODE)
			case State_TimeSchedule_BeginMode1:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE1;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_ModeStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_ModeStrEn);
				printStr(":");
				break;
			case State_TimeSchedule_BeginMode2:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE2;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_ModeStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_ModeStrEn);
				printStr(":");
				break;
			case State_TimeSchedule_BeginMode3:
				g_SetTimeFlag=Set_Mode_Schedule_Idx;
				g_UserScheduleModeValue=MODE3;
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x80, g_ModeStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x80, g_ModeStrEn);
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
		#ifdef HOT_BACKUP
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
		#endif
			default:
				return __FALSE;		
		}		
		//g_CurrentState=State_Input_HOUR;
	}
	return __TRUE;	

}



////调试命令处理DebugSetValue
BOOL DebugCmdProcess(EventValue Val,State currentState,U16 wValue)
{	
	U16 value;
	U8 TempData1;
	WORD pData=0;
	if(Number1Idx==0)
		return __FALSE;
	value=atoi(Number1,Number1Idx);
	InputDateTimeReset();
	if(currentState==State_DebugMode_Start)
	{
		switch(value)
		{


			////不使用测试图卡
			case 110:														//端口，使能，RGB的颜色
				SetTestPatternColor(0,0x00ff00);
				break;
			
				/*
			INPUT_FPGA SET_TEST_PATTERN_COLOR [OutputPort] [IsEnable] [Color_Value]
			OutputPort: 输出端口
			IsEnable：是否显示测试图，0--不显示测试图，正常显示图像，1--显示测试图
			Color_Value：测试图的颜色， 有效位是24位，分别是RGB，每个颜色8位
			*/


			case 111:
				pData=0xff0000;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;

			case 112:
				pData=0x00ff00;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;
			case 113:
				pData=0x0000ff;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;	

			case 114:
				//pData=0x0000ff;										//端口，使能，RGB的颜色
				TempData1 = InputFPGA_SetTestPatternColor(0, 1, 0x0000ff);
				TempData1 = InputFPGA_SetTestPatternColor(1, 1, 0x00ff00);
				TempData1 = InputFPGA_SetTestPatternColor(2, 1, 0xff0000);
				TempData1 = InputFPGA_SetTestPatternColor(3, 1, 0xffffff);
				break;	
			
			case 115:
				pData=0x00ffff;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;	
			
			case 116:
				pData=0xffff00;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;

			case 117:
				pData=0xff00ff;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;

			case 118:
				pData=0xffffff;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;

			case 119:
				pData=0x000000;										//端口，使能，RGB的颜色
				SetTestPatternColor(1,pData);
				break;	
			


			case 1:
				//g_SetTimeFlag=Set_Current_Time_Idx;
				//SetCurrentTime();
				g_InputOldSeriesForWhat=5;//输入高级密码或用户密码，设置当前时间
				DealCurrentState(State_User_Old_SerialNumber);
				break;
			case 2:
				DealCurrentState(State_LockPadTime_Set);
				break;
			case 3:
				DealCurrentState(State_FactoryReset_Yes);
				break;
			case 4:
				g_InputOldSeriesForWhat=2;//输入高级密码或用户密码，设置截止日期
				//if(g_UserSetPasswordVal<1000000)
					DealCurrentState(State_User_Old_SerialNumber);
				//else	
				//{			
				//	g_SetTimeFlag=Set_Try_Use_Idx;
				//	g_CurrentState=State_Input_YEAR;
				//	clearlcd();
				//	chinesedisplay(0x80, g_DeadlineFmtStr);
				//}
				break;
			case 5:
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


			case 21:////数组组装元邋ID
				clearlcd();
				chinesedisplay(0x80, g_InputPackagingNameStr+6);
				//DisFourNumValue(g_Pack_ID, 0x85);
				if(g_Pack_ID/1000!=0)
					DisTwoNumValue(g_Pack_ID/1000, 0x85);
				DisFourNumValue(g_Pack_ID%1000, 0x86);
				chinesedisplay(0x90, g_PackagingName);
				break;
			case 22:
				clearlcd();
				chinesedisplay(0x80, g_InputTestNameStr+6);
				//DisTwoNumValue(g_Test_ID, 0x85);
				if(g_Test_ID/1000!=0)
					DisTwoNumValue(g_Test_ID/1000, 0x85);
				DisFourNumValue(g_Test_ID%1000, 0x86);
				chinesedisplay(0x90, g_TestName);
				break;
			case 23:
				clearlcd();
				chinesedisplay(0x80, g_InputSaleNameStr+6);
				//DisTwoNumValue(g_Sale_ID, 0x85);
				if(g_Sale_ID/1000!=0)
					DisTwoNumValue(g_Sale_ID/1000, 0x85);
				DisFourNumValue(g_Sale_ID%1000, 0x86);
				chinesedisplay(0x90, g_SaleName);
				break;
				
			

			case 70://
				DealCurrentState(State_User_Input_Logo);
				#ifdef DEBUG_DEBUGCMD
					printf("Debug CMD:70;State= %d  \r\n", State_User_Input_Logo);
				#endif
				break;
				
			case 71://
				DealCurrentState(State_User_Set_Tech_TelNumber);
				break;
				
			case 72:
				DealCurrentState(State_User_Input_MathineType);
				break;
				
			case 80://施工方设置用户密码
				g_InputOldSeriesForWhat=0;//输入用户密码或高级密码，设置新用户密码
				
				if(g_UserSetPasswordVal==1000000&&g_TryEndDate[0]==0xffff&&g_TryEndDate[1]==0xff&&g_TryEndDate[2]==0xff)
				{//如果没有设置过用户密码，并且没有设置为试用机器
					DealCurrentState(State_User_Set_SerialNumber);
				}
				else
				{
					DealCurrentState(State_User_Old_SerialNumber);
				}
				break;
				
			case 81://施工方设置试用天数
				g_InputOldSeriesForWhat=1;//输入高级密码或用户密码，设置试用天数
				//if(g_UserSetPasswordVal<1000000)
					DealCurrentState(State_User_Old_SerialNumber);
				//else			
				//	DealCurrentState(State_User_Input_TryDays);
				break;
			case 82://
				DealCurrentState(State_User_Set_TelNumber);
				break;			
			case 83://
				DealCurrentState(State_User_Input_SerialNumber);
				break;
			case 84://取消开机密码
				g_InputOldSeriesForWhat=3;//输入高级密码或用户密码，取消开机密码
				DealCurrentState(State_User_Old_SerialNumber);
				break;
			case 85://开启开机密码
				g_PowerOnPwdFlag=__TRUE;
				Write_EEPROM_Byte(E2ROM_PowerOnPwd_Flag_ADDR, g_PowerOnPwdFlag);
				g_PowerOnPwdOk=__FALSE;

				g_InputBack=g_MainInput;
//				printf("g_InputBack=%d!",g_InputBack);
////				SetInputSourceSwitch(INPUTSOURCE_SVIDEO);  //变成蓝屏
				DealCurrentState(State_MainMenu_Poweron_PassWord);
				break;
			case 86:
				//DealCurrentState(State_HowLong_Save);
				g_InputOldSeriesForWhat=4;//输入高级密码或用户密码，设置有效记录时间
				DealCurrentState(State_User_Old_SerialNumber);
				break;
			case 87:
				if(ReadLog())
				{
					clearlcd();
					chinesedisplay(0x82,"OK!");
				}
				else
				{
					clearlcd();
					chinesedisplay(0x82,"No data!");
				}
				break;
			case 88:
								
				g_InputOldSeriesForWhat=6;//输入高级密码或用户密码，清除记录
				DealCurrentState(State_User_Old_SerialNumber);
				
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
	U8 step=0;
	U8 StepDisPos=0x88;
	WORD pData,TempData1;
	if(Number1Idx==0)
		return __FALSE;
	value=atoi(Number1,Number1Idx);

	switch(currentState)
	{

		////设置大屏总的点数
		case	State_NewMode_Set_HTotalSize:
			if (value>15360) 
				{
					AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
					if(g_LanguageFlag==CHINESE)
					{
						chinesedisplay(0x80, g_DirectSetStr);
						chinesedisplay(LCDADDR90, g_LEDHSizeStr);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(0x83, g_DirectSetStrEn);
						chinesedisplay(LCDADDR90, g_LEDHSizeStrEn);
					}
					return __FALSE;
				}
				g_ScreenTotalHSize=value;
				printf("g_ScreenTotalHSize : %d, Ok!\r\n", g_ScreenTotalHSize);
				break;

			
		case State_NewMode_Set_VTotalSize:
				if(value>8192)
				{
					AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
					if(g_LanguageFlag==CHINESE)
					{
						chinesedisplay(0x81, g_DirectSetStr);
						chinesedisplay(LCDADDR90, g_LEDVSizeStr);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(0x83, g_DirectSetStrEn);
						chinesedisplay(LCDADDR90, g_LEDVSizeStrEn);
					}
					return __FALSE;
				}
				g_ScreenTotalVSize=value;
				
				printf("g_ScreenTotalVSize : %d, Ok!\r\n", g_ScreenTotalVSize);
						
				break;

		////设置输出卡的拼接大小
		case State_NewMode_Set_CardNumber:
			if(value>8)
				{
				AdviceForUser();
				//AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_CardNumber=value;				
				printf("g_CardNumber : %d, Ok!\r\n", value);

			break;
			
		case State_NewMode_Set_CardLayout:
			if(value>4)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_CardLayout=value;				
				printf("g_CardLayout : %d, Ok!\r\n", value);
			break;
		case State_NewMode_Set_Card1HSize:
			if(value>3840)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card1HSize=value;				
				printf("g_Card1HSize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Card1VSize:
			if(value>2160)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card1VSize=value;				
				printf("g_Card1VSize : %d, Ok!\r\n", value);

			break;

		case State_NewMode_Set_Card2HSize:
			if(value>3840)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card2HSize=value;				
				printf("g_Card2HSize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Card2VSize:
			if(value>2160)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card2VSize=value;				
				printf("g_Card2VSize : %d, Ok!\r\n", value);
			break;
		case State_NewMode_Set_Card3HSize:
			if(value>3840)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card3HSize=value;				
				printf("g_Card3HSize : %d, Ok!\r\n", value);
			break;
		case State_NewMode_Set_Card3VSize:
			if(value>2160)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card3VSize=value;				
				printf("g_Card3VSize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Card4HSize:
			if(value>3840)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card4HSize=value;				
				printf("g_Card4HSize : %d, Ok!\r\n", value);
			break;
		case State_NewMode_Set_Card4VSize:
			if(value>2160)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Card4VSize=value;				
				printf("g_Card4VSize : %d, Ok!\r\n", value);

			break;



		////设置图像显示的尺寸
		case State_NewMode_Set_ImageNumber:
			if(value>100)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_ImageNumber=value;				
				printf("g_ImageNumber : %d, Ok!\r\n", value);

			break;
			
		case State_NewMode_Set_ImageLayout:
			if(value>5)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_ImageLayout=value;				
				printf("g_ImageLayout : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image1Hsize:
			if(value>g_ScreenTotalHSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image1Hsize=value;				
				printf("g_Image1 H size : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image1Vsize:
			if(value>g_ScreenTotalVSize)
				{
					AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image1Vsize=value;				
				printf("g_Image1Vsize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image1Hpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image1Hpos=value;				
				printf("g_Image1 H Position : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image1Vpos:
			if(value>g_ScreenTotalVSize)
				{
					AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image1Vpos=value;				
				printf("g_Image1VPosition : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image2Hsize:
			if(value>g_ScreenTotalHSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image2Hsize=value;				
				printf("g_Image2 H size : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image2Vsize:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image2Vsize=value;				
				printf("g_Image2Vsize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image2Hpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image2Hpos=value;				
				printf("g_Image2 H Position : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image2Vpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image2Vpos=value;				
				printf("g_Image2VPosition : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image3Hsize:
			if(value>g_ScreenTotalHSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image3Hsize=value;				
				printf("g_Image3 H size : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image3Vsize:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image3Vsize=value;				
				printf("g_Image3Vsize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image3Hpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image3Hpos=value;				
				printf("g_Image3 H Position : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image3Vpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image3Vpos=value;				
				printf("g_Image3VPosition : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image4Hsize:
			if(value>g_ScreenTotalHSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image4Hsize=value;				
				printf("g_Image4 H size : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image4Vsize:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image4Vsize=value;				
				printf("g_Image4Vsize : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image4Hpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image4Hpos=value;				
				printf("g_Image4 H Position : %d, Ok!\r\n", value);

			break;
		case State_NewMode_Set_Image4Vpos:
			if(value>g_ScreenTotalVSize)
				{
				AdviceForUser();
					delay(10);
					InputFourNumReset();
					clearlcd();
				}
			
				g_Image4Vpos=value;				
				printf("g_Image4 VPosition : %d, Ok!\r\n", value);

			break;
	
	}
		//InputFourNumReset();
		InputDateTimeReset();
	return __TRUE;
}







//-------------------------deal input source Schedule number----------------------------------
void DealInputNumber(U8 cnt,U8 addr)
{	
	_InputSourceType input;
	U8 temp_state;
	input=NumKeyToInputSrc(cnt);

	if(input==INPUTSOURCE_NONE)
		return;

	
	switch(g_CurrentState)
	{

		case State_NewMode_Set_Image1Scr:
			g_Image1Scr=input;
			if (g_ImageNumber==1)				
				{
					temp_state = g_CurrentState;
					g_CurrentState = State_NewMode_Set_SaveGuide;
					#ifdef DEBUG_STATE_MSG
					printf("Set Image-1 Source, Jump to Save Guide State,Current State:%d,Next state:%d\r\n",temp_state,g_CurrentState);
					 #endif
			}
			break;
		case State_NewMode_Set_Image2Scr:
			g_Image2Scr=input;
			if (g_ImageNumber==2)				
				g_CurrentState = State_NewMode_Set_SaveGuide;
			break;
		case State_NewMode_Set_Image3Scr:
			g_Image3Scr=input;
			if (g_ImageNumber==3)				
					g_CurrentState = State_NewMode_Set_SaveGuide;
					
			break;
		case State_NewMode_Set_Image4Scr:
			g_Image4Scr=input;
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
			g_ScheduleInput3=input;
			g_SetInputScheFlag=INSRCOVER;
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


			
	#ifdef HOT_BACKUP
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
	#endif
	}

	//开启提示灯 提示用户按OK
	Start_LED_Flashing(1);
	g_FlashLedKey[0]=OK_KEY;

	
	chinesedisplay(addr, "      ");
	DisplayMainInput(addr,input);
}
//-----------------------------------------------------------------------------------







BOOL SelectOutputResolution()
{
	if(g_MainScalerHSize<=640&&g_MainScalerVSize<=480)
	{
		g_OutputFormatIndex=INDEX640X480;
	}
	else if(g_MainScalerHSize<=800&&g_MainScalerVSize<=600)
	{
		g_OutputFormatIndex=INDEX800X600;
	}
	else if(g_MainScalerHSize<=1024&&g_MainScalerVSize<=768)
	{
		g_OutputFormatIndex=INDEX1024X768;
	}
	else if(g_MainScalerHSize<=1280&&g_MainScalerVSize<=720)
	{
		g_OutputFormatIndex=INDEX1280X720;
	}
	else if(g_MainScalerHSize<=1280&&g_MainScalerVSize<=768)
	{
		g_OutputFormatIndex=INDEX1280X768;
	}
	else if(g_MainScalerHSize<=1280&&g_MainScalerVSize<=800)
	{
		g_OutputFormatIndex=INDEX1280X800;
	}
	else if(g_MainScalerHSize<=1280&&g_MainScalerVSize<=960)
	{
		g_OutputFormatIndex=INDEX1280X960;
	}
	else if(g_MainScalerHSize<=1280&&g_MainScalerVSize<=1024)
	{
		g_OutputFormatIndex=INDEX1280X1024;
	}
	else if(g_MainScalerHSize<=1366&&g_MainScalerVSize<=768)
	{
		g_OutputFormatIndex=INDEX1366X768;
	}
	else if(g_MainScalerHSize<=1400&&g_MainScalerVSize<=1050)
	{
		g_OutputFormatIndex=INDEX1400X1050;
	}
	else if(g_MainScalerHSize<=1440&&g_MainScalerVSize<=900)
	{
		g_OutputFormatIndex=INDEX1440X900;
	}
	else if(g_MainScalerHSize<=1600&&g_MainScalerVSize<=900)
	{
		g_OutputFormatIndex=INDEX1600X900;
	}
	else if(g_MainScalerHSize<=1600&&g_MainScalerVSize<=1200)
	{
		g_OutputFormatIndex=INDEX1600X1200;
	}
	else if(g_MainScalerHSize<=1680&&g_MainScalerVSize<=1050)
	{
		g_OutputFormatIndex=INDEX1680X1050;
	}
	else if(g_MainScalerHSize<=1920&&g_MainScalerVSize<=1080)
	{
		g_OutputFormatIndex=INDEX1920X1080; 
	} 
#if(XP_TO_ID==XP_TO_FLI32626)
	else if(g_MainScalerHSize<=1920&&g_MainScalerVSize<=1200)
	{
		g_OutputFormatIndex=INDEX1920X1200 ;
	}
	else if(g_MainScalerHSize<=2048&&g_MainScalerVSize<=1152)
	{
		g_OutputFormatIndex=INDEX2048x1152 ;
	}
#endif
	else
		return __FALSE;
	return __TRUE;
}




////分析和保存模式1
BOOL ParseAndSetMode()
{	
	U16 HSize=0,VSize=0;
	SetInputSource(g_MainInput, MAIN);
/*	
	if(g_MainInput==INPUTSOURCE_DVI
#if(ENABLE_HDMI)
	||g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2
#endif
	||g_MainInput==INPUTSOURCE_VGA)
	{
		GetInputSrcFormat(MAIN,&HSize,&VSize);		
	}
	*/
	if(HSize!=0&&VSize!=0&&g_ScreenHSize<=HSize&&g_ScreenVSize<=VSize)
	{
		if((HSize==640&&VSize==480)||(HSize==800&&VSize==600)||(HSize==1024&&VSize==768)
			||(HSize==1280&&VSize==1024)||(HSize==1366&&VSize==768)||(HSize==1440&&VSize==900)
			||(HSize==1600&&VSize==1200)||(HSize==1680&&VSize==1050)||(HSize==1920&&VSize==1080)
		#if(XP_TO_ID==XP_TO_FLI32626)
			||(HSize==1920&&VSize==1200)||(HSize==2048&&VSize==1152)
		#endif
			||(HSize==1600&&VSize==900)||(HSize==1280&&VSize==720)||(HSize==1280&&VSize==768)
			||(HSize==1280&&VSize==800)||(HSize==1280&&VSize==960)||(HSize==1400&&VSize==1050)
		)
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
					if(VSize==1024)
						g_OutputFormatIndex=INDEX1280X1024;
					else if(VSize==720)
						g_OutputFormatIndex=INDEX1280X720;
					else if(VSize==768)
						g_OutputFormatIndex=INDEX1280X768;
					else if(VSize==800)
						g_OutputFormatIndex=INDEX1280X800;
					else if(VSize==960)
						g_OutputFormatIndex=INDEX1280X960;
					break;
				case 1366:
				g_OutputFormatIndex=INDEX1366X768;
					break;
				case 1400:
				g_OutputFormatIndex=INDEX1400X1050;
					break;
				case 1440:
				g_OutputFormatIndex=INDEX1440X900;
					break;
				case 1600:					
					if(VSize==1200)
						g_OutputFormatIndex=INDEX1600X1200;			
					else if(VSize==900)
						g_OutputFormatIndex=INDEX1600X900;
					break;
				case 1680:
				g_OutputFormatIndex=INDEX1680X1050;
					break;
				case 1920:
					if(VSize==1080)
						g_OutputFormatIndex=INDEX1920X1080;
			#if(XP_TO_ID==XP_TO_FLI32626)
					else if(VSize==1200)
						g_OutputFormatIndex=INDEX1920X1200;
				break;
				case 2048:
					g_OutputFormatIndex=INDEX2048x1152;
			#endif
				break;
			}
		}
		else
		{
		#if(XP_TO_ID==XP_TO_FLI32626)
			g_OutputFormatIndex=0;
			UserResolutionHSize=HSize;
			UserResolutionVSize=VSize;
			Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
		#else
			if(!SelectOutputResolution())
				return __FALSE;
		#endif
		}
	}
	else
	{
		if(!SelectOutputResolution())
			return __FALSE;
	}

	SetOutputFormat(g_OutputFormatIndex);	
	LoadOutputResolution();
	
#if(XP_TO_ID==XP_TO_FLI32626)
	Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
	SetScalerHVValue(HVSIZE,MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		g_SubScalerHSize=g_ScreenHSize;
		g_SubScalerVSize=g_ScreenVSize;
		SetScalerHVValue(HVSIZE,SUB);
	}
#else
	SetScreenValue(HSIZE,g_ScreenHSize);
	SetScreenValue(VSIZE,g_ScreenVSize);	
	SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
	SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
#endif
	Save(MODE1);
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
	//chinesedisplay(addr, "     ");	
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
	else if(g_CurrentState==State_Input_DAY)
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
		if(g_HourIdx==0)  //if set current time can back space date
		{	
			if(g_SetTimeFlag==Set_Current_Time_Idx)
			{
				g_CurrentState=State_Input_DAY;
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
	else if(g_CurrentState==State_Input_SECOND)
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
		else if(g_SecondIdx==1)
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



void SaveSetupParaToE2PROM()
{

	

}


#ifdef DEBUG_STATE_MSG
void PrintStateValue()
{
	U8 i;	
	for (i=0;i<sizeof(state_string)/sizeof(char*);i++)
		{
		printf("%d = %s \r\n",i,state_string[i]);
		}
}
#endif


	






