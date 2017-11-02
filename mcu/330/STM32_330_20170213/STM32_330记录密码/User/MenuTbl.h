/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: MenuTbl.h
 [Date]:        08-May-2012
 [Comment]:
   MenuTbl header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _MENUTBL_H_
#define _MENUTBL_H_


extern struct rtc_time systmtime;

void TwoImageSetWizard(void);
void LCDDisplayTabInfo(void);
void WizardSet(void);
void LCDDisplayrHVValue(void);
State GetCurrentOuputResolution(void);
void ErrorFullZoom(void);
void FullZoomVideo(U8 MainOrSub);
void FullZoomVGA(U8 MainOrSub);
void FullZoomDVI(U8 MainOrSub);
void FullZoomHDMI(U8 MainOrSub);

void Deal_PIP_Pos(_PIPPos Pos);
void DealFullOrPartKey(void);
void DealMultiWindowKey(void);
void DealActiveImageKey(void);   //A/B key
void DealMenuKey(void);
void DealUpKey(void);
void DealDownKey(void);
void DealOkKey(void);
void DealEscKey(void);
void DealFreezeKey(void);
void DealVolumeKey(void);
void DisMathineIDValue(U32 wValue,U8 addr);

void DealSwapKey(void);
void DealUserModeKey(void);
void DealSaveKey(void);
void DealBackspaceKey(void);
void DealAudioKey(_AudioPara para);
void LEDWarning(_KeypadNum key);
BOOL DealStepKey(void);
void AdviceForUser(void);
void LCDDisplayWhiceImage(void);
void LCDDisplayFreezeState(void);
void LCDDealFreezeState(void);
void SetScreenFULL(void);
void SetScreenPART(void);
void GetWhichInput(U8 *MainOrSub);



#endif
