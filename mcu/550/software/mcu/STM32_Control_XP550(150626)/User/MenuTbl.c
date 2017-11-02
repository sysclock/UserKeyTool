/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: MenuTbl.c
 [Date]:        08-May-2012
 [Comment]:
   MenuTb file
 [Reversion History]:
*******************************************************************************/
#include "header.h"

extern SetTimeFlag g_SetTimeFlag;
extern U8 g_SetInputScheFlag;

void LCDDisplayTabInfo()
{	
	if(g_H_VFlag==__TRUE)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x82, g_SelHStr);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x80, g_SelHStrEn);
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x82, g_SelVStr);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x80, g_SelVStrEn);
	}
}

void WizardSet()
{
	g_CurrentState=State_NewMode_Set;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x83,g_DirectSetStr);
		chinesedisplay(0x91,  g_YesOrNoStr);

	}
	else  if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x82,g_DirectSetStrEn);
		chinesedisplay(0x91,  g_YesOrNoStrEn);
	}
}

void TwoImageSetWizard()
{
	g_H_VFlag=__TRUE;
	g_ActiveImageValue=MAIN;
	DealCurrentState(State_DualImage_Set_MainSize);
	chinesedisplay(0x91, "_   x");
/*	SetPBPImage();
	g_OutputFormatIndex=INDEX1600X1200;
	SetOutputFormat(g_OutputFormatIndex);	
	LoadOutputResolution();
	PanelToScreenPara();*/
}

void LEDWarning(_KeypadNum key)
{
	delay1000(0x400);
	SetKeyLEDLight(KEY_NUM);
	delay1000(0x400);
	SetKeyLEDLight(key);
	delay1000(0x400);
	SetKeyLEDLight(KEY_NUM);
	delay1000(0x400);
	SetKeyLEDLight(key);
	delay1000(0x400);
	SetKeyLEDLight(KEY_NUM);
	delay1000(0x400);
	SetKeyLEDLight(key);
}

void DisplayHV_()
{	
	//以下光标提示用户输入数字，或者按上下键加减1
	if(g_H_VFlag==__TRUE)
		chinesedisplay(LCDADDR91, "_   ");
	else
		chinesedisplay(0x93, "x _   ");
}

void GetWhichInput(U8 *MainOrSub)
{
	U8 image;
	if(g_MutiWindow==SAMEPIPIMAGE)//single channel
	{
		if(g_NextPointImage==SUB)
		{
			image=MAIN;
		}
		else
		{
			image=SUB;
		}
	}
	else
	{
		if(g_ActiveImageValue==MAIN||g_MutiWindow==SINGLEIMAGE)
		{
			image=MAIN;
		}
		else
		{
			image=SUB;
		}
	}
	if(MainOrSub)
		*MainOrSub=image;
}

void LCDDisplayrHVValue()
{
	switch(g_CurrentState)
	{
		case State_ScreenPara_Size_Value:
			DisFourNumValue(g_ScreenHSize,LCDADDR91);
			DisCurWinSizeValue(g_ScreenVSize,0x93,'x');
			break;
		case State_ScreenPara_Pos_Value:
			DisFourNumValue(g_ScreenHPos,LCDADDR91);
			DisCurWinSizeValue(g_ScreenVPos,0x93,'x');
			break;
		case State_ScreenPara_UserResolution_Value:
			DisFourNumValue(UserResolutionHSize,LCDADDR91);
			DisCurWinSizeValue(UserResolutionVSize,0x93,'x');
			break;		
		case State_DualImage_Set_MainSize:
			DisFourNumValue(g_MainScalerHSize,LCDADDR91);
			DisCurWinSizeValue(g_MainScalerVSize,0x93,'x');
			break;
		case State_DualImage_Set_SubSize:
			DisFourNumValue(g_SubScalerHSize,LCDADDR91);
			DisCurWinSizeValue(g_SubScalerVSize,0x93,'x');
			break;			
		case State_ScalerPara_Size_Value:
			if(g_ActiveImageValue==MAIN)
			{	
				DisFourNumValue(g_MainScalerHSize,LCDADDR91);
				DisCurWinSizeValue(g_MainScalerVSize,0x93,'x');
			}
			else
			{
				DisFourNumValue(g_SubScalerHSize,LCDADDR91);
				DisCurWinSizeValue(g_SubScalerVSize,0x93,'x');
			}
			break;
		case State_ScalerPara_Pos_Value:
			if(g_ActiveImageValue==MAIN)
			{	
				DisFourNumValue(g_MainScalerHPos,LCDADDR91);
				DisCurWinSizeValue(g_MainScalerVPos,0x93,'x');
			}
			else
			{
				DisFourNumValue(g_SubScalerHPos,LCDADDR91);
				DisCurWinSizeValue(g_SubScalerVPos,0x93,'x');
			}
			break;
		case State_ZoomPara_Size_Value:
		{
			U8 image;
			GetWhichInput(&image);
			if(image==MAIN)
			{	
				if(g_MainInput==INPUTSOURCE_VGA)
				{
				DisFourNumValue(g_MainVGAZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_MainVGAZoomVSize,0x93,'x');
				}
				else if(g_MainInput==INPUTSOURCE_DVI)
				{
				DisFourNumValue(g_MainDVIZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_MainDVIZoomVSize,0x93,'x');
				}
				else if(g_MainInput==INPUTSOURCE_HDMI)
				{
				DisFourNumValue(g_MainHDMIZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_MainHDMIZoomVSize,0x93,'x');
				}
				else
				{
				DisFourNumValue(g_MainZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_MainZoomVSize,0x93,'x');
				}
			}
			else
			{
				if(g_SubInput==INPUTSOURCE_VGA)
				{
				DisFourNumValue(g_SubVGAZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_SubVGAZoomVSize,0x93,'x');
				}
				else if(g_SubInput==INPUTSOURCE_DVI)
				{
				DisFourNumValue(g_SubDVIZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_SubDVIZoomVSize,0x93,'x');
				}
				else if(g_SubInput==INPUTSOURCE_HDMI)
				{
				DisFourNumValue(g_SubHDMIZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_SubHDMIZoomVSize,0x93,'x');
				}
				else
				{
				DisFourNumValue(g_SubZoomHSize,LCDADDR91);
				DisCurWinSizeValue(g_SubZoomVSize,0x93,'x');
				}
			}
		}
			break;
		case State_ZoomPara_Pos_Value:
		{
			U8 image;
			GetWhichInput(&image);
			if(image==MAIN)
			{	
				if(g_MainInput==INPUTSOURCE_VGA)
				{
				DisFourNumValue(g_MainVGAZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_MainVGAZoomVPos,0x93,'x');
				}
				else if(g_MainInput==INPUTSOURCE_DVI)
				{
				DisFourNumValue(g_MainDVIZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_MainDVIZoomVPos,0x93,'x');
				}
				else if(g_MainInput==INPUTSOURCE_HDMI)
				{
				DisFourNumValue(g_MainHDMIZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_MainHDMIZoomVPos,0x93,'x');
				}
				else
				{
				DisFourNumValue(g_MainZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_MainZoomVPos,0x93,'x');
				}
			}
			else
			{
				if(g_SubInput==INPUTSOURCE_VGA)
				{
				DisFourNumValue(g_SubVGAZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_SubVGAZoomVPos,0x93,'x');
				}
				else if(g_SubInput==INPUTSOURCE_DVI)
				{
				DisFourNumValue(g_SubDVIZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_SubDVIZoomVPos,0x93,'x');
				}
				else if(g_SubInput==INPUTSOURCE_HDMI)
				{
				DisFourNumValue(g_SubHDMIZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_SubHDMIZoomVPos,0x93,'x');
				}
				else
				{
				DisFourNumValue(g_SubZoomHPos,LCDADDR91);
				DisCurWinSizeValue(g_SubZoomVPos,0x93,'x');
				}
			}
		}
			break;
		default:
			break;
	}
}

void AdviceForUser()
{
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x82,g_SetNotStr);
		chinesedisplay(0x90,g_SelectAdvProStr);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x82,g_SetNotStrEn);
		chinesedisplay(0x90,g_SelectAdvProStrEn);
	}
}

void ErrorFullZoom(void)
{
	LEDWarning(FULLPART_KEY);
}

