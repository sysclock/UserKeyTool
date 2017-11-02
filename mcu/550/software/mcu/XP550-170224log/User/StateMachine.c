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

//U8 g_initPIP=0;
U8 g_initPIPSrc=0;
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
		g_CurrentState=State_MainMenu_Menu;
		DealEscKey();
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
//		g_SaveModeValue=mode;
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x81, g_LoadingModeStr);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x81, g_LoadingModeStrEn);
		if(Updata_Execute(EEPROMMemRegU8,EEPROMMemRegU16))
		{
		//	clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x81, g_LoadModeOKStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x81, g_LoadModeOKStrEn);
		}
		//g_CurrentState=State_MainMenu_Menu;
	}
	else
	{
		clearlcd();
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x80, g_LoadNoModeStr);
		else  if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x80, g_LoadNoModeStrEn);
		//g_CurrentState=State_MainMenu_Menu;
	}
}
/*
void UpdateScreenPara()
{
	g_ScreenHSize=DEF_SCREEN_HSIZE;
	g_ScreenVSize=DEF_SCREEN_VSIZE;
	g_ScreenHPos=DEF_SCREEN_HPOS;
	g_ScreenVPos=DEF_SCREEN_VPOS;
	
	g_MainScalerHSize=DEF_MAIN_SCALER_HSIZE;
	g_MainScalerVSize=DEF_MAIN_SCALER_VSIZE;
	g_MainScalerHPos=DEF_MAIN_SCALER_HPOS;
	g_MainScalerVPos=DEF_MAIN_SCALER_VPOS;
}

void ScreenParaToEEPROM()
{	
	//Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
	//Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
	//Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
	//Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
}
*/

BOOL ResetPara()
{
	//恢复1024*768输出分辨率
	//g_OutputFormatIndex=INDEX1024X768;
	//SetOutputFormat(g_OutputFormatIndex);
	//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);	
	//g_PanelWidth=1024;
	//g_PanelHeight=768;
	//热备份禁止
	initHotBackupVariable(__FALSE);
	Write_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,g_EnableHotBackupFlag);
	Write_EEPROM_Byte(E2ROM_HotBackupIdx_ADDR,g_HotBackupIdx);
	Write_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
	if(reset())
	{
		//复位所有屏参缩放
	//#ifdef START_QUICK_SWITCH
		//g_MutiWindow=SAMEPIPIMAGE;
	//#else
		//g_MutiWindow=SINGLEIMAGE;
	//#endif
		//PanelToScreenPara();
		//ScreenParaToEEPROM();
		SetMultiWindowPara(FUSEPIPPARA1, 0);
		//SetScalerHVValue(HVSIZE,MAIN);
		//SetScalerHVValue(HVPOS,MAIN);
		if(Updata_Execute((U8*)genericEEPROMMemU8,(U16*)genericEEPROMMemU16))
		{
			//循环播放禁止
			g_InputCycleIdx=0;
			g_StartCycleInputFlag=__FALSE;
			initCycleInputPara();
			Write_EEPROM_Byte(E2ROM_CycleFlag_ADDR, g_StartCycleInputFlag);
			//回复十分钟自动锁键盘
			g_LockKeypadTime=10;
			Write_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR,10);	
			//通道计划禁止
			CancelInputScheduleMode1();			
			CancelInputScheduleMode2();			
			CancelInputScheduleMode3();	
			//发送命令使32626将初始值写入NVRAM
			//SendColorValue(COLORRGB,7,0);
			return __TRUE;
		}
	}
	return __FALSE;
}

void SetSingleImage()
{
	SetMultiWindowPara(SINGLEIMAGE,0);
	g_SwitchEffectValue=NormalSwitchIdx;
	g_MainScalerHSize=g_ScreenHSize;
	g_MainScalerVSize=g_ScreenVSize;
	g_MainScalerHPos=g_ScreenHPos;
	g_MainScalerVPos=g_ScreenVPos;
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
	g_MutiWindow=SINGLEIMAGE;		
	Save(MODE0);
}
void SetPIPImage()
{
	SetMultiWindowPara(PIPIMAGE,0);				
	g_MutiWindow=PIPIMAGE;
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
	Save(MODE0);
}

void SetPBPImage()
{
	SetMultiWindowPara(PBPIMAGE,0);				
	g_MutiWindow=PBPIMAGE;
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
	Save(MODE0);
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
	return curState;
		
}

