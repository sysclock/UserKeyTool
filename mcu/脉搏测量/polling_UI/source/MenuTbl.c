#include "MenuTbl.h"
#include "lcd_spi.h"

#include "dateset.h"
#include "show.h"
#include "manual.h"
#include "auto.h"
#include "record.h"
#include "set.h"
#include "devicecheck.h"
#include "error1.h"
#include "error3.h"
#include "error4.h"
#include "error4003.h"
#include "error4004.h"
#include "error1001.h"
#include "error5001.h"
#include "templow_4001.h"
#include "temphigh_4002.h"
#include "Page_Set.h"
#include "Page_MeasurePara.h"
#include "Refactory.h"
#include "fb.h"
#include "utc.h"

//20170304  加菜单
#include "date_time.h"
#include "measure_set.h"
#include "other_set.h"
#include "refactory_set.h"
#include "user_para.h"

#include "num_bmp.h"
#include "pw1101_interact.h"
#include "uart.h"
#include "states.h"
#include "fsl_rtc.h"
#include "rtc.h"
#include "flash_spi.h"
#include "fsl_gpio.h"

State  g_CurrentStat=State_Main_Menu;

_Turn_OnOff Auto_Measure_OnOff =TURN_OFF;

uint32_t RIDL;
uint32_t RIDML;
uint32_t RIDMH;
uint32_t RIDH;

uint8_t mmHg_or_kPa=1;
uint8_t mmHg_or_Kpa_Set;

uint8_t Ble_OnOff=1;
uint8_t Ble_OnOff_Set=0;

uint8_t NoDisturb_Mode=1;
uint8_t NoDisturb_Mode_Set;

uint8_t Measure_Mode=2;
uint8_t Measure_Mode_Set;

uint8_t StandbyTime;
uint8_t StandbyTime_Set;

_user_t user;
_user_t user_set;


uint8_t Current_Eletrode=1;
_MeasurePara_t MeasurePara;
_MeasurePara_t MeasurePara_Set;

_AutoMeasurePara_t AutoMeasurePara;
_AutoMeasurePara_t AutoMeasurePara_Set;
 
_alarm_t alarm;
_alarm_t alarm_set;

rtc_datetime_t measure_time;
uint32_t measure_time_data=0;

void ParaSelect_Func(uint8_t para1,uint8_t para2);
void ParaSelect_OK_Func(uint8_t para1,uint8_t para2);


//显示测量时间
void measure_time_display(void)   
{
	RTC_GetDatetime(RTC, &measure_time);
	measure_time_data=RTC->TSR;
	Display_num_size12_6(1,1,measure_time.month/10);
	Display_num_size12_6(1,8,measure_time.month%10);
	Display_num_size12_6(1,15,10);
	Display_num_size12_6(1,20,measure_time.day/10);
	Display_num_size12_6(1,27,measure_time.day%10);
	
	Display_num_size12_6(1,5+35,measure_time.hour/10);
	Display_num_size12_6(1,5+42,measure_time.hour%10);
	Display_num_size12_6(1,5+49,11);
	Display_num_size12_6(1,5+54,measure_time.minute/10);
	Display_num_size12_6(1,5+61,measure_time.minute%10);
}
	

void Func_Display_Record_data()
{
			/////显示时间/////////
			uint32_t seconds;
			seconds=(READ_Record_data_cache[display_record_index*16+1]<<24)+(READ_Record_data_cache[display_record_index*16+2]<<16)+(READ_Record_data_cache[display_record_index*16+3]<<8)+(READ_Record_data_cache[display_record_index*16+4]);
			RTC_GetDatetime_secondsToDatetime(seconds, &measure_time);
			Display_num_size12_6(1,1,measure_time.month/10);
			Display_num_size12_6(1,8,measure_time.month%10);
			Display_num_size12_6(1,15,10);
			Display_num_size12_6(1,20,measure_time.day/10);
			Display_num_size12_6(1,27,measure_time.day%10);
			
			Display_num_size12_6(1,5+35,measure_time.hour/10);
			Display_num_size12_6(1,5+42,measure_time.hour%10);
			Display_num_size12_6(1,5+49,11);
			Display_num_size12_6(1,5+54,measure_time.minute/10);
			Display_num_size12_6(1,5+61,measure_time.minute%10);
			/////////////////////////////////////////////////////
			if(record_total_num==0)
			{
				Display_num_size12_6(1,1,0);
				Display_num_size12_6(1,8,0);
				Display_num_size12_6(1,15,10);
				Display_num_size12_6(1,20,0);
				Display_num_size12_6(1,27,0);
				
				Display_num_size12_6(1,5+35,0);
				Display_num_size12_6(1,5+42,0);
				Display_num_size12_6(1,5+49,11);
				Display_num_size12_6(1,5+54,0);
				Display_num_size12_6(1,5+61,0);
				
				
				Display_num_size12_6(1,99,0);
				Display_num_size12_6(1,106,12);
				Display_num_size12_6(1,113,0);
				measure_result_data[0]=0;
				measure_result_data[1]=0;
				measure_result_data[2]=0;
				measure_result_data[3]=0;
			}
			else if(record_total_num<10)
			{
				Display_num_size12_6(1,99,(RECORD_NUM-display_record_index));
				Display_num_size12_6(1,106,12);
				Display_num_size12_6(1,113,record_total_num);
				measure_result_data[0]=READ_Record_data_cache[display_record_index*16+6];
				measure_result_data[1]=READ_Record_data_cache[display_record_index*16+7];
				measure_result_data[2]=READ_Record_data_cache[display_record_index*16+8];
				measure_result_data[3]=READ_Record_data_cache[display_record_index*16+9];
			}
			else if(record_total_num<100)
			{
				clear_area(1,85,2,92);
				if((RECORD_NUM-display_record_index)>9)
					Display_num_size12_6(1,85,(RECORD_NUM-display_record_index)/10);
				Display_num_size12_6(1,92,(RECORD_NUM-display_record_index)%10);
				Display_num_size12_6(1,99,12);
				Display_num_size12_6(1,106,record_total_num/10);
				Display_num_size12_6(1,113,record_total_num%10);
				measure_result_data[0]=READ_Record_data_cache[display_record_index*16+6];
				measure_result_data[1]=READ_Record_data_cache[display_record_index*16+7];
				measure_result_data[2]=READ_Record_data_cache[display_record_index*16+8];
				measure_result_data[3]=READ_Record_data_cache[display_record_index*16+9];
			}
			else 
			{
				clear_area(1,71,2,85);
				if((RECORD_NUM-display_record_index)>99)
					Display_num_size12_6(1,71,(RECORD_NUM-display_record_index)/100);
				if((RECORD_NUM-display_record_index)>9)
				Display_num_size12_6(1,78,(RECORD_NUM-display_record_index)/10%10);
				Display_num_size12_6(1,85,(RECORD_NUM-display_record_index)%10);
				Display_num_size12_6(1,92,12);
				Display_num_size12_6(1,99,record_total_num/100);
				Display_num_size12_6(1,106,record_total_num/10%10);
				Display_num_size12_6(1,113,record_total_num%10);
				
				measure_result_data[0]=READ_Record_data_cache[display_record_index*16+6];
				measure_result_data[1]=READ_Record_data_cache[display_record_index*16+7];
				measure_result_data[2]=READ_Record_data_cache[display_record_index*16+8];
				measure_result_data[3]=READ_Record_data_cache[display_record_index*16+9];
			}
			if(mmHg_or_kPa==1)
			{
					clear_area(3,20,6,128);
					clear_area(6,20,8,55);
					if((measure_result_data[0]*256+measure_result_data[1])>99)
						Display_num_size10(3,20,(measure_result_data[0]*256+measure_result_data[1])/100);
					Display_num_size10(3,31,(measure_result_data[0]*256+measure_result_data[1])/10%10);
					Display_num_size10(3,42,(measure_result_data[0]*256+measure_result_data[1])%10);		
					
					if(measure_result_data[2]>99)
						Display_num_size10(3,62,measure_result_data[2]/100);
					Display_num_size10(3,73,measure_result_data[2]/10%10);
					Display_num_size10(3,84,measure_result_data[2]%10);	
					
					Display_num_size12_6(4,54,12);
					Display_16_8_data(4,96,&Record_show_mmHgkPa_data[0*16]);
					Display_16_8_data(4,96+8,&Record_show_mmHgkPa_data[1*16]);
					Display_16_8_data(4,96+16,&Record_show_mmHgkPa_data[2*16]);
					Display_16_8_data(4,96+24,&Record_show_mmHgkPa_data[3*16]);
			}
			else
			{
					clear_area(3,20,6,128);
					clear_area(6,20,8,55);
					if((measure_result_data[0]*256+measure_result_data[1]*133)>9999)
						Display_num_size10(3,20,(measure_result_data[0]*256+measure_result_data[1])*133/10000%10);
					Display_num_size10(3,31,(measure_result_data[0]*256+measure_result_data[1])*133/1000%10);
					Display_16_8_data(4,42,&Record_show_mmHgkPa_data[7*16]);
					Display_num_size10(3,46,(measure_result_data[0]*256+measure_result_data[1])/100%10);		
					
					if(measure_result_data[2]*133>9999)
						Display_num_size10(3,68,(measure_result_data[2]*133)/10000%10);
					Display_num_size10(3,79,measure_result_data[2]*133/1000%10);
					Display_16_8_data(4,90,&Record_show_mmHgkPa_data[7*16]);
					Display_num_size10(3,94,measure_result_data[2]*133/100%10);	
							
					Display_num_size12_6(4,57,12);
					Display_16_8_data(4,105,&Record_show_mmHgkPa_data[4*16]);
					Display_16_8_data(4,105+8,&Record_show_mmHgkPa_data[5*16]);
					Display_16_8_data(4,105+16,&Record_show_mmHgkPa_data[6*16]);				
			}
			if(measure_result_data[3]>99)
				Display_num_size10(6,20,measure_result_data[3]/100);	
			Display_num_size10(6,31,measure_result_data[3]/10%10);
			Display_num_size10(6,42,measure_result_data[3]%10);
}
			

uint8_t First_in_Main_Menu=1;


