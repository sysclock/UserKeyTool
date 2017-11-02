#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"

extern unsigned char Key_Value;

/******************************************
  硬件连接    菜单-PB9	   KEY1
  			  确认-PA15	   KEY2
			  场景-PB3	   KEY3
			  电源-PA11	   KEY4
			  FOCUS+  -PB7	 KEY5
			  FOCUS-  -PB8	 KEY6
			  ZOOM+	  -PB5	 KEY7
			  ZOOM-   -PB6	 KEY8
*******************************************/
#define KEY1  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)
#define KEY2  !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define KEY3  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define KEY4  !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)

#define KEY5  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define KEY6  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define KEY7  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
#define KEY8  !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)

#define KEY1_VALUE       1
#define KEY2_VALUE       2
#define KEY3_VALUE       3
#define KEY4_VALUE       4
#define KEY5_VALUE       5
#define KEY6_VALUE       6
#define KEY7_VALUE       7
#define KEY8_VALUE       8


void KEY_GPIO_Config(void);
u8 Scan_Key_Value(void);
extern unsigned char MENU_KEY_FLAG;
extern u8 Key2_flag;
extern u32 Key2_time_count;
extern u8 Key2_num;
extern unsigned char Key1_flag;
extern unsigned int Key1_time_count;
#endif
