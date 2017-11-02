# include "lcd_spi.h"
#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_dspi.h"
#include "num_bmp.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "rtc.h"


uint8_t LCD_Display_OnOff=1; //标识当前OLED是不是显示开模式，1为显示开，2为显示关

#define LCD_DC_H GPIO_SetPinsOutput(GPIOA, 1u << PIN10_IDX)  //LDC_Data/Comm控制
#define LCD_DC_L GPIO_ClearPinsOutput(GPIOA, 1u << PIN10_IDX)

#define LCD_RESET_H GPIO_SetPinsOutput(GPIOA, 1u << PIN11_IDX)  //LCD 复位控制
#define LCD_RESET_L GPIO_ClearPinsOutput(GPIOA, 1u << PIN11_IDX)


#define LCD_VDD_ON	GPIO_ClearPinsOutput(GPIOA, 1u << 12) //LCD_CONTROL
#define LCD_VDD_OFF	GPIO_SetPinsOutput(GPIOA, 1u << 12) //LCD_CONTROL

#define EXAMPLE_DSPI_LCD_BASEADDR SPI1    //LCD 使用的SPI1
#define DSPI_LCD_CLK_SRC DSPI1_CLK_SRC    //LCD 使用的时钟
#define EXAMPLE_DSPI_LCD_PCS_FOR_INIT kDSPI_Pcs0 //片选
#define EXAMPLE_DSPI_LCD_PCS_FOR_TRANSFER kDSPI_MasterPcs0   //数据

#define TRANSFER_BAUDRATE 500000U

#define TRANSFER_SIZE 256U   
uint8_t lcdTxData[TRANSFER_SIZE] = {0U};
dspi_transfer_t lcdXfer;



//软件初略延时，延时值以测量为准
//oLED屏的参考设计就用的该函数
/*
       Should wait at least 1ms. Because in these modes, the core clock is 100MHz
       at most, so this function could obtain the 1ms delay.
     */
void delay(int i)
{
	int j,k;
	for(j=0;j<i;j++)
		for(k=0;k<1100;k++);
}



//配置液晶屏
//官网的SPI配置例程
void Lcd_Spi_Config(void)
{
	uint32_t srcClock_Hz;
	dspi_master_config_t masterConfig;
	
	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame = 8U;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	
	masterConfig.whichPcs = EXAMPLE_DSPI_LCD_PCS_FOR_INIT;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;
	

	//实现LCD所用的SPI口的配置
	srcClock_Hz = CLOCK_GetFreq(DSPI_LCD_CLK_SRC);
  	DSPI_MasterInit(EXAMPLE_DSPI_LCD_BASEADDR, &masterConfig, srcClock_Hz);
	lcdXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_LCD_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
}




//LCD发送指令
void LCD_Transfer_Command(uint8_t data)
{
	LCD_DC_L;
	lcdXfer.txData =&data;
	lcdXfer.dataSize = 1;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_LCD_BASEADDR, &lcdXfer);
	delay(1);
}

//LCD发送数据；
//num表示发送数据的数量
//data表示发送的字符串buffer指针
void LCD_Transfer_Data(uint8_t data ,uint8_t num)
{
	LCD_DC_H;
	lcdXfer.txData =& data;
  	lcdXfer.dataSize = num;
	//spi主发送数据，使用mcu硬核
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_LCD_BASEADDR, &lcdXfer);
	delay(1);
}




//发送写入数据的地址
void lcd_address(uint8_t page,uint8_t column)
{
	column=column-1; 
	page=page-1;
	LCD_Transfer_Command(0xb0+page); 
	LCD_Transfer_Command(((column>>4)&0x0f)+0x10); 
	LCD_Transfer_Command(column&0x0f); 
}


//清屏
void clear_screen()
{
	uint8_t i,j;
	for(j=1;j<9;j++)
	{
		lcd_address(j,1);
		for(i=1;i<129;i++)
		{
			LCD_Transfer_Data(0x00,1);
		}
	}
}



//画波形数组
unsigned char Draw_Data[8][128]={0};
//画波形函数，以屏的左下角为0，0点。实际对应于屏是第8行，每0列
void Draw_Point(uint8_t x,uint8_t y)
{
	uint8_t draw_x_line,draw_x_point;

	draw_x_line=7-x/8;  //7减是因为屏的行数范围是0~7，除8是因为每行有8个点，显示点开始为左上角，变换为右下角
	draw_x_point=7-x%8; //计数出点所在字节的位置，0到7位处理。
	
	Draw_Data[draw_x_line][y] |=1<<draw_x_point;
	
	LCD_Transfer_Command(0xb0+draw_x_line);   //选择要发送的行
	LCD_Transfer_Command(((y>>4)&0x0f)+0x10); //选择要发送的列
	LCD_Transfer_Command(y&0x0f);             //选择要发送的列
	LCD_Transfer_Data(Draw_Data[draw_x_line][y],1);
}





