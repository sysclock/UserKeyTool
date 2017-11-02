/*************************************************  
Copyright (C), 2013-2023, Bycox Tech. Co., Ltd.  
File name:
Author:  LYG  setup~      
Version:        
Date: 20170322 Setup  

Description:    
	// ������ϸ˵���˳����ļ���ɵ���Ҫ���ܣ�������ģ��                  
	// �����Ľӿڣ����ֵ��ȡֵ��Χ�����弰������Ŀ�                  
	// �ơ�˳�򡢶����������ȹ�ϵ  Others:         
	// �������ݵ�˵��  

Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��    
	1,set input image display para
	2,set output board display para
	3,set layout para

History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸� �߼��޸����ݼ���      
	1,//===2017,Jun 16 ; 8:52:20  ;BY YANG@BYCOX.COM===  
		support from 4*4-->8*8
	2. ...

*************************************************/
#define _DISPLAY_C_

#include "header.h"



//the pixel resolution SET of ALL input sources/ports in 8x8 machine 
Resolution_Data InSrcRes[IN_PORT_NUM]=
{
	{0,0,"DVI1"},
	{0,0,"DVI2"},
	{0,0,"DVI3"},
	{0,0,"DVI4"},
	{0,0,"DVI5"},
	{0,0,"DVI6"},
	{0,0,"DVI7"},
	{0,0,"DVI8"}
};


 //����һ�����飬��Ź�IMAGE_TOTAL_NUM������Ĳ���
 //����������ʾ����ʼλλ�á�����λ�á���ȡ��߶Ⱥ��ź�Դ���ⲿ�����0~3����ͼ��ţ�0��ͼ���ʾ�����ã����Ƿ������Դ


u8 scheme_save[SCHEME_NUM]={0};

//INPUT PORT NO./Sequence Number
u8 gMatInpNo;
u8 gMatOutpNo;
u16 gMatImgPara[IN_PORT_NUM][IMAGE_PARA_NUM];

u16 gInImgPara[IMAGE_PARA_NUM];
u16 gSplitImgPara[IN_PORT_NUM][IMAGE_PARA_NUM];


 u8 gResOutCompIn;
//�ܵĻ�������--ÿ������Ĳ��� ����յ�ˮƽ��ֱԴͷ����
//#define BEGIN_X_IDX 0
//#define BEGIN_Y_INDEX 1
//#define END_X_INDEX 2
//#define END_Y_INDEX 3
//#define WIDTH_INDEX 4
//#define HIGH_INDEX 5
//#define SOURCE_IN_INDEX 6
//#define LAYER_INDEX 7
//#define DEAL_INDEX 8
//#define IMAGE_PARA_NUM 9
//IMAGE PARAMETER: 8 CHANNEL INPUT/8 IMAGES:
//in-ports image info para
u16 gImgPara[IN_PORT_NUM][IMAGE_PARA_NUM]={0}; 


//Change the name d_780command_send_buf -->gCmdBuf
char gCmdBuf[SEND_LEN];


//===2017,Jun 14 ; 8:14:16  ;BY YANG@BYCOX.COM===
//change the function name from command_send() to Uart_Send_Cmd_to_FPGA()
void Uart_Send_Cmd_to_FPGA(char *buf)
{
	 u8 i=0;
	   while (1)
	   { 

		   if (buf[i]!='\0')
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


//Read-IP-Address
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
void READ_INPUT_SOURCE_RESOLUTION(void) //��ȡ����ֱ���
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
			InSrcRes[input_board*4+input_board_port].w=input_resolution_width;
			InSrcRes[input_board*4+input_board_port].h=input_resolution_high;
		}
		check=strtok(NULL," ");
	}
	
	for(input_board=0; input_board<4 ; input_board++)
	{
		if(InSrcRes[input_board].w!=0)
		{
			g_Image_Scr=input_board+1;
			break;
		}
	}

	g_Read_Command_Flag=0;
	g_RevUart2Idx=0;
}


//�������������ĸ���������ʹ�������ʹ��ʱ���
//INPUT_FPGA SCALER_SETUP [BoardIndex] [ScalerIndex] [SrcIdx] [InHoriSize] 
//[InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]�
u16 IF_Scaler_Para[IN_BOARD_SCALER_NUM][IF_SCALER_PARA_NUM]={0};

//			OUTPUT_FPGA SCALER_SETUP [OutputSlotIndex] [ScalerIndex] [SrcIdx] [InHoriSize] [InVertSize] [InFirstPixel] [InLastPixel] [InFirstLine] [InLastLine] [OutHoriSize] [OutVertSize]
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
//INPUT_FPGA ADD_WINDOWS [AW_SourceIdx] [Layer] [InLeft] [InTop] [InRight] [InBottom] 
//[DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
//INPUT_FPGA SET_WINDOWS_LAYOUT [BoardIndex] [BackgroundIndex] [ScreenWidth][ScreenHeight]
u16 IF_AW_BG0_Para[8][ADD_WIN_PARA_NUM];//[10];
u16 IF_AW_BG1_Para[8][ADD_WIN_PARA_NUM];
u8 IF_AW_cnt0=0;
u8 IF_AW_cnt1=0;

//the number of input board have used scalers 
u8 inBScalerdNo=0;  //


//the number of Output board have used scalers 
u8 outBScalerdNo=0;

//#define BEGIN_X_IDX 0
//#define BEGIN_Y_INDEX 1
//#define END_X_INDEX 2
//#define END_Y_INDEX 3
//#define WIDTH_INDEX 4
//#define HIGH_INDEX 5
//#define SOURCE_IN_INDEX 6
//#define LAYER_INDEX 7
//#define DEAL_INDEX 8
//#define IMAGE_PARA_NUM 9
//IMAGE PARAMETER: 8 CHANNEL INPUT/8 IMAGES:

