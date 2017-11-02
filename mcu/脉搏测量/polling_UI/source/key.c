#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "key.h"
#include "MenuTbl.h"
#include "flash_spi.h"
#include "fsl_port.h"

#define KEY_LONG_TIME  30//����һ����ֵ ���������𳤶̰���

_OnOff Power_OnOff_Flag=POWER_OFF;
///////////////////////////////////
//SET   PTB 2  �ⲿ����������Ч
//RET   PTB 0
//UP    PTB 1
//RST   RESET_b
//POW   PTA 29
///////////////////////////////////


void delay_20000(uint32_t num)
{
	uint32_t i,j;
	for(i=0;i<num;i++)
		for(j=0;j<20000;j++);
}




//������ʼ��������
void Key_Config(void)
{
	  gpio_pin_config_t key_config = {
        kGPIO_DigitalInput, 1,
    };
		port_pin_config_t config2 = {
				kPORT_PullUp,
				kPORT_FastSlewRate,
				kPORT_PassiveFilterDisable,
				kPORT_OpenDrainDisable,
				kPORT_LowDriveStrength,
				kPORT_MuxAsGpio,
		};
        
        
        port_pin_config_t config3 = {
				kPORT_PullDown,
				kPORT_FastSlewRate,
				kPORT_PassiveFilterDisable,
				kPORT_OpenDrainDisable,
				kPORT_LowDriveStrength,
				kPORT_MuxAsGpio,
		};


        
        
		PORT_SetPinConfig(PORTA,KEY_POW_PIN,&config2);	
		PORT_SetPinConfig(PORTB, USB5_LINK_PIN,&config3);	  
		PORT_SetPinConfig(PORTD, BAT_FULL_PIN,&config3);
    
		GPIO_PinInit(KEY_POW_GPIO, KEY_POW_PIN, &key_config);
		GPIO_PinInit(KEY_RET_GPIO, KEY_RET_PIN, &key_config);
		GPIO_PinInit(KEY_SET_GPIO, KEY_SET_PIN, &key_config);
		GPIO_PinInit(KEY_UD_GPIO, KEY_UD_PIN, &key_config);
		GPIO_PinInit(USB5_LINK_GPIO, USB5_LINK_PIN, &key_config);  //��Ϊ����Ƿ�ΪUSB���磬����ǹ���Ļ������ð����ػ�
		GPIO_PinInit(BAT_FULL_GPIO, BAT_FULL_PIN, &key_config);  //��Ϊ����Ƿ�ΪUSB���磬����ǹ���Ļ������ð����ػ�		
}




//��ⰴ���Ķ���
void Scan_KeyValue(void)
{
	static uint32_t KeyACC=0;
	static _KeypadNum keyPress=KEY_NO_VALUE;
	_KeypadNum keyVal=KEY_NO_VALUE;
	
	if(!GPIO_ReadPinInput(KEY_POW_GPIO,KEY_POW_PIN))  //���µ�Դ��
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_POW_GPIO,KEY_POW_PIN))  //�л���������
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_POW_VALUE);  
		}
	}
	else if(!GPIO_ReadPinInput(KEY_UD_GPIO,KEY_UD_PIN))
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_UD_GPIO,KEY_UD_PIN))  //�л���������
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_UD_VALUE);  	
		}
		
	}
	else if(!GPIO_ReadPinInput(KEY_SET_GPIO,KEY_SET_PIN))
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_SET_GPIO,KEY_SET_PIN))  //�л���������
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_SET_VALUE);  	
		}
		if((Is_ParaSet_States())&&KeyACC>3)
		{
			Deal_ParaSet_Func();
			KeyACC=0;
			keyPress=KEY_NO_VALUE;
		}
	}
	else if(!GPIO_ReadPinInput(KEY_RET_GPIO,KEY_RET_PIN))
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_RET_GPIO,KEY_RET_PIN))  //�л���������
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_RET_VALUE);  	
		}
	}
	else if(keyPress!=KEY_NO_VALUE)  //�Ѿ��а���ֵ�ж�
	{
		keyVal=keyPress;
		if(KeyACC>0 &&KeyACC <KEY_LONG_TIME)  //�ǳ�����
		{		
			if(keyVal==KEY_SET_VALUE) //�����ǰ�����������ü�
			{
				Deal_SET_Key();     //�������ü��Ĵ���
			}
			else if(keyVal==KEY_RET_VALUE) //����/ȡ����
			{
				Deal_RES_Key();			//����ȡ�����Ĵ���
			}
			else if(keyVal==KEY_UD_VALUE) //�л���
			{
				Deal_UD_Key();			//�����л����Ĵ���
			}	
		}
		else if(KeyACC >=KEY_LONG_TIME)  //����������
		{
			if(keyVal==KEY_SET_VALUE) //�������ü�
			{
				
			}
			else if(keyVal==KEY_RET_VALUE) //��������/ȡ����
			{
				
			}
			else if(keyVal==KEY_UD_VALUE) //�����л���    �������
			{

			}		
			else if(keyVal==KEY_POW_VALUE)  //�˴����밴����ȡ��������Ϊ���˴��ǰ��²��������Ч������ϱߵĴ��ڣ���������Ч
			{
				//��������Ѿ��������Ұ���Ϊ����power on/off����Ҳû�н���USB���磬����йػ�
				if(Power_OnOff_Flag==POWER_ON &&(!GPIO_ReadPinInput(USB5_LINK_GPIO,USB5_LINK_PIN)))
				{
                   
					PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAsGpio);            /*����Ϊ��ͨio�� */  //����  2017-1��24��                   
					PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAsGpio);            /*����Ϊ��ͨio�� */
        
					Power_OnOff_Flag=POWER_OFF;
					GPIO_ClearPinsOutput(GPIOB, 1u << 22);	
                      
				}
			}
		}
		KeyACC=0;
		keyPress=KEY_NO_VALUE;
		keyVal=KEY_NO_VALUE;
	}
	
}

