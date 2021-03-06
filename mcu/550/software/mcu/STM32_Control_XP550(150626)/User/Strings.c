/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: strings.h
 [Date]:        11-May-2012
 [Comment]:
   define strings file.
 [Reversion History]:
*******************************************************************************/
#include "header.h"
#define _STRINGS_C_

const U8 g_SingleCharacter[3]={0xA1,0xF3,0};
const U8 g_DualCharacter[3]={0xA1,0xF4,0};
const U8 g_FadeCharacter[3]={0xA1,0xE8,0};
const U8 g_QuickCharacter[3]={0xA1,0xCF,0};
const U8 g_FuseCharacter[3]={0xA1,0xF2,0};
//整数部分最多两位数，小数部分一位数
const U8 g_ReleaseStr[8]="150626";  

#if(MATHINE_TYPE==XP_530)
U8 g_MachineTypeXPStr[21]="BP530";
#elif(MATHINE_TYPE==XP_520)
//U8 g_MachineTypeXPStr[21]="BP520";
U8 g_MachineTypeXPStr[21]="XP550";
#endif

const U8 g_DirectSetMainSizeStr[21]={
	0xC9,0xE8,0xD6,0xC3,0xBB,0xAD,0xC3,0xE6,0x31,0x20,0xB5,0xC4,0xB3,0xDF,0xB4,0xE7,0
};
const U8 g_DirectSetSubSizeStr[21]={
	0xC9,0xE8,0xD6,0xC3,0xBB,0xAD,0xC3,0xE6,0x32,0x20,0xB5,0xC4,0xB3,0xDF,0xB4,0xE7,0
};

//屏参:
const U8 g_ScreenInfo1Str[6]={
0xC6,0xC1,0xB2,0xCE,0x3A,0
};
//屏参:
const U8 g_InResolutionStr[6]={
0xC6,0xC1,0xB2,0xCE,0x3A,0
};
//双画面同步切换
const U8 g_BothSwithStr[21]={
0xCB,0xAB,0xBB,0xAD,0xC3,0xE6,0xCD,0xAC,0xB2,0xBD,0xC7,0xD0,0xBB,0xBB,0
};
const U8 g_BothSwithStrEn[21]="Both Switch Same";
//已开启
const U8 g_StartStr[7]={
0xD2,0xD1,0xBF,0xAA,0xC6,0xF4,0
};
const U8 g_StartStrEn[5]="Yes";
//已取消
const U8 g_CancelStr[7]={
0xD2,0xD1,0xC8,0xA1,0xCF,0xFB,0
};
const U8 g_CancelStrEn[5]="No";
//输出:
const U8 g_OutResolutionStr[6]={
0xCA,0xE4,0xB3,0xF6,0x3A,0
};
//请输入公司序号:
const U8 g_InputLogoStr[21]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xB9,0xAB,0xCB,0xBE,0xD0,0xF2,0xBA,0xC5,0x3A,0
};
//越界
const U8 g_OutRangeStr[5]={
0xD4,0xBD,0xBD,0xE7,0
};
//请输入机器型号(<10):
const U8  g_InputMathineTypeStr[21]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xBB,0xFA,0xC6,0xF7,0xD0,0xCD,0xBA,0xC5,'(','<','1','0',')',':',0
};

//还有七天试用结束
const U8 g_DeadLine7DaysStr[18]={
0xBB,0xB9,0xD3,0xD0,0xC6,0xDF,0xCC,0xEC,0xCA,0xD4,0xD3,0xC3,0xBD,0xE1,0xCA,0xF8,'!',0
};

//型号:
const U8 g_MachineTypeStr[6]={
0xD0,0xCD,0xBA,0xC5,0x3A,0
};
const U8 g_MachineTypeStrEn[6]="Type:";
//(技术支持电话)
U8 g_ContactTel1[13]={
'4','0','0','-','0','9','6','-','5','7','6','8',0  //400-096-5768
};
U8 g_ContactTel2[13]={    //(解锁联系电话)
'4','0','0','-','0','9','6','-','5','7','6','8',0
};

//软件版本:
const U8 g_SoftVersionStr[10]={
0xC8,0xED,0xBC,0xFE,0xB0,0xE6,0xB1,0xBE,0x3A,0
};

//开机时间: __日
const U8 g_StartTimeStr[15]={
0xBF,0xAA,0xBB,0xFA,0xCA,0xB1,0xBC,0xE4,':',' ',' ',' ',0xC8,0xD5,0
};
//__时__分__秒
const U8 g_StartTimeFormatStr[13]={
' ',' ',0xCA,0xB1,' ',' ',0xB7,0xD6,' ',' ',0xC3,0xEB,0
};
//机器ID:
const U8 g_MathineIDStr[8]={
0xBB,0xFA,0xC6,0xF7,'I','D',':',0
};

//请设置试用天数
const U8 g_TryForUsr[15]={
0xC7,0xEB,0xC9,0xE8,0xD6,0xC3,0xCA,0xD4,0xD3,0xC3,0xCC,0xEC,0xCA,0xFD,0
};
//试用已到期
const U8 g_TryOverStr[11]={
0xCA,0xD4,0xD3,0xC3,0xD2,0xD1,0xB5,0xBD,0xC6,0xDA,0
};
//请联系
const U8 g_PurchaseStr[7]={
0xC7,0xEB,0xC1,0xAA,0xCF,0xB5,0
};
//电脑失控!
const U8 g_KeyWarnningStr[10]={
0xB5,0xE7,0xC4,0xD4,0xCA,0xA7,0xBF,0xD8,0x21,0
};
//电脑控制(按键失控)
const U8 g_PCControlStr[20]={
0xB5,0xE7,0xC4,0xD4,0xBF,0xD8,0xD6,0xC6,' ','(',0xB0,0xB4,0xBC,0xFC,0xCA,0xA7,0xBF,0xD8,')',0
};

