/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

extern VideoModeDescription_t VideoModeDescription;
extern int VModeTblIndex;

extern bool CommModeState;
#define NOT_IN_COMM_MODE	0
#define IN_COMM_MODE		1

// Patches
//========
extern byte EmbeddedSynPATCH;

//UART
//====
extern byte TXBusy;

extern byte IDX_InChar;
extern byte NumOfArgs;
extern byte MessageType;
extern bool F_SBUF_DataReady;
extern bool F_CollectingData;

extern bool USRX_OutputChange;

// Simulation Configuration
extern bool g_EDID_Simulation;
extern bool g_Video_Mode_Simulation;

extern byte g_audio_Checksum;	// Audio checksum

// Communication
extern byte g_CommData[];
     
// HDCP Override
extern bool g_HDCP_Override;            
