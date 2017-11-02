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

#if(XP_TO_ID==XP_TO_FLI32626)
static unsigned char g_LoadModeFlag=0;
extern U8 g_RevUart2Data[12];
extern U8 g_RevUartIdx;

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

void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub)
{
	if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_SRC_EVENT;
		g_MainInput=inputsrc;
//		Write_EEPROM_Byte(E2ROM_InputSource_ADDR,g_MainInput);
	}
	else if(MainOrSub==SUB)
	{
		G_SendBuf[EVENTID_IDX]=SUB_SRC_EVENT;
		g_SubInput=inputsrc;
	}
	switch(inputsrc)
	{
		case INPUTSOURCE_CVBS:   
			G_SendBuf[PARA1_IDX]=AV1PARA1;  
			break;
		case INPUTSOURCE_CVBS2:
			G_SendBuf[PARA1_IDX]=AV2PARA1;  
			break;
		case INPUTSOURCE_CVBS3:
			G_SendBuf[PARA1_IDX]=AV3PARA1;  
			break;
		case INPUTSOURCE_CVBS4:
			G_SendBuf[PARA1_IDX]=AV4PARA1;  
			break;
		case INPUTSOURCE_VGA:
			VGA_SEL_H;
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_VGA2:
			VGA_SEL_L;
			G_SendBuf[PARA1_IDX]=VGAPARA1;  
			break;
		case INPUTSOURCE_DVI:
			G_SendBuf[PARA1_IDX]=DVIPARA1;  
			break;
	#if(ENABLE_HDMI)
		case INPUTSOURCE_HDMI:
			G_SendBuf[PARA1_IDX]=HDMIPARA1;  
			break;
	#endif
		case INPUTSOURCE_YPBPR:
			G_SendBuf[PARA1_IDX]=YPBPRPARA1;  
			break;
		case INPUTSOURCE_SDI:
			G_SendBuf[PARA1_IDX]=SDIPARA1;  
			break;
		case INPUTSOURCE_DP:
			G_SendBuf[PARA1_IDX]=DPPARA1;  
			break;
	}
	CheckSum_Send(G_SendBuf);
}

void SetWhichImageOnTop(U8 MainOrSUb)
{
	G_SendBuf[EVENTID_IDX]=IMAGE_ONTOP_EVENT;
	G_SendBuf[PARA1_IDX]=MainOrSUb;
	CheckSum_Send(G_SendBuf);		
}

void SetMultiWindowPara(U8 para)
{	
	G_SendBuf[EVENTID_IDX]=MULTI_EVENT;
	G_SendBuf[PARA1_IDX]=para;
	if(para==FUSEPIPPARA1)
	{		
		G_SendBuf[PARA2_IDX]=g_AlphaValue;
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
		#if(ENABLE_HDMI)
			else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
			{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVSize&0xff);
			}
		#endif
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
		#if(ENABLE_HDMI)
			else if(g_MainInput==INPUTSOURCE_HDMI||g_MainInput==INPUTSOURCE_HDMI2)
			{
			G_SendBuf[PARA2_IDX]=(g_MainHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainHDMIZoomVPos&0xff);
			}
		#endif
			else
			{
			G_SendBuf[PARA2_IDX]=(g_MainZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_MainZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_MainZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_MainZoomVPos&0xff);
			}
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
		#if(ENABLE_HDMI)
			else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
			{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHSize>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHSize&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVSize>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVSize&0xff);
			}
		#endif
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
		#if(ENABLE_HDMI)
			else if(g_SubInput==INPUTSOURCE_HDMI||g_SubInput==INPUTSOURCE_HDMI2)
			{
			G_SendBuf[PARA2_IDX]=(g_SubHDMIZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubHDMIZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubHDMIZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubHDMIZoomVPos&0xff);
			}
		#endif
			else
			{
			G_SendBuf[PARA2_IDX]=(g_SubZoomHPos>>8);
			G_SendBuf[PARA3_IDX]=(g_SubZoomHPos&0xff);
			G_SendBuf[PARA4_IDX]=(g_SubZoomVPos>>8);
			G_SendBuf[PARA5_IDX]=(g_SubZoomVPos&0xff);
			}
		}
	}
	CheckSum_Send(G_SendBuf);	
}


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


