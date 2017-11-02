/**
  ******************************************************************************
  * ÎÄ¼þÃû³Æ £ºdisplay.c 
  * ¹¦    ÄÜ £ºÓÃÀ´½øÐÐÏÔÊ¾ÆÁÄÚÈÝµÄÊä³öÏà¹ØµÄº¯Êý
  * °æ 	  ±¾ £ºV0.0
  * ¿â °æ ±¾ £ºV3.5.0
  * ÈÕ    ÆÚ £º2016-5-24
  * ±à	  Ð´ £ºlyg
   
  *	ÐÞ¸Ä¼ÇÂ¼ £º		   	
  ******************************************************************************
**/ 


#include "DISPLAY.h"
#include "systick.h"
#include "key.h"
#include "eeprom.h"
#include "led.h"

/*·¢ËÍ6×Ö½ÚÖ¸Áî*/



const u8 display_num[10]={0x27,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x025,0x026};
const u8 display_onoff[2][3]={0x1b,0x0e,0x0d,0x0e,0x05,0x05};

//½øÐÐWB MODEÄ£Ê½Ñ¡ÔñÊ±£¬ÓÃÊý×é´æÄ£Ê½µÄÏÔÊ¾ÄÚÈÝ
const u8 display_wb_mode[9][8]={
0x1b,0x1b,0x1b,0x1b,0x00,0x14,0x13,0x0e,  //AUTO
0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x08,0x0d,  //IN
0x1b,0x1b,0x1b,0x1b,0x1b,0x0e,0x14,0x13,  //OUT
0x0e,0x0d,0x04,0x1b,0x0f,0x14,0x12,0x07,  //ONE PUSH
0x1b,0x1b,0x1b,0x1b,0x1b,0x00,0x13,0x0c,  //ATM
0x1b,0x1b,0x1b,0x1b,0x0c,0x00,0x0d,0x14,  //MANU
0x0e,0x14,0x13,0x1b,0x00,0x14,0x13,0x0e,  //OUT AUTO
0x0b,0x00,0x0c,0x1b,0x00,0x14,0x13,0x0e,  //LAM AUTO
0x1b,0x0b,0x00,0x0c,0x1b,0x05,0x08,0x17   //LAM AUTO	
};

const u8 display_select_flag[3]={0x1b,0x4f,0x1c};
const u8 display_ae_mode[5][8]={
char_space,char_space,char_space,char_space,char_A,char_U,char_T,char_O,  //AUTO
char_space,char_space,char_M,char_A,char_N,char_U,char_A,char_L,	//MANUAL
char_S,char_H,char_U,char_T,char_space,char_P,char_R,char_I,  //SHUT PRI
char_I,char_R,char_I,char_S,char_space,char_P,char_R,char_I, //IRIS PRI
char_space,char_space,char_B,char_R,char_I,char_G,char_H,char_T,  //BRIGHT
};
const u8 display_shutter[22][7]={
char_space,char_space,char_space,char_space,num_1,0x4e,num_1,
char_space,char_space,char_space,char_space,num_1,0x4e,num_2,
char_space,char_space,char_space,char_space,num_1,0x4e,num_3,
char_space,char_space,char_space,char_space,num_1,0x4e,num_6,
char_space,char_space,char_space,num_1,0x4e,num_1,num_2,
char_space,char_space,char_space,num_1,0x4e,num_2,num_5,	
char_space,char_space,char_space,num_1,0x4e,num_5,num_0,	
char_space,char_space,char_space,num_1,0x4e,num_7,num_5,
char_space,char_space,num_1,0x4e,num_1,num_0,num_0,	
char_space,char_space,num_1,0x4e,num_1,num_2,num_0,	
char_space,char_space,num_1,0x4e,num_1,num_5,num_0,	
char_space,char_space,num_1,0x4e,num_2,num_1,num_5,	
char_space,char_space,num_1,0x4e,num_3,num_0,num_0,	
char_space,char_space,num_1,0x4e,num_4,num_2,num_5,	
char_space,char_space,num_1,0x4e,num_6,num_0,num_0,		
char_space,num_1,0x4e,num_1,num_0,num_0,num_0,	
char_space,num_1,0x4e,num_1,num_2,num_5,num_0,	
char_space,num_1,0x4e,num_1,num_7,num_5,num_0,	
char_space,num_1,0x4e,num_2,num_5,num_0,num_0,	
char_space,num_1,0x4e,num_3,num_5,num_0,num_0,	
char_space,num_1,0x4e,num_6,num_0,num_0,num_0,	
num_1,0x4e,num_1,num_0,num_0,num_0,num_0	
};

const u8 display_iris[14][5]={
char_C,char_L,char_O,char_S,char_E,  // 0 CLOSE
char_space,char_space,char_F,num_1,num_4,    //05 F14
char_space,char_space,char_F,num_1,num_1,    //06 F11
char_space,char_F,num_9,0x4c,num_6,      //f9.6
char_space,char_space,char_space,char_F,num_8,  //f8
char_space,char_F,num_6,0x4c,num_8,      //f6.8
char_space,char_F,num_5,0x4c,num_6,      //f6.8
char_space,char_F,num_4,0x4c,num_8,      //f6.8	
char_space,char_space,char_space,char_F,num_4,  //f8
char_space,char_F,num_3,0x4c,num_4,      //f6.8
char_space,char_F,num_2,0x4c,num_8,      //f6.8
char_space,char_F,num_2,0x4c,num_4,      //f6.8	
char_space,char_space,char_space,char_F,num_2,  //f8
char_space,char_F,num_1,0x4c,num_6      //f6.8
};

const u8 display_near_limit[15][4]={
char_O,char_V,char_E,char_R,
char_space,num_2,num_5,char_M,
char_space,num_1,num_1,char_M,
char_space,char_space,num_7,char_M,	
num_4,0x4c,num_9,char_M,
num_3,0x4c,num_7,char_M,
num_2,0x4c,num_9,char_M,
num_2,0x4c,num_3,char_M,
num_1,0x4c,num_8,char_M,
num_1,0x4c,num_5,char_M,
num_1,0x4c,num_2,char_M,	
char_space,char_space,num_1,char_M,
num_3,num_0,char_C,char_M,
char_space,num_8,char_C,char_M,
char_space,num_1,char_C,char_M	
};
const u8 display_format[10][7]={
num_1,num_0,num_8,num_0,char_I,num_5,num_0,
num_1,num_0,num_8,num_0,char_I,num_6,num_0,
num_1,num_0,num_8,num_0,char_P,num_6,num_0,
num_1,num_0,num_8,num_0,char_P,num_5,num_0,
num_1,num_0,num_8,num_0,char_P,num_3,num_0,
num_1,num_0,num_8,num_0,char_P,num_2,num_5,		
char_space,num_7,num_2,num_0,char_P,num_6,num_0,
char_space,num_7,num_2,num_0,char_P,num_5,num_0,
char_space,num_7,num_2,num_0,char_P,num_3,num_0,
char_space,num_7,num_2,num_0,char_P,num_2,num_5	
};

const u8 format_to_command[10]={0x04,0x02,0x15,0x14,0x07,0x08,0x0a,0x0c,0x0f,0x11};

/*********************************************
ÒÔÏÂ±äÁ¿ÎªÉèÖÃ²ÎÊýÊ±µÄ±äÁ¿£¬¸³ÖµÎª³ö³§ÉèÖÃµÄÖµ
**********************************************/
u8 save_flag=0;

u8 zoom_speed=10;
u8 zoom_data=10;
u8 dig_zoom=1;
u8 osd=1;
u8 osd_pre;
u8 zoom_display=0;
u8	focus_mode=0;
u8	af_sens=0;
u8	focus_light=0;
u8	near_limit=14;
u8 aperture =4;
u8 color_gain=4;
u8 color_hum=7;
u8 chroma_suppress=2;
u8 gamma=0;
u8 mirror=1;
u8 flip=1;
u8 freeze=1;
u8 pic_effect=0;
u8 wb_mode=0;
u8 red_gain=202;
u8 blue_gain=175;
u8	ae_mode=0;
u8	shutter=5;
u8	iris=9;
u8	gain=9;
u8	bri=9;
u8	slow_ae=1;
u8	slow_shutter=1;
u8	blc=1;
u8	wdr=3;
u8	nr_level=3;
u8	stabilizer=1;
u8	defog_mode=1;
u8	hi_resolution=1;
u8	hi_sensitivity=1;
u8  cameraID=1;
u8	format=0;
u8	LR_hant=1;
u8	baudrate=2;
u8	address=1;
u8	aux=1;



u8 zoom_seeed_pre;
u8 zoom_data_pre;
u8 dig_zoom_pre;
u8 zoom_display_pre;
u8	focus_mode_pre;
u8	af_sens_pre;
u8	focus_light_pre;
u8	near_limit_pre;
u8 aperture_pre;
u8 color_gain_pre;
u8 color_hum_pre;
u8 chroma_suppress_pre;
u8 gamma_pre;
u8 mirror_pre;
u8 flip_pre;
u8 freeze_pre;
u8 pic_effect_pre;
u8 wb_mode_pre;
u8 red_gain_pre;
u8 blue_gain_pre;
u8	ae_mode_pre;
u8	shutter_pre;
u8	iris_pre;
u8	gain_pre;
u8	bri_pre;
u8	slow_ae_pre;
u8	slow_shutter_pre;
u8	blc_pre;
u8	wdr_pre;
u8	nr_level_pre;
u8	stabilizer_pre;
u8	defog_mode_pre;
u8	hi_resolution_pre;
u8	hi_sensitivity_pre;
u8  cameraID_pre;
u8	format_pre;
u8	LR_hant_pre;
u8	baudrate_pre;
u8	address_pre;
u8	aux_pre;
u8 zone_pre;
u8 privacy_data_pre[4][5];

u8 scene_num=0;
u8 scene_data[4][2]={10,0,10,0,10,0,10,0};  //time_s  exist

u8 zone=0;
u8 privacy_data[4][5]={
0x01,20,20,5,5,
0x01,20,20,5,5,
0x01,20,20,5,5,
0x01,20,20,5,5
};
/**************************************************/

u8	zoom_set[9]={0x81,0x01,0x04,0x47,0x03,0x08,0x0b,0x03,0xff};
u16 zoom_irq_num[31]={
0x0000,
0x1851,
0x22be,
0x28f6,
0x2d45,
0x3086,
0x3320,
0x3549,
0x371e,
0x38b3,
0x3a12,
0x3b42,
0x3c47,
0x3d25,
0x3ddf,
0x3e7b,
0x3efb,
0x3f64,
0x3fba,
0x4000,
0x6000,
0x6a80,
0x7000,
0x7300,
0x7500,
0x76c0,
0x7800,
0x78c0,
0x7980,
0x7a00,
0x7ac0
};
u8 zoom_data_command[31][4]={
0,0,0,0,
1,8,5,1,
2,2,0x0b,0x0e,
2,8,0x0f,0x06,
2,0x0d,4,5,
3,0,8,6,
3,3,2,0,
3,5,4,9,
3,7,1,0x0e,
3,8,0x0b,3,
3,0x0a,1,2,
3,0x0b,4,2,
3,0x0c,4,7,
3,0x0d,2,5,
3,0x0d,0x0d,0x0f,
3,0x0e,7,0x0b,
3,0x0e,0x0f,0x0b,
3,0x0f,6,4,
3,0x0f,0x0b,0x0a,
4,0,0,0,  //20±¶Ë
6,0,0,0,
6,0x0a,8,0,
7,0,0,0,
7,3,0,0,
7,5,4,0,
7,6,0x0c,0,
7,8,0,0,
7,8,0x0c,0,
7,9,8,0,
7,0x0a,0,0,
7,0x0a,0x0c,0	
};


/*****ae_modeÉèÖÃÊý¾Ý********/
u8 ae_mode_auto[6]={0x81,0x01,0x04,0x39,0x00,0xff};
u8 ae_mode_manual[6]={0x81,0x01,0x04,0x39,0x03,0xff};
u8 ae_mode_shutpri[6]={0x81,0x01,0x04,0x39,0x0a,0xff};
u8 ae_mode_irisprl[6]={0x81,0x01,0x04,0x39,0x0b,0xff};
u8 ae_mode_bright[6]={0x81,0x01,0x04,0x39,0x0d,0xff};
u8 slow_ae_set[6]={0x81,0x01,0x04,0x5d,0,0xff};		
u8 slow_shutter_set[6]={0x81,0x01,0x04,0x5a,2,0xff};
u8 shutter_set[9]={0x81,0x01,0x04,0x4a,0x00,0x00,0x00,0,0xFF};
u8 iris_set[9]={0x81,0x01,0x04,0x4b,0x00,0x00,0x00,0,0xFF};
u8 gain_set[9]={0x81,0x01,0x04,0x4c,0x00,0x00,0x00,0,0xFF};
u8 bright_set[9]={0x81,0x01,0x04,0x4D,0x00,0x00,0x00,0,0xFF};	

/******focus_pageÉèÖÃÊý¾Ý*******/
u8 focus_mode_set[6]={0x81,0x01,0x04,0x38,2,0xff};
u8 af_sens_set[6]={0x81,0x01,0x04,0x58,2,0xff};
u8 focus_light_set[6]={0x81,0x01,0x04,0x11,0,0xff};
u8 near_limit_set[9]={0x81,0x01,0x04,0x28,0,0,0,0,0xff};

/************WHITE BALANEÉèÖÃÊý¾Ý****************/
u8 wb_mode_set[6]={0x81,0x01,0x04,0x35,0,0xff};
u8 red_gain_set[9]={0x81,0x01,0x04,0x43,0x00,0x00,0,0,0xff};
u8 blue_gain_set[9]={0x81,0x01,0x04,0x44,0x00,0x00,0,0,0xff};
u8 one_push_set[6]={0x81,0x01,0x04,0x35,0x05,0xff};

/*******************IMAGE_PAGEÉèÖÃÊý¾Ý************************/
u8 aperture_set[9]={0x81,0x01,0x04,0x42,0x00,0x00,0x00,0,0xFF};
u8 color_gain_set[9]={0x81,0x01,0x04,0x49,0x00,0x00,0x00,0,0xFF};
u8 color_hum_set[9]={0x81,0x01,0x04,0x4f,0x00,0x00,0x00,0,0xFF};
u8 chroma_suppress_set[6]={0x81,0x01,0x04,0x5f,0,0xff};
u8 gamma_set[6]={0x81,0x01,0x04,0x5b,0,0xff};
u8 mirror_set[6]={0x81,0x01,0x04,0x61,0,0xff};
u8 flip_set[6]={0x81,0x01,0x04,0x66,0,0xff};
u8 freeze_set[6]={0x81,0x01,0x04,0x62,0,0xff};
u8 pic_effect_set[6]={0x81,0x01,0x04,0x63,0,0xff};


/****ADVANCE_PAGEÉèÖÃÊý¾Ý*******/
u8 blc_set[6]={0x81,0x01,0x04,0x33,0x02,0xff};
u8 wdr_set[6]={0x81,0x01,0x04,0x3d,0x00,0xff};
u8 nr_level_set[6]={0x81,0x01,0x53,0x3d,0x00,0xff};
u8 stabilizer_set[6]={0x81,0x01,0x34,0x3d,0x00,0xff};
u8 defog_mode_set[6]={0x81,0x01,0x37,0x3d,0x00,0xff};
u8 expcomp_set[6]={0x81,0x01,0x04,0x3e,0x03,0xff};

/*************PRIVACY_SET ÉèÖÃÊý¾Ý******************************************************/

u8 display_privacy_onoff_command[9]={0x81,0x01,0x04,0x77,0x00,0x00,0x00,0x00,0xff};
u8 display_privacy_area_xywh[14]={0x81,0x01,0x04,0x6f,0x00,0x00,0x00,0x00,0x00,0,3,0,3,0xff};
u8 privacy_mask_color[11]={0x81,0x01,0x04,0x78,0x00,0x00,0x00,0x00,0x0a,0x1a,0xff};
/*************************************************************************************/

/**********SENSER_PAGEÉèÖÃÊý¾Ý**********/
u8 hi_resolution_set[6]={0x81,0x01,0x04,0x52,0x02,0xff};
u8 hi_sensitivity_set[6]={0x81,0x01,0x04,0x5e,0x02,0xff};
/*************************************************/
u8 title_clear[6]={0x81,0x01,0x04,0x74,0x1f,0xff};
u8 title_display_on[6]={0x81,0x01,0x04,0x74,0x2f,0xff};

u8 register_set[8]={0x81,0x01,0x04,0x24,0x00,0x00,0x00,0xff};
u8 camera_display_set[6]={0x81,0x01,0x06,0x06,0x03,0xff};

u8 PowerInq[5]={0x81,0x09,0x04,0x00,0xff};
u8 ZoomIrq[5]={0x81,0x09,0x04,0x47,0xff};

u8 power_off[6]={0x81,0x01,0x04,0x00,0x03,0xff};
u8 power_on[6]={0x81,0x01,0x04,0x00,0x02,0xff};

u8 focusadd_command[6]={0x81,0x01,0x04,0x08,0x02,0xff};
u8 focussub_command[6]={0x81,0x01,0x04,0x08,0x03,0xff};
u8 focusstop_command[6]={0x81,0x01,0x04,0x08,0x00,0xff};
u8 zoomadd_command[6]={0x81,0x01,0x04,0x07,0x22,0xff};
u8 zoomsub_command[6]={0x81,0x01,0x04,0x07,0x32,0xff};
u8 zoomstop_command[6]={0x81,0x01,0x04,0x07,0x00,0xff};
u8 one_push_trigger[6]={0x81,0x01,0x04,0x18,0x01,0xff};

u8 Digita_Zoom_set[6]={0x81,0x01,0x04,0x06,3,0xff};

