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
# include <stdlib.h>


#define INPUT_BOARD0  0
#define INPUT_BOARD1  1
#define OUTPUT_BOARD2  2
#define OUTPUT_BOARD3  3

#define LAYER_INDEX 7
#define SOURCE_IN_INDEX 6
#define DEAL_INDEX 8
#define BEGIN_X_INDEX 0
#define BEGIN_Y_INDEX 1
#define END_X_INDEX 2
#define END_Y_INDEX 3
#define WIDTH_INDEX 4
#define HIGH_INDEX 5


//Resolution_Data input_source_resolution[8]=
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


Resolution_Data input_source_resolution[8]=
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


 //����һ�����飬��Ź�IMAGE_TOTAL_NUM������Ĳ���
 //����������ʾ����ʼλλ�á�����λ�á���ȡ��߶Ⱥ��ź�Դ���ⲿ�����0~3����ͼ��ţ�0��ͼ���ʾ�����ã����Ƿ������Դ


u8 scheme_save[9]={0};

u8 d_matrix_inputsource_num;
u8 d_matrix_outputsource_num;
u16 d_matrix_image_para[4][9];


u16 g_input_imagepara[9];

u16 d_stitching_image_para[IMAGE_TOTAL_NUM][9];
u16 d_image_para[IMAGE_TOTAL_NUM][9]={0}; 

char d_780command_send_buf[SEND_LEN];


void command_send(char *buf)
{
	 u8 i=0;
   while (1)
   {  if (buf[i]!='\0')
     { 
				USART_SendData(USART2, buf[i]);  //����һ��byte������
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}; //�ȴ����ͽ���
        i++;
     }
     else 
		 {
			 delay_ms(20);
			 return;
		 }
   }
}


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

void Read_TCPIP(void) //��ȡIP��Ϣ
{
	int IP[4]={0};
	int IP_Port=0;
	char sendtest[100];
	char  *check=NULL;
	g_Read_Command_Flag=1;
	g_RevUart2Idx=0;
	
	for(IP_Port=0;IP_Port<1024;IP_Port++)
	{
		g_RevUart2Data[IP_Port]=0;
	}
	command_send("GP GIPA 1\r\n");
	delay_ms(20);
	command_send("GP GTPN 1\r\n");
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




void input_RISR(void) //��ȡ����ֱ���
{
	int input_board=0;
	int input_board_port=0;
	int input_resolution_width=0;
	int input_resolution_high=0;
	char sendtest[100];
	char  *check=NULL;
	g_Read_Command_Flag=1;
	g_RevUart2Idx=0;
	command_send("I_F RISR 0 0\r\n");
//	delay_ms(20);
	command_send("I_F RISR 0 1\r\n");
//	delay_ms(20);
	command_send("I_F RISR 0 2\r\n");
//	delay_ms(20);
	command_send("I_F RISR 0 3\r\n");
//	delay_ms(20);
//	command_send("I_F RISR 1 0\r\n");
////	delay_ms(20);
//	command_send("I_F RISR 1 1\r\n");
////	delay_ms(20);
//	command_send("I_F RISR 1 2\r\n");
////	delay_ms(20);
//	command_send("I_F RISR 1 3\r\n");
	delay_ms(200);
//	sprintf(d_780command_send_buf,"INPUT_FPGA READ_INPUT_SOURCE_RESOLUTION 1810 1080 0 0 1920 1080 2 5818\r\nINPUT_FPGA READ_INPUT_SOURCE_RESOLUTION 0 0 0 0 1920 1080 2 5818\r\n");
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
			input_source_resolution[input_board*4+input_board_port].resolution_width=input_resolution_width;
			input_source_resolution[input_board*4+input_board_port].resolution_high=input_resolution_high;
		}
		check=strtok(NULL," ");
	}
	for(input_board=0; input_board<4 ; input_board++)
	{
		if(input_source_resolution[input_board].resolution_width!=0)
		{
			g_Image_Scr=input_board+1;
			break;
		}
	}
	g_Read_Command_Flag=0;
	g_RevUart2Idx=0;
}


//�������������ĸ���������ʹ�������ʹ��ʱ��ֵ
u16 input_zoom_port_data[4][11]={0};
u16 output_zoom_port_data[4][11]={0};

//��������������������Ҫ���ֵĻ������
u16 input_layout_port0_data[8][10];
u16 input_layout_port1_data[8][10];
u8 input_layout_port0_count=0;
u8 input_layout_port1_count=0;


u8 input_zoom_port_used=0;  //
u8 output_zoom_port_used=0;
u8 same_input_zoom(u8 num)
{
	u8 i;
	
	for(i=0;i<input_zoom_port_used;i++)
	{
		if(input_zoom_port_data[i][2]==d_image_para[num][6]
				&&input_zoom_port_data[i][9]==d_image_para[num][4]
				&&input_zoom_port_data[i][10]==d_image_para[num][5])
		{
			return 1;
		}
	}
	return 0;
}

