/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: keypad.h
 [Date]:        09-May-2012
 [Comment]:
   keypad header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "statedata.h"


#ifdef _KEYPAD_C_
  #define _KEYPADDEC_
#else
  #define _KEYPADDEC_ extern
#endif

	
extern U8 G_SendBuf_addscenario[50];
extern U8 g_RevUart2Data[1024];
extern U16 g_RevUart2Idx;
extern U8 g_Read_Command_Flag;
extern U8 g_RevUart2Data_in[20];	
	
typedef enum{
IP1_IDX,
IP2_IDX,
IP3_IDX,
IP4_IDX,
}_StaticIPIdx;


typedef enum{
	CopyMode,
	SpliceMode,
	ScalerMode,
}devicemode;


_KEYPADDEC_ U8 dvcmode;




typedef enum
{
	R,
	G,
	B,
}RGB_Type;


_KEYPADDEC_ U8 g_FirstStart_Flag;

_KEYPADDEC_ U8 g_RTCErrorFlag;
 
_KEYPADDEC_ _StaticIPIdx g_StaticIPIdx;
_KEYPADDEC_ U16 g_Pack_ID;
_KEYPADDEC_ U16 g_Test_ID;
_KEYPADDEC_ U16 g_Sale_ID;

_KEYPADDEC_ BOOL g_AdvertisingFlag;

_KEYPADDEC_ _CommunicationType g_ComTypeFlag;
_KEYPADDEC_ BOOL g_PoweronInputSendFlag;
_KEYPADDEC_ U32 g_UserSetPasswordVal;

_KEYPADDEC_ U8 g_TechTeleNumVal;

_KEYPADDEC_ U8 g_DeviceID;
_KEYPADDEC_ U8 g_InputOldSeriesForWhat;

_KEYPADDEC_ U8 g_TeleNumVal;
_KEYPADDEC_ U8 g_TeleNumVal_input;
_KEYPADDEC_ U8 g_PassWordNumVal;
_KEYPADDEC_ U8 g_PassWordNumVal_input;
_KEYPADDEC_ U8 g_UserTry_Day;
_KEYPADDEC_ U8 g_UserTry_PassWord_input[7];
_KEYPADDEC_ U8 g_UserTry_Tel_input[13];
_KEYPADDEC_ U8 g_UserTry_PassWord[7];
_KEYPADDEC_ U8 g_UserTry_Tel[13];


//设置单次使用或者演示使用所用变量
_KEYPADDEC_ U8 g_UserTry_show_onoff;
_KEYPADDEC_ U8 g_UserTry_show_time;
_KEYPADDEC_ U8 g_UserTry_once_onoff;
_KEYPADDEC_ U8 g_UserTry_once_time;
_KEYPADDEC_ U8 g_UserTry_once_end_time[4];  //用来从存储空间读取或写入结束时间
_KEYPADDEC_ U8 g_UserTry_once_TeleNumVal;
_KEYPADDEC_ U8 g_UserTry_once_Tel_input[13];
_KEYPADDEC_ U8 g_UserTry_once_Tel[13];
_KEYPADDEC_ U8 g_UserTry_once_EndFlag;
_KEYPADDEC_ U32 g_UserTry_OnceEndTime;  //用来存放结束时间


_KEYPADDEC_ U8 g_UserTry_once_managepassword[4];//用来存储管理密码

_KEYPADDEC_ U8 g_UserTry_once_last_password[4];  //用来存储最后验证通过的输入密码
_KEYPADDEC_ U32 last_password_num;

_KEYPADDEC_ U32 g_manage_password;
_KEYPADDEC_ U32 g_manage_password_frist;



_KEYPADDEC_ volatile BOOL g_OrderMFlag;

_KEYPADDEC_ volatile BOOL g_PoweronFlag;

//用户试用是否到期
_KEYPADDEC_ BOOL g_TryTimeEndFlag;
//机器ID
_KEYPADDEC_ U32 ChipUniqueID;
//上位机是否控制中
_KEYPADDEC_ volatile BOOL g_PCControlFlag;

_KEYPADDEC_ BOOL g_PCSendFlag;
_KEYPADDEC_ U8 g_OneSecondISRVal;
 