//�������Ĳ��������ź�Դ�Ĳ��������÷Ŵ�
u8 same_input_zoom(u8 num)
{
	u8 i;
	
	for(i=0;i<inBScalerdNo;i++)
	{
		if(IF_Scaler_Para[i][SrcIdx]==gImgPara[num][SOURCE_IN_INDEX]
			&&IF_Scaler_Para[i][OutHoriSize]==gImgPara[num][WIDTH_INDEX]
			&&IF_Scaler_Para[i][OutVertSize]==gImgPara[num][HIGH_INDEX])

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
	
	for(i=0;i<outBScalerdNo;i++)
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



u8 InFPGA_AddWin_Para_Process(u8 zoom_num,u8 first_use)
{
	if(first_use==1)
	{	
		if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+IF_Scaler_Para[zoom_num][9])<=1920&& (layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10])<=2400)  //������1������Խ���
		{
			IF_AW_BG0_Para[IF_AW_cnt0][AW_SourceIdx]=zoom_num+4;
			IF_AW_BG0_Para[IF_AW_cnt0][Layer]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][InLeft]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][InTop]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][InRight]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][InBottom]=IF_Scaler_Para[zoom_num][10];			
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayLeft]=layout_port0_X;
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayTop]=layout_port0_Y_pre;
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayRight]=layout_port0_X+IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayBottom]=layout_port0_Y_pre+IF_Scaler_Para[zoom_num][10];
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
			IF_AW_BG0_Para[IF_AW_cnt0][AW_SourceIdx]= zoom_num+4;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][InLeft]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG0_Para[IF_AW_cnt0][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayLeft]=0;
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
			IF_AW_BG1_Para[IF_AW_cnt1][InLeft]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG1_Para[IF_AW_cnt1][DisplayLeft]=layout_port1_X;
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
			IF_AW_BG1_Para[IF_AW_cnt1][InLeft]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=IF_Scaler_Para[zoom_num][9];
			IF_AW_BG1_Para[IF_AW_cnt1][5]=IF_Scaler_Para[zoom_num][10];
			
			IF_AW_BG1_Para[IF_AW_cnt1][DisplayLeft]=0;
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
		
		if((layout_port1_Y_pre!=layout_port1_Y || layout_port1_Y==0)&&(layout_port1_X+InSrcRes[zoom_num].w)<=1920&&(layout_port1_Y_pre+InSrcRes[zoom_num].h)<=2400)  //������2������Խ���
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt1+1;
			IF_AW_BG1_Para[IF_AW_cnt1][InLeft]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=InSrcRes[zoom_num].w;
			IF_AW_BG1_Para[IF_AW_cnt1][5]=InSrcRes[zoom_num].h;
			
			IF_AW_BG1_Para[IF_AW_cnt1][DisplayLeft]=layout_port1_X;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y_pre;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=layout_port1_X+InSrcRes[zoom_num].w;
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y_pre+InSrcRes[zoom_num].h;
			if(!layout_port1_X_flag)
			{
				layout_port1_X+=InSrcRes[zoom_num].w;
				layout_port1_X_flag=1;
				layout_port1_Y=layout_port1_Y_pre+InSrcRes[zoom_num].h;
			}
			else
			{
				layout_port1_Y_pre+=InSrcRes[zoom_num].h;
				if(layout_port1_Y_pre>=layout_port1_Y)
					layout_port1_Y=layout_port1_Y_pre;	
			}						
			IF_AW_cnt1++;
			return 2;
		}
		else if((InSrcRes[zoom_num].w)<=1920&&(layout_port1_Y+InSrcRes[zoom_num].h)<=2400)  //������2������Խ���
		{
			IF_AW_BG1_Para[IF_AW_cnt1][0]=zoom_num;
			IF_AW_BG1_Para[IF_AW_cnt1][1]=IF_AW_cnt0+1;
			IF_AW_BG1_Para[IF_AW_cnt1][InLeft]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][3]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][4]=InSrcRes[zoom_num].w;
			IF_AW_BG1_Para[IF_AW_cnt1][5]=InSrcRes[zoom_num].h;
			
			IF_AW_BG1_Para[IF_AW_cnt1][DisplayLeft]=0;
			IF_AW_BG1_Para[IF_AW_cnt1][7]=layout_port1_Y;
			IF_AW_BG1_Para[IF_AW_cnt1][8]=InSrcRes[zoom_num].w;
			IF_AW_BG1_Para[IF_AW_cnt1][9]=layout_port1_Y+InSrcRes[zoom_num].h;
			layout_port1_Y_pre=layout_port1_Y;
			
			layout_port0_X=InSrcRes[zoom_num].w;
			layout_port0_X_flag=0;
			layout_port1_Y=layout_port1_Y_pre+InSrcRes[zoom_num].h;	
			IF_AW_cnt1++;
			return 2;
		}
		
		else if((layout_port0_Y_pre!=layout_port0_Y || layout_port0_Y==0)&&(layout_port0_X+InSrcRes[zoom_num].w)<=1920&& (layout_port0_Y_pre+InSrcRes[zoom_num].h)<=2400)  //������1������Խ���
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]=zoom_num;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][InLeft]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=InSrcRes[zoom_num].w;
			IF_AW_BG0_Para[IF_AW_cnt0][5]=InSrcRes[zoom_num].h;
			
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayLeft]=layout_port0_X;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y_pre;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=layout_port0_X+InSrcRes[zoom_num].w;
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y_pre+InSrcRes[zoom_num].h;
			if(!layout_port0_X_flag)
			{
				layout_port0_X+=InSrcRes[zoom_num].w;
				layout_port0_X_flag=1;
				layout_port0_Y=layout_port0_Y_pre+InSrcRes[zoom_num].h;
			}
			else
			{				
				layout_port0_Y_pre+=InSrcRes[zoom_num].h;
				if(layout_port0_Y_pre>=layout_port0_Y)
					layout_port0_Y=layout_port0_Y_pre;	
			}				
			IF_AW_cnt0++;
			return 1;
		}
		else if((InSrcRes[zoom_num].w)<=1920&& (layout_port0_Y+InSrcRes[zoom_num].h)<=2400)  //������1������Խ���
		{
			IF_AW_BG0_Para[IF_AW_cnt0][0]= zoom_num;
			IF_AW_BG0_Para[IF_AW_cnt0][1]=IF_AW_cnt0+1;
			IF_AW_BG0_Para[IF_AW_cnt0][InLeft]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][3]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][4]=InSrcRes[zoom_num].w;
			IF_AW_BG0_Para[IF_AW_cnt0][5]=InSrcRes[zoom_num].h;
			
			IF_AW_BG0_Para[IF_AW_cnt0][DisplayLeft]=0;
			IF_AW_BG0_Para[IF_AW_cnt0][7]=layout_port0_Y;
			IF_AW_BG0_Para[IF_AW_cnt0][8]=InSrcRes[zoom_num].w;
			IF_AW_BG0_Para[IF_AW_cnt0][9]=layout_port0_Y+InSrcRes[zoom_num].h;
			layout_port0_Y_pre=layout_port0_Y;
			layout_port1_X=InSrcRes[zoom_num].w;
			layout_port1_X_flag=0;
			layout_port0_Y=layout_port0_Y_pre+InSrcRes[zoom_num].h;	
			IF_AW_cnt0++;
			return 1;
		}	
	
	}
	return 0;
	
}


