/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: SendMsg.c
 [Date]:        10-May-2012
 [Comment]:
   SendMsg  file.
 [Reversion History]:
 	uart3 comm with ms182 input card
 u8	
*******************************************************************************/

#define _SENDMSG_C_
#include "header.h"
#include <stdio.h>
static unsigned char g_LoadModeFlag=0;
extern U8 g_RevUart2Data[12];
extern U8 g_RevUartIdx;

extern U8 g_RevUart2Data_in[12];

extern U8 g_RevUart3Data[13];
extern U8 g_RevUart3Idx;
extern U8 g_RevUart3Data_in[12];
extern int gAD7441_InputPort;

//extern struct VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStat;

U8 G_SendBuf[CMDLENTH]={0xAA,0xBB,0x02,0x01,0xFF,0x55,0x55,0x55,0x55,0x55,0x55,0x65};

BOOL WaitUartACK(U32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0)
		{
			if(g_ReceMsgFlag)
			{
				g_ReceMsgFlag=__FALSE;
				return __TRUE;
			}
		}
	}
	return __FALSE;
}


void ClearRevUart2Data()
{
	U8 i=0;
	for(i=0;i<CMDLENTH;i++)
	{
		g_RevUart2Data_in[i]=0;
		g_RevUart2Data[i]=0;
		
	}
}

void ClearRevUart3Data()
{
	U8 i=0;
	for(i=0;i<CMDLENTH;i++)
	{
		g_RevUart3Data_in[i]=0;
		g_RevUart3Data[i]=0;
		
	}
}
void CopyRevUart2Data(U8 *buf)
{
	U8 i=0;
	for(i=0;i<CMDLENTH;i++)
	{
		buf[i]=g_RevUart2Data_in[i];
	}
}

void CopyRevUart3Data(U8 *buf)
{
	U8 i=0;
	for(i=0;i<CMDLENTH;i++)
	{
		buf[i]=g_RevUart3Data_in[i];
	}
}

BOOL CheckProtocolStart(U8 * pBuf)
{
	if(pBuf[0]==0xAA&&pBuf[1]==0xBB)
		return __TRUE;
	else 
		return __FALSE;
}

BOOL CheckSum(U8 * pBuf)
{
	U8 i,sum=0;
	for(i=2;i<CMDLENTH-1;i++)
		sum+=pBuf[i];
	if(sum==pBuf[CMDLENTH-1])
		return __TRUE;
	else 
		return __FALSE;
}

void CheckSum_Send(U8 * pBuf)
{
	U8 sum = 0;
	U8 i;
	for(i=2; i<CMDLENTH-1; i++)
	{
		sum+=pBuf[i];
	}
	pBuf[CMDLENTH-1]=sum;
	
	UART2_SendBuf(pBuf, CMDLENTH);	
}

void CheckSum_Send3(U8 * pBuf)
{
	U8 sum = 0;
	U8 i;
	for(i=2; i<CMDLENTH-1; i++)
	{
		sum+=pBuf[i];
	}
	pBuf[CMDLENTH-1]=sum;
	
	UART3_SendBuf(pBuf, CMDLENTH);	
}


/*************************************************
Function: 		GetUsbState
Description: 		通过串口得到USB播放源下的状态
Calls:
Called By: 		函数暂未使用
Input: 			无输入参数
Output: 			无输出参数
Return:			返回布尔型变量，用来判断是否正确得到串口信息
Others:			lyg 20160628添加
*************************************************/
BOOL GetUsbState(void)
{
	U8 Buf[CMDLENTH]={0};//串口命令长度的BUF数组，初始化为0，用于存放从串口读到的命令
	
	G_SendBuf[EVENTID_IDX]=USB_STATE_EVENT;	//协议功能
	G_SendBuf[PARA1_IDX]=GET_USB_STATE;		//参数1
	g_ReceMsgFlag=__FALSE;					//是否已接受到32626返回的信息标志 置 否
	
	CheckSum_Send3(G_SendBuf);//计算校验和并发送buf
	WaitUartACK(5000);//等待串口应答
	CopyRevUart3Data(Buf);//将串口读到的内容拷贝至BUF数组	

	if( !CheckProtocolStart(Buf) )//判断协议头
		return __FALSE;
	if( !CheckSum(Buf) )//判断校验和
		return __FALSE;

	if( Buf[EVENTID_IDX] == USB_STATE_EVENT )//协议功能为USB状态查询功能时，根据参数1的值，分别置USB状态值
	{
		if(Buf[PARA1_IDX] == USB_STATE_MENU)
		{
			g_UsbState=USBSTATE_MENU;
		}
		else if(Buf[PARA1_IDX]==USB_STATE_PLAY)
		{
			g_UsbState=USBSTATE_PALY;
		}
		ClearRevUart3Data();//清除口数据接收数组
		return __TRUE;//返回正确
	}
	else//协议功能不对，则直接返回错误
		return __FALSE;
}