void FullZoomScreen(U8 MainOrSub,U8 *FullOrPartValue)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();	
	if(g_MainScalerHSize==g_PanelWidth&&g_MainScalerVSize==g_PanelHeight)
	{	
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x82, g_WarnningStr);
			chinesedisplay(0x91, g_PleaseSetScreenStr);
		}
		else
		{
			chinesedisplay(0x82, g_WarnningStrEn);
			chinesedisplay(0x91, "Please Set Size");
		}
		LEDWarning(FULLPART_KEY);
		delay(10);
		ReturnMainWin();
		return;
	}	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x80, g_fullZoom);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x80, g_fullZoomEn);
	wcom(0x90);printStr("*");
	if(*FullOrPartValue==FULL)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_jubu);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_jubuEn);
		
		//*************************2013-3-12**********************************
		SetScreenPART(MainOrSub);		
		//*************************2013-3-12**********************************
		*FullOrPartValue=PART;	
		g_CurrentState=State_FullOrPart_Part;
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_full);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_fullEn);

		//*************************2013-3-12**********************************
		SetScreenFULL(MainOrSub);
		//*************************2013-3-12**********************************
		*FullOrPartValue=FULL;
		g_CurrentState=State_FullOrPart_Full;
	}
	Write_EEPROM_PcPart();
}

void LCDDisplayWhiceImage()
{
	if(g_MutiWindow==PBPIMAGE||g_MutiWindow==PIPIMAGE)
	{
		if(g_ActiveImageValue==MAIN)
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(SELIMGADDR, g_MainImage);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(SELIMGADDR, g_MainImageEn);
		}
		else if(g_ActiveImageValue==SUB)
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(SELIMGADDR, g_SubImage);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(SELIMGADDR, g_SubImageEn);
		}
	}		
}

void LCDDisplayFreezeState()
{
	clearLineFromAddr(0x90);
	chinesedisplay(0x90, "*");
	if(g_ActiveImageValue==MAIN)
	{
		if(g_MainFreezeFlag==UNFREEZE)
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_FreezeStr);
			else
				chinesedisplay(0x91, g_FreezeStrEn);
		}
		else
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_LiveStr);
			else
				chinesedisplay(0x91, g_LiveStrEn);
		}
	}
	else
	{
		if(g_SubFreezeFlag==UNFREEZE)
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_FreezeStr);
			else
				chinesedisplay(0x91, g_FreezeStrEn);
		}
		else
		{
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_LiveStr);
			else
				chinesedisplay(0x91, g_LiveStrEn);
		}
	}
	LCDDisplayWhiceImage();
}

void LCDDealFreezeState()
{
	clearLineFromAddr(0x90);
	chinesedisplay(0x90, ">");
	if(((g_SwitchEffectValue==NormalSwitchIdx||g_SwitchEffectValue==FuseSwitchIdx)&&g_ActiveImageValue==MAIN)||(g_SwitchEffectValue!=NormalSwitchIdx&&g_NextPointImage==SUB))
	{
		if(g_MainFreezeFlag==UNFREEZE)
		{
			g_MainFreezeFlag=FREEZE;
			SetFreezeImage(MAIN,FREEZE);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_LiveStr);
			else
				chinesedisplay(0x91, g_LiveStrEn);
		}
		else
		{
			g_MainFreezeFlag=UNFREEZE;
			SetFreezeImage(MAIN,UNFREEZE);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_FreezeStr);
			else
				chinesedisplay(0x91, g_FreezeStrEn);
		}
	}
	else if(((g_SwitchEffectValue==NormalSwitchIdx||g_SwitchEffectValue==FuseSwitchIdx)&&g_ActiveImageValue==SUB)||(g_SwitchEffectValue!=NormalSwitchIdx&&g_NextPointImage==MAIN))
	{
		if(g_SubFreezeFlag==FREEZE)
		{
			g_SubFreezeFlag=UNFREEZE;
			SetFreezeImage(SUB,UNFREEZE);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_FreezeStr);
			else
				chinesedisplay(0x91, g_FreezeStrEn);
		}
		else
		{
			g_SubFreezeFlag=FREEZE;
			SetFreezeImage(SUB,FREEZE);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x91, g_LiveStr);
			else
				chinesedisplay(0x91, g_LiveStrEn);
		}
	}
	
	LCDDisplayWhiceImage();
}


void Deal_PIP_Pos(_PIPPos Pos)
{
	switch(Pos)
	{
		case PIP_LT:  //Left Top
			g_SubScalerHPos=0;
			g_SubScalerVPos=0;
			break;
		case PIP_LB:	
			g_SubScalerHPos=0;
			g_SubScalerVPos=g_MainScalerVSize-g_SubScalerVSize;
			break;
		case PIP_MC:		
			g_SubScalerHPos=(g_MainScalerHSize-g_SubScalerHSize)/2;
			g_SubScalerVPos=(g_MainScalerVSize-g_SubScalerVSize)/2;
			break;
		case PIP_RT:		
			g_SubScalerHPos=g_MainScalerHSize-g_SubScalerHSize;
			g_SubScalerVPos=0;
			break;
		case PIP_RB:	
			g_SubScalerHPos=g_MainScalerHSize-g_SubScalerHSize;
			g_SubScalerVPos=g_MainScalerVSize-g_SubScalerVSize;
			break;
	}	
	SetScalerHVValue(HVPOS, SUB);
}

void FullZoomVideo(U8 MainOrSub)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();	
	LCDDisplayWhiceImage();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x80, g_fullZoom);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x80, g_fullZoomEn);
	wcom(0x90);printStr("*");
	if(g_FullOrPartValue==FULL)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_jubu);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_jubuEn);
		if(MainOrSub==MAIN)
		{
			//SetZoomValue(HSIZE,g_MainZoomHSize, MainOrSub);
			//SetZoomValue(VSIZE, g_MainZoomVSize, MainOrSub);
			//SetZoomValue(HPOS, g_MainZoomHPos, MainOrSub);
			//SetZoomValue(VPOS, g_MainZoomVPos, MainOrSub);
			SetZoomValueHV(HVSIZE,g_MainZoomHSize,g_MainZoomVSize, MAIN);
			SetZoomValueHV(HVPOS,g_MainZoomHPos,g_MainZoomVPos, MAIN);
		}
		else if(MainOrSub==SUB)
		{
			//SetZoomValue(HSIZE,g_SubZoomHSize, MainOrSub);
			//SetZoomValue(VSIZE, g_SubZoomVSize, MainOrSub);
			//SetZoomValue(HPOS, g_SubZoomHPos, MainOrSub);
			//SetZoomValue(VPOS, g_SubZoomVPos, MainOrSub);
			SetZoomValueHV(HVSIZE,g_SubZoomHSize,g_SubZoomVSize, SUB);
			SetZoomValueHV(HVPOS,g_SubZoomHPos,g_SubZoomVPos, SUB);
		}
		g_FullOrPartValue=PART;	
		g_CurrentState=State_FullOrPart_Part;
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_full);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_fullEn);
		//SetZoomValue(HSIZE,0, MainOrSub);
		//SetZoomValue(VSIZE, 0, MainOrSub);
		//SetZoomValue(HPOS, 0, MainOrSub);
		//SetZoomValue(VPOS, 0, MainOrSub);
		SetZoomValueHV(HVSIZE,0,0, MainOrSub);
		SetZoomValueHV(HVPOS,0,0, MainOrSub);
		g_FullOrPartValue=FULL;
		g_CurrentState=State_FullOrPart_Full;
	}
}

void FullZoomVGA(U8 MainOrSub)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();
	LCDDisplayWhiceImage();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x80, g_fullZoom);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x80, g_fullZoomEn);
	wcom(0x90);printStr("*");
	if(g_FullOrPartValue==FULL)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_jubu);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_jubuEn);
		SetZoomHVValue(HVSIZE,MainOrSub);
		SetZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=PART;
		g_CurrentState=State_FullOrPart_Part;
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_full);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_fullEn);
		SetFullZoomHVValue(HVSIZE,MainOrSub);
		SetFullZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=FULL;	
		g_CurrentState=State_FullOrPart_Full;
	}
}