//函数功能，当前状态如果有图像需要显示，则会调用此函数
//根据状态显示对应的图像
uint8_t  DisBackGround(State CurState)
{
	switch(CurState)
	{
		//新改UI   20170304增加///////
		
		case State_Set_Menu_Data_Time:
			Display_full_screen(Page_Date_Time_Data);
			break;
		case State_Set_Menu_Measure_Set:
			Display_full_screen(Page_Measure_Set_Data);
			break;
		case State_Set_Menu_User_Set:
			Display_full_screen(Page_User_Para_Data);
			break;
		case State_Set_Menu_Other_Set:
			Display_full_screen(Page_Other_Set_Data);
			break;

		//////////////////////////////
		
		
		case State_Main_Menu:
			clear_screen();
			First_in_Main_Menu=1;
			minute_updata_flag=61;
			bat_charge_flag=3;
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
			break;
		case State_Main_Menu_Manual:
			Display_full_screen(Page_manual_Data);
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
			break;
		case State_Manual_DeviceCheck:    //手动测量自检显示
			Display_full_screen(Page_devicecheck_Data);//
			break;
		case State_Manual_Usertouch:
			{	
				clear_screen();
				if(MeasurePara.Eletrode==1) //湿电极显示内空为导联线连接正确
				{
					Display_16_16_data(2,8,&Page_measureusertouch_Data[7*32]);
					Display_16_16_data(2,24,&Page_measureusertouch_Data[8*32]);
					Display_16_16_data(2,40,&Page_measureusertouch_Data[9*32]);
					Display_16_16_data(2,56,&Page_measureusertouch_Data[10*32]);
					Display_16_16_data(2,72,&Page_measureusertouch_Data[11*32]);
					Display_16_16_data(2,88,&Page_measureusertouch_Data[12*32]);
					Display_16_16_data(2,104,&Page_measureusertouch_Data[13*32]);
				}
				else //干电极显示内容为 手指按压干电极
				{
					Display_16_16_data(2,8,&Page_measureusertouch_Data[0]);
					Display_16_16_data(2,24,&Page_measureusertouch_Data[32]);
					Display_16_16_data(2,40,&Page_measureusertouch_Data[64]);
					Display_16_16_data(2,56,&Page_measureusertouch_Data[96]);
					Display_16_16_data(2,72,&Page_measureusertouch_Data[128]);
					Display_16_16_data(2,88,&Page_measureusertouch_Data[160]);
					Display_16_16_data(2,104,&Page_measureusertouch_Data[192]);
				}
				Display_16_16_data(6,8,&Page_measureusertouch_OK_Data[0]);
				Display_16_16_data(6,24,&Page_measureusertouch_OK_Data[32]);
				Display_16_16_data(6,40,&Page_measureusertouch_OK_Data[64]);
				Display_16_16_data(6,56,&Page_measureusertouch_OK_Data[96]);
				Display_16_16_data(6,72,&Page_measureusertouch_OK_Data[128]);
				Display_16_16_data(6,88,&Page_measureusertouch_OK_Data[160]);
				Display_16_16_data(6,104,&Page_measureusertouch_OK_Data[192]);			
			}
			break;
		case State_Manual_Doing:
		case State_Auto_Doing:
			{
				clear_screen();
				Display_16_16_data(3,16,&Page_measuredoing_Data[0]);
				Display_16_16_data(3,32,&Page_measuredoing_Data[32]);
				Display_16_16_data(3,48,&Page_measuredoing_Data[64]);
				Display_16_16_data(3,64,&Page_measuredoing_Data[96]);
				Display_16_16_data(3,80,&Page_measuredoing_Data[128]);
				Display_16_16_data(3,96,&Page_measuredoing_Data[160]);	
				
			}
			break;
		case State_Manual_Measure_OK:
		case State_Auto_Measure_OK:
			{
				Display_full_screen(Record_show_data);
				clear_area(1,1,2,120);
				measure_time_display();	

				if(mmHg_or_kPa==1)
				{
					clear_area(3,20,6,128);
					clear_area(6,20,8,55);
					if((measure_result_data[0]*256+measure_result_data[1])>99)
						Display_num_size10(3,20,(measure_result_data[0]*256+measure_result_data[1])/100);
					Display_num_size10(3,31,(measure_result_data[0]*256+measure_result_data[1])/10%10);
					Display_num_size10(3,42,(measure_result_data[0]*256+measure_result_data[1])%10);		
					
					if(measure_result_data[2]>99)
						Display_num_size10(3,62,measure_result_data[2]/100);
					Display_num_size10(3,73,measure_result_data[2]/10%10);
					Display_num_size10(3,84,measure_result_data[2]%10);	
					
					Display_num_size12_6(4,54,12);
					Display_16_8_data(4,96,&Record_show_mmHgkPa_data[0*16]);
					Display_16_8_data(4,96+8,&Record_show_mmHgkPa_data[1*16]);
					Display_16_8_data(4,96+16,&Record_show_mmHgkPa_data[2*16]);
					Display_16_8_data(4,96+24,&Record_show_mmHgkPa_data[3*16]);
			}
			else
			{
					clear_area(3,20,6,128);
					clear_area(6,20,8,55);
					if((measure_result_data[0]*256+measure_result_data[1])*133>9999)
						Display_num_size10(3,20,(measure_result_data[0]*256+measure_result_data[1])*133/10000%10);
					Display_num_size10(3,31,(measure_result_data[0]*256+measure_result_data[1])*133/1000%10);
					Display_16_8_data(4,42,&Record_show_mmHgkPa_data[7*16]);
					Display_num_size10(3,46,(measure_result_data[0]*256+measure_result_data[1])/100%10);		
					
					if(measure_result_data[2]*133>9999)
						Display_num_size10(3,68,(measure_result_data[2]*133)/10000%10);
					Display_num_size10(3,79,measure_result_data[2]*133/1000%10);
					Display_16_8_data(4,90,&Record_show_mmHgkPa_data[7*16]);
					Display_num_size10(3,94,measure_result_data[2]*133/100%10);	
								
					Display_num_size12_6(4,57,12);
					Display_16_8_data(4,105,&Record_show_mmHgkPa_data[4*16]);
					Display_16_8_data(4,105+8,&Record_show_mmHgkPa_data[5*16]);
					Display_16_8_data(4,105+16,&Record_show_mmHgkPa_data[6*16]);				
			}	
			if(measure_result_data[3]>99)
				Display_num_size10(6,20,measure_result_data[3]/100);	
			Display_num_size10(6,31,measure_result_data[3]/10%10);
			Display_num_size10(6,42,measure_result_data[3]%10);
		
			Save_Measure_Result_To_Flash(measure_time_data,measure_result_data);	
			}
			break;
		case State_Manual_Measure_error1:
		case State_Auto_Measure_error1:	
		{
			Display_full_screen(error1);
			Display_16_12_data(7,104,&Para_OK_Data[0]);
			Display_16_12_data(7,116,&Para_OK_Data[24]);
			Display_16_5_data(7,96,&Paraset_select_OK_data[0]);
		}
			break;
		case State_Manual_LowPower:
		case State_Auto_LowPower:
			Display_full_screen(error_5001);
			break;
		case State_Manual_pw1101_AutoGoing_error:
			Display_full_screen(error_4003);
			break;
		case State_Main_Menu_interact_error:
		case State_Manual_pw1101_interact_error:
		case State_Auto_pw1101_interact_error:
			Display_full_screen(error1001);
			break;
		
		case State_Auto_TempHigh:
		case State_Manual_TempHigh:
			Display_full_screen(Temp_High_Data);
			break;
		case State_Auto_TempLow:
		case State_Manual_TempLow:
			Display_full_screen(Temp_Low_Data);
			break;
		case State_Manual_Measure_False:
		case State_Auto_Measure_False:
			Display_full_screen(Page_error4_Data);//测量失败显示
			break;
		case State_Main_Menu_Auto:
			Display_full_screen(Page_auto_Data);
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
			break;
		case	State_Auto_Measure_IsTurnON:
		{	
			Display_auto_measure_IsTurnOn();
			break;
		}
		case	State_Auto_Measure_TurnOFF:
		{
			Display_full_screen(Page_Auto_Measure_TurnOff_Data);
			break;
		}
		case	State_Auto_Measure_IsTurnOFF:
		{
			Display_full_screen(Page_Auto_Measure_IsTurnOff_Data);
			break;
		}
		case	State_Auto_Measure_TurnON:
		{
			Display_full_screen(Page_Auto_Measure_TurnOn_Data);
			break;
		}
		case	State_Auto_DeviceCheck:
		{
			Display_full_screen(Page_devicecheck_Data);//
			break;
		}
		case State_Main_Menu_Record:
			Display_full_screen(Page_record_Data);
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
			break;	
		case State_Record_Display:
			Display_full_screen(Record_show_data);
			clear_area(1,1,2,120);
			break;
		
		case State_Main_Menu_Set:
			Display_full_screen(Page_set_Data);
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
			break;	
		case State_Set_error:
			Display_full_screen(error_4004);
			break;
		case	State_Set_Menu_GMT:
			Display_full_screen(Page_Set_GMT_Data);
			break;
		case State_Set_Menu_GMT_BackGround:
			Display_full_screen(Page_utc_Data);
			if(GMT_num>127) //负数
			{
				if(GMT_num*(-1)>9)
				{	
					Display_num_size10(3,10+55,12);
					Display_num_size10(3,10+70,GMT_num*(-1)/10);
					Display_num_size10(3,10+81,GMT_num*(-1)%10);
					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
				}
				else
				{
					Display_num_size10(3,10+55,12);
					Display_num_size10(3,10+70,GMT_num*(-1));
					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
				}
			}
			else
			{				
				if(GMT_num>9)
				{	
					Display_num_size10(3,10+55,13);
					Display_num_size10(3,10+70,GMT_num/10);
					Display_num_size10(3,10+81,GMT_num%10);
					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
				}
				else
				{
					Display_num_size10(3,10+55,13);
					Display_num_size10(3,10+70,GMT_num);
					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
				}
			}
			
			GMT_num_set=GMT_num;
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			
			g_CurrentStat=State_GMT_ADD;
			break;
		case  State_GMT_ADD:
		{
			Display_full_screen(Page_utc_Data);
			if(GMT_num>127) //负数
			{
				if(GMT_num*(-1)>9)
				{	

					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
				}
				else
				{

					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
				}
			}
			else
			{				
				if(GMT_num>9)
				{	

					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
				}
				else
				{

					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
				}
			}
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);		//清确认三角
			break;
		}
		case State_GMT_SUB:
		{	
			if(GMT_num_set>127) //负数
			{
				if(GMT_num_set*(-1)>9)
				{	
					Display_8_9_data(2,85,&Paraset_select_addsub_data[18]); //清上三角	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[9]); //画下三角
				}
				else
				{
					Display_8_9_data(2,80,&Paraset_select_addsub_data[18]); //画上三角	
					Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画上三角
				}
			}
			else
			{				
				if(GMT_num_set>9)
				{	
					Display_8_9_data(2,85,&Paraset_select_addsub_data[18]); //画上三角	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[9]); //画上三角
				}
				else
				{
					Display_8_9_data(2,80,&Paraset_select_addsub_data[18]); //画上三角	
					Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画上三角
				}
			}		
			break;
		}
		case State_GMT_OK:
		{
			if(GMT_num_set>127) //负数
			{
				if(GMT_num_set*(-1)>9)
				{	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[18]);  //清下三角	
				}
				else
				{
					Display_8_9_data(6,80,&Paraset_select_addsub_data[18]);  //清下三角	
				}
			}
			else
			{				
				if(GMT_num_set>9)
				{	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[18]);  //清下三角	
				}
				else
				{
					Display_8_9_data(6,80,&Paraset_select_addsub_data[18]); //清下三角	
				}
			}
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}			
		case	State_Set_Menu_HBlood:
			Display_full_screen(Page_Set_HBlood_Data);
			break;
		case State_Set_Menu_HBlood_BackGround:
		{	clear_screen();
			if(alarm.HBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm.HBlood_LowValue/100);
			if(alarm.HBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm.HBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm.HBlood_LowValue%10);
			Display_num_size10(3,10+45,10);
			if(alarm.HBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm.HBlood_HighValue/100);
			if(alarm.HBlood_LowValue>9)
				Display_num_size10(3,10+64,alarm.HBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm.HBlood_HighValue%10);

			
			alarm_set.HBlood_HighValue=alarm.HBlood_HighValue;
			alarm_set.HBlood_LowValue=alarm.HBlood_LowValue;
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_HBlood_Set_Low_ADD;
			break;
		}
		case State_HBlood_Set_Low_ADD:
		{	
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_HBlood_Set_Low_SUB:
			{
				Display_8_9_data(2,33,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,33,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_HBlood_Set_High_ADD:
			{
				Display_8_9_data(6,33,&Paraset_select_addsub_data[18]); //清下三角	
				Display_8_9_data(2,74,&Paraset_select_addsub_data[0]); //画上三角	
				break;
			}
		case State_HBlood_Set_High_SUB:
			{
				Display_8_9_data(2,74,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,74,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_HBlood_Set_OK:
			{
				Display_8_9_data(6,74,&Paraset_select_addsub_data[18]); //清下三角	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			}
			break;	
		case	State_Set_Menu_LBlood:
			Display_full_screen(Page_Set_LBlood_Data);
			break;
		case State_Set_Menu_LBlood_BackGround:
		{
			clear_screen();

			if(alarm.LBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm.LBlood_LowValue/100);
			if(alarm.LBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm.LBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm.LBlood_LowValue%10);
			Display_num_size10(3,10+45,10);
			if(alarm.LBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm.LBlood_HighValue/100);
			if(alarm.LBlood_HighValue>9)
				Display_num_size10(3,10+64,alarm.LBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm.LBlood_HighValue%10);

	
			alarm_set.LBlood_HighValue=alarm.LBlood_HighValue;
			alarm_set.LBlood_LowValue=alarm.LBlood_LowValue;
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角	
			
			g_CurrentStat=State_LBlood_Set_Low_ADD;
			break;
		}
		case State_LBlood_Set_Low_ADD:
		{
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_LBlood_Set_Low_SUB:
			{
				Display_8_9_data(2,33,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,33,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_LBlood_Set_High_ADD:
			{
				Display_8_9_data(6,33,&Paraset_select_addsub_data[18]); //清下三角	
				Display_8_9_data(2,74,&Paraset_select_addsub_data[0]); //画上三角	
				break;
			}

		case State_LBlood_Set_High_SUB:
			{
				Display_8_9_data(2,74,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,74,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_LBlood_Set_OK:
			{
				Display_8_9_data(6,74,&Paraset_select_addsub_data[18]); //清下三角	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			}
			break;
		case	State_Set_Menu_HeartRate:
			Display_full_screen(Page_Set_HeartRate_Data);
			break;
		case State_Set_Menu_HeartRate_BackGround:
		{
			clear_screen();
			if(alarm.HeartRate_LowValue>99)
				Display_num_size10(3,10+12,alarm.HeartRate_LowValue/100);
			if(alarm.HeartRate_LowValue>9)
				Display_num_size10(3,10+23,alarm.HeartRate_LowValue/10%10);
			Display_num_size10(3,10+34,alarm.HeartRate_LowValue%10);
			Display_num_size10(3,10+45,10);
			if(alarm.HeartRate_HighValue>99)
				Display_num_size10(3,10+53,alarm.HeartRate_HighValue/100);
			if(alarm.HeartRate_HighValue>9)
				Display_num_size10(3,10+64,alarm.HeartRate_HighValue/10%10);
			Display_num_size10(3,10+75,alarm.HeartRate_HighValue%10);

			alarm_set.HeartRate_HighValue=alarm.HeartRate_HighValue;
			alarm_set.HeartRate_LowValue=alarm.HeartRate_LowValue;
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角				
			g_CurrentStat=State_HeartRate_Set_Low_ADD;
			break;
		}
		case State_HeartRate_Set_Low_ADD:
		{		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,33,&Paraset_select_addsub_data[0]); //画上三角	
		}
			break;
		case State_HeartRate_Set_Low_SUB:
			{
				Display_8_9_data(2,33,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,33,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_HeartRate_Set_High_ADD:
			{
				Display_8_9_data(6,33,&Paraset_select_addsub_data[18]); //清下三角	
				Display_8_9_data(2,74,&Paraset_select_addsub_data[0]); //画上三角	
				break;
			}
		case State_HeartRate_Set_High_SUB:
			{
				Display_8_9_data(2,74,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,74,&Paraset_select_addsub_data[9]); //画下三角	
				break;
			}
		case State_HeartRate_Set_OK:
			{
				Display_8_9_data(6,74,&Paraset_select_addsub_data[18]); //清下三角	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			}
			break;
		
		case	State_Set_Menu_kPa:    //测量单位
			Display_full_screen(Page_Set_kPa_Data);
			break;
		case State_Select_mmHgkPa:    //选择测量单位
		{
			clear_screen();
			Display_16_8_data(2,51-5,&Page_mmHgkPa_Set_Data[0]);
			Display_16_8_data(2,60-5,&Page_mmHgkPa_Set_Data[16]);
			Display_16_8_data(2,69-5,&Page_mmHgkPa_Set_Data[32]);
			Display_16_8_data(2,78-5,&Page_mmHgkPa_Set_Data[48]);
		
			Display_16_8_data(5,51-5,&Page_mmHgkPa_Set_Data[64]);
			Display_16_8_data(5,60-5,&Page_mmHgkPa_Set_Data[80]);
			Display_16_8_data(5,69-5,&Page_mmHgkPa_Set_Data[96]);
			
			ParaSelect_Func(mmHg_or_kPa,1);
			ParaSelect_OK_Func(mmHg_or_kPa,1);
			mmHg_or_Kpa_Set=mmHg_or_kPa;
			break;
		}
		case	State_Set_Menu_MeasureMode:
			Display_full_screen(Page_Set_MeasureMode_Data);
			break;
		
		case State_Select_MeasureMode:
			{
				clear_screen();
				Display_16_12_data(1,33,&Page_MeasureMode_Set_Data12[0]);
				Display_16_12_data(1,46,&Page_MeasureMode_Set_Data12[24]);
				Display_16_12_data(1,59,&Page_MeasureMode_Set_Data12[240]);
				Display_16_12_data(1,72,&Page_MeasureMode_Set_Data12[264]);
				
				Display_16_12_data(3,33,&Page_MeasureMode_Set_Data12[48]);
				Display_16_12_data(3,46,&Page_MeasureMode_Set_Data12[72]);
				Display_16_12_data(3,59,&Page_MeasureMode_Set_Data12[240]);
				Display_16_12_data(3,72,&Page_MeasureMode_Set_Data12[264]);
				
				Display_16_12_data(5,20,&Page_MeasureMode_Set_Data12[96]);
				Display_16_12_data(5,33,&Page_MeasureMode_Set_Data12[120]);
				Display_16_12_data(5,46,&Page_MeasureMode_Set_Data12[144]);
				Display_16_12_data(5,59,&Page_MeasureMode_Set_Data12[168]);
				Display_16_12_data(5,72,&Page_MeasureMode_Set_Data12[240]);
				Display_16_12_data(5,85,&Page_MeasureMode_Set_Data12[264]);
				
				Display_16_12_data(7,20,&Page_MeasureMode_Set_Data12[192]);
				Display_16_12_data(7,33,&Page_MeasureMode_Set_Data12[216]);
				Display_16_12_data(7,46,&Page_MeasureMode_Set_Data12[144]);
				Display_16_12_data(7,59,&Page_MeasureMode_Set_Data12[168]);
				Display_16_12_data(7,72,&Page_MeasureMode_Set_Data12[240]);
				Display_16_12_data(7,85,&Page_MeasureMode_Set_Data12[264]);
				
				ParaSelect_Func(Measure_Mode,1);
				ParaSelect_OK_Func(Measure_Mode,1);
				Measure_Mode_Set=Measure_Mode;
			}
			break;
		
		case	State_Set_Menu_AutoMeasure:
			Display_full_screen(Page_Set_AutoMeasure_Data);
			break;
		case State_Set_Menu_AutoMeasure_BackGround:
		{
			clear_screen();
			Display_16_16_data(2,0,&Page_AutoMeasure_Set_Dtat[0]);
			Display_16_16_data(2,16,&Page_AutoMeasure_Set_Dtat[32]);
			Display_16_16_data(2,32,&Page_AutoMeasure_Set_Dtat[224]);
			Display_16_16_data(2,62,&Page_AutoMeasure_Set_Dtat[128]);
			Display_16_16_data(2,78,&Page_AutoMeasure_Set_Dtat[0]);
			if(AutoMeasurePara.Timelong>9)
			{	
				Display_num_size16_9(2,40,AutoMeasurePara.Timelong/10);
				Display_num_size16_9(2,51,AutoMeasurePara.Timelong%10);
			}
			else
			{
				Display_num_size16_9(2,45,AutoMeasurePara.Timelong);
			}
			AutoMeasurePara_Set.Timelong=AutoMeasurePara.Timelong;
			
			Display_16_16_data(5,0,&Page_AutoMeasure_Set_Dtat[64]);
			Display_16_16_data(5,16,&Page_AutoMeasure_Set_Dtat[96]);
			Display_16_16_data(5,32,&Page_AutoMeasure_Set_Dtat[224]);
			Display_16_16_data(5,62,&Page_AutoMeasure_Set_Dtat[160]);
			Display_16_16_data(5,78,&Page_AutoMeasure_Set_Dtat[192]);
			if(AutoMeasurePara.Timeinterval>9)
			{	
				Display_num_size16_9(5,40,AutoMeasurePara.Timeinterval/10);
				Display_num_size16_9(5,51,AutoMeasurePara.Timeinterval%10);
			}
			else
			{
				Display_num_size16_9(5,45,AutoMeasurePara.Timeinterval);
			}
			AutoMeasurePara_Set.Timeinterval=AutoMeasurePara.Timeinterval;
			
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(1,45,&Paraset_select_addsub_data[0]); //画上三角	
			
			g_CurrentStat=State_AutoMeasure_Timelong_Set_ADD;
			break;
		}
		case State_AutoMeasure_Timelong_Set_ADD:
		{	
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(1,45,&Paraset_select_addsub_data[0]); //画上三角	

			break;
		}
		case State_AutoMeasure_Timelong_Set_SUB:
		{
			Display_8_9_data(1,45,&Paraset_select_addsub_data[18]); 
			Display_8_9_data(4,45,&Paraset_select_addsub_data[9]); 
			break;
		}
		case State_AutoMeasure_Timeinterval_Set_ADD:
		{
			Display_8_9_data(4,45,&Paraset_select_addsub_data[0]); 
			break;
		}
		case State_AutoMeasure_Timeinterval_Set_SUB:
		{
			Display_8_9_data(4,45,&Paraset_select_addsub_data[18]);
			Display_8_9_data(7,45,&Paraset_select_addsub_data[9]); 			
			break;
		}
		case State_AutoMeasure_Set_OK:
		{
			Display_8_9_data(7,45,&Paraset_select_addsub_data[18]); 	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case	State_Set_Menu_MeasurePara:
			Display_full_screen(Page_Set_MeasurePara_Data);
			break;
		case State_MeasurePara_Groupnum:
			Display_full_screen(Page_MeasurePara_groupnum_Data);
			break;
		case State_MeasurePara_Groupnum_BackGround:
		{
			clear_screen();
			Display_24_20_data(3,20,&Page_MeasurePara_Set_Data[0]);
			Display_24_20_data(3,38,&Page_MeasurePara_Set_Data[60]);
			Display_24_20_data(3,56,&Page_MeasurePara_Set_Data[360]);
			if(MeasurePara.Groupnum>9)
			{	
				Display_num_size10(3,10+64,MeasurePara.Groupnum/10);
				Display_num_size10(3,10+75,MeasurePara.Groupnum%10);
			}
			else
			{
				Display_num_size10(3,79,MeasurePara.Groupnum%10);
			}

			MeasurePara_Set.Groupnum=MeasurePara.Groupnum;
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
	
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Groupnum_Set_ADD;
			break;
		}
		case State_Groupnum_Set_ADD:
		{	
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Groupnum_Set_SUB:
		{
			Display_8_9_data(2,80,&Paraset_select_addsub_data[18]); //清上三角	
			Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画下三角	
			break;
		}
		case State_Groupnum_Set_OK:
		{
			Display_8_9_data(6,80,&Paraset_select_addsub_data[18]); //清下三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case State_MeasurePare_Timelong:
			Display_full_screen(Page_MeasurePara_timelong_Data);
			break;
		case State_MeasurePare_Timelong_BackGround:
		{
			clear_screen();
			Display_24_20_data(3,20,&Page_MeasurePara_Set_Data[120]);
			Display_24_20_data(3,38,&Page_MeasurePara_Set_Data[180]);
			Display_24_20_data(3,56,&Page_MeasurePara_Set_Data[360]);
		
			Display_num_size10(3,79,MeasurePara.Timelong);

			MeasurePara_Set.Timelong=MeasurePara.Timelong;
		
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
	
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Timelong_Set_ADD;
			break;
		}
		case State_Timelong_Set_ADD:
		{				
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);	
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Timelong_Set_SUB:
		{
			Display_8_9_data(2,80,&Paraset_select_addsub_data[18]); //清上三角	
			Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画下三角	
			break;
		}
		case State_Timelong_Set_OK:
		{
			Display_8_9_data(6,80,&Paraset_select_addsub_data[18]); //清下三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case State_MeasurePara_Timeinterval:
			Display_full_screen(Page_MeasurePara_timeinterval_Data);
			break;
		case State_MeasurePara_Timeinterval_BackGround:
		{
			clear_screen();
			Display_24_20_data(3,20,&Page_MeasurePara_Set_Data[240]);
			Display_24_20_data(3,38,&Page_MeasurePara_Set_Data[300]);
			Display_24_20_data(3,56,&Page_MeasurePara_Set_Data[360]);
		
			Display_num_size10(3,79,MeasurePara.Timeinterval);

			MeasurePara_Set.Timeinterval=MeasurePara.Timeinterval;
		
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);	

			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Timeinterval_Set_ADD;
			break;
		}
		case State_Timeinterval_Set_ADD:
		{	
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Timeinterval_Set_SUB:
		{
			Display_8_9_data(2,80,&Paraset_select_addsub_data[18]); //清上三角	
			Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画下三角	
			break;
		}
		case State_Timeinterval_Set_OK:
		{
			Display_8_9_data(6,80,&Paraset_select_addsub_data[18]); //清下三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case State_MeasurePara_Eletrode:
			Display_full_screen(Page_MeasurePara_electrode_Data);
			break;	
		case State_Select_Eletrode:
			{
				clear_screen();
				Display_16_16_data(2,30,&Page_Eletrode_Set_Data[32]);
				Display_16_16_data(2,48,&Page_Eletrode_Set_Data[64]);
				Display_16_16_data(2,66,&Page_Eletrode_Set_Data[96]);
			
				Display_16_16_data(5,30,&Page_Eletrode_Set_Data[0]);
				Display_16_16_data(5,48,&Page_Eletrode_Set_Data[64]);
				Display_16_16_data(5,66,&Page_Eletrode_Set_Data[96]);
				
				ParaSelect_Func(MeasurePara.Eletrode,1);
				ParaSelect_OK_Func(MeasurePara.Eletrode,1);
				MeasurePara_Set.Eletrode=MeasurePara.Eletrode;
			}
			break;
		case State_Set_Menu_Sex:
			Display_full_screen(Page_Set_Sex_Data);
			break;
		case State_Select_Sex:
			{
				clear_screen();
				Display_16_12_data(7,6,&Page_Sex_Data[0]);	
				Display_16_12_data(7,18,&Page_Sex_Data[24]);	
//				Display_16_12_data(7,30,&Page_Sex_Data[48]);	

				Display_16_16_data(2,60,&Page_Sex_Set_Data[0]);	
				Display_16_16_data(5,60,&Page_Sex_Set_Data[32]);
				
				ParaSelect_Func(user.sex,1);
				ParaSelect_OK_Func(user.sex,1);
				user_set.sex=	user.sex;				
			}
			break;
		case State_Set_Menu_Age:
			{
				Display_full_screen(Page_Set_Age_Data);	
			}
			break;
		case State_Set_Menu_Age_BackGround:
		{
			clear_screen();
			Display_24_20_data(3,20,&Page_userparaset_Data[0]);	
			Display_24_20_data(3,40,&Page_userparaset_Data[60]);	
			Display_24_20_data(3,60,&Page_userparaset_Data[120]);	
			Display_num_size10(3,80,user.age/10);
			Display_num_size10(3,91,user.age%10);
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);
			user_set.age=user.age;
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Age_Set_Para_ADD;
			break;
		}
		case State_Age_Set_Para_ADD:
			{
				Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
				Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
			}
			break;
		case State_Age_Set_Para_SUB:
			{
				Display_8_9_data(2,85,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,85,&Paraset_select_addsub_data[9]);		
			}
			break;
		case State_Age_Set_OK:
			{
				Display_8_9_data(6,85,&Paraset_select_addsub_data[18]); //清下三角	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			}
			break;
		case State_Set_Menu_Height:
			{
				Display_full_screen(Page_Set_Height_Data);
			}			
			break;
		case State_Set_Menu_Height_BackGround:
		{
			clear_screen();
			Display_24_20_data(3,20+0,&Page_userparaset_Data[180]);	
			Display_24_20_data(3,20+20,&Page_userparaset_Data[240]);	
			Display_24_20_data(3,20+40,&Page_userparaset_Data[120]);
			if(user.height>99)
				Display_num_size10(3,80,user.height/100);
			Display_num_size10(3,91,user.height/10%10);
			Display_num_size10(3,102,user.height%10);
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);
			
			user_set.height=user.height;
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,91,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Height_Set_Para_ADD;
			break;
		}
		case State_Height_Set_Para_ADD:
			{
				Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
				Display_8_9_data(2,91,&Paraset_select_addsub_data[0]); //画上三角	
			}			
			break;
		case State_Height_Set_Para_SUB:
			{
				Display_8_9_data(2,91,&Paraset_select_addsub_data[18]);	
				Display_8_9_data(6,91,&Paraset_select_addsub_data[9]);		
			}
			break;
		case State_Height_Set_OK:
			{
				Display_8_9_data(6,91,&Paraset_select_addsub_data[18]);	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);	
			}			
			break;			
		case State_Set_Menu_Weight:
			{
				Display_full_screen(Page_Set_Weight_Data);			
			}			
			break;
		case State_Set_Menu_Weight_Back_Ground:
		{
			clear_screen();
			Display_24_20_data(3,20+0,&Page_userparaset_Data[300]);	
			Display_24_20_data(3,20+20,&Page_userparaset_Data[360]);	
			Display_24_20_data(3,20+40,&Page_userparaset_Data[120]);	
			if(user.weight>99)
				Display_num_size10(3,80,user.weight/100);
			Display_num_size10(3,91,user.weight/10%10);
			Display_num_size10(3,102,user.weight%10);
			
			user_set.weight=user.weight;
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,90,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Weight_Set_Para_ADD;
			break;
		}
		case State_Weight_Set_Para_ADD:
			{
				Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
				Display_8_9_data(2,90,&Paraset_select_addsub_data[0]); //画上三角	
			}
			break;
		case State_Weight_Set_Para_SUB:
			{
				Display_8_9_data(2,90,&Paraset_select_addsub_data[18]); //清上三角	
				Display_8_9_data(6,90,&Paraset_select_addsub_data[9]);		
			}
			break;
		case State_Weight_Set_OK:
			{
				Display_8_9_data(6,90,&Paraset_select_addsub_data[18]); //清下三角	
				Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			}
			break;		
		case State_Set_Menu_Date:
		{
			Display_full_screen(Page_Set_Date_Data);
			break;
		}
		case State_Set_Menu_Date_BackGround:
		{
			clear_screen();
			RTC_GetDatetime(RTC, &date_set);
			Display_num_size10(3,10+1,date_set.year/1000);
			Display_num_size10(3,10+12,date_set.year/100%10);
			Display_num_size10(3,10+23,date_set.year/10%10);
			Display_num_size10(3,10+34,date_set.year%10);
			Display_num_size10(3,10+45,10);
			Display_num_size10(3,10+53,date_set.month/10);
			Display_num_size10(3,10+64,date_set.month%10);
			Display_num_size10(3,10+75,10);
			Display_num_size10(3,10+83,date_set.day/10);
			Display_num_size10(3,10+94,date_set.day%10);
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,28,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_Date_Set_Year_ADD;
			break;
		}
		case State_Date_Set_Year_ADD:
		{
				Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
				Display_8_9_data(2,28,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Date_Set_Year_SUB:
		{
			Display_8_9_data(2,28,&Paraset_select_addsub_data[18]); //清上三角	
			Display_8_9_data(6,28,&Paraset_select_addsub_data[9]); //画下三角	
			break;
		}
		case State_Date_Set_Month_ADD:
		{
			Display_8_9_data(6,28,&Paraset_select_addsub_data[18]); //
			Display_8_9_data(2,70,&Paraset_select_addsub_data[0]); //
			break;
		}
		case State_Date_Set_Month_SUB:
		{
			Display_8_9_data(2,70,&Paraset_select_addsub_data[18]); //
			Display_8_9_data(6,70,&Paraset_select_addsub_data[9]); //画下
			break;
		}
		case State_Date_Set_Day_ADD:
		{
			Display_8_9_data(6,70,&Paraset_select_addsub_data[18]); //画下
			Display_8_9_data(2,100,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Date_Set_Day_SUB:
		{
			Display_8_9_data(2,100,&Paraset_select_addsub_data[18]); //画上三角	
			Display_8_9_data(6,100,&Paraset_select_addsub_data[9]); //画上三角	
			break;
		}
		case State_Date_Set_OK:
		{
			Display_8_9_data(6,100,&Paraset_select_addsub_data[18]); //画上三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case State_Set_Menu_Time:
		{
			Display_full_screen(Page_Set_Time_Data);
			break;
		}
		case State_Set_Menu_Time_BackGround:
		{
			clear_screen();
			RTC_GetDatetime(RTC, &date_set);
			Display_num_size10(3,35,date_set.hour/10);
			Display_num_size10(3,46,date_set.hour%10);
			Display_num_size10(3,57,11);
			Display_num_size10(3,65,date_set.minute/10);
			Display_num_size10(3,76,date_set.minute%10);
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
			Display_8_9_data(2,42,&Paraset_select_addsub_data[0]); //画上三角			
			g_CurrentStat=State_Time_Hour_ADD;
			break;
		}
		case State_Time_Hour_ADD:
		{
				Display_16_5_data(7,90,&Paraset_select_OK_data[10]);
				Display_8_9_data(2,42,&Paraset_select_addsub_data[0]); //画上三角			
			break;
		}
		case State_Time_Hour_SUB:
		{
			Display_8_9_data(2,42,&Paraset_select_addsub_data[18]); //画上三角	
			Display_8_9_data(6,42,&Paraset_select_addsub_data[9]); //画上三角	
			break;
		}
		case State_Time_Minute_ADD:
		{
			Display_8_9_data(6,42,&Paraset_select_addsub_data[18]); //画上三角	
			Display_8_9_data(2,72,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_Time_Minute_SUB:
		{
			Display_8_9_data(2,72,&Paraset_select_addsub_data[18]); //画上三角	
			Display_8_9_data(6,72,&Paraset_select_addsub_data[9]); //画上三角	
			break;
		}
		case State_Time_Set_OK:
		{
			Display_8_9_data(6,72,&Paraset_select_addsub_data[18]); //画上三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}

		case State_Set_Menu_Ble:     //设置蓝牙
			Display_full_screen(Page_Set_Ble_Data);
			break;
		
		case State_Select_Ble_OnOff:
			{
				clear_screen();
				Display_16_16_data(2,25,&Page_Ble_Set_Date[0]);	
				Display_16_16_data(2,43,&Page_Ble_Set_Date[32]);	
				Display_16_16_data(2,61,&Page_Ble_Set_Date[128]);	
				Display_16_16_data(2,79,&Page_Ble_Set_Date[160]);	
				
				Display_16_16_data(5,25,&Page_Ble_Set_Date[64]);
				Display_16_16_data(5,43,&Page_Ble_Set_Date[96]);
				Display_16_16_data(5,61,&Page_Ble_Set_Date[128]);
				Display_16_16_data(5,79,&Page_Ble_Set_Date[160]);
				
				ParaSelect_Func(Ble_OnOff,1);
				ParaSelect_OK_Func(Ble_OnOff,1);
				Ble_OnOff_Set=Ble_OnOff;		
			}
			break;
		case State_Set_Menu_NoDisturb:    //防打扰
			Display_full_screen(Page_Set_NoDisturb_Data);
			break;
		case State_Select_NoDisturb_Mode:  //防打拢设置
			{		
				clear_screen();
				Display_16_16_data(2,38,&Page_Nodisturb_Set_Data[0]);	
				Display_16_16_data(2,56,&Page_Nodisturb_Set_Data[32]);	
			
				Display_16_16_data(5,30,&Page_Nodisturb_Set_Data[64]);
				Display_16_16_data(5,48,&Page_Nodisturb_Set_Data[96]);
				Display_16_16_data(5,66,&Page_Nodisturb_Set_Data[128]);
				
				ParaSelect_Func(NoDisturb_Mode,1);
				ParaSelect_OK_Func(NoDisturb_Mode,1);
				NoDisturb_Mode_Set=NoDisturb_Mode;	
			}			
			break;
		case State_Set_Menu_StandbyTime:  //待机时间
			Display_full_screen(Page_Set_StandbyTime_Data);
			break;
		case State_Set_Menu_StandbyTime_BackGround:			
		{
			clear_screen();
			
			Display_16_12_data(7,1,	&Standbytime_Data[0]);
			Display_16_12_data(7,14,&Standbytime_Data[24]);
			Display_16_12_data(7,27,&Standbytime_Data[48]);
			Display_16_12_data(7,40,&Standbytime_Data[0]);
			Display_16_12_data(7,53,&Standbytime_Data[72]);
			Display_16_12_data(7,66,&Standbytime_Data[96]);
			Display_24_20_data(3,75,&Page_MeasurePara_Set_Data[420]);  //秒
			if(StandbyTime>9)
			{	
				clear_area(3,50,5,71);
				Display_num_size10(3,50,StandbyTime/10);
				Display_num_size10(3,61,StandbyTime%10);
			}
			else
			{
				clear_area(3,50,5,71);
				Display_num_size10(3,56,StandbyTime);
			}
			StandbyTime_Set=StandbyTime;
			
			
			Display_16_12_data(7,100,&Para_OK_Data[0]);
			Display_16_12_data(7,112,&Para_OK_Data[24]);		
	
			Display_8_9_data(2,56,&Paraset_select_addsub_data[0]); //画上三角	
			g_CurrentStat=State_StandbyTime_Set_ADD;
			break;
		}
		case State_StandbyTime_Set_ADD:   //待机时间增加
		{
			Display_16_5_data(7,90,&Paraset_select_OK_data[10]);		
			Display_8_9_data(2,56,&Paraset_select_addsub_data[0]); //画上三角	
			break;
		}
		case State_StandbyTime_Set_SUB:
		{
			Display_8_9_data(2,56,&Paraset_select_addsub_data[18]); //画上三角	
			Display_8_9_data(6,56,&Paraset_select_addsub_data[9]); //画上三角	
			break;
		}
		case State_StandbyTime_Set_OK:
		{
			Display_8_9_data(6,56,&Paraset_select_addsub_data[18]); //画上三角	
			Display_16_5_data(7,90,&Paraset_select_OK_data[0]);
			break;
		}
		case State_Set_Menu_DeviceInfo:  //设置页中，显示的设备信息
			Display_full_screen(Page_Set_DeviceInfo_Data);
			break;	
	  case State_Software_Info:        //显示开机的画面
			Display_full_screen(Page_fb_Data);
			break;
		case State_DeviceInfo_Version:  //显示读取的脉感的版本信息
			Display_Version_Info();
			break;
		case State_Device_ID:  //显示设备ID
			Display_Device_ID();
			break;
		case State_PW1101IC_ID:  //显示脉感ID
			Display_PW1101IC_ID();
			break;
		case State_Senser_ID:   //显示传感器ID 
			break;
		case State_Set_Menu_ReFactory:  //恢复出厂设置选项
			Display_full_screen(Page_Refactory_Set_Data);
			break;
		case State_ReFactory_Background: //恢复出厂设置背景
			Display_full_screen(Page_Refactory_Data);
			g_CurrentStat=State_ReFactory_OK;
			Display_16_5_data(5,19,&Paraset_select_OK_data[20]);
			break;
		case State_ReFactory_OK:  //选择确认恢复出厂设置
			Display_16_5_data(5,69,&Paraset_select_OK_data[10]);
			Display_16_5_data(5,19,&Paraset_select_OK_data[20]);
			break;
		case State_ReFactory_ESC: //恢复出厂设置取消
			Display_16_5_data(5,19,&Paraset_select_OK_data[10]);
			Display_16_5_data(5,69,&Paraset_select_OK_data[20]);
			break;
		default:
			break;
	}
	return 1;
}



//当前界面为选择参数界面，返回真
//例：设置性别时为选择性别
uint8_t Is_ParaSelect_States()
{
	switch(g_CurrentStat)
	{
		case State_Select_Eletrode:
		case State_Select_Sex:
		case State_Select_mmHgkPa:
		case State_Select_Ble_OnOff:
		case State_Select_NoDisturb_Mode:
		case State_Select_MeasureMode:
			return 1;
		default:
			break;
	}
	return 0;
}



//进行当前选择指示 参数1表示操作项，参数2表示选择或者取消选择
//参数有效性：参数2 为0时表示取消选择  为1时，表示选择
void ParaSelect_OK_Func(uint8_t para1,uint8_t para2)
{
	switch(g_CurrentStat)
	{
		case State_Select_Eletrode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,90,3,106);  
				}
				else
				{ 
					Display_16_16_data(2,90,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,90,6,106);  	
				}
				else
				{ 
					Display_16_16_data(5,90,& ParaSelect_OK_Data[0]);
				}
			}			
			break;
		}
		case State_Select_Sex:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,84,3,100);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,84,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,84,6,100); 
				}
				else
				{ 
					Display_16_16_data(5,84,& ParaSelect_OK_Data[0]);
				}
			}			
			break;
		}
		
		case State_Select_mmHgkPa:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,90,3,106);  
				}
				else
				{ 
					Display_16_16_data(2,90,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,90,6,106);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,90,& ParaSelect_OK_Data[0]);
				}
			}			
			break;
		}
		case State_Select_Ble_OnOff:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,103,3,119);  
				}
				else
				{ 
					Display_16_16_data(2,103,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,103,6,119);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,103,& ParaSelect_OK_Data[0]);
				}
			}			
			break;
		}
		case State_Select_NoDisturb_Mode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,90,3,116);  
				}
				else
				{ 
					Display_16_16_data(2,90,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,90,6,116);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,90,& ParaSelect_OK_Data[0]);
				}
			}			
			break;
		}
		case State_Select_MeasureMode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(1,102,2,118);  
				}
				else
				{ 
					Display_16_16_data(1,102,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(3,102,4,118);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(3,102,& ParaSelect_OK_Data[0]);
				}
			}	
			else if(para1==3)
			{
				if(para2==0)
				{
					clear_area(5,102,6,118);  
				}
				else
				{ 
					Display_16_16_data(5,102,& ParaSelect_OK_Data[0]);
				}
			}
			else if(para1==4)
			{
				if(para2==0)
				{
					clear_area(7,102,8,118);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(7,102,& ParaSelect_OK_Data[0]);
				}
			}					
			break;
		}
		default:
				break;
	}
}


