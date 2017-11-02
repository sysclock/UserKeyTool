/*************************************************  
Copyright (C), 2013-2023, Bycox Tech. Co., Ltd.  
File name:
Author:  LYG  setup~      
Version:        
Date: 20170322 Setup  

Description:    
	// 用于详细说明此程序文件完成的主要功能，与其他模块                  
	// 或函数的接口，输出值、取值范围、含义及参数间的控                  
	// 制、顺序、独立或依赖等关系  Others:         
	// 其它内容的说明  

Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明    
	1,set display para about processor output image
	2,

History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改 者及修改内容简述      
	1,//===2017,Jun 16 ; 8:52:20  ;BY YANG@BYCOX.COM===  
		support from 4*4-->8*8
	2. ...

*************************************************/

#include "header.h"

extern SetTimeFlag g_SetTimeFlag;
extern U8 g_SetInputScheFlag;
extern U8 InputSrcName[6];
extern struct rtc_time systmtime;

//====the locaiton of lcd display char 
const u8 LCD_DISP_RES_COL = 53;
const u8 LCD_DISP_RES_NAME = 6;

/***********************************
//int Date_Time[8]说明  
//用来存储设置时间时的临时日期时间数据  
//Data_Time[0]~[6] 年月日时分秒星期  
//Data_time[7]用来进行设置时，如年月日
八位数据进行标识设置的是其中的哪个位
************************************************/
int Date_Time[8]={0};   
//串口屏显示函数
extern char GpuBuf[500];


