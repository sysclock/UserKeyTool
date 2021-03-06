#include "flash_spi.h"
#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_dspi.h"
#include "uart.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "rtc.h"


#define EXAMPLE_DSPI_FLASH_BASEADDR SPI0
#define DSPI_FLASH_CLK_SRC DSPI0_CLK_SRC
#define EXAMPLE_DSPI_FLASH_PCS_FOR_INIT kDSPI_Pcs0
#define EXAMPLE_DSPI_FLASH_PCS_FOR_TRANSFER kDSPI_MasterPcs0

#define TRANSFER_BAUDRATE 500000U

#define TRANSFER_SIZE 256U   
uint8_t flashTxData[TRANSFER_SIZE] = {0U};
uint8_t flashRxData[1]={0};
dspi_transfer_t flashXfer;



//
//来源官方网站的例程
////PTA14  SPI-CS配置为输出管脚；看原理图
//外部地址扩展使能，写1才能操作512MBITS
void S25FL512S_Spi_Config(void)
{
	uint32_t srcClock_Hz;
	dspi_master_config_t masterConfig;
	//来源官方网站的例程
	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame = 8U;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	
	masterConfig.whichPcs = EXAMPLE_DSPI_FLASH_PCS_FOR_INIT;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;
		
	srcClock_Hz = CLOCK_GetFreq(DSPI_FLASH_CLK_SRC);

	
  	DSPI_MasterInit(EXAMPLE_DSPI_FLASH_BASEADDR, &masterConfig, srcClock_Hz);
	flashXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_FLASH_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
	
	gpio_pin_config_t IO_config = {
        kGPIO_DigitalOutput, 0,
    };
	//PTA14  SPI-CS配置为输出管脚；看原理图
	GPIO_PinInit(GPIOA, PIN14_IDX, &IO_config);

	//使能S25FL512S_BRWR  0x17,为了操作控制寄存器	
	//外部地址扩展使能，写1才能操作512MBITS
	S25FL512S_WriteBR(0x80);		
}



//////////////////////////////////////////////
//函数功能：进行单字节数据交互
//输入参数：要发送的字节
//输出参数：发送字节时读到的字节
//////////////////////////////////////////////
uint8_t S25FL512S_ReadWriteByte(uint8_t TxData)
{
	flashXfer.txData =& TxData;
  	flashXfer.dataSize = 1;
	flashXfer.rxData =flashRxData;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_FLASH_BASEADDR, &flashXfer);
	return *flashXfer.rxData; 
}

//读取状态寄存器1， 主要是为了读取0位的BUSY状态位
uint8_t S25FL512S_ReadSR1(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_RDSR1);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}

//写状态寄存器1
void S25FL512S_WriteSR1(uint8_t sr)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRSR1);
	S25FL512S_ReadWriteByte(sr);
	S25FL512S_CS_H;
}

//读取Bank Address Register，此寄存器的7位EXTADD标示了四字节地址或者三字节地址
//当这1时为四字节地址，为0时则为三字节配合低0、1位进行询址。0位对应512Mb的A24
//1位对应512Mb的A25，通过26位地址实现512Mb的询址能力。
uint8_t S25FL512S_ReadBR(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRRD);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}
//通过此寄存器的配置，实现512Mb的高地址操作
void S25FL512S_WriteBR(uint8_t br)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRWR);
	S25FL512S_ReadWriteByte(br);
	S25FL512S_CS_H;
}

//使能写操作
void S25FL512S_Write_Enable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WREN);
	S25FL512S_CS_H;
}
//失能写操作
void S25FL512S_Write_Disable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRDI);
	S25FL512S_CS_H;
}

//等待BUSY空闲
void S25FL512S_Wait_Busy(void)
{
	while((S25FL512S_ReadSR1()&0x01)==0x01); //等待BUSY位清空
}


//读取SPI FLASH  ;
//读地址
//读数据长度
//读出的数据送入缓冲区中
void S25FL512S_Read(uint8_t * pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead)   
{ 
		uint32_t i;  
		S25FL512S_CS_L;                            //使能器件   
    S25FL512S_ReadWriteByte(S25FL512S_RD);         //发送读取命令
		S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>24));  //发送32bit地址
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>16));   
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>8));  
    S25FL512S_ReadWriteByte((uint8_t)ReadAddr);  
		
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=S25FL512S_ReadWriteByte(0XFF);   //循环读数  
    }
		S25FL512S_CS_H; 			    	      
}  




//写入少于512个字节的数据
void S25FL512S_Write_Page(uint8_t * pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint16_t i;
	S25FL512S_Write_Enable();
	S25FL512S_Wait_Busy(); 
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_PP);
	S25FL512S_ReadWriteByte((uint8_t)(WriteAddr>>24));
	S25FL512S_ReadWriteByte((uint8_t)(WriteAddr>>16));
	S25FL512S_ReadWriteByte((uint8_t)(WriteAddr>>8));
	S25FL512S_ReadWriteByte((uint8_t)WriteAddr);
	for(i=0;i<NumByteToWrite;i++)
		S25FL512S_ReadWriteByte(pBuffer[i]);
	S25FL512S_CS_H;
	S25FL512S_Wait_Busy();
}


////无检验 S25FL512S SPI FLASH 
void S25FL512S_Write_NoCheck(uint8_t * pBuffer,uint32_t WriteAddr,uint32_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=512-WriteAddr%512; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于512个字节
	while(1)
	{	   
		S25FL512S_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>512)pageremain=512; //一次可以写512个字节
			else pageremain=NumByteToWrite; 	  //不够512个字节了
		}
	}	    
} 






//写一串数据到制定的flash地址中

void S25FL512S_Write(uint8_t * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
	uint32_t secpos;
	uint32_t secoff;
	uint32_t secremain;	     

 	secpos=WriteAddr/(1024*256);//扇区地址  每个扇区是256K字节256x1024=nnn byte
	secoff=WriteAddr%(1024*256);//在扇区内的偏移
	secremain=(1024*256)-secoff;//扇区剩余空间大小   
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于(1024*256)个字节
	while(1) 
	{	
		S25FL512S_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 
			pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
			NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>(1024*256))secremain=1024*256;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	}	 
}


