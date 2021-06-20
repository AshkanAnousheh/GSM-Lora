/*
 * GSM_sim800c.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: Ashkan
 */

#include "GSM_sim800c.h"
#include "serialInterface.h"
#include "usbd_cdc_if.h"
#include "uart.h"
#include "ringbuf.h"
struct config_t{

	char serverIP[13];
	char serverPort[5];

}internetConfig;

uint8_t		tmpUartRxData[200];


GSM_sim800c::GSM_sim800c(){

//	char _ok_[]="OK";


	sprintf(internetConfig.serverIP ,"2.181.129.63");
	sprintf(internetConfig.serverPort , "1680");
}



state_t GSM_sim800c::serialTest()
{
	char tmpRespOfCmd[12] = "\r\n";

	if(sendCmd((char*)"AT\r\n",tmpRespOfCmd,(char*)"OK\r\n",_TIMEOUT_S)==_GSM_FAIL_)
	{
		disp((char*)"serial Error \r\n");
		return _GSM_FAIL_;
	}
	else
	{
		disp((char*)"serial Connection OK\r\n");
		return _GSM_SUCC_;
	}

}


// this method for display LOG

void GSM_sim800c::disp(char * display){

//	LOG(display);
	uint16_t len = strlen(display);
	CDC_Transmit_FS((uint8_t*)display,len);
}


// this method turn off echo

