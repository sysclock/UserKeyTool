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

#include "common.h"

#ifdef _STATEDATA_C_
  #define _STATEDATADEC_
#else
  #define _STATEDATADEC_ extern
#endif

typedef enum _SetTimeFlag{
Set_Time_Nothing=0,
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
#define SECONDADDR    		0x96
#define WEEKADDR               0x99


typedef enum{
	State_MainMenu_Menu,		//0
	State_MainMenu_Menu_StartInfo,	
	State_MainMenu_Menu_VersionInfo,
	State_MainMenu_Menu_IDInfo,	
	
	State_MainMenu_Menu_InOutput,
	State_MainMenu_Menu_In,//5
	State_MainMenu_Menu_Current_Date,
	State_MainMenu_Menu_Logo,
#ifdef POWER_ON_PWD
	State_MainMenu_Poweron_PassWord,
#endif
	State_MainMenu_Not_Menu,
	
	//Full or Part
	State_FullOrPart_Full,//10	
	State_FullOrPart_Part,	
	//multi window
	State_MultiWindow_Single,
	State_MultiWindow_PBP,               
	State_MultiWindow_PIP,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_MultiWindow_SAMEPIP,//15
#endif
	//Main or Sub
	State_ActiveImage_Main,
	State_ActiveImage_Sub,
	
	//
	State_SavePara_Number,
	State_LoadUserPara_Number,
	//screen parameter
	State_MainMenu_Menu_ScreenPara,    //20
	State_ScreenPara_Size,
	State_ScreenPara_Size_Value,
	State_ScreenPara_Pos,                    
	State_ScreenPara_Pos_Value,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_ScreenPara_UserResolution,                     //25
	State_ScreenPara_UserResolution_Value,
#endif
	//scaler parameter
	State_MainMenu_Menu_ScalerPara,
	State_ScalerPara_Size,
	State_ScalerPara_Size_Value,
	State_ScalerPara_Pos,//30
	State_ScalerPara_Pos_Value,
	//Zoom parameter
	State_MainMenu_Menu_ZoomPara,
	State_ZoomPara_Size,
	State_ZoomPara_Size_Value,
	State_ZoomPara_Pos,//35
	State_ZoomPara_Pos_Value,
	State_ZoomPara_Input,
	State_ZoomPara_Input_Format,
	//Color parameter
	State_MainMenu_Menu_ColorPara,
	State_ColorPara_Brightness,//40
	State_ColorPara_Brightness_Value,           
	State_ColorPara_Contrast,
	State_ColorPara_Contrast_Value,

#if(XP_TO_ID==XP_TO_FLI32626)
	State_ColorPara_Saturation,
	State_ColorPara_Saturation_Value,//45
	State_ColorPara_Reset,
	State_ColorPara_Reset_Check,
	State_ColorPara_ADC,
	State_ColorPara_ADC_Calibration,
#endif
	//Advance parameter
	State_MainMenu_Menu_AdvancePara,//50
	//屏幕分辨率
	State_AdvancePara_OutputFormat,
	State_OutputFormat_640x480,
	State_OutputFormat_800x600,                      
	State_OutputFormat_1024x768,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_OutputFormat_1280x720,//55
#endif
	State_OutputFormat_1280x720,
	State_OutputFormat_1280x768,
	State_OutputFormat_1280x800,
	State_OutputFormat_1280x960,
	State_OutputFormat_1280x1024,//60
	State_OutputFormat_1366x768,
	State_OutputFormat_1400x1050,
	State_OutputFormat_1440x900,
	State_OutputFormat_1600x900,              
	State_OutputFormat_1600x1200,      //65       
	State_OutputFormat_1680x1050,
	State_OutputFormat_1920x1080,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_OutputFormat_1920x1200,
	State_OutputFormat_2048x1152,
#endif
	//刷新频率
	State_AdvancePara_OutputHz,//70
	State_OutputHz_30Hz,
	State_OutputHz_50Hz,
	State_OutputHz_60Hz,
	State_OutputHz_NumHz,
	State_FreezeImage_Check,//75
	
	State_AdvancePara_Audio,
	State_Audio_Main,
	State_Audio_Sub,
	State_AdvancePara_LockPadTime,
	State_LockPadTime_Set,//80
	//Factory Reset parameter
	State_AdvancePara_FactoryReset,
	State_FactoryReset_Yes,
	State_FactoryReset_OK,
	State_AdvancePara_Wizard,
	//Language parameter
	State_AdvancePara_Language,//85
	State_Language_English,
	State_Language_Chinese,


////六、用户设置
	State6_MainMenu_Menu_UserPara,
	State_UserPara_SaveMode,
	State_UserPara_RecallMode,//90
	State63_UserPara_SetCurrentTime,
	State_UserPara_SetFadeTime,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_UserPara_SetFadeTime_500,
	State_UserPara_SetFadeTime_1000,
	State_UserPara_SetFadeTime_1500,//95
#endif
	State_UserPara_SetHotBackup,
	State_HotBackup_Set,
	State_HotBackup_Cancel,
	State_HotBackup_Check,
	State_User_HotBackup,//100
	State_User_HotBackup_Continue,
	State_UserPara_SetStaticIP,
	State_UserPara_SetStaticIP_Value,	
	State_SavePara_Mode_check,
	
	//七、Time schedule
	State_MainMenu_TimeSchedule,//105
	State_TimeSchedule_InputSource,
	State_TimeSchedule_Input1,
	State_TimeSchedule_BeginInput1,
	State_TimeSchedule_Begin_Input1,
	State_ScheduleInput1_Once,//110
	State_ScheduleInput1_EveryDay,
	State_ScheduleInput1_InputWeek,
	State_TimeSchedule_CancelInput1,
	State_TimeSchedule_Input2,
	State_TimeSchedule_BeginInput2,//115
	State_TimeSchedule_Begin_Input2,
	State_ScheduleInput2_Once,
	State_ScheduleInput2_EveryDay,
	State_ScheduleInput2_InputWeek,
	State_TimeSchedule_CancelInput2,//120
	State_TimeSchedule_Input3,
	State_TimeSchedule_BeginInput3,
	State_TimeSchedule_Begin_Input3,
	State_ScheduleInput3_Once,
	State_ScheduleInput3_EveryDay,//125
	State_ScheduleInput3_InputWeek,
	State_TimeSchedule_CancelInput3,

#if(ENABLE_SCHEDULE_MODE)
	State_TimeSchedule_UserMode,
	State_TimeSchedule_Mode1,
	State_TimeSchedule_BeginMode1,//130
	State_TimeSchedule_CancelMode1,
	State_TimeSchedule_Mode2,
	State_TimeSchedule_BeginMode2,
	State_TimeSchedule_CancelMode2,
	State_TimeSchedule_Mode3,//135
	State_TimeSchedule_BeginMode3,
	State_TimeSchedule_CancelMode3,
#endif
	State_TimeSchedule_InputCycle,
	State_InputCycle_Set,
	State_InputCycle_Cancel,//140
	State_CycleInput_Continue,
	State_TimeSchedule_CancelAll,
	State_CancelAll_Yes,
	//TechSuppor parameter
	State_MainMenu_Menu_TechSupport,
	State83_TechSuppor_TestPattern,//145
	//State831_TechSuppor_TestPattern_Page,
	State_TechSuppor_TestPatternR,
	State_TechSuppor_TestPatternG,
	State_TechSuppor_TestPatternB,
	State_TechSuppor_DisableTestPattern,
	
	State81_TechSuppor_ContactInfo,//150
	State811_TechSuppor_ContactInfo_Page,	
	State82_TechSuppor_VersionInfo,
	State82_TechSuppor_VersionInfo_Page,
	//setup mode
	State_MainMenu_Menu_DebugMode,
	State_DebugMode_Check,//155
	State_DebugMode_Start,
	
	//
	State_NewMode_Set,
	State_NewMode_Set_HTotalSize,
	State_NewMode_Set_VTotalSize,

////  33 new state
State_NewMode_Set_CardNumber,//160
State_NewMode_Set_CardLayout,
State_NewMode_Set_Card1HSize,
State_NewMode_Set_Card1VSize,
State_NewMode_Set_Card2HSize,
State_NewMode_Set_Card2VSize,//165
State_NewMode_Set_Card3HSize,
State_NewMode_Set_Card3VSize,
State_NewMode_Set_Card4HSize,
State_NewMode_Set_Card4VSize,
State_NewMode_Set_ImageNumber,//170
State_NewMode_Set_ImageLayout,
State_NewMode_Set_Image1Hsize,
State_NewMode_Set_Image1Vsize,
State_NewMode_Set_Image1Hpos,
State_NewMode_Set_Image1Vpos,//175
State_NewMode_Set_Image1Scr,
State_NewMode_Set_Image2Hsize,
State_NewMode_Set_Image2Vsize,
State_NewMode_Set_Image2Hpos,
State_NewMode_Set_Image2Vpos,//180
State_NewMode_Set_Image2Scr,
State_NewMode_Set_Image3Hsize,
State_NewMode_Set_Image3Vsize,
State_NewMode_Set_Image3Hpos,
State_NewMode_Set_Image3Vpos,//185
State_NewMode_Set_Image3Scr,
State_NewMode_Set_Image4Hsize,
State_NewMode_Set_Image4Vsize,
State_NewMode_Set_Image4Hpos,
State_NewMode_Set_Image4Vpos,//190
State_NewMode_Set_Image4Scr,
State_NewMode_Set_SaveGuide,


////
	State_NewMode_Set_InputSource,
	State_DualImage_Set_MainSize,
	State_DualImage_Set_SubSize,//195
////设置时间
	State_Input_YEAR,
	State_Input_MONTH,
	State_Input_DAY,
	State_Input_HOUR,
	State_Input_MIN,//200
	State_Input_SECOND,
	State_Volume_Set,
	State_Lock_Key,
#if(XP_TO_ID==XP_TO_FLI32626)
	State_Fade_Switch_Mode,
	State_Quick_Switch_Mode,//205
	State_Fuse_Switch_Mode,
	State_Fuse_Switch_Value,
#endif
//	State_Try_Use,
	State_User_Input_SerialNumber,
	State_User_Old_SerialNumber,
	State_User_Set_SerialNumber,//210
	State_User_Set_TelNumber,
	State_User_Set_Tech_TelNumber,
	State_User_Input_TryDays,
	State_User_Input_Try7Days,

	State_User_Input_Logo,//215
	State_Input_PackagingName,
	State_Input_TestName,
	State_Input_SaleName,
	State_User_Input_MathineType,
	State_HowLong_Save,//220
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
void AddInputCycle(void);
BOOL InputNumberOver(EventValue Val);
void Copy_Logo(U16 temp);
void Copy_Mathine_Type(U8 *Number1,U8 Number1Idx);


void Copy_ID_To_Name(U16 Idx,State state);
void Read_Mathine_Quality_Info(void);
void DealScreenNumber(U8 cnt,U8 addr,U8 num);
void DealMonthNumber(U8 cnt,U8 addr);
void DealDateNumber(U8 cnt,U8 addr);
void DealHourNumber(U8 cnt,U8 addr);
void DealMinNumber(U8 cnt,U8 addr);
void DealSecondNumber(U8 cnt,U8 addr);
void NumKeySetMonth(EventValue Val,State curState);
void NumKeySetDate(EventValue Val,State curState);
void NumKeySetHour(EventValue Val,State curState);
void NumKeySetMin(EventValue Val,State curState);
void NumKeySetSecond(EventValue Val,State curState);
void LcdDisplaySetOK_ReturnMain(void);
BOOL NumKeySetValue(EventValue Val,State curState);
void InputDateTimeReset(void);
void DealSaveAndRecallNumber(U8 cnt);
void NumKeySetSaveMode(void);
void NumKeySetUserMode(void);
void SetSaveMode(U8 mode);
void SetUserMode(U8 mode);
BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue);
BOOL ParseAndSetMode(void);
void DealInputNumber(U8 cnt,U8 addr);
//void ChangeUserMode(void);
void BackSpaceNumber(U8 addr,U8 num);
void BackScheWeekNum(void);
void ClearScheWeekNum(void);
void BackSpace(void);
BOOL DebugCmdProcess(EventValue Val,State currentState,U16 wValue);
BOOL SetTimeSchedule(EventValue Val,State currentState,U16 wValue);
void DealScheWeekNum(U8 cnt);
BOOL CancelTimeSchedule(EventValue Val,State currentState,U16 wValue);
void InputFourNumReset(void);

void SetCurrentTime(void);
void CancelInputScheduleMode1(void);
void CancelInputScheduleMode2(void);
void CancelInputScheduleMode3(void);

void SaveSetupParaToE2PROM(void);

#endif