void SetScalerHVValue(U8 para,U8 MainOrSUb)
{	
	G_SendBuf[PARA1_IDX]=para;
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		G_SendBuf[EVENTID_IDX]=MAIN_SCALER_EVENT;
		switch(para)
		{
			case HVSIZE:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_SIZE,g_ScreenHSize,g_MainScalerHSize);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_SIZE,g_ScreenVSize,g_MainScalerVSize);
				break;
			case HVPOS:
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_H_POS,g_ScreenHPos,g_MainScalerHPos);
				Write_EEPROM_ScreenScaler(E2ROM_SCREEN_V_POS,g_ScreenVPos,g_MainScalerVPos);
				break;
		}
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

void LoadModeSet(void)
{
	g_LoadModeFlag=1;
	SetOutputFormat(g_OutputFormatIndex);
	SetMultiWindowPara(g_MutiWindow);
	SetInputSource(g_MainInput, MAIN);
	
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		SetInputSource(g_SubInput, SUB);
	}
	if(g_MutiWindow==SAMEPIPIMAGE)
	{
		if(g_NextPointImage==SUB)
			SetWhichImageOnTop(MAIN);
		else
			SetWhichImageOnTop(SUB);
	}
	SetScalerHVValue(HVSIZE,MAIN);
	SetScalerHVValue(HVPOS,MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{		
		SetScalerHVValue(HVSIZE,SUB);
		SetScalerHVValue(HVPOS,SUB);
	}
	delay(20);
	SetZoomHVValue(HVSIZE,MAIN);
	SetZoomHVValue(HVPOS,MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{	
		if(g_MutiWindow==PBPIMAGE)
		{
			if(g_SubZoomHSize>g_PanelWidth/2)
			{
				SetZoomValue(HPOS,g_SubZoomHPos,SUB);
				SetZoomHVValue(HVSIZE,SUB);
				SetZoomValue(VPOS,g_SubZoomVPos,SUB);	
			}
			else
			{
				SetZoomHVValue(HVSIZE,SUB);
				SetZoomHVValue(HVPOS,SUB);				
			}
		}
	}
	SendColorValue(BRIGHTNESS_EVENT,0,g_Brightness_Value);
	SendColorValue(CONTRAST_EVENT,0,g_Contrast_Value);
	SendColorValue(SATURATION_EVENT,0,g_Saturation_Value);
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
//	if(WaitUartACK(2000))
//	{  
		WaitUartACK(2000);
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
//	}
//	else
//		return __FALSE;
	
}

void SetVolumeValue(U8 VolVaule)
{	
}

void SetAudioPara(U8 para)
{
}

#elif(XP_TO_ID==XP_TO_6M16|| XP_TO_ID==XP_TO_FPGA)
extern U8 g_RevUart2Data[4];
unsigned char g_LoadModeFlag=0;
extern U8 g_QueueInCount;

void LoadModeSet(void)
{
	BOOL setScreenPara=__TRUE;
	g_LoadModeFlag=1;
	
	SetOutputFormat(g_OutputFormatIndex);		
	//----------------18bytes----------------------------------	
	SetMultiWindowPara(g_MutiWindow);	
	//----------------------single 36  dual 53----------------------
	///set current input source zoom
/*	if(g_MainInput==INPUTSOURCE_VGA)
	{
		SetZoomValue(HSIZE,g_MainVGAZoomHSize,MAIN);	
		SetZoomValue(VSIZE,g_MainVGAZoomVSize,MAIN);
		SetZoomValue(HPOS,g_MainVGAZoomHPos,MAIN);	
		SetZoomValue(VPOS,g_MainVGAZoomVPos,MAIN);	
	}
	else if(g_MainInput==INPUTSOURCE_DVI)
	{
		SetZoomValue(HSIZE,g_MainDVIZoomHSize,MAIN);	
		SetZoomValue(VSIZE,g_MainDVIZoomVSize,MAIN);
		SetZoomValue(HPOS,g_MainDVIZoomHPos,MAIN);	
		SetZoomValue(VPOS,g_MainDVIZoomVPos,MAIN);	
	}
	else if(g_MainInput==INPUTSOURCE_HDMI)
	{
		SetZoomValue(HSIZE,g_MainHDMIZoomHSize,MAIN);	
		SetZoomValue(VSIZE,g_MainHDMIZoomVSize,MAIN);
		SetZoomValue(HPOS,g_MainHDMIZoomHPos,MAIN);	
		SetZoomValue(VPOS,g_MainHDMIZoomVPos,MAIN);	
	}
	else
	{
		SetZoomValue(HSIZE,g_MainZoomHSize,MAIN);	
		SetZoomValue(VSIZE,g_MainZoomVSize,MAIN);	
		SetZoomValue(HPOS,g_MainZoomHPos,MAIN);	
		SetZoomValue(VPOS,g_MainZoomVPos,MAIN);	
	}
	
	//----------------------single 52  dual 69----------------------
	if(g_MutiWindow!=SINGLEIMAGE)   //if it is PIP or PBP
	{		
		if(g_SubInput==INPUTSOURCE_VGA)
		{
			SetZoomValue(HSIZE,g_SubVGAZoomHSize,SUB);
			SetZoomValue(VSIZE,g_SubVGAZoomVSize,SUB);
			SetZoomValue(HPOS,g_SubVGAZoomHPos,SUB);	
			SetZoomValue(VPOS,g_SubVGAZoomVPos,SUB);	
		}
		else if(g_SubInput==INPUTSOURCE_DVI)
		{
			SetZoomValue(HSIZE,g_SubDVIZoomHSize,SUB);
			SetZoomValue(VSIZE,g_SubDVIZoomVSize,SUB);
			SetZoomValue(HPOS,g_SubDVIZoomHPos,SUB);	
			SetZoomValue(VPOS,g_SubDVIZoomVPos,SUB);	
		}
		else if(g_SubInput==INPUTSOURCE_HDMI)
		{
			SetZoomValue(HSIZE,g_SubHDMIZoomHSize,SUB);
			SetZoomValue(VSIZE,g_SubHDMIZoomVSize,SUB);
			SetZoomValue(HPOS,g_SubHDMIZoomHPos,SUB);	
			SetZoomValue(VPOS,g_SubHDMIZoomVPos,SUB);	
		}
		else
		{
			SetZoomValue(HSIZE,g_SubZoomHSize,SUB);	
			SetZoomValue(VSIZE,g_SubZoomVSize,SUB);
			SetZoomValue(HPOS,g_SubZoomHPos,SUB);	
			SetZoomValue(VPOS,g_SubZoomVPos,SUB);	
		}
	}*/

	SetVGAZoomValue(HSIZE,g_MainVGAZoomHSize,MAIN);	
	SetVGAZoomValue(VSIZE,g_MainVGAZoomVSize,MAIN);	
	SetVGAZoomValue(HPOS,g_MainVGAZoomHPos,MAIN);	
	SetVGAZoomValue(VPOS,g_MainVGAZoomVPos,MAIN);
	
	SetDVIZoomValue(HSIZE,g_MainDVIZoomHSize,MAIN);
	SetDVIZoomValue(VSIZE,g_MainDVIZoomVSize,MAIN);
	SetDVIZoomValue(HPOS,g_MainDVIZoomHPos,MAIN);
	SetDVIZoomValue(VPOS,g_MainDVIZoomVPos,MAIN);
	
	SetHDMIZoomValue(HSIZE,g_MainHDMIZoomHSize,MAIN);
	SetHDMIZoomValue(VSIZE,g_MainHDMIZoomVSize,MAIN);
	SetHDMIZoomValue(HPOS,g_MainHDMIZoomHPos,MAIN);
	SetHDMIZoomValue(VPOS,g_MainHDMIZoomVPos,MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		SetVGAZoomValue(HSIZE,g_SubVGAZoomHSize,SUB);	
		SetVGAZoomValue(VSIZE,g_SubVGAZoomVSize,SUB);
		SetVGAZoomValue(HPOS,g_SubVGAZoomHPos,SUB);	
		SetVGAZoomValue(VPOS,g_SubVGAZoomVPos,SUB);
		
		SetDVIZoomValue(HSIZE,g_SubDVIZoomHSize,SUB);	
		SetDVIZoomValue(VSIZE,g_SubDVIZoomVSize,SUB);	
		SetDVIZoomValue(HPOS,g_SubDVIZoomHPos,SUB);	
		SetDVIZoomValue(VPOS,g_SubDVIZoomVPos,SUB);	
		
		SetHDMIZoomValue(HSIZE,g_SubHDMIZoomHSize,SUB);	
		SetHDMIZoomValue(VSIZE,g_SubHDMIZoomVSize,SUB);	
		SetHDMIZoomValue(HPOS,g_SubHDMIZoomHPos,SUB);	
		SetHDMIZoomValue(VPOS,g_SubHDMIZoomVPos,SUB);	
	}
	if(g_MutiWindow==SINGLEIMAGE)
	{
		SetInputSource(g_MainInput, MAIN);
	}
	else
	{
		SetInputSource(g_MainInput, MAIN);
		SetInputSource(g_SubInput, SUB);
	}
	/*if(g_MainInput!=INPUTSOURCE_VGA
	#if(ENABLE_HDMI)
		&&g_MainInput!=INPUTSOURCE_HDMI&&g_MainInput!=INPUTSOURCE_HDMI2
	#endif
		&&g_MainInput!=INPUTSOURCE_DVI)
	{
		SetZoomValue(HSIZE,g_MainZoomHSize,MAIN);	
		SetZoomValue(VSIZE,g_MainZoomVSize,MAIN);
		SetZoomValue(HPOS,g_MainZoomHPos,MAIN);	
		SetZoomValue(VPOS,g_MainZoomVPos,MAIN);
		if(g_MutiWindow!=SINGLEIMAGE)
		{
			if(g_MutiWindow==PBPIMAGE)
			{
				if(g_SubZoomHSize>g_PanelWidth/2)
				{
					SetZoomValue(HPOS,g_SubZoomHPos,SUB);
					SetZoomValue(HSIZE,g_SubZoomHSize,SUB);
					SetZoomValue(VSIZE,g_SubZoomVSize,SUB);
					SetZoomValue(VPOS,g_SubZoomVPos,SUB);	
				}
				else
				{
					SetZoomValue(HSIZE,g_SubZoomHSize,SUB);	
					SetZoomValue(VSIZE,g_SubZoomVSize,SUB);
					SetZoomValue(HPOS,g_SubZoomHPos,SUB);	
					SetZoomValue(VPOS,g_SubZoomVPos,SUB);						
				}
			}			
		}
	}
	*/
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
	#if(ENABLE_HDMI)
		case INPUTSOURCE_HDMI:
		case INPUTSOURCE_HDMI2:
			if(g_HDMIFullOrPartValue==PART)
				setScreenPara=__FALSE;
			break;
	#endif
	}
	*/
	if(setScreenPara)
	{
		SetScreenValue(HSIZE,g_ScreenHSize);
		SetScreenValue(VSIZE,g_ScreenVSize);
		SetScreenValue(HPOS,g_ScreenHPos);
		SetScreenValue(VPOS,g_ScreenVPos);
		//----------------------single 20  dual 21----------------------
		SetScalerValue(HSIZE,g_MainScalerHSize,MAIN);
		SetScalerValue(VSIZE,g_MainScalerVSize,MAIN);
		SetScalerValue(HPOS,g_MainScalerHPos,MAIN);
		SetScalerValue(VPOS,g_MainScalerVPos,MAIN);
	}
	
	//----------------------single 36  dual 37----------------------
	if(g_MutiWindow!=SINGLEIMAGE)
	{
		if(g_SubScalerHSize>g_PanelWidth/2)
		{
	
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);
			SetScalerValue(HSIZE,g_SubScalerHSize,SUB);	
		}
		else
		{

			SetScalerValue(HSIZE,g_SubScalerHSize,SUB);
			SetScalerValue(HPOS,g_SubScalerHPos,SUB);	
		}
		SetScalerValue(VSIZE,g_SubScalerVSize,SUB);
		SetScalerValue(VPOS,g_SubScalerVPos,SUB);	
	#ifdef SAVE_COLOR
		SetColorValue(BRIGHTNESS, g_Brightness_Value, SUB);
		SetColorValue(CONTRAST, g_Contrast_Value, SUB);
	#endif
	}
