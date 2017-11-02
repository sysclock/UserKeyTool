#ifndef HEADER_H
#define HEADER_H

#define DEBUG88 

//c lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//mcu lib-system
#include "stm32f10x.h"



//user lib-system 
#include "common.h"
#include "date.h"	
#include "delay.h"
#include "strings.h"

//user lib-sub module;HAL
#include "lcd.h"
#include "keypad.h"
#include "USART.h"
#include "i2c.h"
#include "RTC.h"
#include "Flash.h"
#include "Remote.h"

//user lib -color lcd and split data process
#include "display.h"
#include "screen-set.h"
#include "ColorDisplay.h"

//user lib -others


//user lib-func
#include "statedata.h"
#include "sendmsg.h"
#include "statemachine.h"
#include "menutbl.h"

#endif

