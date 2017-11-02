# include "lcd_spi.h"
#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_dspi.h"
#include "num_bmp.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "rtc.h"


uint8_t LCD_Display_OnOff=1; //��ʶ��ǰOLED�ǲ�����ʾ��ģʽ��1Ϊ��ʾ����2Ϊ��ʾ��

#define LCD_DC_H GPIO_SetPinsOutput(GPIOA, 1u << PIN10_IDX)  //LDC_Data/Comm����
#define LCD_DC_L GPIO_ClearPinsOutput(GPIOA, 1u << PIN10_IDX)

#define LCD_RESET_H GPIO_SetPinsOutput(GPIOA, 1u << PIN11_IDX)  //LCD ��λ����
#define LCD_RESET_L GPIO_ClearPinsOutput(GPIOA, 1u << PIN11_IDX)


#define LCD_VDD_ON	GPIO_ClearPinsOutput(GPIOA, 1u << 12) //LCD_CONTROL
#define LCD_VDD_OFF	GPIO_SetPinsOutput(GPIOA, 1u << 12) //LCD_CONTROL

#define EXAMPLE_DSPI_LCD_BASEADDR SPI1    //LCD ʹ�õ�SPI1
#define DSPI_LCD_CLK_SRC DSPI1_CLK_SRC    //LCD ʹ�õ�ʱ��
#define EXAMPLE_DSPI_LCD_PCS_FOR_INIT kDSPI_Pcs0 //Ƭѡ
#define EXAMPLE_DSPI_LCD_PCS_FOR_TRANSFER kDSPI_MasterPcs0   //����

#define TRANSFER_BAUDRATE 500000U

#define TRANSFER_SIZE 256U   
uint8_t lcdTxData[TRANSFER_SIZE] = {0U};
dspi_transfer_t lcdXfer;



//���������ʱ����ʱֵ�Բ���Ϊ׼
//oLED���Ĳο���ƾ��õĸú���
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



//����Һ����
//������SPI��������
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
	

	//ʵ��LCD���õ�SPI�ڵ�����
	srcClock_Hz = CLOCK_GetFreq(DSPI_LCD_CLK_SRC);
  	DSPI_MasterInit(EXAMPLE_DSPI_LCD_BASEADDR, &masterConfig, srcClock_Hz);
	lcdXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_LCD_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
}




//LCD����ָ��
void LCD_Transfer_Command(uint8_t data)
{
	LCD_DC_L;
	lcdXfer.txData =&data;
	lcdXfer.dataSize = 1;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_LCD_BASEADDR, &lcdXfer);
	delay(1);
}

//LCD�������ݣ�
//num��ʾ�������ݵ�����
//data��ʾ���͵��ַ���bufferָ��
void LCD_Transfer_Data(uint8_t data ,uint8_t num)
{
	LCD_DC_H;
	lcdXfer.txData =& data;
  	lcdXfer.dataSize = num;
	//spi���������ݣ�ʹ��mcuӲ��
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_LCD_BASEADDR, &lcdXfer);
	delay(1);
}




//����д�����ݵĵ�ַ
void lcd_address(uint8_t page,uint8_t column)
{
	column=column-1; 
	page=page-1;
	LCD_Transfer_Command(0xb0+page); 
	LCD_Transfer_Command(((column>>4)&0x0f)+0x10); 
	LCD_Transfer_Command(column&0x0f); 
}


//����
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



//����������
unsigned char Draw_Data[8][128]={0};
//�����κ��������������½�Ϊ0��0�㡣ʵ�ʶ�Ӧ�����ǵ�8�У�ÿ0��
void Draw_Point(uint8_t x,uint8_t y)
{
	uint8_t draw_x_line,draw_x_point;

	draw_x_line=7-x/8;  //7������Ϊ����������Χ��0~7����8����Ϊÿ����8���㣬��ʾ�㿪ʼΪ���Ͻǣ��任Ϊ���½�
	draw_x_point=7-x%8; //�������������ֽڵ�λ�ã�0��7λ����
	
	Draw_Data[draw_x_line][y] |=1<<draw_x_point;
	
	LCD_Transfer_Command(0xb0+draw_x_line);   //ѡ��Ҫ���͵���
	LCD_Transfer_Command(((y>>4)&0x0f)+0x10); //ѡ��Ҫ���͵���
	LCD_Transfer_Command(y&0x0f);             //ѡ��Ҫ���͵���
	LCD_Transfer_Data(Draw_Data[draw_x_line][y],1);
}





