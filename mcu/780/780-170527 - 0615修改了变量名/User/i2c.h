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

//	DVI_CURRENT_SCENE_IDX,
//	VGA_CURRENT_SCENE_IDX,
//	BRIGHTNESS_IDX,
//	CONTRAST_IDX,
//	SATURATION_IDX,
//	
//	DVI_RESOLUTION_IDX,
//	DVI_FRAMERATE,
//	VGA_RESOLUTON_IDX,
//	VGA_FRAMERATE,
	
	CHECK_SUM_U8,
	EEPROMU8_SIZE                     // 4
}EEPROMU8Index;

typedef enum{

//	SCREEN_TOTAL_HSIZE_IDX,
//	SCREEN_TOTAL_VSIZE_IDX,
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

	E2ROM_TRYTIME_END_ADDR=E2ROM_SAVEMODE_BEGIN_ADDR+3*INPUT_SIZE,
//	E2ROM_OUTPUT_FORMAT_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE, 

	E2ROM_DeviceID_Addr=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE, 
	E2ROM_EnableHotBackupFlag_ADDR=E2ROM_DeviceID_Addr+2,
	E2ROM_HotBackupIdx_ADDR=E2ROM_EnableHotBackupFlag_ADDR+2,    
	E2ROM_HotBackupInput_ADDR=E2ROM_HotBackupIdx_ADDR+2,
	E2ROM_UserResolution_ADDR=E2ROM_HotBackupInput_ADDR+17,  

	E2ROM_UserPWD_ADDR=E2ROM_UserResolution_ADDR+2, 
	E2ROM_TelephoneNum_ADDR=E2ROM_DeviceID_Addr+5,   //读取电话长度 1个字节+1字节校验
	E2ROM_Telephone_ADDR=E2ROM_TelephoneNum_ADDR+2,  //读取电话号码 12个字节+1字节校验
	
	E2ROM_PassWordNum_ADDR=E2ROM_Telephone_ADDR+13,   //读取密码长度 1个字节+1字节校验
	E2ROM_PassWord_ADDR=E2ROM_PassWordNum_ADDR+2,  //读取密码6个字节+1字节校验
	
	E2ROM_LogoID_ADDR=E2ROM_PassWord_ADDR+7, 
	E2ROM_Tech_TelephoneNum_ADDR=E2ROM_LogoID_ADDR+3, 
	E2ROM_Tech_Telephone_ADDR=E2ROM_Tech_TelephoneNum_ADDR+2, 
	E2ROM_PackagingName_ADDR=E2ROM_Tech_Telephone_ADDR+13,  
	E2ROM_TestName_ADDR=E2ROM_PackagingName_ADDR+6,  
	E2ROM_SaleName_ADDR=E2ROM_TestName_ADDR+6, 
	E2ROM_MathineType_Num_ADDR=E2ROM_SaleName_ADDR+6, 
	E2ROM_MathineType_ADDR=E2ROM_MathineType_Num_ADDR+2, 
	E2ROM_FirstStart_ADDR=E2ROM_MathineType_ADDR+11,

	
	E2ROM_EEPROMU8_BEGIN=E2ROM_FirstStart_ADDR+2,
	
	E2ROM_CURRENT_SCENE_ADDR=E2ROM_FirstStart_ADDR+2,
	E2ROM_BRIGHTNESS_ADDR=E2ROM_CURRENT_SCENE_ADDR+2,
	E2ROM_CONTRAST_ADDR=E2ROM_BRIGHTNESS_ADDR+2,
	E2ROM_SATURATION_ADDR=E2ROM_CONTRAST_ADDR+2,
	E2ROM_VOLUME_ADDR=E2ROM_SATURATION_ADDR+2,
	
	E2ROM_DVI_RESOLUTION_ADDR=E2ROM_VOLUME_ADDR+2,
	E2ROM_DVI_FRAMERATE_ADDR=E2ROM_DVI_RESOLUTION_ADDR+2,
	E2ROM_VGA_RESOLUTON_ADDR=E2ROM_DVI_FRAMERATE_ADDR+2,
	E2ROM_VGA_FRAMERATE_ADDR=E2ROM_VGA_RESOLUTON_ADDR+2,
	
	E2ROM_SCREEN_TOTAL_HSIZE_ADDR=E2ROM_VGA_FRAMERATE_ADDR+2,
	E2ROM_SCREEN_TOTAL_VSIZE_ADDR=E2ROM_SCREEN_TOTAL_HSIZE_ADDR+3,
	
	E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR=E2ROM_SCREEN_TOTAL_VSIZE_ADDR+3,
	E2ROM_SCREEN_LINE_ADDR=E2ROM_SCENARIO_LINE_COL_PIXEL_ADDR+48,
	E2ROM_SCREEN_COL_ADDR=E2ROM_SCREEN_LINE_ADDR+2,
	E2ROM_SCHEME_SAVE_ADDR=E2ROM_SCREEN_COL_ADDR+2,
	
	E2ROM_SHOW_ONOFF_ADDR=E2ROM_SCHEME_SAVE_ADDR+18,    //演示开关存储地址
	E2ROM_SHOW_TIMELONG_ADDR=E2ROM_SHOW_ONOFF_ADDR+2,  //设置演示时长
	E2ROM_ONCE_ONOFF_ADDR=E2ROM_SHOW_TIMELONG_ADDR+2,   //单次授权开关存储地址
	E2ROM_ONCE_TIMELONG_ADDR=E2ROM_ONCE_ONOFF_ADDR+2,   //单次授权使用时间
	E2ROM_ONCE_END_ADDR=E2ROM_ONCE_TIMELONG_ADDR+2,    //通过授权使用结束时间标志
	E2ROM_ONCE_TelephoneNum_ADDR=E2ROM_ONCE_END_ADDR+5,  //存储授权电话的位数，有多少位数据构成
	E2ROM_ONCE_Telephone_ADDR=E2ROM_ONCE_TelephoneNum_ADDR+2, //存储授权电话号码，最多12位


	E2ROM_ONCE_MANAGE_PASSWORD_ADDR=E2ROM_ONCE_Telephone_ADDR+13,  //存储管理密码
	E2ROM_ONCE_LAST_PASSWORD_ADD=E2ROM_ONCE_MANAGE_PASSWORD_ADDR+5,
	
	
	E2ROM_Last_ADDR=E2ROM_ONCE_LAST_PASSWORD_ADD+5,
	
	
}EEPROMAddressType;

