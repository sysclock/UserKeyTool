/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: statemachine.c
 [Date]:        11-May-2012
 [Comment]:
   StateMachine  file.
 [Reversion History]:
*******************************************************************************/
#include "header.h"
#include <stdio.h>

#define _STATEMACHINE_C_

#if(XP_TO_ID==XP_TO_FLI32626)
U8 g_initPIPSrc=0;
#else
//U8 g_initPIP=0;
//U8 g_initPBP=0;
#endif

extern SetTimeFlag g_SetTimeFlag;

void SetSaveMode(U8 mode)
{	
	if(!IsHasSave(mode))
	{
		clearlcd();
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x83, g_SavingStr);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x83, g_SavingStrEn);
		
		Save(mode);
		clearlcd();
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x83, g_SaveOKStr);	
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x83, g_SaveOKStrEn);		
		delay(5);
		//DealEscKey();
		//g_CurrentState=State_MainMenu_Menu;   //save ok return main menu
		ReturnMainWin();
	}
	else
	{
		clearlcd();
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x80, g_HasSaveValueStr);
			chinesedisplay(0x92, g_SaveOverlapStr);
		}
		else  if(g_LanguageFlag==ENGLISH)
		{
			chinesedisplay(0x80, g_HasSaveValueStrEn);
			chinesedisplay(0x92, g_SaveOverlapStrEn);
		}
		g_CurrentState=State_SavePara_Mode_check;    //user want to overlap 
	}
}

void SetUserMode(U8 mode)
{
	if(IsHasSave(mode))
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x81, g_LoadingModeStr);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x81, g_LoadingModeStrEn);
		if(Updata_Execute(EEPROMMemRegU8,EEPROMMemRegU16))
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x81, g_LoadModeOKStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x81, g_LoadModeOKStrEn);
		}
	}
	else
	{
		clearlcd();
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x80, g_LoadNoModeStr);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x80, g_LoadNoModeStrEn);
	}
}

#if(XP_TO_ID==XP_TO_FLI32626)
void ScreenParaToEEPROM()
{	
	Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
	Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
	Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
	Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
}
#endif

BOOL ResetPara()
{	
#if(XP_TO_ID==XP_TO_FLI32626)
	//�ָ�1024*768����ֱ���
	g_OutputFormatIndex=INDEX1024X768;
	SetOutputFormat(g_OutputFormatIndex);
	Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
	LoadOutputResolution();
#endif

#ifdef HOT_BACKUP
	//�ȱ��ݽ�ֹ
	initHotBackupVariable(__FALSE);
	Write_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,g_EnableHotBackupFlag);
	Write_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,g_HotBackupIdx);
	Write_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
#endif


	if(reset())
	{
		if(Updata_Execute(genericEEPROMMemU8,genericEEPROMMemU16))
		{
			//ѭ�����Ž�ֹ
			g_InputCycleIdx=0;
			g_StartCycleInputFlag=__FALSE;
			initCycleInputPara();
			Write_EEPROM_Byte(E2ROM_CycleFlag_ADDR, g_StartCycleInputFlag);
			//�ظ�ʮ�����Զ�������
			//g_LockKeypadTime=10;
			g_OffBackLightTime=g_LockKeypadTime*3+15;
			//Write_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR,10);
			//ͨ���ƻ���ֹ
			CancelInputScheduleMode1();
			CancelInputScheduleMode2();
			CancelInputScheduleMode3();
		#if(XP_TO_ID==XP_TO_FLI32626)		
			//��λ�����������Ž�ȡ����ɫ��ֵ
			PanelToScreenPara();
			ScreenParaToEEPROM();
			g_AlphaValue=0;
			SetMultiWindowPara(FUSEPIPPARA1);
		#else
//			g_initPBP=0;
//			g_initPIP=0;
		#endif
			return __TRUE;
		}
	}
	return __FALSE;
}

void SetSingleImage()
{
	SetMultiWindowPara(SINGLEIMAGE);
#if(XP_TO_ID==XP_TO_FLI32626)	
	g_SwitchEffectValue=NormalSwitchIdx;
	g_MainScalerHSize=g_ScreenHSize;
	g_MainScalerVSize=g_ScreenVSize;
	g_MainScalerHPos=g_ScreenHPos;
	g_MainScalerVPos=g_ScreenVPos;
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
#else
//	if(g_initPBP==1)
//	{
		g_MainScalerHSize=g_ScreenHSize;
		g_MainScalerVSize=g_ScreenVSize;
		g_MainScalerHPos=g_ScreenHPos;
		g_MainScalerVPos=g_ScreenVPos;
//	}
#endif
	g_MutiWindow=SINGLEIMAGE;		
	Save(MODE1);
}
void SetPIPImage()
{
	SetMultiWindowPara(PIPIMAGE);				
	g_MutiWindow=PIPIMAGE;
#if(XP_TO_ID==XP_TO_FLI32626)
	g_SwitchEffectValue=NormalSwitchIdx;

	g_MainScalerHSize=g_ScreenHSize;
	g_MainScalerVSize=g_ScreenVSize;
	g_MainScalerHPos=g_ScreenHPos;
	g_MainScalerVPos=g_ScreenVPos;
	g_SubScalerHSize=g_ScreenHSize/3;
	g_SubScalerVSize=g_ScreenVSize/3;
	g_SubScalerHPos=g_ScreenHPos;
	g_SubScalerVPos=g_ScreenVPos;
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
	SetScalerHVValue(HVSIZE,SUB);
	SetScalerHVValue(HVPOS,SUB);
	
	if(g_initPIPSrc==0)
	{
		g_initPIPSrc=1;
		g_SubInput=INITPIPSRC;
	}
	
	if(!CheckDualSource(g_SubInput-1, g_MainInput-1))
	{
		g_SubInput=g_MainInput;
	}
#else
//	if(g_initPIP==0)
//	{
		g_MainScalerHSize=g_ScreenHSize;
		g_MainScalerVSize=g_ScreenVSize;
		g_MainScalerHPos=g_ScreenHPos;
		g_MainScalerVPos=g_ScreenVPos;
		g_SubScalerHSize=g_ScreenHSize/3;
		g_SubScalerVSize=g_ScreenVSize/3;
		g_SubScalerHPos=g_ScreenHPos;
		g_SubScalerVPos=g_ScreenVPos;
//		g_initPIP=1;
//	}
	SetScalerValue(HSIZE, g_MainScalerHSize, MAIN);
	SetScalerValue(VSIZE, g_MainScalerVSize, MAIN);
	SetScalerValue(HPOS, g_MainScalerHPos, MAIN);
	SetScalerValue(VPOS, g_MainScalerVPos, MAIN);
	SetScalerValue(HSIZE, g_SubScalerHSize, SUB);
	SetScalerValue(VSIZE, g_SubScalerVSize, SUB);
	SetScalerValue(HPOS, g_SubScalerHPos, SUB);
	SetScalerValue(VPOS, g_SubScalerVPos, SUB);
	if(g_MainInput==INPUTSOURCE_VGA)
		SetInputSource(INPUTSOURCE_VGA, SUB);
	g_SubInput=g_MainInput;
#endif	
	Save(MODE1);
}

