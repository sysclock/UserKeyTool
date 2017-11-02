/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#include <stdio.h>
#include <string.h>

#include "../../IIC Comm/D_IIC0.h"

#include "defs.h"
#include "TypeDefs.h"
#include "Constants.h"
//#include "gpio.h"
//#include "i2c_master_sw.h"
#include "TPI_Regs.h"
#include "TPI_Access.h"
#include "SiI9136EDID.h"
#include "si_apiConfigure.h"
#include "Externals.h"  

//#include "../Device/InputFPGA/InputFPGA.h"
//extern BYTE g_InputEdidData[128];

#ifdef DEV_SUPPORT_EDID

#ifdef CBUS_EDID
#include "CyclopsCbus.h"
#endif

#if (IS_CEC == 1)
extern	uint16_t SI_CecGetDevicePA(void);
#endif

#if (IS_CDC)
extern	bool_t CpCdcInit(void);
#endif

// Constants
//==========

#define EDID_BLOCK_0_OFFSET 0x00
#define EDID_BLOCK_1_OFFSET 0x80

#define EDID_BLOCK_SIZE      128
#define EDID_HDR_NO_OF_FF   0x06
#define NUM_OF_EXTEN_ADDR   0x7E

#define EDID_TAG_ADDR       0x00
#define EDID_REV_ADDR       0x01
#define EDID_TAG_IDX        0x02
#define LONG_DESCR_PTR_IDX  0x02
#define MISC_SUPPORT_IDX    0x03

#define ESTABLISHED_TIMING_INDEX        35      // Offset of Established Timing in EDID block
#define NUM_OF_STANDARD_TIMINGS          8
#define STANDARD_TIMING_OFFSET          38
#define LONG_DESCR_LEN                  18
#define NUM_OF_DETAILED_DESCRIPTORS      4

#define DETAILED_TIMING_OFFSET        0x36

// Offsets within a Long Descriptors Block
//========================================
#define PIX_CLK_OFFSET                   0
#define H_ACTIVE_OFFSET                  2
#define H_BLANKING_OFFSET                3
#define V_ACTIVE_OFFSET                  5
#define V_BLANKING_OFFSET                6
#define H_SYNC_OFFSET                    8
#define H_SYNC_PW_OFFSET                 9
#define V_SYNC_OFFSET                   10
#define V_SYNC_PW_OFFSET                10
#define H_IMAGE_SIZE_OFFSET             12
#define V_IMAGE_SIZE_OFFSET             13
#define H_BORDER_OFFSET                 15
#define V_BORDER_OFFSET                 16
#define FLAGS_OFFSET                    17

#define AR16_10                          0
#define AR4_3                            1
#define AR5_4                            2
#define AR16_9                           3

// Data Block Tag Codes
//=====================
#define AUDIO_D_BLOCK       0x01
#define VIDEO_D_BLOCK       0x02
#define VENDOR_SPEC_D_BLOCK 0x03
#define SPKR_ALLOC_D_BLOCK  0x04
#define USE_EXTENDED_TAG    0x07

// Extended Data Block Tag Codes
//==============================
#define COLORIMETRY_D_BLOCK 0x05

#define HDMI_SIGNATURE_LEN  0x03

#define CEC_PHYS_ADDR_LEN   0x02
#define EDID_EXTENSION_TAG  0x02
#define EDID_REV_THREE      0x03
#define EDID_DATA_START     0x04

#define EDID_BLOCK_0        0x00
#define EDID_BLOCK_2_3      0x01

#define VIDEO_CAPABILITY_D_BLOCK 0x00

static uint8_t SiI9136DefaultabEDIDTabl[ 256 ] = 
{

	//	Read EDID from SiI9187 (HDMI port of Philips 227ELH)
	/*
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,                                               
	0x41, 0x0C, 0x7B, 0xC0, 0x64, 0x06, 0x00, 0x00,                                               
	0x17, 0x16, 0x01, 0x03, 0x80, 0x30, 0x1B, 0x78,                                               
	0x2A, 0x0A, 0x45, 0xA7, 0x54, 0x4A, 0x99, 0x25,                                               
	0x11, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0,                                               
	0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,                                               
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,                                               
	0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,                                               
	0x45, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E,                                 
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x41, 0x55, 0x33,
	0x31, 0x32, 0x32, 0x33, 0x30, 0x30, 0x31, 0x36,

//	0x33, 0x36, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x32,		//	Monitor Name
//	0x32, 0x37, 0x45, 0x4C, 0x48, 0x0A, 0x20, 0x20,
//	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x33, 0x36, 0x00, 0x00, 0x00, 0xFC, 0x00, 'H' ,		//	Monitor Name
	'D' , 'M' , 'I' , '-' , 'S' , 'W' , 'I' , 'T' ,
	'C' , 'H' , 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,

	0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x4D,

	0x02, 0x03, 0x22, 0xF1, 0x4F, 0x01, 0x02, 0x03,
	0x05, 0x06, 0x07, 0x10, 0x11, 0x12, 0x13, 0x14,
	0x15, 0x16, 0x1F, 0x04, 0x23, 0x09, 0x17, 0x07,
	0x83, 0x01, 0x00, 0x00, 0x65, 0x03, 0x0C, 0x00,
	0x10, 0x00, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38,		//	Address 0xA6 - 0xA7
	0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xE0, 0x0C,
	0x11, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A,
	0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00,
	0xE0, 0x0C, 0x11, 0x00, 0x00, 0x18, 0x01, 0x1D,
	0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,
	0x55, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E,
	0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20,
	0x0C, 0x40, 0x55, 0x00, 0xE0, 0x0C, 0x11, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa
*/
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x41, 0x0C, 0x7B, 0xC0, 0x60, 0x06, 0x00, 0x00,
0x17, 0x16, 0x01, 0x03, 0x80, 0x30, 0x1B, 0x78, 0x2A, 0x0A, 0x45, 0xA7, 0x54, 0x4A, 0x99, 0x25,
0x11, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0, 0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
0x45, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x41, 0x55, 0x33,
0x31, 0x32, 0x32, 0x33, 0x30, 0x30, 0x31, 0x36, 0x33, 0x32, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x32,
0x32, 0x37, 0x45, 0x4C, 0x48, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x55,

0x02, 0x03, 0x22, 0xF1, 0x4F, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x10, 0x11, 0x12, 0x13, 0x14,
0x15, 0x16, 0x1F, 0x04, 0x23, 0x09, 0x17, 0x07, 0x83, 0x01, 0x00, 0x00, 0x65, 0x03, 0x0C, 0x00,
0x10, 0x00, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xE0, 0x0C,
0x11, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00,
0xE0, 0x0C, 0x11, 0x00, 0x00, 0x18, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,
0x55, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20,
0x0C, 0x40, 0x55, 0x00, 0xE0, 0x0C, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41
};

