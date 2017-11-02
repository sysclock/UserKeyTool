/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: I2C.h
 [Date]:        08-May-2012
 [Comment]:
 I2C header file.
 [Reversion History]:
*******************************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

#ifdef _I2C_C_
  #define _I2CDEC_
#else
  #define _I2CDEC_ extern
#endif

#include "header.h"

#if(ENABLE_HARDI2C)
void I2C_Configuration(void);
#else
void I2CMasterAck(void);
void I2CStart(void); 
void I2CStop(void); 
unsigned char I2CSlaveAck( void );
U8 I2CReadByte(void);
void I2CWriteByte(U8 Data);
#endif
//void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no);
void I2C_Standby_24C(void);
void I2C_ByteWrite_24C(u16 addr,u8 dat);
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no);
//void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no);

void m24xx_write(
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
    U8 * buf,         //-- Data src buf
    int num);           //-- Bytes to write qty

BOOL m24xx_read(
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  U8 * buf,         //-- Data dst buf
  int num);           //-- Bytes to read qty


void m24xx_writeU16(
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
    U16 * buf,         //-- Data src buf
    int num);           //-- Bytes to write qty

BOOL m24xx_readU16(
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  U16 * buf,         //-- Data dst buf
  int num);           //-- Bytes to read qty
        //-- Bytes to read qty


typedef enum{
	OUTPUT_FORMAT,                 //0
	MUTI_SINGLE,
	MAIN_INPUT_SOURCE,
	SUB_INPUT_SOURCE,
	BRIGHTNESS_IDX,
	CONTRAST_IDX,
	SATURATION_IDX,
	CHECK_SUM_U8,
	EEPROMU8_SIZE                     //29
}EEPROMU8Index;

typedef enum{
//#if(XP_TO_ID==XP_TO_6M16)
	SCREEN_HSIZE,    // 1
	SCREEN_VSIZE,
	SCREEN_HPOS,
	SCREEN_VPOS,
	
	MAIN_SCALER_HSIZE,           //5
	MAIN_SCALER_VSIZE,
	MAIN_SCALER_HPOS,
	MAIN_SCALER_VPOS,
//#endif	
	SUB_SCALER_HSIZE,
	SUB_SCALER_VSIZE,              //10
	SUB_SCALER_HPOS,
	SUB_SCALER_VPOS,
	
	MAIN_VGAZOOM_HPOS,
	MAIN_VGAZOOM_VPOS,
	MAIN_DVIZOOM_HPOS,                  //15
	MAIN_DVIZOOM_VPOS,
	MAIN_HDMIZOOM_HPOS,                  //15
	MAIN_HDMIZOOM_VPOS,
	MAIN_OTHERZOOM_HPOS,
	MAIN_OTHERZOOM_VPOS,
	
	SUB_VGAZOOM_HPOS,
	SUB_VGAZOOM_VPOS,                    //20
	SUB_DVIZOOM_HPOS,
	SUB_DVIZOOM_VPOS,
	SUB_HDMIZOOM_HPOS,
	SUB_HDMIZOOM_VPOS,
	SUB_OTHERZOOM_HPOS,
	SUB_OTHERZOOM_VPOS,

	MAIN_VGAZOOM_HRATIO,
	MAIN_VGAZOOM_VRATIO,                     //5
	MAIN_DVIZOOM_HRATIO,
	MAIN_DVIZOOM_VRATIO,
	MAIN_HDMIZOOM_HRATIO,
	MAIN_HDMIZOOM_VRATIO,
	MAIN_OTHERZOOM_HRATIO,
	MAIN_OTHERZOOM_VRATIO,
	
	SUB_VGAZOOM_HRATIO,                      //10
	SUB_VGAZOOM_VRATIO,
	SUB_DVIZOOM_HRATIO,
	SUB_DVIZOOM_VRATIO,
	SUB_HDMIZOOM_HRATIO,
	SUB_HDMIZOOM_VRATIO,
	SUB_OTHERZOOM_HRATIO,
	SUB_OTHERZOOM_VRATIO,                 //15

	CHECK_SUM_U16,                     //25
	
	EEPROMU16_SIZE
}EEPROMU16Index;

