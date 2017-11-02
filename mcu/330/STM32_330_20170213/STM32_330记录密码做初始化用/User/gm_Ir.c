/*
    $Workfile:   gm_Ir.c  $
    $Revision:   1.18  $
    $Date:   Mar 17 2008 20:40:22  $
*/

#define __GM_IR_C__
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
// MODULE:      gm_Ir.c
//
// USAGE:       
//
//******************************************************************************

//******************************************************************************
//  I N C L U D E    F I L E S
//******************************************************************************

#include <string.h>                 // memset, memcpy, etc

#include "LibInc\gm_Global.h"
#include "LibInc\gm_Register.h"
#include "LibInc\gm_HostReg.h"
#include "LibInc\gm_Stdlib.h"
#include "LibInc\gm_mcu186.h"
#include "LibInc\gm_GlobalVariables.h"

#include "LibInc\gm_Ir.h"

//******************************************************************************
#if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx)) //in FLI30436 IR library is lcated in IROM

enum IrStates
{
   IR_STATE_START0 = 0,
   IR_STATE_START1,
   IR_STATE_START2,
   IR_STATE_START3,
   IR_STATE_TROUGH,
   IR_STATE_PAYLOAD,
   IR_STATE_TERM
};

gmt_Ir far* gmp_Ir; //IR internal global data 

//******************************************************************************
//  C O D E
//******************************************************************************

//******************************************************************************
//
// FUNCTION       : void gm_IrSetCallBack(void (*IrCallBack)(WORD))
//
// USAGE          : Revectors the Ir ISR to an Application defined Callback 
//                  routine to handle Raw Decoding of the IR Data in the App.
//                  Defining a callback routine means that the IR Decoding is
//                  superceeded in the ISR.  The ISR aquires the data, calls
//                  the callback routine and exits without attempting to
//                  decode the information
//
// INPUT          :   IrCallBack - Pointer to a function of type;
//                            void callback(WORD)
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
void far gm_IrSetCallBack(void (far* IrCallBack)(WORD))
{
    gmp_Ir->P_IrCallBack = IrCallBack;
}

//******************************************************************************
//
// FUNCTION       : gmt_RET_STAT huge gm_IrGetKey(WORD far* Wp_Cmd, WORD far* Wp_Key,  BYTE far* Bp_Index)
//
// USAGE          : This routine is called in an Application Timer at some 
//                  interval (20ms recommended) which is less than the period of
//                  the protocol (100ms typically.)
//
// INPUT          : Wp_Cmd       - Pointer to Word accepting Command code.
//                  Wp_Key       - Pointer to Word accepting raw Key code.
//                  Bp_Index     - Pointer to Byte accepting Packet Index.
//
// OUTPUT         : Cmd          - Remote Command code
//                  Key          - Raw keycode of keypressed
//                  Index        - Index (0 based) of packet structure that 
//                  decoded the key.
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
gmt_RET_STAT far gm_IrGetKey(WORD far* Wp_Cmd, WORD far* Wp_Key,  BYTE far* Bp_Index)
{
    if (gmp_Ir->valid != OK)
        return gmp_Ir->valid;
    
    *Wp_Cmd = gmp_Ir->cmd;
    *Wp_Key = gmp_Ir->key;
    *Bp_Index = gmp_Ir->index;
    gmp_Ir->valid = ERR_NO_DATA;
    return OK;
}

