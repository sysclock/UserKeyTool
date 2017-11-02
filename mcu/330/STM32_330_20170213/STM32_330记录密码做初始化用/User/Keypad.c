/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: keypad.c
 [Date]:        09-May-2012
 [Comment]:
   keypad subroutines.
 [Reversion History]:
*******************************************************************************/

#define _KEYPAD_C_
#include <stdio.h>
#include "header.h"

#include "remote.h"

//int regcnt=100; //计数寄存器 静态变量
#ifdef XP360
	static U8 g_PCPowerFlag=0;
#endif



//static U8 g_MinTempValue,g_SecTempValue;
#if(XP_TO_ID==XP_TO_FLI32626)
extern U8 g_initPIPSrc;
U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1] =
{
//       MAIN->AV1, AV2,AV3,SV, DVI, YPBPR, VGA, HDMI ,CVBS4,HDMI2,VGA2, DVI2,   SDI,  DP
//SUB

/*AV1     */    {   1,  0,    0,    0,     1,     1,      1,        1,         0,        1,      1,       1,       1,    1},
/*AV2     */    {   0,  1,    0,    0,      1,     1,     1,        1,         0,        1,      1,       1,       1,    1},
/*AV3     */    {   0,  0,    1,    0,      1,     1,     1,        1,         0,        1,      1,        1,       1,    1},
/*SV      */     {   0,  0,    0,    1,      1,     1,     1,        1,         0,        1,      1,        1,       1,    1},
/*DVI     */     {   1,  1,    1,    1,      1,     0,     1,       0,         1,        0,      1,        0,       1,    1},
/*YPBPR */     {   1,  1,    1,    1,      0,     1,     0,       0,          1,        0,      0,        0,       1,    1},
/*VGA    */     {   1,  1,    1,    1,      1,     0,     1,       1,          1,        1,      0,        1,       1,    1},
/*HDMI   */    {   1,  1,    1,    1,      0,      0,     1,       1,          1,        0,      1,        0,       1,    1},
/*CVBS4 */   {    0,  0,    0,    0,      1,      1,     1,       1,          1,        1,      1,        1,       1,    1},
/*HDMI2 */    {    1,  1,    1,    1,      0,     0,     1,       0,          1,        1,      1,        0,       1,    1},
/*VGA2  */     {   1,   1,    1,    1,      1,     0,     0,       1,          1,        1,      1,        1,       1,    1},
/*DVI2   */     {   1,   1,    1,    1,      0,     0,     1,       0,          1,        0,      1,        1,       1,    1},
/*SDI    */      {   1,   1,    1,    1,      1,     1,     1,       1,          1,        1,      1,        1,       1,    1},
/*DP     */      {    1,   1,    1,    1,      1,     1,     1,       1,          1,        1,      1,        1,       1,    1},
};
#else

U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1] =
{
//       MAIN->AV1, AV2,AV3,SV, DVI, YPBPR, VGA, HDMI ,CVBS4,HDMI2,VGA2, DVI2,   SDI,  DP
//SUB

/*AV1     */    {   1,  0,    0,    0,     1,     1,      1,        1,         0,        1,      1,       1,       1,    1},
/*AV2     */    {   0,  1,    0,    0,      1,     1,     1,        1,         0,        1,      1,       1,       1,    1},
/*AV3     */    {   0,  0,    1,    0,      1,     1,     1,        1,         0,        1,      1,        1,       1,    1},
/*SV      */     {   0,  0,    0,    1,      1,     1,     1,        1,         0,        1,      1,        1,       1,    1},
/*DVI     */     {   1,  1,    1,    1,      1,     1,     1,       0,         1,        0,      1,        0,       1,    1},
/*YPBPR */     {   1,  1,    1,    1,      1,     1,     1,       1,          0,        1,      1,        1,       1,    1},
/*VGA    */     {   1,  1,    1,    1,      1,     1,     1,       1,          1,        1,      0,        1,       1,    1},
/*HDMI   */    {   1,  1,    1,    1,      0,     1,     1,        1,          1,        0,      1,        0,       1,    1},
/*CVBS4 */   {    0,  0,    0,    0,      1,     0,     1,        1,          1,        1,      1,        1,       1,    1},
/*HDMI2 */    {    1,  1,    1,    1,      0,     1,     1,       0,          1,        1,      1,        0,       1,    1},
/*VGA2  */     {   1,   1,    1,    1,      1,     1,     0,       1,          1,        1,      1,        1,       1,    1},
/*DVI2   */     {   1,   1,    1,    1,      0,     1,     1,       0,          1,        0,      1,        1,       1,    1},
/*SDI    */      {   1,   1,    1,    1,      1,     1,     1,       1,          1,        1,      1,        1,       1,    1},
/*DP     */      {    1,   1,    1,    1,      1,     1,     1,       1,          1,        1,      1,        1,       1,    1},
};
#endif

void PanelToScalerPara()
{
	g_H_VFlag=__TRUE;
	g_ScreenHSize=g_PanelWidth;
	g_ScreenVSize=g_PanelHeight;
	g_ScreenHPos=0;
	g_ScreenVPos=0;
	
	if(g_MutiWindow==SINGLEIMAGE||g_MutiWindow==PIPIMAGE
#if(XP_ID == XP520)
		||g_MutiWindow==SAMEPIPIMAGE
#endif
		)
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
#if(XP_ID == XP520)
	else if(g_MutiWindow==SAMEPIPIMAGE)
	{
		g_SubScalerHSize=g_PanelWidth;
		g_SubScalerVSize=g_PanelHeight;
		g_SubScalerHPos=0;
		g_SubScalerVPos=0;	
	}
#endif	
}

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
}

void PanelToScreenPara()
{
	PanelToScalerPara();
	PanelToZoomPara();
}

void initColorVariable()
{
	g_Brightness_Value=DEF_BRIGHTNESS;
	g_Contrast_Value=DEF_CONTRACT;
	g_Saturation_Value=DEF_SATURATION;
}

//void InitLockTempTime()
//{
//	g_MinTempValue=systmtime.tm_min;
//	g_SecTempValue=systmtime.tm_sec;
//}

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

#ifdef HOT_BACKUP
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
#endif

void initGlobalVar()
{	
#ifdef POWER_ON_PWD
	g_PowerOnPwdOk=__FALSE;
	g_PowerOnPwdFlag=__FALSE;
	g_howlongtosave=5;//15;
	g_NumSaved=0;
#endif
	g_RTCErrorFlag=0;
	g_BothSwitchFlag=__FALSE;
	g_Pack_ID=0;
	g_Test_ID=0;
	g_Sale_ID=0;
	g_Deadline7DaysFlag=0xFF;
	g_UserSetPasswordVal=INITIALVALUE_USERPASSWORD;////1000000;
	g_TeleNumVal=0;
	g_TechTeleNumVal=0;
	g_MainboardHRevision=0;
	g_MainboardLRevision=0;
	g_OffBackFlag=UNOffBackLight;
	g_FlashLedFlag=__FALSE;
	g_InSrcHSize=1920;
	g_InSrcVSize=1080;
	g_OffBackLightTime=g_LockKeypadTime*3+15;//close LCD backLight
	g_ComTypeFlag=Communication_NONE;
	g_DeviceID=0;
	g_AdvertisingFlag=__FALSE;
	g_StaticIPIdx=IP1_IDX;
#ifdef HOT_BACKUP
	initHotBackupVariable(__FALSE);
#endif
	g_StartCycleInputFlag=__FALSE;
	initCycleInputPara();
	g_CycleInputStartHour=0xff;
	g_CycleInputStartMin=0xff;
	g_InputCycleIdx=0;

	g_TryTimeEndFlag=__FALSE;
#ifdef INT_DELAY_PC_CTL
	g_PCSendFlag=__FALSE;
	g_OneSecondISRVal=0;
#endif
	g_PCControlFlag=__FALSE;
	g_FourByteFlag=__FALSE;
	g_TwoByteFlag=__FALSE;
	g_OneByteFlag=__FALSE;
//	g_MstarIDFlag=__FALSE;
	g_MainboardVersionFlag=__FALSE;
	
	g_WizadFlag=__FALSE;
	g_LockKeypadTime=0;//10;	
	g_LockKeyFlag=UNLOCKKEYPAD;
	g_LanguageFlag=CHINESE;
	g_OutputHzVal=60;
	g_ScheduleInput1=INPUTSOURCE_NONE;
	g_ScheduleInput2=INPUTSOURCE_NONE;
	g_ScheduleInput3=INPUTSOURCE_NONE;

	g_ScheduleInputMode1=ONCESCHE;
	g_ScheduleInputMode2=ONCESCHE;
	g_ScheduleInputMode3=ONCESCHE;

	g_MainFreezeFlag=0;
	g_SubFreezeFlag=0;
//	g_NumLockFlag=0;
	g_VolumeValue=50;    
	
	g_PowerOnFlag=__FALSE;
	g_QueueFullFlag=__FALSE;
	
	g_OutputFormatIndex=INDEX1024X768;
	g_StepValue=1;
	g_MutiWindow=SINGLEIMAGE;
	
	g_FullOrPartValue=PART;  //放大的局部全屏
	
	g_VGAFullOrPartValue=FULL;  //屏参的局部全屏
	g_DVIFullOrPartValue=FULL;
	g_HDMIFullOrPartValue=FULL;
	
//	g_OtherFullOrPartValue=FULL;	
	g_ActiveImageValue=MAIN;
	g_ActiveAudioWin=MAIN;
	   
	g_MainFreezeFlag=UNFREEZE;
	g_SubFreezeFlag=UNFREEZE;
	
	g_PanelWidth=1024;
	g_PanelHeight=768;
	g_MainInput=INIMAINSRC;//INPUTSOURCE_DVI;
	g_SubInput=INITPIPSRC;//INPUTSOURCE_DVI;
	g_TryEndFlag=__FALSE;

	initColorVariable();
#ifdef HOT_BACKUP
	g_PoweronInputSendFlag=__FALSE;
#endif	
		

}