#ifdef SAVE_COLOR
	SetColorValue(BRIGHTNESS, g_Brightness_Value, MAIN);
	SetColorValue(CONTRAST, g_Contrast_Value, MAIN);
#endif
	//----------------------single 52  dual 85----------------------
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
	SetColorValue(BRIGHTNESSR, g_BrightnessR_Value[0], MAIN);
	SetColorValue(BRIGHTNESSG, g_BrightnessG_Value[0], MAIN);  //60 byte
	SetColorValue(BRIGHTNESSB, g_BrightnessB_Value[0], MAIN);
	SetColorValue(CONTRASTR, g_ContrastR_Value[0], MAIN);
	SetColorValue(CONTRASTG, g_ContrastG_Value[0], MAIN);
	SetColorValue(CONTRASTB, g_ContrastB_Value[0], MAIN);
	if(g_MutiWindow!=SINGLEIMAGE)
	{	
		SetColorValue(BRIGHTNESSR, g_BrightnessR_Value[1], SUB);
		SetColorValue(BRIGHTNESSG, g_BrightnessG_Value[1], SUB);
		SetColorValue(BRIGHTNESSB, g_BrightnessB_Value[1], SUB);
		SetColorValue(CONTRASTR, g_ContrastR_Value[1], SUB);
		SetColorValue(CONTRASTG, g_ContrastG_Value[1], SUB);
		SetColorValue(CONTRASTB, g_ContrastB_Value[1], SUB);
	}	