void Send_Command(u8 *send_data)
{
		u8 len=0;
		u8 i=0;
		while((*(send_data+i))!=0xff)
		{
			i++;
		}
		len=i+1;
		USART2_Puts((char *)send_data,len);
	  delay_ms(10);		
}
void initGlobalVar(void)
{
	u32 i;
	for(i=10;i<E2ROM_LAST_ADDR;i++)
		TWI_ReadByte((&work_para[i]),i);
		
	zoom_speed=work_para[E2ROM_ZOOM_SPEED_ADDR];
	zoom_data=work_para[E2ROM_ZOOM_DATA_ADDR];
	dig_zoom=work_para[E2ROM_DIG_ZOOM_ADDR];
	osd=work_para[E2ROM_OSD_ADDR];
	zoom_display=work_para[E2ROM_ZOOM_DISPLAY_ADDR];
	focus_mode=work_para[E2ROM_FOCUS_LIGHT_ADDR];
	af_sens=work_para[E2ROM_AF_SENS_ADDR];
	focus_light=work_para[E2ROM_FOCUS_LIGHT_ADDR];
	near_limit=work_para[E2ROM_NEAR_LIMIT_ADDR];

	aperture =work_para[E2ROM_APERTURE_ADDR];
	color_gain=work_para[E2ROM_COLOR_GAIN_ADDR];
	color_hum=work_para[E2ROM_COLOR_HUM_ADDR];
	chroma_suppress=work_para[E2ROM_CHROMA_SUPPRESS_ADDR];
	gamma=work_para[E2ROM_GAMMA_ADDR];
	mirror=work_para[E2ROM_MIRROR_ADDR];
	flip=work_para[E2ROM_FLIP_ADDR];
	freeze=work_para[E2ROM_FREEZE_ADDR];
	pic_effect=work_para[E2ROM_PIC_EFFECT_ADDR];
	wb_mode=work_para[E2ROM_WB_MODE_ADDR];
	red_gain=work_para[E2ROM_RED_GAIN_ADDR];
	blue_gain=work_para[E2ROM_BAUDRATE_ADDR];
	ae_mode=work_para[E2ROM_AE_MODE_ADDR];
	shutter=work_para[E2ROM_SHUTTER_ADDR];
	iris=work_para[E2ROM_IRIS_ADDR];
	gain=work_para[E2ROM_GAIN_ADDR];
	bri=work_para[E2ROM_BRI_ADDR];
	slow_ae=work_para[E2ROM_SLOW_AE_ADDR];
	slow_shutter=work_para[E2ROM_SLOW_SHUTTER_ADDR];

	blc=work_para[E2ROM_BLC_ADDR];
	wdr=work_para[E2ROM_WDR_ADDR];
	nr_level=work_para[E2ROM_NR_LEVEL_ADDR];
	stabilizer=work_para[E2ROM_STABILIZER_ADDR];
	defog_mode=work_para[E2ROM_DEFOG_MODE_ADDR];

	hi_resolution=work_para[E2ROM_HI_RESOLUTION_ADDR];
	hi_sensitivity=work_para[E2ROM_HI_SENSITIVITY_ADDR];

	cameraID=1;

	format=work_para[E2ROM_FORMAT_ADDR];
	LR_hant=work_para[E2ROM_LR_HANT_ADDR];
	baudrate=work_para[E2ROM_BAUDRATE_ADDR];
	address=work_para[E2ROM_ADDRESS_ADDR];
	aux=work_para[E2ROM_AUX_ADDR];

	scene_num=work_para[E2ROM_SCENE_NUM_ADDR];
	for(i=0;i<6;i++)
		scene_data[i/2][i%2]=work_para[E2ROM_SCENE_DATA_ADDR+i];

	zone=work_para[E2ROM_ZONE_ADDR];
	for(i=0;i<20;i++)
		privacy_data[i/5][i%5]=work_para[E2ROM_PRIVACY_DATA_ADDR+i];
	/////////////	
	LR_hant_pre=LR_hant;
	baudrate_pre=baudrate;
	address_pre=address_pre;
	aux_pre=aux_pre;
	dig_zoom_pre=dig_zoom;
	zoom_display_pre=zoom_display;
	//////////////////////
}


void Zoom_Irq_Function(void)
{
	u8 i,j;
	u16 read_zoom=0;
	USART_RX_COUNT=0;
	Send_Command(ZoomIrq);
	delay_ms(100);
	if(USART_RX_COUNT>=7 &&USART_RX_BUF[USART_RX_COUNT-7]==0x90&&USART_RX_BUF[USART_RX_COUNT-6]==0x50&&USART_RX_BUF[USART_RX_COUNT-1]==0xff)	
	{	
		read_zoom=USART_RX_BUF[USART_RX_COUNT-5]*16*16*16+USART_RX_BUF[USART_RX_COUNT-4]*16*16+USART_RX_BUF[USART_RX_COUNT-3]*16+USART_RX_BUF[USART_RX_COUNT-2];
		//ÅÐ¶Ïµ±Ç°¶Áµ½µÄÖµÊÇ¶à±ä±¶£¬ÀûÓÃ²éÑ¯±ízoom_data_commandµÃµ½Öµ
		
		for(i=0;i<30;i++)
		{
				if(read_zoom>=zoom_irq_num[i] &&read_zoom<zoom_irq_num[i+1])
				{
					j=i+1;
					break;
				}
		}
		if(i==30)
		{
			j=i+2;
		}
		//µ±Ç°jµÄÖµÎªµ±Ç°µÄ±ä±¶
		
		if(j>20) //¹âÑ§±ä½¹ ¼ÆËã½áºÏºóµÄ±ä½¹Öµ
		{
			j=20*(j-20);
		}
		zoom_data=j;
		if(j>99)
			Display_Data_Command(0x0a,0x20,char_Z,char_O,char_O,char_M,0x4a,display_num[j/100],display_num[j/10%10],display_num[j%10],0x1b,0x1b);
		else if(j>9)
			Display_Data_Command(0x0a,0x20,char_Z,char_O,char_O,char_M,0x4a,display_num[j/10%10],display_num[j%10],0x1b,0x1b,0x1b);
		else
			Display_Data_Command(0x0a,0x20,char_Z,char_O,char_O,char_M,0x4a,display_num[j%10],0x1b,0x1b,0x1b,0x1b);
		Send_Command(title_display_on);
		zoomirq_flag=1;
		zoomirq_time=0;
	}
}
void factory_reset(void)
{
	//ÈÃ´æ´¢Æ÷µÈÓÚ³õÊ¼Öµ,È»ºó¿ªÊ¼Ð´EEPROM
	u32 i;
	u8 factory_def_scene_data[4][2]={def_scene_data};
	u8 factory_def_privacy_data[4][5]={def_privacy_data};
	u8 factory_def1_privacy_data[4][5]={def1_privacy_data};
	u8 factory_def2_privacy_data[4][5]={def2_privacy_data};
	u8 factory_def3_privacy_data[4][5]={def3_privacy_data};

	work_para[E2ROM_ZOOM_SPEED_ADDR]=def_zoom_speed;
	work_para[E2ROM_ZOOM_DATA_ADDR]=def_zoom_data;
	work_para[E2ROM_DIG_ZOOM_ADDR]=def_dig_zoom;
	work_para[E2ROM_OSD_ADDR]=def_osd;
	work_para[E2ROM_ZOOM_DISPLAY_ADDR]=def_zoom_display;
	work_para[E2ROM_FOCUS_LIGHT_ADDR]=def_focus_mode;
	work_para[E2ROM_AF_SENS_ADDR]=def_af_sens;
	work_para[E2ROM_FOCUS_LIGHT_ADDR]=def_focus_light;
	work_para[E2ROM_NEAR_LIMIT_ADDR]=def_near_limit;

	work_para[E2ROM_APERTURE_ADDR]=def_aperture;
	work_para[E2ROM_COLOR_GAIN_ADDR]=def_color_gain;
	work_para[E2ROM_COLOR_HUM_ADDR]=def_color_hum;
	work_para[E2ROM_CHROMA_SUPPRESS_ADDR]=def_chroma_suppress;
	work_para[E2ROM_GAMMA_ADDR]=def_gamma;
	work_para[E2ROM_MIRROR_ADDR]=def_mirror;
	work_para[E2ROM_FLIP_ADDR]=def_flip;
	work_para[E2ROM_FREEZE_ADDR]=def_freeze;
	work_para[E2ROM_PIC_EFFECT_ADDR]=def_pic_effect;
	work_para[E2ROM_WB_MODE_ADDR]=def_wb_mode;
	work_para[E2ROM_RED_GAIN_ADDR]=def_red_gain;
	work_para[E2ROM_BLUE_GAIN_ADDR]=def_blue_gain;
	work_para[E2ROM_AE_MODE_ADDR]=def_ae_mode;
	work_para[E2ROM_SHUTTER_ADDR]=def_shutter;
	work_para[E2ROM_IRIS_ADDR]=def_iris;
	work_para[E2ROM_GAIN_ADDR]=def_gain;
	work_para[E2ROM_BRI_ADDR]=def_bri;
	work_para[E2ROM_SLOW_AE_ADDR]=def_slow_ae;
	work_para[E2ROM_SLOW_SHUTTER_ADDR]=def_slow_shutter;

	work_para[E2ROM_BLC_ADDR]=def_blc;
	work_para[E2ROM_WDR_ADDR]=def_wdr;
	work_para[E2ROM_NR_LEVEL_ADDR]=def_nr_level;
	work_para[E2ROM_STABILIZER_ADDR]=def_stabilizer;
	work_para[E2ROM_DEFOG_MODE_ADDR]=def_defog_mode;

	work_para[E2ROM_HI_RESOLUTION_ADDR]=def_hi_resolution;
	work_para[E2ROM_HI_SENSITIVITY_ADDR]=def_hi_sensitivity;

	cameraID=1;

	work_para[E2ROM_FORMAT_ADDR]=def_format;
	work_para[E2ROM_LR_HANT_ADDR]=def_LR_hant;
	work_para[E2ROM_BAUDRATE_ADDR]=def_baudrate;
	work_para[E2ROM_ADDRESS_ADDR]=def_address;
	work_para[E2ROM_AUX_ADDR]=def_aux;

	work_para[E2ROM_SCENE_NUM_ADDR]=def_scene_num;
	for(i=2;i<8;i++)
		work_para[E2ROM_SCENE_DATA_ADDR+i]=factory_def_scene_data[i/2][i%2];

	work_para[E2ROM_ZONE_ADDR]=zone;
	for(i=0;i<20;i++)
		work_para[E2ROM_PRIVACY_DATA_ADDR+i]=factory_def_privacy_data[i/5][i%5];
		
		
		
	work_para[E2ROM_ZOOM_SPEED_SCENE1_ADDR]=def1_zoom_speed;
	work_para[E2ROM_ZOOM_DATA_SCENE1_ADDR]=def1_zoom_data;
	work_para[E2ROM_FOCUS_MODE_SCENE1_ADDR]=def1_focus_mode;
	work_para[E2ROM_AF_SENS_SCENE1_ADDR]=def1_af_sens;
	work_para[E2ROM_FOCUS_LIGHT_SCENE1_ADDR]=def1_focus_light;
	work_para[E2ROM_NEAR_LIMIT_SCENE1_ADDR]=def1_near_limit;
	
	work_para[E2ROM_APERTURE_SCENE1_ADDR]=def1_aperture;
	work_para[E2ROM_COLOR_GAIN_SCENE1_ADDR]=def1_color_gain;
	work_para[E2ROM_COLOR_HUM_SCENE1_ADDR]=def1_color_hum;
	work_para[E2ROM_CHROMA_SUPPRESS_SCENE1_ADDR]=def1_chroma_suppress;
	work_para[E2ROM_GAMMA_SCENE1_ADDR]=def1_gamma;
	work_para[E2ROM_MIRROR_SCENE1_ADDR]=def1_mirror;
	work_para[E2ROM_FLIP_SCENE1_ADDR]=def1_flip;
	work_para[E2ROM_FREEZE_SCENE1_ADDR]=def1_freeze;
	work_para[E2ROM_PIC_EFFECT_SCENE1_ADDR]=def1_pic_effect;
	work_para[E2ROM_WB_MODE_SCENE1_ADDR]=def1_wb_mode;
	work_para[E2ROM_RED_GAIN_SCENE1_ADDR]=def1_red_gain;
	work_para[E2ROM_BLUE_GAIN_SCENE1_ADDR]=def1_blue_gain;
	work_para[E2ROM_AE_MODE_SCENE1_ADDR]=def1_ae_mode;

	work_para[E2ROM_SHUTTER_SCENE1_ADDR]=def1_shutter;
	work_para[E2ROM_IRIS_SCENE1_ADDR]=def1_iris;
	work_para[E2ROM_GAIN_SCENE1_ADDR]=def1_gain;
	work_para[E2ROM_BRI_SCENE1_ADDR]=def1_bri;
	work_para[E2ROM_SLOW_AE_SCENE1_ADDR]=def1_slow_ae;
	work_para[E2ROM_SLOW_SHUTTER_SCENE1_ADDR]=def1_slow_shutter;
	
	work_para[E2ROM_BLC_SCENE1_ADDR]=def1_blc;
	work_para[E2ROM_WDR_SCENE1_ADDR]=def1_wdr;
	work_para[E2ROM_NR_LEVEL_SCENE1_ADDR]=def1_nr_level;
	work_para[E2ROM_STABILIZER_SCENE1_ADDR]=def1_stabilizer;
	work_para[E2ROM_DEFOG_MODE_SCENE1_ADDR]=def1_defog_mode;

	work_para[E2ROM_HI_RESOLUTION_SCENE1_ADDR]=def1_hi_resolution;
	work_para[E2ROM_HI_SENSITIVITY_SCENE1_ADDR]=def1_hi_sensitivity;
	
	work_para[E2ROM_FORMAT_SCENE1_ADDR]=def1_format;
	for(i=0;i<20;i++)
		work_para[E2ROM_PRIVACY_DATA_SCENE1_ADDR+i]=factory_def1_privacy_data[i/5][i%5];

	

	work_para[E2ROM_ZOOM_SPEED_SCENE2_ADDR]=def2_zoom_speed;
	work_para[E2ROM_ZOOM_DATA_SCENE2_ADDR]=def2_zoom_data;
	work_para[E2ROM_FOCUS_MODE_SCENE2_ADDR]=def2_focus_mode;
	work_para[E2ROM_AF_SENS_SCENE2_ADDR]=def2_af_sens;
	work_para[E2ROM_FOCUS_LIGHT_SCENE2_ADDR]=def2_focus_light;
	work_para[E2ROM_NEAR_LIMIT_SCENE2_ADDR]=def2_near_limit;
	
	work_para[E2ROM_APERTURE_SCENE2_ADDR]=def2_aperture;
	work_para[E2ROM_COLOR_GAIN_SCENE2_ADDR]=def2_color_gain;
	work_para[E2ROM_COLOR_HUM_SCENE2_ADDR]=def2_color_hum;
	work_para[E2ROM_CHROMA_SUPPRESS_SCENE2_ADDR]=def2_chroma_suppress;
	work_para[E2ROM_GAMMA_SCENE2_ADDR]=def2_gamma;
	work_para[E2ROM_MIRROR_SCENE2_ADDR]=def2_mirror;
	work_para[E2ROM_FLIP_SCENE2_ADDR]=def2_flip;
	work_para[E2ROM_FREEZE_SCENE2_ADDR]=def2_freeze;
	work_para[E2ROM_PIC_EFFECT_SCENE2_ADDR]=def2_pic_effect;
	work_para[E2ROM_WB_MODE_SCENE2_ADDR]=def2_wb_mode;
	work_para[E2ROM_RED_GAIN_SCENE2_ADDR]=def2_red_gain;
	work_para[E2ROM_BLUE_GAIN_SCENE2_ADDR]=def2_blue_gain;
	work_para[E2ROM_AE_MODE_SCENE2_ADDR]=def2_ae_mode;

	work_para[E2ROM_SHUTTER_SCENE2_ADDR]=def2_shutter;
	work_para[E2ROM_IRIS_SCENE2_ADDR]=def2_iris;
	work_para[E2ROM_GAIN_SCENE2_ADDR]=def2_gain;
	work_para[E2ROM_BRI_SCENE2_ADDR]=def2_bri;
	work_para[E2ROM_SLOW_AE_SCENE2_ADDR]=def2_slow_ae;
	work_para[E2ROM_SLOW_SHUTTER_SCENE2_ADDR]=def2_slow_shutter;
	
	work_para[E2ROM_BLC_SCENE2_ADDR]=def2_blc;
	work_para[E2ROM_WDR_SCENE2_ADDR]=def2_wdr;
	work_para[E2ROM_NR_LEVEL_SCENE2_ADDR]=def2_nr_level;
	work_para[E2ROM_STABILIZER_SCENE2_ADDR]=def2_stabilizer;
	work_para[E2ROM_DEFOG_MODE_SCENE2_ADDR]=def2_defog_mode;

	work_para[E2ROM_HI_RESOLUTION_SCENE2_ADDR]=def2_hi_resolution;
	work_para[E2ROM_HI_SENSITIVITY_SCENE2_ADDR]=def2_hi_sensitivity;
	
	work_para[E2ROM_FORMAT_SCENE2_ADDR]=def2_format;
	for(i=0;i<20;i++)
		work_para[E2ROM_PRIVACY_DATA_SCENE2_ADDR+i]=factory_def2_privacy_data[i/5][i%5];


	work_para[E2ROM_ZOOM_SPEED_SCENE3_ADDR]=def3_zoom_speed;
	work_para[E2ROM_ZOOM_DATA_SCENE3_ADDR]=def3_zoom_data;
	work_para[E2ROM_FOCUS_MODE_SCENE3_ADDR]=def3_focus_mode;
	work_para[E2ROM_AF_SENS_SCENE3_ADDR]=def3_af_sens;
	work_para[E2ROM_FOCUS_LIGHT_SCENE3_ADDR]=def3_focus_light;
	work_para[E2ROM_NEAR_LIMIT_SCENE3_ADDR]=def3_near_limit;
	
	work_para[E2ROM_APERTURE_SCENE3_ADDR]=def3_aperture;
	work_para[E2ROM_COLOR_GAIN_SCENE3_ADDR]=def3_color_gain;
	work_para[E2ROM_COLOR_HUM_SCENE3_ADDR]=def3_color_hum;
	work_para[E2ROM_CHROMA_SUPPRESS_SCENE3_ADDR]=def3_chroma_suppress;
	work_para[E2ROM_GAMMA_SCENE3_ADDR]=def3_gamma;
	work_para[E2ROM_MIRROR_SCENE3_ADDR]=def3_mirror;
	work_para[E2ROM_FLIP_SCENE3_ADDR]=def3_flip;
	work_para[E2ROM_FREEZE_SCENE3_ADDR]=def3_freeze;
	work_para[E2ROM_PIC_EFFECT_SCENE3_ADDR]=def3_pic_effect;
	work_para[E2ROM_WB_MODE_SCENE3_ADDR]=def3_wb_mode;
	work_para[E2ROM_RED_GAIN_SCENE3_ADDR]=def3_red_gain;
	work_para[E2ROM_BLUE_GAIN_SCENE3_ADDR]=def3_blue_gain;
	work_para[E2ROM_AE_MODE_SCENE3_ADDR]=def3_ae_mode;

	work_para[E2ROM_SHUTTER_SCENE3_ADDR]=def3_shutter;
	work_para[E2ROM_IRIS_SCENE3_ADDR]=def3_iris;
	work_para[E2ROM_GAIN_SCENE3_ADDR]=def3_gain;
	work_para[E2ROM_BRI_SCENE3_ADDR]=def3_bri;
	work_para[E2ROM_SLOW_AE_SCENE3_ADDR]=def3_slow_ae;
	work_para[E2ROM_SLOW_SHUTTER_SCENE3_ADDR]=def3_slow_shutter;
	
	work_para[E2ROM_BLC_SCENE3_ADDR]=def3_blc;
	work_para[E2ROM_WDR_SCENE3_ADDR]=def3_wdr;
	work_para[E2ROM_NR_LEVEL_SCENE3_ADDR]=def3_nr_level;
	work_para[E2ROM_STABILIZER_SCENE3_ADDR]=def3_stabilizer;
	work_para[E2ROM_DEFOG_MODE_SCENE3_ADDR]=def3_defog_mode;

	work_para[E2ROM_HI_RESOLUTION_SCENE3_ADDR]=def3_hi_resolution;
	work_para[E2ROM_HI_SENSITIVITY_SCENE3_ADDR]=def3_hi_sensitivity;
	
	work_para[E2ROM_FORMAT_SCENE3_ADDR]=def3_format;
	for(i=0;i<20;i++)
		work_para[E2ROM_PRIVACY_DATA_SCENE3_ADDR+i]=factory_def3_privacy_data[i/5][i%5];
	
	for(i=10;i<E2ROM_LAST_ADDR;i++)
		TWI_WriteByte(work_para[i],i);
		
		
		//¶ÔÏà»ú½øÐÐÉèÖÃ//
	delay_ms(2000);
	delay_ms(2000);
	register_set[4]=0x72;        //Êä³öµÄ¼Ä´æÆ÷Îª72
	register_set[5]=format_to_command[def_format]/16;
	register_set[6]=format_to_command[def_format]%16;
	Send_Command(register_set);  
	delay_ms(1000);
	register_set[4]=0x72;        //Êä³öµÄ¼Ä´æÆ÷Îª72
	register_set[5]=format_to_command[def_format]/16;
	register_set[6]=format_to_command[def_format]%16;
	Send_Command(register_set);  
}
void Set_Command(u8 command_num,u8 num0 ,u8 num1,u8 num2,u8 num3,u8 num4,
				   u8 num5,u8 num6, u8 num7,u8 num8,u8 num9)
{
	  TEXT_Buffer[0]=num0;
	  TEXT_Buffer[1]=num1;
	  TEXT_Buffer[2]=num2;
	  TEXT_Buffer[3]=num3;
	  TEXT_Buffer[4]=num4;
	  TEXT_Buffer[5]=num5;
	  TEXT_Buffer[6]=num6;
	  TEXT_Buffer[7]=num7;
	  TEXT_Buffer[8]=num8;
	  TEXT_Buffer[9]=num9;
	  USART2_Puts((char *)TEXT_Buffer,command_num);
	  delay_ms(20);
}