BOOL IsSetHVParaValueState()
{
	switch(g_CurrentState)
	{
		case State_ZoomPara_Size_Value:
		case State_ZoomPara_Pos_Value:
		case State_ScreenPara_Size_Value:
		case State_ScreenPara_Pos_Value:					
		case State_ScalerPara_Size_Value:	
		case State_ScalerPara_Pos_Value:						
	#if(XP_TO_ID==XP_TO_FLI32626)	
		case State_ScreenPara_UserResolution_Value:
	#endif
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
	#if(XP_TO_ID==XP_TO_FLI32626)
		case State_ColorPara_Saturation_Value:
	#endif
	#ifdef RGB_COLOR
		case State_ColorPara_Brightness_R_Value:
		case State_ColorPara_Brightness_G_Value:
		case State_ColorPara_Brightness_B_Value:
		case State_ColorPara_Contract_R_Value:
		case State_ColorPara_Contract_G_Value:
		case State_ColorPara_Contract_B_Value:
	#endif		
		case State_Volume_Set:
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


//This is used more
void DisplayMainInput(U8 addr,_InputSourceType input)
{
	U8 inputSrc1[6];
	switch(input)
	{
		case INPUTSOURCE_CVBS:
			inputSrc1[0]='C';
			inputSrc1[1]='V';
			inputSrc1[2]='1';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS2:
			inputSrc1[0]='C';
			inputSrc1[1]='V';
			inputSrc1[2]='2';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS3:
			inputSrc1[0]='C';
			inputSrc1[1]='V';
			inputSrc1[2]='3';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS4:
			inputSrc1[0]='C';
			inputSrc1[1]='V';
			inputSrc1[2]='4';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_SVIDEO:
			inputSrc1[0]='C';
			inputSrc1[1]='V';
			inputSrc1[2]='\0';
			break;
		case INPUTSOURCE_YPBPR:
			inputSrc1[0]='Y';
			inputSrc1[1]='P';
			inputSrc1[2]='b';
			inputSrc1[3]='P';
			inputSrc1[4]='r';
			inputSrc1[5]='\0';
			break;
		case INPUTSOURCE_VGA:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='1';
			inputSrc1[4]='\0';
			break;
	//#ifdef XP330		
		case INPUTSOURCE_VGA2:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='2';
			inputSrc1[4]='\0';
			break;
	//#endif		
		case INPUTSOURCE_DVI:
			inputSrc1[0]='D';
			inputSrc1[1]='V';
			inputSrc1[2]='I';
			inputSrc1[3]='1';
			inputSrc1[4]='\0';
			break;
	//#ifdef XP330
		case INPUTSOURCE_DVI2:
			inputSrc1[0]='D';
			inputSrc1[1]='V';
			inputSrc1[2]='I';
			inputSrc1[3]='2';
			inputSrc1[4]='\0';
			break;
	//#endif		
	//#if(ENABLE_HDMI)
		case INPUTSOURCE_HDMI:
			inputSrc1[0]='H';
			inputSrc1[1]='D';
			inputSrc1[2]='M';
			inputSrc1[3]='I';
			inputSrc1[4]='1';
			inputSrc1[5]='\0';
			break;
		case INPUTSOURCE_HDMI2:
			inputSrc1[0]='H';
			inputSrc1[1]='D';
			inputSrc1[2]='M';
			inputSrc1[3]='I';
			inputSrc1[4]='2';
			inputSrc1[5]='\0';
			break;
	//#endif

	
	}
	chinesedisplay(addr,inputSrc1);

	#ifdef XP360
		if(input == INPUTSOURCE_HDMI2)
			{
			if(g_PCPowerFlag)
				chinesedisplay(addr+4,":PC ON");
			else
				chinesedisplay(addr+3,":PC OFF");	
			}	
	#endif
}





void DisplayLCDInputSrc(U8 addr,U8 *chInter)
{	
	U8 inputSrc2[6];
	DisplayMainInput(addr,g_MainInput);
	if(g_MutiWindow!=SINGLEIMAGE)
	{			
		switch(g_SubInput)
		{
			case INPUTSOURCE_CVBS:
				inputSrc2[0]='C';
				inputSrc2[1]='V';
				inputSrc2[2]='B';
				inputSrc2[3]='S';
				inputSrc2[4]='1';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_CVBS2:
				inputSrc2[0]='C';
				inputSrc2[1]='V';
				inputSrc2[2]='B';
				inputSrc2[3]='S';
				inputSrc2[4]='2';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_CVBS3:
				inputSrc2[0]='C';
				inputSrc2[1]='V';
				inputSrc2[2]='B';
				inputSrc2[3]='S';
				inputSrc2[4]='3';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_CVBS4:
				inputSrc2[0]='C';
				inputSrc2[1]='V';
				inputSrc2[2]='B';
				inputSrc2[3]='S';
				inputSrc2[4]='4';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_SVIDEO:
				inputSrc2[0]='S';
				inputSrc2[1]='V';
				inputSrc2[2]='\0';
				break;
			case INPUTSOURCE_YPBPR:
				inputSrc2[0]='Y';
				inputSrc2[1]='P';
				inputSrc2[2]='b';
				inputSrc2[3]='P';
				inputSrc2[4]='r';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_VGA:
				inputSrc2[0]='V';
				inputSrc2[1]='G';
				inputSrc2[2]='A';
				inputSrc2[3]='1';
				inputSrc2[4]='\0';
				break;
			case INPUTSOURCE_VGA2:
				inputSrc2[0]='V';
				inputSrc2[1]='G';
				inputSrc2[2]='A';
				inputSrc2[3]='2';
				inputSrc2[4]='\0';
				break;
			case INPUTSOURCE_DVI:
				inputSrc2[0]='D';
				inputSrc2[1]='V';
				inputSrc2[2]='I';
				inputSrc2[3]='1';
				inputSrc2[4]='\0';
				break;
			case INPUTSOURCE_DVI2:
				inputSrc2[0]='D';
				inputSrc2[1]='V';
				inputSrc2[2]='I';
				inputSrc2[3]='2';
				inputSrc2[4]='\0';
				break;
		#if(ENABLE_HDMI)
			case INPUTSOURCE_HDMI:
				inputSrc2[0]='H';
				inputSrc2[1]='D';
				inputSrc2[2]='M';
				inputSrc2[3]='I';
				inputSrc2[4]='1';
				inputSrc2[5]='\0';
				break;
			case INPUTSOURCE_HDMI2:  //INPUTSOURCE_HDMI2
				inputSrc2[0]='H';
				inputSrc2[1]='D';
				inputSrc2[2]='M';
				inputSrc2[3]='I';
				inputSrc2[4]='2';
				inputSrc2[5]='\0';
				break;
		#endif
		
		}
		printStr(chInter);
		printStr(inputSrc2);
	}
}


static _InputSourceType TwoMode[2]={INPUTSOURCE_NUM,INPUTSOURCE_DVI};          //two mode recall

void LightInputSrcLED()
{
	U32 led1_32=0x01<<KEY_NUM;	
	switch(g_MainInput)
	{
		case INPUTSOURCE_CVBS:
			led1_32=0x01<<CVBS_KEY;
			break;
		case INPUTSOURCE_CVBS2:
			led1_32=0x01<<CVBS2_KEY;
			break;
		case INPUTSOURCE_CVBS3:
			led1_32=0x01<<CVBS3_KEY;
			break;
//		case INPUTSOURCE_YPBPR:
//			led1_32=0x01<<YPBPR_KEY;
//			break;
		case INPUTSOURCE_VGA:
			led1_32=0x01<<VGA_KEY;
			break;

		case INPUTSOURCE_VGA2:
			led1_32=0x01<<VGA2_KEY;
			break;
			
		case INPUTSOURCE_DVI:
			led1_32=0x01<<DVI_KEY;
			break;
		case INPUTSOURCE_DVI2:
			led1_32=0x01<<DVI2_KEY;
			break;
			
	//#if(ENABLE_HDMI)	
		case INPUTSOURCE_HDMI:
			led1_32=0x01<<HDMI_KEY;
			break;
	    #if(XP_ID==XP350)
		case INPUTSOURCE_HDMI2:
			led1_32=0x01<<HDMI2_KEY;
			break;
	    #endif
	//#endif
	
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
		case INPUTSOURCE_CVBS3:
			led1_32|=0x01<<CVBS3_KEY;
			break;
//		case INPUTSOURCE_YPBPR:
//			led1_32|=0x01<<YPBPR_KEY;
//			break;
		case INPUTSOURCE_VGA:
			led1_32|=0x01<<VGA_KEY;
			break;
		case INPUTSOURCE_DVI:
			led1_32|=0x01<<DVI_KEY;
			break;
	//#if(ENABLE_HDMI)	
		case INPUTSOURCE_HDMI:
			led1_32|=0x01<<HDMI_KEY;
			break;
	    #if(XP_ID==XP350)
		case INPUTSOURCE_HDMI2:
			led1_32|=0x01<<HDMI2_KEY;
			break;
	    #endif
	//#endif
	
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

#if(XP_TO_ID==XP_TO_FLI32626)
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

 State GetSwitchStateValue()
 {
 	switch(g_CurrentState)
 	{
		case State_Fade_Switch_Mode:
			if(g_MutiWindow==SINGLEIMAGE)
			{
				SetMultiWindowPara(SAMEPIPIMAGE);
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE)  //从PBP或PIP进入淡入淡出，双画面大小一样
			{	
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);	
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
			}
			else if(g_SwitchEffectValue==FuseSwitchIdx)
			{
				SetMultiWindowPara(FUSEPIPPARA1);
			}
			if(g_initPIPSrc==0)
			{
				g_initPIPSrc=1;
				g_SubInput=INITPIPSRC;
			}
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
				SetMultiWindowPara(SAMEPIPIMAGE);
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE) //从PBP或PIP进入快速切换，双画面大小一样
			{				
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
			}
			else if(g_SwitchEffectValue==FuseSwitchIdx)
			{
				SetMultiWindowPara(FUSEPIPPARA1);
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
				SetMultiWindowPara(SAMEPIPIMAGE);	
			}
			else if(g_MutiWindow==PIPIMAGE||g_MutiWindow==PBPIMAGE) //从PBP或PIP进入快速切换，双画面大小一样
			{				
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,MAIN);
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,MAIN);	
				SetScalerValue(HVSIZE,g_ScreenHSize,g_ScreenVSize,SUB);
				SetScalerValue(HVPOS,g_ScreenHPos,g_ScreenVPos,SUB);
				g_MainScalerHSize=g_ScreenHSize;
				g_MainScalerVSize=g_ScreenVSize;
				g_MainScalerHPos=g_ScreenHPos;
				g_MainScalerVPos=g_ScreenVPos;
				g_SubScalerHSize=g_ScreenHSize;
				g_SubScalerVSize=g_ScreenVSize;
				g_SubScalerHPos=g_ScreenHPos;
				g_SubScalerVPos=g_ScreenVPos;
			}
			SetMultiWindowPara(FUSEPIPPARA1);
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
			g_SwitchEffectValue=FuseSwitchIdx;
			break;
 	}
 	return g_CurrentState;
 }

