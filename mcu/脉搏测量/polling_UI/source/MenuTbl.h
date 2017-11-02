#ifndef _MENUTBL_H_
#define _MENUTBL_H_

#include "stdint.h"
#include "states.h"

typedef enum {TURN_ON,TURN_OFF} _Turn_OnOff;

extern _Turn_OnOff Auto_Measure_OnOff;

typedef struct 
{    
    uint8_t  sex;
		uint8_t  age;
    uint8_t height;
		uint8_t weight;
} _user_t;

typedef struct
{
	uint16_t   HBlood_LowValue;
	uint16_t  HBlood_HighValue;
	uint8_t   LBlood_LowValue;
	uint8_t 	LBlood_HighValue;
	uint8_t   HeartRate_LowValue;
	uint8_t   HeartRate_HighValue;
} _alarm_t;

typedef struct 
{
	uint8_t Groupnum;
	uint8_t Timelong;
	uint8_t Timeinterval;
	uint8_t Eletrode;
}_MeasurePara_t;

typedef struct
{
	uint8_t Timelong;
	uint8_t Timeinterval;
}_AutoMeasurePara_t;

extern _AutoMeasurePara_t AutoMeasurePara;
extern _MeasurePara_t MeasurePara;
extern uint8_t Current_Eletrode;
extern _user_t user;
extern _alarm_t alarm;
extern uint8_t StandbyTime;

extern State  g_CurrentStat;
extern uint8_t mmHg_or_kPa;
extern uint8_t Measure_Mode;

extern uint8_t Ble_OnOff;
extern uint8_t NoDisturb_Mode;


extern uint32_t RIDL;
extern uint32_t RIDML;
extern uint32_t RIDMH;
extern uint32_t RIDH;


void Deal_SET_Key(void);

void Deal_RES_Key(void);

void Deal_UD_Key(void);

void Deal_POW_Key(void);

uint8_t Is_ParaSet_States(void);

uint8_t Is_ParaSelect_States(void);

void Deal_ParaSet_Func(void);

#endif