typedef enum _UserTryEndTimeType{
	TRY_END_YEAR_LOW,
	TRY_END_YEAR_HIGH,
	TRY_END_MON,
	TRY_END_DAY,
	TRY_CHECKSUM,
	TRY_END_SIZE
}UserTryEndTimeType;

typedef enum{
	INPUT_HOUR,
	INPUT_MIN,
	INPUT_SRC,
	INPUT_WEEK,
	CHECK_SUM_INPUT,
	INPUT_SIZE
}InputScheEEIndex;

typedef enum _EEPROMAddressType
{
	E2ROM_LOCKTIME_BEGIN_ADDR=6,
	E2ROM_LANGUAGE_BEGIN_ADDR=E2ROM_LOCKTIME_BEGIN_ADDR+2,
	E2ROM_SAVEMODE_BEGIN_ADDR =E2ROM_LANGUAGE_BEGIN_ADDR+2,
	E2ROM_INPUT_SCHEDULE_ADDR=10*(EEPROMU8_SIZE+EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR,
	E2ROM_TRYTIME_END_ADDR=E2ROM_INPUT_SCHEDULE_ADDR+3*INPUT_SIZE,
	E2ROM_NEXT_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE,
	E2ROM_CycleFlag_ADDR=E2ROM_NEXT_ADDR,
	E2ROM_CycleInput1_ADDR=E2ROM_CycleFlag_ADDR+2,    
	E2ROM_CycleInput2_ADDR=E2ROM_CycleInput1_ADDR+4,   
	E2ROM_CycleInput3_ADDR=E2ROM_CycleInput2_ADDR+4,    
	E2ROM_CycleInput4_ADDR=E2ROM_CycleInput3_ADDR+4,    
	E2ROM_CycleInput5_ADDR=E2ROM_CycleInput4_ADDR+4,    
	E2ROM_CycleInput6_ADDR=E2ROM_CycleInput5_ADDR+4,  
	E2ROM_IP_ADDR=E2ROM_CycleInput6_ADDR+4, 	
	E2ROM_FULL_PART_ADDR=E2ROM_IP_ADDR+5, 
	E2ROM_DeviceID_Addr=E2ROM_FULL_PART_ADDR+4, 
	E2ROM_EnableHotBackupFlag_ADDR=E2ROM_DeviceID_Addr+2,
	E2ROM_HotBackupIdx_ADDR=E2ROM_EnableHotBackupFlag_ADDR+2,    
	E2ROM_HotBackupInput_ADDR=E2ROM_HotBackupIdx_ADDR+2,
	E2ROM_Alpha_ADDR=E2ROM_HotBackupInput_ADDR+17,
	E2ROM_OutputHZ_ADDR=E2ROM_Alpha_ADDR+2, 
	E2ROM_UserPWD_ADDR=E2ROM_OutputHZ_ADDR+2, 
	E2ROM_TelephoneNum_ADDR=E2ROM_UserPWD_ADDR+5, 
	E2ROM_Telephone_ADDR=E2ROM_TelephoneNum_ADDR+2, 
	E2ROM_LogoID_ADDR=E2ROM_Telephone_ADDR+13, 
	E2ROM_Tech_TelephoneNum_ADDR=E2ROM_LogoID_ADDR+3, 
	E2ROM_Tech_Telephone_ADDR=E2ROM_Tech_TelephoneNum_ADDR+2, 
	E2ROM_PackagingName_ADDR=E2ROM_Tech_Telephone_ADDR+13,  
	E2ROM_TestName_ADDR=E2ROM_PackagingName_ADDR+6,  
	E2ROM_SaleName_ADDR=E2ROM_TestName_ADDR+6, 
	E2ROM_MathineType_Num_ADDR=E2ROM_SaleName_ADDR+6, 
	E2ROM_MathineType_ADDR=E2ROM_MathineType_Num_ADDR+2,         	
	E2ROM_BOTH_SWITCH_ADDR=E2ROM_MathineType_ADDR+11,
	E2ROM_HowLongTime_ADDR=E2ROM_BOTH_SWITCH_ADDR+2, 
	E2ROM_PowerOnPwd_Flag_ADDR=E2ROM_HowLongTime_ADDR+2, 
	//´Ë´¦Ìí¼Ó
	E2_ADDR_CardNumber = E2ROM_PowerOnPwd_Flag_ADDR+2,
	E2_ADDR_CardLayout = E2_ADDR_CardNumber+2,
	E2_ADDR_Card1HSize  =  E2_ADDR_CardLayout +     2, 
	E2_ADDR_Card1VSize  =  E2_ADDR_Card1HSize +    2  , 
	E2_ADDR_Card2HSize  =  E2_ADDR_Card1VSize +    2  , 
	E2_ADDR_Card2VSize  =  E2_ADDR_Card2HSize +      2, 
	E2_ADDR_Card3HSize  =  E2_ADDR_Card2VSize +      2, 
	E2_ADDR_Card3VSize  =  E2_ADDR_Card3HSize +      2,
	E2_ADDR_Card4HSize  =  E2_ADDR_Card3VSize +      2,
	E2_ADDR_Card4VSize  =  E2_ADDR_Card4HSize +      2,
	E2_ADDR_ImageNumber =  E2_ADDR_Card4VSize +    2  ,
	E2_ADDR_ImageLayout =  E2_ADDR_ImageNumber+   2  ,
	E2_ADDR_Image1Hsize =  E2_ADDR_ImageLayout+      2,
	E2_ADDR_Image1Vsize =  E2_ADDR_Image1Hsize+      2,
	E2_ADDR_Image1Hpos  =  E2_ADDR_Image1Vsize+      2,
	E2_ADDR_Image1Vpos  =  E2_ADDR_Image1Hpos +      2,
	E2_ADDR_Image2Hsize =  E2_ADDR_Image1Vpos +      2,
	E2_ADDR_Image2Vsize =  E2_ADDR_Image2Hsize+      2,
	E2_ADDR_Image2Hpos  =  E2_ADDR_Image2Vsize+      2,
	E2_ADDR_Image2Vpos  =  E2_ADDR_Image2Hpos +      2,
	E2_ADDR_Image3Hsize =  E2_ADDR_Image2Vpos +      2,
	E2_ADDR_Image3Vsize =  E2_ADDR_Image3Hsize+      2,
	E2_ADDR_Image3Hpos  =  E2_ADDR_Image3Vsize+      2,
	E2_ADDR_Image3Vpos  =  E2_ADDR_Image3Hpos +      2,
	E2_ADDR_Image4Hsize =  E2_ADDR_Image3Vpos +      2,
	E2_ADDR_Image4Vsize =  E2_ADDR_Image4Hsize+      2,
	E2_ADDR_Image4Hpos  =  E2_ADDR_Image4Vsize+      2,
	E2_ADDR_Image4Vpos  =  E2_ADDR_Image4Hpos +      2,        
	E2_ADDR_Image1Scr   =  E2_ADDR_Image4Vpos +      2,
	E2_ADDR_Image2Scr   =  E2_ADDR_Image1Scr  +      2,
	E2_ADDR_Image3Scr   =  E2_ADDR_Image2Scr  +      2,
	E2_ADDR_Image4Scr   =  E2_ADDR_Image3Scr  +      2,
	                         
	
	E2ROM_Log_Num_ADDR=   E2_ADDR_Image4Scr+2, 
	E2ROM_Log_Start_ADDR=E2ROM_Log_Num_ADDR+3,
	E2ROM_Last_ADDR=E2ROM_Log_Start_ADDR+2,
}EEPROMAddressType;



/*
	_KEYPADDEC_ U8				g_CardNumber;
	_KEYPADDEC_ U8				g_CardLayout      ;
	_KEYPADDEC_ U16				g_Card1HSize      ;
	_KEYPADDEC_ U16				g_Card1VSize      ;
	_KEYPADDEC_ U16				g_Card2HSize      ;
	_KEYPADDEC_ U16				g_Card2VSize      ;
	_KEYPADDEC_ U16				g_Card3HSize      ;
	_KEYPADDEC_ U16				g_Card3VSize      ;
	_KEYPADDEC_ U16				g_Card4HSize      ;
	_KEYPADDEC_ U16				g_Card4VSize      ;
	_KEYPADDEC_ U8				g_ImageNumber     ;
	_KEYPADDEC_ U8				g_ImageLayout     ;
	_KEYPADDEC_ U16				g_Image1Hsize     ;
	_KEYPADDEC_ U16				g_Image1Vsize     ;
	_KEYPADDEC_ U16				g_Image1Hpos      ;
	_KEYPADDEC_ U16				g_Image1Vpos      ;
	_KEYPADDEC_ U16				g_Image2Hsize     ;
	_KEYPADDEC_ U16				g_Image2Vsize     ;
	_KEYPADDEC_ U16				g_Image2Hpos      ;
	_KEYPADDEC_ U16				g_Image2Vpos      ;
	_KEYPADDEC_ U16				g_Image3Hsize     ;
	_KEYPADDEC_ U16				g_Image3Vsize     ;
	_KEYPADDEC_ U16				g_Image3Hpos      ;
	_KEYPADDEC_ U16				g_Image3Vpos      ;
	_KEYPADDEC_ U16				g_Image4Hsize     ;
	_KEYPADDEC_ U16				g_Image4Vsize     ;
	_KEYPADDEC_ U16				g_Image4Hpos      ;
	_KEYPADDEC_ U16				g_Image4Vpos      ;

	_KEYPADDEC_ _InputSourceType g_Image1Scr;
	_KEYPADDEC_ _InputSourceType g_Image2Scr;
	_KEYPADDEC_ _InputSourceType g_Image3Scr;
	_KEYPADDEC_ _InputSourceType g_Image4Scr;	

*/

_I2CDEC_ U8 EEPROMMemRegU8[EEPROMU8_SIZE];
_I2CDEC_ U16 EEPROMMemRegU16[EEPROMU16_SIZE];
_I2CDEC_ U8 genericEEPROMMemU8[EEPROMU8_SIZE];
_I2CDEC_ U16 genericEEPROMMemU16[EEPROMU16_SIZE];
extern U8 InputEE[INPUT_SIZE];
#if(XP_TO_ID==XP_TO_FLI32626)
BOOL Read_Eeprom_UserResolution(int addr);
void Write_Eeprom_UserResolution(int addr);
void Read_EEPROM_ScreenScaler(void);
void Write_EEPROM_ScreenScaler(int addr,U16 Screen,U16 Scaler);
#endif

#ifdef HOT_BACKUP
void Write_EEPROM_HotBackupInput(int addr);
void Read_EEPROM_HotBackupInput(int addr);
#endif
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no);
void I2C_ReadS_FPGA(u16 addr ,u8* pBuffer,u16 no);
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no);
void Write_EEPROM_PcPart(void);
BOOL Read_EEPROM_PcPart(void);
BOOL Save(U8 Mode);
BOOL Updata_Execute(U8 *U8EEPROMMem,U16 *U16EEPROMMem);
BOOL reset(void);
BOOL IsHasSave(U8 Mode);
void SaveInput1Schedule(void);
void SaveInput2Schedule(void);
void SaveInput3Schedule(void);
U8 ByteToRepeatWeek(U8 OneB,U8 Mode);
U8 RepeatWeekToByte(U8 mode,U8 *week);
void Write_EEPROM_TryEndDate(int addr,U16 *ch);
BOOL Read_EEPROM_TryEndTime(void);
//void Read_EEPROM_MathineType(void);
BOOL IsInputScheHasSave(U8 Mode);
void FromEEInitInputSche(U8 Mode);
void Write_EEPROM_Byte(int addr,U8 ch);
BOOL Read_EEPROM_Byte(int addr,U8 *ch);
void Write_EEPROM_InputCycle(int addr,_InputSourceType Input,U8 Hour,U8 Min);
void Read_EEPROM_InputCycle(int addr);

BOOL FindInputScheE2PROMAddr(int *addr,U8 Mode);
void SaveInputScheduleToEEPROM(U8 MODE);
void Write_EEPROM_UserPWD(int addr);
void LoadOutputResolution(void);
BOOL Read_EEPROM_Telephone(int addr,U8 *buf,U8 num);
BOOL Read_EEPROM_UserPWD(int addr);
void Write_EEPROM_U16(int addr,U16 value);
void Read_EEPROM_U16(int addr,U16 *ch);
#if 0
unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no);
unsigned char  I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat);
unsigned char  I2C_ByteWriteN_ToDevice(u8 eepaddr,u8 addr, u8 byteno, u8 *p_data);
#endif


BOOL CheckEEPRom(void);

void I2C_EraserS_24C(u16 addr, u16 no);
#endif


