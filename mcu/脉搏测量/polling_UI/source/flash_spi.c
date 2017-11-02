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
//À´Ô´¹Ù·½ÍøÕ¾µÄÀı³Ì
////PTA14  SPI-CSÅäÖÃÎªÊä³ö¹Ü½Å£»¿´Ô­ÀíÍ¼
//Íâ²¿µØÖ·À©Õ¹Ê¹ÄÜ£¬Ğ´1²ÅÄÜ²Ù×÷512MBITS
void S25FL512S_Spi_Config(void)
{
	uint32_t srcClock_Hz;
	dspi_master_config_t masterConfig;
	//À´Ô´¹Ù·½ÍøÕ¾µÄÀı³Ì
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
	//PTA14  SPI-CSÅäÖÃÎªÊä³ö¹Ü½Å£»¿´Ô­ÀíÍ¼
	GPIO_PinInit(GPIOA, PIN14_IDX, &IO_config);

	//Ê¹ÄÜS25FL512S_BRWR  0x17,ÎªÁË²Ù×÷¿ØÖÆ¼Ä´æÆ÷	
	//Íâ²¿µØÖ·À©Õ¹Ê¹ÄÜ£¬Ğ´1²ÅÄÜ²Ù×÷512MBITS
	S25FL512S_WriteBR(0x80);		
}



//////////////////////////////////////////////
//º¯Êı¹¦ÄÜ£º½øĞĞµ¥×Ö½ÚÊı¾İ½»»¥
//ÊäÈë²ÎÊı£ºÒª·¢ËÍµÄ×Ö½Ú
//Êä³ö²ÎÊı£º·¢ËÍ×Ö½ÚÊ±¶Áµ½µÄ×Ö½Ú
//////////////////////////////////////////////
uint8_t S25FL512S_ReadWriteByte(uint8_t TxData)
{
	flashXfer.txData =& TxData;
  	flashXfer.dataSize = 1;
	flashXfer.rxData =flashRxData;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_FLASH_BASEADDR, &flashXfer);
	return *flashXfer.rxData; 
}

//¶ÁÈ¡×´Ì¬¼Ä´æÆ÷1£¬ Ö÷ÒªÊÇÎªÁË¶ÁÈ¡0Î»µÄBUSY×´Ì¬Î»
uint8_t S25FL512S_ReadSR1(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_RDSR1);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}

//Ğ´×´Ì¬¼Ä´æÆ÷1
void S25FL512S_WriteSR1(uint8_t sr)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRSR1);
	S25FL512S_ReadWriteByte(sr);
	S25FL512S_CS_H;
}

//¶ÁÈ¡Bank Address Register£¬´Ë¼Ä´æÆ÷µÄ7Î»EXTADD±êÊ¾ÁËËÄ×Ö½ÚµØÖ·»òÕßÈı×Ö½ÚµØÖ·
//µ±Õâ1Ê±ÎªËÄ×Ö½ÚµØÖ·£¬Îª0Ê±ÔòÎªÈı×Ö½ÚÅäºÏµÍ0¡¢1Î»½øĞĞÑ¯Ö·¡£0Î»¶ÔÓ¦512MbµÄA24
//1Î»¶ÔÓ¦512MbµÄA25£¬Í¨¹ı26Î»µØÖ·ÊµÏÖ512MbµÄÑ¯Ö·ÄÜÁ¦¡£
uint8_t S25FL512S_ReadBR(void)
{
	uint8_t byte=0;
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRRD);
	byte=S25FL512S_ReadWriteByte(0xff);
	S25FL512S_CS_H;
	return byte;
}
//Í¨¹ı´Ë¼Ä´æÆ÷µÄÅäÖÃ£¬ÊµÏÖ512MbµÄ¸ßµØÖ·²Ù×÷
void S25FL512S_WriteBR(uint8_t br)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_BRWR);
	S25FL512S_ReadWriteByte(br);
	S25FL512S_CS_H;
}

//Ê¹ÄÜĞ´²Ù×÷
void S25FL512S_Write_Enable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WREN);
	S25FL512S_CS_H;
}
//Ê§ÄÜĞ´²Ù×÷
void S25FL512S_Write_Disable(void)
{
	S25FL512S_CS_L;
	S25FL512S_ReadWriteByte(S25FL512S_WRDI);
	S25FL512S_CS_H;
}

//µÈ´ıBUSY¿ÕÏĞ
void S25FL512S_Wait_Busy(void)
{
	while((S25FL512S_ReadSR1()&0x01)==0x01); //µÈ´ıBUSYÎ»Çå¿Õ
}


//¶ÁÈ¡SPI FLASH  ;
//¶ÁµØÖ·
//¶ÁÊı¾İ³¤¶È
//¶Á³öµÄÊı¾İËÍÈë»º³åÇøÖĞ
void S25FL512S_Read(uint8_t * pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead)   
{ 
		uint32_t i;  
		S25FL512S_CS_L;                            //Ê¹ÄÜÆ÷¼ş   
    S25FL512S_ReadWriteByte(S25FL512S_RD);         //·¢ËÍ¶ÁÈ¡ÃüÁî
		S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>24));  //·¢ËÍ32bitµØÖ·
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>16));   
    S25FL512S_ReadWriteByte((uint8_t)((ReadAddr)>>8));  
    S25FL512S_ReadWriteByte((uint8_t)ReadAddr);  
		
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=S25FL512S_ReadWriteByte(0XFF);   //Ñ­»·¶ÁÊı  
    }
		S25FL512S_CS_H; 			    	      
}  




//Ğ´ÈëÉÙÓÚ512¸ö×Ö½ÚµÄÊı¾İ
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