const U8 g_YesOrNoStr[18]={
	0xCA,0xC7,'(','O','K',')',' ',' ',' ',' ',0xB7,0xF1,'(','N','O',')',0
};
const U8 g_YesOrNoStrEn[16]="   YES       NO";	
const U8 g_LEDHSizeStr[16]={
	'L','E','D',' ',0xCB,0xAE,0xC6,0xBD,0xB5,0xE3,0xCA,0xFD,':',' ','_',0 
};
const U8 g_LEDHSizeStrEn[12]="LED H Size:";
const U8 g_LEDVSizeStr[16]={
	'L','E','D',' ',0xB4,0xB9,0xD6,0xB1,0xB5,0xE3,0xCA,0xFD,':',' ','_',0 
};
const U8 g_LEDVSizeStrEn[12]="LED V Size:";
const U8 g_SetOKStr[9]={
	0xC9,0xE8,0xD6,0xC3,0xB3,0xC9,0xB9,0xA6,0x0
};
const U8 g_SetOKStrEn[8]="Success";
const U8 g_SetNotStr[14]={
	0xC7,0xD7,0xA1,0xAD,0xB3,0xAC,0xB3,0xF6,0xB7,0xB6,0xCE,0xA7,'!',0
};
const U8 g_SetNotStrEn[16]="Dear,Over range";
const U8 g_SelectAdvProStr[21]={
	0xBB,0xB6,0xD3,0xAD,0xC0,0xB4,0xB5,0xE7,0xD1,0xA1,0xD4,0xF1,0xB8,0xDF,0xB6,0xCB,0xB2,0xFA,0xC6,0xB7,0
};
const U8 g_SelectAdvProStrEn[16]="Please Call Me!";
//北京拓北公司
U8 g_WelcomeStr[21]=
{	
	' ',' ',' ',' ',0xB1,0xB1,0xBE,0xA9,0xcd,0xd8,0xb1,0xb1,0xB9,0xAB,0xCB,0xBE,0
};
U8 g_WelcomeStrEn[21]="      Bycox";

//该方案已设置
const U8  g_HadSetStr[13]={  
	0xB8,0xC3,0xB7,0xBD,0xB0,0xB8,0xD2,0xD1,0xC9,0xE8,0xD6,0xC3,0
};
const U8  g_HadSetStrEn[8]="Had Set";
//按退格键修改或删除
const U8  g_BackForChangeStr[19]={  
	0xB0,0xB4,0xCD,0xCB,0xB8,0xF1,0xBC,0xFC,0xD0,0xDE,0xB8,0xC4,0xBB,0xF2,0xC9,0xBE,0xB3,0xFD,0
};
const U8  g_BackForChangeStrEn[10]="Backspace";
const U8 g_MuteStr[5]={
0xbe,0xb2,0xd2,0xf4,0
};
const U8 g_MuteStrEn[5]="Mute";

	//输入源_
const U8 g_InputStatusStr[8]={ 
	0xCA,0xE4,0xC8,0xEB,0xD4,0xB4,':',0
};
const U8 g_SwapImageStr[9]={ //画面交换
	0xBB,0xAD,0xC3,0xE6,0xBD,0xBB,0xBB,0xBB,0
};
const U8 g_InputStatusStrEn[7]="Input:";

const U8 g_inputsourceStr[13]=  //当前输入信号
{
0xB5,0xB1,0xC7,0xB0,0xCA,0xE4,0xC8,0xEB,0xD0,0xC5,0xBA,0xC5, 0
};
const U8 g_inputsourceStrEn[14]="Current Input";
const U8 g_CVBS1Str[6]=
{
	0xCA,0xD3,0xC6,0xB5,0x31,0
};
const U8 g_CVBS1StrEn[4]="AV1";
const U8 g_CVBS2StrEn[4]="AV2";
const U8 g_CVBS3StrEn[4]="AV3";
const U8 g_CVBS4StrEn[4]="CV4";
const U8 g_CVBS2Str[6]=
{
	0xCA,0xD3,0xC6,0xB5,0x32,0
};
const U8 g_CVBS3Str[6]=
{
	0xCA,0xD3,0xC6,0xB5,0x33,0
};
const U8 g_CVBS4Str[6]=
{
	0xCA,0xD3,0xC6,0xB5,0x34,0
};
const U8 g_SVideoStr[7]=//S端子
{
	0x20,0x53,0xB6,0xCB,0xD7,0xD3,0
};
const U8 g_DVIStr[12]=//数字高清DVI
{
	0xCA,0xFD,0xD7,0xD6,0xB8,0xDF,0xC7,0xE5 ,0x44 ,0x56 ,0x49 ,0
};
const U8 g_DVIStrEn[4]="DVI";
const U8 g_YPbPrStr[14]=//分量高清YPbPr
{
	0xB7,0xD6,0xC1,0xBF,0xB8,0xDF,0xC7,0xE5,0x59,0x50,0x62,0x50,0x72,0
};
const U8 g_YPbPrStrEn[6]="YPbPr";
const U8 g_VGAStr[8]=//电脑VGA
{
	0xB5,0xE7,0xC4,0xD4,0x56,0x47,0x41,0,
};
const U8 g_VGAStrEn[4]="VGA";
const U8 HDMI[13]=//数字高清HDMI
{
	0xCA,0xFD,0xD7,0xD6,0xB8,0xDF,0xC7,0xE5 ,0x48 ,0x44 ,0x4D,0x49,0
};
const U8 HDMIEn[5]="HDMI";


//-------------------schedule string-------------------------------------------
//方案1
const U8 g_Program1Str[6]={
0xB7,0xBD,0xB0,0xB8,0x31,0 
};
const U8 g_Program1StrEn[10]="Program 1";
const U8 g_Program2StrEn[10]="Program 2";
const U8 g_Program3StrEn[10]="Program 3";
//方案2
const U8 g_Program2Str[6]={
0xB7,0xBD,0xB0,0xB8,0x32,0
};
//方案3
const U8 g_Program3Str[6]={
0xB7,0xBD,0xB0,0xB8,0x33,0
};
const U8 g_DateStrEn[17]="Date  _   --  --";
const U8 g_DateStr[17]={
0xC4,0xEA,0xD4,0xC2,0xC8,0xD5,'_',' ',' ',' ','-','-',' ',' ','-','-',0
};
const U8 g_TimeStr[12]={
0xCA,0xB1,0xBC,0xE4,'_',' ',':',' ',' ',' ',':',0	
};
//时间
U8 g_CurrentTimeStr[19]={
0xCA,0xB1,0xBC,0xE4,' ',' ',':',' ',' ',' ',':',' ',' ',' ',' ',' ',0xD6,0xDC,0	
};
const U8 g_CurrentTimeStrEn[12]="Time  :   :";
//时间  00: 00 (24制)
const U8 g_ScheTimeStr[20]={
0xCA,0xB1,0xBC,0xE4,'0','0',':',' ','0','0',' ','(','2','4',0xBD,0xF8,0xD6,0xC6,')',0
};
//时长00: 00 (24制)
const U8 g_CycleTimeStr[20]={
0xCA,0xB1,0xB3,0xA4,'0','0',':',' ','0','0',' ','(',0xCA,0xB1,':',' ',0xB7,0xD6,')',0
};

