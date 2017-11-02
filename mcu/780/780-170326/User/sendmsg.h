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


typedef unsigned char      U8;
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned int       BOOL;






#define HoriSplitMode 0
#define VertiSplitMode 1





#if 0
enum
{

	USART3_Test,
	UART4_Test

};
#endif

enum
{

	IS_USART3,
	IS_USART4

};


void ClearRevUart2Data(void);
void CopyRevUart2Data(U8 *buf);
	
//_SENDMSGDEF_ U8 g_RevUart2Data;

BOOL CheckProtocolStart(U8 * pBuf);
BOOL CheckSum(U8 * pBuf);



U8 DealFactoryReset(void);


void SetVolumeValue(U8 VolVaule);

void SetCurrentTime(void);
void LoadScenario(_KeypadNum loadnum);
void LoadScenarioUseCheck(void);

U8 Set_FramRate(void);
U8 Set_Resolution(void);
U8 Set_ScreenPara(void);
U8 ScenarioSwitch(U8 DviOrVga,  U8  ScenarioID );
U8 Take_ScenarioToDVI(void);
U8 ADD_New_Scenario(void);
U8 Volume_Set(U8 value);
U8 Image_Quality_Set(U8 Sub_ID,U8 value);



BOOL GetUsbState(void);

void SetOutputHzValue(void);

void testzoom(void);
void SetZoomValueWithSel(U8 para,U16 wValue,U8 MainOrSUb,U8 USART_Test);


void Write_EEPROM_WizardPara(void);
void Read_EEPROM_WizardPara(void);

void SetScalerTotalHVValue(U8 para,U8 MainOrSUb,U8 USART_Test);

 //U8 Reset23Buf[CMDLENTH]={0xAA,0xBB,0x02,0x01,0x53,0x05,0x07,0x80,0x04,0x38,0x00,0x1E};

void Uart2Send(U8 * pBuf);
void Uart3Send(U8 * pBuf);
void Uart4Send(U8 * pBuf);

void SendRGBValue(State currentState,U8 para2);


void SendRGBResetValue(void);


#endif


