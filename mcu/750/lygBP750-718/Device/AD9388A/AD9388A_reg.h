/*
 * AD9388A.h
 *
 *  Created on: 2010-6-7
 *      Author: root
 */

#ifndef AD9388A_H_
#define AD9388A_H_

//==============================================================================
// MODULE:	AD9388A.h
//
// USAGE:	Header file contains function prototypes and definitions
//			related to ADC AD9388A driver
//
//******************************************************************************

#include "../../System Code/global.h"

//******************************************************************************
//  G L O B A L    D E F I N I T I O N S
//******************************************************************************

enum AD9983_SOURCE_CHANNEL
{
	AD9983_RGB_SOURCE_CHANNEL,
	AD9983_DVI_A_SOURCE_CHANNEL,
	AD9983_DVI_B_SOURCE_CHANNEL,
	MAX_AD9983_SOURCE_CHANNEL
};

enum PrimaryMode
{
	COMP_SD_HD_PR,
	ANALOG_GRAPHICS,
	HDMI_SD,
	HDMI_COMP,
	HDMI_GR
};

// Device address
#define AD9983_DEV_USER_MAP		0x40
#define AD9983_DEV_USER_MAP1	0x44
#define AD9983_DEV_USER_MAP2	0x60
#define AD9983_DEV_VDP_MAP		0x48
#define AD9983_DEV_RES_MAP		0x4C
#define AD9983_DEV_HDMI_MAP		0x68
#define AD9983_DEV_KSV_MAP		0x64
#define AD9983_DEV_EDID_MAP		0x6C

typedef struct
{
	BYTE B_Bank;
	BYTE B_RegAddr;
	BYTE B_Data;
}	ST_ADC9388_RegDataType;

//******************************************************************************
//	G L O B A L    F U N C T I O N    P R O T O T Y P E S
//******************************************************************************

extern void adc9388_Init(void);
extern void adc9388_Close(void);
extern void adc9388_SetInputChannel(BYTE B_InputChannel);
extern void ParseADC9388CCommad(int TCPConnectfd, BYTE* pCommad);
extern void adc9388_TaskHandle(void);
extern void adc9388_SetPhase(BYTE B_phaseVal);
extern void adc9388_SetGain(BYTE B_channel, WORD W_contrast);
extern void adc9388_SetOffset(BYTE B_channel, SWORD B_blackLevel);
extern void adc9388_SetClampLevel(WORD W_Clamplevel);
extern void adc9388_SetCapturePos(WORD W_HStart, WORD W_Width, WORD W_VStart, WORD W_Height);