//锁键盘时间
_KEYPADDEC_ U8 g_LockKeypadTime; 
//语言选择
_KEYPADDEC_ U8 g_LanguageFlag; 

_KEYPADDEC_ StateData stateData[];

_KEYPADDEC_ U8 g_LockKeyFlag;

_KEYPADDEC_ g_CurrentScene g_Current_Scene;

_KEYPADDEC_ U16	 g_ScenarioID;
_KEYPADDEC_ U8	 g_ImageNum;
_KEYPADDEC_ U8   g_ImageNum_Set;

_KEYPADDEC_ U8   g_SchemeNum;

_KEYPADDEC_ U8  g_Set_First_Flag;

_KEYPADDEC_ U16  g_LineNum;
_KEYPADDEC_ U16  g_ColNum;

_KEYPADDEC_ U16  g_OutPort_Num;
_KEYPADDEC_ U8   g_LayoutNum;
_KEYPADDEC_ U8   g_Image_Scr;
_KEYPADDEC_ U16  g_LineNum_Set;
_KEYPADDEC_ U16  g_ColNum_Set;
_KEYPADDEC_ U8   g_CardTest_Port;
_KEYPADDEC_ U8   g_CardTest_Color;

extern U8 g_MESSAGE_FLAG;
extern U32 g_MESSAGE_DisplayTime_Count;

 //output Index
_KEYPADDEC_ U8 g_OutputFormatIndex;
_KEYPADDEC_ State g_CurrentState; 

_KEYPADDEC_ U8 LCD_UI_NO;

_KEYPADDEC_ U8	devicetypeflag;

_KEYPADDEC_ U8 LCD_BACK_NUM;   //用来调节背光亮度
_KEYPADDEC_ U32 LCD_BACK_NUM_TIME;  //用来定时设置

typedef enum{
	NUM9_KEY,		// K1
	NUM8_KEY,   // K2
	NUM7_KEY,		// K3
	NUM6_KEY,		// K4
	NUM1_KEY,		// K5
	
	NUM2_KEY,		// k6
	NUM3_KEY,		// K7
	NUM4_KEY, 	// K8
	TAKE_KEY	,		// K9
	MUTE_KEY,  // K10
	
	TRAN_KEY,	// k11
	NUM0_KEY,  		// K12
	NUM5_KEY, 	// K13
	REC_KEY,		// K14
	ZOOM_KEY,			// K15
	
	VGA_KEY,			// K16
	OK_KEY	,		// K17
	DOWN_KEY,				// K18
	UP_KEY,			// K19
	ESC_KEY,	// K20 

	NOTUSE_KEY1,	// 21
	NOTUSE_KEY2,
	NOTUSE_KEY3,
	NOTUSE_KEY4, 
	
	KEY_NUM
}_KeypadNum;


_KeypadNum InputSrcToNumKey(_InputSourceType input);
//LED 闪灯提示用户操作
//_KEYPADDEC_ BOOL g_OKCancelLedFlashingFlag; 
_KEYPADDEC_ BOOL g_FlashLedFlag; 
_KEYPADDEC_ U8 g_FlashLedNum; 
_KEYPADDEC_ _KeypadNum g_FlashLedKey[10]; 



void Start_LED_Flashing(U8 num);
//void LED_OK_Cancel_Flashing(void);
#ifdef EC11_BUTTON
  void ScanEC11(void);
#endif
BOOL Check_OK_Cancel_State(void);
void LightInputSrcLED(void);
void ReturnMainWin(void);
void initGlobalVar(void);
void ScanKeypad(void);

void DisplayMainInput(U8 addr,_InputSourceType input);
void ScanKeypadForSerial(void);
//void LCDDisplayPCControl(void);
void DealCurrentState(State CurState);
void SetKeypadLED(U32 LedOfKey);   //keypad led
void SetKeyLEDLight(_KeypadNum key);
BOOL IsSetParaValueState(void);

void initCycleInputPara(void);
_KEYPADDEC_  U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1];



u8 Input_PassWord_Check(void);

void ADD_New_Scenario_SetOver(void);
void Updata_Scenario_Now(void);

void KeypadLockedPrompt(void);
void DealTakeKey(void);



#ifdef EC11_BUTTON
void Exit_EC11_Pin4_Init(void);
#endif

#endif

