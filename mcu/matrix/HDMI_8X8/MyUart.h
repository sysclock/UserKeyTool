#ifndef MYUART_H
#define MYUART_H

void USART1_RXD_IT(void);
void USART2_RXD_IT(void);
void DisposalUart(void);
void SendBufToUart(u8 Len);
#endif
