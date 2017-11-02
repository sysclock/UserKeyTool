/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: strings.h
 [Date]:        11-May-2012
 [Comment]:
   define strings file.
 [Reversion History]:

 
*******************************************************************************/
#ifndef _STRINGS_H_
#define _STRINGS_H_

#include "common.h"

#ifdef _STRINGS_C_
  #define _STRINGSDEC_
#else
  #define _STRINGSDEC_ extern
#endif


_STRINGSDEC_ const U8 g_CurrExecPlan[10] ;

_STRINGSDEC_ const U8 g_EEPROMErrorStr[21];
_STRINGSDEC_ const U8 g_CancelLockKeypadStr[18];
_STRINGSDEC_ const U8  g_SupportTechStrEn[9];
_STRINGSDEC_ const U8 g_BothSwithStr[21];
_STRINGSDEC_ const U8 g_BothSwithStrEn[21];
_STRINGSDEC_ const U8 g_StartStr[7];
_STRINGSDEC_ const U8 g_CancelStr[7];
_STRINGSDEC_ const U8 g_StartStrEn[5];
_STRINGSDEC_ const U8 g_CancelStrEn[5];
_STRINGSDEC_ const U8 g_PCPowerOnStr[13];
_STRINGSDEC_ const U8 g_PCShutOffStr[13];
_STRINGSDEC_ const U8 g_SupportTechStr[15];

_STRINGSDEC_ const U8 g_ImageLayoutGuidWords[23];
_STRINGSDEC_ const U8 g_ImageLayoutGuidWordsEn[21];
_STRINGSDEC_ const U8 g_DisplaySameMode[11];

//_STRINGSDEC_ const U8 g_MachineTypeXPIDStr[21];
_STRINGSDEC_ const U8 g_SyncToLEDStr[21];
_STRINGSDEC_ const U8 g_ScreenInfo1Str[6];
_STRINGSDEC_ const U8 g_InResolutionStr[6];
_STRINGSDEC_ const U8 g_InResolutionStr1[8];
_STRINGSDEC_ const U8 g_InResolutionStr2[8];
_STRINGSDEC_ const U8 g_OutResolutionStr[6];
_STRINGSDEC_ const U8  g_InputMathineTypeStr[21];

_STRINGSDEC_ const U8 g_StartTimeStr[15];
_STRINGSDEC_ const U8 g_StartTimeFormatStr[13];
_STRINGSDEC_ const U8 g_MathineIDStr[8];
_STRINGSDEC_ const U8 g_TryForUsr[15];
_STRINGSDEC_ const U8 g_TryOverStr[11];
_STRINGSDEC_ const U8 g_PurchaseStr[7];

_STRINGSDEC_ const U8  g_TestPattern[13];
_STRINGSDEC_ const U8  g_TestPatternEn[17];

_STRINGSDEC_ const U8  g_TestPatternR[13];
_STRINGSDEC_ const U8  g_TestPatternREn[17];
_STRINGSDEC_ const U8  g_TestPatternG[13];
_STRINGSDEC_ const U8  g_TestPatternGEn[17];
_STRINGSDEC_ const U8  g_TestPatternB[13];
_STRINGSDEC_ const U8  g_TestPatternBEn[17];
_STRINGSDEC_ const U8  g_DisableTestPattern[17];
_STRINGSDEC_ const U8  g_DisableTestPatternEn[20];
//g_DisableTestPattern


_STRINGSDEC_ const U8 g_KeyWarnningStr[10];
_STRINGSDEC_ const U8 g_PCControlStr[20];


_STRINGSDEC_ const U8 g_YesOrNoStr[18];
_STRINGSDEC_ const U8 g_YesOrNoStrEn[16];
_STRINGSDEC_ const U8 g_LEDHSizeStr[16];
_STRINGSDEC_ const U8 g_LEDHSizeStrEn[12];
_STRINGSDEC_ const U8 g_LEDVSizeStr[16];
_STRINGSDEC_ const U8 g_LEDVSizeStrEn[12];
_STRINGSDEC_ const U8 g_SetOKStr[9];
_STRINGSDEC_ const U8 g_SetOKStrEn[8];
_STRINGSDEC_ const U8 g_SetNotStr[14];
_STRINGSDEC_ const U8 g_SetNotStrEn[16];
_STRINGSDEC_ const U8 g_SelectAdvProStr[21];
_STRINGSDEC_ const U8 g_SelectAdvProStrEn[16];