#define ReadBlockEDID(a,b,c)            I2C_ReadBlock(EDID_ROM_ADDR, a, c, b)
#define ReadSegmentBlockEDID(a,b,c,d)   I2C_ReadSegmentBlock(EDID_ROM_ADDR, a, b, d, c)

extern	void SI_CecSetDevicePA (uint16_t);

Type_EDID_Descriptors EDID_Data;        // holds parsed EDID data needed by the FW

//byte EDID_TempData[EDID_BLOCK_SIZE + 3];

bool edidDataValid;

static bool DoEDID_Checksum(byte *);
static bool CheckEDID_Header(byte *);
static void ParseBlock_0_TimingDescripors(byte *);
static byte Parse861Extensions(byte);
static byte Parse861ShortDescriptors(byte *);
static byte Parse861LongDescriptors(byte *);

#if (CONF__TPI_EDID_PRINT == ENABLE)
static void ParseEstablishedTiming(byte *);
static void ParseStandardTiming(byte *);
static bool ParseDetailedTiming(byte *, byte, byte);
#endif

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   byte SiI_ParseEDID()
//
// PURPOSE      :   Extract sink properties from its EDID file and save them in
//                  global structure EDID_Data.
//
// INPUT PARAMS :   Pointer to start of EDID 0
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   EDID_Data
//
// RETURNS      :   A byte error code to indicates success or error type.
//
// NOTE         :   Fields that are not supported by the 9022/4 (such as deep
//                  color) were not parsed.
//
//////////////////////////////////////////////////////////////////////////////

byte SiI_ParseEDID (byte *pEdid, byte *numExt)
{
	byte i, j, k;

	k = k;								// dummy set
	TPI_EDID_PRINT(("\r\n"));
	TPI_EDID_PRINT(("EDID DATA (Segment = 0 Block = 0 Offset = %d):\r\n", (int) EDID_BLOCK_0_OFFSET));

	for (j = 0, i = 0; j < 128; j++)
	{
		k = pEdid[j];
		TPI_EDID_PRINT(("%2.2X ", (int) k));
		i++;

		if (i == 0x10)
		{
			TPI_EDID_PRINT(("\r\n"));
			i = 0;
			}
		}
	TPI_EDID_PRINT(("\r\n"));

	if (!CheckEDID_Header(pEdid))
	{
		// first 8 bytes of EDID must be {0, FF, FF, FF, FF, FF, FF, 0}
		TPI_DEBUG_PRINT(("EDID -> Incorrect Header\r\n"));
		return EDID_INCORRECT_HEADER;
	}

	if (!DoEDID_Checksum(pEdid))
	{
		// non-zero EDID checksum
		TPI_DEBUG_PRINT(("EDID -> Checksum Error\r\n"));
		return EDID_CHECKSUM_ERROR;
	}

	ParseBlock_0_TimingDescripors(pEdid);			// Parse EDID Block #0 Desctiptors

	*numExt = pEdid[NUM_OF_EXTEN_ADDR];	// read # of extensions from offset 0x7E of block 0
	TPI_EDID_PRINT(("EDID -> No 861 Extensions = %d\r\n", (int) *numExt));

	if (!(*numExt))
	{
		// No extensions to worry about
		return EDID_NO_861_EXTENSIONS;
	}

	//return Parse861Extensions(NumOfExtensions);			// Parse 861 Extensions (short and long descriptors);
	return (EDID_OK);
}


