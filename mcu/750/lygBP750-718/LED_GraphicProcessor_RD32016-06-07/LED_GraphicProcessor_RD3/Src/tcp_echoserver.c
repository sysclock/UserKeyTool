/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Christiaan Simons rewrote this file to get a more stable echo example.
 *
 **/

 /* This file was modified by ST */
#include <string.h>
#include "Timer.h"
#include "lwip.h"

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "../User Command/UserCommandPC_API.h"

#if LWIP_TCP

#define TCP_SERVER_PORT		8978
#define TCP_IP_SERVER_TASK_DEBUG	0
// #define MAX_TCP_RX_DATA_SIZE 512
// #define MAX_TCP_TX_DATA_SIZE 384
#define TCP_TxBufLen		2048
#define SQUEUE_COMMAD_BUF_LENGTH	128
#define MAX_RESEND_PACKET_TIMES		2
/* structure for maintaing connection infos to be passed as argument 
   to LwIP callbacks*/
struct tcp_echoserver_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};

typedef struct
{
	u8_t Data[TCP_TxBufLen];
	volatile int Front, Rear;
	struct tcp_echoserver_struct *es;
	int	tcp_echoserver_ErrorSendTimesCount;
	int	gB_SqueueCommadIndex;
	unsigned char	gB_SqueueCommadBuf[SQUEUE_COMMAD_BUF_LENGTH];
}TCP_DataSqueue;

unsigned int gW_TCPServerPort = TCP_SERVER_PORT;
static struct tcp_pcb *tcp_echoserver_pcb = NULL;
struct tcp_echoserver_struct *tcp_echoserver_es = NULL;
struct pbuf tcp_echoserver_pbuf;
/* ECHO protocol states */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

unsigned char tcp_echoserver_init(void);
u8_t TCP_SendData(u8_t *Bp_Buf, int W_Len);
void TCP_SendDataTask(void);

static TCP_DataSqueue	*gStp_TCPSqu_TCPRevBuf = NULL;
static TCP_DataSqueue	*gStp_TCPSqu_TCPSendBuf = NULL;
static TCP_DataSqueue	gTCPSqu_TCPRevBuf[MEMP_NUM_TCP_PCB];
static TCP_DataSqueue	gTCPSqu_TCPSendBuf[MEMP_NUM_TCP_PCB];

static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

