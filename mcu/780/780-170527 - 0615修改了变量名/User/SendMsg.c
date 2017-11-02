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

U8 G_SendBuf[20]={0xAA,0xBB,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//��������BUF��Э���"XP520�ڲ�Э��.doc"



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




BOOL CheckProtocolStart(U8 * pBuf)//���Э��ͷ�Ƿ�Ϊ0xAA BB
{
	if(pBuf[0]==0xAA&&pBuf[1]==0xBB)
		return __TRUE;
	else 
		return __FALSE;
}




BOOL CheckSum(U8 * pBuf)//���pBuf��У���(BUF���һλ)
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
�������ܣ����ô洢�ĳ��� 
��ڲ�����Ҫ���õĳ���ֵ����KeyVal�İ���ֵ���롣
****************************/
extern U8 g_MESSAGE_FLAG;
extern U8  first_display;

void LoadScenario(_KeypadNum loadnum)
{
}


/**********************************************
�������ܣ�TAKE���� �� VGAʹ�õĳ������͵�DVI��
��ڲ�������
���ز�����0 �ɹ��� ������ʧ��
**********************************************/

U8 Take_ScenarioToDVI( )
{

}



/**********************************************
�������ܣ������л�
��ڲ������л�DVI��VGA��������ֵ
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 ScenarioSwitch(U8 DviOrVga,  U8  ScenarioID )
{
	
}

/**********************************************
�������ܣ�����LED��Ļ����
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 Set_ScreenPara(void)
{


}

/**********************************************
�������ܣ�����DVI��VGA������ֱ���
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 Set_Resolution(void)
{
	
}
/**********************************************
�������ܣ�����DVI��VGA�����ˢ����
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 Set_FramRate()
{

}

/**********************************************
�������ܣ���������
��ڲ�����value��ʾֵ
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 Volume_Set(U8 value)
{
	
}

/**********************************************
�������ܣ�����ͼ������
��ڲ�����Sub_ID ��ʾͼ����������(�Աȶȣ����Ͷȣ�����)��value��ʾֵ
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 Image_Quality_Set(U8 Sub_ID,U8 value)
{
	
}

/**********************************************
�������ܣ����������������޸ģ�
�������ݸ�ʽ�������TB3531ͨ��Э��
���ʱ�䣺2016.7.26
����ˣ�lyg
**********************************************/
U8 ADD_New_Scenario(void)
{
	
}