void DealKey2Usb(_KeypadNum keyVale)
{
	GetUsbState();
	//if(g_UsbState==USBSTATE_MENU)
	//{
		G_SendBuf[EVENTID_IDX]=USB_DIRECT_EVENT;
		switch(keyVale)
		{
			case UP_KEY:
				G_SendBuf[PARA1_IDX]=USB_UP_PARA1; 
				break;
			case DOWN_KEY:
				G_SendBuf[PARA1_IDX]=USB_DOWN_PARA1;  
				break;
			case OK_KEY:
				G_SendBuf[PARA1_IDX]=USB_OK_PARA1;  
				break;
			case ESC_KEY:
				G_SendBuf[PARA1_IDX]=USB_BACK_PARA1;  
				break;
			default:
				break;
		}		
	CheckSum_Send3(G_SendBuf);
}


void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub)
{
	if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		MainOrSub=MAIN;
		G_SendBuf[EVENTID_IDX]=MAIN_SRC_EVENT;
		if(inputsrc !=INPUTSOURCE_CVBS4)
			g_MainInput=inputsrc;
	}
	else if(MainOrSub==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SRC_EVENT;
		
		if(inputsrc !=INPUTSOURCE_CVBS4)
			g_SubInput=inputsrc;
	}
	
	
	switch(inputsrc)
	{
		case INPUTSOURCE_CVBS:   
			G_SendBuf[PARA1_IDX]=AV2PARA1;
			break;
		case INPUTSOURCE_CVBS2:
			G_SendBuf[PARA1_IDX]=AV1PARA1;  
			break;
		case INPUTSOURCE_CVBS3:
			G_SendBuf[PARA1_IDX]=AV3PARA1;  
			break;	
		case INPUTSOURCE_CVBS4:
			G_SendBuf[PARA1_IDX]=AV4PARA1;  //实际并没有此输入，用来做演示或者单次使用结束  lyg
			break;
		case INPUTSOURCE_VGA:
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_VGA2:
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_DVI:
			G_SendBuf[PARA1_IDX]=DVIPARA1;  
			break;
		case INPUTSOURCE_DVI2:
			G_SendBuf[PARA1_IDX]=DVI2PARA1;  
			break;
		case INPUTSOURCE_HDMI:
			G_SendBuf[PARA1_IDX]=HDMIPARA1;  
			break;
		case INPUTSOURCE_YPBPR:
			G_SendBuf[PARA1_IDX]=YPBPRPARA1;  
			break;
		case INPUTSOURCE_SDI:			
	#if(MATHINE_TYPE==XP_530)
			if(gAD7441_InputPort!=g_Adv7441CurrentInput)
			{
				Device_adv7441GetVideoStatus();
			}
	#endif
			G_SendBuf[PARA1_IDX]=SDIPARA1;  
			break;
		case INPUTSOURCE_USB:
			G_SendBuf[PARA1_IDX]=USBPARA1;  
			break;
		case INPUTSOURCE_DP:
			G_SendBuf[PARA1_IDX]=DPPARA1;  
			break;
	}
	
	switch(inputsrc)
	{
		case INPUTSOURCE_CVBS:   
		case INPUTSOURCE_CVBS2:
		case INPUTSOURCE_VGA:
		case INPUTSOURCE_DVI:	
		case INPUTSOURCE_HDMI:	
		case INPUTSOURCE_YPBPR:
		case INPUTSOURCE_SDI:		
		case INPUTSOURCE_DP:
			CheckSum_Send(G_SendBuf);
// 			CheckSum_Send4(G_SendBuf);
			break;
		
		case INPUTSOURCE_CVBS3:
		case INPUTSOURCE_VGA2:
		case INPUTSOURCE_DVI2:
		case INPUTSOURCE_USB:
		{
			G_SendBuf[PARA1_IDX]=SDIPARA1;  				
			CheckSum_Send(G_SendBuf);			
			switch(inputsrc)
			{
				case INPUTSOURCE_CVBS3:
					G_SendBuf[PARA1_IDX]=AV3PARA1;  
					break;
			
				case INPUTSOURCE_VGA2:
					G_SendBuf[PARA1_IDX]=VGAPARA1;  
					break;			
				case INPUTSOURCE_DVI2:
					G_SendBuf[PARA1_IDX]=DVI2PARA1;  
					break;			
				case INPUTSOURCE_USB:
					G_SendBuf[PARA1_IDX]=USBPARA1;  
					break;
			}
			G_SendBuf[EVENTID_IDX]=MAIN_SRC_EVENT;
			CheckSum_Send3(G_SendBuf);//to ms182
			break;
		} 			
		
		case INPUTSOURCE_CVBS4:
		{
			if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
			{
				if(g_MainInput_pre==INPUTSOURCE_CVBS || g_MainInput_pre==INPUTSOURCE_CVBS2 ||g_MainInput_pre==INPUTSOURCE_VGA ||g_MainInput_pre==INPUTSOURCE_DVI ||
					 g_MainInput_pre==INPUTSOURCE_HDMI || g_MainInput_pre==INPUTSOURCE_YPBPR ||g_MainInput_pre==INPUTSOURCE_SDI ||g_MainInput_pre==INPUTSOURCE_DP)
				{
					CheckSum_Send(G_SendBuf);
				}
				else if(g_MainInput_pre==INPUTSOURCE_CVBS3 || g_MainInput_pre==INPUTSOURCE_VGA2 ||g_MainInput_pre==INPUTSOURCE_DVI2 ||g_MainInput_pre==INPUTSOURCE_USB )
				{
					G_SendBuf[PARA1_IDX]=AV1PARA1; 
					CheckSum_Send3(G_SendBuf);
				}
			}
			else if(MainOrSub==SUB)
			{
				if(g_SubInput_pre==INPUTSOURCE_CVBS || g_SubInput_pre==INPUTSOURCE_CVBS2 ||g_SubInput_pre==INPUTSOURCE_VGA ||g_SubInput_pre==INPUTSOURCE_DVI ||
					 g_SubInput_pre==INPUTSOURCE_HDMI || g_SubInput_pre==INPUTSOURCE_YPBPR ||g_SubInput_pre==INPUTSOURCE_SDI ||g_SubInput_pre==INPUTSOURCE_DP)
				{
					CheckSum_Send(G_SendBuf);
				}
				else if(g_SubInput_pre==INPUTSOURCE_CVBS3 || g_SubInput_pre==INPUTSOURCE_VGA2 ||g_SubInput_pre==INPUTSOURCE_DVI2 ||g_SubInput_pre==INPUTSOURCE_USB )
				{
					G_SendBuf[PARA1_IDX]=SDIPARA1;  				
					CheckSum_Send(G_SendBuf);	
					G_SendBuf[PARA1_IDX]=AV1PARA1; 
					G_SendBuf[EVENTID_IDX]=MAIN_SRC_EVENT;
					CheckSum_Send3(G_SendBuf);
				}
			}
					
			break;
		}
		
	}
	
	switch(inputsrc)
	{
		case INPUTSOURCE_CVBS:
		case INPUTSOURCE_CVBS2:
		case INPUTSOURCE_CVBS3:
		case INPUTSOURCE_CVBS4:
		case INPUTSOURCE_YPBPR:
			if(g_OtherFullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_VGA:
			if(g_VGAFullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_DVI:
			if(g_DVIFullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_HDMI:
			if(g_HDMIFullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_VGA2:
			if(g_VGA2FullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_DVI2:
			if(g_DVI2FullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
	}
}




void SetWhichImageOnTop(U8 MainOrSUb)
{
	G_SendBuf[EVENTID_IDX]=IMAGE_ONTOP_EVENT;
	G_SendBuf[PARA1_IDX]=MainOrSUb;
	CheckSum_Send(G_SendBuf);		
}

void SetMultiWindowPara(U8 para,U8 alpha)
{	
	G_SendBuf[EVENTID_IDX]=MULTI_EVENT;
	G_SendBuf[PARA1_IDX]=para;
	if(para==FUSEPIPPARA1)
	{		
		G_SendBuf[PARA2_IDX]=alpha;
	}
	CheckSum_Send(G_SendBuf);	
	if(para==SAMEPIPIMAGE)
	{
		g_SubScalerHSize=g_MainScalerHSize;
		g_SubScalerVSize=g_MainScalerVSize;
		g_SubScalerHPos=g_MainScalerHPos;
		g_SubScalerVPos=g_MainScalerVPos;
	}
	if((para==PIPIMAGE||para==PBPIMAGE)&&g_NextPointImage==SUB)
	{
		g_NextPointImage=MAIN;
		SetWhichImageOnTop(SUB);
	}
	if(para==SAMEPIPIMAGE&&	g_LoadModeFlag==0)   //7-8
	{
		g_NextPointImage=SUB;
	}
	
}





void SwapDualInputSrc()
{
	G_SendBuf[EVENTID_IDX]=SWAP_EVENT;
	G_SendBuf[PARA1_IDX]=g_SwitchEffectValue;
	G_SendBuf[PARA2_IDX]=g_FadeTime;
	CheckSum_Send(G_SendBuf);	
}

void SetFreezeImage(U8 MainOrSUb,U8 para)
{	
	G_SendBuf[EVENTID_IDX]=MENU_LOGO_EVENT;
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		G_SendBuf[PARA1_IDX]=0x08;
	else if(MainOrSUb==SUB)
		G_SendBuf[PARA1_IDX]=0x0A;
	G_SendBuf[PARA2_IDX]=para;
	CheckSum_Send(G_SendBuf);	
}




/*
void SetZoomValue(U8 para,U16 wValue,U8 MainOrSUb)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_ZOOM_EVENT;
	}
	G_SendBuf[PARA1_IDX]=para;
	G_SendBuf[PARA2_IDX]=(wValue>>8);
	G_SendBuf[PARA3_IDX]=(wValue&0xff);
	CheckSum_Send(G_SendBuf);	
}*/



//it must be change name; current is sililar with SetZoomHVValue
void SetZoomValueHV(U8 para,U16 wValue1,U16 wValue2,U8 MainOrSUb)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_ZOOM_EVENT;
	}
	G_SendBuf[PARA1_IDX]=para;
	G_SendBuf[PARA2_IDX]=(wValue1>>8);
	G_SendBuf[PARA3_IDX]=(wValue1&0xff);
	G_SendBuf[PARA4_IDX]=(wValue2>>8);
	G_SendBuf[PARA5_IDX]=(wValue2&0xff);
	//UART SEND CMD TO MAIN DSP
	CheckSum_Send(G_SendBuf);	
	#ifdef DEBUG_ZOOM_YJL
		printf("SendMsg.c L503 SetZoomValueHV ZoomH=%d,ZoomV=%d \r\n",wValue1,wValue2);
	#endif
}





void SetFullZoomHVValue(U8 para,U8 MainOrSUb)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_ZOOM_EVENT;
	}
	G_SendBuf[PARA1_IDX]=para;
	G_SendBuf[PARA2_IDX]=0;
	G_SendBuf[PARA3_IDX]=0;
	G_SendBuf[PARA4_IDX]=0;
	G_SendBuf[PARA5_IDX]=0;
	CheckSum_Send(G_SendBuf);	
}


//only echo Part/Full key;set zoom donnt uses this function
void SetZoomHVValue(U8 para,U8 MainOrSUb)
{	
	G_SendBuf[PARA1_IDX]=para;
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
		if(para==HVSIZE)
		{
			if(g_MainInput==INPUTSOURCE_VGA)
			{
			G_SendBuf[PARA2_IDX]=(g_MainVGAZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainVGAZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainVGAZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainVGAZoomVSize&0xff);
			}
			else if(g_MainInput==INPUTSOURCE_DVI)
			{
			G_SendBuf[PARA2_IDX]=(g_MainDVIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainDVIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainDVIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainDVIZoomVSize&0xff);
			}
			else if(g_MainInput==INPUTSOURCE_HDMI)
			{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVSize&0xff);
			}
			else
			{
			G_SendBuf[PARA2_IDX]=(g_MainZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainZoomVSize&0xff);
			}
		}
		else if(para==HVPOS)
		{
			if(g_MainInput==INPUTSOURCE_VGA)
			{
			G_SendBuf[PARA2_IDX]=(g_MainVGAZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainVGAZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainVGAZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainVGAZoomVPos&0xff);
			}
			else if(g_MainInput==INPUTSOURCE_DVI)
			{
			G_SendBuf[PARA2_IDX]=(g_MainDVIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainDVIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainDVIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainDVIZoomVPos&0xff);
			}
			else if(g_MainInput==INPUTSOURCE_HDMI)
			{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVPos&0xff);
			}
			else
			{
			G_SendBuf[PARA2_IDX]=(g_MainZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainZoomVPos&0xff);
			}
		}
		else if(para==VGA_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_MainVGAZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainVGAZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainVGAZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainVGAZoomVSize&0xff);
		}
		else if(para==VGA_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_MainVGAZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainVGAZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainVGAZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainVGAZoomVPos&0xff);
		}
		else if(para==DVI_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_MainDVIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainDVIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainDVIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainDVIZoomVSize&0xff);
		}
		else if(para==DVI_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_MainDVIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainDVIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainDVIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainDVIZoomVPos&0xff);
		}
		else if(para==HDMI_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVSize&0xff);
		}
		else if(para==HDMI_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVPos&0xff);
		}
		else if(para==OTHER_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_MainZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainZoomVSize&0xff);
		}
		else if(para==OTHER_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_MainZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainZoomVPos&0xff);
		}
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_ZOOM_EVENT;
		if(para==HVSIZE)
		{
			if(g_SubInput==INPUTSOURCE_VGA)
			{
			G_SendBuf[PARA2_IDX]=(g_SubVGAZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubVGAZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubVGAZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubVGAZoomVSize&0xff);
			}
			else if(g_SubInput==INPUTSOURCE_DVI)
			{
			G_SendBuf[PARA2_IDX]=(g_SubDVIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubDVIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubDVIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubDVIZoomVSize&0xff);
			}
			else if(g_SubInput==INPUTSOURCE_HDMI)
			{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVSize&0xff);
			}
			else
			{
			G_SendBuf[PARA2_IDX]=(g_SubZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubZoomVSize&0xff);
			}
		}
		else if(para==HVPOS)
		{
			if(g_SubInput==INPUTSOURCE_VGA)
			{
			G_SendBuf[PARA2_IDX]=(g_SubVGAZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubVGAZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubVGAZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubVGAZoomVPos&0xff);
			}
			else if(g_SubInput==INPUTSOURCE_DVI)
			{
			G_SendBuf[PARA2_IDX]=(g_SubDVIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubDVIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubDVIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubDVIZoomVPos&0xff);
			}
			else if(g_SubInput==INPUTSOURCE_HDMI)
			{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVPos&0xff);
			}
			else
			{
			G_SendBuf[PARA2_IDX]=(g_SubZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubZoomVPos&0xff);
			}
		}
		else if(para==VGA_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_SubVGAZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubVGAZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubVGAZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubVGAZoomVSize&0xff);
		}
		else if(para==VGA_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_SubVGAZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubVGAZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubVGAZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubVGAZoomVPos&0xff);
		}
		else if(para==DVI_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_SubDVIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubDVIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubDVIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubDVIZoomVSize&0xff);
		}
		else if(para==DVI_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_SubDVIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubDVIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubDVIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubDVIZoomVPos&0xff);
		}
		else if(para==HDMI_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVSize&0xff);
		}
		else if(para==HDMI_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVPos&0xff);
		}
		else if(para==OTHER_HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_SubZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubZoomVSize&0xff);
		}
		else if(para==OTHER_HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_SubZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubZoomVPos&0xff);
		}
	}
	
	CheckSum_Send(G_SendBuf);	
	#ifdef DEBUG_ZOOM_YJL
		//printf("g_FullOrPartValue=%d\r\n",g_FullOrPartValue);
		printf("SendMsg.v L775 SetZoomHVValue n=%d g_MainVGAZoomHSize=%d,g_MainVGAZoomVSize=%d\r\n",g_debug_zoom++,g_MainVGAZoomHSize,g_MainVGAZoomVSize);
	#endif
	
		
}