//传入要擦除的扇区号
void S25FL512S_Erase_Sector(uint32_t Dst_Addr)   
{  
		Dst_Addr=Dst_Addr*256*1024;     //256K字节
    S25FL512S_Write_Enable();                  //SET WEL 	 
    S25FL512S_Wait_Busy();   
  	S25FL512S_CS_L;                          //使能器件   
    S25FL512S_ReadWriteByte(S25FL512S_SE);      //发送扇区擦除指令 
  	S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>24));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>16));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>8));
		S25FL512S_ReadWriteByte((uint8_t)Dst_Addr);
		S25FL512S_CS_H;                              //取消片选     	      
    S25FL512S_Wait_Busy();				   //等待擦除完成
}  







//把参数保存到flash中
uint8_t user_set_para[SET_PARA_TOTAL_NUM];

//flash必须先擦除才能改写；擦的作用的是把存储区域的内容全部变为0xff，写数据的时候才能把0xff变为目标值
//目前的操作方法256kbyte/sec，相关的group占用一个sector，然后浪费
//mcu的ram只有256kbyte
void Sava_para_to_flash(void)
{
	S25FL512S_Erase_Sector(0);
	S25FL512S_Write(user_set_para,0,SET_PARA_TOTAL_NUM);
}




//保存默认的测量参数到存储区中的扇区0
//这个参数组占用一个扇区，存储到0号扇区
//change name to Save_para_as_default_Value is better
void Save_para_as_define(void)  
{
		user.age=50;   //用户年龄
		user.sex=1;  //用户性别  1男  2 女
		user.height=170; //身高 
		user.weight=70;  //体重
		
		alarm.HBlood_LowValue=80;   //高压下限
		alarm.HBlood_HighValue=180;  //高压上限
		alarm.LBlood_LowValue=40;    //低压下限
		alarm.LBlood_HighValue=90;   //低压上限
		alarm.HeartRate_LowValue=60;  //心率下限
		alarm.HeartRate_HighValue=160;  //心率上限
		
		MeasurePara.Groupnum=11;  //最大测量组数
		MeasurePara.Timelong=3;   //测量时长3s
		MeasurePara.Timeinterval=2; //测量间隔2ms
		MeasurePara.Eletrode=1;    //测量电极
		
		Measure_Mode=4;    //4带完整数据测量   2不带数据测量   
		
		AutoMeasurePara.Timelong=24;        //自动测量时长 小时
		AutoMeasurePara.Timeinterval=10;    //自动测量间隔分钟
		
		StandbyTime=30;  //待机30秒
		GMT_num=8;      //时区
		mmHg_or_kPa=1;  //显示单位   1 mmHg   kPa
		Ble_OnOff=2;    //蓝牙开关，默认关
		NoDisturb_Mode=1;   //防打扰
		
		user_set_para[SET_PARA_AGE_ADD]=user.age;
		user_set_para[SET_PARA_SEX_ADD]=user.sex;
		user_set_para[SET_PARA_HEIGHT_ADD]=user.height;
		user_set_para[SET_PARA_WEIGHT_ADD]=user.weight;
		
		user_set_para[SET_PARA_HB_L_ADD]=(alarm.HBlood_LowValue>>8)&0xff;
		user_set_para[SET_PARA_HB_L_ADD+1]=alarm.HBlood_LowValue&0xff;
		user_set_para[SET_PARA_HB_H_ADD]=(alarm.HBlood_HighValue>>8)&0xff;
		user_set_para[SET_PARA_HB_H_ADD+1]=alarm.HBlood_HighValue&0xff;
		
		user_set_para[SET_PARA_LB_L_ADD]=alarm.LBlood_LowValue;
		user_set_para[SET_PARA_LB_H_ADD]=alarm.LBlood_HighValue;
		user_set_para[SET_PARA_HR_L_ADD]=alarm.HeartRate_LowValue;
		user_set_para[SET_PARA_HR_H_ADD]=alarm.HeartRate_HighValue;
		
		user_set_para[SET_PARA_MEASURE_GROUPNUM_ADD]=MeasurePara.Groupnum;
		user_set_para[SET_PARA_MEASURE_TIMELONG_ADD]=MeasurePara.Timelong;
		user_set_para[SET_PARA_MEASURE_TIMEINTERVAL_ADD]=MeasurePara.Timeinterval;
		user_set_para[SET_PARA_MEASURE_ELETRODE_ADD]=MeasurePara.Eletrode;
		
		user_set_para[SET_PARA_MEASURE_MODE_ADD]=Measure_Mode;
		
		user_set_para[SET_PARA_AUTOMEASURE_TIMELONG_ADD]=AutoMeasurePara.Timelong;
		user_set_para[SET_PARA_AUTOMEASURE_TIMEINTERVAL_ADD]=AutoMeasurePara.Timeinterval;
		
		user_set_para[SET_PARA_STANDBYTIME_ADD]=StandbyTime;
		user_set_para[SET_PARA_GMT_NUM_ADD]=GMT_num;
		
		user_set_para[SET_PARA_mmgH_kPa_ADD]=mmHg_or_kPa;
		
		user_set_para[SET_PARA_BLE_OnOff_ADD]=Ble_OnOff;
		user_set_para[SET_PARA_NoDisturb_Mode_ADD]=NoDisturb_Mode;
	
		Sava_para_to_flash();
}




