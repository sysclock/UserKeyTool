#ifndef _PW1101_INTERACT_H_
#define _PW1101_INTERACT_H_

#include "stdint.h"

#define PW1101_TRUE  0x33
#define PW1101_FALSE 0x34

#define DATA_LEN_INDEX 8
#define COMMAND_ID_INDEX 1
#define DEVICE_ID_INDEX 2
#define COUNT_ID_INDEX_H 3
#define COUNT_ID_INDEX_L 4
#define STATUS_ID_INDEX_H 5
#define STATUS_ID_INDEX_L 6
#define STATUS_ID_INDEX_TorF 7

#define DATA_INDEX_1 9
#define DATA_INDEX_2 10
#define DATA_INDEX_3 11
#define DATA_INDEX_4 12
#define DATA_INDEX_5 13
#define DATA_INDEX_6 14

#define COMMAND_ID_STATUS_CHENK  0x90   //״̬���
#define COMMAND_ID_START_MEASURE 0x91   //��ʼ����
#define COMMAND_ID_STOP_MEASURE  0x92   //ֹͣ����
#define COMMAND_ID_MEASURE_MODE  0x93   //����ģʽ
#define COMMAND_ID_MEASURE_PARA  0x94   //��������
#define COMMAND_ID_USER_PARA     0x95   //�û�����
#define COMMAND_ID_CORRECTED_PARA 0x96  //У������
#define COMMAND_ID_SEND_bmp       0x97  //��������
#define COMMAND_ID_SEND_MEASURE_RESULT 0x98  //����������
#define COMMAND_ID_SEND_MEASURE_DATA 0x99  //�����������
#define COMMAND_ID_SEND_MEASURE_STATUS 0x9A //�������״̬
#define COMMAND_ID_READ_MEASURE_PARA  0x9B  //��ȡ��������
#define COMMAND_ID_READ_USER 0x9C //��ȡ�û�����
#define COMMAND_ID_READ_CORRECTED_PARA  0x9D //��ȡУ׼����
#define COMMAND_ID_READ_IC_INFOMATION  0x9E //��ȡIC��Ϣ
#define COMMAND_ID_SEND_WAVEFORMS_INFO 0x9F //���䲨������ָ��

#define DEVICE_ID_ELE_BPG   0x21 //����Ѫѹ��


#define AUTO_MEASURE_GOING 5
#define MEASURE_SUCCESS  1
#define WEAR_NOT_GOOD  2
#define MEASURE_FAIL   0
#define INTERACT_FAIL  3
#define LOW_POWER   4
#define TEMP_LOW  6
#define TEMP_HIGH  7

#define TEMP_LOW_VALUE 100
#define TEMP_HIGE_VALUE 4000



//����ģʽ�궨��
//1 ��ʾ���ɼ�����
//2 ��ʾ�����Ĳ���ģʽ��ֻ�õ��������
//3 ��ʾ���������ݵĲ���ģʽ
//4 ��ʾ���������ݵĲ���ģʽ
#define MEASURE_MODE1  1
#define MEASURE_MODE2  2
#define MEASURE_MODE3  3
#define MEASURE_MODE4  4


extern uint32_t PW1101_ID_H;
extern uint32_t PW1101_ID_MH;
extern uint32_t PW1101_ID_ML;
extern uint32_t PW1101_ID_L;
extern uint8_t  PW1101_ID_data;   //16�ֽڵ����ֵ

extern uint16_t Software_version;
extern uint16_t Algorithm_version;

uint8_t read_PW1101Information (void);

void pw1101_func_status_check(void);
extern uint8_t measure_result_data[4];
uint8_t Check_CRC8(uint8_t *data);

uint8_t pw1101_func_manual_measure_mode_select(uint32_t para);
uint8_t pw1101_func_manual_measure_nodata(uint32_t);
uint8_t pw1101_func_manual_measure(uint32_t);
uint8_t pw1101_func_auto_measure(void);

#endif
