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
#include "adv7441_i2c.h"

#ifdef _KEYPAD_C_
  #define _KEYPADDEC_
#else
  #define _KEYPADDEC_ extern
#endif

typedef enum _EffectTakeType{
NoTake,
EffectTake,
EffectDelayTake,
}EffectTakeType;

//切换模式
typedef enum _SrcSwitchType{
NormalSwitchIdx=1,
QuickSwitchIdx=2,
FadeSwitchIdx=3,
FuseSwitchIdx=4,
}SrcSwitchType;

typedef enum{
IP1_IDX,
IP2_IDX,
IP3_IDX,
IP4_IDX,
}_StaticIPIdx;

_KEYPADDEC_ U8 g_RTCErrorFlag;
_KEYPADDEC_ BOOL g_powerOnNoChangePartofFullFlag;
_KEYPADDEC_  PortID g_Adv7441CurrentInput;
_KEYPADDEC_  BOOL g_Adv7441VideoDetect;
_KEYPADDEC_  int g_Adv7441FrameWidth;
_KEYPADDEC_  int g_Adv7441FrameHeight;
_KEYPADDEC_  int g_Adv7441FrameHz;
_KEYPADDEC_  int g_Adv7441Interlaced;
 //_KEYPADDEC_ VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;
 
_KEYPADDEC_ _StaticIPIdx g_StaticIPIdx;
_KEYPADDEC_ U16 g_Pack_ID;
_KEYPADDEC_ U16 g_Test_ID;
_KEYPADDEC_ U16 g_Sale_ID;

_KEYPADDEC_ BOOL g_AdvertisingFlag;
_KEYPADDEC_ U8 g_BothSwitchFlag;
_KEYPADDEC_ U8 g_HadAdcCalibrationFlag;
_KEYPADDEC_ _CommunicationType g_ComTypeFlag;
_KEYPADDEC_ BOOL g_PoweronInputSendFlag;
_KEYPADDEC_ U32 g_UserSetPasswordVal;
_KEYPADDEC_ U8 g_TeleNumVal;
_KEYPADDEC_ U8 g_TechTeleNumVal;
_KEYPADDEC_ U8 g_Deadline7DaysFlag;
_KEYPADDEC_ U8 g_DeviceID;
_KEYPADDEC_ U8 g_InputOldSeriesForWhat;
//_KEYPADDEC_ _InputSourceType g_PoweronNoSYNCSrc;

_KEYPADDEC_ volatile BOOL g_PoweronFlag;
//获得32626版本号的整数部分和小数部分的变量
_KEYPADDEC_ U8 g_Fli32626HRevision;
_KEYPADDEC_ U8 g_Fli32626LRevision;

_KEYPADDEC_ U8 g_550subHRevision;
_KEYPADDEC_ U8 g_550subLRevision;

//用户试用是否到期
_KEYPADDEC_ BOOL g_TryTimeEndFlag;

_KEYPADDEC_ BOOL  g_TryEndFlag;


//是否需要记录LOG
_KEYPADDEC_ U8 g_EnableRecordLog;

//开机是否需要密码
_KEYPADDEC_ U8 g_PowerOnPwdFlag;
//VGA通道是否调整
_KEYPADDEC_ U8 g_AutoAdjustVGA_Flag;
//开机有效时间，进行记录
_KEYPADDEC_ U8 g_howlongtosave;
//已经存储了几条记录
_KEYPADDEC_ U16 g_NumSaved;
//存储的地址
_KEYPADDEC_ U16 g_LogAddr;




//机器ID
_KEYPADDEC_ U32 ChipUniqueID;
//上位机是否控制中
_KEYPADDEC_ volatile BOOL g_PCControlFlag;

_KEYPADDEC_ BOOL g_PCSendFlag;
_KEYPADDEC_ U8 g_OneSecondISRVal;
_KEYPADDEC_ BOOL g_WizadFlag;