const U8 g_ScheTimeStrEn[16]="Time00: 00 (24)";
//设置方案
const U8 g_SetScheduleStr[9]={  
	0xC9,0xE8,0xD6,0xC3,0xB7,0xBD,0xB0,0xB8,0
};
const U8 g_SetScheduleStrEn[8]="Setting";
//取消方案
const U8 g_CancelScheduleStr[9]={
	0xC8,0xA1,0xCF,0xFB,0xB7,0xBD,0xB0,0xB8,0
};
const U8 g_CancelScheduleStrEn[15]="Cancel Setting";
const U8 g_ScheduleModeStr[11]={ //请选择模式
0xC7,0xEB,0xD1,0xA1,0xD4,0xF1,0xC4,0xA3,0xCA,0xBD,0
};
const U8 g_ScheduleModeStrEn[14]="Please Select";
const U8 g_ScheduleOnceStr[9]={//执行一次
0xD6,0xB4,0xD0,0xD0,0xD2,0xBB,0xB4,0xCE,0
};
const U8 g_ScheduleOnceStrEn[14]="Only One Time";
const U8 g_ScheduleEverydayStr[9]={//重复执行
0xD6,0xD8,0xB8,0xB4,0xD6,0xB4,0xD0,0xD0,0
};
const U8 g_ScheduleEverydayStrEn[15]="Time And Again";
const U8 g_ScheduleWhichDayStr[11]={//请输入星期
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xD0,0xC7,0xC6,0xDA,0
};
const U8 g_ScheduleWhichDayStrEn[18]="Please Input Week";
//------------------------------------------------------------------------------

const U8 g_fullZoomEn[13]="Full or Part";
const U8 g_fullZoom[11]=
{
0xBE,0xD6 ,0xB2,0xBF ,0xD3,0xEB ,0xC8,0xAB ,0xBE,0xD6 ,0
};
const U8 g_jubuEn[5]="Part";
const U8 g_jubu[9]=
{
0xBE,0xD6 ,0xB2,0xBF ,0xBB,0xAD ,0xC3,0xE6 ,0
};
const U8 g_fullEn[5]="Full";
const U8 g_full[9]=
{
0xC8,0xAB ,0xBE,0xD6 ,0xBB,0xAD ,0xC3,0xE6 ,0
};
const U8 g_SingleOrDualEn[19]="Single or Dual(OK)";
const U8 g_SingleOrDual[21]=
{
0xB5,0xA5 ,0xCB,0xAB ,0xBB,0xAD ,0xC3,0xE6,' ','(',0xB0,0xB4,'O','K',0xC8,0xB7,0xB6,0xA8,')',' ',0
};
const U8 g_SingleChEn[7]="single";
const U8 g_SingleCh[7]=
{
0xB5,0xA5 ,0xBB,0xAD ,0xC3,0xE6 ,0
};
const U8 g_PBPEn[4]="PBP";
const U8 g_PBP[7]=
{
0xBB,0xAD ,0xCD,0xE2 ,0xBB,0xAD ,0
};
const U8 g_PIPEn[4]="PIP";
const U8 g_PIP[7]=
{
0xBB,0xAD ,0xD6,0xD0 ,0xBB,0xAD ,0
};
const U8 g_ActiveImage[13]=
{
0xB5,0xB1 ,0xC7,0xB0 ,0xBB,0xEE,0xB6,0xAF ,0xBB,0xAD,0xC3,0xE6,0
};
const U8 g_ActiveImageEn[13]="Active Image";
const U8 g_ErrorDualInputStr[16]={
0xCB,0xAB,0xBB,0xAD,0xC3,0xE6,0xC9,0xE8,0xD6,0xC3,0xB4,0xED,0xCE,0xF3,'!',0
};
const U8 g_ErrorDualInputStrEn[12]="Cannot Dual";
const U8 g_WarnningStr[11]={
	0xCE,0xC2,0xDC,0xB0,0xCC,0xE1,0xCA,0xBE,0xA3,0xA1,0
};
const U8 g_WarnningStrEn[8]="Warning";
const U8 g_MainImage[6]=
{
0xBB,0xAD,0xC3,0xE6,0x31,0
};
const U8 g_MainImageEn[5]="Main";
const U8 g_SubImage[6]=
{
0xBB,0xAD,0xC3,0xE6,0x32,0
};
const U8 g_SubImageEn[4]="Sub";

const U8 g_MainMenu[7]=
{
0xD6,0xF7,0xB2,0xCB,0xB5,0xA5,0 
};
const U8 g_MainMenuEn[10]="Main Menu";
const U8 g_MainMenuNum[14]=
{
0x20,0x28,0xCA,0xFD,0xD7,0xD6,0xBC,0xFC,0xBF,0xC9,0xD3,0xC3,0x29,0
};
//所有通道均无信号
const U8 g_AllChannelNoSYNCStr[17]={
	0xCB,0xF9,0xD3,0xD0,0xCD,0xA8,0xB5,0xC0,0xBE,0xF9,0xCE,0xDE,0xD0,0xC5,0xBA,0xC5,0
};
const U8 g_MainMenuNumEn[13]="Input Number";

const U8 g_SwitchInputModeStr[13]={//切换模式选择
	0xC7,0xD0,0xBB,0xBB,0xC4,0xA3,0xCA,0xBD,0xD1,0xA1,0xD4,0xF1,0
};
const U8 g_SingleSwapWarnningStr[21]={//请切换通道后再按此键
	0xC7,0xEB,0xC7,0xD0,0xBB,0xBB,0xCD,0xA8,0xB5,0xC0,0xBA,0xF3,0xD4,0xD9,0xB0,0xB4,0xB4,0xCB,0xBC,0xFC,0
};
const U8 g_SwitchInputModeStrEn[12]="Switch Mode";

const U8 g_FadeInFadeOutStr[9]={ //淡入淡出
	0xB5,0xAD,0xC8,0xEB,0xB5,0xAD,0xB3,0xF6,0
};
const U8 g_FadeInFadeOutStrEn[17]="Fade In Fade Out";
const U8 g_QuickSwitchStr[9]={  //无缝切换
	0xCE,0xDE,0xB7,0xEC,0xC7,0xD0,0xBB,0xBB,0
};
const U8 g_FuseSwitchStr[9]={  //融合模式
	0xC8,0xDA,0xBA,0xCF,0xC4,0xA3,0xCA,0xBD,0
};
const U8 g_QuickSwitchStrEn[13]="Quick Switch";
/*
const U8 g_NormalSwitchStr[9]={//正常切换
	0xD5,0xFD,0xB3,0xA3,0xC7,0xD0,0xBB,0xBB,0
};

const U8 g_NormalSwitchStrEn[14]="Normal Switch";
*/
const U8  g_ScreenPara[17]=//一、LED 屏幕设置
{
0xD2,0xBB,0xA1,0xA2,'L','E','D',' ',0xC6,0xC1,0xC4,0xBB,0xC9,0xE8,0xD6,0xC3,0 
};
const U8  g_ScreenParaEn[20]="(1).LED Screen Para";

