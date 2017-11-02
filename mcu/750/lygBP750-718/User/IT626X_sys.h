///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT626X_sys.h>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/

#ifndef _CAT6611_SYS_H_
#define _CAT6611_SYS_H_
////////////////////////////////////////////////////////////////////////////////
// Internal Data Type
////////////////////////////////////////////////////////////////////////////////

typedef enum tagHDMI_Aspec {
    HDMI_4x3 ,
    HDMI_16x9
} HDMI_Aspec;

typedef enum tagHDMI_OutputColorMode {
    HDMI_RGB444,
    HDMI_YUV444,
    HDMI_YUV422
} HDMI_OutputColorMode ;

typedef enum tagHDMI_Colorimetry {
    HDMI_ITU601,
    HDMI_ITU709
} HDMI_Colorimetry ;

///////////////////////////////////////////////////////////////////////
// Output Mode Type
///////////////////////////////////////////////////////////////////////

#define RES_ASPEC_4x3 0
#define RES_ASPEC_16x9 1
#define F_MODE_REPT_NO 0
#define F_MODE_REPT_TWICE 1
#define F_MODE_REPT_QUATRO 3
#define F_MODE_CSC_ITU601 0
#define F_MODE_CSC_ITU709 1

void InitIT626X_Instance(void);
void HDMITX_ChangeDisplayOption(HDMI_Video_Type VideoMode, HDMI_OutputColorMode OutputColorMode);
void HDMITX_SetOutput(void);
void HDMITX_DevLoopProc(void);
BOOL VideoModeDetect(void);
void ConfigfHdmiVendorSpecificInfoFrame(BYTE _3D_Stru);
#endif // _CAT6611_SYS_H_