//******************************************************************************
//
// USAGE          : The default IR Interrupt Service Routine (ISR).  Gets
//                  data from hardware, passes bit timing to the appropriate
//                  Protocol State machine (RECS80, RC5, etc.)
//                  Populates local driver variables.
//
// INPUT          : W_IrqStatus     - Unused at this time
//
// OUTPUT         : None
//
// GLOBALS        : gmp_Ir->valid      - Boolean indicating Valid IR Data
//                  gmp_Ir->cmd  - Holds Command/Address code
//                  gmp_Ir->key  - Holds raw Keycode
//                  gmp_Ir->index   - Holds Index of structure that
//                  decoded the packet.
//
// USED_REGS      : None
//                                         |-- B_IrWatchdog --|
//
//  ----+  +--+ +-+ +-+ +-+    +-+ +-+ +-+ +-------------//-------------+
//      |  |  | | | | | | |    | | | | | | |                            |
//      |  |  | | | | | | |    | | | | | | |                            |
//      +--+  +-+ +-+ +-+ +-//-+ +-+ +-+ +-+                            +--+
//
//      |-------------- Burst -------------|----------- Idle -----------|
//
//      |--------------------------- Period ----------------------------|
//
//
// RECS80 State Machine.
//
//     | L | H |T| 1 | |0| |0|  //
//     
// ----+   +---+ +---+ +-+ +-+  // 
//     |   |   | |   | | | | | 
//     |   |   | |   | | | | | 
//     +---+   +-+   +-+ +-+ +- //
//                  L = Start Low
//                  H = Start High
//                  T = Trough
//                  1 = Logic 1
//                  0 = Logic 0
//
// RC5 State Machine
//
//   | S | S | T | ? | ? | ? // | E |
//     
// --+-+ +-+ +-+ + +-+ +-+ +-+ + // + +-----//
//     | | | | |   | | | | | |        |
//     | | | | |   | | | | | |        |
//     +-+ +-+ +-+-+ +-+ +-+ +-+ // +-+
//                  S = Start Bit = 1 (11b = Normal Key, 10b = +64)
//                  T = Toggle Bit = Any
//                  ? = Any
//                  E = End Packet Forced after last activity
//
// PRE_CONDITION  : 
//
// POST_CONDITION : 
//
//******************************************************************************
#define IR_POLARITY_MASK 0x80
#define IR_BIT_WIDTH_MASK 0x7F

