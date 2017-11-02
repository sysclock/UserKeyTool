#define __TVP5158_C__
/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      TVP5158.c                                                  */
/*																			*/
/* USAGE:       This module contains functions for Microchip TVP5158 */
/*																																			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/****************************************************************************/

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/

//#include "../../System Code/lpc17xx.h"
#include "../../System Code/ASCII.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "../../System Code/Timer.h"
#include "../../IIC Comm/D_IIC0.h"
#include "../../IIC Comm/D_IIC1.h"
#include "TVP5158.h"
#include <stdio.h>

#define TVP5158_DEBUG				0
/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
BYTE TVP5158_GetStatus(BYTE B_ChipIndex, BYTE chNum);
BYTE TVP5158_GetSignalType(BYTE B_ChipIndex, BYTE chNum);
BYTE TVP5158_SelectRead(BYTE B_ChipIndex, BYTE B_Data);
BYTE TVP5158_SelectWrite(BYTE B_ChipIndex, BYTE B_Data);
BYTE TVP5158_VbusWrite(BYTE B_ChipIndex, WORD vbus_addr, BYTE B_Data, BYTE len);
BYTE TVP5158_OfmReset(BYTE B_ChipIndex);
BYTE TVP5158_SoftReset(BYTE B_ChipIndex);
BYTE TVP5158_CheckChipId(BYTE B_ChipIndex);
BYTE TVP5158_SetOfmMode(BYTE B_ChipIndex, BYTE mode, BYTE videoIfMode, BYTE videoSystem, BYTE cascadeStage);
BYTE TVP5158_SetAudioMode(BYTE B_ChipIndex, BYTE mode, BYTE cascadeStage);
BYTE TVP5158_SetAudVol(BYTE B_ChipIndex, BYTE vol, BYTE channel);
BYTE TVP5158_setAudMute(BYTE B_ChipIndex, BYTE enableMute, BYTE channel);
BYTE TVP5158_SetLuminanceBrightness(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
BYTE TVP5158_SetLuminanceContrast(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
BYTE TVP5158_SetLuminanceSaturation(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
BYTE TVP5158_SetLuminanceHue(BYTE B_ChipIndex, BYTE B_Value, BYTE channel);
WORD TVP5158_GetLuminanceBrightness(BYTE B_ChipIndex, BYTE channel);
WORD TVP5158_GetLuminanceContrast(BYTE B_ChipIndex, BYTE channel);
WORD TVP5158_GetLuminanceSaturation(BYTE B_ChipIndex, BYTE channel);
WORD TVP5158_GetLuminanceHue(BYTE B_ChipIndex, BYTE channel);
WORD TVP5158_ReadByte (BYTE B_ChipIndex, BYTE B_Addr);
BYTE TVP5158_WriteByte (BYTE B_ChipIndex, BYTE B_Addr, BYTE B_Data);
BYTE TVP5158_WriteData(BYTE B_ChipIndex, BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/

/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/
BYTE TVP5158_GetStatus(BYTE B_ChipIndex, BYTE chNum)
{
  BYTE status;
	BYTE readMask = (BYTE)(1 << chNum);

  if(chNum >= 4)
    return 0;

  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
  return TVP5158_ReadByte(B_ChipIndex, TVP5158_DEV_STATUS1);
}

BYTE TVP5158_GetSignalType(BYTE B_ChipIndex, BYTE chNum)
{
  BYTE status = 0;
	BYTE readMask = (BYTE)(1 << chNum);

  if(chNum >= 4)
    return 0;

  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
  status = TVP5158_ReadByte(B_ChipIndex, TVP5158_DEV_STATUS2);
	if(status & BIT7)
	{
		status = TVP5158_ReadByte(B_ChipIndex, TVP5158_VIDEO_STANDARD_STATUS) & 0x07;
		if((status == 0x01) || (status == 0x05))
			return TVP5158_NTSC;
		else
			return TVP5158_PAL;
	}
	else
		return TVP5158_NO_SIGNAL;
}

BYTE TVP5158_SelectWrite(BYTE B_ChipIndex, BYTE B_Data)
{
  return TVP5158_WriteByte (B_ChipIndex, TVP5158_DECODER_WRITE_ENABLE, B_Data);
}

BYTE TVP5158_SelectRead(BYTE B_ChipIndex, BYTE B_Data)
{
	return TVP5158_WriteByte (B_ChipIndex, TVP5158_DECODER_READ_ENABLE, B_Data);
}

BYTE TVP5158_VbusWrite(BYTE B_ChipIndex, WORD vbus_addr, BYTE B_Data, BYTE len)
{
	BYTE k = 0;
	BYTE regVal[4];
  k = 0;
  regVal[k] = (BYTE)((vbus_addr >> 0) & 0xFF);
	k++;
  regVal[k] = (BYTE)((vbus_addr >> 8) & 0xFF);
	k++;
  regVal[k] = (BYTE)((vbus_addr >> 16) & 0xFF);
	k++;
	k = TVP5158_WriteData(B_ChipIndex, TVP5158_VBUS_ADDRESS_ACCESS, regVal, k);
	if(k != OK)
		return k;
	k = OK;
  if(len)
		k = TVP5158_WriteByte (B_ChipIndex, TVP5158_VBUS_DATA_ACCESS, B_Data);
	return k;
}

BYTE TVP5158_OfmReset(BYTE B_ChipIndex)
{
	BYTE status = OK;
//  Delay100ms(1);
  status = TVP5158_SelectWrite(B_ChipIndex, 0x01);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Ofm Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}
  status = TVP5158_WriteByte (B_ChipIndex, TVP5158_MISC_OFM_CONTROL, 0x01);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Ofm Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}
  Delay100ms(1);
  return status;
}

BYTE TVP5158_SoftReset(BYTE B_ChipIndex)
{
 	BYTE status = OK;
	BYTE regVal[8];
  BYTE k;
  BYTE vbusStatus;  

  status = TVP5158_SelectWrite(B_ChipIndex, 0xF);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Soft Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}

  k=0;
	regVal[k] = 0x60;
  k++;
  regVal[k] = 0x00;
  k++;
  regVal[k] = 0xB0;
  k++;
  TVP5158_WriteData(B_ChipIndex, TVP5158_VBUS_ADDRESS_ACCESS, regVal, k);
  status = TVP5158_WriteByte (B_ChipIndex, TVP5158_VBUS_DATA_ACCESS, 0);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Soft Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}

  vbusStatus = regVal[0] | 1;
  status = TVP5158_VbusWrite(B_ChipIndex, 0xB00060, vbusStatus, 1);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Soft Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}

  vbusStatus &= ~(0x01);
  status = TVP5158_VbusWrite(B_ChipIndex, 0xB00060, vbusStatus, 1);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Soft Reset ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}
  Delay100ms(3);
  return status;  
}

