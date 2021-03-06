/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: statedata.h
 [Date]:        12-May-2012
 [Comment]:
   Statedata header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _STATEDATA_H_
#define _STATEDATA_H_
//#include "../include/net_config.h"
//#include "../include/common.h"

#ifdef _STATEDATA_C_
  #define _STATEDATADEC_
#else
  #define _STATEDATADEC_ extern
#endif

typedef enum _SetTimeFlag{
Set_Input_Schedule_Idx=1,
Set_Mode_Schedule_Idx=2,
Set_Current_Time_Idx =3,
Set_Try_Use_Idx =4,
Set_InputCycle_Idx,
}SetTimeFlag;

#define YEARADDR                0x83
#define MONTHADDR             0x86
#define DAYADDR                  0x88
#define HOURADDR               0x92
#define MINADDR                  0x94
#define SECONDADDR         0x96
#define WEEKADDR               0x99


typedef enum{
	State_MainMenu_Menu,
	State_MainMenu_Menu_StartInfo,	
	State_MainMenu_Menu_VersionInfo,
	State_MainMenu_Menu_IDInfo,	
	State_MainMenu_Menu_InOutput,
	State_MainMenu_Menu_Current_Date,

	State_MainMenu_Menu_Logo,

	State_MainMenu_Not_Menu,
	//Full or Part
	State_FullOrPart_Full,	
	State_FullOrPart_Part,
	//multi window
	State_MultiWindow_Single,
	State_MultiWindow_PBP,               //15
	State_MultiWindow_PIP,
	State_MultiWindow_SAMEPIP,
	//Main or Sub
	State_ActiveImage_Main,
	State_ActiveImage_Sub,
	
	State_SavePara_Number,
	State_UserPara_Number,
	//screen parameter
	State_MainMenu_Menu_ScreenPara,    //20
	State_ScreenPara_Size,
	State_ScreenPara_Size_Value,
	State_ScreenPara_Pos,                     //25
	State_ScreenPara_Pos_Value,
	State_ScreenPara_UserResolution,                     //25
	State_ScreenPara_UserResolution_Value,
	//scaler parameter
	State_MainMenu_Menu_ScalerPara,
	State_ScalerPara_Size,
	State_ScalerPara_Size_Value,
	State_ScalerPara_Pos,
	State_ScalerPara_Pos_Value,
	//Zoom parameter
	State_MainMenu_Menu_ZoomPara,
	State_ZoomPara_Size,
	State_ZoomPara_Size_Value,			///32	
	State_ZoomPara_Pos,
	State_ZoomPara_Pos_Value,
	State_ZoomPara_Input,
	State_ZoomPara_Input_Format,
	//Color parameter
	State_MainMenu_Menu_ColorPara,
	State_ColorPara_Brightness,
	State_ColorPara_Brightness_Value,           //55
	State_ColorPara_Contrast,
	State_ColorPara_Contrast_Value,
/*	State_ColorPara_Brightness_R,
	State_ColorPara_Brightness_R_Value,
	State_ColorPara_Brightness_G,
	State_ColorPara_Brightness_G_Value,
	State_ColorPara_Brightness_B,                  //60
	State_ColorPara_Brightness_B_Value,
	State_ColorPara_Contrast_R,
	State_ColorPara_Contrast_R_Value,              //65
	State_ColorPara_Contrast_G,
	State_ColorPara_Contrast_G_Value,
	State_ColorPara_Contrast_B,
	State_ColorPara_Contrast_B_Value,*/
	State_ColorPara_Saturation,
	State_ColorPara_Saturation_Value,
	State_ColorPara_Reset,
	State_ColorPara_Reset_Check,
	State_ColorPara_ADC,
	State_ColorPara_ADC_Calibration,
	//Advance parameter
	State_MainMenu_Menu_AdvancePara,
	State_AdvancePara_OutputFormat,
	State_OutputFormat_640x480,
	State_OutputFormat_800x600,                      //75
	State_OutputFormat_1024x768x60,
	State_OutputFormat_1280x720,
	State_OutputFormat_1280x1024,
	State_OutputFormat_1366x768,
	State_OutputFormat_1440x900,
	State_OutputFormat_1600x1200,               //80
	State_OutputFormat_1680x1050,
	State_OutputFormat_1920x1080,
	State_OutputFormat_1920x1200,
	State_OutputFormat_2048x1152,
	State_OutputFormat_2560x960,
	State_OutputFormat_3840x600,
	State_OutputFormat_UserPrefer,
	//ˢ��Ƶ��
	State_AdvancePara_OutputHz,
	State_OutputHz_30Hz,
	State_OutputHz_50Hz,
	State_OutputHz_60Hz,
	State_OutputHz_NumHz,
	State_FreezeImage_Check,
	
	State_AdvancePara_Audio,
	State_Audio_Main,
	State_Audio_Sub,
	State_AdvancePara_LockPadTime,
	State_LockPadTime_Set,

	//Factory Reset parameter
	State_AdvancePara_FactoryReset,
	State_FactoryReset_Yes,
	State_FactoryReset_OK,
	State_AdvancePara_Wizard,
	//Language parameter
	State_AdvancePara_Language,
	State_Language_English,
	State_Language_Chinese,

	//User parameter
	State_MainMenu_Menu_UserPara,
	State_UserPara_SaveMode,
	State_UserPara_RecallMode,
	State_UserPara_SetCurrentTime,
	State_UserPara_SetFadeTime,
	State_UserPara_SetHotBackup,
	State_HotBackup_Set,
	State_HotBackup_Cancel,
	State_HotBackup_Check,
	State_User_HotBackup,
	State_User_HotBackup_Continue,
	State_UserPara_SetFadeTime_1000,
	State_UserPara_SetFadeTime_1500,
	State_UserPara_SetFadeTime_2000,
	State_UserPara_SetStaticIP,
	State_UserPara_SetStaticIP_Value,
	State_SavePara_Mode_check,
	//Time schedule
	State_MainMenu_TimeSchedule,
	State_TimeSchedule_InputSource,
	State_TimeSchedule_Input1,
	State_TimeSchedule_SettingInput1,
	State_TimeSchedule_Begin_Input1,
	State_ScheduleInput1_Once,
	State_ScheduleInput1_EveryDay,
	State_ScheduleInput1_InputWeek,
	State_TimeSchedule_CancelInput1,
	State_TimeSchedule_Input2,
	State_TimeSchedule_SettingInput2,
	State_TimeSchedule_Begin_Input2,
	State_ScheduleInput2_Once,
	State_ScheduleInput2_EveryDay,
	State_ScheduleInput2_InputWeek,
	State_TimeSchedule_CancelInput2,
	State_TimeSchedule_Input3,
	State_TimeSchedule_SettingInput3,
	State_TimeSchedule_Begin_Input3,
	State_ScheduleInput3_Once,
	State_ScheduleInput3_EveryDay,
	State_ScheduleInput3_InputWeek,
	State_TimeSchedule_CancelInput3,
#if(ENABLE_SCHEDULE_MODE)
	State_TimeSchedule_UserMode,
	State_TimeSchedule_Mode1,
	State_TimeSchedule_BeginMode1,
	State_TimeSchedule_CancelMode1,
	State_TimeSchedule_Mode2,
	State_TimeSchedule_BeginMode2,
	State_TimeSchedule_CancelMode2,
	State_TimeSchedule_Mode3,
	State_TimeSchedule_BeginMode3,
	State_TimeSchedule_CancelMode3,
#endif
	State_TimeSchedule_InputCycle,
	State_InputCycle_Set,
	State_InputCycle_Cancel,
	State_CycleInput_Continue,
	State_TimeSchedule_CancelAll,
	State_CancelAll_Yes,
	//TechSuppor parameter
	State_MainMenu_Menu_TechSupport,
	State_TechSuppor_ContactInfo,
	State_TechSuppor_ContactInfo_Page,	
	State_TechSuppor_VersionInfo,
	State_TechSuppor_VersionInfo_Page,
	//setup mode
	State_MainMenu_Menu_DebugMode,
	State_DebugMode_Check,
	State_DebugMode_Start,
	//
	State_NewMode_Set,
	State_NewMode_Set_HSize,
	State_NewMode_Set_VSize,
	State_NewMode_Set_InputSource,
	State_DualImage_Set_MainSize,
	State_DualImage_Set_SubSize,
	State_Input_YEAR,
	State_Input_MONTH,
	State_Input_DOM,
	State_Input_HOUR,
	State_Input_MIN,
	State_Input_SEC,
	State_Volume_Set,
	State_Lock_Key,
	State_Fade_Switch_Mode,
	State_Quick_Switch_Mode,
	State_Fuse_Switch_Mode,
	State_Fuse_Switch_Value,
//	State_Try_Use,
	State_User_Input_SerialNumber,
	State_User_Old_SerialNumber,
	State_User_Set_SerialNumber,
	State_User_Set_TelNumber,
	State_User_Set_Tech_TelNumber,
	State_User_Input_TryDays,
	State_User_Input_Try7Days,
	State_User_Input_Logo,
	State_Input_PackagingName,
	State_Input_TestName,
	State_Input_SaleName,
	State_User_Input_MathineType,
	State_HowLong_Save,
	
	
	State_UserTry_ShowTime,   //20170213 lyg
	State_UserTry_OnceTime,		//20170213 lyg
	State_UserTry_OnceTel,		//20170213 lyg
	State_UserTry_OnceEnd,		//20170213 lyg
	State_UserTry_Once_ID_Time,  //20170213 lyg
	State_UserTry_Once_SetEnd,//20170213 lyg
	
	State_UserTry_Once_Password101,//20170213 lyg
	State_UserTry_Once_Password102,//20170213 lyg
	State_UserTry_Once_Password103,//20170213 lyg
	State_UserTry_Once_Password104,//20170213 lyg
	State_UserTry_Once_Password105,//20170213 lyg
	
	State_UserTry_Once_Password100,//20170213 lyg
	State_UserTry_Once_Password100_new,//20170213 lyg
	State_UserTry_Once_Password100_new1,//20170213 lyg
	State_UserTry_Once_Password100_ok,//20170213 lyg
	
	State_Type_NoUse,
}State;

