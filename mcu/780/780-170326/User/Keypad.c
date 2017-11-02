/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: keypad.c
 [Date]:        09-May-2012
 [Comment]:
   keypad subroutines.
 [Reversion History]:
*******************************************************************************/

#define _KEYPAD_C_

#include "header.h"
#include "strings.h"
#include <stdio.h>
#include "remote.h"

//extern struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;
//int regcnt=100; //计数寄存器 静态变量
extern U8 g_initPIPSrc;

U8 InputSrcName[6] = {0};

U8 tPIPSameSourceMatch[INPUTSOURCE_NUM-1][INPUTSOURCE_NUM-1] =
{
        //MAIN->   AV1, AV2,AV3,AV4, DVI, YPBPR,VGA,VGA2,HDMI ,SDI,DP
//SUB

/*MST_AV1    */    {   1,  0,    0,    0,      1,     1,     1,    1,       1,     1,     1},
/*MST_AV2    */    {   0,  1,    0,    0,      1,     1,     1,    1,       1,     1,     1},
/*MST_AV3    */    {   0,  0,    1,    0,      1,     1,     1,    1,       1,     1,     1},
/*MST_AV4    */    {   0,  0,    0,    1,      1,     1,     1,    1,       1,     1,     1},
/*DVI       */     {   1,  1,    1,    1,      1,     0,     1,    1,       0,     1,     1},
/*YPBPR     */     {   1,  1,    1,    1,      0,     1,     0,    0,       0,     1,     1},
/*VGA       */     {   1,  1,    1,    1,      1,     0,     1,    0,       1,     1,     1},
/*VGA2       */    {   1,  1,    1,    1,      1,     0,     0,    1,       1,     1,     1},
/*HDMI     */      {   1,  1,    1,    1,      0,     0,     1,    1,       1,     1,     1},
/*SDI*/            {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1},
/*DP*/             {   1,  1,    1,    1,      1,     1,     1,    1,       1,     1,     1},
};



void initGlobalVar()
{
	g_RTCErrorFlag=0;
	g_UserSetPasswordVal=1000000;
	g_TeleNumVal=0;
	g_TechTeleNumVal=0;
#ifdef INT_DELAY_PC_CTL
	g_PCSendFlag=__FALSE;
	g_OneSecondISRVal=0;
#endif
	g_ComTypeFlag=Communication_NONE;
	
	g_Set_First_Flag=1;
	g_StaticIPIdx=IP1_IDX;
	g_PCControlFlag=__FALSE;

	g_AdvertisingFlag=__FALSE;

	g_FlashLedFlag=__FALSE;
	g_PoweronFlag=__FALSE;
	
	g_TryTimeEndFlag=__FALSE;

	g_LockKeyFlag=UNLOCKKEYPAD;
	g_LockKeypadTime=0;//10;dont using lock function 
	g_LanguageFlag=CHINESE;

	g_Current_Scene=Scene_1;
	g_Image_Scr=0;
	LCD_UI_NO=1;
	
	LCD_BACK_NUM=100;   //用来调节背光亮度
	LCD_BACK_NUM_TIME=0;
				
}

BOOL IsSetParaValueState()//判断当前是否为参数赋值状态
{
	switch(g_CurrentState)
	{		
		case State_WizardSet_OnePort_layout1:
		case State_WizardSet_TwoPort_layout1:
		case State_WizardSet_TwoPort_layout2:
		case State_WizardSet_ThreePort_layout1:
		case State_WizardSet_ThreePort_layout2:
		case State_WizardSet_FourPort_layout1:
		case State_WizardSet_FourPort_layout2:
		case State_WizardSet_FourPort_layout3:	
		case State_ScreenSet_OnePort_layout1:
		case State_ScreenSet_TwoPort_layout1:
		case State_ScreenSet_TwoPort_layout2:
		case State_ScreenSet_ThreePort_layout1:
		case State_ScreenSet_ThreePort_layout2:
		case State_ScreenSet_FourPort_layout1:
		case State_ScreenSet_FourPort_layout2:
		case State_ScreenSet_FourPort_layout3:	
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


static _InputSourceType TwoMode[2]={INPUTSOURCE_NUM,INPUTSOURCE_DVI};          //two mode recall

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



void DealCurrentState(State CurState)
{	
	g_CurrentState=CurState;

	if(stateData[g_CurrentState].DisBackFunc!=NULL)
		stateData[g_CurrentState].DisBackFunc(CurState);	// 根据当前状态在串口屏上显示信息/////+++++++++++++
}


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

	DealCurrentState(State_Lock_Key);

	delay(5);
	DealEscKey();	
	if(g_LanguageFlag==CHINESE)


	
		chinesedisplay(0x88, g_LockKeyStr+4);
	else
		chinesedisplay(0x88, "LOCK");		
}

void KeypadLock()
{	
	g_CurrentState=State_Lock_Key;
	clearlcd();
	g_LockKeyFlag=LOCKKEYPAD;

	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x90, g_LockKeyStr);
	else
		chinesedisplay(0x90, g_LockKeyStrEn);	

	DealCurrentState(State_Lock_Key);

	delay(5);
	SetKeyLEDLight(KEY_NUM);
	DealEscKey();
	
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x88, g_LockKeyStr+4);
	else
		chinesedisplay(0x88, "LOCK");	
	
}



BOOL Check_OK_Cancel_State()
{
	switch(g_CurrentState)
	{		
		case State_FactoryReset_OK:
		case State_FactoryReset_Yes:
			return __TRUE;
	}
	return __FALSE;
}


void Start_LED_Flashing(U8 num)
{
	g_FlashLedFlag=__TRUE;
	g_FlashLedNum=num;
}



void ScanKeypadForSerial()//键盘长按
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
		if(keyVal==OK_KEY)
		{
			DealOkKey();
		}
		else if(keyVal==ESC_KEY) 
		{
			DealEscKey();
		}
		else if(keyVal==DEBUG_KEY&&g_CurrentState!=State_DebugMode_Start&&g_CurrentState!=State_User_Input_SerialNumber)
		{
			DealCurrentState(State_DebugMode_Start);
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



u8 LoadKey_to_num(U8 cnt)
{
	U8 num=0;
	switch(cnt)
	{
		case NUM1_KEY:
			num=1;
			break;
		case NUM2_KEY:
			num=2;
			break;
		case NUM3_KEY:
			num=3;
			break;
		case NUM4_KEY:
			num=4;
			break;
		case NUM5_KEY:
			num=5;
			break;
		case NUM6_KEY:
			num=6;
			break;
		case NUM7_KEY:
			num=7;
			break;
		case NUM8_KEY:
			num=8;
			break;
		case NUM9_KEY:
			num=9;
			break;
		case NUM0_KEY:
			num=0;
			break;	
		default:
			return __FALSE;
	}
	return num;
}


#ifdef EC11_BUTTON

U8 g_RorL=0;
U8 g_numvalue=0;

void CodingWitchPolling()
{
	static U8 Aold,Bold;	//用来存储上一次调用时编码开关两引脚的电平
	static U8 st=0;		//用来存储以前是否出现两引脚都为高电平的状态
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
	CodingWitchPolling();
}
#endif


/***************键盘扫描函数***************/
void ScanKeypad(void)
{
	//u8 rxdata;
	static U32 LeisureCount=0,LeisureCount1=0;
	int i=0;
	U8 flag=0;
	U8 load_num=0;
	static U8 KeyACC=0;
	static _KeypadNum keyPress=KEY_NUM;
	_KeypadNum keyVal=KEY_NUM;

	#ifdef USE_INFRARED_IR  //遥控板
	U8 Remote_Long_Key=0;
	_KeypadNum IRkeyPress=KEY_NUM;
	if(Remote_Scan())
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
					if(IsSetParaValueState())
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
				}
				break;	
			case 0x48: // 局部/步长

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
				}
				break;				
		}
		RmtCnt=0;
	}
