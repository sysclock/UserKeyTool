/********************************************
【文 件 名】Init.h
【功能说明】初始化函数的声明和数据结构的声明
********************************************/
#ifndef INIT_H
#define INIT_H

#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"

/* Private functions ---------------------------------------------------------*/
#define HARD_VERSION    3		
#define PROT_VERSION	20     
#define SOFT_VERSION    6		

#define TypeVideo 	0x01 
#define TypeVGA	 	0x02
#define TypeRGB  	0x04
#define TypeFM      0x08   
#define TypeDVI     0x08
#define TypeAudio  	0x10

#define SYS_INFO_BAK_ADDR 0x0800FC00
//AT24XX
#define AT_SDA_PIN        GPIO_Pin_7
#define AT_SDA_LOW        GPIO_WriteBit(GPIOB,AT_SDA_PIN,Bit_RESET)
#define AT_SDA_HI     	  GPIO_WriteBit(GPIOB,AT_SDA_PIN,Bit_SET)
#define AT_SDA_IS_HI	  GPIO_ReadInputDataBit(GPIOB,AT_SDA_PIN)
#define AT_SDA_INPUT	  GPIOB->CRL = GPIOB->CRL & (~(0x0000000f << 28)) | (0x00000008 << 28)
#define AT_SDA_OUTPUT	  GPIOB->CRL = GPIOB->CRL & (~(0x0000000f << 28)) | (0x00000003 << 28)

#define AT_SCK_PIN        GPIO_Pin_6
#define AT_SCK_LOW        GPIO_WriteBit(GPIOB,AT_SCK_PIN,Bit_RESET)
#define AT_SCK_HI         GPIO_WriteBit(GPIOB,AT_SCK_PIN,Bit_SET)

//KEY硬件引脚的定义
#define KEY_EN_PIN		GPIO_Pin_4
#define KEY_R0 				GPIO_Pin_10
#define KEY_R1 				GPIO_Pin_12
#define KEY_R2 				GPIO_Pin_11

#define KEY_R0_HIGH		GPIO_WriteBit(GPIOC,KEY_R0,Bit_SET)
#define KEY_R1_HIGH	  GPIO_WriteBit(GPIOC,KEY_R1,Bit_SET)
#define KEY_R2_HIGH		GPIO_WriteBit(GPIOC,KEY_R2,Bit_SET)

#define KEY_R0_LOW   	GPIO_WriteBit(GPIOC,KEY_R0,Bit_RESET)
#define KEY_R1_LOW	  GPIO_WriteBit(GPIOC,KEY_R1,Bit_RESET)
#define KEY_R2_LOW	  GPIO_WriteBit(GPIOC,KEY_R2,Bit_RESET)

#define KEY_EN				GPIO_WriteBit(GPIOA,KEY_EN_PIN,Bit_RESET)
#define KEY_DIS				GPIO_WriteBit(GPIOA,KEY_EN_PIN,Bit_SET)

//BEEP
#define BEEP_PIN      GPIO_Pin_5
#define BEEP_ON     	GPIO_WriteBit(GPIOA,BEEP_PIN,Bit_SET)
#define BEEP_OFF    	GPIO_WriteBit(GPIOA,BEEP_PIN,Bit_RESET)

#define BEEP_START		    if(SysInfo.BeepEn) \
								TIM_Cmd(TIM3,ENABLE)
#define BEEP_STOP			TIM_Cmd(TIM3,DISABLE)

//LCD
#define LCD_RST_PIN   GPIO_Pin_7
#define LCD_RST_LOW   GPIO_WriteBit(GPIOA,LCD_RST_PIN,Bit_RESET)
#define LCD_RST_HI    GPIO_WriteBit(GPIOA,LCD_RST_PIN,Bit_SET)

#define LCD_CS_PIN   GPIO_Pin_6
#define LCD_CS_DIS    GPIO_WriteBit(GPIOA,LCD_CS_PIN,Bit_SET)
#define LCD_CS_EN   GPIO_WriteBit(GPIOA,LCD_CS_PIN,Bit_RESET)

#define LCD_RS_PIN    GPIO_Pin_0
#define LCD_RS_HI     GPIO_WriteBit(GPIOB,LCD_RS_PIN,Bit_SET)
#define LCD_RS_LOW    GPIO_WriteBit(GPIOB,LCD_RS_PIN,Bit_RESET)

#define LCD_WR_PIN    GPIO_Pin_1
#define LCD_WR_HI     GPIO_WriteBit(GPIOB,LCD_WR_PIN,Bit_SET)
#define LCD_WR_LOW    GPIO_WriteBit(GPIOB,LCD_WR_PIN,Bit_RESET)

#define LCD_RD_PIN   GPIO_Pin_2
#define LCD_RD_HI    GPIO_WriteBit(GPIOB,LCD_RD_PIN,Bit_SET)
#define LCD_RD_LOW   GPIO_WriteBit(GPIOB,LCD_RD_PIN,Bit_RESET)

