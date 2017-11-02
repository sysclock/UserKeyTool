#define __IT66121_TASK_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      IT66121Task.c                                      */
/*																			*/
/* USAGE:       This module contains functions for IT66121.	*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "../../System Code/Timer.h"
#include "typedef.h"
#include "hdmitx_sys.h"
#include "hdmitx_drv.h"
#include "IT66121IO.h"
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S																				*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    																			*/
/****************************************************************************/
void IT66121_InitDev(void);
void IT66121_TaskHandle(void);

/****************************************************************************/
/*	S T A T I C   V A R I A B L E S																					*/
/****************************************************************************/
static BYTE IT66121_PollInterruptFlag = 0;

/****************************************************************************/
/*	S T A T I C   R O U T I N E S																						*/
/****************************************************************************/


/****************************************************************************/
/*	C O D E																																	*/
/****************************************************************************/
void IT66121_InitDev(void)
{
	BYTE VenderID[4] = {0};
	if(HDMITX_ReadI2C_ByteN(REG_TX_VENDOR_ID0, VenderID, 4) == ER_FAIL)
		return;
	if((VenderID[0] != 0x54) || (VenderID[1] != 0x49) || (VenderID[2] != 0x12) || ((VenderID[3] & 0x0F) != 0x06))
	{
		printf("%s VenderID = 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", __func__, VenderID[0], VenderID[1], VenderID[2], VenderID[3]);
		return;
	}
	InitHDMITX_Variable();
	InitHDMITX();
//	HDMITX_ChangeDisplayOption(HDMI_720p60, HDMI_RGB444) ;
	HDMITX_ChangeDisplayOption(HDMI_1080p60, HDMI_RGB444) ;
	printf("HDMITX_ChangeDisplayOption(HDMI_1080p60,HDMI_RGB444) ;\r\n");
	printf("IT66121 Init OK\r\n");
	TimerStart(SiI_9136_POLLING_TMR, 100);
}

void IT66121_TaskHandle(void)
{
	if(IT66121_PollInterruptFlag == 1)
		HDMITX_DevLoopProc();
	if (TimerCheck(SiI_9136_POLLING_TMR) == TIMER_TMO)
	{
		IT66121_PollInterruptFlag = 1;
		TimerStart(SiI_9136_POLLING_TMR, 100);
	}
	else
		IT66121_PollInterruptFlag = 0;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    IT66121Task_DummyCall(void)                                 */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void IT66121_DummyCall(void)
{

}
/********************************  END  *************************************/