void FullZoomDVI(U8 MainOrSub)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();	
	LCDDisplayWhiceImage();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x80, g_fullZoom);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x80, g_fullZoomEn);
	wcom(0x90);printStr("*");
	if(g_FullOrPartValue==FULL)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_jubu);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_jubuEn);
		SetZoomHVValue(HVSIZE,MainOrSub);
		SetZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=PART;
		g_CurrentState=State_FullOrPart_Part;
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_full);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_fullEn);
		SetFullZoomHVValue(HVSIZE,MainOrSub);
		SetFullZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=FULL;	
		g_CurrentState=State_FullOrPart_Full;
	}
}
void FullZoomHDMI(U8 MainOrSub)
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();	
	LCDDisplayWhiceImage();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x80, g_fullZoom);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x80, g_fullZoomEn);
	wcom(0x90);printStr("*");
	if(g_FullOrPartValue==FULL)
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_jubu);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_jubuEn);
		SetZoomHVValue(HVSIZE,MainOrSub);
		SetZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=PART;	
		g_CurrentState=State_FullOrPart_Part;
	}
	else
	{
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x91, g_full);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_fullEn);
		SetFullZoomHVValue(HVSIZE,MainOrSub);
		SetFullZoomHVValue(HVPOS,MainOrSub);
		g_FullOrPartValue=FULL;
		g_CurrentState=State_FullOrPart_Full;
	}
}

void DealFullOrPartKey()
{	
	InputNumReset();
	if(g_MutiWindow==SINGLEIMAGE)//single channel
	{
		if(g_MainInput==INPUTSOURCE_VGA)
		{
			if((g_MainVGAZoomHSize==0)&&(g_MainVGAZoomVSize==0))
				FullZoomScreen(MAIN,&g_VGAFullOrPartValue);
			else
				FullZoomVGA(MAIN);
		}
		else if(g_MainInput==INPUTSOURCE_DVI)
		{
			if((g_MainDVIZoomHSize==0)&&(g_MainDVIZoomVSize==0))
				FullZoomScreen(MAIN,&g_DVIFullOrPartValue);
			else
				FullZoomDVI(MAIN);
		}
		else  if(g_MainInput==INPUTSOURCE_HDMI)
		{
			if((g_MainHDMIZoomHSize==0)&&(g_MainHDMIZoomVSize==0))
				FullZoomScreen(MAIN,&g_HDMIFullOrPartValue);
			else
				FullZoomHDMI(MAIN);
		}
		else
		{
			if((g_MainZoomHSize==0)&&(g_MainZoomVSize==0))
//				FullZoomScreen(MAIN,&g_OtherFullOrPartValue);//ErrorFullZoom();//
				
                     ErrorFullZoom();
			else
				FullZoomVideo(MAIN);
		}
	}
	else if(g_MutiWindow==SAMEPIPIMAGE)//single channel
	{
		_InputSourceType inputSource;
		U8 image;
		if(g_NextPointImage==SUB)
		{
			inputSource=g_MainInput;
			image=MAIN;
		}
		else
		{
			inputSource=g_SubInput;
			image=SUB;
		}
		if(inputSource==INPUTSOURCE_VGA)
		{
			if(((image==MAIN)&&(g_MainVGAZoomHSize==0)&&(g_MainVGAZoomVSize==0))
				||((image==SUB)&&(g_SubVGAZoomHSize==0)&&(g_SubVGAZoomVSize==0)))
				FullZoomScreen(image,&g_VGAFullOrPartValue);
			else
				FullZoomVGA(image);
		}
		else if(inputSource==INPUTSOURCE_DVI)
		{
			if(((image==MAIN)&&(g_MainDVIZoomHSize==0)&&(g_MainDVIZoomVSize==0))
				||((image==SUB)&&(g_SubDVIZoomHSize==0)&&(g_SubDVIZoomVSize==0)))
				FullZoomScreen(image,&g_DVIFullOrPartValue);
			else
				FullZoomDVI(image);
		}
		else  if(inputSource==INPUTSOURCE_HDMI)
		{
			if(((image==MAIN)&&(g_MainHDMIZoomHSize==0)&&(g_MainHDMIZoomVSize==0))
				||((image==SUB)&&(g_SubHDMIZoomHSize==0)&&(g_SubHDMIZoomVSize==0)))
				FullZoomScreen(image,&g_HDMIFullOrPartValue);
			else
				FullZoomHDMI(image);
		}
		else
		{
			if(((image==MAIN)&&(g_MainZoomHSize==0)&&(g_MainZoomVSize==0))
				||((image==SUB)&&(g_SubZoomHSize==0)&&(g_SubZoomVSize==0)))
//				FullZoomScreen(image,&g_OtherFullOrPartValue);//ErrorF ullZoom();//
                     ErrorFullZoom();
			else
				FullZoomVideo(image);
		}
	}
	else
	{
		U8 inputSource;	
		if(g_ActiveImageValue==MAIN)
			inputSource=g_MainInput;
		else
			inputSource=g_SubInput;
		if(inputSource==INPUTSOURCE_VGA)
		{
			if((g_ActiveImageValue==MAIN)&&(g_MainVGAZoomHSize==0)&&(g_MainVGAZoomVSize==0))
			{
				FullZoomScreen(MAIN,&g_VGAFullOrPartValue);
			}
			else if((g_ActiveImageValue==SUB)&&(g_SubVGAZoomHSize==0)&&(g_SubVGAZoomVSize==0))
				ErrorFullZoom();
			else
				FullZoomVGA(g_ActiveImageValue);
		}
		else if(inputSource==INPUTSOURCE_DVI)
		{
			if((g_ActiveImageValue==MAIN)&&(g_MainDVIZoomHSize==0)&&(g_MainDVIZoomVSize==0))
			{
				FullZoomScreen(MAIN,&g_DVIFullOrPartValue);
			}
			else if((g_ActiveImageValue==SUB)&&(g_SubDVIZoomHSize==0)&&(g_SubDVIZoomVSize==0))
				ErrorFullZoom();
			else
				FullZoomDVI(g_ActiveImageValue);
		}
		else if(inputSource==INPUTSOURCE_HDMI)
		{
			if((g_ActiveImageValue==MAIN)&&(g_MainHDMIZoomHSize==0)&&(g_MainHDMIZoomVSize==0))
			{
				FullZoomScreen(MAIN,&g_HDMIFullOrPartValue);
			}
			else if((g_ActiveImageValue==SUB)&&(g_SubHDMIZoomHSize==0)&&(g_SubHDMIZoomVSize==0))
				ErrorFullZoom();
			else
				FullZoomHDMI(g_ActiveImageValue);
		}
		else
		{
			if((g_ActiveImageValue==MAIN)&&(g_MainZoomHSize==0)&&(g_MainZoomVSize==0))
				ErrorFullZoom();
			else if((g_ActiveImageValue==SUB)&&(g_SubZoomHSize==0)&&(g_SubZoomVSize==0))
				ErrorFullZoom();
			else
				FullZoomVideo(g_ActiveImageValue);
		}
	}
}

void DealMultiWindowKey()
{
	InputNumReset();
	switch(g_CurrentState)
	{
		case State_MultiWindow_Single:
		case State_MultiWindow_PBP:
		case State_MultiWindow_PIP:
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
			break;
		default:			
			g_CurrentState=State_MultiWindow_Single;
			break;			
	}
	DealCurrentState(g_CurrentState);
				
	wcom(LCDADDR90);
	if(g_CurrentState==stateData[g_CurrentState].GetCurrentStateValue())
		printStr("*");
	else
		printStr(">");
}

BOOL StateDisWhichImage()
{
	if(g_MutiWindow==PBPIMAGE||g_MutiWindow==PIPIMAGE)
	{
		switch(g_CurrentState)
		{	
			case State_ScalerPara_Size:
			case State_ScalerPara_Pos:
			case State_ZoomPara_Size:
			case State_ZoomPara_Pos:
				LCDDisplayWhiceImage();
				return __TRUE;
			default:
				return __FALSE;
		}
	}
	else
		return __FALSE;
}

