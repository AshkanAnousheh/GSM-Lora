/*
 * serialInterface.h
 *
 *  Created on: Dec 6, 2016
 *      Author: Ashkan
 */

#ifndef SERIALINTERFACE_H_
#define SERIALINTERFACE_H_
#include "stm32f4xx_hal.h"

//									 define maximum size of UserTxBufferFS
#define Max_Data_buff			200
//									 define YOUR UART handler struct here for example huart1
#define UART_Handler_struct 	huart2

extern UART_HandleTypeDef UART_Handler_struct;
extern uint8_t UartTxBuffer[200];


class serialInterface {

public :

	uint8_t 	bufRead;

	uint8_t 	bufNotRead;

	uint8_t 	dataBuffer[Max_Data_buff];

	void 		UART_read();

	void		UART_write(char * TxBuffer);

	void	 	getString(char * str,char * ptr);

	void		freeRing();

	void		free();

	void 		free(char * buff,int len);

	void		getInteger(char * addr,char * str,int16_t * arrTmp);

	void	 	getTwoInteger(char * addr,char * str,int16_t * arr);

	serialInterface();


};

extern serialInterface	si;

#endif /* SERIALINTERFACE_H_ */