const U8  g_ScalerPara[17]=//二、画面缩放设置
{
0xB6,0xFE,0xA1,0xA2,0xBB,0xAD,0xC3,0xE6,0xCB,0xF5,0xB7,0xC5,0xC9,0xE8,0xD6,0xC3,0 
};
const U8  g_ScalerParaEn[21]="(2).Dual Scaler Para";

const U8 g_ScreenSizeStr[11]={//1.屏幕点数
'1','.',0xC6,0xC1,0xC4,0xBB,0xB5,0xE3,0xCA,0xFD,0
};
const U8 g_ScalerSizeStr[11]={//1.画面点数
'1','.',0xBB,0xAD,0xC3,0xE6,0xB5,0xE3,0xCA,0xFD,0
};
const U8 g_ZoomSizeStr[11]=//1.截取尺寸
{
'1','.',0xBD,0xD8,0xC8,0xA1,0xB3,0xDF,0xB4,0xE7,0 
};
const U8 g_StartPos[11]={//2.起始位置
'2','.',0xC6,0xF0,0xCA,0xBC,0xCE,0xBB,0xD6,0xC3,0 
};
const U8 g_UserResolutionStr[15]={//3.自定义分辨率
'3','.',0xD7,0xD4,0xB6,0xA8,0xD2,0xE5,0xB7,0xD6,0xB1,0xE6,0xC2,0xCA,0
};

const U8 g_UserSetResolutionStr[15]={//设置定义分辨率
0xC9,0xE8,0xD6,0xC3,0xB6,0xA8,0xD2,0xE5,0xB7,0xD6,0xB1,0xE6,0xC2,0xCA,0
};

const U8 g_ZoomPara[17]=//三、图像截取设置
{
0xC8,0xFD,0xA1,0xA2,0xCD,0xBC,0xCF,0xF1,0xBD,0xD8,0xC8,0xA1,0xC9,0xE8,0xD6,0xC3,0 
};
const U8 g_ZoomParaEn[14]="(3).Zoom Para";
const U8 g_ColorPara[13]=//四、颜色设置
{
0xCB,0xC4,0xA1,0xA2,0xD1,0xD5,0xC9,0xAB,0xC9,0xE8,0xD6,0xC3,0 
};
const U8 g_ColorParaEn[15]="(4).Color Para";
const U8 g_AdvancePara[13]=//五、高级设置
{
0xCE,0xE5,0xA1,0xA2,0xB8,0xDF,0xBC,0xB6,0xC9,0xE8,0xD6,0xC3,0 
};
const U8 g_AdvanceParaEn[17]="(5).Advance Para";

const U8  g_UserParaStr[13]=//六、用户设置
{
0xC1,0xF9,0xA1,0xA2,0xD3,0xC3,0xBB,0xA7,0xC9,0xE8,0xD6,0xC3,0 
};
const U8  g_UserParaStrEn[19]="(6).User parameter";

const U8  g_SaveModeStr[11]=//1.保存模式
{
'1','.',0xB1,0xA3,0xB4,0xE6,0xC4,0xA3,0xCA,0xBD,0 
};
const U8  g_SaveModeStrEn[12]="1.Save Mode";
const U8  g_RecallModeStr[11]=//2.调用模式
{
'2','.',0xB5,0xF7,0xD3,0xC3,0xC4,0xA3,0xCA,0xBD,0 
};
const U8  g_RecallModeStrEn[14]="2.Recall Mode";
const U8  g_SetTimeStr[15]=//3.设置当前时间
{
'3','.',0xC9,0xE8,0xD6,0xC3,0xB5,0xB1,0xC7,0xB0,0xCA,0xB1,0xBC,0xE4,0
};
const U8  g_SetTimeStrEn[11]="3.Set Time";
const U8  g_SetFadeTimeStr[19]=//4.设置淡入淡出时间
{
'4','.',0xC9,0xE8,0xD6,0xC3,0xB5,0xAD,0xC8,0xEB,0xB5,0xAD,0xB3,0xF6,0xCA,0xB1,0xBC,0xE4,0
};
const U8  g_SetFadeTimeStrEn[16]="4.Set Fade Time";
const U8  g_SetHotBackupStr[13]=//5.设置热备份
{
'5','.',0xC9,0xE8,0xD6,0xC3,0xC8,0xC8,0xB1,0xB8,0xB7,0xDD,0
};
//自定义热备份
const U8 g_UserHotBackupStr[13]={
0xD7,0xD4,0xB6,0xA8,0xD2,0xE5,0xC8,0xC8,0xB1,0xB8,0xB7,0xDD,0
};
//已开启热备份
const U8 g_HotBackupStartStr[13]={
0xD2,0xD1,0xBF,0xAA,0xC6,0xF4,0xC8,0xC8,0xB1,0xB8,0xB7,0xDD,0
};
//输入信号源:
const U8 g_InputSignalStr[14]={
0xCA,0xE4,0xC8,0xEB,0xD0,0xC5,0xBA,0xC5,0xD4,0xB4,0x3A,' ','_',0
};
//备份信号源:
const U8 g_BackupSignalStr[12]={
0xB1,0xB8,0xB7,0xDD,0xD0,0xC5,0xBA,0xC5,0xD4,0xB4,0x3A,0
};

const U8  g_SetHotBackupStrEn[19]="5.Set Sinal Backup";

const U8 g_TimeScheduleStr[13]={//七、时间计划
0xC6,0xDF,0xA1,0xA2,0xCA,0xB1,0xBC,0xE4,0xBC,0xC6,0xBB,0xAE,0x0
};
const U8 g_TimeScheduleStrEn[18]="(7).Time Schedule";

const U8 g_InputScheduleStr[11]={
'1','.',0xCD,0xA8,0xB5,0xC0,0xBC,0xC6,0xBB,0xAE,0
};
const U8 g_InputScheduleStrEn[17]="1.Input Schedule";
const U8 g_ModeScheduleStr[11]={
'2','.',0xC4,0xA3,0xCA,0xBD,0xBC,0xC6,0xBB,0xAE,0
};
const U8 g_ModeScheduleStrEn[16]="2.User Schedule";
//循环播放
const U8 g_CycleScheduleStr[11]={
'2','.',0xD1,0xAD,0xBB,0xB7,0xB2,0xA5,0xB7,0xC5,0
};
const U8 g_CycleScheduleStrEn[13]="2.Play round";
//取消所有方案
const U8 g_CancelAllScheduleStr[15]={
0xC8,0xA1,0xCF,0xFB,0xCB,0xF9,0xD3,0xD0,0xB7,0xBD,0xB0,0xB8,0
};
const U8 g_CancelAllScheduleStrEn[18]="2.Cancel Schedule";
const U8 g_TechSupport[13]=//八、技术支持
{
0xB0,0xCB,0xA1,0xA2,0xBC,0xBC,0xCA,0xF5,0xD6,0xA7,0xB3,0xD6,0 
};
const U8 g_TechSupportEn[17]="(8).Supprot Info";
const U8 g_SetUpModeStr[13]=//九、调试模式
{
	0xBE,0xC5,0xA1,0xA2,0xB5,0xF7,0xCA,0xD4,0xC4,0xA3,0xCA,0xBD,0
};
const U8 g_SetUpModeStrEn[11]="(9).Debug";
const U8 g_InputSetupStr[15]={//请输入调试序号
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xB5,0xF7,0xCA,0xD4,0xD0,0xF2,0xBA,0xC5,0
};
const U8 g_InputSetupStrEn[16]="Input Debug No.";