void save_scene_data(u8 save_num)
{
	u8 i;
	switch(save_num)
	{
		case 0:
				work_para[E2ROM_ZOOM_SPEED_ADDR]=zoom_speed;
				work_para[E2ROM_ZOOM_DATA_ADDR]=zoom_data;
				work_para[E2ROM_DIG_ZOOM_ADDR]=dig_zoom;
				work_para[E2ROM_OSD_ADDR]=osd;
				work_para[E2ROM_ZOOM_DISPLAY_ADDR]=zoom_display;
				work_para[E2ROM_FOCUS_LIGHT_ADDR]=focus_mode;
				work_para[E2ROM_AF_SENS_ADDR]=af_sens;
				work_para[E2ROM_FOCUS_LIGHT_ADDR]=focus_light;
				work_para[E2ROM_NEAR_LIMIT_ADDR]=near_limit;

				work_para[E2ROM_APERTURE_ADDR]=aperture;
				work_para[E2ROM_COLOR_GAIN_ADDR]=color_gain;
				work_para[E2ROM_COLOR_HUM_ADDR]=color_hum;
				work_para[E2ROM_CHROMA_SUPPRESS_ADDR]=chroma_suppress;
				work_para[E2ROM_GAMMA_ADDR]=gamma;
				work_para[E2ROM_MIRROR_ADDR]=mirror;
				work_para[E2ROM_FLIP_ADDR]=flip;
				work_para[E2ROM_FREEZE_ADDR]=freeze;
				work_para[E2ROM_PIC_EFFECT_ADDR]=pic_effect;
				work_para[E2ROM_WB_MODE_ADDR]=wb_mode;
				work_para[E2ROM_RED_GAIN_ADDR]=red_gain;
				work_para[E2ROM_BLUE_GAIN_ADDR]=blue_gain;
				work_para[E2ROM_AE_MODE_ADDR]=ae_mode;
				work_para[E2ROM_SHUTTER_ADDR]=shutter;
				work_para[E2ROM_IRIS_ADDR]=iris;
				work_para[E2ROM_GAIN_ADDR]=gain;
				work_para[E2ROM_BRI_ADDR]=bri;
				work_para[E2ROM_SLOW_AE_ADDR]=slow_ae;
				work_para[E2ROM_SLOW_SHUTTER_ADDR]=slow_shutter;

				work_para[E2ROM_BLC_ADDR]=blc;
				work_para[E2ROM_WDR_ADDR]=wdr;
				work_para[E2ROM_NR_LEVEL_ADDR]=nr_level;
				work_para[E2ROM_STABILIZER_ADDR]=stabilizer;
				work_para[E2ROM_DEFOG_MODE_ADDR]=defog_mode;

				work_para[E2ROM_HI_RESOLUTION_ADDR]=hi_resolution;
				work_para[E2ROM_HI_SENSITIVITY_ADDR]=hi_sensitivity;
				work_para[E2ROM_FORMAT_ADDR]=format;
				work_para[E2ROM_LR_HANT_ADDR]=LR_hant;
				work_para[E2ROM_BAUDRATE_ADDR]=baudrate;
				work_para[E2ROM_ADDRESS_ADDR]=address;
				work_para[E2ROM_AUX_ADDR]=aux;

				work_para[E2ROM_SCENE_NUM_ADDR]=scene_num;
				for(i=2;i<8;i++)
					work_para[E2ROM_SCENE_DATA_ADDR+i]=scene_data[i/2][i%2];

				work_para[E2ROM_ZONE_ADDR]=zone;
				for(i=0;i<20;i++)
					work_para[E2ROM_PRIVACY_DATA_ADDR+i]=privacy_data[i/5][i%5];
				
				for(i=10;i<E2ROM_DEF_LAST_ADDR;i++)
						TWI_WriteByte(work_para[i],i);
			break;
		case 1:
			work_para[E2ROM_ZOOM_SPEED_SCENE1_ADDR]=zoom_speed;
			work_para[E2ROM_ZOOM_DATA_SCENE1_ADDR]=zoom_data;
			work_para[E2ROM_FOCUS_MODE_SCENE1_ADDR]=focus_mode;
			work_para[E2ROM_AF_SENS_SCENE1_ADDR]=af_sens;
			work_para[E2ROM_FOCUS_LIGHT_SCENE1_ADDR]=focus_light;
			work_para[E2ROM_NEAR_LIMIT_SCENE1_ADDR]=near_limit;
			
			work_para[E2ROM_APERTURE_SCENE1_ADDR]=aperture;
			work_para[E2ROM_COLOR_GAIN_SCENE1_ADDR]=color_gain;
			work_para[E2ROM_COLOR_HUM_SCENE1_ADDR]=color_hum;
			work_para[E2ROM_CHROMA_SUPPRESS_SCENE1_ADDR]=chroma_suppress;
			work_para[E2ROM_GAMMA_SCENE1_ADDR]=gamma;
			work_para[E2ROM_MIRROR_SCENE1_ADDR]=mirror;
			work_para[E2ROM_FLIP_SCENE1_ADDR]=flip;
			work_para[E2ROM_FREEZE_SCENE1_ADDR]=freeze;
			work_para[E2ROM_PIC_EFFECT_SCENE1_ADDR]=pic_effect;
			work_para[E2ROM_WB_MODE_SCENE1_ADDR]=wb_mode;
			work_para[E2ROM_RED_GAIN_SCENE1_ADDR]=red_gain;
			work_para[E2ROM_BLUE_GAIN_SCENE1_ADDR]=blue_gain;
			work_para[E2ROM_AE_MODE_SCENE1_ADDR]=ae_mode;

			work_para[E2ROM_SHUTTER_SCENE1_ADDR]=shutter;
			work_para[E2ROM_IRIS_SCENE1_ADDR]=iris;
			work_para[E2ROM_GAIN_SCENE1_ADDR]=gain;
			work_para[E2ROM_BRI_SCENE1_ADDR]=bri;
			work_para[E2ROM_SLOW_AE_SCENE1_ADDR]=slow_ae;
			work_para[E2ROM_SLOW_SHUTTER_SCENE1_ADDR]=slow_shutter;
			
			work_para[E2ROM_BLC_SCENE1_ADDR]=blc;
			work_para[E2ROM_WDR_SCENE1_ADDR]=wdr;
			work_para[E2ROM_NR_LEVEL_SCENE1_ADDR]=nr_level;
			work_para[E2ROM_STABILIZER_SCENE1_ADDR]=stabilizer;
			work_para[E2ROM_DEFOG_MODE_SCENE1_ADDR]=defog_mode;

			work_para[E2ROM_HI_RESOLUTION_SCENE1_ADDR]=hi_resolution;
			work_para[E2ROM_HI_SENSITIVITY_SCENE1_ADDR]=hi_sensitivity;
			
			work_para[E2ROM_FORMAT_SCENE1_ADDR]=format;
			for(i=0;i<20;i++)
				work_para[E2ROM_PRIVACY_DATA_SCENE1_ADDR+i]=privacy_data[i/5][i%5];
			for(i=E2ROM_ZOOM_SPEED_SCENE1_ADDR;i<E2ROM_SCENE1_LAST_ADDR;i++)
						TWI_WriteByte(work_para[i],i);	
			for(i=2;i<4;i++)
			{		
				work_para[E2ROM_SCENE_DATA_ADDR+i]=scene_data[i/2][i%2];	
				TWI_WriteByte(work_para[E2ROM_SCENE_DATA_ADDR+i],E2ROM_SCENE_DATA_ADDR+i);
			}
			break;
		case 2:
			work_para[E2ROM_ZOOM_SPEED_SCENE2_ADDR]=zoom_speed;
			work_para[E2ROM_ZOOM_DATA_SCENE2_ADDR]=zoom_data;
			work_para[E2ROM_FOCUS_MODE_SCENE2_ADDR]=focus_mode;
			work_para[E2ROM_AF_SENS_SCENE2_ADDR]=af_sens;
			work_para[E2ROM_FOCUS_LIGHT_SCENE2_ADDR]=focus_light;
			work_para[E2ROM_NEAR_LIMIT_SCENE2_ADDR]=near_limit;
			
			work_para[E2ROM_APERTURE_SCENE2_ADDR]=aperture;
			work_para[E2ROM_COLOR_GAIN_SCENE2_ADDR]=color_gain;
			work_para[E2ROM_COLOR_HUM_SCENE2_ADDR]=color_hum;
			work_para[E2ROM_CHROMA_SUPPRESS_SCENE2_ADDR]=chroma_suppress;
			work_para[E2ROM_GAMMA_SCENE2_ADDR]=gamma;
			work_para[E2ROM_MIRROR_SCENE2_ADDR]=mirror;
			work_para[E2ROM_FLIP_SCENE2_ADDR]=flip;
			work_para[E2ROM_FREEZE_SCENE2_ADDR]=freeze;
			work_para[E2ROM_PIC_EFFECT_SCENE2_ADDR]=pic_effect;
			work_para[E2ROM_WB_MODE_SCENE2_ADDR]=wb_mode;
			work_para[E2ROM_RED_GAIN_SCENE2_ADDR]=red_gain;
			work_para[E2ROM_BLUE_GAIN_SCENE2_ADDR]=blue_gain;
			work_para[E2ROM_AE_MODE_SCENE2_ADDR]=ae_mode;

			work_para[E2ROM_SHUTTER_SCENE2_ADDR]=shutter;
			work_para[E2ROM_IRIS_SCENE2_ADDR]=iris;
			work_para[E2ROM_GAIN_SCENE2_ADDR]=gain;
			work_para[E2ROM_BRI_SCENE2_ADDR]=bri;
			work_para[E2ROM_SLOW_AE_SCENE2_ADDR]=slow_ae;
			work_para[E2ROM_SLOW_SHUTTER_SCENE2_ADDR]=slow_shutter;
			
			work_para[E2ROM_BLC_SCENE2_ADDR]=blc;
			work_para[E2ROM_WDR_SCENE2_ADDR]=wdr;
			work_para[E2ROM_NR_LEVEL_SCENE2_ADDR]=nr_level;
			work_para[E2ROM_STABILIZER_SCENE2_ADDR]=stabilizer;
			work_para[E2ROM_DEFOG_MODE_SCENE2_ADDR]=defog_mode;

			work_para[E2ROM_HI_RESOLUTION_SCENE2_ADDR]=hi_resolution;
			work_para[E2ROM_HI_SENSITIVITY_SCENE2_ADDR]=hi_sensitivity;
			
			work_para[E2ROM_FORMAT_SCENE2_ADDR]=format;
			for(i=0;i<20;i++)
				work_para[E2ROM_PRIVACY_DATA_SCENE2_ADDR+i]=privacy_data[i/5][i%5];
			for(i=E2ROM_ZOOM_SPEED_SCENE2_ADDR;i<E2ROM_SCENE2_LAST_ADDR;i++)
						TWI_WriteByte(work_para[i],i);	
			for(i=4;i<6;i++)
			{		
				work_para[E2ROM_SCENE_DATA_ADDR+i]=scene_data[i/2][i%2];	
				TWI_WriteByte(work_para[E2ROM_SCENE_DATA_ADDR+i],E2ROM_SCENE_DATA_ADDR+i);
			}
			break;
		case 3:
			work_para[E2ROM_ZOOM_SPEED_SCENE3_ADDR]=zoom_speed;
			work_para[E2ROM_ZOOM_DATA_SCENE3_ADDR]=zoom_data;
			work_para[E2ROM_FOCUS_MODE_SCENE3_ADDR]=focus_mode;
			work_para[E2ROM_AF_SENS_SCENE3_ADDR]=af_sens;
			work_para[E2ROM_FOCUS_LIGHT_SCENE3_ADDR]=focus_light;
			work_para[E2ROM_NEAR_LIMIT_SCENE3_ADDR]=near_limit;
			
			work_para[E2ROM_APERTURE_SCENE3_ADDR]=aperture;
			work_para[E2ROM_COLOR_GAIN_SCENE3_ADDR]=color_gain;
			work_para[E2ROM_COLOR_HUM_SCENE3_ADDR]=color_hum;
			work_para[E2ROM_CHROMA_SUPPRESS_SCENE3_ADDR]=chroma_suppress;
			work_para[E2ROM_GAMMA_SCENE3_ADDR]=gamma;
			work_para[E2ROM_MIRROR_SCENE3_ADDR]=mirror;
			work_para[E2ROM_FLIP_SCENE3_ADDR]=flip;
			work_para[E2ROM_FREEZE_SCENE3_ADDR]=freeze;
			work_para[E2ROM_PIC_EFFECT_SCENE3_ADDR]=pic_effect;
			work_para[E2ROM_WB_MODE_SCENE3_ADDR]=wb_mode;
			work_para[E2ROM_RED_GAIN_SCENE3_ADDR]=red_gain;
			work_para[E2ROM_BLUE_GAIN_SCENE3_ADDR]=blue_gain;
			work_para[E2ROM_AE_MODE_SCENE3_ADDR]=ae_mode;

			work_para[E2ROM_SHUTTER_SCENE3_ADDR]=shutter;
			work_para[E2ROM_IRIS_SCENE3_ADDR]=iris;
			work_para[E2ROM_GAIN_SCENE3_ADDR]=gain;
			work_para[E2ROM_BRI_SCENE3_ADDR]=bri;
			work_para[E2ROM_SLOW_AE_SCENE3_ADDR]=slow_ae;
			work_para[E2ROM_SLOW_SHUTTER_SCENE3_ADDR]=slow_shutter;
			
			work_para[E2ROM_BLC_SCENE3_ADDR]=blc;
			work_para[E2ROM_WDR_SCENE3_ADDR]=wdr;
			work_para[E2ROM_NR_LEVEL_SCENE3_ADDR]=nr_level;
			work_para[E2ROM_STABILIZER_SCENE3_ADDR]=stabilizer;
			work_para[E2ROM_DEFOG_MODE_SCENE3_ADDR]=defog_mode;

			work_para[E2ROM_HI_RESOLUTION_SCENE3_ADDR]=hi_resolution;
			work_para[E2ROM_HI_SENSITIVITY_SCENE3_ADDR]=hi_sensitivity;
			
			work_para[E2ROM_FORMAT_SCENE3_ADDR]=format;
			for(i=0;i<20;i++)
				work_para[E2ROM_PRIVACY_DATA_SCENE3_ADDR+i]=privacy_data[i/5][i%5];
			for(i=E2ROM_ZOOM_SPEED_SCENE3_ADDR;i<E2ROM_SCENE3_LAST_ADDR;i++)
						TWI_WriteByte(work_para[i],i);	
			for(i=6;i<8;i++)
			{		
				work_para[E2ROM_SCENE_DATA_ADDR+i]=scene_data[i/2][i%2];	
				TWI_WriteByte(work_para[E2ROM_SCENE_DATA_ADDR+i],E2ROM_SCENE_DATA_ADDR+i);
			}
			break;
		default:
			break;
	}
}
void scene_data_send(u8 run_num)
{
	u8 i;
	switch (run_num)
	{
		case 0:
				zoom_speed=work_para[E2ROM_ZOOM_SPEED_ADDR];
				zoom_data=work_para[E2ROM_ZOOM_DATA_ADDR];
				focus_mode=work_para[E2ROM_FOCUS_LIGHT_ADDR];
				af_sens=work_para[E2ROM_AF_SENS_ADDR];
				focus_light=work_para[E2ROM_FOCUS_LIGHT_ADDR];
				near_limit=work_para[E2ROM_NEAR_LIMIT_ADDR];

				aperture =work_para[E2ROM_APERTURE_ADDR];
				color_gain=work_para[E2ROM_COLOR_GAIN_ADDR];
				color_hum=work_para[E2ROM_COLOR_HUM_ADDR];
				chroma_suppress=work_para[E2ROM_CHROMA_SUPPRESS_ADDR];
				gamma=work_para[E2ROM_GAMMA_ADDR];
				mirror=work_para[E2ROM_MIRROR_ADDR];
				flip=work_para[E2ROM_FLIP_ADDR];
				freeze=work_para[E2ROM_FREEZE_ADDR];
				pic_effect=work_para[E2ROM_PIC_EFFECT_ADDR];
				wb_mode=work_para[E2ROM_WB_MODE_ADDR];
				red_gain=work_para[E2ROM_RED_GAIN_ADDR];
				blue_gain=work_para[E2ROM_BLUE_GAIN_ADDR];
				ae_mode=work_para[E2ROM_AE_MODE_ADDR];
				shutter=work_para[E2ROM_SHUTTER_ADDR];
				iris=work_para[E2ROM_IRIS_ADDR];
				gain=work_para[E2ROM_GAIN_ADDR];
				bri=work_para[E2ROM_BRI_ADDR];
				slow_ae=work_para[E2ROM_SLOW_AE_ADDR];
				slow_shutter=work_para[E2ROM_SLOW_SHUTTER_ADDR];
				
				blc=work_para[E2ROM_BLC_ADDR];
				wdr=work_para[E2ROM_WDR_ADDR];
				nr_level=work_para[E2ROM_NR_LEVEL_ADDR];
				stabilizer=work_para[E2ROM_STABILIZER_ADDR];
				defog_mode=work_para[E2ROM_DEFOG_MODE_ADDR];

				hi_resolution=work_para[E2ROM_HI_RESOLUTION_ADDR];
				hi_sensitivity=work_para[E2ROM_HI_SENSITIVITY_ADDR];
				format=work_para[E2ROM_FORMAT_ADDR];
				zone=work_para[E2ROM_ZONE_ADDR];
				for(i=0;i<20;i++)
					privacy_data[i/5][i%5]=work_para[E2ROM_PRIVACY_DATA_ADDR+i];
					
			break;
		case 1:
				zoom_speed=work_para[E2ROM_ZOOM_SPEED_SCENE1_ADDR];
				zoom_data=work_para[E2ROM_ZOOM_DATA_SCENE1_ADDR];
				focus_mode=work_para[E2ROM_FOCUS_LIGHT_SCENE1_ADDR];
				af_sens=work_para[E2ROM_AF_SENS_SCENE1_ADDR];
				focus_light=work_para[E2ROM_FOCUS_LIGHT_SCENE1_ADDR];
				near_limit=work_para[E2ROM_NEAR_LIMIT_SCENE1_ADDR];

				aperture =work_para[E2ROM_APERTURE_SCENE1_ADDR];
				color_gain=work_para[E2ROM_COLOR_GAIN_SCENE1_ADDR];
				color_hum=work_para[E2ROM_COLOR_HUM_SCENE1_ADDR];
				chroma_suppress=work_para[E2ROM_CHROMA_SUPPRESS_SCENE1_ADDR];
				gamma=work_para[E2ROM_GAMMA_SCENE1_ADDR];
				mirror=work_para[E2ROM_MIRROR_SCENE1_ADDR];
				flip=work_para[E2ROM_FLIP_SCENE1_ADDR];
				freeze=work_para[E2ROM_FREEZE_SCENE1_ADDR];
				pic_effect=work_para[E2ROM_PIC_EFFECT_SCENE1_ADDR];
				wb_mode=work_para[E2ROM_WB_MODE_SCENE1_ADDR];
				red_gain=work_para[E2ROM_RED_GAIN_SCENE1_ADDR];
				blue_gain=work_para[E2ROM_BLUE_GAIN_SCENE1_ADDR];
				ae_mode=work_para[E2ROM_AE_MODE_SCENE1_ADDR];
				shutter=work_para[E2ROM_SHUTTER_SCENE1_ADDR];
				iris=work_para[E2ROM_IRIS_SCENE1_ADDR];
				gain=work_para[E2ROM_GAIN_SCENE1_ADDR];
				bri=work_para[E2ROM_BRI_SCENE1_ADDR];
				slow_ae=work_para[E2ROM_SLOW_AE_SCENE1_ADDR];
				slow_shutter=work_para[E2ROM_SLOW_SHUTTER_SCENE1_ADDR];
				
				blc=work_para[E2ROM_BLC_SCENE1_ADDR];
				wdr=work_para[E2ROM_WDR_SCENE1_ADDR];
				nr_level=work_para[E2ROM_NR_LEVEL_SCENE1_ADDR];
				stabilizer=work_para[E2ROM_STABILIZER_SCENE1_ADDR];
				defog_mode=work_para[E2ROM_DEFOG_MODE_SCENE1_ADDR];

				hi_resolution=work_para[E2ROM_HI_RESOLUTION_SCENE1_ADDR];
				hi_sensitivity=work_para[E2ROM_HI_SENSITIVITY_SCENE1_ADDR];
//				format=work_para[E2ROM_FORMAT_SCENE1_ADDR];
				zone=work_para[E2ROM_ZONE_SCENE1_ADDR];
				for(i=0;i<20;i++)
				privacy_data[i/5][i%5]=work_para[E2ROM_PRIVACY_DATA_SCENE1_ADDR+i];
			break;
		case 2:
				zoom_speed=work_para[E2ROM_ZOOM_SPEED_SCENE2_ADDR];
				zoom_data=work_para[E2ROM_ZOOM_DATA_SCENE2_ADDR];
				focus_mode=work_para[E2ROM_FOCUS_LIGHT_SCENE2_ADDR];
				af_sens=work_para[E2ROM_AF_SENS_SCENE2_ADDR];
				focus_light=work_para[E2ROM_FOCUS_LIGHT_SCENE2_ADDR];
				near_limit=work_para[E2ROM_NEAR_LIMIT_SCENE2_ADDR];

				aperture =work_para[E2ROM_APERTURE_SCENE2_ADDR];
				color_gain=work_para[E2ROM_COLOR_GAIN_SCENE2_ADDR];
				color_hum=work_para[E2ROM_COLOR_HUM_SCENE2_ADDR];
				chroma_suppress=work_para[E2ROM_CHROMA_SUPPRESS_SCENE2_ADDR];
				gamma=work_para[E2ROM_GAMMA_SCENE2_ADDR];
				mirror=work_para[E2ROM_MIRROR_SCENE2_ADDR];
				flip=work_para[E2ROM_FLIP_SCENE2_ADDR];
				freeze=work_para[E2ROM_FREEZE_SCENE2_ADDR];
				pic_effect=work_para[E2ROM_PIC_EFFECT_SCENE2_ADDR];
				wb_mode=work_para[E2ROM_WB_MODE_SCENE2_ADDR];
				red_gain=work_para[E2ROM_RED_GAIN_SCENE2_ADDR];
				blue_gain=work_para[E2ROM_BLUE_GAIN_SCENE2_ADDR];
				ae_mode=work_para[E2ROM_AE_MODE_SCENE2_ADDR];
				shutter=work_para[E2ROM_SHUTTER_SCENE2_ADDR];
				iris=work_para[E2ROM_IRIS_SCENE2_ADDR];
				gain=work_para[E2ROM_GAIN_SCENE2_ADDR];
				bri=work_para[E2ROM_BRI_SCENE2_ADDR];
				slow_ae=work_para[E2ROM_SLOW_AE_SCENE2_ADDR];
				slow_shutter=work_para[E2ROM_SLOW_SHUTTER_SCENE2_ADDR];
				
				blc=work_para[E2ROM_BLC_SCENE2_ADDR];
				wdr=work_para[E2ROM_WDR_SCENE2_ADDR];
				nr_level=work_para[E2ROM_NR_LEVEL_SCENE2_ADDR];
				stabilizer=work_para[E2ROM_STABILIZER_SCENE2_ADDR];
				defog_mode=work_para[E2ROM_DEFOG_MODE_SCENE2_ADDR];

				hi_resolution=work_para[E2ROM_HI_RESOLUTION_SCENE2_ADDR];
				hi_sensitivity=work_para[E2ROM_HI_SENSITIVITY_SCENE2_ADDR];
//				format=work_para[E2ROM_FORMAT_SCENE2_ADDR];
				zone=work_para[E2ROM_ZONE_SCENE2_ADDR];
				for(i=0;i<20;i++)
					privacy_data[i/5][i%5]=work_para[E2ROM_PRIVACY_DATA_SCENE2_ADDR+i];
			break;
		case 3:
				zoom_speed=work_para[E2ROM_ZOOM_SPEED_SCENE3_ADDR];
				zoom_data=work_para[E2ROM_ZOOM_DATA_SCENE3_ADDR];
				focus_mode=work_para[E2ROM_FOCUS_LIGHT_SCENE3_ADDR];
				af_sens=work_para[E2ROM_AF_SENS_SCENE3_ADDR];
				focus_light=work_para[E2ROM_FOCUS_LIGHT_SCENE3_ADDR];
				near_limit=work_para[E2ROM_NEAR_LIMIT_SCENE3_ADDR];

				aperture =work_para[E2ROM_APERTURE_SCENE3_ADDR];
				color_gain=work_para[E2ROM_COLOR_GAIN_SCENE3_ADDR];
				color_hum=work_para[E2ROM_COLOR_HUM_SCENE3_ADDR];
				chroma_suppress=work_para[E2ROM_CHROMA_SUPPRESS_SCENE3_ADDR];
				gamma=work_para[E2ROM_GAMMA_SCENE3_ADDR];
				mirror=work_para[E2ROM_MIRROR_SCENE3_ADDR];
				flip=work_para[E2ROM_FLIP_SCENE3_ADDR];
				freeze=work_para[E2ROM_FREEZE_SCENE3_ADDR];
				pic_effect=work_para[E2ROM_PIC_EFFECT_SCENE3_ADDR];
				wb_mode=work_para[E2ROM_WB_MODE_SCENE3_ADDR];
				red_gain=work_para[E2ROM_RED_GAIN_SCENE3_ADDR];
				blue_gain=work_para[E2ROM_BLUE_GAIN_SCENE3_ADDR];
				ae_mode=work_para[E2ROM_AE_MODE_SCENE3_ADDR];
				shutter=work_para[E2ROM_SHUTTER_SCENE3_ADDR];
				iris=work_para[E2ROM_IRIS_SCENE3_ADDR];
				gain=work_para[E2ROM_GAIN_SCENE3_ADDR];
				bri=work_para[E2ROM_BRI_SCENE3_ADDR];
				slow_ae=work_para[E2ROM_SLOW_AE_SCENE3_ADDR];
				slow_shutter=work_para[E2ROM_SLOW_SHUTTER_SCENE3_ADDR];
				
				blc=work_para[E2ROM_BLC_SCENE3_ADDR];
				wdr=work_para[E2ROM_WDR_SCENE3_ADDR];
				nr_level=work_para[E2ROM_NR_LEVEL_SCENE3_ADDR];
				stabilizer=work_para[E2ROM_STABILIZER_SCENE3_ADDR];
				defog_mode=work_para[E2ROM_DEFOG_MODE_SCENE3_ADDR];

				hi_resolution=work_para[E2ROM_HI_RESOLUTION_SCENE3_ADDR];
				hi_sensitivity=work_para[E2ROM_HI_SENSITIVITY_SCENE3_ADDR];
//				format=work_para[E2ROM_FORMAT_SCENE3_ADDR];
				zone=work_para[E2ROM_ZONE_SCENE3_ADDR];
				for(i=0;i<20;i++)
					privacy_data[i/5][i%5]=work_para[E2ROM_PRIVACY_DATA_SCENE3_ADDR+i];
			break;
		default :
			break;
	}
		zoom_seeed_pre=zoom_speed;
		zoom_data_pre=zoom_data;
		focus_mode_pre=focus_mode;
		af_sens_pre=af_sens;
		focus_light_pre=focus_light;
		near_limit_pre =near_limit;
		aperture_pre=aperture;
		color_gain_pre=color_gain;
		color_hum_pre=color_hum;
		chroma_suppress_pre=chroma_suppress;
		gamma_pre=gamma;
		mirror_pre=mirror;
		flip_pre=flip;
		freeze_pre=freeze;
		pic_effect_pre=pic_effect;
		wb_mode_pre=wb_mode;
		red_gain_pre=red_gain;
		blue_gain_pre=blue_gain;
		ae_mode_pre=ae_mode;
		shutter_pre=shutter;
		iris_pre=iris;
		gain_pre=gain;
		bri_pre=bri;
		slow_ae_pre=slow_ae;
		slow_shutter_pre=slow_shutter;
		blc_pre=blc;
		wdr_pre=wdr;
		nr_level_pre=nr_level;
		stabilizer_pre=stabilizer;
		defog_mode_pre=defog_mode;
		hi_resolution_pre=hi_resolution;
		hi_sensitivity_pre=hi_sensitivity;
		format_pre=format;
		zone_pre=zone;
		for(i=0;i<20;i++)
				privacy_data_pre[i/5][i%5]=privacy_data[i/5][i%5];

	work_para[E2ROM_SCENE_NUM_ADDR]=scene_num;
	TWI_WriteByte(work_para[E2ROM_SCENE_NUM_ADDR],E2ROM_SCENE_NUM_ADDR);	  //´æµ±Ç°³¡¾°ºÅµ½EEPROM

	
	switch(ae_mode)
	{
		case 0:  //auto
			Send_Command(ae_mode_auto);			
			break;
		case 1:  //manual
			Send_Command(ae_mode_manual);  //ÊÖ¶¯Ä£Ê½
			shutter_set[6]=shutter/16;
			shutter_set[7]=shutter%16;
			Send_Command(shutter_set);
			iris_set[6]=(iris+4)>15 ? 1:0 ;
			iris_set[7]=iris==0 ? 0:(iris%16)+4;
			Send_Command(iris_set);
			gain_set[7]=gain;
			Send_Command(gain_set);
			break;
		case 2:  //shut pri
			Send_Command(ae_mode_shutpri);  //Ä£Ê½ÇÐ»»½
			shutter_set[6]=shutter/16;
			shutter_set[7]=shutter%16;
			Send_Command(shutter_set); //¿ìÃÅÉèÖÃ
			break;
		case 3 :  //iris prl
			Send_Command(ae_mode_irisprl);//Ä£Ê½ÇÐ»»
			iris_set[6]=(iris+4)>15 ? 1:0 ;
			iris_set[7]=iris==0 ? 0:(iris%16)+4;
			Send_Command(iris_set); //¹âÈ¦ÉèÖÃ
			break;
		case 4:  //bright
			Send_Command(ae_mode_bright);  //Ä£Ê½ÇÐ»»
			
			bright_set[6]=(bri+4)>15 ? 1:0;
			bright_set[7]=bri==0 ? 0:(bri%16)+4;
			Send_Command(bright_set); //ÁÁ¶È
			break;
		default:
			break;
	}
	
	//×Ô¶¯ÆØ¹âÏìÓ¦//   //(01h to 30h)
	slow_ae_set[4]=slow_ae;
	Send_Command(slow_ae_set); 
	
	//×Ô¶¯ÂýËÙ¿ìÃÅ
	slow_shutter_set[4]=slow_shutter;
	Send_Command(slow_shutter_set);
	
	//ÊýÂë±ä½¹¹Ø±Õ
	Send_Command(Digita_Zoom_set);  
	
	//Ïà»úÏÔÊ¾¿ª¹Ø
	Send_Command(camera_display_set);

	//×Ô¶¯¾Û½¹
	focus_mode_set[4]=focus_mode+2;
	Send_Command(focus_mode_set);// ×Ô¶¯¾Û½¹¹Ø±Õ03  ¿ªÆôÎª02
	
	//×Ô¶¯¶Ô½¹ÁéÃô¶È¸ßµÍ
  af_sens_set[4]=af_sens+2;
	Send_Command(af_sens_set);
	
	//ºìÍâÐ£Õý
	focus_light_set[4]=focus_light;
	Send_Command(focus_light_set);
	
	//¾Û½¹½ü¼«ÏÞÎ»ÖÃ
	near_limit_set[4]=near_limit;
	Send_Command(near_limit_set);

	//×Ô¶¯°×Æ½ºâ
	wb_mode_set[4]=wb_mode;
	Send_Command(wb_mode_set);
	if(wb_mode==8 || wb_mode==5)
	{
		red_gain_set[6]=red_gain/16;
		red_gain_set[7]=red_gain%16;
		Send_Command(red_gain_set);
		blue_gain_set[6]=blue_gain/16;
		blue_gain_set[7]=blue_gain%16;
		Send_Command(blue_gain_set);
	}else if(wb_mode==3)
	{
		Send_Command(one_push_set);
	}
	
	//¹â·´À¡²¹³¥¿ª¹Ø
	blc_set[4]=blc+2;
	Send_Command(blc_set);
	
	//ÆØ¹â²¹³¥
	Send_Command(expcomp_set);
	
	//ÁÁ¶È²¹³¥
	wdr_set[4]=wdr;
	Send_Command(wdr_set);
	
		
	
//	Set_Command(0x06,0x81,0x01,0x04,0x51,0x02,0xff,0x00,0x00,0x00,0x00);
//	Set_Command(0x06,0x81,0x01,0x04,0x53,0x03,0xff,0x00,0x00,0x00,0x00);
//	Set_Command(0x06,0x81,0x01,0x04,0x34,0x03,0xff,0x00,0x00,0x00,0x00);
	
	//È¥Îí´¦Àí
	defog_mode_set[4]=defog_mode;
	Send_Command(defog_mode_set);
	
	//¿×¾¶ÔöÒæ
	aperture_set[7]=aperture;
	Send_Command(aperture_set);
						
	//²ÊÉ«ÔöÒæ
	color_gain_set[7]=color_gain;
	Send_Command(color_gain_set);
	
	color_hum_set[7]=color_hum;
	Send_Command(color_hum_set);
	
	//É«¶ÈÒÖÖÆ
	chroma_suppress_set[4]=chroma_suppress;
	Send_Command(chroma_suppress_set);
	
	//Ù¤ÂêÉèÖÃ
	gamma_set[4]=gamma;
	Send_Command(gamma_set);	

	//¾µÏñÉèÖÃ
	mirror_set[4]=mirror+2;
	Send_Command(mirror_set);	
	
	//Í¼Æ¬·­×ª
	flip_set[4]=flip+2;
	Send_Command(flip_set);	
	
  //»­ÃæÐ§¹û
	pic_effect_set[4]=pic_effect*2;
	Send_Command(pic_effect_set);	
	
	
	//Ïà»ú¸ß·Ö±æÂÊ
	hi_resolution_set[4]=hi_resolution+2;
	Send_Command(hi_resolution_set);
	//¸ßÁéÃô¶È
	hi_sensitivity_set[4]=hi_sensitivity+2;
	Send_Command(hi_sensitivity_set);
	
	for(i=0;i<4;i++)
	{
		if(privacy_data[i][0]==0)
			display_privacy_area_on(i);
		else
			display_privacy_area_off(i);
	}
	//·¢ËÍ¹âÑ§±ä½¹Öµ
	zoom_set[4]=zoom_data_command [zoom_data-1][0];
	zoom_set[5]=zoom_data_command [zoom_data-1][1];
	zoom_set[6]=zoom_data_command [zoom_data-1][2];
	zoom_set[7]=zoom_data_command [zoom_data-1][3];
	Send_Command(zoom_set);
}

