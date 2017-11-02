/*


//===2017,Jun 13 ; 21:44:25  ;BY YANG@BYCOX.COM===
function:
	1,set display para about processor output image
	2,

*/



#include "header.h"

//#include "screen-set.h"
//#include "display.h"
//#include "usart.h"

u8 user_select_screen=0;
u32 g_TFT_total_size_X=594;
u32 g_TFT_total_size_Y=330;
u32 g_TFT_begin_X=183;
u32 g_TFT_begin_Y=130;


u8 g_stitching_screen_line=2;
u8 g_stitching_screen_column=2;
u8 gScrRow=2;
u8 gScrCol=2;
u16 gScrTotX=10000;
u16 gScrTotY=10000;

u8 matrix_stitching_flag=SPLIT_MODE;//Æ´½ÓÄ£Ê½
//¶¨Òå

//row 0 is hori pixel of img;support 8 out port
//row 1 is verti pixel of img
u16 gSplitScrRCPix[2][MAX_OUT_PORTS]={1920,1920,1920,1920,1920,1920,1920,1920,
																			1080,1080,1080,1080,1080,1080,1080,1080};

u16 g_screen_line_column_pixel[2][MAX_OUT_PORTS]={1920,1920,1920,1920,1920,1920,1920,1920,
																			1080,1080,1080,1080,1080,1080,1080,1080};
u16 gUserInScrRCPix[2][MAX_OUT_PORTS];//screen row & col pixel what user input
u16 gScrTotPix[2][MAX_OUT_PORTS];
																		
//u16 g_TFT_X_point[8]={0};
//u16 g_TFT_Y_point[8]={0};



//×î¶àÓĞ8¸öÊä³ö¿Ú£¬ÔÚTFTÆÁÉÏÒ²×î¶àÓĞ8¸ö»­Ãæ£¬Ã¿¸ö°üÀ¨ÆğÊ¼4×Ö½Ú
//½áÊøÎ»ÖÃ4×Ö½Ú£¬ÑÕÉ«ÖµÁ½¸ö×Ö½Ú£¬ËùÒÔ¶¨ÒåÒÔÏÂÊı×é¡£
u16 g_TFT_display_para[8][5]={0};

void Cal_Total_Point(void)
{
	u8 i;
	for(i=0;i<gScrCol;i++)
	{
		if(i==0)
			gScrTotPix[0][0]=g_screen_line_column_pixel[0][0];
		else
			gScrTotPix[0][i]=gScrTotPix[0][i-1]+g_screen_line_column_pixel[0][i];		
	}
	gScrTotX=gScrTotPix[0][i-1];
	for(i=0;i<gScrRow;i++)
	{
		if(i==0)
			gScrTotPix[1][0]=g_screen_line_column_pixel[1][0];
		else
			gScrTotPix[1][i]=gScrTotPix[1][i-1]+g_screen_line_column_pixel[1][i];
	}
	gScrTotY=gScrTotPix[1][i-1];
}





//Õâ¸öº¯ÊıÄ¿Ç°Ã»ÓĞÊ¹ÓÃ£¬ÎªÊ²Ã´//===2017,Jun 13 ; 21:50:51  ;BY YANG@BYCOX.COM===
//void display_TFT_set(void)
//{
//	u8 line,col;
//	u16 point_X,point_Y=0;
//	u16 point_end_X,point_end_Y=0;
//	point_X=(g_TFT_total_size_X-(gScrCol*2))/gScrCol;
//	point_Y=(g_TFT_total_size_Y-(gScrRow*2))/gScrRow;
//	
//	for(col=0;col<gScrCol;col++)
//	{
//		g_TFT_X_point[col]=g_screen_line_column_pixel[0][col]*point_X/1920;
//	}	
//	for(line=0;line<gScrRow;line++)
//	{
//		g_TFT_Y_point[line]=g_screen_line_column_pixel[1][line]*point_Y/1080;
//	}	
//	
//	point_end_Y=130;
//	for(line=0;line<gScrRow;line++)
//	{	
//		point_end_X=183;
//		
//		for(col=0;col<gScrCol;col++)
//		{
//			g_TFT_display_para[line*gScrCol+col][0]=point_end_X;
//			g_TFT_display_para[line*gScrCol+col][2]=point_end_X+g_TFT_X_point[col];	
//			point_end_X=point_end_X+g_TFT_X_point[col]+2;		
//			
//			g_TFT_display_para[line*gScrCol+col][1]=point_end_Y;			
//			g_TFT_display_para[line*gScrCol+col][3]=point_end_Y+g_TFT_Y_point[line];
//			g_TFT_display_para[line*gScrCol+col][4]=0xffff;
//		}
//		point_end_Y=point_end_Y+g_TFT_Y_point[line]+2;	
//	}	
//	
//}





