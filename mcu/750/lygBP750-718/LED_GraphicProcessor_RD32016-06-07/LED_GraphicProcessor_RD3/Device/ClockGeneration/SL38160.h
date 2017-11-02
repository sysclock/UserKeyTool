#ifndef __SL38160_H__
#define __SL38160_H__

#include "stm32f1xx_hal.h"
#include "global.h"

#define	DEVICEID_SL38160_17		0xD2
#define	DEVICEID_SL38160_23		0xDA
#define	DEVICEID_SL38160_18		0xD4
#define	DEVICEID_SL38160_24		0xDC

typedef struct
{
	uint32_t W_StdHFreq;					// Standard H. frequency in 100 Hz unit
	uint32_t W_StdVTotal;					// Standard V. total in line unit
	uint8_t B_Data17;							// uint8_t 17
	uint8_t B_Data18;							// uint8_t 18
	uint8_t B_Data19;							// uint8_t 19
	uint8_t B_Data22;							// uint8_t 22
	uint8_t B_Data40;							// uint8_t 40
	uint8_t B_Data41;							// uint8_t 41
	uint8_t B_Data42;							// uint8_t 42
	uint8_t B_Data43;							// uint8_t 43
	uint8_t B_Data48;							// uint8_t 48
	uint8_t B_Data49;							// uint8_t 49
	uint8_t B_Data50;							// uint8_t 50
	uint8_t B_Data51;							// uint8_t 51
	uint8_t B_Data68;							// uint8_t 68
	uint8_t B_Data69;							// uint8_t 69
	uint8_t B_Data114;						// uint8_t 114
	uint8_t B_Data128;						// uint8_t 128
	uint8_t B_Data129;						// uint8_t 129
} SL38160PixelFreqModeStruct;

extern const uint8_t	MCLK_Regs[12][4];
extern const uint8_t	VCLK_Regs[7][9];
extern const uint8_t	PCLK_Regs[43][7];
extern const uint8_t	PCLK_Regs_Last5[5];

extern void Init_SL38160(void);
extern void Enable_SL38160(uint8_t B_DeviceID);
extern void PowerDown_SL38160(uint8_t B_DeviceID);
extern void Set_SL38160_CLK(uint8_t B_DeviceID, uint8_t audio_clk_num, uint8_t video_clk_num);
#endif /*SL38160_H_*/
