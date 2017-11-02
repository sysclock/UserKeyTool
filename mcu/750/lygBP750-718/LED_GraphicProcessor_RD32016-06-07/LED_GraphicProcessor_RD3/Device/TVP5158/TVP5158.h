#ifndef __TVP5158_H__
#define __TVP5158_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      TVP5158.h                                                   */
/*																			*/
/* USAGE:       Header file for module TVP5158.c								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
typedef enum
{
	TVP5158_DEV0,
	TVP5158_DEV1,
	TVP5158_DEV2,
	TVP5158_DEV3,
	TVP5158_DEV4,
	TVP5158_DEV5,
	TVP5158_DEV_MAX
} TVP5158_DEV;

typedef enum
{
	TVP5158_DECODER0,
	TVP5158_DECODER1,
	TVP5158_DECODER2,
	TVP5158_DECODER3,
	TVP5158_DECODER_MAX
} TVP5158_DECODER;

typedef enum
{
	TVP5158_NTSC,
	TVP5158_PAL,
	TVP5158_NO_SIGNAL,
} TVP5158_SIGNAL_TYPE;

typedef struct														// Structure for TVP5158 One Channel Status
{
	BYTE B_LineAlternating;									// 0 Non line alternating, 	1 Line alternating
	BYTE B_FieldRateStatus;									// 0 60 Hz, 1 50 Hz
	BYTE B_ColorSubcarrierLockStatus;				// 0 Color subcarrier is not locked, Color subcarrier is locked
	BYTE B_VerticalSyncLockStatus;					// 0 Vertical sync is not locked, 1 Vertical sync is locked
	BYTE B_HorizontalSyncLockStatus;				// 0 Horizontal sync is not locked, 1 Horizontal sync is locked
	BYTE B_TV_VCRStatus;										// 0 TV, 1 VCR
	BYTE B_SignalPresent;										// 0 Signal is not present, 1 Signal is present
	BYTE B_WeakSignalDetection;							// 0 No weak signal, 1 Weak signal mode
	BYTE B_PAL_SwitchPolarity;							// 0 PAL switch is zero, 1 PAL switch is one
	BYTE B_FieldSequenceStatus;							// 0 Even field, 1 Odd field
	BYTE B_ColorKilled;											// 0 Color killer is not active, 1 Color killer is active
	BYTE B_MacroVisionDetection;						// 
	BYTE B_VideoStandardStatus;							// 01h (M, J) NTSC, 02h (B, D, G, H, I, N) PAL, 03h (M) PAL, 04h (Combination-N) PAL
																					// 05h NTSC 4.43, 07h PAL 60
	BYTE B_VideoStandardSelect;							// 00h CVBS Autoswitch mode (default), 01h (M, J) NTSC, 02h (B, D, G, H, I, N) PAL,
																					// 03h (M) PAL, 04h (Combination-N) PAL
																					// 05h NTSC 4.43, 07h PAL 60
	BYTE B_AutoContrastMode;								// 00h Enabled, 01h Reserved, 02h User Mode, 03h Disabled (default)
	BYTE B_LuminanceBrightness;							// 0 (dark), 128 (default), 255 (bright)
	BYTE B_LuminanceContrast;								// 0 (minimum contrast), 128 (default), 255 (maximum contrast)
	BYTE B_ChrominanceSaturation;						// 0 (no color), 128 (default), 255 (maximum)
	BYTE B_ChrominanceHue;									// 0000 0000 -180бу, 1000 0000 0бу (default), 1111 1111 +180бу
} ST_TVP5158_CHANNEL_STATUS;

typedef struct														// Structure for TVP5158 Status
{
	BYTE B_ChannelIndex;										// Channel Index
	ST_TVP5158_CHANNEL_STATUS St_TVP5158ChannelStatus[TVP5158_DEV_MAX];
} ST_TVP5158_STATUS;

#define	Pixel_Interleaved_Mode		1

#define MCVIP_VIDEO_DECODER_MODE_2CH_D1                    0   ///< Video decoder mode: 2CH D1 
#define MCVIP_VIDEO_DECODER_MODE_4CH_D1                    1   ///< Video decoder mode: 4CH D1
#define MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1               2   ///< Video decoder mode: 4CH Half-D1
#define MCVIP_VIDEO_DECODER_MODE_4CH_CIF                   3   ///< Video decoder mode: 4CH CIF
#define MCVIP_VIDEO_DECODER_MODE_8CH_CIF                   4   ///< Video decoder mode: 8CH CIF
#define MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_PLUS_D1       5   ///< Video decoder mode: 4CH Half-D1 + D1
#define MCVIP_VIDEO_DECODER_MODE_4CH_CIF_PLUS_D1           6   ///< Video decoder mode: 4CH CIF     + D1
#define MCVIP_VIDEO_DECODER_MODE_8CH_CIF_PLUS_D1           7   ///< Video decoder mode: 8CH CIF     + D1
#define MCVIP_VIDEO_DECODER_MODE_2CH_D1_CROP               8   ///< Video decoder mode: 2CH D1 CROPPED
#define MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_CROP          9   ///< Video decoder mode: 4CH Half-D1 CROPPED
#define MCVIP_VIDEO_DECODER_MODE_4CH_CIF_CROP              10   ///< Video decoder mode: 4CH CIF CROPPED
#define MCVIP_VIDEO_DECODER_MODE_8CH_CIF_CROP             11   ///< Video decoder mode: 8CH CIF CROPPED
#define MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_PLUS_D1_CROP 12   ///< Video decoder mode: 4CH Half-D1 + D1 CROPPED
#define MCVIP_VIDEO_DECODER_MODE_4CH_CIF_PLUS_D1_CROP     13   ///< Video decoder mode: 4CH CIF     + D1 CROPPED
#define MCVIP_VIDEO_DECODER_MODE_8CH_CIF_PLUS_D1_CROP     14   ///< Video decoder mode: 8CH CIF     + D1 CROPPED
#define MCVIP_VIDEO_DECODER_MODE_MAX                      15   ///< Video decoder mode: Max

#define TVP5158_I2C_ADD_DEV0		0xB2						// Dev 0 I2C =1, ID = 0xB2
#define TVP5158_I2C_ADD_DEV1		0xB0						// Dev 1 I2C =1, ID = 0xB0
#define TVP5158_I2C_ADD_DEV2		0xB6						// Dev 2 I2C =0, ID = 0xB6
#define TVP5158_I2C_ADD_DEV3		0xB4						// Dev 3 I2C =0, ID = 0xB4
#define TVP5158_I2C_ADD_DEV4		0xB2						// Dev 4 I2C =0, ID = 0xB2
#define TVP5158_I2C_ADD_DEV5		0xB0						// Dev 5 I2C =0, ID = 0xB0

#define TVP5158_DEV_STATUS1											0x00						// TVP5158 Reg: STATUS1
#define TVP5158_DEV_STATUS2											0x01						// TVP5158 Reg: STATUS2
#define TVP5158_CHIP_ID_MSB											0x08
#define TVP5158_CHIP_ID_LSB											0x09
#define TVP5158_VIDEO_STANDARD_STATUS						0x0C
#define TVP5158_LUMINANCE_BRIGHTNESS						0x10
#define TVP5158_LUMINANCE_CONTRAST							0x11
#define TVP5158_LUMINANCE_SATURATION						0x13
#define TVP5158_LUMINANCE_HUE										0x14
#define TVP5158_VSYNC_FILTER_SHIFT							0x85
#define TVP5158_656_VERSION_F_BIT_CONTROL				0x87
#define TVP5158_FBIT_VBIT_DECODE_CONTROL				0x88
#define TVP5158_FBIT_VBIT_CONTROL								0x89
#define TVP5158_OUTPUT_FORMATTER_CONTROL_2			0xA9
#define TVP5158_AVD_OUTPUT_CONTROL_1						0xB0
#define TVP5158_AVD_OUTPUT_CONTROL_2						0xB1
#define TVP5158_OFM_MODE_CONTROL								0xB2
#define TVP5158_OFM_CHANNEL_SELECT_1						0xB3
#define TVP5158_OFM_CHANNEL_SELECT_2						0xB4
#define TVP5158_OFM_CHANNEL_SELECT_3						0xB5
#define TVP5158_OFM_SUPER_FRAME_SIZE_LSB				0xB6
#define TVP5158_OFM_SUPER_FRAME_SIZE_MSB				0xB7
#define TVP5158_MISC_OFM_CONTROL								0xBA
#define TVP5158_AUDIO_SAMPLE_RATE_CONTROL				0xC0
#define TVP5158_ANALOG_AUDIO_GAIN_CONTROL_1			0xC1
#define TVP5158_ANALOG_AUDIO_GAIN_CONTROL_2			0xC2
#define TVP5158_AUDIO_MODE_CONTROL							0xC3
#define TVP5158_AUDIO_MIXER_SELECT							0xC4
#define TVP5158_AUDIO_MUTE_CONTROL							0xC5
#define TVP5158_AUDIO_MIXING_RATIO_CONTROL_1		0xC6
#define TVP5158_AUDIO_MIXING_RATIO_CONTROL_2		0xC7
#define TVP5158_AUDIO_CASCADE_MODE_CONTROL			0xC8
#define TVP5158_VBUS_DATA_ACCESS				0xE0
#define TVP5158_VBUS_ADDRESS_ACCESS			0xE8
#define TVP5158_DECODER_WRITE_ENABLE		0xFE
#define TVP5158_DECODER_READ_ENABLE			0xFF
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
extern BYTE TVP5158_GetStatus(BYTE B_ChipIndex, BYTE chNum);
extern BYTE TVP5158_GetSignalType(BYTE B_ChipIndex, BYTE chNum);
extern BYTE TVP5158_SelectRead(BYTE B_ChipIndex, BYTE B_Data);
extern BYTE TVP5158_SelectWrite(BYTE B_ChipIndex, BYTE B_Data);
extern BYTE TVP5158_VbusWrite(BYTE B_ChipIndex, WORD vbus_addr, BYTE B_Data, BYTE len);
extern BYTE TVP5158_OfmReset(BYTE B_ChipIndex);
extern BYTE TVP5158_SoftReset(BYTE B_ChipIndex);
extern BYTE TVP5158_CheckChipId(BYTE B_ChipIndex);
extern BYTE TVP5158_SetOfmMode(BYTE B_ChipIndex, BYTE mode, BYTE videoIfMode, BYTE videoSystem, BYTE cascadeStage);
extern BYTE TVP5158_SetAudioMode(BYTE B_ChipIndex, BYTE mode, BYTE cascadeStage);
extern BYTE TVP5158_SetAudVol(BYTE B_ChipIndex, BYTE vol, BYTE channel);
extern BYTE TVP5158_setAudMute(BYTE B_ChipIndex, BYTE enableMute, BYTE channel);
extern BYTE TVP5158_SetLuminanceBrightness(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
extern BYTE TVP5158_SetLuminanceContrast(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
extern BYTE TVP5158_SetLuminanceSaturation(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
extern BYTE TVP5158_SetLuminanceHue(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
extern WORD TVP5158_GetLuminanceBrightness(BYTE B_ChipIndex, BYTE channel);
extern WORD TVP5158_GetLuminanceContrast(BYTE B_ChipIndex, BYTE channel);
extern WORD TVP5158_GetLuminanceSaturation(BYTE B_ChipIndex, BYTE channel);
extern WORD TVP5158_GetLuminanceHue(BYTE B_ChipIndex, BYTE channel);
extern WORD TVP5158_ReadByte (BYTE B_ChipIndex, BYTE B_Addr);
extern BYTE TVP5158_WriteByte (BYTE B_ChipIndex, BYTE B_Addr, BYTE B_Data);
extern BYTE TVP5158_WriteData(BYTE B_ChipIndex, BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len);

#endif
