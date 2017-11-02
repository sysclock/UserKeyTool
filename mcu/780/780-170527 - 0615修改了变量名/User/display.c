/*


1,set input image display para
2,set output board display para
3,set layout para


*/

#include "DISPLAY.h"
#include "USART.h"
#include "screen-set.h"
#include "delay.h"
#include "string.h"
#include <stdlib.h>





//Resolution_Data InSrcRes[8]=
//{
//	{1920,1080},
//	{1920,1080},
//	{1920,1080},
//	{1920,1080},
//	{1920,1080},
//	{1920,1080},
//	{1920,1080},
//	{1920,1080}
//};


Resolution_Data InSrcRes[MAX_IN_PORT]=
{
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0}
};


 //定义一个数组，存放共IMAGE_TOTAL_NUM个画面的参数
 //参数包括显示的起始位位置、结束位置、宽度、高度和信号源（外部输入口0~3）、图层号（0号图层表示不可用）、是否分配资源


u8 scheme_save[9]={0};

u8 g_Mat_InPort_No;
u8 g_Mat_OutPort_No;
u16 gMatImgPara[4][IMAGE_PARA_NUM];


u16 gInImgPara[IMAGE_PARA_NUM];

u16 gSplitImgPara[IMAGE_TOTAL_NUM][IMAGE_PARA_NUM];

//总的画面数量--每个画面的参数 起点终点水平垂直源头层数
//#define BEGIN_X_INDEX 0
//#define BEGIN_Y_INDEX 1
//#define END_X_INDEX 2
//#define END_Y_INDEX 3
//#define WIDTH_INDEX 4
//#define HIGH_INDEX 5
//#define SOURCE_IN_INDEX 6
//#define LAYER_INDEX 7
//#define DEAL_INDEX 8
//#define IMAGE_PARA_NUM 9
u16 gImgPara[IMAGE_TOTAL_NUM][IMAGE_PARA_NUM]={0}; 


//Change the name d_780command_send_buf -->gCmdBuf
char gCmdBuf[SEND_LEN];


//===2017,Jun 14 ; 8:14:16  ;BY YANG@BYCOX.COM===
//change the function name from command_send() to Uart_Send_Cmd_to_FPGA()
void Uart_Send_Cmd_to_FPGA(char *buf)
{
	 u8 i=0;
   while (1)
   {  if (buf[i]!='\0')
     { 
				USART_SendData(USART2, buf[i]);  //发送一个byte到串口
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}; //等待发送结束
        i++;
     }
     else 
		 {
			 delay_ms(20);
			 return;
		 }
   }
}



//
int my_atoi (char *s)
{
	int i=0;
	int num=0;
	while(s[i]!='\0'&& s[i]!=0x0D)
	{
		num=num*10+(s[i]-'0');
		i++;
	}
	return num;
}


//Read-ip-address
void Read_TCPIP(void) //读取IP信息
{
	int IP[4]={0};
	int IP_Port=0;
	char sendtest[SEND_LEN];
	char  *check=NULL;
	g_Read_Command_Flag=1;
	g_RevUart2Idx=0;
	
	for(IP_Port=0;IP_Port<1024;IP_Port++)
	{
		g_RevUart2Data[IP_Port]=0;
	}
	Uart_Send_Cmd_to_FPGA("GP GIPA 1\r\n");
	delay_ms(20);
	Uart_Send_Cmd_to_FPGA("GP GTPN 1\r\n");
	delay_ms(400);
	check=strtok(g_RevUart2Data," ");
	while(check!=NULL)
	{
		if(!strcmp(check,"GIPA"))
		{
			check=strtok(NULL," ");
			IP[0]=my_atoi(check);	
			check=strtok(NULL," ");	
			IP[1]=my_atoi(check);	
			check=strtok(NULL," ");
			IP[2]=my_atoi(check);
			check=strtok(NULL," ");
			IP[3]=my_atoi(check);
		}
		else if(!strcmp(check,"GTPN"))
		{
			check=strtok(NULL," ");
			IP_Port=my_atoi(check);	
		}
		check=strtok(NULL," ");
	}
	g_Read_Command_Flag=0;
	g_RevUart2Idx=0;	
	sprintf(sendtest,"SBC(2);DS16(90,70,'%d.%d.%d.%d',0);\r\n",IP[0],IP[1],IP[2],IP[3]);
	GpuSend(sendtest);	
	sprintf(sendtest,"DS16(90,100,'%d',0);SBC(7);\r\n",IP_Port);
	GpuSend(sendtest);	
}



//INPUT_FPGA READ_INPUT_SOURCE_RESOLUTION 
//read input source resolution
void input_RISR(void) //读取输入分辨率
{
	int input_board=0;
	int input_board_port=0;
	int input_resolution_width=0;
	int input_resolution_high=0;
	char sendtest[SEND_LEN];
	char  *check=NULL;
	g_Read_Command_Flag=1;
	g_RevUart2Idx=0;
	Uart_Send_Cmd_to_FPGA("I_F RISR 0 0\r\n");
//	delay_ms(20);
	Uart_Send_Cmd_to_FPGA("I_F RISR 0 1\r\n");
//	delay_ms(20);
	Uart_Send_Cmd_to_FPGA("I_F RISR 0 2\r\n");
//	delay_ms(20);
	Uart_Send_Cmd_to_FPGA("I_F RISR 0 3\r\n");
//	delay_ms(20);
//	Uart_Send_Cmd_to_FPGA("I_F RISR 1 0\r\n");
////	delay_ms(20);
//	Uart_Send_Cmd_to_FPGA("I_F RISR 1 1\r\n");
////	delay_ms(20);
//	Uart_Send_Cmd_to_FPGA("I_F RISR 1 2\r\n");
////	delay_ms(20);
//	Uart_Send_Cmd_to_FPGA("I_F RISR 1 3\r\n");
	delay_ms(200);
//	sprintf(gCmdBuf,"INPUT_FPGA READ_INPUT_SOURCE_RESOLUTION 1810 1080 0 0 1920 1080 2 5818\r\nINPUT_FPGA READ_INPUT_SOURCE_RESOLUTION 0 0 0 0 1920 1080 2 5818\r\n");
	check=strtok(g_RevUart2Data," ");
	while(check!=NULL)
	{
		if(!strcmp(check,"RISR") || !strcmp(check,"READ_INPUT_SOURCE_RESOLUTION"))
		{
			check=strtok(NULL," ");
			input_board=my_atoi(check);
	
			check=strtok(NULL," ");	
			input_board_port=my_atoi(check);
			
			check=strtok(NULL," ");
			check=strtok(NULL," ");
			
			check=strtok(NULL," ");
			input_resolution_width=my_atoi(check);
			
			check=strtok(NULL," ");
			input_resolution_high=my_atoi(check);
			if(input_board>1)input_board=0;
			if(input_board_port>3)input_board_port=0;
			if(input_resolution_width>1920)input_resolution_width=1920;
			if(input_resolution_high>1080) input_resolution_high=1080;
			InSrcRes[input_board*4+input_board_port].resolution_width=input_resolution_width;
			InSrcRes[input_board*4+input_board_port].resolution_high=input_resolution_high;
		}
		check=strtok(NULL," ");
	}
	
	for(input_board=0; input_board<4 ; input_board++)
	{
		if(InSrcRes[input_board].resolution_width!=0)
		{
			g_Image_Scr=input_board+1;
			break;
		}
	}

	g_Read_Command_Flag=0;
	g_RevUart2Idx=0;
}