//����忪����Դ��������Դ�Ƿ���ͬ
u8 IS_Same_Soure_Between_IF_AW_and_FuncArg(char *return_num, u8 source)
{
	char i=0;
	
	if(IF_AW_cnt0>0)
	{
		for(i=0;i<IF_AW_cnt0;i++)
		{
			if(IF_AW_BG0_Para[i][AW_SourceIdx]==source)
			{
				*return_num=i;
				return 1;
			}
		}
	}


	if(IF_AW_cnt1>0)
	{
		for(i=0;i<IF_AW_cnt1;i++)
		{
			if(IF_AW_BG1_Para[i][AW_SourceIdx]==source)
			{
				*return_num=i;
				return 2;
			}
		}
	}
	
	return 0;
	
}


//��������������������Ҫ���ֵĻ������
//sprintf(gCmdBuf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3],OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);			
//OUTPUT_FPGA SET_WINDOWS_LAYOUT [OutputSlotIndex] [BackgroundIndex] [ScreenWidth] [ScreenHeight]
//OUTPUT_FPGA ADD_WINDOWS [AW_SourceIdx] [Layer] [InLeft] [InTop] [InRight] [InBottom] [DisplayLeft] [DisplayTop] [DisplayRight] [DisplayBottom]
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

//4-->8
u8 OF_AW_cnt[OUT_PORT_NUM]={0};




//image-layout call it
void OutFPGA_AddWin_Para_Process (u8 input_port_num,u8 layout_num,u8 num,u16 begin_x,u16 begin_y,u16 end_x,u16 end_y)
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
					OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][AW_SourceIdx]=input_port_num-1;
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

//#define AW_SourceIdx 0
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
		TryEnd_Clear_Display();
		Uart_Send_Cmd_to_FPGA("GP SDMI 0\r\n");
}




//only for trial machine
void TryEnd_Clear_Display(void)
{
		
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 0 \r\n");
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 1 \r\n");
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 2 \r\n");
		Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 3 3 \r\n");


		if(OUT_PORT_NUM>OUT_PORT_NUM_OF_1CARD)
		{
			Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 2 0 \r\n");
			Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 2 1 \r\n");
			Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 2 2 \r\n");
			Uart_Send_Cmd_to_FPGA("OUTPUT_FPGA CLEAR_OUTPUT_WIN 2 3 \r\n");
		}


		Uart_Send_Cmd_to_FPGA("O_F SUSD 3\r\n");
		delay_ms(LCD_COMM_DELAY_MS);
}


