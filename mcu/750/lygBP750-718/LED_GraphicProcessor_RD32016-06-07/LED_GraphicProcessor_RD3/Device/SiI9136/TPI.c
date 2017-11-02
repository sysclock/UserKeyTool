/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

// Standard C Library
#include <stdio.h>
#include "Timer.h"
#include "D_IIC1.h"
#include "D_IIC2.h"
#include "D_IIC3.h"

// Needs to be rearranged and minimized
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

// Serial Communication Buffer assignments and macros
#define CommandLength				g_CommData[1]
#define Arg(a)                      g_CommData[(2+a)]

// May be empty depending on project definitions
#include "SiI9136EDID.h"
#include "HDCP.h"

#ifdef F_9136
#include "ehdmi.h"
#endif

extern void InitSamabeth(void); //%%%%%%%%%%%%%%%%

#if defined SiI9232_OR_SiI9236
void InitCBusRegs(void);
#endif

#if (IS_CEC == 1)
extern bool SI_CecInit (void);
extern byte SI_CecHandler (byte, bool);
#endif

static void TxHW_Reset (void);

static void WakeUpFromD3 (void);
static void InitializeStateVariables (void);

static void HotPlugService (void);

static bool EnableInterrupts (byte);
static bool DisableInterrupts (byte);

static bool SetPreliminaryInfoFrames (void);

#ifdef MHD_CABLE_HPD
static void InitForceUsbIdSwitchOpen (void);
static void InitReleaseUsbIdSwitchOpen (void);

static void ForceUsbIdSwitchOpen (void);
static void ReleaseUsbIdSwitchOpen (void);

static void OnMobileHdCableConnected (void);
static void OnMobileHdCableDisconnected (void);
#endif

static void OnHdmiCableConnected (void);
static void OnHdmiCableDisconnected (void);

static void OnDownstreamRxPoweredDown (void);
static void OnDownstreamRxPoweredUp (void);

static void TxPowerStateD0 (void);
static void TxPowerStateD3 (void);

#ifdef RCP_ENABLE
extern void CpCbusInitialize(void);
extern void CpCbusHandler(void);
#endif


#define T_EN_TPI       	10
#define T_HPD_DELAY    	10

byte txPowerState;		// Can be referenced externally by chip-specific TX HAL file, so cannot be static.
byte OutputHaveSet = 0;
bool tmdsPoweredUp;
bool hdmiCableConnected;
bool dsRxPoweredUp;
bool mobileHdCableConnected;

#ifdef MHD_CABLE_HPD

#define ENABLE_AUTO_SOFT_RESET			0x04
#define DISABLE_AUTO_SOFT_RESET			0x00
#define ASR_VALUE						ENABLE_AUTO_SOFT_RESET

#define DDC_XLTN_TIMEOUT_MAX_VAL		0x30

bool tclkStable;
bool mobileHdCableConnected;
bool checkTclkStable;
byte cbusRevID;
#endif

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : TPI_Init()
//
// PURPOSE       : TPI initialization: HW Reset, Interrupt enable.
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS      :   TRUE
//
//////////////////////////////////////////////////////////////////////////////
bool TPI_Init(void)
{
	int i = 0;
	TPI_TRACE_PRINT((">>TPI_Init()\r\n"));

	TPI_DEBUG_PRINT(("TPI Firmware Version "));
	for (i=0; i < (int)sizeof(TPI_FW_VERSION); i++)
	{
		TPI_DEBUG_PRINT(("%c", TPI_FW_VERSION[i]));
	}
	TPI_DEBUG_PRINT(("\r\n"));

	txPowerState = TX_POWER_STATE_D0;				// Chip powers up in D2 mode, but init to D0 for testing prupose.

	InitializeStateVariables();

	TxHW_Reset();									// Toggle TX reset pin

	if (StartTPI())									// Enable HW TPI mode, check device ID
	{
#ifdef DEV_SUPPORT_HDCP
		g_HDCP_Override = FALSE;
		HDCP_Init();
#endif
/*
#ifdef DEV_SUPPORT_EHDMI
		EHDMI_Init();
#endif

#if defined SiI9232_OR_SiI9236
		OnMobileHdCableDisconnected();					// Default to USB Mode.
#endif

#if (IS_CEC == 1)
	    SI_CecInit();
#endif

#ifdef RCP_ENABLE
		TPI_DEBUG_PRINT (("\r\ncalling CpCbusInitialize\r\n\r\n"));
		CpCbusInitialize();
#endif
*/
		return TRUE;
	}

	return FALSE;
}