//��ָ������
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


//Һ����Ļ��ʼ��
void LCD_Init(void)
{
		gpio_pin_config_t LCD_config = {
        kGPIO_DigitalOutput, 0,
    };
		GPIO_PinInit(GPIOA, PIN10_IDX, &LCD_config);  //����PTA10Ϊ���===>>LCD_DC 
		GPIO_PinInit(GPIOA, PIN11_IDX, &LCD_config);  //����PTA11Ϊ���===>>LCD_RESET
		GPIO_PinInit(GPIOA, PIN12_IDX, &LCD_config);
		GPIO_PinInit(GPIOA, PIN13_IDX, &LCD_config);
		GPIO_PinInit(GPIOE, PIN6_IDX, &LCD_config);
		
		Lcd_Spi_Config();
		LCD_VDD_ON;
		LCD_RESET_L;    //�͵�ƽ��λ
		delay(500);
		LCD_RESET_H;
		delay(200);
		//����Դ�����磬��֤����
		//Һ�����ֲ��Һ�����������ֲ��о�û������
		LCD_Transfer_Command(0xae); //����ʾ
		LCD_Transfer_Command(0xd5); //����Ƶ��
		LCD_Transfer_Command(0x80);
		LCD_Transfer_Command(0xa8); //duty ����
		LCD_Transfer_Command(0x3f); //duty=1/64
		LCD_Transfer_Command(0xd3); //��ʾƫ��
		LCD_Transfer_Command(0x00);
		LCD_Transfer_Command(0x40); //��ʼ��
		LCD_Transfer_Command(0x8d); //��ѹ����
		LCD_Transfer_Command(0x14);
		LCD_Transfer_Command(0x20); //page address mode
		LCD_Transfer_Command(0x02);
		LCD_Transfer_Command(0xc8); //��ɨ��˳�򣺴��ϵ���
		LCD_Transfer_Command(0xa1); //��ɨ��˳�򣺴�����
		LCD_Transfer_Command(0xda); //sequential configuration
		LCD_Transfer_Command(0x12);
		LCD_Transfer_Command(0x81); //΢���Աȶȣ���ָ���0x81Ϊ�̶�ֵ
		LCD_Transfer_Command(0xcf); //΢���Աȶȵ�ֵ,�����÷�Χ0x00~0xff
		LCD_Transfer_Command(0xd9); //Set Pre-Charge Period
		LCD_Transfer_Command(0xf1);
		LCD_Transfer_Command(0xdb); //Set VCOMH Deselect Level
		LCD_Transfer_Command(0x40);
		clear_screen();
		LCD_VCC_ON;
		LCD_Transfer_Command(0xaf); //����ʾ
}




//����Ļ��ʾ
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


//������ʾ���ο�\display_data\num-bmp.h�ļ�

//��LCD��x��Y�У���5��������ʾ����
void Display_num_size5(uint8_t x,uint8_t y,uint8_t num)
{
	uint8_t i,j;
	for(j=x;j<x+2;j++)
	{
		lcd_address(j,y);
		for(i=0;i<6;i++)
		{
				//num_size5��num_bmp.c�ж���
				LCD_Transfer_Data(num_size5[num][(j-x)*6+i],1);
		}
	}
}



//������ʾ���ο�\display_data\num-bmp.h�ļ�
//��LCD��x��Y�У���ÿ���ַ�12��6����ʾ����
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



//������ʾ���ο�num-bmp.h�ļ�
//��LCD��x��Y�У���10��������ʾ������ģ�е�����
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


