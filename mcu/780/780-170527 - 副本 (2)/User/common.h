/*======================================================
*	Copyright(c) *	All rights reserved.
*
*	file name:  COMMON.h
*	version:  1.0
*	date: 2011-09-24
*======================================================*/
#ifndef _COMMON_H
#define _COMMON_H


//#define KEY_REVERSE
#define MSG_LENGTH 10

#define ENGLISH  1
#define CHINESE  2

#define STM32F103RB
#define NO_LOGO
//#define PC

//#define XP_520 1                //不带输入板	
//#define XP_530 2                //带输入板
//#define MATHINE_TYPE   XP_520 //XP_530//
////#define START_QUICK_SWITCH

#if(MATHINE_TYPE==XP_520)
	#define USE_INFRARED_IR
#endif

#define LOAD_TIME_LONG 160	//180




typedef unsigned char      U8;
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned int       BOOL;




#if(MATHINE_TYPE==XP_530)
#define ADV7441A_INPUT 0
#define SII9125_INPUT 1
#define SDI_INPUT 2
#define INPUT_BOARD ADV7441A_INPUT//SDI_INPUT//SII9125_INPUT//
#endif

//----------------------是否使用飞梭旋钮--------------------
#ifdef STM32F103RB
	//#define TCP_IP
	#define EC11_BUTTON
#endif
//-------------------------------------------------------------------

//-------------------------------------------------------------------------

#define INT_DELAY_PC_CTL		//中断延时判断是否上位机控制

//---------------------------define input source-------------------------------------					

#define DEBUG_KEY	OK_KEY
#define SCEN_KEY  TAKE_KEY
#define MENU_KEY	OK_KEY
#define LOCK_KEY 	ESC_KEY
#define RESET_KEY	FREEZE_KEY
#define BACKSPACE_KEY FULLPART_KEY

//--------------------------------------------------------------------------------

#define SCENARIO_1 	NUM1_KEY	
#define SCENARIO_2 	NUM2_KEY	
#define SCENARIO_3 	NUM3_KEY	
#define SCENARIO_4 	NUM4_KEY
#define SCENARIO_5	NUM5_KEY
#define SCENARIO_6		NUM6_KEY
#define SCENARIO_7 	NUM7_KEY
#define SCENARIO_8		NUM8_KEY
#define SCENARIO_9	NUM9_KEY



typedef enum
{
    INPUTSOURCE_NONE,
    INPUTSOURCE_CVBS,	// VIDEO1 - CVBS
    INPUTSOURCE_CVBS2,	// VIDEO2 - CVBS2
    INPUTSOURCE_CVBS3, 	// VIDEO2 - CVBS2
    INPUTSOURCE_CVBS4, 	// VIDEO2 - CVBS2
    INPUTSOURCE_DVI,	// PC - DVI
    INPUTSOURCE_DVI2,
    INPUTSOURCE_YPBPR,	// VIDEO - YPbPr1
    INPUTSOURCE_VGA,	// PC - VGA
    INPUTSOURCE_VGA2,	// PC - VGA
    INPUTSOURCE_HDMI,	// PC - DVI
    INPUTSOURCE_SDI,
    INPUTSOURCE_USB,
    INPUTSOURCE_USB2,
    INPUTSOURCE_DP,
    INPUTSOURCE_NUM
}_InputSourceType;

typedef enum
{
	Scene_1=1,
	Scene_2,
	Scene_3,
	Scene_4,
	Scene_5,
	Scene_6,
	Scene_7,
	Scene_8,
	Scene_9,
}g_CurrentScene;









//_InputSourceType Color_Input;











typedef enum
{
	USBSTATE_NONE,
	USBSTATE_PALY,
	USBSTATE_MENU,
	USBSTATE_NUM
}_USBSTATE;

typedef enum
{
    Communication_NONE,
    Communication_USART1=0x01,   // 串口1
    Communication_USART2=0x02,   // 串口2
    Communication_USART3=0x03,   // 串口3
    Communication_Ethernet=0x04,  // 以太网
    Communication_NUM
}_CommunicationType;