//清指定区域
void clear_area(uint8_t begin_x,uint8_t begin_y,uint8_t end_x,uint8_t end_y)
{
	uint8_t i,j;
	for(j=begin_x;j<=end_x;j++)
	{
		lcd_address(j,begin_y);
		for(i=begin_y;i<end_y+1;i++)
		{
			LCD_Transfer_Data(0x00,1);
		}
	}
}


//液晶屏幕初始化
void LCD_Init(void)
{
		gpio_pin_config_t LCD_config = {
        kGPIO_DigitalOutput, 0,
    };
		GPIO_PinInit(GPIOA, PIN10_IDX, &LCD_config);  //设置PTA10为输出===>>LCD_DC 
		GPIO_PinInit(GPIOA, PIN11_IDX, &LCD_config);  //设置PTA11为输出===>>LCD_RESET
		GPIO_PinInit(GPIOA, PIN12_IDX, &LCD_config);
		GPIO_PinInit(GPIOA, PIN13_IDX, &LCD_config);
		GPIO_PinInit(GPIOE, PIN6_IDX, &LCD_config);
		
		Lcd_Spi_Config();
		LCD_VDD_ON;
		LCD_RESET_L;    //低电平复位
		delay(500);
		LCD_RESET_H;
		delay(200);
		//以下源自网络，验证可行
		//液晶的手册和液晶控制器的手册中均没有描述
		LCD_Transfer_Command(0xae); //关显示
		LCD_Transfer_Command(0xd5); //晶振频率
		LCD_Transfer_Command(0x80);
		LCD_Transfer_Command(0xa8); //duty 设置
		LCD_Transfer_Command(0x3f); //duty=1/64
		LCD_Transfer_Command(0xd3); //显示偏移
		LCD_Transfer_Command(0x00);
		LCD_Transfer_Command(0x40); //起始行
		LCD_Transfer_Command(0x8d); //升压允许
		LCD_Transfer_Command(0x14);
		LCD_Transfer_Command(0x20); //page address mode
		LCD_Transfer_Command(0x02);
		LCD_Transfer_Command(0xc8); //行扫描顺序：从上到下
		LCD_Transfer_Command(0xa1); //列扫描顺序：从左到右
		LCD_Transfer_Command(0xda); //sequential configuration
		LCD_Transfer_Command(0x12);
		LCD_Transfer_Command(0x81); //微调对比度，本指令的0x81为固定值
		LCD_Transfer_Command(0xcf); //微调对比度的值,可设置范围0x00~0xff
		LCD_Transfer_Command(0xd9); //Set Pre-Charge Period
		LCD_Transfer_Command(0xf1);
		LCD_Transfer_Command(0xdb); //Set VCOMH Deselect Level
		LCD_Transfer_Command(0x40);
		clear_screen();
		LCD_VCC_ON;
		LCD_Transfer_Command(0xaf); //开显示
}




//满屏幕显示
void Display_full_screen(uint8_t * data)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for(i=0;i<128;i++)
		{
			LCD_Transfer_Data(data[j*128+i],1);
		}
	}
}


//字体显示，参考\display_data\num-bmp.h文件

//在LCD的x行Y列，按5号字体显示数字
void Display_num_size5(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<6;i++)
		{
				//num_size5在num_bmp.c中定义
				LCD_Transfer_Data(num_size5[num][(j-x)*6+i],1);
		}
	}
}



//字体显示，参考\display_data\num-bmp.h文件
//在LCD的x行Y列，按每个字符12行6列显示数字
void Display_num_size12_6(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<6;i++)
		{
				LCD_Transfer_Data(num12_6[num][(j-x)*6+i],1);
		}
	}
}



//字体显示，参考num-bmp.h文件
//在LCD的x行Y列，按10号字体显示数字字模中的数字
void Display_num_size10(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+3;j++)
	{
		lcd_address(j,y);
		for(i=0;i<10;i++)
		{
			LCD_Transfer_Data(num_size10[num][(j-x)*10+i],1);
		}
	}
}