//水平
const U8 g_SelHStr[5]=
{
0xCB,0xAE,0xC6,0xBD,0
};
//垂直
const U8 g_SelVStr[5]=
{
0xB4,0xB9,0xD6,0xB1,0 
};
//请先设置屏参
const U8 g_PleaseSetScreenStr[13]={
0xC7,0xEB,0xCF,0xC8,0xC9,0xE8,0xD6,0xC3,0xC6,0xC1,0xB2,0xCE,0 
};

//只用于双通道
const U8 g_UsedDoubleStr[13]={
0xD6,0xBB,0xD3,0xC3,0xD3,0xDA,0xCB,0xAB,0xCD,0xA8,0xB5,0xC0,0
};

const U8 g_SelHStrEn[9]="H Select";
const U8 g_SelVStrEn[9]="V Select";
//选定水平位置
const U8 g_SelHPosStr[13]=
{
0xD1,0xA1,0xB6,0xA8,0xCB,0xAE,0xC6,0xBD,0xCE,0xBB,0xD6,0xC3,0 
};
//选定垂直位置
const U8 g_SelVPosStr[13]=
{
0xD1,0xA1,0xB6,0xA8,0xB4,0xB9,0xD6,0xB1,0xCE,0xBB,0xD6,0xC3,0 
};

//缩放水平点数
const U8 g_SelScalerHStr[13]=
{
0xCB,0xF5,0xB7,0xC5,0xCB,0xAE,0xC6,0xBD,0xB5,0xE3,0xCA,0xFD,0 
};
//缩放垂直点数
const U8 g_SelSclaerVStr[13]=
{
0xCB,0xF5,0xB7,0xC5,0xB4,0xB9,0xD6,0xB1,0xB5,0xE3,0xCA,0xFD,0 
};
//截取水平尺寸
const U8 g_SelZoomHStr[13]={
0xBD,0xD8,0xC8,0xA1,0xCB,0xAE,0xC6,0xBD,0xB3,0xDF,0xB4,0xE7,0 
};
//截取垂直尺寸
const U8 g_SelZoomVStr[13]={
0xBD,0xD8,0xC8,0xA1,0xB4,0xB9,0xD6,0xB1,0xB3,0xDF,0xB4,0xE7,0 
};

const U8 g_BrightnessStr[7]=
{
'1','.',0xC1,0xC1,0xB6,0xC8,0 
};
const U8 g_BrightnessEn[13]="1.Brightness";
/*
const U8 g_OffsetRStr[11]=
{
'2','.',0xBD,0xD8,0xD6,0xB9,0xBA,0xEC,0xC9,0xAB,0 
};
const U8 g_OffsetREn[13]="2.Offset Red";
const U8 g_OffsetGStr[11]=
{
'3','.',0xBD,0xD8,0xD6,0xB9,0xC2,0xCC,0xC9,0xAB,0 
};
const U8 g_OffsetGEn[15]="3.Offset Green";
const U8 g_OffsetBStr[11]=
{
'4','.',0xBD,0xD8,0xD6,0xB9,0xC0,0xB6,0xC9,0xAB,0 
};
const U8 g_OffsetBEn[14]="4.Offset Blue";*/
const U8 g_ContractStr[9]=
{
'2','.',0xB6,0xD4,0xB1,0xC8,0xB6,0xC8,0 
};
const U8 g_ContractEn[11]="2.Contract";
/*
const U8 g_GainRStr[13]=
{
'6','.',0xB2,0xA8,0xB7,0xF9,0xB6,0xC8,0xBA,0xEC,0xC9,0xAB,0 
};
const U8 g_GainREn[11]="6.Gain Red";
const U8 g_GainGStr[13]=
{
'7','.',0xB2,0xA8,0xB7,0xF9,0xB6,0xC8,0xC2,0xCC,0xC9,0xAB,0 
};
const U8 g_GainGEn[13]="7.Gain Green";
const U8 g_GainBStr[13]=
{
'8','.',0xB2,0xA8,0xB7,0xF9,0xB6,0xC8,0xC0,0xB6,0xC9,0xAB,0 
};
const U8 g_GainBEn[12]="8.Gain Blue";*/
const U8 g_SaturationStr[11]=
{
'3','.',0xC9,0xAB,0xB1,0xA5,0xBA,0xCD,0xB6,0xC8,0 
};
const U8 g_SaturationStrEn[13]="3.Saturation";
const U8 g_ColorResetStr[7]=
{
'4','.',0xB8,0xB4,0xCE,0xBB,0
};
const U8 g_ColorResetStrEn[8]="4.Reset";
const U8 g_AdcCalibrationStr[11]=
{
'5','.','A','D','C',' ',0xD0,0xA3,0xD5,0xFD,0
};
const U8 g_AdcCalibrationStrEn[18]="5.ADC Calibration";

//冻结画面
const U8 g_FreezeImage[9]=
{
0xB6,0xB3,0xBD,0xE1,0xBB,0xAD,0xC3,0xE6,0 
};
const U8 g_FreezeImageEn[13]="Freeze Image";

////////////////////////////////////////五、高级设置/////////////////////////////////////////
//-----------------------------------高级设置子菜单---------------------------------------
//1.输出分辨率
const U8 g_outputFormatstr[13]=
{
'1','.',0xCA,0xE4,0xB3,0xF6,0xB7,0xD6,0xB1,0xE6,0xC2,0xCA,0
};
const U8 g_outputFormatstrEn[16]="1.Output Format";

//2.屏幕刷新频率
const U8  g_OutputHzStr[15]=
{
'2','.',0xC6,0xC1,0xC4,0xBB,0xCB,0xA2,0xD0,0xC2,0xC6,0xB5,0xC2,0xCA,0 
};
const U8  g_OutputHzStrEn[19]="2.Output frequency";