//从flash存储器的0号扇区块读取用户设置参数  ；规定整个扇区存储用户设置参数
//读取到user_set_para数组中,user_set_para数组各元素偏移在flash_spi.h中由_FLASH_ADD_TYPE的枚举定义
void Read_para_from_flash(void)
{
	S25FL512S_Read(user_set_para,0,SET_PARA_TOTAL_NUM);
	
	user.age=user_set_para[SET_PARA_AGE_ADD];
	user.sex=user_set_para[SET_PARA_SEX_ADD];
	user.height=user_set_para[SET_PARA_HEIGHT_ADD];
	user.weight=user_set_para[SET_PARA_WEIGHT_ADD];
	
	alarm.HBlood_LowValue=(user_set_para[SET_PARA_HB_L_ADD]<<8)+user_set_para[SET_PARA_HB_L_ADD+1];
	alarm.HBlood_HighValue=(user_set_para[SET_PARA_HB_H_ADD]<<8)+user_set_para[SET_PARA_HB_H_ADD+1];
	alarm.LBlood_LowValue=user_set_para[SET_PARA_LB_L_ADD];
	alarm.LBlood_HighValue=user_set_para[SET_PARA_LB_H_ADD];
	alarm.HeartRate_LowValue=user_set_para[SET_PARA_HR_L_ADD];
	alarm.HeartRate_HighValue=user_set_para[SET_PARA_HR_H_ADD];
	
	MeasurePara.Groupnum=user_set_para[SET_PARA_MEASURE_GROUPNUM_ADD];
	MeasurePara.Timelong=user_set_para[SET_PARA_MEASURE_TIMELONG_ADD];
	MeasurePara.Timeinterval=user_set_para[SET_PARA_MEASURE_TIMEINTERVAL_ADD];
	MeasurePara.Eletrode=user_set_para[SET_PARA_MEASURE_ELETRODE_ADD];
	
	Measure_Mode=user_set_para[SET_PARA_MEASURE_MODE_ADD];
	
	AutoMeasurePara.Timelong=user_set_para[SET_PARA_AUTOMEASURE_TIMELONG_ADD];
	AutoMeasurePara.Timeinterval=user_set_para[SET_PARA_AUTOMEASURE_TIMEINTERVAL_ADD];
	
	StandbyTime=user_set_para[SET_PARA_STANDBYTIME_ADD];
	GMT_num=user_set_para[SET_PARA_GMT_NUM_ADD];
	mmHg_or_kPa=user_set_para[SET_PARA_mmgH_kPa_ADD];
	
	Ble_OnOff=user_set_para[SET_PARA_BLE_OnOff_ADD];
	NoDisturb_Mode=user_set_para[SET_PARA_NoDisturb_Mode_ADD];
	
	if(user.age==0xff &&user.sex==0xff)	//用户没有设置国参数
	{
		Save_para_as_define();	
	}	
	
	
}	





uint32_t Save_Measure_Index_Ptr=3;

uint8_t READ_Record_data_cache[RECORD_NUM*16];


//存入一条测量结果，一条结果为16个字节；flash可以按直接写入对于擦过之后的区域
//存的内容为：0xfc(一条新记录) RTC时间（4字节） 时区1字节  测量结果4字节 用户信息4字节  空两个0xff预留
uint8_t Save_Measure_Result_To_Flash (uint32_t time, uint8_t *data)
{
	uint8_t write_data[16]={0};
	
	write_data[0]=save_state_new;    //存入一条新数据，以后可以判断数据是否存在，
	time++;                         //加入一秒时间，因反算的时候少一秒
	write_data[1]=(time>>24)&0xff;  //时间戳
	write_data[2]=(time>>16)&0xff;
	write_data[3]=(time>>8)&0xff;
	write_data[4]=(time)&0xff;
	write_data[5]=GMT_num;
	write_data[6]=data[0];//来自脉感芯片
	write_data[7]=data[1];
	write_data[8]=data[2];
	write_data[9]=data[3];
	if(user.sex==1) //表示男
		write_data[10]=0x01;
	else 
		write_data[10]=0x00;
	
	write_data[11]=user.age;
	write_data[12]=user.height;
	write_data[13]=user.weight;
	
	if(Save_Measure_Index_Ptr<16383) //256KB   分成 16384个16字节小区域，进行第一条记录的存储
	{
		Save_Measure_Index_Ptr++;
	}
	else //如果已经存完一个整块////块==256KByte==扇区
	{//ptr>16383，证明当前块已经写满，跳转到块头，重新写
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-RECORD_NUM)*16,16*RECORD_NUM);  //从尾巴上读出数据读出来600条，保证不被擦除
		S25FL512S_Erase_Sector(SAVE_MEASURE_RESULT_SECTOR);//擦除整个扇区/块
		S25FL512S_Write(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(4)*16,16*RECORD_NUM);  //读出来的数据写入扇区的开头的部分
		////S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16); //在块的开头写一组数据，只为了获取是否有写满过一次
		Save_Measure_Index_Ptr=3+RECORD_NUM;    //预留出4个块，此处等于4，相当于从第四块存储，0、1、2、3进行了预留		
	}
	
	S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr)*16,16);  //写入一条测量结果
	return 1;
}



uint32_t  record_begin_index;
uint32_t  record_total_num;
uint32_t  display_record_index;


