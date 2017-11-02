#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>

extern u8 power_onoff_flag;
extern u8 TEXT_Buffer[];
extern u8 work_para[];
void turn_on_display(void);
void initGlobalVar(void);
void factory_reset(void);
void Menu_Page(void);
void Display_Data_Command(u8 x,u8 y,u8 num0 ,u8 num1,
						  u8 num2,u8 num3,u8 num4,
						  u8 num5,u8 num6, u8 num7,
						  u8 num8,u8 num9);
void Set_Command(u8 command_num,u8 num0 ,u8 num1,u8 num2,u8 num3,u8 num4,
				   u8 num5,u8 num6, u8 num7,u8 num8,u8 num9);


void Privacy_Select(u8 x, u8 select_flag);
void display_privacy_area_off(char num);
void display_privacy_area_on(char num);

void Send_Command(u8 *send_data);
void Zoom_Irq_Function(void);
void scene_data_send(u8 run_num);
void MANU_WB_SET(void);




extern u8 focus_mode_set[6];
extern u8	format;
extern u8 wb_mode_set[6];
extern u8 title_clear[6];
extern u8 title_display_on[6];
extern u8	focus_mode;
extern u8 power_off[6];
extern u8 power_on[6];
extern u8 wb_mode;
extern u8 focusadd_command[6];
extern u8 focussub_command[6];
extern u8 focusstop_command[6];
extern u8 zoomadd_command[6];
extern u8 zoomsub_command[6];
extern u8 zoomstop_command[6];
extern u8 one_push_trigger[6];
extern const u8 display_num[10];
extern u8	baudrate;
extern u8	address;
extern u8 scene_num;
extern u8 scene_data[4][2];  //time_s  exist

#define char_A 0x00
#define char_B 0x01
#define char_C 0x02
#define char_D 0x03
#define char_E 0x04
#define char_F 0x05
#define char_G 0x06
#define char_H 0x07
#define char_I 0x08
#define char_J 0x09
#define char_K 0x0A
#define char_L 0x0B
#define char_M 0x0C
#define char_N 0x0D
#define char_O 0x0E
#define char_P 0x0F
#define char_Q 0x10
#define char_R 0x11
#define char_S 0x12
#define char_T 0x13
#define char_U 0x14
#define char_V 0x15
#define char_W 0x16
#define char_X 0x17
#define char_Y 0x18
#define char_Z 0x19
#define char_space 0x1b

#define num_0 0x27
#define num_1 0x1e
#define num_2 0x1f
#define num_3 0x20
#define num_4 0x21
#define num_5 0x22
#define num_6 0x23
#define num_7 0x24
#define num_8 0x25
#define num_9 0x26


//   * 硬件连接  菜单-PB9	   KEY1
//   			  确认-PA15	   KEY2
// 			  场景-PB3	   KEY3
// 			  电源-PA11	   KEY4
// 			  FOCUS+  -PB7	 KEY5
// 			  FOCUS-  -PB8	 KEY6
// 			  ZOOM+	  -PB5	 KEY7
// 			  ZOOM-   -PB6	 KEY8
#define MENU	1
#define	ENTER 9
#define SCENE 3
#define POWER 4
#define FOCUS_ADD 5
#define FOCUS_SUBTRACT 6
#define ZOOM_ADD 7
#define ZOOM_SUBTRACT 8

#define WB 2

#define UP  6
#define DOWN 5
#define ADD  7
#define SUBTRACT 8

#define sene_0  0
#define sene_1  1
#define sene_2  2
#define sene_3  3



//恢复出厂的默认值
#define def_zoom_speed  		3
#define def_zoom_data				1
#define  def_dig_zoom				1
#define  def_osd						1
#define  def_zoom_display 	0
#define 	def_focus_mode    0
#define 	def_af_sens    		0
#define 	def_focus_light 	0
#define 	def_near_limit		14	
#define  def_aperture 			4
#define  def_color_gain			4
#define  def_color_hum			7
#define  def_chroma_suppress	2
#define  def_gamma						0
#define  def_mirror					1
#define  def_flip						1
#define  def_freeze					1
#define  def_pic_effect			0
#define  def_wb_mode     		0
#define  def_red_gain 			202
#define  def_blue_gain 			175
#define 	def_ae_mode 			0	
#define 	def_shutter			  5	
#define 	def_iris 					9
#define 	def_gain					9
#define 	def_bri						9
#define 	def_slow_ae 			1
#define 	def_slow_shutter  1
#define 	def_blc						1
#define 	def_wdr						3
#define 	def_nr_level			3
#define 	def_stabilizer		1
#define 	def_defog_mode		1
#define 	def_hi_resolution	1
#define 	def_hi_sensitivity	1
#define 	def_format 				0
#define 	def_LR_hant 			1
#define 	def_baudrate 			2
#define 	def_address				1
#define 	def_aux						1
#define  def_scene_num			0
#define  def_scene_data    10,0,10,0,10,0,10,0      //time_s  exist
#define  def_zone						0
#define  def_privacy_data 0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5