//�Ͳ�����fpga
//call by 1)image_layout_Para_Process();2)save_scenario_to_FPGA()
//
void Output_Set_AW_Para2FPGA(void)
{
	u8 i=0;
	u8 j=0;

	//STEP1
	for(i=0;i<inBScalerdNo;i++)  //��ӡ�������������ֵ
	{	
		sprintf(gCmdBuf,"I_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",IF_Scaler_Para[i][0],IF_Scaler_Para[i][1],IF_Scaler_Para[i][2],IF_Scaler_Para[i][3],IF_Scaler_Para[i][4],IF_Scaler_Para[i][5],IF_Scaler_Para[i][6],IF_Scaler_Para[i][7],IF_Scaler_Para[i][8],IF_Scaler_Para[i][9],IF_Scaler_Para[i][10]);
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}

	//STEP2-1
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

	//STEP2-2
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


	//STEP3
	for(i=0;i<outBScalerdNo;i++)  //��ӡ�������������ֵ
	{		
			#ifdef DEBUG88
				printf("O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d \r\n",	OF_Scaler_Para[i][0],OF_Scaler_Para[i][1],OF_Scaler_Para[i][2],OF_Scaler_Para[i][3],
					OF_Scaler_Para[i][4],OF_Scaler_Para[i][5],OF_Scaler_Para[i][6],OF_Scaler_Para[i][7],OF_Scaler_Para[i][8],OF_Scaler_Para[i][9],OF_Scaler_Para[i][10]);
			#endif
			sprintf(gCmdBuf,"O_F SCALER_SETUP %d %d %d %d %d %d %d %d %d %d %d\r\n",OF_Scaler_Para[i][0],OF_Scaler_Para[i][1],OF_Scaler_Para[i][2],OF_Scaler_Para[i][3],OF_Scaler_Para[i][4],OF_Scaler_Para[i][5],OF_Scaler_Para[i][6],OF_Scaler_Para[i][7],OF_Scaler_Para[i][8],OF_Scaler_Para[i][9],OF_Scaler_Para[i][10]);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}


	//STEP4
	for(i=0;i<OUT_PORT_NUM;i++)
	{
		if(OF_AW_cnt[i]==0)
			sprintf(gCmdBuf,"OUTPUT_FPGA CLEAR_OUTPUT_WIN %d %d\r\n",(i/4)>1?2:3,i);	
		else //if(OF_AW_cnt[i]!=0)
		{
			sprintf(gCmdBuf,"O_F SWL 3 %d 1920 1080\r\n",i);
			Uart_Send_Cmd_to_FPGA(gCmdBuf);
			#ifdef DEBUG88
				printf("O_F SWL %d %d 1920 1080 ,i=%d ;OF_AW_cnt[i]=%d \r\n",(i/4)>1?2:3,i,i,OF_AW_cnt[i]);				
			#endif
			
			
			for(j=0;j<OF_AW_cnt[i];j++)
			{
				#ifdef DEBUG88
				printf("O_F AW %d %d %d %d %d %d %d %d %d %d \r\n \r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3]
					,OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);
				#endif
				sprintf(gCmdBuf,"O_F AW %d %d %d %d %d %d %d %d %d %d\r\n",OF_AW_Para[i][j][0],OF_AW_Para[i][j][1],OF_AW_Para[i][j][2],OF_AW_Para[i][j][3],OF_AW_Para[i][j][4],OF_AW_Para[i][j][5],OF_AW_Para[i][j][6],OF_AW_Para[i][j][7],OF_AW_Para[i][j][8],OF_AW_Para[i][j][9]);				
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
			}
			sprintf(gCmdBuf,"O_F AWE\r\n",i);	
		}
		
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}


	
//	���� ���뿨������ʾ
//�����������������뿨�Ĳ������£�ֻ�иð忨�Ĳ������º󣬰忨�Ž��µ����ò�����Ч��
//INPUT_FPGA SET_UPDATA_SYSTEM_DISPLAY [InputSlotIndex]
	//STEP5: update
	sprintf(gCmdBuf,"I_F SUSD 0\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	if(IN_PORT_NUM>IN_PORT_NUM_OF_1CARD)
	{
		sprintf(gCmdBuf,"I_F SUSD 1\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);		
	}
	
	
	sprintf(gCmdBuf,"O_F SUSD 3\r\n");
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	if(OUT_PORT_NUM>OUT_PORT_NUM_OF_1CARD)
	{
		sprintf(gCmdBuf,"O_F SUSD 2\r\n");
		Uart_Send_Cmd_to_FPGA(gCmdBuf);
	}
}







//����ǰ���ñ��浽����
void save_scenario_to_FPGA(u8 num)
{
	sprintf(gCmdBuf,"GP SDMI %d\r\n",num);
	Uart_Send_Cmd_to_FPGA(gCmdBuf);
	Output_Set_AW_Para2FPGA();
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
	delay_ms(LCD_COMM_DELAY_MS);
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
void inImag_direct_to_out_OF_AW(u8 port,u8 port_num ,u8 num)  
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

	//STEP1
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



	//STEP2
	for(i=line;i<=line_end;i++)
	{
		for(j=col;j<=col_end;j++)
		{
			//CASE1	
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
			//CASE2
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

			//CASE3
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
			//CASE4
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




			//
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4];
		
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=port-1; //�ź�Դ
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //�ź�Դ
				
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5] ;
			}			
		}


		//Ҫ���ǵ�����ģʽ
		//STEP3
		for(i=line;i<=line_end;i++)
			for(j=col;j<=col_end;j++)
			{
				OF_AW_cnt[i*gScrCol+j]++;
			}
}


