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

#include "board.h"

static RadioEvents_t RadioEvents;



const uint8_t MaxPayloadOfDatarate[] = { 59, 59, 59, 123, 250, 250, 250, 250 };

const uint8_t Datarates[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

const int8_t TxPower[]    = { 20, 14, 11,  8,  5,  2 };

rx_json_t RxPacket;

bool RadioCantSendPacket=0;
volatile bool IsReciveData=0;

void RadioInitMcu( void )
{
    RtcInit();
    SpiInit( &SX1276.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC );
    SX1276IoInit( );
}

/*
 * this function occur when Radio successfully transmit
 */
void OnTxDown(void){
	#ifdef DebugRadio

	#endif
	RadioGoToRxMode();

}

/*
 * this function occur when Radio recive packet
 */
void OnRxDown ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
	IsReciveData=1;
	RxPacket.freq=(float)(Freq_On_Rx_Mode /1000000);
	RxPacket.stat=1;
	memcpy(RxPacket.modu,"LORA",4);
	switch (Datarate_On_Rx_Mode)
	{
		case DR_0:
		{
			memcpy(RxPacket.datr,"SF12BW125",9);
			break;
		}
		case DR_1:
		{
			memcpy(RxPacket.datr,"SF11BW125",9);
			break;
		}
		case DR_2:
		{
			memcpy(RxPacket.datr,"SF10BW125",9);
			break;
		}
		case DR_3:
		{
			memcpy(RxPacket.datr,"SF9BW125",8);
			break;
		}
		case DR_4:
		{
			memcpy(RxPacket.datr,"SF8BW125",8);
			break;
		}
		case DR_5:
		{
			memcpy(RxPacket.datr,"SF7BW125",8);
			break;
		}

	}
	switch (CodeRate_On_Rx_Mode)
	{
		case 1:
		{
			memcpy(RxPacket.codr , "4/5",3);
			break;
		}
		case 2:
		{
			memcpy(RxPacket.codr , "4/6",3);
			break;
		}
		case 3:
		{
			memcpy(RxPacket.codr , "4/7",3);
			break;
		}
		case 4:
		{
			memcpy(RxPacket.codr , "4/8",3);
			break;
		}
	}
	RxPacket.rssi=rssi;
	RxPacket.lsnr=(float)snr;
	RxPacket.size=size;
	memcpy(RxPacket.data,payload,size);
	RadioGoToRxMode();
}

void OnRxTimeOut(void)
{
	/*
	 * LOG("RxTimout ")
	 * this event dont occur , for ensure log this
	 */
	RadioGoToRxMode();
}

/*
 * this function occur when Radio cant transmit data on air in (timeout =3e3 ms)
 */
void OnTxTimeOut(void)
{
	RadioGoToRxMode();
	RadioCantSendPacket =1;
}

/*
 * this function occur when CRC error
 */
void OnRxError(void)
{
/*
 * in this function just LOG status = crc error
 */
	RadioGoToRxMode();

}

void RadioGoToRxMode(void)
{

	if(Radio.GetStatus == RF_IDLE)
	{
		Radio.Sleep(); // for write register
		Radio.SetChannel(Freq_On_Rx_Mode);
		Radio.SetRxConfig(MODEM_LORA, BW_On_Rx_Mode , Datarates[Datarate_On_Rx_Mode] , CodeRate_On_Rx_Mode,0 , PreambleLengh_On_Rx_Mode,SymbTimeout_On_Rx_Mode,0,0,CRCEnable_On_Rx_Mode,0,0,IqInverted_On_Rx_Mode,1 );
		Radio.SetMaxPayloadLength( MODEM_LORA, MaxPayloadOfDatarate[Datarate_On_Rx_Mode] + LORA_MAC_FRMPAYLOAD_OVERHEAD );
		Radio.Rx(0);
	}
}


void RadioGoTxMode(PckParamForSendRadio_t *obj)
{
	#ifdef GiveTxParameterOnServer
		Radio.Sleep();	// for write register
		Radio.SetChannel(obj->freq);
		Radio.SetMaxPayloadLength(obj->modem,obj->packetsize);
		Radio.SetTxConfig( obj->modem , obj->txpower ,0 ,obj->BW ,obj->datarate ,obj->coderate ,obj->preambleLen ,0 ,CRCEnable_On_Tx_Mode,0,0,IqInverted_On_Tx_Mode ,3e3 );
		Radio.Send(obj->packet , obj->packetsize);
	#else
		Radio.Sleep();	// for write register
		Radio.SetChannel(Freq_On_Tx_Mode);
		Radio.SetMaxPayloadLength(MODEM_LORA,obj->packetsize);
		Radio.SetTxConfig( MODEM_LORA , TxPower[Power_On_Tx_Mode] ,0 ,BW_On_Tx_Mode ,Datarates[Datarate_On_Tx_Mode] ,CodeRate_On_Tx_Mode ,PreambleLengh_On_Tx_Mode ,0 ,CRCEnable_On_Tx_Mode,0,0,IqInverted_On_Tx_Mode ,3e3 );
		Radio.Send(obj->packet , obj->packetsize);
	#endif
}

void RadioInit(void)
{

	RadioEvents.RxDone=OnRxDown;
	RadioEvents.TxDone=OnTxDown;
	RadioEvents.RxTimeout=OnRxTimeOut;
	RadioEvents.TxTimeout=OnTxTimeOut;
	RadioEvents.RxError=OnRxError;
	Radio.Init(&RadioEvents);

	RadioGoToRxMode();

}