//定义了输入板的四个缩放器的使用情况和使用时的�
//INPUT_FPGA SCALER_SETUP [BoardIndex] [ScalerIndex] [SourceIndex] [InHoriSize] 
//[InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]�
u16 IF_Scaler_Para[IN_BOARD_SCALER_NUM][IF_SCALER_PARA_NUM]={0};

//			OUTPUT_FPGA SCALER_SETUP [OutputSlotIndex] [ScalerIndex] [SourceIndex] [InHoriSize] [InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]
//			OutputSlotIndex: 输出板卡索引号，取值0-3
//			ScalerIndex:输出卡缩放器索引号，取值0-3。输出板有4个缩放器。
//			SourceIndex：信号源索引号，取值0-3。输出板原始输入信号。
//			InHoriSize：缩放器输入图像的长度，如分辨率为1920x1080的信号，InHoriSize为1920。
//			InVertSize：缩放器输入图像的高度，如分辨率为1920x1080的信号，InVertSize为1080。
//			InFirstPixel：缩放器取输入图像X起始位置，取值0- （InHoriSize-1）
//			InLastPixel：缩放器取输入图像X结束位置，取值0- （InHoriSize-1）
//			InFirstLine：缩放器取输入图像Y起始位置，取值0- （InVertSize -1）
//			InLastLine：缩放器取输入图像Y结束位置，取值0- （InVertSize -1）
//			OutHoriSize: 缩放器输出图像的长度
//			OutVertSize：缩放器输出图像的高度
//输出板子缩放器参数	
//sprintf(gCmdBuf,"O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",OF_Scaler_Para[i][0],OF_Scaler_Para[i][1],OF_Scaler_Para[i][2],OF_Scaler_Para[i][3],OF_Scaler_Para[i][4],OF_Scaler_Para[i][5],OF_Scaler_Para[i][6],OF_Scaler_Para[i][7],OF_Scaler_Para[i][8],OF_Scaler_Para[i][9],OF_Scaler_Para[i][10]);


u16 OF_Scaler_Para[IN_BOARD_SCALER_NUM][IF_SCALER_PARA_NUM]={0};

//定义输入板的两个布局所要布局的画面参数
//INPUT_FPGA ADD_WINDOWS [SourceIndex] [Layer] [InLeft] [InTop] [InRight] [InBottom] 
//[DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
//INPUT_FPGA SET_WINDOWS_LAYOUT [BoardIndex] [BackgroundIndex] [ScreenWidth][ScreenHeight]
u16 IF_AW_BG0_Para[8][ADD_WIN_PARA_NUM];//[10];
u16 IF_AW_BG1_Para[8][ADD_WIN_PARA_NUM];
u8 IF_AW_cnt0=0;
u8 IF_AW_cnt1=0;


u8 inBoardUsedScalerNum=0;  //
u8 OutBoardUsedScalerNum=0;
u8 same_input_zoom(u8 num)
{
	u8 i;
	
	for(i=0;i<inBoardUsedScalerNum;i++)
	{
		if(IF_Scaler_Para[i][2]==gImgPara[num][6]
				&&IF_Scaler_Para[i][9]==gImgPara[num][4]
				&&IF_Scaler_Para[i][10]==gImgPara[num][5])
		{
			return 1;
		}
	}
	return 0;
}




//
u8 same_output_zoom(u8 port,u8 num,u8 image_num)
{
	u8 i;
	
	for(i=0;i<OutBoardUsedScalerNum;i++)
	{
			if(OF_Scaler_Para[i][2]==port)
			{
				
				if(port==1 &&OF_Scaler_Para[i][5]==IF_AW_BG0_Para[num][DisplayLeft]
					&&OF_Scaler_Para[i][6]==IF_AW_BG0_Para[num][DisplayTop]
					&&OF_Scaler_Para[i][7]==IF_AW_BG0_Para[num][DisplayRight]
					&&OF_Scaler_Para[i][8]==IF_AW_BG0_Para[num][DisplayBottom]
					&&OF_Scaler_Para[i][9]==gImgPara[image_num][4]
					&&OF_Scaler_Para[i][10]==gImgPara[image_num][5])
				{
					return i;
				}
				else if(port==2 &&OF_Scaler_Para[i][5]==IF_AW_BG1_Para[num][DisplayLeft]
					&&OF_Scaler_Para[i][6]==IF_AW_BG1_Para[num][DisplayTop]
					&&OF_Scaler_Para[i][7]==IF_AW_BG1_Para[num][DisplayRight]
					&&OF_Scaler_Para[i][8]==IF_AW_BG1_Para[num][DisplayBottom]
					&&OF_Scaler_Para[i][9]==gImgPara[image_num][4]
					&&OF_Scaler_Para[i][10]==gImgPara[image_num][5])
				{
					return i;
				}
			}
	}
	return 0;
}

u16 layout_port0_X=0;
u16 layout_port0_X_flag=0;
u16 layout_port0_Y=0;
u16 layout_port0_Y_pre=0;
u16 layout_port1_X=0;
u16 layout_port1_X_flag=0;
u16 layout_port1_Y=0;
u16 layout_port1_Y_pre=0;