typedef  BOOL  (*fpExecFunc)(EventValue, State,U16);
typedef  State  (*fpGetFunc)();
typedef  U16    (*fpGetWindowFunc)();

typedef enum{
	NAV_UP = 0, 
	NAV_DOWN,
	NAV_OK,
	NAV_ESC,
	NUM_OF_NAV_EVENTS
}NAV_DATA_ARRAY_INDEX;

typedef struct{
	U8 startaddr1th;	
	const U8 * string1th;	
	const U8 * string1thEn;
	U8 startaddr2th;
	const U8 * string2th;
	const U8 * string2thEn;
	State navigationData[NUM_OF_NAV_EVENTS];				// navigation states, index 0-3 corresponds to Up/Down/Left/Right
	fpExecFunc ExecFunction;
	fpGetFunc  GetCurrentStateValue;
	fpGetWindowFunc GetWindowSize;
}StateData;

_InputSourceType NumKeyToInputSrc(U8 cnt);
void AddInputCycle(void);
void CancelInputScheduleMode1(void);
void CancelInputScheduleMode2(void);
void CancelInputScheduleMode3(void);
BOOL InputNumber(void);
BOOL InputNumberOver(EventValue Val);
void Copy_Mathine_Type(U8 *Number1,U8 Number1Idx);
void Copy_ID_To_Name(U16 Idx,State state);
void Read_Mathine_Quality_Info(void);
void DealScreenNumber(U8 cnt,U8 addr,U8 num);
void DealMonthNumber(U8 cnt,U8 addr);
void DealDateNumber(U8 cnt,U8 addr);
void DealHourNumber(U8 cnt,U8 addr);
void DealMinNumber(U8 cnt,U8 addr);
void DealSecondNumber(U8 cnt,U8 addr);
//void DealWeekNumber(U8 cnt,U8 addr);
void NumKeySetMonth(EventValue Val,State curState);
void NumKeySetDate(EventValue Val,State curState);
void NumKeySetHour(EventValue Val,State curState);
void NumKeySetMin(EventValue Val,State curState);
void NumKeySetSecond(EventValue Val,State curState);
void LcdDisplaySetOK_ReturnMain(void);
//void NumKeySetWeek(EventValue Val,State curState);

