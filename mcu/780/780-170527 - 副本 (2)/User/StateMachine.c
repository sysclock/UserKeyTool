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


BOOL ResetPara_Total()//恢复出厂函数
{
	u8 i ;
	if(reset())
	{
			g_Current_Scene= DEF_CURRENT_SCENE;
			
			Uart_Send_Cmd_to_FPGA("GP SIPA 192 168 1 100\r\n");
			delay_ms(100);
			Uart_Send_Cmd_to_FPGA("GP STPN 8978\r\n");
			delay_ms(100);
			Uart_Send_Cmd_to_FPGA("GP SCBR 1 0x01 0xC200\r\n");//在软件操作的log记录中找不到
			delay_ms(100);
			
			for(i=0;i<10;i++)
			{
				Clear_Display(i);					
			}
			gScrRow=0;
			gScrCol=0;
			//回复十分钟自动锁键盘
			g_LockKeypadTime=0;
			Write_EEPROM_Byte(E2ROM_LOCKTIME_BEGIN_ADDR,g_LockKeypadTime);	
			return __TRUE;
		
	}
	return __FALSE;
}




void SetCurrentNewTime();

BOOL SetCurrentStateValue(EventValue Val,State currentState,U16 wValue)
{
	U8 led_no;
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
			SetCurrentTime();//****************************************************************WAITING
			
			break;
		case State_FactoryReset_OK:


			GpuSend("SBC(7);CBOF(18,60,200,130,0,7);DS16(18,82,' 正在进行，请稍等...',0);\r\n");
			delay_ms(40);

			delay(1);
			chinesedisplay(0x90, g_waitStr);
			if(ResetPara_Total())
			{
					for(led_no=0;led_no<KEY_NUM;led_no++)
					{
						SetKeyLEDLight((_KeypadNum)led_no);
						delay1000(1000);		
					}	
			}		
			g_CurrentState=State_MainMenu_Menu;
			DealEscKey();
			return __FALSE;
			break;
		default:
			return __FALSE;
		}
	}

/**************************************************************************************************/
/**************************************************************************************************/

	else if((Val==KEY_UP)||(Val==KEY_DOWN))
	{
		switch(currentState)
		{

		#ifdef TCP_IP
			case State_UserPara_SetStaticIP_Value:
				SetStaticIPAddress(wValue);
				break;
		#endif
			//----------------------------set up date and time------------------------------------------
		default:
			return __FALSE;
		}
	}	
	return __TRUE;
}