unsigned char ParseTCPCommadBySelfType(TCP_DataSqueue* Queue, unsigned char B_Data);
static TCP_DataSqueue* NewTCPSqu_TCPRevBuf(void);
static TCP_DataSqueue* FindTCPSqu_TCPRevBuf(struct tcp_echoserver_struct *es);
static int FreeTCPSqu_TCPRevBuf(struct tcp_echoserver_struct *es);
static TCP_DataSqueue* NewTCPSqu_TCPSendBuf(void);
static TCP_DataSqueue* FindTCPSqu_TCPSendBuf(struct tcp_echoserver_struct *es);
static int FreeTCPSqu_TCPSendBuf(struct tcp_echoserver_struct *es);
static void ClearQueue(TCP_DataSqueue* Queue);
static u8_t IsEmptyQueue(TCP_DataSqueue* Queue);
static u8_t EnterQueue(TCP_DataSqueue* Queue, u8_t B_Data);
static u8_t GetQueue(TCP_DataSqueue* Queue);
static u8_t LengthQueue(TCP_DataSqueue* Queue);
/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
unsigned char tcp_echoserver_init(void)
{
	u8_t B_Ret = 0, i = 0;
  /* create new tcp pcb */
  tcp_echoserver_pcb = tcp_new();

  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
    
    /* bind echo_pcb to port TCP_SERVER_PORT (ECHO protocol) */
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, gW_TCPServerPort);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
			B_Ret = 1;
    }
    else 
    {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
			B_Ret = 0;
    }
  }
	tcp_echoserver_es = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		ClearQueue(&gTCPSqu_TCPRevBuf[i]);
		ClearQueue(&gTCPSqu_TCPSendBuf[i]);
	}
	if(B_Ret == 1)
		printf("TCP SERVER INIT OK! TCP_SERVER_PORT: %d \r\n", gW_TCPServerPort);
	else
		printf("TCP SERVER INIT Error!\r\n");
	return B_Ret;
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPDataBuf = NULL;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  // tcp_setprio(newpcb, TCP_PRIO_MIN);
	tcp_setprio(newpcb, TCP_PRIO_NORMAL);

  /* allocate structure es to maintain tcp connection informations */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);
    
    ret_err = ERR_OK;
		tcp_echoserver_es = es;
		
		gStp_TempTCPSqu_TCPDataBuf = NewTCPSqu_TCPRevBuf();
		if(gStp_TempTCPSqu_TCPDataBuf == NULL)
			return ret_err;
		ClearQueue(gStp_TempTCPSqu_TCPDataBuf);
		gStp_TempTCPSqu_TCPDataBuf->es = es;
		gStp_TempTCPSqu_TCPDataBuf = NewTCPSqu_TCPSendBuf();
		if(gStp_TempTCPSqu_TCPDataBuf == NULL)
			return ret_err;
		ClearQueue(gStp_TempTCPSqu_TCPDataBuf);
		gStp_TempTCPSqu_TCPDataBuf->es = es;
  }
  else
  {
    /*  close tcp connection */
    tcp_echoserver_connection_close(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
		tcp_echoserver_es = NULL;
  }
  return ret_err;  
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_echoserver_struct *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet */
      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_echoserver_sent);
      
      /* send remaining data*/
      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_echoserver_sent);
    
    /* send back the received data (echo) */
    tcp_echoserver_send(tpcb, es);
    
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
      es->p = p;
  
      /* send back received data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  
  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

u8_t TCP_SendData(u8_t *Bp_Buf, int W_Len)
{
	int i = 0;
	u8_t *Bp_TempData = NULL;
	if(W_Len <= 0)
		return 0;
	Bp_TempData = Bp_Buf;
	if(gStp_TCPSqu_TCPSendBuf != NULL)
	{
		for(i = 0; i < W_Len; i++, Bp_TempData++)
			if(EnterQueue(gStp_TCPSqu_TCPSendBuf, *Bp_TempData) == 0)
			{
				printf("TCPSendBuf Busy!\r\n");
				gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = MAX_RESEND_PACKET_TIMES;
				return 0;
			}
	}
	return 1;
}

void TCP_SendDataTask(void)
{
	return;
}
/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @param  arg: pointer on argument parameter 
  * @param  err: not used
  * @retval None
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  None
  * @retval None
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}

/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
	int i = 0;
	u8_t *Bp_DataHead = NULL, B_TempData = 0, B_TCPSelfCommandNum = 0, ret = 0;
 	u16_t plen = 0;

  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;

		#if 1
		gStp_TCPSqu_TCPRevBuf = FindTCPSqu_TCPRevBuf(es);
		gStp_TCPSqu_TCPSendBuf = FindTCPSqu_TCPSendBuf(es);			// gStp_TCPSqu_TCPRevBuf and gStp_TCPSqu_TCPSendBuf Continuous Init
		if((gStp_TCPSqu_TCPRevBuf == NULL) || (gStp_TCPSqu_TCPSendBuf == NULL))
		{
			plen = ptr->len;
		 
			/* continue with next pbuf in chain (if any) */
			es->p = ptr->next;
			
			if(es->p != NULL)
			{
				/* increment reference count for es->p */
				pbuf_ref(es->p);
			}
			
			/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
			pbuf_free(ptr);

			/* Update tcp window size to be advertized : should be called when received
			data (with the amount plen) has been processed by the application layer */
			tcp_recved(tpcb, plen);
		}
		
		if(gStp_TCPSqu_TCPRevBuf->es == es)
		{
			while(IsEmptyQueue(gStp_TCPSqu_TCPRevBuf) == 0)
			{
				B_TempData = GetQueue(gStp_TCPSqu_TCPRevBuf);
				ret = ParseTCPCommadBySelfType(gStp_TCPSqu_TCPRevBuf, B_TempData);
				if(ret == 1)
					B_TCPSelfCommandNum++;
				if(B_TCPSelfCommandNum > 6)
					break;
			}
			for(i = 0, Bp_DataHead = ptr->payload; i < ptr->len; i++, Bp_DataHead++)
			{
				B_TempData = *Bp_DataHead;
				ret = EnterQueue(gStp_TCPSqu_TCPRevBuf, B_TempData);
				if(ret == 0)
				{
					printf("Tcp_server Rev Error1\r\n");
					break;
				}
			}
			while(IsEmptyQueue(gStp_TCPSqu_TCPRevBuf) == 0)
			{
				B_TempData = GetQueue(gStp_TCPSqu_TCPRevBuf);
				ret = ParseTCPCommadBySelfType(gStp_TCPSqu_TCPRevBuf, B_TempData);
				if(ret == 1)
					B_TCPSelfCommandNum++;
//				if(B_TCPSelfCommandNum > 6)
//					break;
			}
		}
		#endif
		#if 1
		if(gStp_TCPSqu_TCPSendBuf->es == es)
		{
			gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex = 0;
			memset(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, 0, sizeof(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf));
			while(IsEmptyQueue(gStp_TCPSqu_TCPSendBuf) == 0)
			{
				B_TempData = GetQueue(gStp_TCPSqu_TCPSendBuf);
				if((gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex == sizeof(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf))
						&& (gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex <= tcp_sndbuf(tpcb))
					)
				{
					if(gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount < MAX_RESEND_PACKET_TIMES)
						wr_err = tcp_write(tpcb, gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex, 1);
					else
						wr_err = ERR_MEM;
					if(wr_err == ERR_MEM)
					{
						printf("Tcp_server Send Error1\r\n");
						es->p = ptr;
						gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex = 0;
						memset(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, 0, sizeof(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf));
						gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount++;
						if(gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount >= MAX_RESEND_PACKET_TIMES)
						{
							plen = ptr->len;
						 
							/* continue with next pbuf in chain (if any) */
							es->p = ptr->next;
							
							if(es->p != NULL)
							{
								/* increment reference count for es->p */
								pbuf_ref(es->p);
							}
							
							/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
							pbuf_free(ptr);

							/* Update tcp window size to be advertized : should be called when received
							data (with the amount plen) has been processed by the application layer */
							tcp_recved(tpcb, plen);
							gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = 0;
						}
						return;
					}
					gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex = 0;
					memset(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, 0, sizeof(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf));
				}
				gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf[gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex] = B_TempData;
				gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex++;
			}
		}
		#endif

    /* enqueue data for transmission */

		#if 0
			i = gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex;
			if((i > 0) && (i <= tcp_sndbuf(tpcb)))
				wr_err = tcp_write(tpcb, gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, i, 1);
			else
				wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
			if (wr_err == ERR_OK)
			{
				plen = ptr->len;
			 
				/* continue with next pbuf in chain (if any) */
				es->p = ptr->next;
				
				if(es->p != NULL)
				{
					/* increment reference count for es->p */
					pbuf_ref(es->p);
				}
				
				/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
				pbuf_free(ptr);

				/* Update tcp window size to be advertized : should be called when received
				data (with the amount plen) has been processed by the application layer */
				tcp_recved(tpcb, plen);
				gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = 0;
			}
			else if(wr_err == ERR_MEM)
			{
				/* we are low on memory, try later / harder, defer to poll */
				es->p = ptr;
				printf("Tcp_server Send  Error2\r\n");
			}
			else
			{
			 /* other problem ?? */
			}
			gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex = 0;
		#endif
		#if 1
			i = gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex;
			if((i > 0) && (i <= tcp_sndbuf(tpcb)))
			{
				if(gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount < MAX_RESEND_PACKET_TIMES)
					wr_err = tcp_write(tpcb, gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, i, 1);
				else
					wr_err = ERR_MEM;
				if (wr_err == ERR_OK)
				{
					plen = ptr->len;
				 
					/* continue with next pbuf in chain (if any) */
					es->p = ptr->next;
					
					if(es->p != NULL)
					{
						/* increment reference count for es->p */
						pbuf_ref(es->p);
					}
					
					/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
					pbuf_free(ptr);

					/* Update tcp window size to be advertized : should be called when received
					data (with the amount plen) has been processed by the application layer */
					tcp_recved(tpcb, plen);
					gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = 0;
				}
				else if(wr_err == ERR_MEM)
				{
					/* we are low on memory, try later / harder, defer to poll */
					es->p = ptr;
					gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount++;
					if(gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount >= MAX_RESEND_PACKET_TIMES)
					{
						plen = ptr->len;
					 
						/* continue with next pbuf in chain (if any) */
						es->p = ptr->next;
						
						if(es->p != NULL)
						{
							/* increment reference count for es->p */
							pbuf_ref(es->p);
						}
						
						/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
						pbuf_free(ptr);

						/* Update tcp window size to be advertized : should be called when received
						data (with the amount plen) has been processed by the application layer */
						tcp_recved(tpcb, plen);
						gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = 0;
					}
					printf("Tcp_server Send  Error2\r\n");
				}
				else
				{
				 /* other problem ?? */
				}
				gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadIndex = 0;
				memset(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf, 0, sizeof(gStp_TCPSqu_TCPSendBuf->gB_SqueueCommadBuf));
			}
			if(i == 0)
			{
				plen = ptr->len;
			 
				/* continue with next pbuf in chain (if any) */
				es->p = ptr->next;
				
				if(es->p != NULL)
				{
					/* increment reference count for es->p */
					pbuf_ref(es->p);
				}
				
				/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
				pbuf_free(ptr);

				/* Update tcp window size to be advertized : should be called when received
				data (with the amount plen) has been processed by the application layer */
				tcp_recved(tpcb, plen);
				gStp_TCPSqu_TCPSendBuf->tcp_echoserver_ErrorSendTimesCount = 0;
			}
		#endif
  }
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
		FreeTCPSqu_TCPRevBuf(es);
		FreeTCPSqu_TCPSendBuf(es);

    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);
}

