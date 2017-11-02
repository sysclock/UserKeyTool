#ifndef SWITCH_H
#define SWITCH_H

#include "Init.h"

extern u8* InOut_V;
extern u8* InOut_A;

void SetSwitchV(u8 Input,u8 Output);
void SetSwitchA(u8 Input,u8 Output);
void InitDVI(void);
#endif