void KeepScreenPara()
{
	if((g_ScreenHPos+g_ScreenHSize)<=g_PanelWidth&&(g_ScreenVPos+g_ScreenVSize)<=g_PanelHeight)
	{
		/*PanelToZoomPara();
		SetScalerHVValue(HVSIZE,MAIN);
		SetScalerHVValue(HVPOS,MAIN);
		if(g_MutiWindow!=SINGLEIMAGE)
		{			
			SetScalerHVValue(HVSIZE,SUB);
			SetScalerHVValue(HVPOS,SUB);
		}*/
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
			g_SetTimeFlag=Set_Current_Time_Idx;
			SetCurrentTime();
			break;
		case State_UserPara_SetFadeTime_1000:
			g_FadeTime=1;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;
		case State_UserPara_SetFadeTime_1500:
			g_FadeTime=2;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;
		case State_UserPara_SetFadeTime_2000:
			g_FadeTime=3;
			Write_EEPROM_Byte(E2ROM_FadeTime_ADDR,g_FadeTime);
			break;
		case State_ColorPara_Reset_Check:
			initColorVariable();
			//SendColorValue(BRIGHTNESS_EVENT,0,g_BrightnessValue);
			//SendColorValue(CONTRAST_EVENT,0,g_ContrastValue);
			//SendColorValue(SATURATION_EVENT,0,g_SaturationValue);
			SendColorValue(RESET_COLOR_EVENT,0,0);
/*			SendColorValue(COLORRGB,4,g_OffsetR);
			SendColorValue(COLORRGB,5,g_OffsetG);
			SendColorValue(COLORRGB,6,g_OffsetB);
			SendColorValue(COLORRGB,1,g_GainR);
			SendColorValue(COLORRGB,2,g_GainG);
			SendColorValue(COLORRGB,3,g_GainB);*/
			SendColorValue(COLORRGB,7,0);
			Save(MODE0);
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
			SendAdcCalibration(ADC_CALIBRATION_EVENT,0,1,0);
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_waitStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, g_waitStrEn);
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			break;
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
		case State_OutputHz_30Hz:
			g_OutputHzVal=30;
			SetOutputHzValue();
			Save(MODE0);
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_OutputHz_50Hz:
			g_OutputHzVal=50;
			SetOutputHzValue();
			Save(MODE0);
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_OutputHz_60Hz:
			g_OutputHzVal=60;
			SetOutputHzValue();
			Save(MODE0);
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			break;
		case State_CancelAll_Yes:			
			CancelInputScheduleMode1();			
			CancelInputScheduleMode2();			
			CancelInputScheduleMode3();
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_CancelSuccessStr);
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, g_CancelSuccessStrEn);
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
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
			//g_PanelWidth=640;
			//g_PanelHeight=480;
			//PanelToScreenPara();
			KeepScreenPara();	
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_800x600:
			g_OutputFormatIndex=INDEX800X600;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=800;
			//g_PanelHeight=600;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1024x768x60:
			g_OutputFormatIndex=INDEX1024X768;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1024;
			//g_PanelHeight=768;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1280x720:
			g_OutputFormatIndex=INDEX1280X720;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1280;
			//g_PanelHeight=720;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1280x1024:
			g_OutputFormatIndex=INDEX1280X1024;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1280;
			//g_PanelHeight=1024;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1366x768:
			g_OutputFormatIndex=INDEX1366X768;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1366;
			//g_PanelHeight=768;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1440x900:
			g_OutputFormatIndex=INDEX1440X900;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1440;
			//g_PanelHeight=900;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1600x1200:
			g_OutputFormatIndex=INDEX1600X1200;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1600;
			//g_PanelHeight=1200;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			g_OutputHzVal=50;
			SetOutputHzValue();
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			Save(MODE0);
			break;
		case State_OutputFormat_1680x1050:
			g_OutputFormatIndex=INDEX1680X1050;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1680;
			//g_PanelHeight=1050;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1920x1080:                                                                                                                                 
			g_OutputFormatIndex=INDEX1920X1080;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1920;
			//g_PanelHeight=1080;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
		case State_OutputFormat_1920x1200:
			g_OutputFormatIndex=INDEX1920X1200;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=1920;
			//g_PanelHeight=1200;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			g_OutputHzVal=50;
			SetOutputHzValue();
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			Save(MODE0);
			break;
		case State_OutputFormat_2048x1152:
			g_OutputFormatIndex=INDEX2048x1152;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=2048;
			//g_PanelHeight=1152;
			//PanelToScreenPara();
			KeepScreenPara();
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			g_OutputHzVal=50;
			SetOutputHzValue();
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			Save(MODE0);
			break;
		case State_OutputFormat_2560x960:
			UserResolutionHSize=2560;
			UserResolutionVSize=960;
			g_OutputFormatIndex=0;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=UserResolutionHSize;
			//g_PanelHeight=UserResolutionVSize;
			//Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
			//PanelToScreenPara();	
			KeepScreenPara();				
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			//ScreenParaToEEPROM();
			
			g_OutputHzVal=50;
			SetOutputHzValue();
			//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			Save(MODE0);
			break;
		case State_OutputFormat_3840x600:
			UserResolutionHSize=3840;
			UserResolutionVSize=600;
			g_OutputFormatIndex=0;
			SetOutputFormat(g_OutputFormatIndex);
			//g_PanelWidth=UserResolutionHSize;
			//g_PanelHeight=UserResolutionVSize;
			//Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
			//PanelToScreenPara();	
			KeepScreenPara();				
			//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
			if(g_OutputHzVal!=60)
			{
				g_OutputHzVal=60;
				SetOutputHzValue();
			}
			Save(MODE0);
			break;
			
		case State_SavePara_Mode_check:
			Save(g_SaveModeValue);
			clearlcd();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x83, g_SaveOKStr);	
			else  if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x83, g_SaveOKStrEn);		
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
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
					chinesedisplay(0x90, g_ResetSuccessStrEn);				
				delay(5);
			}
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			break;

			case State_ScreenPara_Size_Value:
			
				if(g_MutiWindow==SINGLEIMAGE)
				{
					SetScalerHVValue(HVSIZE,MAIN);						
				}
				else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==SAMEPIPIMAGE)
				{
					SetScalerHVValue(HVSIZE,MAIN);
					SetScalerHVValue(HVSIZE,SUB);
				}
				else if(g_MutiWindow==PBPIMAGE)
				{
					SetScalerHVValue(HVSIZE,MAIN);
					SetScalerHVValue(HVPOS,SUB);
					SetScalerHVValue(HVSIZE,SUB);
				}			
				Save(MODE0);
				break;
			case State_ScreenPara_Pos_Value:
				if(g_MutiWindow==SINGLEIMAGE)
				{
					SetScalerHVValue(HVPOS,MAIN);
				}
				else
				{
					SetScalerHVValue(HVPOS,MAIN);
					SetScalerHVValue(HVPOS,SUB);
				}
				Save(MODE0);
				break;

			case State_ScalerPara_Size_Value:
				if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
				{
					SetScalerHVValue(HVSIZE,MAIN);
					if(g_MutiWindow==SAMEPIPIMAGE)
					{
						SetScalerHVValue(HVSIZE,SUB);
					}
				}
				else if(g_ActiveImageValue==SUB)
				{
					SetScalerHVValue(HVSIZE,SUB);
				}
				Save(MODE0);
				break;
				
			case State_ScalerPara_Pos_Value:
				if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
				{
					SetScalerHVValue(HVPOS,MAIN);
					if(g_MutiWindow==SAMEPIPIMAGE)
					{
						SetScalerHVValue(HVPOS,SUB);
					}
				}
				else if(g_ActiveImageValue==SUB)
				{
					SetScalerHVValue(HVPOS,SUB);						
				}
				Save(MODE0);
				break;
				
			case State_ZoomPara_Size_Value:
			{
				U8 image;          //当前通道为主还是子
				GetWhichInput(&image);
				SetZoomHVValue(HVSIZE,image);
				Save(MODE0);
			}
			break;
			
			case State_ZoomPara_Pos_Value:
			{
				U8 image;//当前通道为主还是子
				GetWhichInput(&image);
				SetZoomHVValue(HVPOS,image);
				Save(MODE0);
			}
			break;
			
		default:
			return __FALSE;
		}
	}
	else if((Val==KEY_UP)||(Val==KEY_DOWN))
	{
		switch(currentState)
		{
			case State_Fuse_Switch_Value:
				g_AlphaValue=wValue;
				SetMultiWindowPara(FUSEPIPPARA1, g_AlphaValue);
				Write_EEPROM_Byte(E2ROM_Alpha_ADDR, g_AlphaValue);
				break;	
		#ifdef TCP_IP
			case State_UserPara_SetStaticIP_Value:
				SetStaticIPAddress(wValue);
				break;
		#endif
			case State_OutputHz_NumHz:
				g_OutputHzVal=wValue;
				SetOutputHzValue();
				Save(MODE0);
				//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
				break;
			case State_Volume_Set:
				g_VolumeValue=wValue;
				Write_EEPROM_Byte(E2ROM_VOLUME_ADDR, g_VolumeValue);
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
					g_TryEndDate[0]=TimeSet[0];//试用截止年
					g_TryEndDate[1]=TimeSet[1];//试用截止月
					g_TryEndDate[2]=TimeSet[2];//试用截止日
					Write_EEPROM_TryEndDate(E2ROM_TRYTIME_END_ADDR,g_TryEndDate);
					clearlcd();
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x90, g_SetOKStr);
					else  if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x90, g_SetOKStrEn);
					InputNumReset();
					delay(5);
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
				Time_Adjust();
				InputNumReset();
				delay(5);
				ReturnMainWin();
				break;
			case State_ColorPara_Brightness_Value:
				g_BrightnessValue=wValue;
				SendColorValue(BRIGHTNESS_EVENT,0,wValue);
				Save(MODE0);
				break;
			case State_ColorPara_Contrast_Value:
				g_ContrastValue=wValue;
				SendColorValue(CONTRAST_EVENT,0,wValue);
				Save(MODE0);
				break;
			case State_ColorPara_Saturation_Value:
				g_SaturationValue=wValue;
				SendColorValue(SATURATION_EVENT,0,wValue);
				Save(MODE0);
				break;

			case State_ScreenPara_Size_Value:
				if(g_H_VFlag==__TRUE)
				{	
					if(wValue+g_ScreenHPos>g_PanelWidth)
						wValue=g_PanelWidth-g_ScreenHPos;
					g_ScreenHSize=wValue;
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);						
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerHSize=wValue/3;
						//SetScalerHVValue(HVSIZE,SUB);
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerHSize=wValue/2;
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerHPos=g_MainScalerHPos+g_MainScalerHSize;
						//SetScalerHVValue(HVPOS,SUB);
						g_SubScalerHSize=wValue/2;
						//SetScalerHVValue(HVSIZE,SUB);
					}
					else if(g_MutiWindow==SAMEPIPIMAGE)
					{
						g_MainScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,SUB);
					}
					
				}
				else
				{	
					if((wValue+g_ScreenVPos)>g_PanelHeight)
						wValue=g_PanelHeight-g_ScreenVPos;
					g_ScreenVSize=wValue;
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerVSize=wValue;	
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerVSize=wValue/3;
						//SetScalerHVValue(HVSIZE,SUB);
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,SUB);
					}
					else if(g_MutiWindow==SAMEPIPIMAGE)
					{
						g_MainScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						g_SubScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,SUB);
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
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						if(g_SubScalerHPos<wValue)
						{
							g_SubScalerHPos=wValue;
							//SetScalerHVValue(HVPOS,SUB);
						}
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						g_SubScalerHPos=g_MainScalerHPos+g_MainScalerHSize;
						//SetScalerHVValue(HVPOS,SUB);
					}
					else if(g_MutiWindow==SAMEPIPIMAGE)
					{
						g_MainScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						g_SubScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,SUB);
					}
				}
				else
				{
					if((wValue+g_ScreenVSize)>g_PanelHeight)
						wValue=(g_PanelHeight-g_ScreenVSize);
					g_ScreenVPos=wValue;
					if(g_MutiWindow==SINGLEIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
					}
					else if(g_MutiWindow==PIPIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
					}
					else if(g_MutiWindow==PBPIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						g_SubScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,SUB);
					}
					else if(g_MutiWindow==SAMEPIPIMAGE)
					{
						g_MainScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						g_SubScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,SUB);
					}
				}
				//Save(MODE1);
				break;
			case State_ScreenPara_UserResolution_Value:
			{
				if(g_H_VFlag==__TRUE)
				{	
					UserResolutionHSize=wValue;
				}
				else
				{
					UserResolutionVSize=wValue;
					g_OutputFormatIndex=0;
					SetOutputFormat(g_OutputFormatIndex);
					g_PanelWidth=UserResolutionHSize;
					g_PanelHeight=UserResolutionVSize;
					//Write_Eeprom_UserResolution(E2ROM_UserResolution_ADDR);
					//PanelToScreenPara();	
					KeepScreenPara();				
					//Write_EEPROM_Byte(E2ROM_OUTPUT_FORMAT_ADDR,g_OutputFormatIndex);
					//ScreenParaToEEPROM();
					Save(MODE0);
				}
			}
			break;
			case State_ScalerPara_Size_Value:
				if(g_H_VFlag==__TRUE)
				{	
					if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
					{
						if((wValue+g_MainScalerHPos)>(g_ScreenHPos+g_ScreenHSize))
							wValue=(g_ScreenHPos+g_ScreenHSize)-g_MainScalerHPos;
						g_MainScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						if(g_MutiWindow==SAMEPIPIMAGE)
						{
							g_SubScalerHSize=wValue;
							//SetScalerHVValue(HVSIZE,SUB);
						}
					}
					else if(g_ActiveImageValue==SUB)
					{
						if((wValue+g_SubScalerHPos)>(g_ScreenHPos+g_ScreenHSize))
							wValue=(g_ScreenHPos+g_ScreenHSize)-g_SubScalerHPos;
						g_SubScalerHSize=wValue;
						//SetScalerHVValue(HVSIZE,SUB);
					}
				}
				else
				{	
					if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
					{
						if((wValue+g_MainScalerVPos)>(g_ScreenVPos+g_ScreenVSize))
							wValue=(g_ScreenVPos+g_ScreenVSize)-g_MainScalerVPos;
						g_MainScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,MAIN);
						if(g_MutiWindow==SAMEPIPIMAGE)
						{
							g_SubScalerVSize=wValue;
							//SetScalerHVValue(HVSIZE,SUB);
						}
					}
					else if(g_ActiveImageValue==SUB)
					{
						if((wValue+g_SubScalerVPos)>(g_ScreenVPos+g_ScreenVSize))
							wValue=(g_ScreenVPos+g_ScreenVSize)-g_SubScalerVPos;
						g_SubScalerVSize=wValue;
						//SetScalerHVValue(HVSIZE,SUB);
					}
						
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
					//g_SubInput=INPUTSOURCE_DVI;
					//g_MainInput=INPUTSOURCE_DVI;
					g_MutiWindow=PBPIMAGE;
					if(ParseAndSetMode())
					{
						clearLineFromAddr(0x90);
						if(g_LanguageFlag==CHINESE)
						{
							chinesedisplay(0x90, g_SetOKStr);
						}
						else if(g_LanguageFlag==ENGLISH)
						{
							chinesedisplay(0x90, g_SetOKStrEn);
						}
						delay(5);
						ReturnMainWin();
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
					if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
					{
						if((wValue+g_MainScalerHSize)>(g_ScreenHPos+g_ScreenHSize))
							wValue=(g_ScreenHPos+g_ScreenHSize)-g_MainScalerHSize;
						g_MainScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						if(g_MutiWindow==SAMEPIPIMAGE)
						{
							g_SubScalerHPos=wValue;
							//SetScalerHVValue(HVPOS,SUB);
						}
					}
					else if(g_ActiveImageValue==SUB)
					{
						if((wValue+g_SubScalerHSize)>(g_ScreenHPos+g_ScreenHSize))
							wValue=(g_ScreenHPos+g_ScreenHSize)-g_SubScalerHSize;
						g_SubScalerHPos=wValue;
						//SetScalerHVValue(HVPOS,SUB);						
					}
				}
				else
				{
					if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE||g_MutiWindow==SAMEPIPIMAGE)
					{
						if((wValue+g_MainScalerVSize)>(g_ScreenVPos+g_ScreenVSize))
							wValue=(g_ScreenVPos+g_ScreenVSize)-g_MainScalerVSize;
						g_MainScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,MAIN);
						if(g_MutiWindow==SAMEPIPIMAGE)
						{
							g_SubScalerVPos=wValue;
							//SetScalerHVValue(HVPOS,SUB);
						}
					}
					else if(g_ActiveImageValue==SUB)
					{
						if((wValue+g_SubScalerVSize)>(g_ScreenVPos+g_ScreenVSize))
							wValue=(g_ScreenVPos+g_ScreenVSize)-g_SubScalerVSize;
						g_SubScalerVPos=wValue;
						//SetScalerHVValue(HVPOS,SUB);						
					}
				}
				//Save(MODE1);
				break;

			case State_ZoomPara_Size_Value:
			{
				U8 image;          //当前通道为主还是子
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(wValue>g_InSrcHSize)//判断截取的像素是否大于输入像素
						wValue=g_InSrcHSize;
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA)
						{
							g_MainVGAZoomHSize=wValue;
							if(g_MainVGAZoomHPos>g_InSrcHSize-g_MainVGAZoomHSize)
								g_MainVGAZoomHPos=g_InSrcHSize-g_MainVGAZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubVGAZoomHSize=g_MainVGAZoomHSize;
							}
						}
						else if(g_MainInput==INPUTSOURCE_DVI)
						{
							g_MainDVIZoomHSize=wValue;
							
							if(g_MainDVIZoomHPos>g_InSrcHSize-g_MainDVIZoomHSize)
								g_MainDVIZoomHPos=g_InSrcHSize-g_MainDVIZoomHSize;
							
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubDVIZoomHSize=g_MainDVIZoomHSize;
							}
						}
						else if(g_MainInput==INPUTSOURCE_HDMI)
						{
							g_MainHDMIZoomHSize=wValue;
							if(g_MainHDMIZoomHPos>g_InSrcHSize-g_MainHDMIZoomHSize)
								g_MainHDMIZoomHPos=g_InSrcHSize-g_MainHDMIZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubHDMIZoomHSize=g_MainHDMIZoomHSize;
							}
						}
						else
						{
							g_MainZoomHSize=wValue;
							if(g_MainZoomHPos>g_InSrcHSize-g_MainZoomHSize)
								g_MainZoomHPos=g_InSrcHSize-g_MainZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubZoomHSize=g_MainZoomHSize;
							}
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA)
						{
							g_SubVGAZoomHSize=wValue;
							if(g_SubVGAZoomHPos>g_InSrcHSize-g_SubVGAZoomHSize)
								g_SubVGAZoomHPos=g_InSrcHSize-g_SubVGAZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainVGAZoomHSize=g_SubVGAZoomHSize;
							}
						}
						else if(g_SubInput==INPUTSOURCE_DVI)
						{
							g_SubDVIZoomHSize=wValue;
							if(g_SubDVIZoomHPos>g_InSrcHSize-g_SubDVIZoomHSize)
								g_SubDVIZoomHPos=g_InSrcHSize-g_SubDVIZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainDVIZoomHSize=g_SubDVIZoomHSize;
							}
						}
						else if(g_SubInput==INPUTSOURCE_HDMI)
						{
							g_SubHDMIZoomHSize=wValue;
							if(g_SubHDMIZoomHPos>g_InSrcHSize-g_SubHDMIZoomHSize)
								g_SubHDMIZoomHPos=g_InSrcHSize-g_SubHDMIZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainHDMIZoomHSize=g_SubHDMIZoomHSize;
							}
						}
						else
						{
							g_SubZoomHSize=wValue;
							if(g_SubZoomHPos>g_InSrcHSize-g_SubZoomHSize)
								g_SubZoomHPos=g_InSrcHSize-g_SubZoomHSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainZoomHSize=g_SubZoomHSize;
							}
						}
					}
					//SetZoomValueHV(HSIZE,wValue,0,image);
					
				}
				else
				{
					if(wValue>g_InSrcVSize)     //判断截取的像素是否大于输入像素
						wValue=g_InSrcVSize;
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA)
						{
							g_MainVGAZoomVSize=wValue;
							if(g_MainVGAZoomVPos>g_InSrcVSize-g_MainVGAZoomVSize)
								g_MainVGAZoomVPos=g_InSrcVSize-g_MainVGAZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubVGAZoomVSize=g_MainVGAZoomVSize;
							}
						}
						else if(g_MainInput==INPUTSOURCE_DVI)
						{
							g_MainDVIZoomVSize=wValue;
							if(g_MainDVIZoomVPos>g_InSrcVSize-g_MainDVIZoomVSize)
								g_MainDVIZoomVPos=g_InSrcVSize-g_MainDVIZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubDVIZoomVSize=g_MainDVIZoomVSize;
							}
						}
						else if(g_MainInput==INPUTSOURCE_HDMI)
						{
							g_MainHDMIZoomVSize=wValue;
							if(g_MainHDMIZoomVPos>g_InSrcVSize-g_MainHDMIZoomVSize)
								g_MainHDMIZoomVPos=g_InSrcVSize-g_MainHDMIZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubHDMIZoomVSize=g_MainHDMIZoomVSize;
							}
						}
						else
						{
							g_MainZoomVSize=wValue;
							if(g_MainZoomVPos>g_InSrcVSize-g_MainZoomVSize)
								g_MainZoomVPos=g_InSrcVSize-g_MainZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubZoomVSize=g_MainZoomVSize;
							}
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA)
						{
							g_SubVGAZoomVSize=wValue;
							if(g_SubVGAZoomVPos>g_InSrcVSize-g_SubVGAZoomVSize)
								g_SubVGAZoomVPos=g_InSrcVSize-g_SubVGAZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainVGAZoomVSize=g_SubVGAZoomVSize;
							}
						}
						else if(g_SubInput==INPUTSOURCE_DVI)
						{
							g_SubDVIZoomVSize=wValue;
							if(g_SubDVIZoomVPos>g_InSrcVSize-g_SubDVIZoomVSize)
								g_SubDVIZoomVPos=g_InSrcVSize-g_SubDVIZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainDVIZoomVSize=g_SubDVIZoomVSize;
							}
						}
						else if(g_SubInput==INPUTSOURCE_HDMI)
						{
							g_SubHDMIZoomVSize=wValue;
							if(g_SubHDMIZoomVPos>g_InSrcVSize-g_SubHDMIZoomVSize)
								g_SubHDMIZoomVPos=g_InSrcVSize-g_SubHDMIZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainHDMIZoomVSize=g_SubHDMIZoomVSize;
							}
						}
						else
						{
							g_SubZoomVSize=wValue;
							if(g_SubZoomVPos>g_InSrcVSize-g_SubZoomVSize)
								g_SubZoomVPos=g_InSrcVSize-g_SubZoomVSize;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainZoomVSize=g_SubZoomVSize;
							}
						}
					}
					//SetZoomValueHV(VSIZE,wValue,0,image);
				}
				//Save(MODE1);
			}
			break;
			case State_ZoomPara_Pos_Value:
			{
				U8 image;//当前通道为主还是子
				GetWhichInput(&image);
				if(g_H_VFlag==__TRUE)
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA)
						{
							if(wValue>g_InSrcHSize-g_MainVGAZoomHSize)
								wValue=g_InSrcHSize-g_MainVGAZoomHSize;
							g_MainVGAZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubVGAZoomHPos=g_MainVGAZoomHPos;
							}
						}
						else if(g_MainInput==INPUTSOURCE_DVI)
						{
							if(wValue>g_InSrcHSize-g_MainDVIZoomHSize)
								wValue=g_InSrcHSize-g_MainDVIZoomHSize;
							g_MainDVIZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubDVIZoomHPos=g_MainDVIZoomHPos;
							}
						}
						else if(g_MainInput==INPUTSOURCE_HDMI)
						{
							if(wValue>g_InSrcHSize-g_MainHDMIZoomHSize)
								wValue=g_InSrcHSize-g_MainHDMIZoomHSize;
							g_MainHDMIZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubHDMIZoomHPos=g_MainHDMIZoomHPos;
							}
						}
						else
						{
							if(wValue>g_InSrcHSize-g_MainZoomHSize)
								wValue=g_InSrcHSize-g_MainZoomHSize;
							g_MainZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubZoomHPos=g_MainZoomHPos;
							}
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA)
						{
							if(wValue>g_InSrcHSize-g_SubVGAZoomHSize)
								wValue=g_InSrcHSize-g_SubVGAZoomHSize;
							g_SubVGAZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainVGAZoomHPos=g_SubVGAZoomHPos;
							}
						}
						else if(g_SubInput==INPUTSOURCE_DVI)
						{
							if(wValue>g_InSrcHSize-g_SubDVIZoomHSize)
								wValue=g_InSrcHSize-g_SubDVIZoomHSize;
							g_SubDVIZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainDVIZoomHPos=g_SubDVIZoomHPos;
							}
						}
						else if(g_SubInput==INPUTSOURCE_HDMI)
						{
							if(wValue>g_InSrcHSize-g_SubHDMIZoomHSize)
								wValue=g_InSrcHSize-g_SubHDMIZoomHSize;
							g_SubHDMIZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainHDMIZoomHPos=g_SubHDMIZoomHPos;
							}
						}
						else
						{
							if(wValue>g_InSrcHSize-g_SubZoomHSize)
								wValue=g_InSrcHSize-g_SubZoomHSize;
							g_SubZoomHPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainZoomHPos=g_SubZoomHPos;
							}
						}
					}
					//SetZoomValueHV(HPOS,wValue,0,image);
				}
				else
				{
					if(image==MAIN)
					{
						if(g_MainInput==INPUTSOURCE_VGA)
						{
							if(wValue>g_InSrcVSize-g_MainVGAZoomVSize)
								wValue=g_InSrcVSize-g_MainVGAZoomVSize;
							g_MainVGAZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubVGAZoomVPos=g_MainVGAZoomVPos;
							}
						}
						else if(g_MainInput==INPUTSOURCE_DVI)
						{
							if(wValue>g_InSrcVSize-g_MainDVIZoomVSize)
								wValue=g_InSrcVSize-g_MainDVIZoomVSize;
							g_MainDVIZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubDVIZoomVPos=g_MainDVIZoomVPos;
							}
						}
						else if(g_MainInput==INPUTSOURCE_HDMI)
						{
							if(wValue>g_InSrcVSize-g_MainHDMIZoomVSize)
								wValue=g_InSrcVSize-g_MainHDMIZoomVSize;
							g_MainHDMIZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubHDMIZoomVPos=g_MainHDMIZoomVPos;
							}
						}
						else
						{
							if(wValue>g_InSrcVSize-g_MainZoomVSize)
								wValue=g_InSrcVSize-g_MainZoomVSize;
							g_MainZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_SubZoomVPos=g_MainZoomVPos;
							}
						}
					}
					else if(image==SUB)
					{
						if(g_SubInput==INPUTSOURCE_VGA)
						{
							if(wValue>g_InSrcVSize-g_SubVGAZoomVSize)
								wValue=g_InSrcVSize-g_SubVGAZoomVSize;
							g_SubVGAZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainVGAZoomVPos=g_SubVGAZoomVPos;
							}
						}
						else if(g_SubInput==INPUTSOURCE_DVI)
						{
							if(wValue>g_InSrcVSize-g_SubDVIZoomVSize)
								wValue=g_InSrcVSize-g_SubDVIZoomVSize;
							g_SubDVIZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainDVIZoomVPos=g_SubDVIZoomVPos;
							}
						}
						else if(g_SubInput==INPUTSOURCE_HDMI)
						{
							if(wValue>g_InSrcVSize-g_SubHDMIZoomVSize)
								wValue=g_InSrcVSize-g_SubHDMIZoomVSize;
							g_SubHDMIZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainHDMIZoomVPos=g_SubHDMIZoomVPos;
							}
						}
						else
						{
							if(wValue>g_InSrcVSize-g_SubZoomVSize)
								wValue=g_InSrcVSize-g_SubZoomVSize;
							g_SubZoomVPos=wValue;
							if(g_MutiWindow==SAMEPIPIMAGE)
							{
								g_MainZoomVPos=g_SubZoomVPos;
							}
						}
					}
					//SetZoomValueHV(VPOS,wValue,0,image);
				}
				//Save(MODE1);
			}
			break;
			default:
				return __FALSE;
		}
	}	
	return __TRUE;
}




