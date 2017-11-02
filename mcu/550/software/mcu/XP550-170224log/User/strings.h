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

//#include "../include/net_config.h"


#ifdef _STRINGS_C_
  #define _STRINGSDEC_
#else
  #define _STRINGSDEC_ extern
#endif


_STRINGSDEC_  const U8 g_VideoComing[16];
_STRINGSDEC_  const U8 g_DirectSetMainSizeStr[21];
_STRINGSDEC_  const U8 g_DirectSetSubSizeStr[21];
_STRINGSDEC_ const U8 g_ReleaseStr[8];
_STRINGSDEC_ const U8 g_BothSwithStr[21];
_STRINGSDEC_ const U8 g_BothSwithStrEn[21];
_STRINGSDEC_ const U8 g_StartStr[7];
_STRINGSDEC_ const U8 g_CancelStr[7];
_STRINGSDEC_ const U8 g_StartStrEn[5];
_STRINGSDEC_ const U8 g_CancelStrEn[5];
_STRINGSDEC_ U8 g_ContactTel1[13];
_STRINGSDEC_ U8 g_ContactTel2[13];
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


_STRINGSDEC_ const U8 g_InputMinuteStr[21];
_STRINGSDEC_ const U8 g_LogHadReadOut[17];
_STRINGSDEC_ const U8 g_noLog[21];
_STRINGSDEC_ const U8 g_openLog[21];
_STRINGSDEC_ const U8 g_closedLog[21];

_STRINGSDEC_ const U8 g_charOpen[3];
_STRINGSDEC_ const U8 g_charClose[3];


_STRINGSDEC_ U8 g_WelcomeStr[21];
_STRINGSDEC_ U8 g_WelcomeStrEn[21];
_STRINGSDEC_ const U8 g_MuteStr[5];
_STRINGSDEC_ const U8 g_MuteStrEn[5];
_STRINGSDEC_ const U8 g_InputStatusStr[8];
_STRINGSDEC_ const U8 g_InputStatusStrEn[7];
_STRINGSDEC_ const U8 g_Factory[7];
_STRINGSDEC_ const U8 g_FactoryEn[8];

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
_STRINGSDEC_ const U8 g_DVIStr[12];
_STRINGSDEC_ const U8 g_DVIStrEn[4];
_STRINGSDEC_ const U8 g_YPbPrStr[14];
_STRINGSDEC_ const U8 g_YPbPrStrEn[6];
_STRINGSDEC_ const U8 g_VGAStr[8];
_STRINGSDEC_ const U8 g_VGAStrEn[4];
_STRINGSDEC_ const U8 HDMI[13];
_STRINGSDEC_  const U8 HDMIEn[5];
_STRINGSDEC_  const U8 g_AllChannelNoSYNCStr[17];
_STRINGSDEC_  const U8 g_SingleSwapWarnningStr[21];
_STRINGSDEC_  const U8 g_SingleSwitchStr[11];
_STRINGSDEC_  const U8 g_MainSwitchStr[10];
_STRINGSDEC_  const U8 g_SubSwitchStr[10];
//-------------------schedule string-------------------------------------------
//方案1
_STRINGSDEC_  const U8 g_Program1Str[6];
_STRINGSDEC_  const U8 g_Program1StrEn[10];
_STRINGSDEC_  const U8 g_Program2StrEn[10];
_STRINGSDEC_  const U8 g_Program3StrEn[10];
//方案2
_STRINGSDEC_  const U8 g_Program2Str[6];
//方案3
_STRINGSDEC_  const U8 g_Program3Str[6];
_STRINGSDEC_  const U8 g_DateStrEn[17];
_STRINGSDEC_  const U8 g_DateStr[17];
_STRINGSDEC_  const U8 g_TimeStr[12];
//时间
_STRINGSDEC_  U8 g_CurrentTimeStr[19];
_STRINGSDEC_  const U8 g_CurrentTimeStrEn[12];
//时间  00: 00 (24制)
_STRINGSDEC_  const U8 g_ScheTimeStr[20];
_STRINGSDEC_  const U8 g_ScheTimeStrEn[16];
//时长00: 00 (24制)
_STRINGSDEC_  const U8 g_CycleTimeStr[20];
//设置方案
_STRINGSDEC_  const U8 g_SetScheduleStr[9];
_STRINGSDEC_  const U8 g_SetScheduleStrEn[8];
//取消方案
_STRINGSDEC_  const U8 g_CancelScheduleStr[9];
_STRINGSDEC_  const U8 g_CancelScheduleStrEn[15];
_STRINGSDEC_  const U8 g_ScheduleModeStr[11];
_STRINGSDEC_  const U8 g_ScheduleModeStrEn[14];
_STRINGSDEC_  const U8 g_ScheduleOnceStr[9];
_STRINGSDEC_  const U8 g_ScheduleOnceStrEn[14];
_STRINGSDEC_  const U8 g_ScheduleEverydayStr[9];
_STRINGSDEC_  const U8 g_ScheduleEverydayStrEn[15];
_STRINGSDEC_  const U8 g_ScheduleWhichDayStr[11];
_STRINGSDEC_  const U8 g_ScheduleWhichDayStrEn[18];

