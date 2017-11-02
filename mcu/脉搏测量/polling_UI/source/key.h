#ifndef _KEY_H_
#define _KEY_H_

#include "stdint.h"

#define USB5_LINK_GPIO GPIOB
#define USB5_LINK_PIN  23U

#define BAT_FULL_GPIO GPIOD
#define BAT_FULL_PIN 14U

#define KEY_SET_GPIO  GPIOB
#define KEY_SET_PIN  2U
#define KEY_RET_GPIO  GPIOB
#define KEY_RET_PIN  0U
#define KEY_UD_GPIO   GPIOB
#define KEY_UD_PIN  1U
#define KEY_POW_GPIO  GPIOA
#define KEY_POW_PIN  29U

typedef enum{
	KEY_SET_VALUE,
	KEY_RET_VALUE,
	KEY_UD_VALUE,
	KEY_POW_VALUE,
	KEY_NO_VALUE
}_KeypadNum;

typedef enum {POWER_ON,POWER_OFF} _OnOff;

extern _OnOff Power_OnOff_Flag;

void delay_20000(uint32_t num);
void Key_Config(void);
void Scan_KeyValue(void);

#endif