_STRINGSDEC_ const U8 g_InputPackagingNameStr[20];
_STRINGSDEC_ const U8 g_InputTestNameStr[20];
_STRINGSDEC_ const U8 g_InputSaleNameStr[20];

_STRINGSDEC_ const U8 g_BuyTeleStr[21];
_STRINGSDEC_ const U8 g_BuyTeleStrEn[19];
_STRINGSDEC_ U8 g_WelcomeStr[30];
_STRINGSDEC_ U8 g_WelcomeStrEn[21];
_STRINGSDEC_ const U8 g_InputLogoStr[21];
_STRINGSDEC_ const U8 g_AudioStr[15];
_STRINGSDEC_ const U8 g_AudioStrEn[12];
_STRINGSDEC_ const U8 g_MuteStr[9];
_STRINGSDEC_ const U8 g_MuteStrEn[5];

_STRINGSDEC_ const U8 g_YangShanNameStr[7];
_STRINGSDEC_ const U8 g_HuangDanNameStr[7];

_STRINGSDEC_ U8 g_PackagingName[7];
_STRINGSDEC_ U8 g_TestName[7];
_STRINGSDEC_ U8 g_SaleName[7];

_STRINGSDEC_ const U8 g_VolumeStr[9];
_STRINGSDEC_ const U8 g_VolumeStrEn[11];
_STRINGSDEC_ const U8 g_InputStatusStr[8];
_STRINGSDEC_ const U8 g_InputStatusStrEn[7];

_STRINGSDEC_ const U8 g_inputsourceStr[13];
_STRINGSDEC_ const U8 g_inputsourceStrEn[14];
_STRINGSDEC_ const U8 g_CVBS1Str[6];
_STRINGSDEC_ const U8 g_CVBS1StrEn[4];
_STRINGSDEC_ const U8 g_CVBS2StrEn[4];
_STRINGSDEC_ const U8 g_CVBS3StrEn[4];
_STRINGSDEC_ const U8 g_CVBS4StrEn[4];
_STRINGSDEC_ const U8 g_CVBS2Str[6];
_STRINGSDEC_ const U8 g_CVBS3Str[6];
_STRINGSDEC_ const U8 g_CVBS4Str[6];
_STRINGSDEC_ const U8 g_SVideoStr[7];
_STRINGSDEC_ const U8 g_SVideoStrEn[7];
_STRINGSDEC_ const U8 g_DVIStr[12];
_STRINGSDEC_ const U8 g_DVIStrEn[4];
_STRINGSDEC_ const U8 g_YPbPrStr[14];
_STRINGSDEC_ const U8 g_YPbPrStrEn[6];
_STRINGSDEC_ const U8 g_VGAStr[8];
_STRINGSDEC_ const U8 g_VGAStrEn[4];
#if(ENABLE_HDMI)
_STRINGSDEC_ const U8 HDMI[13];
_STRINGSDEC_ const U8 HDMIEn[5];
_STRINGSDEC_ const U8 HDMI2[14];
_STRINGSDEC_ const U8 HDMI2En[6];
#endif

_STRINGSDEC_  const U8 g_AllChannelNoSYNCStr[17];

