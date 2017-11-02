//------------------------------------------------------------------------------
// Copyright © 2007, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
#include <string.h>

#include <local_types.h>
#include <config.h>
//#include <debug_cmd.h>
#include <registers.h>
#include <amf.h>
#include "infofrm.h"

//#include <CEC.h>
#include "../../System Code/global.h"
#include "../../System Code/Timer.h"
//------------------------------------------------------------------------------
// Global State Structure
//------------------------------------------------------------------------------
GlobalStatus_t CurrentStatus;
uint16_t wOldTickCnt = 0;

extern volatile unsigned short gD_TimeTickCnt[INPUT_BOARD_MAX * 8];
//------------------------------------------------------------------------------
// Function: AutoVideoSetup
// Description: Setup registers for Auto Video Mode
//
// Notes: Compile time configuration is done using CONF__* defines in config.h
//------------------------------------------------------------------------------
static void AutoVideoSetup(void)
{
    
	uint8_t unmuteTimeConf[] = {0xFF,0x00,0x00,0xFF,0x00,0x00};
	DEBUG_PRINT(("\r\nBoard %d , Input %d , >>AutoVideoSetup\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
	RegisterWriteBlock(REG__WAIT_CYCLE, &unmuteTimeConf[0],6);	//video unmute wait 

    RegisterWrite(REG__VID_CTRL,  (BIT__IVS   & CONF__VSYNC_INVERT) |
                                  (BIT__IHS   & CONF__HSYNC_INVERT) );  //set HSYNC,VSNC polarity
    RegisterWrite(REG__RPI_AUTO_CONFIG, BIT__CHECKSUM_EN|BIT__V_UNMUTE_EN|BIT__HCDP_EN|BIT__TERM_EN);        //auto config
    RegisterWrite(REG__SRST,      BIT__SWRST_AUTO);            //enable auto sw reset
//    RegisterWrite(REG__VID_AOF,   CONF__OUTPUT_VIDEO_FORMAT);  //set output video format
		RegisterWrite(REG__VID_AOF, VAL__YC422_MUX_8BIT_EMBED_SYNC);
    RegisterModify(REG__AEC_CTRL,  BIT__AVC_EN, SET);                //enable auto video configuration
		RegisterWrite(REG__VID_MODE2, VAL__DITHER_8BITS);	

#if (CONF__ODCK_LIMITED == ENABLE)
	RegisterModify(REG__SYS_PSTOP, MSK__PCLK_MAX, CONF__PCLK_MAX_CNT);
#endif //(CONF__ODCK_LIMITED==ENABLE)


#if (PEBBLES_ES1_ZONE_WORKAROUND == ENABLE)	
	RegisterWrite(REG__AVC_EN2, BIT__AUTO_DC_CONF);			   //mask out auto configure deep color clock
	RegisterWrite(REG__VIDA_XPCNT_EN, BIT__VIDA_XPCNT_EN);	   //en read xpcnt
#endif //(PEBBLES_ES1_ZONE_WORKAROUND == ENABLE)	 

#if (PEBBLES_STARTER_NO_CLK_DIVIDER == ENABLE)
	RegisterModify(REG__AVC_EN2, BIT__AUTO_CLK_DIVIDER,SET);	  //msk out auto clk divider
#endif //(PEBBLES_STARTER_NO_CLK_DIVIDER == ENABLE)
}



//------------------------------------------------------------------------------
// Function: AutoAudioSetup
// Description: Setup registers for Auto Audio Mode
//------------------------------------------------------------------------------
static void AutoAudioSetup(void)
{
    uint8_t abAecEnables[3];

		DEBUG_PRINT(("\r\nBoard %d , Input %d , >>AutoAudioSetup\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
    RegisterModify(REG__ACR_CTRL3, MSK__CTS_THRESH, VAL__CTS_THRESH( CONF__CTS_THRESH_VALUE ));

    abAecEnables[0] = (BIT__SYNC_DETECT        |
                       BIT__CKDT_DETECT        |
                       BIT__CABLE_UNPLUG       );
    abAecEnables[1] = (BIT__HDMI_MODE_CHANGED  |
                       BIT__CTS_REUSED         |
                       BIT__AUDIO_FIFO_UNDERUN |
                       BIT__AUDIO_FIFO_OVERRUN |
                       BIT__FS_CHANGED         |
                       BIT__H_RES_CHANGED      );
#if (CONF__VSYNC_OVERFLOW != ENABLE)   
    abAecEnables[2] = (BIT__V_RES_CHANGED      );
#endif
    RegisterWriteBlock(REG__AEC_EN1, abAecEnables, 3);
	RegisterModify(REG__AEC_CTRL, BIT__CTRL_ACR_EN, SET);

}


//------------------------------------------------------------------------------
// Function: ConfigureSelectedPort
// Description: Setup new input port after port change
//------------------------------------------------------------------------------
static void ConfigureSelectedPort(void)
{		  
	
	DEBUG_PRINT(("Board %d , Input %d , ConfigureSelectedPort PortSelection = %d; \r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, CurrentStatus.PortSelection));
   switch (CurrentStatus.PortSelection)
    {
        case PORT_SELECT__PORT_0:
        {	
            RegisterModify(REG__PORT_SWTCH2, MSK__PORT_EN,VAL__PORT0_EN);     //select port 0
            RegisterWrite(REG__PORT_SWTCH, BIT__DDC0_EN);     //select DDC 0
        }
        break;

        case PORT_SELECT__PORT_1:
        {
            RegisterModify(REG__PORT_SWTCH2, MSK__PORT_EN,VAL__PORT1_EN);     //select port 1
						RegisterWrite(REG__PORT_SWTCH, BIT__DDC1_EN);     //select DDC 1
        }
        break;

        case PORT_SELECT__PORT_2:
        {
            RegisterModify(REG__PORT_SWTCH2, MSK__PORT_EN,VAL__PORT2_EN);     //select port 2
						RegisterWrite(REG__PORT_SWTCH, BIT__DDC2_EN);     //select DDC 2
        }
        break;

        case PORT_SELECT__PORT_3:
        {
            RegisterModify(REG__PORT_SWTCH2, MSK__PORT_EN,VAL__PORT3_EN);     //select port 3
						RegisterWrite(REG__PORT_SWTCH, BIT__DDC3_EN);     //select DDC 3
        }
        break;

        case PORT_SELECT__PORT_7:
        {
            DEBUG_PRINT(("turn off 3V power! \r\n"));	   //power down
        }
        break;
    }	//end of switch

}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Function: SystemDataReset
// Description: Re-initialize receiver state
//------------------------------------------------------------------------------
void SystemDataReset(void)
{

    TurnAudioMute(ON);
    TurnVideoMute(ON);

	CurrentStatus.ResolutionChangeCount = 0;
    CurrentStatus.ColorDepth = 0;
	CurrentStatus.AudioMode = AUDIO_MODE__NOT_INIT;
    RegisterModify(REG__TMDS_CCTRL2, MSK__DC_CTL, VAL__DC_CTL_8BPP_1X);

    ConfigureSelectedPort();

}



//------------------------------------------------------------------------------
// Function: PollPortSwitch
// Description:
//------------------------------------------------------------------------------
void PollPortSwitch(void)
{
    uint8_t bNewPortSelection;

//    bNewPortSelection = HAL_GetPortSelectionDebounced();
		bNewPortSelection = PORT_SELECT__PORT_0;

    if (bNewPortSelection != PORT_SELECT__NO_CHANGE)
    {
        if (CurrentStatus.PortSelection != bNewPortSelection)
				{
					DEBUG_PRINT(("Board %d , Input %d , PortSelection = %d; \r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, bNewPortSelection));
					CurrentStatus.PortSelection = bNewPortSelection;
					SystemDataReset();
				}
    }
}


//------------------------------------------------------------------------------
// Function: InitializePortSwitch
// Description:	called once in SystemInit() function
//------------------------------------------------------------------------------

static void InitializePortSwitch(void)
{
    CurrentStatus.PortSelection = PORT_SELECT__PORT_0;
		PollPortSwitch();
}

//------------------------------------------------------------------------------
// Function: SiI9233Init
// Description: One time initialization at statup
//------------------------------------------------------------------------------
void SiI9233Init(void)
{
		uint8_t EQTable[] = {0x8A,0xAA,0x1A,0x2A};
//		memset(&CurrentStatus, 0, sizeof(CurrentStatus));
//		CurrentStatus.StatusAudioTmr = 0x0FFF;
//		CurrentStatus.StatusVideoTmr = 0x0FFF;

    while( (RegisterRead(REG__BSM_STAT)& BIT__BOOT_DONE )== 0) //wait done
		{
				DEBUG_PRINT(("Board %d , Input %d , BIT__BOOT_DONE = 0; \r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
		}

    if((RegisterRead(REG__BSM_STAT)& BIT__BOOT_ERROR)!=0)
				DEBUG_PRINT(("Board %d , Input %d , First Boot error! \r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));

	RegisterModify(REG__HPD_HW_CTRL,MSK__INVALIDATE_ALL, SET);	//disable auto HPD conf at RESET
	TurnAudioMute(ON);
	TurnVideoMute(ON);


#if(PEBBLES_ES1_STARTER_CONF == ENABLE)
    RegisterWrite(REG__TERMCTRL2, VAL__45OHM); 			//1.term default value	

    RegisterWrite(REG__FACTORY_A87,0x43);              //2.Set PLL mode to internal and set selcalrefg to F
    RegisterWrite(REG__FACTORY_A81,0x18);              //Set PLL zone to auto and set Div20 to 1

    RegisterWrite(REG__DRIVE_CNTL,0x64);               //3.change output strength,  

    RegisterWrite(REG__FACTORY_ABB,0x04);              //4.desable DEC_CON

    RegisterWriteBlock(REG__FACTORY_A92,&EQTable[0],4);//5.Repgrogram EQ table
    RegisterWrite(REG__FACTORY_AB5,0x40);              //EnableEQ

    RegisterWrite(REG__FACTORY_9E5, 0x02);             //6. DLL by pass
		RegisterWrite(REG__FACTORY_A89,0x00);			   			//7. configure the PLLbias 	
		RegisterWrite(REG__FACTORY_00E,0x40);  			   	//for ES1.1 conf only
#endif
		//	Set GPIO1 = SCDT
		RegisterWrite(0x9FC, 0x02);		//	MYJ : Set GPIO1 = SCDT
		RegisterWrite(0x9FA, 0x02);		//	MYJ : Set GPIO1 as Output
				
//    CEC_Init();									// program EDID in this function					  
//		ProgramEDID();
    //set recommended values
    RegisterWrite(REG__AACR_CFG1, CONF__AACR_CFG1_VALUE);   //pll config #1
    RegisterWrite(REG__CBUS_PAD_SC, VAL__SC_CONF);  		//CBUS slew rate 
    RegisterWrite(REG__SRST,  BIT__SWRST_AUTO);             //enable auto sw reset
		RegisterWrite(REG__INFM_CLR,BIT__CLR_GBD|BIT__CLR_ACP);	//clr GBD & ACP

    RegisterWrite(REG__ECC_HDCP_THRES, CONF__HDCPTHRESH & 0xff);      //HDCP threshold low uint8_t
    RegisterWrite(REG__ECC_HDCP_THRES+1, (CONF__HDCPTHRESH>>8) & 0xff);  //HDCP threshold high uint8_t
    AutoVideoSetup();
    AutoAudioSetup();
	    
    SetupInterruptMasks();

	InitializePortSwitch();

    TurnPowerDown(OFF);	 						   	

	RegisterModify(REG__HPD_HW_CTRL,MSK__INVALIDATE_ALL, CLEAR); //CLEAR disable auto HPD conf 

#if (CONF__SUPPORT_3D == ENABLE)
	/* Inti Hdmi Info frame related chip registers and data */
	HdmiInitIf ();
#endif
}

//------------------------------------------------------------------------------
// Function: HdmiTask
// Description: One time initialization at startup
//------------------------------------------------------------------------------
void HdmiTask(void)
{
		uint8_t B_TempData = 0;

    if ((CurrentStatus.AudioState == STATE_AUDIO__REQUEST_AUDIO) ||
        (CurrentStatus.AudioState == STATE_AUDIO__AUDIO_READY))
    {
			B_TempData = CurrentStatus.BoardIndex * 8 + CurrentStatus.ChipIndex * 2 + 1;
//        if (SiI9233_TimerCheck(SiI_9233_AUDIO_TMR) == TIMER_TMO)
        if (SiI9233_TimerCheck(B_TempData) == TIMER_TMO)
        {
            AudioUnmuteHandler();
        }
    }

    if (CurrentStatus.VideoState == STATE_VIDEO__UNMUTE)
    {
				B_TempData = CurrentStatus.BoardIndex * 8 + CurrentStatus.ChipIndex * 2;
//        if (SiI9233_TimerCheck(SiI_9233_VIDEO_TMR) == TIMER_TMO)
        if (SiI9233_TimerCheck(B_TempData) == TIMER_TMO)
        {
					DEBUG_PRINT(("Board %d , Input %d , v time out\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
					ResetVideoControl();
        }
    }
#if (PEBBLES_VIDEO_STATUS_2ND_CHECK==ENABLE)
	if (CurrentStatus.VideoState == STATE_VIDEO__ON)
	{
//					DEBUG_PRINT(("Board %d , Input %d , Test 1\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
				B_TempData = CurrentStatus.BoardIndex * 8 + CurrentStatus.ChipIndex * 2;
//        if ((SiI9233_TimerCheck(SiI_9233_VIDEO_TMR) == TIMER_TMO))
        if ((SiI9233_TimerCheck(B_TempData) == TIMER_TMO))
        {
//					DEBUG_PRINT(("Board %d , Input %d , Test 2\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));
					PclkCheck();
        	//TIMER_Set(TIMER__VIDEO, VIDEO_STABLITY_CHECK_INTERVAL);  // start the video timer
					SiI9233_TimerStart(B_TempData, VIDEO_STABLITY_CHECK_INTERVAL);					
					CurrentStatus.VideoStabilityCheckCount++;

        }
		if (CurrentStatus.VideoStabilityCheckCount == VIDEO_STABLITY_CHECK_TOTAL/VIDEO_STABLITY_CHECK_INTERVAL)
		{
// HL Note			
//			DEBUG_PRINT(("Board %d , Input %d , V CHECK %d times\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex, (int)CurrentStatus.VideoStabilityCheckCount));
			CurrentStatus.VideoState = STATE_VIDEO__CHECKED;
			CurrentStatus.VideoStabilityCheckCount = 0;
		}					  										
	}
#endif //#if (PEBBLES_VIDEO_STATUS_2ND_CHECK==ENABLE)
}

void SiI9233_TimerStart(uint8_t B_Timer, uint16_t W_value)
{
	//
	// Error checking
	//
	if (B_Timer >= (INPUT_BOARD_MAX * 8))				// Within buffer size
		return;							// No, exit
	if (W_value == 0xFFFF)				// Over limit
		W_value--;						// Take the max
	//
	// Reload timer value into buffer
	//
	gD_TimeTickCnt[B_Timer] = W_value;
}

void SiI9233_TimerStop(uint8_t B_Timer)
{
	//
	// Error checking
	//
	if (B_Timer >= (INPUT_BOARD_MAX * 8))			// Within buffer size
		return;							// No, exit
	//
	// Set timer value into buffer to indicate STOP
	//
	gD_TimeTickCnt[B_Timer] = 0xFFFF;			// Load value into timer
}

uint8_t SiI9233_TimerCheck(uint8_t B_Timer)
{
	//
	// Error checking
	//
	if (B_Timer >= (INPUT_BOARD_MAX * 8))				// Within buffer size
		return (TIMER_OK);												// No, exit
	//
	// Set timer value into buffer to indicate STOP
	//
	if (gD_TimeTickCnt[B_Timer] == 0)						// Timer expired ?
	{									// Yes
		gD_TimeTickCnt[B_Timer] = 0xFFFF;					// Auto clear the flag	
		return (TIMER_TMO);												// Timer expired
	}
	else if (gD_TimeTickCnt[B_Timer] == 0xFFFF)						// Timer expired ?
		return (TIMER_TMO);
	else
	{
		return (TIMER_OK);				// Timer running or aborted
	}
}

void SiI9233_TimerTask(void)
{
	/*
	uint16_t wNewTickCnt = 0;
	uint16_t wTickDiff;
	wNewTickCnt = TIMER_GetTickCounter();
	if ( wNewTickCnt > wOldTickCnt )
	{
		wTickDiff = wNewTickCnt - wOldTickCnt;
	}
	else
	{ // counter wrapping
		wTickDiff = ( 0xFFFF - wOldTickCnt ) + wNewTickCnt;
	}
	wOldTickCnt = wNewTickCnt;
	if ((CurrentStatus.StatusAudioTmr != 0x0FFF) && (CurrentStatus.StatusAudioTmr != 0))	// 
	{
		if(wTickDiff >= CurrentStatus.StatusAudioTmr)
			CurrentStatus.StatusAudioTmr = 0; 
		else
			CurrentStatus.StatusAudioTmr -= (BYTE) wTickDiff; 
	}
	if ((CurrentStatus.StatusVideoTmr != 0x0FFF) && (CurrentStatus.StatusVideoTmr != 0))	// 
	{
		if(wTickDiff >= CurrentStatus.StatusVideoTmr)
			CurrentStatus.StatusVideoTmr = 0; 
		else
			CurrentStatus.StatusVideoTmr -= (BYTE) wTickDiff; 
	}
	*/
}

//------------------------------------------------------------------------------
// Function: main
// Description: Main loop
//------------------------------------------------------------------------------
void SiI9233_TempTest(void)
{
/*
	uint16_t wOldTickCnt = 0;
	uint16_t wNewTickCnt = 0;
	uint16_t wTickDiff;
*/
//    DEBUG_INIT();
    DEBUG_PRINT(("Board %d , Input %d , AMF Pebble ES1_2 starter board version 2.3.0\r\n", CurrentStatus.BoardIndex, CurrentStatus.ChipIndex));

    SiI9233Init();
		TimerStart(SiI_9233_POLLING_TMR, 5);

    while (1)
    {
//        if (!DEBUG_PAUSE_FIRMWARE)
        {
						SiI9233_TimerTask();
            if ((TimerCheck(SiI_9233_POLLING_TMR) == TIMER_TMO))
            {
							TimerStart(SiI_9233_POLLING_TMR, 5);
//							PollPortSwitch();  
//							PollInterrupt();
            }
//						CEC_Event_Handler();
						SiI9233_TimerTask();
						PollPortSwitch();  
						PollInterrupt();
						HdmiTask();
						/*
						wNewTickCnt = TIMER_GetTickCounter();
						if ( wNewTickCnt > wOldTickCnt )
						{
							wTickDiff = wNewTickCnt - wOldTickCnt;
						}
						else
						{ // counter wrapping
							wTickDiff = ( 0xFFFF - wOldTickCnt ) + wNewTickCnt;
						}
						wTickDiff >>= 1; 			// scaling ticks to ms
						if ( wTickDiff > 36 )
						{
							wOldTickCnt = wNewTickCnt;
							HdmiProcIfTo( wTickDiff );
						}
					*/
					}
//					DEBUG_POLL();
    }
}

