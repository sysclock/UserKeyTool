
/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "header.h"

#define _ADV7441_I2C_C_

//#define	DEVICE_ADV7441_DEBUG

#ifdef DEVICE_ADV7441_DEBUG
#define ADV_OSA_printf printf
#else
#define ADV_OSA_printf(...)
#endif

#define FXTAL						28636360
#define RETRYNUMS					1

static int gAD7441_Mode= -1;
static int gAD7441_TMDS = 0;//-1;
static int gAD7441_Hpv = 0;
static int gAD7441_Vps = 0;
//static int gAD7441_CurStatus=-1;
int gAD7441_InputPort=-1;
static AD7441_INPUT gAD7441_InterFace = AD7441_INPUT_UNKNOW;
static Device_Source_CH gAD7441_Source_Ch = DEVICE_SOURCE_CP;

static Vps_Adv7441_hpv_vps adv7441_gLineNum[DEVICE_STD_LAST] = {
	{"480I",		263,60,1587,720,240}, // 0-480ix60             
	{"576I",		313,50,1600,720,288},  // 1-576ix50
	{"480I",		263,60,1587,720,240}, // 2-480ix60             
	{"576I",		313,50,1600,720,288},  // 3-576ix50
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{"480P",		0xFFFF,0xFFFF,0xFFFF,0,0}, // 7-480px60             
	{"576P",		0xFFFF,0xFFFF,0xFFFF,0,0},  // 8-576px50
	{"720P60",	750,60,1111,1280,720},  // 9-1280x720x60 
	{"720P50",	750,50,1333,1280,720},  // 10-1280x720x50 
	{"1080I60",	563,60,1481,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	562,50,1777,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1125,60,740,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1125,50,888,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1125,25,1777,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1125,30,1481,1920,1080},  // 16-1920x1080x30p

	{"640x480@60",	525,60,1588,640,480},  // 17-640x480x60   
	{"640x480@72",	520,72,1320,640,480},  // 18-640x480x72   
	{"640x480@75",	500,75,1333,640,480},  // 19-640x480x75   
	{"640x480@85",	509,85,1155,640,480},  // 20-640x480x85   
	{"800x600@60",	628,60,1320,800,600},  // 21-800x600x60   
	{"800x600@72",	666,72,1040,800,600},  // 22-800x600x72
	{"800x600@75",	625,75,1066,800,600},  // 23-800x600x75   
	{"800x600@85",	631,85,931,800,600},  // 24-800x600x85   
	{"1024x768@60",	806,60,1033,1024,768},  // 25-1024x768x60                   
	{"1024x768@70",	806,70,885,1024,768},  // 26-1024x768x70
	{"1024x768@75",	800,75,833,1024,768},  // 27-1024x768x75                  
	{"1024x768@85",	808,85,728,1024,768},  // 28-1024x768x85 
	{"1280x720@60",	750,60,1111,1280,720},  // 29-1280x720x60 
	{"1280x768@60",	798,60,1054,1280,768},  // 30-1280x768x60 
	{"1280x768@75",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 31-1280x768x75 
	{"1280x768@85",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 32-1280x768x85 
	{"1280x800@60",	828,60,1006,1280,800},  // 33-1280x800x60 
	{"1280x960@60",	1000,60,833,1280,960},  // 34-1280x960x60 
	{"1280x1024@60",	1066,60,781,1280,1024}, // 35-1280x1024x60
	{"1280x1024@75",	1066,75,625,1280,1024}, // 36-1280x1024x75
	{"1280x1024@85",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 37-1280x1024x85
	{"1366x768@60",	795,60,1047,1366,768}, // 38-1366x768x60
	{"1440x900@60",	934,60,901,1440,900}, // 39-1440x900x60
	{"1400x1050@60",	1089,60,765,1400,1050}, // 40-1400x1050x60
	{"1400x1050@75",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 41-1400x1050x75
	{"1600x1200@60",	1250,60,666,1600,1200},// 42-1600x1200x60
	{"1920x1080@60_DMT", 1125,60,740,1920,1080}, // 43-1920x1080x60-DMT             
	{"1920x1080@60_GTF", 1125,60,740,1920,1080}, // 44-1920x1080x60-GTF
	{"1920x1200@60",	1244,60,675,1920,1200},// 45-1920x1200x60
	{"2560x1440@60",	1481,60,0xFFFF,2560,1440},// 46-2560x1440x60
	
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
			//The video timing is provided by the application.
			//Used in display mode. */

	{"Max",0xFFFF,0xFFFF,0xFFFF,0,0} // FVID2_STD_MAX
};

static Vps_Adv7441_sav_eav_vbi  adv7441_SavEavVbi[DEVICE_STD_LAST] = {
	{"480I",		0x00,0x00,0x00,0x00},  			// 0-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 1-576ix50     
	{"480I",		0x00,0x00,0x00,0x00},  			// 2-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 3-576ix50     
	{"Revs",		0x00,0x00,0x00,0x00},				// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P",		0x00,0x00,0x00,0x00},  			// 7-480px60     
	{"576P",		0x00,0x00,0x00,0x00},				// 8-576px50   
	{"720P60",	0x00,0x00,0x00,0x00},				// 9-1280x720x60
	{"720P50",	0x00,0x00,0x00,0x00},  			// 10-1280x720x50           
	{"1080I60",	0x00,0x00,0x00,0x00},				// 11-1920x1080x60i              	
	{"1080I50",	0x00,0x00,0x00,0x00},				// 12-1920x1080x50i  
	{"1080P60",	0x00,0x00,0x00,0x00},				// 13-1920x1080x60p      
	{"1080P50",	0x00,0x00,0x00,0x00},				// 14-1920x1080x50p      
	{"1080P25",	0x00,0x00,0x00,0x00},				// 15-1920x1080x25p              
	{"1080P30",	0x00,0x00,0x00,0x00},				// 16-1920x1080x30p  
	
	{"640x480@60",	135,781,525,45},				// 17 -640x480x60            
	{"640x480@72",	160,806,508,28},				// 18 -640x480x72
	{"640x480@75",	164,810,502,22},				// 19 -640x480x75
	{"640x480@85",	126,772,500,20},				// 20 -640x480x85
	{"800x600@60",	202,1008,628,28}, 				// 21 -800x600x60
	{"800x600@72",	178,984,626,26},	 			// 22 -800x600x72               
	{"800x600@75",	225,1031,625,25},	 			// 23 -800x600x75               
	{"800x600@85",	200,1006,631,31},	 			// 24 -800x600x85
	{"1024x768@60",	280,1310,806,38},				 // 25 -1024x768x60              
	{"1024x768@70",	264,1294,806,38},				 // 26 -1024x768x70              
	{"1024x768@75",	260,1290,800,32},				 // 27-1024x768x75              
	{"1024x768@85",	292,1322,808,40},		 		// 28-1024x768x85  
	{"1280x720@60",	320,1606,750,30},				 // 29-1280x720x60
	{"1280x768@60",	320,1606,798,30},				 // 30-1280x768x60
	{"1280x768@75",	0x00,0x00,0x00,0x00},			 // 31-1280x768x75 
	{"1280x768@85",	0x00,0x00,0x00,0x00},	 		// 32-1280x768x85 
	{"1280x800@60",	330,1616,830,30},	 			// 33-1280x800x60
	{"1280x960@60",	340,1626,990,30},	 			// 34-1280x960x60 
	
	{"1280x1024@60",	350,1636,1066,42},	 		// 35-1280x1024x60
	{"1280x1024@75",	376,1662,1066,42},	 		// 36-1280x1024x75	
	{"1280x1024@85",	0x00,0x00,0x00,0x00},	 		// 37-1280x1024x85	
	{"1366x768@60",	266,1638,788,20},	 		       // 38-1366x768x60	
	{"1440x900@60",	350,1796,930,30},	 			// 39-1440x900x60
	{"1400x1050@60",	359,1765,1087,37},	 		// 40-1400x1050x60
	{"1400x1050@75",	0x00,0x00,0x00,0x00},	 		// 41-1400x1050x75
	{"1600x1200@60",	480,2086,1250,50},  			 // 42-1600x1200x60
	{"1920x1080@60_DMT", 300,2326,1100,20},		// 43-1920x1080x60-DMT
	{"1920x1080@60_GTF", 100,2026,1090,10},		// 44-1920x1080x60-GTF
	{"1920x1200@60",	536,2462,1242,38},  			 // 45-1920x1200x60  XXXXXXXXXXXXXXXXXXXXXXXXXXXX
	{"2560x1440@60",	0x00,0x00,0x00,0x00},			// 46-2560x1440x60

    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0x00,0x00,0x00,0x00} // FVID2_STD_MAX
};

static Vps_Adv7441_InMode ArgMode_7441[DEVICE_STD_LAST] = {
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 0-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 1-576ix50     
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 2-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 3-576ix50     
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P60",0x07,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 7-480px60     
	{"576P50",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	// 8-576px50   
	{"720P60",0x0a,0x21,0x5F,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	// 9-1280x720x60
	{"720P50",0x2a,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 10-1280x720x50           
	{"1080I60",0x0c,0x21,0x5d,0x10,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 11-1920x1080x60i              	
	{"1080I50",0x2c,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 12-1920x1080x50i  
	{"1080P60",0x0b,0x21,0x5d,0x00,0x21,0x5e,0x10,0x05,0x21,0x5e,0x10},	// 13-1920x1080x60p      
	{"1080P50",0x2b,0x21,0x5d,0x00,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 14-1920x1080x50p      
	{"1080P25",0x6b,0x61,0x5e,0x00,0xea,0x50,0x10,0x03,0xfa,0x46,0x50},	// 15-1920x1080x25p              
	{"1080P30",0x4b,0x61,0x5e,0x00,0xea,0x50,0x10,0x03,0x51,0x46,0x50},	// 16-1920x1080x30p  
	
	{"640x480@60",0x08,0x11,0x5b,0x10,0x63,0x5b,0x10,0x08,0x11,0x5b,0x10},	 // 17 -640x480x60            
	{"640x480@72",0x09,0x11,0x5b,0x10,0x63,0x5b,0x10,0x09,0x11,0x5b,0x10},  	 // 18 -640x480x72
	{"640x480@75",0x0a,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0a,0x11,0x5c,0x10},  	 // 19 -640x480x75
	{"640x480@85",0x0b,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0b,0x11,0x5c,0x10},	 // 20 -640x480x85
	{"800x600@60",0x01,0x11,0x5c,0x10,0x63,0x5c,0x10,0x01,0x11,0x5c,0x10}, 	 // 21 -800x600x60
	{"800x600@72",0x02,0x11,0x5c,0x10,0x63,0x5c,0x10,0x02,0x11,0x5c,0x10},	 // 22 -800x600x72               
	{"800x600@75",0x03,0x11,0x5c,0x10,0x63,0x5c,0x10,0x03,0x11,0x5c,0x10},	 // 23 -800x600x75               
	{"800x600@85",0x04,0x21,0x5c,0x10,0x63,0x5c,0x10,0x04,0x21,0x5c,0x10},	 // 24 -800x600x85
	{"1024x768@60",0x0c,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0c,0x21,0x5d,0x10},	 // 25 -1024x768x60              
	{"1024x768@70",0x0d,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0d,0x21,0x5d,0x10},	 // 26 -1024x768x70              
	{"1024x768@75",0x0e,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0e,0x21,0x5d,0x10},	 // 27-1024x768x75              
	{"1024x768@85",0x0f,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0f,0x21,0x5d,0x10},	 // 28-1024x768x85
	{"1280x720@60",0x00,0x21,0x5F,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	 // 29-1280x720x60
	{"1280x768@60",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x72,0x58,0x31,0xe0},	 // 30-1280x768x60
	{"1280x768@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 31-1280x768x75 
	{"1280x768@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 32-1280x768x85 
	{"1280x800@60",0x00,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x67,0x70},	// 33-1280x800x60 
	{"1280x960@60",0x00,0x21,0x5d,0x10,0xe7,0x08,0x10,0x01,0xe0,0x3e,0x80},	// 34-1280x960x60 DMT
	{"1280x1024@60",0x05,0x21,0x5d,0x10,0x63,0x5d,0x10,0x05,0x21,0x5d,0x10},	 // 35-1280x1024x60
	{"1280x1024@75",0x06,0x21,0x5d,0x10,0x63,0x5e,0x10,0x05,0x21,0x5e,0x10},	 // 36-1280x1024x75	
	{"1280x1024@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 37-1280x1024x85
	{"1366x768@60",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	 // 38-1366x768x60
	{"1440x900@60",0x00,0x21,0x5d,0x10,0xe7,0x70,0x10,0x02,0x00,0x3a,0x60},	// 39-1440x900x60 DMT
	{"1400x1050@60",0x00,0x21,0x5d,0x10,0xE7,0x48,0x10,0x01,0xBA,0x44,0x1E},	 // 40-1400x1050x60
	{"1400x1050@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 41-1400x1050x75
	{"1600x1200@60",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	 // 42-1600x1200x60
	{"1920x1080@60_DMT",0x00,0x21,0x5d,0x10,0xea,0x10,0xd0,0x01,0xAA,0x46,0x50},	// 43-1920x1080X60-DMT
	{"1920x1080@60_GTF",0x00,0x21,0x5d,0x10,0xe8,0x98,0xd0,0x01,0xA8,0x46,0x50},	// 44-1920x1080X60-GTF
	{"1920x1200@60",0x00,0x21,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //45-1920x1200x60   
	{"2560x1440@60",	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 46-2560x1440x60

    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} // FVID2_STD_MAX
};

static void delay1us(vu32 num)
{
 	while(num-->0)
	{
	}
}

static U8 Device_adv7441_Read8_Usermap( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_USER_MAP_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static U8 Device_adv7441_Read8_Usermap1( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_USER_MAP1_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static U8 Device_adv7441_Read8_Usermap2( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_USER_MAP2_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static U8 Device_adv7441_Read8_Usermap3( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_USER_MAP3_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static U8 Device_adv7441_Read8_Hdmimap( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_HDMI_MAP_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static U8 Device_adv7441_Read8_Ksvmap( U8 RegAddr)
{
//	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	I2C_ReadBuf_FromDevice ( ADV7441_KSV_MAP_ADDR, regAddr[0], regValue, 1);

	return regValue[0];
}

static S32 Device_adv7441_Write8_Usermap( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_USER_MAP_ADDR, regAddr[0], regValue[0]);

	return status;
}

static S32 Device_adv7441_Write8_Usermap1( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_USER_MAP1_ADDR, regAddr[0], regValue[0]);

	return status;
}

static S32 Device_adv7441_Write8_Usermap2( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_USER_MAP2_ADDR, regAddr[0], regValue[0]);

	return status;
}

static S32 Device_adv7441_Write8_Usermap3( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_USER_MAP3_ADDR, regAddr[0], regValue[0]);

	return status;
}

static S32 Device_adv7441_Write8_Hdmimap( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_HDMI_MAP_ADDR, regAddr[0], regValue[0]);

	return status;
}

static S32 Device_adv7441_Write8_Ksvmap( U8 RegAddr, U8 RegVal)
{
	S32 status = 0;
	U8 regAddr[1]={0};
	U8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = I2C_ByteWrite_ToDevice ( ADV7441_KSV_MAP_ADDR, regAddr[0], regValue[0] );

	return status;
}

static int Device_adv7441_check_1366Or1280( int *index)
{
	S32 	retVal = 0;
	S32	lwidth=0;
	unsigned char data;

	if(gAD7441_TMDS)
	{
		data = Device_adv7441_Read8_Hdmimap( 0x07);
		lwidth = (data&0xF)<<8;
		data = Device_adv7441_Read8_Hdmimap( 0x08);
		lwidth += (data&0xFF);
		if(lwidth > 1300)
			*index = DEVICE_STD_VGA_1366X768X60;
		else
			*index = DEVICE_STD_VGA_1280X768X60;
	}
	else
	{
		data = Device_adv7441_Read8_Usermap( 0xB5);
		if(data&0x08)
			*index = DEVICE_STD_VGA_1366X768X60;
		else 
			*index = DEVICE_STD_VGA_1280X768X60;
	}
	
	return retVal;
}

static int Device_adv7441_check_VGAorVideo( int *index)
{
	S32 	retVal = 0;
	unsigned char data;

	
	/*if(gAD7441_TMDS){
		*index = DEVICE_STD_720P_60;
	}
	else*/
	{
		data = Device_adv7441_Read8_Usermap( 0xB5);
		if((data&0x03)==0x03)
			*index = DEVICE_STD_720P_60;
		else
			*index = DEVICE_STD_VGA_1280X720X60;
	}
	return retVal;
}

static int Device_adv7441_check_1080_DMTorGTF( int *index)
{
	S32 	retVal = 0;
	unsigned char data;

	if(!gAD7441_TMDS){
		data = Device_adv7441_Read8_Usermap( 0xB5);
		if(data&0x08)
			*index = DEVICE_STD_VGA_1920X1080X60_GTF;
		else
			*index = DEVICE_STD_VGA_1920X1080X60_DMT;

		if((data&0x03)==0x03)
			*index = DEVICE_STD_1080P_60;
	}
	return retVal;
}

static int Device_ad7441_SetCPSAV_EAV( unsigned int sav, unsigned int eav)
{
	S32 	retVal = 0;
	unsigned char data;

	if((sav == 0)&&(eav == 0))
		return retVal;
	if(gAD7441_TMDS)
		return 0;

	ADV_OSA_printf("sav = %d, eav = %d\n", sav, eav);
	data = (sav>>4)&0xFF;
	Device_adv7441_Write8_Usermap( 0xA2, data);

	data = (sav&0xF)<<4 | ((eav&0xF00)>>8);
	Device_adv7441_Write8_Usermap( 0xA3, data);

	data = eav&0xFF;
	Device_adv7441_Write8_Usermap( 0xA4, data);
	
	return retVal;
}

static int Device_ad7441_SetCPVBI( unsigned int s_vbi, unsigned int e_vbi)
{
	S32 	retVal = 0;
	unsigned char data;
	
	if((s_vbi == 0)||(e_vbi == 0))
		return retVal;
	if(gAD7441_TMDS)
		return 0;
	
	ADV_OSA_printf("s_vbi = %d, e_vbi = %d\n", s_vbi, e_vbi);
	data = (s_vbi>>4)&0xFF;
	Device_adv7441_Write8_Usermap( 0xA5, data);

	data = (s_vbi&0xF)<<4 | ((e_vbi&0xF00)>>8);
	Device_adv7441_Write8_Usermap( 0xA6, data);
	
	data = e_vbi&0xFF;
	Device_adv7441_Write8_Usermap( 0xA7, data);
	return retVal;
}

static int Device_adv7441_SetPhase( int inMode)
{
	S32 	retVal = 0;
	unsigned char data=0;

	if(inMode >= DEVICE_STD_VGA_1280X720X60)
		data = 0x10;
	else
		data = 0x00;
	
	switch(inMode){
		case     DEVICE_STD_VGA_640X480X60:
		break;
		case     DEVICE_STD_VGA_640X480X72:
		break;
		case    DEVICE_STD_VGA_640X480X75:
		break;
		case    DEVICE_STD_VGA_640X480X85:
		break;
		case    DEVICE_STD_VGA_800X600X60:
		break;
		case    DEVICE_STD_VGA_800X600X72:
		break;
		case    DEVICE_STD_VGA_800X600X75:
		break;
		case    DEVICE_STD_VGA_800X600X85:
		break;
		case    DEVICE_STD_VGA_1024X768X60:
		break;
		case    DEVICE_STD_VGA_1024X768X70:
		break;
		case    DEVICE_STD_VGA_1024X768X75:
		break;
		case    DEVICE_STD_VGA_1024X768X85:
		break;
		case    DEVICE_STD_VGA_1280X720X60:
		break;
		case    DEVICE_STD_VGA_1280X768X60:
		break;
		case    DEVICE_STD_VGA_1280X768X75:
		break;
		case    DEVICE_STD_VGA_1280X768X85:
		break;
		case    DEVICE_STD_VGA_1280X800X60:
		break;
		case    DEVICE_STD_VGA_1280X960X60:
		break;
		case    DEVICE_STD_VGA_1280X1024X60:
		break;
		case    DEVICE_STD_VGA_1280X1024X75:
		break;
		case    DEVICE_STD_VGA_1280X1024X85:
		break;
		case    DEVICE_STD_VGA_1366X768X60:
		break;
		case    DEVICE_STD_VGA_1440X900X60:
		break;
		case    DEVICE_STD_VGA_1400X1050X60:
		break;
		case    DEVICE_STD_VGA_1400X1050X75:
		break;
		case    DEVICE_STD_VGA_1600X1200X60:
		break;
		case    DEVICE_STD_VGA_1920X1080X60_DMT:
		break;
		case    DEVICE_STD_VGA_1920X1080X60_GTF:
		break;
		case    DEVICE_STD_VGA_1920X1200X60:
		break;
		case    DEVICE_STD_VGA_2560X1440X60:
		break;
		default:
		break;
	}
	Device_adv7441_Write8_Usermap( 0x6A, data);
	return retVal;
}

static int Device_ad7441_SetFieldOutput()
{
	int ret=0;
	unsigned char data;
//	unsigned short val;
	
	data = Device_adv7441_Read8_Usermap( 0x6B);
	data &= ~(1<<6);
	Device_adv7441_Write8_Usermap( 0x6B, data);
	
	return ret;
}

static int Device_ad7441_SetVSOutput()
{
	int ret=0;
	unsigned char data;
//	unsigned short val;
	
	data = Device_adv7441_Read8_Usermap( 0x6B);
	data |= (1<<6);
	Device_adv7441_Write8_Usermap( 0x6B, data);
	
	
	return ret;
}

static S32 Device_adv7441GetTMDS_A( unsigned int *tmds_a)
{
	int retVal=0;
	unsigned char data;

	data = Device_adv7441_Read8_Hdmimap( 0x04);	
	*tmds_a=(data&(0x01<<3))>>3;

	return retVal;
}

static S32 Device_adv7441GetTMDS_B( unsigned int *tmds_b)
{
	int retVal=0;
	unsigned char data;

	data = Device_adv7441_Read8_Hdmimap( 0x04);	
	*tmds_b=(data&(0x01<<2))>>2;

	return retVal;
}

static S32 Device_adv7441GetInfo_D( ADV7441_SyncInfo *SyncInfo)
{
	S32 	retVal = 0;
	unsigned short	wTotPixLine;
	unsigned short    	wcurrTotField1_Hght;
	unsigned short    	wcurrTotField0_Hght;
	unsigned int    	dwTMDS_Freq;
	unsigned short    	wtemp1; 
	unsigned int    	dwtemp2;
	unsigned short    	wtemp3;
	unsigned short    	wtemp4;
	unsigned int    	dwtemp5;
	unsigned short    	pix_rep;
//	unsigned short   	w_deep_color_mode;
	unsigned short    	wHsync_FrontPorch;
	unsigned short    	wHsync_BackPorch;
	unsigned short    	wHsync_PulseWidth;
	unsigned short    	wFiled0_Vsync_FrontPorch;
	unsigned short    	wFiled0_Vsync_BackPorch;
	unsigned short    	wFiled0_Vsync_PulseWidth;
	unsigned short    	wFiled1_Vsync_FrontPorch;
	unsigned short    	wFiled1_Vsync_BackPorch;
	unsigned short    	wFiled1_Vsync_PulseWidth;
	unsigned char  	temp;
	unsigned short 	width;
	unsigned short   	length;

	temp = Device_adv7441_Read8_Hdmimap( 0x04);	
	SyncInfo->TMDS_A=(temp&(0x01<<3))>>3;//ADV_OSA_printf("SyncInfo->TMDS_A=%d    ",SyncInfo->TMDS_A);
	SyncInfo->TMDS_B=(temp&(0x01<<2))>>2;//ADV_OSA_printf("SyncInfo->TMDS_B=%d    ",SyncInfo->TMDS_B);
	
	temp = Device_adv7441_Read8_Usermap( 0xB1);
	SyncInfo->Interlaced = (temp&0x40) > 0 ? 1 : 0;//ADV_OSA_printf("SyncInfo->Interlaced=%d    ",SyncInfo->Interlaced);
	
	temp = Device_adv7441_Read8_Hdmimap( 0x1E);
	wTotPixLine = (temp&0x0F)<<8;
	temp = Device_adv7441_Read8_Hdmimap( 0x1F);
	wTotPixLine += temp;
	SyncInfo->HTotPix = wTotPixLine;       //ADV_OSA_printf("SyncInfo->HTotPix=%d    ",SyncInfo->HTotPix);   
	
	temp = Device_adv7441_Read8_Hdmimap( 0x09);
	length = (temp&0x0F)<<8;
	temp = Device_adv7441_Read8_Hdmimap( 0x0A);
	length += temp;
	//ADV_OSA_printf("length=%d    ",length);
	if((1150<length)&&(length<1250))
		SyncInfo->Interlaced =0;
	                     

	temp = Device_adv7441_Read8_Hdmimap( 0x26);
	wcurrTotField0_Hght = (temp&0x0F)<<8;
	temp = Device_adv7441_Read8_Hdmimap( 0x27);
	wcurrTotField0_Hght += temp;
	temp = Device_adv7441_Read8_Hdmimap( 0x28);
	wcurrTotField1_Hght = (temp&0x0F)<<8;
	temp = Device_adv7441_Read8_Hdmimap( 0x29);
	wcurrTotField1_Hght += temp;

	if(SyncInfo->Interlaced) {
		ADV_OSA_printf("Interlaced");
		SyncInfo->VTotPix =( (wcurrTotField1_Hght >> 1) 
							+ (wcurrTotField0_Hght >> 1))>>1;
	}
	else {
		ADV_OSA_printf("not Interlaced");
		SyncInfo->VTotPix = (wcurrTotField0_Hght) >> 1;
	}
 //ADV_OSA_printf("SyncInfo->VTotPix=%d    ",SyncInfo->VTotPix);  
//	temp = Device_adv7441_Read8_Hdmimap( 0x0B);
//	w_deep_color_mode = ((temp & 0x60) >> 5);
	temp = Device_adv7441_Read8_Hdmimap( 0x05);
	pix_rep = (temp & 0x0F);
	pix_rep++;										/*pixel repetition factor*/			

	temp = Device_adv7441_Read8_Hdmimap( 0x06);
	wtemp1 = ((FXTAL/10)*temp)/(27000000/10);
	dwTMDS_Freq = (unsigned int )wtemp1;

	wtemp1 = SyncInfo->HTotPix;	
	/*HSync Frequency in 100Hz*/
	if(wtemp1 != 0) 
	{
		dwtemp2 = (unsigned int ) (((dwTMDS_Freq * 1000000)/ (wtemp1))/10);
		SyncInfo->HFreq = (unsigned short ) dwtemp2;
		if(pix_rep != 0)
		{
			SyncInfo->HFreq = SyncInfo->HFreq/pix_rep ;
		}
		//ADV_OSA_printf("SyncInfo->HFreq=%d    ",SyncInfo->HFreq);  
	}

	wtemp3 = SyncInfo->HFreq;										/*VSync Frequency*/
	wtemp4 = SyncInfo->VTotPix;
	if(wtemp4 !=0) {
		dwtemp5 = (unsigned int ) (((wtemp3 * 10)/wtemp4)); //dwtemp5 = (unsigned int ) (((wtemp3 * 10)/

		SyncInfo->VFreq = (unsigned short ) dwtemp5;

		if(SyncInfo->Interlaced) {
			SyncInfo->VFreq = SyncInfo->VFreq ; //SyncInfo->VFreq = SyncInfo->VFreq << 1;
		}
		//ADV_OSA_printf("SyncInfo->VFreq=%d    ",SyncInfo->VFreq);  
	}
	/*
	temp = Device_adv7441_Read8_Hdmimap( 0x20);
	wHsync_FrontPorch = ((temp & 0x3) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x21);
	wHsync_FrontPorch += temp;
	//ADV_OSA_printf("wHsync_FrontPorch=%d    ",wHsync_FrontPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x24);
	wHsync_BackPorch = ((temp & 0x3) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x25);
	wHsync_BackPorch += temp;
//ADV_OSA_printf("wHsync_BackPorch=%d    ",wHsync_BackPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x22);
	wHsync_PulseWidth = ((temp & 0x3) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x23);
	wHsync_PulseWidth += temp;
	//ADV_OSA_printf("wHsync_PulseWidth=%d    ",wHsync_PulseWidth);  
	temp = Device_adv7441_Read8_Hdmimap( 0x2A);
	wFiled0_Vsync_FrontPorch = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x2B);
	wFiled0_Vsync_FrontPorch += temp;
//ADV_OSA_printf("wFiled0_Vsync_FrontPorch=%d    ",wFiled0_Vsync_FrontPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x32);
	wFiled0_Vsync_BackPorch = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x33);
	wFiled0_Vsync_BackPorch += temp;
	//ADV_OSA_printf("wFiled0_Vsync_BackPorch=%d    ",wFiled0_Vsync_BackPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x2E);
	wFiled0_Vsync_PulseWidth = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x2F);
	wFiled0_Vsync_PulseWidth += temp;
	//ADV_OSA_printf("wFiled0_Vsync_PulseWidth=%d    ",wFiled0_Vsync_PulseWidth);  
	temp = Device_adv7441_Read8_Hdmimap( 0x2C);
	wFiled1_Vsync_FrontPorch = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x2D);
	wFiled1_Vsync_FrontPorch += temp;
//ADV_OSA_printf("wFiled1_Vsync_FrontPorch=%d    ",wFiled1_Vsync_FrontPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x34);
	wFiled1_Vsync_BackPorch = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x35);
	wFiled1_Vsync_BackPorch += temp;
//ADV_OSA_printf("wFiled1_Vsync_BackPorch=%d    ",wFiled1_Vsync_BackPorch);  
	temp = Device_adv7441_Read8_Hdmimap( 0x30);
	wFiled1_Vsync_PulseWidth = ((temp & 0x1F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x31);
	wFiled1_Vsync_PulseWidth += temp;
	
	temp = Device_adv7441_Read8_Hdmimap( 0x07);
	width = ((temp & 0x0F) << 8);
	temp = Device_adv7441_Read8_Hdmimap( 0x08);
	width += temp;
//ADV_OSA_printf("width=%d    ",width);  

*/
	return retVal;
}

static S32 Device_adv7441GetInfo_A( int *hpv, int *vps)
{
	S32 	retVal = 0;
	unsigned char val1,val2;
	int fcl;

	val1 = Device_adv7441_Read8_Usermap(0xB3);
	val2 = Device_adv7441_Read8_Usermap(0xB4);
	*hpv=((val1&0x07)<<8)|val2;

	val1 = Device_adv7441_Read8_Usermap(0xCA);
	val2 = Device_adv7441_Read8_Usermap(0xCB);
	fcl =((val1&0x1f)<<8)|val2;
	if(!fcl)
		*vps=0;
	else
		*vps=(FXTAL/fcl)/256;	

	return retVal;
}

static S32 Device_adv7441IsGreenSync()
{
	int ret=0;
	unsigned char regVal=0;

	regVal = Device_adv7441_Read8_Usermap( 0xB5);
	if((regVal & 0x3) == 0x3){
		ret = 1;
	}else{
		ret = 0;
	}
	return ret;
}

static S32 Device_adv7441GetInterface( int *tmds, AD7441_INPUT *InterFace)
{
	S32 	retVal = 0;
	unsigned char regVal=0;
	
	if(*tmds){
		*InterFace = AD7441_DIGITAL_INPUT;
	}else{
		regVal = Device_adv7441_Read8_Usermap( 0xB5);
		if((regVal & 0x3) == 0x3)
			*InterFace = AD7441_YPbPr_INPUT;
		else
			*InterFace = AD7441_ANALOG_INPUT;
	}
	return retVal;
}

static S32 Device_adv7441GetModeIndex(int hpv, int vps, int *index)
{
	S32 	retVal = 0;
	int i=-1;
	
	for(i = 0;i<DEVICE_STD_LAST;i++){
		if(((vps>adv7441_gLineNum[i].vps-3)&&(vps<adv7441_gLineNum[i].vps+3))&&((hpv>adv7441_gLineNum[i].hpv-4)&&(hpv<adv7441_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_LAST){
		for(i = 0;i<DEVICE_STD_LAST;i++){
			if(((vps>adv7441_gLineNum[i].vps-3)&&(vps<adv7441_gLineNum[i].vps+3))&&((hpv>adv7441_gLineNum[i].hpv-10)&&(hpv<adv7441_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_LAST)
		*index = -1;
	else
		*index = i;

	return retVal;
}

static S32 Device_adv7441GetModeIndexSD( int *inMode)
{
	S32 	retVal = 0;
	unsigned char data;
	int		Retry=3;
	
	while(Retry--){
		data = Device_adv7441_Read8_Usermap( 0x13);
		if(data & 0x1){
			if(data & (0x1<<3)){
				if(data & 0x4){
					*inMode = DEVICE_STD_PAL;
				}else{
					*inMode = DEVICE_STD_NTSC;
				}
			}
			break;
		}else{
			delay1us(1000*100);
		}
	}

	if(*inMode != -1){
		delay1us(1000*300);
		data = Device_adv7441_Read8_Usermap( 0x13);
		if(data & 0x1){
		}else{
			*inMode = -1;
		}
	}

	return retVal;
}

static char Device_adv7441GetLineWidth( short int  *w_value)
{
	char b_data;
	b_data=Device_adv7441_Read8_Hdmimap( 0x07);
	*w_value = ((b_data & 0xF) << 8);
	b_data=Device_adv7441_Read8_Hdmimap(  0x08);

	*w_value += (b_data & 0xFF);
	return 0;
}


static S32 Device_adv7441InitPortDVI()
{
	S32 	retVal = 0;
	U8 ucValue;
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x03,0x0C);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x05,0x06);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x06,0x02); 
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x1D,0x40);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x3C,0xA8);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x47,0x0A);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x67,0x27);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x68,0xF0);//0xF2);//
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x6B,0xE1);//0xF2);//
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7B,0x04);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x85,0x19);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xBA,0xA0);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xF4,0x3F);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xC8,0x08);
	
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF0,0x10);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF1,0x0F);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF4,0x20);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x15,0xEC);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x1C,0x49);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x1D,0x04);
	
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x00,0x00);


	return retVal;
}

static S32 Device_adv7441InitPortHDMI()
{
	S32 	retVal = 0;
	U8 ucValue;
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x03,0x0C);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x05,0x06);  //
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x06,0x02);  //
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x1D,0x40);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x3C,0xA8);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x47,0x0A);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x67,0x27);  
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x68,0xF0);//0xF2);//
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x6B,0xE1);//0xF2);//
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7B,0x04);//
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x85,0x19);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xBA,0xA0);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xF4,0x3F);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xC8,0x08);
	
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF0,0x10);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF1,0x0F);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0xF4,0x20);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x15,0xEC);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x1C,0x49);
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x1D,0x04);
	
	I2C_ByteWrite_ToDevice(ADV7441_HDMI_MAP_ADDR,0x00,0x01);

	return retVal;
}

