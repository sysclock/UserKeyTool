/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: Main.c
 [Date]:        09-May-2012
 [Comment]:
   LCD subroutines.
 [Reversion History]:
*******************************************************************************/
#include "header.h"





void delay1000(vu32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0);
	}
}

void delay(vu32 num)
{
	int i;
	while(num-->0)
	{
 		i = 0x200000;
		while (i-- > 0);
	}
}

void delay100ns(vu32 num)
{
 	while(num-->0)
	{
	
	}
}


//清除显示
void clearlcd(void)
{
 	wcom(0x01);
}

void clearLineFromAddr(U8 addr)
{                                              
	chinesedisplay(addr, "                    ");  //one line 10*2 char
}


//----------------------------------------------------------------------------
//lighted LCD
void wcom(u8 d)
{
	 int i,j;
	 int w_bite;
	 int px1=0xf8;
	 delay1000(1);
	 LCD_CS_H;            //PB8=1

	for(i=0;i<8;i++)
	{
		w_bite=0x80&px1;
		if(w_bite==0x80) LCD_DAT_H;            //PB9=1          FIO0SET |= 0x00000040;    //p0.6 sda
		if(w_bite==0x00) LCD_DAT_L;
		px1=px1<<1;
		 
		LCD_CLK_H;  
		LCD_CLK_L;  
	 }
	 px1=d;
	 for(j=0;j<2;j++)
	 {
		for(i=0;i<4;i++)
		{
			w_bite=0x80&px1;
			if(w_bite==0x80) LCD_DAT_H;            //PB9=1          FIO0SET |= 0x00000040;    //p0.6 sda
			if(w_bite==0x00) LCD_DAT_L;
			px1=px1<<1;
			 
			LCD_CLK_H;  
			LCD_CLK_L;  
		    
		}
		for(i=0;i<4;i++)
		{
			LCD_DAT_L;
			LCD_CLK_H;  
			LCD_CLK_L;  

		}	 
	 }
	 LCD_CS_L;            //PB8=0           FIO0CLR |= 0x00000020;    //cs=0

}

void dat(U8 d)
{
	int i,j;
	int w_bite;
	int px1=0xfa;
	delay1000(2);
	LCD_CS_H;            //PB8=1

	 for(i=0;i<8;i++)
	 {
		w_bite=0x80&px1;
		if(w_bite==0x80) LCD_DAT_H;            //PB9=1          FIO0SET |= 0x00000040;    //p0.6 sda
		if(w_bite==0x00) LCD_DAT_L;
		px1=px1<<1;
		 
		LCD_CLK_H;  
		LCD_CLK_L;  
	 }
	 px1=d;
	 for(j=0;j<2;j++)
	 {
		 for(i=0;i<4;i++)
		 {
			w_bite=0x80&px1;
			if(w_bite==0x80) LCD_DAT_H;            //PB9=1          FIO0SET |= 0x00000040;    //p0.6 sda
			if(w_bite==0x00) LCD_DAT_L;
			px1=px1<<1;
			
			LCD_CLK_H;  
			LCD_CLK_L;  
		 }
		 for(i=0;i<4;i++)
		 {
		       LCD_DAT_L;	
			LCD_CLK_H;  
			LCD_CLK_L;  
		 }	   
	 
	 }
	 LCD_CS_L;            //PB8=0           FIO0CLR |= 0x00000020;    //cs=0

}

void printStr(U8 *str)
{
	while(*str!='\0')
	{
		dat(*str); //一个英文字符一次写完
		str++;
	}
}
//显示一位整数
void DisplayOneInt(U8 start,U8 ch)
{
	U8 chh[1]={0};
	chh[0]=ch+48;
	chinesedisplay(start,chh);
}
void chinesedisplay(U8 start,const U8 *pos)
{		 
	 U8 wbyte;
	 delay1000(100);
	 wcom(start); //设置第一行起始点
	 
	 while(*pos!=0)
	 {
		wbyte=*pos;
		dat(wbyte);//将汉字数组数据送入
	 	pos++;
	 }
}
void DisCurWinSizeValue(U16 wValue,U8 addr,U8 chh)
{
	U8 ch[7]={0};
	U8 ch1=0,ch2=0,ch3=0,i=0;
	ch1=wValue/1000%10;
	ch2=wValue/100%10;
	ch3=wValue/10%10;
	ch[i++]=chh;//'*';
	ch[i++]=' ';
	if((ch3==0)&&(ch2==0)&&(ch1==0))
	{
		ch[i++]=wValue+48;
	}
	else if((ch2==0)&&(ch1==0))
	{
		ch[i++]=ch3+48;
		ch[i++]=wValue%10+48;
	}
	else if(ch1==0)
	{
		ch[i++]=ch2+48;
		ch[i++]=ch3+48;
		ch[i++]=wValue%10+48;			
	}
	else
	{
		ch[i++]=ch1+48;
		ch[i++]=ch2+48;
		ch[i++]=ch3+48;
		ch[i++]=wValue%10+48;		
	}
	ch[i]=0;
	chinesedisplay(addr, "      ");
	chinesedisplay(addr, ch);
}