//Õâ¸öº¯ÊıÄ¿Ç°Ã»ÓĞÊ¹ÓÃ£¬ÎªÊ²Ã´//===2017,Jun 13 ; 21:50:51  ;BY YANG@BYCOX.COM===
//u8 test[100];
//void Send_TFT_Screen_Para(void)
//{
//	u8 i=0;
//	u8 j=0;
//	test[0]=0xa5;
//	test[1]=0x5a;
//	test[3]=0x82;
//	test[4]=0x20;
//	test[5]=0x00;
//	
//	test[6]=0x00;
//	test[7]=0x04;
//	
//	test[8]=0x00;
//	test[9]=gScrRow* gScrCol;
//	
//	for(i=0;i<test[9];i++)
//	{
//		for(j=0;j<5;j++)
//		{
//			test[10+i*10+j*2]=g_TFT_display_para[i][j]>>8;
//			test[11+i*10+j*2]=g_TFT_display_para[i][j];
//		}
//	}
//	test[18+user_select_screen*10]=0xf8;
//	test[19+user_select_screen*10]=0x00;
//	test[2]=7+test[9]*10;
//	
//	UART4_SendBuf(test,test[2]+3);
//}




//²âÊÔÊä³ö¶Ë¿Ú,×îÖÕÔÚÕâÀï·¢ËÍÖ¸Áî¸øÖ÷°å
//O_F STPC 2 1 1 0x00FF00 
void OutPort_TestPatternColor(void)
{
	u8 i;
	u8 BoardID;
	

		
	BoardID= (g_CardTest_Port<=4)? MAINOUTBOARDID:SECOUTBOARDID;
	if(g_CardTest_Port>0&&g_CardTest_Port<=MAX_OUT_PORTS)
	{
		switch(g_CardTest_Color)
		{
			case 0://===2017,Jun 14 ; 11:2:27  ;BY YANG@BYCOX.COM=== display black
				sprintf(gCmdBuf,"O_F STPC %d %d 1 0x000000\r\n",BoardID,(g_CardTest_Port-1)%4);	//black
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
				break;
				
			case 1://INPUT_FPGA SET_TEST_PATTERN_COLOR [OutputPort] [IsEnable] [Color_Value] board-id + port-id ?
				sprintf(gCmdBuf,"O_F STPC %d %d 1 0xFF0000\r\n",BoardID,(g_CardTest_Port-1)%4);
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
				break;
			case 2:
				sprintf(gCmdBuf,"O_F STPC %d %d 1 0x00FF00\r\n",BoardID,(g_CardTest_Port-1)%4);
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
				break;
			case 3:
				sprintf(gCmdBuf,"O_F STPC %d %d 1 0x0000FF\r\n",BoardID,(g_CardTest_Port-1)%4);
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
				break;
			case 4:
				for(i=0;i<4;i++)
				{
					sprintf(gCmdBuf,"O_F STPC %d %d 0 0x000000\r\n",(i)/4+3,(i)%4);
					Uart_Send_Cmd_to_FPGA(gCmdBuf);
				}	
				break;

			case 9://===2017,Jun 14 ; 11:2:27  ;BY YANG@BYCOX.COM=== white 
				sprintf(gCmdBuf,"O_F STPC %d %d 1 0xffffff\r\n",BoardID,(g_CardTest_Port-1)%4);	//black
				Uart_Send_Cmd_to_FPGA(gCmdBuf);
				break;	
			default:	
				break;
		}

	}
}



