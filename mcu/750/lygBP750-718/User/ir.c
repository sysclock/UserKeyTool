/*
    $Workfile:   ir.c  $
    $Revision: 1.12 $
    $Date: 2013/01/18 05:48:14 $
*/

#define __IR_C__
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
// MODULE:      IR.c
//
// USAGE:       This module gets IR-remote data from the software driver and
//		performs key debounce to filter out repeated keys sent by the remote.
//
//******************************************************************************

//******************************************************************************
//  I N C L U D E    F I L E S
//******************************************************************************

#include "System\all.h"
#include "StdLibInc\gm_Register.h"
#if (FEATURE_LOW_POWER_MONITOR == ENABLE)
#include "StdLibInc\gm_LPM_IPC.h"
#endif

//******************************************************************************
//  E X T E R N S
//******************************************************************************

#ifdef IR_DEBUG_USE_LOCAL_REMOTE_TABLE
extern ST_IR ROM IRDebugValues[];
#endif

//******************************************************************************
//  L O C A L   F U N C T I O N    P R O T O T Y P E S
//******************************************************************************

KEY_INDEX	IrTranslateKey(BYTE B_Value);

//******************************************************************************
//  L O C A L   V A R I A B L E S
//******************************************************************************
//that memory could be reduced in customer project because of instead of IR_MAX_PACKET_TYPES
//should be used IrProtocol.B_NumPackets (compiler and design limitation)
#ifndef LPM_PERMANENTLY_CONTROLS_PERIPHERAL
#if 0 // not implemented
BYTE IR_Global[sizeof(gmt_Ir) + (IR_MAX_PACKET_TYPES - MINIMAL_ARRAY_SIZE)*sizeof(gmt_IrPacketState)];
#endif
#endif

#ifdef IR_DEBUG_CHARACTERIZE
WORD	W_IrDebugBuffer[0x40];
BYTE	B_IrDebugHead = 0;
BYTE	B_IrDebugTail = 0;
#endif

//******************************************************************************
//  C O D E
//******************************************************************************

#include "DCDI-01.h"


#ifdef IR_ISR_RAM_EXE
BYTE ir_isr_ram_exe_buf[OFFMODE_IR_RAM_BUF];
#endif

#pragma argsused	// Supress compiler warning message
//******************************************************************************
//
// FUNCTION       : void IrCallBack(WORD W_Data)
//
// USAGE          : This function is called directly from IR interrupt.
//                  The application has complete control of received data and
//                  driver will not handle IR data anymore.
//
// INPUT          : W_Data
//
// OUTPUT         : None
//
// GLOBALS        : None
//
// USED_REGS      : None
//
// PRE_CONDITION  :
//
// POST_CONDITION :
//
//******************************************************************************
void IrCallBack(WORD W_Data)
{
#ifdef IR_DEBUG_CHARACTERIZE
   W_IrDebugBuffer[B_IrDebugHead] = W_Data;
   B_IrDebugHead = (B_IrDebugHead + 1) & 0x3F;
#endif
}

#if defined(PORT_IR_USED)
//******************************************************************************
//
// FUNCTION       : KEY_INDEX IrTranslateKey(BYTE B_Value)
//
// USAGE          : Compares Ir value with table code by the method of binary search
//
// INPUT          : B_Value
//
// OUTPUT         : KEY_INDEX - id of pushed key.  If none of the key is pressed, return Key_None
//
// GLOBALS        : Number_Of_IRValues - number of members in array IRValues[]
//                  IRValues - translating Ir code to id key.
//
// USED_REGS      : None
//
// PRE_CONDITION  :
//
// POST_CONDITION :
//
//******************************************************************************
KEY_INDEX IrTranslateKey(BYTE B_Value)
{
   BYTE 		B_Result;
   SWORD 		B_Offset;
   SWORD 		B_Min = 0;
   SWORD 		B_Max = NUMBER_OF_IRValues - 1;
   KEY_INDEX	B_Key = Key_None;
   ST_IR ROM  *P_IRValue = 0;

   //
   // ScanRemoteKey to see if a new value was found.
   //
   while ((B_Value != 0xff) && (B_Min <= B_Max))
   {
      //
      // For IR the codes must match exactly, so do a binary search only
      // returning the value when the codes are exactly equal.
      //
      B_Offset = ((WORD)B_Max + B_Min) / 2;
#ifdef IR_DEBUG_USE_LOCAL_REMOTE_TABLE
      P_IRValue = &IRDebugValues[B_Offset];
#else
      P_IRValue = &IRValues[B_Offset];
#endif

      B_Result = P_IRValue->Code;
      if (B_Result == B_Value)
      {
         B_Key = P_IRValue->KeyIndex;
         B_Value = 0xff; // End Search
      }
      else
      {
         if (B_Result > B_Value)
            B_Max = B_Offset - 1;
         else
            B_Min = B_Offset + 1;
      }
   }

   return(B_Key);
}
#endif

