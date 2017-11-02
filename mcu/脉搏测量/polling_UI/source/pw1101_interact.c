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

//给PW1101发送数据
void PW1101_SEND_DATA(uint8_t *data,uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		PW1101_UART_Send_Byte(data[i]);
	}
}


//异或校验函数
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



//读取的PW1101 ID 的存储变量
uint32_t PW1101_ID_H;
uint32_t PW1101_ID_MH;
uint32_t PW1101_ID_ML;
uint32_t PW1101_ID_L;
uint8_t  PW1101_ID_data=0;   //16字节的异或值

uint16_t Software_version=0x0001;
uint16_t Algorithm_version=0x0001;

//读取IC信息 包括ID号，版本号等
uint8_t read_PW1101Information (void)
{
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	PW1101_Comm_RxQueue_Init();
	while(1)
	{
		if(read_comm_from_queue())
		{
			if(comm_cache[COMMAND_ID_INDEX]==PW1101_txbuff[COMMAND_ID_INDEX] &&comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE)  //如果发送的指令和接收的指令类型一样，且收到正常的回复
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
		PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
		PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
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


//pw1101状态检测
void pw1101_func_status_check(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_STATUS_CHENK;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[9+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}




//设置用户信息
void pw1101_func_Set_UserPara(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_USER_PARA;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
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


//设置测量模式，根据用户设置的模式进行设置
void pw1101_func_Set_MeasureMode(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_MEASURE_MODE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0x01;
	PW1101_txbuff[DATA_INDEX_1]=Measure_Mode;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}

//函数传入最快出结果组数、最多采样组数、采样时长、采样间隔、电极使用情况
void pw1101_func_Measure_Para(uint8_t para1,uint8_t para2,uint8_t para3,uint8_t para4,uint8_t para5)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_MEASURE_PARA;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
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

//开始测量
void pw1101_func_start_measure(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_START_MEASURE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}


//停止测量
void pw1101_func_stop_measure(void)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_STOP_MEASURE;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=0;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}


//收到测量结果后，进行确认
void pw1101_func_confirm_measure_result(uint8_t TorF)
{
	PW1101_txbuff[COMMAND_ID_INDEX]=COMMAND_ID_SEND_MEASURE_RESULT;
	PW1101_txbuff[DEVICE_ID_INDEX]=DEVICE_ID_ELE_BPG;
	PW1101_txbuff[COUNT_ID_INDEX_H]=command_count_num>>8; //指令包统计高8位
	PW1101_txbuff[COUNT_ID_INDEX_L]=command_count_num;    //指令包统计低8位
	PW1101_txbuff[STATUS_ID_INDEX_H]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_L]=0x00;
	PW1101_txbuff[STATUS_ID_INDEX_TorF]=PW1101_TRUE;
	PW1101_txbuff[DATA_LEN_INDEX]=1;
	PW1101_txbuff[DATA_INDEX_1]=TorF;
	PW1101_txbuff[DATA_LEN_INDEX+1+PW1101_txbuff[DATA_LEN_INDEX]]=Check_CRC8(PW1101_txbuff);
	PW1101_SEND_DATA(PW1101_txbuff,10+PW1101_txbuff[DATA_LEN_INDEX]);	
}



//做1ms的延时，此延时并不精确，只在需要不精确延时可以用，更系统的时钟有很大关系
void delay_time(uint32_t num)
{
	uint32_t i,j;
	for(i=0;i<num;i++)
		for(j=0;j<10000;j++);
}





//测量模式选择，现在只支持正常测量和完整数据测量
uint8_t pw1101_func_manual_measure_mode_select(uint32_t para)
{
	switch(Measure_Mode)
	{
		case MEASURE_MODE1:
			break;
		case MEASURE_MODE2:
			return pw1101_func_manual_measure(1);  //无数据测量　
			break;
		case MEASURE_MODE3:
			break;
		case MEASURE_MODE4:
			return pw1101_func_manual_measure(1);  //完整数据测量　
			break;
		default:
			break;
	}
	return 0;
}

//进行手动测量

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
		
		
	uint16_t Blood_data[375];  //存取出的脉搏数据
	uint16_t HeartRate_data[375]; //存取出的心电数据
	uint16_t Blood_Max=0;   //缓存脉搏最大值
	uint16_t Blood_Min=65535;  //缓存脉搏最小值
	uint16_t HeartRate_Max=0;  //心电最大值
	uint16_t HeartRate_Min=65535;  //心电最小值
	uint16_t Blood_Compress=200;  //脉搏压缩比
	uint16_t HeartRate_Compress=200;  //心电压缩比	
		
		
	Display_point(point_num);
	point_num=(point_num+1)%3;
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
	if(Measure_Mode==MEASURE_MODE4) //如果是完整数据的测量方式
	{
		Erase_Next_Sector(); //清除下一个块来进行数据的保存
	}
	PW1101_Comm_RxQueue_Init();
	//设置测量模式
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
		
		
	//设置用户参数
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
	
	//设置测量参数
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
	
	//查询状态
	while(1)
	{
		//调取从队列中接收指令串
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
	
	//开始测量
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
		进行数据的存储
		*/
		if(Measure_Mode==MEASURE_MODE4) //如果为完整数据的测量方式
		{
			if(data_group_count==30)//因现在是测量完3S以后传数据，数据为30组，收够数再进行处理。此处是在原方案基础上修改，因没有交互传输，有一定的分险
			{
				//以下代码实现测据的存储
				for(cache_index=0;cache_index<6000;cache_index++)
				{
					if(PW1101_Data_QueueOut(&g_DataRxQueue,&data_cache[cache_index])== QueueEmpty)
					{
						break;
					}
				}
				if(cache_index!=0)  //从队列中拿到了值
				{
					if(cache_frist) //如果为第一包数据，则进行数据存储时，要加时间信息，20个字节
					{
						curtime=RTC->TSR;				
						data_head_flag[10]=(curtime>>24)&0xff;  //时间戳
						data_head_flag[11]=(curtime>>16)&0xff;
						data_head_flag[12]=(curtime>>8)&0xff;
						data_head_flag[13]=(curtime)&0xff;
						data_head_flag[14]=GMT_num;
						cache_frist=0;
						
						
						if((FLASH_TOTAL_ADD-measure_data_save_ptr)<20)  //如果数据区不存储20个字节数据
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,FLASH_TOTAL_ADD-measure_data_save_ptr);
							measure_data_save_ptr=Measure_Data_Begin;  //跳到开始位置
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20-(FLASH_TOTAL_ADD-measure_data_save_ptr));
							measure_data_save_ptr+=(20-(FLASH_TOTAL_ADD-measure_data_save_ptr));	
						}
						else
						{
							S25FL512S_Write(data_head_flag,measure_data_save_ptr,20);
							measure_data_save_ptr+=20;	
						}						
					}   //时间信息
					
					
					
					if((FLASH_TOTAL_ADD-measure_data_save_ptr)<cache_index)  //如果数据区不够存储数据，同样跳到开始位置
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
					
					
			////////////////显示波形/////////////////////////////////////////////////////
				Blood_Max=0;   //缓存脉搏最大值
				Blood_Min=65535;  //缓存脉搏最小值
				HeartRate_Max=0;  //心电最大值
				HeartRate_Min=65535;  //心电最小值
				Blood_Compress=200;  //脉搏压缩比
				HeartRate_Compress=200;  //心电压缩比	
				for(draw_n=0;draw_n<375;draw_n++)  //从1500组数据中提取出375个数
				{
					//脉搏数据
					data_H=(((data_cache[draw_n*16]&0x0f)<<4)+((data_cache[draw_n*16]&0xf0)>>4)-5)^PW1101_ID_data;  //字节高内高4位低4位倒序，再减5 ，最后与ID的异或值异或
					data_L=(((data_cache[draw_n*16+1]&0x0f)<<4)+((data_cache[draw_n*16+1]&0xf0)>>4)-5)^PW1101_ID_data;  //字节高内高4位低4位倒序，再减5 ，最后与ID的异或值异或
					Blood_data[draw_n]=data_H*256+data_L;
					if(Blood_data[draw_n]>Blood_Max) Blood_Max=Blood_data[draw_n];
					else if(Blood_data[draw_n]<Blood_Min) Blood_Min=Blood_data[draw_n];
					
					data_H=(((data_cache[draw_n*16+2]&0x0f)<<4)+((data_cache[draw_n*16+2]&0xf0)>>4)-5)^PW1101_ID_data;  //字节高内高4位低4位倒序，再减5 ，最后与ID的异或值异或
					data_L=(((data_cache[draw_n*16+3]&0x0f)<<4)+((data_cache[draw_n*16+3]&0xf0)>>4)-5)^PW1101_ID_data;  //字节高内高4位低4位倒序，再减5 ，最后与ID的异或值异或
					
					HeartRate_data[draw_n]=data_H*256+data_L;
					if(HeartRate_data[draw_n]>HeartRate_Max) HeartRate_Max=HeartRate_data[draw_n];
					else if(HeartRate_data[draw_n]<HeartRate_Min) HeartRate_Min=HeartRate_data[draw_n];	
				
				}	
						
				if((Blood_Max-Blood_Min)>12800) //大于200倍
				{
					Blood_Compress=(Blood_Max-Blood_Min)/64;
				}
				
				if((HeartRate_Max-HeartRate_Min)>12800) //大于200倍
				{
					HeartRate_Compress=(HeartRate_Max-HeartRate_Min)/64;
				}
		
				
					for(draw_k=0;draw_k<3;draw_k++)  //三秒的数据画3屏
					{
						for(draw_i=0;draw_i<8;draw_i++)
							for(draw_j=0;draw_j<128;draw_j++)
							Draw_Data[draw_i][draw_j]=0;     //给画波形的数据缓存区清空
						clear_screen();  //把屏的显示区清空	
						pre1=0;
						pre2=0;
						for(draw_m=0;draw_m<125;draw_m++)  //画125个点
						{
							draw_x=(Blood_data[draw_k*125+draw_m]-Blood_Min)/Blood_Compress;  //进行数据的压缩
							if(draw_x>63) draw_x=31; //如果压缩以后数据大于63，那等下31，在下半屏进行显示
							else draw_x/=2;  //如果压缩数据小于63，则进行整除2，显示到下半屏中

							if(pre1!=0)  //如果不是画的第一列的值，那么要从当前的点到上次显示的点画线，保证显示的波形不会散点状
							{
								if(abs(draw_x-pre1)>1)  //通过当前的点与上一次画的点计算要画的了位置
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
									Draw_Point(draw_x,draw_m); //只画一个点
								}
								
							}
							else  //只画一个点
							{
								Draw_Point(draw_x,draw_m);
							}
							pre1=draw_x;
							
							draw_x=(HeartRate_data[draw_k*125+draw_m]-HeartRate_Min)/HeartRate_Compress;  //分屏
							if(draw_x>63) draw_x=31;
							else draw_x/=2;						
							draw_x+=32;  //此处加32是为了显示在屏的上半部分，其于部分一样


							
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
//							delay_time(15);  //约5~6ms
								delay_time(13);  //约5~6ms   刚才是13
						}
					}
					//////////////////////显示结束////////////////////////////////
							
				}
				data_group_count=0;			
			}
		}
		if(read_comm_from_queue())   //从脉冲指令队列中接收数
		{
			
			if((comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_MEASURE_RESULT) && (comm_cache[STATUS_ID_INDEX_TorF]==PW1101_TRUE))
			{	
				//测量成功
				pw1101_func_confirm_measure_result(PW1101_TRUE);
				measure_result_data[0]=comm_cache[9];
				measure_result_data[1]=comm_cache[10];
				measure_result_data[2]=comm_cache[11];
				measure_result_data[3]=comm_cache[12];
				measure_flag=MEASURE_SUCCESS;
				break;
			}
			else if (comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)//返回状态信息
			{
				total_num++;   //进行统计　
				if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO && comm_cache[11]==0xF0)
				{
					measure_flag=MEASURE_FAIL;					
					break;
				}
				else if(comm_cache[COMMAND_ID_INDEX] == COMMAND_ID_SEND_WAVEFORMS_INFO)				
				{
					
					if((comm_cache[DATA_INDEX_1]!=0 ||comm_cache[DATA_INDEX_2]!=0)&&(total_num<4))
					{
						error_count_num++; //如果收到的３次都不正确，测佩戴异常
						if(error_count_num==3)
						{
								pw1101_func_stop_measure();
								measure_flag=WEAR_NOT_GOOD;
								break;						
						}
					}
//					clear_screen();  //把屏的显示区清空	
//					if(comm_cache[DATA_INDEX_1]&0x04) //心电数据干扰过多
//					{
//						Display_Waveforms_Info(10);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x08) //心电有效数据过少
//					{
//						Display_Waveforms_Info(11);
//					}				
//					else if(comm_cache[DATA_INDEX_1]&0x10)  //心电数据幅度过高
//					{
//						Display_Waveforms_Info(1);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x20)  //心电数据幅度过低
//					{
//						Display_Waveforms_Info(2);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x40)  //心电差值过高
//					{
//						Display_Waveforms_Info(3);
//					}
//					else if(comm_cache[DATA_INDEX_1]&0x80)  //心电差值过低
//					{
//						Display_Waveforms_Info(4);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x01)  //脉搏数据幅度过高
//					{
//						Display_Waveforms_Info(5);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x02)  //脉搏数据幅度过低
//					{
//						Display_Waveforms_Info(6);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x04)  //脉搏差值过高
//					{
//						Display_Waveforms_Info(7);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x08)  //脉搏差值过低
//					{
//						Display_Waveforms_Info(8);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x10) //心电数据干扰过多
//					{
//						Display_Waveforms_Info(12);
//					}
//					else if(comm_cache[DATA_INDEX_2]&0x20) //脉搏有效数据过少
//					{
//						Display_Waveforms_Info(13);
//					}				
//					else if(comm_cache[DATA_INDEX_3]==0x02) //计算异常
//					{
//						Display_Waveforms_Info(9);
//					}
//					else  //心电脉搏正常
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
	if(cache_frist==0)  //进行过数据存储
	{
		//更新数据存储指针索引块的数据
		
		Updata_measure_data_add_ptr();
		
	}
	return measure_flag;
//	pw1101_func_confirm_measure_result(PW1101_TRUE);
//	pw1101_func_stop_measure();
}

//后台的自动测量
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
	if(Measure_Mode==MEASURE_MODE4) //如果为完整数据的测量方式
	{
		Erase_Next_Sector(); //清除下一个块来进行数据的保存
	}
	PW1101_Comm_RxQueue_Init();  //初始化接收队列，清空
	
	
	
		//设置测量模式
	while(1)
	{
		if(read_comm_from_queue())    //从队列中获取数据
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
		pw1101_func_Set_MeasureMode();  //设置测量模式
		Delay_S(1);
	}	
	Pw1101_interact_timeout=0;
	Pw1101_interact_flag=1;
		
		
	//设置用户参数
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
	
	//设置测量参数
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
	
	
	//查询状态
	while(1)
	{
		//调取从队列中接收指令串
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
	
	//开始测量
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
		进行数据的存储
		*/
		if(Measure_Mode==MEASURE_MODE4) //如果为完整数据的测量方式
		{
				//以下代码实现测量反返数据的存储
			if(data_group_count==30)
			{	//一条参数6k，最多支持11条
				for(cache_index=0;cache_index<6000;cache_index++)
				{
					if(PW1101_Data_QueueOut(&g_DataRxQueue,&data_cache[cache_index])== QueueEmpty)
					{
						break;
					}
				}
				
				
				if(cache_index!=0)  //从队列中拿到了值
				{
					if(cache_frist)
					{
						curtime=RTC->TSR;				
						data_head_flag[10]=(curtime>>24)&0xff;  //时间戳
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
	if(Measure_Mode==MEASURE_MODE4) //如果为完整数据的测量方式
	{
		if(cache_frist==0)  //进行过数据存储
		{
			//更新数据存储指针索引块的数据	
			Updata_measure_data_add_ptr();		
		}
	}
	return measure_flag;	
//	pw1101_func_stop_measure();
}
