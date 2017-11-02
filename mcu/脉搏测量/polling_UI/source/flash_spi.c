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
//��Դ�ٷ���վ������
////PTA14  SPI-CS����Ϊ����ܽţ���ԭ��ͼ
//�ⲿ��ַ��չʹ�ܣ�д1���ܲ���512MBITS
void S25FL512S_Spi_Config(void)
{
	uint32_t srcClock_Hz;
	dspi_master_config_t masterConfig;
	//��Դ�ٷ���վ������
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
	//PTA14  SPI-CS����Ϊ����ܽţ���ԭ��ͼ
	GPIO_PinInit(GPIOA, PIN14_IDX, &IO_config);

	//ʹ��S25FL512S_BRWR  0x17,Ϊ�˲������ƼĴ���	
	//�ⲿ��ַ��չʹ�ܣ�д1���ܲ���512MBITS
	S25FL512S_WriteBR(0x80);		
}



//////////////////////////////////////////////
//�������ܣ����е��ֽ����ݽ���
//���������Ҫ���͵��ֽ�
//��������������ֽ�ʱ�������ֽ�
//////////////////////////////////////////////
uint8_t S25FL512S_ReadWriteByte(uint8_t TxData)
{
	flashXfer.txData =& TxData;
  	flashXfer.dataSize = 1;
	flashXfer.rxData =flashRxData;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_FLASH_BASEADDR, &flashXfer);
	return *flashXfer.rxData; 
}

//��ȡ״̬�Ĵ���1�� ��Ҫ��Ϊ�˶�ȡ0λ��BUSY״̬λ
uint8_t S25FL512S_ReadSR1(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_RDSR1);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}

//д״̬�Ĵ���1
void S25FL512S_WriteSR1(uint8_t sr)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRSR1);
	S25FL512S_ReadWriteByte(sr);
	S25FL512S_CS_H;
}

//��ȡBank Address Register���˼Ĵ�����7λEXTADD��ʾ�����ֽڵ�ַ�������ֽڵ�ַ
//����1ʱΪ���ֽڵ�ַ��Ϊ0ʱ��Ϊ���ֽ���ϵ�0��1λ����ѯַ��0λ��Ӧ512Mb��A24
//1λ��Ӧ512Mb��A25��ͨ��26λ��ַʵ��512Mb��ѯַ������
uint8_t S25FL512S_ReadBR(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRRD);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}
//ͨ���˼Ĵ��������ã�ʵ��512Mb�ĸߵ�ַ����
void S25FL512S_WriteBR(uint8_t br)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRWR);
	S25FL512S_ReadWriteByte(br);
	S25FL512S_CS_H;
}

//ʹ��д����
void S25FL512S_Write_Enable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WREN);
	S25FL512S_CS_H;
}
//ʧ��д����
void S25FL512S_Write_Disable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRDI);
	S25FL512S_CS_H;
}

//�ȴ�BUSY����
void S25FL512S_Wait_Busy(void)
{
	while((S25FL512S_ReadSR1()&0x01)==0x01); //�ȴ�BUSYλ���
}


//��ȡSPI FLASH  ;
//����ַ
//�����ݳ���
//�������������뻺������
void S25FL512S_Read(uint8_t * pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead)   
{ 
		uint32_t i;  
		S25FL512S_CS_L;                            //ʹ������   
    S25FL512S_ReadWriteByte(S25FL512S_RD);         //���Ͷ�ȡ����
		S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>24));  //����32bit��ַ
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>16));   
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>8));  
    S25FL512S_ReadWriteByte((uint8_t)ReadAddr);  
		
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=S25FL512S_ReadWriteByte(0XFF);   //ѭ������  
    }
		S25FL512S_CS_H; 			    	      
}  




//д������512���ֽڵ�����
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