//这个函数什么用途
u8 inputboard_out_layout(u8 zoom_num,u8 first_use)
{
	if(first_use==1)
	{	
		if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+IF_Scaler_Para[zoom_num][9])<=1920&& (layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10])<=2400)  //布局器1横向可以进行
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]=zoom_num+4;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][2]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG0_Para[IF_AW_cnt0][6]=layout_port0_X;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y_pre;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=layout_port0_X+IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10];
			if(!layout_port0_X_flag)
			{
				layout_port0_X+=IF_Scaler_Para[zoom_num][9];
				layout_port0_X_flag=1;
				layout_port0_Y=layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10];	
			}
			else
			{
				layout_port0_Y_pre+=IF_Scaler_Para[zoom_num][10];
				if(layout_port0_Y_pre>=layout_port0_Y)
					layout_port0_Y=layout_port0_Y_pre;
			}
			
			IF_AW_cnt0++;
			return 1; 
		}
		else if((IF_Scaler_Para[zoom_num][9])<=1920&& (layout_port0_Y+IF_Scaler_Para[zoom_num][10])<=2400)  //布局器1纵向可以进行
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]= zoom_num+4;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][2]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG0_Para[IF_AW_cnt0][6]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y+IF_Scaler_Para[zoom_num][10];
			layout_port0_Y_pre=layout_port0_Y;
			
			layout_port0_X=IF_Scaler_Para[zoom_num][9];
			layout_port0_X_flag=0;
			
			layout_port0_Y=layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10];	
			IF_AW_cnt0++;
			return 1;
		}	
		else if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_Y_pre<layout_port1_Y)&&(layout_port1_X+IF_Scaler_Para[zoom_num][9])<=1920&&(layout_port1_Y_pre+IF_Scaler_Para[zoom_num][10])<=2400)  //布局器2横向可以进行
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num+4;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt1+1;
			IF_AW_BG1_Para[IF_AW_cnt1][2]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG1_Para[IF_AW_cnt1][6]=layout_port1_X;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y_pre;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=layout_port1_X+IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y_pre+IF_Scaler_Para[zoom_num][10];
			if(!layout_port1_X_flag)
			{
				layout_port1_X+=IF_Scaler_Para[zoom_num][9];
				layout_port1_X_flag=1;
				layout_port1_Y=layout_port1_Y_pre+IF_Scaler_Para[zoom_num][10];	
			}
			else
			{
				layout_port1_Y_pre+=IF_Scaler_Para[zoom_num][10];
				if(layout_port1_Y_pre>=layout_port1_Y)
					layout_port1_Y=layout_port1_Y_pre;				
			}
			
			IF_AW_cnt1++;
			return 2;
		}
		else if((IF_Scaler_Para[zoom_num][9])<=1920&&(layout_port1_Y+IF_Scaler_Para[zoom_num][10])<=2400)  //布局器2纵向可以进行
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num+4;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt0+1;
			IF_AW_BG1_Para[IF_AW_cnt1][2]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG1_Para[IF_AW_cnt1][6]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y+IF_Scaler_Para[zoom_num][10];
			layout_port1_Y_pre=layout_port1_Y;
			layout_port1_X=IF_Scaler_Para[zoom_num][9];
			layout_port1_X_flag=0;
			layout_port1_Y=layout_port1_Y_pre+IF_Scaler_Para[zoom_num][10];	
			IF_AW_cnt1++;
			return 2;
		}
		
	}
	else if(first_use==2)
	{
		
		if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_X+InSrcRes[zoom_num].resolution_width)<=1920&&(layout_port1_Y_pre+InSrcRes[zoom_num].resolution_high)<=2400)  //布局器2横向可以进行
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt1+1;
			IF_AW_BG1_Para[IF_AW_cnt1][2]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG1_Para[IF_AW_cnt1][5]=InSrcRes[zoom_num].resolution_high;
			
			IF_AW_BG1_Para[IF_AW_cnt1][6]=layout_port1_X;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y_pre;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=layout_port1_X+InSrcRes[zoom_num].resolution_width;
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y_pre+InSrcRes[zoom_num].resolution_high;
			if(!layout_port1_X_flag)
			{
				layout_port1_X+=InSrcRes[zoom_num].resolution_width;
				layout_port1_X_flag=1;
				layout_port1_Y=layout_port1_Y_pre+InSrcRes[zoom_num].resolution_high;
			}
			else
			{
				layout_port1_Y_pre+=InSrcRes[zoom_num].resolution_high;
				if(layout_port1_Y_pre>=layout_port1_Y)
					layout_port1_Y=layout_port1_Y_pre;	
			}						
			IF_AW_cnt1++;
			return 2;
		}
		else if((InSrcRes[zoom_num].resolution_width)<=1920&&(layout_port1_Y+InSrcRes[zoom_num].resolution_high)<=2400)  //布局器2纵向可以进行
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt0+1;
			IF_AW_BG1_Para[IF_AW_cnt1][2]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG1_Para[IF_AW_cnt1][5]=InSrcRes[zoom_num].resolution_high;
			
			IF_AW_BG1_Para[IF_AW_cnt1][6]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y+InSrcRes[zoom_num].resolution_high;
			layout_port1_Y_pre=layout_port1_Y;
			
			layout_port0_X=InSrcRes[zoom_num].resolution_width;
			layout_port0_X_flag=0;
			layout_port1_Y=layout_port1_Y_pre+InSrcRes[zoom_num].resolution_high;	
			IF_AW_cnt1++;
			return 2;
		}
		
		else if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+InSrcRes[zoom_num].resolution_width)<=1920&& (layout_port0_Y_pre+InSrcRes[zoom_num].resolution_high)<=2400)  //布局器1横向可以进行
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]=zoom_num;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][2]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG0_Para[IF_AW_cnt0][5]=InSrcRes[zoom_num].resolution_high;
			
			IF_AW_BG0_Para[IF_AW_cnt0][6]=layout_port0_X;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y_pre;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=layout_port0_X+InSrcRes[zoom_num].resolution_width;
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y_pre+InSrcRes[zoom_num].resolution_high;
			if(!layout_port0_X_flag)
			{
				layout_port0_X+=InSrcRes[zoom_num].resolution_width;
				layout_port0_X_flag=1;
				layout_port0_Y=layout_port0_Y_pre+InSrcRes[zoom_num].resolution_high;
			}
			else
			{				
				layout_port0_Y_pre+=InSrcRes[zoom_num].resolution_high;
				if(layout_port0_Y_pre>=layout_port0_Y)
					layout_port0_Y=layout_port0_Y_pre;	
			}				
			IF_AW_cnt0++;
			return 1;
		}
		else if((InSrcRes[zoom_num].resolution_width)<=1920&& (layout_port0_Y+InSrcRes[zoom_num].resolution_high)<=2400)  //布局器1纵向可以进行
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]= zoom_num;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][2]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG0_Para[IF_AW_cnt0][5]=InSrcRes[zoom_num].resolution_high;
			
			IF_AW_BG0_Para[IF_AW_cnt0][6]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=InSrcRes[zoom_num].resolution_width;
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y+InSrcRes[zoom_num].resolution_high;
			layout_port0_Y_pre=layout_port0_Y;
			layout_port1_X=InSrcRes[zoom_num].resolution_width;
			layout_port1_X_flag=0;
			layout_port0_Y=layout_port0_Y_pre+InSrcRes[zoom_num].resolution_high;	
			IF_AW_cnt0++;
			return 1;
		}	
	
	}
	return 0;
	
}



u8 find_same_soure_to_outboard(char *return_num, u8 source)
{
	char i=0;
	if(IF_AW_cnt1>0)
	{
		for(i=0;i<IF_AW_cnt1;i++)
		{
			if(IF_AW_BG1_Para[i][0]==source)
			{
				*return_num=i;
				return 2;
			}
		}
	}
	if(IF_AW_cnt0>0)
	{
		for(i=0;i<IF_AW_cnt0;i++)
		{
			if(IF_AW_BG0_Para[i][0]==source)
			{
				*return_num=i;
				return 1;
			}
		}
	}
	return 0;
	
}


//定义输入板的两个布局所要布局的画面参数
//sprintf(gCmdBuf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3],OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);			
//OUTPUT_FPGA SET_WINDOWS_LAYOUT [OutputSlotIndex] [BackgroundIndex] [ScreenWidth] [ScreenHeight]
//OUTPUT_FPGA ADD_WINDOWS [SourceIndex] [Layer] [InLeft] [InTop] [InRight] [InBottom] [DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
//OUTPUT_FPGA ADD_WINDOWS_END
//反复使用ADD_WINDOWS添加多个窗口
//OutputSlotIndex: 输出板卡索引号，取值0-3
//BackgroundIndex：背景索引号，取值0-3
//ScreenWidth，ScreenHeight背景输出的图像分辨率长度和高度
//SourceIndex：信号源在输出板卡中的索引号，取值0-8。0-3是输入卡通过背板直接接入的原始图像信号，4-7是输出卡缩放器输出的信号，8是当前通道的背景信号
//Layer：窗口所在层或窗口优先级，值越大优先级越高，取值1-N
//InLeft，InTop，InRight，InBottom：表示显示窗口需要截取的信号源图像的位置。
//DisplayLeft，DisplayTop，DisplayRight，DisplayBottom：表示显示窗口的位置。
//			
u16 OF_AW_Para[4][8][ADD_WIN_PARA_NUM];
u8 OF_AW_cnt[4]={0};