//是否已接受到32626返回的信息
_KEYPADDEC_ BOOL g_ReceMsgFlag;
//锁键盘时间
_KEYPADDEC_ U8 g_LockKeypadTime; 
//语言选择
_KEYPADDEC_ U8 g_LanguageFlag; 
_KEYPADDEC_ U8 g_OutputHzVal; 
//切换模式的值
_KEYPADDEC_ SrcSwitchType g_SwitchEffectValue; 

//设置热备份的全局变量
_KEYPADDEC_ U8 g_EnableHotBackupFlag;  //是否使能热备份
_KEYPADDEC_ BOOL g_SetBackupInputFlag;  // 判断输入的是输入信号源还是备份信号源
_KEYPADDEC_ U8 g_HotBackupIdx;
_KEYPADDEC_ _InputSourceType g_HotBackupInput[8][2];

//设置通道计划的全局变量
_KEYPADDEC_ _InputSourceType g_ScheduleInput1;
_KEYPADDEC_ _InputSourceType g_ScheduleInput2;
_KEYPADDEC_ _InputSourceType g_ScheduleInput3;
_KEYPADDEC_ U8 g_ScheduleInputMode1;   //判断是单次模式还是重复模式
_KEYPADDEC_ U8 g_ScheduleInputMode2;
_KEYPADDEC_ U8 g_ScheduleInputMode3;

//设置循环播放的全局变量
_KEYPADDEC_ U8 g_StartCycleInputFlag; 
_KEYPADDEC_ U8 g_CycleInputStartHour;
_KEYPADDEC_ U8 g_CycleInputStartMin;
_KEYPADDEC_ U8 g_InputCycleIdx;

_KEYPADDEC_ _InputSourceType g_CycleInput1;
_KEYPADDEC_ _InputSourceType g_CycleInput2;
_KEYPADDEC_ _InputSourceType g_CycleInput3;
_KEYPADDEC_ _InputSourceType g_CycleInput4;
_KEYPADDEC_ _InputSourceType g_CycleInput5;
_KEYPADDEC_ _InputSourceType g_CycleInput6;

_KEYPADDEC_ U8 g_CycleInputHour1;
_KEYPADDEC_ U8 g_CycleInputHour2;
_KEYPADDEC_ U8 g_CycleInputHour3;
_KEYPADDEC_ U8 g_CycleInputHour4;
_KEYPADDEC_ U8 g_CycleInputHour5;
_KEYPADDEC_ U8 g_CycleInputHour6;

_KEYPADDEC_ U8 g_CycleInputMin1;
_KEYPADDEC_ U8 g_CycleInputMin2;
_KEYPADDEC_ U8 g_CycleInputMin3;
_KEYPADDEC_ U8 g_CycleInputMin4;
_KEYPADDEC_ U8 g_CycleInputMin5;
_KEYPADDEC_ U8 g_CycleInputMin6;

//设置淡入淡出时间
_KEYPADDEC_ U8 g_FadeTime;

#if(ENABLE_SCHEDULE_MODE)
_KEYPADDEC_ U8 g_ScheduleUser1;
_KEYPADDEC_ U8 g_ScheduleUser2;
_KEYPADDEC_ U8 g_ScheduleUser3;

_KEYPADDEC_ U8 g_ScheduleUserMode1;
_KEYPADDEC_ U8 g_ScheduleUserMode2;
_KEYPADDEC_ U8 g_ScheduleUserMode3;
#endif
//----------------------------------------------------------------

_KEYPADDEC_ BOOL g_H_VFlag;
_KEYPADDEC_ StateData stateData[];

_KEYPADDEC_ U8 g_SaveModeValue;
_KEYPADDEC_ U8 g_LockKeyFlag;

_KEYPADDEC_ U8 g_InputScheduleModeValue;
_KEYPADDEC_ U8 g_UserScheduleModeValue;