void SetPBPImage()
{
	SetMultiWindowPara(PBPIMAGE);				
	g_MutiWindow=PBPIMAGE;
#if(XP_TO_ID==XP_TO_FLI32626)
	g_SwitchEffectValue=NormalSwitchIdx;
	
	g_MainScalerHSize=g_ScreenHSize/2;
	g_MainScalerVSize=g_ScreenVSize;
	g_MainScalerHPos=g_ScreenHPos;
	g_MainScalerVPos=g_ScreenVPos;
	g_SubScalerHSize=g_ScreenHSize/2;
	g_SubScalerVSize=g_ScreenVSize;
	g_SubScalerHPos=g_ScreenHSize/2+g_ScreenHPos;
	g_SubScalerVPos=g_ScreenVPos;
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
	SetScalerHVValue(HVSIZE,SUB);
	SetScalerHVValue(HVPOS,SUB);
	
	if(g_initPIPSrc==0)
	{
		g_initPIPSrc=1;
		g_SubInput=INITPIPSRC;
	}
	if(!CheckDualSource(g_SubInput-1, g_MainInput-1))
	{
		g_SubInput=g_MainInput;
	}
#else
//	if(g_initPBP==0)
//	{
		g_MainScalerHSize=g_ScreenHSize/2;
		g_MainScalerVSize=g_ScreenVSize;
		g_MainScalerHPos=g_ScreenHPos;
		g_MainScalerVPos=g_ScreenVPos;
		g_SubScalerHSize=g_ScreenHSize/2;
		g_SubScalerVSize=g_ScreenVSize;
		g_SubScalerHPos=(g_ScreenHSize/2+g_ScreenHPos);
		g_SubScalerVPos=g_ScreenVPos;
//		g_initPBP=1;
//	}
	SetScalerValue(HSIZE, g_MainScalerHSize, MAIN);
	SetScalerValue(VSIZE, g_MainScalerVSize, MAIN);
	SetScalerValue(HPOS, g_MainScalerHPos, MAIN);
	SetScalerValue(VPOS, g_MainScalerVPos, MAIN);
	SetScalerValue(HSIZE, g_SubScalerHSize, SUB);
	SetScalerValue(VSIZE, g_SubScalerVSize, SUB);
	SetScalerValue(HPOS, g_SubScalerHPos, SUB);
	SetScalerValue(VPOS, g_SubScalerVPos, SUB);
	g_SubInput=g_MainInput;
	
#endif	
	Save(MODE1);
}

void SetMainScalerValue()
{
	g_H_VFlag=__TRUE;
	SetCurrentStateValue(KEY_UP, State_ScalerPara_Size_Value, g_MainScalerHSize);
	g_H_VFlag=__FALSE;
	SetCurrentStateValue(KEY_UP, State_ScalerPara_Size_Value, g_MainScalerVSize);
	if(g_MainScalerVPos!=0)
	{
		SetCurrentStateValue(KEY_UP, State_ScalerPara_Pos_Value, g_MainScalerVPos);
	}
	if(g_MainScalerHPos!=0)
	{
		g_H_VFlag=__TRUE;
		SetCurrentStateValue(KEY_UP, State_ScalerPara_Pos_Value, g_MainScalerHPos);
	}
}

void SetSubScalerValue()
{
	U8 value;
	value=g_ActiveImageValue;
	g_ActiveImageValue=SUB;
	g_H_VFlag=__TRUE;
	SetCurrentStateValue(KEY_UP, State_ScalerPara_Size_Value, g_SubScalerHSize);
	g_H_VFlag=__FALSE;
	SetCurrentStateValue(KEY_UP, State_ScalerPara_Size_Value, g_SubScalerVSize);
	if(g_SubScalerVPos!=0)
	{
		SetCurrentStateValue(KEY_UP, State_ScalerPara_Pos_Value, g_SubScalerVPos);
	}
	if(g_SubScalerHPos!=0)
	{
		g_H_VFlag=__TRUE;
		SetCurrentStateValue(KEY_UP, State_ScalerPara_Pos_Value, g_SubScalerHPos);
	}
	g_ActiveImageValue=value;

}

void KeepScreenPara()
{
	if((g_ScreenHPos+g_ScreenHSize)<g_PanelWidth&&(g_ScreenVPos+g_ScreenVSize)<g_PanelHeight)
	{
		PanelToZoomPara();
		SetMainScalerValue();
		if(g_MutiWindow!=SINGLEIMAGE)
			SetSubScalerValue();
	}
	else
		PanelToScreenPara();
}

