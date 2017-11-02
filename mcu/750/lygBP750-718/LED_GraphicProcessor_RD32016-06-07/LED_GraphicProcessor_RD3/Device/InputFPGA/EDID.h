 /*---------------------------------------------------------
EDID.h

Version:	0.00.1000

EDID �淶��صĽṹ���塣

0.00.1000 ���EDID 1.3�淶�����DDC������Ϣ�ṹ��

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
		//ͷ
	BYTE 	Header[8];		//8�ֽ�ͷ���̶���ʽΪ��0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00
		//������Ϣ
	unsigned short	VenderID;
	unsigned short	ProductID;
	unsigned int	SerialID;
	BYTE	MfgWeek;
	BYTE	MfgYear;
		//�汾��Ϣ
	BYTE	Version;
	BYTE	Revision;
		//��������
	BYTE	InputDef;
	BYTE	MaxHSize;
	BYTE	MaxVSize;
	BYTE	Gamma;
	BYTE	Feature;
		//��ɫ����
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
		//֧�ֵı�׼�ֱ���
	BYTE	Establish[2];
	BYTE	MfgRsvTim;
		//��׼�ֱ��ʲ�����8��
	unsigned short	Standard[8];
		//��ϸ�ֱ��ʲ�����4��
	DETAIL_TIMING	Detail[4];
		//β
	BYTE	ExtFlag;
	BYTE	Checksum;
}EDID_13, *P_EDID_13;

typedef struct _EDID_EXT
{
	BYTE	Data[128];		//��ʱ��������ṹ���պ���˵
}EDID_EXT, *P_EDID_EXT;

typedef struct _E_EDID
{
	EDID_13		EDID;
	EDID_EXT	Extend;		//ֻ֧��һ����չ��Ϣ��
}E_EDID, *P_E_EDID;

#endif		//End of EDID.h
