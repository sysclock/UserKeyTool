#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "uart.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "Queue.h"
#include "rtc.h"

/////////////////�������ô���uart3/////////////////////
#define BLE_UART_IRQn UART3_RX_TX_IRQn
#define BLE_UART_IRQHandler UART3_RX_TX_IRQHandler

#define BLE_UART UART3
#define BLE_UART_CLKSRC UART3_CLK_SRC

uint8_t  BLE_receive_flag=0;
uint8_t BLE_txbuff[BLE_BUFFER_SIZE] ={0xA5};
uint8_t BLE_rxbuff[BLE_BUFFER_SIZE] ={0xA5};

volatile uint16_t BLE_txIndex; /* Index of the data to send out. */
volatile uint16_t BLE_rxIndex; /* Index of the memory to save new arrived data. */
///////////////////////////////////////////////////////////////////////

/////////////////PW1101���ô���uart2/////////////////////
#define PW1101_UART_IRQn UART2_RX_TX_IRQn
#define PW1101_UART_IRQHandler UART2_RX_TX_IRQHandler

#define PW1101_UART UART2
#define PW1101_UART_CLKSRC UART2_CLK_SRC

uint8_t  PW1101_receive_flag=0;
uint8_t PW1101_txbuff[PW1101_BUFFER_SIZE] ={0xA5};
uint8_t PW1101_rxbuff[PW1101_BUFFER_SIZE] ={0xA5};

volatile uint16_t PW1101_txIndex; /* Index of the data to send out. */
volatile uint16_t PW1101_rxIndex; /* Index of the memory to save new arrived data. */
///////////////////////////////////////////////////////////////////////


/////////////////USB���ô���uart4/////////////////////
#define USB_UART_IRQn UART4_RX_TX_IRQn
#define USB_UART_IRQHandler UART4_RX_TX_IRQHandler

#define USB_UART UART4
#define USB_UART_CLKSRC UART4_CLK_SRC

uint8_t  USB_receive_flag=0;
uint8_t USB_txbuff[USB_BUFFER_SIZE] ={0xA5};
uint8_t USB_rxbuff[USB_BUFFER_SIZE] ={0xA5};

volatile uint16_t USB_txIndex; /* Index of the data to send out. */
volatile uint16_t USB_rxIndex; /* Index of the memory to save new arrived data. */
///////////////////////////////////////////////////////////////////////
uint8_t APP_Interact_Flag=0;
uint8_t APP_CRC_Error_Flag=0;


//�����жϴ���
void BLE_UART_IRQHandler(void)
{
     uint8_t data;
		uint8_t i;
    /* If new data arrived. */
		if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(BLE_UART))
    {
        data = UART_ReadByte(BLE_UART);
				if(BLE_rxIndex==0 )
				{
					if(data==0xA5)
					{
						BLE_rxIndex++;
					}
					else
					{
						BLE_rxIndex=0;
					}
				}
				else if(BLE_rxIndex !=0 && BLE_receive_flag==0)
				{
					BLE_rxbuff[BLE_rxIndex] = data;
					BLE_rxIndex++;
					BLE_rxIndex %= BLE_BUFFER_SIZE;
					
					if(BLE_rxIndex>DATA_LEN_INDEX) //�յ������ݳ����ֽ�
					{
						if(BLE_rxIndex==(10+BLE_rxbuff[DATA_LEN_INDEX]))
						{
							BLE_receive_flag=1;
							///////����յ�����
							if(Check_CRC8(BLE_rxbuff)==BLE_rxbuff[BLE_rxIndex-1])  //�յ���ȷ��ֵ
							{	
								//����յ�������Ϊ�������ݣ�ֱ�ӷ��������
									for(i=0;i<BLE_rxIndex;i++)
										APP_Comm_QueueIn(&g_BLERxQueue,BLE_rxbuff[i]);
									BLE_receive_flag=0;
									BLE_rxIndex=0;
									APP_Interact_Flag=1;
							}
							else
							{
								BLE_receive_flag=0;
								BLE_rxIndex=0;
								APP_CRC_Error_Flag=BLE_COMM;
							}
						}
					}
				}
    }
}



//USBת���ڵ��жϺ���
void USB_UART_IRQHandler(void)
{
    uint8_t data;
		uint8_t i;
    /* If new data arrived. */
		if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(USB_UART))
    {
        data = UART_ReadByte(USB_UART);
				if(USB_rxIndex==0 )
				{
					if(data==0xA5)
					{
						USB_rxIndex++;
					}
					else
					{
						USB_rxIndex=0;
					}
				}
				else if(USB_rxIndex !=0 && USB_receive_flag==0)
				{
					USB_rxbuff[USB_rxIndex] = data;
					USB_rxIndex++;
					USB_rxIndex %= USB_BUFFER_SIZE;
					
					if(USB_rxIndex>DATA_LEN_INDEX) //�յ������ݳ����ֽ�
					{
						if(USB_rxIndex==(10+USB_rxbuff[DATA_LEN_INDEX]))
						{
							USB_receive_flag=1;
							///////����յ�����
							if(Check_CRC8(USB_rxbuff)==USB_rxbuff[USB_rxIndex-1])  //�յ���ȷ��ֵ
							{	
								//����յ�������Ϊ�������ݣ�ֱ�ӷ��������
									for(i=0;i<USB_rxIndex;i++)
										APP_Comm_QueueIn(&g_USBRxQueue,USB_rxbuff[i]);
									USB_receive_flag=0;
									USB_rxIndex=0;
									APP_Interact_Flag=1;
							}
							else
							{
								USB_receive_flag=0;
								USB_rxIndex=0;
								APP_CRC_Error_Flag=USB_COMM;
							}
						}
					}
				}
    }
}



