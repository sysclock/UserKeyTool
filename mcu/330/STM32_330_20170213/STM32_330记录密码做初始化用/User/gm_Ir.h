/*
    $Workfile:   gm_Ir.h  $
    $Revision:   1.12  $
    $Date:   Mar 17 2008 20:41:34  $
*/
#ifndef __GM_IR_H__
#define __GM_IR_H__
//******************************************************************************
//
//              Copyright (C) 2002.  GENESIS MICROCHIP INC. CONFIDENTIAL 
//      All rights reserved.  No part of this program may be reproduced.
//
//     Genesis Microchip Inc., 165 Commerce Valley Dr. West
//     Thornhill, Ontario, Canada, L3T 7V8
//
//==============================================================================
//
// MODULE:      gm_Ir.h
//
// USAGE:		Header file for Library IR remote routines.
//
//******************************************************************************

//******************************************************************************
//  D E F I N E S
//******************************************************************************

#define IR_IRQ_DISABLE						0
#define IR_IRQ_ENABLE						1
#define IR_CHANNEL1							0
#define IR_CFG_RAW							0xC
#define IR_INVALID							0xffff

#define	IR_POLARITY_MASK					0x80
#define	IR_POLARITY_HIGH					0x80
#define	IR_POLARITY_LOW						0x00
#define	IR_BIT_WIDTH_MASK					0x7F

#define IR_MAX_PACKET_TYPES					3

//******************************************************************************
//  E N U M S
//******************************************************************************

#if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
enum IrProtocols
{
	IR_PROTOCOL_RECS80 	= 0,
	IR_PROTOCOL_RC5,
};

enum IrProtocolFlags
{
	IR_FLAG_NONE 		= 0,				// None
	IR_FLAG_REPEAT		= BIT1,				// Repeat Key Packet
	IR_FLAG_MULT_START	= BIT2,				// Packet contains multiple start pulses
	IR_FLAG_POWER		= BIT3,				// Power Key Packet
	IR_FLAG_REVERSE		= BIT4,				// Reverse Bit order MSB...LSB
};
#endif

//******************************************************************************
//  S T R U C T S
//******************************************************************************
//
// Packet Definition
//
#if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
typedef struct {
	BYTE			B_Flags;
	BYTE			B_Start0Min;
	BYTE			B_Start0Max;
	BYTE			B_Start1Min;
	BYTE			B_Start1Max;
	BYTE			B_TroughMin;		// RECS80			RC5
	BYTE			B_TroughMax;		// ------			---------
	BYTE			B_Data0Min;			// Logic0Min		SingleMin
	BYTE			B_Data0Max;			// Logic0Max		SingleMax
	BYTE			B_Data1Min;			// Logic1Min		DoubleMin
	BYTE			B_Data1Max;			// Logic1Max		DoubleMax
	BYTE			B_PayloadCount;
	DWORD 			D_AddrMask;
	BYTE			B_AddrShift;
	DWORD 			D_DataMask;
	BYTE			B_DataShift;
	WORD			W_CmdID;
} gmt_IrPacket;
#endif

//
// Protocol Definition
//
#ifdef __PARADIGM__
   #pragma pack(push)
   #pragma pack(1)
#endif

#if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
typedef struct {
	BYTE			B_Protocol;
	WORD			W_Config;
	WORD			W_Timing;
	BYTE			B_Period;
	BYTE			B_NumPackets;
	gmt_IrPacket	B_Packets[IR_MAX_PACKET_TYPES];	
} gmt_IrProtocol;
#endif

typedef struct {
	BYTE			B_State;
	BYTE			B_StateCnt;
	BYTE			B_SpecialKey;
	WORD			W_Address;
	DWORD			D_Payload;
} gmt_IrPacketState;

#define MINIMAL_ARRAY_SIZE 1
typedef struct {
	gmt_IrProtocol const far* P_IrProtocol;
	void (far* P_IrCallBack)(WORD);
	DWORD expTime;
	gmt_RET_STAT valid;
	WORD	cmd;
	WORD	key;
	BYTE index;
	gmt_IrPacketState packetStates[MINIMAL_ARRAY_SIZE];
} gmt_Ir;

#ifdef __PARADIGM__
   #pragma pack(pop)
#endif

//******************************************************************************
//  F U N C T I O N    P R O T O T Y P E S
//******************************************************************************

#if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
	WORD far gm_IrInit(gmt_IrProtocol const far *pSt_Protocol, void far *p_Ir);
	void far gm_IrSetCallBack(void (far * IrCallBack)(WORD));
	gmt_RET_STAT far gm_IrGetKey(WORD far *cmd, WORD far *key, BYTE far *index);
#endif

WORD gm_IrIsrRamExeOn(void far* p_ram_exe_buf, WORD size);
void gm_IrIsrRamExeOff(gmt_IrProtocol const far* P_Protocol);
gmt_RET_STAT far gm_IrGetData(WORD *Wp_Data);

#endif // __GM_IR_H__

//*********************************  END  **************************************