static void WakeUpFromD3 (void)
{

	TPI_DEBUG_PRINT(("Waking up...\r\n"));

	InitializeStateVariables ();
//	TXHAL_InitPostReset();
	//WriteInitialRegisterValues();
#if (IS_CEC == 1)
    SI_CecInit();
#endif
}


static void InitializeStateVariables (void)
{

#ifdef MHD_CABLE_HPD
	tclkStable = FALSE;
	checkTclkStable = TRUE;
	mobileHdCableConnected = FALSE;
#endif

	tmdsPoweredUp = FALSE;
	hdmiCableConnected = FALSE;
	dsRxPoweredUp = FALSE;
	OutputHaveSet = FALSE;
	mobileHdCableConnected = TRUE;

#ifdef DEV_SUPPORT_EDID
	edidDataValid = FALSE;							// Move this into EDID_Init();
#endif
}


#if defined SiI9232_OR_SiI9236
static void ForceUsbIdSwitchOpen (void)
{
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x90, BIT_0, 0x00);				// Disable discovery
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_6, BIT_6);				// Force USB ID Switch
	WriteIndexedRegister(INDEXED_PAGE_0, 0x92, 0x86);								// Force USB, CBUS discovery
	WriteIndexedRegister(INDEXED_PAGE_0, 0x93, 0x0C);								// Disable CBUS pull-up during RGND measurement
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x79, BIT_5 | BIT_4, BIT_4);		// Force HPD to 0 when not in MobileHD mode.
}


static void ReleaseUsbIdSwitchOpen (void)
{
	Delay1ms(25);
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_6, 0x00);
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x90, BIT_0, BIT_0);				// Enable discovery
}


static void InitForceUsbIdSwitchOpen (void)
{
	I2C_WriteByte(0x72, 0x90, 0x26);					// Disable CBUS discovery
	ReadModifyWriteTPI(0x95, BIT_6, BIT_6);				// Force USB ID switch to open
	WriteByteTPI(0x92, 0x86);							//
	ReadModifyWriteTPI(0x79, BIT_5 | BIT_4, BIT_4);		// Force HPD to 0 when not in Mobile HD mode.
	//pinGpio1 = 0;
}


static void InitReleaseUsbIdSwitchOpen (void)
{
	Delay1ms(25);
	ReadModifyWriteTPI(0x95, BIT_6, 0x00);				// Release USB ID switch
	ReadModifyWriteTPI(0x90, BIT_0, BIT_0);				// Enable CBUS discovery
}
#endif

