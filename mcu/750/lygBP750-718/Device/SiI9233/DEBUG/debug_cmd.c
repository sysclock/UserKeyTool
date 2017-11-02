//------------------------------------------------------------------------------
// Copyright © 2007, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "../../IIC Comm/D_IIC0.h"
#include <local_types.h>
#include <config.h>
//#include <hal.h>
#include <debug_cmd.h>



//This code used only when debug command handler ise enabled
#if (CONF__DEBUG_CMD_HANDLER == ENABLE)



//------------------------------------------------------------------------------
// Configuartion Defines
//------------------------------------------------------------------------------
#define RX_BUF_SIZE     26
#define FUNC_RES_SIZE   33      //in uint8_ts



//------------------------------------------------------------------------------
// Other Defines
//------------------------------------------------------------------------------
#define BEG_FUNC_CALLS          0xA0
#define FUNC_RESULT             0x7F

#define NO_ACK_FROM_IIC_DEV     0xF1
#define COMMAND_NOT_SUPPORTED   0xAA

#define COM_ACK                 0xFF
#define COM_ACK_SII_INTERRUPT   0xFE
#define COM_ACK_IIC_CAPTUTED    0xF0
#define COM_ACK_IIC_NOACK       0xF1
#define COM_ACK_MDDC_CAPTUTED   0xF3
#define COM_ACK_MDDC_NOACK      0xF4

#define FLG_SHORT               0x01    // Used for Ri Short Read
#define FLG_NOSTOP              0x02    // Don't release IIC Bus
#define FLG_CONTD               0x04    // Continued from previous operation

#define CTR_BUSNUM              0x01    // Mask for bus number (only 0 or 1 for now)

#define MDDC_READ       1
#define MDDC_WRITE      0
#define MDDC_NOACK      4

#define IIC_CAPTURED    1
#define IIC_NOACK       2
#define IIC_OK          0

#define TRUE      1
#define FALSE     0

#define START_FRAME_RX  's'



//------------------------------------------------------------------------------
// Local Types
//------------------------------------------------------------------------------
typedef struct {
    uint8_t Dummy;
    uint8_t Nuint8_ts;
    uint8_t Flags;
    uint8_t Ctrl;
    uint8_t SlaveAddr;
    uint8_t RegAddrL;
    uint8_t RegAddrH;
    uint8_t Data[16];
} I2CCommandType;

typedef struct {
    uint8_t Cmd;
    uint8_t notused1;
    uint8_t notused2;
    uint8_t Ctrl;
    uint8_t notused4;
    uint8_t notused5;
} ResetControlType;

typedef struct {
    uint8_t Cmd;
    uint8_t notused1;
    uint8_t notused2;
    uint8_t notused3;
    uint8_t notused4;
    uint8_t notused5;
    uint8_t Data[1];

} MiscControlType;

typedef struct {
    uint8_t Dummy;
    uint8_t Nuint8_ts;
    uint8_t Flags;
    uint8_t Ctrl;
    uint8_t SlaveAddr;
    uint8_t RegAddrL;
    uint8_t RegAddrH;
    uint8_t Data[16];
} FuncCommandType;

typedef union  CmdUnion {

    uint8_t cmdbuf[24];

    I2CCommandType I2CCommand;
    ResetControlType ResetControl;
    MiscControlType MiscControl;
    FuncCommandType FuncCommand;

} CmdType;

typedef enum {

    IIC8BARead,
    IIC10BARead,
    IIC16BARead,
    SystemState,
    VersionInfo,
    IIC8BAWrite = 0x80,
    IIC10BAWrite,
    IIC16BAWrite,
    Resets,
    SetPassiveMode,
    ResetPassiveMode,
    Syncronyze,
    GetErr,
    ResetRX,
    RXAPI_GetNCTS = 0xDB,
    RXAPI_GetABKSV = 0xDC,
    RXAPI_GetIPacket = 0xDD,
    RXAPI_GetVideoInputResolution = 0xDE,
    API_GetLastAPI_ExeTime = 0xDF,
    RXAPI_InitializeSystem = 0xE0,
    RXAPI_GetVersionInfo =  0xE1,
    RXAPI_SetVideoFormat =  0xE2,
    RXAPI_GetVideoFormat =  0xE3,
    RXAPI_GetInfo =         0xE4,
    RXAPI_SetGlobalPower =     0xE5,
    RXAPI_SetAudioVideoMute =  0xE6,
    RXAPI_GetAPIInfo =      0xE7,
    RXAPI_GetSystemInformation = 0xE8,
    RXAPI_GetTasksSchedule =    0xE9,
    RXAPI_SetMasterClock =      0xEA,
    RXAPI_GetAudioInputStatus = 0xEB,
    RXAPI_GetAudioOutputFormat = 0xEC,
    RXAPI_SetAudioOutputFormat = 0xED,
    RXAPI_SetVideoInput = 0xEE,
    API_GetWarningData = 0xEF,
    API_GetErrorData = 0xF0,

    CmdGetGPIO = 0xF1,
    CmdSendGPIO = 0xF2,
    TestSyncSWRst = 0xF3,
    TestPLLId = 0xF4,
    TX_AudioPath = 0xF5,
    AVIInfo = 0xF7,
    TX_VideoPath = 0xF8,
    TX_DE_656_AudioPresetCfg = 0xF9,
    TXVideoAudioCfg = 0xFA,
    MakeRecoveryClock = 0xFB,
    GetAudioStatData = 0xFC,
    GoAudioStatTest = 0xFD,
    OutVCfg = 0xFE,
    GetTimingPrms = 0xFF

} CommandId;