static S32 Device_adv7441InitPortVGA()
{
	S32 	retVal = 0;

	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x03,0x4C);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x05,0x02);
//	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x06,0x0C);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x1D,0x40); //28.63636
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x3C,0xA8);//SOG_SYNC_LEVEL[4:0]

	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x31,0x18);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x32,0x81);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x35,0x22);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x36,0x9E);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x37,0xA1);
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x47,0x0A);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x68,0xF0);//0xF2
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x6B,0xE1);//0xF2
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7B,0x1D);

	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7C,0xC0);//0x00	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7E,0x00);//0x00
/*
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x87,0x20);//0xE8
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x88,0x5A);//0x70
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x8F,0x80);//0x01
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xAB,0x00);//0x4E
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xAC,0x00);//0x20	
	*/
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xC8,0x08);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x91,0x10);
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xBA,0xA0);//0x4E
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xF4,0x1D);//0x20
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x3A,0x11);

	return retVal;
}

static S32 Device_adv7441InitPortYPbPr()
{
	S32 	retVal = 0;
	
	retVal = Device_adv7441_Write8_Usermap(0x00,0x0E);
	retVal = Device_adv7441_Write8_Usermap(0x05,0x01);
	retVal = Device_adv7441_Write8_Usermap(0x06,0x0A);
	retVal = Device_adv7441_Write8_Usermap(0xF1,0x0F);
	
	return retVal;
}

