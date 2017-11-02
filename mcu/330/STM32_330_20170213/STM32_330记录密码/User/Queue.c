#define _QUEUE_C_

#include "Queue.h"

void QueueInit(_FifoQueue *Queue)
{
	if(Queue->front!=0)
    		Queue->front = 0;
	if(Queue->rear!=0)
    		Queue->rear=0;
}

void UartRxQueue_Init(void)
{
	QueueInit(&g_strUartRxQueue);
}

//#ifdef TCP_IP
void Uart3RxQueue_Init(void)
{
	QueueInit(&g_strUart3RxQueue);
}
//#endif

// Queue In
unsigned char QueueIn(_FifoQueue *Queue,unsigned char sdat)
{
    if((Queue->rear+1)%QueueSize == Queue->front)  //少用一个元素空间
    { 
        return QueueFull;
    }
    else
    {   
        Queue->dat[Queue->rear] = sdat;
        Queue->rear = (Queue->rear + 1)%QueueSize;

        return QueueOperateOk;
    }
}

// Queue Out
unsigned char QueueOut(_FifoQueue *Queue,unsigned char *sdat)
{
    if(Queue->front ==Queue->rear)
    {                    // empty
        return QueueEmpty;
    }
    else
    {                    // out
        *sdat = Queue->dat[Queue->front];
        Queue->front = (Queue->front + 1)%QueueSize;
	
        return QueueOperateOk;
    }
}