BOOL SetCurrentStateValue(EventValue Val,State currentState,U16 wValue)
{
	if(Val==KEY_ENTER)
	{
		if(stateData[g_CurrentState].GetCurrentStateValue!=NULL)
		{
			if(currentState==stateData[g_CurrentState].GetCurrentStateValue())
				return __FALSE;
		}	
		switch(currentState)
		{
		case State_UserPara_SetCurrentTime:
			//g_SetTimeFlag=Set_Current_Time_Idx;
			//SetCurrentTime();
			//g_InputOldSeriesForWhat=5;//����߼�������û����룬���õ�ǰʱ��
			//DealCurrentState(State_User_Old_SerialNumber);
			SetCurrentTimeWithPassWordCheck();
			break;
		case State_Audio_Main:
			g_ActiveAudioWin=MAIN;
			SetAudioPara(MAINVOL);
			break;
		case State_Audio_Sub:
			g_ActiveAudioWin=SUB;
			SetAudioPara(SUBVOL);
			break;
	#if(XP_TO_ID==XP_TO_FLI32626)
		case State_UserPara_SetFadeTime_500:
			g_FadeTime=1;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;
		case State_UserPara_SetFadeTime_1000:
			g_FadeTime=2;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;
		case State_UserPara_SetFadeTime_1500:
			g_FadeTime=3;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;

		case State_ColorPara_Reset_Check:
			initColorVariable();
			SendColorValue(BRIGHTNESS_EVENT,0,g_Brightness_Value);
			SendColorValue(CONTRAST_EVENT,0,g_Contrast_Value);
			SendColorValue(SATURATION_EVENT,0,g_Saturation_Value);
			SendColorValue(COLORRGB,7,0);
			Save(MODE1);
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_ResetSuccessStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, g_ResetSuccessStrEn);
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			break;
		case State_ColorPara_ADC_Calibration:
			SendAdcCalibration(7,0,1,0);
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_waitStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, g_waitStrEn);
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			break;
	#endif
		case State_Language_English:
			g_LanguageFlag=ENGLISH;
			chinesedisplay(0x80, g_LanguageEn);
  			Write_EEPROM_Byte(E2ROM_LANGUAGE_BEGIN_ADDR,ENGLISH);
			break;
		case State_Language_Chinese:
			g_LanguageFlag=CHINESE;
			chinesedisplay(0x80, g_Language);
			Write_EEPROM_Byte(E2ROM_LANGUAGE_BEGIN_ADDR,CHINESE);
			break;
			
		case State_OutputHz_25Hz:
			g_OutputHzVal=25;
			SetOutputHzValue();
			Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;

		case State_OutputHz_30Hz:
			g_OutputHzVal=30;
			SetOutputHzValue();
			Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_OutputHz_50Hz:
			g_OutputHzVal=50;
			SetOutputHzValue();
			Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_OutputHz_60Hz:
			g_OutputHzVal=60;
			SetOutputHzValue();
			Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_CancelAll_Yes:
			CancelInputScheduleMode1();
			CancelInputScheduleMode2();
			CancelInputScheduleMode3();
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_CancelAllScheduleSuccessStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, "Cancel All success");
			break;

		case State_MultiWindow_Single:
			SetSingleImage();
			break;
		case State_MultiWindow_PBP:		
			SetPBPImage();
			break;
		case State_MultiWindow_PIP:
			SetPIPImage();
			break;
		case State_OutputFormat_640x480:
			g_OutputFormatIndex=INDEX640X480;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=640;
			g_PanelHeight=480;
			//PanelToScreenPara();
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else		
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_800x600:
			g_OutputFormatIndex=INDEX800X600;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=800;
			g_PanelHeight=600;
			//PanelToScreenPara();
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else	
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_1024x768:
			g_OutputFormatIndex=INDEX1024X768;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1024;
			g_PanelHeight=768;
			//PanelToScreenPara();			
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else		
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_1280x720:
			g_OutputFormatIndex=INDEX1280X720;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=720;
			KeepScreenPara();
			Save(MODE1);
			break;
		case State_OutputFormat_1280x768:
			g_OutputFormatIndex=INDEX1280X768;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=768;
			KeepScreenPara();
			Save(MODE1);
			break;
		case State_OutputFormat_1280x800:
			g_OutputFormatIndex=INDEX1280X800;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=800;
			KeepScreenPara();
			Save(MODE1);
			break;
		case State_OutputFormat_1280x960:
			g_OutputFormatIndex=INDEX1280X960;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=960;
			KeepScreenPara();
			Save(MODE1);
			break;
			
		case State_OutputFormat_1280x1024:
			g_OutputFormatIndex=INDEX1280X1024;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=1024;
			//PanelToScreenPara();
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else		
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_1366x768:
			g_OutputFormatIndex=INDEX1366X768;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1366;
			g_PanelHeight=768;
			//PanelToScreenPara();
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else		
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_1400x1050:
			g_OutputFormatIndex=INDEX1400X1050;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=1050;
			KeepScreenPara();
			Save(MODE1);
			break;
		case State_OutputFormat_1440x900:
			g_OutputFormatIndex=INDEX1440X900;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1440;
			g_PanelHeight=900;
			//PanelToScreenPara();
			KeepScreenPara();
		#if(XP_TO_ID==XP_TO_FLI32626)
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
		#else		
			Save(MODE1);
		#endif
			break;
		case State_OutputFormat_1600x900:
			g_OutputFormatIndex=INDEX1600X900;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1600;
			g_PanelHeight=900;
			KeepScreenPara();
			Save(MODE1);
			break;
		case State_OutputFormat_1600x1200:
			g_OutputFormatIndex=INDEX1600X1200;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1600;
			g_PanelHeight=1200;
			//PanelToScreenPara();
			KeepScreenPara();
	
			Save(MODE1);

			break;
		case State_OutputFormat_1680x1050:
			g_OutputFormatIndex=INDEX1680X1050;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1680;
			g_PanelHeight=1050;
			//PanelToScreenPara();
			KeepScreenPara();
	
			Save(MODE1);

			break;
		case State_OutputFormat_1920x1080:
			g_OutputFormatIndex=INDEX1920X1080;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1920;
			g_PanelHeight=1080;
			//PanelToScreenPara();
			KeepScreenPara();
	
			Save(MODE1);

			break;	
	#if(XP_TO_ID==XP_TO_FLI32626)
		case State_OutputFormat_1280x720:
			g_OutputFormatIndex=INDEX1280X720;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1280;
			g_PanelHeight=720;
			PanelToScreenPara();
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
			break;
		case State_OutputFormat_1920x1200:
			g_OutputFormatIndex=INDEX1920X1200;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=1920;
			g_PanelHeight=1200;
			PanelToScreenPara();
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
			break;
		case State_OutputFormat_2048x1152:
			g_OutputFormatIndex=INDEX2048x1152;
			SetOutputFormat(g_OutputFormatIndex);
			g_PanelWidth=2048;
			g_PanelHeight=1152;
			PanelToScreenPara();
			Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			ScreenParaToEEPROM();
			break;
	#endif
		case State_SavePara_Mode_check:
			Save(g_SaveModeValue);
			g_CurrentState=State_MainMenu_Not_Menu;
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x83, g_SaveOKStr);	
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x83, g_SaveOKStrEn);	
			delay(5);
			ReturnMainWin();
			break;		
		case State_FactoryReset_OK:
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x90, g_waitStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_waitStrEn);
			if(ResetPara())
			{
				clearlcd();
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x90, g_ResetSuccessStr);
				else  if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x90, "Reset Success!");
				delay(5);
			}
			LightInputSrcLED();
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			//ReturnMainWin();
			break;
		
			//-------------------------------------------------------------------------------------------
				
			case State_ScreenPara_Size_Value:
				if(g_H_VFlag==__TRUE)
				{
					SetScreenValue(HSIZE,g_ScreenHSize);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
						SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
					}
					else
					{
						SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
						SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
						SetScalerValue(HSIZE,g_SubScalerHSize,SUB);
						SetScalerValue(HPOS,g_SubScalerHPos,SUB);
					}
				}
				else
				{
					SetScreenValue(VSIZE,g_ScreenVSize);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
						SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
					}
					else
					{
						SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
						SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
						SetScalerValue(VSIZE,g_SubScalerVSize,SUB);
						SetScalerValue(VPOS,g_SubScalerVPos,SUB);
					}				
				}
				Save(MODE1);
				break;
			case State_ScreenPara_Pos_Value:
				if(g_H_VFlag==__TRUE)
				{	
					SetScreenValue(HPOS,g_ScreenHPos);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
					}
					else
					{
						SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
						SetScalerValue(HPOS,g_SubScalerHPos,SUB);
					}
				
				}
				else
				{				
					SetScreenValue(VPOS,g_ScreenVPos);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
					}
					else
					{
						SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
						SetScalerValue(VPOS,g_SubScalerVPos,SUB);
					}				
				}
				Save(MODE1);
				break;
		
			case State_ScalerPara_Size_Value:
			
				if(g_H_VFlag==__TRUE)
				{						
					if(g_MutiWindow==SINGLEIMAGE||g_ActiveImageValue==MAIN)
					{
						SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
					}
					else
					{
						SetScalerValue(HSIZE,g_SubScalerHSize,SUB);
					}
				}
				else
				{						
					if(g_MutiWindow==SINGLEIMAGE||g_ActiveImageValue==MAIN)
					{
						SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
					}
					else
					{
						SetScalerValue(VSIZE,g_SubScalerVSize,SUB);
					}
				}
				Save(MODE1);
				break;
			case State_ScalerPara_Pos_Value:
			
				if(g_H_VFlag==__TRUE)
				{					
					if(g_MutiWindow==SINGLEIMAGE||g_ActiveImageValue==MAIN)
					{
						SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
					}
					else
					{
						SetScalerValue(HPOS,g_SubScalerHPos,SUB);
					}
				}
				else
				{					
					if(g_MutiWindow==SINGLEIMAGE||g_ActiveImageValue==MAIN)
					{
						SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
					}
					else
					{
						SetScalerValue(VPOS,g_SubScalerVPos,SUB);
					}
				}			
				Save(MODE1);
				break;
			case State_ZoomPara_Size_Value:
			{
				U8 image;          //��ǰͨ��Ϊ��������
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(HSIZE,g_MainVGAZoomHSize,MAIN);
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(HSIZE,g_MainDVIZoomHSize,MAIN);
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(HSIZE,g_MainHDMIZoomHSize,MAIN);
						}
					#endif
						else
						{
							SetZoomValue(HSIZE,g_MainZoomHSize,MAIN);
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(HSIZE,g_SubVGAZoomHSize,SUB);
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(HSIZE,g_SubDVIZoomHSize,SUB);
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(HSIZE,g_SubHDMIZoomHSize,SUB);
						}
					#endif
						else
						{
							SetZoomValue(HSIZE,g_SubZoomHSize,SUB);
						}
					}
				}
				else
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(VSIZE,g_MainVGAZoomVSize,MAIN);
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(VSIZE,g_MainDVIZoomVSize,MAIN);
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(VSIZE,g_MainHDMIZoomVSize,MAIN);
						}
					#endif
						else
						{
							SetZoomValue(VSIZE,g_MainZoomVSize,MAIN);
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(VSIZE,g_SubVGAZoomVSize,SUB);
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(VSIZE,g_SubDVIZoomVSize,SUB);
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(VSIZE,g_SubHDMIZoomVSize,SUB);
						}
					#endif
						else
						{
							SetZoomValue(VSIZE,g_SubZoomVSize,SUB);
						}
					}
				}
			}
			Save(MODE1);
			break;
			case State_ZoomPara_Pos_Value:
			{
				U8 image;//��ǰͨ��Ϊ��������
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(HPOS,g_MainVGAZoomHPos,MAIN);
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(HPOS,g_MainDVIZoomHPos,MAIN);
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(HPOS,g_MainHDMIZoomHPos,MAIN);
						}
					#endif
						else
						{
							SetZoomValue(HPOS,g_MainZoomHPos,MAIN);
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(HPOS,g_SubVGAZoomHPos,SUB);
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(HPOS,g_SubDVIZoomHPos,SUB);
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(HPOS,g_SubHDMIZoomHPos,SUB);
						}
					#endif
						else
						{
							SetZoomValue(HPOS,g_SubZoomHPos,SUB);
						}
					}
				}
				else
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(VPOS,g_MainVGAZoomVPos,MAIN);
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(VPOS,g_MainDVIZoomVPos,MAIN);
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(VPOS,g_MainHDMIZoomVPos,MAIN);
						}
					#endif
						else
						{
							SetZoomValue(VPOS,g_MainZoomVPos,MAIN);
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							SetZoomValue(VPOS,g_SubVGAZoomVPos,SUB);
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							SetZoomValue(VPOS,g_SubDVIZoomVPos,SUB);
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							SetZoomValue(VPOS,g_SubHDMIZoomVPos,SUB);
						}
					#endif
						else
						{
							SetZoomValue(VPOS,g_SubZoomVPos,SUB);
						}
					}
				}
			}
			Save(MODE1);
			break;
		default:
			return __FALSE;
		}
	}
	else if((Val==KEY_UP)||(Val==KEY_DOWN))
	{
		switch(currentState)
		{
		#if(XP_TO_ID==XP_TO_FLI32626)		
			case State_Fuse_Switch_Value:
				g_AlphaValue=wValue;
				SetMultiWindowPara(FUSEPIPPARA1);
				Write_EEPROM_Byte(E2ROM_Alpha_ADDR, g_AlphaValue);
				break;
		#endif
			
		#ifdef TCP_IP
			case State_UserPara_SetStaticIP_Value:
				SetStaticIPAddress(wValue);
				break;
		#endif
		#ifdef SUPPORT_ANYHZ
			case State_OutputHz_25Hz:
				//g_OutputHzVal=wValue;
				g_OutputHzVal=25;
				SetOutputHzValue();
				Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
				break;
		#endif
			case State_Volume_Set:
				g_VolumeValue=wValue;
				SetVolumeValue(wValue);
				break;
			//----------------------------set up date and time------------------------------------------
			case State_Input_YEAR:
				TimeSet[0]=wValue;
				g_CurrentState=State_Input_MONTH;
				break;
			case State_Input_MONTH:
				TimeSet[1]=wValue;
				g_CurrentState=State_Input_DOM;
				break;
			case State_Input_DOM:
				TimeSet[2]=wValue;
				if(g_SetTimeFlag==Set_Try_Use_Idx)
				{
					g_TryEndDate[0]=TimeSet[0];//���ý�ֹ��
					g_TryEndDate[1]=TimeSet[1];//���ý�ֹ��
					g_TryEndDate[2]=TimeSet[2];//���ý�ֹ��
					Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);
					LcdDisplaySetOK_ReturnMain();
					ReturnMainWin();
				}
				else
					g_CurrentState=State_Input_HOUR;
				break;
			case State_Input_HOUR:
				wValue%=24;
				TimeSet[3]=wValue;
				g_CurrentState=State_Input_MIN;
				break;
			case State_Input_MIN:
				wValue%=60;
				TimeSet[4]=wValue;
				
				if(g_SetTimeFlag==Set_Current_Time_Idx)	
				{
					g_CurrentState=State_Input_SEC;
				}
				else if(g_SetTimeFlag==Set_Input_Schedule_Idx||g_SetTimeFlag==Set_Mode_Schedule_Idx
					||g_SetTimeFlag==Set_InputCycle_Idx)
				{
					InputNumReset();
				}
				
				break;
			case State_Input_SEC:
				wValue%=60;
				TimeSet[5]=wValue;	
				STM32_Time_Adjust();
				InputNumReset();
				delay(5);
				ReturnMainWin();
				break;

			//-------------------------------------------------------------------------------------------
				
			case State_ScreenPara_Size_Value:
				if(g_H_VFlag==__TRUE)
				{	
					if(wValue+g_ScreenHPos>g_PanelWidth)
						wValue=g_PanelWidth-g_ScreenHPos;
					g_ScreenHSize=wValue;
				
					if(g_MainScalerHPos!=g_ScreenHPos)
					{
						g_MainScalerHPos=g_ScreenHPos;
						//SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
					}			
					//SetScreenValue(HSIZE,wValue);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerHSize=wValue;
						//SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerHSize=wValue;
						//SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
						g_SubScalerHSize=wValue/3;
						//SetScalerValue(HSIZE,g_SubScalerHSize,SUB);
						if(g_SubScalerHPos+g_SubScalerHSize>g_ScreenHPos+g_ScreenHSize)
						{
							g_SubScalerHPos=g_ScreenHPos+g_ScreenHSize-g_SubScalerHSize;
							//SetScalerValue(HPOS,g_SubScalerHPos,SUB);
						}
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerHSize=wValue/2;
						//SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
						if(g_SubScalerHPos!=g_MainScalerHPos+g_MainScalerHSize)
						{
							g_SubScalerHPos=g_MainScalerHPos+g_MainScalerHSize;
							//SetScalerValue(HPOS,g_SubScalerHPos,SUB);
						}
						g_SubScalerHSize=wValue/2;
						//SetScalerValue(HSIZE,g_SubScalerHSize,SUB);
					}
				
				}
				else
				{
					if((wValue+g_ScreenVPos)>g_PanelHeight)
						wValue=g_PanelHeight-g_ScreenVPos;
					g_ScreenVSize=wValue;
				
					if(g_MainScalerVPos!=g_ScreenVPos)
					{
						g_MainScalerVPos=g_ScreenVPos;
						//SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
					}
					//SetScreenValue(VSIZE,wValue);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerVSize=wValue;
						//SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerVSize=wValue;	
						//SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
						g_SubScalerVSize=wValue/3;
						//SetScalerValue(VSIZE,g_SubScalerVSize,SUB);
						if(g_SubScalerVPos+g_SubScalerVSize>g_ScreenVPos+g_ScreenVSize)
						{
							g_SubScalerVPos=g_ScreenVPos+g_ScreenVSize-g_SubScalerVSize;
							//SetScalerValue(VPOS,g_SubScalerVPos,SUB);
						}
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerVSize=wValue;
						//SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
						g_SubScalerVSize=wValue;
						//SetScalerValue(VSIZE,g_SubScalerVSize,SUB);
						if(g_SubScalerVPos!=g_ScreenVPos)
						{
							g_SubScalerVPos=g_ScreenVPos;
							//SetScalerValue(VPOS,g_SubScalerVPos,SUB);
						}
					}
				
				}
				//Save(MODE1);
				break;
			case State_ScreenPara_Pos_Value:
				if(g_H_VFlag==__TRUE)
				{			
					if((wValue+g_ScreenHSize)>g_PanelWidth)
						wValue=(g_PanelWidth-g_ScreenHSize);
					g_ScreenHPos=wValue;
				
					//SetScreenValue(HPOS,wValue);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
						if(g_SubScalerHPos<wValue)
						{
							g_SubScalerHPos=wValue;
							//SetScalerValue(HPOS,g_SubScalerHPos,SUB);
						}
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
						g_SubScalerHPos=g_MainScalerHPos+g_MainScalerHSize;
						//SetScalerValue(HPOS,g_SubScalerHPos,SUB);
					}
				
				}
				else
				{
					if((wValue+g_ScreenVSize)>g_PanelHeight)
						wValue=(g_PanelHeight-g_ScreenVSize);
					g_ScreenVPos=wValue;
				
					//SetScreenValue(VPOS,g_ScreenVPos);
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
						if(g_SubScalerVPos<wValue)
						{
							g_SubScalerVPos=wValue;
							//SetScalerValue(VPOS,g_SubScalerVPos,SUB);
						}
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
						g_SubScalerVPos=wValue;
						//SetScalerValue(VPOS,g_SubScalerVPos,SUB);
					}
				
				}
				//Save(MODE1);
				break;
		
			case State_ScalerPara_Size_Value:
			
				if(g_H_VFlag==__TRUE)
				{			
					if((g_ActiveImageValue==MAIN)&&((wValue+g_MainScalerHPos)>(g_ScreenHPos+g_ScreenHSize)))
						wValue=(g_ScreenHPos+g_ScreenHSize)-g_MainScalerHPos;
					else if((g_ActiveImageValue==SUB)&&((wValue+g_SubScalerHPos)>(g_ScreenHPos+g_ScreenHSize)))
						wValue=(g_ScreenHPos+g_ScreenHSize)-g_SubScalerHPos;
					if(g_ActiveImageValue==MAIN)
						g_MainScalerHSize=wValue;
					else if(g_ActiveImageValue==SUB)
						g_SubScalerHSize=wValue;
					//SetScalerValue(HSIZE,wValue,g_ActiveImageValue);
				}
				else
				{	
					if((g_ActiveImageValue==MAIN)&&((wValue+g_MainScalerVPos)>(g_ScreenVPos+g_ScreenVSize)))
						wValue=(g_ScreenVPos+g_ScreenVSize)-g_MainScalerVPos;
					else if((g_ActiveImageValue==SUB)&&((wValue+g_SubScalerVPos)>(g_ScreenVPos+g_ScreenVSize)))
						wValue=(g_ScreenVPos+g_ScreenVSize)-g_SubScalerVPos;
					if(g_ActiveImageValue==MAIN)
						g_MainScalerVSize=wValue;
					else if(g_ActiveImageValue==SUB)
						g_SubScalerVSize=wValue;
					//SetScalerValue(VSIZE,wValue,g_ActiveImageValue);
				}
			//Save(MODE1);
				break;
			case State_DualImage_Set_MainSize:		
				if(g_H_VFlag==__TRUE)
				{
					g_MainScalerHSize=wValue;
				}
				else
				{
					g_MainScalerVSize=wValue;
				}
				break;
			case State_DualImage_Set_SubSize:	
				if(g_H_VFlag==__TRUE)
				{
					g_SubScalerHSize=wValue;
				}
				else
				{
					g_SubScalerVSize=wValue;
					g_ScreenHSize=g_MainScalerHSize+g_SubScalerHSize;
					g_ScreenVSize=(g_MainScalerVSize>g_SubScalerVSize)?g_MainScalerVSize:g_SubScalerVSize;

					g_MutiWindow=PBPIMAGE;
					if(ParseAndSetMode())
					{
						g_CurrentState=State_MainMenu_Not_Menu;
						clearlcd();		
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_SetOKStr);
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_SetOKStrEn);
						delay(5);
						clearlcd();
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90,  g_CurrentTimeStr);
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90,  g_CurrentTimeStrEn);
						g_CurrentState=State_MainMenu_Menu;
						LightInputSrcLED();	
						g_ActiveImageValue=MAIN;
					}
					else	
					{
						AdviceForUser();
						delay(10);
						WizardSet();
					}
				}
			
				break;
			case State_ScalerPara_Pos_Value:
			
				if(g_H_VFlag==__TRUE)
				{					
					if((g_ActiveImageValue==MAIN)&&((wValue+g_MainScalerHSize)>(g_ScreenHPos+g_ScreenHSize)))
						wValue=(g_ScreenHPos+g_ScreenHSize)-g_MainScalerHSize;
					else if((g_ActiveImageValue==SUB)&&((wValue+g_SubScalerHSize)>(g_ScreenHPos+g_ScreenHSize)))
						wValue=(g_ScreenHPos+g_ScreenHSize)-g_SubScalerHSize;
					if(g_ActiveImageValue==MAIN)
						g_MainScalerHPos=wValue;
					else if(g_ActiveImageValue==SUB)
						g_SubScalerHPos=wValue;
					//SetScalerValue(HPOS,wValue,g_ActiveImageValue);
				}
				else
				{
					if((g_ActiveImageValue==MAIN)&&((wValue+g_MainScalerVSize)>(g_ScreenVPos+g_ScreenVSize)))
						wValue=(g_ScreenVPos+g_ScreenVSize)-g_MainScalerVSize;
					else if((g_ActiveImageValue==SUB)&&((wValue+g_SubScalerVSize)>(g_ScreenVPos+g_ScreenVSize)))
						wValue=(g_ScreenVPos+g_ScreenVSize)-g_SubScalerVSize;
					if(g_ActiveImageValue==MAIN)
						g_MainScalerVPos=wValue;
					else if(g_ActiveImageValue==SUB)
						g_SubScalerVPos=wValue;
					//SetScalerValue(VPOS,wValue,g_ActiveImageValue);
				}
			
			//Save(MODE1);
				break;
			case State_ZoomPara_Size_Value:
			{
				U8 image;          //��ǰͨ��Ϊ��������
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(wValue>g_InSrcHSize)
						wValue=g_InSrcHSize;
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							g_MainVGAZoomHSize=wValue;
							if(g_MainVGAZoomHPos>g_InSrcHSize-g_MainVGAZoomHSize)
								g_MainVGAZoomHPos=g_InSrcHSize-g_MainVGAZoomHSize;
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							g_MainDVIZoomHSize=wValue;
							if(g_MainDVIZoomHPos>g_InSrcHSize-g_MainDVIZoomHSize)
								g_MainDVIZoomHPos=g_InSrcHSize-g_MainDVIZoomHSize;
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							g_MainHDMIZoomHSize=wValue;
							if(g_MainHDMIZoomHPos>g_InSrcHSize-g_MainHDMIZoomHSize)
								g_MainHDMIZoomHPos=g_InSrcHSize-g_MainHDMIZoomHSize;
						}
					#endif
						else
						{
							g_MainZoomHSize=wValue;
							if(g_MainZoomHPos>g_InSrcHSize-g_MainZoomHSize)
								g_MainZoomHPos=g_InSrcHSize-g_MainZoomHSize;
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							g_SubVGAZoomHSize=wValue;
							if(g_SubVGAZoomHPos>g_InSrcHSize-g_SubVGAZoomHSize)
								g_SubVGAZoomHPos=g_InSrcHSize-g_SubVGAZoomHSize;
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							g_SubDVIZoomHSize=wValue;
							if(g_SubDVIZoomHPos>g_InSrcHSize-g_SubDVIZoomHSize)
								g_SubDVIZoomHPos=g_InSrcHSize-g_SubDVIZoomHSize;
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							g_SubHDMIZoomHSize=wValue;
							if(g_SubHDMIZoomHPos>g_InSrcHSize-g_SubHDMIZoomHSize)
								g_SubHDMIZoomHPos=g_InSrcHSize-g_SubHDMIZoomHSize;
						}
					#endif
						else
						{
							g_SubZoomHSize=wValue;
							if(g_SubZoomHPos>g_InSrcHSize-g_SubZoomHSize)
								g_SubZoomHPos=g_InSrcHSize-g_SubZoomHSize;
						}
					}
					//SetZoomValue(HSIZE,wValue,image);
				}
				else
				{
					if(wValue>g_InSrcVSize)
						wValue=g_InSrcVSize;
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							g_MainVGAZoomVSize=wValue;
							if(g_MainVGAZoomVPos>g_InSrcVSize-g_MainVGAZoomVSize)
								g_MainVGAZoomVPos=g_InSrcVSize-g_MainVGAZoomVSize;
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							g_MainDVIZoomVSize=wValue;
							if(g_MainDVIZoomVPos>g_InSrcVSize-g_MainDVIZoomVSize)
								g_MainDVIZoomVPos=g_InSrcVSize-g_MainDVIZoomVSize;
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							g_MainHDMIZoomVSize=wValue;
							if(g_MainHDMIZoomVPos>g_InSrcVSize-g_MainHDMIZoomVSize)
								g_MainHDMIZoomVPos=g_InSrcVSize-g_MainHDMIZoomVSize;
						}
					#endif
						else
						{
							g_MainZoomVSize=wValue;
							if(g_MainZoomVPos>g_InSrcVSize-g_MainZoomVSize)
								g_MainZoomVPos=g_InSrcVSize-g_MainZoomVSize;
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							g_SubVGAZoomVSize=wValue;
							if(g_SubVGAZoomVPos>g_InSrcVSize-g_SubVGAZoomVSize)
								g_SubVGAZoomVPos=g_InSrcVSize-g_SubVGAZoomVSize;
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							g_SubDVIZoomVSize=wValue;
							if(g_SubDVIZoomVPos>g_InSrcVSize-g_SubDVIZoomVSize)
								g_SubDVIZoomVPos=g_InSrcVSize-g_SubDVIZoomVSize;
						}	 
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							g_SubHDMIZoomVSize=wValue;
							if(g_SubHDMIZoomVPos>g_InSrcVSize-g_SubHDMIZoomVSize)
								g_SubHDMIZoomVPos=g_InSrcVSize-g_SubHDMIZoomVSize;
						}
					#endif
						else
						{
							g_SubZoomVSize=wValue;
							if(g_SubZoomVPos>g_InSrcVSize-g_SubZoomVSize)
								g_SubZoomVPos=g_InSrcVSize-g_SubZoomVSize;
						}
					}
					//SetZoomValue(VSIZE,wValue,image);
				}
			}
			//Save(MODE1);
			break;
			case State_ZoomPara_Pos_Value:
			{
				U8 image;//��ǰͨ��Ϊ��������
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							if(wValue>g_InSrcHSize-g_MainVGAZoomHSize)
								wValue=g_InSrcHSize-g_MainVGAZoomHSize;
							g_MainVGAZoomHPos=wValue;
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							if(wValue>g_InSrcHSize-g_MainDVIZoomHSize)
								wValue=g_InSrcHSize-g_MainDVIZoomHSize;
							g_MainDVIZoomHPos=wValue;
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							if(wValue>g_InSrcHSize-g_MainHDMIZoomHSize)
								wValue=g_InSrcHSize-g_MainHDMIZoomHSize;
							g_MainHDMIZoomHPos=wValue;
						}
					#endif
						else
						{
							if(wValue>g_InSrcHSize-g_MainZoomHSize)
								wValue=g_InSrcHSize-g_MainZoomHSize;
							g_MainZoomHPos=wValue;
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							if(wValue>g_InSrcHSize-g_SubVGAZoomHSize)
								wValue=g_InSrcHSize-g_SubVGAZoomHSize;
							g_SubVGAZoomHPos=wValue;
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							if(wValue>g_InSrcHSize-g_SubDVIZoomHSize)
								wValue=g_InSrcHSize-g_SubDVIZoomHSize;
							g_SubDVIZoomHPos=wValue;
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							if(wValue>g_InSrcHSize-g_SubHDMIZoomHSize)
								wValue=g_InSrcHSize-g_SubHDMIZoomHSize;
							g_SubHDMIZoomHPos=wValue;
						}
					#endif
						else
						{
							if(wValue>g_InSrcHSize-g_SubZoomHSize)
								wValue=g_InSrcHSize-g_SubZoomHSize;
							g_SubZoomHPos=wValue;
						}
					}
					//SetZoomValue(HPOS,wValue,image);
				}
				else
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							if(wValue>g_InSrcVSize-g_MainVGAZoomVSize)
								wValue=g_InSrcVSize-g_MainVGAZoomVSize;
							g_MainVGAZoomVPos=wValue;
						}
						else if(g_MainInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							if(wValue>g_InSrcVSize-g_MainDVIZoomVSize)
								wValue=g_InSrcVSize-g_MainDVIZoomVSize;
							g_MainDVIZoomVPos=wValue;
						}
					#if(ENABLE_HDMI)
						else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						{
							if(wValue>g_InSrcVSize-g_MainHDMIZoomVSize)
								wValue=g_InSrcVSize-g_MainHDMIZoomVSize;
							g_MainHDMIZoomVPos=wValue;
						}
					#endif
						else
						{
							if(wValue>g_InSrcVSize-g_MainZoomVSize)
								wValue=g_InSrcVSize-g_MainZoomVSize;
							g_MainZoomVPos=wValue;
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA||g_MainInput==INPUTSOURCE_VGA2)
						{
							if(wValue>g_InSrcVSize-g_SubVGAZoomVSize)
								wValue=g_InSrcVSize-g_SubVGAZoomVSize;
							g_SubVGAZoomVPos=wValue;
						}
						else if(g_SubInput==INPUTSOURCE_DVI||g_MainInput==INPUTSOURCE_DVI2)
						{
							if(wValue>g_InSrcVSize-g_SubDVIZoomVSize)
								wValue=g_InSrcVSize-g_SubDVIZoomVSize;
							g_SubDVIZoomVPos=wValue;
						}
					#if(ENABLE_HDMI)
						else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						{
							if(wValue>g_InSrcVSize-g_SubHDMIZoomVSize)
								wValue=g_InSrcVSize-g_SubHDMIZoomVSize;
							g_SubHDMIZoomVPos=wValue;
						}
					#endif
						else
						{
							if(wValue>g_InSrcVSize-g_SubZoomVSize)
								wValue=g_InSrcVSize-g_SubZoomVSize;
							g_SubZoomVPos=wValue;
						}
					}
					//SetZoomValue(VPOS,wValue,image);
				}
			}
			//Save(MODE1);
			break;
			case State_ColorPara_Brightness_Value:
				#if(STM32_CONTROL_MCU==MSTAR_6M16JS||XP_TO_ID==XP_TO_FLI32626)
					if(wValue>100)
						wValue=100;
					g_Brightness_Value=wValue;
					#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
					SetColorValue(BRIGHTNESS, wValue, MAIN);
					if(g_MutiWindow!=SINGLEIMAGE)
					{
						SetColorValue(BRIGHTNESS, wValue, SUB);
					}
					#elif(XP_TO_ID==XP_TO_FLI32626)
					SendColorValue(BRIGHTNESS_EVENT,0,wValue);
					#endif
				#else
					if(wValue>0xff)
						wValue=0xff;
					SetColorValue(BRIGHTNESSR, wValue, MAIN);
					SetColorValue(BRIGHTNESSG, wValue, MAIN);
					SetColorValue(BRIGHTNESSB, wValue, MAIN);
					if(g_MutiWindow!=SINGLEIMAGE)
					{
						SetColorValue(BRIGHTNESSR, wValue, SUB);
						SetColorValue(BRIGHTNESSG, wValue, SUB);
						SetColorValue(BRIGHTNESSB, wValue, SUB);
					}
					g_BrightnessR_Value[g_ActiveImageValue-1]=wValue;
					g_BrightnessG_Value[g_ActiveImageValue-1]=wValue;
					g_BrightnessB_Value[g_ActiveImageValue-1]=wValue;					
				#endif
				Save(MODE1);
				break;
			case State_ColorPara_Contrast_Value:
				#if(STM32_CONTROL_MCU==MSTAR_6M16JS||XP_TO_ID==XP_TO_FLI32626)
					if(wValue>100)
						wValue=100;
					g_Contrast_Value=wValue;
					#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
					SetColorValue(CONTRAST, wValue, MAIN);
					if(g_MutiWindow!=SINGLEIMAGE)
					{
						SetColorValue(CONTRAST, wValue, SUB);
					}
					#elif(XP_TO_ID==XP_TO_FLI32626)
					SendColorValue(CONTRAST_EVENT,0,wValue);
					#endif
				#else
					if(wValue>0xff)
						wValue=0xff;
					SetColorValue(CONTRASTR, wValue, MAIN);
					SetColorValue(CONTRASTG, wValue, MAIN);
					SetColorValue(CONTRASTB, wValue, MAIN);
					if(g_MutiWindow!=SINGLEIMAGE)
					{
						SetColorValue(CONTRASTR, wValue, SUB);
						SetColorValue(CONTRASTG, wValue, SUB);
						SetColorValue(CONTRASTB, wValue, SUB);
					}
					g_ContrastR_Value[g_ActiveImageValue-1]=wValue;
					g_ContrastG_Value[g_ActiveImageValue-1]=wValue;
					g_ContrastB_Value[g_ActiveImageValue-1]=wValue;
				#endif
				Save(MODE1);
				break;
		
		#ifdef RGB_COLOR
			case State_ColorPara_Brightness_R_Value:
				#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
					if(wValue>20)
						wValue=20;
				#else
					if(wValue>0xff)
						wValue=0xff;
				#endif
				SetColorValue(BRIGHTNESSR, wValue, g_ActiveImageValue);
				g_BrightnessR_Value[g_ActiveImageValue-1]=wValue;
				break;
			case State_ColorPara_Brightness_G_Value:
				#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
					if(wValue>20)
						wValue=20;
				#else
					if(wValue>0xff)
						wValue=0xff;
				#endif
				SetColorValue(BRIGHTNESSG, wValue, g_ActiveImageValue);
				g_BrightnessG_Value[g_ActiveImageValue-1]=wValue;
				break;
			case State_ColorPara_Brightness_B_Value:
				#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
					if(wValue>20)
						wValue=20;
				#else
					if(wValue>0xff)
						wValue=0xff;
				#endif
				SetColorValue(BRIGHTNESSB, wValue, g_ActiveImageValue);
				g_BrightnessB_Value[g_ActiveImageValue-1]=wValue;
				break;
			case State_ColorPara_Contract_R_Value:
				if(wValue>0xff)
					wValue=0xff;
				SetColorValue(CONTRASTR, wValue, g_ActiveImageValue);
				g_ContrastR_Value[g_ActiveImageValue-1]=wValue;
				break;
			case State_ColorPara_Contract_G_Value:
				if(wValue>0xff)
					wValue=0xff;
				SetColorValue(CONTRASTG, wValue, g_ActiveImageValue);
				g_ContrastG_Value[g_ActiveImageValue-1]=wValue;
				break;
			case State_ColorPara_Contract_B_Value:
				if(wValue>0xff)
					wValue=0xff;
				SetColorValue(CONTRASTB, wValue, g_ActiveImageValue);
				g_ContrastB_Value[g_ActiveImageValue-1]=wValue;
				break;
		#endif
		
		#if(XP_TO_ID==XP_TO_FLI32626)
			case State_ColorPara_Saturation_Value:
				if(wValue>100)
					wValue=100;
				g_Saturation_Value=wValue;
				SendColorValue(SATURATION_EVENT,0,wValue);
				Save(MODE1);
				break;
		#endif
			default:
				return __FALSE;
		}
	}	
	return __TRUE;
}


