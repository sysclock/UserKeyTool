/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "global.h"

typedef unsigned char		bool;
typedef unsigned char		byte;
typedef unsigned int		word;
typedef unsigned long		dword;


typedef enum
{
	SS_FATAL_ERROR,
	SS_RX_NOT_READY,
	SS_RX_NEW_VIDEO_MODE,
	SS_TX_READY
} SYSTEM_STATE;

//------------------------------------------------------------------------------
// Video Mode Description
// This is the common API that passes Video Mode information to the firmware,
// whether the source is the AMF libray or Simon.
//------------------------------------------------------------------------------
typedef struct
{
    unsigned char  VIC;					// VIC OR the HDMI_VIC
    unsigned char  AspectRatio;			// 4x3 or 16x9
    unsigned char  ColorSpace;
    unsigned char  ColorDepth;
    unsigned char  HDCPAuthenticated;
    unsigned char  HDMIVideoFormat;		// VIC, HDMI_VIC or 3D
    unsigned char  ThreeDStructure;		// Valid when (HDMIVideoFormat == VMD_HDMIFORMAT_3D)
	unsigned char  ThreeDExtData;		// Valid when (HDMIVideoFormat == VMD_HDMIFORMAT_3D) && (ThreeDStructure == VMD_3D_SIDEBYSIDEHALF)
} VideoModeDescription_t;

#define VIDEO_SETUP_CMD_LEN     	(9)
#define VIDEO_SYNC_SETUP_CMD_LEN	(20)

#define AUDIO_SETUP_CMD_LEN 		(7)
#define I2S_MAPPING_CMD_LEN     	(4)
#define I2S_INPUT_CFG_CMD_LEN   	(1)
#define I2S_STREAM_HDR_CMD_LEN  	(5)

#define AVI_INFOFRAME_CMD_LEN		(14)
#define AUDIO_INFOFRAME_CMD_LEN		(15)

#define COMM_MODE_CMD_LEN			(1)

#define EHDMI_MODE_CMD_LEN			(1)
#define HDCP_OVERRIDE_MODE_CMD_LEN	(1)

#define PRODUCT_CODE_CMD_LEN		(1)
#define TPI_FW_VERSION_CMD_LEN		((byte)sizeof(TPI_FW_VERSION))
#define CTRL_CMD_VERSION_CMD_LEN	(1)