#endif
	g_LoadModeFlag=0;
	g_QueueInCount=0;
}


void AutoAdjustVGA()
{
	UART2_SendByte(0xE4);
}

BOOL WaitOneUartACK(U32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0)
		{
			if(g_OneByteFlag)
			{
				g_OneByteFlag=__FALSE;
				return __TRUE;
			}
		}
	}
	return __FALSE;
}

BOOL WaitTwoUartACK(U32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0)
		{
			if(g_TwoByteFlag)
			{
				g_TwoByteFlag=__FALSE;
				return __TRUE;
			}
		}
	}
	return __FALSE;
}

/*
void SwitchInputAudio(_InputSourceType input)
{
#ifndef TWO_EC11_BOARD
	switch(input)
	{
		case INPUTSOURCE_VGA:
			PC_AUD_SEL0_H;
			PC_AUD_SEL1_H;
			break;
		case INPUTSOURCE_DVI:
			PC_AUD_SEL0_L;
			PC_AUD_SEL1_H;
			break;
	#if(ENABLE_HDMI)
		case INPUTSOURCE_HDMI:
			PC_AUD_SEL0_H;
			PC_AUD_SEL1_L;
			break;
		case INPUTSOURCE_HDMI2:
			PC_AUD_SEL0_L;
			PC_AUD_SEL1_L;
			break;
	#endif
	}
#endif
}
*/
BOOL WaitFourUartACK(U32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0)
		{
			if(g_FourByteFlag)
			{
				g_FourByteFlag=__FALSE;
				return __TRUE;
			}
		}
	}
	return __FALSE;
}

