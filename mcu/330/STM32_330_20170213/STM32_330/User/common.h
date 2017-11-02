/*======================================================
*	Copyright(c) *	All rights reserved.
*
*	file name:  COMMON.h
*	version:  1.0
*	date: 2011-09-24
*======================================================*/
#ifndef _COMMON_H_
#define _COMMON_H_

#define MSG_LENGTH 10
#define INITIALVALUE_USERPASSWORD 320520
#define DISP_MathineID_6Bit

//----------------不同型号切换--------------------------------
#define XP310 1
#define XP320 2                //6M16
//#define XP330 3                //6M16
#define XP350 4                //6M16
////#define XP360 5          //6M16

#define XP520 6                //FLI32626
#define XP550 7                //FLI32626


#define XP_ID   	  		XP350//XP320// XP310//

//#define TWO_EC11_BOARD
//#define WIFI_MODULE
//---------------------是否显示拓北logo(默认显示)----------
#define DISPLAY_LOGO


#if(XP_ID==XP320)
	#define STM32F103RB       //Arm型号选择(默认为STM32F103C8)
	#define TCP_IP                 //是否使用网口
	#define USE_RTC
#endif

#if(XP_ID==XP350)
	#define USE_RTC
	//#define USE_INFRARED_IR      //使用遥控器
	//#define XP360

	#define XP330
	#define STM32F103RB
	//#define TCP_IP    //此处为ENC28J60模块
#endif

#define LogResolution_Min 1
#define LOGWithoutKEY

#ifdef TWO_EC11_BOARD
	#define STM32F103RB
#endif

#define POWER_ON_PWD

//-------------------------------------------------------------------

#define XP_TO_6M16 1
#define XP_TO_FLI32626 2

#define XP_TO_ID XP_TO_6M16


//-------------------------------------------------------------------

#define SUPPORT_ANYHZ
//-------------------------------------------------------------------


//----------------------是否使用飞梭旋钮--------------------
#ifdef STM32F103RB
   #define EC11_BUTTON
#endif
//-------------------------------------------------------------------

//---------------------------是否使用HDMI----------------------
#define ENABLE_HDMI    1
//-------------------------------------------------------------------

#define INT_DELAY_PC_CTL       //中断延时判断是否上位机控制


#if(XP_ID==XP350)
	#define AB_KEY BACKSPACE_KEY
	
	 #define LOCK_KEY ESC_KEY
	 #define VOLUME_KEY MENU_KEY
#endif

//---------------------------define input source-------------------------------------

#define CVBS_KEY NUM1_KEY
#define CVBS2_KEY NUM2_KEY
#define CVBS3_KEY NUM3_KEY
#define VGA_KEY NUM4_KEY
#define VGA2_KEY NUM5_KEY
#define DVI_KEY NUM6_KEY
#define DVI2_KEY NUM7_KEY
    //#if (ENABLE_HDMI)
	   #define HDMI_KEY NUM8_KEY
	#if(XP_ID==XP350)
	   #define HDMI2_KEY NUM9_KEY
	#endif
   // #endif


#if(XP_TO_ID==XP_TO_FLI32626)
	#define SDI_KEY NUM9_KEY
#endif
//--------------------------------------------------------------------------------

typedef enum
{
    INPUTSOURCE_NONE,
    INPUTSOURCE_CVBS=0x01,   // VIDEO1 - CVBS
    INPUTSOURCE_CVBS2=0x02,   // VIDEO2 - CVBS2
    INPUTSOURCE_CVBS3=0x03,   // VIDEO2 - CVBS3
    INPUTSOURCE_SVIDEO=0x04,  // VIDEO - SVideo

    INPUTSOURCE_YPBPR=0x06,//0x06,  // VIDEO - YPbPr1
    INPUTSOURCE_VGA=0x07,    // PC - VGA
  INPUTSOURCE_VGA2=0x0c,    // PC - VGA2

	INPUTSOURCE_HDMI=0x08,    // PC - HDMI
    INPUTSOURCE_HDMI2=0x0a,    // PC - HDMI2
	INPUTSOURCE_CVBS4=0x09,    // PC - CVBS
  
    INPUTSOURCE_DVI=0x05,    // PC - DVI
    INPUTSOURCE_DVI2=0x0b,    // PC - DVI2
    INPUTSOURCE_SDI=0x0d,	//
    INPUTSOURCE_DP=0x0e,
    INPUTSOURCE_NUM
}_InputSourceType;

