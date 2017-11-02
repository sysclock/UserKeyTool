/*********************************************************************************
 * �ļ���  ��simple_server.c
 * ����    ��web�������Ӧ�ú�����
 *          
 * Ӳ�����ӣ� ------------------------------------
 *           |PB1         ��ENC28J60-INT|
 *           |PA6-SPI1-MISO��ENC28J60-SO          |
 *           |PA7-SPI1-MOSI��ENC28J60-SI           |
 *           |PA5-SPI1-SCK ��ENC28J60-SCK        |
 *           |PA4-SPI1-NSS ��ENC28J60-CS          |
 *           |ENC28J60-RST (û��)                      |
 *            ------------------------------------
 *
**********************************************************************************/
#include <string.h>
#include "Queue.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"

#ifdef TCP_IP

#define BUFFER_SIZE 155             //�������ݰ��Ĵ�С

static unsigned char EthernetBuf[BUFFER_SIZE+1];
/* mac��ַ��ip��ַ�ھ������ڱ���Ψһ������������������ͻ���������Ӳ��ɹ� */
static unsigned char mymac[6] = {0x00,0x54,0x55,0x58,0x10,0x24};
static unsigned char g_StaticIPaddr[4] = {192,168,1,15};
/* tcp/www�����˿ںţ���ΧΪ:1-254 */
static unsigned short mywwwport =90; 

#ifdef ENC28J60INT

#ifdef STM32F103RB
void Exit_Enc28j60INT_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//ʹ�ܰ�������GPIOʱ��,ͬʱʹ��AFIOʱ��(��Ҫ�����ⲿ�жϼĴ���)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			//���ð�������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������   	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//��ʼ����������GPIO�Ĵ���״̬

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);  	//ѡ�񰴼����ڵ�GPIO�ܽ������ⲿ�ж���·
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж����� 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�4λ,��16��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��		
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}

#else
void Exit_Enc28j60INT_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// Enable Button GPIO clock //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//ʹ�ܰ�������GPIOʱ��,ͬʱʹ��AFIOʱ��(��Ҫ�����ⲿ�жϼĴ���)
	
	// Configure Button pin as input floating //
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//���ð�������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������   	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//��ʼ����������GPIO�Ĵ���״̬

	// Connect Button EXTI Line to Button GPIO Pin //
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);  	//ѡ�񰴼����ڵ�GPIO�ܽ������ⲿ�ж���·
		
	// Configure Button EXTI line //
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж����� 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
	// Enable and set Button EXTI Interrupt to the lowest priority //
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�4λ,��16��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��		
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}
#endif

#endif

#endif

 
#ifdef EC11_BUTTON
void Exit_EC11_Pin4_Init() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);			//ʹ�ܰ�������GPIOʱ��,ͬʱʹ��AFIOʱ��(��Ҫ�����ⲿ�жϼĴ���)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = BMC_PIN;			//���ð�������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������   	
	GPIO_Init(BMC_GPIO, &GPIO_InitStructure);	//��ʼ����������GPIO�Ĵ���״̬

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);  	//ѡ�񰴼����ڵ�GPIO�ܽ������ⲿ�ж���·
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж����� 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�4λ,��16��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��		
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}
#ifdef TWO_EC11_BOARD
void Exit_EC11U15_Pin4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable Button GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			//ʹ�ܰ�������GPIOʱ��,ͬʱʹ��AFIOʱ��(��Ҫ�����ⲿ�жϼĴ���)
	
	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Pin = BMC2_PIN;			//���ð�������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������   	
	GPIO_Init(BMC2_GPIO, &GPIO_InitStructure);	//��ʼ����������GPIO�Ĵ���״̬

	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  	//ѡ�񰴼����ڵ�GPIO�ܽ������ⲿ�ж���·
		
	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж����� 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�4λ,��16��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��		
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}
#endif
#endif