/*
void SetScalerValue(U8 para,U16 wValue,U16 wVaule2,U8 MainOrSUb)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_SCALER_EVENT;
		switch(para)
		{
			case HSIZE:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
				break;
			case VSIZE:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
				break;
			case HPOS:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
				break;
			case VPOS:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
				break;
			case HVSIZE:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
				break;
			case HVPOS:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
				break;
		}
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SCALER_EVENT;
	}
	G_SendBuf[PARA1_IDX]=para;
	G_SendBuf[PARA2_IDX]=(wValue>>8);
	G_SendBuf[PARA3_IDX]=(wValue&0xff);
	if(para==HVSIZE||para==HVPOS)
	{
		G_SendBuf[PARA4_IDX]=(wVaule2>>8);
		G_SendBuf[PARA5_IDX]=(wVaule2&0xff);
	}
	CheckSum_Send(G_SendBuf);	
}
*/
void SetScalerHVValue(U8 para,U8 MainOrSUb)
{	
	G_SendBuf[PARA1_IDX]=para;
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_SCALER_EVENT;
	/*	switch(para)
		{
			case HVSIZE:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
				break;
			case HVPOS:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
				break;
		}*/
		if(para==HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_MainScalerHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainScalerHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainScalerVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainScalerVSize&0xff);
		}
		else if(para==HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_MainScalerHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainScalerHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainScalerVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainScalerVPos&0xff);
		}
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SCALER_EVENT;
		if(para==HVSIZE)
		{
			G_SendBuf[PARA2_IDX]=(g_SubScalerHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubScalerHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubScalerVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubScalerVSize&0xff);
		}
		else if(para==HVPOS)
		{
			G_SendBuf[PARA2_IDX]=(g_SubScalerHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubScalerHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubScalerVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubScalerVPos&0xff);
		}
	}
	CheckSum_Send(G_SendBuf);	
	
}

