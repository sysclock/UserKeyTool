#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "uart.h"
#include "lcd_spi.h"
#include "error1.h"
#include "num_bmp.h"
#include "flash_spi.h"
#include "Queue.h"
#include "fsl_rtc.h"
#include "rtc.h"

uint8_t measure_result_data[4]={0};

uint16_t command_count_num=1;

//��PW1101��������
void PW1101_SEND_DATA(uint8_t *data,uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		PW1101_UART_Send_Byte(data[i]);
	}
}


//���У�麯��
uint8_t Check_CRC8(uint8_t *data)
{
	uint8_t i;
	uint8_t CRC8=0x00;
	for(i=1;i<9+data[DATA_LEN_INDEX];i++)
	{
		CRC8^=data[i];
	}
	return CRC8;
}



//��ȡ��PW1101 ID �Ĵ洢����
uint32_t PW1101_ID_H;
uint32_t PW1101_ID_MH;
uint32_t PW1101_ID_ML;
uint32_t PW1101_ID_L;
uint8_t  PW1101_ID_data=0;   //16�ֽڵ����ֵ

uint16_t Software_version=0x0001;
uint16_t Algorithm_version=0x0001;

//��ȡIC��Ϣ ����ID�ţ��汾�ŵ�
uint8_t read_PW1101Information (void)
{
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	PW1101_Comm_RxQueue_Init();
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE)  //������͵�ָ��ͽ��յ�ָ������һ�������յ������Ļظ�
			{
				PW1101_ID_H=(comm_cache[9]<<24)+(comm_cache[10]<<16)+(comm_cache[11]<<8)+(comm_cache[12]);
				PW1101_ID_MH=(comm_cache[13]<<24)+(comm_cache[14]<<16)+(comm_cache[15]<<8)+(comm_cache[16]);
				PW1101_ID_ML=(comm_cache[17]<<24)+(comm_cache[18]<<16)+(comm_cache[19]<<8)+(comm_cache[20]);
				PW1101_ID_L=(comm_cache[21]<<24)+(comm_cache[22]<<16)+(comm_cache[23]<<8)+(comm_cache[24]);
				Software_version=(comm_cache[25]<<8)+(comm_cache[26]);
				Algorithm_version=(comm_cache[27]<<8)+(comm_cache[28]);
				
				PW1101_ID_data=comm_cache[9]^comm_cache[10]^comm_cache[11]^comm_cache[12]^comm_cache[13]^comm_cache[14]^comm_cache[15]^comm_cache[16]^comm_cache[17]^comm_cache[18]^comm_cache[19]^comm_cache[20]^comm_cache[21]^comm_cache[22]^comm_cache[23]^comm_cache[24];
				
				break;
			}
		}	
		if(Pw1101_interact_timeout>3)
		{		
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_READ_IC_INFOMATION;
		PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
		PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
		PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
		PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
		PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
		PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
		PW1101_txbuff[DATA_LEN_INDEX]=0;
		PW1101_txbuff[9+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
		PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
		Delay_S(1);
	}	
	return 1;
}


//pw1101״̬���
void pw1101_func_status_check(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_STATUS_CHENK;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[9+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}




//�����û���Ϣ
void pw1101_func_Set_UserPara(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_USER_PARA;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0x04;
	PW1101_txbuff[DATA_INDEX_1]=user.sex;
	PW1101_txbuff[DATA_INDEX_1+1]=user.age;
	PW1101_txbuff[DATA_INDEX_1+2]=user.height;
	PW1101_txbuff[DATA_INDEX_1+3]=user.weight;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}


//���ò���ģʽ�������û����õ�ģʽ��������
void pw1101_func_Set_MeasureMode(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_MEASURE_MODE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0x01;
	PW1101_txbuff[DATA_INDEX_1]=Measure_Mode;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}

//�������������������������������������ʱ��������������缫ʹ�����
void pw1101_func_Measure_Para(uint8_t para1,uint8_t para2,uint8_t para3,uint8_t para4,uint8_t para5)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_MEASURE_PARA;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0x05;
	PW1101_txbuff[DATA_INDEX_1]=0x07;
	PW1101_txbuff[DATA_INDEX_2]=0x0b;
	PW1101_txbuff[DATA_INDEX_3]=0x03;
	PW1101_txbuff[DATA_INDEX_4]=0x02;
	PW1101_txbuff[DATA_INDEX_5]=para5;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}