//读出测量结果  不包括测量数据
//翻查数据或上传记录的时候，读�
//从flash中读出的数据，放在cache中，cache是尾对齐的，便于从cache的最后处开始翻查
uint8_t Read_Record_Data_From_Flash(uint32_t para)
{
	uint32_t i;

	//为了尾对齐
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //当前位置不够RECORD_NUM条记录，可能在末端有数据，则在末端进行读取
	{//小于600个的时候，先读出所有的记录；翻读总是读最新的
		////从末尾读
		//S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //数据读出来
		//从开头读	
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //数据读出来
	}
	else
	{//大于600个数据
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//处理翻查在液晶屏幕上显示;cache是全局变量
	for(i=0;i<RECORD_NUM;i++)
	{
		if((READ_Record_data_cache[i*16]==save_state_new )||(READ_Record_data_cache[i*16]==save_state_read))  //有数据就进行统计，不管是已读或者末读
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	
	return 1;
}




//测量结果存放在1号块中，
//在1号中以时间为关键字，找到时间最近的一次结果
//找最近一次测量结果的存储位置赋给一个全局变量Save_Measure_Index_Ptr
void Find_Save_Index_Ptr(void)
{
	
	uint32_t ptr1=4,ptr2=8191,ptr3=16383;
	uint32_t time1,time2,time3;
	uint8_t  read_data[16]={0};

	S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16);

	//判断flash是否做过写满擦除
	if((read_data[0]!=0xff)&& (read_data[15]!=0xff))//擦除过的
	{
		while(1)
		{
			//read 3 条测量结果，比较看，哪一个是最新的
			//从头、尾、中间读，二分查找
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr1*16,16);
			time1=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr3*16,16);
			time3=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			
			if(time1>time2) //存储位置靠前的时间大，已经有覆盖
			{
				ptr3=ptr2;
				ptr2=(ptr3-ptr1)/2+ptr1;//调整右边界
				if(ptr2==ptr1)   
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else
			 {
				ptr1=ptr2;//调整左边界
				ptr2=(ptr3-ptr1)/2+ptr1;
				if(ptr2==ptr1)
				{
					if( time2>time3)
						Save_Measure_Index_Ptr=ptr2;
					else 
						Save_Measure_Index_Ptr=ptr3;
					break;
				}
				
				
			}
		}	
	}
	else//没写满过，没擦除过
	{
		while(1)
		{
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			if(time2==0xffffffff)//后半部分没有写
			{
				ptr3=ptr2;//移动右边界
				ptr2=(ptr3-ptr1)/2+ptr1;		
				if(ptr2==ptr1)
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else //后半部分已经写了
			{
				ptr1=ptr2;//移动左边界
				ptr2=(ptr3-ptr1)/2+ptr1;		
				if(ptr2==ptr1 )
				{	
					if(ptr2!=0xffffffff)
						Save_Measure_Index_Ptr=ptr2;
					else	
						Save_Measure_Index_Ptr=ptr3;
					break;
				}
			}
		}		
	}
	
}







uint32_t measure_data_save_ptr=Measure_Data_Begin;   //跳过4M的空间开始进行存储

//函数功能，在进形带数据测量时，要先保证存储数据的空间是被擦除过的，因擦除最小为256K，所以当前所在块不进行擦除
//进行一次测量后，所需要的空间不足，对后一个进行，因进行循环写，此方法可以保证写操作的地址为擦除过的
//当前区域存储了3条之后，可能就应该擦除下一个区域
void Erase_Next_Sector(void)
{
	uint32_t sector;
	uint32_t cursector;
	cursector=measure_data_save_ptr/(1024*256);
	sector=(measure_data_save_ptr+Measure_Data_Maximum)/(1024*256);
	if(sector>cursector) //当前块无法满足最大数据量的存储
	{
		S25FL512S_Erase_Sector(sector);
	}
}


//存到4号块中数据指针区
#define measure_data_add_begin (4*256*1024)
#define measure_data_add_end   (5*256*1024)


uint32_t measure_data_add_ptr=measure_data_add_begin;


//更新measure_data_add_ptr位置，每增加一次（完整）测量数据更新一次，更新前把当前测量数据结果位置放到
//measure_data_add_ptr地址中，measure_data_add_ptr自增5，如果写到块结束位置，则返回开始位置
void Updata_measure_data_add_ptr(void)
{
	uint8_t add[5];
	add[0]=(measure_data_save_ptr>>24)&0xff;
	add[1]=(measure_data_save_ptr>>16)&0xff;
	add[2]=(measure_data_save_ptr>>8)&0xff;
	add[3]=(measure_data_save_ptr)&0xff;
	add[4]=0xaa;
	
	if(measure_data_add_ptr<=(measure_data_add_end-5))
	{
		S25FL512S_Write(add,measure_data_add_ptr,5);
		measure_data_add_ptr+=5;
	}
	else
	{
		S25FL512S_Erase_Sector(4);
		S25FL512S_Write(add,measure_data_add_begin,5);
		measure_data_add_ptr=measure_data_add_begin+5;
	}	
}




//找测量数据的最后一个存储地址的指针
//在4号块中查找最后一次存放测量数据地址的位置的指针
//查找结束后measure_data_add_ptr等于找到的位置的下一个位置
//measure_data_save_ptr最后一次存储数据结果位置
//找测量数据的，同样是二分法
void Find_measure_data_add_prt(void)  
{
		uint8_t read_add[5];
		uint32_t ptr1=0;
		uint32_t ptr3=(measure_data_add_end-1)/5;
		uint32_t ptr2=ptr3 /2;
	
		S25FL512S_Read(&(read_add[4]),measure_data_add_begin+4,1); 
		if(read_add[4]==0xff)   //数据区读到为0xff，表示没有数据
		{
			measure_data_add_ptr=measure_data_add_begin;  //数据索引指向索引存储区的开始
			measure_data_save_ptr=Measure_Data_Begin;    //数据存储指针指向数据区的开始位置
		}
		else
		{
			while(1)
			{
				S25FL512S_Read(&(read_add[4]),measure_data_add_begin+ptr2*5+4,1);
				if(read_add[4]==0xff)  //如果读到的数据为0xff，则表示读取空数据区 
				{
					ptr3=ptr2;  
					ptr2=(ptr3-ptr1)/2+ptr1;
					if(ptr2==ptr1) //当查找时，当前查找位置等于开始位置，则表示已经找到
					{
						S25FL512S_Read(&(read_add[0]),measure_data_add_begin+ptr2*5,4);
						measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
						measure_data_add_ptr=measure_data_add_begin+ptr2*5+5;
						break;  
					}
				}
				else
				{
					ptr1=ptr2;//开始指向当前查找位置
					ptr2=(ptr3-ptr1)/2+ptr1; //当前查找位置向后偏移一半
					if(ptr2==ptr1)  //当查找时，当前查找位置等于开始位置，则表示已经找到
					{
						S25FL512S_Read(&(read_add[4]),measure_data_add_begin+ptr3*5+4,1);
						if(read_add[4]==0xff)
						{
							S25FL512S_Read(&(read_add[0]),measure_data_add_begin+ptr2*5,4);
							measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
							measure_data_add_ptr=measure_data_add_begin+ptr2*5+5;
						}
						else
						{
							S25FL512S_Read(&(read_add[0]),measure_data_add_begin+ptr3*5,4);
							measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
							measure_data_add_ptr=measure_data_add_begin+ptr3*5+5;
						}
						break;
					}
				}
			}
		}
}





//查找未上传数据的位置
uint32_t upload_measure_data_add_ptr=0;  //记录要上传的数据的存储地址索引的地址
uint32_t upload_measure_data_save_ptr=0; //记录要上传 数据的存储地址

//查找末上传数据的头，如找到返回1 ，末找到返回0
//收到上传指令后，先传没有上传过的测量结果，再传数据
uint8_t Find_Not_Upload_add_ptr(void)
{
	uint8_t read_add[5]={0};
	uint8_t read_data[20]={0};
	uint8_t find_flag=0;
	
	upload_measure_data_add_ptr=measure_data_add_ptr-10;  
	//从当前存储地址索引的地址减10的地方开始查找，因索引方式原因，实际数据到索引地址偏移10

	while(1)
	{
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		if(read_add[4]!=0xff)
		{
			upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			
			S25FL512S_Read(&read_data[0],upload_measure_data_save_ptr,20);
			//测量数据有固定的数据头{0xaa,0xbb,0xcc,0xdd,0xee,0x11,0x22,0x33,0x44,0x55,time1,time2,time3,time,gmt,0xfc,0xfc,0xfc,0xfc,0xfc}
			//5个0xfc表示末读取，当为0xf0时表示读取
			if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
			{
				//有数据
				if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //未读取
				{
					if(upload_measure_data_add_ptr==measure_data_add_begin)
					{
						upload_measure_data_add_ptr=measure_data_add_begin-5;
						find_flag=1;
						break;
					}
					else
					{
						upload_measure_data_add_ptr=upload_measure_data_add_ptr-5;
						find_flag=1;
					}
				}
				else  //已经读取过的
				{
					break;  //找到最后一次读取的位置
				}
			}
			else
			{
				break; //当前位置数据已经不存在
			}			
		}
		else
		{
			break;//记录指令找到了头
		}
		
	}

	
	if(find_flag)  //如果找到了末上传的数据
	{
		upload_measure_data_add_ptr+=5;	
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
		return 1;
	}
	return 0;
}



//上传数据 参数标识由USB或者BLE上传

# define Date_1970  2208988800000
//转换时间为UTC格式
uint64_t RTC_TO_UTC(uint32_t time_num)
{
	uint64_t num;
	num=((uint64_t)time_num*1000)+(uint64_t)Date_1970;
	return num;
}


//上传一条测量结果 ，按普仁的上传通信协议来实现
void Upload_One_Record(uint8_t port,uint32_t num)
{
	uint32_t i;
	uint32_t write_add=0;
	uint32_t rtc_time_num;
	uint64_t utc_time_num;
	uint8_t write_readflag=save_state_read;
	uint8_t one_record[97]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0x57,0x01,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	//RTC时间转换到UTC时间												
	rtc_time_num=(READ_Record_data_cache[num*16+1]<<24)+(READ_Record_data_cache[num*16+2]<<16)+(READ_Record_data_cache[num*16+3]<<8)+(READ_Record_data_cache[num*16+4]);	
	rtc_time_num=rtc_time_num-READ_Record_data_cache[num*16+5]*3600;//把时区偏差减掉
	utc_time_num=RTC_TO_UTC(rtc_time_num);
	
													
	one_record[10]=0x23;
	one_record[11]=0x23;
	one_record[12]=0x23;
	one_record[13]=0x23;
													
	one_record[14]=0x23;
	one_record[15]=0x23;
	one_record[16]=0x23;
	one_record[17]=0x23;
	
	one_record[18]=0x23;
	one_record[19]=0x23;
	one_record[20]=0x23;
	one_record[21]=0x23;
													
	one_record[22]=0x23;
	one_record[23]=0x23;
	one_record[24]=0x23;
	one_record[25]=0x23;			
													
	one_record[26]=(RIDH>>24)&0xff;
	one_record[27]=(RIDH>>16)&0xff;
	one_record[28]=(RIDH>>8)&0xff;
	one_record[29]=(RIDH)&0xff;
													
	one_record[30]=(RIDMH>>24)&0xff;
	one_record[31]=(RIDMH>>16)&0xff;
	one_record[32]=(RIDMH>>8)&0xff;
	one_record[33]=(RIDMH)&0xff;
	
	one_record[34]=(RIDML>>24)&0xff;
	one_record[35]=(RIDML>>16)&0xff;
	one_record[36]=(RIDML>>8)&0xff;
	one_record[37]=(RIDML)&0xff;
													
	one_record[38]=(RIDL>>24)&0xff;
	one_record[39]=(RIDL>>16)&0xff;
	one_record[40]=(RIDL>>8)&0xff;
	one_record[41]=(RIDL)&0xff;													
	
	//皓脉ID
	
	one_record[42]=0x2a;
	one_record[43]=0x2a;
	one_record[44]=0x2a;
	one_record[45]=0x2a;
	
	one_record[46]=0x2a;
	one_record[47]=0x2a;
	one_record[48]=0x2a;
	one_record[49]=0x2a;
	
	one_record[50]=0x2a;
	one_record[51]=0x2a;
	one_record[52]=0x2a;
	one_record[53]=0x2a;
	
	one_record[54]=0x2a;
	one_record[55]=0x2a;
	one_record[56]=0x2a;
	one_record[57]=0x2a;
	
	one_record[58]=(PW1101_ID_H>>24)&0xff;
	one_record[59]=(PW1101_ID_H>>16)&0xff;
	one_record[60]=(PW1101_ID_H>>8)&0xff;
	one_record[61]=(PW1101_ID_H)&0xff;
	
	one_record[62]=(PW1101_ID_MH>>24)&0xff;
	one_record[63]=(PW1101_ID_MH>>16)&0xff;
	one_record[64]=(PW1101_ID_MH>>8)&0xff;
	one_record[65]=(PW1101_ID_MH)&0xff;
	
	one_record[66]=(PW1101_ID_ML>>24)&0xff;
	one_record[67]=(PW1101_ID_ML>>16)&0xff;
	one_record[68]=(PW1101_ID_ML>>8)&0xff;
	one_record[69]=(PW1101_ID_ML)&0xff;
	
	one_record[70]=(PW1101_ID_L>>24)&0xff;
	one_record[71]=(PW1101_ID_L>>16)&0xff;
	one_record[72]=(PW1101_ID_L>>8)&0xff;
	one_record[73]=(PW1101_ID_L)&0xff;
	
	one_record[74]=((uint64_t)utc_time_num>>56)&0xff;
	one_record[75]=((uint64_t)utc_time_num>>48)&0xff;
	one_record[76]=((uint64_t)utc_time_num>>40)&0xff;
	one_record[77]=((uint64_t)utc_time_num>>32)&0xff;
	one_record[78]=((uint64_t)utc_time_num>>24)&0xff;
	one_record[79]=((uint64_t)utc_time_num>>16)&0xff;
	one_record[80]=((uint64_t)utc_time_num>>8)&0xff;
	one_record[81]=((uint64_t)utc_time_num)&0xff;
	
	one_record[82]=0;
	one_record[83]=READ_Record_data_cache[num*16+10];
	one_record[84]=0;
	one_record[85]=READ_Record_data_cache[num*16+11];
	one_record[86]=0;
	one_record[87]=READ_Record_data_cache[num*16+12];
	one_record[88]=0;
	one_record[89]=READ_Record_data_cache[num*16+13];
	
	one_record[90]=READ_Record_data_cache[num*16+6];
	one_record[91]=READ_Record_data_cache[num*16+7];
	one_record[92]=0;
	one_record[93]=READ_Record_data_cache[num*16+8];
	one_record[94]=0;
	one_record[95]=READ_Record_data_cache[num*16+9];
	one_record[96]=Check_CRC8(one_record);
	
	if(port==USB_COMM)
	{
		for(i=0;i<97;i++)
		{		
			USB_UART_Send_Byte(one_record[i]);
		}
	}
	else
	{
		for(i=0;i<97;i++)
		{		
			BLE_UART_Send_Byte(one_record[i]);
		}
	}
	
	//传完一条测量结果后，进行数据的已读标记
	
	if((Save_Measure_Index_Ptr-3)>((RECORD_NUM-1)-num))
	{
		write_add=Save_Measure_Index_Ptr-(RECORD_NUM-1-num);
	}
	else
	{
		write_add=16384-((RECORD_NUM-1-num)-(Save_Measure_Index_Ptr-4));
	}
	
//	S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //在测量结果的读取标识位写入已读标识
//确认指令之后再写回flash中	
//	delay_20000(500);// 传完以后做一点空循环延时
}



//上传一次完整测量的测量数据，最多66k字节
//按照普仁的通信协议来上传
void Upload_One_Data_Record(uint8_t port,uint32_t add_num)
{
	uint8_t i,j;
	uint8_t read_data[20];
	uint8_t read_add[4];
	uint8_t write_read_flag[5]={0xf0,0xf0,0xf0,0xf0,0xf0};
	uint32_t save_data_add_cache=0;
	uint32_t write_read_flag_add_cache;
	uint32_t save_data_add_cache_end;
	uint32_t rtc_time_num;
	uint64_t utc_time_num;
	uint8_t group_num=0;
	uint8_t data_group_num[12]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0x02,0x02,0x1f,0x5c};
	
	uint8_t data_measure[212]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0xCA,0x03,00};
		
	if(add_num==(measure_data_add_begin-5))  //判断测量数据是否在开端处
	{	
		S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
		if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)   //判断有无数据，为真表示有数据
		{
			//有数据
			if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //未读取
			{
				save_data_add_cache=Measure_Data_Begin;  //得到数据开始地址
				S25FL512S_Read(&read_add[0],measure_data_add_begin,4);
				save_data_add_cache_end=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);		 //数据结束地址
			}
		}
	}
	else
	{
		S25FL512S_Read(&read_add[0],add_num,4);
		save_data_add_cache=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
		if((add_num+5)==measure_data_add_ptr)
		{
			save_data_add_cache_end=measure_data_save_ptr;
		}
		else
		{
			S25FL512S_Read(&read_add[0],add_num+5,4);
			save_data_add_cache_end=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
		}
	}
	write_read_flag_add_cache=save_data_add_cache;
	
	S25FL512S_Read(&read_data[0],save_data_add_cache,20);
	save_data_add_cache+=20;   
	
	rtc_time_num=(read_data[10]<<24)+(read_data[11]<<16)+(read_data[12]<<8)+(read_data[13]);
	rtc_time_num=rtc_time_num-read_data[14]*3600;//把时区偏差减掉
	utc_time_num=RTC_TO_UTC(rtc_time_num);
	utc_time_num-=3000;

	while (save_data_add_cache<save_data_add_cache_end)  //将6k (212x31)乘以项目数
	{
		
			//发送数据组数，数据类型为02，现在均为固定31组
			if(port==USB_COMM)
			{
				for(i=0;i<12;i++)
				{		
					USB_UART_Send_Byte(data_group_num[i]);
				}
			}
			else
			{
				for(i=0;i<12;i++)
				{		
					BLE_UART_Send_Byte(data_group_num[i]);
				}
			}
			
			data_measure[11]=0x23;
			data_measure[12]=0x23;
			data_measure[13]=0x23;												
			data_measure[14]=0x23;
			
			data_measure[15]=0x23;
			data_measure[16]=0x23;
			data_measure[17]=0x23;	
			data_measure[18]=0x23;
			
			data_measure[19]=0x23;
			data_measure[20]=0x23;
			data_measure[21]=0x23;
			data_measure[22]=0x23;
			
			data_measure[23]=0x23;
			data_measure[24]=0x23;
			data_measure[25]=0x23;			
			data_measure[26]=0x23;		
			
			data_measure[27]=(RIDH>>24)&0xff;
			data_measure[28]=(RIDH>>16)&0xff;
			data_measure[29]=(RIDH>>8)&0xff;
			data_measure[30]=(RIDH)&0xff;
															
			data_measure[31]=(RIDMH>>24)&0xff;
			data_measure[32]=(RIDMH>>16)&0xff;
			data_measure[33]=(RIDMH>>8)&0xff;
			data_measure[34]=(RIDMH)&0xff;

			data_measure[35]=(RIDML>>24)&0xff;
			data_measure[36]=(RIDML>>16)&0xff;
			data_measure[37]=(RIDML>>8)&0xff;
			data_measure[38]=(RIDML)&0xff;
															
			data_measure[39]=(RIDL>>24)&0xff;
			data_measure[40]=(RIDL>>16)&0xff;
			data_measure[41]=(RIDL>>8)&0xff;
			data_measure[42]=(RIDL)&0xff;													
			
			//皓脉ID
			
			
			data_measure[43]=0x2a;
			data_measure[44]=0x2a;
			data_measure[45]=0x2a;
			data_measure[46]=0x2a;
			
			data_measure[47]=0x2a;
			data_measure[48]=0x2a;
			data_measure[49]=0x2a;	
			data_measure[50]=0x2a;
			
			data_measure[51]=0x2a;
			data_measure[52]=0x2a;
			data_measure[53]=0x2a;	
			data_measure[54]=0x2a;
			
			data_measure[55]=0x2a;
			data_measure[56]=0x2a;
			data_measure[57]=0x2a;
			data_measure[58]=0x2a;
			
			data_measure[59]=(PW1101_ID_H>>24)&0xff;
			data_measure[60]=(PW1101_ID_H>>16)&0xff;
			data_measure[61]=(PW1101_ID_H>>8)&0xff;
			data_measure[62]=(PW1101_ID_H)&0xff;
			
			data_measure[63]=(PW1101_ID_MH>>24)&0xff;
			data_measure[64]=(PW1101_ID_MH>>16)&0xff;
			data_measure[65]=(PW1101_ID_MH>>8)&0xff;
			data_measure[66]=(PW1101_ID_MH)&0xff;
			
			data_measure[67]=(PW1101_ID_ML>>24)&0xff;
			data_measure[68]=(PW1101_ID_ML>>16)&0xff;
			data_measure[69]=(PW1101_ID_ML>>8)&0xff;
			data_measure[70]=(PW1101_ID_ML)&0xff;
			
			data_measure[71]=(PW1101_ID_L>>24)&0xff;
			data_measure[72]=(PW1101_ID_L>>16)&0xff;
			data_measure[73]=(PW1101_ID_L>>8)&0xff;
			data_measure[74]=(PW1101_ID_L)&0xff;			
			
			
			group_num++;
			data_measure[10]=1;
			utc_time_num+=3000;	
			
			data_measure[75]=((uint64_t)utc_time_num>>56)&0xff;
			data_measure[76]=((uint64_t)utc_time_num>>48)&0xff;
			data_measure[77]=((uint64_t)utc_time_num>>40)&0xff;
			data_measure[78]=((uint64_t)utc_time_num>>32)&0xff;
			data_measure[79]=((uint64_t)utc_time_num>>24)&0xff;
			data_measure[80]=((uint64_t)utc_time_num>>16)&0xff;
			data_measure[81]=((uint64_t)utc_time_num>>8)&0xff;
			data_measure[82]=((uint64_t)utc_time_num)&0xff;
			
			data_measure[83]=group_num;
			data_measure[84]=6000/256;
			data_measure[85]=6000%256;
			S25FL512S_Read(&data_measure[86],save_data_add_cache,125);
			save_data_add_cache=save_data_add_cache+125;
			data_measure[211]=Check_CRC8(data_measure);
			
			if(port==USB_COMM)
			{
				for(i=0;i<212;i++)
				{		
					USB_UART_Send_Byte(data_measure[i]);
				}
			}
			else
			{
				for(i=0;i<212;i++)
				{		
					BLE_UART_Send_Byte(data_measure[i]);
				}
			}
//		delay_20000(500);// 传完以后做一点空循环延时			
			for(i=2;i<31;i++)	//
			{
				data_measure[10]=i;
				S25FL512S_Read(&(data_measure[11]),save_data_add_cache,200);
				
				save_data_add_cache+=200;
				data_measure[211]=Check_CRC8(data_measure);
				if(port==USB_COMM)
				{
					for(j=0;j<212;j++)
					{		
						USB_UART_Send_Byte(data_measure[j]);
					}
				}
				else
				{
					for(j=0;j<212;j++)
					{		
						BLE_UART_Send_Byte(data_measure[j]);
					}
				}
				
//			delay_20000(500);// 传完以后做一点空循环延时
			}
			data_measure[10]=31;
			S25FL512S_Read(&data_measure[11],save_data_add_cache,75);
			save_data_add_cache+=75;
			for(i=86;i<211;i++)
			{
				data_measure[i]=0;
			}
			data_measure[211]=Check_CRC8(data_measure);
			
			
			
			if(port==USB_COMM)
			{
				for(i=0;i<212;i++)
				{		
					USB_UART_Send_Byte(data_measure[i]);
				}
			}
			else
			{
				for(i=0;i<212;i++)
				{		
					BLE_UART_Send_Byte(data_measure[i]);
				}
			}	
	}
	