////�޼��� S25FL512S SPI FLASH 
void S25FL512S_Write_NoCheck(uint8_t * pBuffer,uint32_t WriteAddr,uint32_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=512-WriteAddr%512; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������512���ֽ�
	while(1)
	{	   
		S25FL512S_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>512)pageremain=512; //һ�ο���д512���ֽ�
			else pageremain=NumByteToWrite; 	  //����512���ֽ���
		}
	}	    
} 






//дһ�����ݵ��ƶ���flash��ַ��

void S25FL512S_Write(uint8_t * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
	uint32_t secpos;
	uint32_t secoff;
	uint32_t secremain;	     

 	secpos=WriteAddr/(1024*256);//������ַ  ÿ��������256K�ֽ�256x1024=nnn byte
	secoff=WriteAddr%(1024*256);//�������ڵ�ƫ��
	secremain=(1024*256)-secoff;//����ʣ��ռ��С   
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������(1024*256)���ֽ�
	while(1) 
	{	
		S25FL512S_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 
			pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
			NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>(1024*256))secremain=1024*256;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}	 
}


//����Ҫ������������
void S25FL512S_Erase_Sector(uint32_t Dst_Addr)   
{  
		Dst_Addr=Dst_Addr*256*1024;     //256K�ֽ�
    S25FL512S_Write_Enable();                  //SET WEL 	 
    S25FL512S_Wait_Busy();   
  	S25FL512S_CS_L;                          //ʹ������   
    S25FL512S_ReadWriteByte(S25FL512S_SE);      //������������ָ�� 
  	S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>24));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>16));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>8));
		S25FL512S_ReadWriteByte((uint8_t)Dst_Addr);
		S25FL512S_CS_H;                              //ȡ��Ƭѡ     	      
    S25FL512S_Wait_Busy();				   //�ȴ��������
}  







//�Ѳ������浽flash��
uint8_t user_set_para[SET_PARA_TOTAL_NUM];

//flash�����Ȳ������ܸ�д���������õ��ǰѴ洢���������ȫ����Ϊ0xff��д���ݵ�ʱ����ܰ�0xff��ΪĿ��ֵ
//Ŀǰ�Ĳ�������256kbyte/sec����ص�groupռ��һ��sector��Ȼ���˷�
//mcu��ramֻ��256kbyte
void Sava_para_to_flash(void)
{
	S25FL512S_Erase_Sector(0);
	S25FL512S_Write(user_set_para,0,SET_PARA_TOTAL_NUM);
}




//����Ĭ�ϵĲ����������洢���е�����0
//���������ռ��һ���������洢��0������
//change name to Save_para_as_default_Value is better
void Save_para_as_define(void)  
{
		user.age=50;   //�û�����
		user.sex=1;  //�û��Ա�  1��  2 Ů
		user.height=170; //��� 
		user.weight=70;  //����
		
		alarm.HBlood_LowValue=80;   //��ѹ����
		alarm.HBlood_HighValue=180;  //��ѹ����
		alarm.LBlood_LowValue=40;    //��ѹ����
		alarm.LBlood_HighValue=90;   //��ѹ����
		alarm.HeartRate_LowValue=60;  //��������
		alarm.HeartRate_HighValue=160;  //��������
		
		MeasurePara.Groupnum=11;  //����������
		MeasurePara.Timelong=3;   //����ʱ��3s
		MeasurePara.Timeinterval=2; //�������2ms
		MeasurePara.Eletrode=1;    //�����缫
		
		Measure_Mode=4;    //4���������ݲ���   2�������ݲ���   
		
		AutoMeasurePara.Timelong=24;        //�Զ�����ʱ�� Сʱ
		AutoMeasurePara.Timeinterval=10;    //�Զ������������
		
		StandbyTime=30;  //����30��
		GMT_num=8;      //ʱ��
		mmHg_or_kPa=1;  //��ʾ��λ   1 mmHg   kPa
		Ble_OnOff=2;    //�������أ�Ĭ�Ϲ�
		NoDisturb_Mode=1;   //������
		
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




//��flash�洢����0���������ȡ�û����ò���  ���涨���������洢�û����ò���
//��ȡ��user_set_para������,user_set_para�����Ԫ��ƫ����flash_spi.h����_FLASH_ADD_TYPE��ö�ٶ���
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
	
	if(user.age==0xff &&user.sex==0xff)	//�û�û�����ù�����
	{
		Save_para_as_define();	
	}	
	
	
}	