void far gmp_IrIsr(WORD W_ISRData)
{
   if ((W_ISRData = gm_ReadRegWord(IR_STATUS_DATA)) & IR_DATA_OVERRUN) //Get Data from IR decoder
   {  // Reset FIFO on Overrun
      gm_SetRegBitsWord(IR_CONFIG, CLEAR_BUFFERS);
      return;
   }
   if ((W_ISRData & DATA_AVAILABLE) == 0) 
   {
      return; //no data
   }  

   // Application Callback
   if (gmp_Ir->P_IrCallBack != NULL_PTR)
   {
      (*(gmp_Ir->P_IrCallBack))(W_ISRData);
      return;
   }

   {  // timeout to divide parcels
      DWORD D_Now;
      if ((D_Now = gm_GetSystemTime()) > gmp_Ir->expTime)
      {     
         BYTE j = gmp_Ir->P_IrProtocol->B_NumPackets;
         while (j)
            gmp_Ir->packetStates[j--].B_State = IR_STATE_START0;
      }
      gmp_Ir->expTime = D_Now + (gmp_Ir->P_IrProtocol->B_Period / 2);
   }

   {  // Apply the Bit Information to each Packet Type in the Definition
      BYTE j;     
      for (j = 0; j < gmp_Ir->P_IrProtocol->B_NumPackets; j++)
      {
         gmt_IrPacket const far* P_Packet = &(gmp_Ir->P_IrProtocol->B_Packets[j]);
         gmt_IrPacketState far*P_PacketState = &(gmp_Ir->packetStates[j]);                
         BYTE B_BitWidth = (BYTE)(W_ISRData & IR_BIT_WIDTH_MASK); // Raw Data
         BOOL isPolarityHigh = W_ISRData & IR_POLARITY_MASK; 
            
         if (gmp_Ir->P_IrProtocol->B_Protocol == IR_PROTOCOL_RECS80)
         {
            //status = (*gFp_IrRECS80StateMachine)(&W_Cmd, &W_Data, j);

            // Advance Packet State for 0 Length Start Packet Types
            if ((P_PacketState->B_State == IR_STATE_START0) 
               && (P_Packet->B_Start0Min == 0) 
               && (P_Packet->B_Start0Max == 0))
            {
               P_PacketState->B_State = IR_STATE_START1;
            }

            if ((P_PacketState->B_State == IR_STATE_START1) 
               && (P_Packet->B_Start1Min == 0) 
               && (P_Packet->B_Start1Max == 0))
            {
               P_PacketState->B_State = IR_STATE_TROUGH;
            }
            
            if (P_PacketState->B_State == IR_STATE_START0) // RECS80 - Preamble - Start Lo
            {
               if ((!isPolarityHigh)
                  && (B_BitWidth >= P_Packet->B_Start0Min)
                  && (B_BitWidth <= P_Packet->B_Start0Max))
                  P_PacketState->B_State = IR_STATE_START1;
            }
            else if (P_PacketState->B_State == IR_STATE_START1) // RECS80 - Preamble - Start Hi
            {
               P_PacketState->B_State = ( isPolarityHigh && (B_BitWidth >= P_Packet->B_Start1Min) &&
                  (B_BitWidth <= P_Packet->B_Start1Max)) ? IR_STATE_TROUGH : IR_STATE_START0;
            }
            else if (P_PacketState->B_State ==IR_STATE_TROUGH) // RECS80 - Preamble - Trough
            {  // low state following the start lo and start hi bits.
               if ( (!isPolarityHigh) && (B_BitWidth >= P_Packet->B_TroughMin) && (B_BitWidth <= P_Packet->B_TroughMax))
               {
                  P_PacketState->B_State = IR_STATE_PAYLOAD; 
                  P_PacketState->B_StateCnt = 0; //init fields
                  P_PacketState->D_Payload = 0;       
                  P_PacketState->B_SpecialKey = 0;
                  P_PacketState->W_Address = 0;
               }
               else
                  P_PacketState->B_State = IR_STATE_START0;
            }
            else if (P_PacketState->B_State == IR_STATE_PAYLOAD) // RECS80 - Payload
            {  // Hi/Lo states following the Preamble
               // Start Hi in Payload area (Separater for Global Data)
               if ((P_Packet->B_Flags & IR_FLAG_MULT_START) && isPolarityHigh
                  && (B_BitWidth >= P_Packet->B_Start1Min) && (B_BitWidth <= P_Packet->B_Start1Max))
               {  // Aquire Address early
                  P_PacketState->W_Address = (WORD)((P_PacketState->D_Payload & P_Packet->D_AddrMask) >> P_Packet->B_AddrShift);
               }
               // Bits are High
               else if (isPolarityHigh)
               {
                  // '0'
                  if ((B_BitWidth >= P_Packet->B_Data0Min) && (B_BitWidth <= P_Packet->B_Data0Max))
                  {
                     P_PacketState->D_Payload = P_PacketState->D_Payload << 1;
                     ++P_PacketState->B_StateCnt;
                  }
                  // '1'
                  else if ((B_BitWidth >= P_Packet->B_Data1Min) && (B_BitWidth <= P_Packet->B_Data1Max))
                  {
                     P_PacketState->D_Payload = (P_PacketState->D_Payload << 1) | 0x00000001L;
                     ++P_PacketState->B_StateCnt;
                  }
                  // Error
                  else
                  {
                     P_PacketState->B_State = IR_STATE_START0;
                  }
               }
               // Trough between Bits
               else
               {
                  if ((B_BitWidth < P_Packet->B_TroughMin) || (B_BitWidth > P_Packet->B_TroughMax))
                     P_PacketState->B_State = IR_STATE_START0;
               }
            }  

            // Payload Complete. RECS80 - Termination
            if ((P_PacketState->B_State == IR_STATE_PAYLOAD) && (P_PacketState->B_StateCnt >= P_Packet->B_PayloadCount))
               P_PacketState->B_State = IR_STATE_TERM;
            
         }

         else //if (gmp_Ir->P_IrProtocol->B_Protocol == IR_PROTOCOL_RC5)
         {
            BYTE nbits; //could be maximum 3 bits in one interrupt
            if ((P_PacketState->B_State == IR_STATE_START0) || // Waiting to Start (Allow stretched intervals)
               (  (B_BitWidth >= P_Packet->B_Data0Min) && // Single Transitions
                  (B_BitWidth <= P_Packet->B_Data0Max)))
               nbits = 2;
            else if ((B_BitWidth >= P_Packet->B_Data1Min) // Double Transitions
               && (B_BitWidth <= P_Packet->B_Data1Max))
               nbits = 3;
            else
            {  //restart
               P_PacketState->B_State = IR_STATE_START0;             
               nbits = 2;              
            }

            while(nbits--)
            {
               //status = (*gFp_IrRC5StateMachine)(&W_Cmd, &W_Data, j);             
               
               // RC5 state machine is based purely on the polarity.
               if (P_PacketState->B_State == IR_STATE_START0) // RC5 - Preamble
               {
                  if (isPolarityHigh)
                     P_PacketState->B_State = IR_STATE_START1;
                  P_PacketState->B_SpecialKey = 0;
               }
               else if (P_PacketState->B_State == IR_STATE_START1) // RC5 - Preamble
               {
                  P_PacketState->B_State = isPolarityHigh ? IR_STATE_START0 : IR_STATE_START2;
               }
               else if (P_PacketState->B_State == IR_STATE_START2) // RC5 - Preamble
               {
                  P_PacketState->B_State = IR_STATE_START3;
                  if (!isPolarityHigh) 
                     P_PacketState->B_SpecialKey = 64;
               }
               else if (P_PacketState->B_State == IR_STATE_START3) // RC5 - Preamble
               {
                  P_PacketState->B_State = IR_STATE_PAYLOAD;
                  P_PacketState->B_StateCnt = 0; //init field
                  P_PacketState->D_Payload = 0; //init field
                  P_PacketState->W_Address = 0; //init field
               }
               else if (P_PacketState->B_State == IR_STATE_PAYLOAD) // RC5 - Payload
               {
                  if ((P_PacketState->B_StateCnt & 1) == 0)
                  {     
                     if (P_Packet->B_Flags & IR_FLAG_REVERSE)
                     {
                        P_PacketState->D_Payload >>= 1;
                        if (isPolarityHigh)
                           P_PacketState->D_Payload |= 0x80000000UL;                            
                     }
                     else 
                     {
                        P_PacketState->D_Payload <<= 1;
                        if (isPolarityHigh)
                           P_PacketState->D_Payload |= 1;   
                     }
                  }

                  // Payload Complete
                  if ((++P_PacketState->B_StateCnt >= P_Packet->B_PayloadCount))
                     P_PacketState->B_State = IR_STATE_TERM;               
               }

               if (P_PacketState->B_State == IR_STATE_TERM)
                  break;
               
               // Terminate Packets (RC5 Doesn't get final Bit)            
               if (nbits==1)
               {
                  if ((P_PacketState->B_State == IR_STATE_PAYLOAD) && 
                     (P_PacketState->B_StateCnt == P_Packet->B_PayloadCount - 1))
                     isPolarityHigh = TRUE;
                  else
                     break;
               }
               
            }           
         }

         // Term - This state is reached if the required number of Addr and Data bits are satisfied            
         if (P_PacketState->B_State == IR_STATE_TERM)
         {
            // Decode
            if (P_PacketState->W_Address == 0)
               P_PacketState->W_Address = (WORD)((P_PacketState->D_Payload & P_Packet->D_AddrMask) >> P_Packet->B_AddrShift);
            
            if ((P_PacketState->W_Address == P_Packet->W_CmdID) || 
               (P_Packet->B_PayloadCount == 0) || 
               (P_Packet->W_CmdID == 0xffff))
            {
               gmp_Ir->valid = OK;           
               gmp_Ir->cmd = P_PacketState->W_Address;
               gmp_Ir->key = (WORD)((P_PacketState->D_Payload & P_Packet->D_DataMask) >> P_Packet->B_DataShift) + 
                  P_PacketState->B_SpecialKey;
               gmp_Ir->index = j; 
               gmp_Ir->expTime = 0;
            }
               
            // Key Identified - Reset all Packets
            for (j = gmp_Ir->P_IrProtocol->B_NumPackets; j;)
               gmp_Ir->packetStates[--j].B_State = IR_STATE_START0;

            return;     
         }        
      }
   }
}

