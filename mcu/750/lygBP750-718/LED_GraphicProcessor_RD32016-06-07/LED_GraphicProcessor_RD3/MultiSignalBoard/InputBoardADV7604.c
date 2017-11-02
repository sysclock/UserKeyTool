#define __INPUT_BOARD_ADV7604_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      InputBoardADV7604.c                                                      */
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
#include <stdint.h>                           /* Include standard types */

//#include "type.h"
#include "Serial.h"
#include "global.h"
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "D_IIC3.h"
#include "InputBoardADV7604.h"
#include "../MultiSignalBoard/Param.h"
#include "../Device/InputFPGA/InputFPGA.h"

#define    DVI_IN_HPD_CON      (1 << 16)

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
//extern volatile unsigned short gD_ADV7604TickCnt;
ST_INPUT_BOARD_ADV7604 gSt_InputBoardADV7604;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void InitInputBoardADV7604(void);
void InputBoardADV7604Task(void);
BYTE InputBoardADV7604GetIndexBySoltIndex(BYTE B_SoltIndex);
void InputBoardADV7604ProgramEDID(BYTE B_InputSlotIndex, unsigned char EdidMode);
void InputBoardADV7604SelectInputPort(BYTE B_InputSlotIndex, BYTE InputPortIndex);
void InputBoardADV7604SelectInputTiming(BYTE B_InputSlotIndex, BYTE InputTimingIndex);
void InputBoardADV7604PrintLogStatus(BYTE B_InputSlotIndex);
int InputBoardADV7604GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight);
void InputBoardADV7604SetCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal);
void InputBoardADV7604SetDVCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
static BYTE ADV7604_PollInterruptFlag = 0;
/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static uint16_t ADV7604_TimeGetTickCounter(void);

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
void InitInputBoardADV7604(void)
{
	int i = 0, TempData = 0;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	struct adv7604_platform_data *Stp_ADV7604_platform_pdata = NULL;

	ADV7604_PollInterruptFlag = 1;
	memset(&gSt_InputBoardADV7604, 0, sizeof(gSt_InputBoardADV7604));
	for(i = 0; i < DEVICE_ADV7604_MAX; i++)
	{
		Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[gSt_InputBoardADV7604.B_ActiveChip]);
		Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
		memset(Stp_TempADV7604Status_t, 0, sizeof(struct adv7604_state));
		TempData = ParamRead(NVR_ADV7604_DEV0_PORT + i);
		if((TempData >= 0) && (TempData <= ADV7604_MODE_HDMI))
			Stp_TempADV7604Status_t->mode = (enum adv7604_mode) TempData; 		//ADV7604_MODE_GR;
		else
			Stp_TempADV7604Status_t->mode = ADV7604_MODE_HDMI; 		//ADV7604_MODE_GR;
		Stp_TempADV7604Status_t->rgb_quantization_range = V4L2_DV_RGB_RANGE_AUTO;
		Stp_TempADV7604Status_t->prev_input_status = 1;
		Stp_ADV7604_platform_pdata = &(Stp_TempADV7604Status_t->pdata);
		Stp_ADV7604_platform_pdata->disable_pwrdnb = 1;
		Stp_ADV7604_platform_pdata->disable_cable_det_rst = 1;
		Stp_ADV7604_platform_pdata->ain_sel = ADV7604_AIN7_8_9_NC_SYNC_3_1;
		Stp_ADV7604_platform_pdata->op_ch_sel = ADV7604_OP_CH_SEL_RGB;
		Stp_ADV7604_platform_pdata->op_format_sel = ADV7604_OP_FORMAT_SEL_SDR_ITU656_16;
		Stp_ADV7604_platform_pdata->alt_gamma = 0;
		Stp_ADV7604_platform_pdata->op_656_range = 0;			// 0 -- (0~255), 1--(0x10-0xFF) 656 data range
		Stp_ADV7604_platform_pdata->rgb_out = 0;
		Stp_ADV7604_platform_pdata->alt_data_sat = 0;
		Stp_ADV7604_platform_pdata->blank_data = 1;
		Stp_ADV7604_platform_pdata->insert_av_codes = 1;
		Stp_ADV7604_platform_pdata->replicate_av_codes = 0;
		Stp_ADV7604_platform_pdata->invert_cbcr = 0;
		Stp_ADV7604_platform_pdata->output_bus_lsb_to_msb = 0;
		Stp_ADV7604_platform_pdata->hdmi_free_run_mode = 0;
		Stp_ADV7604_platform_pdata->i2c_avlink = ADV7604_AVLINK_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_cec = ADV7604_CEC_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_infoframe = ADV7604_INFOFRAME_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_esdp = ADV7604_ESDP_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_dpp = ADV7604_DPP_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_afe = ADV7604_AFE_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_repeater = ADV7604_KSV_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_edid = ADV7604_EDID_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_hdmi = ADV7604_HDMI_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_test = ADV7604_TEST_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_cp = ADV7604_CP_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_vdp = ADV7604_VDP_SLAVE_ADDR;
		Stp_ADV7604_platform_pdata->i2c_busindex = DEVICE_ADV7604_MAX - i - 1;
		if(ADV7604_Init(Stp_TempADV7604Status_t) == 0xFF)
			continue;
		TempData = ParamRead(NVR_ADV7604_DEV0_EDID + i);
		ADV7604_LoadEdid(Stp_TempADV7604Status_t, TempData);
		
		TempData = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (i * 4));
		adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_BASE, TempData);
		TempData = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (i * 4) + 1);
		adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_BASE + 1, TempData);
		TempData = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (i * 4) + 2);
		adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_BASE + 2, TempData);
		TempData = ParamRead(NVR_ADV7604_DEV0_BRIGHTNESS + (i * 4) + 3);
		adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_BASE + 3, TempData);

