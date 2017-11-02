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
 * @file infofrm.c
 *
 * This is a description of the file.
 *
 * $Author: $Vladimir Grekhov
 * $Rev: $
 * $Date: $ 9-14-09
 *
 *****************************************************************************/

/***** #include statements ***************************************************/
#include <stdio.h>
#include <string.h>
#include "../../System Code/global.h"
#include <local_types.h>
#include "spec_types.h"
#include "config.h"
#include "registers.h"  /*  defines receiver's chip registers                       */
#include "amf.h"        /*  defines accessor functions receiver's chip registers    */
//#include "hal.h"        /*  macro with printf                                       */
#include "infofrm.h"

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


/***** local variable declarations *******************************************/
/**
 * @brief Typically this should do.
 */
const uint8_t IEEERegistrationId[] = { 0x03, 0x0C, 0x00 };

IfCtrl_t SiI9233IfCtrl;


/***** local function prototypes *********************************************/

#if (CONF__SUPPORT_REPEATER3D == ENABLE)
static uint8_t GetIfTypeFromUnreqBuffer ( void );
#endif
static void SetDecodeIfBuffers ( const DevAddr_t DevAddr, const uint8_t bDecodeIfType );
static void SetIfTo ( SelectIf_t eSelectIf, const uint16_t wTo );
static bool_t GetIf( const uint8_t bIfType );
static bool_t GetIntrAcpStatus ( void );
static void SetIntrAcp ( const bool_t fEnable );
static void SetIntrOnEveryNewIf ( const bool_t fEnable, const uint8_t bIfType );
static void SetToIfBuff ( const uint8_t bIfType );
static void print_tabl (    char const *pMesg,
                            void const *pData,
                            uint16_t wCnt,
                            uint8_t bCol );
static void AviParser ( uint8_t const *pPayload );
static void VsifParser ( uint8_t const *pPayload );

static bool_t ProcIf ( DevAddr_t DevAddr, const uint8_t bIfType, uint8_t bSize );
/***** local functions *******************************************************/
#if (CONF__SUPPORT_REPEATER3D == ENABLE)
static uint8_t GetIfTypeFromUnreqBuffer ( void ){

    return RegisterRead( REG__UNREQ_TYPE );
}
#endif
static uint8_t GetMpegDecodeAddr ( void ){
    return RegisterRead( REG__MPEG_DECODE);
}
/*****************************************************************************/
/**
 *  The description of the function print_tabl().
 *
 *  @param[in]  char const *pMesg   - label being print out
 *  @param[in]  void const *pData   - data being print out
 *  @param[in]  uint16_t    wCnt    - number bytes to be print out
 *  @param[in]  uint8_t     bCol    - number collons to print
 *
 *  @return     none.
 *  @retval     void
 *
 *****************************************************************************/