#endif
	KB_PLOAD_L;			//PB14=0		FIO0CLR |= 0x00040000;    //pload=0	p0.18
	//-------------------------------------------
	for(i=0;i<KEY_NUM;i++)    
	{
		KB_PLOAD_H;		//PB14=1		FIO0SET |= 0x00040000;    //pload=1	p0.18
		KB_SCK_L;		//PB12=0		FIO0CLR |= 0x00020000;    //sck=0	p0.17

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
			} 
			//长按上下键的时候，可以连续发送命令
			if(KeyACC>=10&&(keyPress==UP_KEY||keyPress==DOWN_KEY))
			{
				flag=1;
 				keyVal=keyPress;  //按键弹起的时候,对KeyVal赋值
			}
		}
		else if(keyPress==(KEY_NUM-1-i))
		{	//此处是长按有特殊作用的按键，弹起来的时候不再起短按作用
//			if(KeyACC>0&&!(KeyACC>=LOGNTIME&&(keyPress==LOCK_KEY||keyPress==NUM0_KEY||keyPress==NUM5_KEY				
//				||keyPress==FREEZE_KEY||keyPress==DEBUG_KEY||keyPress==DUAL_KEY)))
//			{
//				flag=1;
//				keyVal=keyPress;  //按键弹起的时候,对KeyVal赋值
//			}	
			if(KeyACC>0&&!(KeyACC>=LOGNTIME&&(keyPress==LOCK_KEY||keyPress==SCEN_KEY||keyPress==NUM5_KEY				
				||keyPress==DEBUG_KEY  || keyPress==MUTE_KEY ||keyPress==VGA_KEY)))
			{
				flag=1;
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
				KeypadLockedPrompt();//键盘锁定，提示解锁函数	
			}
			keyPress=KEY_NUM;
			KeyACC=0;
			g_FlashLedFlag=__FALSE;
		}
		else  //此处是没有按按键
		{
			if(g_LockKeypadTime!=0)
			{//ten minutes
				if(LeisureCount==(g_LockKeypadTime*0x1103B9D))  //大约一分钟
				{
					if(g_LockKeyFlag==UNLOCKKEYPAD&&KeyACC==0)
					{
						LeisureCount=0;
						KeypadLock();
					}
				}
				else if(g_LockKeyFlag==UNLOCKKEYPAD)
				{				
					LeisureCount++;
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
			LCD_BACK_NUM_TIME=0;
			if(LCD_BACK_NUM<100)
			{
				LCD_BACK_NUM=100;	
				Send_LCD_BACK(LCD_BACK_NUM);			
			}
			if(keyVal==OK_KEY)//menu
			{
					DealOkKey();
			}
			else if(keyVal==ESC_KEY)
			{
					DealEscKey();
			}

			else if(!Check_OK_Cancel_State())  //判断是否的选择
			{
				 if(keyVal==UP_KEY)
				{
						DealUpKey();
				}
				else if(keyVal==DOWN_KEY) 
				{
						DealDownKey();
				}
				else if(IsSetParaValueState())		//4位数据的输入	
				{	

						DealScreenNumber(keyVal,LCDADDR91,4);
				}
					switch(g_CurrentState)
					{			
						case State_Input_PackagingName:
						case State_Input_TestName:
						case State_Input_SaleName:
						case State_User_Input_Logo:	
							DealScreenNumber(keyVal,LCDADDR91,5);
							break;	
						case State_LockPadTime_Set:	
						case State_UserTry_ShowTime:  //演示时长设置
						case State_UserTry_OnceTime:  //单次时长设置
							DealScreenNumber(keyVal,LCDADDR91,2);
							break;
						case State_DebugMode_Start:
						case State_User_Input_SerialNumber:
						case State_User_Old_SerialNumber:
						case State_User_Set_SerialNumber:
						case State_UserTrySet_PassWord:
						case State_TryTime_End:
							DealScreenNumber(keyVal,LCDADDR91,6);
							break;		
						case State_UserTry_Once_Password100_new:		
								DealScreenNumber(keyVal,0x86,4);
							break;
						case State_UserTry_Once_Password100_new1:		
								DealScreenNumber(keyVal,0x88,4);
							break;
						
						
						case State_UserTry_OnceEnd:   //输入演示密码或者单次密码
							DealScreenNumber(keyVal,LCDADDR91,9);
							break;
						case 	State_UserTry_Once_Password101:
						case State_UserTry_Once_Password102:
						case State_UserTry_Once_Password103:
						case State_UserTry_Once_Password104:
						case State_UserTry_Once_Password105:	
						case State_UserTry_Once_Password100:		
								DealScreenNumber(keyVal,0x87,6);
							break;
						case State_UserTry_OnceTel:   //启动演示密码或者单次密码时，设置授权电话  //20170213 lyg	
							DealScreenNumber(keyVal,LCDADDR91,11);
							break;
					
					
						case State_UserTrySet_Day:
							DealScreenNumber(keyVal,LCDADDR91,3);
							break;
						case State_UserTrySet_Tel:
							DealScreenNumber(keyVal,LCDADDR91,12);
							break;
						case State_User_Set_TelNumber:
						case State_User_Set_Tech_TelNumber:
							DealScreenNumber(keyVal,LCDADDR91,11);
							break;
						case State_User_Input_MathineType:
							DealScreenNumber(keyVal,LCDADDR91,10);
							break;
//---------------------------------------------------------------------------------------
					//用来输入行列值，一位数据
					case State_WizardSet_OutPort_Num:
					case State_WizardSet_TwoPort_Mode:
					case State_WizardSet_ThreePort_Mode:
					case State_WizardSet_FourPort_Mode:
					case State_ScreenSet_OutPort_Num:	
					case State_ScreenSet_TwoPort_Mode:
					case State_ScreenSet_ThreePort_Mode:
					case State_ScreenSet_FourPort_Mode:
					case State_ScenarioSet_NewImage_ImageNum:
					case State_ScenarioSet_DelScheme_Num:  
					case	State_ScenarioSet_SchemeNum:
						DealScreenNumber(keyVal,0x97,1);
						break;
					
					case State_MatrixMode_ModeSet_input:
					case State_MatrixMode_ModeSet_output:
						DealScreenNumber(keyVal,0x97,1);
						break;
					case State_ScenarioSet_NewImage_ImagePara_Begin_X:
					case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
					case State_ScenarioSet_NewImage_ImagePara_End_X:
					case State_ScenarioSet_NewImage_ImagePara_End_Y:					
						DealScreenNumber(keyVal,0x97,5);
						break;
					
					case State_MainMenu_Menu_ScreenSet_Password:
					case State_MainMenu_Menu_ScenarioSet_Password:
						DealScreenNumber(keyVal,0x97,3);
						break;

//---------------------------------------------------------------------------------------
					case State_AdvanceSet_CardTest_Port:
					case State_AdvanceSet_CardTest_Color:
					case State_WizardSet_InputSource_Set:
					case State_ScenarioSet_NewImage_ImagePara_Source:
						DealInputNumber(keyVal);
						break;

/////////////////lyg//////////////////////						
					case State_UserPara_SetNewTime_Date:
						Deal_SetNewTime_Date(keyVal);
						break;
					case State_UserPara_SetNewTime_Time:
						Deal_SetNewTime_Time(keyVal);
						break;
					case State_UserPara_SetNewTime_Week:
						Deal_SetNewTime_Week(keyVal);
						break;						
////////////////////////////////////////////
					case State_MainMenu_Menu:
								switch(keyVal)
						{
							case SCENARIO_1:
							case SCENARIO_2:
							case SCENARIO_3:
							case SCENARIO_4:
							case SCENARIO_5:
							case SCENARIO_6:
							case SCENARIO_7:
							case SCENARIO_8:
							case SCENARIO_9:
								
								//场景调用//////////////////////////
							load_num=LoadKey_to_num(keyVal);
							if(load_num>0 &&load_num!=g_Current_Scene)
							{
								load_scenario_from_FPGA(load_num);
								g_Current_Scene=load_num;
								Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,g_Current_Scene);
								ColorDisplayCurrentScenario();
							}
							
								break;
							case DEBUG_KEY:         //
								DealRecallKey();    //调用模式
								break;					
							default:
								LEDWarning(keyVal); 
						}
						break;
					default:
						break;
					}		
			}
			else if(Check_OK_Cancel_State())
			{
				LEDWarning(keyVal); 	
			}

			}
		}//以上按键键盘锁定后，无效；
			
			if(KeyACC==LOGNTIME && keyPress==SCEN_KEY) //如果通过快捷键设置场景
			{
//					g_CurrentState=State_ScreenSet_LineNum;  //切换到场景设置输入ID页面
					DealCurrentState(g_CurrentState);	
			}			
			if(KeyACC==LOGNTIME&&keyPress==DEBUG_KEY
				#ifdef USE_INFRARED_IR
					||(Remote_Long_Key==1&&IRkeyPress==DEBUG_KEY)
				#endif
				)
			{
				g_LockKeyFlag=UNLOCKKEYPAD;
				DealCurrentState(State_DebugMode_Start);
			}
				
			if(KeyACC==LOGNTIME&&keyPress==LOCK_KEY
				#ifdef USE_INFRARED_IR
					||(Remote_Long_Key==1&&IRkeyPress==LOCK_KEY)
				#endif
				)
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

					DealCurrentState(State_Lock_Key);
					delay(5);
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