U16 GetCurrentVolumeValue()
{
	U16 wValue;
	wValue=g_VolumeValue;
	return wValue;	
}

/*State GetAVOrYPbPr()
{
	State curState;
	if(g_IsAV4OrYPbPr==0)
		curState=State_AVOrYPbPr_YPbPr;
	else
		curState=State_AVOrYPbPr_AV;
	return curState;
		
}*/

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

State GetFadeTimeStateValue()
{
	State curState=State_UserPara_SetFadeTime_1500;
	if(g_FadeTime==1)
		curState=State_UserPara_SetFadeTime_1000;
	else if(g_FadeTime==2)
		curState=State_UserPara_SetFadeTime_1500;
	else if(g_FadeTime==3)
		curState=State_UserPara_SetFadeTime_2000;
	return curState;
}
State GetOutFormatStateValue()
{
	State curState=State_OutputFormat_UserPrefer;
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
			curState=State_OutputFormat_1024x768x60;
			break;
		case INDEX1280X720:
			curState=State_OutputFormat_1280x720;
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
		case INDEX1600X1200:
			curState=State_OutputFormat_1600x1200;
			break;
		case INDEX1680X1050:
			curState=State_OutputFormat_1680x1050;
			break;
		case INDEX1920X1080:
			curState=State_OutputFormat_1920x1080;
			break;
		case INDEX1920X1200:
			curState=State_OutputFormat_1920x1200;
			break;
		case INDEX2048x1152:
			curState=State_OutputFormat_2048x1152;
			break;
	}
	if(g_PanelWidth==2560&&g_PanelHeight==960)
		curState=State_OutputFormat_2560x960;
	if(g_PanelWidth==3840&&g_PanelHeight==600)
		curState=State_OutputFormat_3840x600;
				
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
	if(g_MutiWindow==SINGLEIMAGE)
		curState=State_MultiWindow_Single;
	else if(g_MutiWindow==PIPIMAGE)
		curState=State_MultiWindow_PIP;
	else if(g_MutiWindow==PBPIMAGE)
		curState=State_MultiWindow_PBP;
	else if(g_MutiWindow==SAMEPIPIMAGE)
		curState=State_MultiWindow_SAMEPIP;
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

