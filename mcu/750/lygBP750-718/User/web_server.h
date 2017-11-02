#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

#include "header.h"

#ifdef EC11_BUTTON
void Exit_EC11_Pin4_Init(void);
#ifdef TWO_EC11_BOARD
void Exit_EC11U15_Pin4_Init(void);
#endif
#endif

#ifdef TCP_IP

  #ifdef ENC28J60INT
    void Exit_Enc28j60INT_Init(void);
  #endif
int Web_Server(void);
void Write_EEPROM_IP(void);
BOOL Read_EEPROM_IP(void);
void DisMathineStaticIP(void);
void SetStaticIPAddress(U8 wValue);
void ReceiveEthernetPacket(void);
void Ethernet_SendBuf(U8 *data, U32 number);
#endif

#endif


