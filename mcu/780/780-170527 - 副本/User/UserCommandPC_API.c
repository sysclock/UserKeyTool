
//
//#include "header.h"
//
//
//
//
//
//
//
//
//
//
//
//
//
// void KeyPad_SetupWizard_to_FPGA()
// {
//
//
//
//
//
//	char i;
//	char NumberOfOutputCard=0;
//	char Slot_ID  = 0;
//	
//	char Slot_ID1 = 1;
//	char scalerNo = 0;
////	const char scalerOutNoAdd = 4;
//	char outPort=0;
//
//	//===========Scaler Various ============= 
//
//	//B3 B4
//	U32 Scaler0_InImageH=0,	Scaler0_InImageV=0;
//	U32 Scaler1_InImageH=0,	Scaler1_InImageV=0;
//	U32 Scaler2_InImageH=0,	Scaler2_InImageV=0;
//	U32 Scaler3_InImageH=0,	Scaler3_InImageV=0;
//
//	//B5 B7
//	U32 Scaler0_InHPos=0,	Scaler0_InVPos=0;
//	U32 Scaler1_InHPos=0,	Scaler1_InVPos=0;
//	U32 Scaler2_InHPos=0,	Scaler2_InVPos=0;
//	U32 Scaler3_InHPos=0,	Scaler3_InVPos=0;
//
//	//B6 B8	
//	U32 Scaler0_InHEnd=0,	Scaler0_InVEnd=0;
//	U32 Scaler1_InHEnd=0,	Scaler1_InVEnd=0;
//	U32 Scaler2_InHEnd=0,	Scaler2_InVEnd=0;
//	U32 Scaler3_InHEnd=0,	Scaler3_InVEnd=0;
//
//	////B9 B10
//	U32 Scaler0_OutWidth=0,	Scaler0_OutHeight=0;
//	U32 Scaler1_OutWidth=0,	Scaler1_OutHeight=0;
//	U32 Scaler2_OutWidth=0,	Scaler2_OutHeight=0;
//	U32 Scaler3_OutWidth=0,	Scaler3_OutHeight=0;
//
///*
//	U32 Image1_H=0,	Image1_V=0;
//	U32 Image2_H=0,	Image2_V=0;
//	U32 Image3_H=0,	Image3_V=0;
//	U32 Image4_H=0,	Image4_V=0;
//*/
//
//	typedef struct{
//		U8 	Scaler_no;//B1
//		U8 	inChID;//B2
//		U32 Sc_InImageH;//B3
//		U32 Sc_InImageV;//B4
//		U32 Sc_InHPos;//B5
//		U32 Sc_InHEnd;//B6
//		U32 Sc_InVPos;//B7		
//		U32 Sc_InVEnd;//B8
//		U32 Sc_OutH;//B9
//		U32 Sc_OutV;//B10	
//	}ScalerSetPara;
//	
//	ScalerSetPara ScalerPara[4]; 	
//
//
//	typedef struct{
//		U8  ID;//inChID;//Channel;
//		U32 H;//inImage_H;
//		U32 V;//inImage_V;
//		}inImageRes;
//	
//	inImageRes ImageRes[4];
//	
//	//===========Add Windows Various ============= 
//	typedef struct{
//		U8	PoseScalerIdx;
//		U8	ImageLayer;
//		U32 InLeft;
//		U32 InTop;
//		U32 InRight;
//		U32 InBottom;
//		U32 DispLeft;
//		U32 DispTop;
//		U32 DispRight;
//		U32 DispBottom;	
//	}AddWinPara;
//
//	AddWinPara winPara[4];
//	
//	WORD W_WizardDataArry[12] = {0};
//	WORD W_WizardDataArry_T[12] = {0};
//	if((iMode<=0)||(iMode>DISPLAY_MODE_INDEX_MAX))
//		return;
//
//	//INPUT_FPGA SET_KEY_MODE 1
//	CMD_INPUTFPGA_SET_KEY_MODE(iMode);
//	
//	CMD_INPUTFPGA_CLEAR_SCALER_USE();
//	CMD_BOARD_RMG();
//
//	//set output resolution:1920*1080p60
//	CMD_BOARD_SOT(3);
//
//	
//	NumberOfOutputCard=gSt_OutputBoarSiI9136.B_ActiveChip;	
//
//
//			ImageRes[0].ID= source2slotID(g_Image1Scr);
//			CMD_BOARD_RISR(ImageRes[0].ID);
//			ImageRes[0].H=gW_ResolutionWidth;
//			ImageRes[0].V=gW_ResolutionHeight;//Image1_V
//		#ifdef DEBUG_STATE_MSG
//		printf("BOARD_RISR before big switch ImageRes[0].ID=%d,ImageRes[0].H=%d,ImageRes[0].V=%d\r\n", ImageRes[0].ID, ImageRes[0].H,ImageRes[0].V);
//		#endif	
//
//
//
//
//
//
//
//
// 
//					//CMD_BOARD_RISR(Slot_ID);
//					ImageRes[0].ID=source2slotID(g_Image1Scr);
//					CMD_BOARD_RISR(ImageRes[0].ID);
//					ImageRes[0].H=gW_ResolutionWidth;
//					ImageRes[0].V=gW_ResolutionHeight;
//					switch(g_CardLayout)
//						{
//						case HoriSplitMode: //左右拼接							
//							Scaler0_InHEnd =(ImageRes[0].H*g_Card1HSize)/g_ScreenTotalHSize;	
//							Scaler0_InVEnd = ImageRes[0].V;
//							Scaler1_InHEnd = ImageRes[0].H-Scaler0_InHEnd;
//							Scaler1_InVEnd = ImageRes[0].V;
//							
//							Scaler0_OutHeight=g_Card1VSize;
//							#ifdef DEBUG_STATE_MSG
//							printf("cardLayout 0leftright, g_Card1HSize=:%d,    g_ScreenTotalHSize=:%d,    gW_ResolutionWidth=:%d,    Scaler0_InHEnd=:%d \r\n",
//									g_Card1HSize,g_ScreenTotalHSize,	gW_ResolutionWidth,Scaler0_InHEnd
//									);				
//							#endif
//							break;
//						case VertiSplitMode://上下拼接
//							Scaler0_InHEnd = ImageRes[0].H;
//							Scaler0_InVEnd = ImageRes[0].V;
//							Scaler0_OutHeight=g_Card1VSize+g_Card2VSize;
//							break;
//						}
//					#ifdef DEBUG_STATE_MSG
//					printf("gW_ResolutionWidth=:%d,    gW_ResolutionHeight=:%d,    g_Card1HSize=:%d,    g_Card1VSize=:%d,    g_ScreenTotalHSize=:%d,    g_ScreenTotalVSize=:%d \r\n",
//							gW_ResolutionWidth,gW_ResolutionHeight,g_Card1HSize,g_Card1VSize,
//							g_ScreenTotalHSize,g_ScreenTotalVSize
//							);
//
//					printf("Scaler0_InHEnd=:%d,    Scaler0_InVEnd=:%d,    Scaler1_InHEnd=:%d,    Scaler1_InVEnd=:%d \r\n",
//							Scaler0_InHEnd,Scaler0_InVEnd,Scaler1_InHEnd,Scaler1_InVEnd
//							);
//					#endif
//
//				//设置缩放器-0
//					W_WizardDataArry[0]=BOARD_ID;	//Slot 0
//					scalerNo = 0;
//					W_WizardDataArry[1]=scalerNo;	//scaler 0					
//					W_WizardDataArry[2]=ImageRes[0].ID;
//					W_WizardDataArry[3]=ImageRes[0].H;//输入信号的长read inBoard resolution first
//					W_WizardDataArry[4]=ImageRes[0].V;//输入信号的高
//					W_WizardDataArry[5]=0;
//					W_WizardDataArry[6]=Scaler0_InHEnd-1;
//					W_WizardDataArry[7]=0;
//					W_WizardDataArry[8]=Scaler0_InVEnd-1;					
//					W_WizardDataArry[9]=g_Card1HSize;
//					//if(g_CardLayout==1)//上下拼接
//					W_WizardDataArry[10]=Scaler0_OutHeight;//g_Card1VSize+g_Card2VSize;
//					//else
//					//	W_WizardDataArry[10]=g_Card1VSize;
//					
//					CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
//					#ifdef DEBUG_STATE_MSG
//					printf("0: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
//							W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
//							W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
//							W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
//							);
//					#endif
//					
//					//设置缩放器-1  上下拼接的时候只需要一个缩放器
//					if(g_CardLayout==0)
//						{
//							W_WizardDataArry[0]=BOARD_ID;	//BOARD 0
//							scalerNo = 1;
//							W_WizardDataArry[1]=scalerNo;	//scaler 1
//							W_WizardDataArry[2]=ImageRes[0].ID;//SLOT ID
//							W_WizardDataArry[3]=ImageRes[0].H;//read inBoard resolution first
//							W_WizardDataArry[4]=ImageRes[0].V;
//							W_WizardDataArry[5]=Scaler0_InHEnd;
//							W_WizardDataArry[6]=ImageRes[0].H-1;
//							W_WizardDataArry[7]=0;
//							W_WizardDataArry[8]=Scaler1_InVEnd-1;					
//							W_WizardDataArry[9]=g_Card2HSize;
//							W_WizardDataArry[10]=g_Card2VSize;
//							
//							CMD_INPUTFPGA_SCALER_SETUP(W_WizardDataArry);
//							#ifdef DEBUG_STATE_MSG
//							printf("1: INPUT_FPGA SCALER_SETUP  %d %d %d %d %d %d %d %d %d %d %d \r\n",
//									W_WizardDataArry[0],W_WizardDataArry[1],W_WizardDataArry[2],W_WizardDataArry[3],
//									W_WizardDataArry[4],W_WizardDataArry[5],W_WizardDataArry[6],W_WizardDataArry[7],
//									W_WizardDataArry[8],W_WizardDataArry[9],W_WizardDataArry[10]
//									);
//							#endif
//						}
//
//					//视频开窗-0
//						//CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(0);	//output port					
//						
//						W_WizardDataArry_T[0]=BOARD_ID;	//Slot 0
//						outPort=0;	
//						W_WizardDataArry_T[1]=outPort;	//outport no. 0
//						W_WizardDataArry_T[2]=g_ScreenOutputHeight;	//g_Card1HSize;
//						W_WizardDataArry_T[3]=g_ScreenOutputWidth;	//g_Card1VSize;						
//						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);						
//						printf("0: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
//							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]
//							);
//						
//						scalerNo = 4;
//						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
//						W_WizardDataArry_T[1]=1;	//image Layer						
//						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
//						W_WizardDataArry_T[3]=ZERO;	//InTop
//						W_WizardDataArry_T[4]=g_Card1HSize;	//InRight
//						W_WizardDataArry_T[5]=g_Card1VSize;	//Inbottom
//						W_WizardDataArry_T[6]=ZERO;	//DispLeft
//						W_WizardDataArry_T[7]=ZERO;	//DispTop
//						W_WizardDataArry_T[8]=g_Card1HSize;	//DispRight
//						W_WizardDataArry_T[9]=g_Card1VSize;	//DispBottom
//						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
//						#ifdef DEBUG_STATE_MSG
//						printf("Win-0: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d  \r\n",
//							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
//							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
//							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
//							);
//						#endif
//						CMD_INPUTFPGA_ADD_WINDOW_END();	
//						
//						
//						//视频开窗-1
//						//CMD_INPUTFPGA_CLEAR_OUTPUT_WINDOWS(1);	//output port					
//						W_WizardDataArry_T[0]=BOARD_ID;
//						outPort=1;
//						W_WizardDataArry_T[1]=outPort;	//outport no. 0
//						W_WizardDataArry_T[2]=g_ScreenOutputHeight;	//g_Card2HSize;Must be Standard Resolution
//						W_WizardDataArry_T[3]=g_ScreenOutputWidth;	//g_Card2VSize;					
//						
//						CMD_INPUTFPGA_SET_WINDOW_LAYOUT(W_WizardDataArry_T);
//						#ifdef DEBUG_STATE_MSG
//						printf("Win-1: INPUT_FPGA SET_WINDOW_LAYOUT %d %d %d %d \r\n",
//							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3]
//							);
//						#endif
//						if(g_CardLayout==1)
//							scalerNo=4;
//						else
//							scalerNo=5;
//						
//						W_WizardDataArry_T[0]=scalerNo;	//output of scaler 0
//						W_WizardDataArry_T[1]=1;	//image Layer						
//						W_WizardDataArry_T[2]=ZERO;	//inLeft	:in,Cut of input image
//						if(g_CardLayout==1)
//								W_WizardDataArry_T[3]=g_Card1VSize;
//							else
//								W_WizardDataArry_T[3]=ZERO;	//InTop
//						W_WizardDataArry_T[4]=g_Card2HSize;	//InRight
//						if(g_CardLayout==1)
//							W_WizardDataArry_T[5]=W_WizardDataArry_T[3]+g_Card2VSize;
//							else
//								W_WizardDataArry_T[5]=g_Card2VSize;	//Inbottom
//						W_WizardDataArry_T[6]=ZERO;	//DispLeft
//						W_WizardDataArry_T[7]=ZERO;	//DispTop
//						W_WizardDataArry_T[8]=g_Card2HSize;	//DispRight
//						W_WizardDataArry_T[9]=g_Card2VSize;	//DispBottom
//						CMD_INPUTFPGA_ADD_WINDOW(W_WizardDataArry_T);
//						#ifdef DEBUG_STATE_MSG
//						printf("1: INPUT_FPGA ADD_WINDOWS %d %d %d %d %d %d %d %d %d %d \r\n",
//							W_WizardDataArry_T[0],W_WizardDataArry_T[1],W_WizardDataArry_T[2],W_WizardDataArry_T[3],
//							W_WizardDataArry_T[4],W_WizardDataArry_T[5],W_WizardDataArry_T[6],W_WizardDataArry_T[7],
//							W_WizardDataArry_T[8],W_WizardDataArry_T[9]
//							);
//						#endif
//						CMD_INPUTFPGA_ADD_WINDOW_END();					
//					
//					#ifdef DEBUG_STATE_MSG
//					printf("1x2,Card Number: %d, Image Number: %d\r\n",g_CardNumber,g_ImageNumber);
//					#endif
//}
//
//
//
//
//
//
//
//
//
//
//
//
//