BYTE TVP5158_CheckChipId(BYTE B_ChipIndex)
{
  BYTE status = OK;
  WORD regVal[2];

  status = TVP5158_SelectRead(B_ChipIndex, 1 | BIT5);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Check Chip Id ERROR!\r\n", B_ChipIndex);
		#endif
		return status;
	}
  regVal[0] = TVP5158_ReadByte(B_ChipIndex, TVP5158_CHIP_ID_MSB);
	if(regVal[0] == 0xFFFF)
	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Check Chip Id ERROR!\r\n", B_ChipIndex);
		#endif
		return (BYTE) ERR;
	}
  regVal[1] = TVP5158_ReadByte(B_ChipIndex, TVP5158_CHIP_ID_LSB);
	if(regVal[1] == 0xFFFF)
	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158 0x%02X Check Chip Id ERROR!\r\n", B_ChipIndex);
		#endif
		return (BYTE) ERR;
	}
  if(regVal[0] != 0x51 || regVal[1] != 0x58 )
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158 0x%02X Check Chip Id ERROR!, Chip_ID = 0x%02X%02X\r\n", B_ChipIndex, regVal[0], regVal[1]);
		#endif
    return (BYTE) ERR;
	}
  return OK;
}

BYTE TVP5158_SetOfmMode(BYTE B_ChipIndex, BYTE mode, BYTE videoIfMode, BYTE videoSystem, BYTE cascadeStage)
{
	int status = OK;
  BYTE B0, B1, B2, B3, B4, B5, B6, B7;
/*
  status = TVP5158_CheckChipId(B_ChipIndex);
  if(status != OK)
	{
    printf("TVP5158_checkChipId() Error\r\n");
    return status;
  }
*/
  status = TVP5158_OfmReset(B_ChipIndex);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158_ofmReset() Error\r\n");
		#endif
    return status;
  }