state_t GSM_sim800c::echoOff(){

	char tmpRespOfCmd[12] = "\r\n";

	if(sendCmd((char*)"ATE0\r\n",tmpRespOfCmd,(char*)"OK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"Echo Not OFF(NO RESPONSE)\r\n");
		return _GSM_FAIL_;
	}
	else
	{
		disp((char*)"Echo OFF\r\n");
		return _GSM_SUCC_;
	}

}

// this method check SIM register status on network

state_t GSM_sim800c::simReg(){

	char tmpRespOfCmd[20] = "+CREG:";
	si.free(digitBuffer,10);
	if(sendCmd((char*)"AT+CREG?\r\n",tmpRespOfCmd,(char*)"+CREG: %d,%d",2,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: SIM Register (NO Response)\r\n");
		return _GSM_FAIL_;
	}
	else
	{
		if((digitBuffer[0] == 0) & (digitBuffer[1] == 1))
		{
			disp((char*)"SIM Registered ..\r\n");
			si.free(digitBuffer,10);
			return _GSM_SUCC_;
		}
		HAL_Delay(1000);
		gsm.simReg();					// try again

	}
	return _GSM_FAIL_;

}



state_t GSM_sim800c::signalPower(){

	char tmp[30];
	char tmpRespOfCmd[20] = "+CSQ:";
	si.free(digitBuffer,10);
	if(sendCmd((char*)"AT+CSQ\r\n",tmpRespOfCmd,(char*)"+CSQ: %d,%d",2,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: Signal Error (NO RESPONSE)\r\n");
		return	_GSM_FAIL_;
	}
	else
	{
		int8_t	dbm = (2*digitBuffer[0])-113;
		sprintf(tmp,"RSSI: %i(dbm) -> %d / 31\r\n",dbm,digitBuffer[0]);
		disp(tmp);
		si.free(digitBuffer,10);
		return	_GSM_SUCC_;
	}



}

state_t GSM_sim800c::operatorName(){

	char tmp[30];
	char tmpRespOfCmd[20] = "+COPS:";
	si.free(digitBuffer,10);
	if(sendCmd((char*)"AT+COPS?\r\n",tmpRespOfCmd,(char*)"+COPS: %d,%d,\"%s",3,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"No Operator Name (NO RESPONSE)\r\n");
		return	_GSM_FAIL_;
	}
	else
	{
		char len = strlen(&digitBuffer[2]);
		digitBuffer[len+1] = 0;
		sprintf(tmp,"operator Name: %s\r\n",&digitBuffer[2]);
		disp(tmp);
		si.free(digitBuffer,10);
		return	_GSM_SUCC_;
	}


}


//=========================================================================================//


state_t GSM_sim800c::apn(){

	char tmpRespOfCmd[40] = "+CSTT:";

	if(sendCmd((char*)"AT+CSTT?\r\n",tmpRespOfCmd,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
			disp((char*)"GSM: APN ERROR (NO Response)\r\n");
			return _APN_ERR_;
	}
	else
	{
		char tmpRespOfCmd2[20] = "\r\n";

		if(sendCmd(_APN_,tmpRespOfCmd2,(char*)"OK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
		{
			disp((char*)"GSM: APN Set Error (NO Response)\r\n");
			return	_APN_ERR_;
		}
		else
		{
			disp((char*)"APN STATE OK..\r\n");
			return	_GPRS_READY_;
		}
	}



}

state_t GSM_sim800c::internetSetup(){

	char tmpRespOfCmd[10] = "\r\n";

	if(sendCmd((char*)"AT+CIICR\r\n",tmpRespOfCmd,(char*)"OK\r\n",_TIMEOUT_L) == _GSM_FAIL_)
	{
		disp((char*)"GSM: Internet Error (NO Response)\r\n");
		return	_INTERNET_ERR_;
	}
	else
	{
		disp((char*)"Internet SET SUCCESS...\r\n");
		return _GPRS_READY_;
	}


}

state_t GSM_sim800c::getIP(){

	char tmpRespOfCmd[25] = ".";

	if(sendCmd((char*)"AT+CIFSR\r\n",tmpRespOfCmd,(char*)"\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: IP ERROR\r\n");
		return _IP_ERR_;
	}
	else
	{
		disp(tmpRespOfCmd);
		return _GPRS_READY_;
	}

}

state_t GSM_sim800c::udpSetup(){

	char tmpRespOfCmd[30] = "CONNECT OK";
	char tmp[50];
	sprintf(tmp,"AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r\n",internetConfig.serverIP,internetConfig.serverPort);

	if(sendCmd(tmp,tmpRespOfCmd,(char*)"\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: SOCKET ERROR (NO Response)\r\n");
		return _UDP_CON_ERR_;
	}
	else
	{
		disp((char*)"UDP SET SECCESS...\r\n");
		return _GPRS_READY_;
	}


}

state_t GSM_sim800c::udpClose(){

	char tmpRespOfCmd[20] = "CLOSE OK";

	if(sendCmd((char*)"AT+CIPCLOSE\r\n",tmpRespOfCmd,(char*)"\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: CLOSE ERROR (NO Response)\r\n");
		return _UDP_CLOSE_ERR_;
	}
	else
	{
		disp((char*)"CLOSE SET SUCCESS...\r\n");
		return _GPRS_READY_;
	}

}


//=========================================================================================//


state_t GSM_sim800c::shut(){

	char tmpRespOfCmd[20] = "SHUT OK";

	if(sendCmd((char*)"AT+CIPSHUT\r\n",tmpRespOfCmd,(char*)"\r\n",_TIMEOUT_L) == _GSM_FAIL_)
	{
		disp((char*)"GSM: SHUT Error (NO Response)\r\n");
		return _SHUT_ERR_;
	}
	else
	{
		disp((char*)"SHUT SET SUCCESS...\r\n");
		return _GPRS_READY_;
	}

}

state_t GSM_sim800c::connect(){

	char tmpRespOfCmd[20] = "+CGATT:";
	si.free(digitBuffer,10);
	if(sendCmd((char*)"AT+CGATT?\r\n",tmpRespOfCmd,(char*)"+CGATT: %d",1,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		gsm.disp((char*)"GSM: Attach to net Error(NO Response)\r\n");
		return _ATCH_ERR_;
	}
	else
	{
		if(digitBuffer[0] == 1)
		{
			disp((char*)"GPRS STATE OK...\r\n");
			return _GPRS_READY_;
		}
		else
		{
			sendCmd((char*)"AT+CGATT=1\r\n",(char*)"\r\nOK\r\n",(char*)"",_TIMEOUT_S);
			gsm.connect();

		}
	}
}

state_t GSM_sim800c::disconnect(){

	char tmpRespOfCmd[12] = "\r\n";

	if(sendCmd((char*)"AT+CGATT=0\r\n",tmpRespOfCmd,(char*)"OK\r\n",_TIMEOUT_S) == _GSM_FAIL_)
	{
		disp((char*)"GSM: Disable Attach Error (NO Response)\r\n");
		return _DEATCH_ERR_;
	}
	else
	{
		disp((char*)"GPRS_OFF SET SUCCESS...\r\n");
		return _GPRS_READY_;
	}

}


//=========================================================================================//


state_t GSM_sim800c::gprsStatus(){

	char tmpRespOfCmd[60] = "STATE:";

	sendCmd((char*)"AT+CIPSTATUS\r\n",tmpRespOfCmd,(char*)"\r\n",_TIMEOUT_S);

	if(strstr(tmpRespOfCmd,(char*)"IP INITIAL"))
		return _APN_ERR_;
	else if(strstr(tmpRespOfCmd,(char*)"IP START"))
		return _INTERNET_ERR_;
	else if(strstr(tmpRespOfCmd,(char*)"IP CONFIG"))
		return _IP_ERR_;
	else if(strstr(tmpRespOfCmd,(char*)"IP GPRSACT"))
		return _IP_ERR_;
	else if(strstr(tmpRespOfCmd,(char*)"IP STATUS"))
		return _UDP_CON_ERR_;
	else if(strstr(tmpRespOfCmd,(char*)"CONNECT OK"))
		return _Ready_To_Send_Data;
	else if(strstr(tmpRespOfCmd,(char*)"PDP DEACT"))
		return _SHUT_ERR_;


		return _UNKNOWN_STATE_;

}


state_t GSM_sim800c::prepareGprs(){

	state_t	status = gprsStatus();

	switch (status)
	{
	case _SHUT_ERR_:
		gsm.shut();
	case _APN_ERR_:
		gsm.apn();
	case _INTERNET_ERR_:
		gsm.internetSetup();
	case _IP_ERR_:
		gsm.getIP();
	case _UDP_CON_ERR_:
		gsm.udpSetup();
	case _Ready_To_Send_Data:
		return _GPRS_READY_;

	break;
	default:
		return _UNKNOWN_STATE_;
	}

}

state_t GSM_sim800c::gsmInit(){


	gsm.serialTest();
	gsm.echoOff();
	gsm.simReg();
	gsm.signalPower();
	gsm.operatorName();
	gsm.connect();
	gsm.gprsInit();

	return _GSM_SUCC_;

}


state_t GSM_sim800c::gprsInit(){

	gsm.setGetRx();
	gsm.prepareGprs();


	return _GPRS_READY_;

}
state_t GSM_sim800c::prepareGprsOld(){

//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"CONNECT OK",50) == _GSM_SUCC_)
//			return _GPRS_READY_;
//		else
//		{
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"PDP DEACT",50) == _GSM_SUCC_)
//			gsm.shut();
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"IP INITIAL",50)== _GSM_SUCC_)
//			gsm.apn();
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"IP START",50) == _GSM_SUCC_)
//			gsm.internetSetup();
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"IP CONFIG",50) == _GSM_SUCC_)
//			gsm.getIP();
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"IP GPRSACT",50) == _GSM_SUCC_)
//			gsm.getIP();
//
//		if(sendCmd((char*)"AT+CIPSTATUS\r\n",(char*)"IP STATUS",50) == _GSM_SUCC_)
//			gsm.udpSetup();