//	if(save_data_add_cache!=0)
//		S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);

}






//当收到数据同步指令后，调用此函数
//在main函数里，接到同步上传指令之后，执行该命令
//同显示的函数Read_Record_Data_From_Flash一样的实现原理
void Upload_data(uint8_t port)
{
	uint32_t i;
	uint8_t data[11]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0x01,0x04,0x46};
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //当前位置不够RECORD_NUM（600）条记录，可能在末端有数据，则在末端进行读取
	{
		///S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //数据读出来
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //数据读出来
	}
	else   //读取RECORD_NUM条记录
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//查找开始上传位置
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //如果是新数据未上传过  未上传时等于0xfc  上传后为0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//逐条上传测量结果
	for(i=record_begin_index;i<RECORD_NUM;i++)
	{
		Upload_One_Record(port,i);  //上传测量结果
	}
	
	
	
	if(port==USB_COMM)   //只USB传输测量数据
	{
		if(measure_data_add_ptr==measure_data_add_begin) //没有数据
		{		
		}
		else if(measure_data_add_ptr==measure_data_add_begin+5)  //当前只有一条数据
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
			Upload_One_Data_Record(port,upload_measure_data_add_ptr);
		}
		else if(Find_Not_Upload_add_ptr())  //在数据表中查找末上传的数据头，如果找到返回真
		{
			//从找到的末上传的头，到当前最后测量数据的结束位置，逐条数据上传
			if(upload_measure_data_add_ptr==measure_data_add_begin)  //如果找到了位置在数据头，刚从地址索引的前一个开始传
			{
				upload_measure_data_add_ptr=measure_data_add_begin-5;
			}
			for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)
			{
				Upload_One_Data_Record(port,i);  //上传一组数据（N*6000）
			}		