/*
  status = TVP5158_patchDownload(B_ChipIndex);
  if(status != OK) {
    OSA_ERROR("TVP5158_patchDownload()\n");
    return status;
  }
*/
  B1 = 0x10;
  B2 = 0x27;					// 0x25
  B3 = 0xE4;
  B4 = 0xE4;
  B5 = 0x00;
  B6 = 0x1B;
  B7 = 0x04;

  switch(mode) {

    case MCVIP_VIDEO_DECODER_MODE_2CH_D1:
      B0 = 0x90;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_D1:
		#if	Pixel_Interleaved_Mode
				B0 = 0x60;												//	Reg 0xB0 : 4-Ch -- Pixel-interleaved mode
				B1 = 0x17;												//	Reg 0xB1
		#else
				B0 = 0xA0;												//	Reg 0xB0 : Line-interleaved
//				B1 = 0x10;												//	Reg 0xB1
				B1 = 0x11;												//	Reg 0xB1
		#endif
      break;
		
    case MCVIP_VIDEO_DECODER_MODE_2CH_D1_CROP:
      B0 = 0x90;
      B1 = 0x50;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1:
      B0 = 0xA2;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_CROP:
      B0 = 0xA2;
      B1 = 0x50;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_CIF:
      B0 = 0xA3;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_CIF_CROP:
      B0 = 0xA3;
      B1 = 0x50;
      break;

    case MCVIP_VIDEO_DECODER_MODE_8CH_CIF:
      if(cascadeStage==0)
        B0 = 0xB3;
      else
        B0 = 0xB7;
      break;

    case MCVIP_VIDEO_DECODER_MODE_8CH_CIF_CROP:
      B1 = 0x50;
      if(cascadeStage==0)
        B0 = 0xB3;
      else
        B0 = 0xB7;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_PLUS_D1:
      B0 = 0xE2;
      B2 = 0x65;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_HALF_D1_PLUS_D1_CROP:
      B0 = 0xE2;
      B1 = 0x50;
      B2 = 0x65;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_CIF_PLUS_D1:
      B0 = 0xE3;
      break;

    case MCVIP_VIDEO_DECODER_MODE_4CH_CIF_PLUS_D1_CROP:
      B0 = 0xE3;
      B1 = 0x50;
      break;

    case MCVIP_VIDEO_DECODER_MODE_8CH_CIF_PLUS_D1:
      B2 = 0x65;
      if(cascadeStage==0)
        B0 = 0xF3;
      else
        B0 = 0xF7;
      break;

    case MCVIP_VIDEO_DECODER_MODE_8CH_CIF_PLUS_D1_CROP:
      B1 = 0x50;
      B2 = 0x65;
      if(cascadeStage==0)
        B0 = 0xF3;
      else
        B0 = 0xF7;
      break;

    default:
			#if(TVP5158_DEBUG == 1)
      printf("TVP5158 Unsupported mode %d\r\n", mode);
			#endif
      return (BYTE) ERR;
  }

  status = TVP5158_SelectWrite(B_ChipIndex, 0x0F);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158_SelectWrite() Error\r\n");
		#endif
    return status;
  }

	TVP5158_WriteByte (B_ChipIndex, TVP5158_OUTPUT_FORMATTER_CONTROL_2, 0x44); //
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AVD_OUTPUT_CONTROL_1, B0);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AVD_OUTPUT_CONTROL_2, B1);
	TVP5158_WriteByte (B_ChipIndex, 0xBE, 0xF0);

	TVP5158_WriteByte (B_ChipIndex, TVP5158_VSYNC_FILTER_SHIFT, 0x00);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_656_VERSION_F_BIT_CONTROL, 0x00);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_FBIT_VBIT_DECODE_CONTROL, 0x01);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_FBIT_VBIT_CONTROL, 0x16);

  status = TVP5158_SelectWrite(B_ChipIndex, 0x01);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158_SelectWrite() Error\r\n");
		#endif
    return status;
  }

  //Caution: 
  //From here on all write go to decoder 1 of 4 only
	TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_MODE_CONTROL, B2);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_CHANNEL_SELECT_1, B3);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_CHANNEL_SELECT_2, B4);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_CHANNEL_SELECT_3, B5);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_SUPER_FRAME_SIZE_LSB, B6);
	status = TVP5158_WriteByte (B_ChipIndex, TVP5158_OFM_SUPER_FRAME_SIZE_MSB, B7);
  if(status != OK)
 	{
		#if(TVP5158_DEBUG == 1)
    printf("TVP5158_SetOfmMode() Error\r\n");
		#endif
    return status;
  }
  Delay10ms(2);
  return status;
}
// AYK - 0731 start
BYTE TVP5158_SetAudioMode(BYTE B_ChipIndex, BYTE mode, BYTE cascadeStage)
{
  BYTE status = OK;
  BYTE C0, C1, C2, C3, C4, C5, C6, C7, C8;

/*
  if (cascadeStage >=2 )
  {
  	status = TVP5158_patchDownload(i2cHndl);
  	if(status != OK)
		{
			printf("TVP5158_patchDownload() Error\r\n");
			return status;
		}
  }
*/

  C0 = 0x00; // 16KHz
  C1 = 0x88; // Gain Ch1 and Ch2
  C2 = 0x88; // Gain Ch3 and Ch4
//MAT 0126
//  C4 = 0x02; // Mixer select, TDM_Chan_Number[2:0] 2: 8 channels
  C4 = 0x04; // Mixer select, TDM_Chan_Number[2:0] 4: 16 channels
  C5 = 0x00; // Audio Mute Control
  C6 = 0x00; // Analog Mixing Ratio Control 1
  C7 = 0x00; // Analog Mixing Ratio Control 2

  switch (cascadeStage)
  {
  	case 0:
      C3 = 0x70; // SD_M disabled, SD_R enabled, TVP5158 Master, DSP mode, 256-fs, 16-bit PCM, SD_R only
  	  C8 = 0x00; // First stage channels 1-4
  	  break;
  	case 1:
  	  C3 = 0x20; // SD_M disabled, SD_R disabled, Master, I2S mode
  	  C8 = 0x01; // Second stage channels 5-8
  	  break;
  	case 2:
  	  C3 = 0x20; // SD_M disabled, SD_R disabled, Master, I2S mode
  	  C8 = 0x02; // Third stage channels 9-12
  	  break;
  	case 3:
  	  C3 = 0x20; // SD_M disabled, SD_R disabled, Master, I2S mode
  	  C8 = 0x03; // Fourth stage channels 13-16
  	  break;
  	default:
      status = (BYTE) ERR;
			#if(TVP5158_DEBUG == 1)
 			printf("TVP5158_SetAudioMode() Error\r\n");
			#endif
			return status;
// 	  break;
  }

  // Audio channel Mute control

  C5 = 0x00;

 /* if((mode == MCVIP_VIDEO_DECODER_MODE_2CH_D1_PORT_A) ||
      (mode == MCVIP_VIDEO_DECODER_MODE_2CH_D1_PORT_B))
  {
      C5 = 0x0C;
   }*/

  status = TVP5158_SelectWrite(B_ChipIndex, 1);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158_SelectWrite() Error\r\n");
		#endif
		return status;
	}
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_SAMPLE_RATE_CONTROL, C0);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_ANALOG_AUDIO_GAIN_CONTROL_1, C1);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_ANALOG_AUDIO_GAIN_CONTROL_2, C2);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_MODE_CONTROL, C3);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_MIXER_SELECT, C4);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_MUTE_CONTROL, C5);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_MIXING_RATIO_CONTROL_1, C6);
	TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_MIXING_RATIO_CONTROL_2, C7);
	status = TVP5158_WriteByte (B_ChipIndex, TVP5158_AUDIO_CASCADE_MODE_CONTROL, C8);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C Audio setting cascadeStage=%d\r\n",cascadeStage);
		#endif
		return status;
	}
  Delay10ms(4);  
  return status;
}
// AYK - 0731 end

