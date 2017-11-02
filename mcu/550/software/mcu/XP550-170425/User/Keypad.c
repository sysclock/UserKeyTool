/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: keypad.c
 [Date]:        09-May-2012
 [Comment]:
   keypad subroutines.
 [Reversion History]:
*******************************************************************************/

#define _KEYPAD_C_

#include "Header.h"
#include "strings.h"
#include <stdio.h>
#include "remote.h"

//extern struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;
//int regcnt=100; //计数寄存器 静态变量
extern U8 g_initPIPSrc;

U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1] =
{
        //MAIN->   AV1, AV2,AV3,AV4, DVI, YPBPR,VGA,VGA2,HDMI ,SDI,DP
//SUB
/*										AV1	 AV2  AV3   AV4     DVI1   DVI2   YBR   VGA      VGA2   HDMI    SDI   USB    DP	 */
/*MST_AV1    */    {   1,  0,    1,    0,      1,     1,     1,    1,       1,     1,     1,     1,     1},
/*MST_AV2    */    {   0,  1,    1,    0,      1,     1,     1,    1,       1,     1,     1,     1,     1},
/*MST_AV3    */    {   1,  1,    1,    0,      1,     0,     1,    1,       0,     1,     1,     0,     1},
/*MST_AV4    */    {   0,  0,    0,    0,      0,     0,     0,    0,       0,     0,     0,		 1,     1},
/*DVI       */     {   1,  1,    1,    1,      1,     1,     1,    1,       1,     0,     1,     1,     1},
/*DVI2      */		 {   1,  1,    0,    1,      1,     1,     1,    1,       0,     1,     1,		 0,			1},
/*YPBPR     */     {   1,  1,    1,    1,      0,     1,     0,    0,       0,     1,     1,     1,     1},
/*VGA       */     {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1,     1,     0},
/*VGA2       */    {   1,  1,    0,    1,      1,     0,     2,    1,       1,     1,     1,		 0,			1},
/*HDMI     */      {   1,  1,    1,    1,      0,     1,     1,    1,       1,     1,     1,		 1,     1},
/*SDI*/            {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1,		 1,			1},
/*USB      */			 {   1,  1,    0,    1,      1,     0,     1,    1,       0,     1,     1,		 1,			1},
/*DP*/             {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1,		 1,			1},
};
		
// U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1] =
// {
//         //MAIN->   AV1, AV2,AV3,AV4, DVI, YPBPR,VGA,VGA2,HDMI ,SDI,DP
// //SUB

// /*MST_AV1    */    {   1,  0,    0,    0,      1,     1,     1,    1,       1,     1,     1},
// /*MST_AV2    */    {   0,  1,    0,    0,      1,     1,     1,    1,       1,     1,     1},
// /*MST_AV3    */    {   0,  0,    1,    0,      1,     1,     1,    1,       1,     1,     1},
// /*MST_AV4    */    {   0,  0,    0,    1,      1,     1,     1,    1,       1,     1,     1},
// /*DVI       */     {   1,  1,    1,    1,      1,     0,     1,    1,       0,     1,     1},
// /*YPBPR     */     {   1,  1,    1,    1,      0,     1,     0,    0,       0,     1,     1},
// /*VGA       */     {   1,  1,    1,    1,      1,     0,     1,    0,       1,     1,     1},
// /*VGA2       */    {   1,  1,    1,    1,      1,     0,     0,    1,       1,     1,     1},
// /*HDMI     */      {   1,  1,    1,    1,      0,     0,     1,    1,       1,     1,     1},
// /*SDI*/            {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1},
// /*DP*/             {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1},
// };

void PanelToZoomPara()
{
	g_MainZoomHSize=0;
	g_MainZoomVSize=0;
	g_MainZoomHPos=0;
	g_MainZoomVPos=0;

	g_SubZoomHSize=0;
	g_SubZoomVSize=0;
	g_SubZoomHPos=0;
	g_SubZoomVPos=0;

	g_MainVGAZoomHSize=0;
	g_MainVGAZoomVSize=0;
	g_MainVGAZoomHPos=0;
	g_MainVGAZoomVPos=0;

	g_SubVGAZoomHSize=0;
	g_SubVGAZoomVSize=0;
	g_SubVGAZoomHPos=0;
	g_SubVGAZoomVPos=0;

	g_MainDVIZoomHSize=0;
	g_MainDVIZoomVSize=0;
	g_MainDVIZoomHPos=0;
	g_MainDVIZoomVPos=0;

	g_SubDVIZoomHSize=0;
	g_SubDVIZoomVSize=0;
	g_SubDVIZoomHPos=0;
	g_SubDVIZoomVPos=0;
	//HDMI
	g_MainHDMIZoomHSize=0;
	g_MainHDMIZoomVSize=0;
	g_MainHDMIZoomHPos=0;
	g_MainHDMIZoomVPos=0;

	g_SubHDMIZoomHSize=0;
	g_SubHDMIZoomVSize=0;
	g_SubHDMIZoomHPos=0;
	g_SubHDMIZoomVPos=0;
	
	g_MainVGA2ZoomHSize=0;
	g_MainVGA2ZoomVSize=0;
	g_MainVGA2ZoomHPos=0;
	g_MainVGA2ZoomVPos=0;

	g_SubVGA2ZoomHSize=0;
	g_SubVGA2ZoomVSize=0;
	g_SubVGA2ZoomHPos=0;
	g_SubVGA2ZoomVPos=0;

	g_MainDVI2ZoomHSize=0;
	g_MainDVI2ZoomVSize=0;
	g_MainDVI2ZoomHPos=0;
	g_MainDVI2ZoomVPos=0;

	g_SubDVI2ZoomHSize=0;
	g_SubDVI2ZoomVSize=0;
	g_SubDVI2ZoomHPos=0;
	g_SubDVI2ZoomVPos=0;
}

void PanelToScreenPara()
{
	g_H_VFlag=__TRUE;
	g_ScreenHSize=g_PanelWidth;
	g_ScreenVSize=g_PanelHeight;
	g_ScreenHPos=0;
	g_ScreenVPos=0;
	
	if(g_MutiWindow==SINGLEIMAGE||g_MutiWindow==PIPIMAGE||g_MutiWindow==SAMEPIPIMAGE)
	{
		g_MainScalerHSize=g_PanelWidth;
		g_MainScalerVSize=g_PanelHeight;
		g_MainScalerHPos=0;
		g_MainScalerVPos=0;
	}
	else if(g_MutiWindow==PBPIMAGE)
	{
		g_MainScalerHSize=g_PanelWidth/2;
		g_MainScalerVSize=g_PanelHeight;
		g_MainScalerHPos=0;
		g_MainScalerVPos=0;
		g_SubScalerHSize=g_PanelWidth/2;
		g_SubScalerVSize=g_PanelHeight;
		g_SubScalerHPos=g_PanelWidth/2;
		g_SubScalerVPos=0;
	}
	if(g_MutiWindow==PIPIMAGE)
	{
		g_SubScalerHSize=g_PanelWidth/3;
		g_SubScalerVSize=g_PanelHeight/3;
		g_SubScalerHPos=0;
		g_SubScalerVPos=0;	
	}
	else if(g_MutiWindow==SAMEPIPIMAGE)
	{
		g_SubScalerHSize=g_PanelWidth;
		g_SubScalerVSize=g_PanelHeight;
		g_SubScalerHPos=0;
		g_SubScalerVPos=0;	
	}
	PanelToZoomPara();
	

}
void initColorVariable()
{
	g_BrightnessValue=DEF_BRIGHTNESS;
	g_ContrastValue=DEF_CONTRACT;
	g_SaturationValue=DEF_SATURATION;
/*	g_OffsetR=DEF_OFFSET_RED;
	g_OffsetG=DEF_OFFSET_GREEN;
	g_OffsetB=DEF_OFFSET_BLUE;
	g_GainR=DEF_GAIN_RED;
	g_GainG=DEF_GAIN_GREEN;
	g_GainB=DEF_GAIN_BLUE;*/
}



void initCycleInputPara()
{	
	g_CycleInput1=INPUTSOURCE_NONE;
	g_CycleInput2=INPUTSOURCE_NONE;
	g_CycleInput3=INPUTSOURCE_NONE;
	g_CycleInput4=INPUTSOURCE_NONE;
	g_CycleInput5=INPUTSOURCE_NONE;
	g_CycleInput6=INPUTSOURCE_NONE;
	g_CycleInputHour1=0xff;
	g_CycleInputHour2=0xff;
	g_CycleInputHour3=0xff;
	g_CycleInputHour4=0xff;
	g_CycleInputHour5=0xff;
	g_CycleInputHour6=0xff;
	g_CycleInputMin1=0xff;
	g_CycleInputMin2=0xff;
	g_CycleInputMin3=0xff;
	g_CycleInputMin4=0xff;
	g_CycleInputMin5=0xff;
	g_CycleInputMin6=0xff;

}

void initHotBackupVariable(BOOL flag)
{
	int i;
	g_EnableHotBackupFlag=flag;
	g_SetBackupInputFlag=__FALSE;
	g_HotBackupIdx=0;
	for(i=0;i<8;i++)  //判断当前输入的信号源是否已设置备份，是就返回
	{
		g_HotBackupInput[i][0]=INPUTSOURCE_NONE;
		g_HotBackupInput[i][1]=INPUTSOURCE_NONE;
	}
}

void initGlobalVar()
{

#ifdef POWER_ON_PWD
		//g_PowerOnPwdOk=__FALSE;
		//g_PowerOnPwdFlag=__FALSE;
		g_howlongtosave=5;//15;
		g_NumSaved=0;
#endif


	g_RTCErrorFlag=0;
	g_WizadFlag=__FALSE;
	g_Deadline7DaysFlag=0xFF;
	g_UserSetPasswordVal=1000000;
	g_TeleNumVal=0;
	g_TechTeleNumVal=0;
	
#ifdef INT_DELAY_PC_CTL
	g_PCSendFlag=__FALSE;
	g_OneSecondISRVal=0;
#endif
	g_ComTypeFlag=Communication_NONE;

	g_StaticIPIdx=IP1_IDX;
	g_PCControlFlag=__FALSE;
//	g_MutiWindow=SAMEPIPIMAGE;
	g_AlphaValue=7;
	g_AdvertisingFlag=__FALSE;
	g_BothSwitchFlag=__FALSE;
	g_HadAdcCalibrationFlag=__FALSE;
	
//	g_OKCancelLedFlashingFlag=__FALSE;
	g_FlashLedFlag=__FALSE;
	g_PoweronFlag=__FALSE;
	initHotBackupVariable(__FALSE);
	
	g_StartCycleInputFlag=__FALSE;
	initCycleInputPara();
	g_CycleInputStartHour=0xff;
	g_CycleInputStartMin=0xff;
	g_InputCycleIdx=0;
	
	g_FadeTime=2;
	g_Fli32626HRevision=6;
	g_Fli32626LRevision=1;
	g_550subHRevision=1;
	g_550subLRevision=0;
	g_ReceMsgFlag=__FALSE;
	g_TryTimeEndFlag=__FALSE;
	g_NextPointImage=SUB;
	g_SwitchEffectValue=NormalSwitchIdx;
	g_LockKeyFlag=UNLOCKKEYPAD;
	g_LockKeypadTime=10;
	g_LanguageFlag=CHINESE;
	g_OutputHzVal=60;
	g_ScheduleInput1=INPUTSOURCE_NONE;
	g_ScheduleInput2=INPUTSOURCE_NONE;
	g_ScheduleInput3=INPUTSOURCE_NONE;

	g_ScheduleInputMode1=ONCESCHE;
	g_ScheduleInputMode2=ONCESCHE;
	g_ScheduleInputMode3=ONCESCHE;
	
	g_OutputFormatIndex=INDEX1024X768;
	g_StepValue=1;
	g_MutiWindow=SINGLEIMAGE;
	g_FullOrPartValue=FULL;
	g_VGAFullOrPartValue=FULL;
	g_DVIFullOrPartValue=FULL;
	g_HDMIFullOrPartValue=FULL;
	g_OtherFullOrPartValue=FULL;
	g_VGA2FullOrPartValue=FULL;
	g_DVI2FullOrPartValue=FULL;
	g_ActiveImageValue=MAIN;
	g_ActiveAudioWin=MAIN;
	g_VolumeValue=100;    
	g_MainFreezeFlag=UNFREEZE;
	g_SubFreezeFlag=UNFREEZE;
	
	g_PanelWidth=1024;
	g_PanelHeight=768;
	g_MainInput=INIMAINSRC;
	g_SubInput=INITPIPSRC;
	g_powerOnNoChangePartofFullFlag=TRUE;
	g_TryEndFlag=TRUE;

	initColorVariable();	
	PanelToScreenPara();


	#ifdef DEBUG_ZOOM_YJL
		g_debug_zoom=0;
	#endif

	//As without init CurrentState,after RST machine,g_correntsatate is 137-guideWizard when boost,and leads cannot zoom at first
	//20170425 yjl
	//g_CurrentState = 0;
	
}



BOOL IsUsbState()//判断当前是否为USB状态
{
	switch(g_CurrentState)
	{
		case State_MainMenu_Menu:
    case State_MainMenu_Not_Menu:
                return __TRUE;
		default:
                return __FALSE;
	}
}
BOOL IsSetParaValueState()
{
	switch(g_CurrentState)
	{
		case State_ScreenPara_Size_Value:
		case State_ScreenPara_Pos_Value:
		case State_ScreenPara_UserResolution_Value:
		case State_ZoomPara_Size_Value:
		case State_ZoomPara_Pos_Value:					
		case State_ScalerPara_Size_Value:
		case State_ScalerPara_Pos_Value:	
		case State_DualImage_Set_MainSize:
		case State_DualImage_Set_SubSize:
			return __TRUE;
		default:
			return __FALSE;
	}
}
BOOL IsSetColorParaValueState()
{
	switch(g_CurrentState)
	{
		case State_ColorPara_Brightness_Value:				
		case State_ColorPara_Contrast_Value:
		case State_ColorPara_Saturation_Value:
/*		case State_ColorPara_Brightness_R_Value:
		case State_ColorPara_Brightness_G_Value:
		case State_ColorPara_Brightness_B_Value:	
		case State_ColorPara_Contrast_R_Value:
		case State_ColorPara_Contrast_G_Value:
		case State_ColorPara_Contrast_B_Value:*/
			return __TRUE;
		default:
			return __FALSE;
	}
}
U8 CheckDualSource(U8 InputSource1, U8 InputSource2)
{
    return tPIPSameSourceMatch[InputSource1][InputSource2];
}

void LCDDisplayPCControl()
{
	clearlcd();
	g_CurrentState=State_MainMenu_Menu;
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80,  g_PCControlStr);
		chinesedisplay(0x90,  g_CurrentTimeStr);	
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80,  "PC Control!");
		chinesedisplay(0x90,  g_CurrentTimeStrEn);	
	}
	SetKeyLEDLight(KEY_NUM);
}


void DisplayMainInput(U8 addr,_InputSourceType input)
{
	U8 inputSrc1[6];
	switch(input)
	{
		case INPUTSOURCE_CVBS:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='1';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS2:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='2';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS3:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='3';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS4:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='4';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA2:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='2';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_DVI:
			inputSrc1[0]='D';
			inputSrc1[1]='V';
			inputSrc1[2]='I';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_HDMI:
			inputSrc1[0]='H';
			inputSrc1[1]='D';
			inputSrc1[2]='M';
			inputSrc1[3]='I';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_YPBPR:
			inputSrc1[0]='Y';
			inputSrc1[1]='P';
			inputSrc1[2]='b';
			inputSrc1[3]='P';
			inputSrc1[4]='r';
			inputSrc1[5]='\0';
			break;
		case INPUTSOURCE_DVI2:
				inputSrc1[0]='D';
				inputSrc1[1]='V';
				inputSrc1[2]='I';
				inputSrc1[3]='2';
				inputSrc1[4]='\0';
				break;
		case INPUTSOURCE_USB:
			inputSrc1[0]='U';
			inputSrc1[1]='S';
			inputSrc1[2]='B';
			inputSrc1[3]='\0';
			break;
	}
	chinesedisplay(addr,inputSrc1);
}