#define MSTAR_6M16GS 0
#define MSTAR_6M16JS 1
#define ST_FLI32626     2

////////////////////////////////////////6m16 ////////////////////////////////////////////////////////

#define STM32_CONTROL_MCU MSTAR_6M16JS//MSTAR_6M16GS//

#define INIMAINSRC  INPUTSOURCE_DVI
#define INITPIPSRC  INPUTSOURCE_DVI
//define ENABLE_6M16_CVBS4    //不使用6M16的AV4

   #ifdef STM32F103RB
	#define HOT_BACKUP
   #endif
#define FULLCOUNT 60
  #if(STM32_CONTROL_MCU==MSTAR_6M16JS)
  #define SAVE_COLOR
  #endif
  

#define SINGLEIMAGE    1
#define PIPIMAGE         2
#define PBPIMAGE        3
#define SWAPIMAGE     4
#define MAINFREEZE   5
#define MAINUNFREEZE  6
#define SUBFREEZE      7
#define SUBUNFREEZE   8

#define DEF_SUB_SCALER_HSIZE                 1024/3
#define DEF_SUB_SCALER_VSIZE                768/3
// Note: The value must match to NewProtocol
#define HPOS     1
#define VPOS     2
#define HSIZE    3
#define VSIZE    4
#define INSRC_HZOOM  5
#define INSRC_VZOOM   6
//OutputFormat
#define INDEX640X480    	1
#define INDEX800X600    	2
#define INDEX1024X768   	3
#define INDEX1280X1024  4
#define INDEX1366X768   	5
#define INDEX1440X900   	6
#define INDEX1600X1200  7
#define INDEX1680X1050  8
#define INDEX1920X1080  9
#define INDEX1600X900    10
#define INDEX1280X720  11
#define INDEX1280X768  12
#define INDEX1280X800  13
#define INDEX1280X960    14
#define INDEX1400X1050    15

//ONE BYTE CMD
#define ONEBYTE                           1
#define ONE_BYTE_MASK           0x8f
#define ONEBYTEDATA              0xf0
#define MULTI                           0x10
#define MAININPUT                   0x20
#define SUBINPUT                     0x30
#define VOLUMECONTROL          0x40
#define SINGLEINPUT                0x50
//TWO BYTES CMD
#define TWOBYTES                         2
#define TWO_BYTE_MASK          0xf0
#define SETVOLVALUECMD        0x01
#define GETVOLVALUECMD        0x02
#define SETOUTFORMATCMD     0x03
#define GETOUTFORMATCMD     0x04
#define SETHZCMD     0x05
#define GETHZCMD     0x06
//FOUR BYTES CMD
#define FOURBYTES                        4
#define FOUR_BYTE_MASK             0x07
#define FOUR_BYTE_PAMA_MASK  0xf8
#define SETSCREENCMD                 0x08
#define GETSCREENCMD                 0x10
#define SETMAINSCALERCMD         0x18
#define GETMAINSCALERCMD         0x20
#define SETSUBSCALERCMD        0x28
#define GETSUBSCALERCMD        0x30
#define SETMAINZOOMCMD         0x38
#define GETMAINZOOMCMD         0x40
#define SETSUBZOOMCMD           0x48
#define GETSUBZOOMCMD           0x50
#define SETVGAMAINZOOMCMD   0x58
#define SETVGASUBZOOMCMD     0x60
#define SETDVIMAINZOOMCMD    0x68
#define SETDVISUBZOOMCMD      0x70
#define SETMAINCOLORCMD        0x78
#define GETMAINCOLORCMD        0x80
#define SETSUBCOLORCMD           0x88
#define GETSUBCOLORCMD           0x90
#define SETHDMIMAINZOOMCMD  0x98
#define SETHDMISUBZOOMCMD     0xA0
////////////////////////////////////6m16 ///////////////////////////////////////////////////////////////



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
  #ifdef TWO_EC11_BOARD
  #define BMA2_GPIO GPIOB
  #define BMA2_PIN GPIO_Pin_1
  #define BMB2_GPIO GPIOB
  #define BMB2_PIN GPIO_Pin_2
  #define BMC2_GPIO GPIOB
  #define BMC2_PIN GPIO_Pin_0
  #define BMA2 GPIO_ReadInputDataBit(BMA2_GPIO, BMA2_PIN)
  #define BMB2 GPIO_ReadInputDataBit(BMB2_GPIO, BMB2_PIN) 
  #define BMC2 GPIO_ReadInputDataBit(BMC2_GPIO, BMC2_PIN)
  #endif
