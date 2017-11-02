//------------------------------------------------------------------------------
// Copyright © 2007, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
#ifndef __AMF_H__
#define __AMF_H__
        
//-------------------------------------------------------------------------------
// Debug Include
//-------------------------------------------------------------------------------
#include <stdio.h>      //for printf and putchar
#include "infofrm.h"

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

// Audio states
#define STATE_AUDIO__MUTED          0
#define STATE_AUDIO__REQUEST_AUDIO  1
#define STATE_AUDIO__AUDIO_READY    2
#define STATE_AUDIO__ON             3

// Video states
#define STATE_VIDEO__MUTED          1
#define STATE_VIDEO__UNMUTE			2
#define STATE_VIDEO__ON             3
#if (PEBBLES_VIDEO_STATUS_2ND_CHECK==ENABLE)
#define STATE_VIDEO__CHECKED        4
#endif


//Audio modes
#define AUDIO_MODE__PCM			0
#define AUDIO_MODE__DSD			1	
#define AUDIO_MODE__HBR			2
#define AUDIO_MODE__NOT_INIT	0x0F
// On, Off
#define ON      1
#define OFF     0

// Set, Clear
#define SET     0xFF
#define CLEAR   0x00

//-------------------------------------------------------------------------------
// Port Select Switch Mask Bits
//-------------------------------------------------------------------------------
#define PORT_SELECT__PORT_0     (0x07)	 //Pebbles starter ES0
#define PORT_SELECT__PORT_1     (0x06)
#define PORT_SELECT__PORT_2     (0x05)
#define PORT_SELECT__PORT_3     (0x04)
#define PORT_SELECT__PORT_7     (0x00)

#define PORT_SELECT__NO_CHANGE  (0xFF)

//-------------------------------------------------------------------------------
// Debug Print Macro 
//   Note: DEBUG_PRINT Requires double parenthesis
//   Example:  DEBUG_PRINT(("hello, world!\n"));
//-------------------------------------------------------------------------------
#if (CONF__DEBUG_PRINT == ENABLE)
//    #define DEBUG_PRINT(x)      printf x
    #define DEBUG_PRINT(x)

#else
    #define DEBUG_PRINT(x)
#endif

//------------------------------------------------------------------------------
// Global Status Structure
//------------------------------------------------------------------------------
typedef struct {
    uint8_t AudioState;
    uint8_t ColorDepth;
	uint8_t VideoState;
//	uint8_t SYNC_StateDetect;
//	uint8_t	PrintMessageFlag;
	uint8_t AudioMode;   //101 added for handling HBR
    uint8_t PortSelection;
	uint8_t ResolutionChangeCount;	   //for zone workaround
	uint8_t VideoStabilityCheckCount; //for ODCK disappear workaround
	uint8_t BoardIndex;
	uint8_t ChipIndex;
	uint16_t StatusAudioTmr;
	uint16_t StatusVideoTmr;
	uint16_t wOldTickCnt;
#if (CONF__SUPPORT_3D == ENABLE)
	IfCtrl_t IfCtrl;
#endif
	uint16_t VideoHSize;
	uint16_t VideoVSize;
} GlobalStatus_t; 
/*
enum
{									//       1ms timer
	SiI_9233_AUDIO_TMR,			// SiI9233 Audio timer
	SiI_9233_VIDEO_TMR			// SiI9233 Video timer
};
*/
//------------------------------------------------------------------------------
// Global status variable
//------------------------------------------------------------------------------
extern GlobalStatus_t CurrentStatus;



//------------------------------------------------------------------------------
// Function Prototypes for mainline code
//------------------------------------------------------------------------------
void SystemDataReset(void);



//------------------------------------------------------------------------------
// Function Prototypes for interrupt handler routines
//------------------------------------------------------------------------------
void SetupInterruptMasks(void);
void AudioUnmuteHandler(void);
void TurnAudioMute (uint8_t qOn);
void TurnVideoMute(uint8_t qOn);
void TurnPowerDown(uint8_t qOn);
void PollInterrupt(void);
void ProgramEDID(void);
uint8_t ResetVideoControl(void);
#if (PEBBLES_VIDEO_STATUS_2ND_CHECK==ENABLE)
uint8_t PclkCheck(void);
#endif
void SiI9233Init(void);
void SiI9233_TimerTask(void);
void HdmiTask(void);
void PollPortSwitch(void);
void SiI9233_TimerStart(uint8_t B_Timer, uint16_t W_value);
void SiI9233_TimerStop(uint8_t B_Timer);
uint8_t SiI9233_TimerCheck(uint8_t B_Timer);
//------------------------------------------------------------------------------
// Function Prototypes for register access routines
//------------------------------------------------------------------------------
uint8_t RegisterRead(uint16_t regAddr);
void RegisterWrite(uint16_t regAddr, uint8_t value);
void RegisterModify(uint16_t regAddr, uint8_t mask, uint8_t value);
void RegisterBitToggle(uint16_t regAddr, uint8_t mask);

void RegisterReadBlock(uint16_t regAddr, uint8_t* buffer, uint8_t length);
void RegisterWriteBlock(uint16_t regAddr, uint8_t* buffer, uint8_t length);




#endif  // __AMF_H__