BYTE TVP5158_SetAudVol(BYTE B_ChipIndex, BYTE vol, BYTE channel)
{
  BYTE status = OK;
	BYTE regAddr = 0;
	BYTE regVal = 0;
	BYTE shiftVal = 0;
  BYTE andVal = 0;
	BYTE lowLimit = 0;
	BYTE hiLimit = 8;

  status = TVP5158_SelectWrite(B_ChipIndex, 1);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio Volume ERROR\r\n");
		#endif
    return status;
	}
	
//MAT if((vol < 0x0) || (vol > 0xF))
	if((vol < lowLimit) || (vol > hiLimit))
		vol = 0x08; // Default value(0 db)
	
    switch(channel)
    {
	    case 0:
	        shiftVal = 0;
	        andVal = 0xF0;
	        regAddr = 0xC1;
	        break;

	    case 1:
	        shiftVal = 4;
	        andVal = 0x0F;
	        regAddr = 0xC1;
	        break;

	    case 2:
	        shiftVal = 0;
	        andVal = 0xF0;
	        regAddr = 0xC2;
	        break;

	    case 3:
	        shiftVal = 4;
	        andVal = 0x0F;
	        regAddr = 0xC2;
	        break;
	}

// read the aud vol register
	status = TVP5158_SelectRead(B_ChipIndex, 1 | BIT5);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio Volume ERROR\r\n");
		#endif
		return status;
	}
	  
	regVal = TVP5158_ReadByte(B_ChipIndex, regAddr);
	regVal &= andVal;
	regVal |= (vol << shiftVal);

	status = TVP5158_WriteByte(B_ChipIndex, regAddr, regVal);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio Volume ERROR\r\n");
		#endif
		return status;
	}
  return status;
}