//字体显示，参考\display_data\num-bmp.h文件
//在LCD的x行Y列，按每个字符16行9列显示数字
void Display_num_size16_9(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<9;i++)
		{
			LCD_Transfer_Data(num_size16_9[num][(j-x)*9+i],1);
		}
	}
}


//字体显示，参考num-bmp.h文件
//在LCD的x行Y列，按25号字体显示数字
void Display_num_size25(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+6;j++)
	{
		lcd_address(j,y);
		for(i=0;i<25;i++)
		{
			LCD_Transfer_Data(num_size25[num][(j-x)*25+i],1);
		}
	}
}






//显示时间之间的分割符号" : "
//点阵打印
void Display_time_sapce(uint8_t x,uint8_t y)
{
	uint8_t i,j;

	lcd_address(4,y);//写道第四行
	for(i=0;i<4;i++)//打印四列
	{
		LCD_Transfer_Data(0x3C,1);//0x3C,对应一列从上到下0011 1100
	}							//避免提取字模		

	lcd_address(7,y);//写到第7行
	for(i=0;i<4;i++)//打印四列
	{
		LCD_Transfer_Data(0x0f,1);//0x0f,对应一列从上到下0000 1111
	}							//以上0x3c 和0x0f，构成了时和分之间的类似":"两个点
			
}


//显示日期的分割符号  " - "
//点阵打印
void Display_date_sapce(uint8_t x,uint8_t y)
{
	uint8_t i;

	lcd_address(1,y);
	for(i=0;i<4;i++)
	{
		LCD_Transfer_Data(0x80,1);//use spi comm  1000 0000
	}
}


//清除空格等分隔符
//点阵打印空格
void Clear_sapce(uint8_t x,uint8_t y)
{
	uint8_t i;

	lcd_address(1,y);
	for(i=0;i<x;i++)
	{
		LCD_Transfer_Data(0x00,1);
	}
	lcd_address(2,y);
	for(i=0;i<x;i++)
	{
		LCD_Transfer_Data(0x00,1);
	}
}

//显示周几
//周，用的字模
void Display_week_data(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<12;i++)
		{
			LCD_Transfer_Data(week_data[num][(j-x)*12+i],1);
		}
	}
}


//参数: x 第几行 小于3  
//参数: y 第几列
//显示一个16*16点阵的字
//显示的字是16*16的，那就是从开始的x处，显示两行（每行8个点）
void Display_16_16_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<16;i++)
		{
			LCD_Transfer_Data(data[(j-x)*16+i],1);
		}
	}
}

//显示一个16*12点阵的字
//显示的字是16*12的，那就是从开始的x处，显示两行（每行8个点）
void Display_16_12_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<12;i++)
		{
			LCD_Transfer_Data(data[(j-x)*12+i],1);//transfer 8 bits
		}
	}
}



//显示一个16*8点阵的字；宽度是8，高度是16
//显示的字是16*8的，高度是16，那就是从开始的x处，显示两行（每行8个点）
void Display_16_8_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<8;i++)
		{
			LCD_Transfer_Data(data[(j-x)*8+i],1);
		}
	}
}



//显示一个24*20点阵的字
//显示的字是24*20的，那就是从开始的x处，显示三行（每行8个点）

void Display_24_20_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i,j;
	for(j=x;j<x+3;j++)
	{
		lcd_address(j,y);
		for(i=0;i<20;i++)
		{
			LCD_Transfer_Data(data[(j-x)*20+i],1);
		}
	}
}


//显示一个8*9点阵的字
//显示的字是8*9的，高度是8，那就是从开始的x处，显示一行（每行8个点）
void Display_8_9_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i;
		lcd_address(x,y);
		for(i=0;i<9;i++)
		{
			LCD_Transfer_Data(data[i],1);
		}
}

//显示一个16*5点阵的字
//显示的字是16*5的，高度是16，那就是从开始的x处，显示2行
void Display_16_5_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<5;i++)
		{
			LCD_Transfer_Data(data[(j-x)*5+i],1);
		}
	}
}



//LCD上显示点，测量数据描点
void Display_point(uint8_t num)
{
	if(num==2)
		Display_16_16_data(3,96,&Page_measuredoing_Data[160]);	
	else if(num==0)
		Display_16_16_data(3,96,&Page_measuredoing_Data[192]);
	else
		Display_16_16_data(3,96,&Page_measuredoing_Data[224]);	
}