void SetPanelScalerHVValue(U8 para,U8 MainOrSUb)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_SCALER_EVENT;
	}
	else if(MainOrSUb==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SCALER_EVENT;
	}
	G_SendBuf[PARA1_IDX]=para;
	if(para==HVSIZE)
	{
	G_SendBuf[PARA2_IDX]=(g_PanelWidth>>8);
	G_SendBuf[PARA3_IDX]=(g_PanelWidth&0xff);
	G_SendBuf[PARA4_IDX]=(g_PanelHeight>>8);
	G_SendBuf[PARA5_IDX]=(g_PanelHeight&0xff);
	}
	else if(para==HVPOS)
	{
	G_SendBuf[PARA2_IDX]=0;
	G_SendBuf[PARA3_IDX]=0;
	G_SendBuf[PARA4_IDX]=0;
	G_SendBuf[PARA5_IDX]=0;
	}
	CheckSum_Send(G_SendBuf);	
}


void SendColorValue(U8 Event_ID,U8 para1,U8 para2)
{
	G_SendBuf[EVENTID_IDX]=Event_ID;
	G_SendBuf[PARA1_IDX]=para1;
	G_SendBuf[PARA2_IDX]=para2;
	CheckSum_Send(G_SendBuf);	
	CheckSum_Send3(G_SendBuf);	
}