static void DealSwitchEffect()
{	
	if(g_CurrentState==State_Fade_Switch_Mode)
	{
		DealCurrentState(State_Fuse_Switch_Mode);
	}
	else if(g_CurrentState==State_Fuse_Switch_Mode)
	{	
		DealCurrentState(State_Quick_Switch_Mode);
	}
	else 
	{		
		DealCurrentState(State_Fade_Switch_Mode);
	}
	chinesedisplay(0x90, "*");
	GetSwitchStateValue();
}

#endif

void SetInputSourceSwitch(_InputSourceType input)
{	
	static U8 modeIndex=0;
	if(g_CurrentState!=State_NewMode_Set)
		g_CurrentState=State_MainMenu_Not_Menu;
#if(XP_TO_ID==XP_TO_FLI32626)
	if(g_MutiWindow==SAMEPIPIMAGE)
	{
		if(g_SwitchEffectValue==FuseSwitchIdx)
		{	
			if(g_SubInput==input||g_MainInput==input) //如果与当前任一输入源相同
			{
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
		else if(g_SwitchEffectValue==FadeSwitchIdx||g_SwitchEffectValue==QuickSwitchIdx)
		{		
			if(g_NextPointImage==MAIN)
			{
				if(g_SubInput==input) //如果与当前输入源相同
				{
					return;	
				}
				else if(CheckDualSource(g_SubInput-1, input-1))  
				{
					if(input==g_MainInput)//如果与当前预备输入源相同
					{
						//g_DelayAndTakeFlag=EffectTake;
						DealSwapKey();
						return;
					}
					else
					{
						SetInputSource(input,MAIN);
						clearlcd();
						DisplayLCDInputSrc(0x81,"      --> ");
						LCDDisplayEffectInfo();
						//g_DelayAndTakeFlag=EffectDelayTake;
						delay(4);
						DealSwapKey();
					}
				}
				else//执行双画面硬切换
				{
					if(!CheckDualSource(g_MainInput-1, input-1))
					{
						LEDWarning(InputSrcToNumKey(input));
						return;	//此处为YPbPr与主子画面都不能双画面
					}
					else
					{
						SetInputSource(input,SUB);
						clearlcd();
						DisplayLCDInputSrc(0x81,"      --> ");
						LCDDisplayEffectInfo();
					}
				}
			}
			else
			{
				if(g_MainInput==input)//如果与当前输入源相同
				{
					return;	
				}
				else if(CheckDualSource(g_MainInput-1, input-1))//执行双画面硬切换
				{
					if(input==g_SubInput)//如果与当前预备输入源相同
					{
						//g_DelayAndTakeFlag=EffectTake;
						DealSwapKey();
						return;
					}
					else
					{
						SetInputSource(input,SUB);
						clearlcd();
						DisplayLCDInputSrc(0x81," <--      ");
						LCDDisplayEffectInfo();
						//g_DelayAndTakeFlag=EffectDelayTake;
						delay(4);
						DealSwapKey();
					}
				}
				else
				{
					if(!CheckDualSource(g_SubInput-1, input-1))
					{
						LEDWarning(InputSrcToNumKey(input));
						return;	//此处为YPbPr与主子画面都不能双画面
					}
					else
					{
						SetInputSource(input,MAIN);
						clearlcd();
						DisplayLCDInputSrc(0x81," <--      ");
						LCDDisplayEffectInfo();
					}
				}
			}
		}
	}
	else
#endif
	{		
		clearlcd();
		if(g_MutiWindow!=SINGLEIMAGE&&((g_ActiveImageValue==MAIN&&!CheckDualSource(g_SubInput-1, input-1))
				||(g_ActiveImageValue==SUB)&&!CheckDualSource(g_MainInput-1, input-1)))
				
		{
			if(g_BothSwitchFlag==__FALSE)
			{
			CannotSetDualChannelTip();
			return;
			}
			else
			{
				//g_MainInput=INPUTSOURCE_SDI;
				//SetInputSource(g_MainInput,MAIN);
			}
		}		
		
		if(g_BothSwitchFlag==__TRUE&&g_MutiWindow!=SINGLEIMAGE)
		{			
			SetInputSource(input,SUB);
			SetInputSource(input,MAIN);
		}
		else
			SetInputSource(input,g_ActiveImageValue);
		
		if(g_MutiWindow==SINGLEIMAGE)
		{
			if(g_MainInput!=input)
			{
			#ifdef ENABLE_6M16_CVBS4
				if(input==INPUTSOURCE_CVBS4&&TwoMode[(modeIndex+1)%2]==INPUTSOURCE_YPBPR) //防止视频4与分量swap
					TwoMode[(modeIndex+1)%2]=INPUTSOURCE_CVBS4;//因为YPbPr和CV4共用一个接口
				else if(input==INPUTSOURCE_YPBPR&&TwoMode[(modeIndex+1)%2]==INPUTSOURCE_CVBS4)
					TwoMode[(modeIndex+1)%2]=INPUTSOURCE_YPBPR;
				else
			#endif
					TwoMode[modeIndex++%2]=input;
			}
			if(g_LanguageFlag==CHINESE)
				chinesedisplay(0x80, g_InputStatusStr);
			else if(g_LanguageFlag==ENGLISH)
				chinesedisplay(0x80, g_InputStatusStrEn);
			DisplayLCDInputSrc(0x83," ");
		}
		else
		{
			chinesedisplay(0x80, g_DualCharacter);
			DisplayLCDInputSrc(0x81," +  ");
		}
		if(g_LanguageFlag==CHINESE)
			chinesedisplay(0x90, g_SyncToLEDStr);
		else//// if(g_LanguageFlag==ENGLISH)
			chinesedisplay(0x90, g_SyncToLEDStrEn);
	}
	Save(MODE1);

Read_EEPROM_Byte(E2ROM_AutoAdjustVGA_ADDR, &g_AutoAdjustVGA_Flag);
	if(g_AutoAdjustVGA_Flag)
					{
						 if(g_MainInput==INPUTSOURCE_VGA)
							{//当单画面时，当前通道已经为VGA时，再按此键可以自动调整VGA
								AutoAdjustVGA();
//								clearLineFromAddr(0x90);
//								if(g_LanguageFlag==CHINESE)
//								{
//									chinesedisplay(0x90, g_AutoAdjustVGAStr);
//								}	
//								else
//								{
//									chinesedisplay(0x90, g_AutoAdjustVGAStrEn);
//								}



//								chinesedisplay(0x97, "ok");
//								delay(30);
								
							}
//							else
//								SetInputSourceSwitch(NumKeyToInputSrc(keyVal));



//								 chinesedisplay(0x97, "ok");

					
					}


	DealEscKey();



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
		g_CurrentState=State_MainMenu_Not_Menu;
		clearlcd();
		if(g_LanguageFlag==CHINESE)
		{
			chinesedisplay(0x82, g_WarnningStr);
			chinesedisplay(0x90, g_SingleSwapWarnningStr);
		}
		else
		{
			chinesedisplay(0x82, g_WarnningStrEn);
			chinesedisplay(0x92, "No Switch");
		}
		LEDWarning(SWAP_KEY);
		delay(10);
		ReturnMainWin();
	}
}


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

	//20170213 lyg
	if(g_CurrentState==State_UserTry_Once_Password101)
	{
		if(g_UserTry_once_onoff) //打开
			chinesedisplay(0x90, g_Password101_off_Str);	
		else
			chinesedisplay(0x90, g_Password101_on_Str);	
	}
	else if(g_CurrentState==State_UserTry_Once_Password102)
	{
		if(g_UserTry_show_onoff) //打开
			chinesedisplay(0x90, g_Password102_off_Str);	
		else
			chinesedisplay(0x90, g_Password102_on_Str);	
	}
	else if(g_CurrentState==State_UserTry_Once_Password103)
	{
			chinesedisplay(0x90, g_Password103_Str);	
	}
	else if(g_CurrentState==State_UserTry_Once_Password104)
	{
			chinesedisplay(0x90, g_Password104_Str);	
	}
	else if(g_CurrentState==State_UserTry_Once_Password105)
	{
			chinesedisplay(0x90, g_Password105_Str);	
	}
	else if(g_CurrentState==State_UserTry_Once_Password100 ||g_CurrentState==State_UserTry_Once_Password100_new ||g_CurrentState==State_UserTry_Once_Password100_new1)
	{
			chinesedisplay(0x90, g_Password100_Str);	
	}
	else if(g_CurrentState== State_UserTry_OnceTel)
	{
			chinesedisplay(0x91,g_Tel_display);	
	}
}

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