//心电脉搏数据正常过高低幅度差值
void Display_Waveforms_Info(uint8_t Info_Num)
{
	switch(Info_Num)
	{
		case 0:  //采集的脉搏、心电数据符合推导血压的限值范围
			//显示12点阵  脉搏心电数据正常
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[72]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[144]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[168]);	
			break;
		case 10:  //心电数据干扰过多
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[20*24]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[21*24]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[24*24]);	
			break;
		case 11:	 //心电有效数据过少
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[22*24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[23*24]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[25*24]);	
			break;
		case 1:  //心电数据幅值过高
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[216]);	
			break;
		case 2:	 //心电数据幅值过低
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[240]);	
			break;
		case 3:		//心电差值过高
			Display_16_12_data(4,28,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[216]);
			break;
		case 4:  //心电差值过低
			Display_16_12_data(4,28,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[240]);
			break;
		case 5: //脉搏数据幅值过高
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[216]);	
			break;
		case 6://脉搏数据幅值过低
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[240]);	
			break;
		case 7:  //脉搏差值过高
			Display_16_12_data(4,28,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[216]);
			break;
		case 8:	 //脉搏差值过低
			Display_16_12_data(4,28,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[240]);
			break;
		case 12: //脉搏数据干扰过多
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[20*24]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[21*24]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[24*24]);	
			break;
		case 13://脉搏有效数据过少
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[22*24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[23*24]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[25*24]);	
			break;
		case 9: //数据计算异常
			Display_16_12_data(4,28,&Waveforms_info_Data12[14*24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[15*24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[16*24]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[17*24]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[18*24]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[19*24]);
			break;
		default:
			break;
	}
}


////心电脉搏数据正常过高低幅度差值
//void Display_Waveforms_Info(uint8_t Info_Num)
//{
//	switch(Info_Num)
//	{
//		case 0:  //采集的脉搏、心电数据符合推导血压的限值范围
//			//显示12点阵  脉搏心电数据正常
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[72]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[144]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[168]);	
//			break;
//		case 10:  //心电数据干扰过多
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[20*24]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[21*24]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[24*24]);	
//			break;
//		case 11:	 //心电有效数据过少
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[22*24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[23*24]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[25*24]);	
//			break;
//		case 1:  //心电数据幅值过高
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[264]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[288]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[216]);	
//			break;
//		case 2:	 //心电数据幅值过低
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[264]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[288]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[240]);	
//			break;
//		case 3:		//心电差值过高
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[216]);
//			break;
//		case 4:  //心电差值过低
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[240]);
//			break;
//		case 5: //脉搏数据幅值过高
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[264]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[288]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[216]);	
//			break;
//		case 6://脉搏数据幅值过低
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[264]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[288]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[240]);	
//			break;
//		case 7:  //脉搏差值过高
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[216]);
//			break;
//		case 8:	 //脉搏差值过低
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[240]);
//			break;
//		case 12: //脉搏数据干扰过多
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[20*24]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[21*24]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[24*24]);	
//			break;
//		case 13://脉搏有效数据过少
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,16,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[22*24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[23*24]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[96]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[120]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[192]);
//			Display_16_12_data(6,100,&Waveforms_info_Data12[25*24]);	
//			break;
//		case 9: //数据计算异常
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[14*24]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[15*24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[16*24]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[17*24]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[18*24]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[19*24]);
//			break;
//		default:
//			break;
//	}
//}

//显示电池电量

void Display_bat_bmp(uint8_t num)
{
	if(num==1) //电满
	{
		Display_16_8_data(1,120,&bat_bmp[0]);
	}
	else if(num==2)
	{
		Display_16_8_data(1,120,&bat_bmp[16]);
	}
	else if(num==3)
	{
		Display_16_8_data(1,120,&bat_bmp[32]);
	}
	else if(num==4)
	{
		Display_16_8_data(1,120,&bat_bmp[48]);
	}
	else if(num==5)
	{
		Display_16_8_data(1,120,&bat_bmp[64]);
	}
}