//-------------------schedule string-------------------------------------------
//方案1
_STRINGSDEC_ const U8 g_Program1Str[6];
_STRINGSDEC_ const U8 g_Program1StrEn[10];
_STRINGSDEC_ const U8 g_Program2StrEn[10];
_STRINGSDEC_ const U8 g_Program3StrEn[10];
//方案2
_STRINGSDEC_ const U8 g_Program2Str[6];
//方案3
_STRINGSDEC_ const U8 g_Program3Str[6];
_STRINGSDEC_ const U8 g_DateStrEn[17];
_STRINGSDEC_ const U8 g_DateStr[17];
_STRINGSDEC_ const U8 g_TimeStr[12];
//时间
_STRINGSDEC_ U8 g_CurrentTimeStr[21];
_STRINGSDEC_ const U8 g_CurrentTimeStrEn[12];
//时间  00: 00 (24制)
_STRINGSDEC_ const U8 g_ScheTimeStr[20];
_STRINGSDEC_ const U8 g_ScheTimeStrEn[16];
//时长00: 00 (时: 分)
_STRINGSDEC_  const U8 g_CycleTimeStr[20];
//该方案已设置
_STRINGSDEC_ const U8  g_HadSetStr[13];
_STRINGSDEC_ const U8  g_HadSetStrEn[8];
//按退格键修改或删除
_STRINGSDEC_ const U8  g_BackForChangeStr[19];
_STRINGSDEC_ const U8  g_BackForChangeStrEn[10];
//设置方案
_STRINGSDEC_ const U8 g_BeginScheduleStr[18];
_STRINGSDEC_ const U8 g_BeginScheduleStrEn[8];
//取消方案
_STRINGSDEC_ const U8 g_CancelScheduleStr[9];
_STRINGSDEC_ const U8 g_CancelScheduleStrEn[15];
_STRINGSDEC_ const U8 g_ScheduleModeStr[11];
_STRINGSDEC_ const U8 g_ScheduleModeStrEn[14];
_STRINGSDEC_ const U8 g_ScheduleOnceStr[9];
_STRINGSDEC_ const U8 g_ScheduleOnceStrEn[14];
_STRINGSDEC_ const U8 g_ScheduleEverydayStr[9];
_STRINGSDEC_ const U8 g_ScheduleEverydayStrEn[15];
_STRINGSDEC_ const U8 g_ScheduleWhichDayStr[11];
_STRINGSDEC_ const U8 g_ScheduleWhichDayStrEn[18];
//------------------------------------------------------------------------------

_STRINGSDEC_ const U8 g_stepEn[15];
_STRINGSDEC_ const U8 g_step[11];
_STRINGSDEC_ const U8 g_ColorstepEn[15];
_STRINGSDEC_ const U8 g_Colorstep[11];
_STRINGSDEC_ const U8 g_fullZoomEn[13];
_STRINGSDEC_ const U8 g_fullZoom[11];
_STRINGSDEC_ const U8 g_jubuEn[5];
_STRINGSDEC_ const U8 g_jubu[9];
_STRINGSDEC_ const U8 g_fullEn[5];
_STRINGSDEC_ const U8 g_full[9];
_STRINGSDEC_ const U8 g_SingleOrDualEn[19];
_STRINGSDEC_ const U8 g_SingleOrDual[21];
_STRINGSDEC_ const U8 g_SingleChEn[7];
_STRINGSDEC_ const U8 g_SingleCh[7];
_STRINGSDEC_ const U8 g_PBPEn[4];
_STRINGSDEC_ const U8 g_PBP[7];
_STRINGSDEC_ const U8 g_PIPEn[4];
_STRINGSDEC_ const U8 g_PIP[7];
//_STRINGSDEC_ const U8 g_error[9]=
//{
//0xB4,0xED ,0xCE,0xF3 ,0xB0,0xB4,0xBC,0xFC ,0
//};
_STRINGSDEC_ const U8 g_ActiveImage[13];
_STRINGSDEC_ const U8 g_ActiveImageEn[13];
_STRINGSDEC_ const U8 g_ErrorDualInputStr[16];
_STRINGSDEC_ const U8 g_ErrorDualInputStrEn[11];
_STRINGSDEC_ const U8 g_WarnningStr[11];
_STRINGSDEC_ const U8 g_WarnningStrEn[8];
_STRINGSDEC_ const U8 g_MainImage[6];
_STRINGSDEC_ const U8 g_MainImageEn[5];
_STRINGSDEC_ const U8 g_SubImage[6];
_STRINGSDEC_ const U8 g_SubImageEn[4];
_STRINGSDEC_ const U8 g_MainMenu[7];
_STRINGSDEC_ const U8 g_MainMenuEn[10];
_STRINGSDEC_ const U8 g_MainMenuNum[12];
_STRINGSDEC_ const U8 g_MainMenuNumEn[13];
_STRINGSDEC_ const U8 g_ScreenPara[17];
_STRINGSDEC_ const U8 g_ScreenParaEn[20];
_STRINGSDEC_ const U8 g_ScalerPara[17];
_STRINGSDEC_ const U8 g_ScalerParaEn[21];
_STRINGSDEC_ const U8 g_ZoomPara[17];
_STRINGSDEC_ const U8 g_ZoomParaEn[14];
_STRINGSDEC_ const U8 g_ColorPara[13];
_STRINGSDEC_ const U8 g_ColorParaEn[15];
_STRINGSDEC_ const U8 g_AdvancePara[13];
_STRINGSDEC_ const U8 g_AdvanceParaEn[17];
_STRINGSDEC_  const U8 g_SingleCharacter[7];
_STRINGSDEC_  const U8 g_DualCharacter[3];
_STRINGSDEC_  const U8 g_FadeCharacter[3];
_STRINGSDEC_  const U8 g_QuickCharacter[3];
_STRINGSDEC_  const U8 g_FuseCharacter[3];

