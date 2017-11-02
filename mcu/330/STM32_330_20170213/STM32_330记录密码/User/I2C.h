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
#if(STM32_CONTROL_MCU==MSTAR_6M16GS)
	MAIN_BRIGHTNESS_R,
	MAIN_BRIGHTNESS_G,
	MAIN_BRIGHTNESS_B,
	MAIN_CONTRACT_R,
	MAIN_CONTRACT_G,                        //20
	MAIN_CONTRACT_B,
	
	SUB_BRIGHTNESS_R,
	SUB_BRIGHTNESS_G,
	SUB_BRIGHTNESS_B,
	SUB_CONTRACT_R,                       //25
	SUB_CONTRACT_G,
	SUB_CONTRACT_B,
#endif
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
#if(XP_TO_ID==XP_TO_FLI32626)
	E2ROM_OUTPUT_FORMAT_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE, //969+90
	E2ROM_SCREEN_H_SIZE=E2ROM_OUTPUT_FORMAT_ADDR+2,  //971+90
	E2ROM_SCREEN_V_SIZE=E2ROM_SCREEN_H_SIZE+5,  //976+90
	E2ROM_SCREEN_H_POS=E2ROM_SCREEN_V_SIZE+5,  //981+90
	E2ROM_SCREEN_V_POS=E2ROM_SCREEN_H_POS+5,  //986+90
	E2ROM_FadeTime_ADDR=E2ROM_SCREEN_V_POS+5,    //991+90
	E2ROM_UserResolution_ADDR=E2ROM_FadeTime_ADDR+2,  
	E2ROM_NEXT_ADDR=E2ROM_UserResolution_ADDR+5,  
#else
	E2ROM_NEXT_ADDR=E2ROM_TRYTIME_END_ADDR+TRY_END_SIZE,
#endif
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
	E2ROM_AutoAdjustVGA_ADDR=E2ROM_PowerOnPwd_Flag_ADDR+1,
	//此处添加
	E2ROM_Log_Num_ADDR=E2ROM_AutoAdjustVGA_ADDR+2, 
	
	E2ROM_EnableRecordLog_ADDR=E2ROM_Log_Num_ADDR+3,
	
	
	//20170213
	E2ROM_SHOW_ONOFF_ADDR=E2ROM_EnableRecordLog_ADDR+2,    //演示开关存储地址
	E2ROM_SHOW_TIMELONG_ADDR=E2ROM_SHOW_ONOFF_ADDR+2,  //设置演示时长
	E2ROM_ONCE_ONOFF_ADDR=E2ROM_SHOW_TIMELONG_ADDR+2,   //单次授权开关存储地址
	E2ROM_ONCE_TIMELONG_ADDR=E2ROM_ONCE_ONOFF_ADDR+2,   //单次授权使用时间
	E2ROM_ONCE_END_ADDR=E2ROM_ONCE_TIMELONG_ADDR+2,    //通过授权使用结束时间标志
	E2ROM_ONCE_TelephoneNum_ADDR=E2ROM_ONCE_END_ADDR+5,  //存储授权电话的位数，有多少位数据构成
	E2ROM_ONCE_Telephone_ADDR=E2ROM_ONCE_TelephoneNum_ADDR+2, //存储授权电话号码，最多12位
	
	E2ROM_ONCE_MANAGE_PASSWORD_ADDR=E2ROM_ONCE_Telephone_ADDR+13,  //存储管理密码
	
	E2ROM_ONCE_LAST_PASSWORD_ADD=E2ROM_ONCE_MANAGE_PASSWORD_ADDR+5,
	
	E2ROM_Log_Start_ADDR=E2ROM_ONCE_LAST_PASSWORD_ADD+9,
	
	//E2ROM_Last_ADDR=E2ROM_Log_Start_ADDR+2,
	
	//asd=E2ROM_Log_Start_ADDR+2, 
		
}EEPROMAddressType;

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

void I2C_EraserS_24C(u16 addr, u16 no);
#endif


