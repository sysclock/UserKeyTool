/**
  ******************************************************************************
  * �ļ����� ��USART.c 
  * ��    �� ����ʼ��USART1
  * �� 	  �� ��V0.0
  * �� �� �� ��V3.5.0
  *	�޸ļ�¼ ��		   	

  *******************************************************************************/ 

/*������ͷ�ļ�-----------------------------------------------*/ 
#include "USART.h"
/*��������---------------------------------------------------*/
u8 U2data;

u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_RX_COUNT=0;       //����״̬���

u8 RS485_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 RS485_RX_COUNT=0;       //����״̬���

u8 RS485_RX_FLAG=0;

/**************************************************************
 ** �������� :USART1_Config
 ** ����˵�� :USART1��ʼ����GPIO��NVIC��USART1��
 ** ������� :��
 ** ������� :��
 ** ���ز��� :��
 ** ע    �� :USART1ʱ��--APB2
***************************************************************/
void USART1_Config(u8 baudrate_num)
{	
	/*1�������ṹ�壺GPIO��NVIC��USART*/
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
	/*2����ʱ�ӣ�ʹ��GPIOʱ��(GPIOA)��ʹ��USARTʱ��(USART2)*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_DeInit(USART1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*3������TX��PA2-TX�����������*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*4������RX��A3-RX��������*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	         //��1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 //USART1ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //��Ӧ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 //ʹ��USART2ͨ��
	NVIC_Init(&NVIC_InitStructure);	

	USART_InitStructure.USART_BaudRate =(1<<baudrate_num)*2400;                     //������115200         
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    //�ֳ�9bit:����żУ��ʱ�ֳ�Ϊ9bit,����żУ��ʱ�ֳ�Ϊ8bit										
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	       //ֹͣλ2λ									
	USART_InitStructure.USART_Parity = USART_Parity_No;		       //żУ��							
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ʹ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������							
  USART_Init(USART1, &USART_InitStructure);   //��ʼ������ USART2   
  USART_Cmd(USART1, ENABLE);					//ʹ��USART����
	
	/*7��ʹ��USART2�����ж�*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
}



/**************************************************************
 ** �������� :USART2_Config
 ** ����˵�� :USART2��ʼ����GPIO��NVIC��USART2��
 ** ������� :��
 ** ������� :��
 ** ���ز��� :��
 ** ע    �� :USART2ʱ��--APB1
***************************************************************/
void USART2_Config(void)
{	
	/*1�������ṹ�壺GPIO��NVIC��USART*/
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
		
	/*2����ʱ�ӣ�ʹ��GPIOʱ��(GPIOA)��ʹ��USARTʱ��(USART2)*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);

	/*3������TX��PA2-TX�����������*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*4������RX��A3-RX��������*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*6������NVIC������
	@	��1
	@ 	USART2ȫ���ж�
	@	��ռ���ȼ�0
	@ 	��Ӧ���ȼ�0
	@	ʹ��USART2ͨ��
	@	��ʼ��NVIC
 	*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	         //��1
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		 //USART2ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 //��Ӧ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 //ʹ��USART2ͨ��
	NVIC_Init(&NVIC_InitStructure);	


	/*6������USART������
	@	������-----115200
	@	�����ֳ�---9
	@	ֹͣλ-----2
	@	У��-------żУ��
	@	ģʽ-------�շ�
	@	Ӳ��������-��
	@	��ʼ��USART2
	@	ʹ��USART2
	*/
	USART_InitStructure.USART_BaudRate =9600;                     //������115200         
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    //�ֳ�9bit:����żУ��ʱ�ֳ�Ϊ9bit,����żУ��ʱ�ֳ�Ϊ8bit										
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	       //ֹͣλ2λ									
	USART_InitStructure.USART_Parity = USART_Parity_No;		       //żУ��							
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ʹ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������							
  USART_Init(USART2, &USART_InitStructure);   //��ʼ������ USART2   
  USART_Cmd(USART2, ENABLE);					//ʹ��USART����
	
	/*7��ʹ��USART2�����ж�*/
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
}
/**************************************************************
 ** ������ :fputc
 ** ����   :�ض���c�⺯��printf��USART
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��printf����
***************************************************************/
int fputc(int ch, FILE *f)
{
   /* ��Printf���ݷ������� */
   USART_SendData(USART2, (unsigned char) ch);
   /* �ȴ����ͽ��� */
   while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
   return (ch);
}
/**************************************************************
 ** ������ :fgetc
 ** ����   :�ض���c�⺯��scanf��USART
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��scanf����
***************************************************************/
int fgetc(FILE *f)
{
	/* �ȴ�����2�������� */
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART2);
}
/**************************************************************
 ** ������ :USART2_Putc
 ** ����   :��USART2_Putc�������ݴ�ӡ������
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void USART2_Putc(unsigned char c)
{
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
}
/**************************************************************
 ** ������ :USART1_Puts
 ** ����   :��USART1_Puts�������ݴ�ӡ������
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void USART1_Puts(char * str,u32 num)
{
    u32 i;
	for(i=0;i<num;i++)
    {
        USART_SendData(USART1, *str++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}
/**************************************************************
 ** ������ :USART2_Puts
 ** ����   :��USART2_Puts�������ݴ�ӡ������
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void USART2_Puts(char * str,u32 num)
{
    u32 i;
	for(i=0;i<num;i++)
    {
        USART_SendData(USART2, *str++);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}
/**************************************************************
 ** ������ :USART2_IRQHandler
 ** ����   :�жϺ����������ڽ��յ��ĺ���ֱ�ӴӴ��ڴ�ӡ����
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void USART2_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{	 
 		USART_ClearITPendingBit(USART2,    USART_IT_RXNE);
		res =USART_ReceiveData(USART2); 	//��ȡ���յ�������
		if(USART_RX_COUNT<30)
		{
			USART_RX_BUF[USART_RX_COUNT]=res;		//��¼���յ���ֵ
			USART_RX_COUNT++;						//������������1 
		} 
		else
		  USART_RX_COUNT=0;
	}  
}

/**************************************************************
 ** ������ :USART1_IRQHandler
 ** ����   :�жϺ����������ڽ��յ��ĺ���ֱ�ӴӴ��ڴ�ӡ����
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void USART1_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //���յ�����
	{	 
 		USART_ClearITPendingBit(USART1,    USART_IT_RXNE);
		if(RS485_RX_FLAG==0)
		{
				res =USART_ReceiveData(USART1); 	//if��ȡ���յ�������		
				RS485_RX_BUF[RS485_RX_COUNT]=res;	
				RS485_RX_COUNT++;
				if(RS485_RX_COUNT==1 && res!=0xff && res!=0xa0)
				{
					RS485_RX_COUNT=0;
				}
				if(( RS485_RX_COUNT==7 &&RS485_RX_BUF[0]==0xff)||( RS485_RX_COUNT==8 &&RS485_RX_BUF[0]==0xa0))
				{
					RS485_RX_FLAG=1;
				}
				if(RS485_RX_BUF[0]==0xff)RS485_RX_COUNT%=7;
				if(RS485_RX_BUF[0]==0xa0)RS485_RX_COUNT%=8;
		}
	}  
}