void turn_on_display(void)
{
	u32 i=0;
	delay_ms(1000);

	while(i<5)
	{
		delay_ms(1000);
		USART_RX_COUNT=0;
		Send_Command(PowerInq);
		i++;
		delay_ms(50);
		//Èç¹ûÊÕµ½0x90 0x50 0x02 0xff ±íÊ¾Ïà»úµçÔ´¿ª
		if(USART_RX_COUNT>0 &&USART_RX_BUF[0]==0x90&&USART_RX_BUF[1]==0x50&&USART_RX_BUF[2]==0x02&&USART_RX_BUF[3]==0xff)	
		{	
			i=6;
		}  
	}
	power_led_on;  //µçÔ´Ö¸Ê¾µÆÁÁ
	
	//ÆØ¹âÉèÖÃ¡¡¸ù¾Ýµ±Ç°µÄ±£´æÖµ½øÐÐÄ£Ê½Ñ¡Ôñ
		
	Set_Command(0x09,0x81,0x01,0x04,0x47,0x00,0x08,0x00,0x04,0xff,0x00);  //µ÷±ä±¶
 	Set_Command(0x09,0x81,0x01,0x04,0x48,0x07,0x00,0x00,0x00,0xff,0x00);
	
	//·¢ËÍ³¡¾°²ÎÊýÐÅÏ¢
	scene_data_send(scene_num);
	
	delay_ms(1000);
	Send_Command(title_clear);

	
	//ÏÔÊ¾°æ±¾ËµÃ÷  BR1508 V1.0
	Display_Data_Command(2,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_B);
	Display_Data_Command(2,0x30,char_R,num_1,num_5,num_0,num_8,0x1b,char_V,num_1,0x4c,num_0);
	//ÏÔÊ¾addressµØÖ·
	Display_Data_Command(3,0x20,0x1b,0x00,0x03,0x03,0x11,0x04,0x12,0x12,0x1b,0x1b);
	Display_Data_Command(3,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																			address>100? display_num[address/100] :0x1b,
																			address>10? display_num[address/10%10]:0x1b,
																			display_num[address%10]); //off
	//ÏÔÊ¾ÉÏÎ»»úÍ¨ÐÅ²¨ÌØÂÊ
	Display_Data_Command(4,0x20,0x1b,0x01,0x00,0x14,0x03,0x11,0x00,0x13,0x04,0x1b);
	Display_Data_Command(4,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(24*(1<<baudrate))>100 ? display_num[(24*(1<<baudrate)/100)] :0x1b,//Èç¹û°ÙÎ»²»µÈÓÚ£°£¬ÏÔÊ¾ÊýÖµ
																		display_num[(24*(1<<baudrate)/10%10)],
																		display_num[(24*(1<<baudrate)%10)],
																		0x27,0x27);  //ÏÔÊ¾²¨ÌØÂÊ 
	//ÏÔÊ¾PROTOCOL PELCO-P/D
	Display_Data_Command(5,0x20,0x1b,0x0f,0x11,0x0e,0x13,0x0e,0x02,0x0e,0x0b,0x1b);
	Display_Data_Command(5,0x30,0x1b,0x0f,0x04,0x0b,0x02,0x0e,0x4f,0x0f,0x4e,0x03);
	
	//ÏÔÊ¾VIDEO MODE 
	
	Display_Data_Command(6,0x20,0x1b,0x05,0x0e,0x11,0x0c,0x00,0x13,0x1b,0x1b,0x1b);
	Display_Data_Command(6,0x30,0x1b,0x1b,0x1b,
																	display_format[format][0],
																	display_format[format][1],
																	display_format[format][2],
																	display_format[format][3],
																	display_format[format][4],
																	display_format[format][5],
																	display_format[format][6]); 
	//ÏÔÊ¾INITED OK
	Display_Data_Command(8,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,char_I,char_N,char_I,char_T,char_E);
  Display_Data_Command(8,0x30,char_D,0x1b,char_O,char_K,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);

	Send_Command(title_display_on);
	delay_ms(5000);

	Send_Command(title_clear);
	Zoom_Irq_Function();

	//ÊýÂë±ä½¹¹Ø±Õ
	Send_Command(Digita_Zoom_set);  
}