//			if(measure_data_add_ptr>(measure_data_add_end-5*300))  //如果4号块中存放索引的地址距离块结尾所能存的索引个数少于一天的量（按5分钟24小时，288次，现留300次）
//			{
//				S25FL512S_Erase_Sector(4);
//				measure_data_add_ptr=measure_data_add_begin;  //数据上传完成后，把索引放到开始位置
//			}
		}
	}
	
	  //进行APP指令响应应答
	if(port==USB_COMM)
	{
		for(i=0;i<11;i++)
			USB_UART_Send_Byte(data[i]);
	}
	else
	{
		for(i=0;i<11;i++)
			BLE_UART_Send_Byte(data[i]);
	}
	
}

//当收到清除数据指令后，调用此函数
//把已读的数据的已读标识更改后写回flash
//和Upload_data函数的实现原理类似，至少在这里做标识而不是上传
void Clear_data(uint8_t port)
{
	uint32_t i;
	uint32_t write_add=0;
	uint8_t read_data[20];
	uint8_t read_add[4];
	uint8_t write_read_flag[5]={0xf0,0xf0,0xf0,0xf0,0xf0};
	uint32_t write_read_flag_add_cache=0;
	
	uint8_t write_readflag=save_state_read;
	
	uint8_t data[11]={0xa5,0x52,0x21,0x00,0x01,0x00,0x01,0x33,0x01,0x01,0x40};

	
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //当前位置不够RECORD_NUM（600）条记录，可能在末端有数据，则在末端进行读取
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //从末端把数据读出来
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //从存储区开始位置把数据读出来
	}
	else
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);//读取600条数据
	}
	
	record_begin_index=RECORD_NUM;
	
	//查找开始标识已上传位置
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //如果是新数据未上传过  未上传时等于0xfc  上传后为0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//逐条标识已上传测量结果写回flash
	for(i=record_begin_index;i<RECORD_NUM;i++)
	{
			if((Save_Measure_Index_Ptr-3)>((RECORD_NUM-1)-i))
			{
				write_add=Save_Measure_Index_Ptr-(RECORD_NUM-1-i);  
			}
			else
			{
				write_add=16384-((RECORD_NUM-1-i)-(Save_Measure_Index_Ptr-4));
			}
			S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //在测量结果的读取标识位写入已读标识
	}
	

	if(measure_data_add_ptr==measure_data_add_begin+5)  //当前只有一条数据
	{
		write_read_flag_add_cache=Measure_Data_Begin;			
		S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);
	}
	else if(Find_Not_Upload_add_ptr())   //查找测量数据的末标识上传的开始位置
	{
		//从找到的末上传的头，到当前最后测量数据的结束位置，逐条数据上传
		if(upload_measure_data_add_ptr==measure_data_add_begin)
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
		}
		for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)  //从开始位置到当前数据结束位置，逐条进行标识
		{
			write_read_flag_add_cache=0;     //进行写入已读标识位置缓存
			if(i==(measure_data_add_begin-5))   //如果数据等于数据索引块的起始位置
			{	
				S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
				if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
				{
					//有数据
					if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //未读取
					{
						write_read_flag_add_cache=Measure_Data_Begin;		//指向开始位置
					}
				}
			}
			else 
			{
				S25FL512S_Read(&read_add[0],i,4);  //从数据索引中，读取数据的存储位置
				write_read_flag_add_cache=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			}
			if(write_read_flag_add_cache!=0)  //如果拿到了要写入已读标识的位置
				S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);  //在数据块的已读标识位所在位置写入已读  0xf0,0xf0,0xf0,0xf0,0xf0
		}		
		if(measure_data_add_ptr>(measure_data_add_end-5*300))  //如果4号块中存放索引的地址距离块结尾所能存的索引个数少于一天的量（按5分钟24小时，288次，现留300次）
		{
			S25FL512S_Erase_Sector(4);
			measure_data_add_ptr=measure_data_add_begin;  //数据上传完成后，把索引放到开始位置
		}
	}	
		
  //进行APP指令响应应答
	if(port==USB_COMM)
	{
		for(i=0;i<11;i++)
			USB_UART_Send_Byte(data[i]);
	}
	else
	{
		for(i=0;i<11;i++)
			BLE_UART_Send_Byte(data[i]);
	}

	
	
	
		
}