//------------------------get current schedule mode-----------------------
State GetScheduleInputMode1()
{	
	State curState;
	if(g_ScheduleInputMode1==ONCESCHE)
		curState=State_ScheduleInput1_Once;
	else
		curState=State_ScheduleInput1_EveryDay;
	return curState;
}
State GetScheduleInputMode2()
{	
	State curState;
	if(g_ScheduleInputMode2==ONCESCHE)
		curState=State_ScheduleInput2_Once;
	else
		curState=State_ScheduleInput2_EveryDay;
	return curState;
}
State GetScheduleInputMode3()
{	
	State curState;
	if(g_ScheduleInputMode3==ONCESCHE)
		curState=State_ScheduleInput3_Once;
	else
		curState=State_ScheduleInput3_EveryDay;
	return curState;
}

State GetCurrentOutputHzState()
{
	State curState=State_OutputHz_60Hz;
	if(g_OutputHzVal==50)
		curState=State_OutputHz_50Hz;
	else if(g_OutputHzVal==60)
		curState=State_OutputHz_60Hz;
	else if(g_OutputHzVal==30)
		curState=State_OutputHz_30Hz;
	else if(g_OutputHzVal==25)
		curState=State_OutputHz_25Hz;
	return curState;
		
}

State GetOutFormatStateValue()
{
	State curState=State_OutputFormat_1024x768;
//	GetOutputFormat();
	switch(g_OutputFormatIndex)
	{
		case INDEX640X480:
			curState=State_OutputFormat_640x480;
			break;
		case INDEX800X600:
			curState=State_OutputFormat_800x600;
			break;
		case INDEX1024X768:
			curState=State_OutputFormat_1024x768;
			break;
		case INDEX1280X1024:
			curState=State_OutputFormat_1280x1024;
			break;
		case INDEX1366X768:
			curState=State_OutputFormat_1366x768;
			break;
		case INDEX1440X900:
			curState=State_OutputFormat_1440x900;
			break;
		case INDEX1600X900:
			curState=State_OutputFormat_1600x900;
			break;
		case INDEX1600X1200:
			curState=State_OutputFormat_1600x1200;
			break;
		case INDEX1680X1050:
			curState=State_OutputFormat_1680x1050;
			break;
		case INDEX1920X1080:
			curState=State_OutputFormat_1920x1080;
			break;
			
		case INDEX1280X720:
			curState=State_OutputFormat_1280x720;
			break;
		case INDEX1280X768:
			curState=State_OutputFormat_1280x768;
			break;
		case INDEX1280X800:
			curState=State_OutputFormat_1280x800;
			break;
		case INDEX1280X960:
			curState=State_OutputFormat_1280x960;
			break;
		case INDEX1400X1050:
			curState=State_OutputFormat_1400x1050;
			break;
	#if(XP_TO_ID==XP_TO_FLI32626)
		case INDEX1280X720:
			curState=State_OutputFormat_1280x720;
			break;
		case INDEX1920X1200:
			curState=State_OutputFormat_1920x1200;
			break;
		case INDEX2048x1152:
			curState=State_OutputFormat_2048x1152;
			break;
	#endif
	}
	return curState;
}

