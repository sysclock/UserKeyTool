/*******************************************************************************
���� �� ����    Lcd.c
����    ����    lcd��������
����    ����    V1.0.0
���������ڡ�    2009-05-24
*******************************************************************************/

#include "Lcd.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

static void LcdOnOff(u8 OnOff)
{
  OnOff &= 0x01;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xae | OnOff);
  LCD_WR_HI;
  LCD_CS_DIS;
}

static void SetStartLine(u8 Line)
{
  Line &= 0x3f;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0x40 | Line);
  LCD_WR_HI;
  LCD_CS_DIS;
}

static void SetPageAddr(u8 Page)
{
  Page %= 8;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xb0 | Page);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetColumnAddr(u8 Addr)
{
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData((Addr>>4) | 0x10);
  LCD_WR_HI;
  LCD_WR_LOW;
  SetPortData(Addr & 0x0F);
  LCD_WR_HI;  
  LCD_CS_DIS; 
}

static void WriteData(u8 Data)
{
  LCD_CS_EN;  
  LCD_RS_HI;
  LCD_WR_LOW;
  SetPortData(Data);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetADC(u8 ADC)
{
  ADC &= 0x01;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xa0 | ADC);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetBias(u8 Bias)
{
  Bias &= 0x01;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xa2 | Bias);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetSHL(u8 SHL)
{
  SHL &= 0x01;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xC0 | (SHL<<3));
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetPowerCtrl(u8 Power)
{
  Power &= 0x07;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0x28 | Power);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetRegRatio(u8 Ratio)
{
  Ratio &= 0x07;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0x20 | Ratio);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetBoostRatio(u8 Step)
{
  Step &= 0x03;
  
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0xf8);
  LCD_WR_HI;
  LCD_WR_LOW;
  SetPortData(Step);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static void SetRefVoltag(u8 Voltage)
{
  LCD_CS_EN;  
  LCD_RS_LOW;
  LCD_WR_LOW;
  SetPortData(0x81);
  LCD_WR_HI;
  LCD_WR_LOW;
  SetPortData(Voltage);
  LCD_WR_HI;
  LCD_CS_DIS; 
}

static u8 WrapData(u8 Data)
{
  u8 Temp = 0;
  
  Temp |= ((Data&0x01)<<7);
  Temp |= ((Data&0x02)<<5);
  Temp |= ((Data&0x04)<<3);
  Temp |= ((Data&0x08)<<1);
  Temp |= ((Data&0x10)>>1);
  Temp |= ((Data&0x20)>>3);
  Temp |= ((Data&0x40)>>5);
  Temp |= ((Data&0x80)>>7);
  
  return Temp;
}

void LcdClear(void)
{
  u8 Page,Addr;
  for(Page=0;Page<8;Page++)
  {
	SetPageAddr(Page);
    for(Addr=0;Addr<128;Addr++)
    {
   	  SetColumnAddr(Addr);
      WriteData(0x00);
    }
  }
}

unsigned char ReadStatusLCD(void)
{
  unsigned int i;
  char ch;
  SetPortData(0xff);//CPLD_WRITE_DATA(LCD_DATA,0xFF);
  LCD_RS_LOW;//CPLD_WRITE_DATA(LCD_RS,0);
  LCD_WR_HI;//CPLD_WRITE_DATA(LCD_RW,1);
  BUS_IN;	
  LCD_CS_EN;//CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0
  LCD_CS_EN; //CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0
  //DelayUs(5);
  LCD_CS_DIS; //CPLD_WRITE_DATA(LCD_CE,1);//LCM_CE = 1
  
  for(i=0;i<0x8000;i++)
  {
    if(!(((GPIOB->IDR)>>8) & 0x80))
    {
      ch=0;
      break;
    }
    if(i==0x7000)
    {
      ch=1;
      break;
    }
  }
  DelayUs(1);
   BUS_OUT;	
  return ch;
}
void WriteCommandLCD(unsigned char WCLCM,int BuysC) //BuysCΪ0ʱ����æ���
{
  if (BuysC)
    ReadStatusLCD(); //������Ҫ���æ
  SetPortData(WCLCM);//CPLD_WRITE_DATA(LCD_DATA,WCLCM);//LCM_Data = WCLCM
  LCD_RS_LOW;//CPLD_WRITE_DATA(LCD_RS,0);//LCM_RS = 0
  LCD_WR_LOW;//CPLD_WRITE_DATA(LCD_RW,0);//LCM_RW = 0 
  LCD_CS_EN;//CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0
  LCD_CS_EN; //CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0
  //DelayUs(5);
  LCD_CS_DIS; //CPLD_WRITE_DATA(LCD_CE,1);//LCM_CE = 1
}

void WriteDataLCD(unsigned char WDLCM)
{
  ReadStatusLCD(); //���æ
  SetPortData(WDLCM);//CPLD_WRITE_DATA(LCD_DATA,WDLCM);//LCM_Data = WCLCM
  LCD_RS_HI;//CPLD_WRITE_DATA(LCD_RS,1);//LCM_RS = 1
  LCD_WR_LOW;//CPLD_WRITE_DATA(LCD_RW,0);//LCM_RW = 0
  LCD_CS_EN;//CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0 //�������ٶ�̫�߿���������С����ʱ
  LCD_CS_EN;//CPLD_WRITE_DATA(LCD_CE,0);//LCM_CE = 0 //��ʱ
  //DelayUs(5);
  LCD_CS_DIS; //CPLD_WRITE_DATA(LCD_CE,1);//LCM_CE = 1
}


void lcd_pos(int pos)
{                          
   WriteCommandLCD(pos|0x80,1);  //����ָ��=80+��ַ����
}


void lcd_clr(void)
{
    WriteCommandLCD(0x01,1); //��ʾ����
}

void LcdDisChar(unsigned int row,unsigned int col,unsigned char dat)
{
  col &= 0xF; //����col���ܴ���15
  row &= 0x1; //row���ܴ���1
  int pos;
  pos=0x40*row+0x1*col;
  lcd_pos(pos);
  WriteCommandLCD(col, 0); //���ﲻ���æ�źţ����͵�ַ��
  WriteDataLCD(dat);
}

void LcdDisStr(unsigned int row,unsigned int col,unsigned char *str,unsigned int size)
{
  unsigned int ListLength;
  ListLength = 0;
  col &= 0xF; //����col���ܴ���15
  row &= 0x1; //row���ܴ���1
  if(col+size>16)
  {
    size=16-col;
  }
  while (ListLength < size) //�������ִ�β���˳�
  {
    if (col <= 0xF) //col����ӦС��0xF
    {
      LcdDisChar(row,col,str[ListLength]); //��ʾ�����ַ�
      ListLength++;
      col++;
    }
  }
}



void LcdInit(void)
{
  SetPortData(0);//CPLD_WRITE_DATA(LCD_DATA,0);//LCM_Data = 0;
  WriteCommandLCD(0x38,0); //������ʾģʽ���ã������æ�ź�
  WriteCommandLCD(0x38,0);
  WriteCommandLCD(0x38,0);
  WriteCommandLCD(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
  WriteCommandLCD(0x08,1); //�ر���ʾ
  WriteCommandLCD(0x01,1); //��ʾ����
  WriteCommandLCD(0x06,1); // ��ʾ����ƶ�����
  WriteCommandLCD(0x0C,1); // ��ʾ�����������
}
/*
void LcdInit(void)
{
  LCD_RST_LOW;
  LCD_CS_DIS;
  LCD_RD_HI;
  LCD_WR_HI;
  LCD_BK_ON;
  LCD_RST_HI;
  //LcdReset();
  SetBias(0);     //1/9
  SetADC(0);      //normal
  SetSHL(1);      //flipped
  SetStartLine(0);
  SetPowerCtrl(0x04);
  DelayMs(50);
  SetPowerCtrl(0x6);
  DelayMs(50);
  SetPowerCtrl(0x7);
  DelayMs(50);
  SetBoostRatio(0);
  SetRegRatio(5);
  SetRefVoltag(0x28); 
  LcdOnOff(1);
  LcdClear();
}

*/

void DrawStr(u8 xAddr,u8 yAddr,char* Str)
{
	u8 i,j;    
	u8 Page;

	if(yAddr<8)Page=1;
	else if(yAddr<16)Page=2;
	else if(yAddr<24)Page=3;
	else if(yAddr<32)Page=4;
	else if(yAddr<40)Page=5;
	else if(yAddr<48)Page=6;
	else if(yAddr<56)Page=7;
	else if(yAddr<64)Page=8;
	
	SetStartLine(yAddr%8);
  	while(!*Str=='\0')
  	{
    	i=0;
    	while(*GB_16[i].Index != '^')
    	{
      		if(*GB_16[i].Index == *Str)
      		{
        		for(j=0;j<6;j++)
        		{
					SetPageAddr(8-(Page%8));					  
					SetColumnAddr(128-(xAddr%128));  
                    WriteData(WrapData(*(GB_16[i].Data+j)));

					SetPageAddr(8-((Page+1)%8));
					SetColumnAddr(128-(xAddr%128));
                    WriteData(WrapData(*(GB_16[i].Data+j+6)));

				  	xAddr++;
				}
            	break;
      		}
      		i++;
    	}
    	Str++;
  	}
}


void myprintf(char *str,...)
{
  char buffer[80];
  va_list para;
  //Get all parameters , if the number of parameter big than 10, the functing only take 10 ahead
  va_start(para, str);
  vsprintf(buffer, str, para);
  va_end(para);
  //DebugStr(buffer);
  DrawStr(0,24,buffer);
}


void DrawStr32(u8 xAddr,u8 yAddr,char* Str)
{
	u8 i,j;     
	u8 Page;

	if(yAddr<8)Page=1;
	else if(yAddr<16)Page=2;
	else if(yAddr<24)Page=3;
	else if(yAddr<32)Page=4;
	else if(yAddr<40)Page=5;
	else if(yAddr<48)Page=6;
	else if(yAddr<56)Page=7;
	else if(yAddr<64)Page=8;
	
	SetStartLine(yAddr%8);
  	while(!*Str=='\0')
  	{
    	i=0;
    	while(*GB_32[i].Index != '^')
    	{
      		if(*GB_32[i].Index == *Str)
      		{
        		for(j=0;j<12;j++)
        		{
					SetPageAddr(8-(Page%8));					  
					SetColumnAddr(128-(xAddr%128));  
                    WriteData(WrapData(*(GB_32[i].Data+j)));
					
					SetPageAddr(8-((Page+1)%8));
					SetColumnAddr(128-(xAddr%128));
                    WriteData(WrapData(*(GB_32[i].Data+j+12)));

					SetPageAddr(8-((Page+2)%8));
					SetColumnAddr(128-(xAddr%128));
                    WriteData(WrapData(*(GB_32[i].Data+j+24)));

					SetPageAddr(8-((Page+3)%8));
					SetColumnAddr(128-(xAddr%128));
                    WriteData(WrapData(*(GB_32[i].Data+j+36)));
					
				  	xAddr++;
				} 
            	break;
      		}
      		i++;
    	}
    	Str++;
  	}
}

void LoadBMP(u8 xAddr,u8 yAddr,const u8 *BMP,u8 Width,u8 Height)
{
	u8 i,j;    		
	u8 Page,Addr;

	if(yAddr<8)Page=1;
	else if(yAddr<16)Page=2;
	else if(yAddr<24)Page=3;
	else if(yAddr<32)Page=4;
	else if(yAddr<40)Page=5;
	else if(yAddr<48)Page=6;
	else if(yAddr<56)Page=7;
	else if(yAddr<64)Page=8;
	
	SetStartLine(yAddr%8);

	for(i=0;i<(Height>>3);i++)
	{
	    Addr = xAddr;
  		for(j=0;j<Width;j++)
  		{
			SetPageAddr(8-((Page+i)%8));
			SetColumnAddr(128-(Addr%128)); 
			WriteData(WrapData(*(BMP+j+i*Width)));
            
			Addr++;
    	}
 	}
}

void DrawLine(u8 xAddr)
{
  for(u8 i=0;i<8;i++)
  {    
    SetPageAddr(i);
    SetColumnAddr(128-xAddr);
    WriteData(0xFF);
  }
}