void SendAdcCalibration(U8 Event_ID,U8 para1,U8 para2,U8 para3)
{
	G_SendBuf[EVENTID_IDX]=Event_ID;
	G_SendBuf[PARA1_IDX]=para1;
	G_SendBuf[PARA2_IDX]=para2;
	G_SendBuf[PARA3_IDX]=para3;
	CheckSum_Send(G_SendBuf);	
}

void SetOutputFormat(U8 nIndex)
{	
	LoadOutputResolution();	
	G_SendBuf[EVENTID_IDX]=OUT_FORMAT_EVENT;
	G_SendBuf[PARA1_IDX]=nIndex;
	if(nIndex==0)
	{
		G_SendBuf[PARA2_IDX]=(UserResolutionHSize>>8);
		G_SendBuf[PARA3_IDX]=(UserResolutionHSize&0xff);
		G_SendBuf[PARA4_IDX]=(UserResolutionVSize>>8);
		G_SendBuf[PARA5_IDX]=(UserResolutionVSize&0xff);
	}
	CheckSum_Send(G_SendBuf);	
	G_SendBuf[PARA1_IDX]=0x03;;	
	CheckSum_Send3(G_SendBuf);	
}

void SetVolumeValue(U8 VolVaule)
{	
	
	G_SendBuf[EVENTID_IDX]=AUDIO_EVENT;
	G_SendBuf[PARA1_IDX]=1;
	G_SendBuf[PARA2_IDX]=VolVaule;		
	CheckSum_Send(G_SendBuf);	
}