static S32 Device_adv7441InitPortCVBS()
{
	S32 	retVal = 0;
	
//##SDP CVBS##
//AUTODETECT CVBS (PAL) 20 Bit 422  Out through HDMI:
	//PAL
	I2C_ByteWrite_ToDevice(0x42,0x00, 0x0B);
	I2C_ByteWrite_ToDevice(0x42,0x03, 0x05);//0x05
	I2C_ByteWrite_ToDevice(0x42,0x05, 0x00);
	I2C_ByteWrite_ToDevice(0x42,0x06, 0x02);
	I2C_ByteWrite_ToDevice(0x42,0x17, 0x41);
	I2C_ByteWrite_ToDevice(0x42,0x1D, 0x40);
	I2C_ByteWrite_ToDevice(0x42,0x31, 0x18);
	I2C_ByteWrite_ToDevice(0x42,0x32, 0x81);
	I2C_ByteWrite_ToDevice(0x42,0x33, 0x84);
	I2C_ByteWrite_ToDevice(0x42,0x34, 0x00);
	I2C_ByteWrite_ToDevice(0x42,0x35, 0x14);
	I2C_ByteWrite_ToDevice(0x42,0x36, 0x92);
	I2C_ByteWrite_ToDevice(0x42,0x37, 0x00);//多加
	I2C_ByteWrite_ToDevice(0x42,0x3A, 0x07);
	I2C_ByteWrite_ToDevice(0x42,0x3C, 0xA8);
	I2C_ByteWrite_ToDevice(0x42,0x47, 0x0A);
	I2C_ByteWrite_ToDevice(0x42,0x8F, 0x50);//多加
	I2C_ByteWrite_ToDevice(0x42,0xF3, 0x0F);

//NTSC
//42 17 41 去掉
//42 34 00 ; H Sync position control
//42 35 22 ; H Sync position control
//42 E5 27 ; V Sync position control
//42 E6 4F ; V Sync position control	
	return retVal;
}

