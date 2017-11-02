#ifndef __SPI_ENC28J60_H
#define __SPI_ENC28J60_H

#include "stm32f10x.h"
#include "header.h"

#ifdef TCP_IP
void SPI_Enc28j60_Init(void);
unsigned char	SPI1_ReadWrite(unsigned char writedat);
#endif

#endif /* __SPI_ENC28J60_H */