void DisCurrentInput(U8 image)
{
	U8 inputSrc1[6];
	U8 display_input;
	if(image==MAIN)
	{
			display_input=g_MainInput;
	}
	else
	{	
			display_input=g_SubInput;
	}
	switch(display_input)
	{
		case INPUTSOURCE_CVBS:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='1';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS2:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='2';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS3:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='3';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_CVBS4:
			inputSrc1[0]='A';
			inputSrc1[1]='V';
			inputSrc1[2]='4';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_VGA2:
			inputSrc1[0]='V';
			inputSrc1[1]='G';
			inputSrc1[2]='A';
			inputSrc1[3]='2';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_DVI:
			inputSrc1[0]='D';
			inputSrc1[1]='V';
			inputSrc1[2]='I';
			inputSrc1[3]='\0';
			break;
		case INPUTSOURCE_HDMI:
			inputSrc1[0]='H';
			inputSrc1[1]='D';
			inputSrc1[2]='M';
			inputSrc1[3]='I';
			inputSrc1[4]='\0';
			break;
		case INPUTSOURCE_YPBPR:
			inputSrc1[0]='Y';
			inputSrc1[1]='P';
			inputSrc1[2]='b';
			inputSrc1[3]='P';
			inputSrc1[4]='r';
			inputSrc1[5]='\0';
			break;
			case INPUTSOURCE_DVI2:
				inputSrc1[0]='D';
				inputSrc1[1]='V';
				inputSrc1[2]='I';
				inputSrc1[3]='2';
				inputSrc1[4]='\0';
				break;
			case INPUTSOURCE_USB:
				inputSrc1[0]='U';
				inputSrc1[1]='S';
				inputSrc1[2]='B';
				inputSrc1[3]='\0';
				break;
	}	
	chinesedisplay(0x98,inputSrc1);
}
void DisInputSrcFormatStr(U8 addr)
{  	 
	U16 HSize=0,VSize=0;	
	U8 image;
	if(g_MutiWindow==SAMEPIPIMAGE)//single channel
	{
		if(g_NextPointImage==SUB)
		{
			image=MAIN;
		}
		else
		{
			image=SUB;
		}
	}
	else
	{		
		if(g_MutiWindow==SINGLEIMAGE)
		{
			image=MAIN;
		}
		else
		{
			image=g_ActiveImageValue;
		}
	}
	GetInputSrcFormat(image,&HSize,&VSize);
////如果读到为0，再一次进行读取
	if(HSize==0 &&VSize==0)
	{
		delay_ms(100);
		GetInputSrcFormat(image,&HSize,&VSize);
	}
	
	
	if(HSize==720&&VSize==240)
		VSize=480;
	else if(HSize==720&&VSize==288)
		VSize=576;
	DisFourNumValue(HSize,addr);
	DisCurWinSizeValue(VSize,addr+2,'x');
	DisCurrentInput(image);
}

void LCDStart(void)
{ 
	U8 led_no;
//	U32 led_key=0xffffffff;
	
	U8 InitInfo[]=
	{
		0xC6,0xF4,
		0xB6,0xAF,
		0xD6,0xD0,
		0xA1,0xA2,
		0xC7,0xEB,
		0xC9,0xD4,
		0xBA,0xF2,
		0xA1,0xAD,0 
	};
	LCD_CLK_H;  
	LCD_CS_H;
	LCD_DAT_H;
	LCD_CS_L;
	LCD_CLK_L;  
	delay(1); 
	wcom(0x38);   //功能设定
	delay(1);
	wcom(0x08);   //关显示指令
	delay(1); 
	wcom(0x06); //游标右移
	delay(1); 
	wcom(0x01);   //清屏
	delay(1);
	wcom(0x0c);   //开显示指令 游标和游标位置关闭
	if(g_LanguageFlag==CHINESE)
	chinesedisplay(0x81, InitInfo);
	else if(g_LanguageFlag==ENGLISH)
		chinesedisplay(0x81, "Start,Please wait");
	for(led_no=0;led_no<KEY_NUM;led_no++)
	{
		SetKeyLEDLight((_KeypadNum)led_no);
		delay1000(500);
	}

	SetKeyLEDLight(KEY_NUM); //清除LED//清除LED

	wcom(0x90);
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	delay1000(500); 
	dat(0xa1);dat(0xf6);
	 
}

void LCD_Reset(void)
{
	LCD_CLK_H;  
	LCD_CS_H;
	LCD_DAT_H;
	LCD_CS_L;
	LCD_CLK_L;  
	delay(1); 
	wcom(0x38);   //功能设定
	delay(1);
	wcom(0x08);   //关显示指令
	delay(1); 
	wcom(0x06); //游标右移
	delay(1); 
// 	wcom(0x01);   //清屏
// 	delay(1);
	wcom(0x0c);   //开显示指令 游标和游标位置关闭

}