u8 same_output_zoom(u8 port,u8 num,u8 image_num)
{
	u8 i;
	
	for(i=0;i<output_zoom_port_used;i++)
	{
			if(output_zoom_port_data[i][2]==port)
			{
				
				if(port==1 &&output_zoom_port_data[i][5]==input_layout_port0_data[num][6]
					&&output_zoom_port_data[i][6]==input_layout_port0_data[num][7]
					&&output_zoom_port_data[i][7]==input_layout_port0_data[num][8]
					&&output_zoom_port_data[i][8]==input_layout_port0_data[num][9]
					&&output_zoom_port_data[i][9]==d_image_para[image_num][4]
					&&output_zoom_port_data[i][10]==d_image_para[image_num][5])
				{
					return i;
				}
				else if(port==2 &&output_zoom_port_data[i][5]==input_layout_port1_data[num][6]
					&&output_zoom_port_data[i][6]==input_layout_port1_data[num][7]
					&&output_zoom_port_data[i][7]==input_layout_port1_data[num][8]
					&&output_zoom_port_data[i][8]==input_layout_port1_data[num][9]
					&&output_zoom_port_data[i][9]==d_image_para[image_num][4]
					&&output_zoom_port_data[i][10]==d_image_para[image_num][5])
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

u8 inputboard_out_layout(u8 zoom_num,u8 first_use)
{
	if(first_use==1)
	{	
		if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+input_zoom_port_data[zoom_num][9])<=1920&& (layout_port0_Y_pre+input_zoom_port_data[zoom_num][10])<=2400)  //������1������Խ���
		{
			input_layout_port0_data[input_layout_port0_count][0]=zoom_num+4;
			input_layout_port0_data[input_layout_port0_count][1]=input_layout_port0_count+1;
			input_layout_port0_data[input_layout_port0_count][2]=0;
			input_layout_port0_data[input_layout_port0_count][3]=0;
			input_layout_port0_data[input_layout_port0_count][4]=input_zoom_port_data[zoom_num][9];
			input_layout_port0_data[input_layout_port0_count][5]=input_zoom_port_data[zoom_num][10];
			
			input_layout_port0_data[input_layout_port0_count][6]=layout_port0_X;
			input_layout_port0_data[input_layout_port0_count][7]=layout_port0_Y_pre;
			input_layout_port0_data[input_layout_port0_count][8]=layout_port0_X+input_zoom_port_data[zoom_num][9];
			input_layout_port0_data[input_layout_port0_count][9]=layout_port0_Y_pre+input_zoom_port_data[zoom_num][10];
			if(!layout_port0_X_flag)
			{
				layout_port0_X+=input_zoom_port_data[zoom_num][9];
				layout_port0_X_flag=1;
				layout_port0_Y=layout_port0_Y_pre+input_zoom_port_data[zoom_num][10];	
			}
			else
			{
				layout_port0_Y_pre+=input_zoom_port_data[zoom_num][10];
				if(layout_port0_Y_pre>=layout_port0_Y)
					layout_port0_Y=layout_port0_Y_pre;
			}
			
			input_layout_port0_count++;
			return 1; 
		}
		else if((input_zoom_port_data[zoom_num][9])<=1920&& (layout_port0_Y+input_zoom_port_data[zoom_num][10])<=2400)  //������1������Խ���
		{
			input_layout_port0_data[input_layout_port0_count][0]= zoom_num+4;
			input_layout_port0_data[input_layout_port0_count][1]=input_layout_port0_count+1;
			input_layout_port0_data[input_layout_port0_count][2]=0;
			input_layout_port0_data[input_layout_port0_count][3]=0;
			input_layout_port0_data[input_layout_port0_count][4]=input_zoom_port_data[zoom_num][9];
			input_layout_port0_data[input_layout_port0_count][5]=input_zoom_port_data[zoom_num][10];
			
			input_layout_port0_data[input_layout_port0_count][6]=0;
			input_layout_port0_data[input_layout_port0_count][7]=layout_port0_Y;
			input_layout_port0_data[input_layout_port0_count][8]=input_zoom_port_data[zoom_num][9];
			input_layout_port0_data[input_layout_port0_count][9]=layout_port0_Y+input_zoom_port_data[zoom_num][10];
			layout_port0_Y_pre=layout_port0_Y;
			
			layout_port0_X=input_zoom_port_data[zoom_num][9];
			layout_port0_X_flag=0;
			
			layout_port0_Y=layout_port0_Y_pre+input_zoom_port_data[zoom_num][10];	
			input_layout_port0_count++;
			return 1;
		}	
		else if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_Y_pre<layout_port1_Y)&&(layout_port1_X+input_zoom_port_data[zoom_num][9])<=1920&&(layout_port1_Y_pre+input_zoom_port_data[zoom_num][10])<=2400)  //������2������Խ���
		{
			input_layout_port1_data[input_layout_port1_count][0]=zoom_num+4;
			input_layout_port1_data[input_layout_port1_count][1]=input_layout_port1_count+1;
			input_layout_port1_data[input_layout_port1_count][2]=0;
			input_layout_port1_data[input_layout_port1_count][3]=0;
			input_layout_port1_data[input_layout_port1_count][4]=input_zoom_port_data[zoom_num][9];
			input_layout_port1_data[input_layout_port1_count][5]=input_zoom_port_data[zoom_num][10];
			
			input_layout_port1_data[input_layout_port1_count][6]=layout_port1_X;
			input_layout_port1_data[input_layout_port1_count][7]=layout_port1_Y_pre;
			input_layout_port1_data[input_layout_port1_count][8]=layout_port1_X+input_zoom_port_data[zoom_num][9];
			input_layout_port1_data[input_layout_port1_count][9]=layout_port1_Y_pre+input_zoom_port_data[zoom_num][10];
			if(!layout_port1_X_flag)
			{
				layout_port1_X+=input_zoom_port_data[zoom_num][9];
				layout_port1_X_flag=1;
				layout_port1_Y=layout_port1_Y_pre+input_zoom_port_data[zoom_num][10];	
			}
			else
			{
				layout_port1_Y_pre+=input_zoom_port_data[zoom_num][10];
				if(layout_port1_Y_pre>=layout_port1_Y)
					layout_port1_Y=layout_port1_Y_pre;				
			}
			
			input_layout_port1_count++;
			return 2;
		}
		else if((input_zoom_port_data[zoom_num][9])<=1920&&(layout_port1_Y+input_zoom_port_data[zoom_num][10])<=2400)  //������2������Խ���
		{
			input_layout_port1_data[input_layout_port1_count][0]=zoom_num+4;
			input_layout_port1_data[input_layout_port1_count][1]=input_layout_port0_count+1;
			input_layout_port1_data[input_layout_port1_count][2]=0;
			input_layout_port1_data[input_layout_port1_count][3]=0;
			input_layout_port1_data[input_layout_port1_count][4]=input_zoom_port_data[zoom_num][9];
			input_layout_port1_data[input_layout_port1_count][5]=input_zoom_port_data[zoom_num][10];
			
			input_layout_port1_data[input_layout_port1_count][6]=0;
			input_layout_port1_data[input_layout_port1_count][7]=layout_port1_Y;
			input_layout_port1_data[input_layout_port1_count][8]=input_zoom_port_data[zoom_num][9];
			input_layout_port1_data[input_layout_port1_count][9]=layout_port1_Y+input_zoom_port_data[zoom_num][10];
			layout_port1_Y_pre=layout_port1_Y;
			layout_port1_X=input_zoom_port_data[zoom_num][9];
			layout_port1_X_flag=0;
			layout_port1_Y=layout_port1_Y_pre+input_zoom_port_data[zoom_num][10];	
			input_layout_port1_count++;
			return 2;
		}
		
	}
	else if(first_use==2)
	{
		
		if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_X+input_source_resolution[zoom_num].resolution_width)<=1920&&(layout_port1_Y_pre+input_source_resolution[zoom_num].resolution_high)<=2400)  //������2������Խ���
		{
			input_layout_port1_data[input_layout_port1_count][0]=zoom_num;
			input_layout_port1_data[input_layout_port1_count][1]=input_layout_port1_count+1;
			input_layout_port1_data[input_layout_port1_count][2]=0;
			input_layout_port1_data[input_layout_port1_count][3]=0;
			input_layout_port1_data[input_layout_port1_count][4]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port1_data[input_layout_port1_count][5]=input_source_resolution[zoom_num].resolution_high;
			
			input_layout_port1_data[input_layout_port1_count][6]=layout_port1_X;
			input_layout_port1_data[input_layout_port1_count][7]=layout_port1_Y_pre;
			input_layout_port1_data[input_layout_port1_count][8]=layout_port1_X+input_source_resolution[zoom_num].resolution_width;
			input_layout_port1_data[input_layout_port1_count][9]=layout_port1_Y_pre+input_source_resolution[zoom_num].resolution_high;
			if(!layout_port1_X_flag)
			{
				layout_port1_X+=input_source_resolution[zoom_num].resolution_width;
				layout_port1_X_flag=1;
				layout_port1_Y=layout_port1_Y_pre+input_source_resolution[zoom_num].resolution_high;
			}
			else
			{
				layout_port1_Y_pre+=input_source_resolution[zoom_num].resolution_high;
				if(layout_port1_Y_pre>=layout_port1_Y)
					layout_port1_Y=layout_port1_Y_pre;	
			}						
			input_layout_port1_count++;
			return 2;
		}
		else if((input_source_resolution[zoom_num].resolution_width)<=1920&&(layout_port1_Y+input_source_resolution[zoom_num].resolution_high)<=2400)  //������2������Խ���
		{
			input_layout_port1_data[input_layout_port1_count][0]=zoom_num;
			input_layout_port1_data[input_layout_port1_count][1]=input_layout_port0_count+1;
			input_layout_port1_data[input_layout_port1_count][2]=0;
			input_layout_port1_data[input_layout_port1_count][3]=0;
			input_layout_port1_data[input_layout_port1_count][4]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port1_data[input_layout_port1_count][5]=input_source_resolution[zoom_num].resolution_high;
			
			input_layout_port1_data[input_layout_port1_count][6]=0;
			input_layout_port1_data[input_layout_port1_count][7]=layout_port1_Y;
			input_layout_port1_data[input_layout_port1_count][8]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port1_data[input_layout_port1_count][9]=layout_port1_Y+input_source_resolution[zoom_num].resolution_high;
			layout_port1_Y_pre=layout_port1_Y;
			
			layout_port0_X=input_source_resolution[zoom_num].resolution_width;
			layout_port0_X_flag=0;
			layout_port1_Y=layout_port1_Y_pre+input_source_resolution[zoom_num].resolution_high;	
			input_layout_port1_count++;
			return 2;
		}
		
		else if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+input_source_resolution[zoom_num].resolution_width)<=1920&& (layout_port0_Y_pre+input_source_resolution[zoom_num].resolution_high)<=2400)  //������1������Խ���
		{
			input_layout_port0_data[input_layout_port0_count][0]=zoom_num;
			input_layout_port0_data[input_layout_port0_count][1]=input_layout_port0_count+1;
			input_layout_port0_data[input_layout_port0_count][2]=0;
			input_layout_port0_data[input_layout_port0_count][3]=0;
			input_layout_port0_data[input_layout_port0_count][4]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port0_data[input_layout_port0_count][5]=input_source_resolution[zoom_num].resolution_high;
			
			input_layout_port0_data[input_layout_port0_count][6]=layout_port0_X;
			input_layout_port0_data[input_layout_port0_count][7]=layout_port0_Y_pre;
			input_layout_port0_data[input_layout_port0_count][8]=layout_port0_X+input_source_resolution[zoom_num].resolution_width;
			input_layout_port0_data[input_layout_port0_count][9]=layout_port0_Y_pre+input_source_resolution[zoom_num].resolution_high;
			if(!layout_port0_X_flag)
			{
				layout_port0_X+=input_source_resolution[zoom_num].resolution_width;
				layout_port0_X_flag=1;
				layout_port0_Y=layout_port0_Y_pre+input_source_resolution[zoom_num].resolution_high;
			}
			else
			{				
				layout_port0_Y_pre+=input_source_resolution[zoom_num].resolution_high;
				if(layout_port0_Y_pre>=layout_port0_Y)
					layout_port0_Y=layout_port0_Y_pre;	
			}				
			input_layout_port0_count++;
			return 1;
		}
		else if((input_source_resolution[zoom_num].resolution_width)<=1920&& (layout_port0_Y+input_source_resolution[zoom_num].resolution_high)<=2400)  //������1������Խ���
		{
			input_layout_port0_data[input_layout_port0_count][0]= zoom_num;
			input_layout_port0_data[input_layout_port0_count][1]=input_layout_port0_count+1;
			input_layout_port0_data[input_layout_port0_count][2]=0;
			input_layout_port0_data[input_layout_port0_count][3]=0;
			input_layout_port0_data[input_layout_port0_count][4]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port0_data[input_layout_port0_count][5]=input_source_resolution[zoom_num].resolution_high;
			
			input_layout_port0_data[input_layout_port0_count][6]=0;
			input_layout_port0_data[input_layout_port0_count][7]=layout_port0_Y;
			input_layout_port0_data[input_layout_port0_count][8]=input_source_resolution[zoom_num].resolution_width;
			input_layout_port0_data[input_layout_port0_count][9]=layout_port0_Y+input_source_resolution[zoom_num].resolution_high;
			layout_port0_Y_pre=layout_port0_Y;
			layout_port1_X=input_source_resolution[zoom_num].resolution_width;
			layout_port1_X_flag=0;
			layout_port0_Y=layout_port0_Y_pre+input_source_resolution[zoom_num].resolution_high;	
			input_layout_port0_count++;
			return 1;
		}	
	
	}
	return 0;
	
}



