#ifndef _COLORDISPLAY_H_
#define _COLORDISPLAY_H_


/**************************************************************************/


extern int Date_Time[8];  //用来存储设置时间时的临时日期时间数据   8用来存储当前设置的位置

void lcdselection(State CurState);

void DisBackGround(State CurState);

void ColorDisplayCurrentScenario(void);




/**************************************************************************/

void ColorDisplay_Num( U8 *Num);
void ColorDisplay_Char(U8 *Num,U8 Number);
void Color_VersionInfo(void);

void Color_OperationGuide(void);
void Color_OperationGuide1(void);
void Display_Wait(void);
void Display_SaveFail(void);
void Display_Saveok(void);
void Color_ColorValue(U16 wValue,State CurState);
void ColorDisplayCurTime(void);

#endif