void DealActiveImageKey()   //A/B key
{
	InputNumReset();
	if(g_MutiWindow==SINGLEIMAGE||g_SwitchEffectValue==FadeSwitchIdx||g_SwitchEffectValue==QuickSwitchIdx)
	{
		g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x82, g_WarnningStr);
			chinesedisplay(0x91, g_UsedDoubleStr);
		}
		else
		{
			chinesedisplay(0x82, g_WarnningStrEn);
			chinesedisplay(0x91, "Used Double");
		}
		LEDWarning(NUM9_KEY);
		delay(10);
		ReturnMainWin();		
	}
	else
	{
		_InputSourceType nInput=INPUTSOURCE_NONE;
		U16 wValue=0;
		if(g_ActiveImageValue==MAIN)
		{
			g_ActiveImageValue=SUB;
		}
		else if(g_ActiveImageValue==SUB)
		{
			g_ActiveImageValue=MAIN;	
		}
		if(StateDisWhichImage())
			return;
		switch(g_CurrentState)
		{	
			case State_ZoomPara_Size:
			case State_ZoomPara_Pos:
			//get input format	
				GetInputSrcFormat(g_ActiveImageValue,&g_InSrcHSize,&g_InSrcVSize);	
			case State_FullOrPart_Part:
			case State_FullOrPart_Full:
				LCDDisplayWhiceImage();
				break;
			case State_FreezeImage_Check:
				wValue=stateData[g_CurrentState].GetWindowSize();
				if(wValue==UNFREEZE)
				{				
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x91, g_FreezeStr);
					else if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x91, g_FreezeStrEn);
				}
				else if(wValue==FREEZE)
				{			
					if(g_LanguageFlag==CHINESE)
						chinesedisplay(0x91, g_LiveStr);
					else if(g_LanguageFlag==ENGLISH)
						chinesedisplay(0x91, g_LiveStrEn);
				}
				LCDDisplayWhiceImage();
				break;
			case State_ZoomPara_Input_Format:
				LCDDisplayWhiceImage();
				DisInputSrcFormatStr(0x91);
				break;
			default:	
				g_CurrentState=State_MainMenu_Not_Menu;			
				clearlcd();
				
				if(g_MutiWindow==SINGLEIMAGE)
					chinesedisplay(0x80, g_SingleCharacter);
				else
					chinesedisplay(0x80, g_DualCharacter);
				
				if(g_ActiveImageValue==MAIN)
				{				
					nInput=g_MainInput;	
					DisplayLCDInputSrc(0x81," +  ");
					
					if(g_LanguageFlag==CHINESE)
					{
						chinesedisplay(SELIMGADDR, g_MainImage);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(SELIMGADDR, g_MainImageEn);
					}
					g_CurrentState=State_ActiveImage_Main;
						
				}
				else if(g_ActiveImageValue==SUB)
				{
					nInput=g_SubInput;		
					DisplayLCDInputSrc(0x81," +  ");
					if(g_LanguageFlag==CHINESE)
					{
						chinesedisplay(SELIMGADDR, g_SubImage);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(SELIMGADDR, g_SubImageEn);
					}
					g_CurrentState=State_ActiveImage_Sub;
				}
				switch(nInput)
				{
					case INPUTSOURCE_CVBS:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_CVBS1Str);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_CVBS1StrEn);	
						break;
					case INPUTSOURCE_CVBS2:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_CVBS2Str);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_CVBS2StrEn);	
						break;
					case INPUTSOURCE_CVBS3:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_CVBS3Str);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_CVBS3StrEn);	
						break;
					case INPUTSOURCE_CVBS4:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_CVBS4Str);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_CVBS4StrEn);	
						break;
					case INPUTSOURCE_VGA:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_VGAStr);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_VGAStrEn);	
						break;
					case INPUTSOURCE_VGA2:
						chinesedisplay(0x90, "VGA2");
						break;
					case INPUTSOURCE_DVI:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_DVIStr);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_DVIStrEn);	
						break;
					case INPUTSOURCE_HDMI:
						if(g_LanguageFlag==CHINESE)	
							chinesedisplay(0x90, HDMI);
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, HDMIEn);
						break;
					case INPUTSOURCE_YPBPR:
						if(g_LanguageFlag==CHINESE)
							chinesedisplay(0x90, g_YPbPrStr);	
						else if(g_LanguageFlag==ENGLISH)
							chinesedisplay(0x90, g_YPbPrStrEn);	
						break;
					case INPUTSOURCE_SDI:
#if(MATHINE_TYPE==XP_520)
						chinesedisplay(0x90, "SDI");
#elif(MATHINE_TYPE==XP_530)
						switch(g_Adv7441CurrentInput)//(pVideoStat->inputPort)
						{
							case DEVICE_PORT_CVBS1:
								chinesedisplay(0x90, "AV3");
								break;
							case DEVICE_PORT_CVBS2:
								chinesedisplay(0x90, "AV4");
								break;
							case DEVICE_PORT_VGA:
								chinesedisplay(0x90, "VGA2");
								break;
							case DEVICE_PORT_DVI:
								chinesedisplay(0x90, "DVI2");
								break;
							case DEVICE_PORT_HDMI:
								chinesedisplay(0x90, "HDMI2");
								break;
						}
#endif
						break;
				}
			break;
		}
	}
}
void DealMenuKey()
{
	if(IsSetupMode())
		return;
//	ClearScheWeekNum();	
	InputNumReset();
	clearlcd();
	switch(g_CurrentState)
	{
		case State_MainMenu_Menu_ScreenPara:
		case State_MainMenu_Menu_ScalerPara:
		case State_MainMenu_Menu_ZoomPara:
		case State_MainMenu_Menu_ColorPara:
		case State_MainMenu_Menu_AdvancePara:
		case State_MainMenu_Menu_UserPara:
		case State_MainMenu_TimeSchedule:
		case State_MainMenu_Menu_TechSupport:
		case State_MainMenu_Menu_DebugMode:
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
			if((stateData[g_CurrentState].startaddr2th==LCDADDR91)&&(stateData[g_CurrentState].GetCurrentStateValue!=NULL))
			{
				wcom(LCDADDR90);
				if(g_CurrentState==stateData[g_CurrentState].GetCurrentStateValue())
					printStr("*");
				else
					printStr(">");
			}
			break;
		default:
			g_CurrentState=State_MainMenu_Menu_ScreenPara;
				
	}	
	DealCurrentState(g_CurrentState);	
}

void DisMathineIDValue(U32 wValue,U8 addr)
{
	U8 ch[5];	
	ch[0]=wValue%10000/1000+48;
	ch[1]=wValue%1000/100+48;
	ch[2]=wValue%100/10+48;
	ch[3]=wValue%10+48;
	ch[4]=0;	
	chinesedisplay(addr, ch);
}

void DisplaySoftRevision(U8 addr)
{
	U8 ch[6];
	U8 chTmp,i=0;
	chTmp=g_Fli32626HRevision/10%10;
	if(chTmp!=0)
	{
		ch[i++]=chTmp+48;
	}
	chTmp=g_Fli32626HRevision%10;
	ch[i++]=chTmp+48;
	ch[i++]='.';
	chTmp=g_Fli32626LRevision%10;
	ch[i++]=chTmp+48;
	ch[i++]='-';
	ch[i]=0;
	wcom(addr);
	printStr(ch);
	printStr((U8*)g_ReleaseStr);
}

