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

#define QueueSize      101

typedef struct
{
    unsigned char  front;
    unsigned char  rear;
    unsigned char dat[QueueSize];
}_FifoQueue;

_QUEUEDEC_  _FifoQueue g_strUartRxQueue;

//#ifdef TCP_IP
_QUEUEDEC_  _FifoQueue g_strUart3RxQueue;
void Uart3RxQueue_Init(void);
//#endif

void UartRxQueue_Init(void);
unsigned char QueueIn(_FifoQueue *Queue,unsigned char sdat);
unsigned char QueueOut(_FifoQueue *Queue,unsigned char *sdat);

#endif