StateData stateData[]=
{
	//State_MainMenu_Menu*********************************MAIN***************0.0.0
	{
		0x80,
		"",
		"",
		0x90,
		g_CurrentTimeStr,
		g_CurrentTimeStrEn,
		{
			State_MainMenu_Menu_IPInfo,		//up
			State_MainMenu_Menu_VersionInfo,	//down
			State_MainMenu_Menu_ScreenSet,   			//ok
			State_MainMenu_Menu},  			//esc
		NULL,
		NULL,
		NULL,DisBackGround
	},
	//State_MainMenu_Menu_StartInfo,	
	{
		0x80,
		g_StartTimeStr,
		"Turn-on:    day",
		0x90,
		g_StartTimeFormatStr,
		"  :   :",
		{
			State_MainMenu_Menu,   			    //up
			State_MainMenu_Menu_VersionInfo,  	//down
			State_MainMenu_Menu,   			    //ok
			State_MainMenu_Menu},			    //esc
		NULL,
		NULL,
		NULL,DisBackGround
	},
	//State_MainMenu_Menu_VersionInfo,*********************************MAIN***************0.1.0
	{
		0x80,
		g_MachineTypeStr,
		g_MachineTypeStrEn,
		0x90,
		g_SoftVersionStr,
		"VerS:",
		//"Version:",
		{
			State_MainMenu_Menu,   	//up
			State_MainMenu_Menu_InOutput,  		//down
			State_MainMenu_Menu,   			    //ok
			State_MainMenu_Menu},		   	    //esc
		NULL,
		NULL,
		NULL,DisBackGround
	},
	//State_MainMenu_Menu_InOutput*********************************MAIN***************0.2.0
	{
		0x80,
		g_MathineIDStr,
		"ID:",
		0x90,
		" ",
		" ",
		{State_MainMenu_Menu_VersionInfo,		//up
			State_MainMenu_Menu_Logo,	    //down
			State_MainMenu_Menu,				//ok
			State_MainMenu_Menu},			    //esc
		NULL,
		NULL,
		NULL,DisBackGround
	},
	//State_MainMenu_Menu_Logo,*********************************MAIN***************0.3.0
	{
		0x80,
		g_InResolutionStr,
		"In:",
		0x90,
		g_OutResolutionStr,
		"Out:",
		{State_MainMenu_Menu_InOutput,             //up
			State_MainMenu_Menu_IPInfo,
			State_MainMenu_Menu,                 //ok
			State_MainMenu_Menu},                //esc
		NULL,
		NULL,
		NULL,DisBackGround
	},
		//State_MainMenu_Menu_IPInfo,*********************************MAIN***************0.3.0
	{
		0x80,
		g_InResolutionStr,
		"In:",
		0x90,
		g_OutResolutionStr,
		"Out:",
		{State_MainMenu_Menu_Logo,             //up
			State_MainMenu_Menu,
			State_MainMenu_Menu,                 //ok
			State_MainMenu_Menu},                //esc
		NULL,
		NULL,
		NULL,DisBackGround
	},

	//State_MainMenu_Menu_OperationGuide*********************************MAIN***************0.4.0
	{
		0x80,g_SupportTechStr,g_SupportTechStrEn,
		0x90,g_ContactTel1,g_ContactTel1,
		{
			State_MainMenu_Menu_Logo,//State_MainMenu_Menu_Current_Date,   //up
			State_MainMenu_Menu_OperationGuide1,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL,&DisBackGround		
	},
	
	//State_MainMenu_Menu_OperationGuide1*********************************MAIN***************0.4.0
	{
		0x80,g_SupportTechStr,g_SupportTechStrEn,
		0x90,g_ContactTel1,g_ContactTel1,
		{
			State_MainMenu_Menu_OperationGuide,//State_MainMenu_Menu_Current_Date,   //up
			State_MainMenu_Menu,  //down
			State_MainMenu_Menu,   //ok
			State_MainMenu_Menu},  //esc
		NULL,
		NULL,
		NULL,&DisBackGround		
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

	//State_MainMenu_Menu_ScreenSet,*********************************一大屏设置***************1.0.0
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_DirectSetStr,g_DirectSetStrEn,
		{
			State_MainMenu_Menu_AdvanceSet,
			State_MainMenu_Menu_ScenarioSet,//State_MainMenu_Menu_ScalerPara,
			State_ScreenSet_OutPort_Num,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		NULL,&DisBackGround
	},
	//State_MainMenu_Menu_ScreenSet_Password,*********************************一大屏设置***************1.0.0
	{
		LCDADDR80,g_MainMenu,g_MainMenuEn,
		LCDADDR90,g_DirectSetStr,g_DirectSetStrEn,
		{
			State_MainMenu_Menu_ScreenSet_Password,
			State_MainMenu_Menu_ScreenSet_Password,//State_MainMenu_Menu_ScalerPara,
			State_ScreenSet_OutPort_Num,			
			State_MainMenu_Menu_ScreenSet
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScreenSet_OutPort_Num,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_OutPort_Num,
			State_ScreenSet_OutPort_Num,
			State_ScreenSet_OnePort_layout1,			
			State_MainMenu_Menu_ScreenSet
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_ScreenSet_TwoPort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_TwoPort_Mode,
			State_ScreenSet_TwoPort_Mode,
			State_ScreenSet_TwoPort_layout1,			
			State_ScreenSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_ScreenSet_ThreePort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_ThreePort_Mode,
			State_ScreenSet_ThreePort_Mode,
			State_ScreenSet_ThreePort_layout1,			
			State_ScreenSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_ScreenSet_FourPort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_TwoPort_Mode,
			State_ScreenSet_TwoPort_Mode,
			State_ScreenSet_FourPort_layout1,			
			State_ScreenSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//  State_ScreenSet_OnePort_layout1,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_OnePort_layout1,
			State_ScreenSet_OnePort_layout1,
			State_ScreenSet_Display,			
			State_ScreenSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_TwoPort_layout1,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_TwoPort_layout1,
			State_ScreenSet_TwoPort_layout1,
			State_ScreenSet_Display,			
			State_ScreenSet_TwoPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_TwoPort_layout2,
		{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_TwoPort_layout2,
			State_ScreenSet_TwoPort_layout2,
			State_ScreenSet_Display,	
			State_ScreenSet_TwoPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_ThreePort_layout1,
		{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_ThreePort_layout1,
			State_ScreenSet_ThreePort_layout1,
			State_ScreenSet_Display,			
			State_ScreenSet_ThreePort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_ThreePort_layout2,
		{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_ThreePort_layout2,
			State_ScreenSet_ThreePort_layout2,
			State_ScreenSet_Display,			
			State_ScreenSet_ThreePort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_FourPort_layout1,
		{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_FourPort_layout1,
			State_ScreenSet_FourPort_layout1,
			State_ScreenSet_Display,			
			State_ScreenSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_FourPort_layout2,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_FourPort_layout2,
			State_ScreenSet_FourPort_layout2,
			State_ScreenSet_Display,			
			State_ScreenSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_ScreenSet_FourPort_layout3,
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_FourPort_layout3,
			State_ScreenSet_FourPort_layout3,
			State_ScreenSet_Display,			
			State_ScreenSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//State_ScreenSet_Display
	{
		0," "," ",
		0," "," ",
		{
			State_ScreenSet_Display,
			State_ScreenSet_Display,
			State_MainMenu_Menu_ScreenSet,	
			State_MainMenu_Menu_ScreenSet
		},ScreenSet_Display,
		NULL,NULL,&DisBackGround
	},
	//State_MainMenu_Menu_ScenarioSet,*********************************二新建方案***************1.0.0
	{
		0," "," ",
		0," "," ",
		{
			State_MainMenu_Menu_ScreenSet, //向上键走到大屏设置项
			State_MainMenu_Menu_MatrixMode,  
			State_ScenarioSet_NewScheme,			
			State_MainMenu_Menu
		},NULL,
		NULL,
		NULL,&DisBackGround
	},	
		//State_MainMenu_Menu_ScenarioSet_Password,*********************************二新建方案***************1.0.0
	{
		0," "," ",
		0," "," ",
		{
			State_MainMenu_Menu_ScenarioSet_Password, //向上键走到大屏设置项
			State_MainMenu_Menu_ScenarioSet_Password,  
			State_ScenarioSet_NewScheme,			
			State_MainMenu_Menu_ScenarioSet
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewScheme,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_DelScheme, 
			State_ScenarioSet_DelScheme,  
			State_ScenarioSet_SchemeNum,	
			State_MainMenu_Menu_ScenarioSet
		},NULL,
		NULL,
		NULL,&DisBackGround
	},	
	//State_ScenarioSet_SchemeNum,*********************************二新建方案***************1.1.2
	{
		0," "," ",
		0," "," ",
		{
			
			State_ScenarioSet_SchemeNum,
			State_ScenarioSet_SchemeNum, 
			State_ScenarioSet_NewImage_ImageNum,			
			State_ScenarioSet_NewScheme
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},	
	//State_ScenarioSet_SchemeSaved,*********************************二新建方案***************1.1.2
	{
		0," "," ",
		0," "," ",
		{
			
			State_ScenarioSet_SchemeSaved,
			State_ScenarioSet_SchemeSaved, 
			State_ScenarioSet_NewImage_ImageNum,			
			State_ScenarioSet_SchemeNum
		},NULL,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_ImageNum,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImageNum, 
			State_ScenarioSet_NewImage_ImageNum,  
			State_ScenarioSet_NewImage_ImagePara_Begin_X,	
			State_ScenarioSet_NewScheme
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},	
	
	//State_ScenarioSet_NewImage_ImagePara_Begin_X,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImagePara_End_Y, 
			State_ScenarioSet_NewImage_ImagePara_Begin_Y,  
			State_ScenarioSet_NewImage_ImagePara_Begin_Y,	
			State_ScenarioSet_NewImage_ImageNum
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},	
	//State_ScenarioSet_NewImage_ImagePara_Begin_Y,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImagePara_Begin_X, 
			State_ScenarioSet_NewImage_ImagePara_End_X,  
			State_ScenarioSet_NewImage_ImagePara_End_X,	
			State_ScenarioSet_NewImage_ImagePara_Begin_X
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_ImagePara_End_X,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImagePara_Begin_Y, 
			State_ScenarioSet_NewImage_ImagePara_End_Y,  
			State_ScenarioSet_NewImage_ImagePara_End_Y,	
			State_ScenarioSet_NewImage_ImagePara_Begin_Y
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_ImagePara_End_Y,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImagePara_End_X, 
			State_ScenarioSet_NewImage_ImagePara_Begin_X,  
			State_ScenarioSet_NewImage_ImagePara_Source,	
			State_ScenarioSet_NewImage_ImagePara_End_X
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_ImagePara_Source,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewImage_ImagePara_Source, 
			State_ScenarioSet_NewImage_ImagePara_Source,  
			State_ScenarioSet_NewImage_OK_NOT,	
			State_ScenarioSet_NewImage_ImagePara_Begin_X
		},Add_New_Image,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_ImagePara_Save,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{			
			State_ScenarioSet_NewImage_ImagePara_Source,
			State_ScenarioSet_NewImage_ImagePara_Begin_X,   
			State_ScenarioSet_NewImage_OK_NOT,	
			State_ScenarioSet_NewImage_ImageNum
		},Add_New_Image,
		NULL,
		NULL,&DisBackGround
	},
	//State_ScenarioSet_NewImage_OK_NOT,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{			
			State_ScenarioSet_NewImage_OK_NOT,
			State_ScenarioSet_NewImage_OK_NOT,   
			State_ScenarioSet_NewScheme,	
			State_ScenarioSet_NewImage_Continue
		},NULL,
		NULL,
		NULL,&DisBackGround
	},	
	//State_ScenarioSet_NewImage_Continue,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{			
			State_ScenarioSet_NewImage_Continue,
			State_ScenarioSet_NewImage_Continue,   
			State_ScenarioSet_NewImage_ImagePara_Begin_X,	
			State_ScenarioSet_NewScheme
		},NULL,
		NULL,
		NULL,&DisBackGround
	},
	
	

		//State_ScenarioSet_DelScheme,*********************************二新建方案***************1.1.1
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_NewScheme,  
			State_ScenarioSet_NewScheme, 		
			State_ScenarioSet_DelScheme_Num,			
			State_MainMenu_Menu_ScenarioSet
		},NULL,
		NULL,
		NULL,&DisBackGround
	},	
		//State_ScenarioSet_DelScheme_Num,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_DelScheme_Num, 
			State_ScenarioSet_DelScheme_Num,  
			State_ScenarioSet_DelScheme_Num_OK,	
			State_ScenarioSet_DelScheme
		},DirectSetValue,
		NULL,
		NULL,&DisBackGround
	},
		//State_ScenarioSet_DelScheme_Num_OK,*********************************二新建方案***************1.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_ScenarioSet_DelScheme_Num_OK, 
			State_ScenarioSet_DelScheme_Num_OK,  
			State_ScenarioSet_DelScheme,	
			State_ScenarioSet_DelScheme_Num
		},DelScheme_OK,
		NULL,
		NULL,&DisBackGround
	},

	//State_MainMenu_Menu_MatrixMode,
	{
		0,NULL,NULL,
		0,NULL,NULL,
		{
			State_MainMenu_Menu_ScenarioSet,
			State_MainMenu_Menu_WizardSet,
			State_MatrixMode_ModeSet_input,
			State_MainMenu_Menu,		
		},NULL,
		NULL,NULL,&DisBackGround
	},
	
	//State_MatrixMode_ModeSet_input,
	{
		0,NULL,NULL,
		0,NULL,NULL,
		{
			State_MatrixMode_ModeSet_input,
			State_MatrixMode_ModeSet_input,
			State_MatrixMode_ModeSet_output,
			State_MainMenu_Menu_MatrixMode,		
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//State_MatrixMode_ModeSet_output,
	{
		0,NULL,NULL,
		0,NULL,NULL,
		{
			State_MatrixMode_ModeSet_output,
			State_MatrixMode_ModeSet_output,
			State_MainMenu_Menu_MatrixMode,
			State_MatrixMode_ModeSet_input,		
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	
	//State_MainMenu_Menu_WizardSet,*********************三 向导设置*******5.0.0
	{
			0," "," ",
			0," "," ",
		{
			State_MainMenu_Menu_MatrixMode,
			State_MainMenu_Menu_AdvanceSet,
			State_WizardSet_Welcome,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL,&DisBackGround
	},	
	//State_WizardSet_Welcome,*********************三 向导设置*******5.0.0
	{
			0," "," ",
		0," "," ",
		{
			State_WizardSet_Welcome,
			State_WizardSet_Welcome,
			State_WizardSet_OutPort_Num,			
			State_MainMenu_Menu_WizardSet
		},NULL,
		NULL,NULL,&DisBackGround
	},	
	//	State_WizardSet_OutPort_Num,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_OutPort_Num,
			State_WizardSet_OutPort_Num,
			State_WizardSet_OnePort_layout1,			
			State_MainMenu_Menu_WizardSet
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_WizardSet_TwoPort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_TwoPort_Mode,
			State_WizardSet_TwoPort_Mode,
			State_WizardSet_TwoPort_layout1,			
			State_WizardSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_WizardSet_ThreePort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_ThreePort_Mode,
			State_WizardSet_ThreePort_Mode,
			State_WizardSet_ThreePort_layout1,			
			State_WizardSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_WizardSet_FourPort_Mode,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_TwoPort_Mode,
			State_WizardSet_TwoPort_Mode,
			State_WizardSet_FourPort_layout1,			
			State_WizardSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//  State_WizardSet_OnePort_layout1,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_OnePort_layout1,
			State_WizardSet_OnePort_layout1,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_OutPort_Num
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_TwoPort_layout1,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_TwoPort_layout1,
			State_WizardSet_TwoPort_layout1,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_TwoPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_TwoPort_layout2,
		{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_TwoPort_layout2,
			State_WizardSet_TwoPort_layout2,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_TwoPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_ThreePort_layout1,
		{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_ThreePort_layout1,
			State_WizardSet_ThreePort_layout1,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_ThreePort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_ThreePort_layout2,
		{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_ThreePort_layout2,
			State_WizardSet_ThreePort_layout2,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_ThreePort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_FourPort_layout1,
		{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_FourPort_layout1,
			State_WizardSet_FourPort_layout1,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_FourPort_layout2,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_FourPort_layout2,
			State_WizardSet_FourPort_layout2,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_WizardSet_FourPort_layout3,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_FourPort_layout3,
			State_WizardSet_FourPort_layout3,
			State_WizardSet_InputSource_Set,			
			State_WizardSet_FourPort_Mode
		},DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_WizardSet_InputSource_Set,
	{
		0," "," ",
		0," "," ",
		{
			State_WizardSet_InputSource_Set,
			State_WizardSet_InputSource_Set,
			State_MainMenu_Menu,			
			State_MainMenu_Menu
		},WizardSet_OK_Function,
		NULL,NULL,&DisBackGround
	},
		//State_MainMenu_Menu_AdvanceSet,******************四高级设置**************5.0.0
	{
			0," "," ",
		0," "," ",
		{
			State_MainMenu_Menu_WizardSet,
			State_MainMenu_Menu_ScreenSet,
			State_AdvanceSet_CardTestSet,			
			State_MainMenu_Menu
		},NULL,
		NULL,NULL,&DisBackGround
	},

	
	//State_AdvanceSet_CardTestSet,***********************四 高级设置图卡设置***************5.1.0
	{
		0," "," ",
		0," "," ",
		{
			State_AdvancePara_FactoryReset,
      State_AdvanceSet_DebugModeSet,
			State_AdvanceSet_CardTest_Port,
			State_MainMenu_Menu_AdvanceSet
		},NULL,
		NULL,NULL,&DisBackGround
	},
	//State_AdvanceSet_CardTest_Port,
	{
		0," "," ",
		0," "," ",
		{
			State_AdvanceSet_CardTest_Color,
			State_AdvanceSet_CardTest_Color,
			State_AdvanceSet_CardTest_Port,			
			State_AdvanceSet_CardTestSet
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//State_AdvanceSet_CardTest_Color,
	{
		0," "," ",
		0," "," ",
		{
			State_AdvanceSet_CardTest_Port,
			State_AdvanceSet_CardTest_Port,
			State_AdvanceSet_CardTest_Color,			
			State_AdvanceSet_CardTestSet
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//State_AdvanceSet_DebugModeSet,
	{
		0," "," ",
		0," "," ",
		{
			State_AdvanceSet_CardTestSet,
			State_UserPara_SetCurrentTime,
			State_DebugMode_Start,			
			State_MainMenu_Menu_AdvanceSet
		},&SetCurrentStateValue,
		NULL,NULL,&DisBackGround
	},
		//State_DebugMode_Start++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_DebugMode_Start,
			State_DebugMode_Start,
			State_DebugMode_Start,	
			State_AdvanceSet_DebugModeSet
		},&DebugSetValue,
		NULL,NULL,&DisBackGround
	} ,
		//State_UserTrySet_Start++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_UserTrySet_End,
			State_UserTrySet_End,
			State_UserTrySet_Day,	
			State_DebugMode_Start
		},NULL,
		NULL,NULL,&DisBackGround
	} ,
		//State_UserTrySet_End++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_UserTrySet_Start,
			State_UserTrySet_Start,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&UserTry_SetData_Clear,
		NULL,NULL,&DisBackGround
	} ,
	//State_UserTrySet_Day++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_UserTrySet_Tel,
			State_UserTrySet_PassWord,
			State_UserTrySet_PassWord,	
			State_UserTrySet_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
	//State_UserTrySet_PassWord,++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_UserTrySet_Day,
			State_UserTrySet_Tel,
			State_UserTrySet_Tel,	
			State_UserTrySet_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
	//	State_UserTrySet_Tel,++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_UserTrySet_PassWord,
			State_UserTrySet_Day,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
	
	//	State_TryTime_End,,++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		0," "," ",
		0," "," ",
		{
			State_TryTime_End,
			State_TryTime_End,
			State_TryTime_End,	
			State_TryTime_End
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
	
	
	//State_AdvanceSet_HelpInfo,
	{
		0," "," ",
		0," "," ",
		{
			State_UserPara_SetCurrentTime,
			State_AdvancePara_FactoryReset,
			State_HelpInfo_Wizard,			
			State_MainMenu_Menu_AdvanceSet
		},NULL,
		NULL,NULL,&DisBackGround
	},
	
		//State_HelpInfo_Wizard,,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_IP,
			State_HelpInfo_Screen,
			State_HelpInfo_Screen,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
	//State_HelpInfo_Screen,,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_TechSupport,
			State_HelpInfo_Scenario,
			State_HelpInfo_Scenario,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
		//State_HelpInfo_Scenario,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_Screen,
			State_HelpInfo_Matrix,
			State_HelpInfo_Matrix,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
		//	State_HelpInfo_Matrix,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_Scenario,
			State_HelpInfo_TechSupport,
			State_HelpInfo_TechSupport,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
		//	State_HelpInfo_TechSupport,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_Matrix,
			State_HelpInfo_IP,
			State_HelpInfo_Wizard,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
	
	//	State_HelpInfo_IP,
	{
		0," "," ",
		0," "," ",
		{
			State_HelpInfo_Matrix,
			State_HelpInfo_Wizard,
			State_HelpInfo_Wizard,			
			State_AdvanceSet_HelpInfo
		},NULL,
		NULL,NULL,&DisBackGround
	},
	
	
	
	
	//State_AdvanceSet_TechSupport,
	{
		0," "," ",
		0," "," ",
		{
			State_AdvanceSet_HelpInfo,
			State_AdvancePara_FactoryReset,
			State_AdvanceSet_TechSupport,			
			State_MainMenu_Menu_AdvanceSet
		},&SetCurrentStateValue,
		NULL,NULL,&DisBackGround
	},
	

	//State_AdvancePara_LockPadTime*********************************六高级设置***************6.2.0
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_LockKeypadStr,g_LockKeypadStrEN,
		{
			State_LockPadTime_Set,
			State_LockPadTime_Set,
			State_LockPadTime_Set,	
			State_LockPadTime_Set
		},NULL,
		NULL,NULL,&DisBackGround
	},
	//State_LockPadTime_Set
	{
		LCDADDR80,g_LockKeypadStr,g_LockKeypadStrEN,
		LCDADDR91,g_CancelLockKeypadStr,"_     (0:Cancel)",
		{
			State_LockPadTime_Set,
			State_LockPadTime_Set,
			State_AdvanceSet_DebugModeSet,	
			State_AdvanceSet_DebugModeSet
		},NULL,
		NULL,NULL,&DisBackGround
	},
	//State_AdvancePara_FactoryReset,*********************************六高级设置***************6.3.0
	{
		LCDADDR80,g_AdvancePara,g_AdvanceParaEn,
		LCDADDR90,g_FactoryReset,g_FactoryResetEn,
		{
			State_AdvanceSet_HelpInfo,
			State_AdvanceSet_CardTestSet,
			State_FactoryReset_Yes,	
			State_MainMenu_Menu_AdvanceSet
		},NULL,
		NULL,NULL,&DisBackGround
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
		NULL,NULL,&DisBackGround
	},
	//State_FactoryReset_OK,/************************************************State_FactoryReset_OK*********/
	{
		LCDADDR80,g_Yes,g_YesEn,
		LCDADDR91,g_YesOrNoStr,g_YesOrNoStrEn,
		{
			State_FactoryReset_Yes,
			State_FactoryReset_Yes,
			State_FactoryReset_OK,	
			State_FactoryReset_Yes
		},&SetCurrentStateValue,
		NULL,NULL,&DisBackGround
	},
	
 	//State_UserPara_SetCurrentTime,*********************************六高级设置***************6.6.1
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetTimeStr,g_SetTimeStrEn,
		{
			State_AdvanceSet_DebugModeSet,
      State_AdvanceSet_HelpInfo,
			State_UserPara_SetNewTime_Date,	
			State_MainMenu_Menu_AdvanceSet
		},NULL,//****************************************************************WAITING
		NULL,NULL,&DisBackGround		
	},
	//State_UserPara_SetCurrentTime_Date,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		{
			State_UserPara_SetNewTime_Week,
			State_UserPara_SetNewTime_Time,
			State_UserPara_SetNewTime_Time,	
			State_UserPara_SetCurrentTime
		},NULL,
		NULL,NULL,&DisBackGround		
	},
	//State_UserPara_SetCurrentTime_Time,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		{
			State_UserPara_SetNewTime_Date,
			State_UserPara_SetNewTime_Week,
			State_UserPara_SetNewTime_Week,	
			State_UserPara_SetCurrentTime
		},NULL,
		NULL,NULL,&DisBackGround		
	},
	//State_UserPara_SetNewTime_Week,
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetFadeTimeStr,g_SetFadeTimeStrEn,
		{
			State_UserPara_SetNewTime_Time,
			State_UserPara_SetNewTime_Date,
			State_UserPara_SetCurrentTime,	
			State_UserPara_SetCurrentTime
		},NULL,
		NULL,NULL,&DisBackGround		
	},
  
	//State_UserPara_SetStaticIP
	{
		LCDADDR80,g_UserParaStr,g_UserParaStrEn,
		LCDADDR90,g_SetStaticIPStr,g_SetStaticIPStrEn,
		{
//			State_UserPara_SetHotBackup,
//			State_UserPara_SetHotBackup,

			State_UserPara_SetStaticIP,
//			State_AdvancePara_UserPara
		},NULL,
		NULL,NULL,&DisBackGround		
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

		
	//State_TechSuppor_ContactInfo,
	{
		LCDADDR80,g_TechSupport,g_TechSupportEn,
		LCDADDR90,g_ContactInfo,g_ContactInfoEn,
		{
			State_TechSuppor_VersionInfo,
			State_TechSuppor_VersionInfo,
			State_TechSuppor_ContactInfo,	
			State_AdvanceSet_TechSupport
		},NULL,
		NULL,NULL,&DisBackGround
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
			State_AdvanceSet_TechSupport
		},NULL,
		NULL,NULL,&DisBackGround
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
		NULL,&DisBackGround
	},

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
	//State_User_Set_Tech_TelNumber++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
	//State_User_Input_Logo+++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
	//State_User_Input_MathineType,+++++++++++++++++++++++++++++++++++++++++++++++++
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
//	State_UserTry_ShowTime,
	{
		0," "," ",
		0," "," ",
		{
			State_UserTry_ShowTime,
			State_UserTry_ShowTime,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
//	State_UserTry_OnceTime,
	{
		0," "," ",
		0," "," ",
		{
			State_UserTry_OnceTime,
			State_UserTry_OnceTime,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
//	State_UserTry_OnceTel,
	{
		0," "," ",
		0," "," ",
		{
			State_UserTry_OnceTel,
			State_UserTry_OnceTel,
			State_DebugMode_Start,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	} ,
//	State_UserTry_OnceEnd,
	{
		0," "," ",
		0," "," ",
		{
			State_UserTry_OnceEnd,
			State_UserTry_OnceEnd,
			State_UserTry_OnceEnd,	
			State_UserTry_OnceEnd
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
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
		NULL,NULL,&DisBackGround
	} ,
	
//	State_USerTry_Once_Password101,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password101,
			State_UserTry_Once_Password101,			
			State_UserTry_Once_SetEnd,
			State_DebugMode_Start,
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_USerTry_Once_Password102,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password102,
			State_UserTry_Once_Password102,
			State_UserTry_Once_SetEnd,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_USerTry_Once_Password103,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password103,
			State_UserTry_Once_Password103,		
			State_UserTry_OnceTel,
			State_DebugMode_Start	
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_USerTry_Once_Password104,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password104,
			State_UserTry_Once_Password104,
			State_UserTry_ShowTime,
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_USerTry_Once_Password105,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password105,
			State_UserTry_Once_Password105,
			State_UserTry_OnceTime,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
//	State_USerTry_Once_Password100,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100,
			State_UserTry_Once_Password100,
			State_UserTry_Once_Password100_new,	
			State_DebugMode_Start
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},

//	State_UserTry_Once_Password100_new,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100_new,
			State_UserTry_Once_Password100_new,
			State_UserTry_Once_Password100_new1,	
			State_UserTry_Once_Password100
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
	},
	//	State_UserTry_Once_Passwore100_new1,
	{
		0x80,NULL,NULL,
		0x90,NULL,NULL,
		{
			State_UserTry_Once_Password100_new1,
			State_UserTry_Once_Password100_new1,
			State_UserTry_Once_Password100_new1,	
			State_UserTry_Once_Password100_new
		},&DirectSetValue,
		NULL,NULL,&DisBackGround
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
		NULL,NULL,&DisBackGround
	},
};

/*******************************************************************************add********************/

void ScreenSet_Display( )
{
	u8 i=0;
	for(i=0;i<g_ColNum;i++)
	{	
		g_stitching_screen_line_column_pixel[0][i]=g_screen_line_column_pixel_input[0][i];
		g_screen_line_column_pixel[0][i]=g_screen_line_column_pixel_input[0][i];
	}
	for(i=0;i<g_LineNum;i++)
	{
		g_stitching_screen_line_column_pixel[1][i]=g_screen_line_column_pixel_input[1][i];
		g_screen_line_column_pixel[1][i]=g_screen_line_column_pixel_input[1][i];		
	}
	
	g_stitching_screen_column=g_ColNum;
	g_stitching_screen_line=g_LineNum;
	g_screen_line=g_stitching_screen_line;
	g_screen_column=g_stitching_screen_column;
	Write_EEPROM_Byte(E2ROM_SCREEN_LINE_ADDR,g_stitching_screen_line);	//
	Write_EEPROM_Byte(E2ROM_SCREEN_COL_ADDR,g_stitching_screen_column);	//
	Write_Screen_line_column_pixel();	
	g_screen_column=g_ColNum;
	g_screen_line=g_LineNum;
	Calculation_total_point();
}

void Matrix_Display_Function(void)
{
	u8 i=0;
	if(d_matrix_outputsource_num==0)  //所有的都输出
	{
		if(d_matrix_inputsource_num==0) //一对一匹配
		{
			for(i=0;i<4;i++)
			{
				if(input_source_resolution[i].resolution_width!=0)
				{	d_matrix_image_para[i][0]=i*1920;
					d_matrix_image_para[i][1]=0;
					d_matrix_image_para[i][2]=d_matrix_image_para[i][0]+1920;
					d_matrix_image_para[i][3]=1080;
					d_matrix_image_para[i][4]=1920;
					d_matrix_image_para[i][5]=1080;
					d_matrix_image_para[i][6]=i;
					d_matrix_image_para[i][7]=i+1;
					d_matrix_image_para[i][8]=0;
				}	
				else
				{
					d_matrix_image_para[i][7]=0;
				}
			}
		}
		else
		{
			for(i=0;i<4;i++)
			{
				if(input_source_resolution[i].resolution_width!=0)
				{
					d_matrix_image_para[i][0]=i*1920;
					d_matrix_image_para[i][1]=0;
					d_matrix_image_para[i][2]=d_matrix_image_para[i][0]+1920;
					d_matrix_image_para[i][3]=1080;
					d_matrix_image_para[i][4]=1920;
					d_matrix_image_para[i][5]=1080;
					d_matrix_image_para[i][6]=d_matrix_inputsource_num-1;
					d_matrix_image_para[i][7]=i+1;
					d_matrix_image_para[i][8]=0;		
				}	
				else
				{
					d_matrix_image_para[i][7]=0;
				}
			}	
		}
	}
	else
	{
		if(d_matrix_inputsource_num==0) //一对一匹配
		{
				if(input_source_resolution[d_matrix_outputsource_num-1].resolution_width!=0)
				{
					d_matrix_image_para[d_matrix_outputsource_num-1][0]=(d_matrix_outputsource_num-1)*1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][1]=0;
					d_matrix_image_para[d_matrix_outputsource_num-1][2]=d_matrix_image_para[d_matrix_outputsource_num-1][0]+1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][3]=1080;
					d_matrix_image_para[d_matrix_outputsource_num-1][4]=1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][5]=1080;
					d_matrix_image_para[d_matrix_outputsource_num-1][6]=d_matrix_outputsource_num-1;
					d_matrix_image_para[d_matrix_outputsource_num-1][7]=d_matrix_outputsource_num;
					d_matrix_image_para[d_matrix_outputsource_num-1][8]=0;	
				}	
				else
				{
					d_matrix_image_para[i][7]=0;
				}					
		}
		else
		{	
				if(input_source_resolution[d_matrix_inputsource_num-1].resolution_width!=0)
				{			
					d_matrix_image_para[d_matrix_outputsource_num-1][0]=(d_matrix_outputsource_num-1)*1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][1]=0;
					d_matrix_image_para[d_matrix_outputsource_num-1][2]=d_matrix_image_para[d_matrix_outputsource_num-1][0]+1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][3]=1080;
					d_matrix_image_para[d_matrix_outputsource_num-1][4]=1920;
					d_matrix_image_para[d_matrix_outputsource_num-1][5]=1080;
					d_matrix_image_para[d_matrix_outputsource_num-1][6]=d_matrix_inputsource_num-1;
					d_matrix_image_para[d_matrix_outputsource_num-1][7]=d_matrix_outputsource_num;
					d_matrix_image_para[d_matrix_outputsource_num-1][8]=0;		
				}	
				else
				{
					d_matrix_image_para[i][7]=0;
				}		
		}
	}	
	for(i=0;i<IMAGE_TOTAL_NUM;i++)
	{
		d_image_para[i][7]=0;
	}
	for(i=0;i<4;i++)
	{
		d_image_para[i][0]=d_matrix_image_para[i][0];
		d_image_para[i][1]=d_matrix_image_para[i][1];
		d_image_para[i][2]=d_matrix_image_para[i][2];
		d_image_para[i][3]=d_matrix_image_para[i][3];
		d_image_para[i][4]=d_matrix_image_para[i][4];	
		d_image_para[i][5]=d_matrix_image_para[i][5];
		d_image_para[i][6]=d_matrix_image_para[i][6];
		d_image_para[i][7]=d_matrix_image_para[i][7];
		d_image_para[i][8]=d_matrix_image_para[i][8];
	}
	if(matrix_stitching_flag== STITCHING_MODE)
	{
		for(i=0;i<4;i++)
		{
			g_screen_line_column_pixel[0][i]=1920;
			g_screen_line_column_pixel[1][i]=1080;
		}
		g_screen_line=1;
		g_screen_column=4;
		Calculation_total_point();
		matrix_stitching_flag=MATRIX_MODE;
	}
	image_layout();
}

