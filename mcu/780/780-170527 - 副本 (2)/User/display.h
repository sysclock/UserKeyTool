#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "stm32f10x.h"

typedef struct {
	unsigned short resolution_width;
	unsigned short resolution_high;	
}Resolution_Data;


//#define IN_PORT_NUM  8
#define IN_PORT_NUM 8
#define OUT_PORT_NUM 8


//保存方案的数量
#define SCHEME_NUM 9

//串口指令的发送长度-单位字符
#define SEND_LEN 100

#define MATRIX_MODE 1
#define SPLIT_MODE 2


#define IN_BOARD_SCALER_NUM 4

#define INPUT_BOARD0  0
#define INPUT_BOARD1  1
#define OUTPUT_BOARD2  2
#define OUTPUT_BOARD3  3


#define BEGIN_X_INDEX 0
#define BEGIN_Y_INDEX 1
#define END_X_INDEX 2
#define END_Y_INDEX 3
#define WIDTH_INDEX 4
#define HIGH_INDEX 5
#define SOURCE_IN_INDEX 6
#define LAYER_INDEX 7
#define DEAL_INDEX 8
#define IMAGE_PARA_NUM 9


//INPUT_FPGA SCALER_SETUP [BoardIndex] [ScalerIndex] [SrcIdx] [InHoriSize] 
//[InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]
#define BoardIndex 0
#define ScalerIndex 1
#define SrcIdx 2
#define InHoriSize 3
#define InVertSize 4
#define InFirstPixel 5
#define InLastPixel 6
#define InFirstLine 7
#define InLastLine 8
#define OutHoriSize 9
#define OutVertSize 10
#define IF_SCALER_PARA_NUM 11


//INPUT_FPGA ADD_WINDOWS [AW_SourceIdx] [Layer] [InLeft] [InTop] [InRight] [InBottom] 
//[DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
//OUTPUT_FPGA ADD_WINDOWS [AW_SourceIdx] [Layer] [InLeft] [InTop] [InRight] [InBottom] [DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
#define AW_SourceIdx 0
#define Layer 1
#define InLeft 2
#define InTop 3
#define InRight 4
#define InBottom 5
#define DisplayLeft 6
#define DisplayTop 7
#define DisplayRight 8
#define DisplayBottom 9
#define ADD_WIN_PARA_NUM 10


//#define 

extern u8 scheme_save[SCHEME_NUM];
extern u8 matrix_stitching_flag;

extern u8 gMatInpNo;
extern u8 gMatOutpNo;

//===2017,Jun 15 ; 9:47:52  ;BY YANG@BYCOX.COM===
//support in ports from 4-->8
extern u16 gMatImgPara[IN_PORT_NUM][IMAGE_PARA_NUM];


extern char gCmdBuf[SEND_LEN];
extern u8 g_Image_Scr;

extern u16 gInImgPara[IMAGE_PARA_NUM];
extern u16 gSplitImgPara[IN_PORT_NUM][IMAGE_PARA_NUM];
extern u16 gImgPara[IN_PORT_NUM][IMAGE_PARA_NUM]; 
extern Resolution_Data InSrcRes[8];

void save_scenario_to_FPGA(u8 num);
u8 load_scenario_from_FPGA(u8 num);
void READ_INPUT_SOURCE_RESOLUTION(void); //读取输入分辨率
void command_send(char *buf);
void Clear_Display( u8 num);
void TryEnd_Clear_Display(void);
void Read_TCPIP(void) ;

//Add_New_Image(),Matrix_Display_Function-->image_layout_Protocal_Prara_Process-->OutFPGA_Scaler_Para_Process,IF_directTo_OF_AddWin_Para_Process
void image_layout_Protocal_Prara_Process(void);
void OutFPGA_Scaler_Para_Process(u8 port,u8 port_num ,u8 num);
void IF_directTo_OF_AddWin_Para_Process(u8 port,u8 port_num ,u8 num) ;
void InFPGA_Scaler_Para_Process(u8 num);
u8 same_output_zoom(u8 port,u8 num,u8 image_num);
u8 howMany_Scaler_zoom_need(u16 num);
void Output_Set_Protocal_Cmd_Send2FPGA(void);
void OutFPGA_AddWin_Para_Process (u8 input_port_num,u8 layout_num,u8 num,u16 begin_x,u16 begin_y,u16 end_x,u16 end_y);


u8 same_input_zoom(u8 num);


#endif
