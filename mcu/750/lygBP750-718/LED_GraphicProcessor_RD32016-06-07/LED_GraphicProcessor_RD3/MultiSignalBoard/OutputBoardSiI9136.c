#define __OUTPUT_BOARD_SI_I9136_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      OutputBoardSiI9136.c                                                      */
/*																			*/
/* USAGE:       This module contains functions for system initialization.	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
//#include "lpc17xx.h"
#include "type.h"
#include "Serial.h"
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "D_IIC3.h"
#include "OutputBoardSiI9136.h"

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
ST_OUTPUT_BOARD_SI_I9136 gSt_OutputBoarSiI9136;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void InitOutputBoarSiI9136(void);
void OutputBoarSiI9136Task(void);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
static BYTE SI_I9136_PollInterruptFlag = 0;

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/


/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void InitOutputBoardSiI9136(void)
{
	int i = 0;
	BYTE B_Data = 0;
	ST_DEVICE_SI_I9136 *Stp_TempDeviceSiI9136 = NULL;
	SiI9136_GlobalStatus_St *Stp_TempSiI9136Status_t = NULL;
	VideoModeDescription_t *Stp_TempVideoModeDescription = NULL;
	SI_I9136_PollInterruptFlag = 0;
	memset(&gSt_OutputBoarSiI9136, 0, sizeof(gSt_OutputBoarSiI9136));
	for(i = 0; i < OUTPUT_BOARD_MAX; i++)
	{
		if(i == 0)
		{
			gB_I2CIndex = 1;																			// IIC2
			gB_TX_SLAVE_Addr = 0x76;
			gB_TPI_BASE_Addr = 0x76;
		}
		if(i == 1)
		{
			gB_I2CIndex = 1;																			// IIC2
			gB_TX_SLAVE_Addr = 0x72;
			gB_TPI_BASE_Addr = 0x72;
		}
		if(i == 2)
		{
			gB_I2CIndex = 0;																			// IIC1
			gB_TX_SLAVE_Addr = 0x76;
			gB_TPI_BASE_Addr = 0x76;
		}
		if(i == 3)
		{
			gB_I2CIndex = 0;																			// IIC1
			gB_TX_SLAVE_Addr = 0x72;
			gB_TPI_BASE_Addr = 0x72;
		}
		B_Data = StartTPI();
		if(B_Data == TRUE)
		{
			Stp_TempDeviceSiI9136 = &(gSt_OutputBoarSiI9136.St_DeviceSiI9136[gSt_OutputBoarSiI9136.B_ActiveChip]);
			Stp_TempDeviceSiI9136->B_BoardIndex = i + INPUT_BOARD_MAX;
			Stp_TempDeviceSiI9136->B_I2CBusIndex = 0;
			Stp_TempDeviceSiI9136->B_SubI2CBusIndex = 0;
			
			Stp_TempSiI9136Status_t = &(Stp_TempDeviceSiI9136->St_SiI9136GlobalStatus_t);
			Stp_TempSiI9136Status_t->txPowerState = TX_POWER_STATE_D0;
			Stp_TempSiI9136Status_t->OutputHaveSet = FALSE;
			Stp_TempSiI9136Status_t->tmdsPoweredUp = FALSE;
			Stp_TempSiI9136Status_t->hdmiCableConnected = FALSE;
			Stp_TempSiI9136Status_t->dsRxPoweredUp = FALSE;
			Stp_TempSiI9136Status_t->mobileHdCableConnected = TRUE;
			#ifdef MHD_CABLE_HPD
				Stp_TempSiI9136Status_t->tclkStable = FALSE;
				Stp_TempSiI9136Status_t->mobileHdCableConnected = FALSE;
				Stp_TempSiI9136Status_t->checkTclkStable = TRUE;
				Stp_TempSiI9136Status_t->cbusRevID = 0;
			#endif
			#ifdef DEV_SUPPORT_HDCP
			Stp_TempSiI9136Status_t->HDCP_TxSupports = FALSE;
			Stp_TempSiI9136Status_t->HDCP_AksvValid = FALSE;
			Stp_TempSiI9136Status_t->HDCP_Started = FALSE;
			Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel = EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;
			#endif
			#ifdef DEV_SUPPORT_EDID
				Stp_TempSiI9136Status_t->edidDataValid = FALSE;
				memset(&(Stp_TempSiI9136Status_t->EDID_Data), 0, sizeof(Type_EDID_Descriptors));
			#endif
			Stp_TempSiI9136Status_t->USRX_OutputChange = TRUE;
			Stp_TempSiI9136Status_t->g_HDCP_Override = FALSE;
			Stp_TempVideoModeDescription = &(Stp_TempSiI9136Status_t->VideoModeDescription);
			Stp_TempVideoModeDescription->VIC = 16; //1080p
			Stp_TempVideoModeDescription->AspectRatio = VMD_ASPECT_RATIO_16x9;
//					Stp_TempVideoModeDescription->ColorSpace = VMD_COLOR_SPACE_RGB;
			Stp_TempVideoModeDescription->ColorSpace = VMD_COLOR_SPACE_YCBCR422;
			Stp_TempVideoModeDescription->ColorDepth = VMD_COLOR_DEPTH_8BIT;
//					Stp_TempVideoModeDescription->HDCPAuthenticated = VMD_HDCP_AUTHENTICATED;
			Stp_TempVideoModeDescription->HDCPAuthenticated = VMD_HDCP_NOT_AUTHENTICATED;
			Stp_TempVideoModeDescription->HDMIVideoFormat = VMD_HDMIFORMAT_CEA_VIC;
			Stp_TempVideoModeDescription->ThreeDStructure = 0;
			Stp_TempVideoModeDescription->ThreeDExtData = 0;

			Stp_TempSiI9136Status_t->B_I2CIndex = gB_I2CIndex;
			Stp_TempSiI9136Status_t->B_TPI_BASE_Addr = gB_TX_SLAVE_Addr;
			Stp_TempSiI9136Status_t->B_TX_SLAVE_Addr = gB_TPI_BASE_Addr;
			
			txPowerState = Stp_TempSiI9136Status_t->txPowerState;
			OutputHaveSet = Stp_TempSiI9136Status_t->OutputHaveSet;
			tmdsPoweredUp = Stp_TempSiI9136Status_t->tmdsPoweredUp;
			hdmiCableConnected = Stp_TempSiI9136Status_t->hdmiCableConnected;
			dsRxPoweredUp = Stp_TempSiI9136Status_t->dsRxPoweredUp;
			mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
			#ifdef MHD_CABLE_HPD
				tclkStable = Stp_TempSiI9136Status_t->tclkStable;
				mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
				checkTclkStable = Stp_TempSiI9136Status_t->checkTclkStable;
				cbusRevID = Stp_TempSiI9136Status_t->cbusRevID;
			#endif
			#ifdef DEV_SUPPORT_HDCP
			HDCP_TxSupports = Stp_TempSiI9136Status_t->HDCP_TxSupports;
			HDCP_AksvValid = Stp_TempSiI9136Status_t->HDCP_AksvValid;
			HDCP_Started = Stp_TempSiI9136Status_t->HDCP_Started;
			HDCP_LinkProtectionLevel = Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel;
			#endif
			#ifdef DEV_SUPPORT_EDID
				edidDataValid = Stp_TempSiI9136Status_t->edidDataValid;
				memcpy(&EDID_Data, &(Stp_TempSiI9136Status_t->EDID_Data), sizeof(Type_EDID_Descriptors));
			#endif
			USRX_OutputChange = Stp_TempSiI9136Status_t->USRX_OutputChange;
			g_HDCP_Override = Stp_TempSiI9136Status_t->g_HDCP_Override;
			memcpy(&VideoModeDescription, Stp_TempVideoModeDescription, sizeof(VideoModeDescription));
			
			TPI_TRACE_PRINT(("Board %d , Output %d , DveAdd 0x%02X , Begin Init.\r\n", Stp_TempDeviceSiI9136->B_BoardIndex, Stp_TempSiI9136Status_t->B_I2CIndex, gB_TX_SLAVE_Addr));
			TPI_Init();
			
			Stp_TempSiI9136Status_t->txPowerState = txPowerState;
			Stp_TempSiI9136Status_t->OutputHaveSet = OutputHaveSet;
			Stp_TempSiI9136Status_t->tmdsPoweredUp = tmdsPoweredUp;
			Stp_TempSiI9136Status_t->hdmiCableConnected = hdmiCableConnected;
			Stp_TempSiI9136Status_t->dsRxPoweredUp = dsRxPoweredUp;
			Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
			#ifdef MHD_CABLE_HPD
				Stp_TempSiI9136Status_t->tclkStable = tclkStable;
				Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
				Stp_TempSiI9136Status_t->checkTclkStable = checkTclkStable;
				Stp_TempSiI9136Status_t->cbusRevID = cbusRevID;
			#endif
			#ifdef DEV_SUPPORT_HDCP
			Stp_TempSiI9136Status_t->HDCP_TxSupports = HDCP_TxSupports;
			Stp_TempSiI9136Status_t->HDCP_AksvValid = HDCP_AksvValid;
			Stp_TempSiI9136Status_t->HDCP_Started = HDCP_Started;
			Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel = HDCP_LinkProtectionLevel;
			#endif
			#ifdef DEV_SUPPORT_EDID
				Stp_TempSiI9136Status_t->edidDataValid = edidDataValid;
				memcpy(&(Stp_TempSiI9136Status_t->EDID_Data), &EDID_Data, sizeof(Type_EDID_Descriptors));
			#endif
			Stp_TempSiI9136Status_t->USRX_OutputChange = USRX_OutputChange;
			Stp_TempSiI9136Status_t->g_HDCP_Override = g_HDCP_Override;
			memcpy(Stp_TempVideoModeDescription, &VideoModeDescription, sizeof(VideoModeDescription));

			gSt_OutputBoarSiI9136.B_ActiveChip++;
			TPI_TRACE_PRINT(("Board %d , Output %d , DveAdd 0x%02X , Init Ok.\r\n", Stp_TempDeviceSiI9136->B_BoardIndex, Stp_TempSiI9136Status_t->B_I2CIndex, gB_TX_SLAVE_Addr));
			printf("Board %d , IIC %d , DveAdd 0x%02X , SiI9136 Init OK\r\n", Stp_TempDeviceSiI9136->B_BoardIndex, Stp_TempSiI9136Status_t->B_I2CIndex + 1, gB_TX_SLAVE_Addr);
		}
	}
	printf("InitOutputBoardSiI9136 OK! ActiveChip = %d\r\n", gSt_OutputBoarSiI9136.B_ActiveChip);
	TimerStart(SiI_9136_POLLING_TMR, 100);
}

void OutputBoardSiI9136Task(void)
{
	int i = 0;
	ST_DEVICE_SI_I9136 *Stp_TempDeviceSiI9136 = NULL;
	SiI9136_GlobalStatus_St *Stp_TempSiI9136Status_t = NULL;
	VideoModeDescription_t *Stp_TempVideoModeDescription = NULL;
	#if 1
	if(SI_I9136_PollInterruptFlag == 1)
	{
		for(i = 0; i < gSt_OutputBoarSiI9136.B_ActiveChip; i++)
		{
			Stp_TempDeviceSiI9136 = &(gSt_OutputBoarSiI9136.St_DeviceSiI9136[i]);
			Stp_TempSiI9136Status_t = &(Stp_TempDeviceSiI9136->St_SiI9136GlobalStatus_t);

			txPowerState = Stp_TempSiI9136Status_t->txPowerState;
			OutputHaveSet = Stp_TempSiI9136Status_t->OutputHaveSet;
			tmdsPoweredUp = Stp_TempSiI9136Status_t->tmdsPoweredUp;
			hdmiCableConnected = Stp_TempSiI9136Status_t->hdmiCableConnected;
			dsRxPoweredUp = Stp_TempSiI9136Status_t->dsRxPoweredUp;
			mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
			#ifdef MHD_CABLE_HPD
				tclkStable = Stp_TempSiI9136Status_t->tclkStable;
				mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
				checkTclkStable = Stp_TempSiI9136Status_t->checkTclkStable;
				cbusRevID = Stp_TempSiI9136Status_t->cbusRevID;
			#endif
			#ifdef DEV_SUPPORT_HDCP
			HDCP_TxSupports = Stp_TempSiI9136Status_t->HDCP_TxSupports;
			HDCP_AksvValid = Stp_TempSiI9136Status_t->HDCP_AksvValid;
			HDCP_Started = Stp_TempSiI9136Status_t->HDCP_Started;
			HDCP_LinkProtectionLevel = Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel;
			#endif
			#ifdef DEV_SUPPORT_EDID
				edidDataValid = Stp_TempSiI9136Status_t->edidDataValid;
				memcpy(&EDID_Data, &(Stp_TempSiI9136Status_t->EDID_Data), sizeof(Type_EDID_Descriptors));
			#endif
			USRX_OutputChange = Stp_TempSiI9136Status_t->USRX_OutputChange;
			g_HDCP_Override = Stp_TempSiI9136Status_t->g_HDCP_Override;
			gB_I2CIndex = Stp_TempSiI9136Status_t->B_I2CIndex;
			gB_TX_SLAVE_Addr = Stp_TempSiI9136Status_t->B_TX_SLAVE_Addr;
			gB_TPI_BASE_Addr = Stp_TempSiI9136Status_t->B_TPI_BASE_Addr;
			Stp_TempVideoModeDescription = &(Stp_TempSiI9136Status_t->VideoModeDescription);
			memcpy(&VideoModeDescription, Stp_TempVideoModeDescription, sizeof(VideoModeDescription));


//			if(SI_I9136_PollInterruptFlag == 1)
				TPI_Poll();
//StartTPI();

			Stp_TempSiI9136Status_t->txPowerState = txPowerState;
			Stp_TempSiI9136Status_t->OutputHaveSet = OutputHaveSet;
			Stp_TempSiI9136Status_t->tmdsPoweredUp = tmdsPoweredUp;
			Stp_TempSiI9136Status_t->hdmiCableConnected = hdmiCableConnected;
			Stp_TempSiI9136Status_t->dsRxPoweredUp = dsRxPoweredUp;
			Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
			#ifdef MHD_CABLE_HPD
				Stp_TempSiI9136Status_t->tclkStable = tclkStable;
				Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
				Stp_TempSiI9136Status_t->checkTclkStable = checkTclkStable;
				Stp_TempSiI9136Status_t->cbusRevID = cbusRevID;
			#endif
			#ifdef DEV_SUPPORT_HDCP
			Stp_TempSiI9136Status_t->HDCP_TxSupports = HDCP_TxSupports;
			Stp_TempSiI9136Status_t->HDCP_AksvValid = HDCP_AksvValid;
			Stp_TempSiI9136Status_t->HDCP_Started = HDCP_Started;
			Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel = HDCP_LinkProtectionLevel;
			#endif
			#ifdef DEV_SUPPORT_EDID
				Stp_TempSiI9136Status_t->edidDataValid = edidDataValid;
				memcpy(&(Stp_TempSiI9136Status_t->EDID_Data), &EDID_Data, sizeof(Type_EDID_Descriptors));
			#endif
			Stp_TempSiI9136Status_t->USRX_OutputChange = USRX_OutputChange;
			Stp_TempSiI9136Status_t->g_HDCP_Override = g_HDCP_Override;
			memcpy(Stp_TempVideoModeDescription, &VideoModeDescription, sizeof(VideoModeDescription));
		}
	}
	#endif
	#if 0
	for(i = 0; i < gSt_OutputBoarSiI9136.B_ActiveChip; i++)
	{
		Stp_TempDeviceSiI9136 = &(gSt_OutputBoarSiI9136.St_DeviceSiI9136[i]);
		Stp_TempSiI9136Status_t = &(Stp_TempDeviceSiI9136->St_SiI9136GlobalStatus_t);

		txPowerState = Stp_TempSiI9136Status_t->txPowerState;
		OutputHaveSet = Stp_TempSiI9136Status_t->OutputHaveSet;
		tmdsPoweredUp = Stp_TempSiI9136Status_t->tmdsPoweredUp;
		hdmiCableConnected = Stp_TempSiI9136Status_t->hdmiCableConnected;
		dsRxPoweredUp = Stp_TempSiI9136Status_t->dsRxPoweredUp;
		mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
		#ifdef MHD_CABLE_HPD
			tclkStable = Stp_TempSiI9136Status_t->tclkStable;
			mobileHdCableConnected = Stp_TempSiI9136Status_t->mobileHdCableConnected;
			checkTclkStable = Stp_TempSiI9136Status_t->checkTclkStable;
			cbusRevID = Stp_TempSiI9136Status_t->cbusRevID;
		#endif
		#ifdef DEV_SUPPORT_HDCP
		HDCP_TxSupports = Stp_TempSiI9136Status_t->HDCP_TxSupports;
		HDCP_AksvValid = Stp_TempSiI9136Status_t->HDCP_AksvValid;
		HDCP_Started = Stp_TempSiI9136Status_t->HDCP_Started;
		HDCP_LinkProtectionLevel = Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel;
		#endif
		#ifdef DEV_SUPPORT_EDID
			edidDataValid = Stp_TempSiI9136Status_t->edidDataValid;
			memcpy(&EDID_Data, &(Stp_TempSiI9136Status_t->EDID_Data), sizeof(Type_EDID_Descriptors));
		#endif
		USRX_OutputChange = Stp_TempSiI9136Status_t->USRX_OutputChange;
		g_HDCP_Override = Stp_TempSiI9136Status_t->g_HDCP_Override;
		gB_I2CIndex = Stp_TempSiI9136Status_t->B_I2CIndex;
		gB_TX_SLAVE_Addr = Stp_TempSiI9136Status_t->B_TX_SLAVE_Addr;
		gB_TPI_BASE_Addr = Stp_TempSiI9136Status_t->B_TPI_BASE_Addr;
		Stp_TempVideoModeDescription = &(Stp_TempSiI9136Status_t->VideoModeDescription);
		memcpy(&VideoModeDescription, Stp_TempVideoModeDescription, sizeof(VideoModeDescription));

//		BoardIICBusSwitch(Stp_TempDeviceSiI9136->B_BoardIndex);

		if(SI_I9136_PollInterruptFlag == 1)
			TPI_Poll();

		Stp_TempSiI9136Status_t->txPowerState = txPowerState;
		Stp_TempSiI9136Status_t->OutputHaveSet = OutputHaveSet;
		Stp_TempSiI9136Status_t->tmdsPoweredUp = tmdsPoweredUp;
		Stp_TempSiI9136Status_t->hdmiCableConnected = hdmiCableConnected;
		Stp_TempSiI9136Status_t->dsRxPoweredUp = dsRxPoweredUp;
		Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
		#ifdef MHD_CABLE_HPD
			Stp_TempSiI9136Status_t->tclkStable = tclkStable;
			Stp_TempSiI9136Status_t->mobileHdCableConnected = mobileHdCableConnected;
			Stp_TempSiI9136Status_t->checkTclkStable = checkTclkStable;
			Stp_TempSiI9136Status_t->cbusRevID = cbusRevID;
		#endif
		#ifdef DEV_SUPPORT_HDCP
		Stp_TempSiI9136Status_t->HDCP_TxSupports = HDCP_TxSupports;
		Stp_TempSiI9136Status_t->HDCP_AksvValid = HDCP_AksvValid;
		Stp_TempSiI9136Status_t->HDCP_Started = HDCP_Started;
		Stp_TempSiI9136Status_t->HDCP_LinkProtectionLevel = HDCP_LinkProtectionLevel;
		#endif
		#ifdef DEV_SUPPORT_EDID
			Stp_TempSiI9136Status_t->edidDataValid = edidDataValid;
			memcpy(&(Stp_TempSiI9136Status_t->EDID_Data), &EDID_Data, sizeof(Type_EDID_Descriptors));
		#endif
		Stp_TempSiI9136Status_t->USRX_OutputChange = USRX_OutputChange;
		Stp_TempSiI9136Status_t->g_HDCP_Override = g_HDCP_Override;
		memcpy(Stp_TempVideoModeDescription, &VideoModeDescription, sizeof(VideoModeDescription));
	}
	#endif
	if (TimerCheck(SiI_9136_POLLING_TMR) == TIMER_TMO)
	{
		SI_I9136_PollInterruptFlag = 1;
		TimerStart(SiI_9136_POLLING_TMR, 100);
	}
	else
		SI_I9136_PollInterruptFlag = 0;
}

/********************************  END  *************************************/
