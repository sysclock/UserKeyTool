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
      U8 Buf[12];	  


void WizardSet()
{
//	g_CurrentState=State_NewMode_Set;
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

/*
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
*/
void DealMenuKey()
{
//	ClearScheWeekNum();	
	InputNumReset();
	clearlcd();
	DealCurrentState(g_CurrentState);	
}

//void DisMathineIDValue(U32 wValue,U8 addr)
//{
//		
//		U8 ch[7];	
//	ch[0]=wValue%1000000/100000+48;
//	ch[1]=wValue%100000/10000+48;
//	ch[2]=wValue%10000/1000+48;
//	ch[3]=wValue%1000/100+48;
//	ch[4]=wValue%100/10+48;
//	ch[5]=wValue%10+48;
//	ch[6]=0;	
//	chinesedisplay(addr, ch);
//}

/*************************************************
Function: 		版本号显示函数
Description: 	以 V1.0.0-150608的形式分别显示主板-控制板版本号
Calls:
Called By: 		
Input: 			显示起始地址
Output: 		无输出参数
Return:			无返回
Others:			李亚南 20150608 最后一次修改 
*************************************************/
void DisplaySoftRevision(U8 addr)
{
//	U8 ch[11];
//	ch[0]='V';
//	ch[1]=g_Fli32626HRevision%10+48;
//	ch[2]='.';
//	ch[3]=g_Fli32626MRevision%10+48;
//	ch[4]='.';
//	ch[5]=g_Fli32626LRevision%10+48;
//	ch[6]='-';
//	ch[7]='\0';
//	wcom(addr);
//	printStr(ch);
//	printStr((U8*)g_ReleaseStr);
}

//void DealMainInfoState()
//{	
//	switch(g_CurrentState)
//	{
//		case State_MainMenu_Menu_VersionInfo:

//			Color_VersionInfo();	
//			break;
//		case State_MainMenu_Menu_OperationGuide://0x84
//			DisMathineIDValue(ChipUniqueID,0x84);
//			if(g_DeviceID!=0)
//			{
//				DisCurWinSizeValue(g_DeviceID,0x86,'-');
//			}
// 			Color_OperationGuide();
//			
//			break;
//		case State_MainMenu_Menu_OperationGuide1://0x84
//			DisMathineIDValue(ChipUniqueID,0x84);
//			if(g_DeviceID!=0)
//			{
//				DisCurWinSizeValue(g_DeviceID,0x86,'-');
//			}
// 			Color_OperationGuide1();
//			
//			break;

//	}
//}



void DealUpKey()
{
	
	if(g_CurrentState==State_ScenarioSet_NewImage_ImagePara_Source ||g_CurrentState==State_WizardSet_InputSource_Set)
	{
		if(g_Image_Scr>1)g_Image_Scr--;
		else g_Image_Scr=4;
		LcdDisp_Follow_Keyboard(g_CurrentState);
	}
	else if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_UP])
	{
		g_CurrentState=stateData[g_CurrentState].navigationData[NAV_UP];
		DealCurrentState(g_CurrentState); 
	}			
}

void DealDownKey()
{
	if(g_CurrentState==State_ScenarioSet_NewImage_ImagePara_Source ||g_CurrentState==State_WizardSet_InputSource_Set)
	{
		if(g_Image_Scr<4)g_Image_Scr++;
		else g_Image_Scr=1;
		LcdDisp_Follow_Keyboard(g_CurrentState);
	}
	if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_DOWN])
	{
		g_CurrentState=stateData[g_CurrentState].navigationData[NAV_DOWN];
		DealCurrentState(g_CurrentState);
	}
