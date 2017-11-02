/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: SendMsg.c
 [Date]:        10-May-2012
 [Comment]:
   SendMsg  file.
 [Reversion History]:
*******************************************************************************/

#define _SENDMSG_C_
#include "header.h"
#include <stdio.h>
static unsigned char g_LoadModeFlag=0;
extern U8 g_RevUart2Data[1024];
extern U16 g_RevUart2Idx;
extern U8 g_RevUart2Data_in[20];
extern int gAD7441_InputPort;



U8 G_SendBuf_addscenario[50]=
{ 0xAA,0xBB,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0};

U8 G_SendBuf[20]={0xAA,0xBB,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//串口命令BUF，协议见"XP520内部协议.doc"



void ClearRevUart2Data()
{
	U8 i=0;
	for(i=0;i<20;i++)
	{
		g_RevUart2Data[i]=0;
		g_RevUart2Data_in[i]=0;
		
	}
}


void CopyRevUart2Data_in(U8 *buf)
{
	U8 i=0;
	for(i=0;i<g_RevUart2Idx;i++)
	{
		buf[i]=g_RevUart2Data_in[i];
	}
}




BOOL CheckProtocolStart(U8 * pBuf)//检查协议头是否为0xAA BB
{
	if(pBuf[0]==0xAA&&pBuf[1]==0xBB)
		return __TRUE;
	else 
		return __FALSE;
}




BOOL CheckSum(U8 * pBuf)//检查pBuf的校验和(BUF最后一位)
{
	U8 i,sum=0;
	for(i=3; i<3+pBuf[MESSAGELEN_IDX]; i++)
	{
		sum+=pBuf[i];
	}
	if(sum==pBuf[i])
		return __TRUE;
	else 
		return __FALSE;
}



void Uart2Send(U8 * pBuf)//????
{
	U8 sum = 0;
	U8 i;
	//g_OrderMFlag=__FALSE;
	for(i=3; i<3+pBuf[MESSAGELEN_IDX]; i++)
	{
		sum+=pBuf[i];
	}
	pBuf[i]=sum;

	
	UART2_SendBuf(pBuf, i+1);

}


U8 DealFactoryReset()
{

}


/***************************
函数功能：调用存储的场景 
入口参数：要调用的场景值，把KeyVal的按键值传入。
****************************/
extern U8 g_MESSAGE_FLAG;
extern U8  first_display;

void LoadScenario(_KeypadNum loadnum)
{
}


/**********************************************
函数功能：TAKE场景 把 VGA使用的场景发送到DVI上
入口参数：无
返回参数：0 成功， 其它：失败
**********************************************/

U8 Take_ScenarioToDVI( )
{

}



/**********************************************
函数功能：场景切换
入口参数：切换DVI或VGA　　场景值
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 ScenarioSwitch(U8 DviOrVga,  U8  ScenarioID )
{
	
}

/**********************************************
函数功能：设置LED屏幕参数
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 Set_ScreenPara(void)
{


}

/**********************************************
函数功能：设置DVI和VGA的输出分辨率
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 Set_Resolution(void)
{
	
}
/**********************************************
函数功能：设置DVI和VGA的输出刷新率
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 Set_FramRate()
{

}

/**********************************************
函数功能：音量设置
入口参数：value表示值
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 Volume_Set(U8 value)
{
	
}

/**********************************************
函数功能：设置图像质量
入口参数：Sub_ID 表示图像质量子组(对比度，饱和度，亮度)，value表示值
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 Image_Quality_Set(U8 Sub_ID,U8 value)
{
	
}

/**********************************************
函数功能：新增场景（覆盖修改）
发送数据格式：请参数TB3531通信协议
完成时间：2016.7.26
完成人：lyg
**********************************************/
U8 ADD_New_Scenario(void)
{
	
}