/*
´Ëº¯ÊýÊµÏÖÁËÔÚxÐÐµÄÒ»ÁÐ»òÕß¶þÁÐ½øÐÐÊ®¸ö×Ö·ûµÄÏÔÊ¾	 yÈ¡Öµ 0x20 ,0x30
*/
void Display_Data_Command(u8 x,u8 y,u8 num0 ,u8 num1,
						  u8 num2,u8 num3,u8 num4,
						  u8 num5,u8 num6, u8 num7,
						  u8 num8,u8 num9)
{
	  TEXT_Buffer[0]=0x81;
	  TEXT_Buffer[1]=0x01;
	  TEXT_Buffer[2]=0x04;
	  TEXT_Buffer[3]=0x73;
	  TEXT_Buffer[4]=y+x;
	  TEXT_Buffer[5]=num0;
	  TEXT_Buffer[6]=num1;
	  TEXT_Buffer[7]=num2;
	  TEXT_Buffer[8]=num3;
	  TEXT_Buffer[9]=num4;
	  TEXT_Buffer[10]=num5;
	  TEXT_Buffer[11]=num6;
	  TEXT_Buffer[12]=num7;
	  TEXT_Buffer[13]=num8;
	  TEXT_Buffer[14]=num9;
	  TEXT_Buffer[15]=0xff;
	  USART2_Puts((char *)TEXT_Buffer,16);
	  delay_ms(20);
}


u8 save_conf_page()
{
	u8 i=1;
	Send_Command(title_clear);
	Display_Data_Command(4,0x20,0x1b,0x12,0x00,0x15,0x04,0x1b,0x02,0x0e,0x0d,0x05);
	Display_Data_Command(4,0x30,0x1b,0x1c,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
	Display_Data_Command(5,0x20,0x1b+i*0x34,0x18,0x04,0x12,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
	Display_Data_Command(5,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
	Display_Data_Command(6,0x20,0x4f-i*0x34,0x0d,0x0e,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
	Display_Data_Command(6,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
	Send_Command(title_display_on);
	Key_Value=0;
	while(Key_Value !=ADD && Key_Value !=SUBTRACT&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case UP:
			case DOWN:
				i=1-i;
				if(i)
				{
					Display_Data_Command(5,0x20,0x4f,0x18,0x04,0x12,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					Display_Data_Command(6,0x20,0x1b,0x0d,0x0e,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				}
				else
				{
					Display_Data_Command(5,0x20,0x1b,0x18,0x04,0x12,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
					Display_Data_Command(6,0x20,0x4f,0x0d,0x0e,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				}
				Send_Command(title_display_on);
				break;
			default:
				break;
		}
	}
	Send_Command(title_clear);
	Key_Value=0;
	return i;
}

void Zoom_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0: //ÏÔÊ¾0ÐÐÄÚÈÝ"                    ZOOM SET"±
				 Display_Data_Command(TITLE_LINE_0,FRONT,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_space);
				 Display_Data_Command(TITLE_LINE_0,BACK,char_space,char_space,char_Z,char_O,char_O,char_M,char_space,char_S,char_E,char_T);
			break;
		case 1: //ÏÔÊ¾µÚ1ÐÐÄÚÈÝ"  ZOOM SPPEEN               N"
				 Display_Data_Command(TITLE_LINE_1,FRONT,display_select_flag[select_flag],char_Z,char_O,char_O,char_M,char_space,char_S,char_P,char_E,char_E);
				 Display_Data_Command(TITLE_LINE_1,BACK,char_D,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_space,display_num[zoom_speed]);
			break;
		case 2: //ÏÔÊ¾µÚ2ÐÐÄÚÈÝ"  DIG	ZOOM             ON/OFF"±
				 Display_Data_Command(TITLE_LINE_2,FRONT,display_select_flag[select_flag],char_D,char_I,char_G,char_space,char_Z,char_O,char_O,char_M,char_space);
					if(dig_zoom)Display_Data_Command(TITLE_LINE_2,BACK,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_O,char_F,char_F);
					else Display_Data_Command(TITLE_LINE_2,BACK,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_space,char_O,char_N);
			break;
		case 3:
				 Display_Data_Command(TITLE_LINE_3,FRONT,display_select_flag[select_flag],char_O,char_S,char_D,char_space,char_space,char_space,char_space,char_space,char_space);
					if(osd)Display_Data_Command(TITLE_LINE_3,BACK,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);
					else Display_Data_Command(TITLE_LINE_3,BACK,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);
			break;
		case 4:		
				 Display_Data_Command(TITLE_LINE_4,FRONT,display_select_flag[select_flag],0x19,0x0e,0x0e,0x0c,0x1b,0x03,0x08,0x12,0x0f);
					if(zoom_display) Display_Data_Command(TITLE_LINE_4,BACK,char_L,char_A,char_Y,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);
					else Display_Data_Command(TITLE_LINE_4,BACK,char_L,char_A,char_Y,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);
			break;
		case 5:		
				 Display_Data_Command(TITLE_LINE_5,FRONT,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(TITLE_LINE_5,BACK,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}


void ZoomSet_Back(u8 select_flag)
{
 switch (select_flag)
	 {
		case 1: 
			zoom_speed=zoom_seeed_pre;	
			break;
		case 2:
			dig_zoom=dig_zoom_pre;
				break;
		case 3:
			osd=osd_pre;
			break;
		case 4:	
			zoom_display=zoom_display_pre;
			break;
		default:			
			break;
	 }
}


void ZoomSet_Page(void)
{
	u8 zoom_select_num=1;
	Send_Command(title_clear);
	Zoom_Select(0,1);
	Zoom_Select(1,1);
	Zoom_Select(2,0);
	Zoom_Select(3,0);
	Zoom_Select(4,0);
	Zoom_Select(5,0);
	Send_Command(title_display_on);
	while(zoom_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					ZoomSet_Back(zoom_select_num);
				else 
				{
					save_flag=0;
				}
				Zoom_Select(zoom_select_num,0);
				if(zoom_select_num<5)
					zoom_select_num++;
				else
					zoom_select_num=1;
				Zoom_Select(zoom_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					ZoomSet_Back(zoom_select_num);
				else 
					save_flag=0;
				Zoom_Select(zoom_select_num,0);
				if(zoom_select_num>1)
					zoom_select_num--;
				else
					zoom_select_num=5;
				Zoom_Select(zoom_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:			
			case SUBTRACT:
				switch(zoom_select_num)
				{
					case 1:
						if(Key_Value==ADD)
						{
								zoom_speed=(zoom_speed+1)%8;
						}
						else
						{
							if(zoom_speed>0)
								zoom_speed--;
							else
								zoom_speed=7;
						}
						Zoom_Select(1,2);
						break;
					case 2:
						dig_zoom=1-dig_zoom;
						Zoom_Select(2,2);
						break;
					case 3:
						osd=1-osd;
						Zoom_Select(3,2);
						break;
					case 4:
						zoom_display=1-zoom_display;
						Zoom_Select(4,2);
						break;
					case 5:
 						Send_Command(title_clear);
						zoom_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;
			default:
				break;				
		}
	}
}
void Focus_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x05,0x0e,0x02,0x14,0x12,0x1b,0x12,0x04,0x13);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0c,0x0e,0x03,0x04,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,
															display_ae_mode[focus_mode][0],
															display_ae_mode[focus_mode][1],
															display_ae_mode[focus_mode][2],
															display_ae_mode[focus_mode][3],
															display_ae_mode[focus_mode][4],
															display_ae_mode[focus_mode][5],
															display_ae_mode[focus_mode][6],
															display_ae_mode[focus_mode][7]);	//´Ë´¦µ÷ÓÃÁËae_modeÖÐµÄAUTO  MANUAL
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x05,0x1b,0x12,0x04,0x0d,0x12,0x1b,0x1b);
				if(af_sens==0) //NORMAL
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x0d,0x0e,0x11,0x0c,0x00,0x0b);
				else
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x0b,0x0e,0x16);
					
			break;
		case 3:
				
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x05,0x0e,0x02,0x14,0x12,0x1b,0x0b,0x08,0x06);
				if(focus_light==0)
				 Display_Data_Command(x,0x30,0x07,0x13,0x1b,0x1b,0x0d,0x0e,0x11,0x0c,0x00,0x0b);
				else
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x08,0x11);	
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0d,0x04,0x00,0x11,0x1b,0x0b,0x08,0x0c,0x08);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
															display_near_limit[near_limit-1][0],	
															display_near_limit[near_limit-1][1],
															display_near_limit[near_limit-1][2],
															display_near_limit[near_limit-1][3]);
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}


void FocusSet_Back(u8 select_flag)
{
 switch (select_flag)
	 {
		case 1: 
			focus_mode=focus_mode_pre;	
			focus_mode_set[4]=focus_mode+2;
			Send_Command(focus_mode_set);
			break;
		case 2:
			af_sens=af_sens_pre;
			af_sens_set[4]=af_sens+2;
			Send_Command(af_sens_set);
				break;
		case 3:
				focus_light=focus_light_pre;
				focus_light_set[4]=focus_light;
				Send_Command(focus_light_set);
			break;
		case 4:	
			near_limit=near_limit_pre;
			near_limit_set[4]=near_limit;
			Send_Command(near_limit_set);
			break;
		default:			
			break;
	 }
}
void FocusSet_Page(void)
{
	u8 focus_select_num=1;
	Send_Command(title_clear);
	Focus_Select(0,1);
	Focus_Select(1,1);
	Focus_Select(2,0);
	Focus_Select(3,0);
	Focus_Select(4,0);
	Focus_Select(5,0);
	Send_Command(title_display_on);
	while(focus_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					FocusSet_Back(focus_select_num);
				else 
					save_flag=0;
				Focus_Select(focus_select_num,0);
				if(focus_select_num<5)
					focus_select_num++;
				else
					focus_select_num=1;
				Focus_Select(focus_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					FocusSet_Back(focus_select_num);
				else 
					save_flag=0;
				Focus_Select(focus_select_num,0);
				if(focus_select_num>1)
					focus_select_num--;
				else
					focus_select_num=5;
				Focus_Select(focus_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(focus_select_num)
				{
					case 1:
						focus_mode=1-focus_mode;
						focus_mode_set[4]=focus_mode+2;
						Send_Command(focus_mode_set);
						Focus_Select(1,2); 
						break;					
					case 2:
						af_sens=1-af_sens;
						af_sens_set[4]=af_sens+2;
						Send_Command(af_sens_set);
						Focus_Select(2,2); 
						break;
					case 3:
						focus_light=1-focus_light;
						focus_light_set[4]=focus_light;
						Send_Command(focus_light_set);
						Focus_Select(3,2); 
						break;
					case 4:
						if(Key_Value==7)
						{
							near_limit%=15;
							near_limit++;
						}
						else
						{
							if(near_limit>1) near_limit--;
							else near_limit=15;
						}
						near_limit_set[4]=near_limit;
						Send_Command(near_limit_set);
						Focus_Select(4,2); 
						break;
					case 5:
						Send_Command(title_clear);
						focus_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;				
		}
	}
}

void Exposure_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x04,0x17);
				 Display_Data_Command(x,0x30,0x0f,0x0e,0x12,0x14,0x11,0x04,0x1b,0x12,0x04,0x13);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x04,0x1b,0x0c,0x0e,0x03,0x04,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,
															display_ae_mode[ae_mode][0],
															display_ae_mode[ae_mode][1],
															display_ae_mode[ae_mode][2],
															display_ae_mode[ae_mode][3],
															display_ae_mode[ae_mode][4],
															display_ae_mode[ae_mode][5],
															display_ae_mode[ae_mode][6],
															display_ae_mode[ae_mode][7]);	
			break;
		case 2:
				Display_Data_Command(x,0x20,display_select_flag[select_flag],0x12,0x07,0x14,0x13,0x13,0x04,0x11,0x1b,0x1b);
				if(ae_mode==1 ||ae_mode==2)
				{
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,
															display_shutter[shutter][0],			
															display_shutter[shutter][1],
															display_shutter[shutter][2],
															display_shutter[shutter][3],
															display_shutter[shutter][4],
															display_shutter[shutter][5],
															display_shutter[shutter][6]);
				}
				else
				{
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x4f);
				}
			break;
		case 3:
				
			Display_Data_Command(x,0x20,display_select_flag[select_flag],0x08,0x11,0x08,0x12,0x1b,0x1b,0x1b,0x1b,0x1b);
			if(ae_mode==1 ||ae_mode==3)
			{
				Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,
															display_iris[iris][0],			
															display_iris[iris][1],
															display_iris[iris][2],
															display_iris[iris][3],
															display_iris[iris][4]);
			}
			else
			{
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x4f);
			}
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x06,0x00,0x08,0x0d,0x1b,0x1b,0x1b,0x1b,0x1b);
				 if(ae_mode==1)
				 {
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,(gain)>5 ? display_num[(gain-1)*2/10] :0x1b,display_num[(gain-1)*2%10],0x03,0x01);
				 }
				 else
				 {
						Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x4f);
				 }
			break;
		case 5:	
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x11,0x08,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				  if(ae_mode==4)
				 {
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,(bri)>9 ? display_num[bri/10] :0x1b,display_num[bri%10]);
				 }
				 else
				 {
						Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x4f);
				 }
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x12,0x0b,0x0e,0x16,0x1b,0x00,0x04,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,slow_ae>9 ?display_num[slow_ae/10]:0x1b,display_num[slow_ae%10]);
			break;
		case 7:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x12,0x0b,0x0e,0x16,0x1b,0x12,0x07,0x14,0x13);
				 Display_Data_Command(x,0x30,0x13,0x04,0x11,0x1b,0x1b,0x1b,0x1b,display_onoff[slow_shutter][0],display_onoff[slow_shutter][1],display_onoff[slow_shutter][2]);
			break;
		case 8:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}






