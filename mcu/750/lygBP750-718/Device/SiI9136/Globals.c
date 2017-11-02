/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#include "defs.h"
#include "TypeDefs.h"
#include "Constants.h"
#include "si_apiConfigure.h"

VideoModeDescription_t VideoModeDescription;		// Video Mode description block
int VModeTblIndex;															// Index to Video Mode Table row for our current mode

bool CommModeState;

// Patches
//========
byte EmbeddedSynPATCH;


//UART
//====
byte TXBusy;

byte IDX_InChar;
byte NumOfArgs;
byte MessageType;
bool F_SBUF_DataReady;
bool F_CollectingData;

bool USRX_OutputChange;


// Simulation Configuration
bool g_EDID_Simulation;
bool g_Video_Mode_Simulation;

// Checksums
byte g_audio_Checksum;	// Audio checksum

// Serial Communication Buffer
byte g_CommData [GLOBAL_BYTE_BUF_BLOCK_SIZE];

// HDCP Override
bool g_HDCP_Override;