//image-layout call it
void ouputboard_out_layout (u8 input_port_num,u8 layout_num,u8 num,u16 begin_x,u16 begin_y,u16 end_x,u16 end_y)
{
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	u32 x_count=0;
	u32 y_count=0;
	u8 RowCnt=0;
	u8 ColCnt=0;
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&begin_x<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && end_x<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&begin_y<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && end_y<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//以上两个循环找到了图像起点所在的输出端口

	
		for(j=col;j<=col_end;j++)
		{	
			OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][0]=input_port_num-1;
//			OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][1]=OF_AW_cnt[line*gScrCol+j];
			if(end_x<=gScrTotPix[0][j])
			{
				if(j==col)//first in function
				{
					if(input_port_num==1)
					{
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=IF_AW_BG0_Para[layout_num][6];
					}
					else
					{
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=IF_AW_BG1_Para[layout_num][6];
					}
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=end_x-begin_x+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
					
				}
				else
				{
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_AW_Para[line*gScrCol+j-1][OF_AW_cnt[line*gScrCol+j-1]][4];
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=end_x-gScrTotPix[0][j-1]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
				}
				
			}
			else
			{
				if(j==col)
				{
				if(input_port_num==1)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=IF_AW_BG0_Para[layout_num][6];
				else
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=IF_AW_BG1_Para[layout_num][6];
				
				OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=gScrTotPix[0][j]-gImgPara[num][BEGIN_X_INDEX]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
				}
				else
				{
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_AW_Para[line*gScrCol+j-1][OF_AW_cnt[line*gScrCol+j-1]][4];
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=g_screen_line_column_pixel[0][j]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];	
				}
				
			}
				if(j==0)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=begin_x;
				else if(j==col)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=begin_x-gScrTotPix[0][j-1];
				else
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=0;
				OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][8]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]-OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6];			
		}
			for(j=line;j<=line_end;j++)
			{			
				if(end_y<=gScrTotPix[1][j])
				{
					if(j==line)//first in function
					{
						if(input_port_num==1)
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=IF_AW_BG0_Para[layout_num][7];
						else
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=IF_AW_BG1_Para[layout_num][7];

						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=end_y-begin_y+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
					else
					{
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_AW_Para[(j-1)*gScrCol+col][OF_AW_cnt[(j-1)*gScrCol+col]][5];
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=end_y-gScrTotPix[1][j-1]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
				}
				else
				{
					if(j==line)
					{
						if(input_port_num==1)
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=IF_AW_BG0_Para[layout_num][7];
						else
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=IF_AW_BG1_Para[layout_num][7];			
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=gScrTotPix[1][j]-gImgPara[num][BEGIN_Y_INDEX]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
				
					}
					else
					{
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_AW_Para[(j-1)*gScrCol+col][OF_AW_cnt[(j-1)*gScrCol+col]][5];
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=g_screen_line_column_pixel[1][j]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
				}
	
				if(j==0)
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=begin_y;
				else if(j==line)
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=begin_y-gScrTotPix[1][j-1];
				
				else
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=0;
				OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][9]=OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]-OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]+	OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7];	
		 }

			for(i=line;i<=line_end;i++)
			{
				for(j=col;j<=col_end;j++)
				{
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][SourceIndex]=input_port_num-1;
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][Layer]=num+1;
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][InLeft]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][InTop]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][3];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][InRight]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][InBottom]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][5];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][DisplayLeft]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][DisplayTop]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][7];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][DisplayRight]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][8];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][DisplayBottom]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][9];
							
				}
			}

//#define SourceIndex 0
//#define Layer 1
//#define InLeft 2
//#define InTop 3
//#define InRight 4
//#define InBottom 5
//#define DisplayLeft 6
//#define DisplayTop 7
//#define DisplayRight 8
//#define DisplayBottom 9
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				OF_AW_cnt[i*gScrCol+j]++;
			}
}


//对输出板上缩放器的内容进行输出布局，输出到输出端口
void outputboard_zoom_out_layout(u8 zoom_num ,u8 num)
{
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	u32 x_count=0;
	u32 y_count=0;
	u8 RowCnt=0;
	u8 ColCnt=0;
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&gImgPara[num][BEGIN_X_INDEX]<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && gImgPara[num][END_X_INDEX]<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&gImgPara[num][BEGIN_Y_INDEX]<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//以上两个循环找到了图像起点所在的输出端口

	
		for(j=col;j<=col_end;j++)
		{	
			OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][0]=zoom_num+3;
			if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
			{
				if(j==col)//first in function
				{
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_Scaler_Para[zoom_num-3][5];
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX]+OF_Scaler_Para[zoom_num-3][5];
					
				}
				else
				{
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_AW_Para[line*gScrCol+j-1][OF_AW_cnt[line*gScrCol+j]][4];
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
				}
				
			}
			else
			{
				if(j==col)
				{
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_Scaler_Para[zoom_num-3][5];	
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=gScrTotPix[0][j]-gImgPara[num][BEGIN_X_INDEX]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
				}
				else
				{
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]=OF_AW_Para[line*gScrCol+j-1][OF_AW_cnt[line*gScrCol+j]][4];
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]=gScrTotPix[0][j]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];	
				}
				
			}
				if(j==0)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX];
				else if(j==col)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX]-gScrTotPix[0][j-1];
				else
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6]=0;
				OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][8]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4]-OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2]+OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6];
			
			
		}

			for(j=line;j<=line_end;j++)
			{			
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][j])
				{
					if(j==line)//first in function
					{
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_Scaler_Para[zoom_num-3][7];	
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
					else
					{
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_AW_Para[(j-1)*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5];
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][j-1]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
				}
				else
				{
					if(j==line)
					{
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_Scaler_Para[zoom_num-3][7];
							OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=gScrTotPix[1][j]-gImgPara[num][BEGIN_Y_INDEX]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];				
					}
					else
					{
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]=OF_AW_Para[(j-1)*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5];
						OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]=gScrTotPix[1][j]+OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3];
					}
				}
	
				if(j==0)
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=gImgPara[num][BEGIN_Y_INDEX];
				else if(j==line)
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=gImgPara[num][BEGIN_Y_INDEX]-gScrTotPix[1][j-1];
				
				else
					OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7]=0;
				OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][9]=OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][5]-OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][3]+	OF_AW_Para[j*gScrCol+col][OF_AW_cnt[j*gScrCol+col]][7];	
		 }

			for(i=line;i<=line_end;i++)
			{
				for(j=col;j<=col_end;j++)
				{
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=zoom_num+3;
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1;
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][2];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][3]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][3];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][4];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][5];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][6];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][7];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][8];
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+col][OF_AW_cnt[i*gScrCol+col]][9];
							
				}
			}
		for(i=line;i<=line_end;i++)
		for(j=col;j<=col_end;j++)
		{
			OF_AW_cnt[i*gScrCol+j]++;
		}
}



void Clear_Display(u8 num)
{
		sprintf(gCmdBuf,"GP SDMI %d\r\n",num);
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
		delay_ms(100);
		TryEnd_Clear_Display();
		Uart_Send_Cmd_to_FPGA("GP SDMI 0\r\n");
		delay_ms(100);	
}


//only for trial machine
void TryEnd_Clear_Display(void)
{
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 0\r\n");
		delay_ms(100);
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 1\r\n");
		delay_ms(100);
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 2\r\n");
		delay_ms(100);
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 4\r\n");
		delay_ms(100);
		Uart_Send_Cmd_to_FPGA("O_F SUSD 3\r\n");
		delay_ms(200);
}