//port ������ʾ�˴��������������ֿ���һ�������ź�Դ
//source ����������������˿�ָ�����ź���Դ
//num ��ʾ���ǻ������Ⱥ�
void OutFPGA_Scaler_Para_Process(u8 port,u8 port_num ,u8 num)  //��������Ҫ�Ŵ󣬴������ʹ�����������зŴ�����еĲ������
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
	
	u8 output_zoom_port_used_pre=outBScalerdNo;
	
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
			cur_Y_point=OF_Scaler_Para[outBScalerdNo-1][8]+1;
		}
		for(j=col;j<=col_end;j++)
		{	
				if(i==line)
				{
					if(port==1)
						OF_Scaler_Para[outBScalerdNo][7]=IF_AW_BG0_Para[port_num][7];
					else if(port==2)
						OF_Scaler_Para[outBScalerdNo][7]=IF_AW_BG1_Para[port_num][7];
										
					if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][line])
					{
						OF_Scaler_Para[outBScalerdNo][10]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX];
						OF_Scaler_Para[outBScalerdNo][8]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h-1;
					}
					else
					{
						OF_Scaler_Para[outBScalerdNo][10]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
						OF_Scaler_Para[outBScalerdNo][8]=OF_Scaler_Para[outBScalerdNo][7]+g_screen_line_column_pixel[1][i]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h/gImgPara[num][HIGH_INDEX]-1;
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
						OF_Scaler_Para[outBScalerdNo][7]=cur_Y_point;
						OF_Scaler_Para[outBScalerdNo][8]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h-1;
						OF_Scaler_Para[outBScalerdNo][10]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][i-1];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=0;
					}
					else
					{
						OF_Scaler_Para[outBScalerdNo][7]=cur_Y_point;
						OF_Scaler_Para[outBScalerdNo][8]=OF_Scaler_Para[outBScalerdNo][7]+g_screen_line_column_pixel[1][i]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h/gImgPara[num][HIGH_INDEX]-1; 
						OF_Scaler_Para[outBScalerdNo][10]=g_screen_line_column_pixel[1][i];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]=0;
					}
				}
	
			OF_Scaler_Para[outBScalerdNo][0]=OUTPUT_BOARD3;
			OF_Scaler_Para[outBScalerdNo][1]=outBScalerdNo;	
			OF_Scaler_Para[outBScalerdNo][2]=port-1;//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			OF_Scaler_Para[outBScalerdNo][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w;// ����Դ�ֱ���
			OF_Scaler_Para[outBScalerdNo][4]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h;//	
			
			if(j==col)
			{	
				if(port==1)
					OF_Scaler_Para[outBScalerdNo][5]=IF_AW_BG0_Para[port_num][6];
				else if(port==2)
					OF_Scaler_Para[outBScalerdNo][5]=IF_AW_BG1_Para[port_num][6];
				
				if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
				{
					OF_Scaler_Para[outBScalerdNo][9]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX];
					OF_Scaler_Para[outBScalerdNo][6]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
				}
				else
				{
					OF_Scaler_Para[outBScalerdNo][9]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];	
					OF_Scaler_Para[outBScalerdNo][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w/gImgPara[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ
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
						OF_Scaler_Para[outBScalerdNo][5]=OF_Scaler_Para[outBScalerdNo-1][6]+1;
						OF_Scaler_Para[outBScalerdNo][6]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w-1;
						OF_Scaler_Para[outBScalerdNo][9]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0; //��ʾ��0�㿪ʼ
					}
					else
					{
						OF_Scaler_Para[outBScalerdNo][5]=OF_Scaler_Para[outBScalerdNo-1][6]+1;
						OF_Scaler_Para[outBScalerdNo][6]=g_screen_line_column_pixel[0][j]*InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w/gImgPara[num][WIDTH_INDEX]+OF_Scaler_Para[outBScalerdNo][5]-1; 
						OF_Scaler_Para[outBScalerdNo][9]=g_screen_line_column_pixel[0][j];
						OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]=0;  //��ʾ��0�㿪ʼ
					}
			}
	
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][0]=outBScalerdNo+4; //�ź�Դ
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][1]=num+1; //�ź�Դ
			
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][2]=0;
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][3]=0;
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4]=OF_Scaler_Para[outBScalerdNo][9];
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5]=OF_Scaler_Para[outBScalerdNo][10];
		
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][8]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][6]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][4];	
			OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][9]=OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][7]+OF_AW_Para[i*gScrCol+j][OF_AW_cnt[i*gScrCol+j]][5] ;
			outBScalerdNo++;				
		}
	}



		
	for(i=line;i<=line_end;i++)
		for(j=col;j<=col_end;j++)
		{							//===2017,Jun 17 ; 18:10:57  ;BY YANG@BYCOX.COM===
			OF_AW_cnt[i*gScrCol+j]++;//����ģʽ��nvs0��ʱ���������д����
		}
		
}