static void print_tabl ( char const *pMesg, void const *pData, uint16_t wCnt, uint8_t bCol ){
uint32_t i;
uint8_t const *pbData;

    pbData = (uint8_t *)pData;
    DEBUG_PRINT (("\r\nBoard %d , Input %d , %s", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, pMesg ));

    if ( pbData != NULL ){
        for ( i = 0; i < wCnt; i++ ){
            if ( i%bCol == 0 ){
                DEBUG_PRINT (( "\r\nBoard %d , Input %d , 0x%X: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, (uint32_t) &pbData[i]));
            }
            DEBUG_PRINT (( " 0x%02X", (uint16_t)pbData[i] ));
        }
        DEBUG_PRINT (( "\r\n"));
    }
    else {
        DEBUG_PRINT (( "\r\nBoard %d , Input %d , NULL pointer", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    }
    DEBUG_PRINT (( "\r\n"));
}

static void print_mpegbuffer( void ){
uint8_t abData[16];

    RegisterReadBlock( REG__MPEG_TYPE, abData, 16 );
    print_tabl ( "Mpeg_buf", abData, 16, 8 );

}

/*****************************************************************************/
/**
 *  The description of the function AviParser().
 *  Displays parsed information
 *  @param[in]  uint8_t const *pPaylaod   - Info frame paylaod
 *
 *  @return     none.
 *  @retval     void
 *
 *****************************************************************************/
static void AviParser ( uint8_t const *pPayload ){

    DEBUG_PRINT (("Board %d , Input %d , Parsing Avi: \r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    /* check for Video Id Code*/
    SiI9233IfCtrl.bVidModeId = (pPayload[IF_AVI_VIC_ADDR] & IF_AVI_VIC_SEL );
    DEBUG_PRINT (("Board %d , Input %d , Vic: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    switch (SiI9233IfCtrl.bVidModeId){
        case   0: DEBUG_PRINT (("Not valid ID"));
                break;
          default:                DEBUG_PRINT (( "%d", (uint16_t) IfCtrl.bVidModeId ));
    }
    DEBUG_PRINT ( ("\r\nBoard %d , Input %d , Exten. Colorimetry Info: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
    switch (pPayload[IF_AVI_EC_ADDR] & ExtnColorimSel){
        case xvYCC601:      DEBUG_PRINT (("xvYCC601"));
            break;
        case xvYCC709:      DEBUG_PRINT (("xvYCC709"));
            break;
        case cYCC601:       DEBUG_PRINT (("cYCC601"));
            break;
        case AdobeYCC601:   DEBUG_PRINT (("AdobeYCC601"));
            break;
        case AdobeRGB:      DEBUG_PRINT (("AdobeRGB"));
            break;
        default:            DEBUG_PRINT (("Res"));
    }
    DEBUG_PRINT ( ("\r\nBoard %d , Input %d , Cont Type: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
    switch (pPayload[IF_AVI_CN_ADDR] & ContTypeSel ){
        case Graphics:
            if ( pPayload[IF_AVI_ITC_ADDR] & ITContSel ){
                DEBUG_PRINT (("Graphics"));
            }
            else {
                DEBUG_PRINT (("No Data"));
            }
            break;
        case Photo:     DEBUG_PRINT (("Photo"));
            break;
        case Cinema:    DEBUG_PRINT (("Cinema"));
            break;
        case Game:      DEBUG_PRINT (("Game"));
            break;
    }
}
/*****************************************************************************/
/**
 *  The description of the function VsifParser().
 *  Displays parsed information
 *  @param[in]  uint8_t const *pPaylaod   - Info frame paylaod
 *
 *  @return     none.
 *  @retval     void
 *
 *****************************************************************************/


static void VsifParser ( uint8_t const *pPayload ){

    DEBUG_PRINT (("Board %d , Input %d , Parsing Vsif: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    /* Check IEEE Registreation Id */
    if ( !memcmp ( &pPayload[IF_VSIF_IEEE_REGID_ADDR], IEEERegistrationId , sizeof(IEEERegistrationId) )){
        /* Check Hdmi Format for 3D*/
        if ( (pPayload[IF_VSIF_HDMI_FORMAT_ADDR] & IF_VSIF_HDMI_FORMAT_SEL) == IF_HDMI_FORMAT_3D ){
            DEBUG_PRINT (("\r\nVsif 3D: "));
            switch ( pPayload[IF_VSIF_3D_STRUCT_ADDR] >> IF_VSIF_3DSTRUCT_SEL ){
                case FramePacking:      DEBUG_PRINT (("FramePacking"));
                    break;
                case FrameAlternative:  DEBUG_PRINT (("FrameAlternative"));
                    break;
                case LineAlternative:   DEBUG_PRINT (("LineAlternative"));
                    break;
                case SiedBySideFull:    DEBUG_PRINT (("SiedBySideFull"));
                    break;
                case LDepth:            DEBUG_PRINT (("LDepth"));
                    break;
                case LDepthGraphGraphDepth: DEBUG_PRINT (("LDepthGraphGraphDepth"));
                    break;
                case SideBySideHalf:        DEBUG_PRINT (("SideBySideHalf"));
                    break;
                default:                    DEBUG_PRINT (("Reserved")); //yma change to reserved
            }
		DEBUG_PRINT ((" \r\n"));			
        }
        else {
            DEBUG_PRINT (("\r\nBoard %d , Input %d , Vsif not 3D", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
        }
    }
    else {
        DEBUG_PRINT (("\r\nBoard %d , Input %d , Vsif IEEE Reg. Id is not matched", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    }
}

/*****************************************************************************/
/**
 *      The description of the function SetDecodeIfBuffers ()
 *
 *  @param[in]  DevAddr Rx Chip leaner address
 *  @param[in]  Rx Chip, decode info frame type
 *
 *  @return     none
 *  @retval     void
 *
 *****************************************************************************/
static void SetDecodeIfBuffers ( const DevAddr_t DevAddr, const uint8_t bDecodeIfType ){
    RegisterWrite( DevAddr, bDecodeIfType );
    if ( REG__MPEG_DECODE == DevAddr ){
        SiI9233IfCtrl.bDecodeTypeMpegBuffer = bDecodeIfType;
    }
    else {
        SiI9233IfCtrl.bDecodeTypeSpdBuffer = bDecodeIfType;
    }
}

static void SetIfTo ( SelectIf_t eSelectIf, const uint16_t wTo ){
    SiI9233IfCtrl.Ins[eSelectIf].wTo = wTo;
}

/*****************************************************************************/
/**
 *  The description of the function SetToIfBuff().
 *
 *  @param[in] bIfType  InfoFrame type
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/
 static void SetToIfBuff ( const uint8_t bIfType ){

    switch( bIfType ){
        case IF_AVI_ID:
            break;
        case IF_VSIF_ID:
            SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC1_ID );
            SiI9233IfCtrl.Ins[ SelIsrc1 ].wTo = VSIF_LONG_CAPTURE_TO;
             /* this time out is used with shared info frame buffer, so cannot set the same time */
            SiI9233IfCtrl.Ins[ SelVsif ].wTo = 0;
            if ( !SiI9233IfCtrl.fSpdDecodeIsrc2 ){
                SiI9233IfCtrl.Ins[ SelIsrc1 ].wTo = 0;
            }
            break;
        case IF_ISRC1_ID:
            if ( !SiI9233IfCtrl.fSpdDecodeIsrc2 ){ /* mpeg buffer is used for Isrc1/Isrc2/Vsif */
                SiI9233IfCtrl.Ins[ SelIsrc2 ].wTo  = VSIF_LONG_CAPTURE_TO;
                SiI9233IfCtrl.Ins[ SelIsrc1 ].wTo  = 0;
                SiI9233IfCtrl.Ins[ SelVsif ].wTo   = 0;
                SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC2_ID );
            }
            else { /* mpeg buffer is used for Isrc1 Vsif */
                SiI9233IfCtrl.Ins[ SelVsif ].wTo = TWO_VIDFR_CAPTURE_TO;
                 /* this time out is used with shared info frame buffer, so cannot set the same time*/
                SiI9233IfCtrl.Ins[ SelIsrc1 ].wTo = 0;
                print_mpegbuffer();
                SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_VSIF_ID );
                SiI9233IfCtrl.fDebug = 1;
            }
            break;
        case IF_ISRC2_ID:
            if ( !SiI9233IfCtrl.fSpdDecodeIsrc2 ){
                SiI9233IfCtrl.Ins[ SelVsif ].wTo = TWO_VIDFR_CAPTURE_TO;
                SiI9233IfCtrl.Ins[ SelIsrc1 ].wTo  = 0;
                SiI9233IfCtrl.Ins[ SelIsrc2 ].wTo = 0;
                SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_VSIF_ID );
            }
            break;
        case IF_SPD_ID:
            SiI9233IfCtrl.Ins[ SelIsrc2].wTo = 0;
            SetDecodeIfBuffers ( REG__SPD_DECODE,  IF_ISRC2_ID );
            break;
        case IF_ACP_ID:
            SiI9233IfCtrl.wNoAcpTotTime    = TIME_WO_ACP_IF;
            /* TODO disable Acp interrupt */
            /* set time out*/
            SiI9233IfCtrl.Ins[ SelAcp].wTo = ACP_CAPTURE_200MS;
            break;
        default: 
			;
    }
 }


static bool_t GetIfRoll( DevAddr_t DevAddr, uint8_t *pbRetIfType ){
uint8_t bSize;
bool_t fValid = TRUE;

    *pbRetIfType = RegisterRead( DevAddr );

    switch ( *pbRetIfType ){
        case IF_SPD_ID:
                        bSize   =   IF_SPD_SIZE;
                        break;
        case IF_VSIF_ID:
                        bSize   =   IF_VSIF_SIZE;
                        break;
        case IF_ISRC1_ID:
        case IF_ISRC2_ID:
                        bSize   =   IF_ISRC_SIZE;
                        break;
        default:
            fValid = FALSE;
    }
    if ( fValid ){
        fValid = ProcIf ( DevAddr, *pbRetIfType, bSize );
    }

    return fValid;
}
/*****************************************************************************/
/**
 *      The description of the function ProcIf()
 *
 *  @param[in]  DevAddr         - device address
 *  @param[in]  bIfType         - info frame type
 *  @param[in]  bSize           - size of infoframe
 *
 *  @return     bool_t
 *  @retval     TRUE for a valid packet
 *
 *****************************************************************************/
bool_t ProcIf ( DevAddr_t DevAddr, const uint8_t bIfType, uint8_t bSize ){
bool_t fValid = FALSE;
uint8_t i;
uint8_t bAuxSize = 0;
uint8_t CheckSum = 0;
//uint8_t NumMissed;
/*
Checksum [1 byte] Checksum of infoframe. The checksum shall be calculated such that byte-sum
of all three bytes of the Packet Header and all valid uint8_ts of the InfoFrame Packet contents
(determined by InfoFrame Length), plus check sum itself, equals zero
*/


    if ( !(bIfType & 0x80) ){
        RegisterReadBlock( DevAddr, SiI9233IfCtrl.abBuff, bSize );
        print_tabl ( "* IF Data 1", SiI9233IfCtrl.abBuff, bSize, 8);
    }
    else {
        RegisterReadBlock( DevAddr, SiI9233IfCtrl.abBuff, IF_HEADER_SIZE );
        print_tabl ( "** IF Data 2", SiI9233IfCtrl.abBuff, IF_HEADER_SIZE, 8);
        if ( SiI9233IfCtrl.abBuff[ IF_LENGTH_ADDR] <= bSize ){
            bAuxSize = SiI9233IfCtrl.abBuff[ IF_LENGTH_ADDR];
            DevAddr += IF_HEADER_SIZE; /* first byte is check sum */
            RegisterReadBlock( DevAddr, &SiI9233IfCtrl.abBuff[IF_HEADER_SIZE], bSize + 1);
            bAuxSize += (IF_HEADER_SIZE + 1); /* header excludes check sum which in counted by adding 1, payload if following of check sum */
            if ( bIfType == IF_VSIF_ID ){
                if ( !memcmp ( SiI9233IfCtrl.abBuff, SiI9233IfCtrl.abVsif, bAuxSize ) ){
                    DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** Same Vsif\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
                }
                else {
                    print_tabl ( "** Got new Vsif", SiI9233IfCtrl.abBuff, SiI9233IfCtrl.abBuff[IF_LENGTH_ADDR], 8);
                    memcpy ( SiI9233IfCtrl.abVsif, SiI9233IfCtrl.abBuff, bSize + IF_HEADER_SIZE );
                    for ( i = 0; i < bAuxSize; i++ ){
                        CheckSum += SiI9233IfCtrl.abBuff[i];
                    }
                    DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** IF 0x%2X Checksum: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, (uint16_t) bIfType));
                    if ( !CheckSum ){ /* */
                        DEBUG_PRINT (( "OK\r\n" ));
                        VsifParser( &SiI9233IfCtrl.abBuff[IF_PAYLOAD_ADDR] );
                        fValid = TRUE;
                    }
                    else {
                        DEBUG_PRINT (( "Failed\r\n" ));
                    }
                    SiI9233IfCtrl.fVsifIsPresent = TRUE;
                }
            }
            else if ( bIfType == IF_GMT_ID ){

            }
            else {
                print_tabl ( "** IF Data", SiI9233IfCtrl.abBuff, SiI9233IfCtrl.abBuff[IF_LENGTH_ADDR], 8);
                for ( i = 0; i < bAuxSize; i++ ){
                    CheckSum += SiI9233IfCtrl.abBuff[i];
                }
                DEBUG_PRINT (( "Board %d , Input %d , ** IF 0x%2X Checksum: ", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, (uint16_t) bIfType));
                if ( !CheckSum ){ /**/
                    fValid = TRUE;
					DEBUG_PRINT (( "OK\r\n" ));
                    if ( bIfType == IF_AVI_ID ){
                        AviParser( &SiI9233IfCtrl.abBuff[IF_PAYLOAD_ADDR] );
                    }                 
                }
                else {
                    DEBUG_PRINT (( "Failed\r\n" ));
                }
            }
        }
        else {
            DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** GetIf: wrong parametr in info frame size\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
        }
    }

    return fValid;
}

/*****************************************************************************/
/**
 *      The description of the function GetIf()
 *
 *  @param[in]  bIfType         - info frame type
 *  @param[in]  bBufferType     - info frame buffer (Isrc can use a differnt buffer)
 *
 *  @return     bool_t
 *  @retval     TRUE for a valid packet
 *
 *****************************************************************************/

static bool_t GetIf( const uint8_t bIfType ){
bool_t fValid = TRUE;
DevAddr_t   DevAddr =   0;
uint8_t     bSize   =   0;

    switch ( bIfType ){

        case IF_VSIF_ID:
                        bSize   =   IF_VSIF_SIZE;
                        DevAddr =   REG__MPEG_TYPE;
                        break;
        case IF_AVI_ID:
                        bSize   =   IF_AVI_SIZE;
                        DevAddr =   REG__AVI_TYPE;
                        break;
        case IF_AUD_ID:
                        bSize   =   IF_AUD_SIZE;
                        DevAddr =   REG__AUD_TYPE;
                        break;
        case IF_SPD_ID:
                        bSize   =   IF_SPD_SIZE;
                        DevAddr =   REG__SPD_TYPE;
                        break;
        case IF_GMT_ID:
                        bSize   =   IF_GMT_SIZE;
                        DevAddr =   REG__GMT_TYPE;
                        break;
        case IF_ACP_ID:
                        bSize   =   IF_ACP_SIZE;
                        DevAddr =   REG__ACP_TYPE;
                        break;
        default:
                        DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** GetIf: unsupported type\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
                        fValid = FALSE;
    }
    if ( fValid ){
        fValid = ProcIf ( DevAddr, bIfType, bSize );
    }

    return fValid;
}

/*****************************************************************************/
/**
 *      The description of the function GetIntrAcpStatus ()
 *      This function checks status of Acp interrupt, it is used in polling manner
 *
 *  @param[in/out]  none
 *
 *  @return     bool_t
 *  @retval     TRUE for a valid packet
 *
 *****************************************************************************/
static bool_t GetIntrAcpStatus ( void ){
bool_t fStatus = FALSE;

    if ( RegisterRead( REG__INTR6) & BIT__NEW_ACP_PKT ){
        RegisterWrite( REG__INTR6,  BIT__NEW_ACP_PKT );
        fStatus = TRUE;
    }
    else {
        fStatus = FALSE;
    }
    return fStatus;
}
/*****************************************************************************/
/**
 *      The description of the function SetIntrAcp ()
 *      This function sets or clears Acp interrupt mode. When Acp packets
 *      are received at required time frame check is done using Acp time out
 *      by polling an interrupt status
 *
 *  @param[in]  fEnable - enab;es or disables Acp interrupts
 *
 *  @return     none
 *  @retval     void
 *
 *****************************************************************************/
static void SetIntrAcp ( const bool_t fEnable ){

    if ( fEnable ){
        SiI9233IfCtrl.fAcpIntr = TRUE;
        RegisterModify( REG__INTR6_UNMASK, BIT__NEW_ACP_PKT, BIT__NEW_ACP_PKT );
    }
    else {
        SiI9233IfCtrl.fAcpIntr = FALSE;
        RegisterModify( REG__INTR6_UNMASK, BIT__NEW_ACP_PKT, 0 );
    }

}
/*****************************************************************************/
/**
 *      The description of the function SetIntrOnEveryNewIf()
 *      This function enables or disables interrupt on every new info frame packet
 *
 *  @param[in]  fEnable - Enables interrupt on every new info packet
 *  @param[in]  bIfType - Type of info frame packet
 *
 *  @return     none
 *  @retval     void
 *
 *****************************************************************************/
static void SetIntrOnEveryNewIf ( const bool_t fEnable, const uint8_t bIfType ){
uint8_t     bBitField;
bool_t      fError = FALSE;

    if ( bIfType == IF_AVI_ID ){
        bBitField = BIT__NEW_AVI_CTRL_INF;
    }
    else if ( bIfType == IF_GMT_ID ){
        bBitField = BIT__NEW_GMT_CTRL_INF;
    }
    else if ( bIfType == IF_AUD_ID ){
        bBitField = BIT__NEW_AUD_CTRL_INF;
    }
    else if ( bIfType == IF_ACP_ID ){
        bBitField = BIT__NEW_ACP_CTRL_INF;
    }
    else {
        DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** SetIntrOnNewIf: wrong parametr\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
        fError = TRUE;
    }
    if ( !fError ){
        if ( fEnable ){
						DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** Set Intr On NewIf: bBitField = 0x%02X\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, bBitField));
            RegisterModify( REG__INTR_IF_CTRL, bBitField, bBitField );
        }
        else {
            RegisterModify( REG__INTR_IF_CTRL, bBitField, 0 );
        }
    }
}

 /***** public functions ******************************************************/


/*****************************************************************************/
/**
 *  The description of the function HdmiInitIf(). This function intializes Info Frame
 *  related registers and data structures. Function calls only once
 *
 *  @param[in,out]      none
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/

void HdmiInitIf ( void ){


    /* clears SiI9233IfCtrl structure */
    memset ( &SiI9233IfCtrl, 0, sizeof (IfCtrl_t) );
    SET_CLR_IF(); /* set clear info frame data when rx stops receiving info frames
                    use for Mpeg (Isrc1/Isrc2/Vsif) buffer, Acp, Gdp*/

/*******************************************************************************


ACP     ACP
            This buffer used for ACP
            Started with Acp interrupt, whe Acp packet received, start polling
            to exclude

SPD SPD/ISRC2
            After receiving first SPD packet usage configured on receiving ISRC2.
            Also consider if SPD wasn't received in 1.25 sec,
            automatically switch to ISRC2 mode

MPEG ISRC1/VS
            Check for VSIF every second. When new AVI is detected set 1 sec window for
            VSIF.

UNREQ
            IRSC1/ISRC2 - receive
            VSIF        - detect, use MPEG buffer

*******************************************************************************/
    /*
        Set decode addreses: only ACP/SPD/MPEG buffers have programmable
        decode addresses
    */
    SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC1_ID );
    SetDecodeIfBuffers ( REG__SPD_DECODE,   IF_SPD_ID );
    /*
        Set info frame time outs: depending on requests after reaching time outs
        will be changed functionality of particular info frame buffer
    */
    /*
        After reaching SPD time out, this info frame buffer will be used for
        other info frames as ISRC1/2
    */

    /*  Time out is set in ms    */
    SetIfTo ( SelSpd,    SPD_CAPTURE_TO );
    SetIfTo ( SelAcp,    0 );
    SetIfTo ( SelVsif,   0 );
    SetIfTo ( SelIsrc1,  ISRC1_CAPTURE_TO );
    /*
        SPD_BUFF:   [SPD][ISRC2 when SPD captured or time outed]
        MPEG_BUFF:  [ISRC1][VSIF][ISRC2 if SPD_BUFF is SPD][ISRC1][VSIF]
    */
    SiI9233IfCtrl.fAcpIntr = TRUE;		 
	SET_PACKET_INTR_MASK();
	

//    IfInitIfTest();
}

/*****************************************************************************/
/**
 *  The description of the function HdmiProcIfTo().
 *
 *  @param[in] wToStep  Info Frame elapsed time from previous to call
 *                      of this function ( measured in MS)
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/
void HdmiProcIfTo ( uint16_t wToStep  ){
//SelectIf_t id;
int id;
uint16_t B_Data = 0;

		B_Data = B_Data;				// Dummy set;
    if ( SiI9233IfCtrl.fAcpIsPresent ){
        SiI9233IfCtrl.wNoAcpTotTime += wToStep;
    }
    for ( id = 0; id < MaxNumIfSel; id++ ){
        if ( SiI9233IfCtrl.Ins[id].wTo ){
            if ( SiI9233IfCtrl.Ins[id].wTo > wToStep ){
                SiI9233IfCtrl.Ins[id].wTo -= wToStep;
            }
            else {
                SiI9233IfCtrl.Ins[id].wTo = 0;
            }
            if ( !SiI9233IfCtrl.Ins[id].wTo ) { /* serve time outs */
                switch ( id ){
                    case SelVsif:
                                    if ( SiI9233IfCtrl.fVsifIsPresent && (!SiI9233IfCtrl.fMpegBuff_IsrcNotFound) ){
                                        SiI9233IfCtrl.fVsifIsPresent = FALSE;
                                        memset ( SiI9233IfCtrl.abVsif, 0, IF_VSIF_SIZE );
                                        SiI9233IfCtrl.fDebug = 0;
                                        print_mpegbuffer();
																				B_Data = GetMpegDecodeAddr();
                                        DEBUG_PRINT (( "\r\nBoard %d , Input %d , ** TX stops sending Vsif %X", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, B_Data ));
                                    }
                                    if ( !SiI9233IfCtrl.fSpdDecodeIsrc2 && SiI9233IfCtrl.fIsrc1Captured  ){
                                        SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC2_ID );
                                        SiI9233IfCtrl.Ins[SelIsrc2].wTo      = TWO_VIDFR_CAPTURE_TO;
                                    }
                                    else {
                                        SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC1_ID );
                                        SiI9233IfCtrl.Ins[SelIsrc1].wTo      = ISRC1_CAPTURE_TO;
                                    }
                                    break;
                    case SelIsrc1:
                                    SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_VSIF_ID );
                                    SiI9233IfCtrl.Ins[SelVsif].wTo      = TWO_VIDFR_CAPTURE_TO;
                                    SiI9233IfCtrl.fMpegBuff_IsrcNotFound = TRUE;
                                    break;
                    case SelIsrc2:
                                    if ( !SiI9233IfCtrl.fSpdDecodeIsrc2 ){ /* this tell us that we share buffer with Vsif */
                                        SiI9233IfCtrl.fMpegBuff_IsrcNotFound = TRUE;
                                        SetDecodeIfBuffers ( REG__MPEG_DECODE,  IF_ISRC1_ID );
                                        SiI9233IfCtrl.Ins[SelIsrc1].wTo      = ISRC1_CAPTURE_TO;
                                    }
                                    else {
                                    /*  do nothing as only ISRC2 used for detection */
                                    }
                                    break;
                    case SelSpd:
                                    SetDecodeIfBuffers ( REG__SPD_DECODE,  IF_ISRC2_ID );
                                    SiI9233IfCtrl.fSpdDecodeIsrc2     = TRUE;
                                    /* As no SPD for such a long time, allocate SPD_BUFF for ISRC2*/
                                    break;
                    case SelAcp:
                                    if ( GetIntrAcpStatus ()){
										SiI9233IfCtrl.fAcpIsPresent = TRUE;
										SiI9233IfCtrl.wNoAcpTotTime = 0;                                        
                                        SiI9233IfCtrl.Ins[SelAcp].wTo = ACP_CAPTURE_200MS;
                                    }
                                    else {
                                        if ( SiI9233IfCtrl.wNoAcpTotTime >= TIME_WO_ACP_IF ){
                                            DEBUG_PRINT ( ( "\r\nBoard %d , Input %d , ** TX stops sending Acp\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
                                            SiI9233IfCtrl.fAcpIsPresent = FALSE;
                                            memset ( SiI9233IfCtrl.abAcp, 0, IF_ACP_SIZE);
                                            SiI9233IfCtrl.fAcpIntr = TRUE;
											SetIntrAcp ( SiI9233IfCtrl.fAcpIntr );
                                        }
                                        else {
                                            SiI9233IfCtrl.Ins[SelAcp].wTo = ACP_CAPTURE_200MS;
                                        }
                                    }
                                    break;
                }
            }
        }
    }
}


/*****************************************************************************/
/**
 *  The description of the function InterInfoFrmProc(). Processing info frame interrupts
 *
 *
 *  @param[in] bNewInfoFrm  new info frames
 *  @param[in] bNoInfoFrm   no info frames
 *
 *  @return             none
 *  @retval             void
 *
 *****************************************************************************/
 /*
        SPD_BUFF:   [SPD][IFRC2 when SPD captured]
        MPEG_BUFF:  [ISRC1][VSIF][ISRC2 if SPD_BUFF is SPD][ISRC1][VSIF]
 */
void InterInfoFrmProc ( uint8_t bNewInfoFrm, uint8_t bNoInfoFrm ){
uint8_t bIfType;

    if ( bNewInfoFrm ){
        if ( bNewInfoFrm & BIT__NEW_AVI_INF ){
            if ( SiI9233IfCtrl.fIntrOnEveryNewAvi ){
                /* set Avi interrupt on change */
                SiI9233IfCtrl.fIntrOnEveryNewAvi =  FALSE;
                SetIntrOnEveryNewIf ( FALSE, IF_AVI_ID );
            }
            DEBUG_PRINT (("Board %d , Input %d , * Got Avi\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
            SetToIfBuff ( IF_AVI_ID );
            if ( GetIf ( IF_AVI_ID )){
                DEBUG_PRINT ((" valid\r\n" ));
            }
        }
        if ( bNewInfoFrm & BIT__VIRT_NEW_GDB_INF ){
            if ( SiI9233IfCtrl.fIntrOnEveryNewGamut ){
                /* set Gamut interrupt on change */
                SiI9233IfCtrl.fIntrOnEveryNewGamut =  FALSE;
                SetIntrOnEveryNewIf ( FALSE, IF_GMT_ID );
            }
            DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Got Gdb\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
            /* TODO: add processing */
        }
        if ( bNewInfoFrm & BIT__NEW_MPEG_INF ){
//            DEBUG_PRINT (( "\r\n* Got if in Mpeg buff: " );
            /* this buffer is used for Vsif and Isrc1/2*/
#if 0
            if ( SiI9233IfCtrl.bDecodeTypeMpegBuffer == IF_VSIF_ID ){
                if ( GetIf( IF_VSIF_ID, SiI9233IfCtrl.bDecodeTypeMpegBuffer ) ){
                }
            }
            else if (   (SiI9233IfCtrl.bDecodeTypeMpegBuffer == IF_ISRC1_ID) ||
                (SiI9233IfCtrl.bDecodeTypeMpegBuffer == IF_ISRC2_ID) ){
            }
#endif
            SiI9233IfCtrl.fMpegBuff_IsrcNotFound = FALSE;
            if ( GetIfRoll( REG__MPEG_TYPE, &bIfType )){
            }
            SetToIfBuff ( bIfType );
        }
        if ( bNewInfoFrm & BIT__NEW_AUD_INF ){
            if ( SiI9233IfCtrl.fIntrOnEveryNewAud ){
                /* set Aud interrupt on change */
                SiI9233IfCtrl.fIntrOnEveryNewAud =  FALSE;
                SetIntrOnEveryNewIf ( FALSE, IF_AUD_ID );
            }
            DEBUG_PRINT ( ( "\r\nBoard %d , Input %d , * Got Aud\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
            SetToIfBuff ( IF_AUD_ID );
        }
        if ( bNewInfoFrm & BIT__VIRT_NEW_ACP_INF ){

            DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Got Acp\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
            GetIf ( IF_ACP_ID );
            SetToIfBuff ( IF_ACP_ID );
			SiI9233IfCtrl.fAcpIsPresent = TRUE;
            if ( SiI9233IfCtrl.fAcpIntr ){
                /*  Disable Acp Interrupts until SiI9233IfCtrl.fAcpIsPresent  */

                SiI9233IfCtrl.fAcpIntr = FALSE;
				SetIntrAcp ( SiI9233IfCtrl.fAcpIntr );
                SetIntrOnEveryNewIf ( TRUE, IF_ACP_ID );
            }
        }
        if ( bNewInfoFrm & BIT__NEW_SPD_INF ){
            DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Got Spd buff: type %x\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, (uint16_t)SiI9233IfCtrl.bDecodeTypeSpdBuffer ));
            /* this buffer is used for Spd and Isrc2*/
            if ( GetIfRoll( REG__SPD_TYPE, &bIfType )){
            }
            SetToIfBuff ( bIfType );
        }
    }
    if ( bNoInfoFrm ){
        if ( bNoInfoFrm & BIT__VIRT_NO_AVI_INF ){
//						if(CurrentStatus.PrintMessageFlag == 1)
							DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Lost Avi\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
            /* set interrupt on every new Avi packet */
            SiI9233IfCtrl.fIntrOnEveryNewAvi = TRUE;
            SetIntrOnEveryNewIf ( TRUE, IF_AVI_ID );
        }
        if ( bNoInfoFrm & BIT__NO_GDB_INF ){
	//					if(CurrentStatus.PrintMessageFlag == 1)
							DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Lost Gdb\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
            /* set interrupt on every new Gamut packet */
            SiI9233IfCtrl.fIntrOnEveryNewGamut = TRUE;
            SetIntrOnEveryNewIf ( TRUE, IF_GMT_ID );
        }
        if ( bNoInfoFrm & BIT__NO_AUD_INF ){
//						if(CurrentStatus.PrintMessageFlag == 1)
							DEBUG_PRINT (( "\r\nBoard %d , Input %d , * Lost Aud\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex ));
            /* set interrupt on every new Audio packet */
            SiI9233IfCtrl.fIntrOnEveryNewAud = TRUE;
            SetIntrOnEveryNewIf ( TRUE, IF_AUD_ID );
        }
    }
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    Infofrm_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void Infofrm_DummyCall(void)
{

}

#endif //#if (CONF__SUPPORT_3D == ENABLE)
/********************************  END  *************************************/
