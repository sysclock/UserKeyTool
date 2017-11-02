#ifndef __SCREEN_SET_H
#define __SCREEN_SET_H

#include "stm32f10x.h"

extern u16 g_TFT_X_point[8];
extern u16 g_TFT_Y_point[8];
extern u8 user_select_screen;
extern u8 g_screen_line;
extern u8 g_screen_column;

extern u8 g_stitching_screen_line;
extern u8 g_stitching_screen_column;

extern u16 d_screen_total_X;
extern u16 d_screen_total_Y;

extern u16 g_screen_line_column_pixel[2][8];
extern u16 g_stitching_screen_line_column_pixel[2][8];
extern u16 g_screen_line_column_pixel_input[2][8];
extern u16 g_screen_total_pixel[2][8];
void Calculation_total_point(void);


u8 Calculation_OutPut_Resolution(u8 port_num);
void display_TFT_set(void);
void Send_TFT_Screen_Para(void);

#endif
