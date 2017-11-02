#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "key.h"
#include "MenuTbl.h"
#include "flash_spi.h"
#include "fsl_port.h"

#define KEY_LONG_TIME  30//定义一个宏值 ，用来区别长短按键

_OnOff Power_OnOff_Flag=POWER_OFF;
///////////////////////////////////
//SET   PTB 2  外部上拉，低有效
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




//按键初始化和设置
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
		GPIO_PinInit(USB5_LINK_GPIO, USB5_LINK_PIN, &key_config);  //做为检测是否为USB供电，如果是供电的话，不用按键关机
		GPIO_PinInit(BAT_FULL_GPIO, BAT_FULL_PIN, &key_config);  //做为检测是否为USB供电，如果是供电的话，不用按键关机		
}




//检测按键的动作
void Scan_KeyValue(void)
{
	static uint32_t KeyACC=0;
	static _KeypadNum keyPress=KEY_NO_VALUE;
	_KeypadNum keyVal=KEY_NO_VALUE;
	
	if(!GPIO_ReadPinInput(KEY_POW_GPIO,KEY_POW_PIN))  //按下电源键
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_POW_GPIO,KEY_POW_PIN))  //切换键被按下
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_POW_VALUE);  
		}
	}
	else if(!GPIO_ReadPinInput(KEY_UD_GPIO,KEY_UD_PIN))
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_UD_GPIO,KEY_UD_PIN))  //切换键被按下
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_UD_VALUE);  	
		}
		
	}
	else if(!GPIO_ReadPinInput(KEY_SET_GPIO,KEY_SET_PIN))
	{
		delay_20000(100);
		if(!GPIO_ReadPinInput(KEY_SET_GPIO,KEY_SET_PIN))  //切换键被按下
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
		if(!GPIO_ReadPinInput(KEY_RET_GPIO,KEY_RET_PIN))  //切换键被按下
		{
			KeyACC++;
			keyPress=(_KeypadNum)(KEY_RET_VALUE);  	
		}
	}
	else if(keyPress!=KEY_NO_VALUE)  //已经有按键值判断
	{
		keyVal=keyPress;
		if(KeyACC>0 &&KeyACC <KEY_LONG_TIME)  //非长按键
		{		
			if(keyVal==KEY_SET_VALUE) //如果当前按键等于设置键
			{
				Deal_SET_Key();     //进行设置键的处理
			}
			else if(keyVal==KEY_RET_VALUE) //返回/取消键
			{
				Deal_RES_Key();			//进行取消键的处理
			}
			else if(keyVal==KEY_UD_VALUE) //切换键
			{
				Deal_UD_Key();			//进行切换键的处理
			}	
		}
		else if(KeyACC >=KEY_LONG_TIME)  //长按键操作
		{
			if(keyVal==KEY_SET_VALUE) //长按设置键
			{
				
			}
			else if(keyVal==KEY_RET_VALUE) //长按返回/取消键
			{
				
			}
			else if(keyVal==KEY_UD_VALUE) //长按切换键    清空数据
			{

			}		
			else if(keyVal==KEY_POW_VALUE)  //此处理与按键读取处的区别为，此处是按下并弹起后生效，如果上边的存在，这里则不生效
			{
				//如果现在已经开机，且按键为长按power on/off键，也没有进行USB供电，则进行关机
				if(Power_OnOff_Flag==POWER_ON &&(!GPIO_ReadPinInput(USB5_LINK_GPIO,USB5_LINK_PIN)))
				{
                   
					PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAsGpio);            /*设置为普通io口 */  //吕鹏  2017-1月24日                   
					PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAsGpio);            /*设置为普通io口 */
        
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

