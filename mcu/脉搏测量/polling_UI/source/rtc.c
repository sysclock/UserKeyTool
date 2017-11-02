#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "rtc.h"
#include "lcd_spi.h"
#include "Queue.h"
#include "MenuTbl.h"

uint16_t RTC_Delay_Time=0;
rtc_datetime_t date;
rtc_datetime_t date_set;

char GMT_num=8;
char GMT_num_set=8;


uint32_t Delay_Time_Num=0;

uint32_t Pw1101_interact_timeout=0;
uint8_t  Pw1101_interact_flag=0;

uint8_t display_point_flag=0;


uint32_t Measure_time_flag=0;

uint32_t bat_display=0;

uint32_t Auto_Measure_TimeInterval_Count=0;
uint32_t Auto_Measure_Total_Num_Count=0; //���в�������ͳ�ƣ���������ò���ʱ���ͼ��ʱ�䣬����������������һ�����Ƶ���ֵ
uint32_t Auto_Measure_Success_Count=0; //ͳ���Զ������ɹ�����
uint32_t Auto_Measure_Fail_Count=0;    //ͳ���Զ�����ʧ�ܴ���

uint32_t StandbyTime_Count=0;  //������ʱ��
uint32_t StandbyTime_Flag=0;  //������ʱ��ʶ����Ϊ1ʱ��ʼ��ʱ


//
void BOARD_SetRtcClockSource(void)
{
	  /* Enable the RTC 32KHz oscillator */
    RTC->CR |= RTC_CR_OSCE_MASK;
}



//ʱ���ж�
void RTC_IRQHandler(void)
{
    if (RTC_GetStatusFlags(RTC))
    {
				//�����жϴ���		
        /* Clear alarm flag */
        RTC_ClearStatusFlags(RTC, kRTC_AlarmInterruptEnable); //�������ж�		
    }
}


//RTC���жϴ�����
void RTC_Seconds_IRQHandler(void)
{
		//�����жϴ���		
		/* Clear alarm flag */
		RTC_ClearStatusFlags(RTC, kRTC_SecondsInterruptEnable); //�������ж�		
	
		if(Delay_Time_Num) Delay_Time_Num--;    //��ʱ������
		if(Pw1101_interact_flag)Pw1101_interact_timeout++;  //PW1101ͨ�ų�ʱ��
				
		if(display_point_flag) display_point_flag--;
		
		if(Measure_time_flag) Measure_time_flag--;
	
		
		bat_display++;
	
		if(Auto_Measure_OnOff==TURN_ON) Auto_Measure_TimeInterval_Count++; //�����ǰ�Զ�����״̬Ϊ�򿪣�Ϊ��һ�ν��м�ʱ
	
		if(StandbyTime_Flag) StandbyTime_Count++;   //������ʱ��
		
		if(LCD_Display_OnOff==Display_OFF && Auto_Measure_OnOff==TURN_ON)
		{
			if(Auto_Measure_TimeInterval_Count>(60*AutoMeasurePara.Timeinterval)) // �Զ�������ʱ������λΪ���ӣ��жϼ����ĵ�λΪ�룬���Գ���60
			{
				if(Auto_Measure_Total_Num_Count<=( AutoMeasurePara.Timelong*60/AutoMeasurePara.Timeinterval+1))  //�������������Զ���ʱʱ���ͼ���������ʱ��
				{
					if(g_CurrentStat==State_Main_Menu ||g_CurrentStat==State_Main_Menu_Manual ||g_CurrentStat==State_Main_Menu_Auto||g_CurrentStat==State_Main_Menu_Record||g_CurrentStat==State_Main_Menu_Set)  //�����ʱ�ѵ�����Ϊ�ɽ���ҳ��״̬
					{					
							BOARD_BootClockRUN();//��������ϵͳʱ��
						// SMC->PMPROT= 0x00;  

						 DisableIRQ(PORTA_IRQn);//�ر��ж�
						 DisableIRQ(PORTB_IRQn);//�ر��ж�
								LCD_VCC_ON;//����12.5��ѹ  ���� 
					
						
							GPIO_SetPinsOutput(GPIOB, 1u << 21);  //���߿���VDDA		
							GPIO_SetPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��
							
									
							LCD_Transfer_Command(0xaf); //����ʾ
							LCD_Display_OnOff=Display_ON;
							if(StandbyTime!=0)
							{
								StandbyTime_Count=0;
								StandbyTime_Flag=1;
							}		
							bat_display=181;      //j��ʾ��־��180��ʾ3����  ����181�������е�����ˢ��
					}
				}
			}			
		}
}


//�뼶��ʱ
void Delay_S(uint32_t num)
{
	Delay_Time_Num=num;
	while(Delay_Time_Num);
}


//ʵʱʱ�ӳ�ʼ��
void User_RTC_Init(void)
{
	rtc_config_t rtcConfig;	
	RTC_GetDefaultConfig(&rtcConfig);
	
	RTC_Init(RTC, &rtcConfig);
	/* Select RTC clock source */
	BOARD_SetRtcClockSource();
	
	RTC_GetDatetime(RTC, &date);
	
	if(date.year<2016)
	{
		date.year = 2016U;
    date.month = 1U;
    date.day = 1U;
    date.hour = 8U;
    date.minute = 0;
    date.second = 0;
		RTC_StopTimer(RTC);
		RTC_SetDatetime(RTC, &date);
		RTC_StartTimer(RTC);
	}
	/* Enable RTC alarm interrupt */
	RTC_EnableInterrupts(RTC, kRTC_SecondsInterruptEnable);
	EnableIRQ(RTC_Seconds_IRQn);
}



