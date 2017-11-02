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
//#define NO_LOGO

#define XP_520 1                //不带输入板	
#define XP_530 2                //带输入板
#define MATHINE_TYPE   XP_520		//XP_520 //
#define START_QUICK_SWITCH

#if(MATHINE_TYPE==XP_530)
#define ADV7441A_INPUT 3//0
#define SII9125_INPUT 1
#define SDI_INPUT 2
#define INPUT_BOARD ADV7441A_INPUT//SII9125_INPUT//SDI_INPUT//
#endif

//----------------------是否使用飞梭旋钮--------------------
#ifdef STM32F103RB
//	#define TCP_IP
   	#define EC11_BUTTON
	
#endif

#define USE_INFRARED_IR      //使用遥控器


//-------------------------------------------------------------------
#define POWER_ON_PWD

//-------------------------------------------------------------------------

#define INT_DELAY_PC_CTL       //中断延时判断是否上位机控制

//---------------------------define input source-------------------------------------
#if(MATHINE_TYPE==XP_530)						
#define CVBS_KEY   NUM1_KEY
#define CVBS2_KEY NUM2_KEY
#define VGA_KEY    NUM3_KEY
#define DVI_KEY     NUM4_KEY
#define HDMI_KEY  NUM5_KEY
#define DUAL_KEY SAVE_KEY
#elif(MATHINE_TYPE==XP_520)							
#define CVBS_KEY NUM1_KEY
#define CVBS2_KEY NUM2_KEY

//#define CVBS3_KEY NUM3_KEY
#define DVI_KEY NUM4_KEY
#define VGA_KEY NUM3_KEY
#define HDMI_KEY NUM5_KEY
#define DUAL_KEY SAVE_KEY

#define CVBS3_KEY NUM6_KEY
#define VGA2_KEY NUM7_KEY
#define DVI2_KEY NUM8_KEY
#define USB_KEY NUM9_KEY



#endif

#define DEBUG_KEY	SWAP_KEY

#define LogResolution_Min 1
#define LOG_Without_AuthorizationKEY



#ifdef KEY_REVERSE
	#define LOCK_KEY ESC_KEY
#else
	#define LOCK_KEY ESC_KEY//BACKSPACE_KEY
#endif
//--------------------------------------------------------------------------------

typedef enum
{
    INPUTSOURCE_NONE,
    INPUTSOURCE_CVBS,   // VIDEO1 - CVBS
    INPUTSOURCE_CVBS2,   // VIDEO2 - CVBS2
    INPUTSOURCE_CVBS3,   // VIDEO2 - CVBS2
    INPUTSOURCE_CVBS4,   // VIDEO2 - CVBS2
    INPUTSOURCE_DVI,    // PC - DVI
		INPUTSOURCE_DVI2,    // PC - DVI
    INPUTSOURCE_YPBPR,  // VIDEO - YPbPr1
    INPUTSOURCE_VGA,    // PC - VGA
    INPUTSOURCE_VGA2,    // PC - VGA
    INPUTSOURCE_HDMI,    // PC - DVI
    INPUTSOURCE_SDI,
	  INPUTSOURCE_USB,
    INPUTSOURCE_DP,
    INPUTSOURCE_NUM
}_InputSourceType;


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
  /*
  #define BMA_GPIO GPIOA
  #define BMA_PIN GPIO_Pin_1
  #define BMB_GPIO GPIOA
  #define BMB_PIN GPIO_Pin_0
  #define BMC_GPIO GPIOC
  #define BMC_PIN GPIO_Pin_6
    */
  #define BMA GPIO_ReadInputDataBit(BMA_GPIO, BMA_PIN)
  #define BMB GPIO_ReadInputDataBit(BMB_GPIO, BMB_PIN) 
  #define BMC GPIO_ReadInputDataBit(BMC_GPIO, BMC_PIN)
#endif
////////////////////////////////是否使用飞梭旋钮//////////////////////////////////////////////

////////////////////////////////声音通道切换，vga切换及LED灯///////////////////////////////
#ifdef STM32F103RB
#ifdef EC11_BUTTON
#define VGA_SEL_GPIO GPIOC
#define VGA_SEL_PIN GPIO_Pin_12
#else
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

#define IO_UART_TX_GPIO GPIOC
#define IO_UART_TX_PIN GPIO_Pin_11
#define IO_UART_RX_GPIO GPIOC
#define IO_UART_RX_PIN GPIO_Pin_12

#define IO_UART_TX_L GPIO_ResetBits(IO_UART_TX_GPIO,IO_UART_TX_PIN)
#define IO_UART_TX_H GPIO_SetBits(IO_UART_TX_GPIO,IO_UART_TX_PIN)
#define IO_UART_RX GPIO_ReadInputDataBit(IO_UART_RX_GPIO, IO_UART_RX_PIN)
*/
#define MAIN_MUTE_GPIO GPIOB
#define MAIN_MUTE_PIN GPIO_Pin_8
#endif

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
#ifdef EC11_BUTTON
#define PC_SEL_HDMI_GPIO GPIOC
#define PC_SEL_HDMI_PIN GPIO_Pin_10

