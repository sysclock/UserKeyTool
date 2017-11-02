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
#define SECDAY			86400L
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
	State_MainMenu_Menu,			      //0
	State_MainMenu_Menu_StartInfo,		  //1
	State_MainMenu_Menu_VersionInfo,	  //2
	State_MainMenu_Menu_InOutput,			  //3
	State_MainMenu_Menu_Logo,		  //4
	State_MainMenu_Menu_IPInfo,

	State_MainMenu_Menu_OperationGuide,			  //5
	State_MainMenu_Menu_OperationGuide1,			  //6
	State_MainMenu_Not_Menu,			  //7 

/*******************************************************************************add********************/

	State_MainMenu_Menu_ScreenSet,
	State_MainMenu_Menu_ScreenSet_Password,
	State_ScreenSet_OutPort_Num,
	
	State_ScreenSet_TwoPort_Mode,
	State_ScreenSet_ThreePort_Mode,
	State_ScreenSet_FourPort_Mode,
	
	State_ScreenSet_OnePort_layout1,
	State_ScreenSet_TwoPort_layout1,
	State_ScreenSet_TwoPort_layout2,
	State_ScreenSet_ThreePort_layout1,
	State_ScreenSet_ThreePort_layout2,
	State_ScreenSet_FourPort_layout1,
	State_ScreenSet_FourPort_layout2,
	State_ScreenSet_FourPort_layout3,
	State_ScreenSet_Display,
	
	State_MainMenu_Menu_ScenarioSet,  //ª≠√Ê…Ë÷√
	State_MainMenu_Menu_ScenarioSet_Password,
	State_ScenarioSet_NewScheme,
	State_ScenarioSet_SchemeNum, 
	State_ScenarioSet_SchemeSaved,
	State_ScenarioSet_NewImage_ImageNum,	
	State_ScenarioSet_NewImage_ImagePara_Begin_X,
	State_ScenarioSet_NewImage_ImagePara_Begin_Y,
	State_ScenarioSet_NewImage_ImagePara_End_X,
	State_ScenarioSet_NewImage_ImagePara_End_Y,
	State_ScenarioSet_NewImage_ImagePara_Source,
	State_ScenarioSet_NewImage_ImagePara_Save,
	State_ScenarioSet_NewImage_OK_NOT,
	State_ScenarioSet_NewImage_Continue,
	
	
	State_ScenarioSet_DelScheme,  
	State_ScenarioSet_DelScheme_Num,	
	State_ScenarioSet_DelScheme_Num_OK,
	
	State_MainMenu_Menu_MatrixMode,
	State_MatrixMode_ModeSet_input,
	State_MatrixMode_ModeSet_output,
	
	State_MainMenu_Menu_WizardSet,
	State_WizardSet_Welcome,
	
	State_WizardSet_OutPort_Num,
	
	State_WizardSet_TwoPort_Mode,
	State_WizardSet_ThreePort_Mode,
	State_WizardSet_FourPort_Mode,
	
	State_WizardSet_OnePort_layout1,
	State_WizardSet_TwoPort_layout1,
	State_WizardSet_TwoPort_layout2,
	State_WizardSet_ThreePort_layout1,
	State_WizardSet_ThreePort_layout2,
	State_WizardSet_FourPort_layout1,
	State_WizardSet_FourPort_layout2,
	State_WizardSet_FourPort_layout3,

	State_WizardSet_InputSource_Set,
	State_MainMenu_Menu_AdvanceSet,
	
	State_AdvanceSet_CardTestSet,
	State_AdvanceSet_CardTest_Port,
	State_AdvanceSet_CardTest_Color,
	State_AdvanceSet_DebugModeSet,
	State_DebugMode_Start,
	State_UserTrySet_Start,
	State_UserTrySet_End,
	State_UserTrySet_Day,
	State_UserTrySet_PassWord,
	State_UserTrySet_Tel,
	
	State_TryTime_End,
	
	State_AdvanceSet_HelpInfo,
	State_HelpInfo_Wizard,
	State_HelpInfo_Screen,
	State_HelpInfo_Scenario,
	State_HelpInfo_Matrix,
	State_HelpInfo_TechSupport,
	State_HelpInfo_IP,
	
	State_AdvanceSet_TechSupport,
	
	State_AdvancePara_LockPadTime,			   //74
	State_LockPadTime_Set,					   //75

	State_AdvancePara_FactoryReset,			  
	State_FactoryReset_Yes,
	State_FactoryReset_OK,

	State_UserPara_SetCurrentTime,
	State_UserPara_SetNewTime_Date,
	State_UserPara_SetNewTime_Time,
	State_UserPara_SetNewTime_Week,

	State_UserPara_SetStaticIP,
	State_UserPara_SetStaticIP_Value,
	
	State_TechSuppor_ContactInfo,
	State_TechSuppor_ContactInfo_Page,	
	State_TechSuppor_VersionInfo,
	State_TechSuppor_VersionInfo_Page,
	State_Lock_Key,

	State_User_Input_SerialNumber,
	State_User_Old_SerialNumber,
	State_User_Set_SerialNumber,
	State_User_Set_TelNumber,
	State_User_Set_TechSupport_PhoneNumber,
	State_User_Change_Manufacturer_Logo,
	State_Input_PackagingName,
	State_Input_TestName,
	State_Input_SaleName,
	State_User_Change_MachineModel,
	
	
	//20170308  lyg
	State_UserTry_ShowTime,
	State_UserTry_OnceTime,
	State_UserTry_OnceTel,
	State_UserTry_OnceEnd,
	State_UserTry_Once_SetEnd,
	
	State_UserTry_Once_Password101,
	State_UserTry_Once_Password102,
	State_UserTry_Once_Password103,
	State_UserTry_Once_Password104,
	State_UserTry_Once_Password105,
	
	State_UserTry_Once_Password100,
	State_UserTry_Once_Password100_new,
	State_UserTry_Once_Password100_new1,
	State_UserTry_Once_Password100_ok,
	
}State;

typedef  BOOL  (*fpExecFunc)(EventValue, State ,U16);
typedef  State  (*fpGetFunc)();
typedef  U16    (*fpGetWindowFunc)();
typedef  void  (*fpDisBackFunc)();

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
	fpDisBackFunc DisBackFunc;
}StateData;

_InputSourceType NumKeyToInputSrc(U8 cnt);
//void AddInputCycle(void);
//void CancelInputScheduleMode1(void);
//void CancelInputScheduleMode2(void);
//void CancelInputScheduleMode3(void);
BOOL InputNumber(void);
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

void LcdDisplaySetOK_ReturnMain(void);
//void NumKeySetWeek(EventValue Val,State curState);

BOOL NumKeySetValue(EventValue Val,State curState);
void InputNumReset(void);


BOOL DirectSetValue(EventValue Val,State currentState,U16 wValue);


void DealInputNumber(U8 cnt);
void BackSpaceNumber(U8 addr,U8 num);
void InputFourNumReset(void);
BOOL DebugSetValue(EventValue Val,State currentState,U16 wValue);
void DealScheWeekNum(U8 cnt);
U32 atoi(U8 *numStr,int Idx);

void AdviceandResetNumber(void);

void InputDateTimeReset(void);


void CardTest_Send(void);

void Num_Clear(void);

void	Deal_SetNewTime_Date(U8 cnt);
void Deal_SetNewTime_Time(U8 cnt);
void Deal_SetNewTime_Week(U8 cnt);
void UserTry_SetData_Clear(void);


//20170308 lyg
extern int Date_Time_End[8];   
BOOL CheckPasswordNumber(void);
#endif