//自动测量模式开启
void Display_auto_measure_IsTurnOn()
{
	uint8_t num=0;
	clear_screen();
	Display_16_12_data(1,18,&page_aoto_mode_on_data[0]);
	Display_16_12_data(1,18+1*13,&page_aoto_mode_on_data[1*24]);
	Display_16_12_data(1,18+2*13,&page_aoto_mode_on_data[2*24]);
	Display_16_12_data(1,18+3*13,&page_aoto_mode_on_data[3*24]);
	Display_16_12_data(1,18+4*13,&page_aoto_mode_on_data[4*24]);
	Display_16_12_data(1,18+5*13,&page_aoto_mode_on_data[5*24]);
	Display_16_12_data(1,18+6*13,&page_aoto_mode_on_data[6*24]);

	Display_16_12_data(3,1+0*13,&page_aoto_mode_on_data[7*24]);
	Display_16_12_data(3,1+1*13,&page_aoto_mode_on_data[8*24]);
	
	clear_area(3,28,4,48);
	
	if(Auto_Measure_Success_Count>99)
		Display_16_8_data(3,1+2*13+1,&hex_8_16[(Auto_Measure_Success_Count/100%10)*16]);
	if(Auto_Measure_Success_Count>9)
		Display_16_8_data(3,1+2*13+8,&hex_8_16[(Auto_Measure_Success_Count/10%10)*16]);

	Display_16_8_data(3,1+2*13+15,&hex_8_16[(Auto_Measure_Success_Count%10)*16]);
		
	Display_16_12_data(3,62-1*13,&page_aoto_mode_on_data[11*24]);
	
	Display_16_12_data(3,66+0*13,&page_aoto_mode_on_data[9*24]);
	Display_16_12_data(3,66+1*13,&page_aoto_mode_on_data[10*24]);
	
	clear_area(3,93,4,114);	
	if(Auto_Measure_Fail_Count>99)
		Display_16_8_data(3,66+2*13+1,&hex_8_16[(Auto_Measure_Fail_Count/100%10)*16]);
	if(Auto_Measure_Fail_Count>9)
		Display_16_8_data(3,66+2*13+8,&hex_8_16[(Auto_Measure_Fail_Count/10%10)*16]);
	Display_16_8_data(3,66+2*13+15,&hex_8_16[(Auto_Measure_Fail_Count%10)*16]);
	
	Display_16_12_data(3,128-1*13,&page_aoto_mode_on_data[11*24]);
	
	Display_16_12_data(5,30+0*13,&page_aoto_mode_on_data[12*24]);
	Display_16_12_data(5,30+1*13,&page_aoto_mode_on_data[13*24]);
	
	num=AutoMeasurePara.Timelong - (Auto_Measure_Total_Num_Count-1)*AutoMeasurePara.Timeinterval/60 ;
	
	clear_area(5,57,6,71);
	if(num>9)
		Display_16_8_data(5,30+2*13+1,&hex_8_16[(num/10%10)*16]);
	Display_16_8_data(5,30+2*13+8,&hex_8_16[(num%10)*16]);
	
	Display_16_12_data(5,47+2*13,&page_aoto_mode_on_data[14*24]);
	Display_16_12_data(5,47+3*13,&page_aoto_mode_on_data[15*24]);
	
	Display_16_12_data(7,26+0*13,&page_aoto_mode_on_data[16*24]);
	Display_16_12_data(7,26+1*13,&page_aoto_mode_on_data[17*24]);
	Display_16_12_data(7,26+2*13,&page_aoto_mode_on_data[18*24]);
	Display_16_12_data(7,26+3*13,&page_aoto_mode_on_data[19*24]);
	Display_16_12_data(7,26+4*13,&page_aoto_mode_on_data[20*24]);
	Display_16_12_data(7,26+5*13,&page_aoto_mode_on_data[21*24]);
}