#ifdef MHD_CABLE_HPD
static void GoToD3 (void)
{

#if defined SYS_BOARD_STARTERKIT
	//pinWol2MhdRxPwr = 1;
#ifdef SK_X02
	pinM2U_VBusCtrl = 0;
#else
	pinVBusIsolate = 0;
#endif
	pinMhdConn = 1;
	pinUsbConn = 0;
#endif

	HDCP_Off();

	TxPowerStateD3();

	mobileHdCableConnected = FALSE;
	hdmiCableConnected = FALSE;
	dsRxPoweredUp = FALSE;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   HotPlugService()
//
// PURPOSE      :   Implement Hot Plug Service Loop activities
//
// INPUT PARAMS :   None
//
// OUTPUT PARAMS:   void
//
// GLOBALS USED :   LinkProtectionLevel
//
// RETURNS      :   An error code that indicates success or cause of failure
//
//////////////////////////////////////////////////////////////////////////////

void HotPlugService (void)
{
	TPI_TRACE_PRINT((">>HotPlugService()\r\n"));

	DisableInterrupts(0xFF);

#ifndef IS_VMODE_SIM
#ifndef RX_ONBOARD
#ifdef SYS_BOARD_FPGA
	TPI_DEBUG_PRINT (("** Only 480P supported: be sure input is 480P."));
	VideoModeDescription.VIC = 2;												// Only 480P is supported on FPGA platform
#else
//	VideoModeDescription.VIC = EDID_Data.VideoDescriptor[0];					// use 1st mode supported by sink
//	VideoModeDescription.VIC = 16;					// use 1st mode supported by sink
	printf("** VideoModeDescription.VIC = 0x%02X\r\n", VideoModeDescription.VIC);
#endif
#endif
	// workaround for Bug#18128
	if (VideoModeDescription.ColorDepth == VMD_COLOR_DEPTH_8BIT)
	{
		// Yes it is, so force 16bpps first!
		VideoModeDescription.ColorDepth = VMD_COLOR_DEPTH_16BIT;
		InitVideo(X1);
		// Now put it back to 8bit and go do the expected InitVideo() call
		VideoModeDescription.ColorDepth = VMD_COLOR_DEPTH_8BIT;
	}
	// end workaround
#endif // IS_VMODE_SIM


#ifndef RX_ONBOARD

#endif // RX_ONBOARD

	InitVideo(X1);						// Set PLL Multiplier to x1 upon power up

#ifndef POWER_STATE_D0_AFTER_TMDS_ENABLE
	TxPowerStateD0();
#endif

	if (IsHDMI_Sink())											// Set InfoFrames only if HDMI output mode
	{
		TPI_DEBUG_PRINT (("AUDIO -> Basic\r\n"));
		SetPreliminaryInfoFrames();
//#if defined SiI9232_OR_SiI9236
//#else
		SetBasicAudio();										// set audio interface to basic audio (an external command is needed to set to any other mode
//#endif
	}
	else
	{
		TPI_DEBUG_PRINT (("AUDIO -> Muted\r\n"));
		SetAudioMute(AUDIO_MUTE_MUTED);
	}

#ifdef DEV_SUPPORT_EHDMI
//	EHDMI_ARC_Common_Enable();
	EHDMI_ARC_Common_With_HEC_Enable();
#endif

#ifdef DEV_SUPPORT_HDCP
	if ((HDCP_TxSupports == TRUE) && (VideoModeDescription.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED))
	{
		if (HDCP_AksvValid == TRUE)
		{
			// AV MUTE
			TPI_DEBUG_PRINT (("TMDS -> Enabled (Video Muted)\r\n"));
			//	Turn LED by GPIO0
			WriteIndexedRegister(0x03, 0xE0, 0x00);		//	Set GPIO0 - GPIO3 all as Output.
			WriteIndexedRegister(0x03, 0xE2, 0x01);		//	Set GPIO0 = 1.
			ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK, LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE | AV_MUTE_NORMAL);
			ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK	| AV_MUTE_MASK,
				LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE | AV_MUTE_MUTED);

			tmdsPoweredUp = TRUE;

			EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT | SECURITY_CHANGE_EVENT | V_READY_EVENT | HDCP_CHANGE_EVENT);
		}

	}
	else
#endif
	{
		TPI_DEBUG_PRINT (("TMDS -> Enabled\r\n"));
		//	Turn LED by GPIO0
		WriteIndexedRegister(0x03, 0xE0, 0x00);		//	Set GPIO0 - GPIO3 all as Output.
		WriteIndexedRegister(0x03, 0xE2, 0x01);		//	Set GPIO0 = 1.
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK, LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE | AV_MUTE_NORMAL);
		EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT | V_READY_EVENT);
	}

#ifdef POWER_STATE_D0_AFTER_TMDS_ENABLE
	TxPowerStateD0();
#endif

#ifdef INFOFRAMES_AFTER_TMDS
//	TxPowerStateD0();
	WriteByteTPI(0xCD, 0x00);	    				// Set last byte of Audio InfoFrame
	WriteByteTPI(TPI_END_RIGHT_BAR_MSB, 0x00);	    // Set last byte of AVI InfoFrame
#endif

#ifdef RX_SAMABETH_ONBOARD
	printf("Initialize Samabeth (Again!):\r\n");
	InitSamabeth();	// %%%% KLUDGE!  Here only until Bug#19502 is fixed!!
#endif
	TPI_DEBUG_PRINT (("HotPlugService Out\r\n"));
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : HW_Reset()
//
// PURPOSE       : Send a
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : Void
//
//////////////////////////////////////////////////////////////////////////////

