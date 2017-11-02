/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: LCD.h
 [Date]:        09-May-2012
 [Comment]:
   LCD header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

#ifndef __TRUE
#define __TRUE        1
#endif

#ifndef __FALSE
#define __FALSE       0
#endif

//LCD Start Address
#define  LCDADDR80       0x80
#define LCDADDR90        0x90
#define LCDADDR91        0x91

// typedef char               S8;
typedef unsigned char      U8;
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned char      BOOL; 

void delay1000(U32 num);
void delay(U32 num);
void wcom(U8 d);
void dat(U8 d);
void clearlcd(void);/*«Â∆¡*/
void clearLineFromAddr(U8 addr);
void printStr(U8 *str);
void LCD_Init(void);
void chinesedisplay(U8 start,const U8 *pos);
void DisplayU8(U8 start,signed char ch);
U8 U8toCh(signed char ch);
void DisCurWinSizeValue(U16 wValue,U8 addr,U8 chh);
void DisInputSrcFormatStr(U8 addr);
void LCDStart(void);
#endif