////ÎŞ¼ìÑé S25FL512S SPI FLASH 
void S25FL512S_Write_NoCheck(uint8_t * pBuffer,uint32_t WriteAddr,uint32_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=512-WriteAddr%512; //µ¥Ò³Ê£ÓàµÄ×Ö½ÚÊı		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//²»´óÓÚ512¸ö×Ö½Ú
	while(1)
	{	   
		S25FL512S_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//Ğ´Èë½áÊøÁË
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //¼õÈ¥ÒÑ¾­Ğ´ÈëÁËµÄ×Ö½ÚÊı
			if(NumByteToWrite>512)pageremain=512; //Ò»´Î¿ÉÒÔĞ´512¸ö×Ö½Ú
			else pageremain=NumByteToWrite; 	  //²»¹»512¸ö×Ö½ÚÁË
		}
	}	    
} 






//Ğ´Ò»´®Êı¾İµ½ÖÆ¶¨µÄflashµØÖ·ÖĞ

void S25FL512S_Write(uint8_t * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
	uint32_t secpos;
	uint32_t secoff;
	uint32_t secremain;	     

 	secpos=WriteAddr/(1024*256);//ÉÈÇøµØÖ·  Ã¿¸öÉÈÇøÊÇ256K×Ö½Ú256x1024=nnn byte
	secoff=WriteAddr%(1024*256);//ÔÚÉÈÇøÄÚµÄÆ«ÒÆ
	secremain=(1024*256)-secoff;//ÉÈÇøÊ£Óà¿Õ¼ä´óĞ¡   
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//²»´óÓÚ(1024*256)¸ö×Ö½Ú
	while(1) 
	{	
		S25FL512S_Write_NoCheck(pBuffer,WriteAddr,secremain);//Ğ´ÒÑ¾­²Á³ıÁËµÄ,Ö±½ÓĞ´ÈëÉÈÇøÊ£ÓàÇø¼ä. 				   
		if(NumByteToWrite==secremain)break;//Ğ´Èë½áÊøÁË
		else//Ğ´ÈëÎ´½áÊø
		{
			secpos++;//ÉÈÇøµØÖ·Ôö1
			secoff=0;//Æ«ÒÆÎ»ÖÃÎª0 	 
			pBuffer+=secremain;  //Ö¸ÕëÆ«ÒÆ
			WriteAddr+=secremain;//Ğ´µØÖ·Æ«ÒÆ	   
			NumByteToWrite-=secremain;				//×Ö½ÚÊıµİ¼õ
			if(NumByteToWrite>(1024*256))secremain=1024*256;	//ÏÂÒ»¸öÉÈÇø»¹ÊÇĞ´²»Íê
			else secremain=NumByteToWrite;			//ÏÂÒ»¸öÉÈÇø¿ÉÒÔĞ´ÍêÁË
		}	 
	}	 
}


//´«ÈëÒª²Á³ıµÄÉÈÇøºÅ
void S25FL512S_Erase_Sector(uint32_t Dst_Addr)   
{  
		Dst_Addr=Dst_Addr*256*1024;     //256K×Ö½Ú
    S25FL512S_Write_Enable();                  //SET WEL 	 
    S25FL512S_Wait_Busy();   
  	S25FL512S_CS_L;                          //Ê¹ÄÜÆ÷¼ş   
    S25FL512S_ReadWriteByte(S25FL512S_SE);      //·¢ËÍÉÈÇø²Á³ıÖ¸Áî 
  	S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>24));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>16));
		S25FL512S_ReadWriteByte((uint8_t)(Dst_Addr>>8));
		S25FL512S_ReadWriteByte((uint8_t)Dst_Addr);
		S25FL512S_CS_H;                              //È¡ÏûÆ¬Ñ¡     	      
    S25FL512S_Wait_Busy();				   //µÈ´ı²Á³ıÍê³É
}  







//°Ñ²ÎÊı±£´æµ½flashÖĞ
uint8_t user_set_para[SET_PARA_TOTAL_NUM];

//flash±ØĞëÏÈ²Á³ı²ÅÄÜ¸ÄĞ´£»²ÁµÄ×÷ÓÃµÄÊÇ°Ñ´æ´¢ÇøÓòµÄÄÚÈİÈ«²¿±äÎª0xff£¬Ğ´Êı¾İµÄÊ±ºò²ÅÄÜ°Ñ0xff±äÎªÄ¿±êÖµ
//Ä¿Ç°µÄ²Ù×÷·½·¨256kbyte/sec£¬Ïà¹ØµÄgroupÕ¼ÓÃÒ»¸ösector£¬È»ºóÀË·Ñ
//mcuµÄramÖ»ÓĞ256kbyte
void Sava_para_to_flash(void)
{
	S25FL512S_Erase_Sector(0);
	S25FL512S_Write(user_set_para,0,SET_PARA_TOTAL_NUM);
}




