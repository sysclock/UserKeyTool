///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <config.h>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/
//#ifdef _CONFIG_H_
//#define _CONFIG_H_

#ifdef EXTERN_HDCPROM
#pragma message("Defined EXTERN_HDCPROM")
#endif // EXTERN_HDCPROM

//#define SUPPORT_EDID
//#define SUPPORT_HDCP
#define SUPPORT_INPUTRGB
//#define SUPPORT_LVDS_6_BIT
#define SUPPORT_LVDS_8_BIT
//#define SUPPORT_LVDS_10_BIT

//#define SUPPORT_INPUTYUV444
//#define SUPPORT_INPUTYUV422
//#define IT626X_SSC
//#define SUPPORT_HBR_AUDIO
#define USE_SPDIF_CHSTAT
#define SUPPORT_MAP3
#define SUPPORT_DISO
#define SUPPORT_REGEN_TIMING
//#define SUPPORT_SPLASH
#define SUPPORT_AUDIO_MONITOR
#define SUPPORT_AUDI_AudSWL 24 //18;20;24
//#define Force_CTS

#define AudioOutDelayCnt 36//60

// #define SUPPORT_HBR_AUDIO
#define SUPPORT_I2S_AUDIO
// #define SUPPORT_SPDIF_AUDIO


// modify the input audio sample frequence here.
#define INPUT_AUDIO_SAMPLE_FREQ 48000L
#define INPUT_SAMPLE_FREQ AUDFS_48KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 32000L
// #define INPUT_SAMPLE_FREQ AUDFS_32KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 44100L
// #define INPUT_SAMPLE_FREQ AUDFS_44p1KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 88200L
// #define INPUT_SAMPLE_FREQ AUDFS_88p2KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 96000L
// #define INPUT_SAMPLE_FREQ AUDFS_96KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 176400L
// #define INPUT_SAMPLE_FREQ AUDFS_176p4KHz

// #define INPUT_AUDIO_SAMPLE_FREQ 192000L
// #define INPUT_SAMPLE_FREQ AUDFS_192KHz

#define OUTPUT_CHANNEL 2

//#define SUPPORT_GPIO_MUTE

#ifndef _MCU_8051_ // DSSSHA need large computation data rather than 8051 supported.
#define SUPPORT_DSSSHA
#endif

#if defined(SUPPORT_INPUTYUV444) || defined(SUPPORT_INPUTYUV422)
#define SUPPORT_INPUTYUV
#endif

//#endif