static void TxHW_Reset(void)
{
	byte B_Data = 0;
	TPI_TRACE_PRINT((">>TxHW_Reset()\r\n"));
/*
	TXHAL_InitPreReset();

#ifdef SCL_LOW_DURING_RESET
	PinI2CSCL = LOW;
	Delay1ms(T_SCLLOW_DELAY_RESET);
#endif

	PinTxHwReset = LOW;
	Delay1ms(TX_HW_RESET_PERIOD);
	PinTxHwReset = HIGH;

#ifdef F_9136
#ifndef SYS_BOARD_FPGA
	PinRxHwReset = LOW;
	Delay1ms(RX_HW_RESET_PERIOD);
	PinRxHwReset = HIGH;
#endif
#endif

#ifdef SCL_LOW_DURING_RESET
	Delay1ms(T_RESET_DELAY_SCLHIGH);
	PinI2CSCL = HIGH;
#endif

	TXHAL_InitPostReset();
*/
	// Does this need to be done for every chip? Should it be moved into TXHAL_InitPostReset() for each applicable device?
	B_Data = 0x14;
	if(gB_I2CIndex == 0)
		I2C1WriteByte(gB_TPI_BASE_Addr, 0x7C, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data);					// HW debounce to 64ms (0x14)
	else
		I2C2WriteByte(gB_TPI_BASE_Addr, 0x7C, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data);					// HW debounce to 64ms (0x14)
#ifdef F_9136
	B_Data = 0xA5;
	if(gB_I2CIndex == 0)
		I2C1WriteByte(gB_TPI_BASE_Addr, 0x82, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data);
#endif
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : StartTPI()
//
// PURPOSE       : Start HW TPI mode by writing 0x00 to TPI address 0xC7. This
//                 will take the Tx out of power down mode.
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : TRUE if HW TPI started successfully. FALSE if failed to.
//
//////////////////////////////////////////////////////////////////////////////

bool StartTPI(void)
{
	byte devID = 0x00;
        word wID = 0x0000;

        TPI_TRACE_PRINT((">>StartTPI()\r\n"));
//while(1)
    WriteByteTPI(TPI_ENABLE, 0x00);            // Write "0" to 72:C7 to start HW TPI mode
        Delay1ms(100);

        devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x03);
        wID = devID;
        wID <<= 8;
        devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x02);
        wID |= devID;
//        printf ("wID = 0x%04X\r\n", (int) wID);

    devID = ReadByteTPI(TPI_DEVICE_ID);

//        printf ("devID = 0x%04X\r\n", (int) devID);

        if (devID == SiI_DEVICE_ID) {
//        printf (SiI_DEVICE_STRING);
        return TRUE;
        }

//    printf ("Unsupported TX\r\n");
    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      :  EnableInterrupts()