//±£´æÄ¬ÈÏµÄ²âÁ¿²ÎÊıµ½´æ´¢ÇøÖĞµÄÉÈÇø0
//Õâ¸ö²ÎÊı×éÕ¼ÓÃÒ»¸öÉÈÇø£¬´æ´¢µ½0ºÅÉÈÇø
//change name to Save_para_as_default_Value is better
void Save_para_as_define(void)  
{
		user.age=50;   //ÓÃ»§ÄêÁä
		user.sex=1;  //ÓÃ»§ĞÔ±ğ  1ÄĞ  2 Å®
		user.height=170; //Éí¸ß 
		user.weight=70;  //ÌåÖØ
		
		alarm.HBlood_LowValue=80;   //¸ßÑ¹ÏÂÏŞ
		alarm.HBlood_HighValue=180;  //¸ßÑ¹ÉÏÏŞ
		alarm.LBlood_LowValue=40;    //µÍÑ¹ÏÂÏŞ
		alarm.LBlood_HighValue=90;   //µÍÑ¹ÉÏÏŞ
		alarm.HeartRate_LowValue=60;  //ĞÄÂÊÏÂÏŞ
		alarm.HeartRate_HighValue=160;  //ĞÄÂÊÉÏÏŞ
		
		MeasurePara.Groupnum=11;  //×î´ó²âÁ¿×éÊı
		MeasurePara.Timelong=3;   //²âÁ¿Ê±³¤3s
		MeasurePara.Timeinterval=2; //²âÁ¿¼ä¸ô2ms
		MeasurePara.Eletrode=1;    //²âÁ¿µç¼«
		
		Measure_Mode=4;    //4´øÍêÕûÊı¾İ²âÁ¿   2²»´øÊı¾İ²âÁ¿   
		
		AutoMeasurePara.Timelong=24;        //×Ô¶¯²âÁ¿Ê±³¤ Ğ¡Ê±
		AutoMeasurePara.Timeinterval=10;    //×Ô¶¯²âÁ¿¼ä¸ô·ÖÖÓ
		
		StandbyTime=30;  //´ı»ú30Ãë
		GMT_num=8;      //Ê±Çø
		mmHg_or_kPa=1;  //ÏÔÊ¾µ¥Î»   1 mmHg   kPa
		Ble_OnOff=2;    //À¶ÑÀ¿ª¹Ø£¬Ä¬ÈÏ¹Ø
		NoDisturb_Mode=1;   //·À´òÈÅ
		
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




//´Óflash´æ´¢Æ÷µÄ0ºÅÉÈÇø¿é¶ÁÈ¡ÓÃ»§ÉèÖÃ²ÎÊı  £»¹æ¶¨Õû¸öÉÈÇø´æ´¢ÓÃ»§ÉèÖÃ²ÎÊı
//¶ÁÈ¡µ½user_set_paraÊı×éÖĞ,user_set_paraÊı×é¸÷ÔªËØÆ«ÒÆÔÚflash_spi.hÖĞÓÉ_FLASH_ADD_TYPEµÄÃ¶¾Ù¶¨Òå
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
	
	if(user.age==0xff &&user.sex==0xff)	//ÓÃ»§Ã»ÓĞÉèÖÃ¹ú²ÎÊı
	{
		Save_para_as_define();	
	}	
	
	
}	





uint32_t Save_Measure_Index_Ptr=3;

uint8_t READ_Record_data_cache[RECORD_NUM*16];


//´æÈëÒ»Ìõ²âÁ¿½á¹û£¬Ò»Ìõ½á¹ûÎª16¸ö×Ö½Ú£»flash¿ÉÒÔ°´Ö±½ÓĞ´Èë¶ÔÓÚ²Á¹ıÖ®ºóµÄÇøÓò
//´æµÄÄÚÈİÎª£º0xfc(Ò»ÌõĞÂ¼ÇÂ¼) RTCÊ±¼ä£¨4×Ö½Ú£© Ê±Çø1×Ö½Ú  ²âÁ¿½á¹û4×Ö½Ú ÓÃ»§ĞÅÏ¢4×Ö½Ú  ¿ÕÁ½¸ö0xffÔ¤Áô
uint8_t Save_Measure_Result_To_Flash (uint32_t time, uint8_t *data)
{
	uint8_t write_data[16]={0};
	
	write_data[0]=save_state_new;    //´æÈëÒ»ÌõĞÂÊı¾İ£¬ÒÔºó¿ÉÒÔÅĞ¶ÏÊı¾İÊÇ·ñ´æÔÚ£¬
	time++;                         //¼ÓÈëÒ»ÃëÊ±¼ä£¬Òò·´ËãµÄÊ±ºòÉÙÒ»Ãë
	write_data[1]=(time>>24)&0xff;  //Ê±¼ä´Á
	write_data[2]=(time>>16)&0xff;
	write_data[3]=(time>>8)&0xff;
	write_data[4]=(time)&0xff;
	write_data[5]=GMT_num;
	write_data[6]=data[0];//À´×ÔÂö¸ĞĞ¾Æ¬
	write_data[7]=data[1];
	write_data[8]=data[2];
	write_data[9]=data[3];
	if(user.sex==1) //±íÊ¾ÄĞ
		write_data[10]=0x01;
	else 
		write_data[10]=0x00;
	
	write_data[11]=user.age;
	write_data[12]=user.height;
	write_data[13]=user.weight;
	
	if(Save_Measure_Index_Ptr<16383) //256KB   ·Ö³É 16384¸ö16×Ö½ÚĞ¡ÇøÓò£¬½øĞĞµÚÒ»Ìõ¼ÇÂ¼µÄ´æ´¢
	{
		Save_Measure_Index_Ptr++;
	}
	else //Èç¹ûÒÑ¾­´æÍêÒ»¸öÕû¿é////¿é==256KByte==ÉÈÇø
	{//ptr>16383£¬Ö¤Ã÷µ±Ç°¿éÒÑ¾­Ğ´Âú£¬Ìø×ªµ½¿éÍ·£¬ÖØĞÂĞ´
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-RECORD_NUM)*16,16*RECORD_NUM);  //´ÓÎ²°ÍÉÏ¶Á³öÊı¾İ¶Á³öÀ´600Ìõ£¬±£Ö¤²»±»²Á³ı
		S25FL512S_Erase_Sector(SAVE_MEASURE_RESULT_SECTOR);//²Á³ıÕû¸öÉÈÇø/¿é
		S25FL512S_Write(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(4)*16,16*RECORD_NUM);  //¶Á³öÀ´µÄÊı¾İĞ´ÈëÉÈÇøµÄ¿ªÍ·µÄ²¿·Ö
		////S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16); //ÔÚ¿éµÄ¿ªÍ·Ğ´Ò»×éÊı¾İ£¬Ö»ÎªÁË»ñÈ¡ÊÇ·ñÓĞĞ´Âú¹ıÒ»´Î
		Save_Measure_Index_Ptr=3+RECORD_NUM;    //Ô¤Áô³ö4¸ö¿é£¬´Ë´¦µÈÓÚ4£¬Ïàµ±ÓÚ´ÓµÚËÄ¿é´æ´¢£¬0¡¢1¡¢2¡¢3½øĞĞÁËÔ¤Áô		
	}
	
	S25FL512S_Write(write_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr)*16,16);  //Ğ´ÈëÒ»Ìõ²âÁ¿½á¹û
	return 1;
}



