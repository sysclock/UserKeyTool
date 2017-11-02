///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <hdmitx_input.h>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2013/05/07
//   @fileversion: ITE_HDMITX_SAMPLE_3.16
//******************************************/
#ifndef _HDMITX_DEBUG_H_
#define _HDMITX_DEBUG_H_


#ifdef HDMITX_INPUT_INFO
LONG CalcPCLK();
LONG CalcAudFS();
LONG CalcRCLK();
BYTE hdmitx_getAudioCount() ;

USHORT hdmitx_getInputHTotal();
USHORT hdmitx_getInputVTotal();
BOOL hdmitx_isInputInterlace();
#endif

#endif