unsigned char ParseTCPCommadBySelfType(TCP_DataSqueue* Queue, unsigned char B_Data)
{
	unsigned char B_Temp = NULL;
	B_Temp = B_Data;
	if(Queue->gB_SqueueCommadIndex >= sizeof(Queue->gB_SqueueCommadBuf))
	{
		memset(Queue->gB_SqueueCommadBuf, 0, sizeof(Queue->gB_SqueueCommadBuf));
		Queue->gB_SqueueCommadIndex = 0;
		#if(TCP_COMMAD_DEBUG == 1)
		printf("TCP Command Length too long!\r\n");
		#endif
	}
	Queue->gB_SqueueCommadBuf[Queue->gB_SqueueCommadIndex] = B_Temp;
	Queue->gB_SqueueCommadIndex++;
	if(B_Temp == '\n')
	{
		#if(TCP_COMMAD_DEBUG == 2)
		printf("%s", Queue->gB_SqueueCommadBuf);
		#endif
		ParseTCPCommad(Queue->gB_SqueueCommadBuf);
		memset(Queue->gB_SqueueCommadBuf, 0, sizeof(Queue->gB_SqueueCommadBuf));
		Queue->gB_SqueueCommadIndex = 0;
		return 1;
	}
	return 0;
}

static TCP_DataSqueue* NewTCPSqu_TCPRevBuf(void)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPRevBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPRevBuf = &gTCPSqu_TCPRevBuf[i];
		if(gStp_TempTCPSqu_TCPRevBuf->es == NULL)
			return gStp_TempTCPSqu_TCPRevBuf;
	}
	return &gTCPSqu_TCPRevBuf[MEMP_NUM_TCP_PCB - 1];
}