//��ʼ����
void pw1101_func_start_measure(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_START_MEASURE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}


//ֹͣ����
void pw1101_func_stop_measure(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_STOP_MEASURE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}


//�յ���������󣬽���ȷ��
void pw1101_func_confirm_measure_result(uint8_t TorF)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_SEND_MEASURE_RESULT;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //ָ���ͳ�Ƹ�8λ
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //ָ���ͳ�Ƶ�8λ
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=1;
	PW1101_txbuff[DATA_INDEX_1]=TorF;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}



//��1ms����ʱ������ʱ������ȷ��ֻ����Ҫ����ȷ��ʱ�����ã���ϵͳ��ʱ���кܴ��ϵ
void delay_time(uint32_t num)
{
	uint32_t i,j;
	for(i=0;i<num;i++)
		for(j=0;j<10000;j++);
}





//����ģʽѡ������ֻ֧�������������������ݲ���
uint8_t pw1101_func_manual_measure_mode_select(uint32_t para)
{
	switch(Measure_Mode)
	{
		case MEASURE_MODE1:
			break;
		case MEASURE_MODE2:
			return pw1101_func_manual_measure(1);  //�����ݲ�����
			break;
		case MEASURE_MODE3:
			break;
		case MEASURE_MODE4:
			return pw1101_func_manual_measure(1);  //�������ݲ�����
			break;
		default:
			break;
	}
	return 0;
}

//�����ֶ�����

