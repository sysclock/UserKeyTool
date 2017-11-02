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

//void DisMathineIDValue(U32 wValue,U8 addr);
void LCDDisplayrHVValue(void);
//void AdviceForUser(void);
void LCDDisplayWhiceImage(void);
void LCDDisplayFreezeState(void);

//void WizardSet(void);
void DealNoUseKey(void);
void ErrorFullZoom(void);






void DealMultiWindowKey(void);
void DealActiveImageKey(void);   //A/B key
void DealMenuKey(void);
void DealUpKey(void);
void DealDownKey(void);
void DealOkKey(void);
void DealEscKey(void);
void DealRecallKey(void);
void DealSaveKey(void);
void DealBackspaceKey(void);
void DealAudioKey(_AudioPara para);
void LEDWarning(_KeypadNum key);

void GetWhichInput(U8 *MainOrSub);


#endif