//					ADV7604_SetRouting(Stp_TempADV7604Status_t, Stp_TempADV7604Status_t->mode, 0, 24);
		Stp_TempDeviceADV7604->B_BoardIndex = i;
		Stp_TempDeviceADV7604->B_I2CBusIndex = 0;
		Stp_TempDeviceADV7604->B_SubI2CBusIndex = 0;
		gSt_InputBoardADV7604.B_ActiveChip++;
//					printf("Board %d , Input %d , ADV7604 Init OK Stp_TempADV7604Status_t = 0x%X\r\n", Stp_TempDeviceADV7604->B_BoardIndex, k, Stp_TempADV7604Status_t);
		printf("Board %d , Input %d , ADV7604 Init OK\r\n", Stp_TempDeviceADV7604->B_BoardIndex, 0);
	}
	Stp_TempADV7604Status_t = &gSt_ADV7604State;
	memset(Stp_TempADV7604Status_t, 0, sizeof(struct adv7604_state));
	Stp_TempADV7604Status_t->mode = ADV7604_MODE_HDMI; 		//ADV7604_MODE_GR;
	Stp_TempADV7604Status_t->rgb_quantization_range = V4L2_DV_RGB_RANGE_AUTO;
	Stp_TempADV7604Status_t->prev_input_status = 1;
	Stp_ADV7604_platform_pdata = &(Stp_TempADV7604Status_t->pdata);
	Stp_ADV7604_platform_pdata->disable_pwrdnb = 1;
	Stp_ADV7604_platform_pdata->disable_cable_det_rst = 1;
	Stp_ADV7604_platform_pdata->ain_sel = ADV7604_AIN7_8_9_NC_SYNC_3_1;
	Stp_ADV7604_platform_pdata->op_ch_sel = ADV7604_OP_CH_SEL_RGB;
	Stp_ADV7604_platform_pdata->op_format_sel = ADV7604_OP_FORMAT_SEL_SDR_ITU656_16;
	Stp_ADV7604_platform_pdata->alt_gamma = 0;
	Stp_ADV7604_platform_pdata->op_656_range = 0;						// 0 -- (0~255), 1--(0x10-0xFF) 656 data range
	Stp_ADV7604_platform_pdata->rgb_out = 0;
	Stp_ADV7604_platform_pdata->alt_data_sat = 0;
	Stp_ADV7604_platform_pdata->blank_data = 1;
	Stp_ADV7604_platform_pdata->insert_av_codes = 1;
	Stp_ADV7604_platform_pdata->replicate_av_codes = 0;
	Stp_ADV7604_platform_pdata->invert_cbcr = 0;
	Stp_ADV7604_platform_pdata->output_bus_lsb_to_msb = 0;
	Stp_ADV7604_platform_pdata->hdmi_free_run_mode = 0;
	Stp_ADV7604_platform_pdata->i2c_avlink = ADV7604_AVLINK_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_cec = ADV7604_CEC_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_infoframe = ADV7604_INFOFRAME_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_esdp = ADV7604_ESDP_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_dpp = ADV7604_DPP_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_afe = ADV7604_AFE_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_repeater = ADV7604_KSV_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_edid = ADV7604_EDID_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_hdmi = ADV7604_HDMI_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_test = ADV7604_TEST_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_cp = ADV7604_CP_SLAVE_ADDR;
	Stp_ADV7604_platform_pdata->i2c_vdp = ADV7604_VDP_SLAVE_ADDR;
	printf("InitInputBoardADV7604 OK! ActiveChip = %d\r\n", gSt_InputBoardADV7604.B_ActiveChip);

	for(i = 0; i < INPUT_BOARD_MAX; i++)
	{
		// Set HPD Control High
		if(i == 0)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
		Delay10ms(100);
		// Set HPD Control Low
		if(i == 0)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	}
	
	TimerStart(ADV7604_POLLING_TMR, 50);
}