void Add_New_Image()
{
	u8 i=0;
	//把新增的画面进行布局显示
	d_stitching_image_para[g_ImageNum_Set-1][0]=g_input_imagepara[0];
	d_stitching_image_para[g_ImageNum_Set-1][1]=g_input_imagepara[1];
	d_stitching_image_para[g_ImageNum_Set-1][2]=g_input_imagepara[2];
	d_stitching_image_para[g_ImageNum_Set-1][3]=g_input_imagepara[3];
	d_stitching_image_para[g_ImageNum_Set-1][4]=g_input_imagepara[2]-g_input_imagepara[0];
	d_stitching_image_para[g_ImageNum_Set-1][5]=g_input_imagepara[3]-g_input_imagepara[1];
	d_stitching_image_para[g_ImageNum_Set-1][6]=g_Image_Scr-1;
	d_stitching_image_para[g_ImageNum_Set-1][7]=g_ImageNum_Set;
	d_stitching_image_para[g_ImageNum_Set-1][8]=0;
	
	for(i=g_ImageNum_Set;i<IMAGE_TOTAL_NUM;i++)
	{
		d_stitching_image_para[i][7]=0;
	}
	
	for(i=0;i<IMAGE_TOTAL_NUM;i++)
	{
		d_image_para[i][0]=d_stitching_image_para[i][0];
		d_image_para[i][1]=d_stitching_image_para[i][1];
		d_image_para[i][2]=d_stitching_image_para[i][2];
		d_image_para[i][3]=d_stitching_image_para[i][3];
		d_image_para[i][4]=d_stitching_image_para[i][4];	
		d_image_para[i][5]=d_stitching_image_para[i][5];
		d_image_para[i][6]=d_stitching_image_para[i][6];
		d_image_para[i][7]=d_stitching_image_para[i][7];
		d_image_para[i][8]=d_stitching_image_para[i][8];
	}

	if(matrix_stitching_flag==MATRIX_MODE )
	{
		
		for(i=0;i<8;i++)
		{
			g_screen_line_column_pixel[0][i]=g_stitching_screen_line_column_pixel[0][i];
			g_screen_line_column_pixel[1][i]=g_stitching_screen_line_column_pixel[1][i];
		}	
		g_screen_line=g_stitching_screen_line;
		g_screen_column=g_stitching_screen_column;
		Calculation_total_point();
		matrix_stitching_flag=STITCHING_MODE;
	}
	image_layout();
}