//
// PURPOSE       :  Enable the interrupts specified in the input parameter
//
// INPUT PARAMS  :  A bit pattern with "1" for each interrupt that needs to be
//                  set in the Interrupt Enable Register (TPI offset 0x3C)
//
// OUTPUT PARAMS :  void
//
// GLOBALS USED  :  None
//
// RETURNS       :  TRUE
//
//////////////////////////////////////////////////////////////////////////////
bool EnableInterrupts(byte Interrupt_Pattern)
{
    TPI_TRACE_PRINT((">>EnableInterrupts()\r\n"));
    ReadSetWriteTPI(TPI_INTERRUPT_ENABLE_REG, Interrupt_Pattern);

#if defined SiI9232_OR_SiI9236
	WriteIndexedRegister(INDEXED_PAGE_0, 0x75, BIT_5);
#endif

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      :  DisableInterrupts()
//
// PURPOSE       :  Enable the interrupts specified in the input parameter
//
// INPUT PARAMS  :  A bit pattern with "1" for each interrupt that needs to be
//                  cleared in the Interrupt Enable Register (TPI offset 0x3C)
//
// OUTPUT PARAMS :  void
//
// GLOBALS USED  :  None
//
// RETURNS       :  TRUE
//
//////////////////////////////////////////////////////////////////////////////
static bool DisableInterrupts(byte Interrupt_Pattern)
{
        TPI_TRACE_PRINT((">>DisableInterrupts()\r\n"));
    ReadClearWriteTPI(TPI_INTERRUPT_ENABLE_REG, Interrupt_Pattern);

    return TRUE;
}


#if defined HAS_CTRL_BUS
static bool CBUS_Discovery (void)
{
    byte i;

    for (i = 0; i < 20; i++)
    {
        WriteByteTPI (TPI_DEVICE_POWER_STATE_CTRL_REG, CTRL_PIN_DRIVEN_TX_BRIDGE | TX_POWER_STATE_D0);                  // Start CBUS self-discovery
        Delay1ms (T_CBUSDISCOVERY_DELAY);

        if (ReadByteCBUS(0x0A) & 0x01)
        {
            TPI_DEBUG_PRINT (("CBUS discovered in %d attempt(s).\r\n", (int)(i + 1)));
             return TRUE;
        }

        WriteByteTPI (TPI_DEVICE_POWER_STATE_CTRL_REG, CTRL_PIN_TRISTATE | TX_POWER_STATE_D0);
        Delay1ms (T_CBUSDISCOVERY_DELAY);
    }

    TPI_DEBUG_PRINT (("CBUS downstream device not detected.\r\n0xC8:0x0A = %02X\r\n", (int)ReadByteCBUS(0x0A)));
    return FALSE;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : SetPreliminaryInfoFrames()
//
// PURPOSE       : Set InfoFrames for default (initial) setup only
//
// INPUT PARAMS  : VIC, output mode,
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : TRUE
//
//////////////////////////////////////////////////////////////////////////////
static bool SetPreliminaryInfoFrames()
{
    byte i;
    //API_Cmd Command;        // to allow using function SetAVI_InfoFrames()

        TPI_TRACE_PRINT((">>SetPreliminaryInfoFrames()\r\n"));

    for (i = 0; i < MAX_COMMAND_ARGUMENTS; i++)
        Arg(i) = 0;

        CommandLength = 0;      // fixes SetAVI_InfoFrames() from going into an infinite loop

    Arg(0) = VideoModeDescription.VIC;

#ifdef DEV_SUPPORT_EDID
        if (EDID_Data.YCbCr_4_4_4)
        {
                Arg(3) = 0x01;
        }
        else
        {
                if (EDID_Data.YCbCr_4_2_2)
                {
                        Arg(3) = 0x02;
                }
        }
#else
        Arg(3) = 0x00;
#endif

    SetAVI_InfoFrames();
    return TRUE;
}


static void TxPowerStateD0 (void)
{

	ReadModifyWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, TX_POWER_STATE_D0);
	TPI_DEBUG_PRINT(("TX Power State D0\r\n"));
	txPowerState = TX_POWER_STATE_D0;
}


static void TxPowerStateD3 (void)
{

#ifdef F_9136
		ReadModifyWriteTPI(TPI_DEEP_COLOR_GCP, BIT_0, ENABLE);
		Delay1ms(10);
		ReadModifyWriteTPI(TPI_DEEP_COLOR_GCP, BIT_0, DISABLE);

	ReadModifyWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, TX_POWER_STATE_D3);
#endif

#ifdef SiI9232_OR_SiI9236
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_1, 0x3D, BIT_0, 0x00);
#endif

	TPI_DEBUG_PRINT(("TX Power State D3\r\n"));
	txPowerState = TX_POWER_STATE_D3;
}


void EnableTMDS (void)
{

    TPI_DEBUG_PRINT(("TMDS -> Enabled\r\n"));
		//	Turn on LED by GPIO0
		WriteIndexedRegister(0x03, 0xE0, 0x00);		//	Set GPIO0 - GPIO3 all as Output.
		WriteIndexedRegister(0x03, 0xE2, 0x01);		//	Set GPIO0 = 1.
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
    tmdsPoweredUp = TRUE;
}


void DisableTMDS (void)
{

    TPI_DEBUG_PRINT(("TMDS -> Disabled\r\n"));

		//	Turn off LED by GPIO0
		WriteIndexedRegister(0x03, 0xE0, 0x00);		//	Set GPIO0 - GPIO3 all as Output.
		WriteIndexedRegister(0x03, 0xE2, 0x00);		//	Set GPIO0 = 0.
    ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
		TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
    tmdsPoweredUp = FALSE;
}


#ifdef DEV_SUPPORT_HDCP
void RestartHDCP (void)
{
	TPI_DEBUG_PRINT (("HDCP -> Restart\r\n"));

	DisableTMDS();
	HDCP_Off();
	EnableTMDS();
}
#endif

void SetAudioMute (byte audioMute)
{
	ReadModifyWriteTPI(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, audioMute);
}