//4.声音控制
const U8  g_AudioStr[11]={
'4','.',0xC9,0xF9,0xD2,0xF4,0xBF,0xD8,0xD6,0xC6 ,0
}; 
const U8  g_AudioStrEn[12]="4.Audio Set";
const U8  g_VolumeStr[9]={
0xD2,0xF4,0xC1,0xBF,0xB5,0xF7,0xBD,0xDA ,0
}; 
const U8  g_VolumeStrEn[11]="Volume Set";
//5.锁键盘时间 (分钟)
const U8 g_LockKeypadStr[21]={
'5','.',0xCB,0xF8,0xBC,0xFC,0xC5,0xCC,0xCA,0xB1,0xBC,0xE4,' ','(',0xB7,0xD6,0xD6,0xD3,')',' ',0
};

const U8 g_CancelLockKeypadStr[18]={
	'_',' ',' ',' ',' ','(','0',':',0xC8,0xA1,0xCF,0xFB,0xCB,0xF8,0xBC,0xFC,')',0
};
const U8 g_LockKeypadStrEN[17]="5.Lock Time(Min)";
//6.恢复出厂设置
const U8 g_FactoryReset[17]={
'6','.',0xBB,0xD6,0xB8,0xB4,0xB3,0xF6,0xB3,0xA7,0xC9,0xE8,0xD6,0xC3,0 
};
const U8 g_FactoryResetEn[16]="6.Factory Reset";
//7.向导设置
const U8 g_DirectSetStr[11]={
0xCF,0xF2,0xB5,0xBC,0xC9,0xE8,0xD6,0xC3,0
};
const U8 g_DirectSetStrEn[17]="Wizard Setting";
//8.语言设置
const U8 g_Language[11]={
'8','.',0xD3,0xEF,0xD1,0xD4,0xC9,0xE8,0xD6,0xC3,0 
};
const U8 g_LanguageEn[11]="8.Language";
//冻结
const U8 g_FreezeStr[5]={
0xB6,0xB3,0xBD,0xE1,0 
};
const U8 g_FreezeStrEn[7]="Freeze";
//解冻
const U8 g_LiveStr[5]={
0xBD,0xE2,0xB6,0xB3,0 
};
const U8 g_LiveStrEn[5]="Live";
//--------------------------------高级设置子菜单--------------------------------------------
////////////////////////////////////////五、高级设置/////////////////////////////////////////

const U8 g_Mode1Str[12]={
' ', '>',0xC4,0xA3,0xCA,0xBD,0x31,0 
};
const U8 g_Mode1StrEn[9]=" >Mode1";
const U8 g_Mode2StrEn[9]=" >Mode2";
const U8 g_Mode3StrEn[9]=" >Mode3";
const U8 g_Mode2Str[12]=
{
' ', '>',0xC4,0xA3,0xCA,0xBD,0x32,0
};
const U8 g_Mode3Str[12]=
{
' ', '>',0xC4,0xA3,0xCA,0xBD,0x33,0
};
const U8 g_ContactInfo[11]=
{
'1','.',0xC1,0xAA,0xCF,0xB5,0xB7,0xBD,0xCA,0xBD,0 
};
const U8 g_ContactInfoEn[15]="1.Contact Info";
const U8 g_Factory[7]=
{
 0xD4,0xAD,0xB3,0xA7,0xA3,0xBA,0
};
const U8 g_FactoryEn[8]="Factory:";


const U8 g_VersionInfo[7]=
{
'2','.',0xB0,0xE6,0xB1,0xBE,0
};
const U8 g_VersionInfoEn[15]="2.Version Info";
//该操作将清除保存数据
const U8 g_Yes[21]=
{
0xB8,0xC3,0xB2,0xD9,0xD7,0xF7,0xBD,0xAB,0xC7,0xE5,0xB3,0xFD,0xB1,0xA3,0xB4,0xE6,0xCA,0xFD,0xBE,0xDD,0
};
const U8 g_YesEn[17]="Will Delete Data";
//是否继续
const U8 g_YesStr[11]={
	0xCA,0xC7,0xB7,0xF1,0xBC,0xCC,0xD0,0xF8,0x0
};
const U8 g_YesStrEn[10]="Continue?";
const U8 g_Chinese[5]=
{
0xD6,0xD0,0xCE,0xC4,0 
};
/*const U8 g_ResettingStr[15]={
	0xD5,0xFD,0xD4,0xDA,0xBB,0xD6,0xB8,0xB4,0xD6,0xD0,0xA1,0xAD,0xA1,0xAD,0
};
const U8 g_ResettingStrEn[14]="Resetting----";*/


const U8 g_ExeModeStr[15]={
0xD5,0xFD,0xD4,0xDA,0xC6,0xF4,0xB6,0xAF,0xC4,0xA3,0xCA,0xBD,0xD2,0xBB,0
};
const U8 g_ExeModeStrEn[19]="Recalling mode1...";
const U8 g_waitStr[11]={		//请稍等……
0xC7,0xEB,0xC9,0xD4,0xB5,0xC8,0xA1,0xAD,0xA1,0xAD,0
}; 	
const U8 g_waitStrEn[14]="wait a minute";

const U8 g_ResetSuccessStrEn[14]="Reset Success";
const U8 g_ResetSuccessStr[11]={
	0xB8,0xB4,0xCE,0xBB,0xB3,0xC9,0xB9,0xA6,0xA3,0xA1,0
};
const U8 g_CancelSuccessStrEn[15]="Cancel Success";
const U8 g_CancelSuccessStr[10]={
	0xC8,0xA1,0xCF,0xFB,0xB3,0xC9,0xB9,0xA6,'!',0
};
const U8 g_SavingStrEn[12]="Saving-----";
const U8 g_SavingStr[11]={
	0xB1,0xA3,0xB4,0xE6,0xD6,0xD0,0xA1,0xAD,0xA1,0xAD,0
};
const U8 g_SaveOKStrEn[13]="Save Success";
const U8 g_SaveOKStr[10]={
	0xB1,0xA3,0xB4,0xE6,0xB3,0xC9,0xB9,0xA6,'!',0
};
const U8 g_LoadingModeStrEn[13]="Loading-----";
const U8 g_LoadingModeStr[15]={
	0xB5,0xF7,0xD3,0xC3,0xC4,0xA3,0xCA,0xBD,0xD6,0xD0,0xA1,0xAD,0xA1,0xAD,0
};
const U8 g_InputZoomStr[17]={
	'3','.',0xB2,0xE9,0xBF,0xB4,0xCA,0xE4,0xC8,0xEB,0xB7,0xD6,0xB1,0xE6,0xC2,0xCA ,0
};
const U8 g_LoadModeOKStrEn[13]="Load Success";
const U8 g_LoadModeOKStr[15]={
	0xB5,0xF7,0xD3,0xC3,0xC4,0xA3,0xCA,0xBD,0xB3,0xC9,0xB9,0xA6,0xA3,0xA1,0
};
const U8 g_LoadNoModeStrEn[8]="No Mode";
const U8 g_LoadNoModeStr[19]={
	0xB8,0xC3,0xC4,0xA3,0xCA,0xBD,0xC3,0xBB,0xD3,0xD0,0xB1,0xA3,0xB4,0xE6,0xC4,0xDA,0xC8,0xDD,0
};
const U8 g_HasSaveValueStrEn[20]="This Mode Has Saved";
const U8 g_HasSaveValueStr[19]={
	0xB8,0xC3,0xC4,0xA3,0xCA,0xBD,0xD2,0xD1,0xBE,0xAD,0xB1,0xA3,0xB4,0xE6,0xC4,0xDA,0xC8,0xDD,0
};
const U8 g_SaveOverlapStrEn[9]="Overlap?";
const U8 g_SaveOverlapStr[10]={
	0xCA,0xC7,0xB7,0xF1,0xB8,0xB2,0xB8,0xC7,'!',0
};
const U8 g_SaveOverlappingStrEn[16]="Overlapping----";
const U8 g_SaveOverlappingStr[15]={
	0xD5,0xFD,0xD4,0xDA,0xB8,0xB2,0xB8,0xC7,0xD6,0xD0,0xA1,0xAD,0xA1,0xAD,0
};

