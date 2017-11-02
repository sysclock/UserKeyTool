///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO.c>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/
#include "typedef.h"
#include "delay.h"
#include "Mcu.h"

/*#ifdef 	_Myson_CS8953_
	#include "CS8953xfr.h"
#endif
*/

#include "IO.h"
//#include "TimerProcess.h"
#include "IT626X_drv.h"
//#include <intrins.h>
//#ifdef Debug_message
//#include <stdio.h>
//#endif
_IDATA BYTE  I2CADR = TXADR; // = RXADR;
_IDATA BYTE  I2CDEV = TXDEV;

#define USE_51_I2CIO

#ifndef USE_51_I2CIO
extern unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no);
extern unsigned char  I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat);
#endif

extern void I2CMasterAck(void);
extern void I2CStart(void); 
extern void I2CStop(void); 
extern unsigned char I2CSlaveAck( void );
extern unsigned char I2CReadByte(void);
extern void I2CWriteByte(unsigned char Data);



BYTE ReadI2C_Byte(BYTE RegAddr)
{
	BYTE p_data;
#ifdef USE_51_I2CIO
	i2c_read_byte(I2CADR, RegAddr, 1, &p_data, I2CDEV);
#else
	I2C_ReadBuf_FromDevice(I2CADR,RegAddr,  &p_data,1);
#endif

	return p_data;
}

SYS_STATUS WriteI2C_Byte(BYTE RegAddr, BYTE d)
{
#ifdef USE_51_I2CIO
	SYS_STATUS flag;
	flag=i2c_write_byte(I2CADR, RegAddr, 1, &d, I2CDEV);
	return !flag;
#else
	I2C_ByteWrite_ToDevice(I2CADR,RegAddr,d);
	return ER_SUCCESS;
#endif
}

SYS_STATUS ReadI2C_ByteN(BYTE RegAddr, BYTE *pData, int N)
{
#ifdef USE_51_I2CIO
	BOOL flag;
	flag = i2c_read_byte(I2CADR, RegAddr, N, pData, I2CDEV);
	return !flag;
#else
	I2C_ReadBuf_FromDevice(I2CADR,RegAddr,  pData,N);
	return ER_SUCCESS;
#endif
 }

SYS_STATUS WriteI2C_ByteN(BYTE RegAddr, cBYTE *pData, int N)
{
#ifdef USE_51_I2CIO
	BOOL flag;
	BYTE I2C_buf[18];
	int     i;
	for (i = 0; i < N; i++)
	{
		I2C_buf[i]=pData[i];
	}
	flag = i2c_write_byte(I2CADR, RegAddr++, N, I2C_buf, I2CDEV);
	return !flag;
#else
	BYTE I2C_buf[18];
	int     i;
	for (i = 0; i < N; i++)
	{
	I2C_buf[i]=pData[i];
	}
	I2C_ByteWriteN_ToDevice(I2CADR,RegAddr++,N,I2C_buf);
	return ER_SUCCESS;
#endif

}

///////////////////////////////////////////////////////////////////////////////
// Start: I2C for 8051
///////////////////////////////////////////////////////////////////////////////


BOOL i2c_8051_wait_ack( BYTE device )
{
   
    return !I2CSlaveAck();
}

BOOL i2c_write_byte( BYTE address, BYTE offset, BYTE byteno, BYTE *p_data, BYTE device )
{
    BYTE i;
    I2CStart();                // S

    I2CWriteByte(address&0xFE);        // slave address (W)
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("w slave address (W)( i2c_8051_wait_ack(device)==1 )\n");
        return 0;
    }

    I2CWriteByte(offset);            // offset
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("w offset( i2c_8051_wait_ack(device)==1 )\n");

        return 0;
    }

    for(i=0; i<byteno-1; i++)
    {
        I2CWriteByte(*p_data);        // write d
        if( i2c_8051_wait_ack(device)==1 )
        {        // As
            I2CStop();

            //printf("w write d( i2c_8051_wait_ack(device)==1 )\n");
            return 0;
        }
        p_data++;
    }

    I2CWriteByte(*p_data);            // write last d
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("w  write last d( i2c_8051_wait_ack(device)==1 )\n");

        return 0;
    }
    else
    {
        I2CStop();
        return 1;
    }
}


BOOL i2c_read_byte( BYTE address, BYTE offset, BYTE byteno, BYTE *p_data, BYTE device )
{
    BYTE i;

    I2CStart();                // S

    I2CWriteByte(address&0xFE);        // slave address (W)
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("r slave address (W)( i2c_8051_wait_ack(device)==1 )\n");
        return 0;
    }

    I2CWriteByte(offset);            // offset
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("r offset( i2c_8051_wait_ack(device)==1 )\n");

        return 0;
    }

    I2CStart();

    I2CWriteByte(address|0x01);        // slave address (R)
    if( i2c_8051_wait_ack(device)==1 )
    {        // As
        I2CStop();
        //printf("r slave address (R)( i2c_8051_wait_ack(device)==1 )\n");
        return 0;
    }

    for(i=0; i<byteno-1; i++)
    {
        *p_data = I2CReadByte();        // read d
    //    i2c_8051_send_ack(LOW, device);        // Am
   	 I2CMasterAck();

        p_data++;
    }

    *p_data = I2CReadByte();            // read last d
   // i2c_8051_send_ack(HIGH, device);            // NAm
    I2CStop();

    return 1;
}


///////////////////// Delay Utility ///////////////////////////
void delay1ms(USHORT ms)
{
	delay_ms(ms);
}


void HoldSystem(void)
{
//    Hold_Pin=1;
//    while(!Hold_Pin);
}