static S32 Device_adv7441InitPortCVBS2()
{
	S32 	retVal = 0;

	
	I2C_ByteWrite_ToDevice(0x42,0x00, 0x0C);
	I2C_ByteWrite_ToDevice(0x42,0x03, 0x05);//0x05
	I2C_ByteWrite_ToDevice(0x42,0x05, 0x00);
	I2C_ByteWrite_ToDevice(0x42,0x06, 0x02);
	I2C_ByteWrite_ToDevice(0x42,0x17, 0x41);
	I2C_ByteWrite_ToDevice(0x42,0x1D, 0x40);
	I2C_ByteWrite_ToDevice(0x42,0x31, 0x18);
	I2C_ByteWrite_ToDevice(0x42,0x32, 0x81);
	I2C_ByteWrite_ToDevice(0x42,0x33, 0x84);
	I2C_ByteWrite_ToDevice(0x42,0x34, 0x00);
	I2C_ByteWrite_ToDevice(0x42,0x35, 0x14);
	I2C_ByteWrite_ToDevice(0x42,0x36, 0x92);
	I2C_ByteWrite_ToDevice(0x42,0x37, 0x00);
	I2C_ByteWrite_ToDevice(0x42,0x3A, 0x07);
	I2C_ByteWrite_ToDevice(0x42,0x3C, 0xA8);
	I2C_ByteWrite_ToDevice(0x42,0x47, 0x0A);
	I2C_ByteWrite_ToDevice(0x42,0x8F, 0x50);
	I2C_ByteWrite_ToDevice(0x42,0xF3, 0x0F);
//	I2C_ByteWrite_ToDevice(0x42,0xF4, 0xD5);
	
	return retVal;
}


