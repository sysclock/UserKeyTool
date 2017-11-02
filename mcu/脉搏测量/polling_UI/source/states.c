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



//�豸�Բ�
uint8_t Func_DeviceCheck(uint32_t para)
{
	uint32_t num;
	PW1101_receive_flag=0;
	PW1101_rxIndex=0;

	if(g_CurrentStat==State_Manual_DeviceCheck && Auto_Measure_OnOff==TURN_ON)
	{
		return AUTO_MEASURE_GOING;      //������ֶ����������Բ⣬���Զ�����ʱ��Ҫ������ʾ�Զ����ڽ�����
	}
	//���ε����͡��¶ȸߡ��¶ȵʹ���
//	if(read_bat_adc()<2647) //��ѹС��3.2V
//	{
//		return LOW_POWER;    //������
//	}
//	Delay_S(1);
//	num=read_temperature_adc();
//	if(num>TEMP_HIGE_VALUE)    //�¶ȸ�
//	{
//		return TEMP_HIGH;
//	}
//	else if(num<TEMP_LOW_VALUE)  //�¶ȵ�
//	{
//		return TEMP_LOW;
//	}
	PW1101_Comm_RxQueue_Init();
	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	while(1)  //��ͨ�Ų���
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


//״̬����״̬���Լ�����ʱ��״̬ת���������ⲿ��������ʾ����
//�˲��ֶ���������ı�д���Ķ���������¹���Ҫ������
//�˴��������StateData���͵�һ������

StateData stateData[]={

	//State_Main_Menu************���˵�**************1.0
	{	
		{
			State_Main_Menu,		//set      //��ҳ��ʱ����SET���������ã���Ȼָ���Զ�  ��Ҫ�������水ȷ���л���ĳһ״̬��ֻҪ�ڴ��޸ļ���
			State_Main_Menu_Manual, //UD   //�л���ָ���ֶ����� 
			State_Main_Menu, //RES			  //���ؼ�ָ���Լ�
		},
		NULL,   //���ⲿ��������
		&DisBackGround,  //������ʾ
	},
//	State_Main_Menu_interact_error,   //����ʱ�����ͨ���쳣�����д�״̬
	{	
		{
			State_Main_Menu,		//set
			State_Main_Menu_interact_error, //UD    
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},	
	//State_Main_Menu_Manual*********�ֶ�����***********1.0
	{	
		{
			State_Manual_DeviceCheck,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//State_Manual_DeviceCheck,   //�ֶ��������豸���
	{	
		{
			State_Manual_DeviceCheck,		//set
			State_Manual_DeviceCheck, //UD
			State_Manual_DeviceCheck, //RES			
		},
		&Func_DeviceCheck,  //�豸�Լ캯��
		&DisBackGround,
	},
//	State_Manual_Usertouch,    //�ֶ�����ʱ����ʾ��ѹ�缫������
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Usertouch, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Manual_Doing,       //��ʼ�ֶ�����
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Doing, //UD
			State_Manual_Doing, //RES			
		},
		&pw1101_func_manual_measure_mode_select,  //ѡ�����ģʽ����
		&DisBackGround,
	},
//	State_Manual_Measure_OK,   //�ֶ���������� 
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_Measure_OK, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
// State_Manual_Measure_error1,  //�������
	{	
		{
			State_Manual_Doing,		//set
			State_Manual_Measure_error1, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_pw1101_interact_error,  �ֶ�����ͨ�Ŵ���
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Main_Menu_Manual, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Manual_pw1101_AutoGoing_error   //�ֶ�����ʱ���Զ��Ѿ�����
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_pw1101_AutoGoing_error, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	}, 
//	State_Manual_LowPower,      //��ѹ��
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_LowPower, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Manual_TempHigh,    //�¶ȸ�
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_TempHigh, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_TempLow,   //�¶ȵ�
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_TempLow, //UD
			State_Main_Menu_Manual, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Manual_Measure_False,   //����ʧ��
	{	
		{
			State_Main_Menu_Manual,		//set
			State_Manual_Measure_False, //UD
			State_Main_Menu_Manual, //RES		
		},
		NULL,
		&DisBackGround,
	},
	//State_Main_Menu_Auto*********�Զ�����************1.0
	{	
		{
			State_Auto_Measure_IsTurnOFF,		//set
			State_Main_Menu_Record, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_IsTurnON,    //��̨�����Ѿ�����
	{	
		{
			State_Auto_Measure_TurnOFF,		//set
			State_Auto_Measure_IsTurnON, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_TurnOFF,   //�ر��Զ�����
	{	
		{
			State_Auto_Measure_TurnOFF,		//set   
			State_Auto_Measure_TurnOFF, //UD  
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Auto_Measure_IsTurnOFF,  //��ǰ�ǹر�״̬
	{	
		{
			State_Auto_DeviceCheck,		//set
			State_Auto_Measure_IsTurnOFF, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Auto_Measure_TurnON,  //���Զ�����
	{	
		{
			State_Auto_Measure_TurnON,		//set
			State_Auto_Measure_TurnON, //UD
			State_Auto_Measure_TurnON, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_DeviceCheck,   //�Զ��������豸�Լ�
	{	
		{
			State_Auto_DeviceCheck,		//set
			State_Auto_DeviceCheck, //UD
			State_Main_Menu_Auto, //RES			
		},
		&Func_DeviceCheck,
		&DisBackGround,
	},
	
	
//	State_Auto_Usertouch,    //û����
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Usertouch, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Auto_Doing,    //�Զ�����������
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Doing, //UD
			State_Auto_Doing, //RES			
		},
		&pw1101_func_manual_measure_mode_select,
		&DisBackGround,
	},
	//	State_Auto_Measure_OK,  //�Զ������յ����
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Auto_Measure_OK, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
// State_Auto_Measure_error1,  //�������
	{	
		{
			State_Auto_Doing,		//set
			State_Auto_Measure_error1, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_pw1101_interact_error,  //�Լ�ʱ��ͨ������
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_LowPower,    //�Լ�ʱ����ѹ��
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_TempHigh,  //�¶ȸ�
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_TempLow,  //�¶ȵ�
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Main_Menu_Auto, //UD
			State_Main_Menu_Auto, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Auto_Measure_False,  //����ʧ��
	{	
		{
			State_Main_Menu_Auto,		//set
			State_Auto_Measure_False, //UD
			State_Main_Menu_Auto, //RES		
		},
		NULL,
		&DisBackGround,
	},

	
	//State_Main_Menu_Record*********��ʷ��¼***********1.0
	{	
		{
			State_Record_Display,		//set
			State_Main_Menu_Set, //UD
			State_Main_Menu, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//State_Record_Display*********��ʷ��¼***********1.0
	{	
		{
			State_Record_Display,		//set
			State_Record_Display, //UD
			State_Main_Menu_Record, //RES			
		},
		&Read_Record_Data_From_Flash,
		&DisBackGround,
	},

		//State_Main_Menu_Set*********����************1.0
	{	
		{
			State_Set_Menu_Data_Time,		//set
			State_Main_Menu_Manual, //UD       //������ҳ�水�л�������������ҳ��
			State_Main_Menu, //RES			     		//������ҳ�水���ؼ�����������ҳ��
		},
		NULL,
		&DisBackGround,
	}, 
	//	State_Set_error,   //����ʱ������Զ��򿪣����д�״̬
	{	
		{
			State_Main_Menu_Set,		//set
			State_Set_error, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_Data_Time,   //���ò˵��е�ʱ������   20170304
	{	
		{
			State_Set_Menu_Date,		//set   //���뵽��������״̬
			State_Set_Menu_Measure_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	//	State_Set_Menu_Date,  //�������� 
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
//	State_Date_Set_Year_ADD,  //����������
	{	
		{
			State_Date_Set_Year_ADD,		//set
			State_Date_Set_Year_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Year_SUB, //�������С
	{	
		{
			State_Date_Set_Year_SUB,		//set
			State_Date_Set_Month_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Month_ADD,   //����������
	{	
		{
			State_Date_Set_Month_ADD,		//set
			State_Date_Set_Month_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Month_SUB,   //�����¼�С
	{	
		{
			State_Date_Set_Month_SUB,		//set
			State_Date_Set_Day_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Day_ADD,  ������
	{	
		{
			State_Date_Set_Day_ADD,		//set
			State_Date_Set_Day_SUB, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_Day_SUB,   �ռ�С
	{	
		{
			State_Date_Set_Day_SUB,		//set
			State_Date_Set_OK, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Date_Set_OK,   ��������ȷ��
	{	
		{
			State_Set_Menu_Date,		//set
			State_Date_Set_Year_ADD, //UD
			State_Set_Menu_Date, //RES			
		},
		NULL,
		&DisBackGround,
	},	
//	State_Set_Menu_Time,   ����ʱ��
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
//	State_Time_Hour_ADD,  Сʱ��
	{	
		{
			State_Time_Hour_ADD,		//set
			State_Time_Hour_SUB, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Time_Hour_SUB,  Сʱ��
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
//	State_Time_Set_OK,   ����ʱ��ȷ��
	{	
		{
			State_Set_Menu_Time,		//set
			State_Time_Hour_ADD, //UD
			State_Set_Menu_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
	

// State_Set_Menu_GMT,  ����ʱ��
	{	
		{
			State_Set_Menu_GMT_BackGround,		//set
			State_Set_Menu_Date, //UD
			State_Set_Menu_Data_Time, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_GMT_BackGround,  //����ʱ���ı���
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
	
//	State_Set_Menu_HBlood,    ���ø�ѹ��ֵ
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
//	State_HBlood_Set_Low_ADD,    ��ѹ���޼�
	{	
		{
			State_HBlood_Set_Low_ADD,		//set
			State_HBlood_Set_Low_SUB, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_HBlood_Set_Low_SUB,  ��ѹ���޼�
	{	
		{
			State_HBlood_Set_Low_SUB,		//set
			State_HBlood_Set_High_ADD, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_High_ADD,  ��ѹ���޼�
	{	
		{
			State_HBlood_Set_High_ADD,		//set
			State_HBlood_Set_High_SUB, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_High_SUB,  ��ѹ���޼�
	{	
		{
			State_HBlood_Set_High_SUB,		//set
			State_HBlood_Set_OK, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_HBlood_Set_OK,  ��ѹ�����������
	{	
		{
			State_Set_Menu_HBlood,		//set
			State_HBlood_Set_Low_ADD, //UD
			State_Set_Menu_HBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_LBlood,   ��ѹ��ֵ
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
//	State_LBlood_Set_Low_ADD,  ��ѹ���޼�
	{	
		{
			State_LBlood_Set_Low_ADD,		//set
			State_LBlood_Set_Low_SUB, //UD
			State_Set_Menu_LBlood, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_LBlood_Set_Low_SUB,  ��ѹ���޼�
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
 
//	State_Set_Menu_HeartRate,   ����������ֵ
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
	
//	State_Set_Menu_kPa, //���ò�����λ
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
//	State_Set_Menu_MeasureMode,   ���ò���ģʽ��ֻ���ֶ�������ʱ����Ч
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
	
//	State_Set_Menu_Measure_Set,  //���ò˵��еĲ�������  20170304
	{	
		{
			State_MeasurePara_Eletrode,		//set
			State_Set_Menu_User_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_Set_Menu_AutoMeasure,   �Զ��������� 
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
//	State_AutoMeasure_Timelong_Set_ADD,  ����ʱ��������Сʱ��
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
//	State_AutoMeasure_Timeinterval_Set_ADD,    ���ü��ʱ�� 
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
//	State_Set_Menu_MeasurePara,  ���ò�������
	{	
		{
			State_MeasurePara_Groupnum,		//set
			State_Set_Menu_Sex, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
	
//	State_MeasurePara_Groupnum,  ���ò�������
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
//	State_MeasurePare_Timelong,  ����ʱ��
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
//	State_MeasurePara_Timeinterval,�������
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
//	State_MeasurePara_Eletrode,   �����缫
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
	
//	State_Set_Menu_User_Set, //���ò˵��û�����  20170304
	{	
		{
			State_Set_Menu_Sex,		//set
			State_Set_Menu_Other_Set, //UD
			State_Main_Menu_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Set_Menu_Sex, //����ҳ�Ա�
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

//	State_Set_Menu_Age,  ��������
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
//	State_Set_Menu_Height,   �������
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
	
//	State_Set_Menu_Weight,  ��������
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
	
//State_Set_Menu_Ble,   ��������
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
//State_Set_Menu_NoDisturb,    //������
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
//State_Set_Menu_StandbyTime,  //����ʱ��
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
//State_Set_Menu_DeviceInfo,  �豸��Ϣ
	{	
		{
			State_Software_Info,		//set
			State_Set_Menu_Ble, //UD
			State_Set_Menu_Other_Set, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Software_Info,   �����Ϣ
	{	
		{
			State_Software_Info,		//set
			State_DeviceInfo_Version, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_DeviceInfo_Version,  �汾��Ϣ
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
//	State_PW1101IC_ID,   ��ʯ��ID��ʾ
	{	
		{
			State_PW1101IC_ID,		//set
			State_Software_Info, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//	State_Senser_ID,�������������ģɣ�
	{	
		{
			State_Senser_ID,		//set
			State_Software_Info, //UD
			State_Set_Menu_DeviceInfo, //RES			
		},
		NULL,
		&DisBackGround,
	},
//State_Set_Menu_ReFactory�����ָ���������
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