//�û�����ʱ�ӵĹ���
uint8_t User_Set_time(void)
{
	date.year = date_set.year;
	date.month = date_set.month;
	date.day = date_set.day;
	date.hour = date_set.hour;
	date.minute = date_set.minute;
	date.second = date_set.second;
	
	RTC_StopTimer(RTC);
	/* Set RTC time to default */
	RTC_SetDatetime(RTC, &date);
	
	/* Enable at the NVIC */	
	RTC_StartTimer(RTC);
	return 1;
}


//�û�����
uint8_t User_Set_UTC(void)
{
	uint32_t time_tsr=0;
	int num;
	RTC_StopTimer(RTC);
	/* Set RTC time to default */
	time_tsr=RTC->TSR;
	
	num=(GMT_num_set-GMT_num)*3600;
	
	RTC->TSR=time_tsr+num;
	
	/* Enable at the NVIC */	
	RTC_StartTimer(RTC);
	return 1;
}


//ͨ����ǰʱ���������  ����0123456
uint8_t RTC_GetDateweek(rtc_datetime_t *tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	lastYear=tm->year -1;

	/*
	 * Number of leap corrections to apply up to end of last year
	 */
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;

	/*
	 * This year is a leap year if it is divisible by 4 except when it is
	 * divisible by 100 unless it is divisible by 400
	 *
	 * e.g. 1904 was a leap year, 1900 was not, 1996 is, and 2000 will be
	 */
	if((tm->year%4==0) &&
	   ((tm->year%100!=0) || (tm->year%400==0)) &&
	   (tm->month >2)) {
		/*
		 * We are past Feb. 29 in a leap year
		 */
		day=1;
	} else {
		day=0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[tm->month-1] + tm->day;

	return day%7;
}



//ʱ����ʾ
uint8_t minute_updata_flag=61;
void Display_current_time(void)
{
	uint8_t week;
	RTC_GetDatetime(RTC, &date);
	
	if(date.minute!=minute_updata_flag)
	{
		week=RTC_GetDateweek(&date);
		if(week==0) week=7;

		Display_num_size5(1,1,date.year/1000);
		Display_num_size5(1,8,(date.year/100)%10);
		Display_num_size5(1,15,(date.year/10)%10);
		Display_num_size5(1,22,date.year%10);
		Display_date_sapce(1,29);
		Display_num_size5(1,34,date.month/10);
		Display_num_size5(1,41,date.month%10);
		Display_date_sapce(1,48);
		Display_num_size5(1,53,date.day/10);
		Display_num_size5(1,60,date.day%10);
		
		Clear_sapce(18,67);
		Display_week_data(1,87,0);
		Display_week_data(1,103,week);
		
		Display_num_size25(3,5,date.hour/10);
		Display_num_size25(3,32,date.hour%10);
		Display_time_sapce(3,59);
		Display_num_size25(3,65,date.minute/10);
		Display_num_size25(3,92,date.minute%10);
		minute_updata_flag=date.minute;
	}
}

# define Date_1970  2208988800000

# define Hour_ms    3600000      //һСʱ��ms��



uint64_t utc_time_num_cache;  //UTCʱ�仺��
uint64_t utc_time_num_offset; //UTCʱ�����RTC��ƫ��
uint32_t RTC_time_num_cache;  //RTCʱ�仺��

void Updata_time(uint8_t comm)
{
	uint32_t utc_time_num_cache_H4;   //UTCʱ��ߣ��ֽ�
	uint32_t utc_time_num_cache_L4;   //UTCʱ��ͣ��ֽ�
	char GMT_cache=0;
	uint8_t i=0;
	if(comm==BLE_COMM)  //������ݴ��������յģ��������ָ������������� �����USB������������
	{
		utc_time_num_cache_H4=(BLE_comm_cache[9]<<24)+(BLE_comm_cache[10]<<16)+(BLE_comm_cache[11]<<8)+(BLE_comm_cache[12]);
		utc_time_num_cache_L4=(BLE_comm_cache[13]<<24)+(BLE_comm_cache[14]<<16)+(BLE_comm_cache[15]<<8)+(BLE_comm_cache[16]);
		GMT_cache=BLE_comm_cache[17];
	}	
	else 
	{
			utc_time_num_cache_H4=(USB_comm_cache[9]<<24)+(USB_comm_cache[10]<<16)+(USB_comm_cache[11]<<8)+(USB_comm_cache[12]);
			utc_time_num_cache_L4=(USB_comm_cache[13]<<24)+(USB_comm_cache[14]<<16)+(USB_comm_cache[15]<<8)+(USB_comm_cache[16]);
		GMT_cache=USB_comm_cache[17];
	}
	
	utc_time_num_cache=((uint64_t)utc_time_num_cache_H4*(uint64_t)(4294967296)+ (uint64_t)utc_time_num_cache_L4);
	utc_time_num_offset=(uint64_t)utc_time_num_cache-(uint64_t)Date_1970+(uint64_t)(GMT_cache*Hour_ms);
	
	if(((uint64_t)utc_time_num_offset %1000)>500)   //UTCʱ�侫��Ϊ���룬RTCΪ�룬�������500���룬���һ�뵽RTCʱ��
		i=1;
	else
		i=0;
	
	
	//����ʱ��
	RTC_time_num_cache=(uint64_t)utc_time_num_offset/1000+i;
	
	RTC_StopTimer(RTC);
	/* Set RTC time to default */
	RTC->TSR=RTC_time_num_cache;
	
	/* Enable at the NVIC */	
	RTC_StartTimer(RTC);
	
		
}