_STRINGSDEC_ const U8  g_UserParaStr[13];
_STRINGSDEC_ const U8  g_UserParaStrEn[19];
_STRINGSDEC_ const U8  g_SaveModeStr[11];
_STRINGSDEC_ const U8  g_SaveModeStrEn[12];
_STRINGSDEC_ const U8  g_RecallModeStr[11];
_STRINGSDEC_ const U8  g_RecallModeStrEn[14];

_STRINGSDEC_ const U8 g_TimeScheduleStr[13];
_STRINGSDEC_ const U8 g_TimeScheduleStrEn[18];
_STRINGSDEC_ const U8 g_InputScheduleStr[11];
_STRINGSDEC_ const U8 g_InputScheduleStrEn[17];
_STRINGSDEC_ const U8 g_ModeScheduleStr[11];
_STRINGSDEC_ const U8 g_ModeScheduleStrEn[16];
_STRINGSDEC_ const U8 g_CancelAllScheduleStr[15];
_STRINGSDEC_ const U8 g_CancelAllScheduleStrEn[18];
_STRINGSDEC_ const U8 g_CancelAllScheduleSuccessStr[16];
_STRINGSDEC_ const U8 g_TechSupport[13];
_STRINGSDEC_ const U8 g_TechSupportEn[17];
_STRINGSDEC_ const U8 g_SetUpModeStr[13];
_STRINGSDEC_ const U8 g_SetUpModeStrEn[10];
_STRINGSDEC_ const U8 g_InputSetupStr[15];
_STRINGSDEC_ const U8 g_InputSetupStrEn[16];
_STRINGSDEC_ const U8 g_HSize[11];
_STRINGSDEC_ const U8 g_HSizeEn[9];
_STRINGSDEC_ const U8 g_VSize[11];
_STRINGSDEC_ const U8 g_VSizeEn[9];
_STRINGSDEC_ const U8 g_HPos[11];
_STRINGSDEC_ const U8 g_HPosEn[8];
_STRINGSDEC_ const U8 g_VPos[11];
_STRINGSDEC_ const U8 g_VPosEn[8];
_STRINGSDEC_ const U8 g_HRatio[11];
_STRINGSDEC_ const U8 g_HRatioEn[10];
_STRINGSDEC_ const U8 g_VRatio[11];
_STRINGSDEC_ const U8 g_VRatioEn[10];
_STRINGSDEC_ const U8 g_Brightness[7];
_STRINGSDEC_ const U8 g_BrightnessEn[13];
_STRINGSDEC_ const U8 g_Contract[9];
_STRINGSDEC_ const U8 g_ContractEn[11];

#ifdef RGB_COLOR
_STRINGSDEC_ const U8 g_BrightnessR[11];
_STRINGSDEC_ const U8 g_BrightnessREn[17];
_STRINGSDEC_ const U8 g_BrightnessG[11];
_STRINGSDEC_ const U8 g_BrightnessGEn[19];
_STRINGSDEC_ const U8 g_BrightnessB[11];
_STRINGSDEC_ const U8 g_BrightnessBEn[18];
_STRINGSDEC_ const U8 g_ContractR[13];
_STRINGSDEC_ const U8 g_ContractREn[15];
_STRINGSDEC_ const U8 g_ContractG[13];
_STRINGSDEC_ const U8 g_ContractGEn[17];
_STRINGSDEC_ const U8 g_ContractB[13];
_STRINGSDEC_ const U8 g_ContractBEn[16];
#endif

