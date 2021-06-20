
#include "uart.h"





UART_HandleTypeDef huart3;



Gpio_t usart_tx;
Gpio_t usart_rx;





/* USART1 init function */  // trasmit only for debug
void UartInit(void)
{



	GpioInit(&usart_tx, USART1_Tx, PIN_ALTERNATE_FCT,PIN_PUSH_PULL,PIN_PULL_UP,GPIO_AF7_USART3);
	GpioInit(&usart_rx, USART1_Rx, PIN_ALTERNATE_FCT,PIN_PUSH_PULL,PIN_PULL_UP,GPIO_AF7_USART3);

	__HAL_RCC_USART3_CLK_ENABLE();

  huart3.Instance = USART3;
  huart3.Init.BaudRate = Buaderate;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    
  }

}

uint32_t LOG(char *buff , ...)
{
	char buffer[MaxLenBufLog];
	va_list valist;
	int x[MaxArgLog]={0};
	int j=0;
	uint32_t len;

	va_start(valist , MaxArgLog);
	for(j=0; j<MaxArgLog;j++)
	{
		x[j]=va_arg(valist , int);
	}
	sprintf(buffer, buff, x[0],x[1],x[2],x[3],x[4]);

	len=strlen(buffer);

	HAL_UART_Transmit(&huart3 ,(uint8_t *)buffer, len , 500 );


	va_end(valist);

	    return len;


}

