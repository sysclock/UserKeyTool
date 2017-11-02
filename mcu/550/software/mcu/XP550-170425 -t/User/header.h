#ifndef HEADER_H
#define HEADER_H
#define TYPE550	//TYPE520
//#define DEBUG_ZOOM_YJL
//#define DEBUG_IR
#define DEBUG_DEMOKEY_FUNC

#ifndef NULL
#define NULL 0
#endif


#include <stdio.h>	
#include <string.h>	
#include "stm32f10x.h"
#include "lcd.h"
#include "Common.h"
#include "adv7441_i2c.h"
#include "Adv7441.h"
#include "keypad.h"
#include "statedata.h"
#include "USART.h"
#include "i2c.h"
#include "date.h"	 
#include "sendmsg.h"
#include "statemachine.h"
#include "strings.h"
#include "menutbl.h"
#include "RTC.h"
#include "delay.h"
#include "web_server.h"
#include "Flash.h"


#include "cat6613.h"
#include "Remote.h"

#define IIC_CONFIG_COMM_TRY_TIME 10
#define TRY_ONE_EIGHT_HOURS_DEMO_KEY 320520720



#endif