void SetAudioPara(U8 para)
{
	G_SendBuf[EVENTID_IDX]=AUDIO_EVENT;
	switch(para)
	{
		case MUTE:
			G_SendBuf[PARA1_IDX]=1;
			G_SendBuf[PARA2_IDX]=0;
			break;
		case MAINVOL:
			G_SendBuf[PARA1_IDX]=2;
			G_SendBuf[PARA2_IDX]=0;
			break;
		case SUBVOL:
			G_SendBuf[PARA1_IDX]=2;
			G_SendBuf[PARA2_IDX]=1;
			break;
	}
	CheckSum_Send(G_SendBuf);	
}
/*
void Load2048X1152ScreenPara()
{
	if(g_OutputFormatIndex==INDEX2048x1152)
	{
		SetOutputFormat(g_OutputFormatIndex);
		if(g_MainScalerHSize!=2048||g_MainScalerVSize!=1152)
		{
			SetScalerHVValue(HVSIZE,MAIN);
		}
		if(g_MainScalerHPos!=0||g_MainScalerVPos!=0)
			SetScalerHVValue(HVPOS,MAIN);
	}
}
*/

void LoadModeSet(void)
{
	
	U16 HSize=0,VSize=0;	
//	U8 image=MAIN;
//	BOOL setScreenPara=__TRUE;
	g_LoadModeFlag=1;
	SetOutputFormat(g_OutputFormatIndex);
	SetOutputHzValue();
	
	SetMultiWindowPara(g_MutiWindow,g_AlphaValue);
	
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		SetInputSource(g_SubInput, SUB);
	}

	SetInputSource(g_MainInput, MAIN);	

	if(g_MutiWindow==SAMEPIPIMAGE)
	{
		if(g_NextPointImage==SUB)
			SetWhichImageOnTop(MAIN);
		else
			SetWhichImageOnTop(SUB);
	}	

	/*
	switch(g_MainInput)
	{
		case INPUTSOURCE_DVI:
			if(g_DVIFullOrPartValue==PART)
				setScreenPara=__FALSE;
			break;
		case INPUTSOURCE_VGA:
			if(g_VGAFullOrPartValue==PART)
				setScreenPara=__FALSE;
			break;
		case INPUTSOURCE_HDMI:
			if(g_HDMIFullOrPartValue==PART)
				setScreenPara=__FALSE;
			break;
	}

	
	if(!setScreenPara)
	{
		SetScreenPART(MAIN);
	}
	if(g_MutiWindow!=SINGLEIMAGE)
	{		
		setScreenPara=__TRUE;
		if(g_MutiWindow==SAMEPIPIMAGE)
		{
			switch(g_SubInput)
			{
				case INPUTSOURCE_DVI:
					if(g_DVIFullOrPartValue==PART)
						setScreenPara=__FALSE;
					break;
				case INPUTSOURCE_VGA:
					if(g_VGAFullOrPartValue==PART)
						setScreenPara=__FALSE;
					break;
				case INPUTSOURCE_HDMI:
					if(g_HDMIFullOrPartValue==PART)
						setScreenPara=__FALSE;
					break;
			}
		}
		if(setScreenPara)
		{
			SetScreenFULL(SUB);			
		}
		else
		{
			SetScreenPART(SUB);
		}
	}
	*/
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{		
		SetScalerHVValue(HVSIZE,SUB);
		SetScalerHVValue(HVPOS,SUB);
	}
	
	SetZoomHVValue(HVSIZE,MAIN);
	SetZoomHVValue(HVPOS,MAIN);
	
	SetZoomHVValue(VGA_HVSIZE,MAIN);
	SetZoomHVValue(VGA_HVPOS,MAIN);
	SetZoomHVValue(DVI_HVSIZE,MAIN);
	SetZoomHVValue(DVI_HVPOS,MAIN);
	SetZoomHVValue(HDMI_HVSIZE,MAIN);
	SetZoomHVValue(HDMI_HVPOS,MAIN);
	SetZoomHVValue(OTHER_HVSIZE,MAIN);
	SetZoomHVValue(OTHER_HVPOS,MAIN);
	
	if(g_MutiWindow!=SINGLEIMAGE)
	{	
		SetZoomHVValue(HVSIZE,SUB);
		SetZoomHVValue(HVPOS,SUB);	
		
		SetZoomHVValue(VGA_HVSIZE,SUB);
		SetZoomHVValue(VGA_HVPOS,SUB);
		SetZoomHVValue(DVI_HVSIZE,SUB);
		SetZoomHVValue(DVI_HVPOS,SUB);
		SetZoomHVValue(HDMI_HVSIZE,SUB);
		SetZoomHVValue(HDMI_HVPOS,SUB);
		SetZoomHVValue(OTHER_HVSIZE,SUB);
		SetZoomHVValue(OTHER_HVPOS,SUB);
	}
	SendColorValue(BRIGHTNESS_EVENT,0,g_BrightnessValue);
	SendColorValue(CONTRAST_EVENT,0,g_ContrastValue);
	SendColorValue(SATURATION_EVENT,0,g_SaturationValue);

	