static TCP_DataSqueue* NewTCPSqu_TCPSendBuf(void)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPSendBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPSendBuf = &gTCPSqu_TCPSendBuf[i];
		if(gStp_TempTCPSqu_TCPSendBuf->es == NULL)
			return gStp_TempTCPSqu_TCPSendBuf;
	}
	return &gTCPSqu_TCPSendBuf[MEMP_NUM_TCP_PCB - 1];
}

static TCP_DataSqueue* FindTCPSqu_TCPRevBuf(struct tcp_echoserver_struct *es)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPRevBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPRevBuf = &gTCPSqu_TCPRevBuf[i];
		if(gStp_TempTCPSqu_TCPRevBuf->es == es)
			return gStp_TempTCPSqu_TCPRevBuf;
	}
	return NULL;
}

static TCP_DataSqueue* FindTCPSqu_TCPSendBuf(struct tcp_echoserver_struct *es)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPSendBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPSendBuf = &gTCPSqu_TCPSendBuf[i];
		if(gStp_TempTCPSqu_TCPSendBuf->es == es)
			return gStp_TempTCPSqu_TCPSendBuf;
	}
	return NULL;
}

static int FreeTCPSqu_TCPRevBuf(struct tcp_echoserver_struct *es)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPRevBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPRevBuf = &gTCPSqu_TCPRevBuf[i];
		if(gStp_TempTCPSqu_TCPRevBuf->es == es)
		{
			gStp_TempTCPSqu_TCPRevBuf->es = NULL;
			ClearQueue(gStp_TempTCPSqu_TCPRevBuf);
			return 1;
		}
	}
	return 0;
}