//		ipHeaderOn();


//		}

//		return _GPRS_FAIL_;
}



//=========================================================================================//



state_t GSM_sim800c::ipHeaderOn(){

	si.free(digitBuffer,10);

	if(sendCmd((char*)"AT+CIPHEAD?\r\n",(char*)"+CIPHEAD:",(char*)"+CIPHEAD: %d",1,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GPRS_FAIL_)
	{
		if(sendCmd((char*)"AT+CIPHEAD=1\r\n",(char*)"\r\nOK\r\n",(char*)"",_TIMEOUT_S) == _GSM_SUCC_)
		{
			disp((char*)"IPHeader SET SUCCESS\r\n");
			return	_IPHDR_ERR_;
		}
		else
		{
			disp((char*)"IPHeader SET FAILED\r\n");
			return	_GPRS_FAIL_;
		}
	}
	else
	{

		if(digitBuffer[0] == 1)
		{
			disp((char*)"IPHdr State OK\r\n");
			si.free(digitBuffer,10);
		}
		else
		{
			sendCmd((char*)"AT+CIPHEAD=1\r\n",(char*)"\r\n",(char*)"OK\r\n",_TIMEOUT_S);
			ipHeaderOn();
		}
		return _GPRS_READY_;

	}


}


state_t GSM_sim800c::setGetRx(){

	si.free(digitBuffer,10);

	char tmpRespOfCmd[60] = "+CIPRXGET:";
	char tmpRespOfCmd2[10] = "\r\n";

	if(sendCmd((char*)"AT+CIPRXGET?\r\n",tmpRespOfCmd,(char*)"+CIPRXGET: %d",1,(char*)"\r\nOK\r\n",_TIMEOUT_S) == _GPRS_FAIL_)
	{

		disp((char*)"GSM: Manually GET RX ERROR (NO Response)\r\n");
		return	_IPHDR_ERR_;

	}
	else
	{

		if(digitBuffer[0] == 1)
		{
			disp((char*)"GETRX State OK\r\n");
			si.free(digitBuffer,10);
		}
		else
		{
			sendCmd((char*)"AT+CIPRXGET=1\r\n",tmpRespOfCmd2,(char*)"OK\r\n",_TIMEOUT_S);
			setGetRx();
		}
		return _GPRS_READY_;

	}


}



//=========================================================================================//


state_t GSM_sim800c::gprs_UDP_Rx(char * p,int *len){

	uint16_t	tlap = 0;
	uint16_t	tnow = HAL_GetTick();

	int			Len;
	char		i=0;
	char tmpRespOfCmd[60] = "+CIPRXGET:";

	si.free(digitBuffer,10);

	if(sendCmd((char*)"AT+CIPRXGET=2,127\r\n",tmpRespOfCmd,(char*)"+CIPRXGET: %d,%d,%d",3,(char*)"\r\n",_TIMEOUT_S) == _GSM_SUCC_)
	{

		*len = digitBuffer[1];
		Len = *len;
		int x=0;
		si.free(p,200);
	    	if(Len > 0)
	    	{

	    		while(!(x == Len + 2))
	    		{
	    			x=ringbuf_elements(&rb);

	    			tlap = HAL_GetTick() - tnow;

	    			if((tlap) > 5000)
	    				return _UDP_RX_FAIL_;


	    		}

	    		for(i=0 ; i<Len ; i++)
	    			*(p+i) =  ringbuf_get(&rb);

	    		while(ringbuf_get(&rb) != -1);

//	    		ringbuf_get(&rb);

	    		return	_GPRS_READY_;
	    	}


			return	_UDP_RX_FAIL_;
	}

	return	_GPRS_FAIL_;
}


state_t GSM_sim800c::gprs_UDP_Tx(char * p,uint8_t len){

	char tmp[40];

			if(prepareGprs() == _GPRS_READY_)
			{
				sprintf(tmp,"AT+CIPSEND=%d\r\n",len);
//				injaa nabayad montazere \r\n bemanad
				if(sendCmd(tmp,(char*)">",(char*)" ",_TIMEOUT_S) == _GSM_FAIL_)
				{
					return _UDP_TX_FAIL;
				}
				else
				{
					if(sendCmd(p,(char*)"SEND OK",(char*)"\r\n",_TIMEOUT_S) == _GSM_FAIL_)
					{
						gsm.disp((char*)"\r\n NOT SEND ! \r\n");
						return _UDP_TX_FAIL;
					}
					else
					{
						gsm.disp((char*)"\r\n **DATA SENT** \r\n");
						return _GPRS_READY_;
					}
				}
			}
			else
			{
				prepareGprs();
				return _UDP_TX_FAIL;
			}


}


//=========================================================================================//


state_t GSM_sim800c::getData(){


	char	*	ptr;
	ptr = 0;

		si.UART_read();

		ptr = strstr((char*)si.dataBuffer,(char*)"+IPD");

		if(ptr)
			return	_GPRS_READY_;
		else
			return _NO_DATA_RX_;



}



state_t GSM_sim800c::sendCmd(char * cmd,char * resp,char * termination,uint16_t timeout){

	HAL_Delay(10);

	si.UART_write(cmd);

	char	*	ptr;
	char	*	ptr2;

	ptr = 0;
	ptr2 = 0;
	uint16_t	tlap = 0;
	uint16_t	tnow = HAL_GetTick();

	int			dummy;
	char		i=0;


	si.free((char *)tmpUartRxData , 200);


	while(!ptr)
	{
		dummy = ringbuf_get(&rb);
		if(dummy != -1)
		{
			tmpUartRxData[i] = (uint8_t)dummy;
			i++;
			ptr = strstr((char*)tmpUartRxData,resp);
		}
		tlap = HAL_GetTick() - tnow;

		if((tlap) > timeout)
			return _GSM_FAIL_;

	}


	while(!ptr2)
	{
		dummy = ringbuf_get(&rb);
		if(dummy != -1)
		{
			tmpUartRxData[i] = (uint8_t)dummy;
			i++;
			ptr2 = strstr(ptr,termination);
		}
		tlap = HAL_GetTick() - tnow;

		if((tlap) > timeout)
			return _GSM_FAIL_;

	}


//	HAL_UART_Receive_IT(&huart2,(uint8_t *)&UartTxBuffer[0],1);

	memcpy(resp,tmpUartRxData,strlen((char*)tmpUartRxData));

	si.free((char *)tmpUartRxData , 200);

	return _GSM_SUCC_;
}




state_t GSM_sim800c::sendCmd(char * cmd,char * resp,char * format,char num,char * termination,uint16_t timeout){

	HAL_Delay(10);

	si.free(digitBuffer,10);

	while(ringbuf_get(&rb) != -1);
	/*
	 * Send Command to GSM :
	 */
	si.UART_write(cmd);

	/*
	 *  Pointers and variables :
	 */
	char	*	ptr;
	char	*	ptr2;

	ptr  = 0;
	ptr2 = 0;
	uint16_t	tlap = 0;
	uint16_t	tnow = HAL_GetTick();

	volatile int	dummy;
	char		i=0;

	/*
	 * initiate tmpUartRxData with zero :
	 */
	si.free((char *)tmpUartRxData , 200);


	while(!ptr)
	{
		dummy = ringbuf_get(&rb);
		if(dummy != -1)
		{
			tmpUartRxData[i] = (uint8_t)dummy;
			i++;
			ptr = strstr((char*)tmpUartRxData,resp);
		}
			tlap = HAL_GetTick() - tnow;

			if((tlap) > timeout)
				return _GSM_FAIL_;

	}


	while(!ptr2)
	{
		dummy = ringbuf_get(&rb);
		if(dummy != -1)
		{
			tmpUartRxData[i] = (uint8_t)dummy;
			i++;
			ptr2 = strstr(ptr,termination);
		}
			tlap = HAL_GetTick() - tnow;

			if((tlap) > timeout)
				return _GSM_FAIL_;

	}



//	HAL_UART_Receive_IT(&huart2,(uint8_t *)&UartTxBuffer[0],1);

	if(num == 1)
	sscanf(ptr,format,&digitBuffer[0]);

	if(num == 2)
	sscanf(ptr,format,&digitBuffer[0],&digitBuffer[1]);

	if(num == 3)
	sscanf(ptr,format,&digitBuffer[0],&digitBuffer[1],&digitBuffer[2]);

	memcpy(resp,tmpUartRxData,strlen((char*)tmpUartRxData));

	si.free((char *)tmpUartRxData , 200);
	return _GSM_SUCC_;


}



GSM_sim800c	gsm;
