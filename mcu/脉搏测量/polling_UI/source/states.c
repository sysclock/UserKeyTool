#include "states.h"
#include "stdlib.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "flash_spi.h"
#include "pw1101_interact.h"
#include "uart.h"
#include "rtc.h"
#include "Queue.h"
#include "bat_adc.h"



//设备自测
uint8_t Func_DeviceCheck(uint32_t para)
{
	uint32_t num;
	PW1101_receive_flag=0;
	PW1101_rxIndex=0;

	if(g_CurrentStat==State_Manual_DeviceCheck && Auto_Measure_OnOff==TURN_ON)
	{
		return AUTO_MEASURE_GOING;      //如果是手动测量做的自测，当自动开启时，要进行提示自动正在进行中
	}
	//屏蔽电量低、温度高、温度低错误
//	if(read_bat_adc()<2647) //电压小于3.2V
//	{
//		return LOW_POWER;    //电量低
//	}
//	Delay_S(1);
//	num=read_temperature_adc();
//	if(num>TEMP_HIGE_VALUE)    //温度高
//	{
//		return TEMP_HIGH;
//	}
//	else if(num<TEMP_LOW_VALUE)  //温度低
//	{
//		return TEMP_LOW;
//	}
	PW1101_Comm_RxQueue_Init();
	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	while(1)  //做通信测试
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE 
				&& (comm_cache[DATA_INDEX_1]==1 || comm_cache[DATA_INDEX_1]==4 || comm_cache[DATA_INDEX_1]==7))
			{
				break;
			}
		}	
		if(Pw1101_interact_timeout>3)
		{		
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_status_check();
		Delay_S(1);
	}	
	
	return 1;
}


//状态机各状态，以及按键时的状态转换，调用外部函数及显示更新
//此部分对整个程序的编写、阅读和理解有致关重要的作用
//此处定义的是StateData类型的一个数组