//显示程序版本信息
void Display_Version_Info(void)
{
	clear_screen();
	Display_16_12_data(2,20+0*13,&Page_Device_Information_Data[0*24]);
	Display_16_12_data(2,20+1*13,&Page_Device_Information_Data[1*24]);
	Display_16_12_data(2,20+2*13,&Page_Device_Information_Data[7*24]);
	Display_16_12_data(2,20+3*13,&Page_Device_Information_Data[8*24]);
	Display_16_8_data(2,20+4*13+0*8,&hex_8_16[19*16]);
	Display_16_8_data(2,20+4*13+1*8,&hex_8_16[18*16]);
		
//	Display_16_8_data(2,22+4*13+2*8,&hex_8_16[1*16]);
	
	Display_16_8_data(2,22+4*13+2*8,&hex_8_16[(((Software_version>>8)&0xff)%10)*16]);
	
	Display_16_8_data(2,22+4*13+3*8,&hex_8_16[20*16]);
	Display_16_8_data(2,22+4*13+4*8,&hex_8_16[((Software_version%256)%10)*16]);
	
	Display_16_12_data(4,20+0*13,&Page_Device_Information_Data[2*24]);
	Display_16_12_data(4,20+1*13,&Page_Device_Information_Data[3*24]);
	Display_16_12_data(4,20+2*13,&Page_Device_Information_Data[7*24]);
	Display_16_12_data(4,20+3*13,&Page_Device_Information_Data[8*24]);
	Display_16_8_data(4,20+4*13+0*8,&hex_8_16[19*16]);
	Display_16_8_data(4,20+4*13+1*8,&hex_8_16[18*16]);
//	Display_16_8_data(4,22+4*13+2*8,&hex_8_16[1*16]);	

	Display_16_8_data(4,22+4*13+2*8,&hex_8_16[(((Algorithm_version>>8)&0xff)%10)*16]);
	Display_16_8_data(4,22+4*13+3*8,&hex_8_16[20*16]);
	Display_16_8_data(4,22+4*13+4*8,&hex_8_16[(((Algorithm_version%256)%16)%10)*16]);
	
	Display_16_12_data(6,14+0*13,&Page_Device_Information_Data[4*24]);
	Display_16_12_data(6,14+1*13,&Page_Device_Information_Data[5*24]);
	Display_16_12_data(6,14+2*13,&Page_Device_Information_Data[6*24]);
	Display_16_12_data(6,14+3*13,&Page_Device_Information_Data[7*24]);
	Display_16_12_data(6,14+4*13,&Page_Device_Information_Data[8*24]);
	Display_16_8_data(6,14+5*13+0*8,&hex_8_16[19*16]);
	Display_16_8_data(6,14+5*13+1*8,&hex_8_16[18*16]);
	
	Display_16_8_data(6,16+5*13+2*8,&hex_8_16[1*16]);
	Display_16_8_data(6,16+5*13+3*8,&hex_8_16[20*16]);
	Display_16_8_data(6,16+5*13+4*8,&hex_8_16[0*16]);
}





