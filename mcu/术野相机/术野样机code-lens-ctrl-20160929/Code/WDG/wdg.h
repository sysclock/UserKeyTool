#ifndef __WDG_H
#define __WDG_H

#include "stm32f10x.h"
//#include "sys.h"

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

#endif