//送参数给fpga
void output_set(void)
{
	u8 i=0;
	u8 j=0;
	for(i=0;i<inBoardUsedScalerNum;i++)  //打印输入板缩放器的值
	{	
			sprintf(gCmdBuf,"I_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",IF_Scaler_Para[i][0],IF_Scaler_Para[i][1],IF_Scaler_Para[i][2],IF_Scaler_Para[i][3],IF_Scaler_Para[i][4],IF_Scaler_Para[i][5],IF_Scaler_Para[i][6],IF_Scaler_Para[i][7],IF_Scaler_Para[i][8],IF_Scaler_Para[i][9],IF_Scaler_Para[i][10]);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}
	if(IF_AW_cnt0>0)
	{	//INPUT_FPGA SET_WINDOWS_LAYOUT [BoardIndex] [BackgroundIndex] [ScreenWidth][ScreenHeight]
		sprintf(gCmdBuf,"I_F SWL 0 0 1920 2400\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
		for(i=0;i<IF_AW_cnt0;i++)
		{
				sprintf(gCmdBuf,"I_F AW %d %d %d %d %d %d %d %d %d %d\r\n",IF_AW_BG0_Para[i][0],IF_AW_BG0_Para[i][1],IF_AW_BG0_Para[i][2],IF_AW_BG0_Para[i][3],IF_AW_BG0_Para[i][4],IF_AW_BG0_Para[i][5],IF_AW_BG0_Para[i][6],IF_AW_BG0_Para[i][7],IF_AW_BG0_Para[i][8],IF_AW_BG0_Para[i][9]);		
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
			delay_ms(10);
		}
		sprintf(gCmdBuf,"I_F AWE\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}
	
	if(IF_AW_cnt1>0)
	{
		sprintf(gCmdBuf,"I_F SWL 0 1 1920 2400\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	
		for(i=0;i<IF_AW_cnt1;i++)
		{
				sprintf(gCmdBuf,"I_F AW %d %d %d %d %d %d %d %d %d %d\r\n",IF_AW_BG1_Para[i][0],IF_AW_BG1_Para[i][1],IF_AW_BG1_Para[i][2],IF_AW_BG1_Para[i][3],IF_AW_BG1_Para[i][4],IF_AW_BG1_Para[i][5],IF_AW_BG1_Para[i][6],IF_AW_BG1_Para[i][7],IF_AW_BG1_Para[i][8],IF_AW_BG1_Para[i][9]);		
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
		}
		sprintf(gCmdBuf,"I_F AWE\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}
	for(i=0;i<OutBoardUsedScalerNum;i++)  //打印输出板缩放器的值
	{	
			sprintf(gCmdBuf,"O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",OF_Scaler_Para[i][0],OF_Scaler_Para[i][1],OF_Scaler_Para[i][2],OF_Scaler_Para[i][3],OF_Scaler_Para[i][4],OF_Scaler_Para[i][5],OF_Scaler_Para[i][6],OF_Scaler_Para[i][7],OF_Scaler_Para[i][8],OF_Scaler_Para[i][9],OF_Scaler_Para[i][10]);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}
	for(i=0;i<4;i++)
	{
		if(OF_AW_cnt[i]!=0)
		{
			sprintf(gCmdBuf,"O_F SWL 3 %d 1920 1080\r\n",i);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
			for(j=0;j<OF_AW_cnt[i];j++)
			{
				sprintf(gCmdBuf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3],OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);				
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
			}
			sprintf(gCmdBuf,"O_F AWE\r\n",i);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
		}
	}
	for(i=0;i<4;i++)
	{
		if(OF_AW_cnt[i]==0)
		{
			sprintf(gCmdBuf,"OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 %d\r\n",i);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
		}
	}
//	命令 输入卡更新显示
//该命令用于设置输入卡的参数更新，只有该板卡的参数更新后，板卡才将新的配置参数有效。
//INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY [InputSlotIndex]
	sprintf(gCmdBuf,"I_F SUSD 0\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	
	sprintf(gCmdBuf,"O_F SUSD 3\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
}


//将当前设置保存到方案
void save_scenario_to_FPGA(u8 num)
{
	sprintf(gCmdBuf,"GP SDMI %d\r\n",num);
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	output_set();
	GpuSend("SBC(7);");
	GpuSend("CBOF(10,45,200,130,7,7);");

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……7秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……6秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……5秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……4秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……3秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……2秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'方案%d正在保存……1秒',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);
	

//	命令 设置显示效果的保存索引
//	GP SDMI [DisplayModeIndex]
//	DisplayModeIndex：显示效果保存到处理器的地址索引，取值0~19
	sprintf(gCmdBuf,"GP SDMI 0\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);

}




u8 load_scenario_from_FPGA(u8 num)
{
	sprintf(gCmdBuf,"GP LDM %d\r\n",num);
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	delay_ms(100);
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	return 1;
}


//是否需要使用缩放器;返回值是需要使用的缩放其的个数
u8 howMany_Scaler_zoom_need(u16 num)
{
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&gImgPara[num][BEGIN_X_INDEX]<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && gImgPara[num][END_X_INDEX]<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}

	
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&gImgPara[num][BEGIN_Y_INDEX]<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//以上两个循环找到了图像起点所在的输出端口	
	return (line_end-line+1)*(col_end-col+1);  //计算出需要的缩放器个数，实际就是要显示到多少个输出口
	
}



 //直接将信号发送至输出板进行处理;不使用缩放器
void input_to_output_layout(u8 port,u8 port_num ,u8 num)  
{
	
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	u32 x_count=0;
	u32 y_count=0;
	u8 RowCnt=0;
	u8 ColCnt=0;
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&gImgPara[num][BEGIN_X_INDEX]<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && gImgPara[num][END_X_INDEX]<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&gImgPara[num][BEGIN_Y_INDEX]<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//以上两个循环找到了图像起点所在的输出端口
		
	for(i=line;i<=line_end;i++)
	{
		for(j=col;j<=col_end;j++)
		{
			if(i==line)
			{
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][line])
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][5]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX];
				else
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][5]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
				
				if(line>0)
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][7]=gImgPara[num][BEGIN_Y_INDEX]-gScrTotPix[1][line-1];
				else						
					OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][7]=gImgPara[num][BEGIN_Y_INDEX];
		
				if(port==1)
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][3]=IF_AW_BG0_Para[port_num][7];
				else if(port==2)
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][3]=IF_AW_BG1_Para[port_num][7];
			
				
			}
			else
			{
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
				{
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][i-1];
				
				}
				else
				{
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[j*gScrCol+j]][5]=gScrTotPix[1][i]-gScrTotPix[1][i-1];
				}
				OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=0;
				OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][3]=OF_AW_Para[i*gScrCol+j-1][OF_AW_cnt[i*gScrCol+j-1]][3];
				
			}					
			if(j==col)
			{			
				if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX];
				else
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];	
				if(col>0)
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX]-gScrTotPix[0][j-1];
				else						
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX];
			if(port==1)
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=IF_AW_BG0_Para[port_num][6];
			else if(port==2)
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=IF_AW_BG1_Para[port_num][6];
		
			}
			else
			{
					if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
					{
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[j*gScrCol+j]][4]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1];	
					}
					else
					{
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[j*gScrCol+j]][4]=gScrTotPix[0][j]-gScrTotPix[0][j-1];
					}
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[j*gScrCol+j]][6]=0;  //显示从0点开始
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=OF_AW_Para[i*gScrCol+j-1][OF_AW_cnt[i*gScrCol+j-1]][2];
			}
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4];
		
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=port-1; //信号源
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //信号源
				
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5] ;
			}			
		}
		
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				OF_AW_cnt[i*gScrCol+j]++;
			}
}