void DisplayLCDInputSrc(U8 addr,U8 *chInter)
{	
	U8 inputSrc1[6];
	DisplayMainInput(addr,g_MainInput);
	if(g_MutiWindow!=SINGLEIMAGE)
	{		
		switch(g_SubInput)
		{
		case INPUTSOURCE_CVBS:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='1';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS2:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='2';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS3:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='3';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS4:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='4';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA2:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='2';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_DVI:
			inputSrc1[0]='D';
			inputSrc1[1]='V';
			inputSrc1[2]='I';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_HDMI:
			inputSrc1[0]='H';
			inputSrc1[1]='D';
			inputSrc1[2]='M';
			inputSrc1[3]='I';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_YPBPR:
			inputSrc1[0]='Y';
			inputSrc1[1]='P';
			inputSrc1[2]='b';
			inputSrc1[3]='P';
			inputSrc1[4]='r';
			inputSrc1[5]='\0';
			break;
			case INPUTSOURCE_DVI2:
				inputSrc1[0]='D';
				inputSrc1[1]='V';
				inputSrc1[2]='I';
				inputSrc1[3]='2';
				inputSrc1[4]='\0';
				break;
			case INPUTSOURCE_USB:
				inputSrc1[0]='U';
				inputSrc1[1]='S';
				inputSrc1[2]='B';
				inputSrc1[3]='\0';
				break;
		}	
		printStr(chInter);
		printStr(inputSrc1);
	}
}

static _InputSourceType TwoMode[2]={INPUTSOURCE_NUM,INPUTSOURCE_DVI};          //two mode recall


void LightInputSrcLED()
{
	U32 led1_32=0x01<<KEY_NUM;	
	//if(g_MutiWindow==SAMEPIPIMAGE)
	switch(g_MainInput)
	{
		case INPUTSOURCE_CVBS:
			led1_32=0x01<<CVBS_KEY;
			break;
		case INPUTSOURCE_CVBS2:
			led1_32=0x01<<CVBS2_KEY;
			break;		
	#if(MATHINE_TYPE==XP_520)
//		case INPUTSOURCE_CVBS3:
//			led1_32=0x01<<CVBS3_KEY;
//			break;
//		case INPUTSOURCE_YPBPR:
//			led1_32=0x01<<YPBPR_KEY;
//			break;
//		case INPUTSOURCE_SDI:
//			led1_32=0x01<<SDI_KEY;
//			break;
	#elif(MATHINE_TYPE==XP_530)
		case INPUTSOURCE_SDI:
			switch(g_Adv7441CurrentInput)//(pVideoStat->inputPort)
			{
				case DEVICE_PORT_CVBS1:
					led1_32=0x01<<NUM6_KEY;
					break;
				case DEVICE_PORT_CVBS2:
					led1_32=0x01<<NUM7_KEY;
					break;
				case DEVICE_PORT_VGA:
					led1_32=0x01<<NUM8_KEY;
					break;
				case DEVICE_PORT_DVI:
					led1_32=0x01<<NUM9_KEY;
					break;
				case DEVICE_PORT_HDMI:
					led1_32=0x01<<NUM0_KEY;
					break;
			}
			break;
	#endif
		case INPUTSOURCE_VGA:
			led1_32=0x01<<VGA_KEY;
			break;
		case INPUTSOURCE_DVI:
			led1_32=0x01<<DVI_KEY;
			break;
		case INPUTSOURCE_HDMI:
			led1_32=0x01<<HDMI_KEY;
			break;
	}
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		switch(g_SubInput)
		{
		case INPUTSOURCE_CVBS:
			led1_32|=0x01<<CVBS_KEY;
			break;
		case INPUTSOURCE_CVBS2:
			led1_32|=0x01<<CVBS2_KEY;
			break;
		
	#if(MATHINE_TYPE==XP_520)
/*		case INPUTSOURCE_CVBS3:
			led1_32|=0x01<<CVBS3_KEY;
			break;
		case INPUTSOURCE_YPBPR:
			led1_32|=0x01<<YPBPR_KEY;
			break;
		case INPUTSOURCE_SDI:
			led1_32|=0x01<<SDI_KEY;
			break; */
	#elif(MATHINE_TYPE==XP_530)
		case INPUTSOURCE_SDI:
			switch(g_Adv7441CurrentInput)//(pVideoStat->inputPort)
			{
				case DEVICE_PORT_CVBS1:
					led1_32|=0x01<<NUM6_KEY;
					break;
				case DEVICE_PORT_CVBS2:
					led1_32|=0x01<<NUM7_KEY;
					break;
				case DEVICE_PORT_VGA:
					led1_32|=0x01<<NUM8_KEY;
					break;
				case DEVICE_PORT_DVI:
					led1_32|=0x01<<NUM9_KEY;
					break;
				case DEVICE_PORT_HDMI:
					led1_32|=0x01<<NUM0_KEY;
					break;
			}
			break;
	#endif
		case INPUTSOURCE_VGA:
			led1_32|=0x01<<VGA_KEY;
			break;
		case INPUTSOURCE_DVI:
			led1_32|=0x01<<DVI_KEY;
			break;
		case INPUTSOURCE_HDMI:
			led1_32|=0x01<<HDMI_KEY;
			break;
		}
	}
	led1_32=~led1_32;
	SetKeypadLED(led1_32);
}
	
void InitSwapFisrtInput()
{
	TwoMode[1]=g_MainInput;
}

void CannotSetDualChannelTip()
{
	g_CurrentState=State_MainMenu_Not_Menu;
	clearlcd();
	if(g_LanguageFlag==CHINESE)	
	{
		chinesedisplay(0x82, g_WarnningStr);	
		chinesedisplay(0x90, g_ErrorDualInputStr);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x82, g_WarnningStrEn);	
		chinesedisplay(0x90, g_ErrorDualInputStrEn);
	}
}

void LCDDisplayEffectInfo(void)
{			
	if(g_SwitchEffectValue==FadeSwitchIdx)
	{
		chinesedisplay(0x80, g_FadeCharacter);
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x92, g_FadeInFadeOutStr);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_SwitchInputModeStrEn);
	}
	else if(g_SwitchEffectValue==QuickSwitchIdx)
	{
		chinesedisplay(0x80, g_QuickCharacter);
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x92,g_QuickSwitchStr);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, g_QuickSwitchStrEn);
	}
	else if(g_SwitchEffectValue==FuseSwitchIdx)
	{
		chinesedisplay(0x80, g_FuseCharacter);
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x92,g_FuseSwitchStr);
		else if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x91, "Fuse Mode");
	}
}

void SetCVBSMain(void)
{		
	if(g_MutiWindow==SAMEPIPIMAGE&&g_NextPointImage==MAIN&&(g_SubInput==INPUTSOURCE_CVBS||g_SubInput==INPUTSOURCE_CVBS2
		||g_SubInput==INPUTSOURCE_CVBS3||g_SubInput==INPUTSOURCE_CVBS4
	#if(MATHINE_TYPE==XP_530)
		||(g_SubInput==INPUTSOURCE_SDI&&(g_Adv7441CurrentInput==DEVICE_PORT_CVBS1||g_Adv7441CurrentInput==DEVICE_PORT_CVBS2))
	#endif
		))
	{
		_InputSourceType preMainInput=g_MainInput;
		SetInputSource(g_SubInput,MAIN);
		g_MainInput=g_SubInput;
		delay(8);
		g_NextPointImage=SUB;
		SetWhichImageOnTop(MAIN);	
		SetInputSource(preMainInput,SUB);					
		clearLineFromAddr(0x81);
		DisplayLCDInputSrc(0x81,"<--     ");			
	}
}

extern int gAD7441_InputPort;

void SetInputSourceSwitch(_InputSourceType input)
{	
	static U8 modeIndex=0;
	Read_EEPROM_PcPart();
// 	DisplayOneInt(0x97,g_VGAFullOrPartValue);DisplayOneInt(0x98,g_DVIFullOrPartValue);DisplayOneInt(0x99,g_HDMIFullOrPartValue);

	
	if(g_CurrentState!=State_NewMode_Set)
		g_CurrentState=State_MainMenu_Not_Menu;
	if(g_SwitchEffectValue==NormalSwitchIdx)
	{
		clearlcd();

		//单画面硬切换
		if(g_MutiWindow==SINGLEIMAGE)
		{
			chinesedisplay(0x80, g_SingleCharacter);
			if(g_MainInput!=input)
				TwoMode[modeIndex++%2]=input;
			
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x92,g_SingleSwitchStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x90, "Switch Main Input");
		}
		else if((g_ActiveImageValue==MAIN&&!CheckDualSource(g_SubInput-1, input-1))
			||(g_ActiveImageValue==SUB)&&!CheckDualSource(g_MainInput-1, input-1))
		{
			if(g_BothSwitchFlag==__FALSE)
			{
				CannotSetDualChannelTip();
				return;		
			}
			else
			{
				g_MainInput=INPUTSOURCE_SDI;
				SetInputSource(g_MainInput,MAIN);
			}
		}
		
		else
		{
			chinesedisplay(0x80, g_DualCharacter);
			if(g_ActiveImageValue==MAIN)
			{
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x92,g_MainSwitchStr);
				else if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x90, "Switch Main Input");
			}
			else
			{
				if(g_LanguageFlag==CHINESE)
					chinesedisplay(0x92,g_SubSwitchStr);
				else if(g_LanguageFlag==ENGLISH)
					chinesedisplay(0x90, "Switch Sub Input");
			}
		}

		//双画面相同时候同时切换
		if(g_BothSwitchFlag==__TRUE&&g_MutiWindow!=SINGLEIMAGE)
		{			
			SetInputSource(input,SUB);
			SetInputSource(input,MAIN);
			
			clearlcd();
			chinesedisplay(0x80, g_DualCharacter);
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x92,g_BothSwithStr+6);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x92, "Both Switch");
		}
		else
			SetInputSource(input,g_ActiveImageValue);
		
		DisplayLCDInputSrc(0x81," +  ");
	}

	//融合模式
	else if(g_SwitchEffectValue==FuseSwitchIdx)
	{	
	#if(MATHINE_TYPE==XP_530)
		adv7441_input=g_Adv7441CurrentInput;
	#endif
		if(g_SubInput==input||g_MainInput==input) //如果与当前任一输入源相同
		{			
			DisplayLCDInputSrc(0x81," +  ");
			return;
		}
		else if((g_ActiveImageValue==MAIN&&!CheckDualSource(g_SubInput-1, input-1))
			||(g_ActiveImageValue==SUB)&&!CheckDualSource(g_MainInput-1, input-1))
		{
			CannotSetDualChannelTip();
			return;
		}
		SetInputSource(input,g_ActiveImageValue);
		clearlcd();
		DisplayLCDInputSrc(0x81," +  ");
		LCDDisplayEffectInfo();
	}

	//淡入淡出切换  
	else if(g_SwitchEffectValue==FadeSwitchIdx||g_SwitchEffectValue==QuickSwitchIdx)
	{		
		if(g_NextPointImage==MAIN)
		{
			if(g_SubInput==input) //如果与当前输入源相同
			{	
			#if(MATHINE_TYPE==XP_530)
				if(adv7441_input!=g_Adv7441CurrentInput)
				{
					adv7441_input=g_Adv7441CurrentInput;
					Save(DEFAULT_MODE);	
					clearlcd();
					DisplayLCDInputSrc(0x81,"     -->");
					LCDDisplayEffectInfo();
				}
			#endif
			}

			//
			else if(CheckDualSource(g_SubInput-1, input-1))  
			{
				if(input==g_MainInput)//如果与当前预备输入源相同
				{
				#if(MATHINE_TYPE==XP_530)
					if(input==INPUTSOURCE_SDI&&adv7441_input!=g_Adv7441CurrentInput)
					{
						adv7441_input=g_Adv7441CurrentInput;
						delay(5);
					}
				#endif
					DealTakeKey();
				}
				else
				{
					SetInputSource(input,MAIN);
					clearlcd();
					DisplayLCDInputSrc(0x81,"     -->");
					LCDDisplayEffectInfo();
					delay(5);
				#if(MATHINE_TYPE==XP_530)
					if(input==INPUTSOURCE_SDI&&adv7441_input!=g_Adv7441CurrentInput)
					{
						adv7441_input=g_Adv7441CurrentInput;
					}
				#endif
					DealTakeKey();
				}
			}

			else//执行双画面硬切换
			{
			#if(MATHINE_TYPE==XP_530)
				adv7441_input=g_Adv7441CurrentInput;
			#endif
				if(!CheckDualSource(g_MainInput-1, input-1))
				{
					LEDWarning(InputSrcToNumKey(input));
					return;	//此处为YPbPr与主子画面都不能双画面
				}
				else
				{
					SetInputSource(input,SUB);
					clearlcd();
					DisplayLCDInputSrc(0x81,"     -->");
					LCDDisplayEffectInfo();
				}
			}
		}


		//
		else
		{
			if(g_MainInput==input)//如果与当前输入源相同
			{
			#if(MATHINE_TYPE==XP_530)
				if(adv7441_input!=g_Adv7441CurrentInput)
				{
					adv7441_input=g_Adv7441CurrentInput;
					Save(DEFAULT_MODE);	
					clearlcd();
					DisplayLCDInputSrc(0x81,"<--     ");
					LCDDisplayEffectInfo();
				}
			#endif
			}
			else if(CheckDualSource(g_MainInput-1, input-1))
			{
				if(input==g_SubInput)//如果与当前预备输入源相同
				{
				#if(MATHINE_TYPE==XP_530)
					if(input==INPUTSOURCE_SDI&&adv7441_input!=g_Adv7441CurrentInput)
					{
						adv7441_input=g_Adv7441CurrentInput;
						delay(5);
					}
				#endif
					DealTakeKey();
				}
				else
				{
					SetInputSource(input,SUB);
					clearlcd();
					DisplayLCDInputSrc(0x81,"<--     ");
					LCDDisplayEffectInfo();
					delay(5);
				#if(MATHINE_TYPE==XP_530)
					if(input==INPUTSOURCE_SDI&&adv7441_input!=g_Adv7441CurrentInput)
					{
						adv7441_input=g_Adv7441CurrentInput;
					}
				#endif
					DealTakeKey();
				}
			}
			else//执行双画面硬切换
			{
			#if(MATHINE_TYPE==XP_530)
				adv7441_input=g_Adv7441CurrentInput;
			#endif
				if(!CheckDualSource(g_SubInput-1, input-1))
				{
					LEDWarning(InputSrcToNumKey(input));
					return;	//此处为YPbPr与主子画面都不能双画面
				}
				else
				{
					SetInputSource(input,MAIN);
					clearlcd();
					DisplayLCDInputSrc(0x81,"<--     ");
					LCDDisplayEffectInfo();
				}
			}
		}
		SetCVBSMain();		
	}
	Save(DEFAULT_MODE);//DEFAULT_MODE
}


	
void ChangeInputSrc()
{	 
	if(TwoMode[0]==g_MainInput&&TwoMode[1]!=g_MainInput&&TwoMode[1]<INPUTSOURCE_NUM)
	{
		SetInputSourceSwitch(TwoMode[1]);
	}
	else	if(TwoMode[1]==g_MainInput&&TwoMode[0]!=g_MainInput&&TwoMode[0]<INPUTSOURCE_NUM)
	{
		SetInputSourceSwitch(TwoMode[0]);
	}
	else
	{
		LEDWarning(SWAP_KEY);
		g_CurrentState=State_MainMenu_Not_Menu;

		clearlcd();
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x82, g_WarnningStr);
			chinesedisplay(0x90,g_SingleSwapWarnningStr);
		}
		else if(g_LanguageFlag==ENGLISH)	
		{
			chinesedisplay(0x82, g_WarnningStrEn);
			chinesedisplay(0x90, "Switch and go on");
		}
		delay(5);
		DealEscKey();
	}
}


