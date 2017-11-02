/*
������ʹ�˾
���в����豸
������
���:
20161201

*/


#include "board.h"
#include "fsl_uart.h"
#include "uart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "lcd_spi.h"
#include "flash_spi.h"
#include "key.h"
#include "rtc.h"
#include "MenuTbl.h"
#include "pw1101_interact.h"
#include "fb.h"
#include "bat_adc.h"
#include "Queue.h"
#include "lowpower.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_smc.h"
uint32_t bat_num=6;
uint8_t bat_charge_flag=4;
uint8_t bat_num_flag=3;

//��������оƬID
void read_chipID(void)
{
	RIDL = SIM->UIDL;   //MCU ID
  RIDML = SIM->UIDML;
  RIDMH = SIM->UIDMH;
  RIDH = SIM->UIDH;
}

void Deal_APP_comm(void);
void Deal_Auto_Measure(void);
void Deal_StandbyTime(void);
void Updata_main_page_info(void);



int main(void)
{
	uint32_t i,j,k;										 										 
		gpio_pin_config_t IOout_config = {
			kGPIO_DigitalOutput, 0,
		};
		//Ӳ���ܽų�ʼ��
		BOARD_InitPins();

		//ʱ�ӳ�ʼ����Դ�Թٷ�
		BOARD_BootClockRUN();
		
		GPIO_PinInit(GPIOB, 22, &IOout_config);	//power-on	 pin		
		GPIO_PinInit(GPIOB, 21, &IOout_config);//vdda pin
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //���߿���VDDA ��1u,�޷�����1
		
		


		//PW1101
		GPIO_PinInit(GPIOD, 0, &IOout_config);
		GPIO_ClearPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��D��0������		
		GPIO_PinInit(GPIOD, 0, &IOout_config);//��������
		GPIO_SetPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��

		
		Key_Config();  //�����ĳ�ʼ��
		
//		User_PIT_Init();  //���ö�ʱ��
		User_RTC_Init(); //����RTCʱ�ӵĳ�ʹ��
		
		Delay_S(3);  //��ʱ3s����
		
		//�Ѿ��������ѵ�ع���ͨ����
		if(Power_OnOff_Flag==POWER_OFF)
		{
			Power_OnOff_Flag=POWER_ON;
			GPIO_SetPinsOutput(GPIOB, 1u << 22);				
		}	
		
		LCD_Init();			 //����LCD���õ�SPI��
		Display_full_screen(Page_fb_Data);  //��ʾ��������
		Delay_S(2); 

		
		S25FL512S_Spi_Config();  
//		S25FL512S_Erase_Sector(0);		
//		S25FL512S_Erase_Sector(1); //�����ã����������������
//		S25FL512S_Erase_Sector(16); //�����ã������������ݾ���
//		S25FL512S_Erase_Sector(4);  //�����ã������������ݾ�������


		Read_para_from_flash();  //��FLASH  �ж�ȡ�û�����
		
		if(Ble_OnOff==1) //�������Ϊ��״̬
		{
			GPIO_ClearPinsOutput(GPIOA, 1u << 5);	 
		}
			
		//�����ʼ��
		BLE_UART_Init();//����BLE����MCUͨ�Ŵ��ڵĳ�ʹ��
		USB_UART_Init(); //����USBת����H340�ĳ�ʹ��
		PW1101_UART_Init();//����PW1101ͨ��
		PW1101_Data_RxQueue_Init();  //����һ�����ݽ��ն���
		PW1101_Comm_RxQueue_Init();  //����һ��ָ����ն���
		APP_Comm_RxQueue_Init();     //�������ڽ���APP���ƵĶ���
		Bat_ADC_Init();            //��ʼ��ADC�ɼ����˺���ʵ���˵�غ��¶ȵ�ADC����
		bat_num=read_bat_adc();	
		read_chipID();  //��ȡID��


		//
		Find_measure_data_add_prt();//�ҵ����һ�μ�¼�������ݴ��λ�õ��������Ӷ��ҵ����һ�����ݴ��λ��
		Find_Save_Index_Ptr(); //�ҵ����һ����¼����������λ��
	
		
		if(read_PW1101Information()==INTERACT_FAIL) //��ȡIC��Ϣ ����ȡ���󣬸���ʾ
			g_CurrentStat=State_Main_Menu_interact_error;
		else		
			g_CurrentStat=State_Main_Menu; 	//��ǰ״̬��������������״̬
		
		stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);//���ؽ��� 
		i=0;

		
    while (1)
    {				
			if(Auto_Measure_OnOff==TURN_ON)  //����Ѿ��򿪺�̨����
			{
				Deal_Auto_Measure();
			}
			
			if(StandbyTime_Count>StandbyTime)  //30�������ʱ��StandbyTime_Count�ĸ�����rtc.RTC_Seconds_IRQHandler��������
			{
				Deal_StandbyTime();
			}
			
			if(g_CurrentStat==State_Main_Menu &&LCD_Display_OnOff!=Display_OFF)  //�����ǰΪ��ҳ��
			{
				Updata_main_page_info(); //�������ݸ���
			}
			
			Scan_KeyValue();	 //���а���ɨ��
					
			//�ж�APP����������Ҫ����ͨ��APP_Interact_Flag��APP_CRC_Error_Flag������ʶ�����жϣ�
			//������ʶ����USB��BLE�Ĵ����ж��н��иı䣬�����APP������Ҫ����,�����Deal_APP_comm����
			if(APP_Interact_Flag ||APP_CRC_Error_Flag) 
			{
				Deal_APP_comm();  //����APP���͵�ָ�������USB��������������������
			}
			
   }
}







 //�����̨���Զ���������һֱ���������
