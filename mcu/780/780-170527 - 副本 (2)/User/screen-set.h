#ifndef __SCREEN_SET_H
#define __SCREEN_SET_H

#include "stm32f10x.h"


#define MAX_OUT_PORTS 8

//extern u16 g_TFT_X_point[8];
//extern u16 g_TFT_Y_point[8];
extern u8 user_select_screen;
extern u8 gScrRow;
extern u8 gScrCol;

extern u8 g_stitching_screen_line;
extern u8 g_stitching_screen_column;

extern u16 gScrTotX;
extern u16 gScrTotY;

extern u16 g_screen_line_column_pixel[2][8];
extern u16 gSplitScrRCPix[2][8];
extern u16 gUserInScrRCPix[2][8];
extern u16 gScrTotPix[2][8];
void Cal_Total_Point(void);


u8 Calculation_OutPut_Resolution(u8 port_num);
//void display_TFT_set(void);
//void Send_TFT_Screen_Para(void);

//��������˿ڣ�Ŀǰ֧�ֵ�ɫ
void OutPort_TestPatternColor(void);
#define MAINOUTBOARDID 3
#define SECOUTBOARDID 2

void Matrix_Display_Function(void);
void ScreenSet_Display(void);
void Add_New_Image(void);
void DelScheme_OK(void);
void Save_Scenario_OK(void);
void WizardSet_OK_Function(void);
void set_lcd_backLight(u8 data);

#ifdef DEBUG88
U8 lcd_disp_debug_info(U8 x,U8 y,char *dispStr);
#endif



#endif
