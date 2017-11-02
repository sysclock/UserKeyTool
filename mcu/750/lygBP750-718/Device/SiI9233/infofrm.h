/******************************************************************************
 *
 * Copyright 2008, Silicon Image, Inc.  All rights reserved.  
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of: Silicon Image, Inc., 1060
 * East Arques Avenue, Sunnyvale, California 94085
 * 
 *****************************************************************************/
/**
 * @file infofrm.h
 *
 * This is a description of the file. 
 *
 * $Author: $Vladimir Grekhov
 * $Rev: $
 * $Date: $9-15-09
 *
 *****************************************************************************/

#ifndef _INFOFRM_H
#define _INFOFRM_H

#include <stdio.h>
#include <string.h>
#include "../../System Code/global.h"
//#include <local_types.h>
#include "spec_types.h"

/***** macro definitions *****************************************************/
#if (CONF__SUPPORT_3D == ENABLE)

/***** local macro definitions ***********************************************/
#define SPD_CAPTURE_TO          1250
#define VSIF_LONG_CAPTURE_TO    1300
#define ISRC1_CAPTURE_TO        500
#define TWO_VIDFR_CAPTURE_TO    300//150//36
#define SPD_ISRC1_CAPTURE_TO    500
#define TIME_WO_ACP_IF          2000//600
#define ACP_CAPTURE_200MS       600//200
#define MAX_NUM_IF              3

#define IFBUFF_SIZE 32

#define SET_CLR_IF() (RegisterWrite( REG__INFM_CLR, 0x7F)) //clear verything
#define SET_PACKET_INTR_MASK() (RegisterWrite( REG__INTR3_UNMASK, 0x0B))
//0x0B =  BIT__NEW_AVI_INF|BIT__NEW_SPD_INF|BIT__NEW_MPEG_INF) 

/***** public type definitions ***********************************************/
/***** local type definitions ************************************************/
typedef uint16_t DevAddr_t;

typedef struct _IfInsCtrl_t {
    uint16_t    wTo;
    uint8_t     bBuffType;
    bool_t      fReceived;
    bool_t      fFailed;
} IfInsCtrl_t;

typedef enum {
    SelVsif
    ,SelIsrc1
    ,SelIsrc2
    ,SelSpd
    ,SelAcp
    ,MaxNumIfSel
} SelectIf_t;


typedef struct _IfCtrl_t {

    IfInsCtrl_t Ins[MaxNumIfSel];
    uint8_t     abBuff[IFBUFF_SIZE];
    uint8_t     abVsif[IF_VSIF_SIZE];
    uint8_t     abAcp[IF_ACP_SIZE];
    uint16_t    wNoAcpTotTime;
    uint8_t     bDecodeTypeMpegBuffer;
    uint8_t     bDecodeTypeSpdBuffer;
    uint8_t     bVidModeId; /* Video Mode Id aquired from Avi */

    bool_t      fAcpIntr;
    bool_t      fSpdCaptured;
    bool_t      fIsrc1Captured;
    bool_t      fSpdDecodeIsrc2;
    bool_t      fIntrOnEveryNewAvi;
    bool_t      fIntrOnEveryNewAud;
    bool_t      fIntrOnEveryNewGamut;
    bool_t      fVsifIsPresent;
    bool_t      fAcpIsPresent;
    bool_t      fMpegBuff_IsrcNotFound;
    /*
        This state is important to track changes of mpeg info frame buffer.
        When Vsif is received and Isrc capturing period is missed, when we go back
        to receive Vsif, if data weren't changed we will not receive an Vsif
        interrupt. Vsif wii receive Time Out and without acctionans applicable
        to Vsif we should set Isrc captuting again.
    */
    bool_t      fDebug;
}IfCtrl_t;

typedef struct _IfFlafs_t {

    uint8_t     bNewIfs;
    uint8_t     bChangedIfs;
    uint8_t     bStoppedIfs;

}IfFlafs_t;

/* info frames, check sum property */

typedef enum {
    IfWCheckSum         /*  generic info frame check sum (Avi, Aud, Spd, Vsif)  */
    ,IfWGamutCheckSum   /*  info frame check sum of gamut packet                */
    ,IfWOCheckSum       /*  info frame without check sum (Acp, Isrc1/2          */
} eIfCheckSumProp_t;

extern IfCtrl_t SiI9233IfCtrl;
#endif
/***** public function prototypes ********************************************/

/*****************************************************************************/
/**
 *  The description of the function HdmiInitIf(). This function intializes Info Frame 
 *	related registers and data structures. Function should be called only once		
 *
 *  @param[in,out]      none
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/

void HdmiInitIf ( void );

/*****************************************************************************/
/**
 *  The description of the function HdmiProcIfTo(). This function handles info frame 
 *	time outs		
 *
 *  @param[in] wToStep  Info Frame elapsed from previous to call ( measured in MS)
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/

void HdmiProcIfTo ( uint16_t wToStep  );

/*****************************************************************************/
/**
 *  The description of the function InterInfoFrmProc(). Processing info frame interrupts
 *
 *
 *  @param[in] bNewInfoFrm  new info frames
 *  @param[in] bNoAvi		no Avi info frames
 *  @param[in] bNoInfoFrm	no info frames
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/

void InterInfoFrmProc ( uint8_t bNewInfoFrm,
					/*	uint8_t bNoAvi,*/
						uint8_t bNoInfoFrm );

#else /* _INFOFRM_H */


#endif /* _INFOFRM_H */
