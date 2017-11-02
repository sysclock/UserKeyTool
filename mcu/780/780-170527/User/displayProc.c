/*************************************************  
Copyright (C), 2013-2023, Bycox Tech. Co., Ltd.  
File name:
Author:  LYG  setup~      
Version:        
Date: 20170322 Setup  

Description:    
	// ÓÃÓÚÏêÏ¸ËµÃ÷´Ë³ÌÐòÎÄ¼þÍê³ÉµÄÖ÷Òª¹¦ÄÜ£¬ÓëÆäËûÄ£¿é                  
	// »òº¯ÊýµÄ½Ó¿Ú£¬Êä³öÖµ¡¢È¡Öµ·¶Î§¡¢º¬Òå¼°²ÎÊý¼äµÄ¿Ø                  
	// ÖÆ¡¢Ë³Ðò¡¢¶ÀÁ¢»òÒÀÀµµÈ¹ØÏµ  Others:         
	// ÆäËüÄÚÈÝµÄËµÃ÷  

Function List:  // Ö÷Òªº¯ÊýÁÐ±í£¬Ã¿Ìõ¼ÇÂ¼Ó¦°üÀ¨º¯ÊýÃû¼°¹¦ÄÜ¼òÒªËµÃ÷    
	1,set display para about processor output image
	2,

History:        // ÐÞ¸ÄÀúÊ·¼ÇÂ¼ÁÐ±í£¬Ã¿ÌõÐÞ¸Ä¼ÇÂ¼Ó¦°üÀ¨ÐÞ¸ÄÈÕÆÚ¡¢ÐÞ¸Ä Õß¼°ÐÞ¸ÄÄÚÈÝ¼òÊö      
	1,//===2017,Jun 16 ; 8:52:20  ;BY YANG@BYCOX.COM===  
		support from 4*4-->8*8
	2. ...

*************************************************/
#define _DISPLAYPROC_C_

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

// the working mode under matrix mode
u8 gMatrixMode;

Resolution_Data gOutRes={1920,1080,"DVI1"};

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



//×î¶àÓÐ8¸öÊä³ö¿Ú£¬ÔÚTFTÆÁÉÏÒ²×î¶àÓÐ8¸ö»­Ãæ£¬Ã¿¸ö°üÀ¨ÆðÊ¼4×Ö½Ú
//½áÊøÎ»ÖÃ4×Ö½Ú£¬ÑÕÉ«ÖµÁ½¸ö×Ö½Ú£¬ËùÒÔ¶¨ÒåÒÔÏÂÊý×é¡£
u16 g_TFT_display_para[8][5]={0};