//这只是一个显示问题，显示keypad中数据结构-节点的字符
void DealCurrentState(State CurState)
{	
	g_CurrentState=CurState;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		if(stateData[g_CurrentState].string1th!=NULL)
			chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1th);
		if(stateData[g_CurrentState].string2th!=NULL)
			chinesedisplay(stateData[g_CurrentState].startaddr2th, stateData[g_CurrentState].string2th);
	}
	else if(g_LanguageFlag==ENGLISH)	
	{
		if(stateData[g_CurrentState].string1thEn!=NULL)
			chinesedisplay(stateData[g_CurrentState].startaddr1th, stateData[g_CurrentState].string1thEn);
		if(stateData[g_CurrentState].string2thEn!=NULL)
			chinesedisplay(stateData[g_CurrentState].startaddr2th, stateData[g_CurrentState].string2thEn);
	}
	
	
	//else if(g_CurrentState==State_UserTry_Once_Password100 ||g_CurrentState==State_UserTry_Once_Password100_new ||g_CurrentState==State_UserTry_Once_Password100_new1)
	 if(g_CurrentState==State_UserTry_Once_Password100_new ||g_CurrentState==State_UserTry_Once_Password100_new1)	
	{
			chinesedisplay(0x90, g_Password100_Str);	
	}
	else if(g_CurrentState== State_UserTry_OnceTel)
	{
			chinesedisplay(0x91,g_Tel_display);	
	}
	
	
}


static void DealSwitchEffect()
{	
	if(g_CurrentState==State_Fade_Switch_Mode)  //淡入淡出
	{
		DealCurrentState(State_Fuse_Switch_Mode);
	}
	else if(g_CurrentState==State_Fuse_Switch_Mode)//融合模式
	{	
		DealCurrentState(State_Quick_Switch_Mode); //快速切换
	}
	else 
	{		
		DealCurrentState(State_Fade_Switch_Mode);  
	}
	chinesedisplay(0x90, "*");
	GetSwitchStateValue();
	
// //	加一条切换目地是改变在淡入淡出的时候，切换画面是反向的情况
//  	SwapDualInputSrc();
	
}
/*
static void DealScreenPara()
{
	switch(g_CurrentState)
	{
		case State_ScalerPara_Size:
		case State_ScalerPara_Pos:
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
			break;
		default:
			g_CurrentState=State_ScalerPara_Size;
			break;
	}
	DealCurrentState(g_CurrentState);	
}
*/
void DisplayStepValue(U8 xPos)
{
	wcom(xPos);
	printStr("±");
	if(g_StepValue==100)
		printStr("100");
	else if(g_StepValue==10)
		printStr("10 ");
	else if(g_StepValue==1)
		printStr("1  ");	
}

/*
void Deal_PIP_Pos(_PIPPos Pos)
{
	U8 PipLT_str[]={0xd7,0xf3,0xc9,0xcf,0xbd,0xc7,0};
	U8 PipLM_str[]={0xd7,0xf3,0xd6,0xd0,0xbc,0xe4,0};
	U8 PipLB_str[]={0xd7,0xf3,0xcf,0xc2,0xbd,0xc7,0};
	U8 PipMC_str[]={0xd6,0xd0,0xd0,0xc4,0};
	U8 PipRT_str[]={0xd3,0xd2,0xc9,0xcf,0xbd,0xc7,0};
	U8 PipRM_str[]={0xd3,0xd2,0xd6,0xd0,0xbc,0xe4,0};
	U8 PipRB_str[]={0xd3,0xd2,0xcf,0xc2,0xbd,0xc7,0};
	switch(Pos)
	{
		case PIP_LT:  //Left Top
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipLT_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Left Top"); 
			g_SubScalerHPos=0;
			g_SubScalerVPos=0;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);			
			break;
		case PIP_LM:	
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipLM_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Left Middle"); 
			g_SubScalerHPos=0;
			g_SubScalerVPos=(g_MainScalerVSize-g_SubScalerVSize)/2;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
		case PIP_LB:		
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipLB_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Left Bottom"); 
			g_SubScalerHPos=0;
			g_SubScalerVPos=g_MainScalerVSize-g_SubScalerVSize;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
		case PIP_MC:		
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipMC_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Center"); 
			g_SubScalerHPos=(g_MainScalerHSize-g_SubScalerHSize)/2;
			g_SubScalerVPos=(g_MainScalerVSize-g_SubScalerVSize)/2;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
		case PIP_RT:		
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipRT_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Right Top"); 
			g_SubScalerHPos=g_MainScalerHSize-g_SubScalerHSize;
			g_SubScalerVPos=0;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
		case PIP_RM:		
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x97, PipRM_str); 
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x97, "Right Middle"); 
			g_SubScalerHPos=g_MainScalerHSize-g_SubScalerHSize;
			g_SubScalerVPos=(g_MainScalerVSize-g_SubScalerVSize)/2;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
		case PIP_RB:		
			if(g_LanguageFlag==CHINESE) 
				chinesedisplay(0x97, PipRB_str);
			else if(g_LanguageFlag==ENGLISH) 
				chinesedisplay(0x97, "Right Bottom"); 
			g_SubScalerHPos=g_MainScalerHSize-g_SubScalerHSize;
			g_SubScalerVPos=g_MainScalerVSize-g_SubScalerVSize;
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(VPOS,g_SubScalerVPos,SUB);		
			break;
	}
	g_CurrentState=State_MultiWindow_PIP;
}
*/

//
void KeypadLockedPrompt()
{
	g_CurrentState=State_Lock_Key;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80, g_LockKeyStr);
		chinesedisplay(0x90, g_LockPromptStr);
	}
	else
	{
		chinesedisplay(0x80, g_LockKeyStrEn);	
		chinesedisplay(0x90, g_LockPromptStrEn);
	}
	delay(5);
	DealEscKey();	
	//20170213 lyg
	 if(g_UserTry_once_onoff)
	{
		display_TrialTimeIs_nHours(0x83);
		//chinesedisplay(0x83,s_try8hours_prompt);
	}
	else if(g_UserTry_168Demo_onoff )
	{
		//chinesedisplay(0x87, "*");
		//chinesedisplay(0x83,s_demo15min_prompt);
		display_DemoTimeIs_nMinutes(0x83);
	}
	
	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x88, g_LockKeyStr+4);
	else
		chinesedisplay(0x88, "LOCK");		
}


//
void KeypadLock()
{	
	g_CurrentState=State_Lock_Key;
	clearlcd();
	g_LockKeyFlag=LOCKKEYPAD;
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x90, g_LockKeyStr);
	else
		chinesedisplay(0x90, g_LockKeyStrEn);	
	delay(5);
	SetKeyLEDLight(KEY_NUM);
	DealEscKey();
	
			//20170213 lyg
	if(g_UserTry_once_onoff)
	{
		display_TrialTimeIs_nHours(0x83);
	}		
	else if(g_UserTry_168Demo_onoff )
	{
		display_DemoTimeIs_nMinutes(0x83);
	}
	
	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x88, g_LockKeyStr+4);
	else
		chinesedisplay(0x88, "LOCK");	
}




BOOL Check_OK_Cancel_State()
{
	switch(g_CurrentState)
	{
//		case State_NewMode_Set:
		case State_FactoryReset_OK:
		case State_FactoryReset_Yes:
		case State_CycleInput_Continue:
		case State_User_HotBackup_Continue:
//			g_OKCancelLedFlashingFlag=__TRUE;
			return __TRUE;
	}
	return __FALSE;
}
/*
void LED_OK_Cancel_Flashing()
{

	g_FlashLedFlag=__TRUE;
	g_FlashLedNum=2;
	g_FlashLedKey[0]=OK_KEY;
	g_FlashLedKey[1]=ESC_KEY;
}*/

void Start_LED_Flashing(U8 num)
{
	g_FlashLedFlag=__TRUE;
	g_FlashLedNum=num;
}



	void ReturnMainWinDontChangeState(void)
	{
		g_AdvertisingFlag=__TRUE;
		clearlcd();
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x80,  g_WelcomeStr);
			chinesedisplay(0x90,  g_CurrentTimeStr);
		}
		else if(g_LanguageFlag==ENGLISH)
		{
			chinesedisplay(0x80,  g_WelcomeStrEn);
			chinesedisplay(0x90,  g_CurrentTimeStrEn);
		}
		//g_CurrentState=State_MainMenu_Menu;
		LightInputSrcLED();
	}


void ReturnMainWin(void)
{	/*
	g_AdvertisingFlag=__TRUE;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80,  g_WelcomeStr);
		chinesedisplay(0x90,  g_CurrentTimeStr);
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80,  g_WelcomeStrEn);
		chinesedisplay(0x90,  g_CurrentTimeStrEn);
	}
	*/
	ReturnMainWinDontChangeState();
	g_CurrentState=State_MainMenu_Menu;
	//LightInputSrcLED();
}


void ScanKeypadForSerial()
{
	static U32 LeisureCount=0;
	int i=0;
	U8 flag=0;
	static _KeypadNum Press_Key=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;
	static BOOL firsttipFlag=__TRUE;
	
	KB_PLOAD_L;	
	for(i=0;i<KEY_NUM;i++)    
	{
		KB_PLOAD_H;
		KB_SCK_L;
		if(KB_SDOUT==0x00)	   //有按键按下
		{ 
			delay1000(0x500);   //去抖
			if(KB_SDOUT==0x00)
			{	
				g_AdvertisingFlag=__FALSE;
				LeisureCount=0;
				firsttipFlag=__TRUE;
				Press_Key=(_KeypadNum)(KEY_NUM-1-i);  
			}
		}
		else if(Press_Key==(KEY_NUM-1-i))  //当按键弹起来
		{	
			flag=1; 
			keyVal=Press_Key;
			Press_Key=KEY_NUM;
			g_FlashLedFlag=__FALSE;
		}
		else
		{
			if(LeisureCount==0x349a4d)  //5s
			{
				if(firsttipFlag)
				{
					firsttipFlag=__FALSE;
					g_CurrentState=State_MainMenu_Not_Menu;
					clearlcd();
					if(g_LanguageFlag==CHINESE)
					{
						chinesedisplay(0x80,  g_TryOverStr);
						chinesedisplay(0x90,  g_PurchaseStr);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(0x80,  "Please payment!");
						chinesedisplay(0x90,  "Tele:");
					}
					chinesedisplay(0x93,  g_ContactTel2);
				}
			}
			else
				LeisureCount++;
		}
		KB_SCK_H;
	}	  
	KB_PLOAD_L;
	if(flag==1)
	{
			 //执行键盘
		if(keyVal==ESC_KEY) 
		{
			DealEscKey();
		}
//		#ifndef EC11_BUTTON 
		else if(keyVal==OK_KEY)
		{
			DealOkKey();
		}
//		#endif
		else if(keyVal==SWAP_KEY)
		{
			DealCurrentState(State_DebugMode_Start);
		}
		else if(keyVal==BACKSPACE_KEY)
		{
			DealBackspaceKey();
		}
		else
		{
			switch(g_CurrentState)
			{
				case State_DebugMode_Start:
				case State_User_Input_SerialNumber:
					DealScreenNumber(keyVal,LCDADDR91,8);
					break;
			}
		}
	}
}

#ifdef EC11_BUTTON

U8 g_RorL=0;
U8 g_numvalue=0;

void CodingWitchPolling()
{
	static U8 Aold,Bold; //用来存储上一次调用时编码开关两引脚的电平
	static U8 st=0;  //用来存储以前是否出现两引脚都为高电平的状态

	if(BMA&&BMB)
		st=1;
	if(st)
	{
		if(BMA==0&&BMB==0)
		{
			if(Aold)//0-254
			{
				st=0;
				if(g_numvalue==0)
					g_numvalue=255;
				else
				{
					g_numvalue--;		
					g_RorL=0;
				}
			}
			if(Bold)
			{
				st=0;
				{
					g_numvalue++;
					g_RorL=1;
				}
				if(g_numvalue==255)
					g_numvalue=0;
			}
		}
	}
	Aold=BMA;
	Bold=BMB;
}

void ScanEC11()
{
//	U8 cha=0;
//	static u16  EC11Cnt=0;
//	static U8 g_preEC11Value=0;
	
	CodingWitchPolling();
	
/*	EC11Cnt++;
	if(EC11Cnt==10000)//在固定时间变化的数
	{
		EC11Cnt=0;
		if(g_preEC11Value!=g_numvalue)
		{
			if(g_RorL==0)
			{
				cha=(g_preEC11Value>g_numvalue)?(g_preEC11Value-g_numvalue):(g_preEC11Value+255-g_numvalue);
				g_preEC11Value=g_numvalue;
				if(cha<3)
					g_StepValue=1;
				else if(cha<7)
					g_StepValue=10;
				else
					g_StepValue=100;
				DealDownKey();
			}
			else
			{
				cha=(g_numvalue>g_preEC11Value)?(g_numvalue-g_preEC11Value):(g_numvalue+255-g_preEC11Value);
				g_preEC11Value=g_numvalue;
				if(cha<3)
					g_StepValue=1;
				else if(cha<7)
					g_StepValue=10;
				else
					g_StepValue=100;
				DealUpKey();
			}
		}
	}*/
}

#endif

U8 lcd_back_flag=0;



//按键扫描函数，同时处理遥控器和旋钮
//这个函数必须精简
void ScanKeypad(void)  //scan keypad
{
	//u8 rxdata;
	static U32 LeisureCount=0,LeisureCount1=0;
	int i=0;
	U8 flag=0;
	static U8 KeyACC=0;
	static _KeypadNum keyPress=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;

#ifdef USE_INFRARED_IR
	U8 Remote_Long_Key=0;
	_KeypadNum IRkeyPress=KEY_NUM; 
#endif
	//printf("enter ScanKeypad func \r\n");

		
#ifdef USE_INFRARED_IR
	
	i=Remote_Scan();

	//printf("Remote_Scan is got: %d;  ircord[2]=:%x  \r\n",i,IRcord[2]);
	
	if(i)
	{
		keyPress=KEY_NUM;
		KeyACC=0;
		g_AdvertisingFlag=__FALSE;

		switch(IRcord[2])
		{
			case 0x80:// 数字1
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM1_KEY;
				}
				break;
			case 0x40:// 数字2
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM2_KEY;
				}
				break;
			case 0xC0:// 数字3
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM3_KEY;
				}
				break;
			case 0x20:// 数字4 
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM4_KEY;
				}
				break;			
			case 0xA0:// 数字5 
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM5_KEY;
				}
				break;			
			case 0x60:// 数字6 
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM6_KEY;
				}				
				break;			
			case 0xE0:// 数字7
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM7_KEY;
				}				
				break;			
			case 0x10:// 数字8 
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM8_KEY;
				}				
				break;			
			case 0x90:// 数字9
				if(RmtCnt<4)
				{
				    flag=1;
				    keyVal=NUM9_KEY;
				}				
				break;			
			case 0x50: // 数字0
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=NUM0_KEY;
				}
				else
				{
					Remote_Long_Key=1;
					IRkeyPress=DEBUG_KEY;
				}
				break;			
			case 0x58:// 方向↑
				if(RmtCnt<4)
				{
					if(g_LockKeyFlag==UNLOCKKEYPAD)
						DealUpKey();
				}				
				break;			
			case 0x78:// 方向↓
				if(RmtCnt<4)
				{
					if(g_LockKeyFlag==UNLOCKKEYPAD)
						DealDownKey();
				}				
				break;
			case 0xF8:// 方向←
				if(RmtCnt<4)
				{
					if(IsSetColorParaValueState()||IsSetParaValueState()||IsSetupMode()||g_CurrentState==State_NewMode_Set_HSize||g_CurrentState==State_NewMode_Set_VSize)
					{
						DealBackspaceKey();
					}
				}				
				break;
			case 0xD8:// 方向→
				if(RmtCnt<4)
				{}				
				break;
			case 0x88:// 保存(双画面)
				if(RmtCnt<4)
				{
					DealSaveKey();
				}
				break;		
			case 0x70: // 调用(交换)
				if(RmtCnt<4)
				{
					DealRecallKey();
				}			
				break;
			case 0xB8:// 主菜单
				if(RmtCnt<4)
				{
				    DealMenuKey();
					printf("main menu key pushed! \r\n");
				}
				break;	
			case 0x48: // 局部/步长
				if(RmtCnt<4)
				{
					if(IsSetColorParaValueState()||IsSetParaValueState())
						DealStepKey();
					else
						DealFullOrPartKey();
				}
				break;					 		
			case 0x30: // ESC(锁)
				if(RmtCnt<4)
				{
					flag=1;
				   	keyVal=ESC_KEY;
				}
					else
				{
					Remote_Long_Key=1;
					IRkeyPress=LOCK_KEY;
				}
				break;			
			case 0xE8: // OK
				if(RmtCnt<4)
				{
				    if(g_LockKeyFlag==UNLOCKKEYPAD)
						DealOkKey();
					printf("OK key pushed! \r\n");
				}
				break;
			/*	
			case 0xC8: //USB1 
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=USB_KEY;
				}
				break;
			case 0x68: // USB2
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=USB2_KEY;
				}
				break;
			*/	
			case 0x28: // AV1
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=CVBS_KEY;
				}
				break;
			case 0xB0: // AV2
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=CVBS2_KEY;
				}
				break;
			case 0xF0: // AV3
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=CVBS3_KEY;
				}
				break;
			case 0xA8: // DVI1
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=DVI_KEY;
				}
				break;
			case 0x98: // DVI2
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=DVI2_KEY;
				}
				break;
			case 0x18: // VGA1
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=VGA_KEY;
				}
				break;
			case 0x00: // VGA2
				if(RmtCnt<4)
				{
				}
				break;
			case 0x08: // HDMI1
				if(RmtCnt<4)
				{
					flag=1;
					keyVal=HDMI_KEY;
				}
				break;
			case 0x38: // HDMI2 
				if(RmtCnt<4)
				{
				}
				break;
				
		}
		RmtCnt=0;
	}