_KEYPADDEC_ U8 g_StepValue;
_KEYPADDEC_ U8 g_VGAFullOrPartValue;
_KEYPADDEC_ U8 g_DVIFullOrPartValue;
_KEYPADDEC_ U8 g_HDMIFullOrPartValue;
_KEYPADDEC_ U8 g_OtherFullOrPartValue;
_KEYPADDEC_ U8 g_VGA2FullOrPartValue;
_KEYPADDEC_ U8 g_DVI2FullOrPartValue;

_KEYPADDEC_ U8 g_FullOrPartValue;
_KEYPADDEC_ U8 g_ActiveImageValue;
_KEYPADDEC_ U8 g_ActiveAudioWin;
_KEYPADDEC_ U8 g_VolumeValue;
 _KEYPADDEC_ U8 g_MainFreezeFlag;
 _KEYPADDEC_ U8 g_SubFreezeFlag;
 _KEYPADDEC_ U8 g_NextPointImage;
//MutiWindow
_KEYPADDEC_ U8 g_MutiWindow;
//input source
_KEYPADDEC_ _InputSourceType g_MainInput;
_KEYPADDEC_ _InputSourceType g_SubInput;
//outputformat
_KEYPADDEC_ U16 g_PanelWidth;
_KEYPADDEC_ U16 g_PanelHeight;

//input src format
_KEYPADDEC_ U16 g_InSrcHSize;
_KEYPADDEC_ U16 g_InSrcVSize;

_KEYPADDEC_ U16 UserResolutionHSize,UserResolutionVSize;
//screen parameter
_KEYPADDEC_ U16 g_ScreenHSize;
_KEYPADDEC_ U16 g_ScreenVSize;
_KEYPADDEC_ U16 g_ScreenHPos;
_KEYPADDEC_ U16 g_ScreenVPos;

_KEYPADDEC_ U8 g_AlphaValue;
//scaler parameter
_KEYPADDEC_ U16 g_MainScalerHSize;
_KEYPADDEC_ U16 g_MainScalerVSize;
_KEYPADDEC_ U16 g_MainScalerHPos;
_KEYPADDEC_ U16 g_MainScalerVPos;

_KEYPADDEC_ U16 g_SubScalerHSize;
_KEYPADDEC_ U16 g_SubScalerVSize;
_KEYPADDEC_ U16 g_SubScalerHPos;
_KEYPADDEC_ U16 g_SubScalerVPos;
//zoom parameter
_KEYPADDEC_ U16 g_MainZoomHSize;
_KEYPADDEC_ U16 g_MainZoomVSize;
_KEYPADDEC_ U16 g_MainZoomHPos;
_KEYPADDEC_ U16 g_MainZoomVPos;

_KEYPADDEC_ U16 g_SubZoomHSize;
_KEYPADDEC_ U16 g_SubZoomVSize;
_KEYPADDEC_ U16 g_SubZoomHPos;
_KEYPADDEC_ U16 g_SubZoomVPos;
//VGA zoom parameter
_KEYPADDEC_ U16 g_MainVGAZoomHSize;
_KEYPADDEC_ U16 g_MainVGAZoomVSize;
_KEYPADDEC_ U16 g_MainVGAZoomHPos;
_KEYPADDEC_ U16 g_MainVGAZoomVPos;

_KEYPADDEC_ U16 g_SubVGAZoomHSize;
_KEYPADDEC_ U16 g_SubVGAZoomVSize;
_KEYPADDEC_ U16 g_SubVGAZoomHPos;
_KEYPADDEC_ U16 g_SubVGAZoomVPos;
//VGA zoom parameter
_KEYPADDEC_ U16 g_MainVGA2ZoomHSize;
_KEYPADDEC_ U16 g_MainVGA2ZoomVSize;
_KEYPADDEC_ U16 g_MainVGA2ZoomHPos;
_KEYPADDEC_ U16 g_MainVGA2ZoomVPos;