//������ʾ���ο�\display_data\num-bmp.h�ļ�
//��LCD��x��Y�У���ÿ���ַ�16��9����ʾ����
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


//������ʾ���ο�num-bmp.h�ļ�
//��LCD��x��Y�У���25��������ʾ����
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






//��ʾʱ��֮��ķָ����" : "
//�����ӡ
void Display_time_sapce(uint8_t x,uint8_t y)
{
	uint8_t i,j;

	lcd_address(4,y);//д��������
	for(i=0;i<4;i++)//��ӡ����
	{
		LCD_Transfer_Data(0x3C,1);//0x3C,��Ӧһ�д��ϵ���0011 1100
	}							//������ȡ��ģ		

	lcd_address(7,y);//д����7��
	for(i=0;i<4;i++)//��ӡ����
	{
		LCD_Transfer_Data(0x0f,1);//0x0f,��Ӧһ�д��ϵ���0000 1111
	}							//����0x3c ��0x0f��������ʱ�ͷ�֮�������":"������
			
}


//��ʾ���ڵķָ����  " - "
//�����ӡ
void Display_date_sapce(uint8_t x,uint8_t y)
{
	uint8_t i;

	lcd_address(1,y);
	for(i=0;i<4;i++)
	{
		LCD_Transfer_Data(0x80,1);//use spi comm  1000 0000
	}
}


//����ո�ȷָ���
//�����ӡ�ո�
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

//��ʾ�ܼ�
//�ܣ��õ���ģ
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


//����: x �ڼ��� С��3  
//����: y �ڼ���
//��ʾһ��16*16�������
//��ʾ������16*16�ģ��Ǿ��Ǵӿ�ʼ��x������ʾ���У�ÿ��8���㣩
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

//��ʾһ��16*12�������
//��ʾ������16*12�ģ��Ǿ��Ǵӿ�ʼ��x������ʾ���У�ÿ��8���㣩
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



//��ʾһ��16*8������֣������8���߶���16
//��ʾ������16*8�ģ��߶���16���Ǿ��Ǵӿ�ʼ��x������ʾ���У�ÿ��8���㣩
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



//��ʾһ��24*20�������
//��ʾ������24*20�ģ��Ǿ��Ǵӿ�ʼ��x������ʾ���У�ÿ��8���㣩

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


//��ʾһ��8*9�������
//��ʾ������8*9�ģ��߶���8���Ǿ��Ǵӿ�ʼ��x������ʾһ�У�ÿ��8���㣩
void Display_8_9_data(uint8_t x,uint8_t y,uint8_t *data)
{
	uint8_t i;
		lcd_address(x,y);
		for(i=0;i<9;i++)
		{
			LCD_Transfer_Data(data[i],1);
		}
}

//��ʾһ��16*5�������
//��ʾ������16*5�ģ��߶���16���Ǿ��Ǵӿ�ʼ��x������ʾ2��
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



//LCD����ʾ�㣬�����������
void Display_point(uint8_t num)
{
	if(num==2)
		Display_16_16_data(3,96,&Page_measuredoing_Data[160]);	
	else if(num==0)
		Display_16_16_data(3,96,&Page_measuredoing_Data[192]);
	else
		Display_16_16_data(3,96,&Page_measuredoing_Data[224]);	
}