#endif


	KB_PLOAD_L;            //PB14=0        FIO0CLR |= 0x00040000;    //pload=0 p0.18
	//-------------------------------------------
	for(i=0;i<KEY_NUM;i++)    
	{
		KB_PLOAD_H;           //PB14=1       FIO0SET |= 0x00040000;    //pload=1 p0.18
		KB_SCK_L;            //PB12=0        FIO0CLR |= 0x00020000;    //sck=0 p0.17
		if(KB_SDOUT==0x00)	   //有按键按下
		{ 
			delay1000(0x200);			
			if(!KB_SDOUT)
			{
				g_AdvertisingFlag=__FALSE;
				LeisureCount=0;
				LeisureCount1=0;
				KeyACC++;			
				keyPress=(_KeypadNum)(KEY_NUM-1-i);  	 				
// 				if(lcd_back_flag)
// 				{
// 					LCD_BK_H;
// 					LCD_Reset(); 	
// 					lcd_back_flag=0;
// 				}				
			} 
			//长按左右键的时候，可以连续发送命令
			#ifndef EC11_BUTTON
			if(KeyACC>=10&&(keyPress==UP_KEY||keyPress==DOWN_KEY)&&(IsSetParaValueState()||IsSetColorParaValueState()||g_CurrentState==State_Volume_Set))
			{
				flag=1;   //
				keyVal=keyPress;  //按键弹起的时候,对KeyVal赋值
			
			}
			#endif
		}
		else if(keyPress==(KEY_NUM-1-i))
		{	//此处是长按有特殊作用的按键，弹起来的时候不再起短按作用
			if(KeyACC>0&&!(KeyACC>=LOGNTIME&&(keyPress==LOCK_KEY||keyPress==NUM1_KEY||keyPress==NUM2_KEY
				||keyPress==NUM3_KEY||keyPress==NUM0_KEY||keyPress==NUM5_KEY||keyPress==NUM8_KEY
				#ifndef EC11_BUTTON
				||keyPress==MENU_KEY
				#endif
				||keyPress==FULLPART_KEY
				||keyPress==FREEZE_KEY||keyPress==SWAP_KEY||keyPress==DUAL_KEY||keyPress==VGA_KEY||keyPress==MUTE_KEY||keyVal==FULLPART_KEY)))
			{
				flag=1;   //
				keyVal=keyPress;  //按键弹起的时候,对KeyVal赋值
			}	
			
			if(g_LockKeyFlag==UNLOCKKEYPAD)
			{
				if(KeyACC>=LOGNTIME&&keyPress==LOCK_KEY)
					;//长按锁键，不亮灯
				else
				{
					SetKeyLEDLight(keyPress);  //right keypad led	
				}
			}
			else if(g_LockKeyFlag==LOCKKEYPAD&&KeyACC>0)
			{
				if(g_CurrentState!=State_UserTry_OnceEnd && g_CurrentState!=State_UserTry_Once_ID_Time)
					KeypadLockedPrompt();
			}
			keyPress=KEY_NUM;
			KeyACC=0;
			g_FlashLedFlag=__FALSE;
		}
		else  //此处是没有按按键
		{
			if(g_LockKeypadTime!=0)
			{//ten minutes
				if(LeisureCount==(g_LockKeypadTime*0x1a03B9D))  //大约一分钟13秒0x3000000
				{
					if(!g_WizadFlag&&g_LockKeyFlag==UNLOCKKEYPAD&&KeyACC==0)
					{
						LeisureCount=0;
						if(g_CurrentState!=State_UserTry_OnceEnd && g_CurrentState!=State_UserTry_Once_ID_Time)
							KeypadLock();
// 						lcd_back_flag=1;
//     						LCD_BK_L;
					}
				}
				else if(g_LockKeyFlag==UNLOCKKEYPAD)
				{				
					LeisureCount++;
				}
			}
			if(g_Deadline7DaysFlag<=7)
			{
				if(LeisureCount1==0x69349a)  //10s
				{//如果某个按键按下去没松开时不弹出提示
					if(!g_WizadFlag&&g_CurrentState!=State_User_Input_Try7Days&&KeyACC==0)   
					{
						LeisureCount1=0;
						DealCurrentState(State_User_Input_Try7Days);
						chinesedisplay(0x93,  g_ContactTel2);
						if(g_Deadline7DaysFlag!=7&&g_Deadline7DaysFlag!=0)
							DisplayDaxieNum(g_Deadline7DaysFlag,0x82);
					}
				}
				else
				{
					LeisureCount1++;
				}
			}
		}
		KB_SCK_H;            //PB12=1        FIO0SET |= 0x00020000;    //sck=1 p0.17
	 }
	 KB_PLOAD_L;            //PB14=0      FIO0CLR |= 0x00040000;    //pload=0 p0.18
	  //------------------------------------------------------
	   //判别哪个键被按下
	if(g_LockKeyFlag==UNLOCKKEYPAD)
	{
		if(flag==1)
		{
			flag=0; 	
			if(keyVal==ESC_KEY) 	
			{	
					if((g_MainInput==INPUTSOURCE_USB||(g_NextPointImage==MAIN&&g_SubInput==INPUTSOURCE_USB))&&IsUsbState())
						DealKey2Usb(keyVal);
					else				
					DealEscKey();
			}	 
//		#ifndef EC11_BUTTON
			else if(keyVal==OK_KEY)//menu
			{	
						if((g_MainInput==INPUTSOURCE_USB||(g_NextPointImage==MAIN&&g_SubInput==INPUTSOURCE_USB))&&IsUsbState())
						DealKey2Usb(keyVal);
					else
						DealOkKey();
			}
//		#endif
			else if(g_CurrentState==State_NewMode_Set)
			{
				if(keyVal==NUM2_KEY)
				{
					TwoImageSetWizard();
					SetPBPImage();
				}
				else
					LEDWarning(keyVal); 
			}
			else if(!Check_OK_Cancel_State())  //判断是否的选择
			{
				if(keyVal==BACKSPACE_KEY) 
					DealBackspaceKey();
				else if(keyVal==UP_KEY) 			
				{	
					if((g_MainInput==INPUTSOURCE_USB||(g_NextPointImage==MAIN&&g_SubInput==INPUTSOURCE_USB))&&IsUsbState())
						DealKey2Usb(keyVal);
					else
						DealUpKey();
				}
				else if(keyVal==DOWN_KEY) 			
				{
						if((g_MainInput==INPUTSOURCE_USB||(g_NextPointImage==MAIN&&g_SubInput==INPUTSOURCE_USB))&&IsUsbState())
						DealKey2Usb(keyVal);
					else
						DealDownKey();
				}
				else if(keyVal==FULLPART_KEY)
				{
//					if(!DealStepKey())
						DealFullOrPartKey();
				}					
				else if(IsSetParaValueState())			
				{	
					if(g_H_VFlag==__TRUE)
						DealScreenNumber(keyVal,LCDADDR91,4);
					else
						DealScreenNumber(keyVal,0x94,4);
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
								DealScreenNumber(keyVal,0x90,3);
								break;
							case IP2_IDX:
								DealScreenNumber(keyVal,0x92,3);
								break;
							case IP3_IDX:
								DealScreenNumber(keyVal,0x94,3);
								break;
							case IP4_IDX:
								DealScreenNumber(keyVal,0x96,3);
								break;
						}
						break;
				#endif
					case State_Input_PackagingName:
					case State_Input_TestName:
					case State_Input_SaleName:
					case State_User_Input_Logo:	
						DealScreenNumber(keyVal,LCDADDR91,5);
						break;	
					case State_LockPadTime_Set:		
					case State_OutputHz_NumHz:	
						DealScreenNumber(keyVal,LCDADDR91,2);
						break;
					case State_User_Input_TryDays:
						DealScreenNumber(keyVal,LCDADDR91,3);
						break;


					case State_HowLong_Save:
						DealScreenNumber(keyVal,LCDADDR91,3);
						break;	
				
					case State_User_Input_SerialNumber:
						DealScreenNumber(keyVal,LCDADDR91,8);
						break;	
					case State_DebugMode_Start:		
					case State_User_Old_SerialNumber:
					case State_User_Set_SerialNumber:
						DealScreenNumber(keyVal,LCDADDR91,6);
						break;
					case State_User_Set_TelNumber:
					case State_User_Set_Tech_TelNumber:
					case State_UserTry_OnceTel:   //启动演示密码或者单次密码时，设置授权电话  //20170213 lyg	
						DealScreenNumber(keyVal,LCDADDR91,11);
						break;
					
					case State_UserTry_ShowTime:  //演示时长设置 //20170213 lyg
					case State_UserTry_OnceTime:  //单次时长设置 //20170213 lyg
							DealScreenNumber(keyVal,0x85,2);
						break;
					case State_UserTry_OnceEnd:   //输入演示密码或者单次密码	//20170213 lyg
							DealScreenNumber(keyVal,0x85,9);
						break;
					case 	State_Cmd51_OnOff_TryOnce:
					case State_Cmd52_OnOff_168Demo:
					case State_UserTry_Once_Password103:
					case State_Cmd54_DemoTimeLength:
					case State_Cmd55_SetTryOnce_nHour:	
					case State_UserTry_Once_Password100:		
							DealScreenNumber(keyVal,0x87,6);
						break;
					case State_UserTry_Once_Password100_new:		
							DealScreenNumber(keyVal,0x86,4);
						break;
					case State_UserTry_Once_Password100_new1:		
							DealScreenNumber(keyVal,0x88,4);
						break;
					//////////////////////////////////lyg 20170213/////////////////////
					
					case State_User_Input_MathineType:
						DealScreenNumber(keyVal,LCDADDR91,10);
						break;
					case State_Input_YEAR:
						DealScreenNumber(keyVal,YEARADDR,4);
						break;
					case State_Input_MONTH:
						DealMonthNumber(keyVal,MONTHADDR);
						break;
					case State_Input_DOM:
						DealDateNumber(keyVal,DAYADDR);
						break;
					case State_Input_HOUR:
						DealHourNumber(keyVal,HOURADDR);
						break;
					case State_Input_MIN:
						DealMinNumber(keyVal,MINADDR);
						break;
					case State_Input_SEC:
						DealSecondNumber(keyVal,SECONDADDR);
						break;
					case State_SavePara_Number:
					case State_UserPara_Number:
						DealSaveAndRecallNumber(keyVal);
						break;
					
					case State_NewMode_Set_HSize:
					case State_NewMode_Set_VSize:
						DealScreenNumber(keyVal,0x97,4);
						break;
					case State_NewMode_Set_InputSource:
						DealInputNumber(keyVal,0x94);
						break;
					case State_TimeSchedule_Begin_Input1:
					case State_TimeSchedule_Begin_Input2:
					case State_TimeSchedule_Begin_Input3:
					case State_InputCycle_Set:
						DealInputNumber(keyVal,0x84);
						break;
					case State_User_HotBackup:
						if(!g_SetBackupInputFlag)
							DealInputNumber(keyVal,0x86);
						else
							DealInputNumber(keyVal,0x96);
						break;
					case State_ScheduleInput1_InputWeek:
					case State_ScheduleInput2_InputWeek:
					case State_ScheduleInput3_InputWeek:
						DealScheWeekNum(keyVal);
						break;
					default:
				   		switch(keyVal)
						{
						case CVBS_KEY:
						case CVBS2_KEY:
						case DVI_KEY:
						case HDMI_KEY:
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;
						case VGA_KEY:
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;
						case NUM0_KEY:
							DealRecallKey();
							break;
					#ifndef TYPE520		
						case CVBS3_KEY:  //CV3
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;		
						case VGA2_KEY: //VGA2
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;		
						case DVI2_KEY:// DVi2
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;		
						case USB_KEY:
// 							g_Adv7441CurrentInput=DEVICE_PORT_DVI;
// 							Write_EEPROM_Byte(E2ROM_ADV7441A_INPUT_ADDR, (U8)g_Adv7441CurrentInput);
							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));
							break;					
					#endif
					
					#ifndef EC11_BUTTON	 
						case MENU_KEY://menu
							DealMenuKey();
							break;
					#endif
						case FREEZE_KEY:
							DealFreezeKey();
							break;
						case SWAP_KEY:
							DealTakeKey();
							Save(DEFAULT_MODE);
							break;
						case DUAL_KEY:
//							DealSwitchEffect();
						 DealMultiWindowKey();
						 break;
						case MUTE_KEY:
							DealMuteKey(MUTE);
							break;
						default:
							LEDWarning(keyVal); 
						}
					}
				}
			}
			else if(Check_OK_Cancel_State())
			{
				LEDWarning(keyVal); 	
			}
		}

		
	#ifdef USE_INFRARED_IR 
		else if(Remote_Long_Key==1)
		{			
			if(IRkeyPress==NUM5_KEY)
				DealSaveKey();
		}
	#endif

	
		else if(KeyACC==LOGNTIME)
		{
			if(keyPress==FREEZE_KEY)
				DealCurrentState(State_FactoryReset_Yes);
			else if(keyPress==DUAL_KEY)
//				DealMultiWindowKey();
          			DealSaveKey();
			else if(keyPress==MUTE_KEY)
				DealVolumeKey();
			else if(keyPress==VGA_KEY)
			{
				AutoAdjustVGA();

				
				/*
				if(VGA_KEY==NUM3_KEY)
				{					
					DealCurrentState(GetCurrentOuputResolution());
					chinesedisplay(0x90, "*");
				}
				*/
			}
//			else if(keyPress==NUM5_KEY)
//				DealSaveKey();
			else if(keyPress==NUM0_KEY)
				DealRecallKey();
			else if(keyPress==NUM1_KEY)
				DealCurrentState(State_NewMode_Set);
			else if(keyPress==NUM2_KEY)
				DealCurrentState(State_ColorPara_Brightness);
			else if(keyPress==NUM3_KEY)
			{
				DealCurrentState(GetCurrentOuputResolution());
				chinesedisplay(0x90, "*");
			} 
			else if(keyPress==FULLPART_KEY)  //特效模式切换
//				DealVolumeKey();
          			DealSwitchEffect();
			#ifndef EC11_BUTTON		
			else if(keyPress==MENU_KEY)
			{
				if(g_OutputHzVal==30)
				{
					g_OutputHzVal=50;
					DealCurrentState(State_OutputHz_50Hz);		
				}
				else if(g_OutputHzVal==60)
				{
					g_OutputHzVal=30;
					DealCurrentState(State_OutputHz_30Hz);
				}
				else
				{
					g_OutputHzVal=60;
					DealCurrentState(State_OutputHz_60Hz);		
				}
				chinesedisplay(0x90, "*");
				SetOutputHzValue();
				//Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
				Save(DEFAULT_MODE);
			}
			#endif
		}		
       }
	if((KeyACC==LOGNTIME&&keyPress==SWAP_KEY)//DEBUG_KEY
	#ifdef USE_INFRARED_IR
		||(Remote_Long_Key==1&&IRkeyPress==SWAP_KEY)
	#endif
		)
	{
		g_LockKeyFlag=UNLOCKKEYPAD;
		DealCurrentState(State_DebugMode_Start);
	}
	if((KeyACC==LOGNTIME&&keyPress==LOCK_KEY)
	#ifdef USE_INFRARED_IR
		||(Remote_Long_Key==1&&IRkeyPress==LOCK_KEY)
	#endif
	)
	{	

		if(g_LockKeyFlag==UNLOCKKEYPAD)
		{
			if(g_CurrentState!=State_UserTry_OnceEnd && g_CurrentState!=State_UserTry_Once_ID_Time)
				KeypadLock();
		}
		else
		{
			g_CurrentState=State_Lock_Key;
			clearlcd();
			g_LockKeyFlag=UNLOCKKEYPAD;
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x90, g_UnLockKeyStr);
			else
				chinesedisplay(0x90, g_UnLockKeyStrEn);	
			delay(5);
			LightInputSrcLED();
			DealEscKey();
		}
	}	
		
}