////////////////////////////////是否使用飞梭旋钮//////////////////////////////////////////////
#ifdef EC11_BUTTON
  #define BMA_GPIO GPIOD
  #define BMA_PIN GPIO_Pin_3
  #define BMB_GPIO GPIOD
  #define BMB_PIN GPIO_Pin_0
  #define BMC_GPIO GPIOD
  #define BMC_PIN GPIO_Pin_1
  #define BMA GPIO_ReadInputDataBit(BMA_GPIO, BMA_PIN)
  #define BMB GPIO_ReadInputDataBit(BMB_GPIO, BMB_PIN) 
  #define BMC GPIO_ReadInputDataBit(BMC_GPIO, BMC_PIN)
#endif
////////////////////////////////是否使用飞梭旋钮//////////////////////////////////////////////

////////////////////////////////声音通道切换，vga切换及LED灯///////////////////////////////
#ifdef STM32F103RB
#define PC_AUD_SEL0_GPIO GPIOC
#define PC_AUD_SEL0_PIN GPIO_Pin_9
#define PC_AUD_SEL1_GPIO GPIOC
#define PC_AUD_SEL1_PIN GPIO_Pin_10

#define LED_LAMP_GREEN_GPIO GPIOB
#define LED_LAMP_GREEN_PIN GPIO_Pin_1
/*
#define VGA_SEL_GPIO GPIOC
#define VGA_SEL_PIN GPIO_Pin_12
#define VGA_SEL_H_GPIO GPIOC
#define VGA_SEL_H_PIN GPIO_Pin_11
*/
//#define IO_UART_TX_GPIO GPIOC
//#define IO_UART_TX_PIN GPIO_Pin_11
//#define IO_UART_RX_GPIO GPIOC
//#define IO_UART_RX_PIN GPIO_Pin_12








#define IO_UART_TX_GPIO GPIOC
#define IO_UART_TX_PIN GPIO_Pin_10
#define IO_UART_RX_GPIO GPIOC
#define IO_UART_RX_PIN GPIO_Pin_11









#define IO_UART_TX_L GPIO_ResetBits(IO_UART_TX_GPIO,IO_UART_TX_PIN)
#define IO_UART_TX_H GPIO_SetBits(IO_UART_TX_GPIO,IO_UART_TX_PIN)
#define IO_UART_RX GPIO_ReadInputDataBit(IO_UART_RX_GPIO, IO_UART_RX_PIN)

#define MAIN_MUTE_GPIO GPIOB
#define MAIN_MUTE_PIN GPIO_Pin_8

#else
#define PC_AUD_SEL0_GPIO GPIOB
#define PC_AUD_SEL0_PIN GPIO_Pin_0
#define PC_AUD_SEL1_GPIO GPIOB
#define PC_AUD_SEL1_PIN GPIO_Pin_1
#define LED_LAMP_GREEN_GPIO GPIOB
#define LED_LAMP_GREEN_PIN GPIO_Pin_5
/*
#define VGA_SEL_GPIO GPIOC
#define VGA_SEL_PIN GPIO_Pin_13
#define VGA_SEL_H_GPIO GPIOA
#define VGA_SEL_H_PIN GPIO_Pin_0
*/
#define MAIN_MUTE_GPIO GPIOB
#define MAIN_MUTE_PIN GPIO_Pin_3
#endif
////////////////////////////////声音通道切换，vga切换及LED灯///////////////////////////////


///////////////////////////////////////////////////DVI及HDMI切换////////////////////////////////////
#ifdef STM32F103RB
#define PC_SEL_HDMI_GPIO GPIOC
#define PC_SEL_HDMI_PIN GPIO_Pin_8

#define PC_SEL_DVI_GPIO GPIOC
#define PC_SEL_DVI_PIN GPIO_Pin_7

#define PC_SEL_DVI_L GPIO_ResetBits(PC_SEL_DVI_GPIO,PC_SEL_DVI_PIN)
#define PC_SEL_DVI_H GPIO_SetBits(PC_SEL_DVI_GPIO,PC_SEL_DVI_PIN)
#else
#define PC_SEL_HDMI_GPIO GPIOB
#define PC_SEL_HDMI_PIN GPIO_Pin_10
#endif

