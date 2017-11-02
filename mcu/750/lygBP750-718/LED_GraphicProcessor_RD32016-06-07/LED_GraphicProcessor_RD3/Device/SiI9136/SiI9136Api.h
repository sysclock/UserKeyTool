#ifndef __SI_I9136_API_H__
#define __SI_I9136_API_H__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      SiI9136Api.h                                              */
/*																			*/
/* USAGE:       Header file for module SiI9233 								*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N S									*/
/****************************************************************************/
#include "defs.h"
#include "TypeDefs.h"
#include "Constants.h"
#include "externals.h"
#include "Macros.h"
#include "TPI_Regs.h"
#include "TPI_Access.h"
#include "TPI.h"
#include "util.h"
#include "AV_Config.h"
#include "si_apiConfigure.h"
#include "SiI9136EDID.h"
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
typedef struct {
	byte txPowerState;		// Can be referenced externally by chip-specific TX HAL file, so cannot be static.
	byte OutputHaveSet;
	bool tmdsPoweredUp;
	bool hdmiCableConnected;
	bool dsRxPoweredUp;
	bool mobileHdCableConnected;
#ifdef MHD_CABLE_HPD
	bool tclkStable;
	bool mobileHdCableConnected;
	bool checkTclkStable;
	byte cbusRevID;
#endif
	
#ifdef DEV_SUPPORT_HDCP
	bool HDCP_TxSupports;
	bool HDCP_AksvValid;
	bool HDCP_Started;
	byte HDCP_LinkProtectionLevel;
#endif	
#ifdef DEV_SUPPORT_EDID
	Type_EDID_Descriptors EDID_Data;
	bool edidDataValid;
#endif
	bool USRX_OutputChange;
	// HDCP Override
	bool g_HDCP_Override;
	VideoModeDescription_t VideoModeDescription;		// Video Mode description block
	int VModeTblIndex;					// Index to Video Mode Table row for our current mode
	byte B_I2CIndex;
	byte B_TX_SLAVE_Addr;
	byte B_TPI_BASE_Addr;
} SiI9136_GlobalStatus_St; 

extern byte txPowerState;		// Can be referenced externally by chip-specific TX HAL file, so cannot be static.
extern byte OutputHaveSet;
extern bool tmdsPoweredUp;
extern bool hdmiCableConnected;
extern bool dsRxPoweredUp;
extern bool mobileHdCableConnected;
#ifdef MHD_CABLE_HPD
extern bool tclkStable;
extern bool mobileHdCableConnected;
extern bool checkTclkStable;
extern byte cbusRevID;
#endif

#ifdef DEV_SUPPORT_HDCP
extern bool HDCP_TxSupports;
extern bool HDCP_AksvValid;
extern bool HDCP_Started;
extern byte HDCP_LinkProtectionLevel;
#endif

#ifdef DEV_SUPPORT_EDID
extern bool edidDataValid;
#endif

extern byte gB_I2CIndex;
extern byte gB_TX_SLAVE_Addr;
extern byte gB_TPI_BASE_Addr;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/

#endif