static S32 Device_adv7441DigitalInterFaceInit()
{
	S32 	retVal = 0;
	Device_adv7441_Write8_Usermap(  0x67, 0x01);//; Embsynconall and INP_CSPACE now 
	Device_adv7441_Write8_Usermap(  0xC9,0x04);//00
	Device_adv7441_Write8_Usermap(  0x85, 0x03);//03; Turn off SSPD and force SOY. For 
	Device_adv7441_Write8_Usermap(  0xBA, 0xA0);//A0; Enable HDMI and Analog in
	Device_adv7441_Write8_Usermap(  0xBF, 0x02);//
	Device_adv7441_Write8_Usermap2( 0xF0,0x10);//43; ADI Recommended Write
	Device_adv7441_Write8_Usermap2( 0xF1,0x0F);//0E; ADI Recommended Write
	Device_adv7441_Write8_Usermap2( 0xF4,0x20);//1D; ADI Recommended Write
	Device_adv7441_Write8_Hdmimap( 0x15,0x00);//00; Disable these mute mask bits
	Device_adv7441_Write8_Hdmimap( 0x1C,0x00);//00; Set MCLKOUT to output 256fs
	Device_adv7441_Write8_Hdmimap( 0x1D,0x40);//40;  & Set Unmute Delay to 1_5 sec.
	return retVal;

}

static S32 Device_adv7441DigitalInterFaceSetRes( int inMode)
{
	S32 	retVal = 0;
	if((inMode >= DEVICE_STD_720P_60)&&(inMode <= DEVICE_STD_1080P_30))
		Device_adv7441_Write8_Usermap( 0x05,0x05);
	else
		Device_adv7441_Write8_Usermap( 0x05,0x06);
	if(!ArgMode_7441[inMode].config[0]){
		Device_adv7441_Write8_Usermap(  0x06,0x00);
		Device_adv7441_Write8_Usermap(  0x87,ArgMode_7441[inMode].config[4]);//; PLL Divide                                                              
		Device_adv7441_Write8_Usermap(  0x88,ArgMode_7441[inMode].config[5]);//; PLL Divide 
		Device_adv7441_Write8_Usermap(  0x8F,ArgMode_7441[inMode].config[7]);//; Set Free                                                         
		Device_adv7441_Write8_Usermap(  0x90,ArgMode_7441[inMode].config[8]);//; Set Free 
		Device_adv7441_Write8_Usermap(  0xAB,ArgMode_7441[inMode].config[9]);//; Set line 
		Device_adv7441_Write8_Usermap(  0xAC,ArgMode_7441[inMode].config[10]);//; Set line 
	}else{
		Device_adv7441_Write8_Usermap(  0x06,ArgMode_7441[inMode].config[0]);
	}

	Device_adv7441_Write8_Usermap( 0x7B,0x0F);
	Device_adv7441_Write8_Usermap( 0x7C,0xC0);
	
	return retVal;
}

static S32 Device_adv7441AnalogInterFaceInit()
{
	S32 	retVal = 0;
	Device_adv7441_Write8_Usermap(0x05,0x02); //  
	Device_adv7441_Write8_Usermap(0x1d,0x40); // Disable TRI_LLC 
	Device_adv7441_Write8_Usermap(0x52,0x00); //  
	Device_adv7441_Write8_Usermap(0x53,0x00); //  
	Device_adv7441_Write8_Usermap(0x54,0x07); //  
	Device_adv7441_Write8_Usermap(0x55,0x0c); //  
	Device_adv7441_Write8_Usermap(0x56,0x99); //  
	Device_adv7441_Write8_Usermap(0x57,0xa9); //  
	Device_adv7441_Write8_Usermap(0x58,0x71); //  
	Device_adv7441_Write8_Usermap(0x59,0x08); //  
	Device_adv7441_Write8_Usermap(0x5a,0x00); //  
	Device_adv7441_Write8_Usermap(0x5b,0x20); //  
	Device_adv7441_Write8_Usermap(0x5c,0xa7); //  
	Device_adv7441_Write8_Usermap(0x5d,0xa9); //  
	Device_adv7441_Write8_Usermap(0x5e,0x1a); //  
	Device_adv7441_Write8_Usermap(0x5f,0x8f); //  
	Device_adv7441_Write8_Usermap(0x60,0x08); //  
	Device_adv7441_Write8_Usermap(0x61,0x00); //  
	Device_adv7441_Write8_Usermap(0x62,0x7a); //  
	Device_adv7441_Write8_Usermap(0x63,0xe1); //  
	Device_adv7441_Write8_Usermap(0x64,0x05); //  
	Device_adv7441_Write8_Usermap(0x65,0x39); //  CSC
	Device_adv7441_Write8_Usermap(0x66,0x1f); //  CSC
	Device_adv7441_Write8_Usermap(0x68,0xF1);  //Auto CSC , YUV Out 
	Device_adv7441_Write8_Usermap(0x6b,0xF3); // 
	
	
	Device_adv7441_Write8_Usermap(0x6a,0x10); //  	Device_adv7441_Write8_Usermap(0x6a,0x00); //  
	Device_adv7441_Write8_Usermap(0x73,0xd0); //d2
	Device_adv7441_Write8_Usermap(0x74,0x64); //04  added 11.12
	Device_adv7441_Write8_Usermap(0x75,0x19); //81  added 11.12
	Device_adv7441_Write8_Usermap(0x76,0x06); //20  added 11.12 
	Device_adv7441_Write8_Usermap(0x77,0x04); //04  
	Device_adv7441_Write8_Usermap(0x78,0x08); //08  
	Device_adv7441_Write8_Usermap(0x79,0x02); //02  
	Device_adv7441_Write8_Usermap(0x7a,0x00); //00  
	Device_adv7441_Write8_Usermap(0x7b,0x1c); //  

	Device_adv7441_Write8_Usermap(0x7d,0x00); //60  
	Device_adv7441_Write8_Usermap(0x7e,0x00); //18  
	Device_adv7441_Write8_Usermap(0x85,0x03); //	Device_adv7441_Write8_Usermap(0x85,0x0E); //
	Device_adv7441_Write8_Usermap(0x47,0x0A);  //Enable Automatic PLL_Qpump and 

	Device_adv7441_Write8_Usermap(0x86,0x0b); //  
	Device_adv7441_Write8_Usermap(0xf4,0x3f);
	Device_adv7441_Write8_Usermap(0x03,0x48); // 
	Device_adv7441_Write8_Usermap(0x04,0x47); //   
	Device_adv7441_Write8_Usermap(0xBA,0xA0);  //Enable HDMI and Analog in
	return retVal;
}

