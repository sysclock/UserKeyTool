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

void TwoImageSetWizard(void);
void Deal_PIP_Pos(_PIPPos Pos);
void DisMathineIDValue(U32 wValue,U8 addr);
State GetCurrentOuputResolution(void);
void LCDDisplayrHVValue(void);
void LCDDisplayTabInfo(void);
void AdviceForUser(void);
void LCDDisplayWhiceImage(void);
void LCDDisplayFreezeState(void);
void LCDDealFreezeState(void);
void WizardSet(void);
void DealNoUseKey(void);
void ErrorFullZoom(void);
void FullZoomScreen(U8 MainOrSub,U8 *FullOrPartValue);
void FullZoomVGA(U8 MainOrSub);
void FullZoomDVI(U8 MainOrSub);
void DealFullOrPartKey(void);
void DealMultiWindowKey(void);
void DealActiveImageKey(void);   //A/B key
void DealMenuKey(void);
void DealUpKey(void);
void DealDownKey(void);
void DealOkKey(void);
void DealEscKey(void);
void DealMuteKey(_AudioPara para);
void DealFreezeKey(void);
BOOL DealStepKey(void);
void DealVolumeKey(void);
void DealTakeKey(void);
void DealRecallKey(void);
void DealSaveKey(void);
void DealBackspaceKey(void);
void DealAudioKey(_AudioPara para);
void LEDWarning(_KeypadNum key);
void SetScreenFULL(U8 image);
void SetScreenPART(U8 image);
void GetWhichInput(U8 *MainOrSub);
#endif