#ifdef MHD_CABLE_HPD
void CheckTxFifoStable (void)
{

	byte bTemp;

	bTemp = ReadIndexedRegister(INDEXED_PAGE_0, 0x3E);
	if ((bTemp & (BIT_7 | BIT_6)) != 0x00)
	{
		TPI_DEBUG_PRINT(("FIFO Overrun / Underrun\r\n"));
		WriteIndexedRegister(INDEXED_PAGE_0, 0x05, BIT_4 | ASR_VALUE);	// Assert Mobile HD FIFO Reset
		Delay1ms(1);
		WriteIndexedRegister(INDEXED_PAGE_0, 0x05, ASR_VALUE);			// Deassert Mobile HD FIFO Reset
	}
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   TPI_Poll ()
//
// PURPOSE      :   Poll Interrupt Status register for new interrupts
//
// INPUT PARAMS :   None
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   LinkProtectionLevel
//
// RETURNS      :   None
//
//////////////////////////////////////////////////////////////////////////////

void TPI_Poll (void)
{

	byte InterruptStatusImage;

#ifdef MHD_CABLE_HPD

#else	// MHD_CABLE_HPD not defined
	if (txPowerState == TX_POWER_STATE_D0)
	{
		InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);
//		TPI_DEBUG_PRINT (("InterruptStatusImage = 0x%02X\r\n", InterruptStatusImage));
		if (InterruptStatusImage & HOT_PLUG_EVENT)
		{

			TPI_DEBUG_PRINT (("HPD  -> "));

			ReadSetWriteTPI(TPI_INTERRUPT_ENABLE_REG, HOT_PLUG_EVENT);  // Enable HPD interrupt bit

			// Repeat this loop while cable is bouncing:
			do
			{
				WriteByteTPI(TPI_INTERRUPT_STATUS_REG, HOT_PLUG_EVENT);
				Delay1ms(T_HPD_DELAY); 			// Delay for metastability protection and to help filter out connection bouncing
				InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);    // Read Interrupt status register
				Delay1ms(1);
			} while (InterruptStatusImage & HOT_PLUG_EVENT);              // loop as long as HP interrupts recur

			if (((InterruptStatusImage & HOT_PLUG_STATE) >> 2) != hdmiCableConnected)
			{

				if (hdmiCableConnected == TRUE)
				{
					OnHdmiCableDisconnected();
				}
				else
				{
					OnHdmiCableConnected();
					ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x0A, 0x08, 0x08);
				}

				if (hdmiCableConnected == FALSE)
				{
					return;
				}
			}
	#ifdef FORCE_MHD_OUTPUT
			if (mobileHdCableConnected == TRUE && hdmiCableConnected == TRUE && dsRxPoweredUp == FALSE)
			{
				OnDownstreamRxPoweredUp();
			}
	#else
	#ifndef SiI9232_OR_SiI9236
			// Check rx power
			if (((InterruptStatusImage & RX_SENSE_STATE) >> 3) != dsRxPoweredUp)
			{
				if (hdmiCableConnected == TRUE)
				{
					if (dsRxPoweredUp == TRUE)
					{
						OnDownstreamRxPoweredDown();
					}
					else
					{
	//					printf("Test 1 0x%02X, 0x%02X\r\n", (InterruptStatusImage & RX_SENSE_STATE), dsRxPoweredUp);
						OnDownstreamRxPoweredUp();
					}
				}

				ClearInterrupt(RX_SENSE_EVENT);
			}
	#endif
	#endif
				
		}
#endif	// MHD_CABLE_HPD


#ifdef HAS_CTRL_BUS
		if (InterruptStatusImage == NON_MASKABLE_INT)
		{
			// Check if NMI has occurred
			TPI_DEBUG_PRINT (("TP -> NMI Detected\r\n"));
			TPI_Init();												// Reset and re-initialize
			HotPlugService();
		}