static S32 Device_adv7441AnalogInterFaceSetRes( int inMode)
{
	S32 	retVal = 0;
	if(!ArgMode_7441[inMode].config[0]){

		Device_adv7441_Write8_Usermap(0x03,0x0C) ;//; Disable TOD
		Device_adv7441_Write8_Usermap(0x05,0x02) ;//; Prim_Mode =010b for GR
		Device_adv7441_Write8_Usermap(0x1D,0x40) ;//; Disable TRI_LLC
//		Device_adv7441_Write8_Usermap(0x37,0x00) ;//; Invert PCLK
		Device_adv7441_Write8_Usermap(0x3C,0xA8) ;//; SOG Sync level for atenuated 
		Device_adv7441_Write8_Usermap(0x68,0xF0);//; Set RGB_OUT

		Device_adv7441_Write8_Usermap(0x73,0xd5); //d2
		Device_adv7441_Write8_Usermap(0x74,0x95); //04  added 11.12
		Device_adv7441_Write8_Usermap(0x75,0x65); //81  added 11.12
		Device_adv7441_Write8_Usermap(0x76,0x59); //20  added 11.12 
//		Device_adv7441_Write8_Usermap(0x7B,0x15) ;//; TURN OFF EAV & SAV CODES, 控制图像宽高
		Device_adv7441_Write8_Usermap(0xF4,0x1D) ;//; Max Drive 
		Device_adv7441_Write8_Usermap(0xBA,0xA0) ;//; Enable HDMI and Analog 
		Device_adv7441_Write8_Usermap(0x06,0x07) ;//; VID_STD=
		Device_adv7441_Write8_Usermap(0x87,ArgMode_7441[inMode].config[4]);//; PLL Divide 
		Device_adv7441_Write8_Usermap(0x88,ArgMode_7441[inMode].config[5]) ;//; PLL Divide Ratio = 870h = 2160d      
		Device_adv7441_Write8_Usermap(0x8F,ArgMode_7441[inMode].config[7]) ;//; Set Free run line length to 382 Decimal   Fpix/28.63636*X= line length                                    
		Device_adv7441_Write8_Usermap(0x90,ArgMode_7441[inMode].config[8]) ;//; Set Free 
		Device_adv7441_Write8_Usermap(0xAB,ArgMode_7441[inMode].config[9]) ;//; Set line 
		Device_adv7441_Write8_Usermap(0xAC,ArgMode_7441[inMode].config[10]) ;//; Set line count to 4E2H for 162MHz     whole line per frame
	
		if((inMode==DEVICE_STD_1080I_50)||(inMode==DEVICE_STD_1080I_60))
			Device_adv7441_Write8_Usermap( 0x05,0x01) ;//; Prim_Mode =010b for GR
		
//		Device_ad7441_SetCPSAV_EAV(adv7441_SavEavVbi[inMode].sav,adv7441_SavEavVbi[inMode].eav);
//		Device_ad7441_SetCPVBI(adv7441_SavEavVbi[inMode].s_vbi,adv7441_SavEavVbi[inMode].e_vbi);
		
//		Device_adv7441_Write8_Usermap(  0xBF, 0x12);//
	}
	else { //Enable HDMI and Analog 
		if((inMode==DEVICE_STD_1080I_50)||(inMode==DEVICE_STD_1080I_60))
			Device_adv7441_Write8_Usermap( 0x05,0x01) ;//; Prim_Mode =010b for GR
		else			
			Device_adv7441_Write8_Usermap(0x05,0x02); //  
		Device_adv7441_Write8_Usermap(0x06,ArgMode_7441[inMode].config[0]); //  can change
		Device_adv7441_Write8_Usermap(0x3a,ArgMode_7441[inMode].config[1]); // can change 
		Device_adv7441_Write8_Usermap(0x3c,ArgMode_7441[inMode].config[2]); // can change 
		Device_adv7441_Write8_Usermap(0x7c,ArgMode_7441[inMode].config[3]); //53   
		Device_adv7441_Write8_Usermap(0x1d,0x40); // Disable TRI_LLC 
		Device_adv7441_Write8_Usermap(0x52,0x00); //  
		Device_adv7441_Write8_Usermap(0x53,0x00); //  
		Device_adv7441_Write8_Usermap(0x54,0x07); //  
		Device_adv7441_Write8_Usermap(0x55,0x0c); //  
		Device_adv7441_Write8_Usermap(0x56,0x99); //  
		Device_adv7441_Write8_Usermap(0x57,0xa9); //  
		Device_adv7441_Write8_Usermap(0x58,0x71); //  
		Device_adv7441_Write8_Usermap(0x59,0x08); //  
		Device_adv7441_Write8_Usermap(0x5a,0x00); //  
		Device_adv7441_Write8_Usermap(0x5b,0x20); //  
		Device_adv7441_Write8_Usermap(0x5c,0xa7); //  
		Device_adv7441_Write8_Usermap(0x5d,0xa9); //  
		Device_adv7441_Write8_Usermap(0x5e,0x1a); //  
		Device_adv7441_Write8_Usermap(0x5f,0x8f); //  
		Device_adv7441_Write8_Usermap(0x60,0x08); //  
		Device_adv7441_Write8_Usermap(0x61,0x00); //  
		Device_adv7441_Write8_Usermap(0x62,0x7a); //  
		Device_adv7441_Write8_Usermap(0x63,0xe1); //  
		Device_adv7441_Write8_Usermap(0x64,0x05); //  
		Device_adv7441_Write8_Usermap(0x65,0x39); //  CSC
		Device_adv7441_Write8_Usermap(0x66,0x1f); //  CSC
		Device_adv7441_Write8_Usermap(0x68,0xF1);  //Auto CSC , YUV Out 
		Device_adv7441_Write8_Usermap(0x6a,0x10); //  调相位
		Device_adv7441_Write8_Usermap(0x6b,0xF3); // 


		Device_adv7441_Write8_Usermap(0x73,0xd5); //d2  调亮度
		Device_adv7441_Write8_Usermap(0x74,0x95); //04  added 11.12
		Device_adv7441_Write8_Usermap(0x75,0x65); //81  added 11.12
		Device_adv7441_Write8_Usermap(0x76,0x59); //20  added 11.12 

		Device_adv7441_Write8_Usermap(0x77,0x04); //04  
		Device_adv7441_Write8_Usermap(0x78,0x08); //08  
		Device_adv7441_Write8_Usermap(0x79,0x02); //02  
		Device_adv7441_Write8_Usermap(0x7a,0x00); //00  
		Device_adv7441_Write8_Usermap(0x7b,0x1c); //  
		Device_adv7441_Write8_Usermap(0x7d,0x00); //60  
		Device_adv7441_Write8_Usermap(0x7e,0x00); //18  
		Device_adv7441_Write8_Usermap(0x85,0x03); // 	Device_adv7441_Write8_Usermap(0x85,0x0E); //
		Device_adv7441_Write8_Usermap(0x87,0x63); //
		Device_adv7441_Write8_Usermap(0x8f,0x00); //
		Device_adv7441_Write8_Usermap(0x90,0x00); //
		Device_adv7441_Write8_Usermap(0x47,0x0A);  //Enable Automatic PLL_Qpump 
		Device_adv7441_Write8_Usermap(0x86,0x0b); //  
		Device_adv7441_Write8_Usermap(0xf4,0x3f);
		Device_adv7441_Write8_Usermap(0x03,0x48); // 
		Device_adv7441_Write8_Usermap(0x04,0x47); //   
		Device_adv7441_Write8_Usermap(0xBA,0xA0); 
	}
	
	Device_adv7441_Write8_Usermap(   0x73, 0x10);//auto AGC
	Device_adv7441_Write8_Usermap(   0xf4, 0xff);
	
	Device_adv7441_SetPhase(inMode);
//	Device_ad7441_SetCPSAV_EAV(adv7441_SavEavVbi[inMode].sav,adv7441_SavEavVbi[inMode].eav);
//	Device_ad7441_SetCPVBI(adv7441_SavEavVbi[inMode].s_vbi,adv7441_SavEavVbi[inMode].e_vbi);
	
	Device_adv7441_Write8_Usermap( 0x7B,0x0F);
	Device_adv7441_Write8_Usermap( 0x7C,0xC0);

	return retVal;
}

static S32 Device_adv7441YPbPrInterFaceSetRes( int inMode)
{
	S32 	retVal = 0;
	Device_adv7441_Write8_Usermap( 0x3A,0x00);
	Device_adv7441_Write8_Usermap( 0x69,0x00);
	Device_adv7441_Write8_Usermap( 0x05,0x01);
	Device_adv7441_Write8_Usermap( 0x06,ArgMode_7441[inMode].config[0]); //  can change
	Device_adv7441_Write8_Usermap( 0x1D,0x40);
	Device_adv7441_Write8_Usermap( 0x3C,0xA8);
	Device_adv7441_Write8_Usermap( 0x47,0x0A);
	Device_adv7441_Write8_Usermap( 0x6B,0xf3);
	Device_adv7441_Write8_Usermap( 0x7B,0x14);
	Device_adv7441_Write8_Usermap( 0xBA,0xA0);
	Device_adv7441_Write8_Usermap( 0x73,0xd5); //d2  调亮度
	Device_adv7441_Write8_Usermap( 0x74,0x95); //04  added 11.12
	Device_adv7441_Write8_Usermap( 0x75,0x65); //81  added 11.12
	Device_adv7441_Write8_Usermap( 0x76,0x59); //20  added 11.12 
	Device_adv7441_Write8_Usermap( 0x7c,0x00);  
	
	Device_adv7441_Write8_Usermap( 0x73, 0x10);//auto AGC
	Device_adv7441_Write8_Usermap( 0xf4, 0xff);
	
	if((inMode==DEVICE_STD_1080P_25)||(inMode==DEVICE_STD_1080P_30)){
	  	Device_adv7441_Write8_Usermap(  0x87,ArgMode_7441[inMode].config[4]);//; PLL Divide 
		Device_adv7441_Write8_Usermap(  0x88,ArgMode_7441[inMode].config[5]) ;//; PLL Divide Ratio = 870h = 2160d      
		Device_adv7441_Write8_Usermap(  0x8F,ArgMode_7441[inMode].config[7]) ;//; Set Free run line length to 382 Decimal   Fpix/28.63636*X= line length              849                     
		Device_adv7441_Write8_Usermap(  0x90,ArgMode_7441[inMode].config[8]) ;//; Set Free 
		Device_adv7441_Write8_Usermap(  0xAB,ArgMode_7441[inMode].config[9]) ;//; Set line 
		Device_adv7441_Write8_Usermap(  0xAC,ArgMode_7441[inMode].config[10]) ;//; Set line count to 4E2H for 162MHz     whole line per frame
	}
	
	Device_adv7441_SetPhase(inMode);
	Device_ad7441_SetCPSAV_EAV(adv7441_SavEavVbi[inMode].sav,adv7441_SavEavVbi[inMode].eav);
	Device_ad7441_SetCPVBI(adv7441_SavEavVbi[inMode].s_vbi,adv7441_SavEavVbi[inMode].e_vbi);
	
	Device_adv7441_Write8_Usermap( 0x7B,0x0F);
	Device_adv7441_Write8_Usermap( 0x7C,0xC0);
	return retVal;
}


static S32 Device_adv7441Config( int inMode, int tmds, AD7441_INPUT InterFace)
{
	S32 	retVal = 0;
	unsigned int regVal=0;

//	Device_adv7441OutputEnable( FALSE);
	switch(InterFace){
		case AD7441_DIGITAL_INPUT:
			Device_adv7441DigitalInterFaceInit();
			Device_adv7441DigitalInterFaceSetRes( inMode);
			break;
		case AD7441_ANALOG_INPUT:
			//if(ArgMode_7441[inMode].config[0]){
				//Device_adv7441Reset();
				//delay1us(100000);
				//Device_adv7441AnalogInterFaceInit();
			//}
			Device_adv7441AnalogInterFaceSetRes( inMode);
			break;
		case AD7441_YPbPr_INPUT:
			Device_adv7441YPbPrInterFaceSetRes( inMode);
			break;
		default:
			ADV_OSA_printf("ADV7441 Unknow Interface in Device_adv7441Config()!\n");
	}

//	if((inMode == DEVICE_STD_1080P_24)||(inMode == DEVICE_STD_1080P_30)){
	if((0)){
		Device_adv7441_Write8_Usermap( 0x8D,0x01);
		Device_adv7441_Write8_Usermap( 0x86,0x1B);
	}else{
		Device_adv7441_Write8_Usermap( 0x8D,0x00);//becase of black line on the left
		Device_adv7441_Write8_Usermap( 0x86,0x0B);
	}

	if((inMode == DEVICE_STD_NTSC)||(inMode == DEVICE_STD_480I)){
		Device_adv7441_Write8_Usermap( 0x05,0x00);
		Device_adv7441_Write8_Usermap( 0x06,0x0A);
	}
	else if((inMode == DEVICE_STD_PAL)||(inMode == DEVICE_STD_576I)){
		Device_adv7441_Write8_Usermap( 0x05,0x00);
		Device_adv7441_Write8_Usermap( 0x06,0x0B);
	}

	regVal = Device_adv7441_Read8_Usermap( 0x12);
	if(regVal & 0x40){
		ADV_OSA_printf("AD7441 is free run!\n");
	}else{
		ADV_OSA_printf("ADV7441 output enable!\n");
		Device_adv7441OutputEnable( TRUE);
	}

	delay1us(500000);
	return retVal;
}

S32 Device_adv7441InvertCbCr ()
{
	unsigned int ret=0;
	unsigned char data;
	
	data = Device_adv7441_Read8_Usermap( 0x27);
	//if(data&0x80)
		data&=0x7F;
	//else
	//	data |=(1<<7);
	Device_adv7441_Write8_Usermap( 0x27, data);

	return ret;
}