_I2CDEC_ U8 EEPROMMemRegU8[EEPROMU8_SIZE];
_I2CDEC_ U16 EEPROMMemRegU16[EEPROMU16_SIZE];
_I2CDEC_ U8 genericEEPROMMemU8[EEPROMU8_SIZE];
_I2CDEC_ 	U16 genericEEPROMMemU16[EEPROMU16_SIZE];
extern U8 InputEE[INPUT_SIZE];

void Write_EEPROM_InputCycle(int addr,_InputSourceType Input,U8 Hour,U8 Min);
void Write_EEPROM_ScreenScaler(int addr,U16 Screen,U16 Scaler);
void Read_EEPROM_ScreenScaler(void);
BOOL Save(U8 Mode);
BOOL Updata_Execute();
BOOL reset(void);
//void LoadOutputResolution(void);
BOOL Read_EEPROM_Telephone(int addr,U8 *buf,U8 num);
void Write_EEPROM_UserPWD(int addr);
BOOL Read_EEPROM_UserPWD(int addr);
void I2CMasterAck(void);

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

void Read_Screen_line_column_pixel(void);
void Write_Screen_line_column_pixel(void);

void Read_Scheme_IsSave(void);
void Write_Scheme_IsSave(void);

void GetMachineInfo(void);


#endif
