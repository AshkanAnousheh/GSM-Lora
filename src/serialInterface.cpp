/*
 * serialInterface.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: Ashkan
 */

#include "serialInterface.h"
#include <string.h>
#include <stdio.h>
extern uint8_t UartTxBuffer[200];
extern char		uartPermission;
int16_t digit[2] = {0,0};

serialInterface::serialInterface(){

	bufRead = 0;
	bufNotRead = 0;


}

void serialInterface::UART_read(){


		  uint32_t buffptr;
		  uint32_t buffsize;

		  if(bufNotRead != bufRead)
		  {
		    if(bufRead > bufNotRead) /* Rollback */
		    {
		      buffsize = Max_Data_buff - bufRead;
		    }
		    else
		    {
		      buffsize = bufNotRead - bufRead;
		    }

		    buffptr = bufRead;

		    memcpy((uint8_t*)&dataBuffer[buffptr],(uint8_t*)&UartTxBuffer[buffptr], buffsize);

//		    si.freeRing();
//
		      bufRead += buffsize;
		      if (bufRead == Max_Data_buff)
		      {
		        bufRead = 0;
		      }


		  }


}

void serialInterface::freeRing(){

	for(int i=0;i<Max_Data_buff;i++) UartTxBuffer[i] = 0;
	bufNotRead = 0;
	bufRead = 0;
}

void serialInterface::free(){

	for(int i=0;i<Max_Data_buff;i++) dataBuffer[i] = 0;
	bufNotRead = 199;
	bufRead = 0;
}

void serialInterface::free(char * buff,int len){

	for(int i=0;i<len;i++) buff[i] = 0;
//	bufNotRead = bufRead = 0;
}

void serialInterface::UART_write(char * TxBuffer){

	uint8_t	len = strlen(TxBuffer);

		if(len != 0)
		HAL_UART_Transmit_IT(&UART_Handler_struct,(uint8_t*)TxBuffer,len);


}

void serialInterface::getString(char * str,char * ptr){

	UART_read();
	ptr = strstr((char*)dataBuffer,str);

}

void serialInterface::getInteger(char* addr,char* str,int16_t * digit){

	UART_read();
	sscanf(addr,str,digit);

}


void serialInterface::getTwoInteger(char* addr,char* str,int16_t * arr){

	UART_read();

	sscanf(addr,str,&arr[0],&arr[1]);

}

serialInterface	si;