void InputBoardADV7604Task(void)
{
	int i = 0;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	unsigned char TempHandled = 0;
	
	if(ADV7604_PollInterruptFlag == 1)
	{
		for(i = 0; i < gSt_InputBoardADV7604.B_ActiveChip; i++)
		{
			Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
			Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
//			printf("Stp_TempADV7604Status_t = 0x%X\r\n", Stp_TempADV7604Status_t);
			adv7604_isr(Stp_TempADV7604Status_t, 0, &TempHandled);
		}
	}
//	ADV7604_TimerTask();
	if (TimerCheck(ADV7604_POLLING_TMR) == TIMER_TMO)
	{
		ADV7604_PollInterruptFlag = 1;
		TimerStart(ADV7604_POLLING_TMR, 50);
	}
	else
		ADV7604_PollInterruptFlag = 0;
}

BYTE InputBoardADV7604GetIndexBySoltIndex(BYTE B_SoltIndex)
{
	BYTE i = 0;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	if(B_SoltIndex >= INPUT_BOARD_MAX)
		return 0xFF;
	for(i = 0; i < gSt_InputBoardADV7604.B_ActiveChip; i++)
	{
		Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
		if(Stp_TempDeviceADV7604->B_BoardIndex == B_SoltIndex)
			return i;
	}
	return 0xFF;
}

void InputBoardADV7604ProgramEDID(BYTE B_InputSlotIndex, unsigned char EdidMode)
{
	BYTE i = 0;
	WORD W_TempData = 0;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	if(B_InputSlotIndex == 0xFF)
	{
		for(i = 0; i < gSt_InputBoardADV7604.B_ActiveChip; i++)
		{
			W_TempData = ParamRead(NVR_ADV7604_DEV0_EDID + i);
			if(W_TempData == EdidMode)
				continue;
			Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
			Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
			ADV7604_WriteEdidToProm(Stp_TempADV7604Status_t, EdidMode);
			// Set HPD Control High
			if(i == 0)
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
			Delay10ms(100);
			// Set HPD Control Low
			if(i == 0)
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
			else
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
			ParamWrite(NVR_ADV7604_DEV0_EDID + i, EdidMode);		// write EDID
		}
	}
	else
	{
		i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
		if(i == 0xFF)
			return;
		W_TempData = ParamRead(NVR_ADV7604_DEV0_EDID + i);
		if(W_TempData == EdidMode)
			return;
		Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
		Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
		ADV7604_WriteEdidToProm(Stp_TempADV7604Status_t, EdidMode);
		
		// Set HPD Control High
		if(i == 0)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
		Delay10ms(100);
		// Set HPD Control Low
		if(i == 0)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		#if 1
		ParamWrite(NVR_ADV7604_DEV0_EDID + B_InputSlotIndex, EdidMode);		// write EDID
		#endif
	}
}