static int FreeTCPSqu_TCPSendBuf(struct tcp_echoserver_struct *es)
{
	int i = 0;
	TCP_DataSqueue	*gStp_TempTCPSqu_TCPSendBuf = NULL;
	for(i = 0; i < MEMP_NUM_TCP_PCB; i++)
	{
		gStp_TempTCPSqu_TCPSendBuf = &gTCPSqu_TCPSendBuf[i];
		if(gStp_TempTCPSqu_TCPSendBuf->es == es)
		{
			gStp_TempTCPSqu_TCPSendBuf->es = NULL;
			ClearQueue(gStp_TempTCPSqu_TCPSendBuf);
			return 1;
		}
	}
	return 0;
}

static void ClearQueue(TCP_DataSqueue* Queue)
{
	Queue->Front = 0;
	Queue->Rear = 0;
	Queue->es = NULL;
	Queue->tcp_echoserver_ErrorSendTimesCount = 0;
	Queue->gB_SqueueCommadIndex = 0;
	memset(Queue->gB_SqueueCommadBuf, 0, sizeof(Queue->gB_SqueueCommadBuf));
}

static u8_t IsEmptyQueue(TCP_DataSqueue* Queue)
{
	if (Queue->Front == Queue->Rear)
		return 1;
	else
		return 0;
}

static u8_t EnterQueue(TCP_DataSqueue* Queue, u8_t B_Data)
{
	if(Queue->Front == ((Queue->Rear + 1) % TCP_TxBufLen))
		return 0;
	else
	{
		Queue->Rear = (Queue->Rear + 1) % TCP_TxBufLen;
		Queue->Data[Queue->Rear] = B_Data;
		return 1;
	}
}

static u8_t GetQueue(TCP_DataSqueue* Queue)
{
	if (Queue->Front == Queue->Rear)
		return NULL;
	else
	{
		Queue->Front = (Queue->Front + 1) % TCP_TxBufLen;
		return Queue->Data[Queue->Front];
	}
}

static u8_t LengthQueue(TCP_DataSqueue* Queue)
{
	int i = (Queue->Rear - Queue->Front + TCP_TxBufLen) % TCP_TxBufLen;
	return i;
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    TCP_DummyCall(void)                                    */
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void TCP_DummyCall(void)
{
	tcp_echoserver_send(NULL, NULL);
	ClearQueue(NULL);
	IsEmptyQueue(NULL);
	EnterQueue(NULL, 0);
	GetQueue(NULL);
	LengthQueue(NULL);
}

#endif /* LWIP_TCP */