u8 find_same_soure_to_outboard(char *return_num, u8 source)
{
	char i=0;
	if(input_layout_port1_count>0)
	{
		for(i=0;i<input_layout_port1_count;i++)
		{
			if(input_layout_port1_data[i][0]==source)
			{
				*return_num=i;
				return 2;
			}
		}
	}
	if(input_layout_port0_count>0)
	{
		for(i=0;i<input_layout_port0_count;i++)
		{
			if(input_layout_port0_data[i][0]==source)
			{
				*return_num=i;
				return 1;
			}
		}
	}
	return 0;
	
}


//��������������������Ҫ���ֵĻ������
u16 ouput_layout_port_data[4][8][10];
u8 ouput_layout_port_count[4]={0};




//
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
	u8 line_count=0;
	u8 col_count=0;
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&begin_x<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && end_x<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&begin_y<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && end_y<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�

	
		for(j=col;j<=col_end;j++)
		{	
			ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][0]=input_port_num-1;
//			ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][1]=ouput_layout_port_count[line*g_screen_column+j];
			if(end_x<=g_screen_total_pixel[0][j])
			{
				if(j==col)//first in function
				{
					if(input_port_num==1)
					{
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=input_layout_port0_data[layout_num][6];
					}
					else
					{
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=input_layout_port1_data[layout_num][6];
					}
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=end_x-begin_x+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
					
				}
				else
				{
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j-1][ouput_layout_port_count[line*g_screen_column+j-1]][4];
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=end_x-g_screen_total_pixel[0][j-1]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
				}
				
			}
			else
			{
				if(j==col)
				{
				if(input_port_num==1)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=input_layout_port0_data[layout_num][6];
				else
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=input_layout_port1_data[layout_num][6];
				
				ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=g_screen_total_pixel[0][j]-d_image_para[num][BEGIN_X_INDEX]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
				}
				else
				{
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j-1][ouput_layout_port_count[line*g_screen_column+j-1]][4];
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=g_screen_line_column_pixel[0][j]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];	
				}
				
			}
				if(j==0)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=begin_x;
				else if(j==col)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=begin_x-g_screen_total_pixel[0][j-1];
				else
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=0;
				ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][8]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]-ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6];			
		}
			for(j=line;j<=line_end;j++)
			{			
				if(end_y<=g_screen_total_pixel[1][j])
				{
					if(j==line)//first in function
					{
						if(input_port_num==1)
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=input_layout_port0_data[layout_num][7];
						else
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=input_layout_port1_data[layout_num][7];

						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=end_y-begin_y+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
					else
					{
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=ouput_layout_port_data[(j-1)*g_screen_column+col][ouput_layout_port_count[(j-1)*g_screen_column+col]][5];
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=end_y-g_screen_total_pixel[1][j-1]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
				}
				else
				{
					if(j==line)
					{
						if(input_port_num==1)
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=input_layout_port0_data[layout_num][7];
						else
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=input_layout_port1_data[layout_num][7];			
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=g_screen_total_pixel[1][j]-d_image_para[num][BEGIN_Y_INDEX]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
				
					}
					else
					{
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=ouput_layout_port_data[(j-1)*g_screen_column+col][ouput_layout_port_count[(j-1)*g_screen_column+col]][5];
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=g_screen_line_column_pixel[1][j]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
				}
	
				if(j==0)
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=begin_y;
				else if(j==line)
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=begin_y-g_screen_total_pixel[1][j-1];
				
				else
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=0;
				ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][9]=ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]-ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]+	ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7];	
		 }

			for(i=line;i<=line_end;i++)
			{
				for(j=col;j<=col_end;j++)
				{
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][0]=input_port_num-1;
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][1]=num+1;
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][3]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][3];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][5];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][7];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][8]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][8];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][9]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][9];
							
				}
			}
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				ouput_layout_port_count[i*g_screen_column+j]++;
			}
}