//显示处理器的ID信息
void Display_Device_ID(void)
{
	clear_screen();
	Display_16_12_data(1,5+0*13,&Page_Device_Information_Data[9*24]);
	Display_16_12_data(1,5+1*13,&Page_Device_Information_Data[10*24]);
	Display_16_8_data(1,5+2*13+0*8,&hex_8_16[16*16]);
	Display_16_8_data(1,5+2*13+1*8,&hex_8_16[17*16]);
	Display_16_8_data(1,5+2*13+2*8,&hex_8_16[19*16]);

	Display_16_8_data(1,5+2*13+3*8,&hex_8_16[((RIDH>>28)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+4*8,&hex_8_16[((RIDH>>24)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+5*8,&hex_8_16[((RIDH>>20)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+6*8,&hex_8_16[((RIDH>>16)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+7*8,&hex_8_16[((RIDH>>12)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+8*8,&hex_8_16[((RIDH>>8)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+9*8,&hex_8_16[((RIDH>>4)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+10*8,&hex_8_16[(RIDH&0x0f)*16]);
	
	Display_16_8_data(3,5+2*13+3*8,&hex_8_16[((RIDMH>>28)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+4*8,&hex_8_16[((RIDMH>>24)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+5*8,&hex_8_16[((RIDMH>>20)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+6*8,&hex_8_16[((RIDMH>>16)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+7*8,&hex_8_16[((RIDMH>>12)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+8*8,&hex_8_16[((RIDMH>>8)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+9*8,&hex_8_16[((RIDMH>>4)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+10*8,&hex_8_16[(RIDMH&0x0f)*16]);
	
	Display_16_8_data(5,5+2*13+3*8,&hex_8_16[((RIDML>>28)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+4*8,&hex_8_16[((RIDML>>24)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+5*8,&hex_8_16[((RIDML>>20)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+6*8,&hex_8_16[((RIDML>>16)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+7*8,&hex_8_16[((RIDML>>12)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+8*8,&hex_8_16[((RIDML>>8)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+9*8,&hex_8_16[((RIDML>>4)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+10*8,&hex_8_16[(RIDML&0x0f)*16]);
	
	Display_16_8_data(7,5+2*13+3*8,&hex_8_16[((RIDL>>28)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+4*8,&hex_8_16[((RIDL>>24)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+5*8,&hex_8_16[((RIDL>>20)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+6*8,&hex_8_16[((RIDL>>16)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+7*8,&hex_8_16[((RIDL>>12)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+8*8,&hex_8_16[((RIDL>>8)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+9*8,&hex_8_16[((RIDL>>4)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+10*8,&hex_8_16[(RIDL&0x0f)*16]);
	
}



//显示脉感模块的ID信息
void Display_PW1101IC_ID(void)
{
	clear_screen();
	Display_16_12_data(1,5+0*13,&Page_Device_Information_Data[0*24]);
	Display_16_12_data(1,5+1*13,&Page_Device_Information_Data[1*24]);
	Display_16_8_data(1,5+2*13+0*8,&hex_8_16[16*16]);
	Display_16_8_data(1,5+2*13+1*8,&hex_8_16[17*16]);
	Display_16_8_data(1,5+2*13+2*8,&hex_8_16[19*16]);

	Display_16_8_data(1,5+2*13+3*8,&hex_8_16[((PW1101_ID_H>>28)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+4*8,&hex_8_16[((PW1101_ID_H>>24)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+5*8,&hex_8_16[((PW1101_ID_H>>20)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+6*8,&hex_8_16[((PW1101_ID_H>>16)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+7*8,&hex_8_16[((PW1101_ID_H>>12)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+8*8,&hex_8_16[((PW1101_ID_H>>8)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+9*8,&hex_8_16[((PW1101_ID_H>>4)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+10*8,&hex_8_16[(PW1101_ID_H&0x0f)*16]);
	
	Display_16_8_data(3,5+2*13+3*8,&hex_8_16[((PW1101_ID_MH>>28)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+4*8,&hex_8_16[((PW1101_ID_MH>>24)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+5*8,&hex_8_16[((PW1101_ID_MH>>20)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+6*8,&hex_8_16[((PW1101_ID_MH>>16)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+7*8,&hex_8_16[((PW1101_ID_MH>>12)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+8*8,&hex_8_16[((PW1101_ID_MH>>8)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+9*8,&hex_8_16[((PW1101_ID_MH>>4)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+10*8,&hex_8_16[(PW1101_ID_MH&0x0f)*16]);
	
	Display_16_8_data(5,5+2*13+3*8,&hex_8_16[((PW1101_ID_ML>>28)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+4*8,&hex_8_16[((PW1101_ID_ML>>24)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+5*8,&hex_8_16[((PW1101_ID_ML>>20)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+6*8,&hex_8_16[((PW1101_ID_ML>>16)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+7*8,&hex_8_16[((PW1101_ID_ML>>12)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+8*8,&hex_8_16[((PW1101_ID_ML>>8)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+9*8,&hex_8_16[((PW1101_ID_ML>>4)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+10*8,&hex_8_16[(PW1101_ID_ML&0x0f)*16]);
	
	Display_16_8_data(7,5+2*13+3*8,&hex_8_16[((PW1101_ID_L>>28)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+4*8,&hex_8_16[((PW1101_ID_L>>24)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+5*8,&hex_8_16[((PW1101_ID_L>>20)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+6*8,&hex_8_16[((PW1101_ID_L>>16)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+7*8,&hex_8_16[((PW1101_ID_L>>12)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+8*8,&hex_8_16[((PW1101_ID_L>>8)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+9*8,&hex_8_16[((PW1101_ID_L>>4)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+10*8,&hex_8_16[(PW1101_ID_L&0x0f)*16]);
}



//显示传感器的ID信息
void Display_Sensor_ID(void)
{
	clear_screen();
	Display_16_12_data(1,5+0*13,&Page_Device_Information_Data[2*24]);
	Display_16_12_data(1,5+1*13,&Page_Device_Information_Data[3*24]);
	Display_16_8_data(1,5+2*13+0*8,&hex_8_16[16*16]);
	Display_16_8_data(1,5+2*13+1*8,&hex_8_16[17*16]);
	Display_16_8_data(1,5+2*13+2*8,&hex_8_16[19*16]);

	Display_16_8_data(1,5+2*13+3*8,&hex_8_16[((PW1101_ID_H>>28)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+4*8,&hex_8_16[((PW1101_ID_H>>24)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+5*8,&hex_8_16[((PW1101_ID_H>>20)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+6*8,&hex_8_16[((PW1101_ID_H>>16)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+7*8,&hex_8_16[((PW1101_ID_H>>12)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+8*8,&hex_8_16[((PW1101_ID_H>>8)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+9*8,&hex_8_16[((PW1101_ID_H>>4)&0x0f)*16]);
	Display_16_8_data(1,5+2*13+10*8,&hex_8_16[(PW1101_ID_H&0x0f)*16]);
	
	Display_16_8_data(3,5+2*13+3*8,&hex_8_16[((PW1101_ID_MH>>28)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+4*8,&hex_8_16[((PW1101_ID_MH>>24)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+5*8,&hex_8_16[((PW1101_ID_MH>>20)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+6*8,&hex_8_16[((PW1101_ID_MH>>16)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+7*8,&hex_8_16[((PW1101_ID_MH>>12)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+8*8,&hex_8_16[((PW1101_ID_MH>>8)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+9*8,&hex_8_16[((PW1101_ID_MH>>4)&0x0f)*16]);
	Display_16_8_data(3,5+2*13+10*8,&hex_8_16[(PW1101_ID_MH&0x0f)*16]);
	
	Display_16_8_data(5,5+2*13+3*8,&hex_8_16[((PW1101_ID_ML>>28)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+4*8,&hex_8_16[((PW1101_ID_ML>>24)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+5*8,&hex_8_16[((PW1101_ID_ML>>20)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+6*8,&hex_8_16[((PW1101_ID_ML>>16)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+7*8,&hex_8_16[((PW1101_ID_ML>>12)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+8*8,&hex_8_16[((PW1101_ID_ML>>8)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+9*8,&hex_8_16[((PW1101_ID_ML>>4)&0x0f)*16]);
	Display_16_8_data(5,5+2*13+10*8,&hex_8_16[(PW1101_ID_ML&0x0f)*16]);
	
	Display_16_8_data(7,5+2*13+3*8,&hex_8_16[((PW1101_ID_L>>28)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+4*8,&hex_8_16[((PW1101_ID_L>>24)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+5*8,&hex_8_16[((PW1101_ID_L>>20)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+6*8,&hex_8_16[((PW1101_ID_L>>16)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+7*8,&hex_8_16[((PW1101_ID_L>>12)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+8*8,&hex_8_16[((PW1101_ID_L>>8)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+9*8,&hex_8_16[((PW1101_ID_L>>4)&0x0f)*16]);
	Display_16_8_data(7,5+2*13+10*8,&hex_8_16[(PW1101_ID_L&0x0f)*16]);
}





//报警功能
void alarm_function(void)
{
	if((measure_result_data[0]*256+measure_result_data[1])>alarm.HBlood_HighValue) //当前测量的值大于高压上限设定的值
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[0*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.HBlood_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HBlood_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HBlood_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HBlood_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if((measure_result_data[0]*256+measure_result_data[1])>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[(measure_result_data[0]*256+measure_result_data[1])/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if((measure_result_data[0]*256+measure_result_data[1])<alarm.HBlood_LowValue) //当前测量的值小于高压下限设定的值
	{
		clear_screen();
		
		Display_16_12_data(3,20+0*13,&Alarm_Data[0*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.HBlood_LowValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HBlood_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HBlood_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HBlood_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if((measure_result_data[0]*256+measure_result_data[1])>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[(measure_result_data[0]*256+measure_result_data[1])/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])%10)*16]);
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[2]>alarm.LBlood_HighValue) //当前测量的值大于低压上限设定的值
	{
		clear_screen();
		
		Display_16_12_data(3,20+0*13,&Alarm_Data[1*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.LBlood_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.LBlood_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.LBlood_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.LBlood_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(measure_result_data[2]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[2]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[2]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[2]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[2]<alarm.LBlood_LowValue) //当前测量的值小于低压下限设定的值
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[1*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.LBlood_LowValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.LBlood_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.LBlood_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.LBlood_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(measure_result_data[2]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[2]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[2]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[2]%10)*16]);
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[3]>alarm.HeartRate_HighValue) //当前测量的值大于心率上限设定的值
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[3*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[4*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.HeartRate_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HeartRate_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HeartRate_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HeartRate_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(measure_result_data[3]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[3]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[3]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[3]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[3]<alarm.HeartRate_LowValue) //当前测量的值小于心率下限设定的值
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[3*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[4*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(alarm.HeartRate_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HeartRate_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HeartRate_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HeartRate_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //显示 ：号
		if(measure_result_data[3]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[3]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[3]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[3]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //开显示
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
}



//自动测量正在进行的状态显示
void auto_measure_going(void)
{
	clear_screen();
	Display_16_16_data(4,25,&auto_measure_going_data[0*32]);
	Display_16_16_data(4,43,&auto_measure_going_data[1*32]);
	Display_16_16_data(4,61,&auto_measure_going_data[2*32]);
	Display_16_16_data(4,79,&auto_measure_going_data[3*32]);
	Display_16_16_data(4,97,&auto_measure_going_data[4*32]);
}