byte DoEdidRead (void)
{
	byte SysCtrlReg;
	byte Result, *pData = NULL;
	byte NumOfExtensions;

	// If we already have valid EDID data, ship this whole thing
	if (edidDataValid == FALSE)
	{
		// Request access to DDC bus from the receiver
		if (GetDDC_Access(&SysCtrlReg))
		{
#ifdef CBUS_EDID
			CyclopsReadEdid01(EDID_BLOCK_0_OFFSET, EDID_BLOCK_SIZE);
#else
//			ReadBlockEDID(EDID_BLOCK_0_OFFSET, EDID_BLOCK_SIZE, g_CommData);		// read first 128 bytes of EDID ROM
			pData = I2C0ReadByte(EDID_ROM_ADDR, EDID_BLOCK_0_OFFSET, I2C_DEV_MEMORY_ADDRESS_8BIT, EDID_BLOCK_SIZE);
			memset((void*) g_CommData, 0, sizeof((void*) g_CommData));
			if(pData != NULL)
				memcpy(g_CommData, pData, EDID_BLOCK_SIZE);
			else
				g_CommData[0] = 0xFF;
#endif
			#if 1
			// HL Add
			if(g_CommData[0] != 0x00)
			{
				TPI_DEBUG_PRINT (("DDC Can Not Read EDID, Use Default EDID!\r\n"));
				Result = SiI_ParseEDID(SiI9136DefaultabEDIDTabl, &NumOfExtensions);
//				InputFPGA_SetCustomEdid(94);
//				Result = SiI_ParseEDID(g_InputEdidData, &NumOfExtensions);
			}
			else
				Result = SiI_ParseEDID(g_CommData, &NumOfExtensions);
			#else
			Result = SiI_ParseEDID(g_CommData, &NumOfExtensions);
			#endif
			if (Result != EDID_OK)
			{
				if (Result == EDID_NO_861_EXTENSIONS)
				{
					EDID_Data.HDMI_Sink = FALSE;
					TPI_DEBUG_PRINT (("EDID -> No 861 Extensions\r\n"));
				}
				else
				{
					TPI_DEBUG_PRINT (("EDID -> Parse FAILED\r\n"));
				}
			}
			else
			{
				TPI_DEBUG_PRINT (("EDID -> Parse OK\r\n"));
				Result = Parse861Extensions(NumOfExtensions);		// Parse 861 Extensions (short and long descriptors);
				if (Result != EDID_OK)
				{
					TPI_DEBUG_PRINT(("EDID -> Extension Parse FAILED\r\n"));
				}
			}

			if (!ReleaseDDC(SysCtrlReg))				// Host must release DDC bus once it is done reading EDID
			{
				TPI_DEBUG_PRINT (("EDID -> DDC bus release failed\r\n"));
				return EDID_DDC_BUS_RELEASE_FAILURE;
			}
		}
		else
		{
			TPI_DEBUG_PRINT (("EDID -> DDC bus request failed\r\n"));
			return EDID_DDC_BUS_REQ_FAILURE;
		}

		edidDataValid = TRUE;
	}
	return EDID_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   ParseBlock_0_TimingDescripors()
//
// PURPOSE      :   Parse EDID Block 0 timing descriptors per EEDID 1.3
//                  standard. printf() values to screen.
//
// INPUT PARAMS :   Pointer to the 128 byte array where the data read from EDID
//                  Block0 is stored.
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   Void
//
//////////////////////////////////////////////////////////////////////////////

static void ParseBlock_0_TimingDescripors(byte *Data)
{
#if (CONF__TPI_EDID_PRINT == ENABLE)
    byte i;
    byte Offset;

    ParseEstablishedTiming(Data);
    ParseStandardTiming(Data);


    for (i = 0; i < NUM_OF_DETAILED_DESCRIPTORS; i++)
    {
        Offset = DETAILED_TIMING_OFFSET + (LONG_DESCR_LEN * i);
        ParseDetailedTiming(Data, Offset, EDID_BLOCK_0);
    }
#else
        Data = Data;    // Dummy usage to suppress warning
#endif
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   byte Parse861Extensions()
//
// PURPOSE      :   Parse CEA-861 extensions from EDID ROM (EDID blocks beyond
//                  block #0). Save short descriptors in global structure
//                  EDID_Data. printf() long descriptors to the screen.
//
// INPUT PARAMS :   The number of extensions in the EDID being parsed
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   EDID_PARSED_OK if EDID parsed correctly. Error code if failed.
//
// NOTE         :   Fields that are not supported by the 9022/4 (such as deep
//                  color) were not parsed.
//
//////////////////////////////////////////////////////////////////////////////

static byte Parse861Extensions(byte NumOfExtensions)
{
    byte i,j,k;

    byte ErrCode;

    byte V_DescriptorIndex = 0;
    byte A_DescriptorIndex = 0;

    byte Segment = 0;
    byte Block = 0;
    byte Offset = 0, *pData = NULL;

    EDID_Data.HDMI_Sink = FALSE;

		k = k;			// dummy set
		V_DescriptorIndex = V_DescriptorIndex;
		A_DescriptorIndex = A_DescriptorIndex;
		Offset = Offset;
    do
    {
        Block++;

        Offset = 0;
        if ((Block % 2) > 0)
        {
            Offset = EDID_BLOCK_SIZE;
        }

        Segment = (byte) (Block / 2);

        if (Block == 1)
        {
#ifdef CBUS_EDID
            CyclopsReadEdid01(EDID_BLOCK_1_OFFSET, EDID_BLOCK_SIZE);
#else
            //ReadBlockEDID(EDID_BLOCK_1_OFFSET, EDID_BLOCK_SIZE, g_CommData);    // read first 128 bytes of EDID ROM
						pData = I2C0ReadByte(EDID_ROM_ADDR, EDID_BLOCK_1_OFFSET, I2C_DEV_MEMORY_ADDRESS_8BIT, EDID_BLOCK_SIZE);
						memset((void*) g_CommData, 0, sizeof((void*) g_CommData));
						if(pData != NULL)
							memcpy(g_CommData, pData, EDID_BLOCK_SIZE);
					/*
						// HL Add, if enable DVI Can not display
						if(g_CommData[0] == 0xFF)
							memcpy(g_CommData, &SiI9136DefaultabEDIDTabl[EDID_BLOCK_1_OFFSET], EDID_BLOCK_SIZE);
					*/
#endif
        }
        else
        {
#ifdef CBUS_EDID
            CyclopsReadEdid23(Offset, EDID_BLOCK_SIZE);
#else
            //ReadSegmentBlockEDID(Segment, Offset, EDID_BLOCK_SIZE, g_CommData);     // read next 128 bytes of EDID ROM
						pData = I2C0ReadByte(EDID_ROM_ADDR, EDID_BLOCK_1_OFFSET + Segment * EDID_BLOCK_SIZE * 2, I2C_DEV_MEMORY_ADDRESS_8BIT, EDID_BLOCK_SIZE);
						memset((void*) g_CommData, 0, sizeof((void*) g_CommData));
						if(pData != NULL)
							memcpy(g_CommData, pData, EDID_BLOCK_SIZE);
					/*
						if(g_CommData[0] == 0xFF)
							memcpy(g_CommData, &SiI9136DefaultabEDIDTabl[EDID_BLOCK_1_OFFSET + Segment * EDID_BLOCK_SIZE * 2], EDID_BLOCK_SIZE);
					*/
#endif
        }

        TPI_EDID_PRINT(("\r\n"));
        TPI_EDID_PRINT(("EDID DATA (Segment = %d Block = %d Offset = %d):\r\n", (int) Segment, (int) Block, (int) Offset));
        for (j=0, i=0; j<128; j++)
        {
            k = g_CommData[j];
            TPI_EDID_PRINT(("%2.2X ", (int) k));
            i++;

            if (i == 0x10)
            {
                TPI_EDID_PRINT(("\r\n"));
                i = 0;
            }
        }
        TPI_EDID_PRINT(("\r\n"));

        if ((NumOfExtensions > 1) && (Block == 1))
        {
            continue;
        }

        ErrCode = Parse861ShortDescriptors(g_CommData);
        if (ErrCode != EDID_SHORT_DESCRIPTORS_OK)
        {
            return ErrCode;
        }

        ErrCode = Parse861LongDescriptors(g_CommData);
        if (ErrCode != EDID_LONG_DESCRIPTORS_OK)
        {
            return ErrCode;
        }

    } while (Block < NumOfExtensions);

    return EDID_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   byte Parse861ShortDescriptors()
//
// PURPOSE      :   Parse CEA-861 extension short descriptors of the EDID block
//                  passed as a parameter and save them in global structure
//                  EDID_Data.
//
// INPUT PARAMS :   A pointer to the EDID 861 Extension block being parsed.
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   EDID_Data
//
// RETURNS      :   EDID_PARSED_OK if EDID parsed correctly. Error code if failed.
//
// NOTE         :   Fields that are not supported by the 9022/4 (such as deep
//                  color) are not parsed.
//
//////////////////////////////////////////////////////////////////////////////

byte Parse861ShortDescriptors(byte *Data)
{
    byte LongDescriptorOffset;
    byte DataBlockLength;
    byte DataIndex;
    byte ExtendedTagCode;
	byte VSDB_BaseOffset = 0;

    byte V_DescriptorIndex = 0;  // static to support more than one extension
    byte A_DescriptorIndex = 0;  // static to support more than one extension

    byte TagCode;

    byte i;
    byte j;

    if (Data[EDID_TAG_ADDR] != EDID_EXTENSION_TAG)
    {
        TPI_EDID_PRINT(("EDID -> Extension Tag Error\r\n"));
        return EDID_EXT_TAG_ERROR;
    }

    if (Data[EDID_REV_ADDR] != EDID_REV_THREE)
    {
        TPI_EDID_PRINT(("EDID -> Revision Error\r\n"));
        return EDID_REV_ADDR_ERROR;
    }

    LongDescriptorOffset = Data[LONG_DESCR_PTR_IDX];    // block offset where long descriptors start

    EDID_Data.UnderScan = ((Data[MISC_SUPPORT_IDX]) >> 7) & LSBIT;  // byte #3 of CEA extension version 3
    EDID_Data.BasicAudio = ((Data[MISC_SUPPORT_IDX]) >> 6) & LSBIT;
    EDID_Data.YCbCr_4_4_4 = ((Data[MISC_SUPPORT_IDX]) >> 5) & LSBIT;
    EDID_Data.YCbCr_4_2_2 = ((Data[MISC_SUPPORT_IDX]) >> 4) & LSBIT;

    DataIndex = EDID_DATA_START;            // 4

    while (DataIndex < LongDescriptorOffset)
    {
        TagCode = (Data[DataIndex] >> 5) & THREE_LSBITS;
        DataBlockLength = Data[DataIndex++] & FIVE_LSBITS;
        if ((DataIndex + DataBlockLength) > LongDescriptorOffset)
        {
            TPI_EDID_PRINT(("EDID -> V Descriptor Overflow\r\n"));
            return EDID_V_DESCR_OVERFLOW;
        }

        i = 0;                                  // num of short video descriptors in current data block

        switch (TagCode)
        {
            case VIDEO_D_BLOCK:
                while ((i < DataBlockLength) && (i < MAX_V_DESCRIPTORS))        // each SVD is 1 byte long
                {
                    EDID_Data.VideoDescriptor[V_DescriptorIndex++] = Data[DataIndex++];
                    i++;
                }
                DataIndex += DataBlockLength - i;   // if there are more STDs than MAX_V_DESCRIPTORS, skip the last ones. Update DataIndex

                TPI_EDID_PRINT(("EDID -> Short Descriptor Video Block\r\n"));
                break;

            case AUDIO_D_BLOCK:
                while (i < DataBlockLength/3)       // each SAD is 3 bytes long
                {
                    j = 0;
                    while (j < AUDIO_DESCR_SIZE)    // 3
                    {
                        EDID_Data.AudioDescriptor[A_DescriptorIndex][j++] = Data[DataIndex++];
                    }
                    A_DescriptorIndex++;
                    i++;
                }
                TPI_EDID_PRINT(("EDID -> Short Descriptor Audio Block\r\n"));
                break;

            case  SPKR_ALLOC_D_BLOCK:
                EDID_Data.SpkrAlloc[i++] = Data[DataIndex++];       // although 3 bytes are assigned to Speaker Allocation, only
                DataIndex += 2;                                     // the first one carries information, so the next two are ignored by this code.
                TPI_EDID_PRINT(("EDID -> Short Descriptor Speaker Allocation Block\r\n"));
                break;

            case USE_EXTENDED_TAG:
                ExtendedTagCode = Data[DataIndex++];

                switch (ExtendedTagCode)
                {
                    case VIDEO_CAPABILITY_D_BLOCK:

                                TPI_EDID_PRINT(("EDID -> Short Descriptor Video Capability Block\r\n"));

                                                // TO BE ADDED HERE: Save "video capability" parameters in EDID_Data data structure
                                                // Need to modify that structure definition
                                                // In the meantime: just increment DataIndex by 1

                                                DataIndex += 1;    // replace with reading and saving the proper data per CEA-861 sec. 7.5.6 while incrementing DataIndex

                                                break;


                    case COLORIMETRY_D_BLOCK:
                        EDID_Data.ColorimetrySupportFlags = Data[DataIndex++] & BITS_1_0;
                        EDID_Data.MetadataProfile = Data[DataIndex++] & BITS_2_1_0;

                                TPI_EDID_PRINT(("EDID -> Short Descriptor Colorimetry Block\r\n"));

                                                break;
                }
                                break;

            case VENDOR_SPEC_D_BLOCK:
                VSDB_BaseOffset = DataIndex - 1;

                if ((Data[DataIndex++] == 0x03) &&    // check if sink is HDMI compatible
                    (Data[DataIndex++] == 0x0C) &&
                    (Data[DataIndex++] == 0x00))

                    EDID_Data.HDMI_Sink = TRUE;
                else
                    EDID_Data.HDMI_Sink = FALSE;

                EDID_Data.CEC_A_B = Data[DataIndex++];  // CEC Physical address
                EDID_Data.CEC_C_D = Data[DataIndex++];

#if (IS_CEC == 1)
				// Take the Address that was passed in the EDID and use this API
				// to set the physical address for CEC.
				{
					uint16_t	phyAddr;
					phyAddr = (uint16_t)EDID_Data.CEC_C_D;	 // Low-order nibbles
					phyAddr |= ((uint16_t)EDID_Data.CEC_A_B << 8); // Hi-order nibbles
					// Is the new PA different from the current PA?
					if (phyAddr != SI_CecGetDevicePA ())
					{
						// Yes!  So change the PA
						SI_CecSetDevicePA (phyAddr);
#if (IS_CDC == 1)
						// If we have CDC, it needs to be re-inited because the PA has changed
						// NOTE: This is a callback into device specific app code in Evita.  Maybe
						// later boards with have a different init func to call.  You are warned.
						CpCdcInit();
#endif // IS_CDC
					}
				}
#endif // IS_CEC

                if ((DataIndex + 7) > VSDB_BaseOffset + DataBlockLength)        // Offset of 3D_Present bit in VSDB
                        EDID_Data._3D_Supported = FALSE;
                else if (Data[DataIndex + 7] >> 7)
                        EDID_Data._3D_Supported = TRUE;
                else
                        EDID_Data._3D_Supported = FALSE;

                DataIndex += DataBlockLength - HDMI_SIGNATURE_LEN - CEC_PHYS_ADDR_LEN; // Point to start of next block
                TPI_EDID_PRINT(("EDID -> Short Descriptor Vendor Block\r\n"));
                TPI_EDID_PRINT(("\r\n"));
                break;

            default:
                TPI_EDID_PRINT(("EDID -> Unknown Tag Code\r\n"));
                return EDID_UNKNOWN_TAG_CODE;

        }                   // End, Switch statement
    }                       // End, while (DataIndex < LongDescriptorOffset) statement

    return EDID_SHORT_DESCRIPTORS_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   byte Parse861LongDescriptors()
//
// PURPOSE      :   Parse CEA-861 extension long descriptors of the EDID block
//                  passed as a parameter and printf() them to the screen.
//                  EDID_Data.
//
// INPUT PARAMS :   A pointer to the EDID block being parsed
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   An error code if no long descriptors found; EDID_PARSED_OK
//                  if descriptors found.
//
//////////////////////////////////////////////////////////////////////////////
byte Parse861LongDescriptors(byte *Data)
{
    byte LongDescriptorsOffset;
    byte DescriptorNum = 1;

    LongDescriptorsOffset = Data[LONG_DESCR_PTR_IDX];   // EDID block offset 2 holds the offset

    if (!LongDescriptorsOffset)                         // per CEA-861-D, table 27
    {
        TPI_DEBUG_PRINT(("EDID -> No Detailed Descriptors\r\n"));
        return EDID_NO_DETAILED_DESCRIPTORS;
    }

    // of the 1st 18-byte descriptor
    while (LongDescriptorsOffset + LONG_DESCR_LEN < EDID_BLOCK_SIZE)
    {
        TPI_EDID_PRINT(("Parse Results - CEA-861 Extension Block #%d, Long Descriptor #%d:\r\n", (int) LongDescriptorsOffset, (int) DescriptorNum));
        TPI_EDID_PRINT(("===============================================================\r\n"));

#if (CONF__TPI_EDID_PRINT == ENABLE)
        if (!ParseDetailedTiming(Data, LongDescriptorsOffset, EDID_BLOCK_2_3))
                        break;
#endif
        LongDescriptorsOffset +=  LONG_DESCR_LEN;
        DescriptorNum++;
    }

    return EDID_LONG_DESCRIPTORS_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   bool DoEDID_Checksum()
//
// PURPOSE      :   Calculte checksum of the 128 byte block pointed to by the
//                  pointer passed as parameter
//
// INPUT PARAMS :   Pointer to a 128 byte block whose checksum needs to be
//                  calculated
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   TRUE if chcksum is 0. FALSE if not.
//
//////////////////////////////////////////////////////////////////////////////

bool DoEDID_Checksum(byte *Block)
{
    byte i;
    byte CheckSum = 0;

    for (i = 0; i < EDID_BLOCK_SIZE; i++)
        CheckSum += Block[i];

    if (CheckSum)
        return FALSE;

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   bool CheckEDID_Header()
//
// PURPOSE      :   Checks if EDID header is correct per VESA E-EDID standard
//
// INPUT PARAMS :   Pointer to 1st EDID block
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   TRUE if Header is correct. FALSE if not.
//
//////////////////////////////////////////////////////////////////////////////

static bool CheckEDID_Header(byte *Block)
{
    byte i = 0;

    if (Block[i])               // byte 0 must be 0
        return FALSE;

    for (i = 1; i < 1 + EDID_HDR_NO_OF_FF; i++)
    {
        if(Block[i] != 0xFF)    // bytes [1..6] must be 0xFF
            return FALSE;
    }

    if (Block[i])               // byte 7 must be 0
        return FALSE;

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   ParseEstablishedTiming()
//
// PURPOSE      :   Parse the established timing section of EDID Block 0 and
//                  print their decoded meaning to the screen.
//
// INPUT PARAMS :   Pointer to the array where the data read from EDID
//                  Block0 is stored.
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   Void
//
//////////////////////////////////////////////////////////////////////////////

#if (CONF__TPI_EDID_PRINT == ENABLE)
static void ParseEstablishedTiming(byte *Data)
{
    TPI_EDID_PRINT(("Parsing Established Timing:\r\n"));
    TPI_EDID_PRINT(("===========================\r\n"));


    // Parse Established Timing Byte #0

    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_7)
        TPI_EDID_PRINT(("720 x 400 @ 70Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_6)
        TPI_EDID_PRINT(("720 x 400 @ 88Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_5)
        TPI_EDID_PRINT(("640 x 480 @ 60Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_4)
        TPI_EDID_PRINT(("640 x 480 @ 67Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_3)
        TPI_EDID_PRINT(("640 x 480 @ 72Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_2)
        TPI_EDID_PRINT(("640 x 480 @ 75Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_1)
        TPI_EDID_PRINT(("800 x 600 @ 56Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX] & BIT_0)
        TPI_EDID_PRINT(("800 x 400 @ 60Hz\r\n"));

    // Parse Established Timing Byte #1:

    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_7)
        TPI_EDID_PRINT(("800 x 600 @ 72Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_6)
        TPI_EDID_PRINT(("800 x 600 @ 75Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_5)
        TPI_EDID_PRINT(("832 x 624 @ 75Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_4)
        TPI_EDID_PRINT(("1024 x 768 @ 87Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_3)
        TPI_EDID_PRINT(("1024 x 768 @ 60Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_2)
        TPI_EDID_PRINT(("1024 x 768 @ 70Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_1)
        TPI_EDID_PRINT(("1024 x 768 @ 75Hz\r\n"));
    if(Data[ESTABLISHED_TIMING_INDEX + 1]  & BIT_0)
        TPI_EDID_PRINT(("1280 x 1024 @ 75Hz\r\n"));

    // Parse Established Timing Byte #2:

    if(Data[ESTABLISHED_TIMING_INDEX + 2] & 0x80)
        TPI_EDID_PRINT(("1152 x 870 @ 75Hz\r\n"));

    if((!Data[0])&&(!Data[ESTABLISHED_TIMING_INDEX + 1]  )&&(!Data[2]))
        TPI_EDID_PRINT(("No established video modes\r\n"));
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   ParseStandardTiming()
//
// PURPOSE      :   Parse the standard timing section of EDID Block 0 and
//                  print their decoded meaning to the screen.
//
// INPUT PARAMS :   Pointer to the array where the data read from EDID
//                  Block0 is stored.
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   Void
//
//////////////////////////////////////////////////////////////////////////////

static void ParseStandardTiming(byte *Data)
{
    byte i;
    byte AR_Code;

    TPI_EDID_PRINT(("Parsing Standard Timing:\r\n"));
    TPI_EDID_PRINT(("========================\r\n"));

    for (i = 0; i < NUM_OF_STANDARD_TIMINGS; i += 2)
    {
        if ((Data[STANDARD_TIMING_OFFSET + i] == 0x01) && ((Data[STANDARD_TIMING_OFFSET + i +1]) == 1))
        {
            TPI_EDID_PRINT(("Standard Timing Undefined\r\n")); // per VESA EDID standard, Release A, Revision 1, February 9, 2000, Sec. 3.9
                }
        else
        {
            TPI_EDID_PRINT(("Horizontal Active pixels: %i\r\n", (int)((Data[STANDARD_TIMING_OFFSET + i] + 31)*8)));    // per VESA EDID standard, Release A, Revision 1, February 9, 2000, Table 3.15

            AR_Code = (Data[STANDARD_TIMING_OFFSET + i +1] & TWO_MSBITS) >> 6;
            TPI_EDID_PRINT(("Aspect Ratio: "));

            switch(AR_Code)
            {
                case AR16_10:
                    TPI_EDID_PRINT(("16:10\r\n"));
                    break;

                case AR4_3:
                    TPI_EDID_PRINT(("4:3\r\n"));
                    break;

                case AR5_4:
                    TPI_EDID_PRINT(("5:4\r\n"));
                    break;

                case AR16_9:
                    TPI_EDID_PRINT(("16:9\r\n"));
                    break;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   ParseDetailedTiming()
//
// PURPOSE      :   Parse the detailed timing section of EDID Block 0 and
//                  print their decoded meaning to the screen.
//
// INPUT PARAMS :   Pointer to the array where the data read from EDID
//                  Block0 is stored.
//
//                  Offset to the beginning of the Detailed Timing Descriptor
//                  data.
//
//                                      Block indicator to distinguish between block #0 and blocks
//                                      #2, #3
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   None
//
// RETURNS      :   Void
//
//////////////////////////////////////////////////////////////////////////////

static bool ParseDetailedTiming(byte *Data, byte DetailedTimingOffset, byte Block)
{
        byte TmpByte;
        byte i;
        word TmpWord;

        TmpWord = Data[DetailedTimingOffset + PIX_CLK_OFFSET] +
                    256 * Data[DetailedTimingOffset + PIX_CLK_OFFSET + 1];

        if (TmpWord == 0x00)            // 18 byte partition is used as either for Monitor Name or for Monitor Range Limits or it is unused
        {
                if (Block == EDID_BLOCK_0)      // if called from Block #0 and first 2 bytes are 0 => either Monitor Name or for Monitor Range Limits
            {
                if (Data[DetailedTimingOffset + 3] == 0xFC) // these 13 bytes are ASCII coded monitor name
                {
                                        TPI_EDID_PRINT(("Monitor Name: "));

                        for (i = 0; i < 13; i++)
                                        {
                        TPI_EDID_PRINT(("%c", Data[DetailedTimingOffset + 5 + i])); // Display monitor name on SiIMon
                                        }
                                        TPI_EDID_PRINT(("\r\n"));
                }

                        else if (Data[DetailedTimingOffset + 3] == 0xFD) // these 13 bytes contain Monitor Range limits, binary coded
                        {
                                TPI_EDID_PRINT(("Monitor Range Limits:\r\n\r\n"));

                                i = 0;
                                TPI_EDID_PRINT(("Min Vertical Rate in Hz: %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                        TPI_EDID_PRINT(("Max Vertical Rate in Hz: %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                                TPI_EDID_PRINT(("Min Horizontal Rate in Hz: %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                                TPI_EDID_PRINT(("Max Horizontal Rate in Hz: %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                                TPI_EDID_PRINT(("Max Supported pixel clock rate in MHz/10: %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                        TPI_EDID_PRINT(("Tag for secondary timing formula (00h=not used): %d\r\n", (int) Data[DetailedTimingOffset + 5 + i++])); //
                                TPI_EDID_PRINT(("Min Vertical Rate in Hz %d\r\n", (int) Data[DetailedTimingOffset + 5 + i])); //
                                        TPI_EDID_PRINT(("\r\n"));
                        }
                        }

                else if (Block == EDID_BLOCK_2_3)                          // if called from block #2 or #3 and first 2 bytes are 0x00 (padding) then this
                {                                                                                          // descriptor partition is not used and parsing should be stopped
                                TPI_EDID_PRINT(("No More Detailed descriptors in this block\r\n"));
                                TPI_EDID_PRINT(("\r\n"));
                                return FALSE;
                        }
                }

                else                                            // first 2 bytes are not 0 => this is a detailed timing descriptor from either block
                {
                        if((Block == EDID_BLOCK_0) && (DetailedTimingOffset == 0x36))
                        {
                        TPI_EDID_PRINT(("\r\n\r\n\r\nParse Results, EDID Block #0, Detailed Descriptor Number 1:\r\n"));
                                TPI_EDID_PRINT(("===========================================================\r\n\r\n"));
                        }
                        else if((Block == EDID_BLOCK_0) && (DetailedTimingOffset == 0x48))
                {
                        TPI_EDID_PRINT(("\r\n\r\n\r\nParse Results, EDID Block #0, Detailed Descriptor Number 2:\r\n"));
                                TPI_EDID_PRINT(("===========================================================\r\n\r\n"));
                        }

            TPI_EDID_PRINT(("Pixel Clock (MHz * 100): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + H_ACTIVE_OFFSET] +
                    256 * ((Data[DetailedTimingOffset + H_ACTIVE_OFFSET + 2] >> 4) & FOUR_LSBITS);
            TPI_EDID_PRINT(("Horizontal Active Pixels: %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + H_BLANKING_OFFSET] +
                    256 * (Data[DetailedTimingOffset + H_BLANKING_OFFSET + 1] & FOUR_LSBITS);
            TPI_EDID_PRINT(("Horizontal Blanking (Pixels): %d\r\n", (int)TmpWord));

            TmpWord = (Data[DetailedTimingOffset + V_ACTIVE_OFFSET] )+
                    256 * ((Data[DetailedTimingOffset + (V_ACTIVE_OFFSET) + 2] >> 4) & FOUR_LSBITS);
            TPI_EDID_PRINT(("Vertical Active (Lines): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + V_BLANKING_OFFSET] +
                    256 * (Data[DetailedTimingOffset + V_BLANKING_OFFSET + 1] & LOW_NIBBLE);
            TPI_EDID_PRINT(("Vertical Blanking (Lines): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + H_SYNC_OFFSET] +
                    256 * ((Data[DetailedTimingOffset + (H_SYNC_OFFSET + 3)] >> 6) & TWO_LSBITS);
            TPI_EDID_PRINT(("Horizontal Sync Offset (Pixels): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + H_SYNC_PW_OFFSET] +
                    256 * ((Data[DetailedTimingOffset + (H_SYNC_PW_OFFSET + 2)] >> 4) & TWO_LSBITS);
            TPI_EDID_PRINT(("Horizontal Sync Pulse Width (Pixels): %d\r\n", (int)TmpWord));

            TmpWord = (Data[DetailedTimingOffset + V_SYNC_OFFSET] >> 4) & FOUR_LSBITS +
                    256 * ((Data[DetailedTimingOffset + (V_SYNC_OFFSET + 1)] >> 2) & TWO_LSBITS);
            TPI_EDID_PRINT(("Vertical Sync Offset (Lines): %d\r\n", (int)TmpWord));

            TmpWord = (Data[DetailedTimingOffset + V_SYNC_PW_OFFSET]) & FOUR_LSBITS +
                    256 * (Data[DetailedTimingOffset + (V_SYNC_PW_OFFSET + 1)] & TWO_LSBITS);
            TPI_EDID_PRINT(("Vertical Sync Pulse Width (Lines): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + H_IMAGE_SIZE_OFFSET] +
                    256 * (((Data[DetailedTimingOffset + (H_IMAGE_SIZE_OFFSET + 2)]) >> 4) & FOUR_LSBITS);
            TPI_EDID_PRINT(("Horizontal Image Size (mm): %d\r\n", (int)TmpWord));

            TmpWord = Data[DetailedTimingOffset + V_IMAGE_SIZE_OFFSET] +
                    256 * (Data[DetailedTimingOffset + (V_IMAGE_SIZE_OFFSET + 1)] & FOUR_LSBITS);
            TPI_EDID_PRINT(("Vertical Image Size (mm): %d\r\n", (int)TmpWord));

            TmpByte = Data[DetailedTimingOffset + H_BORDER_OFFSET];
            TPI_EDID_PRINT(("Horizontal Border (Pixels): %d\r\n", (int)TmpByte));

            TmpByte = Data[DetailedTimingOffset + V_BORDER_OFFSET];
            TPI_EDID_PRINT(("Vertical Border (Lines): %d\r\n", (int)TmpByte));

            TmpByte = Data[DetailedTimingOffset + FLAGS_OFFSET];
            if (TmpByte & BIT_7)
                TPI_EDID_PRINT(("Interlaced\r\n"));
            else
                TPI_EDID_PRINT(("Non-Interlaced\r\n"));

            if (!(TmpByte & BIT_5) && !(TmpByte & BIT_6))
                TPI_EDID_PRINT(("Normal Display, No Stereo\r\n"));
            else
                TPI_EDID_PRINT(("Refer to VESA E-EDID Release A, Revision 1, table 3.17\r\n"));

            if (!(TmpByte & BIT_3) && !(TmpByte & BIT_4))
                TPI_EDID_PRINT(("Analog Composite\r\n"));
            if ((TmpByte & BIT_3) && !(TmpByte & BIT_4))
                TPI_EDID_PRINT(("Bipolar Analog Composite\r\n"));
            else if (!(TmpByte & BIT_3) && (TmpByte & BIT_4))
                TPI_EDID_PRINT(("Digital Composite\r\n"));

            else if ((TmpByte & BIT_3) && (TmpByte & BIT_4))
                TPI_EDID_PRINT(("Digital Separate\r\n"));

                        TPI_EDID_PRINT(("\r\n"));
        }
        return TRUE;
}
#endif
/****************************************************************************/
/*																			*/
/* FUNCTION:    SiI9136EDID_DummyCall(void)                                      */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*							being called.												*/
/*																			*/
/****************************************************************************/
void SiI9136EDID_DummyCall(void)
{
#if (CONF__TPI_EDID_PRINT == ENABLE)
	ParseEstablishedTiming(NULL);
	ParseStandardTiming(NULL);
	ParseDetailedTiming(NULL, 0, 0);
#endif
}

#endif
/********************************  END  *************************************/