_STRINGSDEC_ const U8 g_outputFormatstr[15];
_STRINGSDEC_ const U8 g_outputFormatstrEn[18];
_STRINGSDEC_ const U8 g_FreezeImage[9];
_STRINGSDEC_ const U8 g_FreezeImageEn[13];
_STRINGSDEC_ const U8 g_LockKeypadStr[21];
_STRINGSDEC_ const U8 g_LockKeypadStrEN[17];
//_STRINGSDEC_ const U8 g_gamma[7];
//_STRINGSDEC_ const U8 g_gammaEn[8];
_STRINGSDEC_ const U8 g_FactoryReset[17];
_STRINGSDEC_ const U8 g_FactoryResetEn[16];

_STRINGSDEC_  const U8 g_UserResolutionHStr[11];
_STRINGSDEC_  const U8 g_UserResolutionVStr[11];
//_STRINGSDEC_  const U8 g_SelHStr[5];
//_STRINGSDEC_  const U8 g_SelVStr[5];
_STRINGSDEC_  const U8 g_SelHStrEn[9];
_STRINGSDEC_  const U8 g_SelVStrEn[9];
_STRINGSDEC_  const U8 g_SelScalerHStr[13];
_STRINGSDEC_  const U8 g_SelScalerVStr[13];
_STRINGSDEC_  const U8 g_SelScreenHStr[13];
_STRINGSDEC_  const U8 g_SelScreenVStr[13];
_STRINGSDEC_  const U8 g_OutHPosStr[17];
_STRINGSDEC_  const U8 g_OutVPosStr[17];
_STRINGSDEC_  const U8 g_SelZoomHStr[13];
_STRINGSDEC_  const U8 g_SelZoomVStr[13];
_STRINGSDEC_  const U8 g_InHPosStr[13];
_STRINGSDEC_  const U8 g_InVPosStr[13];
_STRINGSDEC_  const U8 g_ScreenSizeStr[11];
_STRINGSDEC_  const U8 g_ScalerSizeStr[11];
_STRINGSDEC_  const U8 g_ZoomSizeStr[11];
_STRINGSDEC_  const U8 g_StartPosStr[15];
_STRINGSDEC_  const U8 g_InputZoomStr[17];




//设置发送卡的参数

_STRINGSDEC_ const U8 g_SetSendCardNumber[16];
_STRINGSDEC_ const U8 g_SetSendCardNumberEn[16];

_STRINGSDEC_ const U8 g_SetSendCardLayout[16];
_STRINGSDEC_ const U8 g_SetSendCardLayoutEn[16];

_STRINGSDEC_ const U8 g_SetSendCard1Hsize[16];
_STRINGSDEC_ const U8 g_SetSendCard1HsizeEn[16];
_STRINGSDEC_ const U8 g_SetSendCard1Vsize[16];
_STRINGSDEC_ const U8 g_SetSendCard1VsizeEn[16];

_STRINGSDEC_ const U8 g_SetSendCard2Hsize[16];
_STRINGSDEC_ const U8 g_SetSendCard2HsizeEn[16];
_STRINGSDEC_ const U8 g_SetSendCard2Vsize[16];
_STRINGSDEC_ const U8 g_SetSendCard2VsizeEn[16];

_STRINGSDEC_ const U8 g_SetSendCard3Hsize[16];
_STRINGSDEC_ const U8 g_SetSendCard3HsizeEn[16];
_STRINGSDEC_ const U8 g_SetSendCard3Vsize[16];
_STRINGSDEC_ const U8 g_SetSendCard3VsizeEn[16];


_STRINGSDEC_ const U8 g_SetSendCard4Hsize[16];
_STRINGSDEC_ const U8 g_SetSendCard4HsizeEn[16];
_STRINGSDEC_ const U8 g_SetSendCard4Vsize[16];
_STRINGSDEC_ const U8 g_SetSendCard4VsizeEn[16];





//设置输入画面的参数

_STRINGSDEC_ const U8 g_SetImageNumber[16];
_STRINGSDEC_ const U8 g_SetImageNumberEn[16];
_STRINGSDEC_ const U8 g_SetImageLayout[16];
_STRINGSDEC_ const U8 g_SetImageLayoutEn[16];