enum ComStates {
    WaitStartRX = 0,
    ReadyRX = 1,
};



//------------------------------------------------------------------------------
// Globally shared variables
//------------------------------------------------------------------------------
uint8_t DEBUG_PauseFirmware = 0;



//------------------------------------------------------------------------------
// File shared variables
//------------------------------------------------------------------------------
static uint8_t FuncRes[ FUNC_RES_SIZE ];
static uint8_t RXBuffer[ RX_BUF_SIZE ];
static uint8_t bRxIndex   = 0;
static uint8_t bCommState = WaitStartRX;
static uint8_t qBuffInUse = FALSE;
static uint8_t qFuncExe   = FALSE;
static CmdType *CmdPtr;



//------------------------------------------------------------------------------
// Function Name: BlockRead_8BA
// Function Description: Translate I2C Read from legacy call
//------------------------------------------------------------------------------
static uint8_t BlockRead_8BA(I2CCommandType * I2CCommand)
{
		uint8_t *pData = NULL;
		pData = I2C0ReadByte(I2CCommand->SlaveAddr, I2CCommand->RegAddrL, I2C_DEV_MEMORY_ADDRESS_8BIT, I2CCommand->Nuint8_ts);
		memcpy(I2CCommand->Data, pData, I2CCommand->Nuint8_ts);
//    I2C_ReadBlock(I2CCommand->SlaveAddr,I2CCommand->RegAddrL,  I2CCommand->Data,I2CCommand->Nuint8_ts);
    return IIC_OK;
}

//------------------------------------------------------------------------------
// Function Name: BlockWrite_8BA
// Function Description: Translate I2C Write from legacy call
//------------------------------------------------------------------------------
static uint8_t BlockWrite_8BA(I2CCommandType * I2CCommand)
{
//    I2C_WriteBlock(I2CCommand->SlaveAddr, I2CCommand->RegAddrL, I2CCommand->Data, I2CCommand->Nuint8_ts);
		I2C0WriteByte(I2CCommand->SlaveAddr, I2CCommand->RegAddrL, I2C_DEV_MEMORY_ADDRESS_8BIT, I2CCommand->Nuint8_ts, I2CCommand->Data);
    return IIC_OK;
}

//------------------------------------------------------------------------------
// Function Name: CommAccepted
// Function Description: Sends simple confirmation on received command
//------------------------------------------------------------------------------
static void CommAccepted(void)
{
    putchar( COM_ACK );
}

//------------------------------------------------------------------------------
// Function Name: SendHandShake
// Function Description: Sends confirmation on received command
//------------------------------------------------------------------------------
static void SendHandShake(uint8_t IICMDDCErrState)
{
    uint8_t bHandShake;

    if ( !IICMDDCErrState ){
        bHandShake = COM_ACK;
    }
    else if ( IICMDDCErrState == IIC_CAPTURED )
        bHandShake = COM_ACK_IIC_CAPTUTED;
    else if ( IICMDDCErrState == IIC_NOACK )
        bHandShake = COM_ACK_IIC_NOACK;
    else if ( IICMDDCErrState == MDDC_NOACK )
        bHandShake = COM_ACK_MDDC_NOACK;

    putchar( bHandShake );
}

//------------------------------------------------------------------------------
// Function Name: SendDataFrom8BAIICDevice
// Function Description: Sends data which has been reading from IIC device with 8 bit
//                       addressing mode.
//------------------------------------------------------------------------------
static void SendDataFrom8BAIICDevice(void)
{
    uint8_t i; uint8_t bCheckSum, bState;

    bCheckSum = 0;
    bState = BlockRead_8BA(&CmdPtr->I2CCommand);
    SendHandShake( bState );
    if( !bState ) {  // no errors
        for ( i = 0; i < CmdPtr->I2CCommand.Nuint8_ts; i++ ) {
            bCheckSum += CmdPtr->I2CCommand.Data[i];
                 putchar(CmdPtr->I2CCommand.Data[i]);
        }
        putchar( bCheckSum );
    }
}

