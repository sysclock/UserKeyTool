#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "stm32f10x.h"

typedef struct {
	unsigned short resolution_width;
	unsigned short resolution_high;	
}Resolution_Data;


#define IMAGE_TOTAL_NUM  8
#define SEND_LEN 100

#define MATRIX_MODE 1
#define STITCHING_MODE 2


extern u8 scheme_save[9];
extern u8 matrix_stitching_flag;

extern u8 d_matrix_inputsource_num;
extern u8 d_matrix_outputsource_num;
extern u16 d_matrix_image_para[4][9];


extern char d_780command_send_buf[SEND_LEN];
extern u16 g_input_imagepara[9];

extern u8 g_Image_Scr;


extern u16 d_stitching_image_para[IMAGE_TOTAL_NUM][9];
extern u16 d_image_para[IMAGE_TOTAL_NUM][9]; 
extern Resolution_Data input_source_resolution[8];
void image_layout(void);
void save_scenario_to_FPGA(u8 num);
u8 load_scenario_from_FPGA(u8 num);
void input_RISR(void); //读取输入分辨率
void command_send(char *buf);
void Clear_Output( u8 num);
void TryEnd_Clear_Display(void);
void Read_TCPIP(void) ;
#endif
