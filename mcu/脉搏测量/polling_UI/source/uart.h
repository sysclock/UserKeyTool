#ifndef _UART_H_
#define _UART_H_

#include "stdint.h"
#include "fsl_uart.h"

extern uint8_t APP_Interact_Flag;
extern uint8_t APP_CRC_Error_Flag;

#define BLE_BUFFER_SIZE 256
extern uint8_t BLE_txbuff[BLE_BUFFER_SIZE];
extern uint8_t BLE_rxbuff[BLE_BUFFER_SIZE];


extern volatile uint16_t BLE_txIndex; /* Index of the data to send out. */
extern volatile uint16_t BLE_rxIndex; /* Index of the memory to save new arrived data. */
void BLE_UART_IRQHandler(void);
void BLE_UART_Init(void);
void BLE_UART_Send_Byte(uint8_t data);


#define USB_BUFFER_SIZE 256
extern uint8_t USB_txbuff[USB_BUFFER_SIZE];
extern uint8_t USB_rxbuff[USB_BUFFER_SIZE];

extern volatile uint16_t USB_txIndex; /* Index of the data to send out. */
extern volatile uint16_t USB_rxIndex; /* Index of the memory to save new arrived data. */
void USB_UART_IRQHandler(void);
void USB_UART_Init(void);
void USB_UART_Send_Byte(uint8_t data);


#define PW1101_BUFFER_SIZE 256
extern uint8_t PW1101_txbuff[PW1101_BUFFER_SIZE];
extern uint8_t PW1101_rxbuff[PW1101_BUFFER_SIZE];
extern uint8_t  PW1101_receive_flag;

extern volatile uint16_t PW1101_txIndex; /* Index of the data to send out. */
extern volatile uint16_t PW1101_rxIndex; /* Index of the memory to save new arrived data. */
void PW1101_UART_IRQHandler(void);
void PW1101_UART_Init(void);
void PW1101_UART_Send_Byte(uint8_t data);

#define APP_RX_OK 1
#define APP_RX_ERROR 2
void reply_APP(uint8_t para,uint8_t para1) ;


extern uint8_t data_group_count;
#endif