//This is not useful since it only applies to the TVP5158 SD_M output with mixer selected
BYTE TVP5158_setAudMute(BYTE B_ChipIndex, BYTE enableMute, BYTE channel)
{
  BYTE status = OK;
  BYTE regVal;
  BYTE	setMute = 0;
	
  setMute	= enableMute << channel;
  printf("@@@@@@@@@@@@TVP5158: setMute = 0x%02X\r\n",setMute);
  
  status = TVP5158_SelectRead(B_ChipIndex, 1 | BIT5);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio MUTE ERROR\r\n");
		#endif
		return status;
	}
	regVal = TVP5158_ReadByte(B_ChipIndex, TVP5158_AUDIO_MUTE_CONTROL);
	regVal |= setMute;
	if(enableMute > 0)
		setMute = 0x0F;
	else
		setMute = 0;
		
	regVal = setMute;

	//MAT status = TVP5158_selectWrite(i2cHndl, 0xF);
	status = TVP5158_SelectWrite(B_ChipIndex, 0x01);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio MUTE ERROR\r\n");
		#endif
		return status;
	}
    	
  status = TVP5158_WriteByte(B_ChipIndex, TVP5158_AUDIO_MUTE_CONTROL, regVal);
  if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("I2C-Audio MUTE ERROR\r\n");
		#endif
		return status;
	}
	printf(" @@@@@@@@@@@@TVP5158: C5 = %X\n",regVal);		
	return status;
}

BYTE TVP5158_SetLuminanceBrightness(BYTE B_ChipIndex, BYTE B_Value, BYTE channel)
{
  BYTE status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectWrite(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
	status = TVP5158_WriteByte(B_ChipIndex, TVP5158_LUMINANCE_BRIGHTNESS, B_Value);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158_SetLuminanceBrightness ERROR\r\n");
		#endif
		return status;
	}
  return status;
}

BYTE TVP5158_SetLuminanceContrast(BYTE B_ChipIndex, BYTE B_Value, BYTE channel)
{
  BYTE status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectWrite(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
	status = TVP5158_WriteByte(B_ChipIndex, TVP5158_LUMINANCE_CONTRAST, B_Value);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158_SetLuminanceBrightness ERROR\r\n");
		#endif
		return status;
	}
  return status;
}

BYTE TVP5158_SetLuminanceSaturation(BYTE B_ChipIndex, BYTE B_Value, BYTE channel)
{
  BYTE status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectWrite(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
	status = TVP5158_WriteByte(B_ChipIndex, TVP5158_LUMINANCE_SATURATION, B_Value);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158_SetLuminanceBrightness ERROR\r\n");
		#endif
		return status;
	}
  return status;
}