void Deal_Auto_Measure(void) //�����̨���Զ�����
{
	if(Auto_Measure_TimeInterval_Count>(60*AutoMeasurePara.Timeinterval)) // �Զ�������ʱ������λΪ���ӣ��жϼ����ĵ�λΪ�룬���Գ���60
	{
		if(Auto_Measure_Total_Num_Count<=( AutoMeasurePara.Timelong*60/AutoMeasurePara.Timeinterval+1))  //�������������Զ���ʱʱ���ͼ���������ʱ��
		{
			if(g_CurrentStat==State_Main_Menu ||g_CurrentStat==State_Main_Menu_Manual ||g_CurrentStat==State_Main_Menu_Auto||g_CurrentStat==State_Main_Menu_Record||g_CurrentStat==State_Main_Menu_Set)  //�����ʱ�ѵ�����Ϊ�ɽ���ҳ��״̬
			{
				Auto_Measure_TimeInterval_Count=0; //����ǰ�������
				
			//////////////////������///////////////////
				if(LCD_Display_OnOff==Display_OFF)
				{
					LCD_Transfer_Command(0xaf); //����ʾ				
				}
				auto_measure_going();  //��ʾ��̨������
				/////////////////////////////////////////
				if(pw1101_func_auto_measure()==MEASURE_SUCCESS)  //ִ�к�̨����
				{
					Auto_Measure_Success_Count++;	
					Save_Measure_Result_To_Flash(RTC->TSR,measure_result_data);	 //����ɹ������в�������Ĵ洢
					alarm_function(); //���б�����ֵ�ıȽ�		
				}
				else
				{
					Auto_Measure_Fail_Count++;   //����ʧ�ܴ���ͳ��
				}				 
				Auto_Measure_Total_Num_Count++;	 //�ܵĲ�������ͳ��

				/////////////////////////////////
				if(LCD_Display_OnOff==Display_OFF)
				{
					LCD_Transfer_Command(0xae); //����ʾ				
				}
				First_in_Main_Menu=1;
				stateData[g_CurrentStat].DisBackFunc(g_CurrentStat);//���ؽ��� 
				/////////////////////////////////
			}
		}
	}
}



//===========================================================================================================================
//
void EnterSTOPMode()//��Ƭ������͹���ģʽ  ���� 2017-1-24
{
    /* unlock all VLP mode */
    SMC->PMPROT |= 0xFF;
    
    /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    //(enSleepOnExit)?(SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk):(SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk);
    
    /* WFI instruction will start entry into STOP mode */
    __ASM("WFI");
}



