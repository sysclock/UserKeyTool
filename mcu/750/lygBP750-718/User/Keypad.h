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

//���������Ƿ�������ȷ
_KEYPADDEC_ BOOL g_PowerOnPwdOk;
//�����Ƿ���Ҫ����
_KEYPADDEC_ U8 g_PowerOnPwdFlag;
//������Чʱ�䣬���м�¼
_KEYPADDEC_ U8 g_howlongtosave;
//�Ѿ��洢�˼�����¼
_KEYPADDEC_ U16 g_NumSaved;
//�洢�ĵ�ַ
_KEYPADDEC_ U16 g_LogAddr;

///////////////////////////////////////Fli32626//////////////////////////////////////////////////////
#if(XP_TO_ID==XP_TO_FLI32626)
typedef enum _EffectTakeType{
NoTake,
EffectTake,
EffectDelayTake,
}EffectTakeType;

//�л�ģʽ
typedef enum _SrcSwitchType{
NormalSwitchIdx=1,
QuickSwitchIdx=2,
FadeSwitchIdx=3,
FuseSwitchIdx=4,
}SrcSwitchType;
//�Ƿ��ѽ��ܵ�32626���ص���Ϣ
_KEYPADDEC_ BOOL g_ReceMsgFlag;
//�л�ģʽ��ֵ
_KEYPADDEC_ SrcSwitchType g_SwitchEffectValue; 
//���õ��뵭��ʱ��
_KEYPADDEC_ U8 g_FadeTime;
 _KEYPADDEC_ U8 g_NextPointImage; 
_KEYPADDEC_ U16 UserResolutionHSize,UserResolutionVSize;
#endif
///////////////////////////////////////Fli32626//////////////////////////////////////////////////////

//���32626�汾�ŵ��������ֺ�С�����ֵı���
_KEYPADDEC_ U8 g_MainboardHRevision;
_KEYPADDEC_ U8 g_MainboardLRevision;
_KEYPADDEC_ BOOL g_AdvertisingFlag;
_KEYPADDEC_ U8 g_AlphaValue;
_KEYPADDEC_ _CommunicationType g_ComTypeFlag;
_KEYPADDEC_ BOOL g_TryTimeEndFlag;
_KEYPADDEC_ U32 ChipUniqueID;
_KEYPADDEC_ U32 g_UserSetPasswordVal;
_KEYPADDEC_ U8 g_TeleNumVal;
_KEYPADDEC_ U8 g_TechTeleNumVal;
_KEYPADDEC_ U8 g_Deadline7DaysFlag;
_KEYPADDEC_ U8 g_DeviceID;
_KEYPADDEC_ U8 g_InputOldSeriesForWhat;
_KEYPADDEC_ U8 g_BothSwitchFlag;
//��λ���Ƿ������
_KEYPADDEC_ volatile BOOL g_PCControlFlag;
_KEYPADDEC_ BOOL g_PCSendFlag;
_KEYPADDEC_ U8 g_OneSecondISRVal;
_KEYPADDEC_ BOOL g_WizadFlag;
_KEYPADDEC_ U8 g_LockKeypadTime; 
_KEYPADDEC_ U8 g_LanguageFlag; 
_KEYPADDEC_ U8 g_OutputHzVal; 
//����ͨ���ƻ���ȫ�ֱ���
_KEYPADDEC_ _InputSourceType g_ScheduleInput1;
_KEYPADDEC_ _InputSourceType g_ScheduleInput2;
_KEYPADDEC_ _InputSourceType g_ScheduleInput3;

_KEYPADDEC_ U8 g_ScheduleInputMode1;   //�ж��ǵ���ģʽ�����ظ�ģʽ
_KEYPADDEC_ U8 g_ScheduleInputMode2;
_KEYPADDEC_ U8 g_ScheduleInputMode3;