#if defined(PORT_IR_USED)
//******************************************************************************
//
// FUNCTION       : void IrHandler(void)
//
// USAGE          : Processes IR remote key
//                  This function is called by timer interrupt
//
// INPUT          : None
//
// OUTPUT         : None
//
// GLOBALS        : None
//
// USED_REGS      :
//
// PRE_CONDITION  :
//
// POST_CONDITION :
//
//******************************************************************************
void IrHandler(void)
{
   BYTE 			B_Key = Key_None;
   WORD			W_Cmd;
   WORD			W_Data;
   BYTE			B_Index;
   DWORD			D_Now = gm_GetSystemTime();

   static BYTE 	B_PrevKey;// = 0;
   static WORD 	W_PrevData;// = 0;
   static DWORD	D_PrevKeyTime;// = 0;

#ifdef IR_DEBUG_KEY
   WORD			W_Elapsed;
#endif

   //
   // Debug
   //
#ifdef IR_DEBUG_CHARACTERIZE
   while (B_IrDebugTail != B_IrDebugHead)
   {
      W_Data = W_IrDebugBuffer[B_IrDebugTail];
      B_IrDebugTail = (B_IrDebugTail + 1) & 0x3F;
      gm_Printf("(%d) %d", (W_Data & IR_POLARITY_MASK) >> 7, W_Data & IR_BIT_WIDTH_MASK);
   }
#endif

   //
   // Key Release
   //
   if ((B_PrevKey != 0xff) && ((D_Now - D_PrevKeyTime) > (IrProtocol.B_Period + 30)))
   {
      gmvb_ValidIrData = Key_None;
      EnqueueOsdKey();
      B_PrevKey = 0xff;
#ifdef IR_DEBUG_KEY
      gm_Printf("** Release", 0);
#endif
   }

   //
   // Get Keys
   //
   if (gm_IrGetKey(&W_Cmd, &W_Data, &B_Index) == OK)
   {
      //
      // Special Keys
      //
      if (IrProtocol.B_Packets[B_Index].B_Flags & IR_FLAG_REPEAT)
      {
         W_Data = W_PrevData;
      }
      if (IrProtocol.B_Packets[B_Index].B_Flags & IR_FLAG_POWER)
      {
//			gm_Print("IR: System Power Key", 0);
      }
      W_PrevData = W_Data;

      //
      // Debugging (Keep me)
      //
#ifdef IR_DEBUG_KEY
      D_PrevKeyTime = (D_Now - D_PrevKeyTime);
      if (D_PrevKeyTime > 0xffff)
         D_PrevKeyTime = 0xffff;
      W_Elapsed = (WORD)(D_PrevKeyTime & 0xffff);
      gm_Printf("Cmd:0x%X Key:0x%X Inx:%d @ %d", W_Cmd, W_Data, B_Index, W_Elapsed);
#endif

      //
      // Queue New Keys
      //
      B_Key = IrTranslateKey(W_Data);
      if (B_Key != B_PrevKey)
      {
         gmvb_ValidIrData = B_Key;
         EnqueueOsdKey();
         B_PrevKey = B_Key;
      }
      D_PrevKeyTime = D_Now;

      //
      // OK key can abort auto-adjust (user specific)
      //
#ifdef AUTOADJUST_ABORT_KEY
      if ((B_AutoAdjustStatus[gmvb_MainChannel] == AA_START ||
            B_AutoAdjustStatus[gmvb_PipChannel] == AA_START)	&& // AA started
            B_Key == Key0004)						// IR_OK
      {
         B_AutoAdjustStatus[gmvb_MainChannel] = AA_ABORT;	// User want to abort
         B_AutoAdjustStatus[gmvb_PipChannel] = AA_ABORT;	// User want to abort
      }
#endif

   }
}
#endif

#if (FEATURE_SOCIPC == ENABLE)
   #ifdef ST_4K2K_93xx_BOARD  
      #if ENABLE_SOC_IPC_IR_VIA_QUEUE
      IRRecvFormat_t SocIPCIRKeyBuf;
      #else //Original
      BOOL g_IR_GetValidKeys = FALSE;
      BYTE g_IR_Key = 0;
      #endif
   #endif
#endif

