#ifndef __SYSTICK_H
#define __SYSTICK_H

void SYSTICK_Init(void);
void delay_us(u16 nus);
void delay_ms(u16 nms);

extern u8 zoomirq_flag;
extern u32 zoomirq_time;
#endif
