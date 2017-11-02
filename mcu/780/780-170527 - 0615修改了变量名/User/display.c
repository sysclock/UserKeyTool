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


 //����һ�����飬��Ź�IMAGE_TOTAL_NUM������Ĳ���
 //����������ʾ����ʼλλ�á�����λ�á���ȡ��߶Ⱥ��ź�Դ���ⲿ�����0~3����ͼ��ţ�0��ͼ���ʾ�����ã����Ƿ������Դ


u8 scheme_save[9]={0};

u8 g_Mat_InPort_No;
u8 g_Mat_OutPort_No;
u16 gMatImgPara[4][IMAGE_PARA_NUM];


u16 gInImgPara[IMAGE_PARA_NUM];

u16 gSplitImgPara[IMAGE_TOTAL_NUM][IMAGE_PARA_NUM];

//�ܵĻ�������--ÿ������Ĳ��� ����յ�ˮƽ��ֱԴͷ����
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
void Read_TCPIP(void) //��ȡIP��Ϣ
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
void input_RISR(void) //��ȡ����ֱ���
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


//�������������ĸ���������ʹ�������ʹ��ʱ���
//INPUT_FPGA SCALER_SETUP [BoardIndex] [ScalerIndex] [SourceIndex] [InHoriSize] 
//[InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]�
u16 IF_Scaler_Para[IN_BOARD_SCALER_NUM][IF_SCALER_PARA_NUM]={0};

//			OUTPUT_FPGA SCALER_SETUP [OutputSlotIndex] [ScalerIndex] [SourceIndex] [InHoriSize] [InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]
//			OutputSlotIndex: ����忨�����ţ�ȡֵ0-3
//			ScalerIndex:����������������ţ�ȡֵ0-3���������4����������
//			SourceIndex���ź�Դ�����ţ�ȡֵ0-3�������ԭʼ�����źš�
//			InHoriSize������������ͼ��ĳ��ȣ���ֱ���Ϊ1920x1080���źţ�InHoriSizeΪ1920��
//			InVertSize������������ͼ��ĸ߶ȣ���ֱ���Ϊ1920x1080���źţ�InVertSizeΪ1080��
//			InFirstPixel��������ȡ����ͼ��X��ʼλ�ã�ȡֵ0- ��InHoriSize-1��
//			InLastPixel��������ȡ����ͼ��X����λ�ã�ȡֵ0- ��InHoriSize-1��
//			InFirstLine��������ȡ����ͼ��Y��ʼλ�ã�ȡֵ0- ��InVertSize -1��
//			InLastLine��������ȡ����ͼ��Y����λ�ã�ȡֵ0- ��InVertSize -1��
//			OutHoriSize: ���������ͼ��ĳ���
//			OutVertSize�����������ͼ��ĸ߶�
//�����������������	
//sprintf(gCmdBuf,"O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",OF_Scaler_Para[i][0],OF_Scaler_Para[i][1],OF_Scaler_Para[i][2],OF_Scaler_Para[i][3],OF_Scaler_Para[i][4],OF_Scaler_Para[i][5],OF_Scaler_Para[i][6],OF_Scaler_Para[i][7],OF_Scaler_Para[i][8],OF_Scaler_Para[i][9],OF_Scaler_Para[i][10]);


u16 OF_Scaler_Para[IN_BOARD_SCALER_NUM][IF_SCALER_PARA_NUM]={0};

//��������������������Ҫ���ֵĻ������
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


//�������ʲô��;
u8 inputboard_out_layout(u8 zoom_num,u8 first_use)
{
	if(first_use==1)
	{	
		if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+IF_Scaler_Para[zoom_num][9])<=1920&& (layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10])<=2400)  //������1������Խ���
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
		else if((IF_Scaler_Para[zoom_num][9])<=1920&& (layout_port0_Y+IF_Scaler_Para[zoom_num][10])<=2400)  //������1������Խ���
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
		else if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_Y_pre<layout_port1_Y)&&(layout_port1_X+IF_Scaler_Para[zoom_num][9])<=1920&&(layout_port1_Y_pre+IF_Scaler_Para[zoom_num][10])<=2400)  //������2������Խ���
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
		else if((IF_Scaler_Para[zoom_num][9])<=1920&&(layout_port1_Y+IF_Scaler_Para[zoom_num][10])<=2400)  //������2������Խ���
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
		
		if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_X+InSrcRes[zoom_num].resolution_width)<=1920&&(layout_port1_Y_pre+InSrcRes[zoom_num].resolution_high)<=2400)  //������2������Խ���
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
		else if((InSrcRes[zoom_num].resolution_width)<=1920&&(layout_port1_Y+InSrcRes[zoom_num].resolution_high)<=2400)  //������2������Խ���
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
		
		else if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+InSrcRes[zoom_num].resolution_width)<=1920&& (layout_port0_Y_pre+InSrcRes[zoom_num].resolution_high)<=2400)  //������1������Խ���
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
		else if((InSrcRes[zoom_num].resolution_width)<=1920&& (layout_port0_Y+InSrcRes[zoom_num].resolution_high)<=2400)  //������1������Խ���
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