State GetFullOrPartStateValue()
{
	State curState=State_FullOrPart_Full;
	if(g_FullOrPartValue==PART)
		curState=State_FullOrPart_Part;
	else if(g_FullOrPartValue==FULL)
		curState=State_FullOrPart_Full;
	return curState;
	
}

State GetMultiWindowStateValue()
{	
	State curState=State_MultiWindow_Single;
//	g_MutiWindow=GetMultiWindowPara();
	if(g_MutiWindow==SINGLEIMAGE)
		curState=State_MultiWindow_Single;
	else if(g_MutiWindow==PIPIMAGE)
		curState=State_MultiWindow_PIP;
	else if(g_MutiWindow==PBPIMAGE)
		curState=State_MultiWindow_PBP;
#if(XP_TO_ID==XP_TO_FLI32626)
	else if(g_MutiWindow==SAMEPIPIMAGE)
		curState=State_MultiWindow_SAMEPIP;
#endif
	return curState;
}

State GetActiveImageStateValue()
{
	State curState=State_ActiveImage_Main;
	if(g_ActiveImageValue==MAIN)
		curState=State_ActiveImage_Main;
	else if(g_ActiveImageValue==SUB)
		curState=State_ActiveImage_Sub;
	return curState;	
}

State GetActiveAudioStateValue()
{
	State curState=State_Audio_Main;
	if(g_ActiveAudioWin==MAIN)
		curState=State_Audio_Main;
	else if(g_ActiveAudioWin==SUB)
		curState=State_Audio_Sub;
	return curState;
}