void Calc_total_point(void)
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
//´óÆÁ²ÎÊýÉèÖÃå£¿ ¼¸ÐÐ¼¸ÁÐ
void ScreenSet_Display( )
{
	u8 i=0;
	for(i=0;i<g_ColNum;i++)
	{	
		gSplitScrRCPix[0][i]=gUserInScrRCPix[0][i];//ÐÐµÄÁÐ¸ß¶È?
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

	Calc_total_point();
}



//gMatImgParaÊÖ¹¤¸¶¹Ì¶¨ÖµÊÇ²»¶ÔµÄ
//¾ØÕóÄ£Ê½µÄÊµÏÖº¯Êý
//call by DirectSetValue()
void Matrix_Display_Function(void)
{
	u8 i=0;
	
	//STEP 1 MODE VALUE
	if(gMatOutpNo==0)
	{
		if(gMatInpNo==0)
			gMatrixMode=MAT_MOD_0vs0;
		else
			gMatrixMode=MAT_MOD_Nvs0;
	}
	else//gMatOutpNo!=0
	{
		if(gMatInpNo==0)
			gMatrixMode=MAT_MOD_0vsM;
		else
			gMatrixMode=MAT_MOD_NvsM;
	}	

	for(i=0;i<IN_PORT_NUM;i++)
	{	
		gMatImgPara[i][LAYER_INDEX]=0;
	}		


	//STEP2	¾ØÕóÄ£Ê½ÊýÖµ¼ÆËã
	switch(gMatrixMode)
	{
		case MAT_MOD_0vs0:	
			for(i=0;i<IN_PORT_NUM;i++)			
			{	
				
				if(InSrcRes[i].w!=0)
				{	
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
			}
			break;

		case MAT_MOD_Nvs0:		
				
			#ifdef DEBUG88			
			printf("\r\n enter n vs 0 mode gMatInpNo=%d \r\n",gMatInpNo);
			#endif
			//for(i=0;i<IN_PORT_NUM;i++)//Be Wrong~
//			for(i=0;i<OUT_PORT_NUM;i++)
//			{	
//							
//				if(InSrcRes[gMatInpNo-1].w!=0)
//				{					
//					gMatImgPara[i][BEGIN_X_INDEX]=i*1920;
//					gMatImgPara[i][BEGIN_Y_INDEX]=0;
//					gMatImgPara[i][END_X_INDEX]=gMatImgPara[i][BEGIN_X_INDEX]+1920;
//					gMatImgPara[i][END_Y_INDEX]=1080;
//					gMatImgPara[i][WIDTH_INDEX]=1920;
//					gMatImgPara[i][HIGH_INDEX]=1080;
//					gMatImgPara[i][SOURCE_IN_INDEX]=gMatInpNo-1;//is diff with if(gMatInpNo==0) always error
//					gMatImgPara[i][LAYER_INDEX]=i+1;
//					gMatImgPara[i][DEAL_INDEX]=0;		
//				}
//			}			//end for
			for(i=0;i<OUT_PORT_NUM;i++)
			{	
							
				if(InSrcRes[gMatInpNo-1].w!=0)
				{					
					gMatImgPara[i][BEGIN_X_INDEX]=0;//i*1920;
					gMatImgPara[i][BEGIN_Y_INDEX]=0;
					gMatImgPara[i][END_X_INDEX]=gOutRes.w-1;//gMatImgPara[i][BEGIN_X_INDEX]+1920;
					gMatImgPara[i][END_Y_INDEX]=gOutRes.h-1;//1080;
					gMatImgPara[i][WIDTH_INDEX]=gOutRes.w;//1920;
					gMatImgPara[i][HIGH_INDEX]=gOutRes.h;//1080;
					gMatImgPara[i][SOURCE_IN_INDEX]=gMatInpNo-1;//is diff with if(gMatInpNo==0) always error
					gMatImgPara[i][LAYER_INDEX]=i+1;	//output port
					gMatImgPara[i][DEAL_INDEX]=0;		
				}
			}
			break;	//(gMatInpNo!=0) 


		case MAT_MOD_0vsM:		//it must be Wrong~
				
				if(InSrcRes[gMatOutpNo-1].w!=0)
				{					
					gMatImgPara[gMatOutpNo-1][BEGIN_X_INDEX]	=(gMatOutpNo-1)*1920;
					gMatImgPara[gMatOutpNo-1][BEGIN_Y_INDEX]	=0;
					gMatImgPara[gMatOutpNo-1][END_X_INDEX]	=gMatImgPara[gMatOutpNo-1][0]+1920;
					gMatImgPara[gMatOutpNo-1][END_Y_INDEX]	=1080;
					gMatImgPara[gMatOutpNo-1][WIDTH_INDEX]	=1920;
					gMatImgPara[gMatOutpNo-1][HIGH_INDEX]	=1080;
					gMatImgPara[gMatOutpNo-1][SOURCE_IN_INDEX]=gMatOutpNo-1;
					gMatImgPara[gMatOutpNo-1][LAYER_INDEX]	=gMatOutpNo;
					gMatImgPara[gMatOutpNo-1][DEAL_INDEX]	=0;	
				}									
			break;	
		

		case MAT_MOD_NvsM:			
			if(InSrcRes[gMatInpNo-1].w!=0)//ÓÐÐÅºÅÊäÈë£¬ÊäÈë·ç±æÂÊ²»Îª0
				{	
					
					gMatImgPara[gMatOutpNo-1][BEGIN_X_INDEX]	=(gMatOutpNo-1)*1920;
					gMatImgPara[gMatOutpNo-1][BEGIN_Y_INDEX]	=0;
					gMatImgPara[gMatOutpNo-1][END_X_INDEX]	=gMatImgPara[gMatOutpNo-1][0]+1920;
					gMatImgPara[gMatOutpNo-1][END_Y_INDEX]	=1080;
					gMatImgPara[gMatOutpNo-1][WIDTH_INDEX]	=1920;
					gMatImgPara[gMatOutpNo-1][HIGH_INDEX]	=1080;
					gMatImgPara[gMatOutpNo-1][SOURCE_IN_INDEX]=gMatInpNo-1;//diff
					gMatImgPara[gMatOutpNo-1][LAYER_INDEX]	=gMatOutpNo;
					gMatImgPara[gMatOutpNo-1][DEAL_INDEX]	=0;		
				}					
			break;
			
		default:	break;

		}




	//STEP2 ÊýÖµÇ¨ÒÆ
	for(i=0;i<IN_PORT_NUM;i++)
	{
		gImgPara[i][BEGIN_X_INDEX]=gMatImgPara[i][0];
		gImgPara[i][BEGIN_Y_INDEX]=gMatImgPara[i][1];
		gImgPara[i][END_X_INDEX]=gMatImgPara[i][2];
		gImgPara[i][END_Y_INDEX]=gMatImgPara[i][3];
		gImgPara[i][WIDTH_INDEX]=gMatImgPara[i][4];	
		gImgPara[i][HIGH_INDEX]=gMatImgPara[i][5];
		gImgPara[i][SOURCE_IN_INDEX]=gMatImgPara[i][6];
		gImgPara[i][LAYER_INDEX]=gMatImgPara[i][7];
		gImgPara[i][DEAL_INDEX]=gMatImgPara[i][8];
	}


	//STEP 4	Æ´½ÓÄ£Ê½
	if(matrix_stitching_flag== SPLIT_MODE)
	{
		//for(i=0;i<4;i++)
		for(i=0;i<IN_PORT_NUM;i++)
		{
			g_screen_line_column_pixel[0][i]=1920;
			g_screen_line_column_pixel[1][i]=1080;
		}
		gScrRow=1;
		gScrCol=4;			
		
		Calc_total_point();
		matrix_stitching_flag=MATRIX_MODE;
	}


	//STEP 5 gImgParaÊý¾Ý×ª»¯ÎªËõ·ÅÆ÷µÈ²ÎÊý
	image_layout_Para_Process();
}








//WizardSet_OK_Function(void)-->Add_New_Image
//setup new plan/scheme
void Add_New_Image()
{
	u8 i=0;
	//°ÑÐÂÔöµÄ»­Ãæ½øÐÐ²¼¾ÖÏÔÊ¾
	gSplitImgPara[g_ImageNum_Set-1][0]=gInImgPara[0];
	gSplitImgPara[g_ImageNum_Set-1][1]=gInImgPara[1];
	gSplitImgPara[g_ImageNum_Set-1][2]=gInImgPara[2];
	gSplitImgPara[g_ImageNum_Set-1][3]=gInImgPara[3];
	gSplitImgPara[g_ImageNum_Set-1][4]=gInImgPara[2]-gInImgPara[0];
	gSplitImgPara[g_ImageNum_Set-1][5]=gInImgPara[3]-gInImgPara[1];
	gSplitImgPara[g_ImageNum_Set-1][6]=g_Image_Scr-1;
	gSplitImgPara[g_ImageNum_Set-1][7]=g_ImageNum_Set;
	gSplitImgPara[g_ImageNum_Set-1][8]=0;
	
	for(i=g_ImageNum_Set;i<IN_PORT_NUM;i++)
	{
		gSplitImgPara[i][7]=0;
	}
	
	for(i=0;i<IN_PORT_NUM;i++)
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
		Calc_total_point();
		matrix_stitching_flag=SPLIT_MODE;
	}
	image_layout_Para_Process();
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
	for(i=1;i<IN_PORT_NUM;i++)
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

void set_lcd_backLight(u8 data)
{
		char GpuBuf[SEND_LEN];
		sprintf(GpuBuf,"SEBL(%d);\r\n",data);
		GpuSend(GpuBuf);	
}

#ifdef DEBUG88
U8 lcd_disp_debug_info(U8 x,U8 y,char *dispStr)//+++++++++++++++++++++++++++++
{
	char GpuBuf[SEND_LEN];
	sprintf(GpuBuf,"DS12(%d,%d,'%s',0);",x,y,dispStr);
	GpuSend(GpuBuf);
	return 1;
}

#endif


/*
//run in vs2010
#include "stdafx.h"
#include <string.h>

#include <iostream>
 
using namespace std;

void can(int i,char str[])
{
    char m[]="hello";
    char *n="world";



    if(i>=0)
        strcpy(str,m);
    else
        strcpy(str,n);


}


void can2(char *sss)
{
    char m[]="hello";
    char *n="world";

	//cout << sss << endl;
	printf(sss);
}

int main()
{
    char s[100];

    can(-1,s);
    
	cout << s << endl;


    can(1,s);
    cout << s << endl;

	can2("hahahaha");
}
*/