_STRINGSDEC_ const U8 g_SetImage1HSize[16];
_STRINGSDEC_ const U8 g_SetImage1VSize[16];
_STRINGSDEC_ const U8 g_SetImage1HPos[16];
_STRINGSDEC_ const U8 g_SetImage1VPos[16];
_STRINGSDEC_ const U8 g_SetImage1Scr[16];
_STRINGSDEC_ const U8 g_SetImage1ScrEn[16];

_STRINGSDEC_ const U8 g_SetImage2HSize[16];
_STRINGSDEC_ const U8 g_SetImage2VSize[16];
_STRINGSDEC_ const U8 g_SetImage2HPos[16];
_STRINGSDEC_ const U8 g_SetImage2VPos[16];
_STRINGSDEC_ const U8 g_SetImage2Scr[16];
_STRINGSDEC_ const U8 g_SetImage2ScrEn[16];

_STRINGSDEC_ const U8 g_SetImage3HSize[16];
_STRINGSDEC_ const U8 g_SetImage3VSize[16];
_STRINGSDEC_ const U8 g_SetImage3HPos[16];
_STRINGSDEC_ const U8 g_SetImage3VPos[16];
_STRINGSDEC_ const U8 g_SetImage3Scr[16];
_STRINGSDEC_ const U8 g_SetImage3ScrEn[16];


_STRINGSDEC_ const U8 g_SetImage4HSize[16];
_STRINGSDEC_ const U8 g_SetImage4VSize[16];
_STRINGSDEC_ const U8 g_SetImage4HPos[16];
_STRINGSDEC_ const U8 g_SetImage4VPos[16];
_STRINGSDEC_ const U8 g_SetImage4Scr[16];
_STRINGSDEC_ const U8 g_SetImage4ScrEn[16];

_STRINGSDEC_ const U8 g_SetImage1HSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage1VSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage1HPosEn[16];
_STRINGSDEC_ const U8 g_SetImage1VPosEn[16];

_STRINGSDEC_ const U8 g_SetImage2HSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage2VSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage2HPosEn[16];
_STRINGSDEC_ const U8 g_SetImage2VPosEn[16];

_STRINGSDEC_ const U8 g_SetImage3HSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage3VSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage3HPosEn[16];
_STRINGSDEC_ const U8 g_SetImage3VPosEn[16];

_STRINGSDEC_ const U8 g_SetImage4HSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage4VSizeEn[16];
_STRINGSDEC_ const U8 g_SetImage4HPosEn[16];
_STRINGSDEC_ const U8 g_SetImage4VPosEn[16];


_STRINGSDEC_ const U8 g_DirectSetStr[14];
_STRINGSDEC_ const U8 g_DirectSetStr1[14];

_STRINGSDEC_ const U8 g_DirectSetStrEn[17];
_STRINGSDEC_ const U8 g_DirectSetStrEn1[17];

_STRINGSDEC_ const U8 g_Language[11];
_STRINGSDEC_ const U8 g_LanguageEn[11];

_STRINGSDEC_ const U8 g_FreezeStr[5];
_STRINGSDEC_ const U8 g_FreezeStrEn[7];
_STRINGSDEC_ const U8 g_LiveStr[5];
_STRINGSDEC_ const U8 g_LiveStrEn[5];

_STRINGSDEC_ const U8 g_ModeStr[5];
_STRINGSDEC_ const U8 g_ModeStrEn[5];
_STRINGSDEC_ const U8 g_OutRangeStr[5];