//******************************************************************************
//  G L O B A L    D E F I N I T I O N S
//******************************************************************************
// Register address defs
// user map
#define	Output_Control_rw           	0x03
#define	Extended_Output_Control_rw		0x04
#define	Primary_Mode_rw					0x05
#define	Video_Standard_rw				0x06
#define	uss2_slave_address_rw			0x0E
#define	Power_Management_rw				0x0F
#define	Ident_r							0x11
#define	Status 2-r						0x12
#define	Vertical_Scale_Value_1_rw		0x1D
#define	Pixel_Delay_Control_rw			0x27
#define	Misc_Gain_Control_rw			0x2B
#define	AGC_Mode_Control_rw				0x2C
#define	Chroma_Gain_Control_1_w			0x2D
#define	Chroma_Gain_Control_2_w			0x2E
#define	Luma_Gain_Control_1_w			0x2F
#define	Luma_Gain_Control_2_w			0x30
#define	VSync_Field_Control 1_rw		0x31
#define	VSync_Field_Control_2_rw		0x32
#define	VSync_Field_Control_3_rw		0x33
#define	HSync_Position_Control_1_rw		0x34
#define	HSync_Position_Control_2_rw		0x35
#define	HSync_Position_Control_3_rw		0x36
#define	Polarity_rw						0x37
#define	NTSC_Comb_Control_rw			0x38
#define	PAL_Comb_Control_rw				0x39
#define	ADC_Control_rw					0x3A
#define	BIAS_Control					0x3B
#define	TLLC_Control_Analogue_rw		0x3C
#define	CSC_1_rw						0x52
#define	CSC_2_rw						0x53
#define	CSC_3_rw						0x54
#define	CSC_4_rw						0x55
#define	CSC_5_rw						0x56
#define	CSC_6_rw						0x57
#define	CSC_7_rw						0x58
#define	CSC_8_rw						0x59
#define	CSC_9_rw						0x5A
#define	CSC_10_rw						0x5B
#define	CSC_11_rw						0x5C
#define	CSC_12_rw						0x5D
#define	CSC_13_rw						0x5E
#define	CSC_14_rw						0x5F
#define	CSC_15_rw						0x60
#define	CSC_16_rw						0x61
#define	CSC_17_rw						0x62
#define	CSC_18_rw						0x63
#define	CSC_19_rw						0x64
#define	CSC_20_rw						0x65
#define	CSC_21_rw						0x66
#define	CSC_22_rw						0x67
#define	CSC_23_rw						0x68
#define	Config_1_rw						0x69
#define	TLLC_Phase_Adjust_rw			0x6A
#define	CP_Output_Selection_rw			0x6B
#define	CP_Clamp_1_rw					0x6C
#define	CP_Clamp_2_rw					0x6D
#define	CP_Clamp_3_rw					0x6E
#define	CP_Clamp_4_rw					0x6F
#define	CP_Clamp_5_rw					0x70
#define	CP_AGC_1_rw						0x71
#define	CP_AGC_2_rw						0x72
#define	CP_AGC_3_rw						0x73
#define	CP_AGC_4_rw						0x74
#define	CP_AGC_5_rw						0x75
#define	CP_AGC_6_rw						0x76
#define	CP_Offset_1_rw					0x77
#define	CP_Offset_2_rw					0x78
#define	CP_Offset_3_rw					0x79
#define	CP_Offset_4_rw					0x7A
#define	CP_AV_Control_rw				0x7B
#define	CP_HVF_Control_1_rw				0x7C
#define	CP_HVF_Control_2_rw				0x7D
#define	CP_HVF_Control_3_rw				0x7E
#define	CP_HVF_Control_4_rw				0x7F
#define	CP_HVF_Control5_rw				0x80
#define	CP_Measure_Control_1_rw			0x81
#define	CP_Measure_Control_2_rw			0x82
#define	CP_Measure_Control_3_rw			0x83
#define	CP_Measure_Control_4_rw			0x84
#define	CP_Detection_Control_1_rw		0x85
#define	CP_Misc_Control_1_rw			0x86
#define	CP_TLLC_Control_1_rw			0x87
#define	CP_TLLC_Control_2_rw			0x88
#define	CP_TLLC_Control_3_rw			0x89
#define	CP_TLLC_Control_4_rw			0x8A
#define	CP_DE_Ctrl_1_58_rw				0x8B
#define	CP_DE_Ctrl_2_59_rw				0x8C
#define	CP_DE_Ctrl_3_60_rw				0x8D
#define	CP_DE_Ctrl_4_61_rw				0x8E
#define	Free_Run_Line_Length_1_w		0x8F
#define	VBI_Info_r						0x90
#define	Free_Run_Line_Length_2_w		0x90
#define	DPP_CP_64_w						0x91
#define	RB_CP_AGC_1_r					0xA0
#define	RB_CP_AGC_2_r					0xA1
#define	RB_CP_Measure_1_r				0xA2
#define	DPP_CP_81_w						0xA2
#define	RB_CP_Measure_2_r				0xA3
#define	DPP_CP_82_w						0xA3
#define	RB_CP_Measure 3_r				0xA4
#define	DPP_CP_83_w						0xA4
#define	RB_CP_Measure_4_r				0xA5
#define	DPP_CP_84_w						0xA5
#define	RB_CP_Measure_5_r				0xA6
#define	DPP_CP_85_w						0xA6
#define	RB_CP_HSync_Depth_1_r			0xA7
#define	DPP_CP_86_w						0xA7
#define	RB_CP_HSync_Depth_2_r			0xA8
#define	DPP_CP_87_w						0xA8
#define	RB_CP_HSync_Depth_3_r			0xA9
#define	DPP_CP_88_w						0xA9
#define	RB_CP_HSync_Depth_4_r			0xAA
#define	DPP_CP_89_w						0xAA
#define	RB_CP_HSync_Depth_5_r			0xAB
#define	DPP_CP_90_w						0xAB
#define	RB_CP_HSync_Depth_6_r			0xAC
#define	DPP_CP_91_w						0xAC
#define	RB_CP_Peak_Video_1_r			0xAD
#define	RB_CP_Peak_Video_2_r			0xAE
#define	RB_CP_Peak_Video_3_r			0xAF
#define	RB_CP_Peak_Video_4_r			0xB0
#define	RB_Standard_Ident_1_r			0xB1
#define	RB_Standard_Ident_2_r			0xB2
#define	DPP_CP_97_w						0xB2
#define	RB_Standard_Ident_r				0xB3
#define	DPP_CP_98_w						0xB3
#define	RB_Standard_Ident_4_r			0xB4
#define	RB_Standard_Ident_5_r			0xB5
#define	DPP_CP_105_rw					0xBA
#define	DPP_CP_107_r					0xBC
#define	Cp_Debug_0_w					0xBD
#define	CP_DEF_COL_1_rw					0xBF
#define	CP_DEF_COL_2_rw					0xC0
#define	CP_DEF_COL_3_rw					0xC1
#define	CP_DEF_COL_4_rw					0xC2
#define	ADC_Switch_1_rw					0xC3
#define	ADC_Switch_2_rw					0xC4
#define	CP_Clamp_Pos_HS_Ctrl_1_rw		0xC5
#define	CP_Clamp_Pos_HS_Ctrl_2_rw		0xC6
#define	CP_Clamp_Pos_HS_Ctrl_3_rw		0xC7
#define	CP_Clamp_Pos_HS_Ctrl_4_rw		0xC8
#define	DPP_CP_118_rw					0xC9
#define	Field_Length_Count_1_r			0xCA
#define	Field_Length_Count_2_r			0xCB
#define	AFE_CONTROL_1					0xF3
#define	Drive_Strength_rw				0xF4
#define	VS_Mode_Control_rw				0xF9
#define	general_chip_ctl_rw				0xFD
#define	Pin_Checker_Op_rw				0xFE
// user map 1
#define	Interrupt_Configuration_0_rw	0x40
#define	Interrupt_Configuration_1_rw	0x41
#define	Interrupt_Status_1_r			0x42
#define	Interrupt_Clear_1_w				0x42
#define	Interrupt_2_Maskb_1_rw			0x43
#define	Interrupt_Maskb_1_rw			0x44
#define	Raw_Status_2_r					0x45
#define	Interrupt_Status_2_r			0x46
#define	Interrupt_Clear_2_w				0x46
#define	Interrupt_2_Maskb_2_rw			0x47
#define	Interrupt_Maskb_2_rw			0x48
#define	Raw_Status_3_r					0x49
#define	Interrupt_Status_3_r			0x4A
#define	Interrupt_Clear_3_w				0x4A
#define	Interrupt_2_Maskb_3_rw			0x4B
#define	Interrupt_Maskb_3_rw			0x4C
#define	Interrupt_Status_4_r			0x4E
#define	Interrupt_Clear_4_w				0x4E
#define	Interrupt_2_Maskb_4_rw			0x4F
#define	Interrupt_Maskb_4_rw			0x50
#define	HDMI_Raw_Status_1_r				0x60
#define	HDMI_Int_Status_1_r				0x61
#define	HDMI_Int_Clr_1_w				0x61
#define	HDMI_Int2_Maskb_1_rw			0x62
#define	HDMI_Int_Maskb_1_rw				0x63
#define	HDMI_Raw_Status_2_r				0x64
#define	HDMI_Int_Status_2_r				0x65
#define	HDMI_Int_Clr_2_w				0x65
#define	HDMI_Int2_Maskb_2_rw			0x66
#define	HDMI_Int_Maskb_2_rw				0x67
#define	HDMI_Raw_Status_3_r				0x68
#define	HDMI_Int_Status_3_r				0x69
#define	HDMI_Int_Clr_3_w				0x69
#define	HDMI_Int2_Maskb_3_rw			0x6A
#define	HDMI_Int_Maskb_3_rw				0x6B
#define	HDMI_Int_Status_4_r				0x6C
#define	HDMI_Int_Clr_4_w				0x6C
#define	HDMI_Int2_Maskb_4_rw			0x6D
#define	HDMI_Int_Maskb_4_w				0x6E
#define	HDMI_Int_Status_5_r				0x6F
#define	HDMI_Int_Clr_5_w				0x6F
#define	HDMI_Int2_Maskb_5_rw			0x70
#define	HDMI_Int_Maskb_5_rw				0x71
#define	HDMI_Status_6_r					0x72
#define	HDMI_Int_Clr_6_w				0x72
#define	HDMI_Int2_Maskb_6_rw			0x73
#define	HDMI_Int_Maskb_6_rw				0x74
#define	DLL_on_LLC_path_rw				0x75
#define	CP_CONTRAST_rw					0x9A
#define	CP_SATURATION_rw				0x9B
#define	CP_BRIGHTNESS_rw				0x9C
#define	CP_HUE_rw						0x9D
#define	VID_ADJ_BLANK_CTRL_rw			0x9E
// user map 2
#define	rsv_slave_address_rw			0xEA
#define	uss1_slave_address_rw			0xEB
#define	vdp_slave_address_rw			0xEC
#define	ksv_slave_address_rw			0xED
#define	edid_slave_address_rw			0xEE
#define	hdmi_slave_address_rw			0xEF
// HDMI map
#define	REGISTER_00H_rw				0x00
#define	REGISTER_01H_rw				0x01
#define	REGISTER_02H_rw				0x02
#define	REGISTER_03H_rw				0x03
#define	REGISTER_04H_r				0x04
#define	REGISTER_05H_r				0x05
#define	TMDSFREQ_r					0x06
#define	REGISTER_07H_r				0x07
#define	LINE_WIDTH_r				0x08
#define	FIELD_0_HEIGHT_MSB_r		0x09
#define	FIELD_0_HEIGHT_r			0x0A
#define	REGISTER_0BH_r				0x0B
#define	FIELD_1_HEIGHT_r			0x0C
#define	REGISTER_0DH_rw				0x0D
#define	AUDIO_MUTE_SPEED_rw			0x0F
#define	REGISTER_10H_rw				0x10
#define	AUDIO_FIFO_ALMOST_FULL_rw	0x11
#define	AUDIO_FIFO_ALMOST_EMPTY_rw	0x12
#define	AUDIO_COAST_MASK_rw			0x13
#define	MUTE_MASK_19_16_rw			0x14
#define	MUTE_MASK_15_8_rw			0x15
#define	MUTE_MASK_7_0_rw			0x16
#define	PACKETS_DETECTED_MSB_r		0x17
#define	PACKETS_DETECTED_r			0x18
#define	PACKET_STATUS_FLAGS_20_r	0x19
#define	PACKET_STATUS_FLAGS 15_r	0x1A
#define	PACKET_STATUS_FLAGS_7_0_r	0x1B
#define	REGISTER_1CH_rw				0x1C
#define	REGISTER_1DH_rw				0x1D
#define	TOTAL_LINE_WIDTH_MSB_r		0x1E
#define	TOTAL_LINE_WIDTH_r			0x1F
#define	HSYNC_FRONT_PORCH_MS_r		0x20
#define	HSYNC_FRONT_PORCH_r			0x21
#define	HSYNC_PULSE_WIDTH_MSB_r		0x22
#define	HSYNC_PULSE_WIDTH_r			0x23
#define	HSYNC_BACK_PORCH_MSB_r		0x24
#define	HSYNC_BACK_PORCH_r			0x25
#define	FIELD0_TOTAL_HEIGHT_MS_r	0x26
#define	FIELD0_TOTAL_HEIGHT_r		0x27
#define	FIELD1_TOTAL_HEIGHT_MS_r	0x28
#define	FIELD1_TOTAL_HEIGHT_r		0x29
#define	FIELD0_VS_FRONT_PORCH_MSB_r	0x2A
#define	FIELD0_VS_FRONT_PORCH_r		0x2B
#define	FIELD1_VS_FRONT_PORCH_MSB_r	0x2C
#define	FIELD1_VS_FRONT_PORCH_r		0x2D
#define	FIELD0_VS_PULSE_WIDTH_MSB_r	0x2E
#define	FIELD0_VS_PULSE_WIDTH_r		0x2F
#define	FIELD1_VS_PULSE_WIDTH_MSB_r	0x30
#define	FIELD1_VS_PULSE_WIDTH_r		0x31
#define	FIELD0_VS_BACK_PORCH_M_r	0x32
#define	FIELD0_VS_BACK_PORCH_r		0x33
#define	FIELD1_VS_BACK_PORCH_M_r	0x34
#define	FIELD1_VS_BACK_PORCH_r		0x35
#define	CHANNEL_STATUS_DATA_0_r		0x36
#define	CHANNEL_STATUS_DATA_1_r		0x37
#define	CHANNEL_STATUS_DATA_2_r		0x38
#define	CHANNEL_STATUS_DATA_3_r		0x39
#define	CHANNEL_STATUS_DATA_4_r		0x3A
#define	PLL_DIVIDER_rw				0x3C
#define	REGISTER_48H_rw				0x48
#define	REGISTER_5A_w				0x5A
#define	CTS_REG1_r					0x5B
#define	CTS_REG2_r					0x5C
#define	CTS_REG3_N_REG_r			0x5D
#define	N_REG2_r					0x5E
#define	N_REG2						0x5F
#define	GAMUT_MDATA_BODY_0_r		0x60
#define	GAMUT_MDATA_BODY_1_r		0x61
#define	GAMUT_MDATA_BODY_2_r		0x62
#define	GAMUT_MDATA_BODY_3_r		0x63
#define	GAMUT_MDATA_BODY_4_r		0x64
#define	GAMUT_MDATA_BODY_5_r		0x65
#define	GAMUT_MDATA_BODY_6_r		0x66
#define	GAMUT_MDATA_BODY_7_r		0x68
#define	GAMUT_MDATA_BODY_8_r		0x69
#define	GAMUT_MDATA_BODY_9_r		0x6A
#define	GAMUT_MDATA_BODY_10_r		0x6B
#define	GAMUT_MDATA_BODY_11_r		0x6C
#define	GAMUT_MDATA_BODY_12_r		0x6D
#define	GAMUT_MDATA_BODY_13_r		0x6E
#define	GAMUT_MDATA_BODY_14_r		0x70
#define	GAMUT_MDATA_BODY_15_r		0x71
#define	GAMUT_MDATA_BODY_16_r		0x72
#define	GAMUT_MDATA_BODY_17_r		0x73
#define	GAMUT_MDATA_BODY_18_r		0x74
#define	GAMUT_MDATA_BODY_19_r		0x75
#define	GAMUT_MDATA_BODY_20_r		0x76
#define	GAMUT_MDATA_BODY_21_r		0x78
#define	GAMUT_MDATA_BODY_22_r		0x79
#define	GAMUT_MDATA_BODY_23_r		0x7A
#define	GAMUT_MDATA_BODY_24_r		0x7B
#define	GAMUT_MDATA_BODY_25_r		0x7C
#define	GAMUT_MDATA_BODY_26_r		0x7D
#define	GAMUT_MDATA_BODY_27_r		0x7E
#define	AVI_INF_VERS_r				0x80
#define	AVI_DATA_BYTE_1_r			0x81
#define	AVI_INF_BYTE_2_r			0x82
#define	AVI_INF_BYTE_3_r			0x83
#define	AVI_INF_BYTE_4_r			0x84
#define	AVI_INF_BYTE_5_r			0x85
#define	AVI_INF_BYTE_6_r			0x86
#define	AVI_INF_BYTE_7_r			0x88
#define	AVI_INF_BYTE_8_r			0x89
#define	AVI_INF_BYTE_9_r			0x8A
#define	AVI_INF_BYTE_10_r			0x8B
#define	AVI_INF_BYTE_11_r			0x8C
#define	AVI_INF_BYTE_12_r			0x8D
#define	AVI_INF_BYTE_13_r			0x8E
#define	AUDIO_INF_VERS_r			0x90
#define	AUDIO_INF_BYTE_1_r			0x91
#define	AUDIO_INF_BYTE_2_r			0x92
#define	AUDIO_INF_BYTE_3_r			0x93
#define	AUDIO_INF_BYTE_4_r			0x94
#define	AUDIO_INF_BYTE_5_r			0x95
#define	SOURCE_PROD_INF_VERS_r		0x98
#define	SOURCE_PROD_INF_BYTE_00_r	0x99
#define	SOURCE_PROD_INF_BYTE_01_r	0x9A
#define	SOURCE_PROD_INF_BYTE_02_r	0x9B
#define	SOURCE_PROD_INF_BYTE_03_r	0x9C
#define	SOURCE_PROD_INF_BYTE_04_r	0x9D
#define	SOURCE_PROD_INF_BYTE_05_r	0x9E
#define	SOURCE_PROD_INF_BYTE_06_r	0xA0
#define	SOURCE_PROD_INF_BYTE_07_r	0xA1
#define	SOURCE_PROD_INF_BYTE_08_r	0xA2
#define	SOURCE_PROD_INF_BYTE_09_r	0xA3
#define	SOURCE_PROD_INF_BYTE_10_r	0xA4
#define	SOURCE_PROD_INF_BYTE_11_r	0xA5
#define	SOURCE_PROD_INF_BYTE_12_r	0xA6
#define	SOURCE_PROD_INF_BYTE_13_r	0xA8
#define	SOURCE_PROD_INF_BYTE_14_r	0xA9
#define	SOURCE_PROD_INF_BYTE_15_r	0xAA
#define	SOURCE_PROD_INF_BYTE_16_r	0xAB
#define	SOURCE_PROD_INF_BYTE_17_r	0xAC
#define	SOURCE_PROD_INF_BYTE_18_r	0xAD
#define	SOURCE_PROD_INF_BYTE_19_r	0xAE
#define	SOURCE_PROD_INF_BYTE_20_r	0xB0
#define	SOURCE_PROD_INF_BYTE_21_r	0xB1
#define	SOURCE_PROD_INF_BYTE_22_r	0xB2
#define	SOURCE_PROD_INF_BYTE_23_r	0xB3
#define	SOURCE_PROD_INF_BYTE_24_r	0xB4
#define	MPEG_SOURCE_INF_VERS_r		0xB8
#define	MPEG_SOURCE_INF_BYTE_1_r	0xB9
#define	MPEG_SOURCE_INF_BYTE_2_r	0xBA
#define	MPEG_SOURCE_INF_BYTE_3_r	0xBB
#define	MPEG_SOURCE_INF_BYTE_4_r	0xBC
#define	MPEG_SOURCE_INF_BYTE_5_r	0xBD
#define	ACP_TYPE_r					0xC0
#define	ACP_PACKET_BYTE_0_r			0xC1
#define	ACP_PACKET_BYTE_1_r			0xC2
#define	ACP_PACKET_BYTE_2_r			0xC3
#define	ACP_PACKET_BYTE_3_r			0xC4
#define	ACP_PACKET_BYTE_4_r			0xC5
#define	ACP_PACKET_BYTE_5_r			0xC6
#define	ACP_PACKET_BYTE_6_r			0xC8
#define	ACP_PACKET_BYTE_7_r			0xC9
#define	ACP_PACKET_BYTE_8_r			0xCA
#define	ACP_PACKET_BYTE_9_r			0xCB
#define	ACP_PACKET_BYTE_10_r		0xCC
#define	ACP_PACKET_BYTE_11_r		0xCD
#define	ACP_PACKET_BYTE_12_r		0xCE
#define	ACP_PACKET_BYTE_13_r		0xD0
#define	ACP_PACKET_BYTE_14_r		0xD1
#define	ACP_PACKET_BYTE_15_r		0xD2
#define	ACP_PACKET_BYTE_16_r		0xD3
#define	ACP_PACKET_BYTE_17_r		0xD4
#define	ACP_PACKET_BYTE_18_r		0xD5
#define	ACP_PACKET_BYTE_19_r		0xD6
#define	ISRC1_INF_r					0xD8
#define	ISRC1_PACKET_BYTE_0_r		0xD9
#define	ISRC1_PACKET_BYTE_1_r		0xDA
#define	ISRC1_PACKET_BYTE_2_r		0xDB
#define	ISRC1_PACKET_BYTE_3_r		0xDC
#define	ISRC1_PACKET_BYTE_4_r		0xDD
#define	ISRC1_PACKET_BYTE_5_r		0xDE
#define	ISRC1_PACKET_BYTE_6_r		0xE0
#define	ISRC1_PACKET_BYTE_7_r		0xE1
#define	ISRC1_PACKET_BYTE_8_r		0xE2
#define	ISRC1_PACKET_BYTE_9_r		0xE3
#define	ISRC1_PACKET_BYTE_10_r		0xE4
#define	ISRC1_PACKET_BYTE_11_r		0xE5
#define	ISRC1_PACKET_BYTE_12_r		0xE6
#define	ISRC1_PACKET_BYTE_13_r		0xE8
#define	ISRC1_PACKET_BYTE_14_r		0xE9
#define	ISRC1_PACKET_BYTE_15_r		0xEA
#define	ISRC2_PACKET_BYTE_0_r		0xEB
#define	ISRC2_PACKET_BYTE_1_r		0xEC
#define	ISRC2_PACKET_BYTE_2_r		0xED
#define	ISRC2_PACKET_BYTE_3_r		0xEE
#define	ISRC2_PACKET_BYTE_4_r		0xF0
#define	ISRC2_PACKET_BYTE_5_r		0xF1
#define	ISRC2_PACKET_BYTE_6_r		0xF2
#define	ISRC2_PACKET_BYTE_7_r		0xF3
#define	ISRC2_PACKET_BYTE_8_r		0xF4
#define	ISRC2_PACKET_BYTE_9_r		0xF5
#define	ISRC2_PACKET_BYTE_10_r		0xF6
#define	ISRC2_PACKET_BYTE_11_r		0xF8
#define	ISRC2_PACKET_BYTE_12_r		0xF9
#define	ISRC2_PACKET_BYTE_13_r		0xFA
#define	ISRC2_PACKET_BYTE_14_r		0xFB
#define	ISRC2_PACKET_BYTE_15_r		0xFC
#define	ISRC2_PACKET_BYTE_16_r		0xFD
#define	ISRC2_PACKET_BYTE_17_r		0xFE