/*	SendColorValue(COLORRGB,4,g_OffsetR);
	SendColorValue(COLORRGB,5,g_OffsetG);
	SendColorValue(COLORRGB,6,g_OffsetB);
	SendColorValue(COLORRGB,1,g_GainR);
	SendColorValue(COLORRGB,2,g_GainG);
	SendColorValue(COLORRGB,3,g_GainB);*/
	g_LoadModeFlag=0;
	
	
	GetInputSrcFormat(MAIN,&HSize,&VSize);   //设置完后读取一下当前的输入信息分辨率，如果不读取，主板第一次读取的值为720*240，有问题
	////如果读到为0，再一次进行读取
	if(HSize==0 &&VSize==0)
	{
		delay_ms(100);
		GetInputSrcFormat(MAIN,&HSize,&VSize);
	}
	
	#ifdef DEBUG_ZOOM_YJL	
	printf("sendmsg.c 1108 :HSize=%d  VSize=%d \n",HSize,VSize);	
	#endif
	
}





void AutoAdjustVGA()
{
	G_SendBuf[EVENTID_IDX]=0x01;

	clearlcd();
	chinesedisplay(0x80, g_AdcCalibrationStr);//ADC矫正

	CheckSum_Send(G_SendBuf);

	//delay100ns(1000);
	//ReturnMainWinDontChangeState();
	
}


