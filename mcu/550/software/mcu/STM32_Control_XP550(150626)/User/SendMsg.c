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
extern U8 g_RevUart2Data[12];
extern U8 g_RevUartIdx;
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
		g_RevUart2Data[i]=0;
		
	}
}

void CopyRevUart2Data(U8 *buf)
{
	U8 i=0;
	for(i=0;i<CMDLENTH;i++)
	{
		buf[i]=g_RevUart2Data[(g_RevUartIdx+i)%12];
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

void SelVGA(U8 Idx)
{
//	if(Idx==1)
//		GPIO_SetBits(GPIOC,GPIO_Pin_2);
//	else
//		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
}

void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub)
{
	if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		MainOrSub=MAIN;
		G_SendBuf[EVENTID_IDX]=MAIN_SRC_EVENT;
		g_MainInput=inputsrc;
	}
	else if(MainOrSub==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SRC_EVENT;
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
			G_SendBuf[PARA1_IDX]=AV4PARA1;  
			break;
		case INPUTSOURCE_VGA:
			SelVGA(1);
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_VGA2:
			SelVGA(2);
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_DVI:
			G_SendBuf[PARA1_IDX]=DVIPARA1;  
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
		case INPUTSOURCE_DP:
			G_SendBuf[PARA1_IDX]=DPPARA1;  
			break;
	}
	CheckSum_Send(G_SendBuf);

	
	switch(inputsrc)
	{
		case INPUTSOURCE_CVBS:
		case INPUTSOURCE_CVBS2:
		case INPUTSOURCE_CVBS3:
		case INPUTSOURCE_CVBS4:
		case INPUTSOURCE_YPBPR:
		case INPUTSOURCE_SDI:
			if(g_OtherFullOrPartValue==FULL)
				SetScreenFULL(MainOrSub);
			else
				SetScreenPART(MainOrSub);
			break;
		case INPUTSOURCE_VGA:
		case INPUTSOURCE_VGA2:
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
	CheckSum_Send(G_SendBuf);	
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
}

void AutoAdjustVGA()
{
	G_SendBuf[EVENTID_IDX]=0x01;
	CheckSum_Send(G_SendBuf);	
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
	CheckSum_Send(G_SendBuf);	
	WaitUartACK(5000);
	CopyRevUart2Data(Buf);		
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
		return __TRUE;
	}
	else
		return __FALSE;
}


