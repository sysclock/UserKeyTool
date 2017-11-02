#ifndef __SL38160_H__
#define __SL38160_H__

#include "../../System Code/global.h"
#include "../../MultiSignalBoard/StandardSignalMode.h"

#define	DEVICEID_SL38160_17		0xD2
#define	DEVICEID_SL38160_23		0xDA
#define	DEVICEID_SL38160_18		0xD4
#define	DEVICEID_SL38160_24		0xDC

typedef struct
{
	int W_StdHFreq;							// Standard H. frequency in 100 Hz unit
	int W_StdVTotal;						// Standard V. total in line unit
	BYTE B_Data17;							// BYTE 17
	BYTE B_Data18;							// BYTE 18
	BYTE B_Data19;							// BYTE 19
	BYTE B_Data22;							// BYTE 22
	BYTE B_Data40;							// BYTE 40
	BYTE B_Data41;							// BYTE 41
	BYTE B_Data42;							// BYTE 42
	BYTE B_Data43;							// BYTE 43
	BYTE B_Data48;							// BYTE 48
	BYTE B_Data49;							// BYTE 49
	BYTE B_Data50;							// BYTE 50
	BYTE B_Data51;							// BYTE 51
	BYTE B_Data68;							// BYTE 68
	BYTE B_Data69;							// BYTE 69
	BYTE B_Data114;							// BYTE 114
	BYTE B_Data128;							// BYTE 128
	BYTE B_Data129;							// BYTE 129
} SL38160PixelFreqModeStruct;

extern const SL38160PixelFreqModeStruct  SL38160PixelFreqModeStructTable[NumberOfPresetModes];
extern const BYTE	MCLK_Regs[12][4];
extern const BYTE	VCLK_Regs[7][9];
extern const BYTE	PCLK_Regs[43][7];
extern const BYTE	PCLK_Regs_Last5[5];

extern void Init_SL38160(void);
extern void Enable_SL38160(BYTE B_DeviceID);
extern void PowerDown_SL38160(BYTE B_DeviceID);
extern void Set_SL38160_CLK(BYTE B_DeviceID, BYTE audio_clk_num, BYTE video_clk_num);
#endif /*SL38160_H_*/