U8 GetInputSource(U8 AOrB)
{
	U8 buf[ONEBYTE]={0xff},uResult=0;
	buf[0]=(buf[0]&ONE_BYTE_MASK);
	if(AOrB==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[0]=(buf[0]|MAININPUT);
	else if(AOrB==SUB)
		buf[0]=(buf[0]|SUBINPUT);
	buf[0]=(buf[0]&ONEBYTEDATA);
	g_OneByteFlag=__FALSE;
	UART2_SendBuf(buf, ONEBYTE);
	if(WaitOneUartACK(500))
	{
		uResult=g_RevUart2Data[0];
		uResult&=0x0f;
	}
	return uResult;
}


void SetOutputHzValue()
{			
	U8 buf[TWOBYTES]={0xff,0xff};
	buf[0]=(buf[0]&TWO_BYTE_MASK);
	buf[0]=(buf[0]|SETHZCMD);
	buf[1]=g_OutputHzVal;
	UART2_SendBuf(buf, TWOBYTES);
}

void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub)
{
	U8 buf[ONEBYTE]={0xff};
	buf[0]=(buf[0]&ONE_BYTE_MASK);
	if(g_MutiWindow==SINGLEIMAGE)
	{
		/*switch(inputsrc)
		{
			case INPUTSOURCE_CVBS:
			case INPUTSOURCE_CVBS2:
			case INPUTSOURCE_CVBS3:
			case INPUTSOURCE_CVBS4:
			case INPUTSOURCE_SVIDEO:
			case INPUTSOURCE_YPBPR:
					SetScreenFULL();
				break;
			case INPUTSOURCE_VGA:
			case INPUTSOURCE_VGA2:
				if(g_VGAFullOrPartValue==FULL)
					SetScreenFULL();
				else
					SetScreenPART();
				break;
			case INPUTSOURCE_DVI:
			case INPUTSOURCE_DVI2:
				if(g_DVIFullOrPartValue==FULL)
					SetScreenFULL();
				else
					SetScreenPART();
				break;
		#if(ENABLE_HDMI)
			case INPUTSOURCE_HDMI:
			case INPUTSOURCE_HDMI2:
				if(g_HDMIFullOrPartValue==FULL)
					SetScreenFULL();
				else
					SetScreenPART();
				break;
		#endif
		}
		*/
		buf[0]=(buf[0]|SINGLEINPUT);
		g_MainInput=inputsrc;
	}
	else if(MainOrSub==MAIN)
	{
		buf[0]=(buf[0]|MAININPUT);
		g_MainInput=inputsrc;
	}
	else if(MainOrSub==SUB)
	{
		buf[0]=(buf[0]|SUBINPUT);
		g_SubInput=inputsrc;
	}
	buf[0]=(buf[0]&ONEBYTEDATA);
	/*if(inputsrc==INPUTSOURCE_VGA2)
		buf[0]=(buf[0]|INPUTSOURCE_VGA);
	else if(inputsrc==INPUTSOURCE_DVI2)
		buf[0]=(buf[0]|INPUTSOURCE_DVI);
	else if(inputsrc==INPUTSOURCE_HDMI2)
		buf[0]=(buf[0]|INPUTSOURCE_HDMI);
	else
		buf[0]=(buf[0]|inputsrc);
	*/
	UART2_SendBuf(buf, 1);
	if(g_MutiWindow==SINGLEIMAGE||MainOrSub==MAIN)
	{//如果切换的是主通道，那么声音同时切换
		////SwitchInputAudio(g_MainInput);
	}
#ifdef STM32F103RB

#endif

#ifdef TWO_EC11_BOARD
	if(inputsrc==INPUTSOURCE_VGA)
		VGA_SEL_H;
	else if(inputsrc==INPUTSOURCE_VGA2)
		VGA_SEL_L;
	else if(inputsrc==INPUTSOURCE_DVI)
		PC_SEL_DVI_H;
	else if(inputsrc==INPUTSOURCE_DVI2)
		PC_SEL_DVI_L;
#endif
}