#define PC_SEL_DVI_GPIO GPIOC
#define PC_SEL_DVI_PIN GPIO_Pin_11
#else

#define PC_SEL_HDMI_GPIO GPIOC
#define PC_SEL_HDMI_PIN GPIO_Pin_8

#define PC_SEL_DVI_GPIO GPIOC
#define PC_SEL_DVI_PIN GPIO_Pin_7
#endif

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
#ifdef EC11_BUTTON
	#define LCD_CS_GPIO GPIOB//GPIOC
	#define LCD_CS_PIN GPIO_Pin_13//GPIO_Pin_0
	#define LCD_CLK_GPIO GPIOB//GPIOB
	#define LCD_CLK_PIN GPIO_Pin_15//GPIO_Pin_15
	#define LCD_DAT_GPIO GPIOB//GPIOB
	#define LCD_DAT_PIN GPIO_Pin_14//GPIO_Pin_13

	#define KB_SDOUT_GPIO GPIOE
	#define KB_SDOUT_PIN GPIO_Pin_1
#else
	#define LCD_CS_GPIO GPIOC
	#define LCD_CS_PIN GPIO_Pin_1
	#define LCD_CLK_GPIO GPIOC
	#define LCD_CLK_PIN GPIO_Pin_0
	#define LCD_DAT_GPIO GPIOC
	#define LCD_DAT_PIN GPIO_Pin_3
	#define KB_SDOUT_GPIO GPIOB
	#define KB_SDOUT_PIN GPIO_Pin_13
#endif
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
	#define KB_SDOUT_GPIO GPIOB
	#define KB_SDOUT_PIN GPIO_Pin_13
#endif	
	#define KB_SCK_GPIO GPIOE
	#define KB_SCK_PIN GPIO_Pin_0

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
#ifdef EC11_BUTTON
#define LED_SRCK_GPIO GPIOE
#define LED_SRCK_PIN GPIO_Pin_3
#else
#define LED_SRCK_GPIO GPIOB
#define LED_SRCK_PIN GPIO_Pin_15
#endif
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

#define ENABLE_SCHEDULE_MODE 0

#define LOCKKEYPAD 1
#define UNLOCKKEYPAD 2

#define	 INPUTKEYNUMS     7
#define FULLCOUNT 60
#define LOGNTIME 40//100


#define INIMAINSRC  INPUTSOURCE_CVBS      //INPUTSOURCE_DVI
#define INITPIPSRC  INPUTSOURCE_VGA//7-8
//---------------------------------------------------------------------------------------------------
//-------0xAA 0xBB 0x02 0x00 [EventID] [Para1] [Para2] [Para3] [Para4] [Para5] [Para6] [Checksum]--------------
//----------------------------------12 bytes Protocol--------------------------------------------------
#define BRIGHTNESS_EVENT         0x03
#define CONTRAST_EVENT            0x04
#define SATURATION_EVENT        0x05
#define RESET_COLOR_EVENT       0x06
#define ADC_CALIBRATION_EVENT   0x07

#define MAIN_SRC_EVENT             0x08
#define MENU_LOGO_EVENT          0x0F
#define FACTORY_RESET_EVENT   0x10
#define SUB_SRC_EVENT               0x09
#define AUDIO_EVENT               0x0A
#define SWAP_EVENT                   0x59
#define OUT_FORMAT_EVENT        0x51
#define MULTI_EVENT                   0x52
#define MAIN_SCALER_EVENT       0x53
#define SUB_SCALER_EVENT         0x54
#define MAIN_ZOOM_EVENT          0x25
#define SUB_ZOOM_EVENT            0x26
#define MAIN_COLOR_EVENT        0x27
#define SUB_COLOR_EVENT           0x28
#define RETURN_EVENT                 0x21
#define IN_SRC_FORMAT_EVENT   0x31
#define COLORRGB                          0x0D
#define IMAGE_ONTOP_EVENT    0x5C

#define USB_UPGRADE_EVENT		0x30
#define USB_DIRECT_EVENT		0x31
#define USB_PLAY_EVENT			0x32
#define USB_STATE_EVENT			0x3F
#define START_STATE_EVENT		0x21

#define AA_IDX                   0
#define BB_IDX                    1
#define DEVICEID_IDX        2
#define BOARDID_IDX         3
#define EVENTID_IDX         4
#define PARA1_IDX            5
#define PARA2_IDX            6
#define PARA3_IDX            7
#define PARA4_IDX            8
#define PARA5_IDX            9
#define PARA6_IDX            10
#define CHECKSUM_IDX      11

#define AV1PARA1        0x0B
#define AV2PARA1        0x0C
#define AV3PARA1        0x0B
#define AV4PARA1        0x03
#define VGAPARA1        0x02
#define YPBPRPARA1     0x05
#define DVIPARA1         0x08
#define DVI2PARA1         0x09
#define SDIPARA1         0x07
#define HDMIPARA1      0x06
#define USBPARA1      0x0E
#define DPPARA1          0x15

