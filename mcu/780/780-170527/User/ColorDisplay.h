#ifndef _COLORDISPLAY_H_
#define _COLORDISPLAY_H_


/**************************************************************************/


extern int Date_Time[8];  //�����洢����ʱ��ʱ����ʱ����ʱ������   8�����洢��ǰ���õ�λ��

void LcdDisp_Follow_Keyboard(State CurState);

void LcdDisp_BackGround_Graph(State CurState);

void lcd_disp_curr_mode_no(void);




/**************************************************************************/

void LcdDisp_Num( U8 *Num);
void LcdDisp_Char(U8 *Num,U8 Number);
void Color_VersionInfo(void);

void Color_OperationGuide(void);
void Color_OperationGuide1(void);
void Display_Wait(void);
void Display_SaveFail(void);
void Display_Saveok(void);
void Color_ColorValue(U16 wValue,State CurState);
void LCD_DispCurrTime(void);

#endif