//����оƬ�Ĵ���ͨ�Ŵ���
uint8_t data_group_count=0;

void PW1101_UART_IRQHandler(void)
{
    uint8_t data;
		uint8_t i;
    /* If new data arrived. */
		if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(PW1101_UART))
    {
        data = UART_ReadByte(PW1101_UART);
				if(PW1101_rxIndex==0 )
				{
					if(data==0xA5)
					{
						PW1101_rxIndex++;
					}
					else
					{
						PW1101_rxIndex=0;
					}
				}
				else if(PW1101_rxIndex==1 && (data>0x9F ||data<0x90))
				{
					PW1101_rxIndex=0;
				}
				else if(PW1101_rxIndex !=0 && PW1101_receive_flag==0)
				{
					PW1101_rxbuff[PW1101_rxIndex] = data;
					PW1101_rxIndex++;
					PW1101_rxIndex %= PW1101_BUFFER_SIZE;
					
					if(PW1101_rxIndex>DATA_LEN_INDEX) //�յ������ݳ����ֽ�
					{
						if(PW1101_rxIndex==(10+PW1101_rxbuff[DATA_LEN_INDEX]))
						{
							PW1101_receive_flag=1;
							///////����յ�����
							if(Check_CRC8(PW1101_rxbuff)==PW1101_rxbuff[PW1101_rxIndex-1])  //�յ���ȷ��ֵ
							{	
								//����յ�������Ϊ�������ݣ�ֱ�ӷ��������
								if(PW1101_rxbuff[COMMAND_ID_INDEX]==COMMAND_ID_SEND_MEASURE_DATA)
								{
									for(i=1;i<PW1101_rxbuff[DATA_LEN_INDEX];i++)
										PW1101_Data_QueueIn(&g_DataRxQueue,PW1101_rxbuff[9+i]);
									PW1101_receive_flag=0;
									PW1101_rxIndex=0;
									data_group_count++;
								}	
								else
								{
									for(i=0;i<PW1101_rxIndex;i++)
										PW1101_Comm_QueueIn(&g_CommRxQueue,PW1101_rxbuff[i]);
									PW1101_receive_flag=0;
									PW1101_rxIndex=0;
								}
							}
							else
							{
								PW1101_receive_flag=0;
								PW1101_rxIndex=0;
							}
						}
					}
				}
    }
}



//����ģ�鷢��
void BLE_UART_Send_Byte(uint8_t data)
{
		UART_WriteByte(BLE_UART, data);
		while (!(kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(BLE_UART)));
}


//USB����
void USB_UART_Send_Byte(uint8_t data)
{
		UART_WriteByte(USB_UART, data);
		while (!(kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(USB_UART)));
}

//���з���
void PW1101_UART_Send_Byte(uint8_t data)
{
		UART_WriteByte(PW1101_UART, data);
		while (!(kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(PW1101_UART)));
}


void BLE_UART_Init(void)
{
	gpio_pin_config_t LCD_config = {
        kGPIO_DigitalOutput, 0,
    };
	 uart_config_t config;
	  /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
	  UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

		UART_Init(BLE_UART, &config, CLOCK_GetFreq(BLE_UART_CLKSRC));
		UART_EnableInterrupts(BLE_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    		EnableIRQ(BLE_UART_IRQn);
	
		GPIO_PinInit(GPIOA, PIN5_IDX, &LCD_config);  //BLU_Pow
		GPIO_SetPinsOutput(GPIOA, 1u << 5);
		
		GPIO_PinInit(GPIOE, PIN6_IDX, &LCD_config);  //Blue_nRST
		GPIO_SetPinsOutput(GPIOE, 1u << 6);
}


//USB��ʼ��
void USB_UART_Init(void)
{
	 uart_config_t config;
	  /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
	  UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

		UART_Init(USB_UART, &config, CLOCK_GetFreq(USB_UART_CLKSRC));
		UART_EnableInterrupts(USB_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(USB_UART_IRQn);
}




//���г�ʼ��
void PW1101_UART_Init(void)
{
	 uart_config_t config;
	  /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
	  UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

		UART_Init(PW1101_UART, &config, CLOCK_GetFreq(PW1101_UART_CLKSRC));
		UART_EnableInterrupts(PW1101_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(PW1101_UART_IRQn);
	
		gpio_pin_config_t PW1101_config = {
        kGPIO_DigitalOutput, 1,
    };
		GPIO_PinInit(GPIOD, PIN0_IDX, &PW1101_config);  //����PTD0Ϊ���===>>PW1101_nRST
}



//����APP
#define APP_RX_OK 1
#define APP_RX_ERROR 2
void reply_APP(uint8_t para,uint8_t para1)  //����1���ճɹ�ʧ�ܣ�����2��������USB����BLE�ı�־
{
	uint8_t i;
	uint8_t data[11]={0xa5,0x50,0x21,0x00,0x01,0x00,0x01,0x33,0x01,0x01,0x00};
	
	if(para==APP_RX_OK)
	{
		data[1]=0x50;
		data[8]=0x01;
		data[9]=0x01;
		data[10]=Check_CRC8(data);
		if(para1==USB_COMM)
		{
			for(i=0;i<11;i++)
				USB_UART_Send_Byte(data[i]);
		}
		else
		{
			for(i=0;i<11;i++)
				BLE_UART_Send_Byte(data[i]);
		}
	}
	else
	{	
		data[1]=0x00;
		data[8]=0x00;
		data[9]=Check_CRC8(data);
		if(para1==USB_COMM)
		{
			for(i=0;i<10;i++)
				USB_UART_Send_Byte(data[i]);
		}
		else
		{
			for(i=0;i<10;i++)
				BLE_UART_Send_Byte(data[i]);
		}
	}
}

