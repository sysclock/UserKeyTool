#ifndef _COLORDISPLAY_H_
#define _COLORDISPLAY_H_


/**************************************************************************/


extern int Date_Time[8];  //�����洢����ʱ��ʱ����ʱ����ʱ������   8�����洢��ǰ���õ�λ��

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