//�ĵ����������������ߵͷ��Ȳ�ֵ
void Display_Waveforms_Info(uint8_t Info_Num)
{
	switch(Info_Num)
	{
		case 0:  //�ɼ����������ĵ����ݷ����Ƶ�Ѫѹ����ֵ��Χ
			//��ʾ12����  �����ĵ���������
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[72]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[144]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[168]);	
			break;
		case 10:  //�ĵ����ݸ��Ź���
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[20*24]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[21*24]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[24*24]);	
			break;
		case 11:	 //�ĵ���Ч���ݹ���
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[22*24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[23*24]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[25*24]);	
			break;
		case 1:  //�ĵ����ݷ�ֵ����
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[216]);	
			break;
		case 2:	 //�ĵ����ݷ�ֵ����
			Display_16_12_data(4,16,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[240]);	
			break;
		case 3:		//�ĵ��ֵ����
			Display_16_12_data(4,28,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[216]);
			break;
		case 4:  //�ĵ��ֵ����
			Display_16_12_data(4,28,&Waveforms_info_Data12[0]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[240]);
			break;
		case 5: //�������ݷ�ֵ����
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[216]);	
			break;
		case 6://�������ݷ�ֵ����
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[264]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[288]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[240]);	
			break;
		case 7:  //������ֵ����
			Display_16_12_data(4,28,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[216]);
			break;
		case 8:	 //������ֵ����
			Display_16_12_data(4,28,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[312]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[288]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[192]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[240]);
			break;
		case 12: //�������ݸ��Ź���
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[20*24]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[21*24]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[24*24]);	
			break;
		case 13://������Ч���ݹ���
			Display_16_12_data(4,16,&Waveforms_info_Data12[48]);
			Display_16_12_data(4,28,&Waveforms_info_Data12[72]);
			Display_16_12_data(4,40,&Waveforms_info_Data12[22*24]);
			Display_16_12_data(4,52,&Waveforms_info_Data12[23*24]);	
			Display_16_12_data(4,64,&Waveforms_info_Data12[96]);
			Display_16_12_data(4,76,&Waveforms_info_Data12[120]);	
			Display_16_12_data(4,88,&Waveforms_info_Data12[192]);
			Display_16_12_data(4,100,&Waveforms_info_Data12[25*24]);	
			break;
		case 9: //���ݼ����쳣
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


////�ĵ����������������ߵͷ��Ȳ�ֵ
//void Display_Waveforms_Info(uint8_t Info_Num)
//{
//	switch(Info_Num)
//	{
//		case 0:  //�ɼ����������ĵ����ݷ����Ƶ�Ѫѹ����ֵ��Χ
//			//��ʾ12����  �����ĵ���������
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
//		case 10:  //�ĵ����ݸ��Ź���
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
//		case 11:	 //�ĵ���Ч���ݹ���
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
//		case 1:  //�ĵ����ݷ�ֵ����
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
//		case 2:	 //�ĵ����ݷ�ֵ����
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
//		case 3:		//�ĵ��ֵ����
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[216]);
//			break;
//		case 4:  //�ĵ��ֵ����
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[0]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[24]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[240]);
//			break;
//		case 5: //�������ݷ�ֵ����
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
//		case 6://�������ݷ�ֵ����
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
//		case 7:  //������ֵ����
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[216]);
//			break;
//		case 8:	 //������ֵ����
//			clear_area(6,16,7,112);
//			Display_16_12_data(6,28,&Waveforms_info_Data12[48]);
//			Display_16_12_data(6,40,&Waveforms_info_Data12[72]);
//			Display_16_12_data(6,52,&Waveforms_info_Data12[312]);	
//			Display_16_12_data(6,64,&Waveforms_info_Data12[288]);
//			Display_16_12_data(6,76,&Waveforms_info_Data12[192]);	
//			Display_16_12_data(6,88,&Waveforms_info_Data12[240]);
//			break;
//		case 12: //�������ݸ��Ź���
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
//		case 13://������Ч���ݹ���
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
//		case 9: //���ݼ����쳣
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

//��ʾ��ص���

