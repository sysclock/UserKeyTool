/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: SendMsg.h
 [Date]:        10-May-2012
 [Comment]:
   SendMsg header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _SENDMSG_H_
#define _SENDMSG_H_


#ifdef _SENDMSG_C_
  #define _SENDMSGDEF_
#else
  #define _SENDMSGDEF_ extern
#endif

void ClearRevUart2Data(void);
void CopyRevUart2Data(U8 *buf);

void ClearRevUart3Data(void);
void CopyRevUart3Data(U8 *buf);	

//_SENDMSGDEF_ U8 g_RevUart2Data;

BOOL CheckProtocolStart(U8 * pBuf);
BOOL CheckSum(U8 * pBuf);
void CheckSum_Send(U8 * pBuf);
//void Load2048X1152ScreenPara(void);
void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub);
void SetFreezeImage(U8 MainOrSUb,U8 para);

void SetMultiWindowPara(U8 para,U8 alpha);
void SwapDualInputSrc(void);
//void SetZoomValue(U8 para,U16 wValue,U8 MainOrSUb);
void SetZoomValueHV(U8 para,U16 wValue1,U16 wValue2,U8 MainOrSUb);
void SetZoomHVValue(U8 para,U8 MainOrSUb);
void SetFullZoomHVValue(U8 para,U8 MainOrSUb);
void SendColorValue(U8 Event_ID,U8 para1,U8 para2);
void SendAdcCalibration(U8 Event_ID,U8 para1,U8 para2,U8 para3);
//void SetScalerValue(U8 para,U16 wValue,U16 wVaule2,U8 MainOrSUb);
void SetScalerHVValue(U8 para,U8 MainOrSUb);
void SetPanelScalerHVValue(U8 para,U8 MainOrSUb);

void SetOutputFormat(U8 nIndex);
void SetWhichImageOnTop(U8 MainOrSUb);
void SetAudioPara(U8 para);
void SetVolumeValue(U8 VolVaule);

void SetCurrentTime(void);
void DealKey2Usb(_KeypadNum keyVale);
void LoadModeSet(void);
BOOL GetInputSrcFormat(U8 MainOrSub,U16 *hSize,U16 *vSize);
void AutoAdjustVGA(void);
void SetOutputHzValue(void);
#endif