#define 	def1_zoom_speed  		3
#define   def1_zoom_data			1
#define 	def1_focus_mode    0
#define 	def1_af_sens    		0
#define 	def1_focus_light 	0
#define 	def1_near_limit		14	
#define  def1_aperture 			4
#define  def1_color_gain			4
#define  def1_color_hum			7
#define  def1_chroma_suppress	2
#define  def1_gamma						0
#define  def1_mirror					1
#define  def1_flip						1
#define  def1_freeze					1
#define  def1_pic_effect			0
#define  def1_wb_mode     		0
#define  def1_red_gain 			202
#define  def1_blue_gain 			175
#define 	def1_ae_mode 			0	
#define 	def1_shutter			  5	
#define 	def1_iris 					9
#define 	def1_gain					9
#define 	def1_bri						9
#define 	def1_slow_ae 			1
#define 	def1_slow_shutter  1
#define 	def1_blc						1
#define 	def1_wdr						3
#define 	def1_nr_level			3
#define 	def1_stabilizer		1
#define 	def1_defog_mode		1
#define 	def1_hi_resolution	1
#define 	def1_hi_sensitivity	1
#define 	def1_format 				0
#define  def1_zone						0
#define  def1_privacy_data 0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5


#define def2_zoom_speed  		3
#define def2_zoom_data 			5
#define 	def2_focus_mode    0
#define 	def2_af_sens    		0
#define 	def2_focus_light 	0
#define 	def2_near_limit		14	
#define  def2_aperture 			4
#define  def2_color_gain			4
#define  def2_color_hum			7
#define  def2_chroma_suppress	2
#define  def2_gamma						0
#define  def2_mirror					1
#define  def2_flip						1
#define  def2_freeze					1
#define  def2_pic_effect			0
#define  def2_wb_mode     		0
#define  def2_red_gain 			202
#define  def2_blue_gain 			175
#define 	def2_ae_mode 			0	
#define 	def2_shutter			  5	
#define 	def2_iris 					9
#define 	def2_gain					9
#define 	def2_bri						9
#define 	def2_slow_ae 			1
#define 	def2_slow_shutter  1
#define 	def2_blc						1
#define 	def2_wdr						3
#define 	def2_nr_level			3
#define 	def2_stabilizer		1
#define 	def2_defog_mode		1
#define 	def2_hi_resolution	1
#define 	def2_hi_sensitivity	1
#define 	def2_format 				0
#define  def2_zone						0
#define  def2_privacy_data 0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5


#define def3_zoom_speed  		3
#define def3_zoom_data 			10
#define 	def3_focus_mode    0
#define 	def3_af_sens    		0
#define 	def3_focus_light 	0
#define 	def3_near_limit	 14
#define  def3_aperture 			4
#define  def3_color_gain			4
#define  def3_color_hum			7
#define  def3_chroma_suppress	2
#define  def3_gamma						0
#define  def3_mirror					1
#define  def3_flip						1
#define  def3_freeze					1
#define  def3_pic_effect			0
#define  def3_wb_mode     		0
#define  def3_red_gain 			202
#define  def3_blue_gain 			175
#define 	def3_ae_mode 			0	
#define 	def3_shutter			  5	
#define 	def3_iris 					9
#define 	def3_gain					9
#define 	def3_bri						9
#define 	def3_slow_ae 			1
#define 	def3_slow_shutter  1
#define 	def3_blc						1
#define 	def3_wdr						3
#define 	def3_nr_level			3
#define 	def3_stabilizer		1
#define 	def3_defog_mode		1
#define 	def3_hi_resolution	1
#define 	def3_hi_sensitivity	1
#define 	def3_format 				0
#define  def3_zone						0
#define  def3_privacy_data 0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5,0x01,20,20,5,5




typedef enum
{
	TITLE_LINE_0=0,
	TITLE_LINE_1 ,
	TITLE_LINE_2,
	TITLE_LINE_3,
	TITLE_LINE_4,
	TITLE_LINE_5,
	TITLE_LINE_6,
	TITLE_LINE_7,
	TITLE_LINE_8,
	TITLE_LINE_9,
	TITLE_LINE_10,
}_Title_line_Type;


typedef enum
{
	FRONT=0x20,
	BACK=0x30
}_Title_column_Type;




#endif