/*******************************************************************************add********************/
//´óÆÁ²ÎÊıÉèÖÃå£¿ ¼¸ĞĞ¼¸ÁĞ
void ScreenSet_Display( )
{
	u8 i=0;
	for(i=0;i<g_ColNum;i++)
	{	
		gSplitScrRCPix[0][i]=gUserInScrRCPix[0][i];//ĞĞµÄÁĞ¸ß¶È?
		g_screen_line_column_pixel[0][i]=gUserInScrRCPix[0][i];
	}


	
	for(i=0;i<g_LineNum;i++)
	{
		gSplitScrRCPix[1][i]=gUserInScrRCPix[1][i];
		g_screen_line_column_pixel[1][i]=gUserInScrRCPix[1][i];		
	}
	
	g_stitching_screen_column=g_ColNum;
	g_stitching_screen_line=g_LineNum;
	gScrRow=g_stitching_screen_line;
	gScrCol=g_stitching_screen_column;
	Write_EEPROM_Byte(E2ROM_SCREEN_LINE_ADDR,g_stitching_screen_line);	//
	Write_EEPROM_Byte(E2ROM_SCREEN_COL_ADDR,g_stitching_screen_column);	//

	Write_Screen_line_column_pixel();	
	gScrCol=g_ColNum;
	gScrRow=g_LineNum;

	Cal_Total_Point();
}



