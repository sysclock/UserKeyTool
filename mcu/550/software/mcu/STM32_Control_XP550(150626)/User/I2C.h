#include "common.h"

#ifndef __I2C_H
#define __I2C_H

#ifdef _I2C_C_
  #define _I2CDEC_
#else
  #define _I2CDEC_ extern
#endif

#if(ENABLE_HARDI2C)
	void I2C_Configuration(void);
#else
	void I2CStart(void); 
	void I2CStop(void); 
	unsigned char I2CSlaveAck( void );
	U8 I2CReadByte(void);
	void I2CWriteByte(U8 Data);
#endif


void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no);
void I2C_Standby_24C(void);
void I2C_ByteWrite_24C(u16 addr,u8 dat);
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no);
void I2C_WriteS_24C(u16 addr,u8* pBuffer,  u16 no);
//void I2C_Test(void);

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


typedef enum{
	OUT_RESOLUTION_IDX,
	OUT_HZ,
	SWITCH_MODE,
	POINT_NEXT_IMAGE,
	MUTI_SINGLE,
	MAIN_INPUT_SOURCE,
	SUB_INPUT_SOURCE,
	BRIGHTNESS_IDX,
	CONTRAST_IDX,
	SATURATION_IDX,
//	OFFSET_R_IDX,
//	OFFSET_G_IDX,
//	OFFSET_B_IDX,
//	GAIN_R_IDX,
//	GAIN_G_IDX,
//	GAIN_B_IDX,
	CHECK_SUM_U8,
	EEPROMU8_SIZE                     // 4
}EEPROMU8Index;

typedef enum{
	UserResolution_HSize,
	UserResolution_VSize,
	
	SCREEN_HSIZE,    //0
	SCREEN_VSIZE,
	SCREEN_HPOS,
	SCREEN_VPOS,
	
	MAIN_SCALER_HSIZE,           // 4
	MAIN_SCALER_VSIZE,
	MAIN_SCALER_HPOS,
	MAIN_SCALER_VPOS,
	
	SUB_SCALER_HSIZE,
	SUB_SCALER_VSIZE,              //9
	SUB_SCALER_HPOS,
	SUB_SCALER_VPOS,
	
	MAIN_VGAZOOM_HPOS,
	MAIN_VGAZOOM_VPOS,
	MAIN_DVIZOOM_HPOS,                  //14
	MAIN_DVIZOOM_VPOS,
	MAIN_HDMIZOOM_HPOS,                 
	MAIN_HDMIZOOM_VPOS,
	MAIN_OTHERZOOM_HPOS,
	MAIN_OTHERZOOM_VPOS,
	
	SUB_VGAZOOM_HPOS,   // 20
	SUB_VGAZOOM_VPOS,                  
	SUB_DVIZOOM_HPOS,
	SUB_DVIZOOM_VPOS,
	SUB_HDMIZOOM_HPOS,
	SUB_HDMIZOOM_VPOS,
	SUB_OTHERZOOM_HPOS,
	SUB_OTHERZOOM_VPOS,

	MAIN_VGAZOOM_HRATIO,
	MAIN_VGAZOOM_VRATIO,                     //29
	MAIN_DVIZOOM_HRATIO,
	MAIN_DVIZOOM_VRATIO,
	MAIN_HDMIZOOM_HRATIO,
	MAIN_HDMIZOOM_VRATIO,
	MAIN_OTHERZOOM_HRATIO,
	MAIN_OTHERZOOM_VRATIO,          //35
	
	SUB_VGAZOOM_HRATIO,                     
	SUB_VGAZOOM_VRATIO,
	SUB_DVIZOOM_HRATIO,
	SUB_DVIZOOM_VRATIO,
	SUB_HDMIZOOM_HRATIO,
	SUB_HDMIZOOM_VRATIO,
	SUB_OTHERZOOM_HRATIO,
	SUB_OTHERZOOM_VRATIO,                 //43

	CHECK_SUM_U16,                     
	
	EEPROMU16_SIZE                  //45
}EEPROMU16Index;