#endif
////////////////////////////////是否使用飞梭旋钮//////////////////////////////////////////////
////////////////////////////////声音通道切换，vga切换及LED灯///////////////////////////////
#ifdef STM32F103RB
#ifndef TWO_EC11_BOARD
#define PC_AUD_SEL0_GPIO GPIOC
#define PC_AUD_SEL0_PIN GPIO_Pin_0
#define PC_AUD_SEL1_GPIO GPIOC
#define PC_AUD_SEL1_PIN GPIO_Pin_1

#define LED_LAMP_GREEN_GPIO GPIOB
#define LED_LAMP_GREEN_PIN GPIO_Pin_1
#define VGA_SEL_H_GPIO GPIOC
#define VGA_SEL_H_PIN GPIO_Pin_11
#define DVI_SEL_GPIO GPIOD
#define DVI_SEL_PIN GPIO_Pin_5
#define HDMI_SEL_GPIO GPIOD
#define HDMI_SEL_PIN GPIO_Pin_4
#else
#define DVI_SEL_GPIO GPIOD
#define DVI_SEL_PIN GPIO_Pin_5
#define HDMI_SEL_GPIO GPIOD
#define HDMI_SEL_PIN GPIO_Pin_4
#endif
#define VGA_SEL_GPIO GPIOD
#define VGA_SEL_PIN GPIO_Pin_6

#define PC_POWER_ON_GPIO GPIOB
#define PC_POWER_ON_PIN GPIO_Pin_5


//#define MAIN_MUTE_GPIO GPIOB
//#define MAIN_MUTE_PIN GPIO_Pin_8

#else	//// not #ifdef STM32F103RB 
//#define HDMI_SEL_GPIO GPIOB
//#define HDMI_SEL_PIN GPIO_Pin_10

#define PC_AUD_SEL0_GPIO GPIOB
#define PC_AUD_SEL0_PIN GPIO_Pin_0
#define PC_AUD_SEL1_GPIO GPIOB
#define PC_AUD_SEL1_PIN GPIO_Pin_1

#if(XP_ID==XP310)
#define LED_LAMP_GREEN_GPIO GPIOA
#define LED_LAMP_GREEN_PIN GPIO_Pin_7
#else
#define LED_LAMP_GREEN_GPIO GPIOB
#define LED_LAMP_GREEN_PIN GPIO_Pin_5
#endif

#define VGA_SEL_GPIO GPIOC
#define VGA_SEL_PIN GPIO_Pin_13
#define VGA_SEL_H_GPIO GPIOA
#define VGA_SEL_H_PIN GPIO_Pin_0

//#define MAIN_MUTE_GPIO GPIOB
//#define MAIN_MUTE_PIN GPIO_Pin_3
#endif

#define PC_AUD_SEL0_L GPIO_ResetBits(PC_AUD_SEL0_GPIO,PC_AUD_SEL0_PIN)
#define PC_AUD_SEL0_H GPIO_SetBits(PC_AUD_SEL0_GPIO,PC_AUD_SEL0_PIN)
#define PC_AUD_SEL1_L GPIO_ResetBits(PC_AUD_SEL1_GPIO,PC_AUD_SEL1_PIN)
#define PC_AUD_SEL1_H GPIO_SetBits(PC_AUD_SEL1_GPIO,PC_AUD_SEL1_PIN)

#define VGA_SEL_L GPIO_ResetBits(VGA_SEL_GPIO,VGA_SEL_PIN)
#define VGA_SEL_H GPIO_SetBits(VGA_SEL_GPIO,VGA_SEL_PIN)
////////////////////////////////声音通道切换，vga切换及LED灯///////////////////////////////

///////////////////////////////////////////////////DVI及HDMI切换////////////////////////////////////
#ifdef STM32F103RB
#define PC_SEL_DVI_L GPIO_ResetBits(DVI_SEL_GPIO,DVI_SEL_PIN)
#define PC_SEL_DVI_H GPIO_SetBits(DVI_SEL_GPIO,DVI_SEL_PIN)