_STRINGSDEC_  const U8 g_CancelSuccessStrEn[15];
_STRINGSDEC_  const U8 g_CancelSuccessStr[10];
//------------------------------------------------------------------------------

_STRINGSDEC_ const U8 g_TryForUsr[15];
_STRINGSDEC_ const U8 g_TryOverStr[11];
_STRINGSDEC_ const U8 g_PurchaseStr[7];

_STRINGSDEC_ const U8 g_KeyWarnningStr[10];
_STRINGSDEC_ const U8 g_PCControlStr[20];

_STRINGSDEC_  const U8 g_fullZoomEn[13];
_STRINGSDEC_  const U8 g_fullZoom[13];
_STRINGSDEC_  const U8 g_jubuEn[5];
_STRINGSDEC_  const U8 g_jubu[11];
_STRINGSDEC_  const U8 g_fullEn[5];
_STRINGSDEC_  const U8 g_full[9];
_STRINGSDEC_  const U8 g_SingleOrDualEn[19];
_STRINGSDEC_  const U8 g_SingleOrDual[21];
_STRINGSDEC_  const U8 g_SingleChEn[7];
_STRINGSDEC_  const U8 g_SingleCh[7];
_STRINGSDEC_  const U8 g_PBPEn[4];
_STRINGSDEC_  const U8 g_PBP[7];
_STRINGSDEC_  const U8 g_PIPEn[4];
_STRINGSDEC_  const U8 g_PIP[7];
_STRINGSDEC_  const U8 g_ActiveImage[13];
_STRINGSDEC_  const U8 g_ActiveImageEn[13];
_STRINGSDEC_  const U8 g_ErrorDualInputStr[16];
_STRINGSDEC_  const U8 g_ErrorDualInputStrEn[12];
_STRINGSDEC_  const U8 g_WarnningStr[11];
_STRINGSDEC_  const U8 g_WarnningStrEn[8];
_STRINGSDEC_  const U8 g_MainImage[6];
_STRINGSDEC_  const U8 g_MainImageEn[5];
_STRINGSDEC_  const U8 g_SubImage[6];
_STRINGSDEC_  const U8 g_SubImageEn[4];
_STRINGSDEC_  const U8 g_MainMenu[7];
_STRINGSDEC_  const U8 g_MainMenuEn[10];
_STRINGSDEC_  const U8 g_MainMenuNum[14];
_STRINGSDEC_  const U8 g_MainMenuNumEn[13];
_STRINGSDEC_  const U8 g_ScreenPara[17];
_STRINGSDEC_  const U8 g_ScreenParaEn[20];
_STRINGSDEC_ const U8 g_ScreenSizeStr[11];
_STRINGSDEC_  const U8 g_ScalerSizeStr[11];
_STRINGSDEC_  const U8 g_ZoomSizeStr[11];
_STRINGSDEC_  const U8 g_StartPos[11];
_STRINGSDEC_ const U8 g_ScalerPara[17];
_STRINGSDEC_ const U8 g_ScalerParaEn[21];
_STRINGSDEC_  const U8 g_ZoomPara[17];
_STRINGSDEC_  const U8 g_ZoomParaEn[14];
_STRINGSDEC_  const U8 g_InputZoomStr[17];
_STRINGSDEC_  const U8 g_ColorPara[13];
_STRINGSDEC_  const U8 g_ColorParaEn[15];
_STRINGSDEC_  const U8 g_AdvancePara[13];
_STRINGSDEC_  const U8 g_AdvanceParaEn[17];
_STRINGSDEC_  const U8 g_SingleCharacter[3];
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
_STRINGSDEC_ const U8 g_SetTimeStr[15];
_STRINGSDEC_ const U8 g_SetTimeStrEn[11];
_STRINGSDEC_ const U8 g_SetFadeTimeStr[19];
_STRINGSDEC_ const U8 g_SetFadeTimeStrEn[16];
_STRINGSDEC_ const U8  g_SetHotBackupStr[13];
_STRINGSDEC_ const U8  g_SetHotBackupStrEn[19];
_STRINGSDEC_ const U8 g_UserHotBackupStr[13];
_STRINGSDEC_ const U8 g_HotBackupStartStr[13];
//输入信号源:
_STRINGSDEC_ const U8 g_InputSignalStr[14];
//备份信号源:
_STRINGSDEC_ const U8 g_BackupSignalStr[12];
_STRINGSDEC_  const U8 g_TimeScheduleStr[13];
_STRINGSDEC_  const U8 g_TimeScheduleStrEn[18];
_STRINGSDEC_  const U8 g_InputScheduleStr[11];
_STRINGSDEC_  const U8 g_InputScheduleStrEn[17];
_STRINGSDEC_  const U8 g_ModeScheduleStr[11];
_STRINGSDEC_  const U8 g_ModeScheduleStrEn[16];
_STRINGSDEC_  const U8 g_CycleScheduleStr[11];
_STRINGSDEC_  const U8 g_CycleScheduleStrEn[13];
_STRINGSDEC_  const U8 g_CancelAllScheduleStr[15];
_STRINGSDEC_  const U8 g_CancelAllScheduleStrEn[18];
_STRINGSDEC_  const U8 g_TechSupport[13];
_STRINGSDEC_  const U8 g_TechSupportEn[17];
_STRINGSDEC_  const U8 g_SetUpModeStr[13];
_STRINGSDEC_  const U8 g_SetUpModeStrEn[11];
_STRINGSDEC_  const U8 g_InputSetupStr[15];
_STRINGSDEC_  const U8 g_InputSetupStrEn[16];