const U8 g_LockKeyStr[11]={
	0xBC,0xFC,0xC5,0xCC,0xD2,0xD1,0xCB,0xF8,0xA3,0xA1,0
};
const U8 g_LockKeyStrEn[19]="Keypad had Locked!";
const U8 g_UnLockKeyStr[11]={
	0xBD,0xE2,0xCB,0xF8,0xB3,0xC9,0xB9,0xA6,0xA3,0xA1,0
};
const U8 g_UnLockKeyStrEn[18]="Unlocked Success!";

const U8 g_LockPromptStr[15]={//请长按锁键解锁
	0xC7,0xEB,0xB3,0xA4,0xB0,0xB4,0xCB,0xF8,0xBC,0xFC,0xBD,0xE2,0xCB,0xF8,0
};
const U8 g_LockPromptStrEn[20]="Press ESC to Unlock";

const U8 g_SingleSwitchStr[11]={//信号源切换
	0xD0,0xC5,0xBA,0xC5,0xD4,0xB4,0xC7,0xD0,0xBB,0xBB,0
};
const U8 g_MainSwitchStr[10]={//切换画面1
	0xC7,0xD0,0xBB,0xBB,0xBB,0xAD,0xC3,0xE6,0x31,0
};
const U8 g_SubSwitchStr[10]={//切换画面2
	0xC7,0xD0,0xBB,0xBB,0xBB,0xAD,0xC3,0xE6,0x32,0
};


const U8  g_DeadlineFmtStr[21]=//_,0x,0x,0x年,0x月,0x日
{
	0xBD,0xD8,0xD6,0xB9,0xC6,0xDA,0x5F,0x20,0x20,0x20,0xC4,0xEA,0x20,0x20,0xD4,0xC2,0x20,0x20,0xC8,0xD5,0
};

const U8 g_InputSeriesStr[14]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xD7,0xA2,0xB2,0xE1,0xC2,0xEB,':',0
};

//请输入组装员ID:
const U8  g_InputPackagingNameStr[20]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xD7,0xE9,0xD7,0xB0,0xD4,0xB1,0x49,0x44,0x3A,'(','<','5',')',0
};
//请输入测试员ID:
const U8  g_InputTestNameStr[20]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xB2,0xE2,0xCA,0xD4,0xD4,0xB1,0x49,0x44,0x3A,'(','<','5',')',0
};
//请输入销售员ID:
const U8  g_InputSaleNameStr[20]={
	0xC7,0xEB,0xCA,0xE4,0xC8,0xEB,0xCF,0xFA,0xCA,0xDB,0xD4,0xB1,0x49,0x44,0x3A,'(','<','5',')',0
};
//杨珊
const U8  g_YangShanNameStr[7]={
	0xD1,0xEE,0xC9,0xBA,0
};
//黄丹
const U8  g_HuangDanNameStr[7]={
	0xBB,0xC6,0xB5,0xA4,0
};

U8 g_PackagingName[7]={0};
U8 g_TestName[7]={0};
U8 g_SaleName[7]={0};

const U8  g_SetSerialNumOKStr[9]={//密码正确
	0xC3,0xDC,0xC2,0xEB,0xD5,0xFD,0xC8,0xB7,0x0
};
const U8  g_SetSerialNumNotStr[21]={//密码错误，请重新输入
	0xC3,0xDC,0xC2,0xEB,0xB4,0xED,0xCE,0xF3,0xA3,0xAC,0xC7,0xEB,0xD6,0xD8,0xD0,0xC2,0xCA,0xE4,0xC8,0xEB,0
};


U16 TimeSet[6] = {2012, 12, 1, 14, 30, 30};
U16 g_TryEndDate[3] = {0xffff,0xff,0xff};// 记录用户试用截止日期

//6.设置静态IP
const U8  g_SetStaticIPStr[13]={
	'6','.',0xC9,0xE8,0xD6,0xC3,0xBE,0xB2,0xCC,0xAC,0x49,0x50,0
};
const U8  g_SetStaticIPStrEn[16]="6.Set Static IP";

const U8 g_SupportTechStr[9]={
0xC1,0xAA,0xCF,0xB5,0xB5,0xE7,0xBB,0xB0,0
};
const U8  g_SupportTechStrEn[9]="Contact:";

