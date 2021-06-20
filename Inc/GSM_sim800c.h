/*
 * GSM_sim800c.h
 *
 *  Created on: Dec 6, 2016
 *      Author: Ashkan
 */

#ifndef GSM_SIM800C_H_
#define GSM_SIM800C_H_
#include "stm32f4xx_hal.h"

#define _TCP_IP_		(char*)"www.google.com"
#define _TCP_PORT_		(char*)"80"
#define _APN_TCI_		(char*)"AT+CSTT=\"mcinet\"\r\n"
#define	_APN_MTN_		(char*)"AT+CSTT=\"mtnirancell\"\r\n"
#define	_APN_			_APN_TCI_
#define	_APN_NAME_MTN_	(char*)"mtnirancell"
#define	_APN_NAME_TCI_	(char*)"mcinet"

#define	_TIMEOUT_S		20000
#define	_TIMEOUT_L		20000


extern	int				recieveCounter;
typedef enum
{
	_GSM_FAIL_,
	_GSM_SUCC_,
	_GPRS_FAIL_,
	_GPRS_READY_,
	_APN_ERR_,
	_INTERNET_ERR_,
	_IP_ERR_,
	_UDP_CON_ERR_,
	_UDP_CLOSE_ERR_,
	_SHUT_ERR_,
	_ATCH_ERR_,
	_DEATCH_ERR_,
	_IPHDR_ERR_,
	_Ready_To_Send_Data,
	_UDP_TX_FAIL,
	_UDP_RX_FAIL_,
	_NO_DATA_RX_,
	_UNKNOWN_STATE_
}state_t;


class GSM_sim800c {

public :

	state_t 	gsmInit(void);

	state_t 	prepareGprsOld(void);

	state_t		prepareGprs();

	state_t 	gprs_UDP_Tx(char * p,uint8_t len);

	state_t 	gprs_UDP_Rx(char * p,int *len);

	state_t 	gprsStatus();

	state_t		signalPower();

	state_t		getIP();

	state_t		serverStatus();

	state_t		serialTest();

	state_t		echoOff();

	void		disp(char * display);

	state_t		getData();

	state_t		sendCmd(char * cmd,char * resp,char * termination,uint16_t timeout);

	state_t		sendCmd(char * cmd,char * resp,char * format,char num,char * termination,uint16_t timeout);

	state_t		apn();

	state_t		simReg();

	state_t		internetSetup();

	state_t		ipHeaderOn();

	state_t		udpSetup();

	state_t		setGetRx();

	char		digitBuffer[10];

	state_t		operatorName();

	state_t 	connect();

	state_t		disconnect();

	state_t		shut();

	state_t		udpClose();

	state_t		gprsInit();

	GSM_sim800c();

};

extern GSM_sim800c	gsm;
#endif /* GSM_SIM800C_H_ */
