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

#if(XP_TO_ID==XP_TO_FLI32626)
void ClearRevUart2Data(void);
void CopyRevUart2Data(U8 *buf);

BOOL CheckProtocolStart(U8 * pBuf);
BOOL CheckSum(U8 * pBuf);
void CheckSum_Send(U8 * pBuf);
void Load2048X1152ScreenPara(void);
void SwapDualInputSrc(void);
void SetZoomHVValue(U8 para,U8 MainOrSUb);
void SetFullZoomHVValue(U8 para,U8 MainOrSUb);
void SendColorValue(U8 Event_ID,U8 para1,U8 para2);
void SendAdcCalibration(U8 Event_ID,U8 para1,U8 para2,U8 para3);
void SetScalerValue(U8 para,U16 wValue,U16 wVaule2,U8 MainOrSUb);
void SetScalerHVValue(U8 para,U8 MainOrSUb);
void SetPanelScalerHVValue(U8 para,U8 MainOrSUb);

void SetWhichImageOnTop(U8 MainOrSUb);
void SetCurrentTime(void);
#elif(XP_TO_ID==XP_TO_6M16)
BOOL WaitOneUartACK(U32 num);
BOOL WaitTwoUartACK(U32 num);
BOOL WaitFourUartACK(U32 num);
//void CopyToRevUart2(U8 *pBuf);

void SwitchInputAudio(_InputSourceType input);
void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub);
U8 GetInputSource(U8 AOrB);
U8 GetMultiWindowPara(void);
U16 GetZoomValue(U8 para,U8 MainOrSUb);
void SetScreenValue(U8 para,U16 wValue);
BOOL GetScreenValue(U8 para);

void SetScalerValue(U8 para,U16 wValue,U8 MainOrSUb);
BOOL GetScalerValue(U8 para,U8 MainOrSUb);

void SetColorValue(U8 para,U8 wValue,U8 MainOrSUb);
U8 GetColorValue(U8 para,U8 MainOrSUb);

U8 GetOutputFormat(void);

U16 GetMainInputSrcFormat(U8 para);
U16 GetSubInputSrcFormat(U8 para);

void SetVGAZoomValue(U8 para,U16 wValue,U8 MainOrSUb);
void SetDVIZoomValue(U8 para,U16 wValue,U8 MainOrSUb);
void SetHDMIZoomValue(U8 para,U16 wValue,U8 MainOrSUb);
#endif
void SetAudioPara(U8 para);
void SetVolumeValue(U8 VolVaule);
void SetInputSource(_InputSourceType inputsrc,U8 MainOrSub);
BOOL GetInputSrcFormat(U8 MainOrSub,U16 *hSize,U16 *vSize);
void SetMultiWindowPara(U8 para);
void SetZoomValue(U8 para,U16 wValue,U8 MainOrSUb);
void SetFreezeImage(U8 MainOrSUb,U8 para);
void SetOutputFormat(U8 nIndex);
void LoadModeSet(void);
void AutoAdjustVGA(void);
void SetOutputHzValue(void);
#endif