//��������������������Ҫ���ֵĻ������
//sprintf(gCmdBuf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3],OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);			
//OUTPUT_FPGA SET_WINDOWS_LAYOUT [OutputSlotIndex] [BackgroundIndex] [ScreenWidth] [ScreenHeight]
//OUTPUT_FPGA ADD_WINDOWS [SourceIndex] [Layer] [InLeft] [InTop] [InRight] [InBottom] [DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
//OUTPUT_FPGA ADD_WINDOWS_END
//����ʹ��ADD_WINDOWS��Ӷ������
//OutputSlotIndex: ����忨�����ţ�ȡֵ0-3
//BackgroundIndex�����������ţ�ȡֵ0-3
//ScreenWidth��ScreenHeight���������ͼ��ֱ��ʳ��Ⱥ͸߶�
//SourceIndex���ź�Դ������忨�е������ţ�ȡֵ0-8��0-3�����뿨ͨ������ֱ�ӽ����ԭʼͼ���źţ�4-7�������������������źţ�8�ǵ�ǰͨ���ı����ź�
//Layer���������ڲ�򴰿����ȼ���ֵԽ�����ȼ�Խ�ߣ�ȡֵ1-N
//InLeft��InTop��InRight��InBottom����ʾ��ʾ������Ҫ��ȡ���ź�Դͼ���λ�á�
//DisplayLeft��DisplayTop��DisplayRight��DisplayBottom����ʾ��ʾ���ڵ�λ�á�
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
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�

	
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
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�

	
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