_STRINGSDEC_ const U8 g_ContactInfo[11];
_STRINGSDEC_ const U8 g_ContactInfoEn[15];
_STRINGSDEC_ const U8 g_VersionInfo[7];
_STRINGSDEC_ const U8 g_VersionInfoEn[15];
//该操作将清除保存数据
_STRINGSDEC_ const U8 g_Yes[21];
_STRINGSDEC_ const U8 g_YesEn[17];
//是否继续
_STRINGSDEC_ const U8 g_YesStr[11];
_STRINGSDEC_ const U8 g_YesStrEn[10];
_STRINGSDEC_ const U8 g_Chinese[5];
_STRINGSDEC_ const U8 g_ResetSuccessStrEn[14];
_STRINGSDEC_ const U8 g_ResetSuccessStr[11];
_STRINGSDEC_ const U8 g_SavingStrEn[12];
_STRINGSDEC_ const U8 g_SavingStr[11];
_STRINGSDEC_ const U8 g_SaveOKStrEn[13];
_STRINGSDEC_ const U8 g_SaveOKStr[10];
_STRINGSDEC_ const U8 g_LoadingModeStrEn[13];
_STRINGSDEC_ const U8 g_LoadingModeStr[15];
_STRINGSDEC_ const U8 g_LoadModeOKStrEn[13];
_STRINGSDEC_ const U8 g_LoadModeOKStr[15];
_STRINGSDEC_ const U8 g_LoadNoModeStrEn[8];
_STRINGSDEC_ const U8 g_LoadNoModeStr[19];
_STRINGSDEC_ const U8 g_HasSaveValueStrEn[20];
_STRINGSDEC_ const U8 g_HasSaveValueStr[19];
_STRINGSDEC_ const U8 g_SaveOverlapStrEn[9];
_STRINGSDEC_ const U8 g_SaveOverlapStr[10];
_STRINGSDEC_ const U8 g_SaveOverlappingStrEn[16];
_STRINGSDEC_ const U8 g_SaveOverlappingStr[15];
_STRINGSDEC_ const U8 g_LockKeyStr[11];
_STRINGSDEC_ const U8 g_LockKeyStrEn[19];
_STRINGSDEC_ const U8 g_UnLockKeyStr[11];
_STRINGSDEC_ const U8 g_UnLockKeyStrEn[18];
_STRINGSDEC_ const U8 g_LockPromptStr[15];
_STRINGSDEC_ const U8 g_LockPromptStrEn[20];
_STRINGSDEC_ const U8 g_UsedNumInputStr[15];
_STRINGSDEC_ const U8 g_SingleSwapWarnningStr[21];
_STRINGSDEC_ const U8 g_UsedDoubleStr[13];
_STRINGSDEC_ const U8 g_PleaseSetScreenStr[13];
_STRINGSDEC_ U16 TimeSet[6];
_STRINGSDEC_ U16 g_TryEndDate[3];
_STRINGSDEC_ const U8 g_DeadLine7DaysStr[18];
/*
//_STRINGSDEC_  const U8 g_NoEnterpriseStr[21];
_STRINGSDEC_ const U8 g_Enterprise1StrBYCOX[21];
_STRINGSDEC_ const U8 g_Enterprise2Str[21];
_STRINGSDEC_ const U8 g_Enterprise3Str[21];
_STRINGSDEC_ const U8 g_Enterprise4Str[21];
_STRINGSDEC_ const U8 g_Enterprise5Str[21];
_STRINGSDEC_ const U8 g_Enterprise6Str[21];
_STRINGSDEC_ const U8 g_Enterprise7Str[21];
_STRINGSDEC_ const U8 g_Enterprise8Str[21];
_STRINGSDEC_ const U8 g_Enterprise9Str[21];
_STRINGSDEC_ const U8 g_Enterprise10Str[21];
_STRINGSDEC_ const U8 g_Enterprise11Str[21];
_STRINGSDEC_ const U8 g_Enterprise12Str[21];
_STRINGSDEC_ const U8 g_Enterprise13Str[21];
_STRINGSDEC_ const U8 g_Enterprise201Str[21];
_STRINGSDEC_ const U8 g_Enterprise202Str[21];
_STRINGSDEC_ const U8 g_Enterprise203Str[21];
_STRINGSDEC_ const U8 g_Enterprise204Str[21];
_STRINGSDEC_ const U8 g_Enterprise205Str[21];
_STRINGSDEC_ const U8 g_Enterprise206Str[21];
_STRINGSDEC_ const U8 g_Enterprise301Str[21];
_STRINGSDEC_ const U8 g_Enterprise8476Str[21];
_STRINGSDEC_ const U8 g_Enterprise8721Str[21];
_STRINGSDEC_ const U8 g_Enterprise8722Str[21];
_STRINGSDEC_ const U8 g_Enterprise8723Str[21];
_STRINGSDEC_ const U8 g_Enterprise8725Str[21];
_STRINGSDEC_ const U8 g_Enterprise8726Str[21];
_STRINGSDEC_ const U8 g_Enterprise8727Str[21];
*/
_STRINGSDEC_ const U8 g_SetTimeStr[15];
_STRINGSDEC_ const U8 g_SetTimeStrEn[11];
_STRINGSDEC_ const U8 g_SetStaticIPStr[13];
_STRINGSDEC_ const U8 g_SetStaticIPStrEn[16];
_STRINGSDEC_ const U8  g_DeadlineFmtStr[21];
_STRINGSDEC_ const U8 g_InputSeriesStr[16];
_STRINGSDEC_ const U8 g_SetSeriesStr[21];
_STRINGSDEC_ const U8 g_InputOldSeriesStr[21];
_STRINGSDEC_ const U8 g_SetTeleNumStr[21];
_STRINGSDEC_ const U8 g_SetTechTeleNumStr[21];
_STRINGSDEC_ const U8  g_SetSerialNumOKStr[9];
_STRINGSDEC_ const U8  g_SetSerialNumNotStr[21];
_STRINGSDEC_  const U8 g_CycleScheduleStr[11];
_STRINGSDEC_  const U8 g_CycleScheduleStrEn[13];