//port 参数表示了从输入板的两个布局口哪一个来的信号源
//source 参数给输出板的输出端口指明了信号来源
//num 表示的是画面优先号
void outputboard_zoom_and_layout(u8 port,u8 port_num ,u8 num)  //处理因需要放大，从输出板使用缩放器进行放大而进行的布局输出
{
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	u32 x_count=0;
	u32 y_count=0;
	u8 RowCnt=0;
	u8 ColCnt=0;
	u8 output_zoom_port_used_pre=OutBoardUsedScalerNum;
	
	u16 cur_Y_point=0;
	
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&gImgPara[num][BEGIN_X_INDEX]<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && gImgPara[num][END_X_INDEX]<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&gImgPara[num][BEGIN_Y_INDEX]<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//以上两个循环找到了图像起点所在的输出端口
		
	for(i=line;i<=line_end;i++)
	{	
		if(i>=line)
		{
			cur_Y_point=OF_Scaler_Para[OutBoardUsedScalerNum-1][8]+1;
		}
		for(j=col;j<=col_end;j++)
		{	
				if(i==line)
				{
					if(port==1)
						OF_Scaler_Para[OutBoardUsedScalerNum][7]=IF_AW_BG0_Para[port_num][7];
					else if(port==2)
						OF_Scaler_Para[OutBoardUsedScalerNum][7]=IF_AW_BG1_Para[port_num][7];
										
					if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][line])
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][10]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX];
						OF_Scaler_Para[OutBoardUsedScalerNum][8]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high-1;
					}
					else
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][10]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
						OF_Scaler_Para[OutBoardUsedScalerNum][8]=OF_Scaler_Para[OutBoardUsedScalerNum][7]+g_screen_line_column_pixel[1][i]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high/gImgPara[num][HIGH_INDEX]-1;
					}
					
					
					if(line>0)
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][7]=gImgPara[num][BEGIN_Y_INDEX]-gScrTotPix[1][line-1];
					else						
						OF_AW_Para[line*gScrCol+j][OF_AW_cnt[line*gScrCol+j]][7]=gImgPara[num][BEGIN_Y_INDEX];
				}
				else
				{
					if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][7]=cur_Y_point;
						OF_Scaler_Para[OutBoardUsedScalerNum][8]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high-1;
						OF_Scaler_Para[OutBoardUsedScalerNum][10]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][i-1];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=0;
					}
					else
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][7]=cur_Y_point;
						OF_Scaler_Para[OutBoardUsedScalerNum][8]=OF_Scaler_Para[OutBoardUsedScalerNum][7]+g_screen_line_column_pixel[1][i]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high/gImgPara[num][HIGH_INDEX]-1; 
						OF_Scaler_Para[OutBoardUsedScalerNum][10]=g_screen_line_column_pixel[1][i];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=0;
					}
				}
	
			OF_Scaler_Para[OutBoardUsedScalerNum][0]=OUTPUT_BOARD3;
			OF_Scaler_Para[OutBoardUsedScalerNum][1]=OutBoardUsedScalerNum;	
			OF_Scaler_Para[OutBoardUsedScalerNum][2]=port-1;//输入信号源
			///////////////////////////////////////
			//以下的参数应该是根据实际输入源的参数进行赋值
			OF_Scaler_Para[OutBoardUsedScalerNum][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width;// 输入源分辨率
			OF_Scaler_Para[OutBoardUsedScalerNum][4]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high;//	
			
			if(j==col)
			{	
				if(port==1)
					OF_Scaler_Para[OutBoardUsedScalerNum][5]=IF_AW_BG0_Para[port_num][6];
				else if(port==2)
					OF_Scaler_Para[OutBoardUsedScalerNum][5]=IF_AW_BG1_Para[port_num][6];
				
				if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
				{
					OF_Scaler_Para[OutBoardUsedScalerNum][9]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX];
					OF_Scaler_Para[OutBoardUsedScalerNum][6]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width-1; //此次假设输入为1920点宽 还得改成实际输出的值
				}
				else
				{
					OF_Scaler_Para[OutBoardUsedScalerNum][9]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];	
					OF_Scaler_Para[OutBoardUsedScalerNum][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width/gImgPara[num][WIDTH_INDEX]-1; //此次假设输入为1920点宽 还得改成实际输出的值
				}
			 if(col>0)
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX]-gScrTotPix[0][col-1];
				else						
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=gImgPara[num][BEGIN_X_INDEX];
				
			}
			else
			{
					if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][5]=OF_Scaler_Para[OutBoardUsedScalerNum-1][6]+1;
						OF_Scaler_Para[OutBoardUsedScalerNum][6]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width-1;
						OF_Scaler_Para[OutBoardUsedScalerNum][9]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0; //显示从0点开始
					}
					else
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][5]=OF_Scaler_Para[OutBoardUsedScalerNum-1][6]+1;
						OF_Scaler_Para[OutBoardUsedScalerNum][6]=g_screen_line_column_pixel[0][j]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width/gImgPara[num][WIDTH_INDEX]+OF_Scaler_Para[OutBoardUsedScalerNum][5]-1; 
						OF_Scaler_Para[OutBoardUsedScalerNum][9]=g_screen_line_column_pixel[0][j];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0;  //显示从0点开始
					}
			}
	
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=OutBoardUsedScalerNum+4; //信号源
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //信号源
			
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=0;
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][3]=0;
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4]=OF_Scaler_Para[OutBoardUsedScalerNum][9];
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5]=OF_Scaler_Para[OutBoardUsedScalerNum][10];
		
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4];	
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5] ;
			OutBoardUsedScalerNum++;				
		}
	}
		
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				OF_AW_cnt[i*gScrCol+j]++;
			}
}


