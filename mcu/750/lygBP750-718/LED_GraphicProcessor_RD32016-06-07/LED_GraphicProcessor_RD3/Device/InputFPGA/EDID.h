 /*---------------------------------------------------------
EDID.h

Version:	0.00.1000

EDID 规范相关的结构定义。

0.00.1000 完成EDID 1.3规范定义的DDC配置信息结构。

---------------------------------------------------------*/

#ifndef __EDID_H__
#define __EDID_H__

#include "global.h"

/*Constant*/


/*Structure*/
typedef struct _DETAIL_TIMING
{
	unsigned short	PixelFrq;		// Hz/10000
	BYTE	HActive;
	BYTE	HBlank;
	BYTE	HUpper;
	BYTE	VActive;
	BYTE	VBlank;
	BYTE	VUpper;
	BYTE	HSOffset;
	BYTE	HSWidth;
	BYTE	VSync;
	BYTE	SUpper;
	BYTE	HImage;
	BYTE	VImage;
	BYTE	IUpper;
	BYTE	HBorder;
	BYTE	VBorder;
	BYTE	Flags;
}DETAIL_TIMING, *P_DETAIL_TIMING;

typedef struct _EDID_13		//Little Endian
{
		//头
	BYTE 	Header[8];		//8字节头，固定格式为：0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00
		//厂商信息
	unsigned short	VenderID;
	unsigned short	ProductID;
	unsigned int	SerialID;
	BYTE	MfgWeek;
	BYTE	MfgYear;
		//版本信息
	BYTE	Version;
	BYTE	Revision;
		//基本参数
	BYTE	InputDef;
	BYTE	MaxHSize;
	BYTE	MaxVSize;
	BYTE	Gamma;
	BYTE	Feature;
		//颜色参数
	BYTE	RGLow;
	BYTE	BWLow;
	BYTE	Red_x;
	BYTE	Red_y;
	BYTE	Green_x;
	BYTE	Green_y;
	BYTE	Blue_x;
	BYTE	Blue_y;
	BYTE	White_x;
	BYTE	White_y;
		//支持的标准分辨率
	BYTE	Establish[2];
	BYTE	MfgRsvTim;
		//标准分辨率参数，8组
	unsigned short	Standard[8];
		//详细分辨率参数，4组
	DETAIL_TIMING	Detail[4];
		//尾
	BYTE	ExtFlag;
	BYTE	Checksum;
}EDID_13, *P_EDID_13;

typedef struct _EDID_EXT
{
	BYTE	Data[128];		//暂时不定义其结构，日后再说
}EDID_EXT, *P_EDID_EXT;

typedef struct _E_EDID
{
	EDID_13		EDID;
	EDID_EXT	Extend;		//只支持一个扩展信息块
}E_EDID, *P_E_EDID;

#endif		//End of EDID.h