//��������������������ݽ���������֣����������˿�
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
	u8 line_count=0;
	u8 col_count=0;
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&d_image_para[num][BEGIN_X_INDEX]<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&d_image_para[num][BEGIN_Y_INDEX]<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�

	
		for(j=col;j<=col_end;j++)
		{	
			ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][0]=zoom_num+3;
			if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
			{
				if(j==col)//first in function
				{
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=output_zoom_port_data[zoom_num-3][5];
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=d_image_para[num][END_X_INDEX]-d_image_para[num][BEGIN_X_INDEX]+output_zoom_port_data[zoom_num-3][5];
					
				}
				else
				{
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j-1][ouput_layout_port_count[line*g_screen_column+j]][4];
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=d_image_para[num][END_X_INDEX]-g_screen_total_pixel[0][j-1]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
				}
				
			}
			else
			{
				if(j==col)
				{
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=output_zoom_port_data[zoom_num-3][5];	
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=g_screen_total_pixel[0][j]-d_image_para[num][BEGIN_X_INDEX]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
				}
				else
				{
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j-1][ouput_layout_port_count[line*g_screen_column+j]][4];
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]=g_screen_total_pixel[0][j]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];	
				}
				
			}
				if(j==0)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX];
				else if(j==col)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX]-g_screen_total_pixel[0][j-1];
				else
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6]=0;
				ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][8]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4]-ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2]+ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6];
			
			
		}

			for(j=line;j<=line_end;j++)
			{			
				if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][j])
				{
					if(j==line)//first in function
					{
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=output_zoom_port_data[zoom_num-3][7];	
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=d_image_para[num][END_Y_INDEX]-d_image_para[num][BEGIN_Y_INDEX]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
					else
					{
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=ouput_layout_port_data[(j-1)*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5];
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=d_image_para[num][END_Y_INDEX]-g_screen_total_pixel[1][j-1]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
				}
				else
				{
					if(j==line)
					{
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=output_zoom_port_data[zoom_num-3][7];
							ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=g_screen_total_pixel[1][j]-d_image_para[num][BEGIN_Y_INDEX]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];				
					}
					else
					{
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]=ouput_layout_port_data[(j-1)*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5];
						ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]=g_screen_total_pixel[1][j]+ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3];
					}
				}
	
				if(j==0)
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=d_image_para[num][BEGIN_Y_INDEX];
				else if(j==line)
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=d_image_para[num][BEGIN_Y_INDEX]-g_screen_total_pixel[1][j-1];
				
				else
					ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7]=0;
				ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][9]=ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][5]-ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][3]+	ouput_layout_port_data[j*g_screen_column+col][ouput_layout_port_count[j*g_screen_column+col]][7];	
		 }

			for(i=line;i<=line_end;i++)
			{
				for(j=col;j<=col_end;j++)
				{
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][0]=zoom_num+3;
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][1]=num+1;
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][2];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][3]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][3];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][4];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][5];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][6];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][7];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][8]=ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][8];
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][9]=ouput_layout_port_data[i*g_screen_column+col][ouput_layout_port_count[i*g_screen_column+col]][9];
							
				}
			}
		for(i=line;i<=line_end;i++)
		for(j=col;j<=col_end;j++)
		{
			ouput_layout_port_count[i*g_screen_column+j]++;
		}
}



void Clear_Output(u8 num)
{
		sprintf(d_780command_send_buf,"GP SDMI %d\r\n",num);
		command_send(d_780command_send_buf);
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 0\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 1\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 2\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 4\r\n");
		delay_ms(100);
		command_send("O_F SUSD 3\r\n");
		delay_ms(200);		
		command_send("GP SDMI 0\r\n");
		delay_ms(100);	
}



void TryEnd_Clear_Display(void)
{
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 0\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 1\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 2\r\n");
		delay_ms(100);
		command_send("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 4\r\n");
		delay_ms(100);
		command_send("O_F SUSD 3\r\n");
		delay_ms(200);
}



void output_set(void)
{
	u8 i=0;
	u8 j=0;
	for(i=0;i<input_zoom_port_used;i++)  //��ӡ�������������ֵ
	{	
			sprintf(d_780command_send_buf,"I_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",input_zoom_port_data[i][0],input_zoom_port_data[i][1],input_zoom_port_data[i][2],input_zoom_port_data[i][3],input_zoom_port_data[i][4],input_zoom_port_data[i][5],input_zoom_port_data[i][6],input_zoom_port_data[i][7],input_zoom_port_data[i][8],input_zoom_port_data[i][9],input_zoom_port_data[i][10]);
			command_send(d_780command_send_buf);
	}
	if(input_layout_port0_count>0)
	{
		sprintf(d_780command_send_buf,"I_F SWL 0 0 1920 2400\r\n");
		command_send(d_780command_send_buf);
		for(i=0;i<input_layout_port0_count;i++)
		{
				sprintf(d_780command_send_buf,"I_F AW %d %d %d %d %d %d %d %d %d %d\r\n",input_layout_port0_data[i][0],input_layout_port0_data[i][1],input_layout_port0_data[i][2],input_layout_port0_data[i][3],input_layout_port0_data[i][4],input_layout_port0_data[i][5],input_layout_port0_data[i][6],input_layout_port0_data[i][7],input_layout_port0_data[i][8],input_layout_port0_data[i][9]);		
				command_send(d_780command_send_buf);
			delay_ms(10);
		}
		sprintf(d_780command_send_buf,"I_F AWE\r\n");
		command_send(d_780command_send_buf);
	}
	
	if(input_layout_port1_count>0)
	{
		sprintf(d_780command_send_buf,"I_F SWL 0 1 1920 2400\r\n");
		command_send(d_780command_send_buf);
	
		for(i=0;i<input_layout_port1_count;i++)
		{
				sprintf(d_780command_send_buf,"I_F AW %d %d %d %d %d %d %d %d %d %d\r\n",input_layout_port1_data[i][0],input_layout_port1_data[i][1],input_layout_port1_data[i][2],input_layout_port1_data[i][3],input_layout_port1_data[i][4],input_layout_port1_data[i][5],input_layout_port1_data[i][6],input_layout_port1_data[i][7],input_layout_port1_data[i][8],input_layout_port1_data[i][9]);		
				command_send(d_780command_send_buf);
		}
		sprintf(d_780command_send_buf,"I_F AWE\r\n");
		command_send(d_780command_send_buf);
	}
	for(i=0;i<output_zoom_port_used;i++)  //��ӡ�������������ֵ
	{	
			sprintf(d_780command_send_buf,"O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",output_zoom_port_data[i][0],output_zoom_port_data[i][1],output_zoom_port_data[i][2],output_zoom_port_data[i][3],output_zoom_port_data[i][4],output_zoom_port_data[i][5],output_zoom_port_data[i][6],output_zoom_port_data[i][7],output_zoom_port_data[i][8],output_zoom_port_data[i][9],output_zoom_port_data[i][10]);
			command_send(d_780command_send_buf);
	}
	for(i=0;i<4;i++)
	{
		if(ouput_layout_port_count[i]!=0)
		{
			sprintf(d_780command_send_buf,"O_F SWL 3 %d 1920 1080\r\n",i);
			command_send(d_780command_send_buf);
			for(j=0;j<ouput_layout_port_count[i];j++)
			{
				sprintf(d_780command_send_buf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",ouput_layout_port_data[i][j][0],ouput_layout_port_data[i][j][1],ouput_layout_port_data[i][j][2],ouput_layout_port_data[i][j][3],ouput_layout_port_data[i][j][4],ouput_layout_port_data[i][j][5],ouput_layout_port_data[i][j][6],ouput_layout_port_data[i][j][7],ouput_layout_port_data[i][j][8],ouput_layout_port_data[i][j][9]);				
				command_send(d_780command_send_buf);
			}
			sprintf(d_780command_send_buf,"O_F AWE\r\n",i);
			command_send(d_780command_send_buf);
		}
	}
	for(i=0;i<4;i++)
	{
		if(ouput_layout_port_count[i]==0)
		{
			sprintf(d_780command_send_buf,"OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 %d\r\n",i);
			command_send(d_780command_send_buf);
		}
	}
//	���� ���뿨������ʾ
//�����������������뿨�Ĳ������£�ֻ�иð忨�Ĳ������º󣬰忨�Ž��µ����ò�����Ч��
//INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY [InputSlotIndex]
	sprintf(d_780command_send_buf,"I_F SUSD 0\r\n");
	command_send(d_780command_send_buf);
	sprintf(d_780command_send_buf,"O_F SUSD 3\r\n");
	command_send(d_780command_send_buf);
}


//����ǰ���ñ��浽����
void save_scenario_to_FPGA(u8 num)
{
	sprintf(d_780command_send_buf,"GP SDMI %d\r\n",num);
	command_send(d_780command_send_buf);
	output_set();
	GpuSend("SBC(7);");
	GpuSend("CBOF(10,45,200,130,7,7);");

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��7��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��6��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��5��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��4��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��3��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��2��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);

	sprintf(d_780command_send_buf,"DS16(20,80,'����%d���ڱ��桭��1��',0);\r\n",num);
	GpuSend(d_780command_send_buf);
	delay_ms(1000);
	

//	���� ������ʾЧ���ı�������
//	GP SDMI [DisplayModeIndex]
//	DisplayModeIndex����ʾЧ�����浽�������ĵ�ַ������ȡֵ0~19
	sprintf(d_780command_send_buf,"GP SDMI 0\r\n");
	command_send(d_780command_send_buf);

}