#define LCD_BK_PIN    GPIO_Pin_9
#define LCD_BK_ON     GPIO_WriteBit(GPIOC,LCD_BK_PIN,Bit_SET)
#define LCD_BK_OFF    GPIO_WriteBit(GPIOC,LCD_BK_PIN,Bit_RESET)

//LED
#define UART_LED_PIN        GPIO_Pin_8
#define UART_LED_ON         GPIO_WriteBit(GPIOA,UART_LED_PIN,Bit_RESET)
#define UART_LED_OFF     	GPIO_WriteBit(GPIOA,UART_LED_PIN,Bit_SET)

//IR
#define IR_INT_PIN		  GPIO_Pin_11
#define IR_INT_SOURCE		GPIO_PinSource11

//SWITCH
#define UPDATE_PIN        GPIO_Pin_0
#define UPDATE_HI         GPIO_WriteBit(GPIOA,UPDATE_PIN,Bit_SET)
#define UPDATE_LOW        GPIO_WriteBit(GPIOA,UPDATE_PIN,Bit_RESET)

#define DATE_PIN          GPIO_Pin_1
#define DATE_HI           GPIO_WriteBit(GPIOA,DATE_PIN,Bit_SET)
#define DATE_LOW          GPIO_WriteBit(GPIOA,DATE_PIN,Bit_RESET)

#define SDA_PIN             GPIO_Pin_9
#define SDA_HI				GPIO_WriteBit(GPIOB,SDA_PIN,Bit_SET)
#define SDA_LOW				GPIO_WriteBit(GPIOB,SDA_PIN,Bit_RESET)

#define SCK_PIN						GPIO_Pin_8
#define SCK_HI						GPIO_WriteBit(GPIOB,SCK_PIN,Bit_SET)
#define SCK_LOW						GPIO_WriteBit(GPIOB,SCK_PIN,Bit_RESET)

//FM
//旧版本电路
#define FM_SDA_HI				DATE_HI
#define FM_SDA_LOW				DATE_LOW
#define FM_SDA_IS_HI	      GPIO_ReadInputDataBit(GPIOA,DATE_PIN)
#define FM_SDA_INPUT	      GPIOA->CRL = GPIOA->CRL & (~(0x0000000f << 4)) | (0x00000008 << 4)
#define FM_SDA_OUTPUT	      GPIOA->CRL = GPIOA->CRL & (~(0x0000000f << 4)) | (0x00000003 << 4)

#define FM_SCK_HI				UPDATE_HI
#define FM_SCK_LOW				UPDATE_LOW

//---DVI----M211X1-----0~3--------
#define HDMI_SDO              GPIO_Pin_0
#define HDMI_SDO_INPUT        GPIOC->CRL = GPIOC->CRL & (~(0x0000000f << 0)) | (0x00000008 << 0)
#define HDMI_SDO_OUTPUT       GPIOC->CRL = GPIOC->CRL & (~(0x0000000f << 0)) | (0x00000003 << 0)
#define HDMI_SDO_IS_HI        GPIO_ReadInputDataBit(GPIOC,HDMI_SDO)

#define HDMI_SCLK             GPIO_Pin_1
#define HDMI_SCLK_HI          GPIO_WriteBit(GPIOC,HDMI_SCLK,Bit_SET)
#define HDMI_SCLK_LW          GPIO_WriteBit(GPIOC,HDMI_SCLK,Bit_RESET)

#define HDMI_SDI              GPIO_Pin_2
#define HDMI_SDI_HI           GPIO_WriteBit(GPIOC,HDMI_SDI,Bit_SET)
#define HDMI_SDI_LW           GPIO_WriteBit(GPIOC,HDMI_SDI,Bit_RESET)

#define HDMI_xRST             GPIO_Pin_3
#define HDMI_xRST_HI           GPIO_WriteBit(GPIOC,HDMI_xRST,Bit_SET)
#define HDMI_xRST_LW           GPIO_WriteBit(GPIOC,HDMI_xRST,Bit_RESET)
//---DVI----M211X1-----4~7--------
#define HDMI_xOUTDIS          GPIO_Pin_4
#define HDMI_xOUTDIS_HI       GPIO_WriteBit(GPIOC,HDMI_xOUTDIS,Bit_SET)
#define HDMI_xOUTDIS_LW       GPIO_WriteBit(GPIOC,HDMI_xOUTDIS,Bit_RESET)

#define FAN_CTL               GPIO_Pin_5
#define FAN_CTL_HI            GPIO_WriteBit(GPIOC,FAN_CTL,Bit_SET)
#define FAN_CTL_LW            GPIO_WriteBit(GPIOC,FAN_CTL,Bit_RESET)

#define HDMI_xINDIS           GPIO_Pin_6
#define HDMI_xINDIS_HI        GPIO_WriteBit(GPIOC,HDMI_xINDIS,Bit_SET)
#define HDMI_xINDIS_LW        GPIO_WriteBit(GPIOC,HDMI_xINDIS,Bit_RESET)