//	DealMainInfoState();
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
		//如果在设置页面点OK,进行保存
		case State_UserPara_SetNewTime_Date:
		case State_UserPara_SetNewTime_Time:
		case State_UserPara_SetNewTime_Week:
				Set_Current_Time();
		DealEscKey();
			return;

	}

	switch(g_CurrentState)
	{	
		case State_ScenarioSet_NewImage_ImageNum:
		case State_ScenarioSet_DelScheme_Num:
		case State_ScenarioSet_SchemeNum:
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		case State_DebugMode_Start:
		case State_UserTrySet_Day:
		case State_UserTrySet_PassWord:
		case State_TryTime_End:
		case State_UserTrySet_Tel:
		case State_UserTrySet_End:
		case State_FactoryReset_OK:
		case State_WizardSet_OutPort_Num:
		case State_ScreenSet_OutPort_Num:
		case State_WizardSet_TwoPort_Mode:
		case State_WizardSet_ThreePort_Mode:
		case State_WizardSet_FourPort_Mode:	
		case State_ScreenSet_TwoPort_Mode:
		case State_ScreenSet_ThreePort_Mode:
		case State_ScreenSet_FourPort_Mode:			
		case State_MatrixMode_ModeSet_input:
		case State_MatrixMode_ModeSet_output:
		case State_MainMenu_Menu_ScreenSet_Password:
		case State_MainMenu_Menu_ScenarioSet_Password:
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0)) //OK-key, Current-State,0
					return;
		break;
				
		case State_ScenarioSet_NewImage_ImagePara_Save:  //做保存退出，要执行一个外部程序，然后切换状态
		case State_ScenarioSet_DelScheme_Num_OK:
		case State_ScreenSet_Display:
		case State_WizardSet_InputSource_Set:
		case State_ScenarioSet_NewImage_ImagePara_Source:
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0);
			break;

	///当锁屏时间改变后，进行更新，并可以实时显示////	
		case  State_LockPadTime_Set :
				deal_lockpadtime();	
		
		/////////////////////////////////////////
		//20170308 lyg
		case 	State_UserTry_Once_Password101:
		case State_UserTry_Once_Password102:		
		case State_UserTry_Once_Password100_new1:
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0)) //OK-key, Current-State,0
				{
					return;
				}
				else
				{
					g_CurrentState= State_UserTry_Once_SetEnd;
					return;
				}
			break;
		
		case State_UserTry_ShowTime:
		case State_UserTry_OnceTime:
		case State_UserTry_OnceTel:		
		case State_UserTry_Once_Password103:
		case State_UserTry_Once_Password104:
		case State_UserTry_Once_Password105:	
		case State_UserTry_Once_Password100:		
		case State_UserTry_Once_Password100_new:	
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0)) //OK-key, Current-State,0
					return;
			break;
		case State_UserTry_OnceEnd:		
			if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0)) //OK-key, Current-State,0
				{	
					return;
				}
				else  //执行成功
				{
					DealCurrentState(g_CurrentState);
//					DealMainInfoState();
					return;
				}			
			break;
		////////////////////////////////////////
			break;
		default:
			break;
			
		//----------------------end--------------------
	}
	if(IsSetParaValueState())
	{
		if(stateData[g_CurrentState].ExecFunction!=NULL)
				if(!stateData[g_CurrentState].ExecFunction(KEY_ENTER,g_CurrentState,0)) //OK-key, Current-State,0
					return;
	}
	if(g_CurrentState!=stateData[g_CurrentState].navigationData[NAV_OK])
	{
		if(g_CurrentState==State_MainMenu_Menu_ScreenSet&& g_Set_First_Flag  )
		{
			g_CurrentState=State_MainMenu_Menu_ScreenSet_Password;
		}	
		else if(g_CurrentState==State_MainMenu_Menu_ScenarioSet&& g_Set_First_Flag)
		{
			g_CurrentState=State_MainMenu_Menu_ScenarioSet_Password;
		}
		else if(g_CurrentState==State_WizardSet_OutPort_Num)
		{
			if(g_OutPort_Num==2)
				g_CurrentState=State_WizardSet_TwoPort_Mode;
			else if(g_OutPort_Num==3)
				g_CurrentState=State_WizardSet_ThreePort_Mode;
			else if(g_OutPort_Num==4)
				g_CurrentState=State_WizardSet_FourPort_Mode;
			else
			{				
				g_LineNum=1;
				g_ColNum=1;
				g_LineNum_Set=1;
				g_ColNum_Set=0;
				g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
			}
		}
		else if(g_CurrentState==State_ScreenSet_OutPort_Num)
		{
			if(g_OutPort_Num==2)
				g_CurrentState=State_ScreenSet_TwoPort_Mode;
			else if(g_OutPort_Num==3)
				g_CurrentState=State_ScreenSet_ThreePort_Mode;
			else if(g_OutPort_Num==4)
				g_CurrentState=State_ScreenSet_FourPort_Mode;
			else 
			{
				g_LineNum=1;
				g_ColNum=1;
				g_LineNum_Set=1;
				g_ColNum_Set=0;
				g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
			}
		}			
		else if(g_CurrentState ==State_WizardSet_TwoPort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_WizardSet_TwoPort_layout1;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_WizardSet_TwoPort_layout2;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState ==State_WizardSet_ThreePort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_WizardSet_ThreePort_layout1;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_WizardSet_ThreePort_layout2;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState ==State_WizardSet_FourPort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_WizardSet_FourPort_layout1;
				g_LineNum_Set++;
			}
			else if(g_LayoutNum==2)
			{
				g_CurrentState=State_WizardSet_FourPort_layout2;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_WizardSet_FourPort_layout3;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState ==State_ScreenSet_TwoPort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_ScreenSet_TwoPort_layout1;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_ScreenSet_TwoPort_layout2;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState ==State_ScreenSet_ThreePort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_ScreenSet_ThreePort_layout1;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_ScreenSet_ThreePort_layout2;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState ==State_ScreenSet_FourPort_Mode)
		{
			if(g_LayoutNum==1)
			{
				g_CurrentState=State_ScreenSet_FourPort_layout1;
				g_LineNum_Set++;
			}
			else if(g_LayoutNum==2)
			{
				g_CurrentState=State_ScreenSet_FourPort_layout2;
				g_LineNum_Set++;
			}
			else
			{
				g_CurrentState=State_ScreenSet_FourPort_layout3;
				g_LineNum_Set++;
			}
		}
		else if(g_CurrentState==State_ScenarioSet_NewImage_OK_NOT)
		{
			if(g_ImageNum_Set<g_ImageNum)
			{
				g_ImageNum_Set++;
				g_CurrentState=State_ScenarioSet_NewImage_ImagePara_Begin_X;
			}
			else
			{
				Save_Scenario_OK();  //输入完最后一个画面参数后，进行方案的保存
				scheme_save[g_SchemeNum-1]=1;
				Write_EEPROM_Byte(E2ROM_SCHEME_SAVE_ADDR+(g_SchemeNum-1)*2,scheme_save[g_SchemeNum-1]);
				g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
			}
		}
		
		else if(IsSetParaValueState()) //如果为多画面状态跳转情况，根据实际跳转状态执行
		{
				if(g_LineNum_Set<g_LineNum)
				{
					g_LineNum_Set++;
				}
				else if(g_ColNum_Set<g_ColNum)
				{
					g_LineNum_Set++;
					g_ColNum_Set++;
				}
				else
				{
					g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
				}
		}	
		else if(g_CurrentState==State_ScenarioSet_SchemeNum && scheme_save[g_SchemeNum-1]!=0)
		{
			g_CurrentState=State_ScenarioSet_SchemeSaved;
		}
		else
		{
			g_CurrentState=stateData[g_CurrentState].navigationData[NAV_OK];
		}		
		IsExecuteFunc=1;
	}
	
	if(g_CurrentState==State_MainMenu_Menu&&!g_TryTimeEndFlag)
	{
		DealMenuKey();
		return;
	}

	DealCurrentState(g_CurrentState);
}

