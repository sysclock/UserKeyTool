#ifndef __RED_H
#define __RED_H 
 

  
#define RDATA GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)	 //�������������

//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0      		   

extern unsigned char  RmtCnt;	//�������µĴ���
extern unsigned char  IRcord[4];

void Remote_Init(void);    //���⴫��������ͷ���ų�ʼ��
unsigned char Remote_Scan(void);	  
void TIM3_Configuration(void);


#endif
