void SetFreezeImage(U8 MainOrSUb,U8 para)
{	
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		if(para==FREEZE)
			SetMultiWindowPara(MAINFREEZE);
		else
			SetMultiWindowPara(MAINUNFREEZE);
	}
	else
	{
		if(para==FREEZE)
			SetMultiWindowPara(SUBFREEZE);
		else
			SetMultiWindowPara(SUBUNFREEZE);
	}	
}

void SetMultiWindowPara(U8 para)
{				
	U8 buf[ONEBYTE]={0xff};
	buf[0]=(buf[0]&ONE_BYTE_MASK);
	buf[0]=(buf[0]|MULTI);
	buf[0]=(buf[0]&ONEBYTEDATA);
	buf[0]=(buf[0]|para);
	UART2_SendBuf(buf, ONEBYTE);
}

void SetZoomValue(U8 para,U16 wValue,U8 MainOrSUb)
{			
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|SETMAINZOOMCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETSUBZOOMCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);
	UART2_SendBuf(buf, FOURBYTES);
}
U16 GetZoomValue(U8 para,U8 MainOrSub)
{	
	U8 i;
	U16 wResult=0;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSub==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|GETMAINZOOMCMD);
	if(MainOrSub==SUB)
		buf[1]=(buf[1]|GETSUBZOOMCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);	
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);	
	if(WaitFourUartACK(500))
	{
		for(i=0;i<FOURBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		wResult=((buf[3]<<8)|buf[2]);
	}
	return wResult;
}

