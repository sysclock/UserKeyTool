#ifndef HEADER_H
#define HEADER_H
#include <string.h>
#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32_eval.h"




#include "lcd.h"
#include "common.h"
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
#include "XP520.h"
#include "Flash.h"
#include "MenuTbl.h"
#include "remote.h"
#include "spi_enc28j60.h"
#include "Queue.h"

#include "UserDef.h"

#include "../System Code/type.h"
#include "../System Code/Serial.h"
#include "../System Code/global.h"
#include "../System Code/Timer.h"



#include "../MultiSignalBoard/Init.h"
#include "../MultiSignalBoard/Param.h"
#include "../MultiSignalBoard/InputBoardGraphicFPGA.h"
#include "../MultiSignalBoard/InputBoardADV7604.h"
#include "../MultiSignalBoard/InputBoardGV7601.h"
#include "../MultiSignalBoard/InputBoardTVP5158.h"
#include "../MultiSignalBoard/OutputBoardSiI9136.h"

#include "../Device/AD9388A/AD9388A.h"
#include "../Device/InputFPGA/InputFPGA.h"

#include "../User Command/UserCommandPC_API.h"

#include "../WindowsLayout/ScreenWindowLayout.h"





#endif