void SetKeyLEDLight(_KeypadNum key)
{	
	U32 led1_32=0xffffffff;
	led1_32=0x01<<key;
	led1_32=~led1_32;
	SetKeypadLED(led1_32);  //right keypad led	
}



void SetKeypadLED(U32 LedOfKey)   //keypad led
 {
	   int i;
	  
	   for(i=0;i<KEY_NUM;i++)
	   {
	     	if(LedOfKey&(0x01<<(KEY_NUM-1)))
		 	LED_IN_H;             //PB10=1          FIO0SET |= 0x01000000;    //p0.24 led_in	   sd=1
		else
		 	LED_IN_L;             //PB10=0          FIO0CLR |= 0x01000000;    //p0.24 led_in	   sd=0
		 LED_SRCK_L; 
		 delay1000(1);
		 LED_SRCK_H; 
		 LedOfKey=LedOfKey<<1;
	   }	

	   LED_RCK_L;             //PB11=0         FIO0CLR |= 0x04000000;    //p0.26 led_rck   st=0
	   delay1000(1);
	   LED_RCK_H;             //PB11=1        FIO0SET |= 0x04000000;    //p0.26 led_rck   st=1
	   delay1000(1);
	   LED_RCK_L;             //PB11=0         //p0.26 led_rck   st=0
 }


State GetSwitchStateValue()
{
 	switch(g_CurrentState)
 	{
		case State_Fade_Switch_Mode:
			if(g_MutiWindow==SINGLEIMAGE)
			{
				SetMultiWindowPara(SAMEPIPIMAGE,0);
//				SwapDualInputSrc();
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE)  //从PBP或PIP进入淡入淡出，双画面大小一样
			{	
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);	
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
				SetScalerHVValue(HVSIZE,MAIN);
				SetScalerHVValue(HVPOS,MAIN);
				SetScalerHVValue(HVSIZE,SUB);
				SetScalerHVValue(HVPOS,SUB);
			}
			else if(g_SwitchEffectValue==FuseSwitchIdx)
			{
				SetMultiWindowPara(FUSEPIPPARA1,0);
			}
			
/*********************************
不用恢复到VGA
*********************************/
// 			if(g_initPIPSrc==0)
// 			{
// 				g_initPIPSrc=1;
// 				g_SubInput=INITPIPSRC;
// 			}
			if(!CheckDualSource( g_MainInput-1,g_SubInput-1))
			{
 				g_SubInput=g_MainInput;
			}
			g_MutiWindow=SAMEPIPIMAGE;
			g_SwitchEffectValue=FadeSwitchIdx;
			break;
		case State_Quick_Switch_Mode:
			if(g_MutiWindow==SINGLEIMAGE)
			{
				SetMultiWindowPara(SAMEPIPIMAGE,0);
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE) //从PBP或PIP进入快速切换，双画面大小一样
			{
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
				SetScalerHVValue(HVSIZE,MAIN);
				SetScalerHVValue(HVPOS,MAIN);
				SetScalerHVValue(HVSIZE,SUB);
				SetScalerHVValue(HVPOS,SUB);
			}
			else if(g_SwitchEffectValue==FuseSwitchIdx)
			{
				SetMultiWindowPara(FUSEPIPPARA1,0);
			}
			if(g_initPIPSrc==0)
			{
				g_initPIPSrc=1;
				g_SubInput=INITPIPSRC;
			}
			if(!CheckDualSource(g_MainInput-1, g_SubInput-1))
			{
				g_SubInput=g_MainInput;
			}
			g_MutiWindow=SAMEPIPIMAGE;
			g_SwitchEffectValue=QuickSwitchIdx;
			break;
		case State_Fuse_Switch_Mode:
			if(g_MutiWindow==SINGLEIMAGE)
			{
				SetMultiWindowPara(SAMEPIPIMAGE,0);	
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE) //从PBP或PIP进入快速切换，双画面大小一样
			{				
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				//SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				//SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
				SetScalerHVValue(HVSIZE,MAIN);
				SetScalerHVValue(HVPOS,MAIN);
				SetScalerHVValue(HVSIZE,SUB);
				SetScalerHVValue(HVPOS,SUB);
			}
			SetMultiWindowPara(FUSEPIPPARA1,g_AlphaValue);
// 			if(g_initPIPSrc==0)
// 			{
// 				g_initPIPSrc=1;
// 				g_SubInput=INITPIPSRC;
// 			}
			if(!CheckDualSource(g_MainInput-1, g_SubInput-1))
			{
				g_SubInput=g_MainInput;
			}
			g_MutiWindow=SAMEPIPIMAGE;
			g_SwitchEffectValue=FuseSwitchIdx;
			break;
 	}
	Save(DEFAULT_MODE);
 	return g_CurrentState;
 }




