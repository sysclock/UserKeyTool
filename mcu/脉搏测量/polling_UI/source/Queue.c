#define _QUEUE_C_

#include "Queue.h"


_FifoQueue_Data g_DataRxQueue;
_FifoQueue_Comm g_CommRxQueue;

_FifoQueue_Comm g_USBRxQueue;
_FifoQueue_Comm g_BLERxQueue;


//队列初始化
void DataQueueInit(_FifoQueue_Data *Queue)
{
	if(Queue->front!=0)
    		Queue->front = 0;
	if(Queue->rear!=0)
    		Queue->rear=0;
}


//队列初始化--脉感芯片
void PW1101_Data_RxQueue_Init(void)
{
	DataQueueInit(&g_DataRxQueue);
}

// Queue In
unsigned char PW1101_Data_QueueIn(_FifoQueue_Data *Queue,unsigned char sdat)
{
    if((Queue->rear+1)%DataQueueSize == Queue->front)  //少用一个元素空间
    { 
        return QueueFull;
    }
    else
    {   
        Queue->dat[Queue->rear] = sdat;
        Queue->rear = (Queue->rear + 1)%DataQueueSize;
        return QueueOperateOk;
    }
}

// Queue Out
unsigned char PW1101_Data_QueueOut(_FifoQueue_Data *Queue,unsigned char *sdat)
{
    if(Queue->front ==Queue->rear)
    {                    // empty
        return QueueEmpty;
    }
    else
    {                    // out
        *sdat = Queue->dat[Queue->front];
        Queue->front = (Queue->front + 1)%DataQueueSize;
	
        return QueueOperateOk;
    }
}



void CommQueueInit(_FifoQueue_Comm *Queue)
{
	if(Queue->front!=0)
    		Queue->front = 0;
	if(Queue->rear!=0)
    		Queue->rear=0;
}
void PW1101_Comm_RxQueue_Init(void)
{
	CommQueueInit(&g_CommRxQueue);
}

unsigned char PW1101_Comm_QueueIn(_FifoQueue_Comm *Queue,unsigned char sdat)
{
    if((Queue->rear+1)%CommQueueSize == Queue->front)  //少用一个元素空间
    { 
        return QueueFull;
    }
    else
    {   
        Queue->dat[Queue->rear] = sdat;
        Queue->rear = (Queue->rear + 1)%CommQueueSize;
        return QueueOperateOk;
    }
}

// Queue Out
unsigned char PW1101_Comm_QueueOut(_FifoQueue_Comm *Queue,unsigned char *sdat)
{
    if(Queue->front ==Queue->rear)
    {                    // empty
        return QueueEmpty;
    }
    else
    {                    // out
        *sdat = Queue->dat[Queue->front];
        Queue->front = (Queue->front + 1)%CommQueueSize;
	
        return QueueOperateOk;
    }
}


void APP_Comm_RxQueue_Init(void)
{
	CommQueueInit(&g_USBRxQueue); //初始化USB接收队列
}

unsigned char APP_Comm_QueueIn(_FifoQueue_Comm *Queue,unsigned char sdat)
{
    if((Queue->rear+1)%CommQueueSize == Queue->front)  //少用一个元素空间
    { 
        return QueueFull;
    }
    else
    {   
        Queue->dat[Queue->rear] = sdat;
        Queue->rear = (Queue->rear + 1)%CommQueueSize;
        return QueueOperateOk;
    }
}

// Queue Out
unsigned char APP_Comm_QueueOut(_FifoQueue_Comm *Queue,unsigned char *sdat)
{
    if(Queue->front ==Queue->rear)
    {                    // empty
        return QueueEmpty;
    }
    else
    {                    // out
        *sdat = Queue->dat[Queue->front];
        Queue->front = (Queue->front + 1)%CommQueueSize;
	
        return QueueOperateOk;
    }
}

uint8_t comm_cache[CommQueueSize];


//从PW1101指令队列中读取指令
uint8_t read_comm_from_queue(void)
{
	uint8_t cache_index=0;
	while(PW1101_Comm_QueueOut(&g_CommRxQueue,&comm_cache[cache_index])!= QueueEmpty)
	{
		if(cache_index==0 )
		{
			if(comm_cache[cache_index]==0xA5)
			{
				cache_index++;
			}
			else
			{
				cache_index=0;
			}
		}
		else
		{
			cache_index++;
			if(cache_index>8)
			{
				if(cache_index==(10+comm_cache[8]))
				{
						return 1; //已经收到值
				}
			}
		}
	}

	return 0;
}



uint8_t USB_comm_cache[CommQueueSize];

//从USB队列中读取数据
uint8_t read_comm_from_USBqueue(void)
{
	uint8_t cache_index=0;
	while(APP_Comm_QueueOut(&g_USBRxQueue,&USB_comm_cache[cache_index])!= QueueEmpty)
	{
		if(cache_index==0 )
		{
			if(USB_comm_cache[cache_index]==0xA5)
			{
				cache_index++;
			}
			else
			{
				cache_index=0;
			}
		}
		else
		{
			cache_index++;
			if(cache_index>8)
			{
				if(cache_index==(10+USB_comm_cache[8]))
				{
						return 1; //已经收到值
				}
			}
		}
	}

	return 0;
}

uint8_t BLE_comm_cache[CommQueueSize];

//从蓝牙队列中读取数据
uint8_t read_comm_from_BLEqueue(void)
{
	uint8_t cache_index=0;
	while(APP_Comm_QueueOut(&g_BLERxQueue,&BLE_comm_cache[cache_index])!= QueueEmpty)
	{
		if(cache_index==0 )
		{
			if(BLE_comm_cache[cache_index]==0xA5)
			{
				cache_index++;
			}
			else
			{
				cache_index=0;
			}
		}
		else
		{
			cache_index++;
			if(cache_index>8)
			{
				if(cache_index==(10+BLE_comm_cache[8]))
				{
						return 1; //已经收到值
				}
			}
		}
	}

	return 0;
}

