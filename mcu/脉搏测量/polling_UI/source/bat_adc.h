#ifndef _BAT_ADC_H_
#define _BAT_ADC_H_

#include "stdint.h"
                                                                                                       
#define BAT_ADC16_BASE ADC0
#define BAT_ADC16_CHANNEL_GROUP 0U
#define BAT_ADC16_USER_CHANNEL 14U

void Bat_ADC_Init(void);
uint32_t read_bat_adc(void);
uint32_t read_temperature_adc(void);

#endif