void Exposure_Back(u8 select_flag)
{
 switch (select_flag)
	 {
		case 1: 
			ae_mode=ae_mode_pre;
			switch(ae_mode)
			{
				case 0:  //auto
					Send_Command(ae_mode_auto);			
					break;
				case 1:  //manual
					Send_Command(ae_mode_manual);  //ÊÖ¶¯Ä£Ê½
					shutter_set[6]=shutter/16;
					shutter_set[7]=shutter%16;
					Send_Command(shutter_set);
					iris_set[6]=(iris+4)>15 ? 1:0 ;
					iris_set[7]=iris==0 ? 0:(iris%16)+4;
					Send_Command(iris_set);
					gain_set[7]=gain;
					Send_Command(gain_set);
					break;
				case 2:  //shut pri
					Send_Command(ae_mode_shutpri);  //Ä£Ê½ÇÐ»»½
					shutter_set[6]=shutter/16;
					shutter_set[7]=shutter%16;
					Send_Command(shutter_set); //¿ìÃÅÉèÖÃ
					break;
				case 3 :  //iris prl
					Send_Command(ae_mode_irisprl);//Ä£Ê½ÇÐ»»
					iris_set[6]=(iris+4)>15 ? 1:0 ;
					iris_set[7]=iris==0 ? 0:(iris%16)+4;
					Send_Command(iris_set); //¹âÈ¦ÉèÖÃ
					break;
				case 4:  //bright
					Send_Command(ae_mode_bright);  //Ä£Ê½ÇÐ»»
					
					bright_set[6]=(bri+4)>15 ? 1:0;
					bright_set[7]=bri==0 ? 0:(bri%16)+4;
					Send_Command(bright_set); //ÁÁ¶È
					break;
				default:
					break;
			}
			break;
		case 2:
			shutter=shutter_pre;
			shutter_set[6]=shutter/16;
			shutter_set[7]=shutter%16;
			Send_Command(shutter_set); //¿ìÃÅÉèÖÃ
				break;
		case 3:
				iris=iris_pre;
				iris_set[6]=(iris+4)>15 ? 1:0 ;
				iris_set[7]=iris==0 ? 0:(iris%16)+4;
				Send_Command(iris_set); //¹âÈ¦ÉèÖÃ //¹âÈ¦ÉèÖÃ
			break;
		case 4:	
			gain=gain_pre;
			gain_set[8]=gain;
			Send_Command(gain_set);//ÔöÒæÉèÖÃ
			break;
	  case 5:
			bri=bri_pre;
			bright_set[6]=(bri+4)>15 ? 1:0;
			bright_set[7]=bri==0 ? 0:(bri%16)+4;
			Send_Command(bright_set); //ÁÁ¶È
				break;
		case 6:
			slow_ae=slow_ae_pre;	
			slow_ae_set[4]=slow_ae;
			Send_Command(slow_ae_set); //Ä£Ê½ÇÐ»»
			break;
		case 7:	
			slow_shutter=slow_shutter_pre;
			slow_shutter_set[4]=slow_shutter;
			Send_Command(slow_shutter_set);
			break;
		default:			
			break;
	 }
}

void ExposureSet_Page(void)
{
	u8 exposure_select_num=1;
	Send_Command(title_clear);
	Exposure_Select(0,1);
	Exposure_Select(1,1);
	Exposure_Select(2,0);
	Exposure_Select(3,0);
	Exposure_Select(4,0);
	Exposure_Select(5,0);
	Exposure_Select(6,0);
	Exposure_Select(7,0);
	Exposure_Select(8,0);
	Send_Command(title_display_on);
	while(exposure_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					Exposure_Back(exposure_select_num);
				else
					save_flag=0;
				Exposure_Select(exposure_select_num,0);
				if(exposure_select_num<8)
					exposure_select_num++;
				else
					exposure_select_num=1;
				Exposure_Select(exposure_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					Exposure_Back(exposure_select_num);
				else
					save_flag=0;
				Exposure_Select(exposure_select_num,0);
				if(exposure_select_num>1)
					exposure_select_num--;
				else
					exposure_select_num=8;
				Exposure_Select(exposure_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(exposure_select_num)
				{
					case 1:
						if(Key_Value==ADD)
							ae_mode=(ae_mode+1)%5;
						else
						{
							if(ae_mode>0)ae_mode--;
							else ae_mode=0;
						}
						switch(ae_mode)
						{
							case 0:  //auto
								Send_Command(ae_mode_auto);			
								break;
							case 1:  //manual
								Send_Command(ae_mode_manual);  //ÊÖ¶¯Ä£Ê½
								shutter_set[6]=shutter/16;
								shutter_set[7]=shutter%16;
								Send_Command(shutter_set);
								iris_set[6]=(iris+4)>15 ? 1:0 ;
								iris_set[7]=iris==0 ? 0:(iris%16)+4;
								Send_Command(iris_set);
								gain_set[7]=gain;
								Send_Command(gain_set);
								break;
							case 2:  //shut pri
								Send_Command(ae_mode_shutpri);  //Ä£Ê½ÇÐ»»½
								shutter_set[6]=shutter/16;
								shutter_set[7]=shutter%16;
								Send_Command(shutter_set); //¿ìÃÅÉèÖÃ
								break;
							case 3 :  //iris prl
								Send_Command(ae_mode_irisprl);//Ä£Ê½ÇÐ»»
								iris_set[6]=(iris+4)>15 ? 1:0 ;
								iris_set[7]=iris==0 ? 0:(iris%16)+4;
								Send_Command(iris_set); //¹âÈ¦ÉèÖÃ
								break;
							case 4:  //bright
								Send_Command(ae_mode_bright);  //Ä£Ê½ÇÐ»»
								
								bright_set[6]=(bri+4)>15 ? 1:0;
								bright_set[7]=bri==0 ? 0:(bri%16)+4;
								Send_Command(bright_set); //ÁÁ¶È
								break;
							default:
								break;
						}
						Exposure_Select(1,2);		
						Exposure_Select(2,0); 
						Exposure_Select(3,0);
						Exposure_Select(4,0);
						Exposure_Select(5,0);
						break;
					case 2:
						if(ae_mode==1 ||ae_mode==2)
						{
							if(Key_Value==ADD)
								shutter=(shutter+1)%22;
							else
							{
								if(shutter>0)shutter--;
								else shutter=21;
							}
							shutter_set[6]=shutter/16;
							shutter_set[7]=shutter%16;
							Send_Command(shutter_set); //¿ìÃÅÉèÖÃ
							Exposure_Select(2,2); 
						}
						break;
					case 3:
						if(ae_mode==1 ||ae_mode==3)
						{
							if(Key_Value==ADD)
								iris=(iris+1)%14;
							else
							{
								if(iris>0)iris--;
								else iris=13;
							}
							iris_set[6]=(iris+4)>15 ? 1:0 ;
							iris_set[7]=iris==0 ? 0:(iris%16)+4;
							Send_Command(iris_set); //¹âÈ¦ÉèÖÃ //¹âÈ¦ÉèÖÃ
							Exposure_Select(3,2); 
						}
						break;
					case 4:
						if(ae_mode==1)
						{
							if(Key_Value==ADD)
							{
								gain%=15;
								gain++;
							}
							else
							{
								if(gain>1)gain--;
								else gain=15;
							}
							gain_set[8]=gain;
							Send_Command(gain_set);//ÔöÒæÉèÖÃ
							Exposure_Select(4,2); 						
						}
						break;
					case 5:
						if(ae_mode==4)
						{
							if(Key_Value==ADD)
								bri=(bri+1)%28;
							else
							{
								if(bri>0)bri--;
								else bri=27;
							}
							bright_set[6]=(bri+4)>15 ? 1:0;
							bright_set[7]=bri==0 ? 0:(bri%16)+4;
							Send_Command(bright_set); //ÁÁ¶È
							Exposure_Select(5,2); 
						}
						break;
					case 6:
						if(Key_Value==ADD)
						{
							slow_ae%=48;
							slow_ae++;
						}
						else
						{
							if(slow_ae>1)slow_ae--;
							else slow_ae=48;
						}
						slow_ae_set[4]=slow_ae;
						Send_Command(slow_ae_set); //Ä£Ê½ÇÐ»»
						Exposure_Select(6,2); 
						break;
					case 7:
						slow_shutter=1-slow_shutter;
						slow_shutter_set[4]=slow_shutter;
						Send_Command(slow_shutter_set);
						Exposure_Select(7,2);
						break;
					case 8:
						Send_Command(title_clear);
						exposure_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;		
			default:
				break;				
		}
	}
}

void White_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x16,0x07,0x08);
				 Display_Data_Command(x,0x30,0x13,0x04,0x1B,0x01,0x00,0x0B,0x00,0x0D,0x02,0x04);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x16,0x01,0x1b,0x0c,0x0e,0x03,0x04,0x1b,0x1b);
						Display_Data_Command(x,0x30,0x1b,0x1b,
																	display_wb_mode[wb_mode][0],
																	display_wb_mode[wb_mode][1],
																	display_wb_mode[wb_mode][2],
																	display_wb_mode[wb_mode][3],
																	display_wb_mode[wb_mode][4],
																	display_wb_mode[wb_mode][5],
																	display_wb_mode[wb_mode][6],
																	display_wb_mode[wb_mode][7]);
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x11,0x04,0x03,0x1B,0x06,0x00,0x08,0x0D,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(red_gain/100)>0 ?display_num[red_gain/100] :0x1b ,
																		red_gain>9 ?display_num[red_gain/10%10] :0x1b,
																		display_num[red_gain%10]);
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x0B,0x14,0x04,0x1b,0x06,0x00,0x08,0x0D);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(blue_gain/100)>0 ?display_num[blue_gain/100] :0x1b ,
																		blue_gain>9 ?display_num[blue_gain/10%10] :0x1b,
																		display_num[blue_gain%10] );
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0E,0x0D,0x04,0x1b,0x0F,0x14,0x12,0x07,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0X1B);
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}



void White_Balance_Back(u8 select_flag)
{
 switch (select_flag)
	 {
		case 1: 
			wb_mode=wb_mode_pre;
			wb_mode_set[4]=wb_mode;
			Send_Command(wb_mode_set);
			if(wb_mode==8 || wb_mode==5)
			{
				red_gain_set[6]=red_gain/16;
				red_gain_set[7]=red_gain%16;
				Send_Command(red_gain_set);
				blue_gain_set[6]=blue_gain/16;
				blue_gain_set[7]=blue_gain%16;
				Send_Command(blue_gain_set);
			}
			if(wb_mode==3)
			{
				Send_Command(one_push_set);
			}
			break;
		case 2:
			red_gain=red_gain_pre;
			red_gain_set[6]=red_gain/16;
			red_gain_set[7]=red_gain%16;
			Send_Command(red_gain_set);
				break;
		case 3:
			blue_gain=blue_gain_pre;	
			blue_gain_set[6]=blue_gain/16;
			blue_gain_set[7]=blue_gain%16;
			Send_Command(blue_gain_set);
			break;
		default:			
			break;
	 }
}
void White_Balance_Page(void)
{
	u8 white_select_num=1;
	Send_Command(title_clear);
	White_Select(0,1);
	White_Select(1,1);
	White_Select(2,0);
	White_Select(3,0);
	White_Select(4,0);
	White_Select(5,0);
	Send_Command(title_display_on);
// 	Set_Command(0x06,0x81,0x01,0x04,0x35,wb_mode,0xff,0x00,0x00,0x00,0x00);  //ÓÃµ±Ç°²ÎÊý¸üÐÂÄ£Ê½Â
	while(white_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					White_Balance_Back(white_select_num);
				else
					save_flag=0;
				White_Select(white_select_num,0);
				if(white_select_num<5)
					white_select_num++;
				else
					white_select_num=1;
				White_Select(white_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					White_Balance_Back(white_select_num);
				else
					save_flag=0;
				White_Select(white_select_num,0);
				if(white_select_num>1)
					white_select_num--;
				else
					white_select_num=5;
				White_Select(white_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(white_select_num)
				{
					case 1:
						if(Key_Value==ADD)
						{
							wb_mode=(wb_mode+1)%9;
						}
						else
						{
							if(wb_mode>0)
								wb_mode--;
							else
								wb_mode=8;
						}
						wb_mode_set[4]=wb_mode;
						Send_Command(wb_mode_set);
						if(wb_mode==8 || wb_mode==5)
						{
							red_gain_set[6]=red_gain/16;
							red_gain_set[7]=red_gain%16;
							Send_Command(red_gain_set);
							blue_gain_set[6]=blue_gain/16;
							blue_gain_set[7]=blue_gain%16;
							Send_Command(blue_gain_set);
						}
						if(wb_mode==3)
						{
							Send_Command(one_push_set);
						}
						White_Select(1,2);

						break;
					case 2:
						if(wb_mode==8 || wb_mode==5)
						{
							if(Key_Value==ADD)
								red_gain=(red_gain+1)%256;
							else
							{
								if(red_gain>0)
									red_gain--;
								else
									red_gain=255;
							}
							
							red_gain_set[6]=red_gain/16;
							red_gain_set[7]=red_gain%16;
							Send_Command(red_gain_set);
							White_Select(2,2);
						}
						break;
					case 3:
						if(wb_mode==8 || wb_mode==5)
						{
							if(Key_Value==ADD)
								blue_gain=(blue_gain+1)%256;
							else
							{
								if(blue_gain>0)
									blue_gain--;
								else
									blue_gain=255;
							}
							blue_gain_set[6]=blue_gain/16;
							blue_gain_set[7]=blue_gain%16;
							Send_Command(blue_gain_set);
							White_Select(3,2);
						}
						break;
					case 4:
						break;
					case 5:
						Send_Command(title_clear);
						white_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;
			default:
				break;				
		}
	}
}


void MANU_WB_SET_Back(u8 select_flag)
{
 switch (select_flag)
	 {
		case 2:
			red_gain=red_gain_pre;
			red_gain_set[6]=red_gain/16;
			red_gain_set[7]=red_gain%16;
			Send_Command(red_gain_set);
				break;
		case 3:
			blue_gain=blue_gain_pre;	
			blue_gain_set[6]=blue_gain/16;
			blue_gain_set[7]=blue_gain%16;
			Send_Command(blue_gain_set);
			break;
		default:			
			break;
	 }
}

void MANU_WB_SET(void)
{
	
	u8 white_select_num=2;
	wb_mode=5;
	Send_Command(title_clear);
	White_Select(0,1);
	White_Select(1,0);
	White_Select(2,1);
	White_Select(3,0);
	White_Select(4,0);
	White_Select(5,0);
	Send_Command(title_display_on);
	wb_mode_set[4]=wb_mode;
	Send_Command(wb_mode_set);
	while(white_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					MANU_WB_SET_Back(white_select_num);
				else
					save_flag=0;
				White_Select(white_select_num,0);
				if(white_select_num<5)
					white_select_num++;
				else
					white_select_num=2;
				White_Select(white_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					MANU_WB_SET_Back(white_select_num);
				else
					save_flag=0;
				White_Select(white_select_num,0);
				if(white_select_num>2)
					white_select_num--;
				else
					white_select_num=5;
				White_Select(white_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(white_select_num)
				{
					case 2:

							if(Key_Value==ADD)
								red_gain=(red_gain+1)%256;
							else
							{
								if(red_gain>0)
									red_gain--;
								else
									red_gain=255;
							}
							
							red_gain_set[6]=red_gain/16;
							red_gain_set[7]=red_gain%16;
							Send_Command(red_gain_set);
							White_Select(2,2);
						break;
					case 3:

							if(Key_Value==ADD)
								blue_gain=(blue_gain+1)%256;
							else
							{
								if(blue_gain>0)
									blue_gain--;
								else
									blue_gain=255;
							}
							blue_gain_set[6]=blue_gain/16;
							blue_gain_set[7]=blue_gain%16;
							Send_Command(blue_gain_set);
							White_Select(3,2);
						break;
					case 4:
						break;
					case 5:
						Send_Command(title_clear);
						white_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;
			default:
				break;				
		}
	}
}


void display_privacy_area_on(char num)
{
			display_privacy_onoff_command[7]|=(1<<num);
			Send_Command(display_privacy_onoff_command);
			display_privacy_area_xywh[4]=num;
			display_privacy_area_xywh[5]=privacy_data[num][1]/16;
			display_privacy_area_xywh[6]=privacy_data[num][1]%16;
			display_privacy_area_xywh[7]=privacy_data[num][2]/16;
			display_privacy_area_xywh[8]=privacy_data[num][2]%16;
			display_privacy_area_xywh[9]=privacy_data[num][3]/16;
			display_privacy_area_xywh[10]=privacy_data[num][3]%16;
			display_privacy_area_xywh[11]=privacy_data[num][4]/16;
			display_privacy_area_xywh[12]=privacy_data[num][4]%16;
			Send_Command(display_privacy_area_xywh);
			Send_Command(privacy_mask_color);
}
void display_privacy_area_off(char num)
{
			display_privacy_onoff_command[7] &=~(1<<num);
			Send_Command(display_privacy_onoff_command);
}
void Privacy_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1B,0x0f,0x11,0x08,0x15,0x00,0x02,0x18);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x19,0x0e,0x0d,0x04,0x1B,0x1B,0x1B,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,zone); //ÏÔÊ¾£Á£Â£Ã£Ä
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_D,char_I,char_S,char_P,char_L,char_A,char_Y,0x1b,0x1b);
				if(privacy_data[zone][0])  //µ±ÒþË½ÏÔÊ¾¹ØÊ±
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);
				else
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);		
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x17,0x4a,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(privacy_data[zone][1]/100)>0 ?display_num[privacy_data[zone][1]/100] :0x1b ,
																		privacy_data[zone][1]>9 ?display_num[privacy_data[zone][1]/10%10] :0x1b,
																		display_num[privacy_data[zone][1]%10] );
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x18,0x4a,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(privacy_data[zone][2]/100)>0 ?display_num[privacy_data[zone][2]/100] :0x1b ,
																		privacy_data[zone][2]>9 ?display_num[privacy_data[zone][2]/10%10] :0x1b,
																		display_num[privacy_data[zone][2]%10] );
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x16,0x4a,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b ,
																		privacy_data[zone][3]>9 ?display_num[privacy_data[zone][3]/10%10] :0x1b,
																		display_num[privacy_data[zone][3]%10] );
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x07,0x4a,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b ,
																		privacy_data[zone][4]>9 ?display_num[privacy_data[zone][4]/10%10] :0x1b,
																		display_num[privacy_data[zone][4]%10] );
			break;
		case 7:
			Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
			Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;		
		default:			
			break;
	 }
}
//   u8 Centerline[6]={0x81,0x01,0x04,0x7c,0x03,0xff}; //ÖÐÐÄÏß
// u8 set_mask[11]={
// 0x81,0x01,0x04,0x76,0x00,0x00 ,0x05,0x00,0x03,0x00,0xff
// };