void InputBoardADV7604SelectInputPort(BYTE B_InputSlotIndex, BYTE InputPortIndex)
{
	BYTE i = 0;
	WORD W_TempData = 0;
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	W_TempData = ParamRead(NVR_ADV7604_DEV0_PORT + B_InputSlotIndex);
	if(W_TempData == InputPortIndex)
		return;
	i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return;
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	i = (BYTE) ADV7604_SetRouting(Stp_TempADV7604Status_t, InputPortIndex, 0, 0);
	if(i == 1)
		ParamWrite(NVR_ADV7604_DEV0_PORT + B_InputSlotIndex, InputPortIndex);		// write InputPort
}

void InputBoardADV7604SelectInputTiming(BYTE B_InputSlotIndex, BYTE InputTimingIndex)
{
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	BYTE i = 0;
	i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return;
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	ADV7604_SetTimings(Stp_TempADV7604Status_t, InputTimingIndex);
}

void InputBoardADV7604PrintLogStatus(BYTE B_InputSlotIndex)
{
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	BYTE i = 0;
	i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return;
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	adv7604_log_status(Stp_TempADV7604Status_t);
	#if 1
	{
		long TempWidth = 0, TempHeight = 0;
		if(InputBoardADV7604GetSignalWidthHeight(i, &TempWidth, &TempHeight) == 0xFF)
			printf("InputBoardADV7604GetSignal No Signal! Width = %d, Height = %d\r\n", TempWidth, TempHeight);
		else
			printf("InputBoardADV7604GetSignal Width = %d, Height = %d\r\n", TempWidth, TempHeight);
	}
	#endif
}

void InputBoardADV7604SetCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal)
{
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	BYTE i = 0;
	i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return;
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_BASE + CtrlId, CtrlVal);
	ParamWrite(NVR_ADV7604_DEV0_BRIGHTNESS + (B_InputSlotIndex * 4) + CtrlId, (WORD) CtrlVal);		//
}

void InputBoardADV7604SetDVCtrl(BYTE B_InputSlotIndex, int CtrlId, unsigned long CtrlVal)
{
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	BYTE i = 0;
	i = InputBoardADV7604GetIndexBySoltIndex(B_InputSlotIndex);
	if(i == 0xFF)
		return;
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[i]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	adv7604_set_ctrl(Stp_TempADV7604Status_t, V4L2_CID_DV_CLASS_BASE + CtrlId, CtrlVal);
}

int InputBoardADV7604GetSignalWidthHeight(BYTE B_InputIndex, long *pWidth, long *pHeight)
{
	ST_DEVICE_ADV7604 *Stp_TempDeviceADV7604 = NULL;
	struct adv7604_state *Stp_TempADV7604Status_t = NULL;
	if(B_InputIndex >= gSt_InputBoardADV7604.B_ActiveChip)
	{
		*pWidth = 1024;
		*pHeight = 768;
		return 0xFF;
	}
	Stp_TempDeviceADV7604 = &(gSt_InputBoardADV7604.St_DeviceADV7604[B_InputIndex]);
	Stp_TempADV7604Status_t = &(Stp_TempDeviceADV7604->St_ADV7604GlobalStatus_t);
	if(ADV7604_GetSignalActiveWidthHeight(Stp_TempADV7604Status_t, pWidth, pHeight) == 0)
	{
		*pWidth = 0;
		*pHeight = 0;
		return 1;
	}
	return 1;
}

static uint16_t ADV7604_TimeGetTickCounter(void)
{
	uint16_t rv;
	rv = gD_ADV7604TickCnt;
	return rv;
}
/****************************************************************************/
/*																			*/
/* FUNCTION:    InputBoardADV7604_DummyCall(void)                            */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void InputBoardADV7604_DummyCall(void)
{
	ADV7604_TimeGetTickCounter();
}
/********************************  END  *************************************/