void SetVGAZoomValue(U8 para,U16 wValue,U8 MainOrSUb)
{			
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|SETVGAMAINZOOMCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETVGASUBZOOMCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);
	UART2_SendBuf(buf, FOURBYTES);
}
void SetDVIZoomValue(U8 para,U16 wValue,U8 MainOrSUb)
{			
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|SETDVIMAINZOOMCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETDVISUBZOOMCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);
	UART2_SendBuf(buf, FOURBYTES);
}
void SetHDMIZoomValue(U8 para,U16 wValue,U8 MainOrSUb)
{			
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|SETHDMIMAINZOOMCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETHDMISUBZOOMCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);
	UART2_SendBuf(buf, FOURBYTES);
}
void SetScreenValue(U8 para,U16 wValue)
{		
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	buf[1]=(buf[1]|SETSCREENCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);	
	UART2_SendBuf(buf, FOURBYTES);
}

BOOL GetScreenValue(U8 para)
{
	U8 i;
	U16 wResult=0;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	buf[1]=(buf[1]|GETSCREENCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);	
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);	
//	delay1000(500);
	if(WaitFourUartACK(500))
	{
		for(i=0;i<FOURBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		wResult=((buf[3]<<8)|buf[2]);
		if(para==HSIZE)
			g_ScreenHSize=wResult;
		else if(para==VSIZE)
			g_ScreenVSize=wResult;
		else if(para==HPOS)
			g_ScreenHPos=wResult;
		else if(para==VPOS)
			g_ScreenVPos=wResult;
		return __TRUE;
	}
	else
		return __FALSE;
}

void SetScalerValue(U8 para,U16 wValue,U8 MainOrSUb)
{		
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
	{
		buf[1]=(buf[1]|SETMAINSCALERCMD);
	}
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETSUBSCALERCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=(wValue&0xff);
	buf[3]=(wValue>>8);	
	UART2_SendBuf(buf, FOURBYTES);
}

BOOL GetScalerValue(U8 para,U8 MainOrSUb)
{	
	U8 i;
	U16 wResult=0;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|GETMAINSCALERCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|GETSUBSCALERCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);	
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);	
	if(WaitFourUartACK(500))
	{
		for(i=0;i<FOURBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		wResult=((buf[3]<<8)|buf[2]);
		if(MainOrSUb==MAIN)
		{
			if(para==HSIZE)
				g_MainScalerHSize=wResult;
			else if(para==VSIZE)
				g_MainScalerVSize=wResult;
			else if(para==HPOS)
				g_MainScalerHPos=wResult;
			else if(para==VPOS)
				g_MainScalerVPos=wResult;
		}
		else if(MainOrSUb==SUB)
		{
			if(para==HSIZE)
				g_SubScalerHSize=wResult;
			else if(para==VSIZE)
				g_SubScalerVSize=wResult;
			else if(para==HPOS)
				g_SubScalerHPos=wResult;
			else if(para==VPOS)
				g_SubScalerVPos=wResult;

		}
		return __TRUE;
	}
	else
		return __FALSE;
}

void SetColorValue(U8 para,U8 wValue,U8 MainOrSUb)
{			
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|SETMAINCOLORCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|SETSUBCOLORCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	buf[2]=wValue;
	UART2_SendBuf(buf, FOURBYTES);	
}

U8 GetColorValue(U8 para,U8 MainOrSUb)
{	
	U8 i;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	if(MainOrSUb==MAIN||g_MutiWindow==SINGLEIMAGE)
		buf[1]=(buf[1]|GETMAINCOLORCMD);
	if(MainOrSUb==SUB)
		buf[1]=(buf[1]|GETSUBCOLORCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);	
	if(WaitFourUartACK(500))
	{		
		for(i=0;i<FOURBYTES;i++)
		{
			buf[i]=g_RevUart2Data[i];
		}
		return buf[2];
	}		
	else if(para==BRIGHTNESS||para==CONTRAST)
		return 50;
	else if(para==BRIGHTNESSR||para==BRIGHTNESSG||para==BRIGHTNESSB)
		return 10;
	else if(para==CONTRASTR||para==CONTRASTG||para==CONTRASTB)
		return 128;
	return 0;
}

