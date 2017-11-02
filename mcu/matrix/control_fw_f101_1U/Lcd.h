#ifndef LCD_H
#define LCD_H

#include "Init.h"

typedef struct                  // 汉字字模数据结构
{
	u8 Index[2];               // 汉字内码索引
	u8 Data[12];                       // 点阵码数据
}typFNT_GB16;
extern  const typFNT_GB16 GB_16[];

typedef struct                  // 汉字字模数据结构
{
	u8 Index[2];               // 汉字内码索引
	u8 Data[48];                       // 点阵码数据
}typFNT_GB32;
extern  const typFNT_GB32 GB_32[];

typedef struct 
{
	const u8 BMP[8];
}TYPE_BMP;
extern const TYPE_BMP BMP_NUM[];

extern const u8 LOGO_BMP[];

extern const u8 NULL_BMP[];
extern const u8 VGA_BMP[];
extern const u8 RGB_BMP[];
extern const u8 AUD_BMP[];
extern const u8 VID_BMP[];

extern const u8 ARROW0_BMP[];
extern const u8 ARROW1_BMP[];

extern const u8 SAVE_BMP[];
extern const u8 LOAD_BMP[];
extern const u8 SET_BMP[];

void LcdInit(void);
void LcdClear(void);
void DrawStr(u8 xAddr,u8 yAddr,char* Str);
void DrawStr32(u8 xAddr,u8 yAddr,char* Str);
void LcdDisStr(unsigned int row,unsigned int col,unsigned char *str,unsigned int size);
void lcd_clr(void);

void LoadBMP(u8 xAddr,u8 yAddr,const u8 *BMP,u8 Width,u8 Height);
void DrawLine(u8 xAddr);
void myprintf(char *str,...);

#endif

