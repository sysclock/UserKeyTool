#ifndef __QUEUE_H
#define __QUEUE_H

#include "stdint.h"

#define QueueFull      0
#define QueueEmpty     1
#define QueueOperateOk 2

#define DataQueueSize     6001

#define CommQueueSize 256

typedef struct
{
    unsigned int  front;
    unsigned int  rear;
    unsigned char dat[DataQueueSize];
}_FifoQueue_Data;

typedef struct
{
    unsigned int  front;
    unsigned int  rear;
    unsigned char dat[CommQueueSize];
}_FifoQueue_Comm;


extern  _FifoQueue_Data g_DataRxQueue;

extern  _FifoQueue_Comm g_CommRxQueue;
extern  _FifoQueue_Comm g_USBRxQueue;
extern  _FifoQueue_Comm g_BLERxQueue;

void PW1101_Data_RxQueue_Init(void);
unsigned char PW1101_Data_QueueIn(_FifoQueue_Data *Queue,unsigned char sdat);
unsigned char PW1101_Data_QueueOut(_FifoQueue_Data *Queue,unsigned char *sdat);


void PW1101_Comm_RxQueue_Init(void);
unsigned char PW1101_Comm_QueueIn(_FifoQueue_Comm *Queue,unsigned char sdat);
unsigned char PW1101_Comm_QueueOut(_FifoQueue_Comm *Queue,unsigned char *sdat);


extern uint8_t comm_cache[CommQueueSize];

uint8_t read_comm_from_queue(void);


void APP_Comm_RxQueue_Init(void);
unsigned char APP_Comm_QueueIn(_FifoQueue_Comm *Queue,unsigned char sdat);
unsigned char APP_Comm_QueueOut(_FifoQueue_Comm *Queue,unsigned char *sdat);
extern uint8_t USB_comm_cache[CommQueueSize];
extern uint8_t BLE_comm_cache[CommQueueSize];

uint8_t read_comm_from_USBqueue(void);
uint8_t read_comm_from_BLEqueue(void);
#endif