#define USBPARA1	  	0x0E
#define USB2PARA1	  	0x0F

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


//define as 0 and 1 is better
#define MAIN       1
#define SUB         2

//-----------------------------schedule macro-----------------------

#define ONCESCHE       0
#define MORESCHE      1
#define INSRCOVER      1
#define INSRCINI       0

//----------------------------------------------------------------
#define _BIT0     0x01
#define _BIT1     0x02
#define _BIT2     0x04
#define _BIT3     0x08
#define _BIT4     0x10
#define _BIT5     0x20
#define _BIT6     0x40
#define _BIT7     0x80

#define DEF_BRIGHTNESS      50
#define DEF_CONTRACT      50
#define DEF_SATURATION      50

#define DEF_OFFSET_RED      32      //(1)
#define DEF_OFFSET_GREEN      29   //(-2)
#define DEF_OFFSET_BLUE      36      //(5)

#define DEF_GAIN_RED           231
#define DEF_GAIN_GREEN      230
#define DEF_GAIN_BLUE         212

#define DEF_USR_RESOLUTION_HSIZE 3840
#define DEF_USR_RESOLUTION_VSIZE 640

#define DEF_SCREEN_HSIZE                 1024
#define DEF_SCREEN_VSIZE                768
#define DEF_SCREEN_HPOS                  0
#define DEF_SCREEN_VPOS                   0


#define DEF_MAIN_SCALER_HSIZE               1024
#define DEF_MAIN_SCALER_VSIZE                768
#define DEF_MAIN_SCALER_HPOS                   0
#define DEF_MAIN_SCALER_VPOS                   0
#define DEF_SUB_SCALER_HSIZE                 1024
#define DEF_SUB_SCALER_VSIZE                768
#define DEF_SUB_SCALER_HPOS                    0
#define DEF_SUB_SCALER_VPOS                    0
// Note: The value must match to NewProtocol
#define HSIZE    1
#define VSIZE    2
#define HPOS    3
#define VPOS     4
#define HVSIZE  5
#define HVPOS   6
#define VGA_HVSIZE  11
#define VGA_HVPOS   12
#define DVI_HVSIZE  13
#define DVI_HVPOS   14
#define HDMI_HVSIZE  15
#define HDMI_HVPOS   16
#define OTHER_HVSIZE  17
#define OTHER_HVPOS   18

#define SELIMGADDR   0x97
#define STEPADDR       0x87
#define INFORADDR 0x81
///
#define MODE0      0
#define MODE1      1
#define MODE2      2
#define MODE3      3
#define MODE4      4
#define MODE5      5
#define MODE6      6
#define MODE7      7
#define MODE8      8
#define MODE9      9
#define MODE10     10
#define DEFAULT_MODE MODE0 

//OutputFormat
#define INDEX640X480    	       0x0E
#define INDEX800X600    	        0x0D
#define INDEX1024X768   	  0x0C
#define INDEX1024X768X75   	  0x0B
#define INDEX1280X720   	         0x0A
#define INDEX1280X1024  	   0x09
#define INDEX1366X768   	         0x08
#define INDEX1440X900   	         0x07
#define INDEX1600X1200  	   0x06
#define INDEX1680X1050  	     0x05
#define INDEX1920X1080  	     0x03
#define INDEX1920X1200   	0x02
#define INDEX2048x1152  	0x01

#define CMDLENTH   12
/*
//ONE BYTE CMD
#define ONEBYTE                           1
#define ONEBYTECMD               0x8f
#define ONEBYTEDATA             0xf0

#define MULTI                           0x10
#define MAININPUT                   0x20
#define SUBINPUT                     0x30
#define VOLUMECONTROL          0x40
#define SINGLEINPUT                0x50

//TWO BYTES CMD
#define TWOBYTES                         2
#define TWOBYTESCMD             0xf0

#define SETVOLVALUECMD        0x01
#define GETVOLVALUECMD        0x02
#define SETOUTFORMATCMD     0x03
#define GETOUTFORMATCMD     0x04


//FOUR BYTES CMD
#define FOURBYTES                        4
#define FOURBYTESCMD            0x07
#define FOURBYTESPARA          0xf8

#define SETSCREENCMD              0x08
#define GETSCREENCMD              0x10
#define SETMAINSCALERCMD      0x18
#define GETMAINSCALERCMD     0x20
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
#define SETMAINCOLORCMD         0x78
#define GETMAINCOLORCMD         0x80
#define SETSUBCOLORCMD           0x88
#define GETSUBCOLORCMD           0x90
*/

//-------------------Default Value-----------------------------------
//#define QUEUEFULL 1
//#define QUEUEEMPTY 0



//-----------------------------------------------------------------





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



void Check_TryDemoMachine_andEchoHumanOperation(unsigned short n);

#endif