#if(ENABLE_SCHEDULE_MODE)
_KEYPADDEC_ U8 g_ScheduleUser1;
_KEYPADDEC_ U8 g_ScheduleUser2;
_KEYPADDEC_ U8 g_ScheduleUser3;

_KEYPADDEC_ U8 g_ScheduleUserMode1;
_KEYPADDEC_ U8 g_ScheduleUserMode2;
_KEYPADDEC_ U8 g_ScheduleUserMode3;
#endif
//----------------------------------------------------------------
//����ѭ�����ŵ�ȫ�ֱ���
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

#ifdef HOT_BACKUP
_KEYPADDEC_ BOOL g_PoweronInputSendFlag;
//�����ȱ��ݵ�ȫ�ֱ���
_KEYPADDEC_ U8 g_EnableHotBackupFlag;  //�Ƿ�ʹ���ȱ���
_KEYPADDEC_ BOOL g_SetBackupInputFlag;  // �ж�������������ź�Դ���Ǳ����ź�Դ
_KEYPADDEC_ U8 g_HotBackupIdx;
_KEYPADDEC_ _InputSourceType g_HotBackupInput[8][2];
#endif
//------------------------------------------------------------------
_KEYPADDEC_ volatile BOOL g_FourByteFlag;
_KEYPADDEC_ volatile BOOL g_TwoByteFlag;
_KEYPADDEC_ volatile BOOL g_OneByteFlag;
//_KEYPADDEC_ volatile BOOL g_MstarIDFlag;
_KEYPADDEC_ volatile BOOL g_MainboardVersionFlag;
//------------------------------------------------------------------
_KEYPADDEC_ BOOL g_H_VFlag;
_KEYPADDEC_ StateData stateData[];

_KEYPADDEC_ volatile BOOL g_PowerOnFlag;
_KEYPADDEC_ volatile BOOL g_QueueFullFlag;

//_KEYPADDEC_ BOOL g_CancelAllScheduleFlag;
//_KEYPADDEC_ U8 g_IsAV4OrYPbPr;
_KEYPADDEC_ U8 g_SaveModeValue;
_KEYPADDEC_ U8 g_LockKeyFlag;

_KEYPADDEC_ U8 g_InputScheduleModeValue;
_KEYPADDEC_ U8 g_UserScheduleModeValue;

_KEYPADDEC_ U8 g_StepValue;
//_KEYPADDEC_ U8 g_SingleOrDualValue;
_KEYPADDEC_ U8 g_VGAFullOrPartValue;
_KEYPADDEC_ U8 g_DVIFullOrPartValue;
_KEYPADDEC_ U8 g_HDMIFullOrPartValue;
//_KEYPADDEC_ U8 g_OtherFullOrPartValue;
_KEYPADDEC_ U8 g_FullOrPartValue;
_KEYPADDEC_ U8 g_ActiveImageValue;
_KEYPADDEC_ U8 g_ActiveAudioWin;

//MutiWindow
_KEYPADDEC_ U8 g_MutiWindow;
//input source
_KEYPADDEC_ _InputSourceType g_MainInput;
_KEYPADDEC_ _InputSourceType g_SubInput;
//outputformat
_KEYPADDEC_ U16 g_PanelWidth;
_KEYPADDEC_ U16 g_PanelHeight;
//screen parameter
_KEYPADDEC_ U16 g_ScreenHSize;
_KEYPADDEC_ U16 g_ScreenVSize;

_KEYPADDEC_ U16 g_ScreenTotalHSize;
_KEYPADDEC_ U16 g_ScreenTotalVSize;