//�Ͳ�����fpga
void output_set(void)
{
	u8 i=0;
	u8 j=0;
	for(i=0;i<inBoardUsedScalerNum;i++)  //��ӡ�������������ֵ
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
	for(i=0;i<OutBoardUsedScalerNum;i++)  //��ӡ�������������ֵ
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
//	���� ���뿨������ʾ
//�����������������뿨�Ĳ������£�ֻ�иð忨�Ĳ������º󣬰忨�Ž��µ����ò�����Ч��
//INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY [InputSlotIndex]
	sprintf(gCmdBuf,"I_F SUSD 0\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	
	sprintf(gCmdBuf,"O_F SUSD 3\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
}


//����ǰ���ñ��浽����
void save_scenario_to_FPGA(u8 num)
{
	sprintf(gCmdBuf,"GP SDMI %d\r\n",num);
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	output_set();
	GpuSend("SBC(7);");
	GpuSend("CBOF(10,45,200,130,7,7);");

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��7��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��6��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��5��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��4��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��3��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��2��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);

	sprintf(gCmdBuf,"DS16(20,80,'����%d���ڱ��桭��1��',0);\r\n",num);
	GpuSend(gCmdBuf);
	delay_ms(1000);
	

//	���� ������ʾЧ���ı�������
//	GP SDMI [DisplayModeIndex]
//	DisplayModeIndex����ʾЧ�����浽�������ĵ�ַ������ȡֵ0~19
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


//�Ƿ���Ҫʹ��������;����ֵ����Ҫʹ�õ�������ĸ���
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
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�	
	return (line_end-line+1)*(col_end-col+1);  //�������Ҫ��������������ʵ�ʾ���Ҫ��ʾ�����ٸ������
	
}



 //ֱ�ӽ��źŷ������������д���;��ʹ��������
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
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
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
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[j*gScrCol+j]][6]=0;  //��ʾ��0�㿪ʼ
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=OF_AW_Para[i*gScrCol+j-1][OF_AW_cnt[i*gScrCol+j-1]][2];
			}
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4];
		
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=port-1; //�ź�Դ
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //�ź�Դ
				
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5] ;
			}			
		}
		
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				OF_AW_cnt[i*gScrCol+j]++;
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
	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
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
			OF_Scaler_Para[OutBoardUsedScalerNum][2]=port-1;//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			OF_Scaler_Para[OutBoardUsedScalerNum][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
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
					OF_Scaler_Para[OutBoardUsedScalerNum][6]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
				}
				else
				{
					OF_Scaler_Para[OutBoardUsedScalerNum][9]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];	
					OF_Scaler_Para[OutBoardUsedScalerNum][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width/gImgPara[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
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
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0; //��ʾ��0�㿪ʼ
					}
					else
					{
						OF_Scaler_Para[OutBoardUsedScalerNum][5]=OF_Scaler_Para[OutBoardUsedScalerNum-1][6]+1;
						OF_Scaler_Para[OutBoardUsedScalerNum][6]=g_screen_line_column_pixel[0][j]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width/gImgPara[num][WIDTH_INDEX]+OF_Scaler_Para[OutBoardUsedScalerNum][5]-1; 
						OF_Scaler_Para[OutBoardUsedScalerNum][9]=g_screen_line_column_pixel[0][j];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0;  //��ʾ��0�㿪ʼ
					}
			}
	
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=OutBoardUsedScalerNum+4; //�ź�Դ
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //�ź�Դ
			
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

	//��������ѭ���ҵ���ͼ��������ڵ�����˿�
		
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
					IF_Scaler_Para[inBoardUsedScalerNum][8]=InSrcRes[num].resolution_high-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
				}
				else
				{					
					IF_Scaler_Para[inBoardUsedScalerNum][10]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
					IF_Scaler_Para[inBoardUsedScalerNum][8]=IF_Scaler_Para[inBoardUsedScalerNum][7]+(gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX])*InSrcRes[num].resolution_high/gImgPara[num][HIGH_INDEX]-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
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
			IF_Scaler_Para[inBoardUsedScalerNum][2]=gImgPara[num][SOURCE_IN_INDEX];//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			IF_Scaler_Para[inBoardUsedScalerNum][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
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
					IF_Scaler_Para[inBoardUsedScalerNum][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[num].resolution_width/gImgPara[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ						
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
			//���½�������������ڲ���//
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



//add-new-image,matrix call image_layout���沼�ִ�����
void image_layout(void)
{
	u8 i=0;
	u8 zoom_need_j=0;
	u8 numb_zoom_need_scaler=0;
	char  source_to_outboard_num=0;
	char select_inputboard_layout_port_num=0;
	u8 select_inputboard_layout_port=1;
	u8 find_same_input_zoom=0; //������ͬ������
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
	for(i=0;i<IMAGE_TOTAL_NUM;i++) //����ͼ������
	{
		if(gImgPara[i][LAYER_INDEX]!=0) //ͼ��Ų�����0����ʾ��ͼ�����
		{
			if(gImgPara[i][WIDTH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width && gImgPara[i][HIGH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high
				||gImgPara[i][WIDTH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width && gImgPara[i][HIGH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high)  //��ǰͼ���ͼ��ߴ�С��
			{
				find_same_input_zoom=same_input_zoom(i);
				if(find_same_input_zoom==0)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
				{		
					if(inBoardUsedScalerNum<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
					{						
						IF_Scaler_Para[inBoardUsedScalerNum][0]=INPUT_BOARD0;
						IF_Scaler_Para[inBoardUsedScalerNum][1]=inBoardUsedScalerNum;	
						IF_Scaler_Para[inBoardUsedScalerNum][2]=gImgPara[i][SOURCE_IN_INDEX];//�����ź�Դ
						///////////////////////////////////////
						//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
						IF_Scaler_Para[inBoardUsedScalerNum][3]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width;// ����Դ�ֱ���
						IF_Scaler_Para[inBoardUsedScalerNum][4]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high;//
						IF_Scaler_Para[inBoardUsedScalerNum][5]=0;//
						IF_Scaler_Para[inBoardUsedScalerNum][6]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width-1;//
						IF_Scaler_Para[inBoardUsedScalerNum][7]=0;//
						IF_Scaler_Para[inBoardUsedScalerNum][8]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high-1;//
						/////////////////////////////////////////						
						IF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[i][4];//����Ŀ�
						IF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[i][5];//����ĸ�	
					
						//���½�������������ڲ���//
						select_inputboard_layout_port=inputboard_out_layout(inBoardUsedScalerNum,1);
						
						inBoardUsedScalerNum++;
						if(select_inputboard_layout_port==1)
							select_inputboard_layout_port_num=IF_AW_cnt0-1;
						else if(select_inputboard_layout_port==2)
							select_inputboard_layout_port_num=IF_AW_cnt1-1;
						if(select_inputboard_layout_port>0)
							ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
					}	
					else  //�޿ɷ����������
					{
						//
						//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽����壬�ͷ���ͼ��
						{
							same_output_zoom_num=same_output_zoom(select_inputboard_layout_port,source_to_outboard_num,i);
							if(!same_output_zoom_num)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
							{		
								//����˴�˵�����԰�Դ�źŴ��������						
								if(OutBoardUsedScalerNum<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
								{											
									OF_Scaler_Para[inBoardUsedScalerNum][0]=OUTPUT_BOARD3;
									OF_Scaler_Para[inBoardUsedScalerNum][1]=OutBoardUsedScalerNum;	
									OF_Scaler_Para[inBoardUsedScalerNum][2]=select_inputboard_layout_port;//�����ź�Դ
									///////////////////////////////////////
									//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
									OF_Scaler_Para[inBoardUsedScalerNum][3]=1920;// ����Դ�ֱ���
									OF_Scaler_Para[inBoardUsedScalerNum][4]=2400;//
									if(select_inputboard_layout_port==1) //�ڲ�����1
									{
										OF_Scaler_Para[inBoardUsedScalerNum][5]=IF_AW_BG0_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBoardUsedScalerNum][6]=IF_AW_BG0_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBoardUsedScalerNum][7]=IF_AW_BG0_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBoardUsedScalerNum][8]=IF_AW_BG0_Para[source_to_outboard_num][9]-1;//
									}
									else	if(select_inputboard_layout_port==2) //�ڲ�����2
									{
										OF_Scaler_Para[inBoardUsedScalerNum][5]=IF_AW_BG1_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBoardUsedScalerNum][6]=IF_AW_BG1_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBoardUsedScalerNum][7]=IF_AW_BG1_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBoardUsedScalerNum][8]=IF_AW_BG1_Para[source_to_outboard_num][9]-1;//
									}
									/////////////////////////////////////////						
									OF_Scaler_Para[inBoardUsedScalerNum][9]=gImgPara[i][4];//����Ŀ�
									OF_Scaler_Para[inBoardUsedScalerNum][10]=gImgPara[i][5];//����ĸ�	
									
									same_output_zoom_num=OutBoardUsedScalerNum;
									OutBoardUsedScalerNum++;
									
									outputboard_zoom_out_layout(same_output_zoom_num,i);//�ô������ 0����ʱ�ŵ�
								}						
								else//ͼ���ù����Ժ�û���������������ţ���ֱ�Ӱ�����Դ�ź�������Ͻ�
								{	
									ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
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
					ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);

				}
				
			}
			else if(gImgPara[i][WIDTH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_width || gImgPara[i][HIGH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].resolution_high) //��Ҫ�Ŵ�
			{
					
				numb_zoom_need_scaler=howMany_Scaler_zoom_need(i);
				if((4-OutBoardUsedScalerNum)>=numb_zoom_need_scaler)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
				{																							
				//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽�����
						{					
							outputboard_zoom_and_layout(select_inputboard_layout_port,source_to_outboard_num,i);										
						}	
						else if((4-inBoardUsedScalerNum)>=numb_zoom_need_scaler) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
						{
							inputboard_zoom_and_layout(i);
						}								
				}
				else if((4-inBoardUsedScalerNum)>=numb_zoom_need_scaler) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
				{
					 inputboard_zoom_and_layout(i);
				}	
				else
				{
						select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
						if(select_inputboard_layout_port==0) //û��
						{
							select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(select_inputboard_layout_port>0)  //����������϶�û�����������ã��ֿ��԰�ԭ�źŷ��͵�����壬�򲻽������ţ���Ե���ʾ
						{
								ouputboard_out_layout(select_inputboard_layout_port,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);								
						}	
				}				
			}
			else //ͼ�����������һ�������÷�����������ֱ�Ӵ����������з־���ʾ
			{
				//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
				select_inputboard_layout_port=find_same_soure_to_outboard(&source_to_outboard_num,gImgPara[i][6]);
				if(select_inputboard_layout_port==0) //û��
				{
					select_inputboard_layout_port=inputboard_out_layout(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
					if(select_inputboard_layout_port==1)source_to_outboard_num=IF_AW_cnt0-1;
					else if(select_inputboard_layout_port==2)source_to_outboard_num=IF_AW_cnt1-1;							
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