void Privacy_Set_Back(u8 select_flag)
{
	 switch (select_flag)
	 {
		case 1: 
			zone=zone_pre;
			Privacy_Select(1,2);
			Privacy_Select(2,0);
			Privacy_Select(3,0);
			Privacy_Select(4,0);
			Privacy_Select(5,0);
			Privacy_Select(6,0);
			if(privacy_data[zone][0]==0)
				display_privacy_area_on(zone);
		case 2:
			privacy_data[zone][0]=privacy_data_pre[zone][0];
			if(privacy_data[zone][0]==0)
				display_privacy_area_on(zone);
			else
				display_privacy_area_off(zone);
				break;
		case 3:
				privacy_data[zone][1]=privacy_data_pre[zone][1];
				if(privacy_data[zone][0]==0)
					display_privacy_area_on(zone);
			break;
		case 4:	
			privacy_data[zone][2]=privacy_data_pre[zone][2];
				if(privacy_data[zone][0]==0)
					display_privacy_area_on(zone);
			break;
	  case 5:
			privacy_data[zone][3]=privacy_data_pre[zone][3];
				if(privacy_data[zone][0]==0)
					display_privacy_area_on(zone);
				break;
		case 6:
			privacy_data[zone][4]=privacy_data_pre[zone][4];
				if(privacy_data[zone][0]==0)
					display_privacy_area_on(zone);
			break;
		default:			
			break;
	 }
}
void Privacy_Set_Page(void)
{
	u8 i=0;
	u8 privacy_select_num=1;
	Send_Command(title_clear);
	Privacy_Select(0,0);
	Privacy_Select(1,1);
	Privacy_Select(2,0);
	Privacy_Select(3,0);
	Privacy_Select(4,0);
	Privacy_Select(5,0);
	Privacy_Select(6,0);
	Privacy_Select(7,0);
	Send_Command(title_display_on);
	
	//Èç¹ûÓÐ´ò¿ªµÄÒþË½ÇøÏÔÊ¾£¬½øÐÐÏÔÊ¾
		if(privacy_data[zone][0]==0)
			display_privacy_area_on(zone);
//   	Send_Command(Centerline);
// 		Send_Command(set_mask);
	while(privacy_select_num<10&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:  //±£´æÍË³ö
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					Privacy_Set_Back(privacy_select_num);
				else
					save_flag=0;
				Privacy_Select(privacy_select_num,0);
				if(privacy_select_num<7)
					privacy_select_num++;
				else
					privacy_select_num=1;
				Privacy_Select(privacy_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					Privacy_Set_Back(privacy_select_num);
				else
					save_flag=0;
				Privacy_Select(privacy_select_num,0);
				if(privacy_select_num>1)
					privacy_select_num--;
				else
					privacy_select_num=7;
				Privacy_Select(privacy_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(privacy_select_num)
				{
					case 1:
						if(Key_Value==ADD)
						{
							zone=(zone+1)%4;
						}
						else
						{
							if(zone>0) zone--;
							else zone=3;
						}
						Privacy_Select(1,2);
						Privacy_Select(2,0);
						Privacy_Select(3,0);
						Privacy_Select(4,0);
						Privacy_Select(5,0);
						Privacy_Select(6,0);
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						break;
					case 2:
						privacy_data[zone][0]=1-privacy_data[zone][0];
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						else
							display_privacy_area_off(zone);
						Privacy_Select(2,2);
						break;
					case 3:
						if(Key_Value==ADD)
							privacy_data[zone][1]++;
						else
							privacy_data[zone][1]--;
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						Privacy_Select(3,2);
						break;
					case 4:
						if(Key_Value==ADD)
							privacy_data[zone][2]++;
						else
							privacy_data[zone][2]--;
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						Privacy_Select(4,2);
						break;
					case 5:
						if(Key_Value==ADD)
							privacy_data[zone][3]=(privacy_data[zone][3]+1)%81;
						else
						{	
							if(privacy_data[zone][3]>4)
								privacy_data[zone][3]--;
							else
								privacy_data[zone][3]=80;
						}
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						Privacy_Select(5,2);
						break;
					case 6:
						if(Key_Value==ADD)
							privacy_data[zone][4]=(privacy_data[zone][4]+1)%81;
						else
						{	
							if(privacy_data[zone][4]>4)
								privacy_data[zone][4]--;
							else
								privacy_data[zone][4]=80;
						}
						if(privacy_data[zone][0]==0)
							display_privacy_area_on(zone);
						Privacy_Select(6,2);
						break;
					case 7:
						Send_Command(title_clear);
						privacy_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;
			
		}
	}
}
void Advance_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1B,0x00,0x03,0x15,0x00,0x0D,0x02,0x04);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0f,0x11,0x08,0x15,0x00,0x02,0x18,0x1b,0x12);
				 Display_Data_Command(x,0x30,0x04,0x13,0x4c,0x4c,0x4c,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x0b,0x02,0x1B,0x1B,0x1B,0x1B,0x1B,0x1b);
				 if(blc)
				 {
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);  //off
				 }
				 else
				 {
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N); 
				 }
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x04,0x17,0x0f,0x02,0x0e,0x0c,0x0f,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x0e,0x05,0x05);  //off
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x16,0x03,0x11,0x1B,0x1B,0x1B,0x1B,0x1B,0x1b);
				 if(wdr==2)
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);  //ON
				 else if(wdr==3)
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);  //ON
				 else
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_A,char_U,char_T,char_O);  //ON
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x08,0x11,0x02,0x14,0x13,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x00,0x14,0x13,0x0e);
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0d,0x11,0x1b,0x0b,0x04,0x15,0x04,0x0b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_num[nr_level]); //ÏÔÊ¾Êý×Ö3£¬¸ù¾ÝÊµ¼Êµ÷Õû
			break;
		case 7:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x12,0x13,0x00,0x01,0x08,0x0b,0x08,0x19,0x04);
				 if(stabilizer)
				 {
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);  //off
				 }
				 else
				 {
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N); 
				 }
			break;
		case 8:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x03,0x04,0x05,0x0e,0x06,0x1b,0x0c,0x0e,0x03);
				 if(defog_mode)
				 {
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);  //off
				 }
				 else
				 {
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N); 
				 }
			break;
		case 9:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x0b,0x00,0x11,0x0c,0x1b,0x0e,0x14,0x13);
				 Display_Data_Command(x,0x30,0x04,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x0e,0x05,0x05);  //off
			break;
		case 10:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}


void Advance_Back(u8 select_flag)
{
	 switch (select_flag)
	 {
		case 2:
			blc=blc_pre;
			blc_set[4]=blc+2;
			Send_Command(blc_set);
			break;
		case 4:	
			wdr=wdr_pre;
			wdr_set[4]=wdr;
			Send_Command(wdr_set);
			break;
		case 6:
			nr_level=nr_level_pre;
			nr_level_set[4]=nr_level;
			Send_Command(nr_level_set);
			break;
		case 7:
			stabilizer=stabilizer_pre;
			stabilizer_set[4]=stabilizer;
			Send_Command(stabilizer_set);
			break;
		case 8:
			defog_mode=defog_mode_pre;
			defog_mode_set[4]=defog_mode;
			Send_Command(defog_mode_set);
			break;		
		default:			
			break;
	 }
}
void Advance_Page(void)
{
	u8 advance_select_num=1;
	Send_Command(title_clear);
	Advance_Select(0,1);
	Advance_Select(1,1);
	Advance_Select(2,0);
	Advance_Select(3,0);
	Advance_Select(4,0);
	Advance_Select(5,0);
	Advance_Select(6,0);
	Advance_Select(7,0);
	Advance_Select(8,0);
	Advance_Select(9,0);
	Advance_Select(10,0);
	Send_Command(title_display_on);
	while(advance_select_num<15&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü

				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					Advance_Back(advance_select_num);
				else
					save_flag=0;
				Advance_Select(advance_select_num,0);
				if(advance_select_num<10)
					advance_select_num++;
				else
					advance_select_num=1;
				Advance_Select(advance_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					Advance_Back(advance_select_num);
				else
					save_flag=0;
				Advance_Select(advance_select_num,0);
				if(advance_select_num>1)
					advance_select_num--;
				else
					advance_select_num=10;
				Advance_Select(advance_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(advance_select_num)
				{
					case 1:
						Privacy_Set_Page();  //ÉèÖÃÒþË½±£»¤
		
							Advance_Select(0,1);
							Advance_Select(1,1);
							Advance_Select(2,0);
							Advance_Select(3,0);
							Advance_Select(4,0);
							Advance_Select(5,0);
							Advance_Select(6,0);
							Advance_Select(7,0);
							Advance_Select(8,0);
							Advance_Select(9,0);
							Advance_Select(10,0);
							Send_Command(title_display_on);		
							Key_Value=0;
						break;
					case 2:
						blc=1-blc;
						blc_set[4]=blc+2;
						Send_Command(blc_set);
						Advance_Select(2,2);
						break;
					case 3:
						//expcomp ÎÞ·¨ÉèÖÃ,±£ÁôÒÔºóÌí¼Ó
						break;
					case 4:
						if(Key_Value==ADD)
						{
							if(wdr==3)wdr=0;
							else if(wdr==0) wdr=2;
							else wdr=3;
						}
						else
						{
							if(wdr==2)wdr=0;
							else if(wdr==0) wdr=3;
							else wdr=2;
						}
						wdr_set[4]=wdr;
						Send_Command(wdr_set);
						Advance_Select(4,2);
						break;
					case 5:
						//ircut ÎÞ·¨ÉèÖÃ£¬±£ÁôÒÔºóÌí¼Ó
						break;
					case 6:
						if(Key_Value==ADD)
							nr_level=(nr_level+1)%6;
						else
						{
							if(nr_level>0) nr_level--;
							else nr_level=5;
						}
						nr_level_set[4]=nr_level;
						Send_Command(nr_level_set);
						Advance_Select(6,2);
						break;
					case 7:
						stabilizer=1-stabilizer;
						stabilizer_set[4]=stabilizer;
						Send_Command(stabilizer_set);
						Advance_Select(7,2);
						break;
					case 8:
						defog_mode=1-defog_mode;
						defog_mode_set[4]=defog_mode;
						Send_Command(defog_mode_set);
						Advance_Select(8,2);
						break;
					case 9:
						//alarm out ÉèÖÃÎÞÐ§
						break;
					case 10:
						Send_Command(title_clear);
						advance_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;				
		}
	}
}





void Image_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1B,0x1B,0x1B,0x08,0x0c,0x00,0x06,0x04);
			break;
		case 1:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x0f,0x04,0x11,0x13,0x14,0x11,0x04,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b+(aperture/10)*3,display_num[aperture%10]); 
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x02,0x0e,0x0b,0x0e,0x11,0x1B,0x06,0x00,0x08);
				 Display_Data_Command(x,0x30,0x0d,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b+(color_gain/10)*3,display_num[color_gain%10]); 
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x02,0x0e,0x0b,0x0e,0x11,0x1b,0x07,0x14,0x04);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b+(color_hum/10)*3,display_num[color_hum%10]);  
			break;
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x02,0x07,0x11,0x0e,0x0c,0x00,0x1B,0x12,0x14);
				 Display_Data_Command(x,0x30,0x0f,0x0f,0x11,0x04,0x12,0x12,0x1b,0x1b,0x1b,display_num[chroma_suppress]); 
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x06,0x00,0x0c,0x0c,0x00,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_num[gamma]);
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0c,0x08,0x11,0x11,0x0e,0x11,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_onoff[mirror][0],display_onoff[mirror][1],display_onoff[mirror][2]); 
			break;
		case 7:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x05,0x0b,0x08,0x0f,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_onoff[flip][0],display_onoff[flip][1],display_onoff[flip][2]);  //off
			break;
		case 8:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x05,0x11,0x04,0x04,0x19,0x04,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_onoff[freeze][0],display_onoff[freeze][1],display_onoff[freeze][2]); //off
			break;
		case 9:		
				 switch(pic_effect)
						{
							case 0:
								Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0f,0x08,0x02,0x1b,0x04,0x05,0x05,0x04,0x02);
								Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x0e,0x05,0x05);  //off
								break;								
							case 1:
								Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0f,0x08,0x02,0x1b,0x04,0x05,0x05,0x04,0x02);
								Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x0d,0x04,0x06,0x4c,0x00,0x11,0x13); 
								break;
							case 2:
								Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0f,0x08,0x02,0x1b,0x04,0x05,0x05,0x04,0x02);
								Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x01,0x4c,0x16); 
								break;
						}

			break;
		case 10:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}


void Image_Back(u8 select_flag)
{
	 switch (select_flag)
	 {
		 case 1:
			aperture=aperture_pre; 
			aperture_set[7]=aperture;
			 Send_Command(aperture_set);
			 break;
		 case 2:
			color_gain=color_gain_pre; 
			color_gain_set[7]=color_gain;
						Send_Command(color_gain_set);
			break;
		 case 3:
			 color_hum=color_hum_pre;
			 color_hum_set[7]=color_hum;
						Send_Command(color_hum_set);
			 break;
		case 4:	
			chroma_suppress=chroma_suppress_pre;
			chroma_suppress_set[4]=chroma_suppress;
						Send_Command(chroma_suppress_set);	
			break;
		case 5:
			gamma=gamma_pre;
			gamma_set[4]=gamma;
						Send_Command(gamma_set);		
			break;
		case 6:
			mirror=mirror_pre;
			mirror_set[4]=mirror+2;
						Send_Command(mirror_set);		
			break;
		case 7:
			flip=flip_pre;
			flip_set[4]=flip+2;
						Send_Command(flip_set);	
			break;
		case 8:
			freeze=freeze_pre;
			freeze_set[4]=freeze+2;
						Send_Command(freeze_set);	
			break;	
		case 9:
			pic_effect=pic_effect_pre;
			pic_effect_set[4]=pic_effect*2;
						Send_Command(pic_effect_set);	
			break;
		default:			
			break;
	 }
}

void Image_Page(void)
{
	u8 image_select_num=1;
	Send_Command(title_clear);
	Image_Select(0,1);
	Image_Select(1,1);
	Image_Select(2,0);
	Image_Select(3,0);
	Image_Select(4,0);
	Image_Select(5,0);
	Image_Select(6,0);
	Image_Select(7,0);
	Image_Select(8,0);
	Image_Select(9,0);
	Image_Select(10,0);
	Send_Command(title_display_on);
	while(image_select_num<15&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					Image_Back(image_select_num);
				else
					save_flag=0;
				Image_Select(image_select_num,0);
				if(image_select_num<10)
					image_select_num++;
				else
					image_select_num=1;
				Image_Select(image_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					Image_Back(image_select_num);
				else
					save_flag=0;
				Image_Select(image_select_num,0);
				if(image_select_num>1)
					image_select_num--;
				else
					image_select_num=10;
				Image_Select(image_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(image_select_num)
				{
					case 1:  //aperture  ¿×¾¶			
						if(Key_Value==ADD)
							aperture=(aperture+1)%16;
						else
						{
							if(aperture>0)
								aperture--;
							else
								aperture=15;
						}
						aperture_set[7]=aperture;
						Send_Command(aperture_set);
						Image_Select(1,2);
						break;
					case 2:  // color gainÑÕÉ««
						if(Key_Value==ADD)
							color_gain=(color_gain+1)%15;
						else
						{
							if(color_gain>0)
								color_gain--;
							else
								color_gain=14;
						}	
						color_gain_set[7]=color_gain;
						Send_Command(color_gain_set);
						Image_Select(2,2);
						break;
					case 3:  //color hum ÑÕµ÷
						if(Key_Value==ADD)
							color_hum=(color_hum+1)%15;
						else
						{
							if(color_hum>0)
								color_hum--;
							else
								color_hum=14;
						}
						color_hum_set[7]=color_hum;
						Send_Command(color_hum_set);
						Image_Select(3,2);
						break;
					case 4:   //chroma suppress É«¶ÈÒÖÖÆ
						if(Key_Value==ADD)
							chroma_suppress=(chroma_suppress+1)%4;
						{
							if(chroma_suppress>0)
								chroma_suppress--;
							else
								chroma_suppress=3;
						}
						chroma_suppress_set[4]=chroma_suppress;
						Send_Command(chroma_suppress_set);		
						Image_Select(4,2);
						break;
					case 5:   //gamma Ù¤ÂíÉ	
						if(Key_Value==ADD)
							gamma=(gamma+1)%5;
						else
						{
							if(gamma>0)
								gamma--;
							else
								gamma=4;
						}
						gamma_set[4]=gamma;
						Send_Command(gamma_set);			
						Image_Select(5,2);
						break;
					case 6:
						mirror=1-mirror;
						mirror_set[4]=mirror+2;
						Send_Command(mirror_set);		
						Image_Select(6,2);				
						break;
					case 7:
						flip=1-flip;
						flip_set[4]=flip+2;
						Send_Command(flip_set);	
						Image_Select(7,2);	
						break;
					case 8:
						freeze=1-freeze;
					  freeze_set[4]=freeze+2;
						Send_Command(freeze_set);	
						Image_Select(8,2);			
						break;
					case 9:
						if(Key_Value==ADD)
							pic_effect=(pic_effect+1)%3;
						else
						{
							if(pic_effect>0)
								pic_effect--;
							else
								pic_effect=2;
						}
					  pic_effect_set[4]=pic_effect*2;
						Send_Command(pic_effect_set);	
						Image_Select(9,2);
						break;					
					case 10:
						Send_Command(title_clear);
						image_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;
			default:
				break;				
		}
	}
}



void Sensor_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1B,0x1B,0x12,0x04,0x0d,0x12,0x0e,0x11);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x07,0x08,0x1b,0x11,0x04,0x12,0x0e,0x0b,0x14);
				 if(hi_resolution)
					Display_Data_Command(x,0x30,0x13,0x08,0x0e,0x0d,0x1b,0x1b,0x1b,0x0e,0x05,0x05);
				 else
					Display_Data_Command(x,0x30,0x13,0x08,0x0e,0x0d,0x1b,0x1b,0x1b,0x1b,char_O,char_N); 
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x07,0x08,0x1b,0x12,0x04,0x0d,0x12,0x08,0x13);
				 if(hi_sensitivity)
				  	Display_Data_Command(x,0x30,0x08,0x15,0x08,0x13,0x18,0x1b,0x1b,0x0e,0x05,0x05);
				 else
						Display_Data_Command(x,0x30,0x08,0x15,0x08,0x13,0x18,0x1b,0x1b,0x1b,char_O,char_N);
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}