//******************************************************************************
//
// USAGE          : Initializes the IR driver.  Defines Protocol data structure, 
//                  programs timing registers, sets ISR vector, and enables the IR
//                  interrupt (IRQ1).
//
// INPUT          : P_Protocol - Pointer to Protocol Definition structure provided by Application.
//                            P_Ir - buffer for internal global variables
//
// OUTPUT         : return size of RAM buffer required to support defined protocol
//
// GLOBALS        : None
//
// USED_REGS      : IRQ1_IR_DECORDER        (WO)
//                  IR_CONFIG                (WO)
//                  IR_TIMEBASE_MEASURESTEP    (WO)
//                  IR_START_PULSE_CMD_END    (WO)
//
// PRE_CONDITION  : 
//
// POST_CONDITION : 
//
//******************************************************************************
WORD far gm_IrInit(gmt_IrProtocol const far *pSt_Protocol, void far *p_Ir)
{
    unsigned u = 0;
    if (pSt_Protocol != NULL_PTR)
    {
        u = sizeof(gmt_Ir) + (pSt_Protocol->B_NumPackets - MINIMAL_ARRAY_SIZE)*sizeof(gmt_IrPacketState);
        if (p_Ir != NULL_PTR)
        {            
            gm_InterruptDisable(IRQ1_IR_DECORDER, IRDECODER_MASK);

            gm_WriteRegWord(IR_CONFIG, pSt_Protocol->W_Config);
            gm_WriteRegWord(IR_TIMEBASE_MEASURESTEP, pSt_Protocol->W_Timing);
            gm_WriteRegWord(IR_START_PULSE_CMD_END, 0x00ff);
    
            // Initialization
            _fmemset(p_Ir, 0, u);
            gmp_Ir = p_Ir;
            gmp_Ir->P_IrProtocol = pSt_Protocol;
            gmp_Ir->valid = ERR_NO_DATA;
        
            gm_InterruptSetFunc(IRQ1_IR_DECORDER, gmp_IrIsr);            
        
            // Enable IR interrupt mask
            gm_InterruptEnable(IRQ1_IR_DECORDER, IRDECODER_MASK);        
            gm_SetRegBitsWord(IR_CONFIG, DATA_IRQ_EN);
        }
    }
    return u;
}
#else
    extern gmt_Ir far* gmp_Ir; //IR internal global data 