//image layout call it
void inputboard_zoom_and_layout(u8 num)
{
	u8 line=0;
	u8 line_flag=0;
	u8 col_flag=0;
	u8 col=0;
	u8 line_end=0;
	u8 col_end=0;
	u8 line_end_flag=0;
	u8 col_end_flag=0;
	u8 i,j;
	u32 x_count=0;
	u32 y_count=0;
	u8 RowCnt=0;
	u8 ColCnt=0;
	u8 select_inputboard_layout_port=1;
	char select_inputboard_layout_port_num=0;
	u8 input_zoom_port_used_pre=inBoardUsedScalerNum;
	
	u16 curimage_begin_x=0;
	u16 curimage_begin_y=0;
	u16 curimage_end_x=0;
	u16 curimage_end_y=0;
	
	u16 cur_Y_point=0;
	for(i=0;i<gScrCol;i++)
	{
			if(col_flag==0&&gImgPara[num][BEGIN_X_INDEX]<gScrTotPix[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && gImgPara[num][END_X_INDEX]<=gScrTotPix[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}

	
	for(i=0;i<gScrRow;i++)
	{
			if(line_flag==0&&gImgPara[num][BEGIN_Y_INDEX]<gScrTotPix[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}

	//以上两个循环找到了图像起点所在的输出端口
		
	for(i=line;i<=line_end;i++)
	{
		if(i>line)
		{
			cur_Y_point=IF_Scaler_Para[inBoardUsedScalerNum-1][8]+1;
		}
		for(j=col;j<=col_end;j++)
		{
			if(i==line)
			{
				IF_Scaler_Para[inBoardUsedScalerNum][7]=0;				
				curimage_begin_y=gImgPara[num][BEGIN_Y_INDEX];
				
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][line])
				{	
					IF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX];
					IF_Scaler_Para[inBoardUsedScalerNum][8]=InSrcRes[num].resolution_high-1; //此次假设输入为1080高 还得改成实际输出的值
				}
				else
				{					
					IF_Scaler_Para[inBoardUsedScalerNum][10]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
					IF_Scaler_Para[inBoardUsedScalerNum][8]=IF_Scaler_Para[inBoardUsedScalerNum][7]+(gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX])*InSrcRes[num].resolution_high/gImgPara[num][HIGH_INDEX]-1; //此次假设输入为1080高 还得改成实际输出的值
				}
	
			}
			else
			{
				curimage_begin_y=gScrTotPix[1][i-1];
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
				{
					IF_Scaler_Para[inBoardUsedScalerNum][7]=cur_Y_point;
					IF_Scaler_Para[inBoardUsedScalerNum][8]=InSrcRes[num].resolution_high-1; 
					IF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][i-1];
				}
				else
				{
					IF_Scaler_Para[inBoardUsedScalerNum][7]=cur_Y_point;
					IF_Scaler_Para[inBoardUsedScalerNum][8]=IF_Scaler_Para[inBoardUsedScalerNum][7]+g_screen_line_column_pixel[1][i]*InSrcRes[num].resolution_high/gImgPara[num][HIGH_INDEX]-1;
					IF_Scaler_Para[inBoardUsedScalerNum][10]=g_screen_line_column_pixel[1][i];
				}
				
			}
				
			IF_Scaler_Para[inBoardUsedScalerNum][0]=INPUT_BOARD0;
			IF_Scaler_Para[inBoardUsedScalerNum][1]=inBoardUsedScalerNum;	
			IF_Scaler_Para[inBoardUsedScalerNum][2]=gImgPara[num][SOURCE_IN_INDEX];//输入信号源
			///////////////////////////////////////
			//以下的参数应该是根据实际输入源的参数进行赋值
			IF_Scaler_Para[inBoardUsedScalerNum][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width;// 输入源分辨率
			IF_Scaler_Para[inBoardUsedScalerNum][4]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_high;//	
			
			if(j==col)
			{	
				IF_Scaler_Para[inBoardUsedScalerNum][5]=0;
				
						
			
				curimage_begin_x=gImgPara[num][BEGIN_X_INDEX];
				
				if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
				{
					IF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX];
					IF_Scaler_Para[inBoardUsedScalerNum][6]=InSrcRes[num].resolution_width-1;
				}
				else
				{
					IF_Scaler_Para[inBoardUsedScalerNum][9]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];
					IF_Scaler_Para[inBoardUsedScalerNum][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[num].resolution_width/gImgPara[num][WIDTH_INDEX]-1; //此次假设输入为1920点宽 还得改成实际输出的值						
				}
			}
			else
			{
					curimage_begin_x=gScrTotPix[0][j-1];
					if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
					{
						IF_Scaler_Para[inBoardUsedScalerNum][5]=IF_Scaler_Para[inBoardUsedScalerNum-1][6]+1;
						IF_Scaler_Para[inBoardUsedScalerNum][6]=InSrcRes[num].resolution_width-1;
						IF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1];
					}
					else
					{
						IF_Scaler_Para[inBoardUsedScalerNum][5]=IF_Scaler_Para[inBoardUsedScalerNum-1][6]+1;
						IF_Scaler_Para[inBoardUsedScalerNum][6]=IF_Scaler_Para[inBoardUsedScalerNum][5]+g_screen_line_column_pixel[0][j]*InSrcRes[num].resolution_width/gImgPara[num][WIDTH_INDEX]-1; 
						IF_Scaler_Para[inBoardUsedScalerNum][9]=g_screen_line_column_pixel[0][j];
					}
			}
			//以下进行输入板的输出口布局//
			select_inputboard_layout_port=inputboard_out_layout(inBoardUsedScalerNum,1);
			curimage_end_x=curimage_begin_x+IF_Scaler_Para[inBoardUsedScalerNum][9];
			curimage_end_y=curimage_begin_y+IF_Scaler_Para[inBoardUsedScalerNum][10];
			inBoardUsedScalerNum++;
			if(select_inputboard_layout_port==1)
				select_inputboard_layout_port_num=IF_AW_cnt0-1;
			else if(select_inputboard_layout_port==2)
				select_inputboard_layout_port_num=IF_AW_cnt1-1;
			ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,num,curimage_begin_x,curimage_begin_y,curimage_end_x,curimage_end_y);	
		}	
	}	
}