void SetOutputHzValue()
{	
	G_SendBuf[EVENTID_IDX]=0x02;
	G_SendBuf[PARA1_IDX]=0x00;
	G_SendBuf[PARA2_IDX]=g_OutputHzVal;
	CheckSum_Send(G_SendBuf);	
}



BOOL GetInputSrcFormat(U8 MainOrSub,U16 *hSize,U16 *vSize)
{
	U16 wResult=0;
	U8 Buf[CMDLENTH]={0};
	G_SendBuf[PARA1_IDX]=0x07;
 	if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
		G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
	else if(MainOrSub==SUB)
		G_SendBuf[EVENTID_IDX]=SUB_ZOOM_EVENT;
	
	g_ReceMsgFlag=__FALSE;
	g_RevUartIdx=0;
	g_RevUart3Idx=0;
	
	if(MainOrSub==MAIN)
	{
		if((g_MainInput==INPUTSOURCE_CVBS3) ||(g_MainInput==INPUTSOURCE_VGA2)||(g_MainInput==INPUTSOURCE_DVI2)||(g_MainInput==INPUTSOURCE_USB))
		{
			CheckSum_Send3(G_SendBuf);
			WaitUartACK(8000);
			CopyRevUart3Data(Buf);			
		}	
		else
		{
			CheckSum_Send(G_SendBuf);	
			WaitUartACK(8000);
			CopyRevUart2Data(Buf);	
		}
	}
	else
	{
		if((g_SubInput==INPUTSOURCE_CVBS3) ||(g_SubInput==INPUTSOURCE_VGA2)||(g_SubInput==INPUTSOURCE_DVI2)||(g_SubInput==INPUTSOURCE_USB))
		{
			G_SendBuf[EVENTID_IDX]=MAIN_ZOOM_EVENT;
			CheckSum_Send3(G_SendBuf);
			WaitUartACK(8000);
			CopyRevUart3Data(Buf);	
		}
		else
		{
			CheckSum_Send(G_SendBuf);	
			WaitUartACK(8000);
			CopyRevUart2Data(Buf);			
		}
	}
	
	if(!CheckProtocolStart(Buf))
		return __FALSE;
	if(!CheckSum(Buf))
		return __FALSE;
	if(Buf[EVENTID_IDX]==G_SendBuf[EVENTID_IDX]&&Buf[PARA1_IDX]==0x07)
	{
		wResult=((Buf[PARA2_IDX]<<8)|Buf[PARA3_IDX]);
		if(hSize)
			*hSize=wResult;
		wResult=((Buf[PARA4_IDX]<<8)|Buf[PARA5_IDX]);
		if(vSize)
			*vSize=wResult;
		ClearRevUart2Data();
		ClearRevUart3Data();
		return __TRUE;
	}
	else
		return __FALSE;
}