BYTE TVP5158_SetLuminanceHue(BYTE B_ChipIndex, BYTE B_Value, BYTE channel)
{
  BYTE status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectWrite(B_ChipIndex, readMask);
  if(status != OK)
    return 0;
	status = TVP5158_WriteByte(B_ChipIndex, TVP5158_LUMINANCE_HUE, B_Value);
	if(status != OK)
	{
		#if(TVP5158_DEBUG == 1)
		printf("TVP5158_SetLuminanceBrightness ERROR\r\n");
		#endif
		return status;
	}
  return status;
}

WORD TVP5158_GetLuminanceBrightness(BYTE B_ChipIndex, BYTE channel)
{
  WORD status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0xFFFF;
	status = TVP5158_ReadByte(B_ChipIndex, TVP5158_LUMINANCE_BRIGHTNESS);
  return status;
}

WORD TVP5158_GetLuminanceContrast(BYTE B_ChipIndex, BYTE channel)
{
  WORD status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0xFFFF;
	status = TVP5158_ReadByte(B_ChipIndex, TVP5158_LUMINANCE_CONTRAST);
  return status;
}

WORD TVP5158_GetLuminanceSaturation(BYTE B_ChipIndex, BYTE channel)
{
  WORD status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0xFFFF;
	status = TVP5158_ReadByte(B_ChipIndex, TVP5158_LUMINANCE_SATURATION);
  return status;
}

WORD TVP5158_GetLuminanceHue(BYTE B_ChipIndex, BYTE channel)
{
  WORD status = 0;
	BYTE readMask = (BYTE)(1 << channel);

  if(channel >= 4)
    return 0;
  status = TVP5158_SelectRead(B_ChipIndex, readMask);
  if(status != OK)
    return 0xFFFF;
	status = TVP5158_ReadByte(B_ChipIndex, TVP5158_LUMINANCE_HUE);
  return status;
}

WORD TVP5158_ReadByte(BYTE B_ChipIndex, BYTE B_Addr)
{
	BYTE* pB_Data = NULL;
	WORD W_Data = 0xFFFF;
	switch (B_ChipIndex)
	{
		/*
		case TVP5158_DEV0:
			if ((pB_Data = I2C1ReadByte(TVP5158_I2C_ADD_DEV0, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C1ReadByte(TVP5158_I2C_ADD_DEV0, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV0, TVP5158_I2C_ADD_DEV0);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = (BYTE) *pB_Data;
			}
			else
				W_Data = (BYTE) *pB_Data;
			break;
		case TVP5158_DEV1:
			if ((pB_Data = I2C1ReadByte(TVP5158_I2C_ADD_DEV1, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C1ReadByte(TVP5158_I2C_ADD_DEV1, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV1, TVP5158_I2C_ADD_DEV1);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = (BYTE) *pB_Data;
			}
			else
				W_Data = (BYTE) *pB_Data;
			break;
		*/
		case TVP5158_DEV2:
			if ((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV2, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV2, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV2, TVP5158_I2C_ADD_DEV2);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = (BYTE) *pB_Data;
			}
			else
				W_Data = (BYTE) *pB_Data;
			break;
		case TVP5158_DEV3:
			if ((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV3, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV3, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV3, TVP5158_I2C_ADD_DEV3);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = (BYTE) *pB_Data;
			}
			else
				W_Data = (BYTE) *pB_Data;
			break;
		case TVP5158_DEV4:
			if ((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV4, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV4, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV4, TVP5158_I2C_ADD_DEV4);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = (BYTE) *pB_Data;
			}
			else
				W_Data = (BYTE) *pB_Data;
			break;
		case TVP5158_DEV5:
			if ((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV5, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
			{
				Delay10ms(1);
				if((pB_Data = I2C0ReadByte(TVP5158_I2C_ADD_DEV5, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1)) == NULL)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X read ERROR!\r\n", TVP5158_DEV5, TVP5158_I2C_ADD_DEV5);
					#endif
					W_Data = 0xFFFF;
				}
				else
					W_Data = *pB_Data;
			}
			else
				W_Data = *pB_Data;
			#if(TVP5158_DEBUG == 2)
			printf("TVP5158 0x%02X read:[0x%02X] = 0x%02X\r\n", TVP5158_I2C_ADD_DEV5, B_Addr, W_Data);
			#endif
			break;
		default:
			W_Data = 0xFFFF;
			#if(TVP5158_DEBUG == 1)
			printf("TVP5158 read ERROR, NO This Dev!\r\n");
			#endif
			break;
	}
	return W_Data;
}

BYTE TVP5158_WriteByte (BYTE B_ChipIndex, BYTE B_Addr, BYTE B_Data)
{
	BYTE i = 0;
	switch (B_ChipIndex)
	{
		/*
		case TVP5158_DEV0:
			i = 0;
			while (I2C1WriteByte(TVP5158_I2C_ADD_DEV0, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV0, TVP5158_I2C_ADD_DEV0);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV1:
			i = 0;
			while (I2C1WriteByte(TVP5158_I2C_ADD_DEV1, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV1, TVP5158_I2C_ADD_DEV1);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		*/
		case TVP5158_DEV2:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV2, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV2, TVP5158_I2C_ADD_DEV2);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV3:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV3, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV3, TVP5158_I2C_ADD_DEV3);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV4:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV4, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV4, TVP5158_I2C_ADD_DEV4);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV5:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV5, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &B_Data) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write ERROR!\r\n", TVP5158_DEV5, TVP5158_I2C_ADD_DEV5);
					#endif
					return (BYTE) ERR;
				}
			}
			#if(TVP5158_DEBUG == 2)
			printf("TVP5158 0x%02X Write:[0x%02X] = 0x%02X\r\n", TVP5158_I2C_ADD_DEV5, B_Addr, B_Data);
			#endif
			break;
		default:
			#if(TVP5158_DEBUG == 1)
			printf("TVP5158 write ERROR, NO This Dev!\r\n");
			#endif
			return (BYTE) ERR;
	}
	return OK;
}