void DelScheme_OK()
{
	d_stitching_image_para[g_SchemeNum-1][7]=0;
	d_image_para[g_SchemeNum-1][7]=0;
	scheme_save[g_SchemeNum-1]=0;
	Write_EEPROM_Byte(E2ROM_SCHEME_SAVE_ADDR+(g_SchemeNum-1)*2,scheme_save[g_SchemeNum-1]);
	Clear_Output(g_SchemeNum);
}


//保存方案
void Save_Scenario_OK()
{
	save_scenario_to_FPGA(g_SchemeNum);
	g_Current_Scene=g_SchemeNum;
	Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,g_Current_Scene);
	load_scenario_from_FPGA(g_Current_Scene);
}


void WizardSet_OK_Function(void)
{
	u8 i=0;

	char GpuBuf[100];
	ScreenSet_Display();
	g_input_imagepara[0]=0;;
	g_input_imagepara[1]=0;
	g_input_imagepara[2]=g_screen_total_pixel[0][g_screen_column-1];
	g_input_imagepara[3]=g_screen_total_pixel[1][g_screen_line-1];
	g_input_imagepara[6]=g_Image_Scr-1;
	g_ImageNum_Set=1;
	for(i=1;i<IMAGE_TOTAL_NUM;i++)
	{
		d_stitching_image_para[i][7]=0;
	}
	Add_New_Image();
	delay_ms(1000);
	sprintf(GpuBuf,"SBC(5);DS16(10,10,'向导1.5 保存方案   ',0);\r\n");
	GpuSend(GpuBuf);	
	save_scenario_to_FPGA(1);
	g_Current_Scene=1;
	Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,g_Current_Scene);
	load_scenario_from_FPGA(g_Current_Scene);
}