#define PC_SEL_HDMI_L GPIO_ResetBits(PC_SEL_HDMI_GPIO,PC_SEL_HDMI_PIN)
#define PC_SEL_HDMI_H GPIO_SetBits(PC_SEL_HDMI_GPIO,PC_SEL_HDMI_PIN)
///////////////////////////////////////////////////DVI及HDMI切换////////////////////////////////////


///////////////////////////////////////////LCD液晶屏幕和按键驱动////////////////////////////////////
#ifdef STM32F103RB
	#define LCD_CS_GPIO GPIOB
	#define LCD_CS_PIN GPIO_Pin_13
	#define LCD_CLK_GPIO GPIOB
	#define LCD_CLK_PIN GPIO_Pin_15
	#define LCD_DAT_GPIO GPIOB
	#define LCD_DAT_PIN GPIO_Pin_14
	#define KB_PLOAD_GPIO GPIOB
	#define KB_PLOAD_PIN GPIO_Pin_9
#else
	#define LCD_CS_GPIO GPIOA
	#define LCD_CS_PIN GPIO_Pin_11	
	#define LCD_CLK_GPIO GPIOA
	#define LCD_CLK_PIN GPIO_Pin_12	
	#define LCD_DAT_GPIO GPIOB
	#define LCD_DAT_PIN GPIO_Pin_6	
	#define KB_PLOAD_GPIO GPIOB
	#define KB_PLOAD_PIN GPIO_Pin_2
#endif	
	#define KB_SCK_GPIO GPIOE
	#define KB_SCK_PIN GPIO_Pin_0
	
	#define KB_SDOUT_GPIO GPIOE
	#define KB_SDOUT_PIN GPIO_Pin_1
	#define LCD_BK_GPIO GPIOC
	#define LCD_BK_PIN GPIO_Pin_9


#define KB_PLOAD_L GPIO_ResetBits(KB_PLOAD_GPIO,KB_PLOAD_PIN)
#define KB_PLOAD_H GPIO_SetBits(KB_PLOAD_GPIO,KB_PLOAD_PIN)
#define KB_SCK_L GPIO_ResetBits(KB_SCK_GPIO,KB_SCK_PIN)
#define KB_SCK_H GPIO_SetBits(KB_SCK_GPIO,KB_SCK_PIN)
#define KB_SDOUT GPIO_ReadInputDataBit(KB_SDOUT_GPIO, KB_SDOUT_PIN)

#define LCD_CS_L GPIO_ResetBits(LCD_CS_GPIO,LCD_CS_PIN)
#define LCD_CS_H GPIO_SetBits(LCD_CS_GPIO,LCD_CS_PIN)
#define LCD_CLK_L GPIO_ResetBits(LCD_CLK_GPIO,LCD_CLK_PIN)
#define LCD_CLK_H GPIO_SetBits(LCD_CLK_GPIO,LCD_CLK_PIN)
#define LCD_DAT_L GPIO_ResetBits(LCD_DAT_GPIO,LCD_DAT_PIN)
#define LCD_DAT_H GPIO_SetBits(LCD_DAT_GPIO,LCD_DAT_PIN)

#define LCD_BK_L GPIO_ResetBits(LCD_BK_GPIO,LCD_BK_PIN)
#define LCD_BK_H GPIO_SetBits(LCD_BK_GPIO,LCD_BK_PIN)

///////////////////////////////////////LCD液晶屏幕和按键驱动////////////////////////////////////////


///////////////////////////////////////////按键盘的LED灯驱动////////////////////////////////////
#define LED_IN_GPIO GPIOE
#define LED_IN_PIN GPIO_Pin_2
#define LED_SRCK_GPIO GPIOE
#define LED_SRCK_PIN GPIO_Pin_3
#define LED_RCK_GPIO GPIOE
#define LED_RCK_PIN GPIO_Pin_4
#define LED_IN_L GPIO_ResetBits(LED_IN_GPIO,LED_IN_PIN)
#define LED_IN_H GPIO_SetBits(LED_IN_GPIO,LED_IN_PIN)
#define LED_SRCK_L GPIO_ResetBits(LED_SRCK_GPIO,LED_SRCK_PIN)
#define LED_SRCK_H GPIO_SetBits(LED_SRCK_GPIO,LED_SRCK_PIN)
#define LED_RCK_L GPIO_ResetBits(LED_RCK_GPIO,LED_RCK_PIN)
#define LED_RCK_H GPIO_SetBits(LED_RCK_GPIO,LED_RCK_PIN)
///////////////////////////////////////////按键盘的LED灯驱动/////////////////////////////////////////