#ifdef WBK_FUNCTION_GETIRVALUE_USED	// WB4
//******************************************************************************
//
// FUNCTION       : BYTE GetIRValue( BYTE B_DriverIndex )
//
// USAGE          : IR driver function. Processes IR remote key.
//                  This function is called by timer interrupt
//
// INPUT          : B_DriverIndex - index of this keypad in WBK_Keypads[].
//
// OUTPUT         : TRUE or FALSE
//
// GLOBALS        : WBK_Keypads[]
//
// USED_REGS      : None
//
// PRE_CONDITION  :
//
// POST_CONDITION :
//
//******************************************************************************
#if (FEATURE_SOCIPC == ENABLE)
BYTE GetIRValue( BYTE B_DriverIndex )	// WB4 function
{
#ifdef ST_4K2K_93xx_BOARD  
   BYTE 		B_Key = Key_None;
   WORD			W_Cmd;
   WORD			W_Data;
   BYTE			B_Index;
   DWORD			D_Now = gm_GetSystemTime();

   static BYTE 	B_PrevKey;// = 0;
   static DWORD	D_PrevKeyTime = 0;
   static BYTE count;// = 0;
   WBK_KEYPAD_DEFINITION  ROM *Sp_Keypad = (WBK_KEYPAD_DEFINITION ROM*) WBK_Keypads[B_DriverIndex];

#ifdef IR_DEBUG_KEY
   WORD			W_Elapsed;
#endif


   if (++count >= 3)
   {
      count = 0;//call handler every 24ms (= 8ms*3; every third iteration) SCR#2088
   }
   else
      return FALSE;


   // Key Release
   if ((B_PrevKey != 0xff) && ((D_Now - D_PrevKeyTime) > (IrProtocol.B_Period + 40)) && ((D_Now - D_PrevKeyTime) < 700))	//700 is arbitrary.  This is to take away the case when powering down, system time will reset to 0
   {
      {
         gmvb_ValidIrData = Key_None;
		 
#if ENABLE_SOC_IPC_IR_VIA_QUEUE
		SocIPCIRKeyBuf.IRKey = Key_None;
		SocIPCIRKeyBuf.ValidKey = TRUE;
		SocIPCIRBufferInsertKey(SocIPCIRKeyBuf);		
#else //Original
         g_IR_Key = Key_None;
		 g_IR_GetValidKeys = TRUE;
#endif

         B_PrevKey = 0xff;
      }
#ifdef IR_DEBUG_KEY
      gm_Printf("** Release", 0);
#endif
   }

   // Get Keys
#ifdef LPM_PERMANENTLY_CONTROLS_PERIPHERAL
   {
      BYTE buf[3];
      if (!gm_MessageReceived(gmd_MESSAGE_IR, buf, sizeof(buf)))
         return TRUE;
      W_Cmd = buf[0];
      W_Data = buf[1];
      B_Index = buf[2];
      W_Cmd=W_Cmd;
   }
#else
   // Misssion has not support IR.
   return FALSE;
#endif


   // Debugging (Keep me)
#ifdef IR_DEBUG_KEY
   D_PrevKeyTime = (D_Now - D_PrevKeyTime);
   if (D_PrevKeyTime > 0xffff)
      D_PrevKeyTime = 0xffff;
   W_Elapsed = (WORD)(D_PrevKeyTime & 0xffff);
   gm_Printf("Cmd:0x%X Key:0x%X Inx:%d @ %d", W_Cmd, W_Data, B_Index, W_Elapsed);
#endif

   if (IrProtocol.B_Packets[B_Index].B_Flags & IR_FLAG_REPEAT)
      B_Key = B_PrevKey; // Special process for REPEAT Key
   else
      B_Key = TranslateCodeToKey( Sp_Keypad->Bp_KeyValues,Sp_Keypad->B_KeysNumber, Sp_Keypad->B_CodeLen, Sp_Keypad->B_Delta, W_Data);

   // Queue New Keys
   if (B_Key != B_PrevKey)
   {
      {
         gmvb_ValidIrData = B_Key;
#if ENABLE_SOC_IPC_IR_VIA_QUEUE
		SocIPCIRKeyBuf.IRKey = B_Key;
#else //Original
		 g_IR_Key = B_Key;
#endif
         B_PrevKey = B_Key;
#if ENABLE_SOC_IPC_IR_VIA_QUEUE		 
		SocIPCIRKeyBuf.ValidKey = TRUE;
		SocIPCIRBufferInsertKey(SocIPCIRKeyBuf);
#else //Original
		 g_IR_GetValidKeys = TRUE;		 
#endif
      }
   }
   D_PrevKeyTime = D_Now;

   return TRUE;

#else
   UNUSED_VAR(B_DriverIndex); // For compiler warning ...
	return FALSE;
#endif
}
#else //Original
BYTE GetIRValue( BYTE B_DriverIndex )	// WB4 function
{
   BYTE 			B_Key = Key_None;
   WORD			W_Cmd;
   WORD			W_Data;
   BYTE			B_Index;
   DWORD			D_Now = gm_GetSystemTime();

   static BYTE 	B_PrevKey;// = 0;
   static DWORD	D_PrevKeyTime = 0;
   static BYTE count;// = 0;
   WBK_KEYPAD_DEFINITION  ROM *Sp_Keypad = (WBK_KEYPAD_DEFINITION ROM*) WBK_Keypads[B_DriverIndex];

#ifdef IR_DEBUG_KEY
   WORD			W_Elapsed;
#endif


   if (++count >= 3)
   {
      count = 0;//call handler every 24ms (= 8ms*3; every third iteration) SCR#2088
   }
   else
      return FALSE;

   //
   // Debug
   //
#ifdef IR_DEBUG_CHARACTERIZE
   while (B_IrDebugTail != B_IrDebugHead)
   {
      W_Data = W_IrDebugBuffer[B_IrDebugTail];
      B_IrDebugTail = (B_IrDebugTail + 1) & 0x3F;
      gm_Printf("(%d) %d", (W_Data & IR_POLARITY_MASK) >> 7, W_Data & IR_BIT_WIDTH_MASK);
   }
#endif

   // Key Release
   if ((B_PrevKey != 0xff) && ((D_Now - D_PrevKeyTime) > (IrProtocol.B_Period + 40)) && ((D_Now - D_PrevKeyTime) < 700))	//700 is arbitrary.  This is to take away the case when powering down, system time will reset to 0
   {
      {
         gmvb_ValidIrData = Key_None;
         EnqueueOsdKey(Key_None);
         B_PrevKey = 0xff;
      }
#ifdef IR_DEBUG_KEY
      gm_Printf("** Release", 0);
#endif
   }

   // Get Keys
#ifdef LPM_PERMANENTLY_CONTROLS_PERIPHERAL
   {
      BYTE buf[3];
      if (!gm_MessageReceived(gmd_MESSAGE_IR, buf, sizeof(buf)))
         return TRUE;
      W_Cmd = buf[0];
      W_Data = buf[1];
      B_Index = buf[2];
      W_Cmd=W_Cmd;
   }
#else
   // Misssion has not support IR.
   return FALSE;
#endif


   // Debugging (Keep me)
#ifdef IR_DEBUG_KEY
   D_PrevKeyTime = (D_Now - D_PrevKeyTime);
   if (D_PrevKeyTime > 0xffff)
      D_PrevKeyTime = 0xffff;
   W_Elapsed = (WORD)(D_PrevKeyTime & 0xffff);
   gm_Printf("Cmd:0x%X Key:0x%X Inx:%d @ %d", W_Cmd, W_Data, B_Index, W_Elapsed);
#endif

   if (IrProtocol.B_Packets[B_Index].B_Flags & IR_FLAG_REPEAT)
      B_Key = B_PrevKey; // Special process for REPEAT Key
   else
      B_Key = TranslateCodeToKey(
#ifdef IR_DEBUG_USE_LOCAL_REMOTE_TABLE
                 IRDebugValues,
#else
                 Sp_Keypad->Bp_KeyValues,
#endif
                 Sp_Keypad->B_KeysNumber, Sp_Keypad->B_CodeLen, Sp_Keypad->B_Delta, W_Data);

   // Queue New Keys
   if (B_Key != B_PrevKey)
   {
      {
         gmvb_ValidIrData = B_Key;
         EnqueueOsdKey(B_Key);
         B_PrevKey = B_Key;
      }
   }
   D_PrevKeyTime = D_Now;

   // OK key can abort auto-adjust (user specific)
   /*	  #ifdef AUTOADJUST_ABORT_KEY
   		if (B_AutoAdjustStatus == AA_START &&		// AA started
   			B_Key == Key0004)						// IR_OK
   		{
   			B_AutoAdjustStatus = AA_ABORT;			// User want to abort
   		}
   	  #endif
   */
   return TRUE;
}
#endif
#endif

#ifdef WBK_FUNCTION_INITIRDRIVER_USED
//******************************************************************************
//
// FUNCTION       : BYTE InitIRDriver( BYTE B_Index )
//
// USAGE          : Empty.
//
// INPUT          : B_Index
//
// OUTPUT         : None
//
// GLOBALS        : None
//
// USED_REGS      : None
//
// PRE_CONDITION  :
//
// POST_CONDITION :
//
//******************************************************************************
BYTE InitIRDriver( BYTE B_Index )	// WB4 function
{
   return B_Index;
}
#endif
/***********************************  END  ************************************/