#endif

//******************************************************************************


//******************************************************************************
//
// FUNCTION       : void gm_IrIsrRamExeOn(void far* p_ram_exe_buf, WORD size)
//
// USAGE          : Download all code in RAM buffer and switch IR ISR execution to it
//
// INPUT          : p_ram_exe_buf - Pointer to RAM buffer
//                : size - size of buffer 
//
// OUTPUT         : None
//
// GLOBALS        : P_IrProtocol
//
// USED_REGS      : IRQ1_IR_DECORDER:     (WO)
//
//******************************************************************************
typedef void (far * FPT_IrIsr)(WORD);

WORD gm_IrIsrRamExeOn(void far* p_ram_exe_buf, WORD size)
{
    WORD w_ProtocolSize = offsetof(gmt_IrProtocol, B_Packets) + (gmp_Ir->P_IrProtocol->B_NumPackets )*sizeof(gmt_IrPacket);
   if (gmp_Ir && gmp_Ir->P_IrProtocol && p_ram_exe_buf &&
        (size >= w_ProtocolSize
        #if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
            + ((BYTE near*)gm_IrInit-(BYTE near*)gm_IrGetKey) //if it is in IROM no need to execute it from RAM
        #endif
        ))
   {
       #if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
         WORD w;
        #endif

        #if (FEATURE_INTERRUPT_SERVICE == REV1)
            gm_InterruptDisable(IRQ1_IR_DECORDER, IRDECODER_MASK);
        #else
            INTERRUPT_DISABLE(MISC_OCMMASK, IRDECODER_MASK);
        #endif            

      gmp_Ir->P_IrCallBack = NULL_PTR;

        #if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
          _fmemcpy(p_ram_exe_buf, gm_IrGetKey,  (w = (BYTE near*)gmp_IrIsr-(BYTE near*)gm_IrGetKey) );
          p_ram_exe_buf = ((BYTE far*)p_ram_exe_buf) + w;
        #endif

      _fmemcpy(p_ram_exe_buf, gmp_Ir->P_IrProtocol, w_ProtocolSize);
      gmp_Ir->P_IrProtocol = p_ram_exe_buf;
      p_ram_exe_buf = ((BYTE far*)p_ram_exe_buf)+w_ProtocolSize;

        #if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
          //near pointer is used in size definition because of assumed that all functions are allocated in one segment
          _fmemcpy(p_ram_exe_buf, gmp_IrIsr, (BYTE near*)gm_IrInit-(BYTE near*)gmp_IrIsr);
          gm_InterruptSetFunc(IRQ1_IR_DECORDER, (FPT_IrIsr)p_ram_exe_buf);
        #endif

        // Enable IR interrupt mask
        #if (FEATURE_INTERRUPT_SERVICE == REV1)
            gm_InterruptEnable(IRQ1_IR_DECORDER, IRDECODER_MASK);
        #else
            INTERRUPT_ENABLE(MISC_OCMMASK, IRDECODER_MASK);
        #endif            
   }
   return (w_ProtocolSize
        #if ((FEATURE_IROM != IROM_FLI30436)&&(FEATURE_IROM != IROM_FLI326xx)&&(FEATURE_IROM != IROM_FLI326xxBx))
           + ((BYTE near*)gm_IrInit-(BYTE near*)gm_IrGetKey)
       #endif
        );
}

