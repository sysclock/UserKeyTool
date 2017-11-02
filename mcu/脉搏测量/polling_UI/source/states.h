#ifndef _STATES_H_
#define _STATES_H_

#include "key.h"
#include "stdint.h"



typedef enum{
	NAV_SET = 0, 
	NAV_UD,
	NAV_RES,
	NUM_OF_NAV_EVENTS
}NAV_DATA_ARRAY_INDEX;



typedef enum{
	State_Main_Menu,		//0     　主菜单
	State_Main_Menu_interact_error,
	State_Main_Menu_Manual,	//手动测量
	
	State_Manual_DeviceCheck,
	State_Manual_Usertouch,
	State_Manual_Doing,
	State_Manual_Measure_OK,
	State_Manual_Measure_error1,
	State_Manual_pw1101_interact_error,
	State_Manual_pw1101_AutoGoing_error,
	State_Manual_LowPower,
	State_Manual_TempHigh,
	State_Manual_TempLow,
	State_Manual_Measure_False,
	
	State_Main_Menu_Auto,//自动测量
	
	State_Auto_Measure_IsTurnON,
	State_Auto_Measure_TurnOFF,
	State_Auto_Measure_IsTurnOFF,
	State_Auto_Measure_TurnON,
	
	
	State_Auto_DeviceCheck,
	State_Auto_Usertouch,
	State_Auto_Doing,
	State_Auto_Measure_OK,
	State_Auto_Measure_error1,
	State_Auto_pw1101_interact_error,
	State_Auto_LowPower,
	State_Auto_TempHigh,
	State_Auto_TempLow,
	State_Auto_Measure_False,
	
	/////////////////////////
	State_Main_Menu_Record,	//历史记录
	State_Record_Display,
	////////////////////////////////
	
	State_Main_Menu_Set,//设置
	State_Set_error,
	
	State_Set_Menu_Data_Time,	 //20170304
			State_Set_Menu_Date,
					State_Set_Menu_Date_BackGround,
					State_Date_Set_Year_ADD,
					State_Date_Set_Year_SUB,
					State_Date_Set_Month_ADD,
					State_Date_Set_Month_SUB,
					State_Date_Set_Day_ADD,
					State_Date_Set_Day_SUB,
					State_Date_Set_OK,
			State_Set_Menu_Time,
					State_Set_Menu_Time_BackGround,
					State_Time_Hour_ADD,
					State_Time_Hour_SUB,
					State_Time_Minute_ADD,
					State_Time_Minute_SUB,
					State_Time_Set_OK,
			State_Set_Menu_GMT,
					State_Set_Menu_GMT_BackGround,
					State_GMT_ADD,
					State_GMT_SUB,
					State_GMT_OK,
	State_Set_Menu_HBlood,
			State_Set_Menu_HBlood_BackGround,
			State_HBlood_Set_Low_ADD,
			State_HBlood_Set_Low_SUB,
			State_HBlood_Set_High_ADD,
			State_HBlood_Set_High_SUB,
			State_HBlood_Set_OK,
	State_Set_Menu_LBlood,
			State_Set_Menu_LBlood_BackGround,
			State_LBlood_Set_Low_ADD,
			State_LBlood_Set_Low_SUB,
			State_LBlood_Set_High_ADD,
			State_LBlood_Set_High_SUB,
			State_LBlood_Set_OK,
	State_Set_Menu_HeartRate,
			State_Set_Menu_HeartRate_BackGround,
			State_HeartRate_Set_Low_ADD,
			State_HeartRate_Set_Low_SUB,
			State_HeartRate_Set_High_ADD,
			State_HeartRate_Set_High_SUB,
			State_HeartRate_Set_OK,
	State_Set_Menu_kPa,
		State_Select_mmHgkPa,
	State_Set_Menu_MeasureMode,
		State_Select_MeasureMode,
		
	
	State_Set_Menu_Measure_Set,
	State_Set_Menu_AutoMeasure,
		State_Set_Menu_AutoMeasure_BackGround,
		State_AutoMeasure_Timelong_Set_ADD,
		State_AutoMeasure_Timelong_Set_SUB,
		State_AutoMeasure_Timeinterval_Set_ADD,
		State_AutoMeasure_Timeinterval_Set_SUB,
		State_AutoMeasure_Set_OK,
	State_Set_Menu_MeasurePara,
			State_MeasurePara_Groupnum,
			State_MeasurePara_Groupnum_BackGround,
				State_Groupnum_Set_ADD,
				State_Groupnum_Set_SUB,
				State_Groupnum_Set_OK,
			State_MeasurePare_Timelong,
			State_MeasurePare_Timelong_BackGround,
				State_Timelong_Set_ADD,
				State_Timelong_Set_SUB,
				State_Timelong_Set_OK,
			State_MeasurePara_Timeinterval,
				State_MeasurePara_Timeinterval_BackGround,
				State_Timeinterval_Set_ADD,
				State_Timeinterval_Set_SUB,
				State_Timeinterval_Set_OK,
			State_MeasurePara_Eletrode,
				State_Select_Eletrode,
	

	State_Set_Menu_User_Set,  //用户设置 20170304
	State_Set_Menu_Sex, //设置页性别
				State_Select_Sex,
	State_Set_Menu_Age,
			State_Set_Menu_Age_BackGround,
			State_Age_Set_Para_ADD,
			State_Age_Set_Para_SUB,
			State_Age_Set_OK,
	State_Set_Menu_Height,
			State_Set_Menu_Height_BackGround,
			State_Height_Set_Para_ADD,
			State_Height_Set_Para_SUB,
			State_Height_Set_OK,
	State_Set_Menu_Weight,
			State_Set_Menu_Weight_Back_Ground,
			State_Weight_Set_Para_ADD,
			State_Weight_Set_Para_SUB,
			State_Weight_Set_OK,
			
	State_Set_Menu_Other_Set,  //其它设置 20170304
	State_Set_Menu_Ble,
		State_Select_Ble_OnOff,
	State_Set_Menu_NoDisturb,    //防打扰
		State_Select_NoDisturb_Mode,
	State_Set_Menu_StandbyTime,  //待机时间
		State_Set_Menu_StandbyTime_BackGround,
		State_StandbyTime_Set_ADD,
		State_StandbyTime_Set_SUB,
		State_StandbyTime_Set_OK,
	State_Set_Menu_DeviceInfo,
		State_Software_Info,
		State_DeviceInfo_Version,
		State_Device_ID,
		State_PW1101IC_ID,
		State_Senser_ID,
	State_Set_Menu_ReFactory,
		State_ReFactory_Background,
		State_ReFactory_OK,
		State_ReFactory_ESC
			
}State;

extern uint8_t  bat_charge_flag;

extern uint8_t  First_in_Main_Menu;
uint8_t DisBackGround(State );
uint8_t Func_DeviceCheck(uint32_t );
typedef  uint8_t (*fpExecFunc)(uint32_t);
typedef  uint8_t (*fpDisBackFunc)(State );
//uint8_t Is_ParaSet_States();

typedef struct{
	
	State navigationData[NUM_OF_NAV_EVENTS];		// navigation states, index 0-3 corresponds to Up/Down/Left/Right
	fpExecFunc ExecFunction;
	fpDisBackFunc DisBackFunc;
}StateData;


extern StateData stateData[];
#endif 
