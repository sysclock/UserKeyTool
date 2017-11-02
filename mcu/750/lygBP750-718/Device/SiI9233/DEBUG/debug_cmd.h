//------------------------------------------------------------------------------
// Copyright © 2007, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
#ifndef __DEBUG_CMD_H__
#define __DEBUG_CMD_H__



//-------------------------------------------------------------------------------
// Debug Command Handler Polling Macro
//-------------------------------------------------------------------------------
#if (CONF__DEBUG_CMD_HANDLER == ENABLE)
    #define DEBUG_INIT()             DEBUG_PauseFirmware = 0
    #define DEBUG_PAUSE_FIRMWARE    (DEBUG_PauseFirmware != 0)
    #define DEBUG_POLL()             DEBUG_PollHandler()
#else
    #define DEBUG_INIT()
    #define DEBUG_PAUSE_FIRMWARE    (0)
    #define DEBUG_POLL()
#endif



//-------------------------------------------------------------------------------
// Global debug variable
//-------------------------------------------------------------------------------
extern uint8_t DEBUG_PauseFirmware;



//-------------------------------------------------------------------------------
// Function Prototypes
//-------------------------------------------------------------------------------
void DEBUG_PollHandler(void);
void DEBUG_HandleReceivedChar(uint8_t achar);



#endif  // __DEBUG_CMD_H__