//******************************************************************************
//
// FUNCTION       : void gm_IrIsrRamExeOff(gmt_IrProtocol const far* P_Protocol)
//
// USAGE          : Restore IR ISR execution out of ROM
//
// INPUT          : P_Protocol - Pointer to Protocol Definition structure
//                     provided by Application.
//
// OUTPUT         : None
//
// GLOBALS        : P_IrProtocol
//
// USED_REGS      : IRQ1_IR_DECORDER:     (WO)
//
//******************************************************************************
void gm_IrIsrRamExeOff(gmt_IrProtocol const far* P_Protocol)
{
   gm_IrInit(P_Protocol, gmp_Ir);
}

//******************************************************************************
//
// FUNCTION       : gmt_RET_STAT gm_IrGetData(WORD* Wp_Data)
//
// USAGE          : Retrives IR decoded data.  If there is no error, then data is
//                  copied to WORD pointed to by Wp_Data; otherwise, clears the FIFO
//                  buffer and returns error code.
//
// INPUT          : Wp_Data   -  Pointer to the received value
//
// OUTPUT         : OK
//                  ERR_NO_DATA
//                  ERR_OVERFLOW
//
// GLOBALS        : None
//
// USED_REGS      : IR_STATUS_DATA     (RO)
//                  IR_CONFIG       (WO)
//
// PRE_CONDITION  : 
//
// POST_CONDITION : 
//
//******************************************************************************
gmt_RET_STAT far gm_IrGetData(WORD* Wp_Data)
{
   if ((*Wp_Data = gm_ReadRegWord(IR_STATUS_DATA)) & IR_DATA_OVERRUN) //Get Data from IR decoder
   {  // Reset FIFO on Overrun
      gm_SetRegBitsWord(IR_CONFIG, CLEAR_BUFFERS);
      return ERR_OVERFLOW;
   }  
   return ((*Wp_Data & DATA_AVAILABLE) ? OK : ERR_NO_DATA);
}