const U8 g_SetSeriesStr[21]={
	0xC9,0xE8,0xD6,0xC3,0xCA,0xD4,0xD3,0xC3,0xC3,0xDC,0xC2,0xEB,'(','<','6',')',':',0
};
const U8 g_InputOldSeriesStr[21]={
	0xB5,0xB1,0xC7,0xB0,0xCA,0xD4,0xD3,0xC3,0xC3,0xDC,0xC2,0xEB,'(','<','6',')',':',0
};
//设置联系电话(<11):
const U8 g_SetTeleNumStr[21]={
	0xC9,0xE8,0xD6,0xC3,0xC1,0xAA,0xCF,0xB5,0xB5,0xE7,0xBB,0xB0,' ','(','<','1','1',')',':',0
};
//设置技术电话(<11):
const U8 g_SetTechTeleNumStr[21]={
	0xC9,0xE8,0xD6,0xC3,0xBC,0xBC,0xCA,0xF5,0xB5,0xE7,0xBB,0xB0,' ','(','<','1','1',')',':',0
};
const U8 g_EEPROMErrorStr[21]={//存储器错误,请联系
0xB4,0xE6,0xB4,0xA2,0xC6,0xF7,0xB4,0xED,0xCE,0xF3,0x2C,0x20,0xC7,0xEB,0xC1,0xAA,0xCF,0xB5,0
};
//欢迎使用
const U8 g_NoEnterpriseStr[21]=
{
	' ',' ',' ',' ',' ',' ',0xBB,0xB6,0xD3,0xAD,0XCA,0XB9,0xD3,0xC3,0
};
//北京拓北公司
const U8 g_Enterprise1StrBYCOX[21]={
' ',' ',' ',' ',0xB1,0xB1,0xBE,0xA9,0xcd,0xd8,0xb1,0xb1,0xB9,0xAB,0xCB,0xBE,0
};
//凯视达科技
const U8 g_Enterprise2Str[21]={
' ',' ',' ',' ',0xBF,0xAD,0xCA,0xD3,0xB4,0xEF,0xBF,0xC6,0xBC,0xBC,0x0
};
//唯奥视讯
const U8 g_Enterprise3Str[21]={
	' ',' ',' ',' ',' ',' ',0xCE,0xA8,0xB0,0xC2,0xCA,0xD3,0xD1,0xB6,0x0
};
//视睿讯科技
const U8 g_Enterprise4Str[21]={
	' ',' ',' ',' ',0xCA,0xD3,0xEE,0xA3,0xD1,0xB6,0xBF,0xC6,0xBC,0xBC,0x0
};
//视诚科技
const U8 g_Enterprise5Str[21]={
' ',' ',' ',' ',' ',' ',0xCA,0xD3,0xB3,0xCF,0xBF,0xC6,0xBC,0xBC,0x0
};
//迈普视通技术
const U8 g_Enterprise6Str[21]={
	' ',' ',' ',' ',0xC2,0xF5,0xC6,0xD5,0xCA,0xD3,0xCD,0xA8,0xBC,0xBC,0xCA,0xF5,0x0
};
//爱普视科技
const U8 g_Enterprise7Str[21]={
	' ',' ',' ',' ',0xB0,0xAE,0xC6,0xD5,0xCA,0xD3,0xBF,0xC6,0xBC,0xBC,0x0
};
//	8：瑞江科技
const U8 g_Enterprise8Str[21]={
	' ',' ',' ',' ',' ',' ',0xC8,0xF0,0xBD,0xAD,0xBF,0xC6,0xBC,0xBC,0x0
};
//	9：神达实业
const U8 g_Enterprise9Str[21]={
	' ',' ',' ',' ',' ',' ',0xC9,0xF1,0xB4,0xEF,0xCA,0xB5,0xD2,0xB5,0x0
};
//	10：洲泰科技
const U8 g_Enterprise10Str[21]={
	' ',' ',' ',' ',' ',' ',0xD6,0xDE,0xCC,0xA9,0xBF,0xC6,0xBC,0xBC,0x0
};
//	11：杰思创科技
const U8 g_Enterprise11Str[21]={
	' ',' ',' ',' ',0xBD,0xDC,0xCB,0xBC,0xB4,0xB4,0xBF,0xC6,0xBC,0xBC,0x0
};
//	12：创凯电子
const U8 g_Enterprise12Str[21]={
	' ',' ',' ',' ',' ',' ',0xB4,0xB4,0xBF,0xAD,0xB5,0xE7,0xD7,0xD3,0x0
};
//	13：富视威科技
const U8 g_Enterprise13Str[21]={
	' ',' ',' ',' ',0xB8,0xBB,0xCA,0xD3,0xCD,0xFE,0xBF,0xC6,0xBC,0xBC,0x0
};

//北京利亚德公司
const U8 g_Enterprise201Str[21]={
' ',' ',0xB1,0xB1,0xBE,0xA9,0xC0,0xFB,0xD1,0xC7,0xB5,0xC2,0xB9,0xAB,0xCB,0xBE,0
};
//深圳锐拓公司
const U8 g_Enterprise202Str[21]={
' ',' ',' ',' ',0xC9,0xEE,0xDB,0xDA,0xC8,0xF1,0xCD,0xD8,0xB9,0xAB,0xCB,0xBE,0
};

//深圳洲明公司
const U8 g_Enterprise203Str[21]={
' ',' ',' ',' ',0xC9,0xEE,0xDB,0xDA,0xD6,0xDE,0xC3,0xF7,0xB9,0xAB,0xCB,0xBE,0
};
//北京远程悦动信息技术
const U8 g_Enterprise204Str[21]={
0xB1,0xB1,0xBE,0xA9,0xD4,0xB6,0xB3,0xCC,0xD4,0xC3,0xB6,0xAF,0xD0,0xC5,0xCF,0xA2,0xBC,0xBC,0xCA,0xF5,0
};
//四川国彬电子科技
const U8 g_Enterprise205Str[21]={
' ',' ',0xCB,0xC4,0xB4,0xA8,0xB9,0xFA,0xB1,0xF2,0xB5,0xE7,0xD7,0xD3,0xBF,0xC6,0xBC,0xBC,0
};
//颐景丰科技
const U8 g_Enterprise206Str[21]={
' ',' ',' ',' ',0xD2,0xC3,0xBE,0xB0,0xB7,0xE1,0xBF,0xC6,0xBC,0xBC,0
};

//北京中恒示阳公司
const U8 g_Enterprise301Str[21]={
' ',' ',0xB1,0xB1,0xBE,0xA9,0xD6,0xD0,0xBA,0xE3,0xCA,0xBE,0xD1,0xF4,0xB9,0xAB,0xCB,0xBE,0
};
//上海诣阔科技
const U8 g_Enterprise4958Str[21]={
' ',' ',' ',' ',0xC9,0xCF,0xBA,0xA3,0xD2,0xE8,0xC0,0xAB,0xBF,0xC6,0xBC,0xBC,0
};
//三台奇玉广告
const U8 g_Enterprise7075Str[21]={
' ',' ',' ',' ',0xC8,0xFD,0xCC,0xA8,0xC6,0xE6,0xD3,0xF1,0xB9,0xE3,0xB8,0xE6,0
};
//绵阳崎升光电
const U8 g_Enterprise7076Str[21]={
' ',' ',' ',' ',0xC3,0xE0,0xD1,0xF4,0xC6,0xE9,0xC9,0xFD,0xB9,0xE2,0xB5,0xE7,0
};
//北京世纪瑞彩光电科技
const U8 g_Enterprise8476Str[21]={
0xB1,0xB1,0xBE,0xA9,0xCA,0xC0,0xBC,0xCD,0xC8,0xF0,0xB2,0xCA,0xB9,0xE2,0xB5,0xE7,0xBF,0xC6,0xBC,0xBC,0
};