typedef enum _UserTryEndTimeType{
	TRY_END_YEAR_LOW,
	TRY_END_YEAR_HIGH,
	TRY_END_MON,
	TRY_END_DAY,
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
	E2ROM_INPUT_SCHEDULE_ADDR=10*(EEPROMU8_SIZE+EEPROMU16_SIZE*2)+E2ROM_SAVEMODE_BEGIN_ADDR, //950+90
	E2ROM_TRYTIME_END_ADDR=E2ROM_INPUT_SCHEDULE_ADDR+3*INPUT_SIZE,  //965+90
	//E2ROM_OUTPUT_FORMAT_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE, //969+90
	//E2ROM_SCREEN_H_SIZE=E2ROM_OUTPUT_FORMAT_ADDR+2,  //971+90
	//E2ROM_SCREEN_V_SIZE=E2ROM_SCREEN_H_SIZE+5,  //976+90
	//E2ROM_SCREEN_H_POS=E2ROM_SCREEN_V_SIZE+5,  //981+90
	//E2ROM_SCREEN_V_POS=E2ROM_SCREEN_H_POS+5,  //986+90
	E2ROM_FadeTime_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE, //E2ROM_SCREEN_V_POS+5,    //991+90
	E2ROM_CycleFlag_ADDR=E2ROM_FadeTime_ADDR+2,    
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
	//E2ROM_UserResolution_ADDR=E2ROM_HotBackupInput_ADDR+17,  
	E2ROM_Alpha_ADDR=E2ROM_HotBackupInput_ADDR+17,  //E2ROM_UserResolution_ADDR+5,          	
	E2ROM_BOTH_SWITCH_ADDR=E2ROM_Alpha_ADDR+2, 
	//E2ROM_OutputHZ_ADDR=E2ROM_BOTH_SWITCH_ADDR+2, 
	E2ROM_UserPWD_ADDR=E2ROM_BOTH_SWITCH_ADDR+2, //E2ROM_OutputHZ_ADDR+2, 
	E2ROM_TelephoneNum_ADDR=E2ROM_UserPWD_ADDR+5, 
	E2ROM_Telephone_ADDR=E2ROM_TelephoneNum_ADDR+2, 
	E2ROM_Tech_TelephoneNum_ADDR=E2ROM_Telephone_ADDR+13, 
	E2ROM_Tech_Telephone_ADDR=E2ROM_Tech_TelephoneNum_ADDR+2, 
	E2ROM_PackagingName_ADDR=E2ROM_Tech_Telephone_ADDR+13,  
	E2ROM_TestName_ADDR=E2ROM_PackagingName_ADDR+6,  
	E2ROM_SaleName_ADDR=E2ROM_TestName_ADDR+6, 
	E2ROM_MathineType_Num_ADDR=E2ROM_SaleName_ADDR+6, 
	E2ROM_MathineType_ADDR=E2ROM_MathineType_Num_ADDR+2, 
	E2ROM_ADC_CABLIBRATION_ADDR=E2ROM_MathineType_ADDR+22, 
	E2ROM_ADV7441A_INPUT_ADDR=E2ROM_ADC_CABLIBRATION_ADDR+2, 
	E2ROM_VOLUME_ADDR=E2ROM_ADV7441A_INPUT_ADDR+2, 
	E2ROM_Last_ADDR=E2ROM_VOLUME_ADDR+2, 
		
}EEPROMAddressType;

_I2CDEC_ U8 EEPROMMemRegU8[EEPROMU8_SIZE];
_I2CDEC_ U16 EEPROMMemRegU16[EEPROMU16_SIZE];
_I2CDEC_ U8 genericEEPROMMemU8[EEPROMU8_SIZE];
_I2CDEC_ 	U16 genericEEPROMMemU16[EEPROMU16_SIZE];
extern U8 InputEE[INPUT_SIZE];
BOOL Read_Eeprom_UserResolution(int addr);
void Write_Eeprom_UserResolution(int addr);
void Write_EEPROM_HotBackupInput(int addr);
void Read_EEPROM_HotBackupInput(int addr);
void Write_EEPROM_InputCycle(int addr,_InputSourceType Input,U8 Hour,U8 Min);
//void Write_EEPROM_ScreenScaler(int addr,U16 Screen,U16 Scaler);
//void Read_EEPROM_ScreenScaler(void);
BOOL Save(U8 Mode);
BOOL Updata_Execute(U8 *U8EEPROMMem,U16 *U16EEPROMMem);
BOOL reset(void);
void LoadOutputResolution(void);
BOOL Read_EEPROM_Telephone(int addr,U8 *buf,U8 num);
void Write_EEPROM_UserPWD(int addr);
BOOL Read_EEPROM_UserPWD(int addr);
void I2CMasterAck(void);
BOOL IsHasSave(U8 Mode);
void SaveInput1Schedule(void);
void SaveInput2Schedule(void);
void SaveInput3Schedule(void);
U8 ByteToRepeatWeek(U8 OneB,U8 Mode);
BOOL Read_EEPROM_TryEndTime(void);
void Write_EEPROM_TryEndDate(int addr,U16 *ch);
BOOL Read_EEPROM_Byte(int addr,U8 *ch);
void Read_EEPROM_InputCycle(int addr);
U8 CheckDualSource(U8 InputSource1, U8 InputSource2);

BOOL IsInputScheHasSave(U8 Mode);
void FromEEInitInputSche(U8 Mode);
void Write_EEPROM_Byte(int addr,U8 ch);
BOOL Read_EEPROM_Language(void);
void SaveInputScheduleToEEPROM(U8 Mode);
unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no);
unsigned char  I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat);
void Write_EEPROM_U16(int addr,U16 value);
void Read_EEPROM_U16(int addr,U16 *ch);
void Read_EEPROM_MathineType(void);
void Write_EEPROM_PcPart(void);
BOOL Read_EEPROM_PcPart(void);
#endif