BOOL NumKeySetValue(EventValue Val,State curState);

void Copy_Logo(U16 temp);
void InputNumReset(void);
void DealSaveAndRecallNumber(U8 cnt);
void NumKeySetSaveMode(void);
void NumKeySetUserMode(void);
void SetSaveMode(U8 mode);
void SetUserMode(U8 mode);
BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue);
BOOL ParseAndSetMode(void);
void DisplayLCDInputSrc(U8 addr,U8 *chInter);
void DealInputNumber(U8 cnt,U8 addr);
//void ChangeUserMode(void);
void BackSpaceNumber(U8 addr,U8 num);
void BackScheWeekNum(void);
void ClearScheWeekNum(void);
void BackSpace(void);
BOOL IsSetupMode(void);
void InputFourNumReset(void);
BOOL DebugSetValue(EventValue Val,State currentState,U16 wValue);
BOOL SetTimeSchedule(EventValue Val,State currentState,U16 wValue);
void DealScheWeekNum(U8 cnt);
BOOL CancelTimeSchedule(EventValue Val,State currentState,U16 wValue);
U32 atoi(U8 *numStr,int Idx);


//20170213 lyg
extern int Date_Time[8];   
BOOL CheckPasswordNumber(void);
void display_DemoTimeIs_nMinutes(U8 displayLCDAddr);
void display_TrialTimeIs_nHours(U8 displayLCDAddr);
#endif

