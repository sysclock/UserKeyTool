/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: Main.c
 [Date]:        09-May-2012
 [Comment]:
   LCD subroutines.
 [Reversion History]:
*******************************************************************************/
#include "header.h"
#include <stdio.h>

void delay1000(U32 num)
{
 	int i;
	while(num-->0)
	{
		i = 0x200;
		while (i-- > 0);
	}
}

void delay(U32 num)
{
	int i;
	while(num-->0)
	{
 		i = 0x200000;
		while (i-- > 0);
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
void wcom(U8 d)
{
	 int i,j;
	 int w_bite;
	 int px1=0xf8;
	 delay1000(1);
	 LCD_CS_H;//FIO0SET |= 0x00000020;    //cs=1 p0.5

	 for(i=0;i<8;i++)
	 {
	   w_bite=0x80&px1;
	   if(w_bite==0x80) LCD_DAT_H;//FIO0SET |= 0x00000040;    //p0.6 sda
	   if(w_bite==0x00) LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
	   px1=px1<<1;
	 
	   LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
	   LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
	 }
	 px1=d;
	 for(j=0;j<2;j++)
	 {
	   for(i=0;i<4;i++)
	   {
	       w_bite=0x80&px1;
	       if(w_bite==0x80) LCD_DAT_H;//FIO0SET |= 0x00000040;    //p0.6 sda
	       if(w_bite==0x00) LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
	       px1=px1<<1;
	 
	       LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
	       LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
	    
	   }
	   for(i=0;i<4;i++)
	   {
	       LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
		LCD_CLK_H;//  FIO0SET |= 0x00000010;    //p0.4 clk
	       LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
	       
	   }
	   
	 
	 }
	 LCD_CS_L;//FIO0CLR |= 0x00000020;    //cs=0

}

void dat(U8 d)
{
	 int i,j;
	 int w_bite;
	 int px1=0xfa;
	 delay1000(2);
	 LCD_CS_H;//FIO0SET |= 0x00000020;    //cs=1 p0.5

	 for(i=0;i<8;i++)
	 {
		   w_bite=0x80&px1;
		   if(w_bite==0x80) LCD_DAT_H;//FIO0SET |= 0x00000040;    //p0.6 sda
		   if(w_bite==0x00) LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
		   px1=px1<<1;
		 
		   LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
		   LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
	 }
	 px1=d;
	 for(j=0;j<2;j++)
	 {
		   for(i=0;i<4;i++)
		   {
		       w_bite=0x80&px1;
		       if(w_bite==0x80) LCD_DAT_H;//FIO0SET |= 0x00000040;    //p0.6 sda
		       if(w_bite==0x00) LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
		       px1=px1<<1;
		 
		       LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
		       LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
		    
		   }
		   for(i=0;i<4;i++)
		   {
		      LCD_DAT_L;//FIO0CLR |= 0x00000040;    //p0.6 sda
		      LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
		      LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
		       
		   }	   
	 
	 }
	 LCD_CS_L;//FIO0CLR |= 0x00000020;    //cs=0

}


void LCDStart2OffBack(void)
{	
	 wcom(0x38);   //功能设定
	 delay(1);
	 wcom(0x08);   //关显示指令
	 delay(1); 
	 wcom(0x06); //游标右移
	 delay(1); 
	 wcom(0x01);   //清屏
	 delay(1);
	 wcom(0x0c);   //开显示指令 游标和游标位置关闭
}

void LCD_Init(void)
{	
	 LCD_CLK_H;//FIO0SET |= 0x00000010;    //p0.4 clk
	 LCD_CS_H;//FIO0SET |= 0x00000020;    //p0.5 cs
	 LCD_DAT_H;//FIO0SET |= 0x00000040;    //p0.6 sda

	 LCD_CS_L;//FIO0CLR |= 0x00000020;    //cs=0
	 LCD_CLK_L;//FIO0CLR |= 0x00000010;    //p0.4 clk
	 
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
}

void LCDStart(void)
{ 
	U8 led_no;
//	U32 led_key=0xffffff;
	U8 InitInfo[16]=
	{
		0xC6,0xF4,
		0xB6,0xAF,
		0xD6,0xD0,
		0xA1,0xA2,
		0xC7,0xEB,
		0xC9,0xD4,
		0xBA,0xF2,
		0xA1,0xAD, 
	};
	g_CurrentState=	State_MainMenu_Not_Menu;
	
	LCD_Init();
	if(g_LanguageFlag==CHINESE)
		chinesedisplay(0x81, InitInfo);
	else if(g_LanguageFlag==ENGLISH)
	 	chinesedisplay(0x81, "Start,Please wait");
	 
	 for(led_no=0;led_no<KEY_NUM;led_no++)
	 {
		SetKeyLEDLight((_KeypadNum)led_no);
		delay1000(500);
	 }
	 
	 SetKeyLEDLight(KEY_NUM); //清除LED
	
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

void printStr(U8 *str)
{
	while(*str!='\0')
	{
		dat(*str); //一个英文字符一次写完
		str++;
	}
}
//显示一个整数
void DisplayU8(U8 start,U8 ch)
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
	 
	 while(*pos!=NULL)
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
	ch[i++]=chh;
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

void DisInputSrcFormatStr(U8 addr)
{
	U16 HSize=0,VSize=0;	
	U8 image;
#if(XP_ID==XP520)
	if(g_MutiWindow==SAMEPIPIMAGE)//single channel
	{
		if(g_NextPointImage==SUB)
			image=MAIN;
		else
			image=SUB;
	}
	else
#endif
	{		
		if(g_MutiWindow==SINGLEIMAGE)
			image=MAIN;
		else
			image=g_ActiveImageValue;
	}
	GetInputSrcFormat(image,&HSize,&VSize);
	DisFourNumValue(HSize,addr);
	DisCurWinSizeValue(VSize,addr+2,'x');
}


void LCD_DisplayValue_6Bit(U32 wValue,U8 addr)
{
	U8 ch[7];	        //320520
	ch[0]=wValue%1000000/100000+48;
	ch[1]=wValue%100000/10000+48;
	ch[2]=wValue%10000/1000+48;
	ch[3]=wValue%1000/100+48;
	ch[4]=wValue%100/10+48;
	ch[5]=wValue%10+48;
	ch[6]=0;	
	chinesedisplay(addr, ch);
}