//键盘已锁提示
//当键盘已锁时按下按键
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
	if(g_UserTry_show_onoff )
	{
		chinesedisplay(0x87, "*");
	}
	else if(g_UserTry_once_onoff)
	{
		chinesedisplay(0x87, "#");
	}
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x88, g_LockKeyStr+4);
	else
		chinesedisplay(0x88, "LOCK");		
}

void KeypadLock()
{	
	g_LockKeyFlag=LOCKKEYPAD;
	g_CurrentState=State_Lock_Key;
	clearlcd();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x90, g_LockKeyStr);
	else
		chinesedisplay(0x90, g_LockKeyStrEn);	
	delay(5);
	SetKeyLEDLight(KEY_NUM);
	DealEscKey();
	
	//20170213 lyg
	if(g_UserTry_show_onoff )
	{
		chinesedisplay(0x87, "*");
	}
	else if(g_UserTry_once_onoff)
	{
		chinesedisplay(0x87, "#");
	}
	
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x88, g_LockKeyStr+4);
	}
	else
	{
		chinesedisplay(0x88, "LOCK");	
	}
		
}



BOOL Check_OK_Cancel_State()
{
	switch(g_CurrentState)
	{
		case State_FactoryReset_OK:
		case State_FactoryReset_Yes:
		case State_CycleInput_Continue:
	#ifdef HOT_BACKUP
		case State_User_HotBackup_Continue:
	#endif
			return __TRUE;
	}
	return __FALSE;
}

#ifdef POWER_ON_PWD
void ScanKeypadForPowerOnPwd()
{
	static U32 LeisureCount=0;
	int i=0;
	U8 flag=0;
	static _KeypadNum Press_Key=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;
	static BOOL firsttipFlag=__TRUE;

#if (XP_ID!=XP310)	
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
			g_FlashLedFlag=__FALSE;
			flag=1; 
			keyVal=Press_Key;
			Press_Key=KEY_NUM;	
		}
		else
		{
			if(LeisureCount==0x349a4d)  //5s
			{
				if(firsttipFlag)
				{
					firsttipFlag=__FALSE;
					if(g_CurrentState!=State_MainMenu_Poweron_PWD)
						DealCurrentState(State_MainMenu_Poweron_PWD);
				}
			}
			else
				LeisureCount++;
		}
		KB_SCK_H;
	}	  
	KB_PLOAD_L;
#endif
	
	if(flag==1)
	{
		//执行键盘
		if(keyVal==ESC_KEY) 
		{
			DealEscKey();
		}
	#ifndef TWO_EC11_BOARD	
		else if(keyVal==OK_KEY)
		{
			DealOkKey();
		}
	#endif
		else if(keyVal==UP_KEY)
		{
			if(g_CurrentState==State_MainMenu_Menu)
				DealCurrentState(State_MainMenu_Poweron_PWD);
			else
				DealUpKey();
		}
		else if(keyVal==DOWN_KEY)
		{
			if(g_CurrentState==State_MainMenu_Menu_Logo)
				DealCurrentState(State_MainMenu_Poweron_PWD);
			else
				DealDownKey();
		}
		else if(keyVal==BACKSPACE_KEY)
		{
			DealBackspaceKey();
		}
		else
		{
			switch(g_CurrentState)
			{
				case State_MainMenu_Poweron_PWD:
					DealScreenNumber(keyVal,LCDADDR91,8);
					break;
			}
		}
	}
}
#endif

void ScanKeypadForSerial()
{
	static U32 LeisureCount=0;
	int i=0;
	U8 flag=0;
	static _KeypadNum Press_Key=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;
	static BOOL firsttipFlag=__TRUE;

#if (XP_ID!=XP310)	
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
			g_FlashLedFlag=__FALSE;
			flag=1; 
			keyVal=Press_Key;
			Press_Key=KEY_NUM;	
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
						chinesedisplay(0x93,  g_ContactTel2);
					}
					else if(g_LanguageFlag==ENGLISH)
					{
						chinesedisplay(0x80,  "Please Payment!");
						chinesedisplay(0x90,  "For a Serial Number");
					}
				}
			}
			else
				LeisureCount++;
		}
		KB_SCK_H;
	}	  
	KB_PLOAD_L;
#endif
	
	if(flag==1)
	{
		//执行键盘
		if(keyVal==ESC_KEY) 
		{
			DealEscKey();
		}
	#ifndef TWO_EC11_BOARD	
		else if(keyVal==OK_KEY)
		{
			DealOkKey();
		}
	#endif
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
					DealScreenNumber(keyVal,LCDADDR91,6);
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
			if(Bold)//0-254
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
			if(Aold)
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

#ifdef TWO_EC11_BOARD
U8 CodingWitchPollingU15()
{
	static U8 Aold,Bold; //用来存储上一次调用时编码开关两引脚的电平
	static U8 st=0;  //用来存储以前是否出现两引脚都为高电平的状态
	static U8 tmp=0;

	if(BMA2&&BMB2)
		st=1;
	if(st)
	{
		if(BMA2==0&&BMB2==0)
		{
			if(Aold)
			{
				st=0;
				if(tmp==255)
					tmp=0;
				else
				{
					tmp++;
					DealUpKey();
				}
			}
			if(Bold)
			{
				st=0;
				if(tmp==0)
					tmp=255;
				else
				{
					tmp--;					
					DealDownKey();
				}
			}
		}
	}
	Aold=BMA2;
	Bold=BMB2;
	return tmp;
}
#endif

void ScanEC11()
{
	CodingWitchPolling();
#ifdef TWO_EC11_BOARD
	CodingWitchPollingU15();
#endif
}
#endif

void ScanKeypad()  //scan keypad
{
	static U32 LeisureCount=0,LeisureCount1=0,OffBackCount=0;
	int i=0;
	U8 flag=0;
	static U8 KeyACC=0;	
	static _KeypadNum Press_Key=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;	
	
#ifdef USE_INFRARED_IR
	if(Remote_Scan())
	{
		Press_Key=KEY_NUM;
		KeyACC=0;
		g_AdvertisingFlag=__FALSE;
		switch(IRcord[2])
		{
			// 1(局部全屏)
			case 0xA2: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM1_KEY;
				}
				else
				{
				}
				break;
			// 2(A/B)
			case 0x62: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM2_KEY;
				}
				else
				{
				}
				break;
			// 3(单双)
			case 0xE2:
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM3_KEY;
				}
				else
				{
				}
				break;
			// 4(冻结)
			case 0x22: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM4_KEY;
				}
				else
				{
				}
				break;
			// 5
			case 0x02: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM5_KEY;
				}
				else
				{
				}
				break;
			// 6
			case 0xC2: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM6_KEY;
				}
				else
				{
				}
				break;
			// 7
			case 0xE0: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM7_KEY;
				}
				else
				{
				}
				break;
			// 8
			case 0xA8: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM8_KEY;
				}
				else
				{
				}
				break;
			// 9
			case 0x90: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM9_KEY;
				}
				else
				{
				}
				break;
			// 0
			case 0x68: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM0_KEY;
				}
				else
				{
				}
				break;
			// UP
			case 0x98: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=UP_KEY;
				}
				else
				{
				}
				break;
			// down
			case 0xB0: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=DOWN_KEY;
				}
				else
				{
				}
				break;
			// menu
			case 0x30:
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=MENU_KEY;
				}
				else
				{
				} 
				break;
			// save(	保存模式)
			case 0x18: 
				if(RmtCnt<4)
				{
					Save(MODE1);
				}
				else
				{
					KeyACC=LOGNTIME;
					Press_Key=DUAL_KEY;
				}			
				break;
			// recall
			case 0x7A: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=NUM0_KEY;
				}
				else
				{
				}
				break;
			// A
			case 0x10: 
			
				break;
			// V
			case 0x38: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=FULLPART_KEY;
				}
			
				break;
			// AV
			case 0x5A: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=DUAL_KEY;
				}
							
				break;
			// D-DVI(退格,长按)
			case 0x42: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=AB_KEY;
				}
				else
				{
				}
				break;
			// ESC(锁键,长按)
			case 0x4A: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=ESC_KEY;
				}
				else
				{
					KeyACC=LOGNTIME;
					Press_Key=LOCK_KEY;
				}
				break;
			// ENTER(调试，长按)
			case 0x52: 
				if(RmtCnt<4)
				{
				flag=1;
				keyVal=OK_KEY;
				}
				else
				{
					KeyACC=LOGNTIME;
					Press_Key=SWAP_KEY;
				}
				break;
		}
		RmtCnt=0;
	}
