/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
PinsProfile:
- !!product 'Pins v2.0'
- !!processor 'MK24FN1M0xxx12'
- !!package 'MK24FN1M0VDC12'
- !!mcu_data 'ksdk2_0'
- !!processor_version '1.0.1'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitPins:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: E2, peripheral: SPI1, signal: SCK, pin_signal: ADC0_DP2/ADC1_SE6a/PTE2/LLWU_P1/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/TRACE_D2}
  - {pin_num: E4, peripheral: SPI1, signal: PCS1, pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT}
  - {pin_num: E3, peripheral: SPI1, signal: SOUT, pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN}
  - {pin_num: K9, peripheral: SPI0, signal: PCS0_SS, pin_signal: PTA14/SPI0_PCS0/UART0_TX/I2C2_SCL/I2S0_RX_BCLK/I2S0_TXD1}
  - {pin_num: L9, peripheral: SPI0, signal: SCK, pin_signal: PTA15/SPI0_SCK/UART0_RX/I2S0_RXD0}
  - {pin_num: J10, peripheral: SPI0, signal: SOUT, pin_signal: PTA16/SPI0_SOUT/UART0_CTS_b/UART0_COL_b/I2S0_RX_FS/I2S0_RXD1}
  - {pin_num: H10, peripheral: SPI0, signal: SIN, pin_signal: ADC1_SE17/PTA17/SPI0_SIN/UART0_RTS_b/I2S0_MCLK}
  - {pin_num: F1, peripheral: USB0, signal: DP, pin_signal: USB0_DP}
  - {pin_num: F2, peripheral: USB0, signal: DM, pin_signal: USB0_DM}
  - {pin_num: H7, peripheral: UART3, signal: TX, pin_signal: PTE4/LLWU_P2/SPI1_PCS0/UART3_TX/SDHC0_D3/TRACE_D0}
  - {pin_num: G4, peripheral: UART3, signal: RX, pin_signal: PTE5/SPI1_PCS2/UART3_RX/SDHC0_D2/FTM3_CH0}
  - {pin_num: F3, peripheral: GPIOE, signal: 'GPIO, 6', pin_signal: PTE6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/FTM3_CH1/USB_SOF_OUT}
  - {pin_num: C6, peripheral: GPIOC, signal: 'GPIO, 10', pin_signal: ADC1_SE6b/PTC10/I2C1_SCL/FTM3_CH6/I2S0_RX_FS/FB_AD5}
  - {pin_num: D6, peripheral: GPIOC, signal: 'GPIO, 9', pin_signal: ADC1_SE5b/CMP0_IN3/PTC9/FTM3_CH5/I2S0_RX_BCLK/FB_AD6/FTM2_FLT0}
  - {pin_num: A7, peripheral: GPIOC, signal: 'GPIO, 8', pin_signal: ADC1_SE4b/CMP0_IN2/PTC8/FTM3_CH4/I2S0_MCLK/FB_AD7}
  - {pin_num: B7, peripheral: GPIOC, signal: 'GPIO, 7', pin_signal: CMP0_IN1/PTC7/SPI0_SIN/USB_SOF_OUT/I2S0_RX_FS/FB_AD8}
  - {pin_num: J9, peripheral: GPIOA, signal: 'GPIO, 10', pin_signal: PTA10/FTM2_CH0/FTM2_QD_PHA/TRACE_D0}
  - {pin_num: J4, peripheral: GPIOA, signal: 'GPIO, 11', pin_signal: PTA11/FTM2_CH1/I2C2_SDA/FTM2_QD_PHB}
  - {pin_num: K8, peripheral: GPIOA, signal: 'GPIO, 12', pin_signal: CMP2_IN0/PTA12/CAN0_TX/FTM1_CH0/I2C2_SCL/I2S0_TXD0/FTM1_QD_PHA}
  - {pin_num: L8, peripheral: GPIOA, signal: 'GPIO, 13', pin_signal: CMP2_IN1/PTA13/LLWU_P4/CAN0_RX/FTM1_CH1/I2C2_SDA/I2S0_TX_FS/FTM1_QD_PHB}
  - {pin_num: B9, peripheral: ADC0, signal: 'SE, 14', pin_signal: ADC0_SE14/PTC0/SPI0_PCS4/PDB0_EXTRG/USB_SOF_OUT/FB_AD14/I2S0_TXD1}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port A Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB);													 /* Port B Clock Gate Control: Clock enabled */	
  CLOCK_EnableClock(kCLOCK_PortC);                           /* Port C Clock Gate Control: Clock enabled */	
	CLOCK_EnableClock(kCLOCK_PortD);													 /* Port D Clock Gate Control: Clock enabled */	
  CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */
	
	PORT_SetPinMux(PORTA, PIN5_IDX, kPORT_MuxAsGpio);         /* PORTA5 (pin K7) is configured as PTA5 */

  PORT_SetPinMux(PORTA, PIN10_IDX, kPORT_MuxAsGpio);         /* PORTA10 (pin J9) is configured as PTA10 */
  PORT_SetPinMux(PORTA, PIN11_IDX, kPORT_MuxAsGpio);         /* PORTA11 (pin J4) is configured as PTA11 */
  PORT_SetPinMux(PORTA, PIN12_IDX, kPORT_MuxAsGpio);         /* PORTA12 (pin K8) is configured as PTA12 */
  PORT_SetPinMux(PORTA, PIN13_IDX, kPORT_MuxAsGpio);         /* PORTA13 (pin L8) is configured as PTA13 */
  PORT_SetPinMux(PORTA, PIN14_IDX, kPORT_MuxAsGpio);         /* PORTA14 (pin K9) is configured as SPI0_PCS0 */
  PORT_SetPinMux(PORTA, PIN15_IDX, kPORT_MuxAlt2);           /* PORTA15 (pin L9) is configured as SPI0_SCK */
  PORT_SetPinMux(PORTA, PIN16_IDX, kPORT_MuxAlt2);           /* PORTA16 (pin J10) is configured as SPI0_SOUT */
  PORT_SetPinMux(PORTA, PIN17_IDX, kPORT_MuxAlt2);           /* PORTA17 (pin H10) is configured as SPI0_SIN */
  PORT_SetPinMux(PORTC, PIN0_IDX, kPORT_PinDisabledOrAnalog); /* PORTC0 (pin B9) is configured as ADC0_SE14 */
  PORT_SetPinMux(PORTC, PIN10_IDX, kPORT_MuxAsGpio);         /* PORTC10 (pin C6) is configured as PTC10 */
  PORT_SetPinMux(PORTC, PIN7_IDX, kPORT_MuxAsGpio);          /* PORTC7 (pin B7) is configured as PTC7 */
  PORT_SetPinMux(PORTC, PIN8_IDX, kPORT_MuxAsGpio);          /* PORTC8 (pin A7) is configured as PTC8 */
  PORT_SetPinMux(PORTC, PIN9_IDX, kPORT_MuxAsGpio);          /* PORTC9 (pin D6) is configured as PTC9 */
	
	PORT_SetPinMux(PORTC, PIN0_IDX, kPORT_PinDisabledOrAnalog); /* PORTC0 (pin B9) is configured as ADC0_SE14 */
	
  PORT_SetPinMux(PORTE, PIN0_IDX, kPORT_MuxAlt2);            /* PORTE0 (pin E4) is configured as SPI1_PCS1 */
  PORT_SetPinMux(PORTE, PIN1_IDX, kPORT_MuxAlt2);            /* PORTE1 (pin E3) is configured as SPI1_SOUT */
  PORT_SetPinMux(PORTE, PIN2_IDX, kPORT_MuxAlt2);            /* PORTE2 (pin E2) is configured as SPI1_SCK */
  PORT_SetPinMux(PORTE, PIN4_IDX, kPORT_MuxAlt3);            /* PORTE4 (pin H7) is configured as UART3_TX */
  PORT_SetPinMux(PORTE, PIN5_IDX, kPORT_MuxAlt3);            /* PORTE5 (pin G4) is configured as UART3_RX */
	
	PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAlt3);            /* PORTC14 (pin A5) is configured as UART4_RX */
  PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAlt3);            /* PORTC15 (pin B5) is configured as UART4_TX */
	
	PORT_SetPinMux(PORTD, PIN2_IDX, kPORT_MuxAlt3);            /* PORTD2 (pin C3) is configured as UART2_RX */
  PORT_SetPinMux(PORTD, PIN3_IDX, kPORT_MuxAlt3);            /* PORTD3 (pin B3) is configured as UART2_TX */
	PORT_SetPinMux(PORTD, PIN0_IDX, kPORT_MuxAsGpio);					 /* PORTD0 (pin H11) is configured as PTD0*/
	
  PORT_SetPinMux(PORTE, PIN6_IDX, kPORT_MuxAsGpio);          /* PORTE6 (pin F3) is configured as PTE6 */
	
	//////////////定义四个按键////////////////////////
	PORT_SetPinMux(PORTB,PIN0_IDX,kPORT_MuxAsGpio);            /* PORTB0 (pin G11) is configured as PTB0*/
	PORT_SetPinMux(PORTB,PIN1_IDX,kPORT_MuxAsGpio);            /* PORTB1 (pin G10) is configured as PTB1*/
	PORT_SetPinMux(PORTB,PIN2_IDX,kPORT_MuxAsGpio);            /* PORTB2 (pin G9) is configured as PTB2*/
	PORT_SetPinMux(PORTA,PIN29_IDX,kPORT_MuxAsGpio);            /* PORTA29 (pin H11) is configured as PTA29*/
	//////////////////////////////////////////////////
	PORT_SetPinMux(PORTB, PIN21_IDX, kPORT_MuxAsGpio);          /* PORTB22 (pin F9) is configured as PTB22 */
	PORT_SetPinMux(PORTB, PIN22_IDX, kPORT_MuxAsGpio);          /* PORTB22 (pin F8) is configured as PTB22 */
	PORT_SetPinMux(PORTB, PIN23_IDX, kPORT_MuxAsGpio);          /* PORTB23 (pin E8) is configured as PTB23 */
	
	PORT_SetPinMux(PORTD, PIN14_IDX, kPORT_MuxAsGpio);					//充电状态
	
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