_STRINGSDEC_  const U8 g_SelHStr[5];
_STRINGSDEC_  const U8 g_SelVStr[5];
_STRINGSDEC_  const U8 g_SelHStrEn[9];
_STRINGSDEC_  const U8 g_SelVStrEn[9];
_STRINGSDEC_  const U8 g_SelScalerHStr[13];
_STRINGSDEC_  const U8 g_SelSclaerVStr[13];
_STRINGSDEC_  const U8 g_SelZoomHStr[13];
_STRINGSDEC_  const U8 g_SelZoomVStr[13];
_STRINGSDEC_  const U8 g_UserResolutionStr[15];
_STRINGSDEC_  const U8 g_UserSetResolutionStr[15];
_STRINGSDEC_  const U8 g_SelHPosStr[13];
_STRINGSDEC_  const U8 g_SelVPosStr[13];
_STRINGSDEC_ const U8 g_UsedDoubleStr[13];
_STRINGSDEC_ const U8 g_PleaseSetScreenStr[13];

_STRINGSDEC_ const U8 g_SwitchInputModeStr[13];
_STRINGSDEC_ const U8 g_SwitchInputModeStrEn[12];
_STRINGSDEC_ const U8 g_FadeInFadeOutStr[9];
_STRINGSDEC_ const U8 g_FadeInFadeOutStrEn[17];
_STRINGSDEC_ const U8 g_QuickSwitchStr[9];
_STRINGSDEC_ const U8 g_FuseSwitchStr[9];
_STRINGSDEC_ const U8 g_QuickSwitchStrEn[13];
_STRINGSDEC_ const U8 g_SwapImageStr[9];