//
void Matrix_Display_Function(void)
{
	u8 i=0;
	if(g_Mat_OutPort_No==0)  //ËùÓĞµÄ¶¼Êä³ö
	{
		if(g_Mat_InPort_No==0) //Ò»¶ÔÒ»Æ¥Åä
		{
			for(i=0;i<4;i++)
			{
				if(InSrcRes[i].resolution_width!=0)
				{	
					//#define BEGIN_X_INDEX 0
					//#define BEGIN_Y_INDEX 1
					//#define END_X_INDEX 2
					//#define END_Y_INDEX 3
					//#define WIDTH_INDEX 4
					//#define HIGH_INDEX 5
					//#define SOURCE_IN_INDEX 6
					//#define LAYER_INDEX 7
					//#define DEAL_INDEX 8
					//#define IMAGE_PARA_NUM
					gMatImgPara[i][BEGIN_X_INDEX]	=i*1920;
					gMatImgPara[i][BEGIN_Y_INDEX]	=0;
					gMatImgPara[i][END_X_INDEX]		=gMatImgPara[i][0]+1920;
					gMatImgPara[i][END_Y_INDEX]		=1080;
					gMatImgPara[i][WIDTH_INDEX]		=1920;
					gMatImgPara[i][HIGH_INDEX]		=1080;
					gMatImgPara[i][SOURCE_IN_INDEX]	=i;
					gMatImgPara[i][LAYER_INDEX]		=i+1;
					gMatImgPara[i][DEAL_INDEX]		=0;
				}	
				else
				{
					gMatImgPara[i][LAYER_INDEX]=0;
				}
			}//end for
		}//(g_Mat_InPort_No==0) //Ò»¶ÔÒ»Æ¥Åä
		else	//(g_Mat_InPort_No!=0) //Ò»¶ÔnÆ¥Åä
		{
			for(i=0;i<4;i++)
			{
				if(InSrcRes[i].resolution_width!=0)
				{
					gMatImgPara[i][0]=i*1920;
					gMatImgPara[i][1]=0;
					gMatImgPara[i][2]=gMatImgPara[i][0]+1920;
					gMatImgPara[i][3]=1080;
					gMatImgPara[i][4]=1920;
					gMatImgPara[i][5]=1080;
					gMatImgPara[i][SOURCE_IN_INDEX]=g_Mat_InPort_No-1;//is diff with if(g_Mat_InPort_No==0)
					gMatImgPara[i][7]=i+1;
					gMatImgPara[i][8]=0;		
				}	
				else
				{
					gMatImgPara[i][7]=0;
				}
			}	//end for
		}	//(g_Mat_InPort_No!=0) //Ò»¶ÔnÆ¥Å
	}//if(g_Mat_OutPort_No==0)  //ËùÓĞµÄ¶¼Êä³ö
	else//²»ÊÇËùÓĞµÄ¶Ë¿Ú¶¼Êä³ö
	{
		if(g_Mat_InPort_No==0) //Ò»¶ÔÒ»Æ¥Åä
		{
				if(InSrcRes[g_Mat_OutPort_No-1].resolution_width!=0)
				{
					gMatImgPara[g_Mat_OutPort_No-1][0]=(g_Mat_OutPort_No-1)*1920;
					gMatImgPara[g_Mat_OutPort_No-1][1]=0;
					gMatImgPara[g_Mat_OutPort_No-1][2]=gMatImgPara[g_Mat_OutPort_No-1][0]+1920;
					gMatImgPara[g_Mat_OutPort_No-1][3]=1080;
					gMatImgPara[g_Mat_OutPort_No-1][4]=1920;
					gMatImgPara[g_Mat_OutPort_No-1][5]=1080;
					gMatImgPara[g_Mat_OutPort_No-1][6]=g_Mat_OutPort_No-1;
					gMatImgPara[g_Mat_OutPort_No-1][7]=g_Mat_OutPort_No;
					gMatImgPara[g_Mat_OutPort_No-1][8]=0;	
				}	
				else
				{
					gMatImgPara[i][7]=0;
				}					
		}//g_Mat_InPort_No==0
		else//g_Mat_InPort_No!=0
		{	
				if(InSrcRes[g_Mat_InPort_No-1].resolution_width!=0)
				{			
					gMatImgPara[g_Mat_OutPort_No-1][0]=(g_Mat_OutPort_No-1)*1920;
					gMatImgPara[g_Mat_OutPort_No-1][1]=0;
					gMatImgPara[g_Mat_OutPort_No-1][2]=gMatImgPara[g_Mat_OutPort_No-1][0]+1920;
					gMatImgPara[g_Mat_OutPort_No-1][3]=1080;
					gMatImgPara[g_Mat_OutPort_No-1][4]=1920;
					gMatImgPara[g_Mat_OutPort_No-1][5]=1080;
					gMatImgPara[g_Mat_OutPort_No-1][6]=g_Mat_InPort_No-1;//diff
					gMatImgPara[g_Mat_OutPort_No-1][7]=g_Mat_OutPort_No;
					gMatImgPara[g_Mat_OutPort_No-1][8]=0;		
				}	
				else
				{
					gMatImgPara[i][7]=0;
				}		
		}
	}

	
	for(i=0;i<IMAGE_TOTAL_NUM;i++)
	{
		gImgPara[i][7]=0;
	}
	for(i=0;i<4;i++)
	{
		gImgPara[i][0]=gMatImgPara[i][0];
		gImgPara[i][1]=gMatImgPara[i][1];
		gImgPara[i][2]=gMatImgPara[i][2];
		gImgPara[i][3]=gMatImgPara[i][3];
		gImgPara[i][4]=gMatImgPara[i][4];	
		gImgPara[i][5]=gMatImgPara[i][5];
		gImgPara[i][6]=gMatImgPara[i][6];
		gImgPara[i][7]=gMatImgPara[i][7];
		gImgPara[i][8]=gMatImgPara[i][8];
	}
	if(matrix_stitching_flag== SPLIT_MODE)
	{
		for(i=0;i<4;i++)
		{
			g_screen_line_column_pixel[0][i]=1920;
			g_screen_line_column_pixel[1][i]=1080;
		}
		gScrRow=1;
		gScrCol=4;


			
		
		Cal_Total_Point();
		matrix_stitching_flag=MATRIX_MODE;
	}
	image_layout();
}