u8 Input_PassWord_Check(void)
{
	u8 i=0;
	if(g_PassWordNumVal==g_PassWordNumVal_input)
	{
		for(i=0;i<g_PassWordNumVal;i++)
		{
			if(g_UserTry_PassWord[i]!=g_UserTry_PassWord_input[i])
			{
				return 0;
			}
		}
		g_CurrentState=State_MainMenu_Not_Menu;
	}
	return 1;
}

void Send_LCD_BACK(u8 data)
{
		char GpuBuf[100];
		sprintf(GpuBuf,"SEBL(%d);\r\n",data);
		GpuSend(GpuBuf);	
}


#ifdef EC11_BUTTON
void Exit_EC11_Pin4_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD| RCC_APB2Periph_AFIO, ENABLE);			//使能按键所在GPIO时钟,同时使能AFIO时钟(需要设置外部中断寄存器)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = BMC_PIN;			//设置按键盘所在引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入   	
	GPIO_Init(BMC_GPIO, &GPIO_InitStructure);	//初始化按键所在GPIO寄存器状态

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);  	//选择按键所在的GPIO管脚用作外部中断线路
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //外部中断触发沿选择:设置输入线路下降沿为中断请求		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//先占优先级4位,共16级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道		
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}
#endif
/*******************************************************************************add********************/
