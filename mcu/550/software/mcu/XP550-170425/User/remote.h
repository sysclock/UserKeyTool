#ifndef __RED_H
#define __RED_H 
 

  
#define RDATA GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)	 //红外数据输入脚

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0      		   

extern unsigned char  RmtCnt;	//按键按下的次数
extern unsigned char  IRcord[4];

void Remote_Init(void);    //红外传感器接收头引脚初始化
unsigned char Remote_Scan(void);	  
void TIM3_Configuration(void);


#endif
















