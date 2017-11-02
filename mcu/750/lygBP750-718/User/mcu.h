///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <mcu.h>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/

///////////////////////////////////////////////////////////////////////////////
// Include file
///////////////////////////////////////////////////////////////////////////////
#ifndef _MCU_H_
#define _MCU_H_

//#define Debug_message

#ifdef Debug_message
#include <stdio.h>
//#define DEBUG 
#define HDMITX_DEBUG_PRINTF(x) printf x
#else
#define HDMITX_DEBUG_PRINTF(x)
#endif
///////////////////////////////////////////////////////////////////////////////
// Global Definition
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Type Definition
///////////////////////////////////////////////////////////////////////////////
//#include "typedef.h"
///////////////////////////////////////////////////////////////////////////////
// Constant Definition
///////////////////////////////////////////////////////////////////////////////
#define TXDEV           0
#define ExtEDID         1
#define IntEDID         2


#define TXADR           0x98

#define DELAY_TIME      1           // unit = 1 us;
#define IDLE_TIME       100         // unit = 1 ms;

#define HIGH            1
#define LOW             0

#define ACTIVE          1
#define DISABLE         0

#define DevNum          1
#define LOOPMS          20          // 20-> 5 ,  030408, Clive
/*
///////////////////////////////////////////////////////////////////////////////
// 8051 Definition
///////////////////////////////////////////////////////////////////////////////
#ifdef _Myson_CS8953_
sbit SDA_PORT               = P0^1;
sbit SCL_PORT               = P0^0;
sbit Hold_Pin               = P0^2;
#else
sbit SDA_PORT               = P1^1;//P1^1;//1.3
sbit SCL_PORT               = P1^0;//P1^0;//1.2
sbit Hold_Pin               = P1^6;
#endif
*/
#endif      // _MCU_H_