_STRINGSDEC_  const U8 g_BrightnessStr[7];
_STRINGSDEC_  const U8 g_BrightnessEn[13];
/*_STRINGSDEC_  const U8 g_OffsetRStr[11];
_STRINGSDEC_  const U8 g_OffsetREn[13];
_STRINGSDEC_  const U8 g_OffsetGStr[11];
_STRINGSDEC_  const U8 g_OffsetGEn[15];
_STRINGSDEC_  const U8 g_OffsetBStr[11];
_STRINGSDEC_  const U8 g_OffsetBEn[14];*/
_STRINGSDEC_  const U8 g_ContractStr[9];
_STRINGSDEC_  const U8 g_ContractEn[11];
_STRINGSDEC_ const U8 g_SaturationStr[11];
_STRINGSDEC_ const U8 g_SaturationStrEn[13];
_STRINGSDEC_ const U8 g_ColorResetStr[7];
_STRINGSDEC_ const U8 g_ColorResetStrEn[8];
_STRINGSDEC_ const U8 g_AdcCalibrationStr[11];
_STRINGSDEC_ const U8 g_AdcCalibrationStrEn[18];
/*_STRINGSDEC_  const U8 g_GainRStr[13];
_STRINGSDEC_  const U8 g_GainREn[11];
_STRINGSDEC_  const U8 g_GainGStr[13];
_STRINGSDEC_  const U8 g_GainGEn[13];
_STRINGSDEC_  const U8 g_GainBStr[13];
_STRINGSDEC_  const U8 g_GainBEn[12];*/
_STRINGSDEC_  const U8 g_outputFormatstr[13];
_STRINGSDEC_  const U8 g_outputFormatstrEn[16];
_STRINGSDEC_  const U8 g_FreezeImage[9];
_STRINGSDEC_  const U8 g_FreezeImageEn[13];

_STRINGSDEC_ const U8 g_AudioStr[11];
_STRINGSDEC_ const U8 g_AudioStrEn[12];
_STRINGSDEC_ const U8 g_VolumeStr[9];
_STRINGSDEC_ const U8 g_VolumeStrEn[11];
_STRINGSDEC_ const U8 g_LockKeypadStr[21];
_STRINGSDEC_ const U8 g_LockKeypadStrEN[17];
_STRINGSDEC_ const U8 g_CancelLockKeypadStr[18];
_STRINGSDEC_  const U8 g_FactoryReset[17];
_STRINGSDEC_  const U8 g_FactoryResetEn[16];

_STRINGSDEC_ const U8 g_EEPROMErrorStr[21];
_STRINGSDEC_  const U8 g_DirectSetStr[11];
_STRINGSDEC_  const U8 g_DirectSetStrEn[17];
_STRINGSDEC_  const U8 g_Language[11];
_STRINGSDEC_  const U8 g_LanguageEn[11];
_STRINGSDEC_  const U8 g_FreezeStr[5];
_STRINGSDEC_  const U8 g_FreezeStrEn[7];
_STRINGSDEC_  const U8 g_LiveStr[5];
_STRINGSDEC_  const U8 g_LiveStrEn[5];
_STRINGSDEC_  const U8 g_Mode1Str[12];
_STRINGSDEC_  const U8 g_Mode1StrEn[9];
_STRINGSDEC_  const U8 g_Mode2StrEn[9];
_STRINGSDEC_  const U8 g_Mode3StrEn[9];
_STRINGSDEC_  const U8 g_Mode2Str[12];
_STRINGSDEC_  const U8 g_Mode3Str[12];
_STRINGSDEC_  const U8 g_ContactInfo[11];
_STRINGSDEC_  const U8 g_ContactInfoEn[15];
_STRINGSDEC_  const U8 g_VersionInfo[7];
_STRINGSDEC_  const U8 g_VersionInfoEn[15];
//该操作将清除保存数据
_STRINGSDEC_  const U8 g_Yes[21];
_STRINGSDEC_  const U8 g_YesEn[17];
//是否继续
_STRINGSDEC_  const U8 g_YesStr[11];
_STRINGSDEC_  const U8 g_YesStrEn[10];
_STRINGSDEC_  const U8 g_Chinese[5];
//_STRINGSDEC_  const U8 g_ResettingStr[15];
//_STRINGSDEC_  const U8 g_ResettingStrEn[14];
_STRINGSDEC_  const U8 g_ExeModeStr[15];
_STRINGSDEC_  const U8 g_ExeModeStrEn[19];
_STRINGSDEC_  const U8 g_waitStr[11];
_STRINGSDEC_  const U8 g_waitStrEn[14];
_STRINGSDEC_  const U8 g_ResetSuccessStrEn[14];
_STRINGSDEC_  const U8 g_ResetSuccessStr[11];
_STRINGSDEC_  const U8 g_SavingStrEn[12];
_STRINGSDEC_  const U8 g_SavingStr[11];
_STRINGSDEC_  const U8 g_SaveOKStrEn[13];
_STRINGSDEC_  const U8 g_SaveOKStr[10];
_STRINGSDEC_  const U8 g_LoadingModeStrEn[13];
_STRINGSDEC_  const U8 g_LoadingModeStr[15];
_STRINGSDEC_  const U8 g_LoadModeOKStrEn[13];
_STRINGSDEC_  const U8 g_LoadModeOKStr[15];
_STRINGSDEC_  const U8 g_LoadNoModeStrEn[8];
_STRINGSDEC_  const U8 g_LoadNoModeStr[19];
_STRINGSDEC_  const U8 g_HasSaveValueStrEn[20];
_STRINGSDEC_  const U8 g_HasSaveValueStr[19];
_STRINGSDEC_  const U8 g_SaveOverlapStrEn[9];
_STRINGSDEC_  const U8 g_SaveOverlapStr[10];
_STRINGSDEC_  const U8 g_SaveOverlappingStrEn[16];
_STRINGSDEC_  const U8 g_SaveOverlappingStr[15];
_STRINGSDEC_  const U8 g_LockKeyStr[11];
_STRINGSDEC_  const U8 g_LockKeyStrEn[19];
_STRINGSDEC_  const U8 g_UnLockKeyStr[11];
_STRINGSDEC_  const U8 g_UnLockKeyStrEn[18];
_STRINGSDEC_  const U8 g_LockPromptStr[15];
_STRINGSDEC_  const U8 g_LockPromptStrEn[20];
//该方案已设置
_STRINGSDEC_ const U8  g_HadSetStr[13];
_STRINGSDEC_ const U8  g_HadSetStrEn[8];
//按退格键修改或删除
_STRINGSDEC_ const U8  g_BackForChangeStr[19];
_STRINGSDEC_ const U8  g_BackForChangeStrEn[10];
_STRINGSDEC_  U16 TimeSet[6];
_STRINGSDEC_ U16 g_TryEndDate[3];

