/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#ifndef _DEFS_H_
#define _DEFS_H_


void TXHAL_InitMicroGpios (void);
void TXHAL_InitPreReset (void);
void TXHAL_InitPostReset (void);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SiI9136
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define F_9136

#define HALTIMER

// Timers - Target system uses these timers
#define ELAPSED_TIMER               0xFF
#define ELAPSED_TIMER1              0xFE

typedef enum TimerId
{
    TIMER_0 = 0,		// DO NOT USE - reserved for TimerWait()
    TIMER_POLLING,		// Reserved for main polling loop
    TIMER_2,			// Available
    TIMER_3,			// Available
    TIMER_SERIAL,
    TIMER_COUNT			// MUST BE LAST!!!!
} timerId_t;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Project Definitions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define T_MONITORING_PERIOD		5
#define R_INIT_PERIOD			5
#define R_MONITORING_PERIOD		600

#define TX_HW_RESET_PERIOD      200
#define RX_HW_RESET_PERIOD		600

#define INT_CONTROL 0x00 // Interrupt pin is push-pull and active high (this is normally 0x06)
#define SiI_DEVICE_ID           0xB4

#define SiI_DEVICE_STRING       "SiI 9136\n"

#define SCK_SAMPLE_EDGE			0x80

//#define F_9022A_9334            // for now other TPI new features...
//#define F_9334            	// for Evita only features, such as CDC
// HL add
//#define DEV_EMBEDDED				// No Hs Vs and De
#define FORCE_MHD_OUTPUT 		// HL add
//#define MHD_CABLE_HPD				// HL add

#define CLOCK_EDGE_FALLING	1
//#define RX_ONBOARD
//#define DEEP_COLOR
#define I2S_AUDIO

#define DEV_SUPPORT_EDID
#define DEV_SUPPORT_HDCP
//#define DEV_SUPPORT_CEC_FEATURE_ABORT
//#define DEV_SUPPORT_CEC_CONFIG_CPI_0
//#define DEV_SUPPORT_EHDMI

#define DEV_INDEXED_READ
#define DEV_INDEXED_WRITE
#define DEV_INDEXED_RMW

#define HDCP_DONT_CLEAR_BSTATUS

// READKSV and KSVFORWARD defines are mutually exclusive ... 
// if hardware supports KSV forwarding then enable KSVFORWARD in firmware
// otherwise firmware must perform the KSV forwarding by enabling READKSV in firmware
//#define READKSV
#define KSVFORWARD

#ifdef KSVFORWARD
#undef READKSV
#endif

/*\
| | TPI API Version
\*/

#define TPI_PRODUCT_CODE		1

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug Definitions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "stm32f10x.h"
#ifndef DISABLE
	#define DISABLE 0x00
#endif

#ifndef ENABLE
	#define ENABLE  0xFF
#endif

// Compile debug prints inline or not
#define CONF__TPI_DEBUG_PRINT   (DISABLE)			// HL
#define CONF__TPI_EDID_PRINT    (DISABLE)			// HL
#define CONF__TPI_TRACE_PRINT	(DISABLE)				// HL
#define CONF__TPI_CDC_PRINT		(DISABLE)
#define CONF__TPI_TIME_PRINT	(DISABLE)

/*\
| | Trace Print Macro
| |
| | Note: TPI_DEBUG_PRINT Requires double parenthesis
| | Example:  TPI_DEBUG_PRINT(("hello, world!\n"));
\*/
#define DEBUG 0

#if (CONF__TPI_TRACE_PRINT == ENABLE)
    #define TPI_TRACE_PRINT(x)      if (DEBUG == 0) printf x
#else
    #define TPI_TRACE_PRINT(x)
#endif

/*\
| | Debug Print Macro
| |
| | Note: TPI_DEBUG_PRINT Requires double parenthesis
| | Example:  TPI_DEBUG_PRINT(("hello, world!\n"));
\*/

#if (CONF__TPI_DEBUG_PRINT == ENABLE)
    #define TPI_DEBUG_PRINT(x)      if (DEBUG == 0) printf x
#else
    #define TPI_DEBUG_PRINT(x)
#endif

/*\
| | EDID Print Macro
| |
| | Note: To enable EDID description printing, both CONF__TPI_EDID_PRINT and CONF__TPI_DEBUG_PRINT must be enabled
| |
| | Note: TPI_EDID_PRINT Requires double parenthesis
| | Example:  TPI_EDID_PRINT(("hello, world!\n"));
\*/

#if (CONF__TPI_EDID_PRINT == ENABLE)
    #define TPI_EDID_PRINT(x)       if (DEBUG == 0) TPI_DEBUG_PRINT(x)
#else
    #define TPI_EDID_PRINT(x)
#endif


/*\
| | CDC Print Macro
| |
| | Note: To enable CDC description printing, both CONF__TPI_CDC_PRINT and CONF__TPI_DEBUG_PRINT must be enabled
| |
| | Note: TPI_CDC_PRINT Requires double parenthesis
| | Example:  TPI_CDC_PRINT(("hello, world!\n"));
\*/

#if (CONF__TPI_CDC_PRINT == ENABLE)
    #define TPI_CDC_PRINT(x)       if (DEBUG == 0) TPI_DEBUG_PRINT(x)
#else
    #define TPI_CDC_PRINT(x)
#endif


/*\
| | Timer Print Macro
| |
| | Note: To enable Timer description printing, both CONF__TPI_TIME_PRINT and CONF__TPI_DEBUG_PRINT must be enabled
| |
| | Note: TPI_CDC_PRINT Requires double parenthesis
| | Example:  TPI_CDC_PRINT(("hello, world!\n"));
\*/

#if (CONF__TPI_TIME_PRINT == ENABLE)
    #define TPI_TIME_PRINT(x)       if (DEBUG == 0) TPI_DEBUG_PRINT(x)
#else
    #define TPI_TIME_PRINT(x)
#endif


#endif // _DEFS_H_