U16 GetCurrentWindowValue()
{
	U16 wValue=0;
	switch(g_CurrentState)
	{
		case State_Fuse_Switch_Value:
			wValue=g_AlphaValue;
			break;
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
			if(g_ActiveImageValue==MAIN)
			{
				if(g_H_VFlag==__TRUE)
					wValue=g_MainScalerHSize;
				else
					wValue=g_MainScalerVSize;
			}
			else if(g_ActiveImageValue==SUB)
			{
				if(g_H_VFlag==__TRUE)
					wValue=g_SubScalerHSize;
				else
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
			U8 image;          //当前通道为主还是子
			GetWhichInput(&image);
			if(g_H_VFlag==__TRUE)
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomHSize;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomHSize;
					else if(g_MainInput==INPUTSOURCE_HDMI)
						wValue=g_MainHDMIZoomHSize;
					else
						wValue=g_MainZoomHSize;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomHSize;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomHSize;
					else if(g_SubInput==INPUTSOURCE_HDMI)
						wValue=g_SubHDMIZoomHSize;
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
					else if(g_MainInput==INPUTSOURCE_HDMI)
						wValue=g_MainHDMIZoomVSize;
					else
						wValue=g_MainZoomVSize;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomVSize;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomVSize;
					else if(g_SubInput==INPUTSOURCE_HDMI)
						wValue=g_SubHDMIZoomVSize;
					else
						wValue=g_SubZoomVSize;
				}
			}
		}
		break;
		case State_ZoomPara_Pos_Value:
		{
			U8 image;          //当前通道为主还是子
			GetWhichInput(&image);
			if(g_H_VFlag==__TRUE)
			{
				if(image==MAIN)
				{
					if(g_MainInput==INPUTSOURCE_VGA)
						wValue=g_MainVGAZoomHPos;
					else if(g_MainInput==INPUTSOURCE_DVI)
						wValue=g_MainDVIZoomHPos;
					else if(g_MainInput==INPUTSOURCE_HDMI)
						wValue=g_MainHDMIZoomHPos;
					else
						wValue=g_MainZoomHPos;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomHPos;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomHPos;
					else if(g_SubInput==INPUTSOURCE_HDMI)
						wValue=g_SubHDMIZoomHPos;
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
					else if(g_MainInput==INPUTSOURCE_HDMI)
						wValue=g_MainHDMIZoomVPos;
					else
						wValue=g_MainZoomVPos;
				}
				else if(image==SUB)
				{
					if(g_SubInput==INPUTSOURCE_VGA)
						wValue=g_SubVGAZoomVPos;
					else if(g_SubInput==INPUTSOURCE_DVI)
						wValue=g_SubDVIZoomVPos;
					else if(g_SubInput==INPUTSOURCE_HDMI)
						wValue=g_SubHDMIZoomVPos;
					else
						wValue=g_SubZoomVPos;
				}
			}
		}
		break;
		case State_ColorPara_Brightness_Value:
			wValue=g_BrightnessValue;
			break;
		case State_ColorPara_Contrast_Value:
			wValue=g_ContrastValue;
			break;
		case State_ColorPara_Saturation_Value:
				wValue=g_SaturationValue;
			break;
/*		case State_ColorPara_Brightness_R_Value:
				wValue=g_OffsetR;
			break;
		case State_ColorPara_Brightness_G_Value:
				wValue=g_OffsetG;
			break;
		case State_ColorPara_Brightness_B_Value:
				wValue=g_OffsetB;
			break;
		case State_ColorPara_Contrast_R_Value:
				wValue=g_GainR;
			break;
		case State_ColorPara_Contrast_G_Value:
				wValue=g_GainG;
			break;
		case State_ColorPara_Contrast_B_Value:
				wValue=g_GainB;
			break;*/
		case State_FreezeImage_Check:			
			if(g_ActiveImageValue==MAIN)
				wValue=g_MainFreezeFlag;
			else if(g_ActiveImageValue==SUB)
				wValue=g_SubFreezeFlag;
			break;
	}
	return wValue;
}