//===========================================================================================================================
void PORTB_IRQHandler(void)//�����жϷ�����  ���� 2017-1-24
{   
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_SET_PIN);  //���жϱ�־λ
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_RET_PIN);  //���жϱ�־λ
//		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<KEY_UD_PIN);  //���жϱ�־λ
		GPIO_ClearPinsInterruptFlags(GPIOB, 1U<<USB5_LINK_PIN);  //���жϱ�־λ
	
        BOARD_BootClockRUN();//��������ϵͳʱ��
       // SMC->PMPROT= 0x00;
     
		 DisableIRQ(PORTA_IRQn);//�ر��ж�   
		 DisableIRQ(PORTB_IRQn);//�ر��ж�
        LCD_VCC_ON;//����12.5��ѹ  ���� 
		
		gpio_pin_config_t IOout_config = {
			kGPIO_DigitalOutput, 0,
		};
	
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //���߿���VDDA
		GPIO_SetPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��
		
		LCD_Transfer_Command(0xaf); //����ʾ		
		
		LCD_Display_OnOff=Display_ON;
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}	
		bat_display=181;      //j��ʾ��־��180��ʾ3����  ����181�������е�����ˢ��			
		bat_charge_flag=3;
}

//===========================================================================================================================
void PORTA_IRQHandler(void)//�����жϷ�����  ���� 2017-1-24
{   
		GPIO_ClearPinsInterruptFlags(GPIOA, 1U<<KEY_POW_PIN);  //���жϱ�־λ
	
    		BOARD_BootClockRUN();//��������ϵͳʱ��
       // SMC->PMPROT= 0x00;  (�������õ�ʱ��ʱ��û�����õ�120MHZģʽ��������)
  
		 DisableIRQ(PORTA_IRQn);//�ر��ж�
		 DisableIRQ(PORTB_IRQn);//�ر��ж�
        LCD_VCC_ON;//����12.5��ѹ  ���� 
	
	
		GPIO_SetPinsOutput(GPIOB, 1u << 21);  //���߿���VDDA		
		GPIO_SetPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��
		
				
		LCD_Transfer_Command(0xaf); //����ʾ
		LCD_Display_OnOff=Display_ON;
			if(StandbyTime!=0)
			{
				StandbyTime_Count=0;
				StandbyTime_Flag=1;
			}		
		bat_display=181;      //j��ʾ��־��180��ʾ3����  ����181�������е�����ˢ��
		bat_charge_flag=3;
}
//===========================================================================================================================

//������ʱ���ʱ�ѵ������д˺���ִ��
void Deal_StandbyTime(void)    
{
	if(g_CurrentStat==State_Main_Menu ||g_CurrentStat==State_Main_Menu_Manual ||g_CurrentStat==State_Main_Menu_Auto||g_CurrentStat==State_Main_Menu_Record||g_CurrentStat==State_Main_Menu_Set)
	{
		LCD_Transfer_Command(0xae); //����ʾ
		LCD_Display_OnOff=Display_OFF;
		StandbyTime_Flag=0;					
		StandbyTime_Count=0;
        LCD_VCC_OFF;//�ر�LCD����VCC 12.5V ��ѹ   
		
//        PORT_SetPinInterruptConfig(PORTB,KEY_SET_PIN, kPORT_InterruptFallingEdge);//����PTB2��Ϊ�ж�			
//				PORT_SetPinInterruptConfig(PORTB,KEY_RET_PIN, kPORT_InterruptFallingEdge);//����PTB0��Ϊ�ж�    //RES
//				PORT_SetPinInterruptConfig(PORTB,KEY_UD_PIN, kPORT_InterruptFallingEdge);//����PTB1��Ϊ�ж�    //UD
				PORT_SetPinInterruptConfig(PORTB,USB5_LINK_PIN, kPORT_InterruptRisingEdge);//����PTB23��Ϊ�ж�   //USB_LINK
        	EnableIRQ(PORTB_IRQn);//�����ж�       

		PORT_SetPinInterruptConfig(PORTA,KEY_POW_PIN, kPORT_InterruptFallingEdge);//����PTA29��Ϊ�ж�   //POWER
		//�����Ķ�����ͨ���жϻ��ѣ�

		EnableIRQ(PORTA_IRQn);//�����ж�    
        	EnterSTOPMode();  //muc�͹���ģʽ  ���� 2017-1-24
		
		GPIO_ClearPinsOutput(GPIOB, 1u << 21);  //���Ϳ���VDDA
		GPIO_ClearPinsOutput(GPIOD, 1u);  //����PW1101�ĸ�λ��    
		
	}
}