#endif
/*
#ifdef FORCE_MHD_OUTPUT
		if (mobileHdCableConnected == TRUE && hdmiCableConnected == TRUE && dsRxPoweredUp == FALSE)
		{
			OnDownstreamRxPoweredUp();
		}
#else
#ifndef SiI9232_OR_SiI9236
		// Check rx power
		if (((InterruptStatusImage & RX_SENSE_STATE) >> 3) != dsRxPoweredUp)
		{
			if (hdmiCableConnected == TRUE)
			{
				if (dsRxPoweredUp == TRUE)
				{
					OnDownstreamRxPoweredDown();
				}
				else
				{
//					printf("Test 1 0x%02X, 0x%02X\r\n", (InterruptStatusImage & RX_SENSE_STATE), dsRxPoweredUp);
					OnDownstreamRxPoweredUp();
				}
			}

			ClearInterrupt(RX_SENSE_EVENT);
		}
#endif
#endif
*/
		// Check if Audio Error event has occurred:
		if (InterruptStatusImage & AUDIO_ERROR_EVENT)
		{
			//TPI_DEBUG_PRINT (("TP -> Audio Error Event\r\n"));
			//  The hardware handles the event without need for host intervention (PR, p. 31)
			ClearInterrupt(AUDIO_ERROR_EVENT);
		}

#ifdef DEV_SUPPORT_HDCP
		if ((hdmiCableConnected == TRUE)
			&& (dsRxPoweredUp == TRUE)
			&& (VideoModeDescription.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED))
		{
			HDCP_CheckStatus(InterruptStatusImage);
//			printf("Test 3 0x%02X, 0x%02X\r\n", (InterruptStatusImage & RX_SENSE_STATE), dsRxPoweredUp);
		}
#endif

#ifdef RX_ONBOARD
		if ((tmdsPoweredUp == TRUE) && (USRX_OutputChange == TRUE))
		{
			TPI_TRACE_PRINT(("TP -> Video Mode...\r\n"));
			DisableTMDS();

#ifdef DEV_SUPPORT_HDCP
			if ((HDCP_TxSupports == TRUE)
							&& (HDCP_AksvValid == TRUE)
							&& (VideoModeDescription.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED))
			{
				RestartHDCP();
			}
#endif
			HotPlugService();
			USRX_OutputChange = FALSE;

		}

#endif

#if (IS_CEC == 1)
		SI_CecHandler(0 , 0);
#endif

#ifndef MHD_CABLE_HPD
	}
#endif

#ifdef F_9136
#ifndef SYS_BOARD_FPGA
else if (txPowerState == TX_POWER_STATE_D3)
	{
		InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);
//		printf("Test 2 0x%02X, 0x%02X\r\n", InterruptStatusImage, dsRxPoweredUp);
		if (InterruptStatusImage & HOT_PLUG_EVENT)
		{
			TPI_DEBUG_PRINT (("InterruptStatusImage = 0x%02X\r\n", InterruptStatusImage));
			TPI_Init();
		}
	}
#endif
#endif
}


#ifdef MHD_CABLE_HPD
static void OnMobileHdCableConnected(void)
{

	TPI_DEBUG_PRINT (("MHD Connected\r\n"));

	mobileHdCableConnected = TRUE;

#if defined SiI9232_OR_SiI9236
	if (txPowerState == TX_POWER_STATE_D3)
	{
	    StartTPI();
	    //EnableInterrupts();
	    TxPowerStateD0();
	}

#ifdef SYS_BOARD_STARTERKIT
	pinM2U_VBusCtrl = 1;
#ifdef SK_X02
#else
	pinVBusIsolate = 1;
#endif
	pinMhdConn = 0;
	pinUsbConn = 1;
#endif

	WriteIndexedRegister(INDEXED_PAGE_0, 0xA0, 0x10);

	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x79, BIT_4, 0x00);		// Un-force HPD


	ReadModifyWriteCBUS(0x08, BIT_4, 0x00);		// Clear status bit


	if (pin9290_938x == settingMode9290)
	{
		TPI_DEBUG_PRINT (("Setting DDC Burst Mode\r\n"));
		WriteByteCBUS(0x07, DDC_XLTN_TIMEOUT_MAX_VAL | 0x0E); 	// Increase DDC translation layer timer (burst mode)
		WriteByteCBUS(0x47, 0x03);
	}
	else
	{
		TPI_DEBUG_PRINT (("Setting DDC Byte Mode\r\n"));
		WriteByteCBUS(0x07, DDC_XLTN_TIMEOUT_MAX_VAL | 0x02); 	// Increase DDC translation layer timer (byte mode)

		ReadModifyWriteCBUS(0x44, BIT_1, BIT_1);				// Enable segment pointer workaround
	}


	WriteByteCBUS (0x21, 0x81); // Heartbeat Enable