void DealMainInfoState()
{	
	switch(g_CurrentState)
	{
		case State_MainMenu_Menu_InOutput:
			//DisInputSrcFormatStr(0x83);
			DisFourNumValue(g_ScreenHSize,0x83);
			DisCurWinSizeValue(g_ScreenVSize,0x85,'x');
			DisFourNumValue( g_PanelWidth,0x93);
			DisCurWinSizeValue(g_PanelHeight,0x95,'x');
			break;
		case State_MainMenu_Menu_VersionInfo:
			DisplaySoftRevision(0x95);
			chinesedisplay(0x83,g_MachineTypeXPStr);
			break;
		case State_MainMenu_Menu_IDInfo://0x84
			DisMathineIDValue(ChipUniqueID,0x84);
			if(g_DeviceID!=0)
			{
				DisCurWinSizeValue(g_DeviceID,0x86,'-');
			}
			//DisFourNumValue(g_ScreenHSize,0x93);
			//DisCurWinSizeValue(g_ScreenVSize,0x95,'x');
			#ifdef TCP_IP
			  DisMathineStaticIP();
			#endif
			break;
		case State_MainMenu_Menu_Current_Date:
			DisplayTime();
			chinesedisplay(0x91,g_WelcomeStr);
			chinesedisplay(0x90,g_Factory);
			if(g_TryEndDate[2]!=0xff&&g_TryEndDate[1]!=0xff)  //当设置为试用机器显示截止期
			{
				chinesedisplay(0x90,g_DeadlineFmtStr);
				DisFourNumValue(g_TryEndDate[0],0x93);
				DisTwoNumValue(g_TryEndDate[1],0x96);
				DisTwoNumValue(g_TryEndDate[2],0x98);
			}
			break;
		case State_MainMenu_Menu:	
			clearLineFromAddr(0x80);
			if(g_SwitchEffectValue==FadeSwitchIdx)
			{				
				chinesedisplay(0x80, g_FadeCharacter);
				if(g_NextPointImage==MAIN)
				{
					DisplayLCDInputSrc(0x81," -> ");
				}
				else
				{
					DisplayLCDInputSrc(0x81," <- ");
				}
			}
			else if(g_SwitchEffectValue==NormalSwitchIdx)
			{
				if(g_MutiWindow==SINGLEIMAGE)
					chinesedisplay(0x80, g_SingleCharacter);
				else
					chinesedisplay(0x80, g_DualCharacter);
				DisplayLCDInputSrc(0x81," +  ");
			}
			else if(g_SwitchEffectValue==QuickSwitchIdx)
			{
				chinesedisplay(0x80, g_QuickCharacter);
				if(g_NextPointImage==MAIN)
				{
					DisplayLCDInputSrc(0x81," -> ");
				}
				else
				{
					DisplayLCDInputSrc(0x81," <- ");
				}
			}
			else if(g_SwitchEffectValue==FuseSwitchIdx)
			{
				chinesedisplay(0x80, g_FuseCharacter);
				DisplayLCDInputSrc(0x81," +  ");
			}
			break;
	}
}

void DealUpKey()
{
	U8 IsExecuteFunc=0;
	if(IsSetupMode())
		return;
	if((g_CurrentState==State_SavePara_Number)||(g_CurrentState==State_UserPara_Number))
		return;
	if(stateData[g_CurrentState].string2th!=NULL||g_CurrentState==State_OutputHz_NumHz) // 擦除上一页的信息
	{
		InputNumReset();
		if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_UP])
		{
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_UP];
			IsExecuteFunc=1;
		}
		DealCurrentState(g_CurrentState);
		StateDisWhichImage();
		if(stateData[g_CurrentState].ExecFunction!=NULL&&IsExecuteFunc==0)
		{
			if(stateData[g_CurrentState].ExecFunction(KEY_UP,g_CurrentState,0))
				delay(1);
		}

		if((stateData[g_CurrentState].startaddr2th==LCDADDR91)&&(stateData[g_CurrentState].GetCurrentStateValue!=NULL))
		{
			wcom(LCDADDR90);
			if(g_CurrentState==stateData[g_CurrentState].GetCurrentStateValue())
				printStr("*");
			else
				printStr(">");
		}
		if(g_CurrentState==State_Fuse_Switch_Value)
		{			

			DisTwoNumValue(g_AlphaValue,LCDADDR91);
		}
	}
	else // 不擦除上一页的信息		
	{	
		U16 wValue=0;
		InputNumReset();
		if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_DOWN])
		{
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
		}
		if(stateData[g_CurrentState].string2th!=NULL)
		{
			DealCurrentState(g_CurrentState);
			return;
		}
		if(stateData[g_CurrentState].GetWindowSize!=NULL)
			wValue=stateData[g_CurrentState].GetWindowSize();	
		
		
		switch(g_CurrentState)
		{
			case State_ZoomPara_Size_Value:
			case State_ZoomPara_Pos_Value:
			case State_ScalerPara_Size_Value:
			case State_ScalerPara_Pos_Value:				
				LCDDisplayWhiceImage();
			case State_ScreenPara_Size_Value:
			case State_ScreenPara_Pos_Value:			
			case State_Volume_Set:
				if(wValue-g_StepValue<0)
					wValue=0;
				else
					wValue-=g_StepValue;
				DisplayStepValue(STEPADDR);
				break;	
			case State_ColorPara_Brightness_Value:
			case State_ColorPara_Contrast_Value:
			case State_ColorPara_Saturation_Value:
			case State_Fuse_Switch_Value:

				if(wValue-g_StepValue<0)
					wValue=0;
				else
					wValue-=g_StepValue;
				DisFourNumValue(wValue,LCDADDR91);	
				DisplayStepValue(STEPADDR);
				break;	

				
		}
		if(stateData[g_CurrentState].ExecFunction!=NULL)
			stateData[g_CurrentState].ExecFunction(KEY_UP,g_CurrentState,wValue);
		
		
		switch(g_CurrentState)
		{
			case State_FreezeImage_Check:
				LCDDealFreezeState();
				break;
			case State_ScreenPara_Size_Value:
			case State_ScreenPara_Pos_Value:
			case State_ScalerPara_Size_Value:
			case State_ScalerPara_Pos_Value:
			case State_ZoomPara_Size_Value:
			case State_ZoomPara_Pos_Value:
				LCDDisplayrHVValue();
				break;		
			case State_Volume_Set:
				DisCurWinSizeValue(g_VolumeValue,LCDADDR90,'*');
				break;
		}
	}
	DealMainInfoState();
}