//������Ϣ
void Updata_main_page_info(void)
{
	unsigned int i=0;
	unsigned char data[5];
	Display_current_time();	
				
	if(GPIO_ReadPinInput(USB5_LINK_GPIO,USB5_LINK_PIN)) //����Ѿ���������
	{
		if(GPIO_ReadPinInput(BAT_FULL_GPIO,BAT_FULL_PIN) &&bat_charge_flag!=1)
		{
			bat_charge_flag=1;
			Display_bat_bmp(1);//����
		}
		else if(bat_charge_flag!=2 && bat_charge_flag!=1)
		{
			bat_charge_flag=2;
			Display_bat_bmp(4);//���
		}
		bat_num_flag=3;   //Ϊ��ֹ������ʾ��������һ���������־������ֻ���ٳ��ʱ�ɽ���������
		bat_display=0;
		First_in_Main_Menu=0;
	}
	else if(bat_charge_flag!=3)
	{
		bat_charge_flag=3;
		bat_display=181;      //j��ʾ��־��180��ʾ3����  ����181�������е�����ˢ��
		First_in_Main_Menu=1;
	}
	
	if(bat_display>30 || First_in_Main_Menu) //3���Ӹ���һ�µ��״̬
	{	
		bat_num=read_bat_adc();	
		if(bat_num>3576  &&  bat_num_flag>2)  //Լ����4.0V
		{
			Display_bat_bmp(1);//����
			bat_num_flag=3;   //��ʾ���ǵ�����
		}
		else if(bat_num>3258  && bat_num_flag>1) //����3.7V
		{
			Display_bat_bmp(2);//�е�
			bat_num_flag=2;   //��ʾ���е�
		}
		else if(bat_num>2921  && bat_num_flag>0) //����3.3V
		{
			Display_bat_bmp(3);//�͵�
			bat_num_flag=1;   //��ʾ���е�
		}
		else if(bat_num>2844) //����3.2V
		{
			//������ʾ 	
			Display_full_screen(Page_LowPower_Data);
			Delay_S(3);
			minute_updata_flag=61;
			clear_screen();
			bat_num_flag=0; 
			Display_bat_bmp(5);//�͵�
		}
		else
		{
			//�ػ�
			//USBת���ڵ�����IOҪ���ó���ͨIO�ſ��ԣ������޷��ػ�
			PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAsGpio);            /*����Ϊ��ͨio�� */  //����  2017-1��24��                   
			PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAsGpio);            /*����Ϊ��ͨio�� */
			
			
			Power_OnOff_Flag=POWER_OFF;
			GPIO_ClearPinsOutput(GPIOB, 1u << 22);	
		}
		bat_display=0;
		First_in_Main_Menu=0;
	}
}



//Appͨ��
void Deal_APP_comm(void)
{
		if(APP_Interact_Flag) //���Ϊ�棬��ʾAPP������������
		{
		 if(read_comm_from_USBqueue()) //USB�յ�����
		 {
			 if(USB_comm_cache[1]==0x50) //�����ǰָ��Ϊ
			 {					 
				 Updata_time(USB_COMM);  //����ʱ��
				 reply_APP(APP_RX_OK,USB_COMM); //���лظ�
			 }
			 else if(USB_comm_cache[1]==0x51) //�ϴ�����
			 {
				 Upload_data(USB_COMM);
			 }
			 else if(USB_comm_cache[1]==0x52) //��ʶ����ʾ��ʶ������Ϊ�Ѷ���ʵ���൱��������ݣ��´�ͬ������ʱ�����ϴ���
			 {
				 Clear_data(USB_COMM);
			 }
		 }
		 else if(read_comm_from_BLEqueue())  //�����յ�����
		 {
				if(BLE_comm_cache[1]==0x50) //�����ǰָ��Ϊ
			 {
				 Updata_time(BLE_COMM);
				 reply_APP(APP_RX_OK,BLE_COMM);   //������ʱ�����APPӦ��
			 }
			 else if(BLE_comm_cache[1]==0x51) //�ϴ�����
			 {
				 Upload_data(BLE_COMM);
			 }
			 else if(BLE_comm_cache[1]==0x52) //��ʶ����ʾ��ʶ������Ϊ�Ѷ���ʵ���൱��������ݣ��´�ͬ������ʱ�����ϴ���
			 {
				 Clear_data(BLE_COMM);   //�������
			 }
		 }
		 else
		 {
			 APP_Interact_Flag=0;  //�������
		 }
		}
		if(APP_CRC_Error_Flag)  //�յ�������������
		{
			reply_APP(APP_RX_ERROR,APP_CRC_Error_Flag);
		 APP_CRC_Error_Flag=0;
		}	

		if(StandbyTime!=0)
		{
			StandbyTime_Count=0;  //��ʱ���
			StandbyTime_Flag=1;   //�򿪼�ʱ��־
		}			
}