uint32_t  record_begin_index;
uint32_t  record_total_num;
uint32_t  display_record_index;


//¶Á³ö²âÁ¿½á¹û  ²»°üÀ¨²âÁ¿Êı¾İ
//·­²éÊı¾İ»òÉÏ´«¼ÇÂ¼µÄÊ±ºò£¬¶Á³
//´ÓflashÖĞ¶Á³öµÄÊı¾İ£¬·ÅÔÚcacheÖĞ£¬cacheÊÇÎ²¶ÔÆëµÄ£¬±ãÓÚ´ÓcacheµÄ×îºó´¦¿ªÊ¼·­²é
uint8_t Read_Record_Data_From_Flash(uint32_t para)
{
	uint32_t i;

	//ÎªÁËÎ²¶ÔÆë
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //µ±Ç°Î»ÖÃ²»¹»RECORD_NUMÌõ¼ÇÂ¼£¬¿ÉÄÜÔÚÄ©¶ËÓĞÊı¾İ£¬ÔòÔÚÄ©¶Ë½øĞĞ¶ÁÈ¡
	{//Ğ¡ÓÚ600¸öµÄÊ±ºò£¬ÏÈ¶Á³öËùÓĞµÄ¼ÇÂ¼£»·­¶Á×ÜÊÇ¶Á×îĞÂµÄ
		////´ÓÄ©Î²¶Á
		//S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //Êı¾İ¶Á³öÀ´
		//´Ó¿ªÍ·¶Á	
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //Êı¾İ¶Á³öÀ´
	}
	else
	{//´óÓÚ600¸öÊı¾İ
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//´¦Àí·­²éÔÚÒº¾§ÆÁÄ»ÉÏÏÔÊ¾;cacheÊÇÈ«¾Ö±äÁ¿
	for(i=0;i<RECORD_NUM;i++)
	{
		if((READ_Record_data_cache[i*16]==save_state_new )||(READ_Record_data_cache[i*16]==save_state_read))  //ÓĞÊı¾İ¾Í½øĞĞÍ³¼Æ£¬²»¹ÜÊÇÒÑ¶Á»òÕßÄ©¶Á
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	
	return 1;
}




//²âÁ¿½á¹û´æ·ÅÔÚ1ºÅ¿éÖĞ£¬
//ÔÚ1ºÅÖĞÒÔÊ±¼äÎª¹Ø¼ü×Ö£¬ÕÒµ½Ê±¼ä×î½üµÄÒ»´Î½á¹û
//ÕÒ×î½üÒ»´Î²âÁ¿½á¹ûµÄ´æ´¢Î»ÖÃ¸³¸øÒ»¸öÈ«¾Ö±äÁ¿Save_Measure_Index_Ptr
void Find_Save_Index_Ptr(void)
{
	
	uint32_t ptr1=4,ptr2=8191,ptr3=16383;
	uint32_t time1,time2,time3;
	uint8_t  read_data[16]={0};

	S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE,16);

	//ÅĞ¶ÏflashÊÇ·ñ×ö¹ıĞ´Âú²Á³ı
	if((read_data[0]!=0xff)&& (read_data[15]!=0xff))//²Á³ı¹ıµÄ
	{
		while(1)
		{
			//read 3 Ìõ²âÁ¿½á¹û£¬±È½Ï¿´£¬ÄÄÒ»¸öÊÇ×îĞÂµÄ
			//´ÓÍ·¡¢Î²¡¢ÖĞ¼ä¶Á£¬¶ş·Ö²éÕÒ
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr1*16,16);
			time1=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr3*16,16);
			time3=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			
			if(time1>time2) //´æ´¢Î»ÖÃ¿¿Ç°µÄÊ±¼ä´ó£¬ÒÑ¾­ÓĞ¸²¸Ç
			{
				ptr3=ptr2;
				ptr2=(ptr3-ptr1)/2+ptr1;//µ÷ÕûÓÒ±ß½ç
				if(ptr2==ptr1)   
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else
			 {
				ptr1=ptr2;//µ÷Õû×ó±ß½ç
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
	else//Ã»Ğ´Âú¹ı£¬Ã»²Á³ı¹ı
	{
		while(1)
		{
			S25FL512S_Read(read_data,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+ptr2*16,16);
			time2=(read_data[1]<<24)+(read_data[2]<<16)+(read_data[3]<<8)+(read_data[4]);
			if(time2==0xffffffff)//ºó°ë²¿·ÖÃ»ÓĞĞ´
			{
				ptr3=ptr2;//ÒÆ¶¯ÓÒ±ß½ç
				ptr2=(ptr3-ptr1)/2+ptr1;		
				if(ptr2==ptr1)
				{
					Save_Measure_Index_Ptr=ptr2;
					break;
				}
			}
			else //ºó°ë²¿·ÖÒÑ¾­Ğ´ÁË
			{
				ptr1=ptr2;//ÒÆ¶¯×ó±ß½ç
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







uint32_t measure_data_save_ptr=Measure_Data_Begin;   //Ìø¹ı4MµÄ¿Õ¼ä¿ªÊ¼½øĞĞ´æ´¢

//º¯Êı¹¦ÄÜ£¬ÔÚ½øĞÎ´øÊı¾İ²âÁ¿Ê±£¬ÒªÏÈ±£Ö¤´æ´¢Êı¾İµÄ¿Õ¼äÊÇ±»²Á³ı¹ıµÄ£¬Òò²Á³ı×îĞ¡Îª256K£¬ËùÒÔµ±Ç°ËùÔÚ¿é²»½øĞĞ²Á³ı
//½øĞĞÒ»´Î²âÁ¿ºó£¬ËùĞèÒªµÄ¿Õ¼ä²»×ã£¬¶ÔºóÒ»¸ö½øĞĞ£¬Òò½øĞĞÑ­»·Ğ´£¬´Ë·½·¨¿ÉÒÔ±£Ö¤Ğ´²Ù×÷µÄµØÖ·Îª²Á³ı¹ıµÄ
//µ±Ç°ÇøÓò´æ´¢ÁË3ÌõÖ®ºó£¬¿ÉÄÜ¾ÍÓ¦¸Ã²Á³ıÏÂÒ»¸öÇøÓò
void Erase_Next_Sector(void)
{
	uint32_t sector;
	uint32_t cursector;
	cursector=measure_data_save_ptr/(1024*256);
	sector=(measure_data_save_ptr+Measure_Data_Maximum)/(1024*256);
	if(sector>cursector) //µ±Ç°¿éÎŞ·¨Âú×ã×î´óÊı¾İÁ¿µÄ´æ´¢
	{
		S25FL512S_Erase_Sector(sector);
	}
}


//´æµ½4ºÅ¿éÖĞÊı¾İÖ¸ÕëÇø
#define measure_data_add_begin (4*256*1024)
#define measure_data_add_end   (5*256*1024)


uint32_t measure_data_add_ptr=measure_data_add_begin;


//¸üĞÂmeasure_data_add_ptrÎ»ÖÃ£¬Ã¿Ôö¼ÓÒ»´Î£¨ÍêÕû£©²âÁ¿Êı¾İ¸üĞÂÒ»´Î£¬¸üĞÂÇ°°Ñµ±Ç°²âÁ¿Êı¾İ½á¹ûÎ»ÖÃ·Åµ½
//measure_data_add_ptrµØÖ·ÖĞ£¬measure_data_add_ptr×ÔÔö5£¬Èç¹ûĞ´µ½¿é½áÊøÎ»ÖÃ£¬Ôò·µ»Ø¿ªÊ¼Î»ÖÃ
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




//ÕÒ²âÁ¿Êı¾İµÄ×îºóÒ»¸ö´æ´¢µØÖ·µÄÖ¸Õë
//ÔÚ4ºÅ¿éÖĞ²éÕÒ×îºóÒ»´Î´æ·Å²âÁ¿Êı¾İµØÖ·µÄÎ»ÖÃµÄÖ¸Õë
//²éÕÒ½áÊøºómeasure_data_add_ptrµÈÓÚÕÒµ½µÄÎ»ÖÃµÄÏÂÒ»¸öÎ»ÖÃ
//measure_data_save_ptr×îºóÒ»´Î´æ´¢Êı¾İ½á¹ûÎ»ÖÃ
//ÕÒ²âÁ¿Êı¾İµÄ£¬Í¬ÑùÊÇ¶ş·Ö·¨
void Find_measure_data_add_prt(void)  
{
		uint8_t read_add[5];
		uint32_t ptr1=0;
		uint32_t ptr3=(measure_data_add_end-1)/5;
		uint32_t ptr2=ptr3 /2;
	
		S25FL512S_Read(&(read_add[4]),measure_data_add_begin+4,1); 
		if(read_add[4]==0xff)   //Êı¾İÇø¶Áµ½Îª0xff£¬±íÊ¾Ã»ÓĞÊı¾İ
		{
			measure_data_add_ptr=measure_data_add_begin;  //Êı¾İË÷ÒıÖ¸ÏòË÷Òı´æ´¢ÇøµÄ¿ªÊ¼
			measure_data_save_ptr=Measure_Data_Begin;    //Êı¾İ´æ´¢Ö¸ÕëÖ¸ÏòÊı¾İÇøµÄ¿ªÊ¼Î»ÖÃ
		}
		else
		{
			while(1)
			{
				S25FL512S_Read(&(read_add[4]),measure_data_add_begin+ptr2*5+4,1);
				if(read_add[4]==0xff)  //Èç¹û¶Áµ½µÄÊı¾İÎª0xff£¬Ôò±íÊ¾¶ÁÈ¡¿ÕÊı¾İÇø 
				{
					ptr3=ptr2;  
					ptr2=(ptr3-ptr1)/2+ptr1;
					if(ptr2==ptr1) //µ±²éÕÒÊ±£¬µ±Ç°²éÕÒÎ»ÖÃµÈÓÚ¿ªÊ¼Î»ÖÃ£¬Ôò±íÊ¾ÒÑ¾­ÕÒµ½
					{
						S25FL512S_Read(&(read_add[0]),measure_data_add_begin+ptr2*5,4);
						measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
						measure_data_add_ptr=measure_data_add_begin+ptr2*5+5;
						break;  
					}
				}
				else
				{
					ptr1=ptr2;//¿ªÊ¼Ö¸Ïòµ±Ç°²éÕÒÎ»ÖÃ
					ptr2=(ptr3-ptr1)/2+ptr1; //µ±Ç°²éÕÒÎ»ÖÃÏòºóÆ«ÒÆÒ»°ë
					if(ptr2==ptr1)  //µ±²éÕÒÊ±£¬µ±Ç°²éÕÒÎ»ÖÃµÈÓÚ¿ªÊ¼Î»ÖÃ£¬Ôò±íÊ¾ÒÑ¾­ÕÒµ½
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





//²éÕÒÎ´ÉÏ´«Êı¾İµÄÎ»ÖÃ
uint32_t upload_measure_data_add_ptr=0;  //¼ÇÂ¼ÒªÉÏ´«µÄÊı¾İµÄ´æ´¢µØÖ·Ë÷ÒıµÄµØÖ·
uint32_t upload_measure_data_save_ptr=0; //¼ÇÂ¼ÒªÉÏ´« Êı¾İµÄ´æ´¢µØÖ·

//²éÕÒÄ©ÉÏ´«Êı¾İµÄÍ·£¬ÈçÕÒµ½·µ»Ø1 £¬Ä©ÕÒµ½·µ»Ø0
//ÊÕµ½ÉÏ´«Ö¸Áîºó£¬ÏÈ´«Ã»ÓĞÉÏ´«¹ıµÄ²âÁ¿½á¹û£¬ÔÙ´«Êı¾İ
uint8_t Find_Not_Upload_add_ptr(void)
{
	uint8_t read_add[5]={0};
	uint8_t read_data[20]={0};
	uint8_t find_flag=0;
	
	upload_measure_data_add_ptr=measure_data_add_ptr-10;  
	//´Óµ±Ç°´æ´¢µØÖ·Ë÷ÒıµÄµØÖ·¼õ10µÄµØ·½¿ªÊ¼²éÕÒ£¬ÒòË÷Òı·½Ê½Ô­Òò£¬Êµ¼ÊÊı¾İµ½Ë÷ÒıµØÖ·Æ«ÒÆ10

	while(1)
	{
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		if(read_add[4]!=0xff)
		{
			upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			
			S25FL512S_Read(&read_data[0],upload_measure_data_save_ptr,20);
			//²âÁ¿Êı¾İÓĞ¹Ì¶¨µÄÊı¾İÍ·{0xaa,0xbb,0xcc,0xdd,0xee,0x11,0x22,0x33,0x44,0x55,time1,time2,time3,time,gmt,0xfc,0xfc,0xfc,0xfc,0xfc}
			//5¸ö0xfc±íÊ¾Ä©¶ÁÈ¡£¬µ±Îª0xf0Ê±±íÊ¾¶ÁÈ¡
			if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
			{
				//ÓĞÊı¾İ
				if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //Î´¶ÁÈ¡
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
				else  //ÒÑ¾­¶ÁÈ¡¹ıµÄ
				{
					break;  //ÕÒµ½×îºóÒ»´Î¶ÁÈ¡µÄÎ»ÖÃ
				}
			}
			else
			{
				break; //µ±Ç°Î»ÖÃÊı¾İÒÑ¾­²»´æÔÚ
			}			
		}
		else
		{
			break;//¼ÇÂ¼Ö¸ÁîÕÒµ½ÁËÍ·
		}
		
	}

	
	if(find_flag)  //Èç¹ûÕÒµ½ÁËÄ©ÉÏ´«µÄÊı¾İ
	{
		upload_measure_data_add_ptr+=5;	
		S25FL512S_Read(&read_add[0],upload_measure_data_add_ptr,5);
		upload_measure_data_save_ptr=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
		return 1;
	}
	return 0;
}



//ÉÏ´«Êı¾İ ²ÎÊı±êÊ¶ÓÉUSB»òÕßBLEÉÏ´«

# define Date_1970  2208988800000
//×ª»»Ê±¼äÎªUTC¸ñÊ½
uint64_t RTC_TO_UTC(uint32_t time_num)
{
	uint64_t num;
	num=((uint64_t)time_num*1000)+(uint64_t)Date_1970;
	return num;
}


//ÉÏ´«Ò»Ìõ²âÁ¿½á¹û £¬°´ÆÕÈÊµÄÉÏ´«Í¨ĞÅĞ­ÒéÀ´ÊµÏÖ
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
	
	//RTCÊ±¼ä×ª»»µ½UTCÊ±¼ä												
	rtc_time_num=(READ_Record_data_cache[num*16+1]<<24)+(READ_Record_data_cache[num*16+2]<<16)+(READ_Record_data_cache[num*16+3]<<8)+(READ_Record_data_cache[num*16+4]);	
	rtc_time_num=rtc_time_num-READ_Record_data_cache[num*16+5]*3600;//°ÑÊ±ÇøÆ«²î¼õµô
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
	
	//ğ©ÂöID
	
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
	
	//´«ÍêÒ»Ìõ²âÁ¿½á¹ûºó£¬½øĞĞÊı¾İµÄÒÑ¶Á±ê¼Ç
	
	if((Save_Measure_Index_Ptr-3)>((RECORD_NUM-1)-num))
	{
		write_add=Save_Measure_Index_Ptr-(RECORD_NUM-1-num);
	}
	else
	{
		write_add=16384-((RECORD_NUM-1-num)-(Save_Measure_Index_Ptr-4));
	}
	
//	S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //ÔÚ²âÁ¿½á¹ûµÄ¶ÁÈ¡±êÊ¶Î»Ğ´ÈëÒÑ¶Á±êÊ¶
//È·ÈÏÖ¸ÁîÖ®ºóÔÙĞ´»ØflashÖĞ	
//	delay_20000(500);// ´«ÍêÒÔºó×öÒ»µã¿ÕÑ­»·ÑÓÊ±
}



//ÉÏ´«Ò»´ÎÍêÕû²âÁ¿µÄ²âÁ¿Êı¾İ£¬×î¶à66k×Ö½Ú
//°´ÕÕÆÕÈÊµÄÍ¨ĞÅĞ­ÒéÀ´ÉÏ´«
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
		
	if(add_num==(measure_data_add_begin-5))  //ÅĞ¶Ï²âÁ¿Êı¾İÊÇ·ñÔÚ¿ª¶Ë´¦
	{	
		S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
		if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)   //ÅĞ¶ÏÓĞÎŞÊı¾İ£¬ÎªÕæ±íÊ¾ÓĞÊı¾İ
		{
			//ÓĞÊı¾İ
			if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //Î´¶ÁÈ¡
			{
				save_data_add_cache=Measure_Data_Begin;  //µÃµ½Êı¾İ¿ªÊ¼µØÖ·
				S25FL512S_Read(&read_add[0],measure_data_add_begin,4);
				save_data_add_cache_end=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);		 //Êı¾İ½áÊøµØÖ·
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
	rtc_time_num=rtc_time_num-read_data[14]*3600;//°ÑÊ±ÇøÆ«²î¼õµô
	utc_time_num=RTC_TO_UTC(rtc_time_num);
	utc_time_num-=3000;

	while (save_data_add_cache<save_data_add_cache_end)  //½«6k (212x31)³ËÒÔÏîÄ¿Êı
	{
		
			//·¢ËÍÊı¾İ×éÊı£¬Êı¾İÀàĞÍÎª02£¬ÏÖÔÚ¾ùÎª¹Ì¶¨31×é
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
			
			//ğ©ÂöID
			
			
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
//		delay_20000(500);// ´«ÍêÒÔºó×öÒ»µã¿ÕÑ­»·ÑÓÊ±			
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
				
//			delay_20000(500);// ´«ÍêÒÔºó×öÒ»µã¿ÕÑ­»·ÑÓÊ±
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






//µ±ÊÕµ½Êı¾İÍ¬²½Ö¸Áîºó£¬µ÷ÓÃ´Ëº¯Êı
//ÔÚmainº¯ÊıÀï£¬½Óµ½Í¬²½ÉÏ´«Ö¸ÁîÖ®ºó£¬Ö´ĞĞ¸ÃÃüÁî
//Í¬ÏÔÊ¾µÄº¯ÊıRead_Record_Data_From_FlashÒ»ÑùµÄÊµÏÖÔ­Àí
void Upload_data(uint8_t port)
{
	uint32_t i;
	uint8_t data[11]={0xa5,0x51,0x21,0x00,0x01,0x00,0x01,0x33,0x01,0x04,0x46};
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //µ±Ç°Î»ÖÃ²»¹»RECORD_NUM£¨600£©Ìõ¼ÇÂ¼£¬¿ÉÄÜÔÚÄ©¶ËÓĞÊı¾İ£¬ÔòÔÚÄ©¶Ë½øĞĞ¶ÁÈ¡
	{
		///S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //Êı¾İ¶Á³öÀ´
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //Êı¾İ¶Á³öÀ´
	}
	else   //¶ÁÈ¡RECORD_NUMÌõ¼ÇÂ¼
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);
	}
	
	record_begin_index=RECORD_NUM;
	
	//²éÕÒ¿ªÊ¼ÉÏ´«Î»ÖÃ
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //Èç¹ûÊÇĞÂÊı¾İÎ´ÉÏ´«¹ı  Î´ÉÏ´«Ê±µÈÓÚ0xfc  ÉÏ´«ºóÎª0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//ÖğÌõÉÏ´«²âÁ¿½á¹û
	for(i=record_begin_index;i<RECORD_NUM;i++)
	{
		Upload_One_Record(port,i);  //ÉÏ´«²âÁ¿½á¹û
	}
	
	
	
	if(port==USB_COMM)   //Ö»USB´«Êä²âÁ¿Êı¾İ
	{
		if(measure_data_add_ptr==measure_data_add_begin) //Ã»ÓĞÊı¾İ
		{		
		}
		else if(measure_data_add_ptr==measure_data_add_begin+5)  //µ±Ç°Ö»ÓĞÒ»ÌõÊı¾İ
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
			Upload_One_Data_Record(port,upload_measure_data_add_ptr);
		}
		else if(Find_Not_Upload_add_ptr())  //ÔÚÊı¾İ±íÖĞ²éÕÒÄ©ÉÏ´«µÄÊı¾İÍ·£¬Èç¹ûÕÒµ½·µ»ØÕæ
		{
			//´ÓÕÒµ½µÄÄ©ÉÏ´«µÄÍ·£¬µ½µ±Ç°×îºó²âÁ¿Êı¾İµÄ½áÊøÎ»ÖÃ£¬ÖğÌõÊı¾İÉÏ´«
			if(upload_measure_data_add_ptr==measure_data_add_begin)  //Èç¹ûÕÒµ½ÁËÎ»ÖÃÔÚÊı¾İÍ·£¬¸Õ´ÓµØÖ·Ë÷ÒıµÄÇ°Ò»¸ö¿ªÊ¼´«
			{
				upload_measure_data_add_ptr=measure_data_add_begin-5;
			}
			for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)
			{
				Upload_One_Data_Record(port,i);  //ÉÏ´«Ò»×éÊı¾İ£¨N*6000£©
			}		
//			if(measure_data_add_ptr>(measure_data_add_end-5*300))  //Èç¹û4ºÅ¿éÖĞ´æ·ÅË÷ÒıµÄµØÖ·¾àÀë¿é½áÎ²ËùÄÜ´æµÄË÷Òı¸öÊıÉÙÓÚÒ»ÌìµÄÁ¿£¨°´5·ÖÖÓ24Ğ¡Ê±£¬288´Î£¬ÏÖÁô300´Î£©
//			{
//				S25FL512S_Erase_Sector(4);
//				measure_data_add_ptr=measure_data_add_begin;  //Êı¾İÉÏ´«Íê³Éºó£¬°ÑË÷Òı·Åµ½¿ªÊ¼Î»ÖÃ
//			}
		}
	}
	
	  //½øĞĞAPPÖ¸ÁîÏìÓ¦Ó¦´ğ
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

//µ±ÊÕµ½Çå³ıÊı¾İÖ¸Áîºó£¬µ÷ÓÃ´Ëº¯Êı
//°ÑÒÑ¶ÁµÄÊı¾İµÄÒÑ¶Á±êÊ¶¸ü¸ÄºóĞ´»Øflash
//ºÍUpload_dataº¯ÊıµÄÊµÏÖÔ­ÀíÀàËÆ£¬ÖÁÉÙÔÚÕâÀï×ö±êÊ¶¶ø²»ÊÇÉÏ´«
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

	
	if(Save_Measure_Index_Ptr<(3+RECORD_NUM))  //µ±Ç°Î»ÖÃ²»¹»RECORD_NUM£¨600£©Ìõ¼ÇÂ¼£¬¿ÉÄÜÔÚÄ©¶ËÓĞÊı¾İ£¬ÔòÔÚÄ©¶Ë½øĞĞ¶ÁÈ¡
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(16384-(RECORD_NUM-(Save_Measure_Index_Ptr-3)))*16,16*(RECORD_NUM-(Save_Measure_Index_Ptr-3)));  //´ÓÄ©¶Ë°ÑÊı¾İ¶Á³öÀ´
		S25FL512S_Read(&READ_Record_data_cache[(RECORD_NUM-(Save_Measure_Index_Ptr-3))*16],SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+4*16,16*(Save_Measure_Index_Ptr-3));  //´Ó´æ´¢Çø¿ªÊ¼Î»ÖÃ°ÑÊı¾İ¶Á³öÀ´
	}
	else
	{
		S25FL512S_Read(READ_Record_data_cache,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+(Save_Measure_Index_Ptr-RECORD_NUM+1)*16,16*RECORD_NUM);//¶ÁÈ¡600ÌõÊı¾İ
	}
	
	record_begin_index=RECORD_NUM;
	
	//²éÕÒ¿ªÊ¼±êÊ¶ÒÑÉÏ´«Î»ÖÃ
	for(i=0;i<RECORD_NUM;i++)
	{
		if(READ_Record_data_cache[i*16]==save_state_new)  //Èç¹ûÊÇĞÂÊı¾İÎ´ÉÏ´«¹ı  Î´ÉÏ´«Ê±µÈÓÚ0xfc  ÉÏ´«ºóÎª0xf0
		{
			record_begin_index=i;
			record_total_num=RECORD_NUM-i;
			i=RECORD_NUM;
			display_record_index=RECORD_NUM-1;
		}
	}
	//ÖğÌõ±êÊ¶ÒÑÉÏ´«²âÁ¿½á¹ûĞ´»Øflash
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
			S25FL512S_Write(&write_readflag,SAVE_MEASURE_RESULT_SECTOR*SECTOR_SIZE+write_add*16,1);  //ÔÚ²âÁ¿½á¹ûµÄ¶ÁÈ¡±êÊ¶Î»Ğ´ÈëÒÑ¶Á±êÊ¶
	}
	

	if(measure_data_add_ptr==measure_data_add_begin+5)  //µ±Ç°Ö»ÓĞÒ»ÌõÊı¾İ
	{
		write_read_flag_add_cache=Measure_Data_Begin;			
		S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);
	}
	else if(Find_Not_Upload_add_ptr())   //²éÕÒ²âÁ¿Êı¾İµÄÄ©±êÊ¶ÉÏ´«µÄ¿ªÊ¼Î»ÖÃ
	{
		//´ÓÕÒµ½µÄÄ©ÉÏ´«µÄÍ·£¬µ½µ±Ç°×îºó²âÁ¿Êı¾İµÄ½áÊøÎ»ÖÃ£¬ÖğÌõÊı¾İÉÏ´«
		if(upload_measure_data_add_ptr==measure_data_add_begin)
		{
			upload_measure_data_add_ptr=measure_data_add_begin-5;
		}
		for(i=upload_measure_data_add_ptr;i<measure_data_add_ptr-5;i+=5)  //´Ó¿ªÊ¼Î»ÖÃµ½µ±Ç°Êı¾İ½áÊøÎ»ÖÃ£¬ÖğÌõ½øĞĞ±êÊ¶
		{
			write_read_flag_add_cache=0;     //½øĞĞĞ´ÈëÒÑ¶Á±êÊ¶Î»ÖÃ»º´æ
			if(i==(measure_data_add_begin-5))   //Èç¹ûÊı¾İµÈÓÚÊı¾İË÷Òı¿éµÄÆğÊ¼Î»ÖÃ
			{	
				S25FL512S_Read(&read_data[0],Measure_Data_Begin,20);
				if(read_data[0]==0xaa &&read_data[1]==0xbb &&read_data[2]==0xcc && read_data[3]==0xdd && read_data[4]==0xee)
				{
					//ÓĞÊı¾İ
					if(read_data[15]==0xfc &&read_data[16]==0xfc &&read_data[17]==0xfc && read_data[18]==0xfc && read_data[19]==0xfc)  //Î´¶ÁÈ¡
					{
						write_read_flag_add_cache=Measure_Data_Begin;		//Ö¸Ïò¿ªÊ¼Î»ÖÃ
					}
				}
			}
			else 
			{
				S25FL512S_Read(&read_add[0],i,4);  //´ÓÊı¾İË÷ÒıÖĞ£¬¶ÁÈ¡Êı¾İµÄ´æ´¢Î»ÖÃ
				write_read_flag_add_cache=(read_add[0]<<24)+(read_add[1]<<16)+(read_add[2]<<8)+(read_add[3]);
			}
			if(write_read_flag_add_cache!=0)  //Èç¹ûÄÃµ½ÁËÒªĞ´ÈëÒÑ¶Á±êÊ¶µÄÎ»ÖÃ
				S25FL512S_Write(write_read_flag,write_read_flag_add_cache+15,5);  //ÔÚÊı¾İ¿éµÄÒÑ¶Á±êÊ¶Î»ËùÔÚÎ»ÖÃĞ´ÈëÒÑ¶Á  0xf0,0xf0,0xf0,0xf0,0xf0
		}		
		if(measure_data_add_ptr>(measure_data_add_end-5*300))  //Èç¹û4ºÅ¿éÖĞ´æ·ÅË÷ÒıµÄµØÖ·¾àÀë¿é½áÎ²ËùÄÜ´æµÄË÷Òı¸öÊıÉÙÓÚÒ»ÌìµÄÁ¿£¨°´5·ÖÖÓ24Ğ¡Ê±£¬288´Î£¬ÏÖÁô300´Î£©
		{
			S25FL512S_Erase_Sector(4);
			measure_data_add_ptr=measure_data_add_begin;  //Êı¾İÉÏ´«Íê³Éºó£¬°ÑË÷Òı·Åµ½¿ªÊ¼Î»ÖÃ
		}
	}	
		
  //½øĞĞAPPÖ¸ÁîÏìÓ¦Ó¦´ğ
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