void DealDownKey()
{
	U8 IsExecuteFunc=0;
	if(IsSetupMode())
		return;
	if((g_CurrentState==State_SavePara_Number)||(g_CurrentState==State_UserPara_Number))
		return;
	if(stateData[g_CurrentState].string2th!=NULL||g_CurrentState==State_OutputHz_NumHz)  // 擦除上一页的信息
	{
		InputNumReset();
		if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_DOWN])
		{
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
			IsExecuteFunc=1;
		}
		DealCurrentState(g_CurrentState);
		StateDisWhichImage();
		if(stateData[g_CurrentState].ExecFunction!=NULL&&IsExecuteFunc==0)
		{
			if(stateData[g_CurrentState].ExecFunction(KEY_DOWN,g_CurrentState,0))
				delay(1);
		}
		
		if((stateData[g_CurrentState].startaddr2th==LCDADDR91)&&(stateData[g_CurrentState].GetCurrentStateValue!=NULL))
		{
			wcom(LCDADDR90);
			if(g_CurrentState==stateData[g_CurrentState].GetCurrentStateValue())
				printStr("*");
			else
				printStr(">");
		}
		if(g_CurrentState==State_Fuse_Switch_Value)
		{			

			DisTwoNumValue(g_AlphaValue,LCDADDR91);;
		}
	}	
	else // 不擦除上一页的信息
	{		
		U16 wValue=0;
		InputNumReset();
		if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_UP])
		{
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_UP];
		}
		if(stateData[g_CurrentState].GetWindowSize!=NULL)
			wValue=stateData[g_CurrentState].GetWindowSize();
		switch(g_CurrentState)
		{
			case State_ScreenPara_Size_Value:	
				/*if(g_H_VFlag==__TRUE)
				{			
					if((wValue+g_StepValue+g_ScreenHPos)>g_PanelWidth)
						wValue=(g_PanelWidth-g_ScreenHPos);
					else
						wValue+=g_StepValue;
				}
				else
				{
					if((wValue+g_StepValue+g_ScreenVPos)>g_PanelHeight)
						wValue=(g_PanelHeight-g_ScreenVPos);
					else
						wValue+=g_StepValue;
				}*/
				wValue+=g_StepValue;
				break;
			case State_ScreenPara_Pos_Value:	
				/*if(g_H_VFlag==__TRUE)
				{			
				if((wValue+g_StepValue+g_ScreenHSize)>g_PanelWidth)
					wValue=(g_PanelWidth-g_ScreenHSize);
				else
					wValue+=g_StepValue;
				}
				else
				{
				if((wValue+g_StepValue+g_ScreenVSize)>g_PanelHeight)
					wValue=(g_PanelHeight-g_ScreenVSize);
				else
					wValue+=g_StepValue;
				}*/
				wValue+=g_StepValue;
				break;
			case State_ScalerPara_Size_Value:	
				/*if(g_H_VFlag==__TRUE)
				{
					if(g_ActiveImageValue==MAIN&&wValue+g_StepValue+g_MainScalerHPos>(g_ScreenHPos+g_ScreenHSize))
						wValue=g_PanelWidth-g_MainScalerHPos;
					else if(g_ActiveImageValue==SUB&&wValue+g_StepValue+g_SubScalerHPos>(g_ScreenHPos+g_ScreenHSize))
						wValue=g_PanelWidth-g_SubScalerHPos;
					else
						wValue+=g_StepValue;
				}
				else
				{		
					if(g_ActiveImageValue==MAIN&&wValue+g_StepValue+g_MainScalerVPos>(g_ScreenVPos+g_ScreenVSize))
						wValue=g_PanelHeight-g_MainScalerVPos;					
					else if(g_ActiveImageValue==SUB&&wValue+g_StepValue+g_SubScalerVPos>(g_ScreenVPos+g_ScreenVSize))
						wValue=g_PanelHeight-g_SubScalerVPos;
					else
						wValue+=g_StepValue;
				}*/
				wValue+=g_StepValue;
 				LCDDisplayWhiceImage();
 				DisplayStepValue(STEPADDR);
				break;
			case State_ScalerPara_Pos_Value:	
				/*if(g_H_VFlag==__TRUE)
				{		
				if(g_ActiveImageValue==MAIN&&wValue+g_StepValue+g_MainScalerHSize>(g_ScreenHPos+g_ScreenHSize))
					wValue=g_PanelWidth-g_MainScalerHSize;
				else if(g_ActiveImageValue==SUB&&wValue+g_StepValue+g_SubScalerHSize>(g_ScreenHPos+g_ScreenHSize))
					wValue=g_PanelWidth-g_SubScalerHSize;
				else
					wValue+=g_StepValue;
				}
				else
				{
				if(g_ActiveImageValue==MAIN&&wValue+g_StepValue+g_MainScalerVSize>(g_ScreenVPos+g_ScreenVSize))
					wValue=g_PanelHeight-g_MainScalerVSize;
				else if(g_ActiveImageValue==SUB&&wValue+g_StepValue+g_SubScalerVSize>(g_ScreenVPos+g_ScreenVSize))
					wValue=g_PanelHeight-g_SubScalerVSize;
				else
					wValue+=g_StepValue;
				}*/
				wValue+=g_StepValue;
 				LCDDisplayWhiceImage();	
 				DisplayStepValue(STEPADDR);
				break;
			case State_ZoomPara_Size_Value:
			case State_ZoomPara_Pos_Value:
				wValue+=g_StepValue;
 				LCDDisplayWhiceImage();	
 				DisplayStepValue(STEPADDR);
				break;
			case State_ColorPara_Brightness_Value:
			case State_ColorPara_Contrast_Value:
			case State_ColorPara_Saturation_Value:
				if(wValue+g_StepValue>100)
					wValue=100;
				else
					wValue+=g_StepValue;
				DisFourNumValue(wValue,LCDADDR91);
				DisplayStepValue(STEPADDR);
				break;

			case State_Volume_Set:
				if(wValue+g_StepValue>100)
					wValue=100;
				else
					wValue+=g_StepValue;
 				DisplayStepValue(STEPADDR);
				break;
			case State_Fuse_Switch_Value:
				if(wValue+g_StepValue>15)
					wValue=15;
				else
					wValue+=g_StepValue;
				DisTwoNumValue(wValue,LCDADDR91);
 				DisplayStepValue(STEPADDR);
				break;
			
				
		}			
		if(stateData[g_CurrentState].ExecFunction!=NULL)
			stateData[g_CurrentState].ExecFunction(KEY_DOWN,g_CurrentState,wValue);
		switch(g_CurrentState)
		{
			case State_FreezeImage_Check:
				LCDDealFreezeState();
				break;
			case State_ScreenPara_Size_Value:
			case State_ScreenPara_Pos_Value:
			case State_ScalerPara_Size_Value:	
			case State_ScalerPara_Pos_Value:	
			case State_ZoomPara_Size_Value:
			case State_ZoomPara_Pos_Value:
				LCDDisplayrHVValue();				
				break;
			case State_Volume_Set:
				DisCurWinSizeValue(g_VolumeValue,LCDADDR90,'*');
				break;
		}
	}
	DealMainInfoState();
}

State GetCurrentOuputResolution()
{
	switch(g_PanelWidth)
	{
		case 640:
			g_CurrentState=State_OutputFormat_640x480;
			break;
		case 800:
			g_CurrentState=State_OutputFormat_800x600;
			break;
		case 1024:
			g_CurrentState=State_OutputFormat_1024x768x60;
			break;
		case 1280:
			if(g_PanelHeight==1024)
				g_CurrentState=State_OutputFormat_1280x1024;
			else if(g_PanelHeight==720)
				g_CurrentState=State_OutputFormat_1280x720;
			break;
		case 1366:
			g_CurrentState=State_OutputFormat_1366x768;
			break;
		case 1440:
			g_CurrentState=State_OutputFormat_1440x900;
			break;
		case 1600:
			g_CurrentState=State_OutputFormat_1600x1200;
			break;
		case 1680:
			g_CurrentState=State_OutputFormat_1680x1050;
			break;
		case 1920:
			if(g_PanelHeight==1080)
				g_CurrentState=State_OutputFormat_1920x1080;
			else if(g_PanelHeight==1200)
				g_CurrentState=State_OutputFormat_1920x1200;
			break;
		case 2048:
			g_CurrentState=State_OutputFormat_2048x1152;
		default:
			g_CurrentState=State_OutputFormat_UserPrefer;
			break;
	}
	if(g_PanelWidth==2560&&g_PanelHeight==960)
		g_CurrentState=State_OutputFormat_2560x960;
	if(g_PanelWidth==3840&&g_PanelHeight==600)
		g_CurrentState=State_OutputFormat_3840x600;
				
	return g_CurrentState;
}

