/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif



#define STM32F4

#define LoraMacDebug

//#define TimerDebug
//#define USE_DEBUGGER




#ifdef STM32F4
#include "stm32f4xx_hal.h"
#elif defined STM32F3
#include "stm32f3xx_hal.h"
#elif defined STM32F2
#include "stm32f2xx_hal.h"
#elif defined STM32F1
#include "stm32f1xx_hal.h"
#elif defined STM32F0
#include "stm32f0xx_hal.h"
#endif



#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


//#include "usbd_cdc_if.h"
#include "uart.h"
#include "timer.h"

#include "utilities.h"
#include "rtc_board.h"

#include "gpio.h"
#include "gpio-board.h"
#include "spi-board.h"
#include "spi.h"

#include "sx1276-board.h"
#include "sx1276.h"
#include "radio.h"




/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0
#endif

/*
 * for debug
 */
#define DebugRadio


/*!
 * Board MCU pins definitions
 */

#define RADIO_RESET                                 PB_14

#define RADIO_MOSI                                  PB_15
#define RADIO_MISO                                  PC_2
#define RADIO_SCLK                                  PB_13
#define RADIO_NSS                                   PC_4

#define RADIO_DIO_0                                 PE_11
#define RADIO_DIO_1                                 PE_12
#define RADIO_DIO_2                                 PE_13
#define RADIO_DIO_3                                 PE_14
//#define RADIO_DIO_4                                 PB_1
#define RADIO_DIO_5                                 PE_15

#define RADIO_ANT_SWITCH_HF                         PE_7  //Tx in my radio
#define RADIO_ANT_SWITCH_LF                         PE_8	// Rx in my radio
#define USART1_Tx									PD_8
#define USART1_Rx									PB_11

//#define OSC_LSE_IN                                  PC_14
//#define OSC_LSE_OUT                                 PC_15

//#define OSC_HSE_IN                                  PH_0
//#define OSC_HSE_OUT                                 PH_1

typedef struct PckParamForSendRadio_s
{
	uint32_t freq;
	RadioModems_t modem;
	uint8_t txpower;
	uint8_t BW;
	uint8_t datarate;
	uint8_t coderate;
	uint8_t preambleLen;
	uint16_t packetsize;
	uint8_t *packet;
}	PckParamForSendRadio_t;

typedef  struct  rx_JSON
{

  char    	time[30];
  uint32_t  tmst;
  uint8_t   chan;
  uint16_t  rfch;
  float    	freq;
  uint8_t   stat;
  char    	modu[5];
  char    	datr[15];
  char    	codr[5];
  int16_t   rssi;
  float    	lsnr;
  uint16_t  size;
  uint8_t   data[255];

}rx_json_t;

extern rx_json_t RxPacket;
extern bool RadioCantSendPacket;
extern volatile bool IsReciveData;
/*!
 * LoRaMac TxPower definition
 */

#define TX_POWER_10_DBM                             0
#define TX_POWER_07_DBM                             1
#define TX_POWER_04_DBM                             2
#define TX_POWER_01_DBM                             3
#define TX_POWER_M2_DBM                             4
#define TX_POWER_M5_DBM                             5

/*!
 * LoRaMac datarates definition
 */

#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK


/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */




#define LORA_MAC_FRMPAYLOAD_OVERHEAD	13
/*
 * if we want get Tx parameter on server this define should be uncomment
 * otherwise when we want setting this parameter in manual method comment this define
 */
#define GiveTxParameterOnServer


/*
 * adjust Rx parameter for Radio
 */

#define Freq_On_Rx_Mode			433375000
#define Datarate_On_Rx_Mode		DR_0
#define CodeRate_On_Rx_Mode		1
#define BW_On_Rx_Mode			0

#define SymbTimeout_On_Rx_Mode	5 /* DR_0 & DR_1 & DR_2 -->5
 	 	 	 	 	 	 	 	 	 DR_3 & DR_4--> 8
 	 	 	 	 	 	 	 	 	 DR_5 -->10 */
#define PreambleLengh_On_Rx_Mode	8
#define CRCEnable_On_Rx_Mode		0
#define IqInverted_On_Rx_Mode		1


/*
 * brief 	adjust parameter in Tx Mode for when that we want dont give this
 * 			parameter of server and we adjust then manual
 */

#define Freq_On_Tx_Mode				433375000
#define BW_On_Tx_Mode				0
#define	CRCEnable_On_Tx_Mode		1
#define IqInverted_On_Tx_Mode		0
#define Power_On_Tx_Mode			TX_POWER_10_DBM
#define Datarate_On_Tx_Mode			DR_0
#define CodeRate_On_Tx_Mode			1
#define PreambleLengh_On_Tx_Mode	8







/*!
 * \brief Initializes the target board peripherals.
 */
void RadioInitMcu( void );

void OnTxDown(void);

void OnRxDown ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

void OnRxTimeOut(void);

void OnTxTimeOut(void);

void OnRxError(void);


void RadioGoToRxMode(void);

void RadioGoTxMode(PckParamForSendRadio_t*);

void RadioInit(void);

#ifdef __cplusplus
}
#endif


#endif // __BOARD_H__