void Display_bat_bmp(uint8_t num)
{
	if(num==1) //����
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



//�Զ�����ģʽ����
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




//��ʾ����汾��Ϣ
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





//��ʾ��������ID��Ϣ
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



//��ʾ����ģ���ID��Ϣ
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



//��ʾ��������ID��Ϣ
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





//��������
void alarm_function(void)
{
	if((measure_result_data[0]*256+measure_result_data[1])>alarm.HBlood_HighValue) //��ǰ������ֵ���ڸ�ѹ�����趨��ֵ
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[0*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.HBlood_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HBlood_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HBlood_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HBlood_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if((measure_result_data[0]*256+measure_result_data[1])>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[(measure_result_data[0]*256+measure_result_data[1])/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if((measure_result_data[0]*256+measure_result_data[1])<alarm.HBlood_LowValue) //��ǰ������ֵС�ڸ�ѹ�����趨��ֵ
	{
		clear_screen();
		
		Display_16_12_data(3,20+0*13,&Alarm_Data[0*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.HBlood_LowValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HBlood_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HBlood_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HBlood_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if((measure_result_data[0]*256+measure_result_data[1])>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[(measure_result_data[0]*256+measure_result_data[1])/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[((measure_result_data[0]*256+measure_result_data[1])%10)*16]);
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[2]>alarm.LBlood_HighValue) //��ǰ������ֵ���ڵ�ѹ�����趨��ֵ
	{
		clear_screen();
		
		Display_16_12_data(3,20+0*13,&Alarm_Data[1*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.LBlood_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.LBlood_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.LBlood_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.LBlood_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(measure_result_data[2]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[2]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[2]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[2]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[2]<alarm.LBlood_LowValue) //��ǰ������ֵС�ڵ�ѹ�����趨��ֵ
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[1*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[2*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.LBlood_LowValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.LBlood_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.LBlood_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.LBlood_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(measure_result_data[2]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[2]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[2]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[2]%10)*16]);
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[3]>alarm.HeartRate_HighValue) //��ǰ������ֵ�������������趨��ֵ
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[3*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[4*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[5*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.HeartRate_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HeartRate_HighValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HeartRate_HighValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HeartRate_HighValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(measure_result_data[3]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[3]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[3]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[3]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else if(measure_result_data[3]<alarm.HeartRate_LowValue) //��ǰ������ֵС�����������趨��ֵ
	{
		clear_screen();
		Display_16_12_data(3,20+0*13,&Alarm_Data[3*24]);
		Display_16_12_data(3,20+1*13,&Alarm_Data[4*24]);
		Display_16_12_data(3,20+2*13,&Alarm_Data[6*24]);
		Display_16_12_data(3,20+3*13,&Alarm_Data[7*24]);

		Display_16_8_data(3,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(alarm.HeartRate_HighValue>99)
			Display_16_8_data(3,20+4*13+1*8,&hex_8_16[alarm.HeartRate_LowValue/100*16]);
		Display_16_8_data(3,20+4*13+2*8,&hex_8_16[(alarm.HeartRate_LowValue/10%10)*16]);
		Display_16_8_data(3,20+4*13+3*8,&hex_8_16[(alarm.HeartRate_LowValue%10)*16]);
	
	
		Display_16_12_data(5,20+0*13,&Alarm_Data[8*24]);
		Display_16_12_data(5,20+1*13,&Alarm_Data[9*24]);
		Display_16_12_data(5,20+2*13,&Alarm_Data[10*24]);
		Display_16_12_data(5,20+3*13,&Alarm_Data[11*24]);

		Display_16_8_data(5,20+4*13+0*8,&hex_8_16[19*16]);  //��ʾ ����
		if(measure_result_data[3]>99)
			Display_16_8_data(5,20+4*13+1*8,&hex_8_16[measure_result_data[3]/100*16]);
		Display_16_8_data(5,20+4*13+2*8,&hex_8_16[(measure_result_data[3]/10%10)*16]);
		Display_16_8_data(5,20+4*13+3*8,&hex_8_16[(measure_result_data[3]%10)*16]);
		
		if(LCD_Display_OnOff==Display_OFF)
		{
			LCD_Transfer_Command(0xaf); //����ʾ
			LCD_Display_OnOff=Display_ON;							
		}
		Delay_S(3);
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
}



//�Զ��������ڽ��е�״̬��ʾ
void auto_measure_going(void)
{
	clear_screen();
	Display_16_16_data(4,25,&auto_measure_going_data[0*32]);
	Display_16_16_data(4,43,&auto_measure_going_data[1*32]);
	Display_16_16_data(4,61,&auto_measure_going_data[2*32]);
	Display_16_16_data(4,79,&auto_measure_going_data[3*32]);
	Display_16_16_data(4,97,&auto_measure_going_data[4*32]);
}
