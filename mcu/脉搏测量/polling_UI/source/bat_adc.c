#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc16.h"
#include "pin_mux.h"
#include "bat_adc.h"
#include "key.h"
#include "uart.h"

adc16_config_t adc16ConfigStruct;
adc16_channel_config_t adc16ChannelConfigStruct;
adc16_channel_config_t adc16Channel_Temperature_ConfigStruct;



//电池测量的ADC初始化
void Bat_ADC_Init(void)
{
	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	ADC16_Init(BAT_ADC16_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(BAT_ADC16_BASE, false); /* Make sure the software trigger is used. */

	adc16ChannelConfigStruct.channelNumber = BAT_ADC16_USER_CHANNEL;
	adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	adc16ChannelConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
	
		adc16Channel_Temperature_ConfigStruct.channelNumber = 26;
	adc16Channel_Temperature_ConfigStruct.enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	adc16Channel_Temperature_ConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
}



//读电池电源的电压
uint32_t read_bat_adc(void)
{
	unsigned int i=0;
	unsigned short data[5];
	uint32_t adc_num=0;
	unsigned char num;
	
	
	
	for(i=0;i<5;i++)
	{
		ADC16_SetChannelConfig(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);
		while (0U == (kADC16_ChannelConversionDoneFlag &
								ADC16_GetChannelStatusFlags(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP)))
		{
		}
		data[i]=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);		
		adc_num+=data[i];
	}	
	return adc_num/5;
}




//读温度
uint32_t read_temperature_adc(void)
{
	uint32_t adc_num;
	ADC16_SetChannelConfig(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP, &adc16Channel_Temperature_ConfigStruct);
	while (0U == (kADC16_ChannelConversionDoneFlag &
								ADC16_GetChannelStatusFlags(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP)))
	{
	}
	adc_num=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);
	delay_20000(100);// 做一点空循环延时
	adc_num+=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);
	delay_20000(100);// 做一点空循环延时
	adc_num+=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);
	delay_20000(100);// 做一点空循环延时
	adc_num+=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);
	delay_20000(100);// 做一点空循环延时
	adc_num+=ADC16_GetChannelConversionValue(BAT_ADC16_BASE, BAT_ADC16_CHANNEL_GROUP);
	return adc_num/5;
}