U16 GetCurrentVolumeValue()
{
	U16 wValue;
	wValue=g_VolumeValue;
	return wValue;	
}

U16 GetCurrentWindowValue()
{
	U16 wValue=0;
	switch(g_CurrentState)
	{
	#if(XP_TO_ID==XP_TO_FLI32626)
		case State_Fuse_Switch_Value:
			wValue=g_AlphaValue;
			break;
	#endif
		case State_ScreenPara_Size_Value:
			if(g_H_VFlag==__TRUE)
				wValue=g_ScreenHSize;
			else
				wValue=g_ScreenVSize;
			break;
		case State_ScreenPara_Pos_Value:
			if(g_H_VFlag==__TRUE)
				wValue=g_ScreenHPos;
			else
				wValue=g_ScreenVPos;
			break;
		case State_ScalerPara_Size_Value:
			if(g_H_VFlag==__TRUE)
			{
				if(g_ActiveImageValue==MAIN)
					wValue=g_MainScalerHSize;
				else if(g_ActiveImageValue==SUB)
					wValue=g_SubScalerHSize;
			}
			else
			{
				if(g_ActiveImageValue==MAIN)
					wValue=g_MainScalerVSize;
				else if(g_ActiveImageValue==SUB)
					wValue=g_SubScalerVSize;
			}
			break;
		case State_ScalerPara_Pos_Value:
			if(g_H_VFlag==__TRUE)
			{
				if(g_ActiveImageValue==MAIN)
					wValue=g_MainScalerHPos;
				else if(g_ActiveImageValue==SUB)
					wValue=g_SubScalerHPos;
			}
			else
			{
				if(g_ActiveImageValue==MAIN)
					wValue=g_MainScalerVPos;
				else if(g_ActiveImageValue==SUB)
					wValue=g_SubScalerVPos;
			}
			break;
		case State_ZoomPara_Size_Value:
		{
			U8 image;          //��ǰͨ��Ϊ��������
			GetWhichInput(&image);
			if(g_H_VFlag==__TRUE)
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomHSize;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomHSize;
				#if(ENABLE_HDMI)
					else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						wValue=g_MainHDMIZoomHSize;
				#endif
					else
						wValue=g_MainZoomHSize;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomHSize;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomHSize;
				#if(ENABLE_HDMI)
					else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						wValue=g_SubHDMIZoomHSize;
				#endif
					else
						wValue=g_SubZoomHSize;
				}
			}
			else
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomVSize;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomVSize;
				#if(ENABLE_HDMI)
					else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						wValue=g_MainHDMIZoomVSize;
				#endif
					else
						wValue=g_MainZoomVSize;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomVSize;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomVSize;
				#if(ENABLE_HDMI)
					else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						wValue=g_SubHDMIZoomVSize;
				#endif
					else
						wValue=g_SubZoomVSize;
				}
			}
		}
			break;
		case State_ZoomPara_Pos_Value:
		{
			U8 image;          //��ǰͨ��Ϊ��������
			GetWhichInput(&image);
			if(g_H_VFlag==__TRUE)
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomHPos;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomHPos;
				#if(ENABLE_HDMI)
					else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						wValue=g_MainHDMIZoomHPos;
				#endif
					else
						wValue=g_MainZoomHPos;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomHPos;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomHPos;
				#if(ENABLE_HDMI)
					else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						wValue=g_SubHDMIZoomHPos;
				#endif
					else
						wValue=g_SubZoomHPos;
				}
			}
			else
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomVPos;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomVPos;
				#if(ENABLE_HDMI)
					else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
						wValue=g_MainHDMIZoomVPos;
				#endif
					else
						wValue=g_MainZoomVPos;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomVPos;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomVPos;
				#if(ENABLE_HDMI)
					else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
						wValue=g_SubHDMIZoomVPos;
				#endif
					else
						wValue=g_SubZoomVPos;
				}
			}
		}
		break;
		case State_Volume_Set:
			wValue=GetCurrentVolumeValue();
			break;
		case State_ColorPara_Brightness_Value:
			
	#if(STM32_CONTROL_MCU==MSTAR_6M16JS||XP_TO_ID==XP_TO_FLI32626)
			wValue=g_Brightness_Value;
			break;			
	#endif
		
	#ifdef RGB_COLOR
		case State_ColorPara_Brightness_R_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(BRIGHTNESSR, g_ActiveImageValue);
			g_BrightnessR_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_BrightnessR_Value[g_ActiveImageValue-1];
			#endif
			break;
		case State_ColorPara_Brightness_G_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(BRIGHTNESSG, g_ActiveImageValue);
			g_BrightnessG_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_BrightnessG_Value[g_ActiveImageValue-1];
			#endif
			break;
		case State_ColorPara_Brightness_B_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(BRIGHTNESSB, g_ActiveImageValue);
			g_BrightnessB_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_BrightnessB_Value[g_ActiveImageValue-1];
			#endif	
		break;
	#endif
		case State_ColorPara_Contrast_Value:
		#if(STM32_CONTROL_MCU==MSTAR_6M16JS||XP_TO_ID==XP_TO_FLI32626)
			wValue=g_Contrast_Value;
			break;
		#endif
		
	#ifdef RGB_COLOR
		case State_ColorPara_Contract_R_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(CONTRASTR, g_ActiveImageValue);
			g_ContrastR_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_ContrastR_Value[g_ActiveImageValue-1];
			#endif
			break;
		case State_ColorPara_Contract_G_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(CONTRASTG, g_ActiveImageValue);
			g_ContrastG_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_ContrastG_Value[g_ActiveImageValue-1];
			#endif
			break;
		case State_ColorPara_Contract_B_Value:
			#if(STM32_CONTROL_MCU==MSTAR_6M16JS)
			wValue=GetColorValue(CONTRASTB, g_ActiveImageValue);
			g_ContrastB_Value[g_ActiveImageValue-1]=wValue;
			#elif(STM32_CONTROL_MCU==MSTAR_6M16GS)
			wValue=g_ContrastB_Value[g_ActiveImageValue-1];
			#endif 
		break;
	#endif
	#if(XP_TO_ID==XP_TO_FLI32626)
		case State_ColorPara_Saturation_Value:
				wValue=g_Saturation_Value;
			break;
	#endif
	}
	return wValue;
}


#if(XP_TO_ID==XP_TO_FLI32626)
State GetFadeTimeStateValue()
{
	State curState=State_UserPara_SetFadeTime_1000;
	if(g_FadeTime==1)
		curState=State_UserPara_SetFadeTime_500;
	else if(g_FadeTime==2)
		curState=State_UserPara_SetFadeTime_1000;
	else if(g_FadeTime==3)
		curState=State_UserPara_SetFadeTime_1500;
	return curState;
}

#endif