u8 load_scenario_from_FPGA(u8 num)
{
	sprintf(d_780command_send_buf,"GP LDM %d\r\n",num);
	command_send(d_780command_send_buf);
	delay_ms(100);
	command_send(d_780command_send_buf);
	return 1;
}



u8 how_zoom_need(u16 num)
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
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&d_image_para[num][BEGIN_X_INDEX]<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&d_image_para[num][BEGIN_Y_INDEX]<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
	
	return (line_end-line+1)*(col_end-col+1);  //�������Ҫ��������������ʵ�ʾ���Ҫ��ʾ�����ٸ������
	
}

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
	u8 line_count=0;
	u8 col_count=0;
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&d_image_para[num][BEGIN_X_INDEX]<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&d_image_para[num][BEGIN_Y_INDEX]<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
	for(i=line;i<=line_end;i++)
	{
		for(j=col;j<=col_end;j++)
		{
			if(i==line)
			{
				if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][line])
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][5]=d_image_para[num][END_Y_INDEX]-d_image_para[num][BEGIN_Y_INDEX];
				else
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][5]=g_screen_total_pixel[1][line]-d_image_para[num][BEGIN_Y_INDEX];
				
				if(line>0)
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][7]=d_image_para[num][BEGIN_Y_INDEX]-g_screen_total_pixel[1][line-1];
				else						
					ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][7]=d_image_para[num][BEGIN_Y_INDEX];
		
				if(port==1)
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][3]=input_layout_port0_data[port_num][7];
				else if(port==2)
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][3]=input_layout_port1_data[port_num][7];
			
				
			}
			else
			{
				if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
				{
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5]=d_image_para[num][END_Y_INDEX]-g_screen_total_pixel[1][i-1];
				
				}
				else
				{
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[j*g_screen_column+j]][5]=g_screen_total_pixel[1][i]-g_screen_total_pixel[1][i-1];
				}
				ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]=0;
				ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][3]=ouput_layout_port_data[i*g_screen_column+j-1][ouput_layout_port_count[i*g_screen_column+j-1]][3];
				
			}					
			if(j==col)
			{			
				if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4]=d_image_para[num][END_X_INDEX]-d_image_para[num][BEGIN_X_INDEX];
				else
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4]=g_screen_total_pixel[0][col]-d_image_para[num][BEGIN_X_INDEX];	
				if(col>0)
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX]-g_screen_total_pixel[0][j-1];
				else						
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX];
			if(port==1)
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=input_layout_port0_data[port_num][6];
			else if(port==2)
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=input_layout_port1_data[port_num][6];
		
			}
			else
			{
					if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
					{
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[j*g_screen_column+j]][4]=d_image_para[num][END_X_INDEX]-g_screen_total_pixel[0][j-1];	
					}
					else
					{
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[j*g_screen_column+j]][4]=g_screen_total_pixel[0][j]-g_screen_total_pixel[0][j-1];
					}
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[j*g_screen_column+j]][6]=0;  //��ʾ��0�㿪ʼ
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=ouput_layout_port_data[i*g_screen_column+j-1][ouput_layout_port_count[i*g_screen_column+j-1]][2];
			}
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][8]=ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]+ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4];
		
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][0]=port-1; //�ź�Դ
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][1]=num+1; //�ź�Դ
				
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][9]=ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]+ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5] ;
			}			
		}
		
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				ouput_layout_port_count[i*g_screen_column+j]++;
			}
}