StateData stateData[]=
{
	//State_MainMenu_Menu
	{
		0x80,
		"",
		"",
		0x90,
		g_CurrentTimeStr,
		g_CurrentTimeStrEn,
		{
			State_MainMenu_Menu_Logo,  //up
			State_MainMenu_Menu_StartInfo,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_StartInfo,	
	{
		0x80,
		g_StartTimeStr,
		"Turn-on:    day",
		0x90,
		g_StartTimeFormatStr,
		"  :   :",
		{State_MainMenu_Menu,   //up
			State_MainMenu_Menu_VersionInfo,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_VersionInfo,
	{
		0x80,
		g_MachineTypeStr,
		g_MachineTypeStrEn,
		0x90,
		g_SoftVersionStr,
		"Version:",
		{State_MainMenu_Menu_StartInfo,   //up
			State_MainMenu_Menu_IDInfo,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_IDInfo
	{
		0x80,
		g_MathineIDStr,
		"ID:",
		0x90,
		" ",
		" ",
		{State_MainMenu_Menu_VersionInfo,   //up
			State_MainMenu_Menu_InOutput,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_InOutput,
	{
		0x80,
		g_ScreenInfo1Str,
		"Para",
		0x90,
		g_OutResolutionStr,
		"Out:",
		{State_MainMenu_Menu_IDInfo,   //up
			State_MainMenu_Menu_Current_Date,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_Current_Date
	{
		0x80,g_DateStr,g_DateStrEn,
		0x90,g_Factory,g_FactoryEn,

		{
			State_MainMenu_Menu_InOutput,   //up		
			State_MainMenu_Menu_Logo,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
		
	},

	//State_MainMenu_Menu_Logo
	{
		0x80,g_SupportTechStr,g_SupportTechStrEn,
		0x90,g_ContactTel1,g_ContactTel1,
		{
			State_MainMenu_Menu_Current_Date,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL		
	},

	//State_MainMenu_Not_Menu
	{
		0x80,
		NULL,
		NULL,
		0x90,
		" ",
		" ",
		{State_MainMenu_Menu,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	
	//State_FullOrPart_Full,	
	{
		0x80, g_fullZoom,g_fullZoomEn,
		0x91, g_full,g_fullEn,
		{State_FullOrPart_Part,
			State_FullOrPart_Part,
			State_FullOrPart_Full,
			State_MainMenu_Menu},
		&SetCurrentStateValue,
		&GetFullOrPartStateValue,
		NULL
			
	},
	//State_FullOrPart_Part,
	{
		0x80, g_fullZoom,g_fullZoomEn,
		0x91, g_jubu,g_jubuEn,
		{
			State_FullOrPart_Full,
			State_FullOrPart_Full,
			State_FullOrPart_Part,
			State_MainMenu_Menu},
		&SetCurrentStateValue,
		&GetFullOrPartStateValue,
		NULL
	},	
	//State_MultiWindow_Single,
	{
		LCDADDR80,g_SingleOrDual,g_SingleOrDualEn,
		LCDADDR91,g_SingleCh,g_SingleChEn,
		{
			State_MultiWindow_PIP,
			State_MultiWindow_PBP,
			State_MultiWindow_Single,
			State_MainMenu_Menu		
		},&SetCurrentStateValue,
		&GetMultiWindowStateValue,
		NULL
	},
	
	//State_MultiWindow_PBP,
	{		
		LCDADDR80,g_SingleOrDual,g_SingleOrDualEn,
		LCDADDR91,g_PBP,g_PBPEn,
		{
			State_MultiWindow_Single,  //up
			State_MultiWindow_PIP,  //down
			State_MultiWindow_PBP,   //ok			
			State_MainMenu_Menu	//esc
		},&SetCurrentStateValue,
		&GetMultiWindowStateValue,
		NULL
	},
	//State_MultiWindow_PIP,
	{		
		LCDADDR80,g_SingleOrDual,g_SingleOrDualEn,
		LCDADDR91,g_PIP,g_PIPEn,
		{
			State_MultiWindow_PBP,
			State_MultiWindow_Single,
			State_MultiWindow_PIP,			
			State_MainMenu_Menu
		},&SetCurrentStateValue,
		&GetMultiWindowStateValue,
		NULL
	},
	//State_MultiWindow_SAMEPIP
	{		
		LCDADDR80," "," ",
		LCDADDR91," "," ",
		{
			State_MultiWindow_SAMEPIP,
			State_MultiWindow_SAMEPIP,
			State_MultiWindow_SAMEPIP,			
			State_MainMenu_Menu
		},NULL,
		&GetMultiWindowStateValue,
		NULL
	},
	//State_ActiveImage_Main,
	{	
		LCDADDR80,g_ActiveImage,g_ActiveImageEn,
		LCDADDR91,g_MainImage,g_MainImageEn,
		{
			State_ActiveImage_Sub,
			State_ActiveImage_Sub,
			State_ActiveImage_Main,			
			State_MainMenu_Menu
		},&SetCurrentStateValue,
		&GetActiveImageStateValue,
		NULL
	},
	//State_ActiveImage_Sub,
	{
		LCDADDR80,g_ActiveImage,g_ActiveImageEn,
		LCDADDR91,g_SubImage,g_SubImageEn,
		{
			State_ActiveImage_Main,
			State_ActiveImage_Main,
			State_ActiveImage_Sub,		
			State_MainMenu_Menu
		},&SetCurrentStateValue,
		&GetActiveImageStateValue,
		NULL
	},
	//State_SavePara_Number,
	{
		LCDADDR80,g_SaveModeStr,g_SaveModeStrEn,
		0x84,g_MainMenuNum,g_MainMenuNumEn,
		{
			State_SavePara_Number,
			State_SavePara_Number,
			State_SavePara_Number,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		NULL
	},
	//State_UserPara_Number,
	{
		LCDADDR80,g_RecallModeStr,g_RecallModeStrEn,
		0x84,g_MainMenuNum,g_MainMenuNumEn,
		{
			State_UserPara_Number,
			State_UserPara_Number,
			State_UserPara_Number,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		NULL
	},//
	//State_MainMenu_Menu_ScreenPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_ScreenPara,g_ScreenParaEn,
		{
			State_MainMenu_Menu_DebugMode,
			State_MainMenu_Menu_ScalerPara,
			State_ScreenPara_Size,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		NULL
	},
	//State_ScreenPara_Size,
	{
		LCDADDR80,g_ScreenPara,g_ScreenParaEn,
		LCDADDR90,g_ScreenSizeStr,"1.Screen Size",
		{
			State_ScreenPara_UserResolution,
			State_ScreenPara_Pos,
			State_ScreenPara_Size_Value,			
			State_MainMenu_Menu_ScreenPara
		},NULL,
		NULL,
		NULL
	},
	//State_ScreenPara_Size_Value
	{
		LCDADDR80,g_SelScalerHStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ScreenPara_Size_Value,
			State_ScreenPara_Size_Value,
			State_ScreenPara_Size_Value,			
			State_ScreenPara_Size
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},

	//State_ScreenPara_Pos,
	{
		LCDADDR80,g_ScreenPara,g_ScreenParaEn,
		LCDADDR90,g_StartPos,"2.Start Position",
		{
			State_ScreenPara_Size,
			State_ScreenPara_UserResolution,
			State_ScreenPara_Pos_Value,
			State_MainMenu_Menu_ScreenPara
		},NULL,
		NULL,NULL
	},
	//State_ScreenPara_Pos_Value,
	{
		LCDADDR80,g_SelHPosStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ScreenPara_Pos_Value,
			State_ScreenPara_Pos_Value,
			State_ScreenPara_Pos_Value,			
			State_ScreenPara_Pos
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ScreenPara_UserResolution,                    
	{
		LCDADDR80,g_ScreenPara,g_ScreenParaEn,
		LCDADDR90,g_UserResolutionStr,"3.User Resolution",
		{
			State_ScreenPara_Pos,
			State_ScreenPara_Size,
			State_ScreenPara_UserResolution_Value,
			State_MainMenu_Menu_ScreenPara
		},NULL,
		NULL,NULL
	},
	//State_ScreenPara_UserResolution_Value,
	{
		LCDADDR80,g_UserSetResolutionStr,"User Set Resolution",
		LCDADDR91,NULL,NULL,
		{
			State_ScreenPara_UserResolution_Value,
			State_ScreenPara_UserResolution_Value,
			State_ScreenPara_UserResolution_Value,			
			State_ScreenPara_UserResolution
		},NULL,//&SetCurrentStateValue,
		NULL,
		NULL
	},
	//State_MainMenu_Menu_ScalerPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_ScalerPara,g_ScalerParaEn,
		{
			State_MainMenu_Menu_ScreenPara,
			State_MainMenu_Menu_ZoomPara,
			State_ScalerPara_Size,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_ScalerPara_Size,
	{
		LCDADDR80,g_ScalerPara,g_ScalerParaEn,
		LCDADDR90,g_ScalerSizeStr,"1.Scale",
		{
			State_ScalerPara_Pos,
			State_ScalerPara_Pos,
			State_ScalerPara_Size_Value,			
			State_MainMenu_Menu_ScalerPara
		},NULL,
		NULL,
		NULL
	},
	//State_ScalerPara_Size_Value,
	{
		0x80,g_SelScalerHStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ScalerPara_Size_Value,
			State_ScalerPara_Size_Value,
			State_ScalerPara_Size_Value,			
			State_ScalerPara_Size
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ScalerPara_Pos,
	{
		LCDADDR80,g_ScalerPara,g_ScalerParaEn,
		LCDADDR90,g_StartPos,"2.Start Position",
		{
			State_ScalerPara_Size,
			State_ScalerPara_Size,
			State_ScalerPara_Pos_Value,			
			State_MainMenu_Menu_ScalerPara
		},NULL,
		NULL,
		NULL
	},
	//State_ScalerPara_Pos_Value,
	{
		LCDADDR80,g_SelHPosStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ScalerPara_Pos_Value,
			State_ScalerPara_Pos_Value,
			State_ScalerPara_Pos_Value,			
			State_ScalerPara_Pos
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},

	//State_MainMenu_Menu_ZoomPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_ZoomPara,g_ZoomParaEn,
		{
			State_MainMenu_Menu_ScalerPara,
			State_MainMenu_Menu_ColorPara,
			State_ZoomPara_Size,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_ZoomPara_Size,
	{
		LCDADDR80,g_ZoomPara,g_ZoomParaEn,
		LCDADDR90,g_ZoomSizeStr,"1.Zoom",
		{
			State_ZoomPara_Input,
			State_ZoomPara_Pos,
			State_ZoomPara_Size_Value,			
			State_MainMenu_Menu_ZoomPara
		},NULL,
		NULL,
		NULL
	},
	//State_ZoomPara_Size_Value,
	{
		LCDADDR80,g_SelZoomHStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ZoomPara_Size_Value,
			State_ZoomPara_Size_Value,
			State_ZoomPara_Size_Value,			
			State_ZoomPara_Size
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},

	//State_ZoomPara_Pos,
	{
		LCDADDR80,g_ZoomPara,g_ZoomParaEn,
		LCDADDR90,g_StartPos,"2.Start Position",
		{
			State_ZoomPara_Size,
			State_ZoomPara_Input,
			State_ZoomPara_Pos_Value,			
			State_MainMenu_Menu_ZoomPara
		},NULL,
		NULL,
		NULL
	},
	//State_ZoomPara_Pos_Value,
	{
		LCDADDR80,g_SelHPosStr,g_SelHStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ZoomPara_Pos_Value,
			State_ZoomPara_Pos_Value,
			State_ZoomPara_Pos_Value,			
			State_ZoomPara_Pos
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ZoomPara_Input,
	{
		LCDADDR80,g_ZoomPara,g_ZoomParaEn,
		LCDADDR90,g_InputZoomStr,"3.Input Format",
		{
			State_ZoomPara_Pos,
			State_ZoomPara_Size,
			State_ZoomPara_Input_Format,			
			State_MainMenu_Menu_ZoomPara
		},NULL,
		NULL,
		NULL
	},
	//State_ZoomPara_Input_Format,
	{
		LCDADDR80,g_InputZoomStr+6,"Input Format",
		LCDADDR90,NULL,NULL,
		{
			State_ZoomPara_Input_Format,
			State_ZoomPara_Input_Format,
			State_ZoomPara_Input_Format,			
			State_ZoomPara_Input
		},NULL,
		NULL,
		NULL
	},

	//State_MainMenu_Menu_ColorPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_ColorPara,g_ColorParaEn,
		{
			State_MainMenu_Menu_ZoomPara,
			State_MainMenu_Menu_AdvancePara,
			State_ColorPara_Brightness,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_BrightnessStr,g_BrightnessEn,
		{
			State_ColorPara_ADC,
			State_ColorPara_Contrast,
			State_ColorPara_Brightness_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness_Value,
	{
		LCDADDR80,g_BrightnessStr,g_BrightnessEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Brightness_Value,
			State_ColorPara_Brightness_Value,
			State_ColorPara_Brightness_Value,			
			State_ColorPara_Brightness
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contrast,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_ContractStr,g_ContractEn,
		{
			State_ColorPara_Brightness,
			State_ColorPara_Saturation,
			State_ColorPara_Contrast_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contrast_Value,
	{
		LCDADDR80,g_ContractStr,g_ContractEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contrast_Value,
			State_ColorPara_Contrast_Value,
			State_ColorPara_Contrast_Value,			
			State_ColorPara_Contrast
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
/*	//State_ColorPara_Brightness_R,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_OffsetRStr,g_OffsetREn,
		{
			State_ColorPara_Brightness,
			State_ColorPara_Brightness_G,
			State_ColorPara_Brightness_R_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness_R_Value,
	{
		LCDADDR80,g_OffsetRStr,g_OffsetREn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Brightness_R_Value,
			State_ColorPara_Brightness_R_Value,
			State_ColorPara_Brightness_R_Value,			
			State_ColorPara_Brightness_R
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Brightness_G,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_OffsetGStr,g_OffsetGEn,
		{
			State_ColorPara_Brightness_R,
			State_ColorPara_Brightness_B,
			State_ColorPara_Brightness_G_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness_G_Value,
	{
		LCDADDR80,g_OffsetGStr,g_OffsetGEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Brightness_G_Value,
			State_ColorPara_Brightness_G_Value,
			State_ColorPara_Brightness_G_Value,			
			State_ColorPara_Brightness_G
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Brightness_B,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_OffsetBStr,g_OffsetBEn,
		{
			State_ColorPara_Brightness_G,
			State_ColorPara_Contrast,
			State_ColorPara_Brightness_B_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness_B_Value,
	{
		LCDADDR80,g_OffsetBStr,g_OffsetBEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Brightness_B_Value,
			State_ColorPara_Brightness_B_Value,
			State_ColorPara_Brightness_B_Value,			
			State_ColorPara_Brightness_B
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contrast_R,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_GainRStr,g_GainREn,
		{
			State_ColorPara_Contrast,
			State_ColorPara_Contrast_G,
			State_ColorPara_Contrast_R_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contrast_R_Value,
	{
		LCDADDR80,g_GainRStr,g_GainREn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contrast_R_Value,
			State_ColorPara_Contrast_R_Value,
			State_ColorPara_Contrast_R_Value,			
			State_ColorPara_Contrast_R
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contrast_G,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_GainGStr,g_GainGEn,
		{
			State_ColorPara_Contrast_R,
			State_ColorPara_Contrast_B,
			State_ColorPara_Contrast_G_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contrast_G_Value,
	{
		LCDADDR80,g_GainGStr,g_GainGEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contrast_G_Value,
			State_ColorPara_Contrast_G_Value,
			State_ColorPara_Contrast_G_Value,			
			State_ColorPara_Contrast_G
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contrast_B,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_GainBStr,g_GainBEn,
		{
			State_ColorPara_Contrast_G,
			State_ColorPara_Saturation,
			State_ColorPara_Contrast_B_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contrast_B_Value,
	{
		LCDADDR80,g_GainBStr,g_GainBEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contrast_B_Value,
			State_ColorPara_Contrast_B_Value,
			State_ColorPara_Contrast_B_Value,			
			State_ColorPara_Contrast_B
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},*/
	//State_ColorPara_Saturation
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_SaturationStr,g_SaturationStrEn,
		{
			State_ColorPara_Contrast,
			State_ColorPara_Reset,
			State_ColorPara_Saturation_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Saturation_Value
	{
		LCDADDR80,g_SaturationStr,g_SaturationStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Saturation_Value,
			State_ColorPara_Saturation_Value,
			State_ColorPara_Saturation_Value,			
			State_ColorPara_Saturation
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Reset,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_ColorResetStr,g_ColorResetStrEn,
		{
			State_ColorPara_Saturation,
			State_ColorPara_ADC,
			State_ColorPara_Reset_Check,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Reset_Check,
	{
		LCDADDR80,g_ColorResetStr,g_ColorResetStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_ColorPara_Reset,
			State_ColorPara_Reset,
			State_ColorPara_Reset_Check,			
			State_ColorPara_Reset
		},&SetCurrentStateValue,
		NULL,
		NULL
	},

			
	//State_ColorPara_ADC, //menu4.5,stateval=46
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_AdcCalibrationStr,g_AdcCalibrationStrEn,
		{
			State_ColorPara_Reset,
			State_ColorPara_Brightness,
			State_ColorPara_ADC_Calibration,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	
	//State_ColorPara_ADC_Calibration, //menu4.5.0,stateval=47
	{
		LCDADDR80,g_AdcCalibrationStr,g_AdcCalibrationStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_ColorPara_ADC,
			State_ColorPara_ADC,
			State_ColorPara_ADC_Calibration,			
			State_ColorPara_ADC
		},&SetCurrentStateValue,
		NULL,
		NULL
	},

			
	//State_MainMenu_Menu_AdvancePara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_AdvancePara,g_AdvanceParaEn,
		{
			State_MainMenu_Menu_ColorPara,
			State_MainMenu_Menu_UserPara,
			State_AdvancePara_OutputFormat,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_AdvancePara_OutputFormat,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_outputFormatstr,g_outputFormatstrEn,
		{
			State_AdvancePara_Language,
			State_AdvancePara_OutputHz,
			State_OutputFormat_640x480,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_OutputFormat_640x480,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"640x480","640x480",
		{
			State_OutputFormat_UserPrefer,//State_OutputFormat_3840x600,//State_OutputFormat_2048x1152,
			State_OutputFormat_800x600,
			State_OutputFormat_640x480,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_800x600,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"800x600","800x600",
		{
			State_OutputFormat_640x480,
			State_OutputFormat_1024x768x60,
			State_OutputFormat_800x600,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1024x768x60,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1024x768","1024x768",
		{
			State_OutputFormat_800x600,
			State_OutputFormat_1280x720,
			State_OutputFormat_1024x768x60,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1280x720,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x720","1280x720",
		{
			State_OutputFormat_1024x768x60,
			State_OutputFormat_1280x1024,
			State_OutputFormat_1280x720,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1280x1024,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x1024","1280x1024",
		{
			State_OutputFormat_1280x720,
			State_OutputFormat_1366x768,
			State_OutputFormat_1280x1024,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1366x768,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1366x768","1366x768",
		{
			State_OutputFormat_1280x1024,
			State_OutputFormat_1440x900,
			State_OutputFormat_1366x768,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1440x900,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1440x900","1440x900",
		{
			State_OutputFormat_1366x768,
			State_OutputFormat_1600x1200,
			State_OutputFormat_1440x900,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1600x1200,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1600x1200@50","1600x1200@50",
		{
			State_OutputFormat_1440x900,
			State_OutputFormat_1680x1050,
			State_OutputFormat_1600x1200,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1680x1050,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1680x1050","1680x1050",
		{
			State_OutputFormat_1600x1200,
			State_OutputFormat_1920x1080,
			State_OutputFormat_1680x1050,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1920x1080,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1920x1080","1920x1080",
		{
			State_OutputFormat_1680x1050,
			State_OutputFormat_1920x1200,
			State_OutputFormat_1920x1080,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1920x1200,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1920x1200@50","1920x1200@50",
		{
			State_OutputFormat_1920x1080,
			State_OutputFormat_2048x1152,
			State_OutputFormat_1920x1200,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_2048x1152,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"2048x1152@50","2048x1152@50",
		{
			State_OutputFormat_1920x1200,
			State_OutputFormat_2560x960,//State_OutputFormat_640x480,
			State_OutputFormat_2048x1152,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_2560x960,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"2560x960@50","2560x960@50",
		{
			State_OutputFormat_2048x1152,
			State_OutputFormat_3840x600,
			State_OutputFormat_2560x960,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_3840x600
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"3840x600","3840x600",
		{
			State_OutputFormat_2560x960,
			State_OutputFormat_UserPrefer,//State_OutputFormat_640x480,
			State_OutputFormat_3840x600,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_UserPrefer
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,g_UserResolutionStr+2,"User Resolution",
		{
			State_OutputFormat_3840x600,
			State_OutputFormat_640x480,
			State_ScreenPara_UserResolution_Value,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_AdvancePara_OutputHz,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_OutputHzStr,g_OutputHzStrEn,
		{
			State_AdvancePara_OutputFormat,
			State_AdvancePara_Audio,
			State_OutputHz_30Hz,			
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_OutputHz_30Hz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"30HZ","30HZ",
		{
			State_OutputHz_NumHz,
			State_OutputHz_50Hz,	
			State_OutputHz_30Hz,		
			State_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
	//State_OutputHz_50Hz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"50HZ","50HZ",
		{
			State_OutputHz_30Hz,
			State_OutputHz_60Hz,
			State_OutputHz_50Hz,			
			State_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
	//State_OutputHz_60Hz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"60HZ","60HZ",
		{
			State_OutputHz_50Hz,
			State_OutputHz_NumHz,
			State_OutputHz_60Hz,			
			State_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
	//State_OutputHz_NumHz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,NULL,NULL,
		{
			State_OutputHz_60Hz,
			State_OutputHz_30Hz,
			State_OutputHz_NumHz,			
			State_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
	
	//State_FreezeImage_Check,
	{
		LCDADDR80,g_FreezeImage,g_FreezeImageEn,
		LCDADDR91,NULL,NULL,
		{
			State_FreezeImage_Check,
			State_FreezeImage_Check,
			State_FreezeImage_Check,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		&GetCurrentWindowValue
	},
	//State_AdvancePara_Audio,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_AudioStr,g_AudioStrEn,
		{
			State_AdvancePara_OutputHz,
			State_AdvancePara_LockPadTime,
			State_Audio_Main,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_Audio_Main
	{
		LCDADDR80,g_AudioStr,g_AudioStrEn,
		LCDADDR91,g_MainImage,g_MainImageEn,
		{
			State_Audio_Sub,
			State_Audio_Sub,
			State_Audio_Main,	
			State_AdvancePara_Audio
		},&SetCurrentStateValue,
		&GetActiveAudioStateValue,NULL
	},
	//State_Audio_Sub
	{
		LCDADDR80,g_AudioStr,g_AudioStrEn,
		LCDADDR91,g_SubImage,g_SubImageEn,
		{
			State_Audio_Main,
			State_Audio_Main,
			State_Audio_Sub,	
			State_AdvancePara_Audio
		},&SetCurrentStateValue,
		&GetActiveAudioStateValue,NULL
	},
	//State_AdvancePara_LockPadTime
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_LockKeypadStr,g_LockKeypadStrEN,
		{
			State_AdvancePara_Audio,
			State_AdvancePara_FactoryReset,
			State_LockPadTime_Set,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_LockPadTime_Set
	{
		LCDADDR80,g_LockKeypadStr,g_LockKeypadStrEN,
		LCDADDR91,g_CancelLockKeypadStr,"_     (0:Cancel)",
		{
			State_LockPadTime_Set,
			State_LockPadTime_Set,
			State_LockPadTime_Set,	
			State_AdvancePara_LockPadTime
		},&DebugSetValue,
		NULL,NULL
	},
	//State_AdvancePara_FactoryReset,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_FactoryReset,g_FactoryResetEn,
		{
			State_AdvancePara_LockPadTime,
			State_AdvancePara_Wizard,
			State_FactoryReset_Yes,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_FactoryReset_Yes,
	{
		LCDADDR80,g_FactoryReset,g_FactoryResetEn,
		0x93,g_YesStr,g_YesStrEn,
		{
			State_FactoryReset_OK,
			State_AdvancePara_FactoryReset,
			State_FactoryReset_OK,	
			State_AdvancePara_FactoryReset
		},NULL,
		NULL,NULL
	},
	//State_FactoryReset_OK,
	{
		LCDADDR80,g_Yes,g_YesEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_FactoryReset_Yes,
			State_FactoryReset_Yes,
			State_FactoryReset_OK,	
			State_FactoryReset_Yes
		},&SetCurrentStateValue,
		NULL,NULL
	},
	//State_AdvancePara_Wizard
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_DirectSetStr,g_DirectSetStrEn,
		{
			State_AdvancePara_FactoryReset,
			State_AdvancePara_Language,
			State_NewMode_Set,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL	
	},

	//State_AdvancePara_Language,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_Language,g_LanguageEn,
		{
			State_AdvancePara_Wizard,
			State_AdvancePara_OutputFormat,
			State_Language_English,	
			State_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_Language_English,
	{
		LCDADDR80,g_Language,g_LanguageEn,
		LCDADDR91,"English","English",
		{
			State_Language_Chinese,
			State_Language_Chinese,
			State_Language_English,	
			State_AdvancePara_Language
		},&SetCurrentStateValue,
		NULL,NULL
	},
	//State_Language_Chinese,
	{
		LCDADDR80,g_Language,g_LanguageEn,
		LCDADDR91,g_Chinese,g_Chinese,
		{
			State_Language_English,
			State_Language_English,
			State_Language_Chinese,	
			State_AdvancePara_Language
		},&SetCurrentStateValue,
		NULL,NULL
	},
	
//	State_MainMenu_Menu_UserPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_UserParaStr,g_UserParaStrEn,
		{
			State_MainMenu_Menu_AdvancePara,
			State_MainMenu_TimeSchedule,
			State_UserPara_SaveMode,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
//	State_UserPara_SaveMode,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SaveModeStr,g_SaveModeStrEn,
		{
			State_UserPara_SetStaticIP,
			//State_UserPara_SetHotBackup,
			State_UserPara_RecallMode,
			State_SavePara_Number,	
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL
	},
//	State_UserPara_RecallMode,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_RecallModeStr,g_RecallModeStrEn,
		{
			State_UserPara_SaveMode,
			State_UserPara_SetCurrentTime,
			State_UserPara_Number,	
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL
	},	
	//State_UserPara_SetCurrentTime,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetTimeStr,g_SetTimeStrEn,
		{
			State_UserPara_RecallMode,
			State_UserPara_SetFadeTime,
			State_UserPara_SetCurrentTime,	
			State_MainMenu_Menu_UserPara
		},&SetCurrentStateValue,
		NULL,NULL		
	},
	//State_UserPara_SetFadeTime
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		{
			State_UserPara_SetCurrentTime,
			State_UserPara_SetHotBackup,
			State_UserPara_SetFadeTime_1500,	
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL		
	},
	//State_UserPara_SetHotBackup
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetHotBackupStr,g_SetHotBackupStrEn,
		{
			State_UserPara_SetFadeTime,
			State_UserPara_SetStaticIP,//State_UserPara_SaveMode,
			State_HotBackup_Set,	
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL
	},
	//State_HotBackup_Set,
	{
		LCDADDR80,g_SetHotBackupStr,g_SetHotBackupStrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_HotBackup_Cancel,
			State_HotBackup_Cancel,
			State_HotBackup_Check,	
			State_UserPara_SetHotBackup
		},NULL,
		NULL,NULL
	},
	//State_HotBackup_Cancel,
	{
		LCDADDR80,g_SetHotBackupStr,g_SetHotBackupStrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_HotBackup_Set,
			State_HotBackup_Set,
			State_HotBackup_Cancel,	
			State_UserPara_SetHotBackup
		},&CancelTimeSchedule,
		NULL,NULL
	},	
	//State_HotBackup_Check
	{
		0x82,g_UserHotBackupStr,"User Set Backup",
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_HotBackup_Check,
			State_HotBackup_Check,
			State_User_HotBackup,
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},	
	//State_User_HotBackup
	{
		0x80,g_InputSignalStr,"Input",
		0x90,g_BackupSignalStr,"Backup",
		{
			State_User_HotBackup,
			State_User_HotBackup,
			State_User_HotBackup,
			State_HotBackup_Check
		},&SetTimeSchedule,
		NULL,NULL
	},	
	//State_User_HotBackup_Continue
	{
		0x83,g_YesStr,g_YesStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_User_HotBackup_Continue,
			State_User_HotBackup_Continue,
			State_User_HotBackup,
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_UserPara_SetFadeTime_1000
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"0.5","0.5",
		{
			State_UserPara_SetFadeTime_2000,
			State_UserPara_SetFadeTime_1500,
			State_UserPara_SetFadeTime_1000,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
	//State_UserPara_SetFadeTime_1500
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"1.0","1.0",
		{
			State_UserPara_SetFadeTime_1000,
			State_UserPara_SetFadeTime_2000,
			State_UserPara_SetFadeTime_1500,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
	//State_UserPara_SetFadeTime_2000
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"1.5","1.5",
		{
			State_UserPara_SetFadeTime_1500,
			State_UserPara_SetFadeTime_1000,
			State_UserPara_SetFadeTime_2000,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
	//State_UserPara_SetStaticIP
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetStaticIPStr,g_SetStaticIPStrEn,
		{
			State_UserPara_SetHotBackup,
			State_UserPara_SaveMode,
	#ifdef TCP_IP
			State_UserPara_SetStaticIP_Value,	
	#else
			State_UserPara_SetStaticIP,
	#endif
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL		
	},
	//State_UserPara_SetStaticIP_Value
	{
		LCDADDR80,g_SetStaticIPStr,g_SetStaticIPStrEn,
		LCDADDR90,"_  .   .   .","   .   .   .",
		{
			State_UserPara_SetStaticIP,
			State_UserPara_SetStaticIP,
			State_UserPara_SetStaticIP_Value,	
			State_UserPara_SetStaticIP
		},NULL,//&SetCurrentStateValue,
		NULL,NULL
	},
	//State_SavePara_Mode_check,
	{
		LCDADDR80," "," ",
		LCDADDR90,g_SaveOverlappingStr,g_SaveOverlappingStrEn,
		{
			State_SavePara_Mode_check,
			State_SavePara_Number,
			State_SavePara_Mode_check,			
			State_SavePara_Number
		},&SetCurrentStateValue,
		NULL,
		NULL
	},
	//State_MainMenu_TimeSchedule,
	{			
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_TimeScheduleStr,g_TimeScheduleStrEn,
		{
			State_MainMenu_Menu_UserPara,
			State_MainMenu_Menu_TechSupport,
			State_TimeSchedule_InputSource,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},

	//State_TimeSchedule_InputSource,
	{			
		LCDADDR80,g_TimeScheduleStr,g_TimeScheduleStrEn,
		LCDADDR90,g_InputScheduleStr,g_InputScheduleStrEn,
		{
			State_TimeSchedule_InputCycle,
			#if(ENABLE_SCHEDULE_MODE)
			State_TimeSchedule_UserMode,
			#else
			State_TimeSchedule_InputCycle,	
			#endif
			State_TimeSchedule_Input1,	
			State_MainMenu_TimeSchedule
		},NULL,
		NULL,NULL		
	},	
	//State_TimeSchedule_Input1,
	{
		LCDADDR80,g_InputScheduleStr,g_InputScheduleStrEn,
		LCDADDR91,g_Program1Str,g_Program1StrEn,
		{
			State_TimeSchedule_CancelAll,
			State_TimeSchedule_Input2,
			State_TimeSchedule_SettingInput1,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_SettingInput1,
	{
		LCDADDR80,g_Program1Str,g_Program1StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelInput1,
			State_TimeSchedule_CancelInput1,
			State_TimeSchedule_Begin_Input1,	
			State_TimeSchedule_Input1
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_Begin_Input1
	{
		LCDADDR80,g_InputStatusStr,g_InputStatusStrEn,
		LCDADDR90,g_ScheTimeStr,g_ScheTimeStrEn,
		{
			State_TimeSchedule_Begin_Input1,
			State_TimeSchedule_Begin_Input1,
			State_TimeSchedule_Begin_Input1,	
			State_TimeSchedule_SettingInput1
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_ScheduleInput1_Once,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleOnceStr,g_ScheduleOnceStrEn,
		{
			State_ScheduleInput1_EveryDay,
			State_ScheduleInput1_EveryDay,
			State_ScheduleInput1_Once,
			State_TimeSchedule_Begin_Input1
		},NULL,
		&GetScheduleInputMode1,NULL
	},			
	//State_ScheduleInput1_EveryDay,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleEverydayStr,g_ScheduleEverydayStrEn,
		{
			State_ScheduleInput1_Once,
			State_ScheduleInput1_Once,			
			State_ScheduleInput1_InputWeek,
			State_TimeSchedule_Begin_Input1
		},NULL,
		&GetScheduleInputMode1,NULL
	},	
	//State_ScheduleInput1_InputWeek,
	{
		0x80,g_ScheduleWhichDayStr,g_ScheduleWhichDayStrEn,
		LCDADDR90,"_","_",
		{
			State_ScheduleInput1_InputWeek,
			State_ScheduleInput1_InputWeek,			
			State_ScheduleInput1_InputWeek,
			State_ScheduleInput1_EveryDay
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_CancelInput1,
	{
		LCDADDR80,g_Program1Str,g_Program1StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_SettingInput1,
			State_TimeSchedule_SettingInput1,
			State_TimeSchedule_CancelInput1,	
			State_TimeSchedule_Input1
		},&CancelTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_Input2,
	{
		LCDADDR80,g_InputScheduleStr,g_InputScheduleStrEn,
		LCDADDR91,g_Program2Str,g_Program2StrEn,
		{
			State_TimeSchedule_Input1,
			State_TimeSchedule_Input3,
			State_TimeSchedule_SettingInput2,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_SettingInput2,
	{
		LCDADDR80,g_Program2Str,g_Program2StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelInput2,
			State_TimeSchedule_CancelInput2,
			State_TimeSchedule_Begin_Input2,	
			State_TimeSchedule_Input2
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_Begin_Input2
	{
		LCDADDR80,g_InputStatusStr,g_InputStatusStrEn,
		LCDADDR90,g_ScheTimeStr,g_ScheTimeStrEn,
		{
			State_TimeSchedule_Begin_Input2,
			State_TimeSchedule_Begin_Input2,
			State_TimeSchedule_Begin_Input2,	
			State_TimeSchedule_SettingInput2
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_ScheduleInput2_Once,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleOnceStr,g_ScheduleOnceStrEn,
		{
			State_ScheduleInput2_EveryDay,
			State_ScheduleInput2_EveryDay,
			State_ScheduleInput2_Once,
			State_TimeSchedule_Begin_Input2
		},NULL,
		&GetScheduleInputMode2,NULL
	},			
	//State_ScheduleInput2_EveryDay,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleEverydayStr,g_ScheduleEverydayStrEn,
		{
			State_ScheduleInput2_Once,
			State_ScheduleInput2_Once,
			State_ScheduleInput2_InputWeek,
			State_TimeSchedule_Begin_Input2
		},NULL,
		&GetScheduleInputMode2,NULL
	},	
	//State_ScheduleInput2_InputWeek,
	{
		0x80,g_ScheduleWhichDayStr,g_ScheduleWhichDayStrEn,
		LCDADDR90,"_","_",
		{
			State_ScheduleInput2_InputWeek,
			State_ScheduleInput2_InputWeek,			
			State_ScheduleInput2_InputWeek,
			State_ScheduleInput2_EveryDay
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_CancelInput2,
	{
		LCDADDR80,g_Program2Str,g_Program2StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_SettingInput2,
			State_TimeSchedule_SettingInput2,
			State_TimeSchedule_CancelInput2,	
			State_TimeSchedule_Input2
		},&CancelTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_Input3,
	{
		LCDADDR80,g_InputScheduleStr,g_InputScheduleStrEn,
		LCDADDR91,g_Program3Str,g_Program3StrEn,
		{
			State_TimeSchedule_Input2,
			State_TimeSchedule_CancelAll,
			State_TimeSchedule_SettingInput3,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_SettingInput3,
	{
		LCDADDR80,g_Program3Str,g_Program3StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelInput3,
			State_TimeSchedule_CancelInput3,
			State_TimeSchedule_Begin_Input3,	
			State_TimeSchedule_Input3
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_Begin_Input3
	{
		LCDADDR80,g_InputStatusStr,g_InputStatusStrEn,
		LCDADDR90,g_ScheTimeStr,g_ScheTimeStrEn,
		{
			State_TimeSchedule_Begin_Input3,
			State_TimeSchedule_Begin_Input3,
			State_TimeSchedule_Begin_Input3,	
			State_TimeSchedule_SettingInput3
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_ScheduleInput3_Once,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleOnceStr,g_ScheduleOnceStrEn,
		{
			State_ScheduleInput3_EveryDay,
			State_ScheduleInput3_EveryDay,
			State_ScheduleInput3_Once,
			State_TimeSchedule_Begin_Input3
		},NULL,
		&GetScheduleInputMode3,NULL
	},			
	//State_ScheduleInput3_EveryDay,
	{
		0x80,g_ScheduleModeStr,g_ScheduleModeStrEn,
		LCDADDR91,g_ScheduleEverydayStr,g_ScheduleEverydayStrEn,
		{
			State_ScheduleInput3_Once,
			State_ScheduleInput3_Once,
			State_ScheduleInput3_InputWeek,
			State_TimeSchedule_Begin_Input3
		},NULL,
		&GetScheduleInputMode3,NULL
	},
	//State_ScheduleInput3_InputWeek,
	{
		0x80,g_ScheduleWhichDayStr,g_ScheduleWhichDayStrEn,
		LCDADDR90,"_","_",
		{
			State_ScheduleInput3_InputWeek,
			State_ScheduleInput3_InputWeek,			
			State_ScheduleInput3_InputWeek,
			State_ScheduleInput3_EveryDay
		},NULL,
		NULL,NULL
	},		
	//State_TimeSchedule_CancelInput3,
	{
		LCDADDR80,g_Program3Str,g_Program3StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_SettingInput3,
			State_TimeSchedule_SettingInput3,
			State_TimeSchedule_CancelInput3,	
			State_TimeSchedule_Input3
		},&CancelTimeSchedule,
		NULL,NULL
	},	
#if(ENABLE_SCHEDULE_MODE)
	//State_TimeSchedule_UserMode,
	{					
		LCDADDR80,g_TimeScheduleStr,g_TimeScheduleStrEn,
		LCDADDR90,g_ModeScheduleStr,g_ModeScheduleStrEn,
		{
			State_TimeSchedule_InputSource,
			State_TimeSchedule_InputCycle,
			State_TimeSchedule_Mode1,	
			State_MainMenu_TimeSchedule
		},NULL,
		NULL,NULL
	},	
	//State_TimeSchedule_Mode1,
	{					
		LCDADDR80,g_ModeScheduleStr,g_ModeScheduleStrEn,
		LCDADDR91,g_Program1Str,g_Program1StrEn,
		{
			State_TimeSchedule_Mode3,
			State_TimeSchedule_Mode2,
			State_TimeSchedule_BeginMode1,	
			State_TimeSchedule_UserMode
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginMode1,
	{
		LCDADDR80,g_Program1Str,g_Program1StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelMode1,
			State_TimeSchedule_CancelMode1,
			State_TimeSchedule_BeginMode1,	
			State_TimeSchedule_Mode1
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_CancelMode1,
	{
		LCDADDR80,g_Program1Str,g_Program1StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_BeginMode1,
			State_TimeSchedule_BeginMode1,
			State_TimeSchedule_CancelMode1,	
			State_TimeSchedule_Mode1
		},&CancelTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_Mode2,
	{				
		LCDADDR80,g_ModeScheduleStr,g_ModeScheduleStrEn,
		LCDADDR91,g_Program2Str,g_Program2StrEn,
		{
			State_TimeSchedule_Mode1,
			State_TimeSchedule_Mode3,
			State_TimeSchedule_BeginMode2,	
			State_TimeSchedule_UserMode
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginMode2,
	{
		LCDADDR80,g_Program2Str,g_Program2StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelMode2,
			State_TimeSchedule_CancelMode2,
			State_TimeSchedule_BeginMode2,	
			State_TimeSchedule_Mode2
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_CancelMode2,
	{
		LCDADDR80,g_Program2Str,g_Program2StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_BeginMode2,
			State_TimeSchedule_BeginMode2,
			State_TimeSchedule_CancelMode2,	
			State_TimeSchedule_Mode2
		},&CancelTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_Mode3,
	{				
		LCDADDR80,g_ModeScheduleStr,g_ModeScheduleStrEn,
		LCDADDR91,g_Program3Str,g_Program3StrEn,
		{
			State_TimeSchedule_Mode2,
			State_TimeSchedule_Mode1,
			State_TimeSchedule_BeginMode3,	
			State_TimeSchedule_UserMode
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginMode3,
	{
		LCDADDR80,g_Program3Str,g_Program3StrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_TimeSchedule_CancelMode3,
			State_TimeSchedule_CancelMode3,
			State_TimeSchedule_BeginMode3,	
			State_TimeSchedule_Mode3
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_TimeSchedule_CancelMode3,
	{
		LCDADDR80,g_Program3Str,g_Program3StrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_TimeSchedule_BeginMode3,
			State_TimeSchedule_BeginMode3,
			State_TimeSchedule_CancelMode3,	
			State_TimeSchedule_Mode3
		},&CancelTimeSchedule,
		NULL,NULL
	},
#endif
	//State_TimeSchedule_InputCycle,
	{					
		LCDADDR80,g_TimeScheduleStr,g_TimeScheduleStrEn,
		LCDADDR90,g_CycleScheduleStr,g_CycleScheduleStrEn,
		{
		#if(ENABLE_SCHEDULE_MODE)
			State_TimeSchedule_UserMode
		#else
			State_TimeSchedule_InputSource,
		#endif
			State_TimeSchedule_InputSource,
			State_InputCycle_Set,	
			State_MainMenu_TimeSchedule
		},NULL,
		NULL,NULL
	},	
	//State_InputCycle_Set,
	{
		LCDADDR80,g_CycleScheduleStr,g_CycleScheduleStrEn,
		LCDADDR91,g_SetScheduleStr,g_SetScheduleStrEn,
		{
			State_InputCycle_Cancel,
			State_InputCycle_Cancel,
			State_InputCycle_Set,	
			State_TimeSchedule_InputCycle
		},&SetTimeSchedule,
		NULL,NULL
	},
	//State_InputCycle_Cancel,
	{
		LCDADDR80,g_CycleScheduleStr,g_CycleScheduleStrEn,
		LCDADDR91,g_CancelScheduleStr,g_CancelScheduleStrEn,
		{
			State_InputCycle_Set,
			State_InputCycle_Set,
			State_InputCycle_Cancel,	
			State_TimeSchedule_InputCycle
		},&CancelTimeSchedule,
		NULL,NULL
	},
	//State_CycleInput_Continue
	{
		0x83,g_YesStr,g_YesStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_CycleInput_Continue,
			State_CycleInput_Continue,
			State_InputCycle_Set,
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	
	//State_TimeSchedule_CancelAll,
	{			
		LCDADDR80,g_TimeScheduleStr,g_TimeScheduleStrEn,
		LCDADDR90,g_CancelAllScheduleStr,g_CancelAllScheduleStrEn,
		{
			State_TimeSchedule_Input3,
			State_TimeSchedule_Input1,
			State_CancelAll_Yes,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL		
	},
	//State_CancelAll_Yes,
	{
		LCDADDR80,g_CancelAllScheduleStr,g_CancelAllScheduleStrEn,
		0x93,g_YesStr,g_YesStrEn,
		{
			State_TimeSchedule_CancelAll,
			State_TimeSchedule_CancelAll,
			State_CancelAll_Yes,	
			State_TimeSchedule_CancelAll
		},&SetCurrentStateValue,
		NULL,NULL
	},
	//State_MainMenu_Menu_TechSupport,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_TechSupport,g_TechSupportEn,
		{
			State_MainMenu_TimeSchedule,
			State_MainMenu_Menu_DebugMode,
			State_TechSuppor_ContactInfo,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_TechSuppor_ContactInfo,
	{
		LCDADDR80,g_TechSupport,g_TechSupportEn,
		LCDADDR90,g_ContactInfo,g_ContactInfoEn,
		{
			State_TechSuppor_VersionInfo,
			State_TechSuppor_VersionInfo,
			State_TechSuppor_ContactInfo_Page,	
			State_MainMenu_Menu_TechSupport
		},NULL,
		NULL,NULL
	},
	//State_TechSuppor_ContactInfo_Page,	
	{
		LCDADDR80,g_SupportTechStr,g_SupportTechStrEn,
		LCDADDR90,g_ContactTel1,g_ContactTel1,
		{
			State_TechSuppor_ContactInfo_Page,
			State_TechSuppor_ContactInfo_Page,
			State_TechSuppor_ContactInfo_Page,	
			State_TechSuppor_ContactInfo
		},NULL,
		NULL,NULL
	},
	//State_TechSuppor_VersionInfo,
	{
		LCDADDR80,g_TechSupport,g_TechSupportEn,
		LCDADDR90,g_VersionInfo,g_VersionInfoEn,
		{
			State_TechSuppor_ContactInfo,
			State_TechSuppor_ContactInfo,
			State_TechSuppor_VersionInfo_Page,	
			State_MainMenu_Menu_TechSupport
		},NULL,
		NULL,NULL
	},
	//State_TechSuppor_VersionInfo_Page,	
	{
		LCDADDR80,g_VersionInfo,g_VersionInfoEn,
		LCDADDR90,g_ReleaseStr,g_ReleaseStr,
		{
			State_TechSuppor_VersionInfo_Page,
			State_TechSuppor_VersionInfo_Page,
			State_TechSuppor_VersionInfo_Page,	
			State_TechSuppor_VersionInfo
		},NULL,
		NULL,NULL
	},
	
	//State_MainMenu_Menu_DebugMode,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_SetUpModeStr,g_SetUpModeStrEn,
		{
			State_MainMenu_Menu_TechSupport,
			State_MainMenu_Menu_ScreenPara,
			State_DebugMode_Check,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL		
	},
	//State_DebugMode_Check,
	{
		LCDADDR80,g_SetUpModeStr,g_SetUpModeStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_DebugMode_Start,
			State_MainMenu_Menu_DebugMode,
			State_DebugMode_Start,	
			State_MainMenu_Menu_DebugMode
		},NULL,
		NULL,NULL
	} ,
	//State_DebugMode_Start
	{
		LCDADDR80,g_InputSetupStr,g_InputSetupStrEn,
		LCDADDR91,"_","_",
		{
			State_DebugMode_Start,
			State_DebugMode_Check,
			State_DebugMode_Start,	
			State_DebugMode_Check
		},&DebugSetValue,
		NULL,NULL
	} ,
	//State_NewMode_Set,
	{
		0x83,g_DirectSetStr,g_DirectSetStrEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_NewMode_Set,
			State_NewMode_Set,
			State_NewMode_Set_HSize,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},	
	//State_NewMode_Set_HSize,
	{
		0x83,g_DirectSetStr,g_DirectSetStrEn,
		LCDADDR90,g_LEDHSizeStr,g_LEDHSizeStrEn,
		{
			State_NewMode_Set_HSize,
			State_NewMode_Set_HSize,
			State_NewMode_Set_VSize,	
			State_NewMode_Set
		},&DirectSetValue,
		NULL,NULL
	},	
	//State_NewMode_Set_VSize,
	{
		0x83,g_DirectSetStr,g_DirectSetStrEn,
		LCDADDR90,g_LEDVSizeStr,g_LEDVSizeStrEn,
		{
			State_NewMode_Set_VSize,
			State_NewMode_Set_VSize,
			State_NewMode_Set_InputSource,	
			State_NewMode_Set_HSize
		},&DirectSetValue,
		NULL,NULL
	},	
	//State_NewMode_Set_InputSource
	{
		0x83,g_DirectSetStr,g_DirectSetStrEn,
		LCDADDR90,g_InputStatusStr,g_InputStatusStrEn,
		{
			State_NewMode_Set_InputSource,
			State_NewMode_Set_InputSource,
			State_MainMenu_Menu,	
			State_NewMode_Set_VSize
		},NULL,
		NULL,NULL
	},	
	//State_DualImage_Set_MainSize
	{
		0x80,g_DirectSetMainSizeStr,"Set Main Image Size",
		LCDADDR91,NULL,NULL,
		{
			State_DualImage_Set_MainSize,
			State_DualImage_Set_MainSize,
			State_DualImage_Set_MainSize,	
			State_NewMode_Set
		},NULL,
		NULL,NULL
	},
	//State_DualImage_Set_SubSize
	{
		0x80,g_DirectSetSubSizeStr,"Set Sub Image Size",
		LCDADDR91,NULL,NULL,
		{
			State_DualImage_Set_SubSize,
			State_DualImage_Set_SubSize,
			State_DualImage_Set_SubSize,	
			State_DualImage_Set_MainSize
		},NULL,
		NULL,NULL
	},	
	//State_Input_YEAR
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_YEAR,
			State_Input_YEAR,
			State_Input_MONTH,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Input_MONTH,
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_MONTH,
			State_Input_MONTH,
			State_Input_DOM,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Input_DOM,
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_DOM,
			State_Input_DOM,
			State_Input_HOUR,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Input_HOUR,
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_HOUR,
			State_Input_HOUR,
			State_Input_MIN,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Input_MIN,
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_MIN,
			State_Input_MIN,
			State_Input_SEC,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Input_SEC,
	{
		0x80,NULL,NULL,
		LCDADDR90,NULL,NULL,
		{
			State_Input_SEC,
			State_Input_SEC,
			State_Input_SEC,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_Volume_Set,
	{
		LCDADDR80,g_VolumeStr,g_VolumeStrEn,
		LCDADDR90,NULL,NULL,
		{
			State_Volume_Set,
			State_Volume_Set,
			State_Volume_Set,			
			State_MainMenu_Menu
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentVolumeValue
	},
	//State_Lock_Key
	{
		0x80,
		"",
		"",
		0x90,
		g_CurrentTimeStr,
		g_CurrentTimeStrEn,
		{State_MainMenu_Menu,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_Fade_Switch_Mode,
	{
		0x80,g_SwitchInputModeStr,g_SwitchInputModeStrEn,
		0x91,g_FadeInFadeOutStr,g_FadeInFadeOutStrEn,
		{State_Fade_Switch_Mode,   //State_Fuse_Switch_Mode,   //up
			State_Fade_Switch_Mode,   //State_Quick_Switch_Mode,  //down
			State_Fade_Switch_Mode,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,//&GetSwitchStateValue,
		NULL
	},
	//State_Quick_Switch_Mode,
	{
		0x80,g_SwitchInputModeStr,g_SwitchInputModeStrEn,
		0x91,g_QuickSwitchStr,g_QuickSwitchStrEn,
		{State_Quick_Switch_Mode,   //State_Fade_Switch_Mode,   //up
			State_Quick_Switch_Mode,   //State_Fuse_Switch_Mode,  //down
			State_Quick_Switch_Mode,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,//&GetSwitchStateValue,
		NULL
	},
	//State_Fuse_Switch_Mode,
	{
		0x80,g_SwitchInputModeStr,g_SwitchInputModeStrEn,
		0x91,g_FuseSwitchStr,"Fuse Mode",
		{State_Fuse_Switch_Value,   //State_Quick_Switch_Mode,   //up
			State_Fuse_Switch_Value,   //State_Fade_Switch_Mode,  //down
			State_Fuse_Switch_Value,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,//&GetSwitchStateValue,
		NULL
	},
	//State_Fuse_Switch_Value,
	{
		0x80,g_FuseSwitchStr,"Fuse Mode",
		0x91,NULL,NULL,
		{State_Fuse_Switch_Value,    //up
			State_Fuse_Switch_Value,    //down
			State_Fuse_Switch_Value,   //ok
			State_Fuse_Switch_Mode},  //esc
		&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue,
	},
/*	//State_Try_Use	
	{
		0x80,
		g_TryForUsr,
		"Try to Use",
		0x90,
		g_YesOrNoStr,
		g_YesOrNoStrEn,
		{State_Try_Use,   //up
			State_Try_Use,  //down
			State_Input_YEAR,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},*/
	
	//State_User_Input_SerialNumber
	{
		0x80,g_InputSeriesStr,"Input Serial number",
		0x91,"_","_",
		{State_User_Input_SerialNumber,   //up
			State_User_Input_SerialNumber,  //down
			State_User_Input_SerialNumber,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Old_SerialNumber
	{
		0x80,g_InputOldSeriesStr,"Input Old Serial:",
		0x91,"_","_",
		{State_User_Old_SerialNumber,   //up
			State_User_Old_SerialNumber,  //down
			State_User_Old_SerialNumber,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Set_SerialNumber
	{
		0x80,g_SetSeriesStr,"Set Serial number",
		0x91,"_","_",
		{State_User_Set_SerialNumber,   //up
			State_User_Set_SerialNumber,  //down
			State_User_Set_SerialNumber,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Set_TelNumber
	{
		0x80,g_SetTeleNumStr,"Set Telephone",
		0x91,"_","_",
		{State_User_Set_TelNumber,   //up
			State_User_Set_TelNumber,  //down
			State_User_Set_TelNumber,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Set_Tech_TelNumber
	{
		0x80,g_SetTechTeleNumStr,"Set Technology Tele:",
		0x91,"_","_",
		{State_User_Set_Tech_TelNumber,   //up
			State_User_Set_Tech_TelNumber,  //down
			State_User_Set_Tech_TelNumber,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Input_TryDays
	{
		0x80,g_TryForUsr,"Input Days",
		0x91,"_","_",
		{State_User_Input_TryDays,   //up
			State_User_Input_TryDays,  //down
			State_User_Input_TryDays,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Input_Try7Days
	{
		0x80,g_DeadLine7DaysStr,"Deadline 7 Days",
		0x90,g_PurchaseStr,"Tele:",
		{State_MainMenu_Menu,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Input_Logo
	{
		0x80,g_InputLogoStr,"Input Logo",
		0x91,"_","_",
		{State_User_Input_Logo,   //up
			State_User_Input_Logo,  //down
			State_User_Input_Logo,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_Input_PackagingName,
	{
		0x80,g_InputPackagingNameStr,"Input Packaging ID:",
		0x91,"_","_",
		{State_Input_PackagingName,   //up
			State_Input_PackagingName,  //down
			State_Input_PackagingName,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_Input_TestName,
	{
		0x80,g_InputTestNameStr,"Input Test ID:",
		0x91,"_","_",
		{State_Input_TestName,   //up
			State_Input_TestName,  //down
			State_Input_TestName,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_Input_SaleName,
	{
		0x80,g_InputSaleNameStr,"Input salesman ID:",
		0x91,"_","_",
		{State_Input_SaleName,   //up
			State_Input_SaleName,  //down
			State_Input_SaleName,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
	//State_User_Input_MathineType,
	{
		0x80,g_InputMathineTypeStr,"Input Type(<10):",
		0x91,"_","_",
		{State_User_Input_MathineType,   //up
			State_User_Input_MathineType,  //down
			State_User_Input_MathineType,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},


	//State_HowLong_Save,
	{
		0x80,g_InputMinuteStr,"Input Minute(<256):",
		0x91,"_","_",
		{State_HowLong_Save,   //up
			State_HowLong_Save,  //down
			State_HowLong_Save,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},	
	
//20170213 lyg ///////////////////////////////////////////
	//	State_UserTry_ShowTime,
	{
		0x80,g_ShowTimeStr,g_ShowTimeStrEn,
		0x90,g_ShowTimeRangeStr,g_ShowTimeRangeStrEn,
		{
			State_UserTry_ShowTime,
			State_UserTry_ShowTime,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	} ,
//	State_UserTry_OnceTime,
	{
		0x80,g_OnceTimeStr,g_OnceTimeStrEn,
		0x90,g_OnceTimeRangeStr,g_OnceTimeRangeStrEn,
		{
			State_UserTry_OnceTime,
			State_UserTry_OnceTime,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	} ,
//	State_UserTry_OnceTel,
	{
		0x80,g_OnceTelStr,g_OnceTelStrEn,
		0x90," "," ",
		{
			State_UserTry_OnceTel,
			State_UserTry_OnceTel,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	} ,
//	State_UserTry_OnceEnd,
	{
		0x80,g_OnceEndInputPwStr,g_OnceEndInputPwStrEn,
		0x90,g_OnceEndTelStr,g_OnceEndTelStrEn,
		{
			State_UserTry_Once_ID_Time,
			State_UserTry_Once_ID_Time,
			State_UserTry_OnceEnd,	
			State_UserTry_OnceEnd
		},&DirectSetValue,
		NULL,NULL
	} ,
//	State_UserTry_Once_ID_Time,
	{
		0x80,g_OnceEndIDStr,g_OnceEndIDStrEn,
		0x90,g_OnceEndTimeStr,g_OnceEndTimeStrEn,
		{
			State_UserTry_OnceEnd,
			State_UserTry_OnceEnd,
			State_UserTry_Once_ID_Time,	
			State_UserTry_Once_ID_Time
		},&DirectSetValue,
		NULL,NULL
	} ,
//	State_UserTry_Once_SetEnd,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_DebugMode_Start,
			State_DebugMode_Start,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},NULL,
		NULL,NULL
	} ,
	
//	State_USerTry_Once_Password101,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_Cmd51_OnOff_TryOnce,
			State_Cmd51_OnOff_TryOnce,			
			State_UserTry_Once_SetEnd,
			State_DebugMode_Start,
		},&DirectSetValue,
		NULL,NULL
	},
//	State_USerTry_Once_Password102,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_Cmd52_OnOff_168Demo,
			State_Cmd52_OnOff_168Demo,
			State_UserTry_Once_SetEnd,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	},
//	State_USerTry_Once_Password103,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password103,
			State_UserTry_Once_Password103,		
			State_UserTry_OnceTel,
			State_DebugMode_Start	
		},&DirectSetValue,
		NULL,NULL
	},
//	State_USerTry_Once_Password104,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_Cmd54_DemoTimeLength,
			State_Cmd54_DemoTimeLength,
			State_UserTry_ShowTime,
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	},
//	State_USerTry_Once_Password105,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_Cmd55_SetTryOnce_nHour,
			State_Cmd55_SetTryOnce_nHour,
			State_UserTry_OnceTime,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	},
//	State_USerTry_Once_Password100,
	{
		0x80,g_InputManagePwStr,g_InputManagePwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100,
			State_UserTry_Once_Password100,
			State_UserTry_Once_Password100_new,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL
	},

//	State_UserTry_Once_Password100_new,
	{
		0x80,g_InputNewPwStr,g_InputNewPwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100_new,
			State_UserTry_Once_Password100_new,
			State_UserTry_Once_Password100_new1,	
			State_UserTry_Once_Password100
		},&DirectSetValue,
		NULL,NULL
	},
	//	State_UserTry_Once_Passwore100_new1,
	{
		0x80,g_InputNew1PwStr,g_InputNewPwStrEn,   //输入密码
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100_new1,
			State_UserTry_Once_Password100_new1,
			State_UserTry_Once_Password100_new1,	
			State_UserTry_Once_Password100_new
		},&DirectSetValue,
		NULL,NULL
	},
//	State_UserTry_Once_Password100_ok,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_DebugMode_Start,
			State_DebugMode_Start,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},NULL,
		NULL,NULL
	},
///////////////////////////////////////////////////////////
};