#endif

#if (XP_ID!=XP310)
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

				if(g_OffBackFlag==OffBackLight)//若背光关闭
				{
					LCD_BK_H;//开LCD屏幕背光
					LCDStart2OffBack();//初始化LCD参数
					g_OffBackFlag=UNOffBackLight;//置 关屏标志 为 未关屏状态
					OffBackCount=0;//关屏计数器归0
				}
				OffBackCount = 0;
				LeisureCount=0;   //只要有按键，该值归零
				LeisureCount1=0;
			  	KeyACC++;			
				Press_Key=(_KeypadNum)(KEY_NUM-1-i);  		
				g_AdvertisingFlag=__FALSE;
			}
		#ifndef TWO_EC11_BOARD
			//长按上下键的时候，按键不弹起来可以发命令
			if(KeyACC>=5&&(Press_Key==UP_KEY||Press_Key==DOWN_KEY)&&(IsSetColorParaValueState()||IsSetHVParaValueState()))
			{
				flag=1;  //当在按下状态下，处理按键
				keyVal=Press_Key;  
			}
		#endif
		}
		else if(Press_Key==(KEY_NUM-1-i))  //当按键弹起来
		{	//长按锁键弹起来不做任何处理，排除长按锁键		
			if(KeyACC>0&&!(KeyACC>=LOGNTIME&&(Press_Key==LOCK_KEY||Press_Key==SWAP_KEY
				||Press_Key==FREEZE_KEY||Press_Key==NUM1_KEY||Press_Key==NUM2_KEY
				||Press_Key==NUM3_KEY||Press_Key==DUAL_KEY
		#ifndef TWO_EC11_BOARD
				||Press_Key==MENU_KEY
		#else
				||Press_Key==NUM0_KEY		
		#endif
		
		#if(XP_ID==XP520)
				||Press_Key==DUAL_KEY 
		#endif 
			
		#ifdef XP360
				||Press_Key==NUM9_KEY
		#endif
				||Press_Key==VGA_KEY
			))) 
			{
				flag=1;   
				keyVal=Press_Key;  //按键弹起的时候,对KeyVal赋值
			}
			
			if(g_LockKeyFlag==UNLOCKKEYPAD)
			{
				if(KeyACC>=LOGNTIME&&Press_Key==LOCK_KEY)
					;//长按锁键，不亮灯
				else
				{	
					SetKeyLEDLight(Press_Key);  //turn on keypad led	
				}
			}
			else if(g_LockKeyFlag==LOCKKEYPAD&&KeyACC>0)
			{
				KeypadLockedPrompt();  //提示用户按键已锁
			}
			Press_Key=KEY_NUM;
			KeyACC=0;	
			g_FlashLedFlag=__FALSE;
		}
		else   //此处是没有按按键
		{

//			if(g_OffBackLightTime!=0)//如果关背光时间!=0 关背光时间为锁键盘时间的5倍
//			{
//				if(OffBackCount==(g_OffBackLightTime*0x1333320))//若计数器时间==关屏时间  //0x1000000大约为50秒
//				{
//					if(g_OffBackFlag==UNOffBackLight&&KeyACC==0)//若关背光状态标志 为 未关&&没有按键 
//					{
//						LCD_BK_L;//关LCD屏幕背光
//						g_OffBackFlag=OffBackLight;	//置 关背光标志 为 关状态					
//						OffBackCount=0;//关屏计数器置0，停止计数						
//					}
//				}
//				else if(g_OffBackFlag==UNOffBackLight)//若关背光状态标志 为 未关
//				{				
//					OffBackCount++;//关屏计数器计数
//				}
//			}
//			else if(g_OffBackLightTime==0)//如果关背光时间为0 默认30分钟关背光
//			{
//				if(OffBackCount==(30*0x1333320))
//				{
//					if(g_OffBackFlag==UNOffBackLight&&KeyACC==0)
//					{
//						LCD_BK_L;//关LCD屏幕背光
//						g_OffBackFlag=OffBackLight;						
//						OffBackCount=0;						
//					}
//				}
//				else if(g_OffBackFlag==UNOffBackLight)
//				{				
//					OffBackCount++;
//				}
//			}


			if(g_LockKeypadTime!=0)//ten minutes lock keypad	
			{
				if(LeisureCount==(g_LockKeypadTime*0x2773B9D))
				{//KeyACC!=0 如果某个按键按下去没松开时不自动锁键盘
					if(!g_WizadFlag&&g_LockKeyFlag==UNLOCKKEYPAD&&KeyACC==0)   
					{
						LeisureCount=0; 
						KeypadLock();//十分钟后自动锁键盘
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
		KB_SCK_H;
	}	  
	KB_PLOAD_L;
#endif

	if(g_LockKeyFlag==UNLOCKKEYPAD)
	{
		if(flag==1)
		{
			flag=0;  

			 //执行键盘
			if(keyVal==ESC_KEY) 
			{
				DealEscKey();
			}
		#ifndef TWO_EC11_BOARD
			else if(keyVal==OK_KEY)
			{
				DealOkKey();
			}
		#endif
			
			else if(g_CurrentState==State_NewMode_Set)
			{
				if(keyVal==NUM2_KEY)
					TwoImageSetWizard();
				else
					LEDWarning(keyVal); 
			}
			else if(!Check_OK_Cancel_State())  //判断是否的选择
			{
				if(keyVal==BACKSPACE_KEY) //退格按键
					DealBackspaceKey();
			#ifndef TWO_EC11_BOARD
				else if(keyVal==UP_KEY)//up
					DealUpKey();
				else if(keyVal==DOWN_KEY)//down
					DealDownKey();
			#endif	
				else if(keyVal==FULLPART_KEY)//A/B按键
				{
					if(!DealStepKey())
						DealFullOrPartKey();
				}
				else if(IsSetHVParaValueState())	
				{	//如果设置参数数值，只有按数字键有效，按其他键会闪灯
					if(g_H_VFlag==__TRUE)
						DealScreenNumber(keyVal,LCDADDR91,4);
					else
						DealScreenNumber(keyVal,0x94,4);
				}
				else if( IsSetColorParaValueState())
					DealScreenNumber(keyVal,LCDADDR91,4);
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
#ifdef WORKER_ID_REC						
					case State_Input_PackagingName:
					case State_Input_TestName:
					case State_Input_SaleName:
#endif						
					case State_User_Input_Logo:	
						DealScreenNumber(keyVal,LCDADDR91,5);
						break;					
					case State_LockPadTime_Set:			
#ifdef SUPPORT_ANYHZ
					//case State_OutputHz_25Hz:	
#endif
						DealScreenNumber(keyVal,LCDADDR91,2);
						break;

					case State_User_Input_TryDays:
					case State_HowLong_Save:
						DealScreenNumber(keyVal,LCDADDR91,3);
						break;
					case State_User_Input_SerialNumber:
					case State_User_Set_SerialNumber:
					case State_DebugMode_Start:
							DealScreenNumber(keyVal,LCDADDR91,6);
						break;
					case State_User_Old_SerialNumber:
						DealScreenNumber(keyVal,LCDADDR91,8);
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
							DealScreenNumber(keyVal,0x85,8);
						break;
					case 	State_UserTry_Once_Password101:
					case State_UserTry_Once_Password102:
					case State_UserTry_Once_Password103:
					case State_UserTry_Once_Password104:
					case State_UserTry_Once_Password105:	
					case State_UserTry_Once_Password100:		
							DealScreenNumber(keyVal,0x87,6);
						break;
					case State_UserTry_Once_Password100_new:		
							DealScreenNumber(keyVal,0x86,4);
						break;
					case State_UserTry_Once_Password100_new1:		
							DealScreenNumber(keyVal,0x88,4);
						break;
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
				#ifdef HOT_BACKUP
					case State_User_HotBackup:
						if(!g_SetBackupInputFlag)
							DealInputNumber(keyVal,0x86);
						else
							DealInputNumber(keyVal,0x96);
						break;
				#endif
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
						case CVBS3_KEY:
//						case YPBPR_KEY:
						case VGA_KEY:
						case DVI_KEY:
					#if(ENABLE_HDMI)	
						case HDMI_KEY:
					    #if(XP_ID==XP350)
						case HDMI2_KEY:  
					    #endif
					#endif
					
					//#ifdef TWO_EC11_BOARD
						case VGA2_KEY:
						case DVI2_KEY:
					//#endif

							SetInputSourceSwitch(NumKeyToInputSrc(keyVal));    //HDMI 1
							break;
					#ifndef TWO_EC11_BOARD	
//						case NUM5_KEY:
//							DealSaveKey();
//							g_SaveModeValue=MODE1;
//							SetCurrentStateValue(KEY_ENTER, State_SavePara_Mode_check, NULL);
//							break; 
						case NUM0_KEY:
							DealUserModeKey();
							break;	
					#endif
					#if(XP_ID==XP320||XP_ID==XP310)				
						case AB_KEY:
							DealActiveImageKey();
							break;
					#endif
				#ifndef TWO_EC11_BOARD								
						case MENU_KEY://menu
					#if(XP_ID==XP320||XP_ID==XP310||XP_ID==XP520)
							DealMenuKey();
					#elif(XP_ID==XP350)
						DealAudioKey(MUTE);
						/*	
						#if(!defined XP330)&&(!defined XP360)		//没有定义330或360
							DealAudioKey(MUTE);
						#else
							clearlcd();
							if(g_LanguageFlag==CHINESE)
							{
								chinesedisplay(0x82, g_WarnningStr);
								chinesedisplay(0x90, g_NoUseStr);
							}
							else
							{
								chinesedisplay(0x82, g_WarnningStrEn);
								chinesedisplay(0x92, "No Support@This Type");
							}
							LEDWarning(SWAP_KEY);	
						#endif
						*/
					#endif
							break;
				#endif
						case FREEZE_KEY:
							DealFreezeKey();
							break;
						case SWAP_KEY:
							DealSwapKey();
							break;
						case DUAL_KEY:
					#if(XP_ID==XP520)
							DealSwitchEffect();					
					#else
							DealMultiWindowKey();
					#endif
							break;
						default:
							LEDWarning(keyVal); 	
							break;
					       }
					       break;
					}
				}
			}
			
			else if(Check_OK_Cancel_State())  //如果判断是否，OK和ESC
			{
				LEDWarning(keyVal); 	
			}
		}	 
		else if(KeyACC==LOGNTIME)
		{				
			if(Press_Key==FREEZE_KEY)
				DealCurrentState(State_FactoryReset_Yes);
		#if(XP_ID==XP520)
			else if(Press_Key==DUAL_KEY)
				DealMultiWindowKey();
		#endif
			else if(Press_Key==NUM1_KEY)
				DealCurrentState(State_NewMode_Set);
			else if(Press_Key==NUM2_KEY)
				DealCurrentState(State_ColorPara_Brightness);
			else if(Press_Key==NUM3_KEY)
			{
				DealCurrentState(GetCurrentOuputResolution());
				chinesedisplay(0x90, "*");
			}
		#ifndef TWO_EC11_BOARD
		   #if(XP_ID==XP350)
			else if(Press_Key==VOLUME_KEY)
				DealVolumeKey();
		   #else
			else if(Press_Key==MENU_KEY)
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
				Write_EEPROM_Byte(E2ROM_OutputHZ_ADDR,g_OutputHzVal);
			}
		   #endif
		#else		
			else if(Press_Key==NUM0_KEY)
				DealUserModeKey();
		#endif
			else if(Press_Key==DUAL_KEY)
				DealSaveKey();
		#ifdef XP360
			else if(Press_Key==NUM9_KEY)
			{
				STM32_CTR_PC_OnOff();
				
 				////PC_POWER_ONOFF_LCD_DISPLAY();
				
				delay(8);
				LightInputSrcLED();
				DealEscKey();
			}
		#endif
			else if(Press_Key==VGA_KEY)
				AutoAdjustVGA();
		}
       }
	if(KeyACC==LOGNTIME&&Press_Key==SWAP_KEY)
	{
		g_LockKeyFlag=UNLOCKKEYPAD;
		DealCurrentState(State_DebugMode_Start);
	}
	if(KeyACC==LOGNTIME&&Press_Key==LOCK_KEY)//ESC_KEY
	{	
		if(g_LockKeyFlag==UNLOCKKEYPAD)
		{
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

void Start_LED_Flashing(U8 num)
{
	g_FlashLedFlag=__TRUE;
	g_FlashLedNum=num;
}

void ReturnMainWin()
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
	g_CurrentState=State_MainMenu_Menu;
	LightInputSrcLED();
}


void SetKeyLEDLight(_KeypadNum key)  //only turn one on
{	
	U32 led1_32=0xffffff;
	led1_32=0x01<<key;
	led1_32=~led1_32;
	SetKeypadLED(led1_32);  //turn on keypad led	
}




 void SetKeypadLED(U32 LedOfKey)   //keypad led
 {
	   int i;
	  
	   for(i=0;i<KEY_NUM;i++)
	   {
	     	if(LedOfKey&(0x01<<(KEY_NUM-1)))
		 	LED_IN_H;//FIO0SET |= 0x01000000;    //p0.24 led_in	   sd=1
		else
		 	LED_IN_L;//FIO0CLR |= 0x01000000;    //p0.24 led_in	   sd=0
		 
		 LED_SRCK_L;//FIO0CLR |= 0x02000000;    //p0.25 led_srck  sck=0
		 delay1000(1);
		 LED_SRCK_H;//FIO0SET |= 0x02000000;    //p0.25 led_srck  sck=1
		 LedOfKey=LedOfKey<<1;
	   }	

	   LED_RCK_L;// FIO0CLR |= 0x04000000;    //p0.26 led_rck   st=0
	   delay1000(1);
	   LED_RCK_H;// FIO0SET |= 0x04000000;    //p0.26 led_rck   st=1
	   delay1000(1);
	   LED_RCK_L;// FIO0CLR |= 0x04000000;    //p0.26 led_rck   st=0
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
		//#ifdef POWER_ON_PWD
		//	State_MainMenu_Poweron_PWD,
		//#else
			State_MainMenu_Menu_Logo,
		//#endif
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
		//0x85,g_MachineTypeXPStr,g_MachineTypeXPStr,
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
		0x90,
		" ",
		" ",
		{State_MainMenu_Menu_InOutput,   //up
			State_MainMenu_Menu_Logo,
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL
		
	},
	//State_MainMenu_Menu_Logo
	{
		0x80,g_SupportTechStr,g_SupportTechStrEn,
		0x90,g_TechSupportTel1,g_TechSupportTel1,
		{
			State_MainMenu_Menu_Current_Date,   //up			
		//#ifdef POWER_ON_PWD
		//	State_MainMenu_Poweron_PWD,
		//#else
			State_MainMenu_Menu,  //down
		//#endif
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL		
	},
	
#ifdef POWER_ON_PWD
	//State_MainMenu_Poweron_PWD,
	{
		0x80,g_InputTodayPwdStr,"Input Today Password",
		0x91,"_","_",
		{
			State_MainMenu_Menu_Logo,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL		
	},
#endif
	//State_MainMenu_Not_Menu
	{
		0x80,
		NULL,
		NULL,
		0x90,
		NULL,
		NULL,
		{State_MainMenu_Not_Menu,   //up
			State_MainMenu_Not_Menu,  //down
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
#if(XP_TO_ID==XP_TO_FLI32626)
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
#endif
	//State_ActiveImage_Main,
	{	
		LCDADDR80,g_ActiveImage,g_ActiveImageEn,
		SELIMGADDR,g_MainImage,g_MainImageEn,
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
		SELIMGADDR,g_SubImage,g_SubImageEn,
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
	},	
	//State_MainMenu_Menu_ScreenPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_ScreenPara,g_ScreenParaEn,
		{		
			State_MainMenu_Menu_DebugMode,//State_AdvancePara_OutputFormat,
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
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_ScreenPara_UserResolution,	
		#else
			State_ScreenPara_Pos,
		#endif
			State_ScreenPara_Pos,
			State_ScreenPara_Size_Value,			
			State_MainMenu_Menu_ScreenPara
		},NULL,
		NULL,
		NULL
	},
	//State_ScreenPara_Size_Value
	{
		LCDADDR80,g_SelScreenHStr,g_SelHStrEn,
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
		LCDADDR90,g_StartPosStr,"2.Start Position",
		{
			State_ScreenPara_Size,
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_ScreenPara_UserResolution,	
		#else
			State_ScreenPara_Size,
		#endif
			State_ScreenPara_Pos_Value,
			State_MainMenu_Menu_ScreenPara
		},NULL,
		NULL,NULL
	},
	//State_ScreenPara_Pos_Value,
	{
		LCDADDR80,g_OutHPosStr,g_SelHStrEn,
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
#if(XP_TO_ID==XP_TO_FLI32626)
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
		LCDADDR80,g_UserResolutionHStr,"3.User Resolution",
		LCDADDR91,NULL,NULL,
		{
			State_ScreenPara_UserResolution_Value,
			State_ScreenPara_UserResolution_Value,
			State_ScreenPara_UserResolution_Value,			
			State_ScreenPara_UserResolution
		},&SetCurrentStateValue,
		NULL,
		NULL
	},
#endif
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
		LCDADDR80,g_SelScalerHStr,g_SelHStrEn,
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
		LCDADDR90,g_StartPosStr,"2.Start Position",
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
		LCDADDR80,g_OutHPosStr,g_SelHStrEn,
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
			State_MainMenu_Menu_ColorPara,//
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
		LCDADDR90,g_StartPosStr,"2.Start Position",
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
		LCDADDR80,g_InHPosStr,g_SelHStrEn,
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
			State5_MainMenu_Menu_AdvancePara,
			State_ColorPara_Brightness,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_Brightness,g_BrightnessEn,
		{
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_ColorPara_ADC,
		#else
			State_ColorPara_Contrast,
		#endif
			State_ColorPara_Contrast,
			State_ColorPara_Brightness_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Brightness_Value,
	{
		LCDADDR80,g_Brightness,g_BrightnessEn,
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
		LCDADDR90,g_Contract,g_ContractEn,
		{
			State_ColorPara_Brightness,
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_ColorPara_Saturation,
		#else
			State_ColorPara_Brightness,
		#endif
			State_ColorPara_Contrast_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contrast_Value,
	{
		LCDADDR80,g_Contract,g_ContractEn,
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
#ifdef RGB_COLOR
	//State_ColorPara_Brightness_R,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_BrightnessR,g_BrightnessREn,
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
		LCDADDR80,g_BrightnessR,g_BrightnessREn,
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
		LCDADDR90,g_BrightnessG,g_BrightnessGEn,
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
		LCDADDR80,g_BrightnessG,g_BrightnessGEn,
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
		LCDADDR90,g_BrightnessB,g_BrightnessBEn,
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
		LCDADDR80,g_BrightnessB,g_BrightnessBEn,
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
	//State_ColorPara_Contract_R,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_ContractR,g_ContractREn,
		{
			State_ColorPara_Contrast,
			State_ColorPara_Contract_G,
			State_ColorPara_Contract_R_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contract_R_Value,
	{
		LCDADDR80,g_ContractR,g_ContractREn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contract_R_Value,
			State_ColorPara_Contract_R_Value,
			State_ColorPara_Contract_R_Value,			
			State_ColorPara_Contract_R
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contract_G,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_ContractG,g_ContractGEn,
		{
			State_ColorPara_Contract_R,
			State_ColorPara_Contract_B,
			State_ColorPara_Contract_G_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contract_G_Value,
	{
		LCDADDR80,g_ContractG,g_ContractGEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contract_G_Value,
			State_ColorPara_Contract_G_Value,
			State_ColorPara_Contract_G_Value,			
			State_ColorPara_Contract_G
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
	//State_ColorPara_Contract_B,
	{
		LCDADDR80,g_ColorPara,g_ColorParaEn,
		LCDADDR90,g_ContractB,g_ContractBEn,
		{
			State_ColorPara_Contract_G,
			State_ColorPara_Brightness,
			State_ColorPara_Contract_B_Value,			
			State_MainMenu_Menu_ColorPara
		},NULL,
		NULL,NULL
	},
	//State_ColorPara_Contract_B_Value,
	{
		LCDADDR80,g_ContractB,g_ContractBEn,
		LCDADDR90,NULL,NULL,
		{
			State_ColorPara_Contract_B_Value,
			State_ColorPara_Contract_B_Value,
			State_ColorPara_Contract_B_Value,			
			State_ColorPara_Contract_B
		},&SetCurrentStateValue,
		NULL,
		&GetCurrentWindowValue
	},
#endif	
#if(XP_TO_ID==XP_TO_FLI32626)
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
	//State_ColorPara_ADC,
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
	//State_ColorPara_ADC_Calibration,
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
#endif

	//5 State5_MainMenu_Menu_AdvancePara,
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
			State_AdvancePara_Language,//
			State52_AdvancePara_OutputHz,
			State_OutputFormat_640x480,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
	//State_OutputFormat_640x480,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"640x480","640x480",
		{
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_OutputFormat_1920x1200,	
		#else
			State_OutputFormat_1920x1080,
		#endif
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
			State_OutputFormat_1024x768,
			State_OutputFormat_800x600,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1024x768,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1024x768","1024x768",
		{
			State_OutputFormat_800x600,
			State_OutputFormat_1280x720,
			State_OutputFormat_1024x768,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
#if(XP_TO_ID==XP_TO_FLI32626)
	//State_OutputFormat_1280x720,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x720","1280x720",
		{
			State_OutputFormat_1024x768,
			State_OutputFormat_1280x1024,
			State_OutputFormat_1280x720,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
#endif
	//State_OutputFormat_1280x720
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x720","1280x720",
		{
			State_OutputFormat_1024x768,
			State_OutputFormat_1280x768,
			State_OutputFormat_1280x720,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1280x768
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x768","1280x768",
		{
			State_OutputFormat_1280x720,
			State_OutputFormat_1280x800,
			State_OutputFormat_1280x768,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1280x800
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x800","1280x800",
		{
			State_OutputFormat_1280x768,
			State_OutputFormat_1280x960,
			State_OutputFormat_1280x800,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1280x960
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1280x960","1280x960",
		{
			State_OutputFormat_1280x800,
			State_OutputFormat_1280x1024,
			State_OutputFormat_1280x960,			
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
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_OutputFormat_1280x720,	
		#else
			State_OutputFormat_1280x960,
		#endif
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
			State_OutputFormat_1400x1050,
			State_OutputFormat_1366x768,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1400x1050
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1400x1050","1400x1050",
		{
			State_OutputFormat_1366x768,
			State_OutputFormat_1440x900,
			State_OutputFormat_1400x1050,			
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
			State_OutputFormat_1400x1050,
			State_OutputFormat_1600x900,
			State_OutputFormat_1440x900,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1600x900,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1600x900","1600x900",
		{
			State_OutputFormat_1440x900,
			State_OutputFormat_1600x1200,
			State_OutputFormat_1600x900,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
	//State_OutputFormat_1600x1200,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1600x1200","1600x1200",
		{
			State_OutputFormat_1600x900,
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
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_OutputFormat_1920x1200,	
		#else
			State_OutputFormat_640x480,
		#endif
			State_OutputFormat_1920x1080,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
#if(XP_TO_ID==XP_TO_FLI32626)
	//State_OutputFormat_1920x1200,
	{
		LCDADDR80,g_outputFormatstr,g_outputFormatstrEn,
		LCDADDR91,"1920x1200","1920x1200",
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
		LCDADDR91,"2048x1152","2048x1152",
		{
			State_OutputFormat_1920x1200,
			State_OutputFormat_640x480,
			State_OutputFormat_2048x1152,			
			State_AdvancePara_OutputFormat
		},&SetCurrentStateValue,
		&GetOutFormatStateValue,
		NULL
	},
#endif
	//State52_AdvancePara_OutputHz,
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_OutputHzStr,g_OutputHzStrEn,
		{
			State_AdvancePara_OutputFormat,
			State_AdvancePara_Audio,
			State_OutputHz_30Hz,			
			State5_MainMenu_Menu_AdvancePara
		},NULL,
		NULL,NULL
	},
	//State_OutputHz_30Hz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"30HZ","30HZ",
		{
			State_OutputHz_25Hz,//State_OutputHz_60Hz,//State_OutputHz_NumHz,
			State_OutputHz_50Hz,	
			State_OutputHz_30Hz,		
			State52_AdvancePara_OutputHz
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
			State52_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
	//State_OutputHz_60Hz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"60HZ","60HZ",
		{
			State_OutputHz_50Hz,
			State_OutputHz_25Hz,//State_OutputHz_NumHz,
			State_OutputHz_60Hz,			
			State52_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
#ifdef SUPPORT_ANYHZ	
	//State_OutputHz_NumHz,
	{
		LCDADDR80,g_OutputHzStr,g_OutputHzStrEn,
		LCDADDR91,"25HZ","25HZ",
		{
			State_OutputHz_60Hz,
			State_OutputHz_30Hz,
			State_OutputHz_25Hz,			
			State52_AdvancePara_OutputHz
		},&SetCurrentStateValue,
		&GetCurrentOutputHzState,NULL
	},
#endif	
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
			State52_AdvancePara_OutputHz,
			State_AdvancePara_LockPadTime,
			State_Audio_Main,	
			State5_MainMenu_Menu_AdvancePara
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
			State5_MainMenu_Menu_AdvancePara
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
			State5_MainMenu_Menu_AdvancePara
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
			State_FactoryReset_OK,
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
			State5_MainMenu_Menu_AdvancePara
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
			State5_MainMenu_Menu_AdvancePara
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
	
//	6 State_MainMenu_Menu_UserPara,
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_UserParaStr,g_UserParaStrEn,
		{
			State5_MainMenu_Menu_AdvancePara,
			State_MainMenu_TimeSchedule,
			State_UserPara_SaveMode,	
			State_MainMenu_Menu
		},NULL,
		NULL,NULL
	},
//	6.1 State_UserPara_SaveMode,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SaveModeStr,g_SaveModeStrEn,
		{
		#ifdef TCP_IP
			State_UserPara_SetStaticIP,
		#else
			#ifdef XP360
			State66_PC_PowerOnOff,
			#else
			State_UserPara_SetHotBackup,
			#endif
		#endif
			State_UserPara_RecallMode,
			State_SavePara_Number,	
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL
	},
//	6.2 State_UserPara_RecallMode,
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
	//6.3 State_UserPara_SetCurrentTime,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetTimeStr,g_SetTimeStrEn,
		{
			State_UserPara_RecallMode,
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_UserPara_SetFadeTime,
		#else
			State_UserPara_SetHotBackup,
		#endif
			State_UserPara_SetCurrentTime,	
			State_MainMenu_Menu_UserPara
		},&SetCurrentStateValue,
		NULL,NULL		
	},
	//6.4 State_UserPara_SetFadeTime
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		{
			State_UserPara_SetCurrentTime,
			State_UserPara_SetHotBackup,
	#if(XP_TO_ID==XP_TO_FLI32626)
			State_UserPara_SetFadeTime_1000,	
	#else
			State_UserPara_SetFadeTime,
	#endif
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL		
	},
#if(XP_TO_ID==XP_TO_FLI32626)
	//State_UserPara_SetFadeTime_500
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"0.5","0.5",
		{
			State_UserPara_SetFadeTime_1500,
			State_UserPara_SetFadeTime_1000,
			State_UserPara_SetFadeTime_500,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
	//State_UserPara_SetFadeTime_1000
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"1.0","1.0",
		{
			State_UserPara_SetFadeTime_500,
			State_UserPara_SetFadeTime_1500,
			State_UserPara_SetFadeTime_1000,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
	//State_UserPara_SetFadeTime_1500
	{
		LCDADDR80,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		LCDADDR91,"1.5","1.5",
		{
			State_UserPara_SetFadeTime_1000,
			State_UserPara_SetFadeTime_500,
			State_UserPara_SetFadeTime_1500,	
			State_UserPara_SetFadeTime
		},&SetCurrentStateValue,
		&GetFadeTimeStateValue,NULL		
	},
#endif
	//6.5 State_UserPara_SetHotBackup
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetHotBackupStr,g_SetHotBackupStrEn,
		{
		#if(XP_TO_ID==XP_TO_FLI32626)
			State_UserPara_SetFadeTime,
		#else
			State_UserPara_SetCurrentTime,
		#endif
		
		   #ifdef TCP_IP
			State_UserPara_SetStaticIP,
		   #else
			#ifdef XP360
		   	State66_PC_PowerOnOff,
		   	#else
			State_UserPara_SaveMode,
			#endif
		   #endif
	#ifdef HOT_BACKUP
			State_HotBackup_Set,	
	#else
			State_UserPara_SetHotBackup,
	#endif
			State_MainMenu_Menu_UserPara
		},NULL,
		NULL,NULL
	},
	//State_HotBackup_Set,
	{
		LCDADDR80,g_SetHotBackupStr,g_SetHotBackupStrEn,
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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

#ifdef XP360
	//State66_PC_PowerOnOff,  g_PCPowerOnStr g_PCShutOffStr
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_MenuPCPowerOnStr,g_PCPowerOnStrEn,
		{
			State_UserPara_SetHotBackup,
			State_UserPara_SaveMode,
			State661_PC_PowerOn_Page,			
			State_MainMenu_Menu_UserPara
		},&SetCurrentStateValue,
		NULL,
		NULL
	},
	//State661_PC_PowerOn_Page 
	{
		
		LCDADDR80,g_MenuPCPowerOnStr,g_PCPowerOnStrEn,
		LCDADDR90,g_PCPowerOnStr,g_PCPowerOnStrEn,
		{
			State662_PC_PowerOff_Page,
			State662_PC_PowerOff_Page,
			State661_PC_PowerOn_Page,			
			State66_PC_PowerOnOff
		},
		#ifdef XP360
			&STM32_CTR_PC_OnOff,
		#else
		NULL,
		#endif
		NULL,
		NULL
	},
	//State662_PC_PowerOff_Page 
	{
		
		LCDADDR80,g_MenuPCPowerOnStr,g_PCPowerOnStrEn,
		LCDADDR90,g_PCShutOffStr,g_PCShutOffStrEn,
		{
			State661_PC_PowerOn_Page,
			State661_PC_PowerOn_Page,
			State662_PC_PowerOff_Page,			
			State66_PC_PowerOnOff
		},
		#ifdef XP360
			&STM32_CTR_PC_OnOff,
		#else
		NULL,
		#endif
		NULL,
		NULL
	},
#endif	
			
	//七State_MainMenu_TimeSchedule,
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
			State_TimeSchedule_BeginInput1,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginInput1,
	{
		LCDADDR80,g_Program1Str,g_Program1StrEn,
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
			State_TimeSchedule_BeginInput1
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
			State_TimeSchedule_BeginInput1,
			State_TimeSchedule_BeginInput1,
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
			State_TimeSchedule_BeginInput2,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginInput2,
	{
		LCDADDR80,g_Program2Str,g_Program2StrEn,
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
			State_TimeSchedule_BeginInput2
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
			State_TimeSchedule_BeginInput2,
			State_TimeSchedule_BeginInput2,
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
			State_TimeSchedule_BeginInput3,	
			State_TimeSchedule_InputSource
		},NULL,
		NULL,NULL
	},
	//State_TimeSchedule_BeginInput3,
	{
		LCDADDR80,g_Program3Str,g_Program3StrEn,
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
			State_TimeSchedule_BeginInput3
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
			State_TimeSchedule_BeginInput3,
			State_TimeSchedule_BeginInput3,
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
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
		LCDADDR91,g_BeginScheduleStr,g_BeginScheduleStrEn,
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
			State_MainMenu_TimeSchedule
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
	//八State_MainMenu_Menu_TechSupport,
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
	//8.1 State_TechSuppor_ContactInfo,
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
		LCDADDR90,g_TechSupportTel1,g_TechSupportTel1,
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
			State_MainMenu_Menu_ScreenPara,//State_AdvancePara_OutputFormat,
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
			State_DebugMode_Check,
			State_DebugMode_Check,
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
			State_DebugMode_Check,
			State_DebugMode_Check,
			State_DebugMode_Start,	
			State_DebugMode_Check
		},&DebugSetValue,
		NULL,NULL
	} ,
	//State_NewMode_Set,
	{
		0x83,g_DirectSetStr+2,g_DirectSetStrEn+2,
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
		0x83,g_DirectSetStr+2,g_DirectSetStrEn+2,
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
		0x83,g_DirectSetStr+2,g_DirectSetStrEn+2,
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
		0x83,g_DirectSetStr+2,g_DirectSetStrEn+2,
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
#if(XP_TO_ID==XP_TO_FLI32626)
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
#endif
	
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
		0x80,g_InputOldSeriesStr,"Input User Serial:",
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
		0x80,g_SetSeriesStr,"Set Serial Number",
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
#ifdef WORKER_ID_REC	
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
		{	State_Input_SaleName,   //up
			State_Input_SaleName,  //down
			State_Input_SaleName,   //ok
			State_DebugMode_Start},  //esc
		NULL,
		NULL,
		NULL
	},
#endif	
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
	
	
	//20170213
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
			State_UserTry_Once_Password101,
			State_UserTry_Once_Password101,			
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
			State_UserTry_Once_Password102,
			State_UserTry_Once_Password102,
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
			State_UserTry_Once_Password104,
			State_UserTry_Once_Password104,
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
			State_UserTry_Once_Password105,
			State_UserTry_Once_Password105,
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
};

#ifdef XP360
void PC_POWER_ONOFF_LCD_DISPLAY()
{
				////LCD_Init();
				g_CurrentState=State_MainMenu_Not_Menu;
				clearlcd();

				if(g_LanguageFlag==CHINESE)
				{
					if(g_PCPowerFlag==0)
					{
						g_PCPowerFlag=1;
						chinesedisplay(0x81, g_PCPowerOnStr);
					}
					else
					{
						g_PCPowerFlag=0;
						chinesedisplay(0x81, g_PCShutOffStr);
					}
				}
				else if(g_LanguageFlag==ENGLISH)
				{
					if(g_PCPowerFlag==0)
					{
						g_PCPowerFlag=1;
						chinesedisplay(0x81, "PC Power On!");
					}
					else
					{
						g_PCPowerFlag=0;
						chinesedisplay(0x81, "PC Shut Off!");
					}
					
				}

}
#endif