BYTE TVP5158_WriteData(BYTE B_ChipIndex, BYTE B_Addr, BYTE *Bp_Buf, BYTE B_Len)
{
	BYTE i = 0;
	#if(TVP5158_DEBUG == 2)
	BYTE *Bp_TempData = NULL;
	#endif
	switch (B_ChipIndex)
	{
		/*
		case TVP5158_DEV0:
			i = 0;
			while (I2C1WriteByte(TVP5158_I2C_ADD_DEV0, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV0, TVP5158_I2C_ADD_DEV0);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV1:
			i = 0;
			while (I2C1WriteByte(TVP5158_I2C_ADD_DEV1, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV1, TVP5158_I2C_ADD_DEV1);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		*/
		case TVP5158_DEV2:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV2, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV2, TVP5158_I2C_ADD_DEV2);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV3:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV3, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV3, TVP5158_I2C_ADD_DEV3);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV4:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV4, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV4, TVP5158_I2C_ADD_DEV4);
					#endif
					return (BYTE) ERR;
				}
			}
			break;
		case TVP5158_DEV5:
			i = 0;
			while (I2C0WriteByte(TVP5158_I2C_ADD_DEV5, B_Addr, I2C_DEV_MEMORY_ADDRESS_8BIT, B_Len, Bp_Buf) == (BYTE) ERR)
			{
				Delay10ms(1);
				i++;
				if(i == 5)
				{
					#if(TVP5158_DEBUG == 1)
					printf("TVP5158 0x%02X, I2CDEV 0x%02X write Data ERROR!\r\n", TVP5158_DEV5, TVP5158_I2C_ADD_DEV5);
					#endif
					return (BYTE) ERR;
				}
			}
			#if(TVP5158_DEBUG == 2)
			for(i = 0, Bp_TempData = Bp_Buf; i < B_Len; i++, Bp_TempData++)
				printf("TVP5158 0x%02X Write:[0x%02X] = 0x%02X\r\n", TVP5158_I2C_ADD_DEV5, B_Addr + i, *Bp_TempData);
			#endif
			break;
		default:
			#if(TVP5158_DEBUG == 1)
			printf("TVP5158 Write Data ERROR, NO This Dev!\r\n");
			#endif
			return (BYTE) ERR;
	}
	return OK;
}
/****************************************************************************/
/*																			*/
/* FUNCTION:    TVP5158_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void TVP5158_DummyCall(void)
{

}
/********************************  END  *************************************/