//extern U8 Number1[4];
//extern U8 Number2[4];
void DealOkKey()
{
	U8 IsExecuteFunc=0;
	State g_PreState=g_CurrentState;
	static U8 Main_HV_Idx=0,Sub_HV_Idx=0;
	if(InputNumberOver(KEY_UP))
		return;
	switch(g_CurrentState)
	{	
		case State_CycleInput_Continue:
			g_CurrentState=State_InputCycle_Set;
			SetTimeSchedule(KEY_ENTER,g_CurrentState,0);
			return;
		case State_ScheduleInput1_Once:
			g_ScheduleInputMode1=ONCESCHE;
		case State_ScheduleInput1_InputWeek:
			clearlcd();
			//写入EEPROM
			SaveInput1Schedule();
				
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x90, g_SetOKStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_SetOKStrEn);
			//ClearScheWeekNum();
			
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			LightInputSrcLED();
			DealEscKey();
			return;
		case State_ScheduleInput2_Once:
			g_ScheduleInputMode2=ONCESCHE;
		case State_ScheduleInput2_InputWeek:
			clearlcd();
			//写入EEPROM
			SaveInput2Schedule();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x90, g_SetOKStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_SetOKStrEn);
			//ClearScheWeekNum();
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			LightInputSrcLED();
			DealEscKey();
			return;
		case State_ScheduleInput3_Once:
			g_ScheduleInputMode3=ONCESCHE;
		case State_ScheduleInput3_InputWeek:
			clearlcd();
			//写入EEPROM
			SaveInput3Schedule();
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x90, g_SetOKStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_SetOKStrEn);
			//ClearScheWeekNum();
			delay(5);
			g_CurrentState=State_MainMenu_Menu;
			LightInputSrcLED();
			DealEscKey();
			return;
	}
	//---------------------Schedule set up---------------
	if(g_SetInputScheFlag==INSRCOVER)  //输入源输入结束，进入设置时间
	{
		g_SetInputScheFlag=INSRCINI;
		g_CurrentState=State_Input_HOUR;
		chinesedisplay(HOURADDR, "_");
		return;
	}
	//------------------------------------------------
		
	switch(g_CurrentState)
	{	
		case State_SavePara_Number:
			NumKeySetSaveMode();
			return;
		case State_UserPara_Number:			
			return;
		// direct setup
		//----------------------start--------------------
		case State_NewMode_Set_InputSource:
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
			}
			else	
			{
				AdviceForUser();
				delay(10);
				WizardSet();
			}
			
			return;
		case State_NewMode_Set_HSize:
		case State_NewMode_Set_VSize:
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0))
					return;
			break;
		//----------------------end--------------------

	}
	clearlcd();
	//显示本机已经设定的分辨率
	if(g_CurrentState==State_AdvancePara_OutputFormat)  
	{
		g_CurrentState=GetCurrentOuputResolution();		
	}

	else if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_OK])
	{
		g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
		IsExecuteFunc=1;
	}
	//
	if(g_CurrentState==State_MainMenu_Menu)
	{
		DealMenuKey();
		return;
	}

	DealCurrentState(g_CurrentState);
	if(stateData[g_CurrentState].string2th!=NULL)
	{

		//判断下一个状态是不是输入重复模式
		//如果是显示已经设置的星期
		switch(g_CurrentState)	  
		{
		#ifdef TCP_IP
			case State_UserPara_SetStaticIP_Value:
				g_StaticIPIdx=IP1_IDX;	
				break;
		#endif
			case State_ScheduleInput1_InputWeek:
				DisplaySavedWeek1();
				break;
			case State_ScheduleInput2_InputWeek:
				DisplaySavedWeek2();
				break;
			case State_ScheduleInput3_InputWeek:
				DisplaySavedWeek3();
				break;
			case State_ZoomPara_Size://放大设置二级菜单
				if(g_MutiWindow==SAMEPIPIMAGE)
				{
					if(g_NextPointImage==MAIN)
						GetInputSrcFormat(SUB,&g_InSrcHSize,&g_InSrcVSize);	
					else
						GetInputSrcFormat(MAIN,&g_InSrcHSize,&g_InSrcVSize);
				}
				else
					GetInputSrcFormat(g_ActiveImageValue,&g_InSrcHSize,&g_InSrcVSize);	
			case State_ScalerPara_Size://双画面二级菜单
			case State_ColorPara_Brightness://颜色设置二级菜单
				LCDDisplayWhiceImage();//显示A/B
				break;
			case State_HotBackup_Check:  //在6、用户设置->5、设置热备份->设置方案，按OK即开启热备份
				g_EnableHotBackupFlag=__TRUE;
				Write_EEPROM_Byte(E2ROM_EnableHotBackupFlag_ADDR,g_EnableHotBackupFlag);
				break;
			case State_User_HotBackup:  //在此处添加显示第几个备份设置
			{
				U8 ch[3]={0};
				ch[0]=g_HotBackupIdx+49;
				ch[1]=':';
				wcom(0x85);
				printStr(ch);
				wcom(0x95);
				printStr(ch);
				//显示之前显示过，但没有按OK,此时可修改或按OK继续
				if(g_HotBackupInput[g_HotBackupIdx][0]!=INPUTSOURCE_NONE)
				{
					DisplayMainInput(0x86,g_HotBackupInput[g_HotBackupIdx][0]);	
				}
				if(g_HotBackupInput[g_HotBackupIdx][1]!=INPUTSOURCE_NONE)
				{
					DisplayMainInput(0x96,g_HotBackupInput[g_HotBackupIdx][1]);
				}
			}
			break;
		}
		if((stateData[g_CurrentState].ExecFunction!=NULL)&&(IsExecuteFunc==0||g_CurrentState==State_TimeSchedule_Begin_Input1
			||g_CurrentState==State_TimeSchedule_Begin_Input2||g_CurrentState==State_TimeSchedule_Begin_Input3))
		{
			if(stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0))
				delay(1);
		}
		
		if((stateData[g_CurrentState].startaddr2th==LCDADDR91)&&(stateData[g_CurrentState].GetCurrentStateValue!=NULL))
		{
			wcom(LCDADDR90);
			if(g_CurrentState==stateData[g_CurrentState].GetCurrentStateValue())
				printStr("*");
			else
				printStr(">");
		}
	}
	else
	{
		U16 wValue=0;
		if(stateData[g_CurrentState].GetWindowSize!=NULL)
			wValue=stateData[g_CurrentState].GetWindowSize();
		switch(g_CurrentState)
		{
			case State_OutputHz_NumHz:
				NumKeySetValue(KEY_UP,g_CurrentState);
				InputNumReset();
				DisCurWinSizeValue(g_OutputHzVal,0x90,'*');
				break;
			case State_ZoomPara_Input_Format:
				LCDDisplayWhiceImage();
				DisInputSrcFormatStr(0x91);
				break;	
			case State_ZoomPara_Size_Value:
			case State_ZoomPara_Pos_Value:	
			case State_ScalerPara_Size_Value:			
			case State_ScalerPara_Pos_Value:
				LCDDisplayWhiceImage();
			case State_ScreenPara_Size_Value:
			case State_ScreenPara_Pos_Value:
				DisplayStepValue(STEPADDR);	
			case State_ScreenPara_UserResolution_Value:
				if(!NumKeySetValue(KEY_UP,g_CurrentState))
				{
					if(stateData[g_CurrentState].ExecFunction!=NULL)
						stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0);
				}
				else
					SetCurrentStateValue(KEY_ENTER,g_CurrentState, 0);
				InputNumReset();
				LCDDisplayrHVValue();
				if(g_PreState!=g_CurrentState)
				{
					g_H_VFlag=__TRUE;
					DisplayHV_();
				}
				else if(g_H_VFlag==__TRUE)
				{
					g_H_VFlag=__FALSE;
					DisplayHV_();
				}
				LCDDisplayTabInfo();
				break;
			case State_DualImage_Set_MainSize:
				Main_HV_Idx++;
				Main_HV_Idx%=2;
				if(Main_HV_Idx==1)
				{
					if(NumKeySetValue(KEY_UP,g_CurrentState)==__TRUE)
					{
						InputNumReset();
						LCDDisplayrHVValue();
						g_H_VFlag=__FALSE;
						DisplayHV_();
					}
					else
					{
						Main_HV_Idx++;
					}
				}
				else
				{
					if(NumKeySetValue(KEY_UP,g_CurrentState)==__TRUE)
					{
						InputNumReset();
						LCDDisplayrHVValue();
						g_H_VFlag=__TRUE;
						g_ActiveImageValue=SUB;
						DealCurrentState(State_DualImage_Set_SubSize);
						chinesedisplay(0x91, "_   x");
					}
					else
					{
						Main_HV_Idx++;
					}
				}
				break;
			case State_DualImage_Set_SubSize:
				Sub_HV_Idx++;
				Sub_HV_Idx%=2;
				if(Sub_HV_Idx==1)
				{
					if(NumKeySetValue(KEY_UP,g_CurrentState)==TRUE)
					{
						InputNumReset();
						LCDDisplayrHVValue();
						g_H_VFlag=__FALSE;
						DisplayHV_();
					}
					else
					{
						Sub_HV_Idx++;
					}
				}
				else
				{
					if(NumKeySetValue(KEY_UP,g_CurrentState)==TRUE)
					{
						InputNumReset();
						LCDDisplayrHVValue();
						g_H_VFlag=__TRUE;
					}
					else
					{
						Sub_HV_Idx++;
					}
				}
				break;

			case State_ColorPara_Brightness_Value:
			case State_ColorPara_Contrast_Value:
			case State_ColorPara_Saturation_Value:
			case State_Fuse_Switch_Value:
				InputNumReset();
				DisFourNumValue(wValue,LCDADDR91);
				DisplayStepValue(STEPADDR);
				break;
			case State_FreezeImage_Check:
				LCDDisplayFreezeState();
				break;
		}
	}
	if(g_CurrentState==State_MainMenu_Menu)
	{
		DealMainInfoState();
	}	
}
void DealEscKey()
{
	clearlcd();
	InputNumReset();
	//提示用户热备份已开启
	if(g_CurrentState==State_HotBackup_Check||g_CurrentState==State_User_HotBackup_Continue)
	{
		Write_EEPROM_HotBackupInput(E2ROM_HotBackupInput_ADDR);
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x80,g_HotBackupStartStr);
		else if(g_LanguageFlag==ENGLISH)	
			chinesedisplay(0x80,"Backup opened");
		delay(5);
		clearlcd();
	}
	g_CurrentState=stateData[g_CurrentState].navigationData[NAV_ESC];
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1th);		
		chinesedisplay(stateData[g_CurrentState].startaddr2th, stateData[g_CurrentState].string2th);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1thEn);		
		chinesedisplay(stateData[g_CurrentState].startaddr2th, stateData[g_CurrentState].string2thEn);
	}
	DealMainInfoState();
	StateDisWhichImage();
	
}

