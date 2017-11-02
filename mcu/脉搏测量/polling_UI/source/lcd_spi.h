#ifndef _LCD_SPI_H_
#define _LCD_SPI_H_

#include "stdint.h"

extern uint8_t LCD_Display_OnOff;
#define Display_ON 1
#define Display_OFF 2

#define	LCD_VCC_ON	GPIO_SetPinsOutput(GPIOA, 1u << 13)  //�����ѹ12.5 ������Ч
#define	LCD_VCC_OFF	GPIO_ClearPinsOutput(GPIOA, 1u << 13)  //�����ѹ12.5 ������Ч

//����������
extern unsigned char Draw_Data[8][128];
//�����κ��������������½�Ϊ0��0�㡣ʵ�ʶ�Ӧ�����ǵ�8�У�ÿ0��
void Draw_Point(uint8_t x,uint8_t y);


void clear_screen(void);
void clear_area(uint8_t ,uint8_t ,uint8_t ,uint8_t );
void Lcd_Spi_Config(void);
void LCD_Init(void);
void Display_full_screen(unsigned char * data);


void Display_num_size5(uint8_t x,uint8_t y,uint8_t num);
void Display_num_size10(uint8_t x,uint8_t y,uint8_t num);
void Display_num_size16_9(uint8_t x,uint8_t y,uint8_t num);
void Display_num_size25(uint8_t x,uint8_t y,uint8_t num);
void Display_week_data(uint8_t x,uint8_t y,uint8_t num);
void Display_16_16_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_16_12_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_16_8_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_24_20_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_8_9_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_16_5_data(uint8_t x,uint8_t y,uint8_t *data);
void Display_num_size12_6(uint8_t x,uint8_t y,uint8_t num);
void Display_point(uint8_t num);
void Display_Waveforms_Info(uint8_t Info_Num);

void Display_time_sapce(uint8_t x,uint8_t y);
void Display_date_sapce(uint8_t x,uint8_t y);
void Clear_sapce(uint8_t x,uint8_t y);
void Display_bat_bmp(uint8_t num);
void Display_auto_measure_IsTurnOn();
void Display_Version_Info(void);
void Display_Device_ID(void);
void Display_PW1101IC_ID(void);
void Display_Sensor_ID(void);

void LCD_Transfer_Command(uint8_t data);
void alarm_function(void);

void auto_measure_going(void);

#endif
