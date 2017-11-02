#include "stm32f10x.h"
#include <stdio.h>
#include "remote.h"
#include "header.h"


void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	// TIM3 clock enable //	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


	// ---------------------------------------------------------------
	//TIM3CLK ��PCLK1=36MHz
	//TIM3CLK = 36 MHz, Prescaler = 7200, TIM3 counter clock = 5K,���ı�һ��Ϊ5K,���ھ�Ϊ10K
	//--------------------------------------------------------------- //
	// Time base configuration //
	TIM_TimeBaseStructure.TIM_Period = 10; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������10Ϊ100us
	TIM_TimeBaseStructure.TIM_Prescaler =(720-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  100Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//����жϣ�
	
	// Enables the Update event for TIM3 //
	//TIM_UpdateDisableConfig(TIM3,ENABLE); 	//ʹ�� TIM3 �����¼� 

	// TIM IT enable //
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
	TIM3, 
	TIM_IT_Update   //TIM �ж�Դ
	// |TIM_IT_Trigger   //TIM �����ж�Դ 
	,ENABLE  //ʹ��
	);

	
	//TIM3 enable counter //
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}


void Remote_Init(void)    			  
{
	GPIO_InitTypeDef GPIO_InitStructure;
    	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
		
	// Enable and set Button EXTI Interrupt to the lowest priority //
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//��ռ���ȼ�4λ,��16��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��		
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
  	
	// Connect Button EXTI Line to Button GPIO Pin //
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);  	//ѡ�񰴼����ڵ�GPIO�ܽ������ⲿ�ж���·
		
	// Configure Button EXTI line //
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж����� 	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	//�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����		
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		
	//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);		
	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
}

unsigned short  irtime=1;//������ȫ�ֱ���

unsigned char   irok=0;
unsigned char   IRcord[4];  //�����ĺ����룬�ֱ��� �ͻ��룬�ͻ��룬�����룬�����뷴��
unsigned short   irdata[33]; //33���ߵ͵�ƽ��ʱ������
u8  RmtCnt=0;	//�������µĴ���

extern U8 g_RorL;
extern U8 g_numvalue;
static u16  EC11Cnt=0;
static U8 g_preEC11Value=0;
  
void TIM3_IRQHandler(void)   //TIM3�ж�100us�ж�һ��
{
	U8 cha=0;
	TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
	irtime++;
	#ifdef EC11_BUTTON
	EC11Cnt++;
	if(EC11Cnt==2000)//�ڹ̶�ʱ��仯����
	{
		EC11Cnt=0;
		if(g_preEC11Value!=g_numvalue)
		{
			if(g_RorL==0)
			{
				g_preEC11Value=g_numvalue;

				if(g_LockKeyFlag==UNLOCKKEYPAD)
				{
//					if( IsSetParaValueState())
                    	DealDownKey();
//					else
//						DealUpKey();
				}
				else if(g_LockKeyFlag==LOCKKEYPAD)
				{
					KeypadLockedPrompt();
				}
				
			}
			else
			{
				g_preEC11Value=g_numvalue;

				if(g_LockKeyFlag==UNLOCKKEYPAD)
				{
//					if( IsSetParaValueState())
                    	DealUpKey();
//					else
//					   	DealDownKey();
				}
				else if(g_LockKeyFlag==LOCKKEYPAD)
				{
				KeypadLockedPrompt();
				}				
			}
		}
	}
#endif
}
 
//�ⲿ�ж�1�������
void EXTI15_10_IRQHandler(void)
{	
	
	           //���պ����źŴ���
	static unsigned char   startflag=1;//0;                //�Ƿ�ʼ�����־λ
	static unsigned char  i=0; 
	
	
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0x00)		//��������:�͵�ƽ��Ч
	{
		//chinesedisplay(0x80, "ANJIAN");
		if(startflag)                         
		{

			if(irtime>100&&irtime<150)//������ TC9012��ͷ�룬9ms+4.5ms
			{
				i=0;
			}
   	     //�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1
			if(irtime<800)
			{
				irdata[i]=irtime;
			}
			else
			{
				RmtCnt++;				
			}			
			
			irtime=0;
			i++;
			if(i==33)
			{
				irok=1;
				i=0;
				RmtCnt=0;				
			}			
			startflag=0;
		}
		else
		{
			irtime=0;
			startflag=1;
		}
	} 
	EXTI_ClearITPendingBit(EXTI_Line11);  //���EXTI1��·����λ  

}
	  

/******************************************************************/
/*                    ������뺯������                            */
/******************************************************************/
void Ircordpro(void)//������ֵ������
{ 
	unsigned char i, j, k;
	unsigned char value=0; 
	unsigned short cord=0;

	k=1;
	for(i=0;i<4;i++)      //����4���ֽ�
	{
		for(j=1;j<=8;j++) //����1���ֽ�8λ
		{
			cord=irdata[k];
			
			//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����	  
			if(cord>18)
			{
				value=value|0x01;//0x80;
			}
			else 
			{
				value=value&0xFE;
			}
			if(j<8)
			{
				value=value<<1;//>>1;
			}
			k++;
		}
		IRcord[i]=value;
	 //   printf("%c",IRcord[i]);	
				
		value=0;     
	} 	 
	
    
}

unsigned char Remote_Scan(void)
{
	if(irok)                        //������պ��˽��к��⴦��
	{   
		irok=0;
		delay_ms(500);
		Ircordpro();
	
		if((IRcord[0]==(u8)~IRcord[1])&&IRcord[0]==REMOTE_ID)
		{
			if(IRcord[2]==(u8)~IRcord[3])
			{				
				return 1;
			}
		}
	}
	return 0;
}

