// Repeater KSV Map
#define	BKSV_0_r					0x00
#define	BKSV_1_r					0x01
#define	BKSV_2_r					0x02
#define	BKSV_3_r					0x03
#define	BKSV_4_r					0x04
#define	Ri_0_r						0x08
#define	Ri_1_r						0x09
#define	Pj_r						0x0A
#define	AKSV_0_rw					0x10
#define	AKSV_1_rw					0x11
#define	AKSV_2_rw					0x12
#define	AKSV_3_rw					0x13
#define	AKSV_4_rw					0x14
#define	Ainfo_rw					0x15
#define	AN_0_rw						0x18
#define	AN_1_rw						0x19
#define	AN_2_rw						0x1A
#define	AN_3_rw						0x1B
#define	AN_4_rw						0x1C
#define	AN_5_rw						0x1D
#define	AN_6_rw						0x1E
#define	AN_7_rw						0x20
#define	SHA_A_1_rw					0x21
#define	SHA_A_2_rw					0x22
#define	SHA_A_3_rw					0x23
#define	SHA_B_0_rw					0x24
#define	SHA_B_1_rw					0x25
#define	SHA_B_2_rw					0x26
#define	SHA_B_3_rw					0x27
#define	SHA_C_0_rw					0x28
#define	SHA_C_1_rw					0x29
#define	SHA_C_2_rw					0x2A
#define	SHA_C_3_rw					0x2B
#define	SHA_D_0_rw					0x2C
#define	SHA_D_1_rw					0x2D
#define	SHA_D_2_rw					0x2E
#define	SHA_D_3_rw					0x2F
#define	SHA_E_0_rw					0x30
#define	SHA_E_1_rw					0x31
#define	SHA_E_2_rw					0x32
#define	SHA_E_3_rw					0x33
#define	Bcaps_rw					0x40
#define	BStatus_0_rw				0x41
#define	BStatus_1_rw				0x42
#define	SPA_PortB_A_B_rw			0x70
#define	SPA_PortB_C_D_rw			0x71
#define	SPA_Location_rw				0x72
#define	CTRL_BITS_rw				0x73
#define	PortB_Chksum_rw				0x76
#define	KSV0_B0_rw					0x80
#define	KSV0_B1_rw					0x81
#define	KSV0_B2_rw					0x82
#define	KSV0_B3_rw					0x83
#define	KSV0_B4_rw					0x84
#define	KSV1_B0_rw					0x85
#define	KSV1_B1_rw					0x86
#define	KSV1_B2_rw					0x87
#define	KSV1_B3_rw					0x88
#define	KSV1_B4_rw					0x89
#define	KSV2_B0_rw					0x8A
#define	KSV2_B1_rw					0x8B
#define	KSV2_B2_rw					0x8C
#define	KSV2_B3_rw					0x8D
#define	KSV2_B4_rw					0x8E
#define	KSV3_B0_rw					0x8F
#define	KSV3_B1_rw					0x90
#define	KSV3_B2_rw					0x91
#define	KSV3_B3_rw					0x92
#define	KSV3_B4_rw					0x93
#define	KSV4_B0_rw					0x94
#define	KSV4_B1_rw					0x95
#define	KSV4_B2_rw					0x96
#define	KSV4_B3_rw					0x97
#define	KSV4_B4_rw					0x98
#define	KSV5_B0_rw					0x99
#define	KSV5_B1_rw					0x9A
#define	KSV5_B2_rw					0x9B
#define	KSV5_B3_rw					0x9C
#define	KSV5_B4_rw					0x9D
#define	KSV6_B0_rw					0x9E
#define	KSV6_B1_rw					0x9F
#define	KSV6_B2_rw					0xA0
#define	KSV6_B3_rw					0xAA
#define	KSV6_B4_rw					0xA2
#define	KSV7_B0_rw					0xA3
#define	KSV7_B1_rw					0xA4
#define	KSV7_B2_rw					0xA5