_KEYPADDEC_ U16 g_SubVGA2ZoomHSize;
_KEYPADDEC_ U16 g_SubVGA2ZoomVSize;
_KEYPADDEC_ U16 g_SubVGA2ZoomHPos;
_KEYPADDEC_ U16 g_SubVGA2ZoomVPos;
//DVI Zoom parameter
_KEYPADDEC_ U16 g_MainDVIZoomHSize;
_KEYPADDEC_ U16 g_MainDVIZoomVSize;
_KEYPADDEC_ U16 g_MainDVIZoomHPos;
_KEYPADDEC_ U16 g_MainDVIZoomVPos;

_KEYPADDEC_ U16 g_SubDVIZoomHSize;
_KEYPADDEC_ U16 g_SubDVIZoomVSize;
_KEYPADDEC_ U16 g_SubDVIZoomHPos;
_KEYPADDEC_ U16 g_SubDVIZoomVPos;
//DVI2 Zoom parameter
_KEYPADDEC_ U16 g_MainDVI2ZoomHSize;
_KEYPADDEC_ U16 g_MainDVI2ZoomVSize;
_KEYPADDEC_ U16 g_MainDVI2ZoomHPos;
_KEYPADDEC_ U16 g_MainDVI2ZoomVPos;

_KEYPADDEC_ U16 g_SubDVI2ZoomHSize;
_KEYPADDEC_ U16 g_SubDVI2ZoomVSize;
_KEYPADDEC_ U16 g_SubDVI2ZoomHPos;
_KEYPADDEC_ U16 g_SubDVI2ZoomVPos;
//HDMI Zoom parameter
_KEYPADDEC_ U16 g_MainHDMIZoomHSize;
_KEYPADDEC_ U16 g_MainHDMIZoomVSize;
_KEYPADDEC_ U16 g_MainHDMIZoomHPos;
_KEYPADDEC_ U16 g_MainHDMIZoomVPos;

_KEYPADDEC_ U16 g_SubHDMIZoomHSize;
_KEYPADDEC_ U16 g_SubHDMIZoomVSize;
_KEYPADDEC_ U16 g_SubHDMIZoomHPos;
_KEYPADDEC_ U16 g_SubHDMIZoomVPos;

//Color
_KEYPADDEC_ U8 g_BrightnessValue;
_KEYPADDEC_ U8 g_ContrastValue;
_KEYPADDEC_ U8 g_SaturationValue;
/*_KEYPADDEC_ U8 g_OffsetR;
_KEYPADDEC_ U8 g_OffsetG;
_KEYPADDEC_ U8 g_OffsetB;
_KEYPADDEC_ U8 g_GainR;
_KEYPADDEC_ U8 g_GainG;
_KEYPADDEC_ U8 g_GainB;
*/

 //output Index
_KEYPADDEC_ U8 g_OutputFormatIndex;
_KEYPADDEC_ State g_CurrentState; 



_KEYPADDEC_ _USBSTATE g_UsbState;