void DealVolumeKey()
{
	U8 wValue=50;
	g_CurrentState=State_Volume_Set;
	clearlcd();	
	if(stateData[g_CurrentState].GetWindowSize!=NULL)
		wValue=stateData[g_CurrentState].GetWindowSize();	
	DisCurWinSizeValue(wValue,LCDADDR90,'*');
	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1th);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1thEn);
       //显示步长
	DisplayStepValue(STEPADDR);
}

void DealMuteKey(_AudioPara para)
{
	static U8 muteFlag=0;
	U8 wValue=50;
	g_CurrentState=State_Volume_Set;
	clearlcd();		
	if(muteFlag==0)
	{	
		muteFlag=1;
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x80, g_AudioStr+2);
			chinesedisplay(0x91, g_MuteStr);
		}
		else
		{
			chinesedisplay(0x80, g_AudioStrEn+2);
			chinesedisplay(0x91, g_MuteStrEn);
		}	
		SetAudioPara(para);	
	}
	else
	{
		muteFlag=0;
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x80, g_AudioStr+2);
		}
		else
		{
			chinesedisplay(0x80, g_AudioStrEn+2);
		}
		if(stateData[g_CurrentState].GetWindowSize!=NULL)
		wValue=stateData[g_CurrentState].GetWindowSize();	
	       SetVolumeValue(wValue);

	}

}

void DealFreezeKey(void)
{
		g_CurrentState=State_FreezeImage_Check;
		clearlcd();	
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x80, g_FreezeImage);
		else
			chinesedisplay(0x80, g_FreezeImageEn);
		LCDDealFreezeState();
}

BOOL DealStepKey()
{
//改变步长
	if(IsSetParaValueState()||IsSetColorParaValueState()||g_CurrentState==State_Fuse_Switch_Value||g_CurrentState==State_Volume_Set)
	{
		if(g_StepValue==1)
			g_StepValue=10;
		else if(g_StepValue==10)
			g_StepValue=100;
		else if(g_StepValue==100)
			g_StepValue=1;
				
		DisplayStepValue(STEPADDR);
		return __TRUE;
	}
	return __FALSE;
}

void DealTakeKey()
{	
	if(g_MutiWindow==SINGLEIMAGE)
	{
		ChangeInputSrc();
	}
	else
	{
		SwapDualInputSrc();
		if(g_CurrentState!=State_NewMode_Set)
			g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		if(g_SwitchEffectValue==FadeSwitchIdx)
		{
			chinesedisplay(0x80, g_FadeCharacter);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x92, g_FadeInFadeOutStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_SwitchInputModeStrEn);
			if(g_NextPointImage==SUB)
			{
				DisplayLCDInputSrc(0x81,"-->     ");
				delay(1);
				DisplayLCDInputSrc(0x81," -->    ");
				delay(1);
				DisplayLCDInputSrc(0x81,"  -->   ");
				delay(1);
				DisplayLCDInputSrc(0x81,"   -->  ");
				delay(1);
				DisplayLCDInputSrc(0x81,"    --> ");
				delay(1);
				DisplayLCDInputSrc(0x81,"     -->");
				g_NextPointImage=MAIN;
			}
			else
			{
				DisplayLCDInputSrc(0x81,"     <--");
				delay(1);
				DisplayLCDInputSrc(0x81,"    <-- ");
				delay(1);
				DisplayLCDInputSrc(0x81,"   <--  ");
				delay(1);
				DisplayLCDInputSrc(0x81,"  <--   ");
				delay(1);
				DisplayLCDInputSrc(0x81," <--    ");
				delay(1);
				DisplayLCDInputSrc(0x81,"<--     ");
				g_NextPointImage=SUB;
			}
			
		}
		else if(g_SwitchEffectValue==NormalSwitchIdx||g_SwitchEffectValue==FuseSwitchIdx)
		{
			_InputSourceType inputSwap;
			inputSwap=g_MainInput;
			g_MainInput=g_SubInput;
			g_SubInput=inputSwap;
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x92, g_SwapImageStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x92, "Swap Image");
			if(g_SwitchEffectValue==NormalSwitchIdx)
				chinesedisplay(0x80, g_DualCharacter);
			else
				chinesedisplay(0x80, g_FuseCharacter);				
			DisplayLCDInputSrc(0x81,"  +  ");
		}
		else if(g_SwitchEffectValue==QuickSwitchIdx)
		{
			chinesedisplay(0x80, g_QuickCharacter);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x92, g_QuickSwitchStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, g_QuickSwitchStrEn);
			if(g_NextPointImage==SUB)
			{
				DisplayLCDInputSrc(0x81,"     -->");
				g_NextPointImage=MAIN;
			}
			else
			{
				DisplayLCDInputSrc(0x81,"<--     ");
				g_NextPointImage=SUB;
			}
		}
		SetCVBSMain();
	}
}

void DealRecallKey()
{
	g_CurrentState=State_UserPara_Number;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(LCDADDR80, g_RecallModeStr+2);
		chinesedisplay(0x84, g_MainMenuNum);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(LCDADDR80, g_RecallModeStrEn+2);
		chinesedisplay(0x90, g_MainMenuNumEn);
	}
}


void DealSaveKey()
{
	g_CurrentState=State_SavePara_Number;	
					
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(LCDADDR80, g_SaveModeStr+2);
		chinesedisplay(0x84, g_MainMenuNum);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(LCDADDR80,g_SaveModeStrEn+2);
		chinesedisplay(0x84, g_MainMenuNumEn);
	}
}

void DealBackspaceKey(void)
{
	if(IsSetParaValueState())	
	{
		if(g_H_VFlag==__TRUE)
			BackSpaceNumber(LCDADDR91,4);
		else
			BackSpaceNumber(0x94,4);
	}
	else 
	{
		switch(g_CurrentState)
		{
		#ifdef TCP_IP
			case State_UserPara_SetStaticIP_Value:
				switch(g_StaticIPIdx)
				{
					case IP1_IDX:
						BackSpaceNumber(0x90,3);
						break;
					case IP2_IDX:
						BackSpaceNumber(0x92,3);
						break;
					case IP3_IDX:
						BackSpaceNumber(0x94,3);
						break;
					case IP4_IDX:
						BackSpaceNumber(0x96,3);
						break;
				}
				break;
		#endif
			case State_Input_PackagingName:
			case State_Input_TestName:
			case State_Input_SaleName:
			case State_User_Input_Logo:
				BackSpaceNumber(LCDADDR91,5);
				break;
			case State_User_Input_TryDays:
				BackSpaceNumber(LCDADDR91,3);
				break;
			case State_User_Set_TelNumber:
				BackSpaceNumber(LCDADDR91,11);
				break;	
			case State_User_Input_MathineType:
				BackSpaceNumber(LCDADDR91,10);
				break;				
			case State_User_Input_SerialNumber:
			case State_DebugMode_Start:
			case State_User_Old_SerialNumber:
			case State_User_Set_SerialNumber:
				BackSpaceNumber(LCDADDR91,6);
				break;
			case State_LockPadTime_Set:
				BackSpaceNumber(LCDADDR91,2);
				break;
			case State_NewMode_Set_HSize:
			case State_NewMode_Set_VSize:
				BackSpaceNumber(0x97,4);
				break;
			case State_Input_YEAR:
				BackSpaceNumber(0x83,4);
				break;
			case State_Input_MONTH:
			case State_Input_DOM:
			case State_Input_HOUR:
			case State_Input_MIN:
			case State_Input_SEC:
				BackSpace();
				break;
			case State_ScheduleInput1_InputWeek:
			case State_ScheduleInput2_InputWeek:
			case State_ScheduleInput3_InputWeek:
				BackScheWeekNum();
				break;
			default:
				DealActiveImageKey();
				break;
			
		}
	}		
		
}

void SetScreenFULL(U8 image)
{
	SetScalerHVValue(HVSIZE, image);
	SetScalerHVValue(HVPOS, image);
}

void SetScreenPART(U8 image)
{
	SetPanelScalerHVValue(HVPOS, image);
	SetPanelScalerHVValue(HVSIZE, image);
}