StateData stateData[]={

	//State_Main_Menu************主菜单**************1.0
	{	
		{
			State_Main_Menu,		//set      //主页面时，按SET键不起作用，依然指向自动  如要在主界面按确键切换到某一状态，只要在此修改即可
			State_Main_Menu_Manual, //UD   //切换键指向手动测量 
			State_Main_Menu, //RES			  //返回键指向自己
		},
		NULL,   //无外部函数调用
		&DisBackGround,  //更新显示
	},
//	State_Main_Menu_interact_error,   //开机时候如果通信异常，进行此状态
	{	
		{
			State_Main_Menu,		//set
			State_Main_Menu_interact_error, //UD    
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},	
	//State_Main_Menu_Manual*********手动测量***********1.0
	{	
		{
			State_Manual_DeviceCheck,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//State_Manual_DeviceCheck,   //手动测量的设备检测
	{	
		{
			State_Manual_DeviceCheck,		//set
			State_Manual_DeviceCheck, //UD
			State_Manual_DeviceCheck, //RES			
		},
		&Func_DeviceCheck,  //设备自检函数
		&DisBackGround,
	},
//	State_Manual_Usertouch,    //手动测量时，提示按压电极或导联线
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Usertouch, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Manual_Doing,       //开始手动测量
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Doing, //UD
			State_Manual_Doing, //RES			
		},
		&pw1101_func_manual_measure_mode_select,  //选择测量模式函数
		&DisBackGround,
	},
//	State_Manual_Measure_OK,   //手动测量出结果 
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_Measure_OK, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
// State_Manual_Measure_error1,  //佩戴问题
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Measure_error1, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_pw1101_interact_error,  手动测量通信错误
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Main_Menu_Manual, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Manual_pw1101_AutoGoing_error   //手动测量时，自动已经开启
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_pw1101_AutoGoing_error, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	}, 
//	State_Manual_LowPower,      //电压低
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_LowPower, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Manual_TempHigh,    //温度高
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_TempHigh, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_TempLow,   //温度低
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_TempLow, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_Measure_False,   //测量失败
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_Measure_False, //UD
			State_Main_Menu_Manual, //RES		
		},
		NULL,
		&DisBackGround,
	},
	//State_Main_Menu_Auto*********自动测量************1.0
	{	
		{
			State_Auto_Measure_IsTurnOFF,		//set
			State_Main_Menu_Record, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_IsTurnON,    //后台测量已经开启
	{	
		{
			State_Auto_Measure_TurnOFF,		//set
			State_Auto_Measure_IsTurnON, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_TurnOFF,   //关闭自动测量
	{	
		{
			State_Auto_Measure_TurnOFF,		//set   
			State_Auto_Measure_TurnOFF, //UD  
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Auto_Measure_IsTurnOFF,  //当前是关闭状态
	{	
		{
			State_Auto_DeviceCheck,		//set
			State_Auto_Measure_IsTurnOFF, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Auto_Measure_TurnON,  //打开自动测量
	{	
		{
			State_Auto_Measure_TurnON,		//set
			State_Auto_Measure_TurnON, //UD
			State_Auto_Measure_TurnON, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_DeviceCheck,   //自动测量的设备自检
	{	
		{
			State_Auto_DeviceCheck,		//set
			State_Auto_DeviceCheck, //UD
			State_Main_Menu_Auto, //RES			
		},
		&Func_DeviceCheck,
		&DisBackGround,
	},
	
	
//	State_Auto_Usertouch,    //没有用
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Usertouch, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Auto_Doing,    //自动测量进行中
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Doing, //UD
			State_Auto_Doing, //RES			
		},
		&pw1101_func_manual_measure_mode_select,
		&DisBackGround,
	},
	//	State_Auto_Measure_OK,  //自动测量收到结果
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Auto_Measure_OK, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
// State_Auto_Measure_error1,  //佩戴问题
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Measure_error1, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_pw1101_interact_error,  //自检时，通信问题
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_LowPower,    //自检时，电压低
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_TempHigh,  //温度高
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_TempLow,  //温度低
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_False,  //测量失败
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Auto_Measure_False, //UD
			State_Main_Menu_Auto, //RES		
		},
		NULL,
		&DisBackGround,
	},

	
	//State_Main_Menu_Record*********历史记录***********1.0
	{	
		{
			State_Record_Display,		//set
			State_Main_Menu_Set, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//State_Record_Display*********历史记录***********1.0
	{	
		{
			State_Record_Display,		//set
			State_Record_Display, //UD
			State_Main_Menu_Record, //RES			
		},
		&Read_Record_Data_From_Flash,
		&DisBackGround,
	},

		//State_Main_Menu_Set*********设置************1.0
	{	
		{
			State_Set_Menu_Data_Time,		//set
			State_Main_Menu_Manual, //UD       //在设置页面按切换键，跳到测量页面
			State_Main_Menu, //RES			     		//在设置页面按返回键，跳到测量页面
		},
		NULL,
		&DisBackGround,
	}, 
	//	State_Set_error,   //设置时，如果自动打开，进行此状态
	{	
		{
			State_Main_Menu_Set,		//set
			State_Set_error, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_Data_Time,   //设置菜单中的时间设置   20170304
	{	
		{
			State_Set_Menu_Date,		//set   //进入到设置日期状态
			State_Set_Menu_Measure_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//	State_Set_Menu_Date,  //设置日期 
	{	
		{
			State_Set_Menu_Date_BackGround,		//set
			State_Set_Menu_Time, //UD
			State_Set_Menu_Data_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_Date_BackGround,
	{	
		{
			State_Set_Menu_Date_BackGround,		//set
			State_Set_Menu_Date_BackGround, //UD
			State_Set_Menu_Date_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Year_ADD,  //设置年增加
	{	
		{
			State_Date_Set_Year_ADD,		//set
			State_Date_Set_Year_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Year_SUB, //设置年减小
	{	
		{
			State_Date_Set_Year_SUB,		//set
			State_Date_Set_Month_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Month_ADD,   //设置月增加
	{	
		{
			State_Date_Set_Month_ADD,		//set
			State_Date_Set_Month_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Month_SUB,   //设置月减小
	{	
		{
			State_Date_Set_Month_SUB,		//set
			State_Date_Set_Day_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Day_ADD,  日增加
	{	
		{
			State_Date_Set_Day_ADD,		//set
			State_Date_Set_Day_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Day_SUB,   日减小
	{	
		{
			State_Date_Set_Day_SUB,		//set
			State_Date_Set_OK, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_OK,   日期设置确认
	{	
		{
			State_Set_Menu_Date,		//set
			State_Date_Set_Year_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},	
//	State_Set_Menu_Time,   设置时间
		{	
			{
				State_Set_Menu_Time_BackGround,		//set
				State_Set_Menu_GMT, //UD
				State_Set_Menu_Data_Time, //RES			
			},
			NULL,
			&DisBackGround,
		},
//		State_Set_Menu_Time_BackGround
		{	
			{
				State_Set_Menu_Time_BackGround,		//set
				State_Set_Menu_Time_BackGround, //UD
				State_Set_Menu_Time_BackGround, //RES			
			},
			NULL,
			&DisBackGround,
		},
//	State_Time_Hour_ADD,  小时加
	{	
		{
			State_Time_Hour_ADD,		//set
			State_Time_Hour_SUB, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Time_Hour_SUB,  小时减
	{	
		{
			State_Time_Hour_SUB,		//set
			State_Time_Minute_ADD, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Time_Minute_ADD,
	{	
		{
			State_Time_Minute_ADD,		//set
			State_Time_Minute_SUB, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Time_Minute_SUB,
	{	
		{
			State_Time_Minute_SUB,		//set
			State_Time_Set_OK, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Time_Set_OK,   设置时间确认
	{	
		{
			State_Set_Menu_Time,		//set
			State_Time_Hour_ADD, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
	

// State_Set_Menu_GMT,  设置时区
	{	
		{
			State_Set_Menu_GMT_BackGround,		//set
			State_Set_Menu_Date, //UD
			State_Set_Menu_Data_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_GMT_BackGround,  //设置时区的背景
	{	
		{
			State_Set_Menu_GMT_BackGround,		//set
			State_Set_Menu_GMT_BackGround, //UD
			State_Set_Menu_GMT_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_GMT_ADD,
	{	
		{
			State_GMT_ADD,		//set
			State_GMT_SUB, //UD
			State_Set_Menu_GMT, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_GMT_SUB,
	{	
		{
			State_GMT_SUB,		//set
			State_GMT_OK, //UD
			State_Set_Menu_GMT, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_GMT_OK,
	{	
		{
			State_Set_Menu_GMT,		//set
			State_GMT_ADD, //UD
			State_Set_Menu_GMT, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_HBlood,    设置高压限值
	{	
		{
			State_Set_Menu_HBlood_BackGround,		//set
			State_Set_Menu_LBlood, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_HBlood_BackGround,
	{	
		{
			State_Set_Menu_HBlood_BackGround,		//set
			State_Set_Menu_HBlood_BackGround, //UD
			State_Set_Menu_HBlood_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_Low_ADD,    高压下限加
	{	
		{
			State_HBlood_Set_Low_ADD,		//set
			State_HBlood_Set_Low_SUB, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_HBlood_Set_Low_SUB,  高压下限减
	{	
		{
			State_HBlood_Set_Low_SUB,		//set
			State_HBlood_Set_High_ADD, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_High_ADD,  高压上限加
	{	
		{
			State_HBlood_Set_High_ADD,		//set
			State_HBlood_Set_High_SUB, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_High_SUB,  高压上限减
	{	
		{
			State_HBlood_Set_High_SUB,		//set
			State_HBlood_Set_OK, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_OK,  高压上限设置完成
	{	
		{
			State_Set_Menu_HBlood,		//set
			State_HBlood_Set_Low_ADD, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_LBlood,   低压限值
	{	
		{
			State_Set_Menu_LBlood_BackGround,		//set
			State_Set_Menu_HeartRate, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_LBlood_BackGround	
	{	
		{
			State_Set_Menu_LBlood_BackGround,		//set
			State_Set_Menu_LBlood_BackGround, //UD
			State_Set_Menu_LBlood_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_LBlood_Set_Low_ADD,  低压下限加
	{	
		{
			State_LBlood_Set_Low_ADD,		//set
			State_LBlood_Set_Low_SUB, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_LBlood_Set_Low_SUB,  低压下限减
	{	
		{
			State_LBlood_Set_Low_SUB,		//set
			State_LBlood_Set_High_ADD, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_LBlood_Set_High_ADD,
	{	
		{
			State_LBlood_Set_High_ADD,		//set
			State_LBlood_Set_High_SUB, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_LBlood_Set_High_SUB,
	{	
		{
			State_LBlood_Set_High_SUB,		//set
			State_LBlood_Set_OK, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_LBlood_Set_OK,
	{	
		{
			State_Set_Menu_LBlood,		//set
			State_LBlood_Set_Low_ADD, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
 
//	State_Set_Menu_HeartRate,   设置心率限值
	{	
		{
			State_Set_Menu_HeartRate_BackGround,		//set
			State_Set_Menu_kPa, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_HeartRate_BackGround,
	{	
		{
			State_Set_Menu_HeartRate_BackGround,		//set
			State_Set_Menu_HeartRate_BackGround, //UD
			State_Set_Menu_HeartRate_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_HeartRate_Set_Low_ADD,
	{	
		{
			State_HeartRate_Set_Low_ADD,		//set
			State_HeartRate_Set_Low_SUB, //UD
			State_Set_Menu_HeartRate, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HeartRate_Set_Low_SUB,
	{	
		{
			State_HeartRate_Set_Low_SUB,		//set
			State_HeartRate_Set_High_ADD, //UD
			State_Set_Menu_HeartRate, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HeartRate_Set_High_ADD,
	{	
		{
			State_HeartRate_Set_High_ADD,		//set
			State_HeartRate_Set_High_SUB, //UD
			State_Set_Menu_HeartRate, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HeartRate_Set_High_SUB,
	{	
		{
			State_HeartRate_Set_High_SUB,		//set
			State_HeartRate_Set_OK, //UD
			State_Set_Menu_HeartRate, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HeartRate_Set_OK,
	{	
		{
			State_Set_Menu_HeartRate,		//set
			State_HeartRate_Set_Low_ADD, //UD
			State_Set_Menu_HeartRate, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_kPa, //设置测量单位
	{	
		{
			State_Select_mmHgkPa,		//set
			State_Set_Menu_MeasureMode, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	state_Select_mmHgkPa,
	{	
		{
			State_Set_Menu_kPa,		//set
			State_Select_mmHgkPa, //UD
			State_Set_Menu_kPa, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_MeasureMode,   设置测量模式，只在手动测量的时候生效
	{	
		{
			State_Select_MeasureMode,		//set
			State_Set_Menu_AutoMeasure, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Select_MeasureMode,
	{	
		{
			State_Set_Menu_MeasureMode,		//set
			State_Select_MeasureMode, //UD
			State_Set_Menu_MeasureMode, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_Measure_Set,  //设置菜单中的测量设置  20170304
	{	
		{
			State_MeasurePara_Eletrode,		//set
			State_Set_Menu_User_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_AutoMeasure,   自动测量设置 
	{	
		{
			State_Set_Menu_AutoMeasure_BackGround,		//set
			State_MeasurePara_Eletrode, //UD
			State_Set_Menu_Measure_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_AutoMeasure_BackGround
	{	
		{
			State_Set_Menu_AutoMeasure_BackGround,		//set
			State_Set_Menu_AutoMeasure_BackGround, //UD
			State_Set_Menu_AutoMeasure_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_AutoMeasure_Timelong_Set_ADD,  设置时长，测量小时数
	{	
		{
			State_AutoMeasure_Timelong_Set_ADD,		//set
			State_AutoMeasure_Timelong_Set_SUB, //UD
			State_Set_Menu_AutoMeasure, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_AutoMeasure_Timelong_Set_SUB,
	{	
		{
			State_AutoMeasure_Timelong_Set_SUB,		//set
			State_AutoMeasure_Timeinterval_Set_ADD, //UD
			State_Set_Menu_AutoMeasure, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_AutoMeasure_Timeinterval_Set_ADD,    设置间隔时间 
	{	
		{
			State_AutoMeasure_Timeinterval_Set_ADD,		//set
			State_AutoMeasure_Timeinterval_Set_SUB, //UD
			State_Set_Menu_AutoMeasure, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_AutoMeasure_Timeinterval_Set_SUB,
	{	
		{
			State_AutoMeasure_Timeinterval_Set_SUB,		//set
			State_AutoMeasure_Set_OK, //UD
			State_Set_Menu_AutoMeasure, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_AutoMeasure_Set_OK,
	{	
		{
			State_Set_Menu_AutoMeasure,		//set
			State_AutoMeasure_Timelong_Set_ADD, //UD
			State_Set_Menu_AutoMeasure, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_MeasurePara,  设置测量参数
	{	
		{
			State_MeasurePara_Groupnum,		//set
			State_Set_Menu_Sex, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_MeasurePara_Groupnum,  设置测量组数
	{	
		{
			State_MeasurePara_Groupnum_BackGround,		//set
			State_MeasurePare_Timelong, //UD
			State_Set_Menu_MeasurePara, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePara_Groupnum_BackGround,
	{	
		{
			State_MeasurePara_Groupnum_BackGround,		//set
			State_MeasurePara_Groupnum_BackGround, //UD
			State_MeasurePara_Groupnum_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Groupnum_Set_ADD,
	{	
		{
			State_Groupnum_Set_ADD,		//set
			State_Groupnum_Set_SUB, //UD
			State_MeasurePara_Groupnum, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Groupnum_Set_SUB,
	{	
		{
			State_Groupnum_Set_SUB,		//set
			State_Groupnum_Set_OK, //UD
			State_MeasurePara_Groupnum, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Groupnum_Set_OK,
	{	
		{
			State_MeasurePara_Groupnum,		//set
			State_Groupnum_Set_ADD, //UD
			State_MeasurePara_Groupnum, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePare_Timelong,  测量时间
	{	
		{
			State_MeasurePare_Timelong_BackGround,		//set
			State_MeasurePara_Timeinterval, //UD
			State_Set_Menu_MeasurePara, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePare_Timelong_BackGround
	{	
		{
			State_MeasurePare_Timelong_BackGround,		//set
			State_MeasurePare_Timelong_BackGround, //UD
			State_MeasurePare_Timelong_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Timelong_Set_ADD,
	{	
		{
			State_Timelong_Set_ADD,		//set
			State_Timelong_Set_SUB, //UD
			State_MeasurePare_Timelong, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Timelong_Set_SUB,
	{	
		{
			State_Timelong_Set_SUB,		//set
			State_Timelong_Set_OK, //UD
			State_MeasurePare_Timelong, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Timelong_Set_OK,
	{	
		{
			State_MeasurePare_Timelong,		//set
			State_Timelong_Set_ADD, //UD
			State_MeasurePare_Timelong, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePara_Timeinterval,测量间隔
	{	
		{
			State_MeasurePara_Timeinterval_BackGround,		//set
			State_MeasurePara_Eletrode, //UD
			State_Set_Menu_MeasurePara, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePara_Timeinterval_BackGround
		{	
		{
			State_MeasurePara_Timeinterval_BackGround,		//set
			State_MeasurePara_Timeinterval_BackGround, //UD
			State_MeasurePara_Timeinterval_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//	State_Timeinterval_Set_ADD,
	{	
		{
			State_Timeinterval_Set_ADD,		//set
			State_Timeinterval_Set_SUB, //UD
			State_MeasurePara_Timeinterval, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Timeinterval_Set_SUB,
	{	
		{
			State_Timeinterval_Set_SUB,		//set
			State_Timeinterval_Set_OK, //UD
			State_MeasurePara_Timeinterval, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Timeinterval_Set_OK,
	{	
		{
			State_MeasurePara_Timeinterval,		//set
			State_Timeinterval_Set_ADD, //UD
			State_MeasurePara_Timeinterval, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_MeasurePara_Eletrode,   测量电极
	{	
		{
			State_Select_Eletrode,		//set
			State_Set_Menu_AutoMeasure, //UD
			State_Set_Menu_Measure_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	// State_Select_Eletrode
	{	
		{
			State_MeasurePara_Eletrode,		//set
			State_Select_Eletrode, //UD
			State_MeasurePara_Eletrode, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_User_Set, //设置菜单用户设置  20170304
	{	
		{
			State_Set_Menu_Sex,		//set
			State_Set_Menu_Other_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_Sex, //设置页性别
	{	
		{
			State_Select_Sex,		//set
			State_Set_Menu_Age, //UD
			State_Set_Menu_User_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Select_Sex,
	{	
		{
			State_Set_Menu_Sex,		//set
			State_Select_Sex, //UD
			State_Set_Menu_Sex, //RES			
		},
		NULL,
		&DisBackGround,
	},

//	State_Set_Menu_Age,  设置年龄
	{	
		{
			State_Set_Menu_Age_BackGround,		//set
			State_Set_Menu_Height, //UD
			State_Set_Menu_User_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_Age_BackGround	
		{	
		{
			State_Set_Menu_Age_BackGround,		//set
			State_Set_Menu_Age_BackGround, //UD
			State_Set_Menu_Age_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Age_Set_Para_ADD,
	{	
		{
			State_Age_Set_Para_ADD,		//set
			State_Age_Set_Para_SUB, //UD
			State_Set_Menu_Age, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Age_Set_Para_SUB,
	{	
		{
			State_Age_Set_Para_SUB,		//set
			State_Age_Set_OK, //UD
			State_Set_Menu_Age, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Age_Set_OK,
	{	
		{
			State_Set_Menu_Age,		//set
			State_Age_Set_Para_ADD, //UD
			State_Set_Menu_Age, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_Height,   设置身高
	{	
		{
			State_Set_Menu_Height_BackGround,		//set
			State_Set_Menu_Weight, //UD
			State_Set_Menu_User_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},	
//	State_Set_Menu_Height_BackGround
	{	
		{
			State_Set_Menu_Height_BackGround,		//set
			State_Set_Menu_Height_BackGround, //UD
			State_Set_Menu_Height_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},	
	//	State_Height_Set_Para_ADD,
	{	
		{
			State_Height_Set_Para_ADD,		//set
			State_Height_Set_Para_SUB, //UD
			State_Set_Menu_Height, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Height_Set_Para_SUB,
	{	
		{
			State_Height_Set_Para_SUB,		//set
			State_Height_Set_OK, //UD
			State_Set_Menu_Height, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Height_Set_OK,
	{	
		{
			State_Set_Menu_Height,		//set
			State_Height_Set_Para_ADD, //UD
			State_Set_Menu_Height, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_Weight,  设置体重
	{	
		{
			State_Set_Menu_Weight_Back_Ground,		//set
			State_Set_Menu_Sex, //UD
			State_Set_Menu_User_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_Weight_Back_Ground
	{	
		{
			State_Set_Menu_Weight_Back_Ground,		//set
			State_Set_Menu_Weight_Back_Ground, //UD
			State_Set_Menu_Weight_Back_Ground, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//	State_Weight_Set_Para_ADD,
	{	
		{
			State_Weight_Set_Para_ADD,		//set
			State_Weight_Set_Para_SUB, //UD
			State_Set_Menu_Weight, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Weight_Set_Para_SUB,
	{	
		{
			State_Weight_Set_Para_SUB,		//set
			State_Weight_Set_OK, //UD
			State_Set_Menu_Weight, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Weight_Set_OK,
	{	
		{
			State_Set_Menu_Weight,		//set
			State_Weight_Set_Para_ADD, //UD
			State_Set_Menu_Weight, //RES			
		},
		NULL,
		&DisBackGround,
	},

	
//	State_Set_Menu_Other_Set,
	{	
		{
			State_Set_Menu_Ble,		//set
			State_Set_Menu_ReFactory, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//State_Set_Menu_Ble,   蓝牙开关
	{	
		{
			State_Select_Ble_OnOff,		//set
			State_Set_Menu_DeviceInfo, //UD
			State_Set_Menu_Other_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Select_Ble_OnOff,
	{	
		{
			State_Set_Menu_Ble,		//set
			State_Select_Ble_OnOff, //UD
			State_Set_Menu_Ble, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_NoDisturb,    //防打扰
	{	
		{
			State_Select_NoDisturb_Mode,		//set
			State_Set_Menu_StandbyTime, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Select_NoDisturb_Mode
	{
		{
			State_Set_Menu_NoDisturb,		//set
			State_Select_NoDisturb_Mode, //UD
			State_Set_Menu_NoDisturb, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_StandbyTime,  //待机时间
	{	
		{
			State_Set_Menu_StandbyTime_BackGround,		//set
			State_Set_Menu_DeviceInfo, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_StandbyTime_BackGround,
	{	
		{
			State_Set_Menu_StandbyTime_BackGround,		//set
			State_Set_Menu_StandbyTime_BackGround, //UD
			State_Set_Menu_StandbyTime_BackGround, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_StandbyTime_Set_ADD,
	{	
		{
			State_StandbyTime_Set_ADD,		//set
			State_StandbyTime_Set_SUB, //UD
			State_Set_Menu_StandbyTime, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_StandbyTime_Set_SUB,
	{	
		{
			State_StandbyTime_Set_SUB,		//set
			State_StandbyTime_Set_OK, //UD
			State_Set_Menu_StandbyTime, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_StandbyTime_Set_OK,
	{	
		{
			State_Set_Menu_StandbyTime,		//set
			State_StandbyTime_Set_ADD, //UD
			State_Set_Menu_StandbyTime, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_DeviceInfo,  设备信息
	{	
		{
			State_Software_Info,		//set
			State_Set_Menu_Ble, //UD
			State_Set_Menu_Other_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Software_Info,   软件信息
	{	
		{
			State_Software_Info,		//set
			State_DeviceInfo_Version, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_DeviceInfo_Version,  版本信息
	{	
		{
			State_DeviceInfo_Version,		//set
			State_Device_ID, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Device_ID,
	{	
		{
			State_Device_ID,		//set
			State_PW1101IC_ID, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_PW1101IC_ID,   脉石的ID显示
	{	
		{
			State_PW1101IC_ID,		//set
			State_Software_Info, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Senser_ID,　　　传感器的ＩＤ
	{	
		{
			State_Senser_ID,		//set
			State_Software_Info, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_ReFactory　　恢复出厂设置
	{	
		{
			State_ReFactory_Background,		//set
			State_Set_Menu_Data_Time, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},


//State_ReFactory_Background,
	{	
		{
			State_ReFactory_Background,		//set
			State_ReFactory_Background, //UD
			State_ReFactory_Background, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_ReFactory_OK,
	{	
		{
			State_Main_Menu,		//set
			State_ReFactory_ESC, //UD
			State_Set_Menu_ReFactory, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_ReFactory_ESC
	{	
		{
			State_Set_Menu_ReFactory,		//set
			State_ReFactory_OK, //UD
			State_Set_Menu_ReFactory, //RES			
		},
		NULL,
		&DisBackGround,
	},

};