uint8_t data_cache[6000];
uint8_t pw1101_func_manual_measure(uint32_t data)
{
	
	uint8_t draw_i,draw_j,draw_k,draw_m;
	uint16_t draw_n;
	uint8_t pre1,pre2;
	uint8_t pre_i;
	uint8_t data_H;
	uint8_t data_L;
	uint8_t draw_x;
	
	uint8_t point_num=2;
	uint8_t measure_flag=0;
	uint8_t error_count_num=0;
	uint8_t total_num=0; 

	uint8_t display_page_flag=0;
//	uint8_t data_cache[6000];
	uint32_t cache_index=0;
	uint8_t cache_frist=1;
	uint8_t data_head_flag[20]={0xaa,0xbb,0xcc,0xdd,0xee,0x11,0x22,0x33,0x44,0x55,0,0,0,0,0,0xfc,0xfc,0xfc,0xfc,0xfc};
	uint32_t curtime;
		
		
	uint16_t Blood_data[375];  //��ȡ������������
	uint16_t HeartRate_data[375]; //��ȡ�����ĵ�����
	uint16_t Blood_Max=0;   //�����������ֵ
	uint16_t Blood_Min=65535;  //����������Сֵ
	uint16_t HeartRate_Max=0;  //�ĵ����ֵ
	uint16_t HeartRate_Min=65535;  //�ĵ���Сֵ
	uint16_t Blood_Compress=200;  //����ѹ����
	uint16_t HeartRate_Compress=200;  //�ĵ�ѹ����	
		
		
	Display_point(point_num);
	point_num=(point_num+1)%3;
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	if(Measure_Mode==MEASURE_MODE4) //������������ݵĲ�����ʽ
	{
		Erase_Next_Sector(); //�����һ�������������ݵı���
	}
	PW1101_Comm_RxQueue_Init();
	//���ò���ģʽ
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{	
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Set_MeasureMode();
		Delay_S(1);
		Display_point(point_num);
		point_num=(point_num+1)%3;
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
		
		
	//�����û�����
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{	
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Set_UserPara();
		Delay_S(1);
		Display_point(point_num);
		point_num=(point_num+1)%3;
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	//���ò�������
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Measure_Para(MeasurePara.Groupnum-4,MeasurePara.Groupnum,MeasurePara.Timeinterval,MeasurePara.Timelong,Current_Eletrode);
		Delay_S(1);	
		Display_point(point_num);
		point_num=(point_num+1)%3;		
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	//��ѯ״̬
	while(1)
	{
		//��ȡ�Ӷ����н���ָ�
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE 
				&& (comm_cache[DATA_INDEX_1]==1 || comm_cache[DATA_INDEX_1]==4 || comm_cache[DATA_INDEX_1]==7))
			{
				break;
			}
			Pw1101_interact_timeout=0;
			Pw1101_interact_flag=1;
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_status_check();
		Delay_S(1);	
		Display_point(point_num);
		point_num=(point_num+1)%3;		
	}	
		
	Delay_S(1);
	Display_point(point_num);
	point_num=(point_num+1)%3;
	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	//��ʼ����
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{
				break;
			}
			Pw1101_interact_timeout=0;
			Pw1101_interact_flag=1;
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_start_measure();
		Delay_S(1);
		Display_point(point_num);
		point_num=(point_num+1)%3;
	}	
	display_point_flag=1;
	Measure_time_flag= MEASURE_TIMEOUT;
	while(1)
	{
		/*
		�������ݵĴ洢
		*/
		if(Measure_Mode==MEASURE_MODE4) //���Ϊ�������ݵĲ�����ʽ
		{
			if(data_group_count==30)//�������ǲ�����3S�Ժ����ݣ�����Ϊ30�飬�չ����ٽ��д����˴�����ԭ�����������޸ģ���û�н������䣬��һ���ķ���
			{
				//���´���ʵ�ֲ�ݵĴ洢
				for(cache_index=0;cache_index<6000;cache_index++)
				{
					if(PW1101_Data_QueueOut(&g_DataRxQueue,&data_cache[cache_index])== QueueEmpty)
					{
						break;
					}
				}
				if(cache_index!=0)  //�Ӷ������õ���ֵ
				{
					if(cache_frist) //���Ϊ��һ�����ݣ���������ݴ洢ʱ��Ҫ��ʱ����Ϣ��20���ֽ�
					{
						curtime=RTC->TSR;				
						data_head_flag[10]=(curtime>>24)&0xff;  //ʱ���
						data_head_flag[11]=(curtime>>16)&0xff;
						data_head_flag[12]=(curtime>>8)&0xff;
						data_head_flag[13]=(curtime)&0xff;
						data_head_flag[14]=GMT_num;
						cache_frist=0;
						
						
						if((FLASH_TOTAL_ADD-measure_data_save_ptr)<20)  //������������洢20���ֽ�����
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,FLASH_TOTAL_ADD-measure_data_save_ptr);
							measure_data_save_ptr=Measure_Data_Begin;  //������ʼλ��
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20-(FLASH_TOTAL_ADD-measure_data_save_ptr));
							measure_data_save_ptr+=(20-(FLASH_TOTAL_ADD-measure_data_save_ptr));	
						}
						else
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20);
							measure_data_save_ptr+=20;	
						}						
					}   //ʱ����Ϣ
					
					
					
					if((FLASH_TOTAL_ADD-measure_data_save_ptr)<cache_index)  //��������������洢���ݣ�ͬ��������ʼλ��
					{
						S25FL512S_Write(data_cache,measure_data_save_ptr,FLASH_TOTAL_ADD-measure_data_save_ptr);
						measure_data_save_ptr=Measure_Data_Begin;
						S25FL512S_Write(data_cache,measure_data_save_ptr,cache_index-(FLASH_TOTAL_ADD-measure_data_save_ptr));
						measure_data_save_ptr+=(cache_index-(FLASH_TOTAL_ADD-measure_data_save_ptr));	
					}
					else
					{
						S25FL512S_Write(data_cache,measure_data_save_ptr,cache_index);
						measure_data_save_ptr+=cache_index;	
					}	
					
					
			////////////////��ʾ����/////////////////////////////////////////////////////
				Blood_Max=0;   //�����������ֵ
				Blood_Min=65535;  //����������Сֵ
				HeartRate_Max=0;  //�ĵ����ֵ
				HeartRate_Min=65535;  //�ĵ���Сֵ
				Blood_Compress=200;  //����ѹ����
				HeartRate_Compress=200;  //�ĵ�ѹ����	
				for(draw_n=0;draw_n<375;draw_n++)  //��1500����������ȡ��375����
				{
					//��������
					data_H=(((data_cache[draw_n*16]&0x0f)<<4)+((data_cache[draw_n*16]&0xf0)>>4)-5)^PW1101_ID_data;  //�ֽڸ��ڸ�4λ��4λ�����ټ�5 �������ID�����ֵ���
					data_L=(((data_cache[draw_n*16+1]&0x0f)<<4)+((data_cache[draw_n*16+1]&0xf0)>>4)-5)^PW1101_ID_data;  //�ֽڸ��ڸ�4λ��4λ�����ټ�5 �������ID�����ֵ���
					Blood_data[draw_n]=data_H*256+data_L;
					if(Blood_data[draw_n]>Blood_Max) Blood_Max=Blood_data[draw_n];
					else if(Blood_data[draw_n]<Blood_Min) Blood_Min=Blood_data[draw_n];
					
					data_H=(((data_cache[draw_n*16+2]&0x0f)<<4)+((data_cache[draw_n*16+2]&0xf0)>>4)-5)^PW1101_ID_data;  //�ֽڸ��ڸ�4λ��4λ�����ټ�5 �������ID�����ֵ���
					data_L=(((data_cache[draw_n*16+3]&0x0f)<<4)+((data_cache[draw_n*16+3]&0xf0)>>4)-5)^PW1101_ID_data;  //�ֽڸ��ڸ�4λ��4λ�����ټ�5 �������ID�����ֵ���
					
					HeartRate_data[draw_n]=data_H*256+data_L;
					if(HeartRate_data[draw_n]>HeartRate_Max) HeartRate_Max=HeartRate_data[draw_n];
					else if(HeartRate_data[draw_n]<HeartRate_Min) HeartRate_Min=HeartRate_data[draw_n];	
				
				}	
						
				if((Blood_Max-Blood_Min)>12800) //����200��
				{
					Blood_Compress=(Blood_Max-Blood_Min)/64;
				}
				
				if((HeartRate_Max-HeartRate_Min)>12800) //����200��
				{
					HeartRate_Compress=(HeartRate_Max-HeartRate_Min)/64;
				}
		
				
					for(draw_k=0;draw_k<3;draw_k++)  //��������ݻ�3��
					{
						for(draw_i=0;draw_i<8;draw_i++)
							for(draw_j=0;draw_j<128;draw_j++)
							Draw_Data[draw_i][draw_j]=0;     //�������ε����ݻ��������
						clear_screen();  //��������ʾ�����	
						pre1=0;
						pre2=0;
						for(draw_m=0;draw_m<125;draw_m++)  //��125����
						{
							draw_x=(Blood_data[draw_k*125+draw_m]-Blood_Min)/Blood_Compress;  //�������ݵ�ѹ��
							if(draw_x>63) draw_x=31; //���ѹ���Ժ����ݴ���63���ǵ���31�����°���������ʾ
							else draw_x/=2;  //���ѹ������С��63�����������2����ʾ���°�����

							if(pre1!=0)  //������ǻ��ĵ�һ�е�ֵ����ôҪ�ӵ�ǰ�ĵ㵽�ϴ���ʾ�ĵ㻭�ߣ���֤��ʾ�Ĳ��β���ɢ��״
							{
								if(abs(draw_x-pre1)>1)  //ͨ����ǰ�ĵ�����һ�λ��ĵ����Ҫ������λ��
								{
									if(draw_x>pre1)
									{
										for(pre_i=pre1;pre_i<draw_x;pre_i++)
												Draw_Point(pre_i,draw_m);
									}
									else
									{
										for(pre_i=draw_x;pre_i<pre1;pre_i++)
												Draw_Point(pre_i,draw_m);
									}
								}
								else
								{
									Draw_Point(draw_x,draw_m); //ֻ��һ����
								}
								
							}
							else  //ֻ��һ����
							{
								Draw_Point(draw_x,draw_m);
							}
							pre1=draw_x;
							
							draw_x=(HeartRate_data[draw_k*125+draw_m]-HeartRate_Min)/HeartRate_Compress;  //����
							if(draw_x>63) draw_x=31;
							else draw_x/=2;						
							draw_x+=32;  //�˴���32��Ϊ����ʾ�������ϰ벿�֣����ڲ���һ��


							
							if(pre2!=0)
							{
								if(abs(draw_x-pre2)>1)
								{
									if(draw_x>pre2)
									{
										for(pre_i=pre2;pre_i<draw_x;pre_i++)
												Draw_Point(pre_i,draw_m);
									}
									else
									{
										for(pre_i=draw_x;pre_i<pre2;pre_i++)
												Draw_Point(pre_i,draw_m);
									}
								}
								else
								{
									Draw_Point(draw_x,draw_m);
								}
								
							}
							else
							{
								Draw_Point(draw_x,draw_m);
							}
							pre2=draw_x;
//							delay_time(15);  //Լ5~6ms
								delay_time(13);  //Լ5~6ms   �ղ���13
						}
					}
					//////////////////////��ʾ����////////////////////////////////
							
				}
				data_group_count=0;			
			}
		}
		if(read_comm_from_queue())   //������ָ������н�����
		{
			
			if((comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_MEASURE_RESULT) && (comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE))
			{	
				//�����ɹ�
				pw1101_func_confirm_measure_result(PW1101_TRUE);
				measure_result_data[0]=comm_cache[9];
				measure_result_data[1]=comm_cache[10];
				measure_result_data[2]=comm_cache[11];
				measure_result_data[3]=comm_cache[12];
				measure_flag=MEASURE_SUCCESS;
				break;
			}
			else if (comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)//����״̬��Ϣ
			{
				total_num++;   //����ͳ�ơ�
				if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO && comm_cache[11]==0xF0)
				{
					measure_flag=MEASURE_FAIL;					
					break;
				}
				else if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)				
				{
					
					if((comm_cache[DATA_INDEX_1]!=0 ||comm_cache[DATA_INDEX_2]!=0)&&(total_num<4))
					{
						error_count_num++; //����յ��ģ��ζ�����ȷ��������쳣
						if(error_count_num==3)
						{
								pw1101_func_stop_measure();
								measure_flag=WEAR_NOT_GOOD;
								break;						
						}
					}
//					clear_screen();  //��������ʾ�����	
//					if(comm_cache[DATA_INDEX_1]&0x04) //�ĵ����ݸ��Ź���
//					{
//						Display_Waveforms_Info(10);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x08) //�ĵ���Ч���ݹ���
//					{
//						Display_Waveforms_Info(11);
//					}				
//					else if(comm_cache[DATA_INDEX_1]&0x10)  //�ĵ����ݷ��ȹ���
//					{
//						Display_Waveforms_Info(1);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x20)  //�ĵ����ݷ��ȹ���
//					{
//						Display_Waveforms_Info(2);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x40)  //�ĵ��ֵ����
//					{
//						Display_Waveforms_Info(3);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x80)  //�ĵ��ֵ����
//					{
//						Display_Waveforms_Info(4);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x01)  //�������ݷ��ȹ���
//					{
//						Display_Waveforms_Info(5);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x02)  //�������ݷ��ȹ���
//					{
//						Display_Waveforms_Info(6);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x04)  //������ֵ����
//					{
//						Display_Waveforms_Info(7);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x08)  //������ֵ����
//					{
//						Display_Waveforms_Info(8);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x10) //�ĵ����ݸ��Ź���
//					{
//						Display_Waveforms_Info(12);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x20) //������Ч���ݹ���
//					{
//						Display_Waveforms_Info(13);
//					}				
//					else if(comm_cache[DATA_INDEX_3]==0x02) //�����쳣
//					{
//						Display_Waveforms_Info(9);
//					}
//					else  //�ĵ���������
//					{
//						Display_Waveforms_Info(0);
//						error_count_num=0;
//					}
				}
			}
		}