#endif
}


static void OnMobileHdCableDisconnected(void)
{

	TPI_DEBUG_PRINT (("MobileHD Disconnected\r\n"));

	mobileHdCableConnected = FALSE;

	OnHdmiCableDisconnected();

#if defined SiI9232_OR_SiI9236
	//pinWol2MhdRxPwr = 1;
//	pinVBusIsolate = 0;
#ifdef SYS_BOARD_STARTERKIT
	pinMhdConn = 1;
	pinUsbConn = 0;
#endif

	WriteIndexedRegister(INDEXED_PAGE_0, 0xA0, 0xD0);

	I2C_WriteByte(0xC8, 0x21, 0x01); // Heartbeat Disable

	//TxPowerState(TX_POWER_STATE_D3);
#else
	ReadClearWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);			// Disable self-discovery
	ReadSetWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);
	ReadClearWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);

	WriteByteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_D3);
#endif
}
#endif


static void OnHdmiCableConnected(void)
{
	TPI_DEBUG_PRINT (("HDMI Connected\r\n"));

#ifdef DEV_SUPPORT_MHD
	//Without this delay, the downstream BKSVs wil not be properly read, resulting in HDCP not being available.
	Delay100ms(5);
#else
	// No need to call TPI_Init here unless TX has been powered down on cable removal.
	//TPI_Init();
#endif

	hdmiCableConnected = TRUE;

#ifdef HDCP_DONT_CLEAR_BSTATUS
#else
	if ((HDCP_TxSupports == TRUE) && (HDCP_AksvValid == TRUE) && (VideoModeDescription.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED)) {
		WriteIndexedRegister(INDEXED_PAGE_0, 0xCE, 0x00); // Clear BStatus
		WriteIndexedRegister(INDEXED_PAGE_0, 0xCF, 0x00);
	}
#endif

#ifdef DEV_SUPPORT_EDID
	DoEdidRead();
#endif

#ifdef READKSV
	ReadModifyWriteTPI(0xBB, 0x08, 0x08);
#endif

	if (IsHDMI_Sink())              // select output mode (HDMI/DVI) according to sink capabilty
	{
		TPI_DEBUG_PRINT (("HDMI Sink Detected\r\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPUT_MODE_HDMI);
	}
	else
	{
		TPI_DEBUG_PRINT (("DVI Sink Detected\r\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPUT_MODE_DVI);
	}

#if defined SiI9232_OR_SiI9236
	OnDownstreamRxPoweredUp();		// RX power not determinable? Force to on for now.
#endif
}


static void OnHdmiCableDisconnected(void)
{

	TPI_DEBUG_PRINT (("HDMI Disconnected\r\n"));

	hdmiCableConnected = FALSE;

#ifdef DEV_SUPPORT_EDID
	edidDataValid = FALSE;
#endif

	OnDownstreamRxPoweredDown();

#ifdef F_9136
//	TxPowerStateD3();
#endif
	OutputHaveSet = FALSE;
}


static void OnDownstreamRxPoweredDown(void)
{

	TPI_DEBUG_PRINT (("\r\nDSRX -> Powered Down\r\n"));
	dsRxPoweredUp = FALSE;

#ifdef DEV_SUPPORT_HDCP
	if ((HDCP_TxSupports == TRUE)
					&& (HDCP_AksvValid == TRUE)
					&& (VideoModeDescription.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED))
	{
		HDCP_Off();
	}
#endif

	DisableTMDS();
    ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK,OUTPUT_MODE_DVI);	// Set to DVI output mode to reset HDCP
}


static void OnDownstreamRxPoweredUp(void)
{
	TPI_DEBUG_PRINT (("\r\nDSRX -> Powered Up\r\n"));
	dsRxPoweredUp = TRUE;

	HotPlugService();

	USRX_OutputChange = FALSE;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    SiI9136TPI_DummyCall(void)                                      */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*							being called.												*/
/*																			*/
/****************************************************************************/
void SiI9136TPI_DummyCall(void)
{
	WakeUpFromD3 ();
	TxPowerStateD3();
	tmdsPoweredUp = tmdsPoweredUp;
	TxHW_Reset();
}

/********************************  END  *************************************/

