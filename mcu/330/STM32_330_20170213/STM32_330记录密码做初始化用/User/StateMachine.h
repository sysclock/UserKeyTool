/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: statemachine.h
 [Date]:        11-May-2012
 [Comment]:
   StateMachine header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "header.h"

#ifdef _STATEMACHINE_C_
  #define _STATEMACHINEDEC_
#else
  #define _STATEMACHINEDEC_ extern
#endif

BOOL ResetPara(void);
void SetSingleImage(void);
void SetPIPImage(void);
void SetPBPImage(void);
BOOL SetCurrentStateValue(EventValue enVal,State state,U16 wValue);
State GetOutFormatStateValue(void);
State GetFullOrPartStateValue(void);
State GetMultiWindowStateValue(void);
State GetActiveImageStateValue(void);
State GetActiveAudioStateValue(void);
U16 GetCurrentWindowValue(void);
U16 GetCurrentVolumeValue(void);
//State GetAVOrYPbPr(void);
State GetScheduleMode(void);
//------------------------get current schedule mode-------------------------------
State GetScheduleInputMode1(void);
State GetScheduleInputMode2(void);
State GetScheduleInputMode3(void);
//------------------------get current schedule mode-------------------------------

#if(XP_TO_ID==XP_TO_FLI32626)
State GetFadeTimeStateValue(void);

#endif
State GetCurrentOutputHzState(void);
#endif