//SPG(n) :display batch graph of n 显示第n个批处理界面	
void LcdDisp_BackGround_Graph(State CurState)/////////////////+++++++++++++++++++++++++++++++++++++++
{
	u32 total=0;
	u8 i=0;
	u8 BackClolor[IN_PORT_NUM]={LCD_COLOR_BLACK};
// 	U8 InputSrcName[6];
	char GpuBuf[500];
//	U16 wValue=0;
//	U16 HSize=0,VSize=0;
//	U16 Temp16;
	switch(CurState)
	{
		
		case State_MainMenu_Menu_ScreenSet_Password:
		case State_MainMenu_Menu_ScenarioSet_Password:
				if(LCD_UI_NO !=61)
				{							
					GpuSend("SPG(61);\r");	
					delay_ms(LCD_COMM_DELAY_MS);
				}
			LCD_UI_NO = 61;
			break;		
		case State_MainMenu_Menu_ScreenSet:
		case State_MainMenu_Menu_ScenarioSet:		
		case State_MainMenu_Menu_WizardSet:
		case State_MainMenu_Menu_AdvanceSet:
		case State_MainMenu_Menu_MatrixMode:
			if(LCD_UI_NO != 5)
				{	//主菜单
					GpuSend("SPG(5);\r");	
					delay_ms(LCD_COMM_DELAY_MS);								
					GpuSend("SPG(5);\r");	
					delay_ms(LCD_COMM_DELAY_MS);
				}
			LCD_UI_NO = 5;
			break;
		case State_MatrixMode_ModeSet_input:
		case State_MatrixMode_ModeSet_output:
			if(LCD_UI_NO != 58)
				{						
					GpuSend("SPG(58);\r");	
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO =58;
				}
			break;
		case State_WizardSet_Welcome:
			if(LCD_UI_NO != 24)
				{			
					GpuSend("SPG(24);\r");	
					delay_ms(LCD_COMM_DELAY_MS);
				}
			LCD_UI_NO = 24;
			break;
		case State_ScreenSet_Display:
			if(LCD_UI_NO != 15)
			{
				GpuSend("SPG(15);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 15;
			break;		
		case State_ScenarioSet_NewScheme:			
		case State_ScenarioSet_DelScheme:
			if(LCD_UI_NO != 9)
			{
				GpuSend("SPG(9);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 9;
			break;	
		case State_ScenarioSet_NewImage_ImageNum:
			if(LCD_UI_NO != 10)
			{
				GpuSend("SPG(10);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);	
			}
			LCD_UI_NO = 10;
			break;		
		case State_ScenarioSet_SchemeSaved:
			if(LCD_UI_NO != 59)
			{
				GpuSend("SPG(59);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 59;
			break;	
	
		case State_ScenarioSet_DelScheme_Num_OK:
			if(LCD_UI_NO != 14)
			{
				GpuSend("SPG(14);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);	
			}
			LCD_UI_NO = 14;
			break;		
		case State_ScenarioSet_DelScheme_Num:
			if(LCD_UI_NO != 11)
			{
				GpuSend("SPG(11);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);	
			}
			LCD_UI_NO = 11;
			break;	
		case	State_ScenarioSet_SchemeNum:
			if(LCD_UI_NO != 18)
			{
				GpuSend("SPG(18);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);	
			}
			LCD_UI_NO = 18;
			break;	
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		case State_ScenarioSet_NewImage_ImagePara_Save:	
		{	
			if(LCD_UI_NO != 13)
			{
				GpuSend("SPG(13);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
				
				if(g_ImageNum_Set==1 &&g_ImageNum==1)
				{
					gInImgPara[0]=0;
					gInImgPara[1]=0;
					gInImgPara[2]=gScrTotX;
					gInImgPara[3]=gScrTotY;
					GpuSend("SBC(7);");	
					sprintf(GpuBuf,"DS16(68,63,'  %d',0);",gInImgPara[0]);
					GpuSend(GpuBuf);
					GpuSend("SBC(7);");	
					sprintf(GpuBuf,"DS16(170,63,' %d',0);",gInImgPara[1]);
					GpuSend(GpuBuf);
					GpuSend("SBC(7);");	
					sprintf(GpuBuf,"DS16(68,93,'%d',0);",gScrTotX);
					GpuSend(GpuBuf);
					GpuSend("SBC(7);");	
					sprintf(GpuBuf,"DS16(170,93,'%d',0);\r\n",gScrTotY);
					GpuSend(GpuBuf);
					delay_ms(10);
				}
			}
			LCD_UI_NO = 13;
			break;
		}
		
		case State_ScenarioSet_NewImage_OK_NOT:
		{	
			if(LCD_UI_NO != 8)
			{
				GpuSend("SPG(8);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 8;
			break;
		}
		case State_ScenarioSet_NewImage_Continue:
		{	
			if(LCD_UI_NO != 60)
			{
				GpuSend("SPG(60);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 60;
			break;
		}	
		case State_ScreenSet_OutPort_Num:
			if(LCD_UI_NO!=45)
				{
					GpuSend("SPG(45);\r\n");				
					LCD_UI_NO=45;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_OutPort_Num:
			if(LCD_UI_NO!=28)
				{
					GpuSend("SPG(28);\r\n");				
					LCD_UI_NO=28;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_TwoPort_Mode:
			if(LCD_UI_NO!=49)
				{
					GpuSend("SPG(49);\r\n");				
					LCD_UI_NO=49;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_TwoPort_Mode:
			if(LCD_UI_NO!=36)
				{
					GpuSend("SPG(36);\r\n");				
					LCD_UI_NO=36;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_OnePort_layout1:
			if(LCD_UI_NO!=57)
				{
					GpuSend("SPG(57);\r\n");				
					LCD_UI_NO=57;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_OnePort_layout1:
			if(LCD_UI_NO!=44)
				{
					GpuSend("SPG(44);\r\n");				
					LCD_UI_NO=44;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_TwoPort_layout1:
			if(LCD_UI_NO!=50)
				{
					GpuSend("SPG(50);\r\n");				
					LCD_UI_NO=50;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_TwoPort_layout1:
			if(LCD_UI_NO!=37)
				{
					GpuSend("SPG(37);\r\n");				
					LCD_UI_NO=37;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_TwoPort_layout2:
			if(LCD_UI_NO!=51)
				{
					GpuSend("SPG(51);\r\n");				
					LCD_UI_NO=51;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_TwoPort_layout2:
			if(LCD_UI_NO!=38)
				{
					GpuSend("SPG(38);\r\n");				
					LCD_UI_NO=38;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_ThreePort_Mode:	
			if(LCD_UI_NO!=48)
				{
					GpuSend("SPG(48);\r\n");				
					LCD_UI_NO=48;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_ThreePort_Mode:
			if(LCD_UI_NO!=35)
				{
					GpuSend("SPG(35);\r\n");				
					LCD_UI_NO=35;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_ThreePort_layout1:	
			if(LCD_UI_NO!=52)
				{
					GpuSend("SPG(52);\r\n");				
					LCD_UI_NO=52;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_ThreePort_layout1:
			if(LCD_UI_NO!=39)
				{
					GpuSend("SPG(39);\r\n");				
					LCD_UI_NO=39;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_ThreePort_layout2:
			if(LCD_UI_NO!=53)
				{
					GpuSend("SPG(53);\r\n");				
					LCD_UI_NO=53;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_ThreePort_layout2:
			if(LCD_UI_NO!=40)
				{
					GpuSend("SPG(40);\r\n");				
					LCD_UI_NO=40;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_FourPort_Mode:	
			if(LCD_UI_NO!=47)
				{
					GpuSend("SPG(47);\r\n");				
					LCD_UI_NO=47;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_FourPort_Mode:
			if(LCD_UI_NO!=34)
				{
					GpuSend("SPG(34);\r\n");				
					LCD_UI_NO=34;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_FourPort_layout1:
			if(LCD_UI_NO!=54)
				{
					GpuSend("SPG(54);\r\n");				
					LCD_UI_NO=54;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_FourPort_layout1:
			if(LCD_UI_NO!=41)
				{
					GpuSend("SPG(41);\r\n");				
					LCD_UI_NO=41;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_FourPort_layout2:		
			if(LCD_UI_NO!=55)
				{
					GpuSend("SPG(55);\r\n");				
					LCD_UI_NO=55;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_FourPort_layout2:
			if(LCD_UI_NO!=42)
				{
					GpuSend("SPG(42);\r\n");				
					LCD_UI_NO=42;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_ScreenSet_FourPort_layout3:
			if(LCD_UI_NO!=56)
				{
					GpuSend("SPG(56);\r\n");				
					LCD_UI_NO=56;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;
		case State_WizardSet_FourPort_layout3:
			if(LCD_UI_NO!=43)
				{
					GpuSend("SPG(43);\r\n");				
					LCD_UI_NO=43;
					delay_ms(LCD_COMM_DELAY_MS);
				}
			break;		
		
		case State_ScenarioSet_NewImage_ImagePara_Source:				
		case State_WizardSet_InputSource_Set:
			if(LCD_UI_NO!=30)
				{
					GpuSend("SPG(30);\r\n");				
					LCD_UI_NO=30;
					READ_INPUT_SOURCE_RESOLUTION();
				}
			break;
		case State_AdvanceSet_CardTestSet:
		case State_AdvanceSet_DebugModeSet:
		case State_AdvanceSet_TechSupport:
		case State_AdvanceSet_HelpInfo:
		case State_AdvancePara_FactoryReset:
		case State_UserPara_SetCurrentTime:
		if(LCD_UI_NO != 17)
			{
				GpuSend("SPG(17);\r\n");				
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO = 17;
			break;
		case State_AdvanceSet_CardTest_Port:
		case State_AdvanceSet_CardTest_Color:
			if(LCD_UI_NO!=19)
			{
				GpuSend("SPG(19);\r\n");
				delay_ms(LCD_COMM_DELAY_MS);
			}
			LCD_UI_NO=19;
			break;		
		case State_DebugMode_Start:
			if(LCD_UI_NO != 20)
				{

					GpuSend("SPG(20);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 20;
				}
			
			break;	
		case State_UserTrySet_Start:
		case State_UserTrySet_End:
		{
			if(LCD_UI_NO!=21)
			{
				GpuSend("SPG(21);\r\n");					
				delay_ms(LCD_COMM_DELAY_MS);
				LCD_UI_NO = 21;
			}
			break;
		}
		case State_UserTrySet_Day:
		case State_UserTrySet_PassWord:
		case State_UserTrySet_Tel:
		{
			if(LCD_UI_NO!=22)
			{
				GpuSend("SPG(22);\r\n");					
				delay_ms(LCD_COMM_DELAY_MS);
				LCD_UI_NO = 22;
			}
			break;
		}
		case State_TryTime_End:
		{
			if(LCD_UI_NO!=23)
			{
				GpuSend("SPG(23);\r\n");					
				delay_ms(LCD_COMM_DELAY_MS);
				GpuSend("SBC(7);");	
				for(i=0;i<g_TeleNumVal;i++)
				{	
					sprintf(GpuBuf,"DS16(%d,86,'%c',0);",90+i*8,g_UserTry_Tel[i]);
					GpuSend(GpuBuf);
				}
				
				GpuSend("\r\n");	
			LCD_UI_NO = 23;
			}
			GpuSend("CBOF(120,111,185,126,0,7);\r\n");	
			break;
		}
		case State_LockPadTime_Set:
					GpuSend("SPG(26);\r\n");
					delay_ms(LCD_COMM_DELAY_MS);
			LCD_UI_NO = 26;
			sprintf(GpuBuf,"SBC(7);DS12(123,78,'%d',0);\r\n",g_LockKeypadTime);
			GpuSend(GpuBuf);
			
		break;

	
		case State_FactoryReset_Yes:
		case State_FactoryReset_OK:
		{
			if(LCD_UI_NO != 27)
				{
					GpuSend("SPG(27);\r");
					delay_ms(LCD_COMM_DELAY_MS);
				}
			LCD_UI_NO = 27;


			if(g_CurrentState==State_FactoryReset_Yes)

				{
		
					GpuSend("SBC(7);CBOF(46,71,174,85,1,7);DS12(86,72,'是否继续?',0);\r\n");

				}

			else if(g_CurrentState==State_FactoryReset_OK)
			
				{			
			
					GpuSend("SBC(4);DS12(47,72,'该操作将清除配置数据!',0);\r\n");
				}
				delay_ms(LCD_COMM_DELAY_MS);
		
			break;
		}
		case State_UserPara_SetNewTime_Date:
		{	
			if(LCD_UI_NO != 32)
				{
					Date_Time[0]=systmtime.tm_year;				
					Date_Time[1]=systmtime.tm_mon;
					Date_Time[2]=systmtime.tm_mday;
					Date_Time[3]=systmtime.tm_wday ;
					Date_Time[4]=systmtime.tm_hour ;
					Date_Time[5]=systmtime.tm_min ;
					Date_Time[6]=systmtime.tm_sec ;
					Date_Time[7]=0;
					GpuSend("SPG(32);\r");		

					GpuSend("SBC(5);");		
					sprintf(GpuBuf,"DS16(90,93,'%02d:%02d:%02d',0);",Date_Time[4],Date_Time[5],Date_Time[6]);
					GpuSend(GpuBuf);
					delay_ms(LCD_COMM_DELAY_MS);
					
// 					if(g_LanguageFlag==CHINESE)
// 					{
						switch(Date_Time[3])
						{
							case 0:
								GpuSend("DS16(90,125,'周日',0);");
								break;
							case 1:
								GpuSend("DS16(90,125,'周一',0);");
								break;
							case 2:
								GpuSend("DS16(90,125,'周二',0);");
								break;
							case 3:
								GpuSend("DS16(90,125,'周三',0);");
								break;
							case 4:
								GpuSend("DS16(90,125,'周四',0);");
								break;
							case 5:
								GpuSend("DS16(90,125,'周五',0);");
								break;
							case 6:
								GpuSend("DS16(90,125,'周六',0);");
								break;
						}
					sprintf(GpuBuf,"DS16(90,62,'%04d-%02d-%02d',0);",Date_Time[0],Date_Time[1],Date_Time[2]);
					GpuSend(GpuBuf);
					GpuSend("SBC(7);\r\n");		
					//delay_ms(20);	

					
					delay_ms(LCD_COMM_DELAY_MS);

				}
			LCD_UI_NO = 32;
			break;
			}
	case State_MainMenu_Menu:
			if(LCD_UI_NO!=4)
			{
					GpuSend("SPG(4);\r");
					delay_ms(LCD_COMM_DELAY_MS);
					GpuSend("SPG(4);\r");
					delay_ms(LCD_COMM_DELAY_MS);
					GpuSend("SPG(4);\r");
					delay_ms(LCD_COMM_DELAY_MS);
					GpuSend("SPG(4);\r");
					delay_ms(LCD_COMM_DELAY_MS);
				
			}				
			if(g_LockKeyFlag==LOCKKEYPAD)
			{
				GpuSend("CBOF(180,9,220,43,1,5);SBC(5);DS16(184,18,'已锁',0);SBC(7);\r\n");
				delay_ms(10);				

			}
			LCD_UI_NO=4;
			break;

		case State_Lock_Key:

			if(LCD_UI_NO != 2)
				{
					GpuSend("SPG(2);\r");					
					delay_ms(LCD_COMM_DELAY_MS);
				}
			LCD_UI_NO = 2;

			if(g_LockKeyFlag==LOCKKEYPAD)
			{GpuSend("DS16(78,79,'键盘已锁',0);\r\n");	
			}
			else if(g_LockKeyFlag==UNLOCKKEYPAD)
			{
			GpuSend("DS16(78,79,'解锁成功',0);\r\n");	
			}
			break;
			
	
		case State_HelpInfo_Wizard:
					GpuSend("SPG(102);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO =102;
			break;
		case State_HelpInfo_Screen:
				GpuSend("SPG(103);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 103;
			break;
		case State_HelpInfo_Scenario:
					GpuSend("SPG(104);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 104;
			break;
		case State_HelpInfo_Matrix:
					GpuSend("SPG(105);\r\n");				
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 105;		
			break;
		case State_HelpInfo_TechSupport:
					if(gManufactoryID==1)
						GpuSend("SPG(106);\r\n");		//中性的机器			
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 106;
			break;
		
		case State_MainMenu_Menu_IPInfo:
					GpuSend("SPG(107);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 107;
					Read_TCPIP();
			break;
		case State_HelpInfo_IP:
					GpuSend("SPG(108);\r\n");					
					delay_ms(LCD_COMM_DELAY_MS);
					LCD_UI_NO = 108;
					Read_TCPIP();
			break;
		
		case State_UserTry_ShowTime:
			GpuSend("SPG(62);\r\n");					
			delay_ms(LCD_COMM_DELAY_MS);
			LCD_UI_NO = 62;
			break;
		case State_UserTry_OnceTime:
			GpuSend("SPG(63);\r\n");					
			delay_ms(LCD_COMM_DELAY_MS);
			LCD_UI_NO = 63;
			break;	
		case State_UserTry_OnceTel:
			GpuSend("SPG(64);\r\n");					
			delay_ms(LCD_COMM_DELAY_MS);
			LCD_UI_NO = 64;	
			break;
		case State_UserTry_OnceEnd:
			if(LCD_UI_NO!=65)
			{
				TryEnd_Clear_Display();
				GpuSend("SPG(65);\r\n");			
				delay_ms(LCD_COMM_DELAY_MS);
				for(i=0;i<g_UserTry_once_TeleNumVal;i++)
				{
					sprintf(GpuBuf,"DS16(%d,91,'%c',0);\r\n",100+i*8,g_UserTry_once_Tel[i]);
					GpuSend(GpuBuf);
				}
				GpuSend("\r\n");	
				Date_Time[0]=systmtime.tm_year;				
				Date_Time[1]=systmtime.tm_mon;
				Date_Time[2]=systmtime.tm_mday;
				Date_Time[3]=systmtime.tm_hour;
				sprintf(GpuBuf,"DS16(100,111,'%04d-%02d-%02d:%02d',0);",Date_Time[0],Date_Time[1],Date_Time[2],Date_Time[3]);
					GpuSend(GpuBuf);		
				sprintf(GpuBuf,"DS16(100,131,'%4d',0);\r\n",ChipUniqueID%10000);
					GpuSend(GpuBuf);	
				delay_ms(LCD_COMM_DELAY_MS);					
				LCD_UI_NO = 65;	
			}
			break;
		//20170213 lyg
			//cmd 5x
			case State_UserTry_Once_Password100:
			case State_UserTry_Once_Password101://设置单次试用启停
			case State_UserTry_Once_Password102:
			case State_UserTry_Once_Password103:
			case State_UserTry_Once_Password104:
			case State_UserTry_Once_Password105:

			//cmd 7x change manufacturer information
			//case State_User_Change_Manufacturer_Logo:	
			case State_User_Set_TechSupport_PhoneNumber:
			case State_User_Change_MachineModel:	
				if(LCD_UI_NO!=66)
				{
					GpuSend("SPG(66);\r\n");//administrator keyword			
					delay_ms(LCD_COMM_DELAY_MS);			
					LCD_UI_NO = 66;	
				}	
				break;
			
			case State_UserTry_Once_Password100_new:
			case State_UserTry_Once_Password100_new1:
				if(LCD_UI_NO!=67)
				{
					GpuSend("SPG(67);\r\n");			
					delay_ms(LCD_COMM_DELAY_MS);			
					LCD_UI_NO = 67;	
				}	
				break;
			///////70
			case State_User_Change_Manufacturer_Logo:
				if(LCD_UI_NO!=70)
				{
					//GpuSend("SPG(70);\r\n");
					sprintf(GpuBuf,"CLS(%d);\r\n",LCD_COLOR_CLEAR);
					GpuSend(GpuBuf);	
					delay_ms(LCD_COMM_DELAY_MS/10);
					sprintf(GpuBuf,"DS16(100,131,'%4d',0);\r\n",ChipUniqueID%10000);
					GpuSend(GpuBuf);	
					delay_ms(LCD_COMM_DELAY_MS);			
					LCD_UI_NO = 70;	
				}	
				break;
			
		default:
			break;
		
	}
	if(IsSetParaValueState())
	{
		LcdDisp_Follow_Keyboard(CurState);
	}
	switch(CurState)
	{
		case State_MainMenu_Menu_ScreenSet:	// 主菜单屏幕设置
		case State_MainMenu_Menu_ScenarioSet:	// 主菜单缩放设置
		case State_MainMenu_Menu_MatrixMode:
		case State_MainMenu_Menu_WizardSet:	
		case State_MainMenu_Menu_AdvanceSet:		// 主菜单颜色设置
		case State_WizardSet_OutPort_Num:   //输出口（使用发送卡个数）
		case State_ScreenSet_OutPort_Num:
		case State_WizardSet_TwoPort_Mode:
		case State_WizardSet_ThreePort_Mode:
		case State_WizardSet_FourPort_Mode:
			
		case State_ScreenSet_TwoPort_Mode:
		case State_ScreenSet_ThreePort_Mode:
		case State_ScreenSet_FourPort_Mode:
		case State_WizardSet_InputSource_Set:
		case State_MatrixMode_ModeSet_input:
		case State_MatrixMode_ModeSet_output:
			LcdDisp_Follow_Keyboard(CurState);
			break;
		case State_ScreenSet_Display:
		{	
			GpuSend("SBC(7);");
		//显示总高和宽
			total=0;
			for(i=0;i<g_LineNum;i++)
			  total+=gUserInScrRCPix[1][i];
			if(total>9999)
				sprintf(GpuBuf,"DS12(10,105,'%d',0);",total);
			else if(total>999)
				sprintf(GpuBuf,"DS12(13,105,'%d',0);",total);
			else if(total>99)
				sprintf(GpuBuf,"DS12(16,105,'%d',0);",total);
			else if(total>9)
				sprintf(GpuBuf,"DS12(19,105,'%d',0);",total);
			else 
				sprintf(GpuBuf,"DS12(22,105,'%d',0);",total);
			GpuSend(GpuBuf);
			
			total=0;
			for(i=0;i<g_ColNum;i++)
			  total+=gUserInScrRCPix[0][i];
			if(total>9999)
				sprintf(GpuBuf,"DS12(105,50,'%d',0);",total);
			else if(total>999)
				sprintf(GpuBuf,"DS12(108,50,'%d',0);",total);
			else if(total>99)
				sprintf(GpuBuf,"DS12(111,50,'%d',0);",total);
			else if(total>9)
				sprintf(GpuBuf,"DS12(114,50,'%d',0);",total);
			else 
				sprintf(GpuBuf,"DS12(117,50,'%d',0);",total);
			GpuSend(GpuBuf);			
			GpuSend("\r\n");	
			for(i=1;i<g_LineNum;i++)		
			{
				sprintf(GpuBuf,"PL(42,%d,195,%d,3);",86*i/g_LineNum+72,86*i/g_LineNum+72);
				GpuSend(GpuBuf);	
			}
			for(i=1;i<g_ColNum;i++)		
			{
				sprintf(GpuBuf,"PL(%d,72,%d,158,3);",153*i/g_ColNum+42,153*i/g_ColNum+42);
				GpuSend(GpuBuf);	
			}
			GpuSend("\r\n");	
			break;

		}	
		
		case State_ScenarioSet_NewScheme:
		case State_ScenarioSet_DelScheme:
		case State_ScenarioSet_DelScheme_Num:
		case State_ScenarioSet_NewImage_ImageNum:
			LcdDisp_Follow_Keyboard(CurState);
			break;			
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		case State_ScenarioSet_NewImage_ImagePara_Source:
		case State_ScenarioSet_NewImage_ImagePara_Save:	
			LcdDisp_Follow_Keyboard(CurState);
			break;
		
		case State_AdvanceSet_CardTestSet:
		case State_AdvanceSet_DebugModeSet:
		case State_AdvanceSet_TechSupport:
		case State_AdvanceSet_HelpInfo:
		case State_AdvancePara_FactoryReset:
		case State_UserPara_SetCurrentTime:
			LcdDisp_Follow_Keyboard(CurState);
			break;

		case State_AdvanceSet_CardTest_Port:
		case State_AdvanceSet_CardTest_Color:
			LcdDisp_Follow_Keyboard(CurState);
			break;
		
		case State_UserTrySet_Start:
		case State_UserTrySet_End:
		case State_UserTrySet_Day:
		case State_UserTrySet_PassWord:
		case State_UserTrySet_Tel:
			LcdDisp_Follow_Keyboard(CurState);
			break;	
		case State_TechSuppor_ContactInfo:
			if(gManufactoryID==1)
			{
			sprintf(GpuBuf,"SBC(7);DS12(86,55,'联系电话：',0);DS16(62,73,'%s',0);DS12(86,109,'程序版本：',0);DS16(86,127,'%s',0);\r\n",g_ContactTel1,g_ReleaseStr);
			GpuSend(GpuBuf);
			}
			break;

		case State_MainMenu_Menu:
			lcd_disp_curr_mode_no();
			break;
		case State_MainMenu_Menu_Logo:
			if(gManufactoryID==1)
			{
				GpuSend("SPG(100);\r\n");	
				delay_ms(LCD_COMM_DELAY_MS);
			}
				LCD_UI_NO=100;
			break;
		
		case State_MainMenu_Menu_InOutput:
			GpuSend("SPG(101);\r\n");	
			READ_INPUT_SOURCE_RESOLUTION();
			
			for (i=0;i<IN_PORT_NUM;i++)
				if(InSrcRes[i].w==0)
					BackClolor[i]=LCD_COLOR_GRAY;
				else
					BackClolor[i]=LCD_COLOR_GREEN;
			
			//sprintf(GpuBuf,"SBC(2);DS16(90,60,'%d*%d',0);DS16(90,80,'%d*%d',0);DS16(90,100,'%d*%d',0);DS16(90,120,'%d*%d',0);\r\n",
			//===2017,Jun 19 ; 15:58:5  ;BY YANG@BYCOX.COM===
			sprintf(GpuBuf,"SBC(2);DS12(%d,48,'%s',0);DS12(%d,64,'%s',0);DS12(%d,80,'%s',0);DS12(%d,96,'%s',0);\r\n",
			LCD_DISP_RES_NAME,InSrcRes[0].name,
			LCD_DISP_RES_NAME,InSrcRes[1].name,
			LCD_DISP_RES_NAME,InSrcRes[2].name,
			LCD_DISP_RES_NAME,InSrcRes[3].name);
			GpuSend(GpuBuf);
				delay_ms(LCD_COMM_DELAY_MS);

			sprintf(GpuBuf,"SBC(%d);DS12(%d,48,'%d*%d',0);SBC(%d);DS12(%d,64,'%d*%d',0);SBC(%d);DS12(%d,80,'%d*%d',0);SBC(%d);DS12(%d,96,'%d*%d',0);\r\n",
			BackClolor[0],	LCD_DISP_RES_COL,InSrcRes[0].w,InSrcRes[0].h,
			BackClolor[1],LCD_DISP_RES_COL,InSrcRes[1].w,InSrcRes[1].h,
			BackClolor[2],LCD_DISP_RES_COL,InSrcRes[2].w,InSrcRes[2].h,
			BackClolor[3],LCD_DISP_RES_COL,InSrcRes[3].w,InSrcRes[3].h);
			GpuSend(GpuBuf);
				delay_ms(LCD_COMM_DELAY_MS);

			
			LCD_UI_NO=101;
			break;
			
		case State_MainMenu_Menu_VersionInfo:

			Color_VersionInfo();	
			LCD_UI_NO=101;
			break;
		
		case State_MainMenu_Menu_StartInfo:
		case State_MainMenu_Menu_OperationGuide:
			break;


		case State_TechSuppor_VersionInfo:
			LcdDisp_Follow_Keyboard(CurState);
			break;

		case State_UserPara_SetNewTime_Date:
		case State_UserPara_SetNewTime_Time:
		case State_UserPara_SetNewTime_Week:
			LcdDisp_Follow_Keyboard(CurState);
			break;
	}
}



/******************************
在默认的显示页面上显示但钱的用户模式序号
只显示一个数字
//===2017,Jun 17 ; 14:6:32  ;BY YANG@BYCOX.COM===
******************************/
void lcd_disp_curr_mode_no()
{

	char GpuBuf[500];
	
	GpuSend("SBC(5);");				
	sprintf(GpuBuf,"DS24(160,23,'%d',0);",g_Current_Scene);
	GpuSend(GpuBuf);
	GpuSend("SBC(7);\r\n");		
}




void LCD_DispCurrTime()//++++++++++++++++++++++++++++++++++
{
	char GpuBuf[500];
	GpuSend("SBC(5);");		
	sprintf(GpuBuf,"DS16(6,155,'%02d:%02d:%02d',0);",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
	GpuSend(GpuBuf);
	delay_ms(20);
	if(g_LanguageFlag==CHINESE)
	{
		switch(systmtime.tm_wday)
		{
			case 0:
				GpuSend("DS16(86,155,'周日',0);");
				break;
			case 1:
				GpuSend("DS16(86,155,'周一',0);");
				break;
			case 2:
				GpuSend("DS16(86,155,'周二',0);");
				break;
			case 3:
				GpuSend("DS16(86,155,'周三',0);");
				break;
			case 4:
				GpuSend("DS16(86,155,'周四',0);");
				break;
			case 5:
				GpuSend("DS16(86,155,'周五',0);");
				break;
			case 6:
				GpuSend("DS16(86,155,'周六',0);");
				break;
		}
	}
	else if(g_LanguageFlag==ENGLISH)
	{
		switch(systmtime.tm_wday)
		{
			case 0:
				GpuSend("DS16(86,155,'Sun.',0);");
				break;	
			case 1:
				GpuSend("DS16(86,155,'Mon.',0);");
				break;
			case 2:
				GpuSend("DS16(86,155,'Tues',0);");
				break;
			case 3:
				GpuSend("DS16(86,155,'Wed.',0);");
				break;
			case 4:
				GpuSend("DS16(86,155,'Thur',0);");
				break;
			case 5:
				GpuSend("DS16(86,155,'Fri.',0);");
				break;
			case 6:
				GpuSend("DS16(86,155,'Sat.',0);");
				break;		
		}
	}

	sprintf(GpuBuf,"DS16(134,155,'%04d-%02d-%02d',0);",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday);
	GpuSend(GpuBuf);
	GpuSend("SBC(7);\r\n");		
	delay_ms(20);
}

LcdDispSoftRev(U8 SoftVersion[])//+++++++++++++++++++++++++++++
{
	char GpuBuf[100];
	sprintf(GpuBuf,"DS12(60,131,'%s',0);",SoftVersion);
	GpuSend(GpuBuf);
}

LcdDispMachineID(U8 MachineIDValue[])//+++++++++++++++++++++++++++++
{
	char GpuBuf[100];
	sprintf(GpuBuf,"DS12(160,144,'%s',0);",MachineIDValue);
	GpuSend(GpuBuf);
}

LcdDispMachineRev(U8 MachineRevision[])//+++++++++++++++++++++++++++++
{
	char GpuBuf[100];
	sprintf(GpuBuf,"DS12(60,144,'%s',0);",MachineRevision);
	GpuSend(GpuBuf);
}

LcdDisp_PhoneNum(U8 PhoneNumber[])//+++++++++++++++++++++++++++++
{
	char GpuBuf[100];
	sprintf(GpuBuf,"DS12(60,157,'%s',0);",PhoneNumber);
	GpuSend(GpuBuf);
}




//===2017,Jun 13 ; 18:22:11  ;BY YANG@BYCOX.COM===
//color lcd display set along with keyboard operation
void LcdDisp_Follow_Keyboard(State CurState)
{

	char GpuBuf[100];
	switch(CurState)
	{
		case State_MainMenu_Menu_ScreenSet:	
		case State_MainMenu_Menu_ScenarioSet:
		case State_MainMenu_Menu_MatrixMode:
		case State_MainMenu_Menu_WizardSet:
		case State_MainMenu_Menu_AdvanceSet:	
		{
			GpuSend("SBC(5);");
			GpuSend("CBOF(28,55,100,80,7,5);CBOX(28,55,100,80,7,0);DS12(31,62,'一.大屏设置',0);CBOF(120,55,192,80,7,5);CBOX(120,55,192,80,7,0);DS12(123,62,'四.向导设置',0);");
			GpuSend("CBOF(28,88,100,113,7,5);CBOX(28,88,100,113,7,0);DS12(31,95,'二.方案管理',0);CBOF(120,88,192,113,7,5);CBOX(120,88,192,113,7,0);DS12(123,95,'五.高级设置',0);");
			GpuSend("CBOF(28,121,100,146,7,5);CBOX(28,121,100,146,7,0);DS12(31,128,'三.矩阵模式',0);");
			GpuSend("SBC(4);");		
			switch(CurState)
			{			
				case State_MainMenu_Menu_ScreenSet:
					{

					GpuSend("CBOF(28,55,100,80,7,4);CBOX(28,55,100,80,7,0);DS12(31,62,'一.大屏设置',0);\r\n");

					break;
					}
				case State_MainMenu_Menu_ScenarioSet:
					{
						
					GpuSend("CBOF(28,88,100,113,7,4);CBOX(28,88,100,113,7,0);DS12(31,95,'二.方案管理',0);\r\n");
					
					break;
					}
				case State_MainMenu_Menu_MatrixMode:
					GpuSend("CBOF(28,121,100,146,7,4);CBOX(28,121,100,146,7,0);DS12(31,128,'三.矩阵模式',0);\r\n");
					break;
				case State_MainMenu_Menu_WizardSet:
					{
						
					GpuSend("CBOF(120,55,192,80,7,4);CBOX(120,55,192,80,7,0);DS12(123,62,'四.向导设置',0);\r\n");
					break;
					}
				case State_MainMenu_Menu_AdvanceSet:
					{
						
					GpuSend("CBOF(120,88,192,113,7,4);CBOX(120,88,192,113,7,0);DS12(123,95,'五.高级设置',0);\r\n");
					break;
					}
			}
			delay_ms(LCD_COMM_DELAY_MS);	
			break;
		}	

		case State_ScenarioSet_NewScheme:
		case State_ScenarioSet_DelScheme:
		{
			GpuSend("SBC(5);");
			GpuSend("CBOF(77,60,143,85,5,5);CBOX(77,60,143,85,5,0);DS12(85,65,'新建方案',0);");
			GpuSend("CBOF(77,90,143,115,5,5);CBOX(77,90,143,115,5,0);DS12(85,95,'删除方案',0);");
			GpuSend("SBC(4);");	
			switch(CurState)
			{			
				case State_ScenarioSet_NewScheme:
					GpuSend("CBOF(77,60,143,85,5,4);CBOX(77,60,143,85,5,0);DS12(85,65,'新建方案',0);\r\n");
					break;
				case State_ScenarioSet_DelScheme:
					GpuSend("CBOF(77,90,143,115,5,4);CBOX(77,90,143,115,5,0);DS12(85,95,'删除方案',0);\r\n");	
					break;
			}			
			break;
		}
		
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		{
			GpuSend("SBC(5);");
			sprintf(GpuBuf,"DS16(10,10,'新建方案>>画面%d 参数',0);",g_ImageNum_Set);
			GpuSend(GpuBuf);
			GpuSend("CBOF(7,60,65,82,5,5);CBOX(7,60,65,82,5,0);DS12(17,65,'起点X:',0);");
			GpuSend("CBOF(110,60,168,82,5,5);CBOX(110,60,168,82,5,0);DS12(120,65,'起点Y:',0);\r\n");
			GpuSend("CBOF(7,90,65,112,5,5);CBOX(7,90,65,112,5,0);DS12(17,95,'终点X:',0);");
			GpuSend("CBOF(110,90,168,112,5,5);CBOX(110,90,168,112,5,0);DS12(120,95,'终点Y:',0);\r\n");
			delay_ms(10);
			GpuSend("SBC(4);");		
			GpuSend("CBOF(10,130,140,160,1,7);");		
			switch(CurState)
			{
				case State_ScenarioSet_NewImage_ImagePara_Begin_X:
					GpuSend("CBOF(7,60,65,82,5,4);CBOX(7,60,65,82,5,0);DS12(17,65,'起点X:',0);\r\n");
					break;
				case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
					GpuSend("CBOF(110,60,168,82,5,4);CBOX(110,60,168,82,5,0);DS12(120,65,'起点Y:',0);\r\n");
					break;
				case State_ScenarioSet_NewImage_ImagePara_End_X:
					GpuSend("CBOF(7,90,65,112,5,4);CBOX(7,90,65,112,5,0);DS12(17,95,'终点X:',0);\r\n");
					break;
				case State_ScenarioSet_NewImage_ImagePara_End_Y:
					GpuSend("CBOF(110,90,168,112,5,4);CBOX(110,90,168,112,5,0);DS12(120,95,'终点Y:',0);\r\n");
					break;			
			}			
			break;
		}					

		
		case State_AdvanceSet_CardTestSet:
		case State_AdvanceSet_DebugModeSet:
		case State_AdvanceSet_TechSupport:
		case State_AdvanceSet_HelpInfo:
		case State_AdvancePara_FactoryReset:
		case State_UserPara_SetCurrentTime:
		{		
			GpuSend("SBC(5);");
			GpuSend("CBOF(28,50,100,75,7,5);CBOX(28,50,100,75,7,0);DS12(40,57,'图卡测试',0);");
			GpuSend("CBOF(28,85,100,110,7,5);CBOX(28,85,100,110,7,0);DS12(40,92,'调试模式',0);");
			GpuSend("CBOF(28,120,100,145,7,5);CBOX(28,120,100,145,7,0);DS12(40,127,'时间设置',0);");
			GpuSend("CBOF(120,50,192,75,7,5);CBOX(120,50,192,75,7,0);DS12(132,57,'帮助信息',0);");
//			GpuSend("CBOF(120,85,192,110,7,5);CBOX(120,85,192,110,7,0);DS12(132,92,'技术支持',0);");
			GpuSend("CBOF(120,85,192,110,7,5);CBOX(120,85,192,110,7,0);DS12(132,92,'恢复出厂',0);");
			GpuSend("SBC(4);");				
			switch(CurState)
			{
				case State_AdvanceSet_CardTestSet:
					GpuSend("CBOF(28,50,100,75,7,4);CBOX(28,50,100,75,7,0);DS12(40,57,'图卡测试',0);\r\n");
					break;
				case State_AdvanceSet_DebugModeSet:
					GpuSend("CBOF(28,85,100,110,7,4);CBOX(28,85,100,110,7,0);DS12(40,92,'调试模式',0);\r\n");
					break;
				case State_UserPara_SetCurrentTime:
					GpuSend("CBOF(28,120,100,145,7,4);CBOX(28,120,100,145,7,0);DS12(40,127,'时间设置',0);\r\n");
					break;
				case State_AdvanceSet_HelpInfo:
						GpuSend("CBOF(120,50,192,75,7,4);CBOX(120,50,192,75,7,0);DS12(132,57,'帮助信息',0);\r\n");
					break;
//				case State_AdvanceSet_TechSupport:
//					GpuSend("CBOF(120,85,192,110,7,4);CBOX(120,85,192,110,7,0);DS12(132,92,'技术支持',0);\r\n");
//					break;	
				case State_AdvancePara_FactoryReset:
					GpuSend("CBOF(120,85,192,110,7,4);CBOX(120,85,192,110,7,0);DS12(132,92,'恢复出厂',0);\r\n");
					break;		
			}	
			break;
		}		
		case State_WizardSet_OutPort_Num:
		case State_ScreenSet_OutPort_Num:
			GpuSend("CBOF(146,69,184,87,3,15);SBC(15);DS16(160,71,'2',7);\r\n");
			break;
		case State_WizardSet_TwoPort_Mode:
		case State_ScreenSet_TwoPort_Mode:	
			GpuSend("CBOF(171,146,194,169,1,15);SBC(15);DS16(178,150,'2',7);\r\n");
			break;
		case State_WizardSet_ThreePort_Mode:
		case State_ScreenSet_ThreePort_Mode:	
			GpuSend("CBOF(171,146,194,169,1,15);SBC(15);DS16(178,150,'2',7);\r\n");
			break;
		case State_WizardSet_FourPort_Mode:
		case State_ScreenSet_FourPort_Mode:	
			GpuSend("CBOF(171,146,194,169,1,15);SBC(15);DS16(178,150,'2',7);\r\n");
			break;
		case State_MatrixMode_ModeSet_input:
			GpuSend("CBOF(66,66,94,89,3,15);SBC(15);DS16(77,70,'0',7);\r\n");
			break;
		case State_MatrixMode_ModeSet_output:
			GpuSend("CBOF(126,66,154,89,3,15);SBC(15);DS16(137,70,'0',7);\r\n");
			break;
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
			if(g_LineNum_Set<=g_LineNum)
			{
				
				//在此处进行设置行高度显示
				
				switch(CurState)
				{
					
					case State_WizardSet_TwoPort_layout1:
					case State_ScreenSet_TwoPort_layout1:						
						if(g_LineNum_Set==1)
						{
							GpuSend("CBOF(95,60,125,120,1,7);CBOX(95,60,125,120,1,0);PL(95,90,125,91);CBOF(96,61,124,89,1,5);SBC(5);DS16(105,67,'1',0);SBC(7);DS16(105,97,'2',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(95,60,125,120,1,7);CBOX(95,60,125,120,1,0);PL(95,90,125,91);CBOF(96,92,124,119,1,5);SBC(5);DS16(105,97,'2',0);SBC(7);DS16(105,67,'1',0);\r\n");
						}
						break;
					
					case State_WizardSet_TwoPort_layout2:
					case State_ScreenSet_TwoPort_layout2:
						GpuSend("CBOF(75,75,155,105,1,5);CBOX(75,75,155,105,1,0);PL(115,75,116,105);SBC(5);DS16(90,82,'1',0);DS16(130,82,'2',0);SBC(7);\r\n");
						break;
					
					case State_WizardSet_ThreePort_layout1:
					case State_ScreenSet_ThreePort_layout1:
						if(g_LineNum_Set==1)
						{
							GpuSend("CBOF(95,60,125,120,1,7);CBOX(95,60,125,120,1,0);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,61,124,79,1,5);SBC(5);DS16(107,62,'1',0);SBC(7);DS16(107,82,'2',0);DS16(107,102,'3',0);\r\n");
						}
						else if(g_LineNum_Set==2)
						{
							GpuSend("CBOF(95,60,125,120,1,7);CBOX(95,60,125,120,1,0);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,82,124,99,1,5);SBC(5);DS16(107,82,'2',0);SBC(7);DS16(107,62,'1',0);DS16(107,102,'3',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(95,60,125,120,1,7);CBOX(95,60,125,120,1,0);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,102,124,119,1,5);SBC(5);DS16(107,102,'3',0);SBC(7);DS16(107,62,'1',0);DS16(107,82,'2',0);\r\n");
						}
						break;
					case State_WizardSet_ThreePort_layout2:
					case State_ScreenSet_ThreePort_layout2:
						GpuSend("CBOF(55,75,175,105,1,5);CBOX(55,75,175,105,1,0);PL(95,75,96,105);PL(135,75,136,105);SBC(5);DS16(70,82,'1',0);DS16(110,82,'2',0);DS16(150,82,'3',0);SBC(7);\r\n");
						break;
					
					case State_ScreenSet_FourPort_layout1:
					case State_WizardSet_FourPort_layout1:
						if(g_LineNum_Set==1)
						{
							GpuSend("CBOF(95,43,125,120,1,7);CBOX(95,43,125,120,1,0);PL(95,60,125,61);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,44,124,59,1,5);SBC(5);DS16(107,44,'1',0);SBC(7);DS16(107,62,'2',0);DS16(107,82,'3',0);DS16(107,102,'4',0);\r\n");
						}
						else if(g_LineNum_Set==2)
						{
							GpuSend("CBOF(95,43,125,120,1,7);CBOX(95,43,125,120,1,0);PL(95,60,125,61);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,62,124,79,1,5);SBC(5);DS16(107,62,'2',0);SBC(7);DS16(107,44,'1',0);DS16(107,82,'3',0);DS16(107,102,'4',0);\r\n");
						}
						else if(g_LineNum_Set==3)
						{
							GpuSend("CBOF(95,43,125,120,1,7);CBOX(95,43,125,120,1,0);PL(95,60,125,61);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,82,124,99,1,5);SBC(5);DS16(107,82,'3',0);SBC(7);DS16(107,44,'1',0);DS16(107,62,'2',0);DS16(107,102,'4',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(95,43,125,120,1,7);CBOX(95,43,125,120,1,0);PL(95,60,125,61);PL(95,80,125,81);PL(95,100,125,101);CBOF(96,102,124,119,1,5);SBC(5);DS16(107,102,'4',0);SBC(7);DS16(107,44,'1',0);DS16(107,62,'2',0);DS16(107,82,'3',0);\r\n");
						}
						break;
						
					case State_WizardSet_FourPort_layout2:
					case State_ScreenSet_FourPort_layout2:
						if(g_LineNum_Set==1)
						{
							GpuSend("CBOF(70,60,150,120,1,7);CBOX(70,60,150,120,1,0);CBOF(71,61,149,89,1,5);PL(110,60,111,120);PL(70,90,150,91);SBC(5);DS16(87,70,'1',0);DS16(127,70,'2',0);SBC(7);DS16(87,100,'3',0);DS16(127,100,'4',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(70,60,150,120,1,7);CBOX(70,60,150,120,1,0);CBOF(71,92,149,119,1,5);PL(110,60,111,120);PL(70,90,150,91);SBC(5);DS16(87,100,'3',0);DS16(127,100,'4',0);SBC(7);DS16(87,70,'1',0);DS16(127,70,'2',0);\r\n");
						}
						break;
						
					case State_WizardSet_FourPort_layout3:
					case State_ScreenSet_FourPort_layout3:
							GpuSend("CBOF(40,70,200,110,1,5);CBOX(40,70,200,110,1,0);PL(80,70,81,110);PL(120,70,121,110);PL(160,70,161,110);SBC(5);DS16(57,83,'1',0);DS16(97,83,'2',0);DS16(137,83,'3',0);DS16(177,83,'4',0);SBC(7);\r\n");					
						break;
					
				}
				sprintf(GpuBuf,"SBC(7);DS16(15,130,'第%d行垂直点数:',0);",g_LineNum_Set);
				GpuSend(GpuBuf);
				gUserInScrRCPix[1][g_LineNum_Set-1]=1080;
				sprintf(GpuBuf,"SBC(15);CBOF(131,126,204,149,1,15);DS16(140,130,'%d',7);\r\n",gUserInScrRCPix[1][g_LineNum_Set-1]);
				GpuSend(GpuBuf);
			}
			else
			{
				switch(CurState)
				{
					
					case State_WizardSet_TwoPort_layout1:
					case State_ScreenSet_TwoPort_layout1:						
						GpuSend("CBOF(95,60,125,120,1,5);CBOX(95,60,125,120,1,0);PL(95,90,125,91);SBC(5);DS16(105,67,'1',0);DS16(105,97,'2',0);SBC(7);\r\n");
						break;
										
					case State_WizardSet_TwoPort_layout2:
					case State_ScreenSet_TwoPort_layout2:
						if(g_ColNum_Set==1)
						{
							GpuSend("CBOF(75,75,155,105,1,7);CBOX(75,75,155,105,1,0);PL(115,75,116,105);CBOF(76,76,114,104,1,5);SBC(5);DS16(90,82,'1',0);SBC(7);DS16(130,82,'2',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(75,75,155,105,1,7);CBOX(75,75,155,105,1,0);PL(115,75,116,105);CBOF(116,76,154,104,1,5);SBC(5);DS16(130,82,'2',0);SBC(7);DS16(90,82,'1',0);\r\n");
						}
						break;					
					case State_WizardSet_ThreePort_layout1:
					case State_ScreenSet_ThreePort_layout1:
							GpuSend("CBOF(95,60,125,120,1,5);CBOX(95,60,125,120,1,0);PL(95,80,125,81);PL(95,100,125,101);SBC(5);DS16(107,62,'1',0);DS16(107,82,'2',0);DS16(107,102,'3',0);SBC(7);\r\n");
							break;
					case State_WizardSet_ThreePort_layout2:
					case State_ScreenSet_ThreePort_layout2:
						if(g_ColNum_Set==1)
						{
							GpuSend("CBOF(55,75,175,105,1,7);CBOX(55,75,175,105,1,0);PL(95,75,96,105);PL(135,75,136,105);CBOF(56,76,94,104,1,5);SBC(5);DS16(70,82,'1',0);SBC(7);DS16(110,82,'2',0);DS16(150,82,'3',0);\r\n");							
						}
						else if(g_ColNum_Set==2)
						{
							GpuSend("CBOF(55,75,175,105,1,7);CBOX(55,75,175,105,1,0);PL(95,75,96,105);PL(135,75,136,105);CBOF(97,76,134,104,1,5);SBC(5);DS16(110,82,'2',0);SBC(7);DS16(70,82,'1',0);DS16(150,82,'3',0);\r\n");	
						}
						else
						{
							GpuSend("CBOF(55,75,175,105,1,7);CBOX(55,75,175,105,1,0);PL(95,75,96,105);PL(135,75,136,105);CBOF(136,76,174,104,1,5);SBC(5);DS16(150,82,'3',0);SBC(7);DS16(70,82,'1',0);DS16(110,82,'2',0);\r\n");
						}
					break;
					
					case State_ScreenSet_FourPort_layout1:
					case State_WizardSet_FourPort_layout1:
							GpuSend("CBOF(95,43,125,120,1,5);CBOX(95,43,125,120,1,0);PL(95,60,125,61);PL(95,80,125,81);PL(95,100,125,101);SBC(5);DS16(107,44,'1',0);DS16(107,62,'2',0);DS16(107,82,'3',0);DS16(107,102,'4',0);SBC(7);\r\n");
						break;
						
					case State_WizardSet_FourPort_layout2:
					case State_ScreenSet_FourPort_layout2:
						if(g_ColNum_Set==1)
						{
							GpuSend("CBOF(70,60,150,120,1,7);CBOX(70,60,150,120,1,0);CBOF(71,61,109,119,1,5);PL(110,60,111,120);PL(70,90,150,91);SBC(5);DS16(87,70,'1',0);DS16(87,100,'3',0);SBC(7);DS16(127,70,'2',0);DS16(127,100,'4',0);\r\n");
						}
						else
						{
							GpuSend("CBOF(70,60,150,120,1,7);CBOX(70,60,150,120,1,0);CBOF(112,61,149,119,1,5);PL(110,60,111,120);PL(70,90,150,91);SBC(5);DS16(127,70,'2',0);DS16(127,100,'4',0);SBC(7);DS16(87,70,'1',0);DS16(87,100,'3',0);\r\n");
						}
						break;						
					case State_WizardSet_FourPort_layout3:
					case State_ScreenSet_FourPort_layout3:
						if(g_ColNum_Set==1)
						{
								GpuSend("CBOF(40,70,200,110,1,7);CBOX(40,70,200,110,1,0);CBOF(41,71,79,109,1,5);PL(80,70,81,110);PL(120,70,121,110);PL(160,70,161,110);SBC(5);DS16(57,83,'1',0);SBC(7);DS16(97,83,'2',0);DS16(137,83,'3',0);DS16(177,83,'4',0);\r\n");	
						}
						else if(g_ColNum_Set==2)
						{
								GpuSend("CBOF(40,70,200,110,1,7);CBOX(40,70,200,110,1,0);CBOF(82,70,119,110,1,5);PL(80,70,81,110);PL(120,70,121,110);PL(160,70,161,110);SBC(5);DS16(97,83,'2',0);SBC(7);DS16(57,83,'1',0);DS16(137,83,'3',0);DS16(177,83,'4',0);\r\n");	
						}
						else if(g_ColNum_Set==3)
						{
								GpuSend("CBOF(40,70,200,110,1,7);CBOX(40,70,200,110,1,0);CBOF(122,70,159,110,1,5);PL(80,70,81,110);PL(120,70,121,110);PL(160,70,161,110);SBC(5);DS16(137,83,'3',0);SBC(7);DS16(57,83,'1',0);DS16(97,83,'2',0);DS16(177,83,'4',0);\r\n");	
						}
						else
						{
								GpuSend("CBOF(40,70,200,110,1,7);CBOX(40,70,200,110,1,0);CBOF(162,70,199,110,1,5);PL(80,70,81,110);PL(120,70,121,110);PL(160,70,161,110);SBC(5);DS16(177,83,'4',0);SBC(7);DS16(57,83,'1',0);DS16(97,83,'2',0);DS16(137,83,'3',0);\r\n");	
						}										
						break;					
				}				
				sprintf(GpuBuf,"SBC(7);DS16(15,130,'第%d列水平点数:',0);",g_ColNum_Set);
				GpuSend(GpuBuf);
				gUserInScrRCPix[0][g_ColNum_Set-1]=1920;
				sprintf(GpuBuf,"SBC(15);CBOF(131,126,204,149,1,15);DS16(140,130,'%d',7);\r\n",gUserInScrRCPix[0][g_ColNum_Set-1]);
				GpuSend(GpuBuf);
			}
			break;		


			
		case State_ScenarioSet_NewImage_ImagePara_Source:
		case State_WizardSet_InputSource_Set:
		{
				GpuSend("SBC(5);");
				if(CurState==State_ScenarioSet_NewImage_ImagePara_Source)
				{
					sprintf(GpuBuf,"DS16(10,10,'新建方案>>画面%d 信号源',0);\r\n",g_ImageNum_Set);			
				}
				else
				{
					sprintf(GpuBuf,"DS16(10,10,'向导1.4 输入源选择',0);\r\n");		
				}
				GpuSend(GpuBuf);
				if(InSrcRes[0].w!=0)
				{
					GpuSend("SBC(5);");
					GpuSend("CBOF(28,58,100,87,7,5);CBOX(28,58,100,87,7,0);DS12(43,66,'1.DVI-1',0);\r\n");	
				}	
				else 
				{
					GpuSend("SBC(7);");
					GpuSend("CBOF(28,58,100,87,7,7);CBOX(28,58,100,87,7,0);DS12(43,66,'1.DVI-1',0);\r\n");	
				}
				
				
				if(InSrcRes[1].w!=0)
				{
					GpuSend("SBC(5);");
					GpuSend("CBOF(120,58,192,87,7,5);CBOX(120,58,192,87,7,0);DS12(133,66,'2.DVI-2',0);\r\n");
				}	
				else 
				{
					GpuSend("SBC(7);");
					GpuSend("CBOF(120,58,192,87,7,7);CBOX(120,58,192,87,7,0);DS12(133,66,'2.DVI-2',0);\r\n");
				}
					
				if(InSrcRes[2].w!=0)
				{
					GpuSend("SBC(5);");
					GpuSend("CBOF(28,98,100,127,7,5);CBOX(28,98,100,127,7,0);DS12(43,106,'3.DVI-3',0);\r\n");
				}	
				else 
				{
					GpuSend("SBC(7);");
					GpuSend("CBOF(28,98,100,127,7,7);CBOX(28,98,100,127,7,0);DS12(43,106,'3.DVI-3',0);\r\n");
				}
		
				if(InSrcRes[3].w!=0)
				{
					GpuSend("SBC(5);");
					GpuSend("CBOF(120,98,192,127,7,5);CBOX(120,98,192,127,7,0);DS12(133,106,'4.DVI-4',0);\r\n");
				}	
				else 
				{
					GpuSend("SBC(7);");
					GpuSend("CBOF(120,98,192,127,7,7);CBOX(120,98,192,127,7,0);DS12(133,106,'4.DVI-4',0);\r\n");
				}
								
				GpuSend("SBC(4);");	

				switch(g_Image_Scr)
				{
					case 1:
						{
						GpuSend("CBOF(28,58,100,87,7,4);CBOX(28,58,100,87,7,0);DS12(43,66,'1.DVI-1',0);\r\n");					
						break;
						}
					case 2:
						{							
						GpuSend("CBOF(120,58,192,87,7,4);CBOX(120,58,192,87,7,0);DS12(133,66,'2.DVI-2',0);\r\n");	
						break;
						}
					case 3:
						{							
						GpuSend("CBOF(28,98,100,127,7,4);CBOX(28,98,100,127,7,0);DS12(43,106,'3.DVI-3',0);\r\n");
						break;
						}
					case 4:
						{							
						GpuSend("CBOF(120,98,192,127,7,4);CBOX(120,98,192,127,7,0);DS12(133,106,'4.DVI-4',0);\r\n");
						break;
						}
				}
		}
		break;
		case State_AdvanceSet_CardTest_Port:
		{
			GpuSend("SBC(5);");		
			GpuSend("CBOF(10,110,80,137,7,5);CBOX(10,110,80,137,7,0);DS12(22,118,'输出颜色',0);");
			GpuSend("SBC(4);");		
			GpuSend("CBOF(10,50,80,77,7,4);CBOX(10,50,80,77,7,0);DS12(22,58,'输出端口',0);\r\n");
			
			//GpuSend("CBOF(100,45,200,105,0,7);CBOX(100,45,200,105,0,0);");
			GpuSend("CBOF(100,45,200,110,0,7);CBOX(100,45,200,110,0,0);");
			GpuSend("PL(100,75,200,75,0);\r\n");
			GpuSend("PL(125,45,125,105,0);PL(150,45,150,105,0);PL(175,45,175,105,0);\r\n");
			GpuSend("SBC(7);");		
			GpuSend("DS16(109,54,'1',0);DS16(134,54,'2',0);DS16(159,54,'3',0);DS16(184,54,'4',0);\r\n");//69-->54
			GpuSend("DS16(109,89,'5',0);DS16(134,89,'6',0);DS16(159,89,'7',0);DS16(184,89,'8',0);\r\n");//

			GpuSend("SBC(5);");	
			switch(g_CardTest_Port)
			{
				case 1:
					GpuSend("CBOF(101,46,124,74,0,5);CBOX(101,46,124,74,0,7);DS16(109,54,'1',0);\r\n");//69-->54
					break;
				case 2:
					GpuSend("CBOF(126,46,149,74,0,5);CBOX(126,46,149,74,0,7);DS16(134,54,'2',0);\r\n");
					break;
				case 3:
					GpuSend("CBOF(151,46,174,74,0,5);CBOX(101,46,174,74,0,7);DS16(159,54,'3',0);\r\n");
					break;
				case 4:
					GpuSend("CBOF(176,46,199,74,0,5);CBOX(176,46,199,74,0,7);DS16(184,54,'4',0);\r\n");
					break;

				case 5:
					GpuSend("CBOF(101,81,124,109,0,5);CBOX(101,81,124,109,0,7);DS16(109,89,'5',0);\r\n");
					break;
				case 6:
					GpuSend("CBOF(126,81,149,109,0,5);CBOX(126,81,149,109,0,7);DS16(134,89,'6',0);\r\n");
					break;
				case 7:
					GpuSend("CBOF(151,81,174,109,0,5);CBOX(101,81,174,109,0,7);DS16(159,89,'7',0);\r\n");
					break;
				case 8:
					GpuSend("CBOF(176,81,199,109,0,5);CBOX(176,81,199,109,0,7);DS16(184,89,'8',0);\r\n");
					break;	
				default:
					break;				
			}			
			break;
		}		
		case State_AdvanceSet_CardTest_Color:
		{
			GpuSend("SBC(5);");		
			//GpuSend("CBOF(10,60,80,87,7,5);CBOX(10,60,80,87,7,0);DS12(22,68,'输出端口',0);\r\n");
			//===2017,Jun 14 ; 11:8:40  ;BY YANG@BYCOX.COM=== sub 15 in verti 
			GpuSend("CBOF(10,50,80,77,7,5);CBOX(10,50,80,77,7,0);DS12(22,58,'输出端口',0);\r\n");
			GpuSend("SBC(4);");		
			GpuSend("CBOF(10,110,80,137,7,4);CBOX(10,110,80,137,7,0);DS12(22,118,'输出颜色',0);");
						
			GpuSend("SBC(1);CBOF(100,110,130,140,3,7);CBOF(105,115,125,135,3,1);CBOX(105,115,125,135,3,0);DS16(111,119,'1',0);\r\n");
			GpuSend("SBC(2);CBOF(135,110,165,140,3,7);CBOF(140,115,160,135,3,2);CBOX(140,115,160,135,3,0);DS16(146,119,'2',0);\r\n");
			GpuSend("SBC(3);CBOF(170,110,200,140,3,7);CBOF(175,115,195,135,3,3);CBOX(175,115,195,135,3,0);DS16(181,119,'3',0);\r\n");
		
			switch(g_CardTest_Color)
			{
				case 1:
					GpuSend("SBC(1);CBOF(100,110,130,140,3,1);CBOX(100,110,130,140,3,0);DS24(110,115,'1',0);\r\n");
					break;
				case 2:
					GpuSend("SBC(2);CBOF(135,110,165,140,3,2);CBOX(135,110,165,140,3,0);DS24(145,115,'2',0);\r\n");
					break;
				case 3:
					GpuSend("SBC(3);CBOF(170,110,200,140,3,3);CBOX(170,110,200,140,3,0);DS24(180,115,'3',0);\r\n");
					break;
				default:
					break;				
			}			
			break;
		}
		case State_UserTrySet_Start:
		case State_UserTrySet_End:
		{
			GpuSend("SBC(5);");
			GpuSend("CBOF(77,75,143,93,7,5);CBOX(77,75,143,93,7,0);DS12(85,78,'开启试用',0);");
			GpuSend("CBOF(77,113,143,131,7,5);CBOX(77,113,143,131,7,0);DS12(85,117,'关闭试用',0);");	
			GpuSend("SBC(4);");	
			switch(CurState)
			{
				case State_UserTrySet_Start:
					GpuSend("CBOF(77,75,143,93,7,4);CBOX(77,75,143,93,7,0);DS12(85,78,'开启试用',0);\r\n");
					break;
				case State_UserTrySet_End:
					GpuSend("CBOF(77,113,143,131,7,4);CBOX(77,113,143,131,7,0);DS12(85,117,'关闭试用',0);\r\n");	
					break;
			}
			break;
		}
		case State_UserTrySet_Day:
		case State_UserTrySet_PassWord:
		case State_UserTrySet_Tel:
		{
				if(Is_Input_Num())
				{
						InputNumReset();
				}
			GpuSend("SBC(5);");
			GpuSend("CBOF(29,58,100,77,7,5);CBOX(29,58,100,77,7,0);DS12(35,61,' 试用天数:',0);");
			GpuSend("CBOF(29,83,100,102,7,5);CBOX(29,83,100,102,7,0);DS12(35,86,' 试用密码:',0);");	
			GpuSend("CBOF(29,108,100,127,7,5);CBOX(29,108,100,127,7,0);DS12(35,111,' 解锁电话:',0);");	
			GpuSend("SBC(4);");	
			
			switch(CurState)
			{
				case State_UserTrySet_Day:
					GpuSend("CBOF(29,58,100,77,7,4);CBOX(29,58,100,77,7,0);DS12(35,61,' 试用天数:',0);\r\n");
					break;
				case State_UserTrySet_PassWord:
					GpuSend("CBOF(29,83,100,102,7,4);CBOX(29,83,100,102,7,0);DS12(35,86,' 试用密码:',0);\r\n");	
					break;
				case State_UserTrySet_Tel:
					GpuSend("CBOF(29,108,100,127,7,4);CBOX(29,108,100,127,7,0);DS12(35,111,' 解锁电话:',0);\r\n");	
					break;
			}
			break;
		}
		case State_UserPara_SetNewTime_Date:
		case State_UserPara_SetNewTime_Time:
		case State_UserPara_SetNewTime_Week:
		{
			GpuSend("SBC(5);");	
			GpuSend("CBOF(28,60,80,78,7,5);CBOX(28,60,80,78,7,0);CBOF(28,92,80,110,7,5);CBOX(28,92,80,110,7,0);CBOF(28,124,80,142,7,5);CBOX(28,124,80,142,7,0);");
			GpuSend("DS12(36,62,'年月日:',0);")	;
			GpuSend("DS12(38,95,'时 间：',0);")	;	
			GpuSend("DS12(38,127,'星 期：',0);");	
			GpuSend("\r\n;");	
			delay_ms(10);
			sprintf(GpuBuf,"DS16(90,62,'%04d-%02d-%02d',0);",Date_Time[0],Date_Time[1],Date_Time[2]);
			GpuSend(GpuBuf);
			sprintf(GpuBuf,"DS16(90,93,'%02d:%02d:%02d',0);\r\n",Date_Time[4],Date_Time[5],Date_Time[6]);
			GpuSend(GpuBuf);				
			switch(Date_Time[3])
			{
				case 0:
					GpuSend("DS16(90,125,'周日',0);");
					break;
				case 1:
					GpuSend("DS16(90,125,'周一',0);");
					break;
				case 2:
					GpuSend("DS16(90,125,'周二',0);");
					break;
				case 3:
					GpuSend("DS16(90,125,'周三',0);");
					break;
				case 4:
					GpuSend("DS16(90,125,'周四',0);");
					break;
				case 5:
					GpuSend("DS16(90,125,'周五',0);");
					break;
				case 6:
					GpuSend("DS16(90,125,'周六',0);");
					break;
			}
			GpuSend("\r\n;");	
			delay_ms(10);
			GpuSend("SBC(4);");	
			switch(CurState)
			{
				case State_UserPara_SetNewTime_Date:
					GpuSend("CBOF(28,60,80,78,7,4);CBOX(28,60,80,78,7,0);DS12(36,62,'年月日:',0);")	;	
					sprintf(GpuBuf,"SBC(6);DS16(90,62,'%d',0);",Date_Time[0]/(1000));
					GpuSend(GpuBuf);
					Date_Time[7]=0;
					break;				
				case State_UserPara_SetNewTime_Time:
					GpuSend("CBOF(28,92,80,110,7,4);CBOX(28,92,80,110,7,0);DS12(38,95,'时 间：',0);")	;	
					sprintf(GpuBuf,"SBC(6);DS16(90,93,'%d',0);",Date_Time[4]/10);
					GpuSend(GpuBuf);
					Date_Time[7]=0;
					break;
				case State_UserPara_SetNewTime_Week:
					GpuSend("CBOF(28,124,80,142,7,4);CBOX(28,124,80,142,7,0);DS12(38,127,'星 期：',0);SBC(6);");	
					switch(Date_Time[3])
					{
						case 0:
							GpuSend("DS16(90,125,'周日',0);");
							break;
						case 1:
							GpuSend("DS16(90,125,'周一',0);");
							break;
						case 2:
							GpuSend("DS16(90,125,'周二',0);");
							break;
						case 3:
							GpuSend("DS16(90,125,'周三',0);");
							break;
						case 4:
							GpuSend("DS16(90,125,'周四',0);");
							break;
						case 5:
							GpuSend("DS16(90,125,'周五',0);");
							break;
						case 6:
							GpuSend("DS16(90,125,'周六',0);");
							break;
					}
					break;
				default:
					break;
			}
			GpuSend("SBC(5);\r\n");	
			break;
		
			default:
			break;
		}		
	}

}



//彩屏显示字符
void LcdDisp_Char(U8 *Num,U8 Number)
{
	u8 i;
	char GpuBuf[50];
	switch(g_CurrentState)
	{		case State_UserTry_Once_Password100_new:		
				GpuSend("SBC(7);");	
				for(i=0;i<Number;i++)
				{				
					sprintf(GpuBuf,"DS16(%d,65,'%c',0);",130+i*8,Num[i]);
					GpuSend(GpuBuf);
				}
				GpuSend("\r\n");	
				break;
			case State_UserTry_Once_Password100_new1:		
				GpuSend("SBC(7);");	
				for(i=0;i<Number;i++)
				{				
					sprintf(GpuBuf,"DS16(%d,100,'%c',0);",130+i*8,Num[i]);
					GpuSend(GpuBuf);
				}
				GpuSend("\r\n");	
				break;
		
			case State_UserTry_Once_Password101:
			case State_UserTry_Once_Password102:
			case State_UserTry_Once_Password103:
			case State_UserTry_Once_Password104:
			case State_UserTry_Once_Password105:	
			case State_UserTry_Once_Password100:		
				GpuSend("SBC(15);");	
				for(i=0;i<Number;i++)
				{				
					sprintf(GpuBuf,"DS16(%d,83,'%c',0);",130+i*8,Num[i]);
					GpuSend(GpuBuf);
				}
				GpuSend("\r\n");	
				break;
		case State_UserTry_ShowTime:
		case State_UserTry_OnceTime:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,83,'%c',0);",105+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_UserTry_OnceTel:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,80,'%c',0);",105+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_UserTry_OnceEnd:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,71,'%c',0);",105+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_MainMenu_Menu_ScreenSet_Password:
		case State_MainMenu_Menu_ScenarioSet_Password:
			GpuSend("SBC(15);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,83,'%c',0);",152+i*10,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("SBC(7);\r\n");	
			break;
		case State_MatrixMode_ModeSet_output:
				GpuSend("CBOF(126,66,154,89,3,15);SBC(15);");			
				sprintf(GpuBuf,"DS16(137,70,'%c',0);\r\n",Num[0]);
				GpuSend(GpuBuf);
				break;
		case State_MatrixMode_ModeSet_input:
			GpuSend("CBOF(66,66,94,89,3,15);SBC(15);");			
			sprintf(GpuBuf,"DS16(77,70,'%c',0);\r\n",Num[0]);
			GpuSend(GpuBuf);
			break;
		case State_ScenarioSet_NewImage_ImageNum:
			GpuSend("CBOF(146,81,184,99,3,15);SBC(15);");			
			sprintf(GpuBuf,"DS16(158,83,'%c',0);\r\n",Num[0]);
			GpuSend(GpuBuf);
			break;
		case State_ScenarioSet_DelScheme_Num:
		case State_ScenarioSet_SchemeNum:
			GpuSend("CBOF(131,81,159,99,3,15);SBC(15);");			
			sprintf(GpuBuf,"DS16(142,83,'%c',0);\r\n",Num[0]);
			GpuSend(GpuBuf);
			break;	
		case State_DebugMode_Start:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,80,'%c',0);",132+i*10,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_UserTrySet_Day:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,61,'%c',0);",117+i*10,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_UserTrySet_PassWord:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,86,'%c',0);",117+i*10,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_UserTrySet_Tel:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,111,'%c',0);",105+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
		case State_TryTime_End:
			GpuSend("SBC(7);");	
			for(i=0;i<Number;i++)
			{				
				sprintf(GpuBuf,"DS16(%d,111,'%c',0);",125+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
			break;
	}

	
	if(IsSetParaValueState())
	{
		GpuSend("SBC(15);CBOF(131,126,204,149,1,15);");	
			for(i=0;i<Number;i++)                                                                                                                                                                      
			{				
				sprintf(GpuBuf,"DS16(%d,130,'%c',0);",140+i*8,Num[i]);
				GpuSend(GpuBuf);
			}
			GpuSend("\r\n");	
	}
}





void LcdDisp_Num( U8 *Num)
{

 	char ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8,ch9,ch10,ch11,ch12;
	char GpuBuf[500];
	ch1=Num[0]-48;
	ch2=Num[1]-48;
	ch3=Num[2]-48;
	ch4=Num[3]-48;
	ch5=Num[4]-48;
	ch6=Num[5]-48;
	ch7=Num[6]-48;
	ch8=Num[7]-48;
	ch9=Num[8]-48;
	ch10=Num[9]-48;
	ch11=Num[10]-48;
	ch12=Num[11]-48;
//	delay_ms(500);
	
	switch(g_CurrentState)
	{
		case State_WizardSet_OutPort_Num:
		case State_ScreenSet_OutPort_Num:
			sprintf(GpuBuf,"SBC(15);DS16(160,71,'%d',0);\r\n",ch1);
			GpuSend(GpuBuf);
			delay_ms(50);	
			break;
		case State_WizardSet_TwoPort_Mode:
		case State_WizardSet_ThreePort_Mode:
		case State_WizardSet_FourPort_Mode:
		case State_ScreenSet_TwoPort_Mode:
		case State_ScreenSet_ThreePort_Mode:
		case State_ScreenSet_FourPort_Mode:
			sprintf(GpuBuf,"SBC(15);DS16(178,150,'%d',0);\r\n",ch1);
			GpuSend(GpuBuf);
			delay_ms(50);	
			break;
			break;
		case State_ScenarioSet_NewImage_ImagePara_Begin_X:
		{
				if(Num[0]==0&&Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{	;	}
			else if(Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,63,'  %d  ',0);\r\n",ch1);
				GpuSend(GpuBuf);

			}
			else if(Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,63,' %d%d  ',0);\r\n",ch1,ch2);
				GpuSend(GpuBuf);

			}
			else if(Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,63,' %d%d%d ',0);\r\n",ch1,ch2,ch3);
				GpuSend(GpuBuf);
			}
			else if(Num[4]==0)
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,63,' %d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4);
				GpuSend(GpuBuf);
			}
			else
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,63,'%d%d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4,ch5);
				GpuSend(GpuBuf);
			}
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_Begin_Y:
		{
			if(Num[0]==0&&Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{	;	}
			else if(Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,63,'  %d  ',0);\r\n",ch1);
				GpuSend(GpuBuf);

			}
			else if(Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,63,' %d%d  ',0);\r\n",ch1,ch2);
				GpuSend(GpuBuf);

			}
			else if(Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,63,' %d%d%d ',0);\r\n",ch1,ch2,ch3);
				GpuSend(GpuBuf);
			}
			else if(Num[4]==0)
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,63,' %d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4);
				GpuSend(GpuBuf);
			}
			else
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,63,'%d%d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4,ch5);
				GpuSend(GpuBuf);
			}
			break;
		}
		
		case State_ScenarioSet_NewImage_ImagePara_End_X:
		{
			if(Num[0]==0&&Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{	;	}
			else if(Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,93,'  %d  ',0);\r\n",ch1);
				GpuSend(GpuBuf);

			}
			else if(Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,93,' %d%d  ',0);\r\n",ch1,ch2);
				GpuSend(GpuBuf);

			}
			else if(Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,93,' %d%d%d ',0);\r\n",ch1,ch2,ch3);
				GpuSend(GpuBuf);
			}
			else if(Num[4]==0)
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,93,' %d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4);
				GpuSend(GpuBuf);
			}
			else
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(68,93,'%d%d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4,ch5);
				GpuSend(GpuBuf);
			}
			break;
		}
		case State_ScenarioSet_NewImage_ImagePara_End_Y:
		{
			if(Num[0]==0&&Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{	;	}
			else if(Num[1]==0&&Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,93,'  %d  ',0);\r\n",ch1);
				GpuSend(GpuBuf);

			}
			else if(Num[2]==0&&Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,93,' %d%d  ',0);\r\n",ch1,ch2);
				GpuSend(GpuBuf);

			}
			else if(Num[3]==0&&Num[4]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,93,' %d%d%d ',0);\r\n",ch1,ch2,ch3);
				GpuSend(GpuBuf);
			}
			else if(Num[4]==0)
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,93,' %d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4);
				GpuSend(GpuBuf);
			}
			else
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS16(170,93,'%d%d%d%d%d',0);\r\n",ch1,ch2,ch3,ch4,ch5);
				GpuSend(GpuBuf);
			}	
			break;
		}

		default:
		{
			GpuSend("CBOF(61,133,76,163,1,7);CBOF(88,133,104,163,1,7);CBOF(116,133,132,163,1,7);CBOF(144,133,158,163,1,7);\r\n");

			if(Num[0]==0&&Num[1]==0&&Num[2]==0&&Num[3]==0)
			{	;	}
			else if(Num[1]==0&&Num[2]==0&&Num[3]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS24(146,136,'%d',0);\r\n",ch1);
				GpuSend(GpuBuf);
				delay_ms(50);	

			}
			else if(Num[2]==0&&Num[3]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS24(118,136,'%d',0);",ch1);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(146,136,'%d',0);\r\n",ch2);
				GpuSend(GpuBuf);
				delay_ms(50);	

			}
			else if(Num[3]==0)
			{

				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"DS24(90,136,'%d',0);",ch1);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(118,136,'%d',0);",ch2);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(146,136,'%d',0);\r\n",ch3);
				GpuSend(GpuBuf);
				delay_ms(50);	


			}
			else
			{
				GpuSend("SBC(7);");	
				sprintf(GpuBuf,"SBC(7);DS24(62,136,'%d',0);",ch1);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(90,136,'%d',0);",ch2);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(118,136,'%d',0);",ch3);
				GpuSend(GpuBuf);
				sprintf(GpuBuf,"DS24(146,136,'%d',0);\r\n",ch4);
				GpuSend(GpuBuf);
				delay_ms(50);	
			}
			break;
		}
	}
}




void Color_VersionInfo(void)
{

		char GpuBuf[500];
		U32 wValue;
	  U8 ch[11];
		wValue=ChipUniqueID%1000000;

		//暂时显示用，后更新为实际TB3531的值
		ch[0]='V';
		ch[1]=1+48;
		ch[2]='.';
		ch[3]=0+48;
		ch[4]='.';
		ch[5]=0+48;
		ch[6]='-';
		ch[7]='\0';

		GpuSend("CBOF(0,0,220,176,1,5);CBOF(0,30,220,145,1,2);");

		GpuSend("SBC(2);");
		GpuSend("DS16(23,46,'机器型号:',0);");
		sprintf(GpuBuf,"DS16(106,48,'%s',0);",g_MachineTypeXPStr);
		GpuSend(GpuBuf);

		GpuSend("DS16(23,79,'机器ID号:',0);");
		sprintf(GpuBuf,"DS16(106,79,'%d',0);",wValue);
		GpuSend(GpuBuf);

		GpuSend("DS16(23,111,'软件版本:  ',0);");
		sprintf(GpuBuf,"DS16(106,111,'%s%s',0);",ch,g_ReleaseStr);
		GpuSend(GpuBuf);
		GpuSend("SBC(5);");	
		GpuSend("DS16(199,154,'P2',1);");

		GpuSend("SBC(7);");		
		GpuSend("\r\n");

}



void Color_OperationGuide(void)
{
		GpuSend("CBOF(74,10,138,26,1,5);");
		GpuSend("SBC(5);");
		GpuSend("DS24(74,10,'操作指南',0);");
		GpuSend("SBC(7);");
		GpuSend("\r\n");	
}



void Color_OperationGuide1(void)
{
		GpuSend("CBOF(74,10,138,26,1,5);");
		GpuSend("SBC(5);");
		GpuSend("DS24(74,10,'操作指南',0);");
		GpuSend("SBC(7);");
		GpuSend("\r\n");	
}


void Display_Wait(void)
{
	GpuSend("SBC(7);DS16(74,106,'请稍等...',0);\r\n");

}
void Display_SaveFail(void)
{
		GpuSend("SBC(7);DS16(74,106,'保存失败',0);\r\n");
		delay(1);
}
void Display_Saveok(void)
{
		GpuSend("SBC(7);DS16(74,106,'保存成功',0);\r\n");
		delay(1);
}