void DealEscKey()
{

	if(Is_Input_Num())
	{
			InputNumReset();
			switch(g_CurrentState)  //实现了取消当前输入的参数，把当前区域清空，后期可以把所有的输入清空进行整理，加到此处
			{
					case State_MainMenu_Menu_ScreenSet_Password:
					case State_MainMenu_Menu_ScenarioSet_Password:
						GpuSend("CBOF(145,80,185,100,3,15);CBOX(145,80,185,100,3,0);\r\n");
						break;
					case State_MatrixMode_ModeSet_input:		
								GpuSend("CBOF(66,66,94,89,3,15);\r\n");
						break;
					case State_MatrixMode_ModeSet_output:	
							GpuSend("CBOF(126,66,154,89,3,15);\r\n");
						break;
					case State_ScreenSet_OutPort_Num:
					case State_WizardSet_OutPort_Num:
								GpuSend("CBOF(146,69,184,87,3,15);\r\n");
						break;
					case State_ScreenSet_TwoPort_Mode:
					case State_WizardSet_TwoPort_Mode:
								GpuSend("CBOF(171,146,194,169,1,15);\r\n");
						break;
					case State_ScreenSet_ThreePort_Mode:
					case State_WizardSet_ThreePort_Mode:
								GpuSend("CBOF(171,146,194,169,1,15);\r\n");
						break;
					case State_ScreenSet_FourPort_Mode:
					case State_WizardSet_FourPort_Mode:
								GpuSend("CBOF(171,146,194,169,1,15);\r\n");
						break;
								
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
						if(g_LineNum_Set<=g_LineNum)
						{
								GpuSend("CBOF(131,126,204,149,1,15);\r\n");
						}
						else
						{
								GpuSend("CBOF(131,126,204,149,1,15);\r\n");
						}
						break;
					case State_ScenarioSet_NewImage_ImageNum:
							GpuSend("CBOF(146,81,184,99,3,15);\r\n");	
						break;
					case State_ScenarioSet_DelScheme_Num:
					case State_ScenarioSet_SchemeNum:
							GpuSend("CBOF(131,81,159,99,3,15);\r\n");	
						break;
					case State_ScenarioSet_NewImage_ImagePara_Begin_X:
							GpuSend("SBC(7);CBOF(68,63,106,82,1,7);\r\n");
							break;
					case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
							GpuSend("SBC(7);CBOF(170,63,208,82,1,7);\r\n");
						break;
					case State_ScenarioSet_NewImage_ImagePara_End_X:
							GpuSend("SBC(7);CBOF(68,93,106,112,1,7);\r\n");
							break;
					case State_ScenarioSet_NewImage_ImagePara_End_Y:
						GpuSend("SBC(7);CBOF(170,93,208,102,1,7);\r\n");
						break;	
					
					
					case 	State_UserTry_Once_Password101:
					case State_UserTry_Once_Password102:
					case State_UserTry_Once_Password103:
					case State_UserTry_Once_Password104:
					case State_UserTry_Once_Password105:	
					case State_UserTry_Once_Password100:		
						GpuSend("CBOF(125,80,200,100,3,15);CBOX(125,80,200,100,3,0);\r\n");
						break;
					case State_UserTry_Once_Password100_new:
						GpuSend("CBOF(125,62,200,82,3,7);\r\n");
						break;
					case State_UserTry_Once_Password100_new1:
						GpuSend("CBOF(125,97,200,117,3,7);\r\n");
						break;
					
					case State_DebugMode_Start:
						GpuSend("CBOF(130,79,200,99,3,7);\r\n");
						break;
					case State_UserTry_OnceEnd:
						GpuSend("CBOF(105,70,165,86,3,7);\r\n");
						break;
					default:
						break;
			}		
			DealCurrentState(g_CurrentState);			
	}
	else
	{
		InputNumReset();
		if(g_CurrentState== State_AdvanceSet_CardTest_Port ||g_CurrentState==State_AdvanceSet_CardTest_Color)
		{	
			g_CardTest_Color=4;
			OutPort_TestPatternColor();	
			g_CardTest_Color=0;			
		}
		
		g_CurrentState=stateData[g_CurrentState].navigationData[NAV_ESC];
		DealCurrentState(g_CurrentState);

//		DealMainInfoState();
		Num_Clear();
	}		
}


void DealRecallKey()
{
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80, g_NotSup);
		chinesedisplay(0x90, "                    ");
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80, g_NotSupEn);
		chinesedisplay(0x90, "                    ");
	}
	delay_ms(1000);
	//g_CurrentState=State_MainMenu_Not_Menu;
	DealEscKey();

}


void DealSaveKey()
{					
	clearlcd();
	if(g_LanguageFlag==CHINESE)
	{
		chinesedisplay(0x80, g_NotSup);
		chinesedisplay(0x90, "                    ");
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		chinesedisplay(0x80, g_NotSupEn);
		chinesedisplay(0x90, "                    ");
	}
	delay_ms(1000);
	g_CurrentState=State_MainMenu_Not_Menu;
	DealEscKey();
}

void DealBackspaceKey(void)
{
	if(g_CurrentState==State_LockPadTime_Set||IsSetParaValueState())	
	{

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
			case State_User_Change_Manufacturer_Logo:
				BackSpaceNumber(LCDADDR91,5);
				break;
			case State_User_Set_TelNumber:
				BackSpaceNumber(LCDADDR91,11);
				break;	
			case State_User_Change_MachineModel:
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
			default:
				//DealActiveImageKey();
				break;
			
		}
	}		
		
}