//------------------------------------------------------------------------------
// Function Name: WriteDataTo8BAIICDevice
// Function Description: Writes data to IIC device with 8 bit addressing mode.
//------------------------------------------------------------------------------
static void WriteDataTo8BAIICDevice(void)
{
    uint8_t bState;

    bState = BlockWrite_8BA((I2CCommandType *)&CmdPtr->I2CCommand);
    SendHandShake( bState );
}

//------------------------------------------------------------------------------
// Function Name: SendResultOfFunction
// Function Description: Sends function result through UART
//------------------------------------------------------------------------------
static void SendResultOfFunction(void)
{
    uint8_t bCheckSum, i;

    bCheckSum = 0;
    for (i = 0; i < CmdPtr->FuncCommand.Nuint8_ts; i++){
        bCheckSum +=  FuncRes[i];
        putchar( FuncRes[i]);
    }

    putchar( bCheckSum );
}

//------------------------------------------------------------------------------
// Function Name: ClearFuncBuff
// Function Description: ckears function result buffer
//------------------------------------------------------------------------------
static void ClearFuncBuff(void)
{
    uint8_t i;

    for ( i = 0; i < FUNC_RES_SIZE; i++)
        FuncRes[i] = 0;
}

//------------------------------------------------------------------------------
// Function Name: ParseCommand
// Function Description: this function is used for parsing of command which received
//                       trough UART interface
//------------------------------------------------------------------------------
static void ParseCommand(void)
{
    if (CmdPtr->cmdbuf[0] >= BEG_FUNC_CALLS) // check for function space Id
    {
        ClearFuncBuff();

        if (CmdPtr->cmdbuf[0] != FUNC_RESULT)
            switch (CmdPtr->cmdbuf[0])  // function decoding
            {
                default:
                    putchar(COMMAND_NOT_SUPPORTED);
            }
    }
    else if (CmdPtr->cmdbuf[0] == FUNC_RESULT)
    {
            CommAccepted();
            SendResultOfFunction();
    }
    else
    {
        switch(CmdPtr->cmdbuf[0])  // command decoding
        {
            case IIC8BARead:
                SendDataFrom8BAIICDevice();
                break;
            case IIC8BAWrite:
                WriteDataTo8BAIICDevice();
                break;
            case SetPassiveMode:
                CommAccepted();
                DEBUG_PauseFirmware = TRUE;
                break;
            case ResetPassiveMode:
                CommAccepted();
                DEBUG_PauseFirmware = FALSE;
                break;
            default:
                putchar(COMMAND_NOT_SUPPORTED);
        }
   }

   qBuffInUse = FALSE;
}

//------------------------------------------------------------------------------
// Function Name: GetCommand
// Function Description: receives command, makes check sum check
//------------------------------------------------------------------------------
static uint8_t GetCommand(void)
{
    uint8_t bCommandLength, bCheckSum, i;
    uint8_t qResult = FALSE;

    if ( RXBuffer[0] & 0x80 )
        bCommandLength = RXBuffer[1] + 6; // 6 is header length
    else
        bCommandLength = 6;

    if( bRxIndex >= RX_BUF_SIZE){ // PC speed is too fast, it sent more uint8_ts than
                                  // processed by MCU
        bRxIndex = 0; bCommState = WaitStartRX;
    }
    else if ((bCommandLength + 1 + 1) == bRxIndex){  // it's good, but need to check bCheckSum anyway

        qBuffInUse = TRUE;
        bCheckSum = START_FRAME_RX;
        bRxIndex = 0; bCommState = WaitStartRX;
        CmdPtr = (CmdType *)&RXBuffer[0];

        for ( i = 0; i <= bCommandLength; i++)
            bCheckSum += RXBuffer[i];
        if ( bCheckSum == RXBuffer[bCommandLength + 1] )
            qResult = TRUE;
        else
            qBuffInUse = FALSE;
    }

    return qResult;
}

//------------------------------------------------------------------------------
// Function Name: DEBUG_PollHandler
// Function Description: this checks if commad was received makes parsing and
//                       executing
//------------------------------------------------------------------------------
void DEBUG_PollHandler(void)
{
    if( GetCommand() )
        ParseCommand();
}

//------------------------------------------------------------------------------
// Function Name: DEBUG_HandleReceivedChar
// Function Description: handle incoming chars from UART
//------------------------------------------------------------------------------
void DEBUG_HandleReceivedChar(uint8_t achar)
{
    static uint8_t bChCount = 0;

    if ( bCommState == ReadyRX ) {
        if ( !qBuffInUse ){
            if ( bRxIndex < RX_BUF_SIZE ){
                RXBuffer[ bRxIndex++ ] = achar;
                bChCount++;
                if ( qFuncExe && ( bChCount == 8 ) ) {
                    putchar(0xE0);
                    bChCount = 0;
                    bRxIndex = 0;
                }
            }
        }
    }
    else {
        if ( achar == START_FRAME_RX ){
            bCommState = ReadyRX;
            bChCount = 0;
        }
    }
}


#endif //(CONF__DEBUG_CMD_HANDLER == ENABLE)