#define	KSV7_B3_rw					0xA6
#define	KSV7_B4_rw					0xA7
#define	KSV8_B0_rw					0xA8
#define	KSV8_B1_rw					0xA9
#define	KSV8_B2_rw					0xAA
#define	KSV8_B3_rw					0xAB
#define	KSV8_B4_rw					0xAC
#define	KSV9_B0_rw					0xAD
#define	KSV9_B1_rw					0xAE
#define	KSV9_B2_rw					0xAF
#define	KSV9_B3_rw					0xB0
#define	KSV9_B4_rw					0xB1
#define	KSV10_B0_rw					0xB2
#define	KSV10_B1_rw					0xB3
#define	KSV10_B2_rw					0xB4
#define	KSV10_B3_rw					0xB5
#define	KSV10_B4_rw					0xB6
#define	KSV11_B0_rw					0xB7
#define	KSV11_B1_rw					0xB8
#define	KSV11_B2_rw					0xB9
#define	KSV11_B3_rw					0xBA
#define	KSV11_B4_rw					0xBB
#define	KSV12_B0_rw					0xBC
#define	KSV12_B1_rw					0xBD
#define	KSV12_B2_rw					0xBE
#define	KSV12_B3_rw					0xBF
#define	KSV12_B4_rw					0xC0
#define	KSV13_B0_rw					0xC1
#define	KSV13_B1_rw					0xC2
#define	KSV13_B2_rw					0xC3
#define	KSV13_B3_rw					0xC4
#define	KSV13_B4_rw					0xC5
#define	KSV14_B0_rw					0xC6
#define	KSV14_B1_rw					0xC7
#define	KSV14_B2_rw					0xC8
#define	KSV14_B3_rw					0xC9
#define	KSV14_B4_rw					0xCA
#define	KSV15_B0_rw					0xCB
#define	KSV15_B1_rw					0xCC
#define	KSV15_B2_rw					0xCD
#define	KSV15_B3_rw					0xCE
#define	KSV15_B4_rw					0xCF
#define	KSV16_B0_rw					0xD0
#define	KSV16_B1_rw					0xD1
#define	KSV16_B2_rw					0xD2
#define	KSV16_B3_rw					0xD3
#define	KSV16_B4_rw					0xD4
#define	KSV17_B0_rw					0xD5
#define	KSV17_B1_rw					0xD6
#define	KSV17_B2_rw					0xD7
#define	KSV17_B3_rw					0xD8
#define	KSV17_B4_rw					0xD9
#define	KSV18_B0_rw					0xDA
#define	KSV18_B1_rw					0xDB
#define	KSV18_B2_rw					0xDC
#define	KSV18_B3_rw					0xDD
#define	KSV18_B4_rw					0xDE
#define	KSV19_B0_rw					0xDF
#define	KSV19_B1_rw					0xE0
#define	KSV19_B2_rw					0xE1
#define	KSV19_B3_rw					0xE2
#define	KSV19_B4_rw					0xE3
#define	KSV20_B0_rw					0xE4
#define	KSV20_B1_rw					0xE5
#define	KSV20_B2_rw					0xE6
#define	KSV20_B3_rw					0xE7
#define	KSV20_B4_rw					0xE8
#define	KSV21_B0_rw					0xE9
#define	KSV21_B1_rw					0xEA
#define	KSV21_B2_rw					0xEB
#define	KSV21_B3_rw					0xEC
#define	KSV21_B4_rw					0xED
#define	KSV22_B0_rw					0xEE
#define	KSV22_B1_rw					0xEF
#define	KSV22_B2_rw					0xF0
#define	KSV22_B3_rw					0xF1
#define	KSV22_B4_rw					0xF2
#define	KSV23_B0_rw					0xF3
#define	KSV23_B1_rw					0xF4
#define	KSV23_B2_rw					0xF5
#define	KSV23_B3_rw					0xF6
#define	KSV23_B4_rw					0xF7


#endif /* AD9388A_H_ */
