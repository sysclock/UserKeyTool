/*
    $Workfile:   ir.h  $
    $Revision: 1.4 $
    $Date: 2011/12/15 01:43:00 $
*/
#ifndef __IR_H__
#define __IR_H__
//******************************************************************************
// COPYRIGHT (C) STMicroelectronics 2011.
//
// All rights reserved. This document contains proprietary and
// confidential information of the STMicroelectronics Group. This
// document is not to be copied in whole or part. STMicroelectronics
// assumes no responsibility for the consequences of use of such
// information nor for any infringement of patents or other rights of
// third parties which may result from its use. No license is granted by
// implication or otherwise under any patent or patent rights of
// STMicroelectronics.
// STMicroelectronics
// products are not authorized for use as critical components in life
// support devices or systems without express written approval of
// STMicroelectronics.
//==============================================================================
//
// MODULE:      IR.h
//
// USAGE:	Header file for IR remote IR.c to define all remote setting values.
//
//******************************************************************************

//******************************************************************************
//  D E F I N E S
//******************************************************************************

//#define IR_IRQ_DISABLE						0
//#define IR_IRQ_ENABLE						1
//#define IR_CHANNEL1							0
#define IR_CFG_RAW							0xC
//#define IR_INVALID							0xffff

#define	IR_POLARITY_MASK					0x80
//#define	IR_POLARITY_HIGH					0x80
//#define	IR_POLARITY_LOW					0x00
#define	IR_BIT_WIDTH_MASK					0x7F

//******************************************************************************
//  S E L E C T   R E M O T E
//******************************************************************************


// *** D e b u g ***************************************************************

//	#define	IR_DEBUG_CHARACTERIZE
//	#define	IR_DEBUG_KEY
//	#define IR_DEBUG_USE_LOCAL_REMOTE_TABLE

#ifdef IR_REMOTE_GENERIC_RECS80
#define IR_MAX_PACKET_TYPES	 2
#else
#define IR_MAX_PACKET_TYPES	 1
#endif

#pragma pack(push)
#pragma pack(1)
typedef struct
{
   BYTE			B_Protocol;
   WORD			W_Config;
   WORD			W_Timing;
   BYTE			B_Period;
   BYTE			B_NumPackets;
   gmt_IrPacket	B_Packets[3];
} gmt_IrProtocolLpm;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
   BYTE			B_Protocol;
   WORD			W_Config;
   WORD			W_Timing;
   BYTE			B_Period;
   BYTE			B_NumPackets;
   gmt_IrPacket	B_Packets[IR_MAX_PACKET_TYPES];
} gmt_IrProtocolExt;
#pragma pack(pop)

extern ROM gmt_IrProtocolExt IrProtocol;
extern BYTE IR_Global[];

#ifdef IR_ISR_RAM_EXE
//offsetof(gmt_IrProtocol, B_Packets) + (IrProtocol.B_NumPackets)*sizeof(gmt_IrPacket) +
//	offset gm_IrInit - offset gmp_IrIsr
#define OFFMODE_IR_RAM_BUF 0x5E0
extern BYTE ir_isr_ram_exe_buf[OFFMODE_IR_RAM_BUF];
#endif

//******************************************************************************
//  P R O T O T Y P E S
//******************************************************************************

void	IrHandler(void);
void	IrCallBack(WORD W_Data);

#endif // __IR_H__

//*********************************  END  **************************************