S32 Device_SiI9125InitPort( PortID port)
{
	int ret=0;

	switch(port){
		case DEVICE_PORT_DVI:
			if(gAD7441_InputPort != port)
			{
				I2C_ByteWrite_ToDevice ( 0x60, 0x08, 0x05);
				I2C_ByteWrite_ToDevice ( 0x60, 0x09, 0xA2);
				I2C_ByteWrite_ToDevice ( 0x60, 0x5F, 0xC8);
				I2C_ByteWrite_ToDevice ( 0x60, 0xB5, 0x04);
				gAD7441_InputPort = port;
				ADV_OSA_printf("Device_SiI9125InitPor DVI!\n");
			}
			else
			{
				ADV_OSA_printf("Device_SiI9125InitPort Set Input Port DVI Do nothing!\n");
			}
			break;
		case DEVICE_PORT_HDMI:
			if(gAD7441_InputPort != port)
			{
				I2C_ByteWrite_ToDevice ( 0x60, 0x08, 0x05);
				I2C_ByteWrite_ToDevice ( 0x60, 0x09, 0x91);
				I2C_ByteWrite_ToDevice ( 0x60, 0x5F, 0xC8);
				I2C_ByteWrite_ToDevice ( 0x60, 0xB5, 0x04);
				gAD7441_InputPort = port;
				ADV_OSA_printf("Device_SiI9125InitPor HDMI!\n");
			}
			else
			{
				ADV_OSA_printf("Device_SiI9125InitPort Set Input Port DVI Do nothing!\n");
			}
			break;
		default:
			break;
		}
	return ret;
	
}

S32 Device_adv7441InitPort( PortID port)
{
	int ret=0;

	switch(port){
		case DEVICE_PORT_DVI:
			if(gAD7441_InputPort != port)
			{
				Device_adv7441Reset();
				Device_adv7441InitPortDVI();
				gAD7441_InputPort = port;
				delay1us(1000*500);
			}
			else
			{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port DVI Do nothing!\n");
			}
			break;
		case DEVICE_PORT_HDMI:
			if(gAD7441_InputPort != port){
				Device_adv7441Reset();
				Device_adv7441InitPortHDMI();
				gAD7441_InputPort = port;
				delay1us(1000*500);
			}
			else
			{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port HDMI Do nothing!\n");
			}
			break;
		case DEVICE_PORT_VGA:
			if(gAD7441_InputPort != port){
				Device_adv7441Reset();
				Device_adv7441InitPortVGA();
				gAD7441_InputPort = port;				
				delay1us(1000*500);	
			}
			else
			{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port VGA Do nothing!\n");
			}
			break;
		case DEVICE_PORT_YPbPr:
			if(gAD7441_InputPort != port)
			{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port YPbPr!\n");
				Device_adv7441Reset();
				Device_adv7441InitPortYPbPr();
				gAD7441_InputPort = port;				
				delay1us(1000*500);
			}else{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port YPbPr Do nothing!\n");
			}
			break;
		case DEVICE_PORT_CVBS1:
			if(gAD7441_InputPort != port){
				Device_adv7441Reset();
				Device_adv7441InitPortCVBS();
				Device_ad7441_SetFieldOutput();
				gAD7441_InputPort = port;					
				delay1us(1000*500);
			}else{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port CVBS1 Do nothing!\n");
			}
			break;
		case DEVICE_PORT_CVBS2:
			if(gAD7441_InputPort != port){
				Device_adv7441Reset();
				Device_adv7441InitPortCVBS2();
				Device_ad7441_SetFieldOutput();
				gAD7441_InputPort = port;					
				delay1us(1000*500);
			}else{
				ADV_OSA_printf("Device_adv7441InitPort Set Input Port CVBS2 Do nothing!\n");
			}
			break;
		case DEVICE_PORT_SDI:
			ADV_OSA_printf("Device_adv7441InitPort Input Port Error!= PORT_SDI\n");
			break;
		case DEVICE_PORT_AUTO:
			gAD7441_InputPort = port;				
			break;
		default:
			ADV_OSA_printf("Device_adv7441InitPort Input Port Error! PORT= %d\n",port);
	}
	return ret;
}

static S32 Device_adv7441GetResolutionDVI()
{
	S32 	retVal = 0;
//	S32	hpv=0, vps=0, tmds_a=-1;
	int 		inMode;
	ADV7441_SyncInfo sysinformation;
	unsigned char regVal=0;
	short int  width;

	regVal = Device_adv7441_Read8_Usermap( 0x12);
	if(regVal & 0x40){
		ADV_OSA_printf("We Must Reset ADV7441 Becase of FreeRun!\n");
		Device_adv7441Reset();
		gAD7441_Mode = -1;
		gAD7441_TMDS = 0;//-1;
		gAD7441_Hpv= -1;
		gAD7441_Vps= -1;
		gAD7441_InterFace = AD7441_INPUT_UNKNOW;
		gAD7441_InputPort = -1;

		return 0;
	}
	Device_adv7441GetInfo_D(&sysinformation);
	
	if(gAD7441_InputPort==DEVICE_PORT_DVI)
	{
		gAD7441_TMDS = sysinformation.TMDS_A;
		ADV_OSA_printf("TMDS_A!\n");
	}
	else if(gAD7441_InputPort==DEVICE_PORT_HDMI)
	{
		gAD7441_TMDS = sysinformation.TMDS_B;
		ADV_OSA_printf("TMDS_B!\n");
	}
	
	if(!gAD7441_TMDS){
		gAD7441_Mode = -1;
		ADV_OSA_printf("return!\n");
		return 0;
	}
	gAD7441_Hpv=sysinformation.VTotPix;
	gAD7441_Vps=sysinformation.VFreq;
	ADV_OSA_printf("gAD7441_Hpv=%d!\n",gAD7441_Hpv);
	ADV_OSA_printf("gAD7441_Vps=%d!\n",gAD7441_Vps);
	Device_adv7441GetModeIndex(gAD7441_Hpv, gAD7441_Vps, &inMode);
	
	if((inMode == DEVICE_STD_VGA_1366X768X60)||(inMode == DEVICE_STD_VGA_1280X768X60))
		Device_adv7441_check_1366Or1280( &inMode);
	if((inMode == DEVICE_STD_VGA_1280X720X60)||(inMode == DEVICE_STD_720P_60))
		Device_adv7441_check_VGAorVideo( &inMode);
	if((inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_1080P_60))
		Device_adv7441_check_1080_DMTorGTF(&inMode);

/*
	if(gAD7441_TMDS&&(inMode==-1)){
		Device_adv7441GetLineWidth( &width);
		if(((width>637)&&(width<645))){
			inMode=gAD7441_Mode;
		}
		if(((width>795)&&(width<805))){
			inMode=gAD7441_Mode;
		}
	}

	ADV_OSA_printf("inMode=%d",inMode);
	*/
	if(inMode == -1){
	//	Device_adv7441OutputEnable	( FALSE);
	}

	if(/*(gAD7441_Mode != inMode)&&*/(inMode != -1)){
		ADV_OSA_printf("     inMode=%d    ",inMode);
		gAD7441_Mode = inMode;
		Device_adv7441GetInterface( &gAD7441_TMDS, &gAD7441_InterFace);
		Device_adv7441Config( gAD7441_Mode, gAD7441_TMDS, gAD7441_InterFace);
		if((inMode == DEVICE_STD_NTSC)||(inMode == DEVICE_STD_PAL)
			||(inMode == DEVICE_STD_480I)||(inMode == DEVICE_STD_576I)
			||(inMode == DEVICE_STD_1080I_50)||(inMode == DEVICE_STD_1080I_60)){
			Device_ad7441_SetFieldOutput();
		}else{
			Device_ad7441_SetVSOutput();
		}
	}else{
		gAD7441_Mode = inMode;
		ADV_OSA_printf("inMode == -1!\n");
	}	


	return retVal;
}

static S32 Device_adv7441GetResolutionVGA()
{
	S32 	retVal = 0;
	S32	hpv=0, vps=0;//, tmds_a=-1;
	int 		inMode;
//	int 		retry = RETRYNUMS;
//	ADV7441_SyncInfo sysinformation;
	unsigned char regVal=0;
	short int  width;

	gAD7441_TMDS = 0;
	regVal = Device_adv7441_Read8_Usermap( 0x12);
	if(regVal & 0x40){

	}
	Device_adv7441GetInfo_A(&hpv,&vps);
	gAD7441_Hpv = hpv;
	gAD7441_Vps = vps;

	Device_adv7441GetModeIndex(gAD7441_Hpv, gAD7441_Vps, &inMode);
	//ADV_OSA_printf("VGA Inmode=%d   ",inMode);
	if((inMode == DEVICE_STD_VGA_1366X768X60)||(inMode == DEVICE_STD_VGA_1280X768X60))
		Device_adv7441_check_1366Or1280( &inMode);
	if((inMode == DEVICE_STD_VGA_1280X720X60)||(inMode == DEVICE_STD_720P_60))
		Device_adv7441_check_VGAorVideo( &inMode);
	if((inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_1080P_60))
		Device_adv7441_check_1080_DMTorGTF( &inMode);

	if(gAD7441_TMDS&&(inMode==-1)){
		Device_adv7441GetLineWidth( &width);
		if(((width>637)&&(width<645))){
			inMode=gAD7441_Mode;
		}
		if(((width>795)&&(width<805))){
			inMode=gAD7441_Mode;
		}
	}


	if(inMode == -1){
	//	Device_adv7441OutputEnable	( FALSE);
	}

	if(/*(gAD7441_Mode != inMode)&&*/(inMode != -1)){
		gAD7441_Mode = inMode;
		//Device_adv7441GetInterface( &gAD7441_TMDS, &gAD7441_InterFace);
		/*if(Device_adv7441IsGreenSync()){
//			gAD7441_Mode = -1;
			gAD7441_InterFace = AD7441_ANALOG_INPUT;//AD7441_YPbPr_INPUT;
			Device_adv7441Config( gAD7441_Mode, 0, gAD7441_InterFace);
			if((inMode == DEVICE_STD_NTSC)||(inMode == DEVICE_STD_PAL)
				||(inMode == DEVICE_STD_480I)||(inMode == DEVICE_STD_576I)
				||(inMode == DEVICE_STD_1080I_50)||(inMode == DEVICE_STD_1080I_60)){
				Device_ad7441_SetFieldOutput();
			}else{
				Device_ad7441_SetVSOutput();
			}
		}else*/{
			gAD7441_InterFace = AD7441_ANALOG_INPUT;
			Device_adv7441Config( gAD7441_Mode, 0, gAD7441_InterFace);
			if((inMode == DEVICE_STD_NTSC)||(inMode == DEVICE_STD_PAL)
				||(inMode == DEVICE_STD_480I)||(inMode == DEVICE_STD_576I)
				||(inMode == DEVICE_STD_1080I_50)||(inMode == DEVICE_STD_1080I_60)){
				Device_ad7441_SetFieldOutput();
			}else{
				Device_ad7441_SetVSOutput();
			}
		}
	}else{
		gAD7441_Mode = inMode;
	}	

	return retVal;
}