_KEYPADDEC_ U16 g_ScreenHPos;
_KEYPADDEC_ U16 g_ScreenVPos;

	_KEYPADDEC_ U16			g_CardNumber;
	_KEYPADDEC_ U16				g_CardLayout      ;
	_KEYPADDEC_ U16				g_Card1HSize      ;
	_KEYPADDEC_ U16				g_Card1VSize      ;
	_KEYPADDEC_ U16				g_Card2HSize      ;
	_KEYPADDEC_ U16				g_Card2VSize      ;
	_KEYPADDEC_ U16				g_Card3HSize      ;
	_KEYPADDEC_ U16				g_Card3VSize      ;
	_KEYPADDEC_ U16				g_Card4HSize      ;
	_KEYPADDEC_ U16				g_Card4VSize      ;
	_KEYPADDEC_ U16				g_ImageNumber     ;
	_KEYPADDEC_ U16				g_ImageLayout     ;
	_KEYPADDEC_ U16				g_Image1Hsize     ;
	_KEYPADDEC_ U16				g_Image1Vsize     ;
	_KEYPADDEC_ U16				g_Image1Hpos      ;
	_KEYPADDEC_ U16				g_Image1Vpos      ;
	_KEYPADDEC_ U16				g_Image2Hsize     ;
	_KEYPADDEC_ U16				g_Image2Vsize     ;
	_KEYPADDEC_ U16				g_Image2Hpos      ;
	_KEYPADDEC_ U16				g_Image2Vpos      ;
	_KEYPADDEC_ U16				g_Image3Hsize     ;
	_KEYPADDEC_ U16				g_Image3Vsize     ;
	_KEYPADDEC_ U16				g_Image3Hpos      ;
	_KEYPADDEC_ U16				g_Image3Vpos      ;
	_KEYPADDEC_ U16				g_Image4Hsize     ;
	_KEYPADDEC_ U16				g_Image4Vsize     ;
	_KEYPADDEC_ U16				g_Image4Hpos      ;
	_KEYPADDEC_ U16				g_Image4Vpos      ;

_KEYPADDEC_ _InputSourceType g_Image1Scr;
_KEYPADDEC_ _InputSourceType g_Image2Scr;
_KEYPADDEC_ _InputSourceType g_Image3Scr;
_KEYPADDEC_ _InputSourceType g_Image4Scr;

//input src format
_KEYPADDEC_ U16 g_InSrcHSize;
_KEYPADDEC_ U16 g_InSrcVSize;
_KEYPADDEC_ U8 g_RTCErrorFlag;

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
//DVI Zoom parameter
_KEYPADDEC_ U16 g_MainDVIZoomHSize;
_KEYPADDEC_ U16 g_MainDVIZoomVSize;
_KEYPADDEC_ U16 g_MainDVIZoomHPos;
_KEYPADDEC_ U16 g_MainDVIZoomVPos;

_KEYPADDEC_ U16 g_SubDVIZoomHSize;
_KEYPADDEC_ U16 g_SubDVIZoomVSize;
_KEYPADDEC_ U16 g_SubDVIZoomHPos;
_KEYPADDEC_ U16 g_SubDVIZoomVPos;
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

_KEYPADDEC_ U8 g_Brightness_Value;
_KEYPADDEC_ U8 g_Contrast_Value;
_KEYPADDEC_ U8 g_Saturation_Value;

#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
_KEYPADDEC_ U8 g_BrightnessR_Value[2];
_KEYPADDEC_ U8 g_BrightnessG_Value[2];
_KEYPADDEC_ U8 g_BrightnessB_Value[2];
_KEYPADDEC_ U8 g_ContrastR_Value[2];
_KEYPADDEC_ U8 g_ContrastG_Value[2];
_KEYPADDEC_ U8 g_ContrastB_Value[2];
#endif
 //output Index
 _KEYPADDEC_ U8 g_OutputFormatIndex;
_KEYPADDEC_ State g_CurrentState; 
 _KEYPADDEC_ U8 g_MainFreezeFlag;
 _KEYPADDEC_ U8 g_SubFreezeFlag;
_KEYPADDEC_ U8 g_VolumeValue;
 
//_KEYPADDEC_ State g_CurrentValueState;