_STRINGSDEC_ const U8 g_ScreenInfo1Str[6];
_STRINGSDEC_ const U8 g_InResolutionStr[6];
_STRINGSDEC_ const U8 g_OutResolutionStr[6];
_STRINGSDEC_ const U8 g_MachineTypeStr[6];
_STRINGSDEC_ const U8 g_MachineTypeStrEn[6];
_STRINGSDEC_ const U8 g_SoftVersionStr[10];
_STRINGSDEC_ const U8 g_StartTimeStr[15];
_STRINGSDEC_ const U8 g_StartTimeFormatStr[13];
_STRINGSDEC_ const U8 g_MathineIDStr[8];
_STRINGSDEC_ const U8  g_DeadlineFmtStr[21];
_STRINGSDEC_ const U8 g_InputSeriesStr[14];
_STRINGSDEC_ const U8  g_SetSerialNumOKStr[9];
_STRINGSDEC_ const U8  g_SetSerialNumNotStr[21];
_STRINGSDEC_ const U8 g_SetStaticIPStr[13];
_STRINGSDEC_ const U8 g_SetStaticIPStrEn[16];

_STRINGSDEC_ const U8  g_OutputHzStr[15];
_STRINGSDEC_ const U8  g_OutputHzStrEn[19];

_STRINGSDEC_ const U8 g_InputPackagingNameStr[20];
_STRINGSDEC_ const U8 g_InputTestNameStr[20];
_STRINGSDEC_ const U8 g_InputSaleNameStr[20];
_STRINGSDEC_ const U8 g_YangShanNameStr[7];
_STRINGSDEC_ const U8 g_HuangDanNameStr[7];
_STRINGSDEC_ U8 g_PackagingName[7];
_STRINGSDEC_ U8 g_TestName[7];
_STRINGSDEC_ U8 g_SaleName[7];


_STRINGSDEC_ const U8 g_InputLogoStr[21];
_STRINGSDEC_ const U8 g_OutRangeStr[5];
_STRINGSDEC_ const U8  g_InputMathineTypeStr[21];
_STRINGSDEC_ const U8 g_DeadLine7DaysStr[18];