///////////////////////////////////Start STM32 I2C Config//////////////////////////////////////////////////////////

#ifdef STM32F103RB
	#define ENABLE_SOFTI2C   1//0
#else	
	#define ENABLE_SOFTI2C   1
#endif

#define ENABLE_HARDI2C (!ENABLE_SOFTI2C)

#ifdef STM32F103RB
	#define EEPROM_SDA_PIN GPIO_Pin_7
	#define EEPROM_SCL_PIN GPIO_Pin_6
#else
	#define EEPROM_SDA_PIN GPIO_Pin_9
	#define EEPROM_SCL_PIN GPIO_Pin_8
#endif

#if(ENABLE_SOFTI2C)
	#define EEPROM_SCL_1   GPIO_SetBits(GPIOB,EEPROM_SCL_PIN)
	#define EEPROM_SCL_0   GPIO_ResetBits(GPIOB,EEPROM_SCL_PIN)
	#define EEPROM_SDA_1  GPIO_SetBits(GPIOB,EEPROM_SDA_PIN)
	#define EEPROM_SDA_0  GPIO_ResetBits(GPIOB,EEPROM_SDA_PIN)
	#define EEPROM_SDA GPIO_ReadInputDataBit(GPIOB, EEPROM_SDA_PIN)
#endif
////////////////////////////////////End STM32 I2C Config////////////////////////////////////////////////////////


//----------------------------------------------------------------------------------



#define LOCKKEYPAD 1
#define UNLOCKKEYPAD 2

#define INPUTKEYNUMS     7
#define FULLCOUNT 60
#define LOGNTIME 40//100


#define DEF_CURRENT_SCENE  1     //场景为1
#define DEF_VGA_CURRENT_SCENE  1    //恢复出厂设置时:场景为1

#define DEF_OUTPORT_NUM 2
#define DEF_LAYOUT_NUM 2

#define DEF_BRIGHTNESS      50
#define DEF_CONTRAST      50
#define DEF_SATURATION      50
#define DEF_VOLUME          50


#define DEF_SCREEN_TOTAL_HSIZE                 1024
#define DEF_SCREEN_TOTAL_VSIZE                768


#define DVI 1
#define VGA 0

#define KEYPAD   1
#define PC       0
#define TB3531   2

/*******************************
系统控制消息组及子组宏定义
*******************************/

#define SYSTEM_GROUPID 0x08
#define FACTORY_RESET 0X05


/*******************************
通知消息组及子组宏定义
*******************************/

#define DVI_DISCONNECT 0x02
#define DVI_RECONNECT  0x03


/*******************************
音量组及子组宏定义
********************************/
#define VOLUMEGROUPID  0x05
#define VOLUME_SET 0x01
#define VOLUME_CHECK 0x02

/*******************************
图像质量组及子组宏定义
********************************/
#define IMAGEGROUPID  0x02
#define IMAGE_CONTRAST 0x01
#define IMAGE_SATURATION 0x02
#define IMAGE_BRIGHTNESS    0x03



/******************************
LED屏幕消息消入子组宏定义
******************************/

#define LEDPARPGROUPID  0x07
#define LED_SET  0x01
#define LED_CHECK 0x02


/*******************************
视频输出接口组及子组宏定义
*******************************/
#define OUTPUTGROUPID  0x01
#define OUTPUT_RESOLUTION 0x01
#define OUTPUT_FRAMERATE   0x02
#define OUTPUT_CHECK 0x03