void Sensor_Bace(u8 select_flag)
{
	switch(select_flag)
	{
		case 1:
			hi_resolution=hi_resolution_pre;
			hi_resolution_set[4]=hi_resolution+2;
			Send_Command(hi_resolution_set);
			break;
		case 2:
			hi_sensitivity=hi_sensitivity_pre;
			hi_sensitivity_set[4]=hi_sensitivity+2;
			Send_Command(hi_sensitivity_set);
			break;
	}
}

void Sensor_Page(void)
{
	u8 sensor_select_num=1;
	Send_Command(title_clear);
	Sensor_Select(0,1);
	Sensor_Select(1,1);
	Sensor_Select(2,0);
	Sensor_Select(3,0);
	Send_Command(title_display_on);
	while(sensor_select_num<15&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
				break;
			case UP:
				if(save_flag==0)
					Sensor_Bace(sensor_select_num);
				else
					save_flag=0;
				Sensor_Select(sensor_select_num,0);
				if(sensor_select_num<3)
					sensor_select_num++;
				else
					sensor_select_num=1;
				Sensor_Select(sensor_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					Sensor_Bace(sensor_select_num);
				else
					save_flag=0;
				Sensor_Select(sensor_select_num,0);
				if(sensor_select_num>1)
					sensor_select_num--;
				else
					sensor_select_num=3;
				Sensor_Select(sensor_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(sensor_select_num)
				{
					case 1:
						hi_resolution=1-hi_resolution;
						hi_resolution_set[4]=hi_resolution+2;
						Send_Command(hi_resolution_set);
						Sensor_Select(1,2);
						break;
					case 2:
						hi_sensitivity=1-hi_sensitivity;
						hi_sensitivity_set[4]=hi_sensitivity+2;
						Send_Command(hi_sensitivity_set);
						Sensor_Select(2,2);
						break;
					case 3:
						Send_Command(title_clear);
						sensor_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;				
		}
	}
}


void Preset_Conf_Select(u8 x, u8 select_flag)
{
	switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,char_P,char_R);
				 Display_Data_Command(x,0x30,char_E,char_S,char_E,char_T,0x1b,char_S,char_C,char_E,char_N,char_E);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_S,char_C,char_E,char_N,char_E,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,display_num[scene_num]);
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_T,char_I,char_M,char_E,0x4e,char_S,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																			scene_data[scene_num][0]>99? display_num[scene_data[scene_num][0]/100] :0x1b,
																			scene_data[scene_num][0]>9? display_num[scene_data[scene_num][0]/10%10]:0x1b,
																			display_num[scene_data[scene_num][0]%10]); 
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_E,char_X,char_I,char_S,char_T,0x1b,0x1b,0x1b,0x1b);
				 if(scene_data[scene_num][1]) //Èç¹û´æÔÚ¾ÍÏÔÊ¾£Ä£Å£Æû
					 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,char_U,char_N,char_D,char_E,char_F);  
				 else
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_D,char_E,char_F);   
			break;	
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_S,char_E,char_T,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_R,char_U,char_N,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_D,char_E,char_L,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 7:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_B,char_A,char_C,char_K,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
	
}

void Preset_Conf_Page(void)
{
	u8 i;
	u8 preset_conf_select_num=1;
	Send_Command(title_clear);
	Preset_Conf_Select(0,0);
	Preset_Conf_Select(1,1);
	Preset_Conf_Select(2,0);
	Preset_Conf_Select(3,0);
	Preset_Conf_Select(4,0);
	Preset_Conf_Select(5,0);
	Preset_Conf_Select(6,0);
	Preset_Conf_Select(7,0);
	Send_Command(title_display_on);
	while(preset_conf_select_num<15&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case UP:
				Preset_Conf_Select(preset_conf_select_num,0);
				if(preset_conf_select_num<7)
					preset_conf_select_num++;
				else
					preset_conf_select_num=1;
				Preset_Conf_Select(preset_conf_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				Preset_Conf_Select(preset_conf_select_num,0);
				if(preset_conf_select_num>1)
					preset_conf_select_num--;
				else
					preset_conf_select_num=7;
				Preset_Conf_Select(preset_conf_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(preset_conf_select_num)
				{
					case 1:
						if(Key_Value==ADD)
						{
							scene_num%=3;
							scene_num=scene_num+1;
						}							
						else
						{
							if(scene_num>1)scene_num--;
							else scene_num=3;
						}
						Preset_Conf_Select(1,2);
						Preset_Conf_Select(2,0);
						Preset_Conf_Select(3,0);
						break;
					case 2:
						if(Key_Value==ADD)scene_data[scene_num][0]++;
						else scene_data[scene_num][0]--;
						Preset_Conf_Select(2,2);
						break;
					case 3:
						break;
					case 4:
						if(scene_num!=0)  //³¡¾°£°²»¿ÉÒÔÐÞ¸Ä
						{
							scene_data[scene_num][1]=0;
							Preset_Conf_Select(4,2);
							Display_Data_Command(4,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_K);
							delay_ms(1000);
							Preset_Conf_Select(4,1);
							Preset_Conf_Select(3,0);
							Display_Data_Command(4,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
							save_scene_data(scene_num);
						}
						break;
					case 5:
						if(scene_data[scene_num][1]==0)
						{
							scene_data_send(scene_num);
							Display_Data_Command(5,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_K);
							delay_ms(1000);							
							Display_Data_Command(5,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
						}
						break;
					case 6:
						if(scene_num!=0)  //³¡¾°£°²»¿ÉÒÔÐÞ¸Ä
						{
							scene_data[scene_num][1]=1;
							Preset_Conf_Select(6,2);
							Display_Data_Command(6,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_K);
							delay_ms(1000);
							Preset_Conf_Select(6,1);
							Preset_Conf_Select(3,0);
							Display_Data_Command(6,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
							break;
						}
					case 7:
						Send_Command(title_clear);
						preset_conf_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
// 				Send_Command(title_display_on);
				break;
			default :
				break;
				
		}
	}
	
}

void General_Set_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
		case 0:
				 Display_Data_Command(x,0x20,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x06);
				 Display_Data_Command(x,0x30,0x04,0x0d,0x04,0x11,0x00,0x0b,0x1b,0x12,0x04,0x13);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x02,0x00,0x0c,0x04,0x11,0x00,0x1b,0x08,0x03);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,num_1);
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x05,0x0e,0x11,0x0c,0x00,0x13,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,
																	display_format[format][0],
																	display_format[format][1],
																	display_format[format][2],
																	display_format[format][3],
																	display_format[format][4],
																	display_format[format][5],
																	display_format[format][6]); 
			break;
		case 3:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_L,0x4e,char_R,0x1b,char_H,char_A,char_N,char_D,0x1b);
				 if(LR_hant)
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_F,char_F);  
				 else
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);   
			break;	
		case 4:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x0f,0x11,0x0e,0x13,0x0e,0x02,0x0e,0x0b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x0f,0x04,0x0b,0x02,0x0e,0x4f,0x0f,0x4e,0x03);
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x14,0x03,0x11,0x00,0x13,0x04,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,
																		(24*(1<<baudrate))>100 ? display_num[(24*(1<<baudrate)/100)] :0x1b,//Èç¹û°ÙÎ»²»µÈÓÚ£°£¬ÏÔÊ¾ÊýÖµ
																		display_num[(24*(1<<baudrate)/10%10)],
																		display_num[(24*(1<<baudrate)%10)],
																		0x27,0x27);  //ÏÔÊ¾²¨ÌØÂÊ 
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x03,0x03,0x11,0x04,0x12,0x12,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
																			address>100? display_num[address/100] :0x1b,
																			address>10? display_num[address/10%10]:0x1b,
																			display_num[address%10]); //off
			break;
		case 7:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],char_P,char_R,char_E,char_S,char_E,char_T,0x1b,char_S,char_C);
				 Display_Data_Command(x,0x30,char_E,char_N,char_E,0x4c,0x4c,0x4c,0x1b,0x1b,0x1b,0x1b); 
			break;
		case 8:		
				Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x14,0x17,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);				 
				if(aux)
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x0e,0x05,0x05);  //off
				else
					Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,char_O,char_N);  //off
			break;
		case 9:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x01,0x00,0x02,0x0a,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}

void General_Set_Bace(u8 select_flag)
{
	switch(select_flag)
	{
		case 2:
			format=format_pre;
			break;
		case 3:
			LR_hant=LR_hant_pre;
			break;
		case 4:
			break;
		case 5:
			baudrate=baudrate_pre;
			break;
		case 6:
			address=address_pre;
			break;
		case 8:
			aux=aux_pre;
			break;
		default:
			break;
	}
}
void General_Set_Page(void)
{
	u8 general_set_select_num=1;
	Send_Command(title_clear);
	General_Set_Select(0,1);
	General_Set_Select(1,1);
	General_Set_Select(2,0);
	General_Set_Select(3,0);
	General_Set_Select(4,0);
	General_Set_Select(5,0);
	General_Set_Select(6,0);
	General_Set_Select(7,0);
	General_Set_Select(8,0);
	General_Set_Select(9,0);
	Send_Command(title_display_on);
	while(general_set_select_num<15&& power_onoff_flag)
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case ENTER:	 //È·¶¨¼ü
				save_flag=1;
							if(general_set_select_num==2)
				{
						work_para[E2ROM_FORMAT_ADDR]=format;
						TWI_WriteByte(work_para[E2ROM_FORMAT_ADDR],E2ROM_FORMAT_ADDR);
						register_set[5]=format_to_command[format]/16;
						register_set[6]=format_to_command[format]%16;
						Send_Command(register_set); 
					
				}
				if(general_set_select_num==5)
				{
					work_para[E2ROM_BAUDRATE_ADDR]=baudrate;
					TWI_WriteByte(work_para[E2ROM_BAUDRATE_ADDR],E2ROM_BAUDRATE_ADDR);
				}
				if(general_set_select_num==6)
				{
					work_para[E2ROM_ADDRESS_ADDR]=address;
					TWI_WriteByte(work_para[E2ROM_ADDRESS_ADDR],E2ROM_ADDRESS_ADDR);
				}
				break;
			case UP:
				if(save_flag==0)
					General_Set_Bace(general_set_select_num);
				else
					save_flag=0;
				General_Set_Select(general_set_select_num,0);
				if(general_set_select_num<9)
					general_set_select_num++;
				else
					general_set_select_num=1;
				General_Set_Select(general_set_select_num,1);	
				Send_Command(title_display_on);
				break;
			case DOWN:
				if(save_flag==0)
					General_Set_Bace(general_set_select_num);
				else
					save_flag=0;
				General_Set_Select(general_set_select_num,0);
				if(general_set_select_num>1)
					general_set_select_num--;
				else
					general_set_select_num=9;
				General_Set_Select(general_set_select_num,1);
				Send_Command(title_display_on);
				break;
			case ADD:
			case SUBTRACT:
				switch(general_set_select_num)
				{
					case 1:
						//CAMERA ID ÉèÖÃ¡¡ÏÖÔÚÓÃ¹Ì¶¨£±£¬²»½øÐÐµ÷½Ú
						break;
					case 2:
						if(Key_Value==ADD)
							format=(format+1)%10;
						else
						{
							if(format>0) format--;
							else format=9;
						}
						//´Ë´¦Ö»½øÐÐÏÔÊ¾£¬±£´æµÄÊ±ºò·¢ËÍÃüÁî
						General_Set_Select(2,2);						
						break;
					case 3:
						LR_hant=1-LR_hant;
						General_Set_Select(3,2);	
						break;
					case 4:
						//protocol ²»ÓÃÐÞ¸Ä
						break;
					case 5:
						if(Key_Value==ADD)
							baudrate=(baudrate+1)%5;
						else
						{
							if(baudrate>0) baudrate--;
							else baudrate=4;
						}
						General_Set_Select(5,2);
						break;
					case 6:
						if(Key_Value==ADD)
						{
							address%=255;
							address++;;
						}
						else
						{
							if(address>1)address--;
							else address=255;
						}
						General_Set_Select(6,2);
							break;
					case 7:
						Preset_Conf_Page();
					
							General_Set_Select(0,1);
							General_Set_Select(1,0);
							General_Set_Select(2,0);
							General_Set_Select(3,0);
							General_Set_Select(4,0);
							General_Set_Select(5,0);
							General_Set_Select(6,0);
							General_Set_Select(7,1);
							General_Set_Select(8,0);
							General_Set_Select(9,0);
							Send_Command(title_display_on);	
							Key_Value=0;
						break;
					case 8:
						aux=1-aux;
						General_Set_Select(8,2);
						break;
					case 9:
						Send_Command(title_clear);
						general_set_select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
						Key_Value=0;
						break;
					default:
						break;
				}
				Send_Command(title_display_on);
				break;
			default:
				break;				
		}
	}
}



void Menu_Select(u8 x, u8 select_flag)
{
	 switch (x)
	 {
	 	case 0:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x19,0x0e,0x0e,0x0c,0x1b,0x12,0x04,0x13,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 1:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x05,0x0e,0x02,0x14,0x12,0x1b,0x12,0x04,0x13);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 2:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x04,0x17,0x0f,0x0e,0x12,0x14,0x11,0x04,0x1b);
				 Display_Data_Command(x,0x30,0x12,0x04,0x13,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 3:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x16,0x07,0x08,0x13,0x04,0x1b,0x01,0x00,0x0b);
				 Display_Data_Command(x,0x30,0x00,0x0d,0x02,0x04,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 4:
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x00,0x03,0x15,0x00,0x0d,0x02,0x04,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 5:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x08,0x0c,0x00,0x06,0x04,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 6:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x12,0x04,0x0d,0x12,0x0e,0x11,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 7:	
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x06,0x04,0x0d,0x04,0x11,0x00,0x0b,0x1b,0x12);
				 Display_Data_Command(x,0x30,0x04,0x13,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 8:	  
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x03,0x04,0x05,0x00,0x14,0x0b,0x13,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		case 9:		
				 Display_Data_Command(x,0x20,display_select_flag[select_flag],0x04,0x17,0x08,0x13,0x1b,0x1b,0x1b,0x1b,0x1b);
				 Display_Data_Command(x,0x30,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b);
			break;
		default:			
			break;
	 }
}

void Menu_Page(void)
{
	u8 factory_reset_flag=0;
	u8 select_num=0;	
	Send_Command(title_clear);
	Menu_Select(0,1);
	Menu_Select(1,0);
	Menu_Select(2,0);
	Menu_Select(3,0);
	Menu_Select(4,0);
	Menu_Select(5,0);
	Menu_Select(6,0);
	Menu_Select(7,0);
	Menu_Select(8,0);
	Menu_Select(9,0);
	Send_Command(title_display_on);
	while(select_num<10&& power_onoff_flag )
	{
		Key_Value=0;
		Key_Value=Scan_Key_Value();	
		switch(Key_Value)
		{
			case UP: 
				
				//Èç¹û¸Õ²Å»Ö¸´³ö³§ÉèÖÃÏî
				if(factory_reset_flag==1)
				{
					Display_Data_Command(8,0x30,0x1b,0x1b,0x1b,0x1b,char_C,char_A,char_N,char_C,char_L,char_E);
					delay_ms(1000);
					factory_reset_flag=0;
				}	
				
				Menu_Select(select_num,0);	
				if(select_num<9)
					select_num++;
				else
					select_num=0;
				Menu_Select(select_num,1);	
				Send_Command(title_display_on);
			  break;
			case DOWN:
					//Èç¹û¸Õ²Å»Ö¸´³ö³§ÉèÖÃÏî
				if(factory_reset_flag==1)
				{
					Display_Data_Command(8,0x30,0x1b,0x1b,0x1b,0x1b,char_C,char_A,char_N,char_C,char_L,char_E);
					delay_ms(1000);
					factory_reset_flag=0;
				}	
				Menu_Select(select_num,0);
				if(select_num>0)
					select_num--;
				else
					select_num=9;
				Menu_Select(select_num,1);
				Send_Command(title_display_on);
			  break;
			case ADD:				
			case SUBTRACT: 
			  if(select_num<8)
			  {
				  switch(select_num)
				  {
					  	case 0:
							ZoomSet_Page();
							break;
						case 1:
							FocusSet_Page();
							break;
						case 2:
							ExposureSet_Page();
							break;
						case 3:
							White_Balance_Page();
							break;
						case 4:
							Advance_Page();
							break;
						case 5:
							Image_Page();
							break;
						case 6:
							Sensor_Page();
							break;
						case 7:
							General_Set_Page();
							break;
						default:
							break;
				  }
						
					
					Send_Command(title_clear);
					Menu_Select(0,0);
					Menu_Select(1,0);
					Menu_Select(2,0);
					Menu_Select(3,0);
					Menu_Select(4,0);
					Menu_Select(5,0);
					Menu_Select(6,0);
					Menu_Select(7,0);
					Menu_Select(8,0);
					Menu_Select(9,0);
					Menu_Select(select_num,1);
					Send_Command(title_display_on);
					Key_Value=0;
			  }
				else if(select_num==8)
				{
						factory_reset_flag=1;
						Menu_Select(8,2);
						Display_Data_Command(8,0x30,0x1b,0x1b,0x1b,char_C,char_O,char_N,char_F,char_I,char_R,char_M);
						Send_Command(title_display_on);
				}
			  else				
			  {
			  	Send_Command(title_clear);
					MENU_KEY_FLAG=0;
				  select_num=15; //ÍË³öwhileÑ­»·±êÖ¾
				  Key_Value=0;
			  }
			  break;
			default :
			  break;
		} 
	}
	delay_ms(200);
}  

