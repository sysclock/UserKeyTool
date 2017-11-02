#ifndef __QUEUE_H
#define __QUEUE_H
#include "common.h"

#ifdef _QUEUE_C_
    #define _QUEUEDEC_
#else
    #define _QUEUEDEC_ extern
#endif


#define QueueFull      0
#define QueueEmpty     1
#define QueueOperateOk 2

#define QueueSize      2048

typedef struct
{
    unsigned int  front;
    unsigned int  rear;
    unsigned char dat[QueueSize];
}_FifoQueue;

_QUEUEDEC_  _FifoQueue g_strUartRxQueue;


_QUEUEDEC_  _FifoQueue g_strUart3RxQueue;
void Uart3RxQueue_Init(void);


void UartRxQueue_Init(void);
unsigned char QueueIn(_FifoQueue *Queue,unsigned char sdat);
unsigned char QueueOut(_FifoQueue *Queue,unsigned char *sdat);

#endif