/*******************************
场景组及子组宏定义
*******************************/
#define SCENEGROUPID 		 0x04
#define SCENESWITCH  		 0x01
#define SCENETAKE    		 0x02
#define SCENEGET     		 0x03
#define SCENEFULLPART	 	 0x04
#define SCENECHECKFULLPART	 0x05
#define SCENEP2P     		 0x06
#define SCENECHECKP2P	 	 0x07
#define SCENECHECK			 0x08
#define SCENEADDNEW				 0x09




#define PARA1_IDX		7
#define PARA2_IDX		8
#define PARA3_IDX		9
#define PARA4_IDX		10
#define PARA5_IDX		11
#define PARA6_IDX		12
#define PARA7_IDX		13
#define PARA8_IDX		14
#define PARA9_IDX		15
#define PARA10_IDX		16
#define PARA11_IDX		17
#define PARA12_IDX		18

#define PARA_SubImage1_IDX 8
#define PARA_SubImage2_IDX 17
#define PARA_SubImage3_IDX 26
#define PARA_SubImage4_IDX 35


#define DEFAULT_LEN 4
#define AA_IDX			0
#define BB_IDX			1
#define MESSAGELEN_IDX	2
#define MESSAGEID_IDX		3
#define EVENTGROUPID_IDX	4
#define EVENTID_IDX		5
#define RESERVED_IDX 		6

#define USB_UP_PARA1		0x01
#define USB_DOWN_PARA1	0x02
#define USB_OK_PARA1		0x05
#define USB_BACK_PARA1		0x06
#define USB_EXIT_PARA1		0x07

#define USB_PLAY_PARA1		0x01
#define USB_STOP_PARA1		0x02
#define USB_FASTPALY		0x03
#define USB_FASTBACK		0x05
#define USB_NEXT_PARA1		0x06
#define USB_BEFORE_PARA1	0x07
#define USB_RETURN_PARA1	0x08
#define GET_USB_STATE		0x01
#define USB_STATE_PLAY		0x11
#define USB_STATE_MENU		0x10

#define NO_INPUT_SIGHNAL	0x09
#define YE_INPUT_SIGHNAL	0x0A

#define ORDER_EVENT			0xF0
#define ORDER_OK_P2			0x4F
#define ORDER_OK_P3			0x4B
#define ORDER_FAIL_P2		0x46
#define ORDER_FAIL_P3		0x41


#define DPPARA1          		0x15
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
#define SINGLEIMAGE    1
#define SAMEPIPIMAGE         2
#define PBPIMAGE       4// 3
#define PIPIMAGE    3// 4

#define FUSEPIPPARA1    5// 4


#define FREEZE   1
#define UNFREEZE  0

#define FULL      0
#define PART     1

#define MAIN       1
#define SUB         2


//----------------------------------------------------------------
#define _BIT0     0x01
#define _BIT1     0x02
#define _BIT2     0x04
#define _BIT3     0x08
#define _BIT4     0x10
#define _BIT5     0x20
#define _BIT6     0x40
#define _BIT7     0x80


//OutputFormat
#define INDEX640X480    	       0x0C
#define INDEX800X600    	        0x0B
#define INDEX1024X768   	  0x0A
#define INDEX1280X720   	         0x09
#define INDEX1280X1024  	   0x08
#define INDEX1366X768   	         0x07
#define INDEX1440X900   	         0x06
#define INDEX1600X1200  	   0x05
#define INDEX1680X1050  	     0x04
#define INDEX1920X1080  	     0x03
#define INDEX1920X1200   	0x02
#define INDEX2048x1152  	0x01



#define INDEX60HZ 0x01
#define INDEX50HZ 0x02
#define INDEX30HZ 0x03


typedef enum {
	NO_EVENT = 0,
	
	KEY_MENU=1,
	KEY_UP=2,
	KEY_DOWN=3,
	KEY_ENTER=4,
	KEY_ESC=5,

	TOTAL_EVENT
} EventValue;


typedef enum{	
MUTE= 1,
MAINVOL=2,
SUBVOL=3,
}_AudioPara;

//PIP Postion
typedef enum{
PIP_LT,
PIP_LM ,
PIP_LB ,
PIP_MC,
PIP_RT,
PIP_RM,
PIP_RB,
PIP_NUM
}_PIPPos;



#endif