_STRINGSDEC_  const U8 g_ExeModeStr[15];
_STRINGSDEC_  const U8 g_ExeModeStrEn[19];
_STRINGSDEC_  const U8 g_waitStr[11];
_STRINGSDEC_  const U8 g_waitStrEn[14];

_STRINGSDEC_  const U8 g_DirectSetMainSizeStr[21];
_STRINGSDEC_  const U8 g_DirectSetSubSizeStr[21];
_STRINGSDEC_  const U8 g_MainInputStr[21];
_STRINGSDEC_  const U8 g_SubInputStr[21];

_STRINGSDEC_  const U8 g_CancelSuccessStrEn[15];
_STRINGSDEC_  const U8 g_CancelSuccessStr[10];
_STRINGSDEC_ const U8  g_SetHotBackupStr[13];
_STRINGSDEC_ const U8  g_SetHotBackupStrEn[19];
_STRINGSDEC_ const U8 g_UserHotBackupStr[13];
_STRINGSDEC_ const U8 g_HotBackupStartStr[13];
_STRINGSDEC_ const U8 g_InputSignalStr[14];//输入信号源:
_STRINGSDEC_ const U8 g_BackupSignalStr[12];//备份信号源:
_STRINGSDEC_ const U8 g_SwapImageStr[9];
_STRINGSDEC_ const U8 g_SetFadeTimeStr[19];
_STRINGSDEC_ const U8 g_SetFadeTimeStrEn[16];

_STRINGSDEC_ const U8 g_SaturationStr[11];
_STRINGSDEC_ const U8 g_SaturationStrEn[13];
_STRINGSDEC_ const U8 g_ColorResetStr[7];
_STRINGSDEC_ const U8 g_ColorResetStrEn[8];
_STRINGSDEC_ const U8 g_AdcCalibrationStr[11];
_STRINGSDEC_ const U8 g_AdcCalibrationStrEn[18];
_STRINGSDEC_  const U8 g_UserResolutionStr[15];

_STRINGSDEC_ const U8 g_SwitchInputModeStr[13];
_STRINGSDEC_ const U8 g_SwitchInputModeStrEn[12];
_STRINGSDEC_ const U8 g_FadeInFadeOutStr[9];
_STRINGSDEC_ const U8 g_FadeInFadeOutStrEn[17];
_STRINGSDEC_ const U8 g_QuickSwitchStr[9];
_STRINGSDEC_ const U8 g_QuickSwitchStrEn[13];
_STRINGSDEC_ const U8 g_FuseSwitchStr[9];
_STRINGSDEC_ const U8  g_OutputHzStr[15];
_STRINGSDEC_ const U8  g_OutputHzStrEn[19];
_STRINGSDEC_ const U8 g_InputMinuteStr[21];
_STRINGSDEC_ const U8 g_InputTodayPwdStr[21];


_STRINGSDEC_ const U8 g_SaveGuide[16];
_STRINGSDEC_ const U8 g_SaveGuideEn[16];
//void InitMathineType(void);

#endif
