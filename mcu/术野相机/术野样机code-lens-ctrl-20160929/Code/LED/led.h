#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define power_led_on GPIO_SetBits(GPIOB, GPIO_Pin_2);	
#define power_led_off GPIO_ResetBits(GPIOB, GPIO_Pin_2);	

void LED_GPIO_Config(void);	

#endif