//WizardSet_OK_Function(void)-->Add_New_Image
//setup new plan/scheme
void Add_New_Image()
{
	u8 i=0;
	//°ÑĞÂÔöµÄ»­Ãæ½øĞĞ²¼¾ÖÏÔÊ¾
	gSplitImgPara[g_ImageNum_Set-1][0]=gInImgPara[0];
	gSplitImgPara[g_ImageNum_Set-1][1]=gInImgPara[1];
	gSplitImgPara[g_ImageNum_Set-1][2]=gInImgPara[2];
	gSplitImgPara[g_ImageNum_Set-1][3]=gInImgPara[3];
	gSplitImgPara[g_ImageNum_Set-1][4]=gInImgPara[2]-gInImgPara[0];
	gSplitImgPara[g_ImageNum_Set-1][5]=gInImgPara[3]-gInImgPara[1];
	gSplitImgPara[g_ImageNum_Set-1][6]=g_Image_Scr-1;
	gSplitImgPara[g_ImageNum_Set-1][7]=g_ImageNum_Set;
	gSplitImgPara[g_ImageNum_Set-1][8]=0;
	
	for(i=g_ImageNum_Set;i<IMAGE_TOTAL_NUM;i++)
	{
		gSplitImgPara[i][7]=0;
	}
	
	for(i=0;i<IMAGE_TOTAL_NUM;i++)
	{
		gImgPara[i][0]=gSplitImgPara[i][0];
		gImgPara[i][1]=gSplitImgPara[i][1];
		gImgPara[i][2]=gSplitImgPara[i][2];
		gImgPara[i][3]=gSplitImgPara[i][3];
		gImgPara[i][4]=gSplitImgPara[i][4];	
		gImgPara[i][5]=gSplitImgPara[i][5];
		gImgPara[i][6]=gSplitImgPara[i][6];
		gImgPara[i][7]=gSplitImgPara[i][7];
		gImgPara[i][8]=gSplitImgPara[i][8];
	}

	if(matrix_stitching_flag==MATRIX_MODE )
	{
		
		for(i=0;i<8;i++)
		{
			g_screen_line_column_pixel[0][i]=gSplitScrRCPix[0][i];
			g_screen_line_column_pixel[1][i]=gSplitScrRCPix[1][i];
		}	
		gScrRow=g_stitching_screen_line;
		gScrCol=g_stitching_screen_column;
		Cal_Total_Point();
		matrix_stitching_flag=SPLIT_MODE;
	}
	image_layout();
}

void DelScheme_OK()
{
	gSplitImgPara[g_SchemeNum-1][7]=0;
	gImgPara[g_SchemeNum-1][7]=0;
	scheme_save[g_SchemeNum-1]=0;
	Write_EEPROM_Byte(E2ROM_SCHEME_SAVE_ADDR+(g_SchemeNum-1)*2,scheme_save[g_SchemeNum-1]);
	Clear_Display(g_SchemeNum);
}


//±£´æ·½°¸
void Save_Scenario_OK()
{
	save_scenario_to_FPGA(g_SchemeNum);
	g_Current_Scene=g_SchemeNum;
	Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,g_Current_Scene);
	load_scenario_from_FPGA(g_Current_Scene);
}


void WizardSet_OK_Function(void)
{
	u8 i=0;

	char GpuBuf[SEND_LEN];
	ScreenSet_Display();
	gInImgPara[0]=0;;
	gInImgPara[1]=0;
	gInImgPara[2]=gScrTotPix[0][gScrCol-1];
	gInImgPara[3]=gScrTotPix[1][gScrRow-1];
	gInImgPara[6]=g_Image_Scr-1;
	g_ImageNum_Set=1;
	for(i=1;i<IMAGE_TOTAL_NUM;i++)
	{
		gSplitImgPara[i][7]=0;
	}
	Add_New_Image();
	delay_ms(1000);
	sprintf(GpuBuf,"SBC(5);DS16(10,10,'Ïòµ¼1.5 ±£´æ·½°¸   ',0);\r\n");
	GpuSend(GpuBuf);	
	save_scenario_to_FPGA(1);
	g_Current_Scene=1;
	Write_EEPROM_Byte(E2ROM_CURRENT_SCENE_ADDR,g_Current_Scene);
	load_scenario_from_FPGA(g_Current_Scene);
}

void Send_LCD_BACK(u8 data)
{
		char GpuBuf[SEND_LEN];
		sprintf(GpuBuf,"SEBL(%d);\r\n",data);
		GpuSend(GpuBuf);	
}