void SetOutputFormat(U8 nIndex)
{			
	U8 buf[TWOBYTES]={0xff,0xff};
	buf[0]=(buf[0]&TWO_BYTE_MASK);
	buf[0]=(buf[0]|SETOUTFORMATCMD);
	buf[1]=nIndex;
	UART2_SendBuf(buf, TWOBYTES);
}

void SetVolumeValue(U8 VolVaule)
{			
	U8 buf[TWOBYTES]={0xff,0xff};
	buf[0]=(buf[0]&TWO_BYTE_MASK);
	buf[0]=(buf[0]|SETVOLVALUECMD);
	buf[1]=VolVaule;
	UART2_SendBuf(buf, TWOBYTES);	
}

void SetAudioPara(U8 para)
{
	_InputSourceType input=INPUTSOURCE_NONE;
	U8 buf[ONEBYTE]={0xff};
	buf[0]=(buf[0]&ONE_BYTE_MASK);
	buf[0]=(buf[0]|VOLUMECONTROL);
	buf[0]=(buf[0]&ONEBYTEDATA);
	buf[0]=(buf[0]|para);
	UART2_SendBuf(buf, ONEBYTE);
	if(para==MAINVOL)
		input=g_MainInput;
	else if(para==SUBVOL)
		input=g_SubInput;
	////SwitchInputAudio(input);
}

U8 GetOutputFormat()
{	
	U8 i;
	U8 buf[TWOBYTES]={0xff,0x00};
	buf[0]=(buf[0]&TWO_BYTE_MASK);
	buf[0]=(buf[0]|GETOUTFORMATCMD);	
	g_TwoByteFlag=__FALSE;
	UART2_SendBuf(buf, TWOBYTES);
	if(WaitTwoUartACK(1000))
	{
		for(i=0;i<TWOBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		return buf[1];
	}
	else
		return 0;
}

U16 GetMainInputSrcFormat(U8 para)
{
	U8 i;
	U16 wResult=0;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	buf[1]=(buf[1]|GETSCREENCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);	
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);	
//	delay1000(2000);
	if(WaitFourUartACK(5000))
	{
		for(i=0;i<FOURBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		wResult=((buf[3]<<8)|buf[2]);
	}
	return wResult;
}

U16 GetSubInputSrcFormat(U8 para)
{
	U8 i;
	U16 wResult=0;
	U8 buf[FOURBYTES]={0xff,0xff,0x00,0x00};
	buf[1]=(buf[1]&FOUR_BYTE_MASK);
	buf[1]=(buf[1]|GETSUBSCALERCMD);
	buf[1]=(buf[1]&FOUR_BYTE_PAMA_MASK);
	buf[1]=(buf[1]|para);	
	g_FourByteFlag=__FALSE;
	UART2_SendBuf(buf, FOURBYTES);
	if(WaitFourUartACK(2000))
	{
		for(i=0;i<FOURBYTES;i++)
			buf[i]=g_RevUart2Data[i];
		wResult=((buf[3]<<8)|buf[2]);
	}
	return wResult;
}

BOOL GetInputSrcFormat(U8 MainOrSub,U16 *hSize,U16 *vSize)
{
	U16 HSize=0,VSize=0;
	if(MainOrSub==MAIN)
	{
		HSize=GetMainInputSrcFormat(INSRC_HZOOM);
		VSize=GetMainInputSrcFormat(INSRC_VZOOM);
	}
	else
	{
		HSize=GetSubInputSrcFormat(INSRC_HZOOM);
		VSize=GetSubInputSrcFormat(INSRC_VZOOM);
	}
	if(hSize)
		*hSize=HSize;
	if(vSize)
		*vSize=VSize;
	
	
	if(HSize!=0&&VSize!=0)
		return __TRUE;
	return __FALSE;
}
#endif