//		if((display_point_flag==0)&&(display_page_flag==0))
//		{
//			display_point_flag=1;
//			Display_point(point_num);
//			point_num=(point_num+1)%3;
//		}
		if(Measure_time_flag==0) 
		{
			pw1101_func_stop_measure();
			measure_flag=MEASURE_FAIL;
			break;
		}
		
		
	}	
	if(cache_frist==0)  //���й����ݴ洢
	{
		//�������ݴ洢ָ�������������
		
		Updata_measure_data_add_ptr();
		
	}
	return measure_flag;
//	pw1101_func_confirm_measure_result(PW1101_TRUE);
//	pw1101_func_stop_measure();
}

//��̨���Զ�����
uint8_t pw1101_func_auto_measure(void)
{
	uint8_t point_num=2;
	uint8_t measure_flag=0;
	uint8_t error_count_num=0;
	uint8_t total_num=0; 
	
	uint8_t display_page_flag=0;
//	uint8_t data_cache[6000];
	uint32_t cache_index=0;
	uint8_t cache_frist=1;
	uint8_t data_head_flag[20]={0xaa,0xbb,0xcc,0xdd,0xee,0x11,0x22,0x33,0x44,0x55,0,0,0,0,0,0xfc,0xfc,0xfc,0xfc,0xfc};
	uint32_t curtime;
		
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	if(Measure_Mode==MEASURE_MODE4) //���Ϊ�������ݵĲ�����ʽ
	{
		Erase_Next_Sector(); //�����һ�������������ݵı���
	}
	PW1101_Comm_RxQueue_Init();  //��ʼ�����ն��У����
	
	
	
		//���ò���ģʽ
	while(1)
	{
		if(read_comm_from_queue())    //�Ӷ����л�ȡ����
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{	
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Set_MeasureMode();  //���ò���ģʽ
		Delay_S(1);
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
		
		
	//�����û�����
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{	
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Set_UserPara();
		Delay_S(1);
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	//���ò�������
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{
				break;
			}
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_Measure_Para(MeasurePara.Groupnum-4,MeasurePara.Groupnum,MeasurePara.Timeinterval,MeasurePara.Timelong,Current_Eletrode);
		Delay_S(1);	
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	
	//��ѯ״̬
	while(1)
	{
		//��ȡ�Ӷ����н���ָ�
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE 
				&& (comm_cache[DATA_INDEX_1]==1 || comm_cache[DATA_INDEX_1]==4 || comm_cache[DATA_INDEX_1]==7))
			{
				break;
			}
			Pw1101_interact_timeout=0;
			Pw1101_interact_flag=1;
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_status_check();
		Delay_S(1);	
	}		
	Delay_S(1);
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	
	//��ʼ����
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE && comm_cache[DATA_INDEX_1]==1)
			{
				break;
			}
			Pw1101_interact_timeout=0;
			Pw1101_interact_flag=1;
		}	
		if(Pw1101_interact_timeout>INTERACT_TIMEOUT)
		{			
				Pw1101_interact_flag=0;
				Pw1101_interact_timeout=0;
				return INTERACT_FAIL;
		}
		pw1101_func_start_measure();
		Delay_S(1);
	}	
	display_point_flag=1;
	Measure_time_flag= MEASURE_TIMEOUT;	
	while(1)
	{
		/*
		�������ݵĴ洢
		*/
		if(Measure_Mode==MEASURE_MODE4) //���Ϊ�������ݵĲ�����ʽ
		{
				//���´���ʵ�ֲ����������ݵĴ洢
			if(data_group_count==30)
			{	//һ������6k�����֧��11��
				for(cache_index=0;cache_index<6000;cache_index++)
				{
					if(PW1101_Data_QueueOut(&g_DataRxQueue,&data_cache[cache_index])== QueueEmpty)
					{
						break;
					}
				}
				
				
				if(cache_index!=0)  //�Ӷ������õ���ֵ
				{
					if(cache_frist)
					{
						curtime=RTC->TSR;				
						data_head_flag[10]=(curtime>>24)&0xff;  //ʱ���
						data_head_flag[11]=(curtime>>16)&0xff;
						data_head_flag[12]=(curtime>>8)&0xff;
						data_head_flag[13]=(curtime)&0xff;
						data_head_flag[14]=GMT_num;
						cache_frist=0;
						
						
						if((FLASH_TOTAL_ADD-measure_data_save_ptr)<20)
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,FLASH_TOTAL_ADD-measure_data_save_ptr);
							measure_data_save_ptr=Measure_Data_Begin;
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20-(FLASH_TOTAL_ADD-measure_data_save_ptr));
							measure_data_save_ptr+=(20-(FLASH_TOTAL_ADD-measure_data_save_ptr));	
						}
						else
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20);
							measure_data_save_ptr+=20;	
						}						
					}
					if((FLASH_TOTAL_ADD-measure_data_save_ptr)<cache_index)
					{
						S25FL512S_Write(data_cache,measure_data_save_ptr,FLASH_TOTAL_ADD-measure_data_save_ptr);
						measure_data_save_ptr=Measure_Data_Begin;
						S25FL512S_Write(data_cache,measure_data_save_ptr,cache_index-(FLASH_TOTAL_ADD-measure_data_save_ptr));
						measure_data_save_ptr+=(cache_index-(FLASH_TOTAL_ADD-measure_data_save_ptr));	
					}
					else
					{
						S25FL512S_Write(data_cache,measure_data_save_ptr,cache_index);
						measure_data_save_ptr+=cache_index;	
					}	
				}			
				data_group_count=0;			
			}				
		}
		if(read_comm_from_queue())
		{

			if((comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_MEASURE_RESULT) && (comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE))
			{	
				pw1101_func_confirm_measure_result(PW1101_TRUE);
				measure_result_data[0]=comm_cache[9];
				measure_result_data[1]=comm_cache[10];
				measure_result_data[2]=comm_cache[11];
				measure_result_data[3]=comm_cache[12];
				measure_flag=MEASURE_SUCCESS;
				break;
			}
			else if (comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)
			{
				total_num++;
				if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO && comm_cache[11]==0xF0)
				{
					measure_flag=MEASURE_FAIL;					
					break;
				}
				else if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)				
				{
					
					if((comm_cache[DATA_INDEX_1]!=0 ||comm_cache[DATA_INDEX_2]!=0)&&(total_num<4))
					{
						error_count_num++;
						if(error_count_num==3)
						{
								pw1101_func_stop_measure();
								measure_flag=WEAR_NOT_GOOD;
								break;						
						}
					}					
				}
			}
		}
		if(Measure_time_flag==0) 
		{
			pw1101_func_stop_measure();
			measure_flag=MEASURE_FAIL;
			break;
		}	
	}	
	if(Measure_Mode==MEASURE_MODE4) //���Ϊ�������ݵĲ�����ʽ
	{
		if(cache_frist==0)  //���й����ݴ洢
		{
			//�������ݴ洢ָ�������������	
			Updata_measure_data_add_ptr();		
		}
	}
	return measure_flag;	
//	pw1101_func_stop_measure();
}