//add-new-image,matrix call image_layout画面布局处理函数
void image_layout(void)
{
	u8 i=0;
	u8 zoom_need_j=0;
	u8 numb_zoom_need_scaler=0;
	char  source_to_outboard_num=0;
	char select_inputboard_layout_port_num=0;
	u8 select_inputboard_layout_port=1;
	u8 find_same_input_zoom=0; //查找相同的缩放
	u8 same_output_zoom_num=0;
	for(i=0 ;i<4;i++)
	{	
		OF_AW_cnt[i]=0;
	}
	layout_port0_X=0;
	layout_port0_X_flag=0;
	layout_port0_Y=0;
	layout_port0_Y_pre=0;
	layout_port1_X=0;
	layout_port1_X_flag=0;
	layout_port1_Y=0;
	layout_port1_Y_pre=0;
	IF_AW_cnt0=0;
	IF_AW_cnt1=0;
	
//	gImgPara[0][0]=0;
//	gImgPara[0][1]=0;
//	gImgPara[0][2]=7680;
//	gImgPara[0][3]=1080;
//	gImgPara[0][4]=7680;
//	gImgPara[0][5]=1080;
//	gImgPara[0][6]=0;
//	gImgPara[0][7]=1;
//	gImgPara[0][8]=0;

//	gImgPara[1][0]=0;
//	gImgPara[1][1]=100;
//	gImgPara[1][2]=7680;
//	gImgPara[1][3]=800;
//	gImgPara[1][4]=7680;
//	gImgPara[1][5]=700;
//	gImgPara[1][6]=1;
//	gImgPara[1][7]=2;
//	gImgPara[1][8]=0;
//	
//	gImgPara[2][0]=22;
//	gImgPara[2][1]=70;
//	gImgPara[2][2]=268;
//	gImgPara[2][3]=480;
//	gImgPara[2][4]=246;
//	gImgPara[2][5]=410;
//	gImgPara[2][6]=0;
//	gImgPara[2][7]=3;
//	gImgPara[2][8]=0;

//	gImgPara[3][0]=1876;
//	gImgPara[3][1]=50;
//	gImgPara[3][2]=2130;
//	gImgPara[3][3]=496;
//	gImgPara[3][4]=254;
//	gImgPara[3][5]=446;
//	gImgPara[3][6]=1;
//	gImgPara[3][7]=4;
//	gImgPara[3][8]=0;
	
	inBoardUsedScalerNum=0;
	OutBoardUsedScalerNum=0;
	source_to_outboard_num=0;
	for(i=0;i<IMAGE_TOTAL_NUM;i++) //处理图层数据
	{
		if(gImgPara[i][LAYER_INDEX]!=0) //图层号不等于0，表示有图像存在
		{
			if(gImgPara[i][WIDTH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width && gImgPara[i][HIGH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high
				||gImgPara[i][WIDTH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width && gImgPara[i][HIGH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high)  //当前图像层图像尺寸小于
			{
				find_same_input_zoom=same_input_zoom(i);
				if(find_same_input_zoom==0)  //如果没有已经存在的缩放方式与当前处理的相同
				{		
					if(inBoardUsedScalerNum<4)//有可分配的缩放器且可以安排布局，在输入板上进行缩放
					{						
						IF_Scaler_Para[inBoardUsedScalerNum][0]=INPUT_BOARD0;
						IF_Scaler_Para[inBoardUsedScalerNum][1]=inBoardUsedScalerNum;	
						IF_Scaler_Para[inBoardUsedScalerNum][2]=gImgPara[i][SOURCE_IN_INDEX];//输入信号源
						///////////////////////////////////////
						//以下的参数应该是根据实际输入源的参数进行赋值
						IF_Scaler_Para[inBoardUsedScalerNum][3]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width;// 输入源分辨率
						IF_Scaler_Para[inBoardUsedScalerNum][4]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high;//
						IF_Scaler_Para[inBoardUsedScalerNum][5]=0;//
						IF_Scaler_Para[inBoardUsedScalerNum][6]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width-1;//
						IF_Scaler_Para[inBoardUsedScalerNum][7]=0;//
						IF_Scaler_Para[inBoardUsedScalerNum][8]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high-1;//
						/////////////////////////////////////////						
						IF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[i][4];//画面的宽
						IF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[i][5];//画面的高	
					
						//以下进行输入板的输出口布局//
						select_inputboard_layout_port=inputboard_out_layout(inBoardUsedScalerNum,1);
						
						inBoardUsedScalerNum++;
						if(select_inputboard_layout_port==1)
							select_inputboard_layout_port_num=IF_AW_cnt0-1;
						else if(select_inputboard_layout_port==2)
							select_inputboard_layout_port_num=IF_AW_cnt1-1;
						if(select_inputboard_layout_port>0)
							ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
					}	
					else  //无可分配的缩放器
					{
						//
						//应该找一下布局中是否已经把此信号源带入到输出板 ，如没有再进行布局
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //没有
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//进行布局
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //直接将信号发送至输出板进行处理 ,如果信号无法通过布局器送入到输出板，就放弃图像
						{
							same_output_zoom_num=same_output_zoom(select_inputboard_layout_port,source_to_outboard_num,i);
							if(!same_output_zoom_num)  //如果没有已经存在的缩放方式与当前处理的相同
							{		
								//进入此处说明可以把源信号带入输出板						
								if(OutBoardUsedScalerNum<4)//有可分配的缩放器且可以安排布局，在输出板上进行缩放
								{											
									OF_Scaler_Para[inBoardUsedScalerNum][0]=OUTPUT_BOARD3;
									OF_Scaler_Para[inBoardUsedScalerNum][1]=OutBoardUsedScalerNum;	
									OF_Scaler_Para[inBoardUsedScalerNum][2]=select_inputboard_layout_port;//输入信号源
									///////////////////////////////////////
									//以下的参数应该是根据实际输入源的参数进行赋值
									OF_Scaler_Para[inBoardUsedScalerNum][3]=1920;// 输入源分辨率
									OF_Scaler_Para[inBoardUsedScalerNum][4]=2400;//
									if(select_inputboard_layout_port==1) //在布局器1
									{
										OF_Scaler_Para[inBoardUsedScalerNum][5]=IF_AW_BG0_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBoardUsedScalerNum][6]=IF_AW_BG0_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBoardUsedScalerNum][7]=IF_AW_BG0_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBoardUsedScalerNum][8]=IF_AW_BG0_Para[source_to_outboard_num][9]-1;//
									}
									else	if(select_inputboard_layout_port==2) //在布局器2
									{
										OF_Scaler_Para[inBoardUsedScalerNum][5]=IF_AW_BG1_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBoardUsedScalerNum][6]=IF_AW_BG1_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBoardUsedScalerNum][7]=IF_AW_BG1_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBoardUsedScalerNum][8]=IF_AW_BG1_Para[source_to_outboard_num][9]-1;//
									}
									/////////////////////////////////////////						
									OF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[i][4];//画面的宽
									OF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[i][5];//画面的高	
									
									same_output_zoom_num=OutBoardUsedScalerNum;
									OutBoardUsedScalerNum++;
									
									outputboard_zoom_out_layout(same_output_zoom_num,i);//得传入参数 0是暂时放的
								}						
								else//图像拿过来以后，没有缩放器进行缩放，就直接把输入源信号往输出上接
								{	
									ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
								}							
							}
							else  //存在相同的缩放
							{
								outputboard_zoom_out_layout(same_output_zoom_num,i);
							}					
						}		
					}						
				}
				else //已经存在一个相同的缩放
				{
					//查找相同的缩放器位置
					find_same_soure_to_outboard(&select_inputboard_layout_port_num,find_same_input_zoom+4);
					ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);

				}
				
			}
			else if(gImgPara[i][WIDTH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width || gImgPara[i][HIGH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high) //需要放大
			{
					
				numb_zoom_need_scaler=howMany_Scaler_zoom_need(i);
				if((4-OutBoardUsedScalerNum)>=numb_zoom_need_scaler)//有可分配的缩放器且可以安排布局，在输入板上进行缩放
				{																							
				//应该找一下布局中是否已经把此信号源带入到输出板 ，如没有再进行布局
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //没有
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//进行布局
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //直接将信号发送至输出板进行处理 ,如果信号无法通过布局器送入到输出板
						{					
							outputboard_zoom_and_layout(select_inputboard_layout_port,source_to_outboard_num,i);										
						}	
						else if((4-inBoardUsedScalerNum)>=numb_zoom_need_scaler) //如果没有办法传入到输出板，则查看输入板上的缩放器是否可用，可用就进行布局
						{
							inputboard_zoom_and_layout(i);
						}								
				}
				else if((4-inBoardUsedScalerNum)>=numb_zoom_need_scaler) //如果没有办法传入到输出板，则查看输入板上的缩放器是否可用，可用就进行布局
				{
					 inputboard_zoom_and_layout(i);
				}	
				else
				{
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //没有
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//进行布局
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //输入输出板上都没有缩放器可用，又可以把原信号发送到输出板，则不进行缩放，点对点显示
						{
								ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);								
						}	
				}				
			}
			else //图像输出与输入一样，不用分配缩放器，直接传入输出板进行分局显示
			{
				//应该找一下布局中是否已经把此信号源带入到输出板 ，如没有再进行布局
				select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
				if(select_inputboard_layout_port==0) //没有
				{
					select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//进行布局
					if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
					else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
				}
				if(select_inputboard_layout_port>0)  //直接将信号发送至输出板进行处理 ,如果信号无法通过布局器送入到输出板
				{					
					input_to_output_layout(select_inputboard_layout_port,source_to_outboard_num,i);										
				}		
			}
		}
	}
	output_set();
	
}


void  I_F_SET_WINDOWS_LAYOUT(void)
{
	
}