_STRINGSDEC_ const U8 g_SupportTechStr[9];
_STRINGSDEC_ const U8  g_SupportTechStrEn[9];
_STRINGSDEC_ const U8 g_SetSeriesStr[21];
_STRINGSDEC_ const U8 g_InputOldSeriesStr[21];
_STRINGSDEC_ const U8 g_SetTeleNumStr[21];
_STRINGSDEC_ const U8 g_SetTechTeleNumStr[21];
_STRINGSDEC_ U8 g_MachineTypeXPStr[21];


_STRINGSDEC_  const U8 g_NoEnterpriseStr[21];
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
_STRINGSDEC_ const U8 g_Enterprise7075Str[21];
_STRINGSDEC_ const U8 g_Enterprise7076Str[21];
_STRINGSDEC_ const U8 g_Enterprise4958Str[21];


//20170213 lyg
_STRINGSDEC_ const U8 g_ShowTimeStr[10];
_STRINGSDEC_ const U8 g_ShowTimeStrEn[15];
	
_STRINGSDEC_ const U8 g_OnceTimeStr[10];
_STRINGSDEC_ const U8 g_OnceTimeStrEn[15];

_STRINGSDEC_ const U8 g_ShowTimeRangeStr[15]; //演示时长范围
_STRINGSDEC_ const U8 g_ShowTimeRangeStrEn[15];

_STRINGSDEC_ const U8 g_OnceTimeRangeStr[15]; //单次时长范围
_STRINGSDEC_ const U8 g_OnceTimeRangeStrEn[15];


_STRINGSDEC_ const U8 g_ShowTimeLongStr[15];//演示时长  分钟
_STRINGSDEC_ const U8 g_OnceTimeLongStr[15];//单次时长  分钟
	
_STRINGSDEC_ const U8 g_OnceTelStr[13];
_STRINGSDEC_ const U8 g_OnceTelStrEn[14];


_STRINGSDEC_ const U8 g_OnceEndInputPwStr[12]; //输入密码：
_STRINGSDEC_ const U8 g_OnceEndInputPwStrEn[11]; //输入密码：

_STRINGSDEC_ const U8 g_InputManagePwStr[16]; //输入管理密码：
_STRINGSDEC_ const U8 g_InputManagePwStrEn[16]; //输入密码：

_STRINGSDEC_ const U8 g_OnceEndTelStr[6]; //联系电话：
_STRINGSDEC_ const U8 g_OnceEndTelStrEn[4]; //联系电话：


_STRINGSDEC_ const U8 g_OnceEndIDStr[8]; //机器ID;
_STRINGSDEC_ const U8 g_OnceEndIDStrEn[10];

_STRINGSDEC_ const U8 g_OnceEndTimeStr[6];  //时间：
_STRINGSDEC_ const U8 g_OnceEndTimeStrEn[5];

_STRINGSDEC_ const U8 g_ShowTimeStartStr[13]; //演示密码开启
_STRINGSDEC_ const U8 g_ShowTimeStopStr[13]; //演示密码关闭

_STRINGSDEC_ const U8 g_OnceTimeStartStr[13]; //单次密码开启
_STRINGSDEC_ const U8 g_OnceTimeStopStr[13]; //单次密码关闭
_STRINGSDEC_ const U8 g_ManagePasswordOK[17]; //管理密码设置完成

_STRINGSDEC_ const U8 g_InputNewPwStr[14]; //输入新密码：
_STRINGSDEC_ const U8 g_InputNewPwStrEn[13]; //输入新密码：

_STRINGSDEC_ const U8 g_InputNew1PwStr[18]; //再次输入新密码：
_STRINGSDEC_ const U8 g_InputNew1PwStrEn[17]; //输入密码

_STRINGSDEC_ const U8 g_Tel_display[2];
_STRINGSDEC_ const u8 g_Password101_on_Str[17]; //开启单次密码功能
_STRINGSDEC_ const u8 g_Password101_off_Str[17]; //开启单次密码功能

_STRINGSDEC_ const u8 g_Password102_on_Str[17]; //开启单次密码功能
_STRINGSDEC_ const u8 g_Password102_off_Str[17]; //开启单次密码功能

_STRINGSDEC_ const u8 g_Password103_Str[17]; //设置授权联系电话

_STRINGSDEC_ const u8 g_Password104_Str[17]; //设置演示密码时长
_STRINGSDEC_ const u8 g_Password105_Str[17]; //设置单次密码时长
_STRINGSDEC_ const u8 g_Password100_Str[13]; //重置管理密码

#endif