//image layout call it
void InFPGA_Scaler_Para_Process(u8 num)
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
	u8 AW_BG_No=1;
	char select_inputboard_layout_port_num=0;
	u8 input_zoom_port_used_pre=inBScalerdNo;
	
	u16 curimage_begin_x=0;
	u16 curimage_begin_y=0;
	u16 curimage_end_x=0;
	u16 curimage_end_y=0;
	
	u16 cur_Y_point=0;
	//step1
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
	//step2	
	for(i=line;i<=line_end;i++)
	{
		if(i>line)
		{
			cur_Y_point=IF_Scaler_Para[inBScalerdNo-1][8]+1;
		}


		for(j=col;j<=col_end;j++)
		{
			//section1
			if(i==line)
			{
				IF_Scaler_Para[inBScalerdNo][7]=0;				
				curimage_begin_y=gImgPara[num][BEGIN_Y_INDEX];
				
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][line])
				{	
					IF_Scaler_Para[inBScalerdNo][10]=gImgPara[num][END_Y_INDEX]-gImgPara[num][BEGIN_Y_INDEX];
					IF_Scaler_Para[inBScalerdNo][8]=InSrcRes[num].h-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
				}
				else
				{					
					IF_Scaler_Para[inBScalerdNo][10]=gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX];
					IF_Scaler_Para[inBScalerdNo][8]=IF_Scaler_Para[inBScalerdNo][7]+(gScrTotPix[1][line]-gImgPara[num][BEGIN_Y_INDEX])*InSrcRes[num].h/gImgPara[num][HIGH_INDEX]-1; //�˴μ�������Ϊ1080�� ���øĳ�ʵ�������ֵ
				}
	
			}
			else
			{
				curimage_begin_y=gScrTotPix[1][i-1];
				if(gImgPara[num][END_Y_INDEX]<=gScrTotPix[1][i])
				{
					IF_Scaler_Para[inBScalerdNo][7]=cur_Y_point;
					IF_Scaler_Para[inBScalerdNo][8]=InSrcRes[num].h-1; 
					IF_Scaler_Para[inBScalerdNo][10]=gImgPara[num][END_Y_INDEX]-gScrTotPix[1][i-1];
				}
				else
				{
					IF_Scaler_Para[inBScalerdNo][7]=cur_Y_point;
					IF_Scaler_Para[inBScalerdNo][8]=IF_Scaler_Para[inBScalerdNo][7]+g_screen_line_column_pixel[1][i]*InSrcRes[num].h/gImgPara[num][HIGH_INDEX]-1;
					IF_Scaler_Para[inBScalerdNo][10]=g_screen_line_column_pixel[1][i];
				}
				
			}
				
			IF_Scaler_Para[inBScalerdNo][0]=INPUT_BOARD0;
			IF_Scaler_Para[inBScalerdNo][1]=inBScalerdNo;	
			IF_Scaler_Para[inBScalerdNo][2]=gImgPara[num][SOURCE_IN_INDEX];//�����ź�Դ
			///////////////////////////////////////
			//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
			IF_Scaler_Para[inBScalerdNo][3]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].w;// ����Դ�ֱ���
			IF_Scaler_Para[inBScalerdNo][4]=InSrcRes[gImgPara[num][SOURCE_IN_INDEX]].h;//	

			//section2
			if(j==col)
			{	
				IF_Scaler_Para[inBScalerdNo][5]=0;
				
						
			
				curimage_begin_x=gImgPara[num][BEGIN_X_INDEX];
				
				if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
				{
					IF_Scaler_Para[inBScalerdNo][9]=gImgPara[num][END_X_INDEX]-gImgPara[num][BEGIN_X_INDEX];
					IF_Scaler_Para[inBScalerdNo][6]=InSrcRes[num].w-1;
				}
				else
				{
					IF_Scaler_Para[inBScalerdNo][9]=gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX];
					IF_Scaler_Para[inBScalerdNo][6]=(gScrTotPix[0][col]-gImgPara[num][BEGIN_X_INDEX])*InSrcRes[num].w/gImgPara[num][WIDTH_INDEX]-1; //�˴μ�������Ϊ1920��� ���øĳ�ʵ�������ֵ						
				}
			}
			else
			{
					curimage_begin_x=gScrTotPix[0][j-1];
					if(gImgPara[num][END_X_INDEX]<=gScrTotPix[0][j])
					{
						IF_Scaler_Para[inBScalerdNo][5]=IF_Scaler_Para[inBScalerdNo-1][6]+1;
						IF_Scaler_Para[inBScalerdNo][6]=InSrcRes[num].w-1;
						IF_Scaler_Para[inBScalerdNo][9]=gImgPara[num][END_X_INDEX]-gScrTotPix[0][j-1];
					}
					else
					{
						IF_Scaler_Para[inBScalerdNo][5]=IF_Scaler_Para[inBScalerdNo-1][6]+1;
						IF_Scaler_Para[inBScalerdNo][6]=IF_Scaler_Para[inBScalerdNo][5]+g_screen_line_column_pixel[0][j]*InSrcRes[num].w/gImgPara[num][WIDTH_INDEX]-1; 
						IF_Scaler_Para[inBScalerdNo][9]=g_screen_line_column_pixel[0][j];
					}
			}
			//���½�������������ڲ���//

			//section3
			AW_BG_No=InFPGA_AddWin_Para_Process(inBScalerdNo,1);
			curimage_end_x=curimage_begin_x+IF_Scaler_Para[inBScalerdNo][9];
			curimage_end_y=curimage_begin_y+IF_Scaler_Para[inBScalerdNo][10];
			inBScalerdNo++;

			if(AW_BG_No==1)
				select_inputboard_layout_port_num=IF_AW_cnt0-1;
			else if(AW_BG_No==2)
				select_inputboard_layout_port_num=IF_AW_cnt1-1;

			//section4
			OutFPGA_AddWin_Para_Process(AW_BG_No,select_inputboard_layout_port_num,num,curimage_begin_x,curimage_begin_y,curimage_end_x,curimage_end_y);	
		}	

	
	}	


}