#ifdef KEY_REVERSE
typedef enum{
	DUAL_KEY,      
	FREEZE_KEY,  
	SWAP_KEY, 
	MENU_KEY,        
	FULLPART_KEY, 
	NOTUSE_KEY1,  
	BACKSPACE_KEY, 
	NOTUSE_KEY2,
	NUM0_KEY,
	NUM9_KEY, 
	NUM8_KEY, 
	NUM7_KEY, 
	NUM5_KEY,  
	NUM4_KEY, 
	NUM3_KEY, 
	NUM2_KEY,    
	NUM6_KEY, 
	NOTUSE_KEY3,  
	OK_KEY,  
	DOWN_KEY,  
	NUM1_KEY,        
	NOTUSE_KEY4,    
	ESC_KEY,          
	UP_KEY, 
	KEY_NUM
}_KeypadNum;
#else
#ifdef EC11_BUTTON
typedef enum{                
		/*NUM1_KEY,//BACKSPACE_KEY,  
		NUM2_KEY,//FULLPART_KEY,
		NUM3_KEY,//FREEZE_KEY,
		NUM4_KEY,//SWAP_KEY,    
		NUM6_KEY,//DUAL_KEY,  
		NUM7_KEY,//NUM1_KEY,
		NUM9_KEY, //NUM2_KEY,
		NUM8_KEY,//NUM3_KEY,
		NUM5_KEY, //NUM4_KEY,                       
		BACKSPACE_KEY,//NUM6_KEY,
		SWAP_KEY,//NUM7_KEY,
		ESC_KEY,//NUM8_KEY,                           
		NUM0_KEY,//NUM9_KEY,                        
		FREEZE_KEY,//NUM5_KEY,    
		DUAL_KEY, //ESC_KEY,                
		FULLPART_KEY, //NUM0_KEY,  */
		NUM9_KEY,
		NUM8_KEY,
		NUM7_KEY,
		NUM6_KEY,
		NUM1_KEY,

		
		NUM2_KEY,//6
		NUM3_KEY,
		NUM4_KEY,
		SAVE_KEY,
		FREEZE_KEY,

		FULLPART_KEY,//11
		NUM0_KEY,
		NUM5_KEY,
		BACKSPACE_KEY,
		MUTE_KEY,

			
		
		SWAP_KEY,//16
		OK_KEY,
		DOWN_KEY,			
		UP_KEY,
		ESC_KEY,

		NOTUSE_KEY1,
		NOTUSE_KEY2,
		NOTUSE_KEY3,
		NOTUSE_KEY4,
		
		KEY_NUM
	}_KeypadNum;
#else
typedef enum{
	BACKSPACE_KEY, 
	MENU_KEY,
	FULLPART_KEY, 
	FREEZE_KEY,
	SWAP_KEY,         
	NOTUSE_KEY1, 
	DUAL_KEY,         
	NOTUSE_KEY2,
	NUM1_KEY,
	NUM2_KEY,
	NUM3_KEY,
	NUM4_KEY, 
	NUM6_KEY, 
	NUM7_KEY,
	NUM8_KEY,  
	NUM9_KEY, 
	NUM5_KEY,          
	NOTUSE_KEY3,
	UP_KEY,                  
	ESC_KEY,
	NUM0_KEY,         
	NOTUSE_KEY4,
	DOWN_KEY,
	OK_KEY,            
	KEY_NUM
}_KeypadNum;
#endif
#endif


_KeypadNum InputSrcToNumKey(_InputSourceType input);
//LED 闪灯提示用户操作
//_KEYPADDEC_ BOOL g_OKCancelLedFlashingFlag; 
_KEYPADDEC_ BOOL g_FlashLedFlag; 
_KEYPADDEC_ U8 g_FlashLedNum; 
_KEYPADDEC_ _KeypadNum g_FlashLedKey[10]; 
#ifdef EC11_BUTTON
  void ScanEC11(void);
#endif

void SetCVBSMain(void);
void PanelToZoomPara(void);
void Start_LED_Flashing(U8 num);
//void LED_OK_Cancel_Flashing(void);
void initHotBackupVariable(BOOL flag);
BOOL IsSetColorParaValueState(void);
BOOL Check_OK_Cancel_State(void);
void LightInputSrcLED(void);
void ReturnMainWin(void);
void ReturnMainWinDontChangeState(void);

void InitSwapFisrtInput(void);
void DisplayStepValue(U8 xPos);
void initColorVariable(void);
void initGlobalVar(void);
void ScanKeypad(void);
void PanelToScreenPara(void);
void DisplayMainInput(U8 addr,_InputSourceType input);
void SetInputSourceSwitch(_InputSourceType input);
void LCDDisplayEffectInfo(void);
void ScanKeypadForSerial(void);
void LCDDisplayPCControl(void);
void DealCurrentState(State CurState);
void SetKeypadLED(U32 LedOfKey);   //keypad led
void SetKeyLEDLight(_KeypadNum key);
void ChangeInputSrc(void);
BOOL IsSetParaValueState(void);
State GetSwitchStateValue(void);
void initCycleInputPara(void);
_KEYPADDEC_  U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1];
 
#endif


  