//进行当前选择指示 参数1表示操作项，参数2表示选择或者取消选择
//参数有效性：参数2 为0时表示取消选择  为1时，表示选择
void ParaSelect_Func(uint8_t para1,uint8_t para2)
{
	switch(g_CurrentStat)
	{
		case State_Select_Eletrode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,14,3,29);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,14,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,14,6,29);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,14,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}			
			break;
		}
		case State_Select_Sex:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,44,3,59);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,44,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,44,6,59);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,44,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}			
			break;
		}
		
		case State_Select_mmHgkPa:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,30,3,45);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,30,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,30,6,45);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,30,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}			
			break;
		}
		case State_Select_Ble_OnOff:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,9,3,24);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,9,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,9,6,24);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,9,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}			
			break;
		}
		case State_Select_NoDisturb_Mode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(2,14,3,29);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(2,14,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(5,14,6,29);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,14,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}			
			break;
		}
		case State_Select_MeasureMode:
		{
			if(para1==1)
			{
				if(para2==0)
				{
					clear_area(1,4,2,19);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(1,4,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==2)
			{
				if(para2==0)
				{
					clear_area(3,4,4,19);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(3,4,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}	
			else if(para1==3)
			{
				if(para2==0)
				{
					clear_area(5,4,6,19);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(5,4,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}
			else if(para1==4)
			{
				if(para2==0)
				{
					clear_area(7,4,8,19);  //画右三角				
				}
				else
				{ 
					Display_16_16_data(7,4,&	ParaSelect_OK_Data[32]);  //画右三角	
				}
			}				
			break;
		}
		default:
				break;
	}
}

//当选择完后，进行参数的保存工作
void Deal_ParaSelect_OK_Func(void)
{
	switch(g_CurrentStat)
	{
		case State_Select_Eletrode:
			{
				if(MeasurePara.Eletrode!=MeasurePara_Set.Eletrode)
				{
					ParaSelect_OK_Func(MeasurePara.Eletrode,0);
					ParaSelect_OK_Func(MeasurePara_Set.Eletrode,1);
					//进行存储		
					MeasurePara.Eletrode=MeasurePara_Set.Eletrode;
					user_set_para[SET_PARA_MEASURE_ELETRODE_ADD]=MeasurePara.Eletrode;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		case State_Select_Sex:
			{
					if(user.sex!=user_set.sex)
				{
					ParaSelect_OK_Func(user.sex,0);
					ParaSelect_OK_Func(user_set.sex,1);
					//进行存储
					
					user.sex=user_set.sex;
					user_set_para[SET_PARA_SEX_ADD]=user.sex;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		case State_Select_mmHgkPa:
			{
					if(mmHg_or_kPa!=mmHg_or_Kpa_Set)
				{
					ParaSelect_OK_Func(mmHg_or_kPa,0);
					ParaSelect_OK_Func(mmHg_or_Kpa_Set,1);
					//进行存储
					
					mmHg_or_kPa=mmHg_or_Kpa_Set;
					user_set_para[SET_PARA_mmgH_kPa_ADD]=mmHg_or_kPa;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		case State_Select_Ble_OnOff:
			{
					if(Ble_OnOff!=Ble_OnOff_Set)
				{
					ParaSelect_OK_Func(Ble_OnOff,0);
					ParaSelect_OK_Func(Ble_OnOff_Set,1);
					//进行存储
					
					Ble_OnOff=Ble_OnOff_Set;
					if(Ble_OnOff==1) //如果蓝牙为打开状态
					{
						GPIO_ClearPinsOutput(GPIOA, 1u << 5);		
					}
					else
					{
						GPIO_SetPinsOutput(GPIOA, 1u << 5);		
					}
					user_set_para[SET_PARA_BLE_OnOff_ADD]=Ble_OnOff;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		case State_Select_NoDisturb_Mode:
			{
					if(NoDisturb_Mode!=NoDisturb_Mode_Set)
				{
					ParaSelect_OK_Func(NoDisturb_Mode,0);
					ParaSelect_OK_Func(NoDisturb_Mode_Set,1);
					//进行存储
					
					NoDisturb_Mode=NoDisturb_Mode_Set;
					user_set_para[SET_PARA_NoDisturb_Mode_ADD]=NoDisturb_Mode;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		case State_Select_MeasureMode:
			{
				if(Measure_Mode!=Measure_Mode_Set)
				{
					ParaSelect_OK_Func(Measure_Mode,0);
					ParaSelect_OK_Func(Measure_Mode_Set,1);
					//进行存储					
					Measure_Mode=Measure_Mode_Set;
					user_set_para[SET_PARA_MEASURE_MODE_ADD]=Measure_Mode;
					Sava_para_to_flash();
					Delay_S(1);
				}
			}
			break;
		default:
			break;
	}
}
//当进行了选择后，要进行参数赋值 
void Deal_ParaSelect_Func(void)
{
	switch(g_CurrentStat)
	{
		case State_Select_Eletrode:
			ParaSelect_Func(MeasurePara_Set.Eletrode,0);
			MeasurePara_Set.Eletrode=3-MeasurePara_Set.Eletrode;   //让设置电极在干湿之间切换    干电极 2   湿电极（默认）1
			//做指向切换
			ParaSelect_Func(MeasurePara_Set.Eletrode,1);
			break;
		case State_Select_Sex:
			ParaSelect_Func(user_set.sex,0);
			user_set.sex=3-user_set.sex;
			ParaSelect_Func(user_set.sex,1);
			break;
		case State_Select_mmHgkPa:
			ParaSelect_Func(mmHg_or_Kpa_Set,0);
			mmHg_or_Kpa_Set=3-mmHg_or_Kpa_Set;
			ParaSelect_Func(mmHg_or_Kpa_Set,1);
			break;
		case State_Select_Ble_OnOff:
			ParaSelect_Func(Ble_OnOff_Set,0);
			Ble_OnOff_Set=3-Ble_OnOff_Set;
			ParaSelect_Func(Ble_OnOff_Set,1);
			break;
		case State_Select_NoDisturb_Mode:
			ParaSelect_Func(NoDisturb_Mode_Set,0);
			NoDisturb_Mode_Set=3-NoDisturb_Mode_Set;
			ParaSelect_Func(NoDisturb_Mode_Set,1);
			break;
		case State_Select_MeasureMode:
			ParaSelect_Func(Measure_Mode_Set,0);
			if(Measure_Mode_Set<4) Measure_Mode_Set++;
			else Measure_Mode_Set=1;
			ParaSelect_Func(Measure_Mode_Set,1);
			break;
		default:
			break;
	}
}



//如果当前为设置参数状态
//如设置年龄等，可以变化的值
uint8_t Is_ParaSet_States()
{
	switch(g_CurrentStat)
	{
		case State_Age_Set_Para_ADD:
		case State_Age_Set_Para_SUB:
		case State_Age_Set_OK:
			
		case State_Height_Set_Para_ADD:
		case State_Height_Set_Para_SUB:
		case State_Height_Set_OK:
			
		case State_Weight_Set_Para_ADD:
		case State_Weight_Set_Para_SUB:
		case State_Weight_Set_OK:
			
		case State_Date_Set_Year_ADD:
		case State_Date_Set_Year_SUB:
		case State_Date_Set_Month_ADD:
		case State_Date_Set_Month_SUB:
		case State_Date_Set_Day_ADD:
		case State_Date_Set_Day_SUB:
		case State_Time_Hour_ADD:
		case State_Time_Hour_SUB:
		case State_Time_Minute_ADD:
		case State_Time_Minute_SUB:
		case State_Date_Set_OK:
		case State_Time_Set_OK:
		case State_GMT_ADD:
		case State_GMT_SUB:
		case State_GMT_OK:
			
		case State_HBlood_Set_High_ADD:
		case State_HBlood_Set_High_SUB:
		case State_HBlood_Set_Low_ADD:
		case State_HBlood_Set_Low_SUB:
		case State_HBlood_Set_OK:			
		
		case State_LBlood_Set_High_ADD:
		case State_LBlood_Set_High_SUB:
		case State_LBlood_Set_Low_ADD:
		case State_LBlood_Set_Low_SUB:
		case State_LBlood_Set_OK:
			
		case State_HeartRate_Set_High_ADD:
		case State_HeartRate_Set_High_SUB:
		case State_HeartRate_Set_Low_ADD:
		case State_HeartRate_Set_Low_SUB:
		case State_HeartRate_Set_OK:
			
		case State_Groupnum_Set_ADD:
		case State_Groupnum_Set_SUB:
		case State_Groupnum_Set_OK:
			
		case State_Timelong_Set_ADD:
		case State_Timelong_Set_SUB:
		case State_Timelong_Set_OK:
			
		case State_Timeinterval_Set_ADD:
		case State_Timeinterval_Set_SUB:
		case State_Timeinterval_Set_OK:
			
		case State_AutoMeasure_Timeinterval_Set_ADD:
		case State_AutoMeasure_Timeinterval_Set_SUB:
		case State_AutoMeasure_Timelong_Set_ADD:
		case State_AutoMeasure_Timelong_Set_SUB:
		case State_AutoMeasure_Set_OK:
		
		case State_StandbyTime_Set_ADD:
		case State_StandbyTime_Set_SUB:
		case State_StandbyTime_Set_OK:
			
		case State_ReFactory_OK:

			return 1;
		default:
			break;
	}
	return 0;
}

void Deal_ParaSet_Func(void)
{
	switch(g_CurrentStat)
	{
		case State_Age_Set_Para_ADD:
		{
			user_set.age++;
			if (user_set.age>99)user_set.age=18;
			Display_num_size10(3,80,user_set.age/10);
			Display_num_size10(3,91,user_set.age%10);
			break;
		}
		case State_Age_Set_Para_SUB:
		{	
			user_set.age--;
			if (user_set.age<18)user_set.age=99;
			Display_num_size10(3,80,user_set.age/10);
			Display_num_size10(3,91,user_set.age%10);
			break;
		}
		case State_Age_Set_OK:
		{
			if(user.age!=user_set.age)
				{
					//进行存储			
					user.age=user_set.age;
					user_set_para[SET_PARA_AGE_ADD]=user.age;
					Sava_para_to_flash();	
					Delay_S(1);
				}
			break;	
		}
		case State_Height_Set_Para_ADD:
		{	
			user_set.height++;
			if (user_set.height>200)user_set.height=70;
			if(user_set.height>99)
				Display_num_size10(3,80,user_set.height/100);
			else
				clear_area(3,80,5,90);
			Display_num_size10(3,91,user_set.height/10%10);
			Display_num_size10(3,102,user_set.height%10);
			break;
		}
		case State_Height_Set_Para_SUB:
		{	
			user_set.height--;
			if (user_set.height<70)user_set.height=200;
			if(user_set.height>99)
				Display_num_size10(3,80,user_set.height/100);
			else
				clear_area(3,80,5,90);
			Display_num_size10(3,91,user_set.height/10%10);
			Display_num_size10(3,102,user_set.height%10);
			break;
		}
		case State_Height_Set_OK:
		{
			if(user.height!=user_set.height)
				{
					//进行存储	
					user.height=user_set.height;
					user_set_para[SET_PARA_HEIGHT_ADD]=user.height;
					Sava_para_to_flash();	
					Delay_S(1);
				}
			break;	
		}
		case State_Weight_Set_Para_ADD:
		{
			user_set.weight++;
			if (user_set.weight>120)user_set.weight=20;
			if(user_set.weight>99)
				Display_num_size10(3,80,user_set.weight/100);
			else
				clear_area(3,80,5,90);
			Display_num_size10(3,91,user_set.weight/10%10);
			Display_num_size10(3,102,user_set.weight%10);
			break;
		}
		case State_Weight_Set_Para_SUB:
		{
			user_set.weight--;
			if (user.weight<20)user_set.weight=120;
			if(user_set.weight>99)
				Display_num_size10(3,80,user_set.weight/100);
			else
				clear_area(3,80,5,90);
			Display_num_size10(3,91,user_set.weight/10%10);
			Display_num_size10(3,102,user_set.weight%10);
			break;	
		}	
		case State_Weight_Set_OK:
		{
			if(user.weight!=user_set.weight)
				{
					//进行存储
					user.weight=user_set.weight;
					user_set_para[SET_PARA_WEIGHT_ADD]=user.weight;
					Sava_para_to_flash();					
					Delay_S(1);
				}
			break;	
		}		
		case State_Date_Set_Year_ADD:
		{
				if((++date_set.year)>2030)  date_set.year=2030;
				Display_num_size10(3,10+1,date_set.year/1000);
				Display_num_size10(3,10+12,date_set.year/100%10);
				Display_num_size10(3,10+23,date_set.year/10%10);
				Display_num_size10(3,10+34,date_set.year%10);
			break;
		}
		case State_Date_Set_Year_SUB:
		{
			if((--date_set.year)<1970) date_set.year=1970;
				Display_num_size10(3,10+1,date_set.year/1000);
				Display_num_size10(3,10+12,date_set.year/100%10);
				Display_num_size10(3,10+23,date_set.year/10%10);
				Display_num_size10(3,10+34,date_set.year%10);
			break;
		}
		case State_Date_Set_Month_ADD:
		{
			if((++date_set.month)>12) date_set.month=12;
				Display_num_size10(3,10+53,date_set.month/10);
				Display_num_size10(3,10+64,date_set.month%10);
			break;
		}
		case State_Date_Set_Month_SUB:
		{
			if((--date_set.month)<1) date_set.month=1;
			Display_num_size10(3,10+53,date_set.month/10);
				Display_num_size10(3,10+64,date_set.month%10);
			break;
		}
		case State_Date_Set_Day_ADD:
			{
				if((date_set.month==1)||(date_set.month==3)||(date_set.month==5)||(date_set.month==7)||(date_set.month==8)
						||(date_set.month==10)||(date_set.month==12))
					{
						if((++date_set.day)>31) date_set.day=31;
					}
					else if(date_set.month==2)
					{
						 if((date_set.year%4==0 && date_set.year%100 !=0)||(date_set.year%400==0))
						 {
							 if((++date_set.day)>29) date_set.day=29;
						 }
						 else
						 {
							 if((++date_set.day)>28) date_set.day=28;
						 }
					}
					else
					{
						if((++date_set.day)>30) date_set.day=30;
					}

					Display_num_size10(3,10+83,date_set.day/10);
					Display_num_size10(3,10+94,date_set.day%10);
						
				break;
			}	
		case State_Date_Set_Day_SUB:
		{	
			if((--date_set.day)<1) date_set.day=1;
				Display_num_size10(3,10+83,date_set.day/10);
				Display_num_size10(3,10+94,date_set.day%10);	
		
			break;
		}
		case State_Time_Hour_ADD:
		{
			if((++date_set.hour)>23) date_set.hour=0;
				Display_num_size10(3,35,date_set.hour/10);
				Display_num_size10(3,46,date_set.hour%10);
			break;
		}
		case State_Time_Hour_SUB:
		{
			if((date_set.hour)>0) date_set.hour--;
			else date_set.hour=23;
				Display_num_size10(3,35,date_set.hour/10);
				Display_num_size10(3,46,date_set.hour%10);
			break;
		}
		case State_Time_Minute_ADD:
		{
			if((++date_set.minute)>59) date_set.minute=0;
			Display_num_size10(3,65,date_set.minute/10);
				Display_num_size10(3,76,date_set.minute%10);
			break;
		}
		case State_Time_Minute_SUB:
		{	
			if((date_set.minute)>0) date_set.minute--;
			else date_set.minute=59;
			Display_num_size10(3,65,date_set.minute/10);
				Display_num_size10(3,76,date_set.minute%10);
			break;	
		}
		
		
		case State_Date_Set_OK:
		case State_Time_Set_OK:
		{
			User_Set_time();
			break;
		}
		
		
		case State_GMT_ADD:
		{	
			clear_area(2,64,5,102);
			GMT_num_set=GMT_num_set+1;
			if(GMT_num_set<127 && GMT_num_set>14)
			{
				GMT_num_set=-12;
			}
			if(GMT_num_set>127) //负数
			{
				Display_num_size10(3,10+55,12);
				
				if(GMT_num_set<247)
				{	
					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
					Display_num_size10(3,10+70,(256-GMT_num_set)/10);
					Display_num_size10(3,10+81,(256-GMT_num_set)%10);
				}
				else
				{
					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
					Display_num_size10(3,10+70,(256-GMT_num_set));
				}
			}
			else
			{		
				Display_num_size10(3,10+55,13);		
				if(GMT_num_set>9)
				{	
					Display_8_9_data(2,85,&Paraset_select_addsub_data[0]); //画上三角	
					Display_num_size10(3,10+70,GMT_num_set/10);
					Display_num_size10(3,10+81,GMT_num_set%10);
				}
				else
				{
					Display_8_9_data(2,80,&Paraset_select_addsub_data[0]); //画上三角	
					Display_num_size10(3,10+70,GMT_num_set);
				}
			}
			
			break;
		}
		case State_GMT_SUB:
		{
			clear_area(3,64,6,102);
			GMT_num_set=GMT_num_set-1;
			if(GMT_num_set>127 && GMT_num_set<244)
			{
				GMT_num_set=14;
			}
			if(GMT_num_set>127) //负数
			{
				Display_num_size10(3,10+55,12);
				
				if(GMT_num_set<247)
				{	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[9]); //画下三角	
					Display_num_size10(3,10+70,(256-GMT_num_set)/10);
					Display_num_size10(3,10+81,(256-GMT_num_set)%10);
				}
				else
				{
					Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画下三角	
					Display_num_size10(3,10+70,(256-GMT_num_set));
				}
			}
			else
			{		
				Display_num_size10(3,10+55,13);		
				if(GMT_num_set>9)
				{	
					Display_8_9_data(6,85,&Paraset_select_addsub_data[9]); //画下三角	
					Display_num_size10(3,10+70,GMT_num_set/10);
					Display_num_size10(3,10+81,GMT_num_set%10);
				}
				else
				{
					Display_8_9_data(6,80,&Paraset_select_addsub_data[9]); //画下三角	
					Display_num_size10(3,10+70,GMT_num_set);
				}
			}
			break;
		}
		case State_GMT_OK:
		{
			User_Set_UTC();
			GMT_num=GMT_num_set;
			user_set_para[SET_PARA_GMT_NUM_ADD]=GMT_num;
			Sava_para_to_flash();
			break;
		}
		case State_HBlood_Set_High_ADD:
		{
			if((++alarm_set.HBlood_HighValue)>300) alarm_set.HBlood_HighValue=20;	
			clear_area(3,63,5,95);
			if(alarm_set.HBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.HBlood_HighValue/100);
			if(alarm_set.HBlood_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.HBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.HBlood_HighValue%10);
			break;
		}
		case State_HBlood_Set_High_SUB:
		{
			if((--alarm_set.HBlood_HighValue)<20) alarm_set.HBlood_HighValue=300;
			clear_area(3,63,5,95);			
			if(alarm_set.HBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.HBlood_HighValue/100);
			if(alarm_set.HBlood_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.HBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.HBlood_HighValue%10);
			break;
		}
		case State_HBlood_Set_Low_ADD:
		{
			if((++alarm_set.HBlood_LowValue)>300) alarm_set.HBlood_LowValue=20;	
			clear_area(3,22,5,54);
			if(alarm_set.HBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.HBlood_LowValue/100);
			if(alarm_set.HBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.HBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.HBlood_LowValue%10);
			break;
		}
		case State_HBlood_Set_Low_SUB:
		{
			if((--alarm_set.HBlood_LowValue)<20) alarm_set.HBlood_LowValue=300;	
			clear_area(3,22,5,54);
			if(alarm_set.HBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.HBlood_LowValue/100);
			if(alarm_set.HBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.HBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.HBlood_LowValue%10);
			break;
		}
		case State_HBlood_Set_OK:
		{
				if((alarm.HBlood_LowValue!=alarm_set.HBlood_LowValue) || (alarm.HBlood_HighValue!=alarm_set.HBlood_HighValue))
				{
					//进行存储
					
					alarm.HBlood_LowValue=alarm_set.HBlood_LowValue;
					alarm.HBlood_HighValue=alarm_set.HBlood_HighValue;					
					user_set_para[SET_PARA_HB_L_ADD]=(alarm.HBlood_LowValue>>8)&0xff;
					user_set_para[SET_PARA_HB_L_ADD+1]=alarm.HBlood_LowValue&0xff;
					user_set_para[SET_PARA_HB_H_ADD]=(alarm.HBlood_HighValue>>8)&0xff;
					user_set_para[SET_PARA_HB_H_ADD+1]=alarm.HBlood_HighValue&0xff;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}
		case State_LBlood_Set_High_ADD:
		{
			if((++alarm_set.LBlood_HighValue)>200) alarm_set.LBlood_HighValue=20;	
			clear_area(3,63,5,95);
			if(alarm_set.LBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.LBlood_HighValue/100);
			if(alarm_set.LBlood_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.LBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.LBlood_HighValue%10);
			break;
		}
		case State_LBlood_Set_High_SUB:
		{
			if((--alarm_set.LBlood_HighValue)<20) alarm_set.LBlood_HighValue=200;	
			clear_area(3,63,5,95);
			if(alarm_set.LBlood_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.LBlood_HighValue/100);
			if(alarm_set.LBlood_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.LBlood_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.LBlood_HighValue%10);
			break;
		}
		case State_LBlood_Set_Low_ADD:
		{
			if((++alarm_set.LBlood_LowValue)>200) alarm_set.LBlood_LowValue=20;	
			clear_area(3,22,5,54);
			if(alarm_set.LBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.LBlood_LowValue/100);
			if(alarm_set.LBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.LBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.LBlood_LowValue%10);
			break;
		}
		case State_LBlood_Set_Low_SUB:
		{
			if((--alarm_set.LBlood_LowValue)<20) alarm_set.LBlood_LowValue=200;	
			clear_area(3,22,5,54);
			if(alarm_set.LBlood_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.LBlood_LowValue/100);
			if(alarm_set.LBlood_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.LBlood_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.LBlood_LowValue%10);
			break;
		}	
		case State_LBlood_Set_OK:
		{
				if((alarm.LBlood_LowValue!=alarm_set.LBlood_LowValue) || (alarm.LBlood_HighValue!=alarm_set.LBlood_HighValue))
				{
					//进行存储
					
					alarm.LBlood_LowValue=alarm_set.LBlood_LowValue;
					alarm.LBlood_HighValue=alarm_set.LBlood_HighValue;
					user_set_para[SET_PARA_LB_L_ADD]=alarm.LBlood_LowValue;
					user_set_para[SET_PARA_LB_H_ADD]=alarm.LBlood_HighValue;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}
		
		case State_HeartRate_Set_High_ADD:
		{
			if((++alarm_set.HeartRate_HighValue)>200) alarm_set.HeartRate_HighValue=20;	
			clear_area(3,63,5,95);
			if(alarm_set.HeartRate_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.HeartRate_HighValue/100);
			if(alarm_set.HeartRate_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.HeartRate_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.HeartRate_HighValue%10);
			break;
		}
		case State_HeartRate_Set_High_SUB:
		{
			if((--alarm_set.HeartRate_HighValue)<20) alarm_set.HeartRate_HighValue=200;	
			clear_area(3,63,5,95);
			if(alarm_set.HeartRate_HighValue>99)
				Display_num_size10(3,10+53,alarm_set.HeartRate_HighValue/100);
			if(alarm_set.HeartRate_HighValue>9)
				Display_num_size10(3,10+64,alarm_set.HeartRate_HighValue/10%10);
			Display_num_size10(3,10+75,alarm_set.HeartRate_HighValue%10);
			break;
		}
		case State_HeartRate_Set_Low_ADD:
		{
			if((++alarm_set.HeartRate_LowValue)>200) alarm_set.HeartRate_LowValue=20;	
			clear_area(3,22,5,54);
			if(alarm_set.HeartRate_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.HeartRate_LowValue/100);
			if(alarm_set.HeartRate_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.HeartRate_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.HeartRate_LowValue%10);
			break;
		}
		case State_HeartRate_Set_Low_SUB:
		{
			if((--alarm_set.HeartRate_LowValue)<20) alarm_set.HeartRate_LowValue=200;	
			
			clear_area(3,22,5,54);
			if(alarm_set.HeartRate_LowValue>99)
				Display_num_size10(3,10+12,alarm_set.HeartRate_LowValue/100);
			if(alarm_set.HeartRate_LowValue>9)
				Display_num_size10(3,10+23,alarm_set.HeartRate_LowValue/10%10);
			Display_num_size10(3,10+34,alarm_set.HeartRate_LowValue%10);
			break;
		}	
		case State_HeartRate_Set_OK:
		{
				if((alarm.HeartRate_LowValue!=alarm_set.HeartRate_LowValue) || (alarm.HeartRate_HighValue!=alarm_set.HeartRate_HighValue))
				{
					//进行存储
					
					alarm.HeartRate_LowValue=alarm_set.HeartRate_LowValue;
					alarm.HeartRate_HighValue=alarm_set.HeartRate_HighValue;
					
					user_set_para[SET_PARA_HR_L_ADD]=alarm.HeartRate_LowValue;
					user_set_para[SET_PARA_HR_H_ADD]=alarm.HeartRate_HighValue;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}
		case State_Groupnum_Set_ADD:
		{
			if((++MeasurePara_Set.Groupnum)>20) MeasurePara_Set.Groupnum=9;
			clear_area(3,74,5,96);
			if(MeasurePara_Set.Groupnum>9)
			{	
				Display_num_size10(3,10+64,MeasurePara_Set.Groupnum/10);
				Display_num_size10(3,10+75,MeasurePara_Set.Groupnum%10);
			}
			else
			{
				Display_num_size10(3,79,MeasurePara_Set.Groupnum%10);
			}
			break;
		}
		case State_Groupnum_Set_SUB:
		{
			if((--MeasurePara_Set.Groupnum)<9) MeasurePara_Set.Groupnum=20;
			clear_area(3,74,5,96);
			if(MeasurePara_Set.Groupnum>9)
			{	
				Display_num_size10(3,10+64,MeasurePara_Set.Groupnum/10);
				Display_num_size10(3,10+75,MeasurePara_Set.Groupnum%10);
			}
			else
			{
				Display_num_size10(3,79,MeasurePara_Set.Groupnum);
			}
			break;
		}
		case State_Groupnum_Set_OK:
		{
			if(MeasurePara.Groupnum!=MeasurePara_Set.Groupnum)
				{
					//进行存储			
					MeasurePara.Groupnum=MeasurePara_Set.Groupnum;
					user_set_para[SET_PARA_MEASURE_GROUPNUM_ADD]=MeasurePara.Groupnum;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}
			
		case State_Timelong_Set_ADD:
		{
			if((++MeasurePara_Set.Timelong)>5) MeasurePara_Set.Timelong=2;
				Display_num_size10(3,79,MeasurePara_Set.Timelong);
			break;
		}
		case State_Timelong_Set_SUB:
		{
			if((--MeasurePara_Set.Timelong)<2) MeasurePara_Set.Timelong=5;
				Display_num_size10(3,79,MeasurePara_Set.Timelong);
			break;
		}
		case State_Timelong_Set_OK:
		{
			if(MeasurePara.Timelong!=MeasurePara_Set.Timelong)
				{
					//进行存储		
					MeasurePara.Timelong=MeasurePara_Set.Timelong;
					user_set_para[SET_PARA_MEASURE_TIMELONG_ADD]=MeasurePara.Timelong;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}
		case State_Timeinterval_Set_ADD:
		{
			if((++MeasurePara_Set.Timeinterval)>5) MeasurePara_Set.Timeinterval=1;
				Display_num_size10(3,79,MeasurePara_Set.Timeinterval);
			break;
		}
		case State_Timeinterval_Set_SUB:
		{
			if((--MeasurePara_Set.Timeinterval)<1) MeasurePara_Set.Timeinterval=5;
				Display_num_size10(3,79,MeasurePara_Set.Timeinterval);
			break;
		}
		case State_Timeinterval_Set_OK:
		{
			if(MeasurePara.Timeinterval!=MeasurePara_Set.Timeinterval)
				{
					//进行存储
					MeasurePara.Timeinterval=MeasurePara_Set.Timeinterval;
					user_set_para[SET_PARA_MEASURE_TIMEINTERVAL_ADD]=MeasurePara.Timeinterval;
					Sava_para_to_flash();
					Delay_S(1);
				}
			break;	
		}	

		case State_AutoMeasure_Timelong_Set_ADD:
		{
			if((++AutoMeasurePara_Set.Timelong)>99) AutoMeasurePara_Set.Timelong=1;
			clear_area(2,40,3,60);
			if(AutoMeasurePara_Set.Timelong>9)
			{	
				Display_num_size16_9(2,40,AutoMeasurePara_Set.Timelong/10);
				Display_num_size16_9(2,51,AutoMeasurePara_Set.Timelong%10);
			}
			else
			{
				Display_num_size16_9(2,45,AutoMeasurePara_Set.Timelong);
			}
			break;
		}
		case State_AutoMeasure_Timelong_Set_SUB:
		{
			if((--AutoMeasurePara_Set.Timelong)<1) AutoMeasurePara_Set.Timelong=99;
			clear_area(2,40,3,60);
			if(AutoMeasurePara_Set.Timelong>9)
			{	
				Display_num_size16_9(2,40,AutoMeasurePara_Set.Timelong/10);
				Display_num_size16_9(2,51,AutoMeasurePara_Set.Timelong%10);
			}
			else
			{
				Display_num_size16_9(2,45,AutoMeasurePara_Set.Timelong);
			}
			break;
		}		
		case State_AutoMeasure_Timeinterval_Set_ADD:
		{
			if((++AutoMeasurePara_Set.Timeinterval)>99) AutoMeasurePara_Set.Timeinterval=1;
			clear_area(5,40,6,60);	
			if(AutoMeasurePara_Set.Timeinterval>9)
			{	
				Display_num_size16_9(5,40,AutoMeasurePara_Set.Timeinterval/10);
				Display_num_size16_9(5,51,AutoMeasurePara_Set.Timeinterval%10);
			}
			else
			{
				Display_num_size16_9(5,45,AutoMeasurePara_Set.Timeinterval);
			}
			break;
		}
		case State_AutoMeasure_Timeinterval_Set_SUB:
		{
			if((--AutoMeasurePara_Set.Timeinterval)<1) AutoMeasurePara_Set.Timeinterval=99;		
			clear_area(5,40,6,60);		
			if(AutoMeasurePara_Set.Timeinterval>9)
			{	
				Display_num_size16_9(5,40,AutoMeasurePara_Set.Timeinterval/10);
				Display_num_size16_9(5,51,AutoMeasurePara_Set.Timeinterval%10);
			}
			else
			{
				Display_num_size16_9(5,45,AutoMeasurePara_Set.Timeinterval);
			}
			break;
		}
		
		case State_AutoMeasure_Set_OK:
		{
			if((AutoMeasurePara.Timeinterval !=AutoMeasurePara_Set.Timeinterval)||(AutoMeasurePara.Timelong !=AutoMeasurePara_Set.Timelong))
			{
				//进行存储
				
				AutoMeasurePara.Timeinterval =AutoMeasurePara_Set.Timeinterval;
				AutoMeasurePara.Timelong =AutoMeasurePara_Set.Timelong;	
				user_set_para[SET_PARA_AUTOMEASURE_TIMELONG_ADD]=AutoMeasurePara.Timelong;
				user_set_para[SET_PARA_AUTOMEASURE_TIMEINTERVAL_ADD]=AutoMeasurePara.Timeinterval;
				Sava_para_to_flash();
				Delay_S(1);
			}
			break;
		}		
		case State_StandbyTime_Set_ADD:
		{
			if((++StandbyTime_Set)>99) StandbyTime_Set=5;
			clear_area(3,50,5,71);
			if(StandbyTime_Set>9)
			{	
				Display_num_size10(3,50,StandbyTime_Set/10);
				Display_num_size10(3,61,StandbyTime_Set%10);
			}
			else
			{
				Display_num_size10(3,56,StandbyTime_Set);
			}
			break;
		}
		case State_StandbyTime_Set_SUB:
		{
			if((--StandbyTime_Set)<5) StandbyTime_Set=99;
			clear_area(3,50,5,71);
			
			if(StandbyTime_Set>9)
			{	
				Display_num_size10(3,50,StandbyTime_Set/10);
				Display_num_size10(3,61,StandbyTime_Set%10);
			}
			else
			{
				Display_num_size10(3,56,StandbyTime_Set);
			}
			break;
		}
		case State_StandbyTime_Set_OK:  //在设置待机时间的确定项上点OK键
		{
			if(StandbyTime!=StandbyTime_Set)
			{
				//进行存储			
				StandbyTime=StandbyTime_Set;
				user_set_para[SET_PARA_STANDBYTIME_ADD]=StandbyTime;
				Sava_para_to_flash();
				Delay_S(1);
			}
			break;
		}
		case State_ReFactory_OK:  //在恢复出厂的确定项上点OK
		{
			//进行初始化操作  主要进行参数的默认值设置		
			S25FL512S_Erase_Sector(1); //测试用，擦除测量结果据区
			S25FL512S_Erase_Sector(16); //测试用，擦除测量数据据区
			S25FL512S_Erase_Sector(4);
			Save_para_as_define();
			Find_measure_data_add_prt();//找到最后一次记录数据存放位置的索引，从而找到最后一次数据存放位置
			Find_Save_Index_Ptr(); //找到最后一条记录存放位置
			break;
		}
		default:
			break;
	}

}



//函数说明:所有按设置键的操作都进入此函数进行处理
void Deal_SET_Key(void)
{	
	uint8_t return_data;
	
	if(LCD_Display_OnOff==Display_OFF)  //如果当前为屏关显示状态要先打开显示
	{
		LCD_Transfer_Command(0xaf); //开显示
		LCD_Display_OnOff=Display_ON;	
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	
	StandbyTime_Count=0; //计数清空
	
	if(Is_ParaSet_States())  //如果为设置参数状态
	{
		Deal_ParaSet_Func(); //执行设置
	}
	else if(Is_ParaSelect_States()) //如果是选择类的参数设置
	{
		Deal_ParaSelect_OK_Func();  //进行入选择函数
	}
	
	if(g_CurrentStat != stateData[g_CurrentStat].navigationData[NAV_SET] ) //如果SET键有功能的话
	{	
		if(g_CurrentStat==State_Main_Menu_Auto && Auto_Measure_OnOff==TURN_ON )  //如果当前为自动测量，按确认键时，已经打开自动
		{
			g_CurrentStat=State_Auto_Measure_IsTurnON;  //转到已经开启状态，显示测量成功失败等信息
		}
		else if(g_CurrentStat==State_Auto_Measure_IsTurnON)  //在开启状态，按确认进行关闭
		{
			Auto_Measure_OnOff=TURN_OFF;
			Auto_Measure_TimeInterval_Count=0;
			g_CurrentStat=stateData[g_CurrentStat].navigationData[NAV_SET];
		}
		else if(g_CurrentStat==State_Main_Menu_Set &&Auto_Measure_OnOff==TURN_ON)  //如在设置项按确认，且自动测量开启
		{
			g_CurrentStat=State_Set_error;   //切换到设置错误状态，提示无法进行设置
		}
		else
			g_CurrentStat=stateData[g_CurrentStat].navigationData[NAV_SET];
		
		if(stateData[g_CurrentStat].DisBackFunc !=NULL )   //如果要有背景需要加截
		{
			stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);	
		}
		if(stateData[g_CurrentStat].ExecFunction !=NULL ) //如果有外部的函数需要调用 
		{
			if(g_CurrentStat==State_Manual_DeviceCheck)  //如果为手动测量设备检测状态
			{
				return_data=stateData[g_CurrentStat].ExecFunction(1);  //检测通过
				if(return_data==1)
				{						
					g_CurrentStat=State_Manual_Usertouch;  //提示用户进行测量
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==INTERACT_FAIL)  //通信失败
				{
					g_CurrentStat=State_Manual_pw1101_interact_error;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==AUTO_MEASURE_GOING)    //自动测量开启中
				{
					g_CurrentStat=State_Manual_pw1101_AutoGoing_error;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==LOW_POWER)  //电量低
				{					
						g_CurrentStat=State_Manual_LowPower;
						stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==TEMP_HIGH) //温度高
				{
					g_CurrentStat=State_Manual_TempHigh;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==TEMP_LOW) //温度低
				{
					g_CurrentStat=State_Manual_TempLow;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
			}
			if(g_CurrentStat==State_Auto_DeviceCheck)   //自动测量设备自测
			{
				return_data=stateData[g_CurrentStat].ExecFunction(1);
				if(return_data==1)
				{	
					g_CurrentStat=State_Auto_Measure_TurnON;   //自动测量开启，提示已经测量的信息
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
					Delay_S(2);				
					g_CurrentStat=State_Auto_Doing;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==INTERACT_FAIL)  //通信失败
				{
					g_CurrentStat=State_Auto_pw1101_interact_error;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==LOW_POWER)   //电压低
				{					
						g_CurrentStat=State_Auto_LowPower;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==TEMP_HIGH) //温度高
				{
					g_CurrentStat=State_Auto_TempHigh;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
				else if(return_data==TEMP_LOW) //温度低
				{
					g_CurrentStat=State_Auto_TempLow;
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
			}
			if(g_CurrentStat==State_Record_Display)  //显示测量的历史记录
			{
				if(stateData[g_CurrentStat].ExecFunction(1))
				{						
					Func_Display_Record_data();
				}
			}
			if(g_CurrentStat==State_Manual_Doing)  //如果状态为进行数据数据采集
			{
				Current_Eletrode=MeasurePara.Eletrode; //手动的时候为用户设置的采集电极
				switch(stateData[g_CurrentStat].ExecFunction(1))  //进行手动测量 
				{
					case MEASURE_SUCCESS:     //返回状态为测量成功，状态切换到测量成功状态
						g_CurrentStat=State_Manual_Measure_OK;
						break;
					case WEAR_NOT_GOOD:   //没有佩戴好
						g_CurrentStat=State_Manual_Measure_error1;
						break;
					case INTERACT_FAIL:  //通信失败
						g_CurrentStat=State_Manual_pw1101_interact_error;
						break;
					case MEASURE_FAIL:  //测量失败
						g_CurrentStat=State_Manual_Measure_False;
						break;
					default:					
						 break;
				}
				stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
			}
			else if(g_CurrentStat==State_Auto_Doing)  //如果状态为进行数据数据采集
			{
				Current_Eletrode=0x01;  //自动的时候为湿电极
				switch(stateData[g_CurrentStat].ExecFunction(1))  //点自动测量的时候进行一次测量,所调用函数可在状态机的State_Auto_Doing中查看
				{
					case MEASURE_SUCCESS:  //测量成功后，打开后能自动测量计时器，并把当前测量统计到自动测量次数中
						g_CurrentStat=State_Auto_Measure_OK;  
						Auto_Measure_Success_Count=1;	
						Auto_Measure_OnOff=TURN_ON;
						Auto_Measure_Total_Num_Count=1;
						Auto_Measure_TimeInterval_Count=0; //做完自动测量的第一次后，清一下计数器，保存下一次开始的的间隔为设置的间隔时间
						break;
					case WEAR_NOT_GOOD:   //没有佩戴好
						g_CurrentStat=State_Auto_Measure_error1;
						break;
					case INTERACT_FAIL:
						g_CurrentStat=State_Auto_pw1101_interact_error;
						break;
					case MEASURE_FAIL:
						g_CurrentStat=State_Auto_Measure_False;
						break;
					default:					
						 break;
				}
				stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
//				Auto_Measure_Total_Num_Count=1;
//				Auto_Measure_TimeInterval_Count=0; //做完自动测量的第一次后，清一下计数器，保存下一次开始的的间隔为设置的间隔时间
			}
		}
	}
}


//按取消键时，函数被调用
void Deal_RES_Key(void)
{
	if(LCD_Display_OnOff==Display_OFF)
	{
		LCD_Transfer_Command(0xaf); //开显示
		LCD_Display_OnOff=Display_ON;	
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	StandbyTime_Count=0;
	if(g_CurrentStat != stateData[g_CurrentStat].navigationData[NAV_RES] ) //如果SET键有功能的话
	{
		g_CurrentStat=stateData[g_CurrentStat].navigationData[NAV_RES];
		if(stateData[g_CurrentStat].DisBackFunc !=NULL )
		{
			stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
		}
	}
}



//按切换键时，函数被调用
void Deal_UD_Key(void)
{	
	if(LCD_Display_OnOff==Display_OFF)
	{
		LCD_Transfer_Command(0xaf); //开显示
		LCD_Display_OnOff=Display_ON;
//        GPIO_SetPinsOutput(GPIOA, 1u << 13);//开启12.5高压  吕鹏
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
	}
	else
	{
			StandbyTime_Count=0;
			if(Is_ParaSelect_States())  //如果当前状态机为选择参数状态，则进行选择参数处理函数
			{
				Deal_ParaSelect_Func(); //参数设置中的选择参数设置，如性别设置中选择男女,参数为有限个选项时，进行选择
			}
			if(g_CurrentStat != stateData[g_CurrentStat].navigationData[NAV_UD] ) //如果UD键有功能的话，这个状态在states.c中的状态机里设置
			{
				g_CurrentStat=stateData[g_CurrentStat].navigationData[NAV_UD];
				if(stateData[g_CurrentStat].DisBackFunc !=NULL )
				{
					stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);
				}
			}
			if(g_CurrentStat==State_Record_Display)  //如果当前为历史记录查询状态时，按UD键来翻查记录 
			{
				if(display_record_index>record_begin_index)  display_record_index--;
				else display_record_index=RECORD_NUM-1;
				Func_Display_Record_data();
			}
	}
	
}




//
void Deal_POW_Key(void)
{
	
}