static S32 Device_adv7441GetResolutionYPbPr()
{
	S32 	retVal = 0;
	S32	hpv=0, vps=0;//, tmds_a=-1;
	int 		inMode;
//	int 		retry = RETRYNUMS;
//	ADV7441_SyncInfo sysinformation;
	unsigned char regVal=0;
	short int  width;
	
	regVal = Device_adv7441_Read8_Usermap( 0x12);
	if(regVal & 0x40){
		ADV_OSA_printf("We Must Reset ADV7441 Becase of FreeRun!\n");
		Device_adv7441Reset();
//		gAD7441_CurStatus = -1;
		gAD7441_Mode = -1;
		gAD7441_TMDS = 0;//-1;
		gAD7441_Hpv= -1;
		gAD7441_Vps= -1;
		gAD7441_InterFace = AD7441_INPUT_UNKNOW;
		gAD7441_InputPort = -1;



		return 0;
	}

	Device_adv7441GetInfo_A(&hpv,&vps);
	gAD7441_Hpv = hpv;
	gAD7441_Vps = vps;

	Device_adv7441GetModeIndex(gAD7441_Hpv, gAD7441_Vps, &inMode);
	if((inMode == DEVICE_STD_VGA_1366X768X60)||(inMode == DEVICE_STD_VGA_1280X768X60))
		Device_adv7441_check_1366Or1280( &inMode);
	if((inMode == DEVICE_STD_VGA_1280X720X60)||(inMode == DEVICE_STD_720P_60))
		Device_adv7441_check_VGAorVideo( &inMode);
	if((inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_1080P_60))
		Device_adv7441_check_1080_DMTorGTF( &inMode);


	if(gAD7441_TMDS&&(inMode==-1)){
		Device_adv7441GetLineWidth( &width);
		if(((width>637)&&(width<645))){
			inMode=gAD7441_Mode;
		}
		if(((width>795)&&(width<805))){
			inMode=gAD7441_Mode;
		}
	}


	if(inMode == -1){
		//Device_adv7441OutputEnable	( FALSE);
	}

	if((gAD7441_Mode != inMode)&&(inMode != -1)){
		gAD7441_Mode = inMode;
		if(Device_adv7441IsGreenSync()){
			//Device_adv7441GetInterface( &gAD7441_TMDS, &gAD7441_InterFace);
			gAD7441_InterFace = AD7441_YPbPr_INPUT;
			Device_adv7441Config( gAD7441_Mode, 0, gAD7441_InterFace);
			if((inMode == DEVICE_STD_NTSC)||(inMode == DEVICE_STD_PAL)
				||(inMode == DEVICE_STD_480I)||(inMode == DEVICE_STD_576I)
				||(inMode == DEVICE_STD_1080I_50)||(inMode == DEVICE_STD_1080I_60)){
				Device_ad7441_SetFieldOutput();
			}else{
				Device_ad7441_SetVSOutput();
			}
		}else{
			gAD7441_Mode = -1;
		}
	}else{
		gAD7441_Mode = inMode;
	}	

	if(gAD7441_Mode != -1){
		ADV_OSA_printf("###########################################\n");
		ADV_OSA_printf("Device_adv7441GetResolutionYPbPr()!\n");
		ADV_OSA_printf("gAD7441_std = %s \n",ArgMode_7441[gAD7441_Mode].name);
		ADV_OSA_printf("gAD7441_hpv = %d gAD7441_vps = %d \n",gAD7441_Hpv,gAD7441_Vps);
		ADV_OSA_printf("gAD7441_Mode = %d gAD7441_TMDS = %d gAD7441_InterFace = %d\n",gAD7441_Mode,gAD7441_TMDS,gAD7441_InterFace);
	}else{
		ADV_OSA_printf("###########################################\n");
		ADV_OSA_printf("Device_adv7441GetResolutionYPbPr()!\n");
		ADV_OSA_printf("gAD7441_std = unknow \n");
		ADV_OSA_printf("gAD7441_hpv = %d gAD7441_vps = %d \n",gAD7441_Hpv,gAD7441_Vps);
		ADV_OSA_printf("gAD7441_Mode = %d gAD7441_TMDS = %d gAD7441_InterFace = %d\n",gAD7441_Mode,gAD7441_TMDS,gAD7441_InterFace);
		ADV_OSA_printf("###########################################\n");
	}
	return retVal;
}


static S32 Device_adv7441GetResolutionCVBS()
{
//	S32	tmds_a=-1;
	int 		inMode;
	
	inMode = -1;
	
	gAD7441_TMDS = 0;
	
	Device_adv7441GetModeIndexSD( &inMode);
	gAD7441_Mode = inMode;

	if(gAD7441_Mode != -1){
		ADV_OSA_printf("###########################################\n");
		ADV_OSA_printf("Device_adv7441GetResolutionCVBS()!\n");
		ADV_OSA_printf("gAD7441_std = %s \n",ArgMode_7441[gAD7441_Mode].name);
		ADV_OSA_printf("###########################################\n");
		
	}else{
		ADV_OSA_printf("###########################################\n");
		ADV_OSA_printf("Device_adv7441GetResolutionCVBS()!\n");
		ADV_OSA_printf("gAD7441_std = unknow \n");
		ADV_OSA_printf("###########################################\n");
		Device_adv7441OutputEnable( FALSE);
		
	}
	return inMode;
}

static S32 Device_adv7441GetResolution()
{

	int ret=0;
	switch(gAD7441_InputPort)
	{
		case DEVICE_PORT_DVI:
		case DEVICE_PORT_HDMI:
			Device_adv7441GetResolutionDVI();
			break;
		case DEVICE_PORT_VGA:
			Device_adv7441GetResolutionVGA();
			break;
		case DEVICE_PORT_YPbPr:
			//Device_adv7441GetResolutionYPbPr();
			break;
		case DEVICE_PORT_CVBS1:
		case DEVICE_PORT_CVBS2:
			Device_adv7441GetResolutionCVBS();
			break;
	}
	return ret;
}


/* reset ADV7441 OFM logic  */
S32 Device_adv7441Reset ()
{
	S32           retVal = 0;
	U8           regVal;

	retVal = Device_adv7441_Write8_Usermap(0x0F,0x80);
	while(1){
		delay1us(100);
		regVal = Device_adv7441_Read8_Usermap(0x0F);
		if(!(regVal & 0x80)){
			printf("ADV7441 Reset OK!\n");
			break;
		}else{
			ADV_OSA_printf("waiting ADV7441 Reset.......\n");
		}
	}
	if (0 == retVal)
	{
		retVal = Device_adv7441_Write8_Usermap(0x05,0x02);
		retVal = Device_adv7441_Write8_Usermap(0x85,0x03);
		retVal = Device_adv7441_Write8_Usermap(0x03,0x0C);
	}
	return (retVal);    
}

/*
  Enable ADV7441 output port
*/
S32 Device_adv7441OutputEnable (  bool enable )
{
	S32 status = 0;
	unsigned int regVal=0;

	if(enable){
		regVal = Device_adv7441_Read8_Usermap(0x03);
		regVal = regVal&(~(1<<6));
		Device_adv7441_Write8_Usermap( 0x03,regVal);
		regVal = Device_adv7441_Read8_Usermap(0x04);
		regVal = regVal|(1<<3);
		Device_adv7441_Write8_Usermap( 0x04,regVal);
	}else{
		regVal = Device_adv7441_Read8_Usermap(0x03);
		regVal = regVal|(1<<6);
		Device_adv7441_Write8_Usermap( 0x03,regVal);
		regVal = Device_adv7441_Read8_Usermap(0x04);
		regVal = regVal&(~(1<<3));
		Device_adv7441_Write8_Usermap( 0x04,regVal);
	}

	return status;
}




#if(MATHINE_TYPE==XP_530)
/*
  Get ADV7441 detect vide standard status

  Can be called for each channel
*/
S32 Device_adv7441GetVideoStatus (void)
{	
	S32 status = 0;
//#if(INPUT_BOARD==ADV7441A_INPUT)
	ADV7441_SyncInfo sysinformation;
	
	Device_adv7441InitPort( g_Adv7441CurrentInput);
	ADV_OSA_printf("g_Adv7441CurrentInput=%d!\n",g_Adv7441CurrentInput);
	Device_adv7441GetResolution();
	if(gAD7441_Mode != -1){
		g_Adv7441VideoDetect = TRUE;
		g_Adv7441FrameWidth = adv7441_gLineNum[gAD7441_Mode].width;
		ADV_OSA_printf("g_Adv7441FrameWidth=%d!\n",g_Adv7441FrameWidth);
		g_Adv7441FrameHeight = adv7441_gLineNum[gAD7441_Mode].hight;
		ADV_OSA_printf("g_Adv7441FrameHeight=%d!\n",g_Adv7441FrameHeight);
		g_Adv7441FrameHz = adv7441_gLineNum[gAD7441_Mode].vps;
		g_Adv7441Interlaced = FALSE;
		
		if(gAD7441_Mode < DEVICE_STD_CIF)	
			g_Adv7441Interlaced = TRUE;
		if(gAD7441_Mode >DEVICE_STD_720P_50 &&  gAD7441_Mode <DEVICE_STD_1080P_60)	
			g_Adv7441Interlaced = TRUE;
	}else{
		g_Adv7441Interlaced = FALSE;
	}
	Device_SiI9125InitPort( g_Adv7441CurrentInput);
//#elif(INPUT_BOARD==SII9125_INPUT)
//	Device_SiI9125InitPort( g_Adv7441CurrentInput);
//#elif(INPUT_BOARD==SDI_INPUT)
//
//#endif
	return status;
}

#endif