//add-new-image,matrix call image_layout���沼�ִ�����
//gImgPara-->Scaler/Zoom Para
//Call by:(1)Add_New_Image()
//		(2)Matrix_Display_Function(void)
void image_layout_Para_Process(void)
{
	u8 i=0;
	u8 zoom_need_j=0;
	u8 numb_zoom_need_scaler=0;
	char  source_to_outboard_num=0;
	char select_inputboard_layout_port_num=0;
	u8 AW_BG_No=1;
	u8 find_same_input_zoom=0; //������ͬ������
	u8 same_output_zoom_num=0;

	//for(i=0 ;i<4;i++)
	for(i=0 ;i<OUT_PORT_NUM;i++)
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
	
	
	inBScalerdNo=0;
	outBScalerdNo=0;
	source_to_outboard_num=0;

	
	#ifdef DEBUG88
	printf("LAYER-IDX:  1=%d,2=%d,3=%d,4=%d,5=%d,6=%d,7=%d,8=%d \r\n"
		,gImgPara[0][LAYER_INDEX],gImgPara[1][LAYER_INDEX],gImgPara[2][LAYER_INDEX],gImgPara[3][LAYER_INDEX]
		,gImgPara[4][LAYER_INDEX],gImgPara[5][LAYER_INDEX],gImgPara[6][LAYER_INDEX],gImgPara[7][LAYER_INDEX]);
	#endif
	
	//STEP 1:PROCESS SCALER or Add windows PARA
	for(i=0;i<IN_PORT_NUM;i++) //����ͼ������
	{
		#ifdef DEBUG88
			printf("i=%d,same-input=%d,InSrcRes.Wid=%d \r\n",i,find_same_input_zoom,InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w-1);	
			
			printf("i=%d,WIDTH gImgPara compare %d Vs InputSourceRes %d; \r\n HIGH:gImgPara compare %d Vs InSrcRes %d;\r\n",i,
			gImgPara[i][WIDTH_INDEX],InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w, 
			gImgPara[i][HIGH_INDEX],InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h);
		#endif

		if(gImgPara[i][LAYER_INDEX]!=0) //ͼ��Ų�����0����ʾ��ͼ�����
			{
			if(gImgPara[i][WIDTH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w 
					&& gImgPara[i][HIGH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h				
					||gImgPara[i][WIDTH_INDEX]<=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w 
					&& gImgPara[i][HIGH_INDEX]<InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h)  //��ǰͼ���ͼ��ߴ�С�
					gResOutCompIn=RES_InLarge;

			else if(gImgPara[i][WIDTH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w
					|| gImgPara[i][HIGH_INDEX]>InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h) //��Ҫ�Ŵ�
					gResOutCompIn=RES_OutLarge;
			else
					gResOutCompIn=RES_InEQOut;
			}
		else
			gResOutCompIn=RES_InIMGLOST;
	
		

//			case 1:width & high less than & almost only 1 equal
			switch(gResOutCompIn)
			{
				case RES_InLarge:
				
				find_same_input_zoom=same_input_zoom(i);
				#ifdef DEBUG88
				printf("Case 1 i=%d,out<=in,same-input =%d \r\n",i,find_same_input_zoom);
				#endif
				
				if(find_same_input_zoom==0)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
				{		
					if(inBScalerdNo<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
					{						
						IF_Scaler_Para[inBScalerdNo][BoardIndex]=INPUT_BOARD0;
						IF_Scaler_Para[inBScalerdNo][ScalerIndex]=inBScalerdNo;	
						IF_Scaler_Para[inBScalerdNo][SrcIdx]=gImgPara[i][SOURCE_IN_INDEX];//�����ź�Դ
						///////////////////////////////////////
						//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
						IF_Scaler_Para[inBScalerdNo][InHoriSize]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w;// ����Դ�ֱ���
						IF_Scaler_Para[inBScalerdNo][InVertSize]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h;//
						IF_Scaler_Para[inBScalerdNo][InFirstPixel]=0;//
						IF_Scaler_Para[inBScalerdNo][InLastPixel]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].w-1;//
						IF_Scaler_Para[inBScalerdNo][InFirstLine]=0;//
						IF_Scaler_Para[inBScalerdNo][InLastLine]=InSrcRes[gImgPara[i][SOURCE_IN_INDEX]].h-1;//
						/////////////////////////////////////////						
						IF_Scaler_Para[inBScalerdNo][OutHoriSize]=gImgPara[i][4];//����Ŀ�
						IF_Scaler_Para[inBScalerdNo][OutVertSize]=gImgPara[i][5];//����ĸ�	
					
						//���½�������������ڲ���//
						AW_BG_No=InFPGA_AddWin_Para_Process(inBScalerdNo,1);
						
						inBScalerdNo++;
						if(AW_BG_No==1)
							select_inputboard_layout_port_num=IF_AW_cnt0-1;
						else if(AW_BG_No==2)
							select_inputboard_layout_port_num=IF_AW_cnt1-1;
						if(AW_BG_No>0)
							OutFPGA_AddWin_Para_Process(AW_BG_No,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
					}	
					else  //�޿ɷ����������
					{
						//
						//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						AW_BG_No=IS_Same_Soure_Between_IF_AW_and_FuncArg(&source_to_outboard_num,gImgPara[i][6]);
						if(AW_BG_No==0) //û��
						{
							AW_BG_No=InFPGA_AddWin_Para_Process(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(AW_BG_No==1)source_to_outboard_num=IF_AW_cnt0-1;
							else if(AW_BG_No==2)source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(AW_BG_No>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽����壬�ͷ���ͼ��
						{
							same_output_zoom_num=same_output_zoom(AW_BG_No,source_to_outboard_num,i);
							if(same_output_zoom_num==0)  //���û���Ѿ����ڵ����ŷ�ʽ�뵱ǰ�������ͬ
							{		
								//����˴�˵�����԰�Դ�źŴ��������						
								if(outBScalerdNo<4)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
								{											
									OF_Scaler_Para[inBScalerdNo][0]=OUTPUT_BOARD3;
									OF_Scaler_Para[inBScalerdNo][1]=outBScalerdNo;	
									OF_Scaler_Para[inBScalerdNo][2]=AW_BG_No;//�����ź�Դ
									///////////////////////////////////////
									//���µĲ���Ӧ���Ǹ���ʵ������Դ�Ĳ������и�ֵ
									OF_Scaler_Para[inBScalerdNo][3]=1920;// ����Դ�ֱ���
									OF_Scaler_Para[inBScalerdNo][4]=2400;//
									if(AW_BG_No==1) //�ڲ�����1
									{
										OF_Scaler_Para[inBScalerdNo][5]=IF_AW_BG0_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBScalerdNo][6]=IF_AW_BG0_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBScalerdNo][7]=IF_AW_BG0_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBScalerdNo][8]=IF_AW_BG0_Para[source_to_outboard_num][9]-1;//
									}
									else	if(AW_BG_No==2) //�ڲ�����2
									{
										OF_Scaler_Para[inBScalerdNo][5]=IF_AW_BG1_Para[source_to_outboard_num][6];//
										OF_Scaler_Para[inBScalerdNo][6]=IF_AW_BG1_Para[source_to_outboard_num][8]-1;//
										OF_Scaler_Para[inBScalerdNo][7]=IF_AW_BG1_Para[source_to_outboard_num][7];;//
										OF_Scaler_Para[inBScalerdNo][8]=IF_AW_BG1_Para[source_to_outboard_num][9]-1;//
									}
									/////////////////////////////////////////						
									OF_Scaler_Para[inBScalerdNo][9]=gImgPara[i][4];//����Ŀ�
									OF_Scaler_Para[inBScalerdNo][10]=gImgPara[i][5];//����ĸ�	
									
									same_output_zoom_num=outBScalerdNo;
									outBScalerdNo++;
									
									outputboard_zoom_out_layout(same_output_zoom_num,i);//�ô������ 0����ʱ�ŵ�
								}						
								else//ͼ���ù����Ժ�û���������������ţ���ֱ�Ӱ�����Դ�ź�������Ͻ�
								{	
									OutFPGA_AddWin_Para_Process(AW_BG_No,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);
								}							
							}

							else  //������ͬ������
							{
								outputboard_zoom_out_layout(same_output_zoom_num,i);
							}		

							
						}		
					}						
				}


				//if(find_same_input_zoom==1) 	
				else //�Ѿ�����һ����ͬ������
				{
					//������ͬ��������λ��
					IS_Same_Soure_Between_IF_AW_and_FuncArg(&select_inputboard_layout_port_num,find_same_input_zoom+4);
					OutFPGA_AddWin_Para_Process(AW_BG_No,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);

				}
				break;
			

//			case 2:width or high larger than source
			case RES_OutLarge:
			{					
				numb_zoom_need_scaler=howMany_Scaler_zoom_need(i);
				#ifdef DEBUG88
				printf("Case 2 i=%d,Out>in; numb_zoom_need_scaler =%d,outBScalerdNo=%d \r\n",i,numb_zoom_need_scaler,outBScalerdNo);
				#endif
				if((4-outBScalerdNo)>=numb_zoom_need_scaler)//�пɷ�����������ҿ��԰��Ų��֣���������Ͻ�������
				{																							
				//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
						AW_BG_No=IS_Same_Soure_Between_IF_AW_and_FuncArg(&source_to_outboard_num,gImgPara[i][SOURCE_IN_INDEX]);
						#ifdef DEBUG88
						printf("display.c L1842 AW_BG_No=%d,\r\n",AW_BG_No);
						#endif
						if(AW_BG_No==0) //û��
						{
							AW_BG_No=InFPGA_AddWin_Para_Process(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(AW_BG_No==1)
								source_to_outboard_num=IF_AW_cnt0-1;
							else if(AW_BG_No==2)
								source_to_outboard_num=IF_AW_cnt1-1;	
						#ifdef DEBUG88
						printf("display.c L1850 AW_BG_No=%d,\r\n",AW_BG_No);
						#endif
						}

						#ifdef DEBUG88
						printf("display.c L1857 AW_BG_No=%d,\r\n",AW_BG_No);
						#endif
						if(AW_BG_No>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽�����
						{					
							OutFPGA_Scaler_Para_Process(AW_BG_No,source_to_outboard_num,i);										

						}	
						else if((4-inBScalerdNo)>=numb_zoom_need_scaler) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
						{
							InFPGA_Scaler_Para_Process(i);

						}
						
				}
				else if((4-inBScalerdNo)>=numb_zoom_need_scaler) //���û�а취���뵽����壬��鿴������ϵ��������Ƿ���ã����þͽ��в���
				{
					 InFPGA_Scaler_Para_Process(i);
					 #ifdef DEBUG88
						printf("display.c L1875 AW_BG_No=%d,\r\n",AW_BG_No);
					#endif
				}	
				else
				{
						AW_BG_No=IS_Same_Soure_Between_IF_AW_and_FuncArg(&source_to_outboard_num,gImgPara[i][6]);
						if(AW_BG_No==0) //û��
						{
							AW_BG_No=InFPGA_AddWin_Para_Process(gImgPara[i][SOURCE_IN_INDEX],2);//���в���
							if(AW_BG_No==1)
								source_to_outboard_num=IF_AW_cnt0-1;
							else if(AW_BG_No==2)
								source_to_outboard_num=IF_AW_cnt1-1;							
						}
						if(AW_BG_No>0)  //����������϶�û�����������ã��ֿ��԰�ԭ�źŷ��͵�����壬�򲻽������ţ���Ե���ʾ
						{
								OutFPGA_AddWin_Para_Process(AW_BG_No,select_inputboard_layout_port_num,i,gImgPara[i][BEGIN_X_INDEX],gImgPara[i][BEGIN_Y_INDEX],gImgPara[i][END_X_INDEX],gImgPara[i][END_Y_INDEX]);								
						}
						#ifdef DEBUG88
						printf("display.c L1894 AW_BG_No=%d,\r\n",AW_BG_No);
						#endif
				}				
			}
			break;
			
//			case 3 :width & high both equal
			case RES_InEQOut://ͼ�����������һ�������÷�����������ֱ�Ӵ����������з־���ʾ
							//Ӧ����һ�²������Ƿ��Ѿ��Ѵ��ź�Դ���뵽����� ����û���ٽ��в���
				AW_BG_No=IS_Same_Soure_Between_IF_AW_and_FuncArg(&source_to_outboard_num,gImgPara[i][SOURCE_IN_INDEX]);
				#ifdef DEBUG88
				printf("case 3 i=%d,Exactly EQUAL,AW_BG_No =%d \r\n",i,AW_BG_No);
				#endif
				if(AW_BG_No==0) //û��
				{
					AW_BG_No=InFPGA_AddWin_Para_Process(gImgPara[i][SOURCE_IN_INDEX],2);//���в���

					if(AW_BG_No==1)
						source_to_outboard_num=IF_AW_cnt0-1;
					else if(AW_BG_No==2)
						source_to_outboard_num=IF_AW_cnt1-1;							
				}
				
				if(AW_BG_No>0)  //ֱ�ӽ��źŷ������������д��� ,����ź��޷�ͨ�����������뵽�����
				{					
					inImag_direct_to_out_OF_AW(AW_BG_No,source_to_outboard_num,i);										
				}		
				break;
				
			default:break;
			}

		}


	//STEP2 :SEND PARA TO FPGA��fpga����ָ��
	Output_Set_AW_Para2FPGA();
	
}





void  I_F_SET_WINDOWS_LAYOUT(void)
{
	
}