uint32_t Save_Measure_Index_Ptr=3;

uint8_t READ_Record_data_cache[RECORD_NUM*16];


//����һ�����������һ�����Ϊ16���ֽڣ�flash���԰�ֱ��д����ڲ���֮�������
//�������Ϊ��0xfc(һ���¼�¼) RTCʱ�䣨4�ֽڣ� ʱ��1�ֽ�  �������4�ֽ� �û���Ϣ4�ֽ�  ������0xffԤ��
uint8_t Save_Measure_Result_To_Flash (uint32_t time, uint8_t *data)
{
	uint8_t write_data[16]={0};
	
	write_data[0]=save_state_new;    //����һ�������ݣ��Ժ�����ж������Ƿ���ڣ�
	time++;                         //����һ��ʱ�䣬�����ʱ����һ��
	write_data[1]=(time>>24)&0xff;  //ʱ���
	write_data[2]=(time>>16)&0xff;
	write_data[3]=(time>>8)&0xff;
	write_data[4]=(time)&0xff;
	write_data[5]=GMT_num;
	write_data[6]=data[0];//��������оƬ
	write_data[7]=data[1];
	write_data[8]=data[2];
	write_data[9]=data[3];
	if(user.sex==1) //��ʾ��
		write_data[10]=0x01;
	else 
		write_data[10]=0x00;
	
	write_data[11]=user.age;
	write_data[12]=user.height;
	write_data[13]=user.weight;
	
	if(Save_Measure_Index_Ptr<16383) //256KB   �ֳ� 16384��16�ֽ�С���򣬽��е�һ����¼�Ĵ洢
	{
		Save_Measure_Index_Ptr++;
	}
	else //����Ѿ�����һ������////��==256KByte==����
	{//ptr>16383��֤����ǰ���Ѿ�д������ת����ͷ������д
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-RECORD_NUM)*16,16*RECORD_NUM);  //��β���϶������ݶ�����600������֤��������
		S25FL512S_Erase_Sector(SAVE_MEASURE_RESULT_SECTOR);//������������/��
		S25FL512S_Write(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(4)*16,16*RECORD_NUM);  //������������д�������Ŀ�ͷ�Ĳ���
		////S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16); //�ڿ�Ŀ�ͷдһ�����ݣ�ֻΪ�˻�ȡ�Ƿ���д����һ��
		Save_Measure_Index_Ptr=3+RECORD_NUM;    //Ԥ����4���飬�˴�����4���൱�ڴӵ��Ŀ�洢��0��1��2��3������Ԥ��		
	}
	
	S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr)*16,16);  //д��һ���������
	return 1;
}



uint32_t  record_begin_index;
uint32_t  record_total_num;
uint32_t  display_record_index;