//port ������ʾ�˴��������������ֿ���һ�������ź�Դ
//source ����������������˿�ָ�����ź���Դ
//num ��ʾ���ǻ������Ⱥ�
void outputboard_zoom_and_layout(u8 port,u8 port_num ,u8 num)  //��������Ҫ�Ŵ󣬴������ʹ�����������зŴ�����еĲ������
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
	u8 line_count=0;
	u8 col_count=0;
	u8 output_zoom_port_used_pre=output_zoom_port_used;
	
	u16 cur_Y_point=0;
	
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&d_image_para[num][BEGIN_X_INDEX]<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&d_image_para[num][BEGIN_Y_INDEX]<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
	for(i=line;i<=line_end;i++)
	{	
		if(i>=line)
		{
			cur_Y_point=output_zoom_port_data[output_zoom_port_used-1][8]+1;
		}
		for(j=col;j<=col_end;j++)
		{	
				if(i==line)
				{
					if(port==1)
						output_zoom_port_data[output_zoom_port_used][7]=input_layout_port0_data[port_num][7];
					else if(port==2)
						output_zoom_port_data[output_zoom_port_used][7]=input_layout_port1_data[port_num][7];
										
					if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][line])
					{
						output_zoom_port_data[output_zoom_port_used][10]=d_image_para[num][END_Y_INDEX]-d_image_para[num][BEGIN_Y_INDEX];
						output_zoom_port_data[output_zoom_port_used][8]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high-1;
					}
					else
					{
						output_zoom_port_data[output_zoom_port_used][10]=g_screen_total_pixel[1][line]-d_image_para[num][BEGIN_Y_INDEX];
						output_zoom_port_data[output_zoom_port_used][8]=output_zoom_port_data[output_zoom_port_used][7]+g_screen_line_column_pixel[1][i]*input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high/d_image_para[num][HIGH_INDEX]-1;
					}
					
					
					if(line>0)
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][7]=d_image_para[num][BEGIN_Y_INDEX]-g_screen_total_pixel[1][line-1];
					else						
						ouput_layout_port_data[line*g_screen_column+j][ouput_layout_port_count[line*g_screen_column+j]][7]=d_image_para[num][BEGIN_Y_INDEX];
				}
				else
				{
					if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
					{
						output_zoom_port_data[output_zoom_port_used][7]=cur_Y_point;
						output_zoom_port_data[output_zoom_port_used][8]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high-1;
						output_zoom_port_data[output_zoom_port_used][10]=d_image_para[num][END_Y_INDEX]-g_screen_total_pixel[1][i-1];
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]=0;
					}
					else
					{
						output_zoom_port_data[output_zoom_port_used][7]=cur_Y_point;
						output_zoom_port_data[output_zoom_port_used][8]=output_zoom_port_data[output_zoom_port_used][7]+g_screen_line_column_pixel[1][i]*input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high/d_image_para[num][HIGH_INDEX]-1; 
						output_zoom_port_data[output_zoom_port_used][10]=g_screen_line_column_pixel[1][i];
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]=0;
					}
				}
	
			output_zoom_port_data[output_zoom_port_used][0]=OUTPUT_BOARD3;
			output_zoom_port_data[output_zoom_port_used][1]=output_zoom_port_used;	
			output_zoom_port_data[output_zoom_port_used][2]=port-1;//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			output_zoom_port_data[output_zoom_port_used][3]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
			output_zoom_port_data[output_zoom_port_used][4]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high;//	
			
			if(j==col)
			{	
				if(port==1)
					output_zoom_port_data[output_zoom_port_used][5]=input_layout_port0_data[port_num][6];
				else if(port==2)
					output_zoom_port_data[output_zoom_port_used][5]=input_layout_port1_data[port_num][6];
				
				if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
				{
					output_zoom_port_data[output_zoom_port_used][9]=d_image_para[num][END_X_INDEX]-d_image_para[num][BEGIN_X_INDEX];
					output_zoom_port_data[output_zoom_port_used][6]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
				}
				else
				{
					output_zoom_port_data[output_zoom_port_used][9]=g_screen_total_pixel[0][col]-d_image_para[num][BEGIN_X_INDEX];	
					output_zoom_port_data[output_zoom_port_used][6]=(g_screen_total_pixel[0][col]-d_image_para[num][BEGIN_X_INDEX])*input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width/d_image_para[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
				}
			 if(col>0)
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX]-g_screen_total_pixel[0][col-1];
				else						
					ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=d_image_para[num][BEGIN_X_INDEX];
				
			}
			else
			{
					if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
					{
						output_zoom_port_data[output_zoom_port_used][5]=output_zoom_port_data[output_zoom_port_used-1][6]+1;
						output_zoom_port_data[output_zoom_port_used][6]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width-1;
						output_zoom_port_data[output_zoom_port_used][9]=d_image_para[num][END_X_INDEX]-g_screen_total_pixel[0][j-1];
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=0; //��ʾ��0�㿪ʼ
					}
					else
					{
						output_zoom_port_data[output_zoom_port_used][5]=output_zoom_port_data[output_zoom_port_used-1][6]+1;
						output_zoom_port_data[output_zoom_port_used][6]=g_screen_line_column_pixel[0][j]*input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width/d_image_para[num][WIDTH_INDEX]+output_zoom_port_data[output_zoom_port_used][5]-1; 
						output_zoom_port_data[output_zoom_port_used][9]=g_screen_line_column_pixel[0][j];
						ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]=0;  //��ʾ��0�㿪ʼ
					}
			}
	
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][0]=output_zoom_port_used+4; //�ź�Դ
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][1]=num+1; //�ź�Դ
			
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][2]=0;
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][3]=0;
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4]=output_zoom_port_data[output_zoom_port_used][9];
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5]=output_zoom_port_data[output_zoom_port_used][10];
		
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][8]=ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][6]+ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][4];	
			ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][9]=ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][7]+ouput_layout_port_data[i*g_screen_column+j][ouput_layout_port_count[i*g_screen_column+j]][5] ;
			output_zoom_port_used++;				
		}
	}
		
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				ouput_layout_port_count[i*g_screen_column+j]++;
			}
}



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
	u8 line_count=0;
	u8 col_count=0;
	u8 select_inputboard_layout_port=1;
	char select_inputboard_layout_port_num=0;
	u8 input_zoom_port_used_pre=input_zoom_port_used;
	
	u16 curimage_begin_x=0;
	u16 curimage_begin_y=0;
	u16 curimage_end_x=0;
	u16 curimage_end_y=0;
	
	u16 cur_Y_point=0;
	for(i=0;i<g_screen_column;i++)
	{
			if(col_flag==0&&d_image_para[num][BEGIN_X_INDEX]<g_screen_total_pixel[0][i] )
			{
				col=i;
				col_flag=1;
			}
			if(col_end_flag==0 && d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][i])
			{
				col_end=i;
				col_end_flag=1;
			}
	}
	for(i=0;i<g_screen_line;i++)
	{
			if(line_flag==0&&d_image_para[num][BEGIN_Y_INDEX]<g_screen_total_pixel[1][i] )
			{
				line=i;
				line_flag=1;
			}
			if(line_end_flag==0 && d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
			{
				line_end=i;
				line_end_flag=1;
			}
	}
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
	for(i=line;i<=line_end;i++)
	{
		if(i>line)
		{
			cur_Y_point=input_zoom_port_data[input_zoom_port_used-1][8]+1;
		}
		for(j=col;j<=col_end;j++)
		{
			if(i==line)
			{
				input_zoom_port_data[input_zoom_port_used][7]=0;				
				curimage_begin_y=d_image_para[num][BEGIN_Y_INDEX];
				
				if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][line])
				{	
					input_zoom_port_data[input_zoom_port_used][10]=d_image_para[num][END_Y_INDEX]-d_image_para[num][BEGIN_Y_INDEX];
					input_zoom_port_data[input_zoom_port_used][8]=input_source_resolution[num].resolution_high-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
				}
				else
				{					
					input_zoom_port_data[input_zoom_port_used][10]=g_screen_total_pixel[1][line]-d_image_para[num][BEGIN_Y_INDEX];
					input_zoom_port_data[input_zoom_port_used][8]=input_zoom_port_data[input_zoom_port_used][7]+(g_screen_total_pixel[1][line]-d_image_para[num][BEGIN_Y_INDEX])*input_source_resolution[num].resolution_high/d_image_para[num][HIGH_INDEX]-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
				}
	
			}
			else
			{
				curimage_begin_y=g_screen_total_pixel[1][i-1];
				if(d_image_para[num][END_Y_INDEX]<=g_screen_total_pixel[1][i])
				{
					input_zoom_port_data[input_zoom_port_used][7]=cur_Y_point;
					input_zoom_port_data[input_zoom_port_used][8]=input_source_resolution[num].resolution_high-1; 
					input_zoom_port_data[input_zoom_port_used][10]=d_image_para[num][END_Y_INDEX]-g_screen_total_pixel[1][i-1];
				}
				else
				{
					input_zoom_port_data[input_zoom_port_used][7]=cur_Y_point;
					input_zoom_port_data[input_zoom_port_used][8]=input_zoom_port_data[input_zoom_port_used][7]+g_screen_line_column_pixel[1][i]*input_source_resolution[num].resolution_high/d_image_para[num][HIGH_INDEX]-1;
					input_zoom_port_data[input_zoom_port_used][10]=g_screen_line_column_pixel[1][i];
				}
				
			}
				
			input_zoom_port_data[input_zoom_port_used][0]=INPUT_BOARD0;
			input_zoom_port_data[input_zoom_port_used][1]=input_zoom_port_used;	
			input_zoom_port_data[input_zoom_port_used][2]=d_image_para[num][SOURCE_IN_INDEX];//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			input_zoom_port_data[input_zoom_port_used][3]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
			input_zoom_port_data[input_zoom_port_used][4]=input_source_resolution[d_image_para[num][SOURCE_IN_INDEX]].resolution_high;//	
			
			if(j==col)
			{	
				input_zoom_port_data[input_zoom_port_used][5]=0;
				
						
			
				curimage_begin_x=d_image_para[num][BEGIN_X_INDEX];
				
				if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
				{
					input_zoom_port_data[input_zoom_port_used][9]=d_image_para[num][END_X_INDEX]-d_image_para[num][BEGIN_X_INDEX];
					input_zoom_port_data[input_zoom_port_used][6]=input_source_resolution[num].resolution_width-1;
				}
				else
				{
					input_zoom_port_data[input_zoom_port_used][9]=g_screen_total_pixel[0][col]-d_image_para[num][BEGIN_X_INDEX];
					input_zoom_port_data[input_zoom_port_used][6]=(g_screen_total_pixel[0][col]-d_image_para[num][BEGIN_X_INDEX])*input_source_resolution[num].resolution_width/d_image_para[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ						
				}
			}
			else
			{
					curimage_begin_x=g_screen_total_pixel[0][j-1];
					if(d_image_para[num][END_X_INDEX]<=g_screen_total_pixel[0][j])
					{
						input_zoom_port_data[input_zoom_port_used][5]=input_zoom_port_data[input_zoom_port_used-1][6]+1;
						input_zoom_port_data[input_zoom_port_used][6]=input_source_resolution[num].resolution_width-1;
						input_zoom_port_data[input_zoom_port_used][9]=d_image_para[num][END_X_INDEX]-g_screen_total_pixel[0][j-1];
					}
					else
					{
						input_zoom_port_data[input_zoom_port_used][5]=input_zoom_port_data[input_zoom_port_used-1][6]+1;
						input_zoom_port_data[input_zoom_port_used][6]=input_zoom_port_data[input_zoom_port_used][5]+g_screen_line_column_pixel[0][j]*input_source_resolution[num].resolution_width/d_image_para[num][WIDTH_INDEX]-1; 
						input_zoom_port_data[input_zoom_port_used][9]=g_screen_line_column_pixel[0][j];
					}
			}
			//���½�������������ڲ���//
			select_inputboard_layout_port=inputboard_out_layout(input_zoom_port_used,1);
			curimage_end_x=curimage_begin_x+input_zoom_port_data[input_zoom_port_used][9];
			curimage_end_y=curimage_begin_y+input_zoom_port_data[input_zoom_port_used][10];
			input_zoom_port_used++;
			if(select_inputboard_layout_port==1)
				select_inputboard_layout_port_num=input_layout_port0_count-1;
			else if(select_inputboard_layout_port==2)
				select_inputboard_layout_port_num=input_layout_port1_count-1;
			ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,num,curimage_begin_x,curimage_begin_y,curimage_end_x,curimage_end_y);	
		}	
	}	
}