#define PC_SEL_HDMI_L GPIO_ResetBits(HDMI_SEL_GPIO,HDMI_SEL_PIN)
#define PC_SEL_HDMI_H GPIO_SetBits(HDMI_SEL_GPIO,HDMI_SEL_PIN)
#endif

///////////////////////////////////////////////////DVI及HDMI切换////////////////////////////////////

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
///////////////////////////////////////////按键盘的LED灯驱动//////////////////////////////////////////

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
#define KB_SCK_GPIO GPIOE
#define KB_SCK_PIN GPIO_Pin_0
#define KB_SDOUT_GPIO GPIOE
#define KB_SDOUT_PIN GPIO_Pin_1
#define LCD_BK_GPIO GPIOC
#define LCD_BK_PIN GPIO_Pin_9

 #else
#define LCD_CS_GPIO GPIOA
#define LCD_CS_PIN GPIO_Pin_11
#define LCD_CLK_GPIO GPIOA
#define LCD_CLK_PIN GPIO_Pin_12
#define LCD_DAT_GPIO GPIOB
#define LCD_DAT_PIN GPIO_Pin_6
#define KB_PLOAD_GPIO GPIOB
#define KB_PLOAD_PIN GPIO_Pin_2
#define KB_SCK_GPIO GPIOB
#define KB_SCK_PIN GPIO_Pin_12
#define KB_SDOUT_GPIO GPIOB
#define KB_SDOUT_PIN GPIO_Pin_13
#endif


#define OffBackLight	1
#define UNOffBackLight 2


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


///////////////////////////////////STM32 I2C Config//////////////////////////////////////////////////////////
#define ENABLE_SOFTI2C   1//0
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
////////////////////////////////////STM32 I2C Config////////////////////////////////////////////////////////


/////////////////////////////////////////schedule macro///////////////////////////////////////////////////
#define ENABLE_SCHEDULE_MODE 0   //模式时间计划不用

#define ONCESCHE       0
#define MORESCHE      1
#define INSRCOVER      1
#define INSRCINI       0
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENGLISH  1
#define CHINESE  2
#define LOCKKEYPAD           1
#define UNLOCKKEYPAD       2
#define LOGNTIME 15

#define FREEZE   1
#define UNFREEZE  0

#define FULL      0
#define PART     1
#define MAIN       1
#define SUB         2

#define _BIT0     0x01
#define _BIT1     0x02
#define _BIT2     0x04
#define _BIT3     0x08
#define _BIT4     0x10
#define _BIT5     0x20
#define _BIT6     0x40
#define _BIT7     0x80


#define DEF_SCREEN_HSIZE                 1024
#define DEF_SCREEN_VSIZE                768
#define DEF_SCREEN_HPOS                  0
#define DEF_SCREEN_VPOS                   0


#define DEF_MAIN_SCALER_HSIZE                1024
#define DEF_MAIN_SCALER_VSIZE                768
#define DEF_MAIN_SCALER_HPOS                   0
#define DEF_MAIN_SCALER_VPOS                   0
#define DEF_SUB_SCALER_HPOS                    0
#define DEF_SUB_SCALER_VPOS                    0


#define SELIMGADDR   0x97
#define STEPADDR       0x87

#define MODE1      1
#define MODE2      2
#define MODE3      3
#define MODE4      4
#define MODE5      5
#define MODE6      6
#define MODE7      7
#define MODE8      8
#define MODE9      9
#define MODE0      0


//COLOR
#define CONTRAST 0
#define BRIGHTNESSR    1
#define BRIGHTNESSG    2
#define BRIGHTNESSB    3
#define CONTRASTR        4
#define CONTRASTG       5
#define CONTRASTB       6
#define BRIGHTNESS    7

typedef enum
{
    Communication_NONE,
    Communication_USART1=0x01,   // 串口1
    Communication_USART2=0x02,   // 串口2
    Communication_USART3=0x03,   // 串口3
    Communication_Ethernet=0x04,  // 以太网
    Communication_NUM
}_CommunicationType;

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

#define DEF_BRIGHTNESS      50
#define DEF_CONTRACT          50
#define DEF_SATURATION      50
#endif
