#ifndef _ADV7441_I2C_
#define _ADV7441_I2C_

#include "stm32f10x.h"
#include "common.h"


#ifdef _ADV7441_I2C_C_
  #define _ADV7441_I2C_DEC_
#else
  #define _ADV7441_I2C_DEC_ extern
#endif

#define USER_MAP_ADDR           0x42
#define ADT7441_EDID_ADDR      0x6E
#define ADV7441_USER_MAP_ADDR           0x42
#define ADV7441_USER_MAP1_ADDR         0x46
#define ADV7441_USER_MAP2_ADDR         0x62
#define ADV7441_USER_MAP3_ADDR         0x4A
#define ADV7441_HDMI_MAP_ADDR           0x6A
#define ADV7441_KSV_MAP_ADDR             0x66

typedef enum{
	DEVICE_PORT_DVI,
	DEVICE_PORT_HDMI,
	DEVICE_PORT_VGA,
	DEVICE_PORT_YPbPr,
	DEVICE_PORT_CVBS1,
	DEVICE_PORT_CVBS2,
	DEVICE_PORT_SDI,
	DEVICE_PORT_AUTO,
}PortID;

/*
struct VCAP_VIDEO_SOURCE_CH_STATUS_S
{
	PortID inputPort;
	int isVideoDetect;
	int frameWidth;
	int frameHeight;
	int frameHz;
	int isInterlaced;
};*/
	

typedef enum{
	DEVICE_STD_NTSC,
	DEVICE_STD_PAL,
	DEVICE_STD_480I,
	DEVICE_STD_576I,

	DEVICE_STD_CIF, 
	DEVICE_STD_HALF_D1,
	DEVICE_STD_D1,

	DEVICE_STD_480P,
	DEVICE_STD_576P,
	DEVICE_STD_720P_60,    //9
	DEVICE_STD_720P_50,
	DEVICE_STD_1080I_60,
	DEVICE_STD_1080I_50,
	DEVICE_STD_1080P_60,
	DEVICE_STD_1080P_50,
	DEVICE_STD_1080P_25,
	DEVICE_STD_1080P_30,   //16
	
	
	DEVICE_STD_VGA_640X480X60,
	DEVICE_STD_VGA_640X480X72,
	DEVICE_STD_VGA_640X480X75,
	DEVICE_STD_VGA_640X480X85,        //20
	DEVICE_STD_VGA_800X600X60,
	DEVICE_STD_VGA_800X600X72,
	DEVICE_STD_VGA_800X600X75,
	DEVICE_STD_VGA_800X600X85,
	DEVICE_STD_VGA_1024X768X60,    //25
	DEVICE_STD_VGA_1024X768X70,
	DEVICE_STD_VGA_1024X768X75,
	DEVICE_STD_VGA_1024X768X85,
	DEVICE_STD_VGA_1280X720X60,    //29
	DEVICE_STD_VGA_1280X768X60,
	DEVICE_STD_VGA_1280X768X75,
	DEVICE_STD_VGA_1280X768X85,
	DEVICE_STD_VGA_1280X800X60,
	DEVICE_STD_VGA_1280X960X60,
	DEVICE_STD_VGA_1280X1024X60,
	DEVICE_STD_VGA_1280X1024X75,
	DEVICE_STD_VGA_1280X1024X85,
	DEVICE_STD_VGA_1366X768X60,
	DEVICE_STD_VGA_1440X900X60,
	DEVICE_STD_VGA_1400X1050X60,
	DEVICE_STD_VGA_1400X1050X75,
	DEVICE_STD_VGA_1600X1200X60,
	DEVICE_STD_VGA_1920X1080X60_DMT,
	DEVICE_STD_VGA_1920X1080X60_GTF,
	DEVICE_STD_VGA_1920X1200X60,
	DEVICE_STD_VGA_2560X1440X60,

	DEVICE_STD_MUX_2CH_D1,
	DEVICE_STD_MUX_2CH_HALF_D1,
	DEVICE_STD_MUX_2CH_CIF,
	DEVICE_STD_MUX_4CH_D1,
	DEVICE_STD_MUX_4CH_CIF, 
	DEVICE_STD_MUX_4CH_HALF_D1,
	DEVICE_STD_MUX_8CH_CIF,
	DEVICE_STD_MUX_8CH_HALF_D1,
	DEVICE_STD_AUTO_DETECT,
	DEVICE_STD_CUSTOM, 
	DEVICE_STD_MAX,
	DEVICE_STD_LAST,
}_Divice_Std_Idx;

typedef struct{
	unsigned char acName[20];
	int hpv;
	int vps;
	int htotal;
	int width;
	int hight;	
}Vps_Adv7441_hpv_vps;

typedef struct{
	unsigned char acName[20];
	int sav;
	int eav;
	int s_vbi;
	int e_vbi;
}Vps_Adv7441_sav_eav_vbi;

typedef struct{
	unsigned char name[20];
	unsigned char config[11];
}Vps_Adv7441_InMode;

typedef struct
{
	int TMDS_A;
	int TMDS_B;
	unsigned char Interlaced;
	unsigned short HTotPix;
	unsigned short VTotPix;
	unsigned short HFreq;
	unsigned short VFreq;
	

} ADV7441_SyncInfo;


typedef enum{
	AD7441_INPUT_UNKNOW,
	AD7441_DIGITAL_INPUT,
	AD7441_YPbPr_INPUT,
	AD7441_ANALOG_INPUT,
}AD7441_INPUT;

typedef enum{
	DEVICE_SOURCE_ALL,
	DEVICE_SOURCE_CP,
	DEVICE_SOURCE_CVBS,
}Device_Source_CH;

S32 Device_adv7441InvertCbCr (void);
S32 Device_adv7441OutputEnable (  bool enable );
S32 Device_adv7441Reset (void);
static U8 Device_adv7441_Read8_Usermap( U8 RegAddr);
static U8 Device_adv7441_Read8_Usermap1( U8 RegAddr);
static U8 Device_adv7441_Read8_Usermap2( U8 RegAddr);
static U8 Device_adv7441_Read8_Usermap3( U8 RegAddr);
static U8 Device_adv7441_Read8_Hdmimap( U8 RegAddr);
static U8 Device_adv7441_Read8_Ksvmap( U8 RegAddr);

static S32 Device_adv7441_Write8_Usermap( U8 RegAddr, U8 RegVal);
static S32 Device_adv7441_Write8_Usermap1( U8 RegAddr, U8 RegVal);
static S32 Device_adv7441_Write8_Usermap2( U8 RegAddr, U8 RegVal);
static S32 Device_adv7441_Write8_Usermap3( U8 RegAddr, U8 RegVal);
static S32 Device_adv7441_Write8_Hdmimap( U8 RegAddr, U8 RegVal);
static S32 Device_adv7441_Write8_Ksvmap( U8 RegAddr, U8 RegVal);

static S32 Device_adv7441GetTMDS_A( unsigned int *tmds_a);
static S32 Device_adv7441GetTMDS_B( unsigned int *tmds_b);

S32 Device_adv7441GetVideoStatus (void);
S32 Device_adv7441InitPort( PortID port);
//void VGA2_Auto(void);

#endif