//�����������  ��������������
//�������ݻ��ϴ���¼��ʱ�򣬶��
//��flash�ж��������ݣ�����cache�У�cache��β����ģ����ڴ�cache����󴦿�ʼ����
uint8_t Read_Record_Data_From_Flash(uint32_t para)
{
	uint32_t i;

	//Ϊ��β����
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //��ǰλ�ò���RECORD_NUM����¼��������ĩ�������ݣ�����ĩ�˽��ж�ȡ
	{//С��600����ʱ���ȶ������еļ�¼���������Ƕ����µ�
		////��ĩβ��
		//S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //���ݶ�����
		//�ӿ�ͷ��	
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //���ݶ�����
	}
	else
	{//����600������
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//��������Һ����Ļ����ʾ;cache��ȫ�ֱ���
	for(i=0;i<RECORD_NUM;i++)
	{
		if((READ_Record_data_cache[i*16]==save_state_new )||(READ_Record_data_cache[i*16]==save_state_read))  //�����ݾͽ���ͳ�ƣ��������Ѷ�����ĩ��
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	
	return 1;
}




//������������1�ſ��У�
//��1������ʱ��Ϊ�ؼ��֣��ҵ�ʱ�������һ�ν��
//�����һ�β�������Ĵ洢λ�ø���һ��ȫ�ֱ���Save_Measure_Index_Ptr
void Find_Save_Index_Ptr(void)
{
	
	uint32_t ptr1=4,ptr2=8191,ptr3=16383;
	uint32_t time1,time2,time3;
	uint8_t  read_data[16]={0};

	S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16);

	//�ж�flash�Ƿ�����д������
	if((read_data[0]!=0xff)&& (read_data[15]!=0xff))//��������
	{
		while(1)
		{
			//read 3 ������������ȽϿ�����һ�������µ�
			//��ͷ��β���м�������ֲ���
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr1*16,16);
			time1=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr3*16,16);
			time3=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			
			if(time1>time2) //�洢λ�ÿ�ǰ��ʱ����Ѿ��и���
			{
				ptr3=ptr2;
				ptr2=(ptr3-ptr1)/2+ptr1;//�����ұ߽�
				if(ptr2==ptr1)   
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else
			 {
				ptr1=ptr2;//������߽�
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
	else//ûд������û������
	{
		while(1)
		{
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			if(time2==0xffffffff)//��벿��û��д
			{
				ptr3=ptr2;//�ƶ��ұ߽�
				ptr2=(ptr3-ptr1)/2+ptr1;		
				if(ptr2==ptr1)
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else //��벿���Ѿ�д��
			{
				ptr1=ptr2;//�ƶ���߽�
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







uint32_t measure_data_save_ptr=Measure_Data_Begin;   //����4M�Ŀռ俪ʼ���д洢

//�������ܣ��ڽ��δ����ݲ���ʱ��Ҫ�ȱ�֤�洢���ݵĿռ��Ǳ��������ģ��������СΪ256K�����Ե�ǰ���ڿ鲻���в���
//����һ�β���������Ҫ�Ŀռ䲻�㣬�Ժ�һ�����У������ѭ��д���˷������Ա�֤д�����ĵ�ַΪ��������
//��ǰ����洢��3��֮�󣬿��ܾ�Ӧ�ò�����һ������
void Erase_Next_Sector(void)
{
	uint32_t sector;
	uint32_t cursector;
	cursector=measure_data_save_ptr/(1024*256);
	sector=(measure_data_save_ptr+Measure_Data_Maximum)/(1024*256);
	if(sector>cursector) //��ǰ���޷���������������Ĵ洢
	{
		S25FL512S_Erase_Sector(sector);
	}
}


//�浽4�ſ�������ָ����
#define measure_data_add_begin (4*256*1024)
#define measure_data_add_end   (5*256*1024)


uint32_t measure_data_add_ptr=measure_data_add_begin;


//����measure_data_add_ptrλ�ã�ÿ����һ�Σ��������������ݸ���һ�Σ�����ǰ�ѵ�ǰ�������ݽ��λ�÷ŵ�
//measure_data_add_ptr��ַ�У�measure_data_add_ptr����5�����д�������λ�ã��򷵻ؿ�ʼλ��
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




//�Ҳ������ݵ����һ���洢��ַ��ָ��
//��4�ſ��в������һ�δ�Ų������ݵ�ַ��λ�õ�ָ��
//���ҽ�����measure_data_add_ptr�����ҵ���λ�õ���һ��λ��
//measure_data_save_ptr���һ�δ洢���ݽ��λ��
//�Ҳ������ݵģ�ͬ���Ƕ��ַ�
void Find_measure_data_add_prt(void)  
{
		uint8_t read_add[5];
		uint32_t ptr1=0;
		uint32_t ptr3=(measure_data_add_end-1)/5;
		uint32_t ptr2=ptr3 /2;
	
		S25FL512S_Read(&(read_add[4]),measure_data_add_begin+4,1); 
		if(read_add[4]==0xff)   //����������Ϊ0xff����ʾû������
		{
			measure_data_add_ptr=measure_data_add_begin;  //��������ָ�������洢���Ŀ�ʼ
			measure_data_save_ptr=Measure_Data_Begin;    //���ݴ洢ָ��ָ���������Ŀ�ʼλ��
		}
		else
		{
			while(1)
			{
				S25FL512S_Read(&(read_add[4]),measure_data_add_begin+ptr2*5+4,1);
				if(read_add[4]==0xff)  //�������������Ϊ0xff�����ʾ��ȡ�������� 
				{
					ptr3=ptr2;  
					ptr2=(ptr3-ptr1)/2+ptr1;
					if(ptr2==ptr1) //������ʱ����ǰ����λ�õ��ڿ�ʼλ�ã����ʾ�Ѿ��ҵ�
					{
						S25FL512S_Read(&(read_add[0]),measure_data_add_begin+ptr2*5,4);
						measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
						measure_data_add_ptr=measure_data_add_begin+ptr2*5+5;
						break;  
					}
				}
				else
				{
					ptr1=ptr2;//��ʼָ��ǰ����λ��
					ptr2=(ptr3-ptr1)/2+ptr1; //��ǰ����λ�����ƫ��һ��
					if(ptr2==ptr1)  //������ʱ����ǰ����λ�õ��ڿ�ʼλ�ã����ʾ�Ѿ��ҵ�
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





//����δ�ϴ����ݵ�λ��
uint32_t upload_measure_data_add_ptr=0;  //��¼Ҫ�ϴ������ݵĴ洢��ַ�����ĵ�ַ
uint32_t upload_measure_data_save_ptr=0; //��¼Ҫ�ϴ� ���ݵĴ洢��ַ

//����ĩ�ϴ����ݵ�ͷ�����ҵ�����1 ��ĩ�ҵ�����0
//�յ��ϴ�ָ����ȴ�û���ϴ����Ĳ���������ٴ�����
uint8_t Find_Not_Upload_add_ptr(void)
{
	uint8_t read_add[5]={0};
	uint8_t read_data[20]={0};
	uint8_t find_flag=0;
	
	upload_measure_data_add_ptr=measure_data_add_ptr-10;  
	//�ӵ�ǰ�洢��ַ�����ĵ�ַ��10�ĵط���ʼ���ң���������ʽԭ��ʵ�����ݵ�������ַƫ��10

	while(1)
	{
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		if(read_add[4]!=0xff)
		{
			upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			
			S25FL512S_Read(&read_data[0],upload_measure_data_save_ptr,20);
			//���������й̶�������ͷ{0xaa,0xbb,0xcc,0xdd,0xee,0x11,0x22,0x33,0x44,0x55,time1,time2,time3,time,gmt,0xfc,0xfc,0xfc,0xfc,0xfc}
			//5��0xfc��ʾĩ��ȡ����Ϊ0xf0ʱ��ʾ��ȡ
			if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
			{
				//������
				if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //δ��ȡ
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
				else  //�Ѿ���ȡ����
				{
					break;  //�ҵ����һ�ζ�ȡ��λ��
				}
			}
			else
			{
				break; //��ǰλ�������Ѿ�������
			}			
		}
		else
		{
			break;//��¼ָ���ҵ���ͷ
		}
		
	}

	
	if(find_flag)  //����ҵ���ĩ�ϴ�������
	{
		upload_measure_data_add_ptr+=5;	
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
		return 1;
	}
	return 0;
}



//�ϴ����� ������ʶ��USB����BLE�ϴ�

# define Date_1970  2208988800000
//ת��ʱ��ΪUTC��ʽ
uint64_t RTC_TO_UTC(uint32_t time_num)
{
	uint64_t num;
	num=((uint64_t)time_num*1000)+(uint64_t)Date_1970;
	return num;
}


//�ϴ�һ��������� �������ʵ��ϴ�ͨ��Э����ʵ��
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
	
	//RTCʱ��ת����UTCʱ��												
	rtc_time_num=(READ_Record_data_cache[num*16+1]<<24)+(READ_Record_data_cache[num*16+2]<<16)+(READ_Record_data_cache[num*16+3]<<8)+(READ_Record_data_cache[num*16+4]);	
	rtc_time_num=rtc_time_num-READ_Record_data_cache[num*16+5]*3600;//��ʱ��ƫ�����
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
	
	//���ID
	
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
	
	//����һ����������󣬽������ݵ��Ѷ����
	
	if((Save_Measure_Index_Ptr-3)>((RECORD_NUM-1)-num))
	{
		write_add=Save_Measure_Index_Ptr-(RECORD_NUM-1-num);
	}
	else
	{
		write_add=16384-((RECORD_NUM-1-num)-(Save_Measure_Index_Ptr-4));
	}
	
//	S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //�ڲ�������Ķ�ȡ��ʶλд���Ѷ���ʶ
//ȷ��ָ��֮����д��flash��	
//	delay_20000(500);// �����Ժ���һ���ѭ����ʱ
}



//�ϴ�һ�����������Ĳ������ݣ����66k�ֽ�
//�������ʵ�ͨ��Э�����ϴ�
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
		
	if(add_num==(measure_data_add_begin-5))  //�жϲ��������Ƿ��ڿ��˴�
	{	
		S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
		if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)   //�ж��������ݣ�Ϊ���ʾ������
		{
			//������
			if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //δ��ȡ
			{
				save_data_add_cache=Measure_Data_Begin;  //�õ����ݿ�ʼ��ַ
				S25FL512S_Read(&read_add[0],measure_data_add_begin,4);
				save_data_add_cache_end=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);		 //���ݽ�����ַ
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
	rtc_time_num=rtc_time_num-read_data[14]*3600;//��ʱ��ƫ�����
	utc_time_num=RTC_TO_UTC(rtc_time_num);
	utc_time_num-=3000;

	while (save_data_add_cache<save_data_add_cache_end)  //��6k (212x31)������Ŀ��
	{
		
			//����������������������Ϊ02�����ھ�Ϊ�̶�31��
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
			
			//���ID
			
			
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
//		delay_20000(500);// �����Ժ���һ���ѭ����ʱ			
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
				
//			delay_20000(500);// �����Ժ���һ���ѭ����ʱ
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






//���յ�����ͬ��ָ��󣬵��ô˺���
//��main������ӵ�ͬ���ϴ�ָ��֮��ִ�и�����
//ͬ��ʾ�ĺ���Read_Record_Data_From_Flashһ����ʵ��ԭ��
void Upload_data(uint8_t port)
{
	uint32_t i;
	uint8_t data[11]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0x01,0x04,0x46};
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //��ǰλ�ò���RECORD_NUM��600������¼��������ĩ�������ݣ�����ĩ�˽��ж�ȡ
	{
		///S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //���ݶ�����
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //���ݶ�����
	}
	else   //��ȡRECORD_NUM����¼
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//���ҿ�ʼ�ϴ�λ��
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //�����������δ�ϴ���  δ�ϴ�ʱ����0xfc  �ϴ���Ϊ0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//�����ϴ��������
	for(i=record_begin_index;i<RECORD_NUM;i++)
	{
		Upload_One_Record(port,i);  //�ϴ��������
	}
	
	
	
	if(port==USB_COMM)   //ֻUSB�����������
	{
		if(measure_data_add_ptr==measure_data_add_begin) //û������
		{		
		}
		else if(measure_data_add_ptr==measure_data_add_begin+5)  //��ǰֻ��һ������
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
			Upload_One_Data_Record(port,upload_measure_data_add_ptr);
		}
		else if(Find_Not_Upload_add_ptr())  //�����ݱ��в���ĩ�ϴ�������ͷ������ҵ�������
		{
			//���ҵ���ĩ�ϴ���ͷ������ǰ���������ݵĽ���λ�ã����������ϴ�
			if(upload_measure_data_add_ptr==measure_data_add_begin)  //����ҵ���λ��������ͷ���մӵ�ַ������ǰһ����ʼ��
			{
				upload_measure_data_add_ptr=measure_data_add_begin-5;
			}
			for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)
			{
				Upload_One_Data_Record(port,i);  //�ϴ�һ�����ݣ�N*6000��
			}		
//			if(measure_data_add_ptr>(measure_data_add_end-5*300))  //���4�ſ��д�������ĵ�ַ������β���ܴ��������������һ���������5����24Сʱ��288�Σ�����300�Σ�
//			{
//				S25FL512S_Erase_Sector(4);
//				measure_data_add_ptr=measure_data_add_begin;  //�����ϴ���ɺ󣬰������ŵ���ʼλ��
//			}
		}
	}
	
	  //����APPָ����ӦӦ��
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

//���յ��������ָ��󣬵��ô˺���
//���Ѷ������ݵ��Ѷ���ʶ���ĺ�д��flash
//��Upload_data������ʵ��ԭ�����ƣ���������������ʶ�������ϴ�
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

	
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //��ǰλ�ò���RECORD_NUM��600������¼��������ĩ�������ݣ�����ĩ�˽��ж�ȡ
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //��ĩ�˰����ݶ�����
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //�Ӵ洢����ʼλ�ð����ݶ�����
	}
	else
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);//��ȡ600������
	}
	
	record_begin_index=RECORD_NUM;
	
	//���ҿ�ʼ��ʶ���ϴ�λ��
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //�����������δ�ϴ���  δ�ϴ�ʱ����0xfc  �ϴ���Ϊ0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//������ʶ���ϴ��������д��flash
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
			S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //�ڲ�������Ķ�ȡ��ʶλд���Ѷ���ʶ
	}
	

	if(measure_data_add_ptr==measure_data_add_begin+5)  //��ǰֻ��һ������
	{
		write_read_flag_add_cache=Measure_Data_Begin;			
		S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);
	}
	else if(Find_Not_Upload_add_ptr())   //���Ҳ������ݵ�ĩ��ʶ�ϴ��Ŀ�ʼλ��
	{
		//���ҵ���ĩ�ϴ���ͷ������ǰ���������ݵĽ���λ�ã����������ϴ�
		if(upload_measure_data_add_ptr==measure_data_add_begin)
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
		}
		for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)  //�ӿ�ʼλ�õ���ǰ���ݽ���λ�ã��������б�ʶ
		{
			write_read_flag_add_cache=0;     //����д���Ѷ���ʶλ�û���
			if(i==(measure_data_add_begin-5))   //������ݵ����������������ʼλ��
			{	
				S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
				if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
				{
					//������
					if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //δ��ȡ
					{
						write_read_flag_add_cache=Measure_Data_Begin;		//ָ��ʼλ��
					}
				}
			}
			else 
			{
				S25FL512S_Read(&read_add[0],i,4);  //�����������У���ȡ���ݵĴ洢λ��
				write_read_flag_add_cache=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			}
			if(write_read_flag_add_cache!=0)  //����õ���Ҫд���Ѷ���ʶ��λ��
				S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);  //�����ݿ���Ѷ���ʶλ����λ��д���Ѷ�  0xf0,0xf0,0xf0,0xf0,0xf0
		}		
		if(measure_data_add_ptr>(measure_data_add_end-5*300))  //���4�ſ��д�������ĵ�ַ������β���ܴ��������������һ���������5����24Сʱ��288�Σ�����300�Σ�
		{
			S25FL512S_Erase_Sector(4);
			measure_data_add_ptr=measure_data_add_begin;  //�����ϴ���ɺ󣬰������ŵ���ʼλ��
		}
	}	
		
  //����APPָ����ӦӦ��
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