#define HDMI_xCS              GPIO_Pin_7
#define HDMI_xCS_HI           GPIO_WriteBit(GPIOC,HDMI_xCS,Bit_SET)
#define HDMI_xCS_LW           GPIO_WriteBit(GPIOC,HDMI_xCS,Bit_RESET)

/*
//新版本电路
#define FM_SDA_HI				SDA_HI
#define FM_SDA_LOW				SDA_LOW
#define FM_SDA_IS_HI	      GPIO_ReadInputDataBit(GPIOB,SDA_PIN)
#define FM_SDA_INPUT	      GPIOB->CRH = GPIOB->CRH & (~(0x0000000f << 4)) | (0x00000008 << 4)
#define FM_SDA_OUTPUT	      GPIOB->CRH = GPIOB->CRH & (~(0x0000000f << 4)) | (0x00000003 << 4)

#define FM_SCK_HI				SCK_HI
#define FM_SCK_LOW				SCK_LOW
*/
//BUS
#define BUS_OUT								GPIOB->CRH = 0x33333333
#define BUS_IN								GPIOB->CRH = 0x88888888

//EEPROM ADDR
/*
#define SAVE_V_ADDR	    0x000		//预置位视频基地址
#define SAVE_A_ADDR	    0x3E0		//预置位音频基地址
*/
#define FIX_ADD			0x7C0		//设备地址
#define FIX_BAUD		0X7C1		//设备波特率
#define FIX_TYPE		0X7C2		//设备类型
#define FIX_INPUT		0X7C3		//输入路数
#define FIX_OUTPUT	    0X7C4		//输出路数
#define FIX_LOGO		0X7D0		//产品LOGO字符串
#define FIX_SN			0X7E0		//产品序列号
#define FIX_BEEP		0X7E4		//蜂鸣器开关
#define FIX_TIME		0X7E5		//产品出厂日期
#define FIX_INFO		0X7F0		//产品型号字符串
/*
#define OUTPUT_MAX	    32			//最大输出路数
#define SAVE_NUM		30			//保存的最大组数
*/
#define Baud_4800		0
#define Baud_9600		1
#define Baud_38400	    2
#define Baud_115200	    3

#define IR_START		TIM4->CNT = 0,TIM_Cmd(TIM4,ENABLE)
#define IR_STOP			TIM_Cmd(TIM4,DISABLE)

#define UART_START      TIM2->CNT = 0,TIM_Cmd(TIM2,ENABLE)
#define UART_STOP       TIM_Cmd(TIM2,DISABLE)

extern uc32 BaudArray[4];
extern const char* pLogo[4];
typedef struct
{
	u8 			InitOK;			//初始化成功
    u8          TypeModifyEn;   //修改类型使能
    u8          PresetCount;    //预置组总数
    u8          AStartAddr;     //音频预置组起始地址
	u8 			Addr;
	u8 			Baud;
	u8 			Type;
	u8 			InputNum;
	u8 			OutputNum;
	u8 			Logo[17];
	u8			Info[17];
	u8 			SN;
	u8			BeepEn;
        u8                      CycleEn;
}Sys_T;
extern Sys_T SysInfo;		//系统信息

typedef struct
{
//	u8 UartRecvFlag;	//开始接收标识
  	u8 UartFlag;		//接收完毕标识
  	u8 RecvLen;
  	u8 RecvBuf[64];
	u8 SendBuf[256];
}Uart_T;
extern Uart_T Uart;

typedef struct
{
  bool HeadFlag;    //头标志
  bool EndFlag;     //结束标志
  u8 Index;
  u32 Data;
}IR_T;
extern IR_T IRInfo;

extern vu8 TickFlag;		//10ms定时标志
extern u32 LcdBkSec;		//液晶背光亮的时间

void GPIO_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void SysTick_Configuration(void);
void PeriphClkInit(void);
void TimeInit(void);
void IWDGInit(void);
void Com_Config(u32 Baud);
void SetDefSysInfo(u8 Type,u8 InputNum,u8 OutputNum,u8* pLogo,u8* pInfo);
void ReadSysInfo(void);
void IR_ISR(void);
void DelayMs(int);
void DelayUs(int);
bool CheckSectury(void);
bool DataInit(void);
void ResetSysInfo(void);
void FlashWriteStr(void);
inline void SetPortData(u8 Data)
{
	u16 Temp = Data;
  GPIOB->ODR = (GPIOB->ODR&0x00FF) | (Temp<<8);
}

inline void ReadPortData(u8 *pData)
{
	*pData = (GPIOB->IDR)>>8;
}

inline void SetAAddr(u8 Addr)
{
	GPIOC->ODR = (GPIOC->ODR&0xFFF0) | (Addr&0x0f);
}

inline void SetVAddr(u8 Addr)
{
	GPIOC->ODR = (GPIOC->ODR&0xFF0F) | (Addr<<4);
}

inline void SetKeyRVal(u8 Val)
{
	GPIOC->ODR = (GPIOC->ODR&0xE3FF) | ((Val&0x07)<<10);
}
#endif
