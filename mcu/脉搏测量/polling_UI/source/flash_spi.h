#ifndef _FLASH_SPI_H_
#define _FLASH_SPI_H_

#include "stdint.h"

//Bank Address Register  控制大于16M时,大地址的操作。bit7置1，可以四字节为地址操作，默认为0，三个字节
//bit1，bit0可以指定内部高地址2位的值，对就地址为BA25、BA24，以此实现26位地址操作，实现了64MB的控制
#define S25FL512S_BRRD  0x16
#define S25FL512S_BRWR  0x17


//使能读写指令  Enable  Disable
#define S25FL512S_WREN   0x06
#define S25FL512S_WRDI   0x04

//写入页指令
#define S25FL512S_PP  0x02
//读指令操作
#define S25FL512S_RD  0x03

//擦除一个扇区  Sector Erase 
#define S25FL512S_SE  0xD8

#define S25FL512S_RDSR1 0x05
#define S25FL512S_WRSR1 0x01

#define S25FL512S_CS_H	GPIO_SetPinsOutput(GPIOA, 1u << 14) //FLASH_CS置高
#define S25FL512S_CS_L	GPIO_ClearPinsOutput(GPIOA, 1u << 14) //FLASH_CS置高

#define RECORD_NUM 600  //历史记录条数
extern uint8_t  READ_Record_data_cache[RECORD_NUM*16];
extern uint32_t  record_begin_index;
extern uint32_t  record_total_num;
extern uint32_t  display_record_index;



#define SAVE_MEASURE_RESULT_SECTOR  1  //进行测量结果数据存储的块区
#define SECTOR_SIZE (1024*256)

#define save_state_new 0xfc
#define save_state_read 0xf0

void S25FL512S_Spi_Config(void);
uint8_t S25FL512S_ReadSR1(void);
void S25FL512S_WriteSR1(uint8_t sr);
uint8_t S25FL512S_ReadBR(void);
void S25FL512S_WriteBR(uint8_t br);
void S25FL512S_Write_Enable(void);
void S25FL512S_Write_Disable(void);
void S25FL512S_Write_Page(uint8_t * pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void S25FL512S_Write_NoCheck(uint8_t * pBuffer,uint32_t WriteAddr,uint32_t NumByteToWrite);
void S25FL512S_Erase_Sector(uint32_t Dst_Addr);

uint8_t Save_Measure_Result_To_Flash(uint32_t,uint8_t *);

extern uint32_t Save_Measure_Index_Ptr;

uint8_t Read_Record_Data_From_Flash(uint32_t para);

void Find_Save_Index_Ptr(void);
void S25FL512S_Write(uint8_t * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void S25FL512S_Read(uint8_t * pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead);


#define FLASH_TOTAL_ADD  (256*256*1024)    //64M
#define Measure_Data_Begin  (16*256*1024)      //跳过4M的空间开始进行存储

//一个完整的测量周期需要的最大空间  
//200表示的是100ms内进行了50次4字节的采集
//30表示的是采集间隔为3S，，共有30个100ms
//11表示的是最大的采集组数
//加20，此20个字节用来存储一次完整数据的头标识，进行数据查找用
#define Measure_Data_Maximum  (200*30*11+20)   
extern uint32_t measure_data_save_ptr;   
void Erase_Next_Sector(void);


void Updata_measure_data_add_ptr(void);
void Find_measure_data_add_prt(void);

void Data_record(void);

void Upload_data(uint8_t port);
void Clear_data(uint8_t port);

typedef enum {
	
	SET_PARA_SEX_ADD =0,
	SET_PARA_AGE_ADD=SET_PARA_SEX_ADD+1,
	SET_PARA_HEIGHT_ADD=SET_PARA_AGE_ADD+1,
	SET_PARA_WEIGHT_ADD=SET_PARA_HEIGHT_ADD+1,
	
	SET_PARA_HB_L_ADD =SET_PARA_WEIGHT_ADD+1,
	SET_PARA_HB_H_ADD =SET_PARA_HB_L_ADD+2,
	SET_PARA_LB_L_ADD =SET_PARA_HB_H_ADD+2,
	SET_PARA_LB_H_ADD =SET_PARA_LB_L_ADD+1,
	SET_PARA_HR_H_ADD =SET_PARA_LB_H_ADD+1,
	SET_PARA_HR_L_ADD =SET_PARA_HR_H_ADD+1,
	
	SET_PARA_MEASURE_GROUPNUM_ADD =SET_PARA_HR_L_ADD+1,
	SET_PARA_MEASURE_TIMELONG_ADD =SET_PARA_MEASURE_GROUPNUM_ADD+1,
	SET_PARA_MEASURE_TIMEINTERVAL_ADD =SET_PARA_MEASURE_TIMELONG_ADD+1,
	SET_PARA_MEASURE_ELETRODE_ADD=SET_PARA_MEASURE_TIMEINTERVAL_ADD+1,
	
	SET_PARA_MEASURE_MODE_ADD=SET_PARA_MEASURE_ELETRODE_ADD+1,
	
	SET_PARA_AUTOMEASURE_TIMELONG_ADD=SET_PARA_MEASURE_MODE_ADD+1,
	SET_PARA_AUTOMEASURE_TIMEINTERVAL_ADD=SET_PARA_AUTOMEASURE_TIMELONG_ADD+1,
	
	SET_PARA_STANDBYTIME_ADD=SET_PARA_AUTOMEASURE_TIMEINTERVAL_ADD+1,
	SET_PARA_GMT_NUM_ADD=SET_PARA_STANDBYTIME_ADD+1,
	
	SET_PARA_mmgH_kPa_ADD=SET_PARA_GMT_NUM_ADD+1,
	
	SET_PARA_BLE_OnOff_ADD=SET_PARA_mmgH_kPa_ADD+1,
	SET_PARA_NoDisturb_Mode_ADD=SET_PARA_BLE_OnOff_ADD+1,
	
	SET_PARA_TOTAL_NUM=SET_PARA_NoDisturb_Mode_ADD+1,
	
}_FLASH_ADD_TYPE;


extern uint8_t user_set_para[SET_PARA_TOTAL_NUM];
void Save_para_as_define(void);
void Sava_para_to_flash(void);
void Read_para_from_flash(void);
#endif