void image_layout(void)
{
	u8 i=0;
	u8 zoom_need_j=0;
	u8 zoom_need=0;
	char  source_to_outboard_num=0;
	char select_inputboard_layout_port_num=0;
	u8 select_inputboard_layout_port=1;
	u8 find_same_input_zoom=0; //������ͬ������
	u8 same_output_zoom_num=0;
	for(i=0 ;i<4;i++)
	{	
		ouput_layout_port_count[i]=0;
	}
	layout_port0_X=0;
	layout_port0_X_flag=0;
	layout_port0_Y=0;
	layout_port0_Y_pre=0;
	layout_port1_X=0;
	layout_port1_X_flag=0;
	layout_port1_Y=0;
	layout_port1_Y_pre=0;
	input_layout_port0_count=0;
	input_layout_port1_count=0;
	
//	d_image_para[0][0]=0;
//	d_image_para[0][1]=0;
//	d_image_para[0][2]=7680;
//	d_image_para[0][3]=1080;
//	d_image_para[0][4]=7680;
//	d_image_para[0][5]=1080;
//	d_image_para[0][6]=0;
//	d_image_para[0][7]=1;
//	d_image_para[0][8]=0;

//	d_image_para[1][0]=0;
//	d_image_para[1][1]=100;
//	d_image_para[1][2]=7680;
//	d_image_para[1][3]=800;
//	d_image_para[1][4]=7680;
//	d_image_para[1][5]=700;
//	d_image_para[1][6]=1;
//	d_image_para[1][7]=2;
//	d_image_para[1][8]=0;
//	
//	d_image_para[2][0]=22;
//	d_image_para[2][1]=70;
//	d_image_para[2][2]=268;
//	d_image_para[2][3]=480;
//	d_image_para[2][4]=246;
//	d_image_para[2][5]=410;
//	d_image_para[2][6]=0;
//	d_image_para[2][7]=3;
//	d_image_para[2][8]=0;

//	d_image_para[3][0]=1876;
//	d_image_para[3][1]=50;
//	d_image_para[3][2]=2130;
//	d_image_para[3][3]=496;
//	d_image_para[3][4]=254;
//	d_image_para[3][5]=446;
//	d_image_para[3][6]=1;
//	d_image_para[3][7]=4;
//	d_image_para[3][8]=0;
	
	input_zoom_port_used=0;
	output_zoom_port_used=0;
	source_to_outboard_num=0;
	for(i=0;i<IMAGE_TOTAL_NUM;i++) //����ͼ������
	{
		if(d_image_para[i][LAYER_INDEX]!=0) //ͼ��Ų�����0����ʾ��ͼ�����
		{
			if(d_image_para[i][WIDTH_INDEX]<input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_width && d_image_para[i][HIGH_INDEX]<=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_high
				||d_image_para[i][WIDTH_INDEX]<=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_width && d_image_para[i][HIGH_INDEX]<input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_high)  //��ǰͼ���ͼ��ߴ�С��
			{
				find_same_input_zoom=same_input_zoom(i);
				if(find_same_input_zoom==0)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
				{		
					if(input_zoom_port_used<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
					{						
						input_zoom_port_data[input_zoom_port_used][0]=INPUT_BOARD0;
						input_zoom_port_data[input_zoom_port_used][1]=input_zoom_port_used;	
						input_zoom_port_data[input_zoom_port_used][2]=d_image_para[i][SOURCE_IN_INDEX];//�����ź�Դ
						///////////////////////////////////////
						//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
						input_zoom_port_data[input_zoom_port_used][3]=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
						input_zoom_port_data[input_zoom_port_used][4]=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_high;//
						input_zoom_port_data[input_zoom_port_used][5]=0;//
						input_zoom_port_data[input_zoom_port_used][6]=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_width-1;//
						input_zoom_port_data[input_zoom_port_used][7]=0;//
						input_zoom_port_data[input_zoom_port_used][8]=input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_high-1;//
						/////////////////////////////////////////						
						input_zoom_port_data[input_zoom_port_used][9]=d_image_para[i][4];//����Ŀ�
						input_zoom_port_data[input_zoom_port_used][10]=d_image_para[i][5];//����ĸ�	
					
						//���½�������������ڲ���//
						select_inputboard_layout_port=inputboard_out_layout(input_zoom_port_used,1);
						
						input_zoom_port_used++;
						if(select_inputboard_layout_port==1)
							select_inputboard_layout_port_num=input_layout_port0_count-1;
						else if(select_inputboard_layout_port==2)
							select_inputboard_layout_port_num=input_layout_port1_count-1;
						if(select_inputboard_layout_port>0)
							ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,d_image_para[i][BEGIN_X_INDEX],d_image_para[i][BEGIN_Y_INDEX],d_image_para[i][END_X_INDEX],d_image_para[i][END_Y_INDEX]);
					}	
					else  //�޿ɷ����������
					{
						//
						//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,d_image_para[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(d_image_para[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=input_layout_port0_count-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=input_layout_port1_count-1;							
						}
						if(select_inputboard_layout_port>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽����壬�ͷ���ͼ��
						{
							same_output_zoom_num=same_output_zoom(select_inputboard_layout_port,source_to_outboard_num,i);
							if(!same_output_zoom_num)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
							{		
								//����˴�˵�����԰�Դ�źŴ��������						
								if(output_zoom_port_used<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
								{											
									output_zoom_port_data[input_zoom_port_used][0]=OUTPUT_BOARD3;
									output_zoom_port_data[input_zoom_port_used][1]=output_zoom_port_used;	
									output_zoom_port_data[input_zoom_port_used][2]=select_inputboard_layout_port;//�����ź�Դ
									///////////////////////////////////////
									//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
									output_zoom_port_data[input_zoom_port_used][3]=1920;// ����Դ�ֱ���
									output_zoom_port_data[input_zoom_port_used][4]=2400;//
									if(select_inputboard_layout_port==1) //�ڲ�����1
									{
										output_zoom_port_data[input_zoom_port_used][5]=input_layout_port0_data[source_to_outboard_num][6];//
										output_zoom_port_data[input_zoom_port_used][6]=input_layout_port0_data[source_to_outboard_num][8]-1;//
										output_zoom_port_data[input_zoom_port_used][7]=input_layout_port0_data[source_to_outboard_num][7];;//
										output_zoom_port_data[input_zoom_port_used][8]=input_layout_port0_data[source_to_outboard_num][9]-1;//
									}
									else	if(select_inputboard_layout_port==2) //�ڲ�����2
									{
										output_zoom_port_data[input_zoom_port_used][5]=input_layout_port1_data[source_to_outboard_num][6];//
										output_zoom_port_data[input_zoom_port_used][6]=input_layout_port1_data[source_to_outboard_num][8]-1;//
										output_zoom_port_data[input_zoom_port_used][7]=input_layout_port1_data[source_to_outboard_num][7];;//
										output_zoom_port_data[input_zoom_port_used][8]=input_layout_port1_data[source_to_outboard_num][9]-1;//
									}
									/////////////////////////////////////////						
									output_zoom_port_data[input_zoom_port_used][9]=d_image_para[i][4];//����Ŀ�
									output_zoom_port_data[input_zoom_port_used][10]=d_image_para[i][5];//����ĸ�	
									
									same_output_zoom_num=output_zoom_port_used;
									output_zoom_port_used++;
									
									outputboard_zoom_out_layout(same_output_zoom_num,i);//�ô������ 0����ʱ�ŵ�
								}						
								else//ͼ���ù����Ժ�û���������������ţ���ֱ�Ӱ�����Դ�ź�������Ͻ�
								{	
									ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,d_image_para[i][BEGIN_X_INDEX],d_image_para[i][BEGIN_Y_INDEX],d_image_para[i][END_X_INDEX],d_image_para[i][END_Y_INDEX]);
								}							
							}
							else  //������ͬ������
							{
								outputboard_zoom_out_layout(same_output_zoom_num,i);
							}					
						}		
					}						
				}
				else //�Ѿ�����һ����ͬ������
				{
					//������ͬ��������λ��
					find_same_soure_to_outboard(&select_inputboard_layout_port_num,find_same_input_zoom+4);
					ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,d_image_para[i][BEGIN_X_INDEX],d_image_para[i][BEGIN_Y_INDEX],d_image_para[i][END_X_INDEX],d_image_para[i][END_Y_INDEX]);

				}
				
			}
			else if(d_image_para[i][WIDTH_INDEX]>input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_width || d_image_para[i][HIGH_INDEX]>input_source_resolution[d_image_para[i][SOURCE_IN_INDEX]].resolution_high) //��Ҫ�Ŵ�
			{
					
				zoom_need=how_zoom_need(i);
				if((4-output_zoom_port_used)>=zoom_need)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
				{																							
				//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,d_image_para[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(d_image_para[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=input_layout_port0_count-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=input_layout_port1_count-1;							
						}
						if(select_inputboard_layout_port>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽�����
						{					
							outputboard_zoom_and_layout(select_inputboard_layout_port,source_to_outboard_num,i);										
						}	
						else if((4-input_zoom_port_used)>=zoom_need) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
						{
							inputboard_zoom_and_layout(i);
						}								
				}
				else if((4-input_zoom_port_used)>=zoom_need) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
				{
					 inputboard_zoom_and_layout(i);
				}	
				else
				{
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,d_image_para[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(d_image_para[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=input_layout_port0_count-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=input_layout_port1_count-1;							
						}
						if(select_inputboard_layout_port>0)  //����������϶�û�����������ã��ֿ��԰�ԭ�źŷ��͵�����壬�򲻽������ţ���Ե���ʾ
						{
								ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,d_image_para[i][BEGIN_X_INDEX],d_image_para[i][BEGIN_Y_INDEX],d_image_para[i][END_X_INDEX],d_image_para[i][END_Y_INDEX]);								
						}	
				}				
			}
			else //ͼ�����������һ�������÷�����������ֱ�Ӵ����������з־���ʾ
			{
				//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
				select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,d_image_para[i][6]);
				if(select_inputboard_layout_port==0) //û��
				{
					select_inputboard_layout_port=inputboard_out_layout(d_image_para[i][SOURCE_IN_INDEX],2);//���в���
					if(select_inputboard_layout_port==1)source_to_outboard_num=input_layout_port0_count-1;
					else if(select_inputboard_layout_port==2)source_to_outboard_num=input_layout_port1_count-1;							
				}
				if(select_inputboard_layout_port>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽�����
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



