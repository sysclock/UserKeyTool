#include "screen-set.h"
#include "display.h"
#include "usart.h"

u8 user_select_screen=0;
u32 g_TFT_total_size_X=594;
u32 g_TFT_total_size_Y=330;
u32 g_TFT_begin_X=183;
u32 g_TFT_begin_Y=130;


u8 g_stitching_screen_line=2;
u8 g_stitching_screen_column=2;
u8 g_screen_line=2;
u8 g_screen_column=2;
u16 d_screen_total_X=10000;
u16 d_screen_total_Y=10000;

u8 matrix_stitching_flag=STITCHING_MODE;
//定义

u16 g_stitching_screen_line_column_pixel[2][8]={1920,1920,1920,1920,1920,1920,1920,1920,
																			1080,1080,1080,1080,1080,1080,1080,1080};

u16 g_screen_line_column_pixel[2][8]={1920,1920,1920,1920,1920,1920,1920,1920,
																			1080,1080,1080,1080,1080,1080,1080,1080};
u16 g_screen_line_column_pixel_input[2][8];
u16 g_screen_total_pixel[2][8];
																		
u16 g_TFT_X_point[8]={0};
u16 g_TFT_Y_point[8]={0};



//最多有8个输出口，在TFT屏上也最多有8个画面，每个包括起始4字节
//结束位置4字节，颜色值两个字节，所以定义以下数组。
u16 g_TFT_display_para[8][5]={0};

void Calculation_total_point(void)
{
	u8 i;
	for(i=0;i<g_screen_column;i++)
	{
		if(i==0)
			g_screen_total_pixel[0][0]=g_screen_line_column_pixel[0][0];
		else
			g_screen_total_pixel[0][i]=g_screen_total_pixel[0][i-1]+g_screen_line_column_pixel[0][i];		
	}
	d_screen_total_X=g_screen_total_pixel[0][i-1];
	for(i=0;i<g_screen_line;i++)
	{
		if(i==0)
			g_screen_total_pixel[1][0]=g_screen_line_column_pixel[1][0];
		else
			g_screen_total_pixel[1][i]=g_screen_total_pixel[1][i-1]+g_screen_line_column_pixel[1][i];
	}
	d_screen_total_Y=g_screen_total_pixel[1][i-1];
}






void display_TFT_set(void)
{
	u8 line,col;
	u16 point_X,point_Y=0;
	u16 point_end_X,point_end_Y=0;
	point_X=(g_TFT_total_size_X-(g_screen_column*2))/g_screen_column;
	point_Y=(g_TFT_total_size_Y-(g_screen_line*2))/g_screen_line;
	
	for(col=0;col<g_screen_column;col++)
	{
		g_TFT_X_point[col]=g_screen_line_column_pixel[0][col]*point_X/1920;
	}	
	for(line=0;line<g_screen_line;line++)
	{
		g_TFT_Y_point[line]=g_screen_line_column_pixel[1][line]*point_Y/1080;
	}	
	
	point_end_Y=130;
	for(line=0;line<g_screen_line;line++)
	{	
		point_end_X=183;
		
		for(col=0;col<g_screen_column;col++)
		{
			g_TFT_display_para[line*g_screen_column+col][0]=point_end_X;
			g_TFT_display_para[line*g_screen_column+col][2]=point_end_X+g_TFT_X_point[col];	
			point_end_X=point_end_X+g_TFT_X_point[col]+2;		
			
			g_TFT_display_para[line*g_screen_column+col][1]=point_end_Y;			
			g_TFT_display_para[line*g_screen_column+col][3]=point_end_Y+g_TFT_Y_point[line];
			g_TFT_display_para[line*g_screen_column+col][4]=0xffff;
		}
		point_end_Y=point_end_Y+g_TFT_Y_point[line]+2;	
	}	
	
}


u8 test[100];
void Send_TFT_Screen_Para(void)
{
	u8 i=0;
	u8 j=0;
	test[0]=0xa5;
	test[1]=0x5a;
	test[3]=0x82;
	test[4]=0x20;
	test[5]=0x00;
	
	test[6]=0x00;
	test[7]=0x04;
	
	test[8]=0x00;
	test[9]=g_screen_line* g_screen_column;
	
	for(i=0;i<test[9];i++)
	{
		for(j=0;j<5;j++)
		{
			test[10+i*10+j*2]=g_TFT_display_para[i][j]>>8;
			test[11+i*10+j*2]=g_TFT_display_para[i][j];
		}
	}
	test[18+user_select_screen*10]=0xf8;
	test[19+user_select_screen*10]=0x00;
	test[2]=7+test[9]*10;
	
	UART4_SendBuf(test,test[2]+3);
}