typedef enum{
IP1_IDX,
IP2_IDX,
IP3_IDX,
IP4_IDX,
}_StaticIPIdx;


_KEYPADDEC_ U8 g_StaticIPIdx;
_KEYPADDEC_ U16 g_Pack_ID;
_KEYPADDEC_ U16 g_Test_ID;
_KEYPADDEC_ U16 g_Sale_ID;



    #ifndef TWO_EC11_BOARD
	typedef enum{ 
				
				NUM9_KEY,           //K1
				NUM8_KEY,
				NUM7_KEY,
				NUM6_KEY,
				NUM1_KEY,
				
				NUM2_KEY,
				NUM3_KEY,
				NUM4_KEY, 
				DOWN_KEY,      //K10
				DUAL_KEY,
		
				NOTUSE_KEY1,//FULLPART_KEY, 
				NUM0_KEY,  
				NUM5_KEY, 
				NOTUSE_KEY2,//BACKSPACE_KEY, 	
				FREEZE_KEY,//SWAP_KEY,
		
				UP_KEY, 
				NOTUSE_KEY3,
				OK_KEY,			
				ESC_KEY,              //K20
				NOTUSE_KEY4,
				
				NOTUSE_KEY5,
				NOTUSE_KEY6,
				NOTUSE_KEY7,
				NOTUSE_KEY8,
				KEY_NUM
			}_KeypadNum;
    #else	
			typedef enum{                
				NUM1_KEY,//BACKSPACE_KEY,  
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
				FULLPART_KEY, //NUM0_KEY,  
				KEY_NUM
			}_KeypadNum;
    #endif

 
//���LED���ƿ���
_KEYPADDEC_ BOOL g_FlashLedFlag; 
_KEYPADDEC_ U8 g_FlashLedNum; 
_KEYPADDEC_ _KeypadNum g_FlashLedKey[5]; 

void Start_LED_Flashing(U8 num);
#ifdef EC11_BUTTON
  void ScanEC11(void);
#endif

#ifdef HOT_BACKUP
void initHotBackupVariable(BOOL flag);
#endif

void ScanKeypadForPowerOnPwd(void);
void PanelToScalerPara(void);
void PanelToZoomPara(void);
//void InitLockTempTime(void);
BOOL Check_OK_Cancel_State(void);
void LightInputSrcLED(void);
void ReturnMainWin(void);
void LCDDisplayPCControl(void);
void DisplayStepValue(U8 xPos);
void initGlobalVar(void);
void InitSwapFisrtInput(void);
void ScanKeypad(void);
void SetKeypadLED(U32 LedOfKey);
void PanelToScreenPara(void);
void DisplayMainInput(U8 addr,_InputSourceType input);
void SetInputSourceSwitch(_InputSourceType input);
void ChangeInputSrc(void);
BOOL IsSetColorParaValueState(void);
BOOL IsSetHVParaValueState(void);
void SetKeyLEDLight(_KeypadNum key);
void DealCurrentState(State CurState);
void ScanKeypadForSerial(void);
void initCycleInputPara(void);
void initColorVariable(void);
void DisplayLCDInputSrc(U8 addr,U8 *chInter);
U8 CheckDualSource(U8 InputSource1, U8 InputSource2);
char SetTestPatternColor(BOOL enable_Pattern, U32 pData);

char SetTestPatternColorR(void);
char SetTestPatternColorG(void);
char SetTestPatternColorB(void);
char unSetTestPatternColor(void);

#if(XP_TO_ID==XP_TO_FLI32626)
	void LCDDisplayEffectInfo(void);
	State GetSwitchStateValue(void);
#endif

_KEYPADDEC_  U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1];

_InputSourceType NumKeyToInputSrc(U8 cnt);
U8 NumKeyToInputMode(U8 cnt);
_KeypadNum InputSrcToNumKey(_InputSourceType input);

void ProcessWizardSet(void);
extern  char test_data[];
#endif



