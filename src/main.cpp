/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "ringbuf.h"
#include "serialInterface.h"
#include "GSM_sim800c.h"

#include "prepareJSON.h"

#include "base64.h"
#include "board.h"

#include <inttypes.h>


/* ============================================================================= */

uint8_t 		UartTxBuffer[200];
char			tmpBuffer[20];
uint16_t		time;
uint32_t		delayBetweenCode;
uint8_t			payload_b64[350];

char			recievedDataFlag = 0;
char			udpRxBuffer[200];
int				udpRxLen;
char			tmpbuff[256];
int				lenOfData;
HAL_UART_StateTypeDef uartState;
uint32_t		uartError;
char			uartPermission = 1;
int				recieveCounter = 0;
/* ============================================================================== */

UART_HandleTypeDef		huart2;
RNG_HandleTypeDef		hrng;
TimerEvent_t			mytim2;
TIM_HandleTypeDef		htim2;

Gpio_t					ledpd14;

/* ============================================================================== */


void 	func(void);

void	SystemClock_Config(void);
void 	Error_Handler(void);

static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RNG_Init(void);
static void MX_TIM2_Init(void);


/* ============================================================================== */
void HAL_UART_TxCpltCallback (UART_HandleTypeDef *huart){


}


void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart){



		if (huart->Instance == USART2)	//current UART
		{

			ringbuf_put(&rb,inputChar);
			HAL_UART_Receive_IT(&huart2,(&inputChar),1);

		}

}


int main(void)
{

	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	/* Initialize UART2 for GSM */
	MX_USART2_UART_Init();

	/* Initialize USB CDC for Debug and Configure Gateway */
	MX_USB_DEVICE_Init();
	/* Initialize RNG Peripheral For Generate Random Numbers */
	MX_RNG_Init();
	/* Initialize TIMER2 (32bit) For Timestamp */
	MX_TIM2_Init();
	/* Initialize UART RX Interrupt  */
	HAL_UART_Receive_IT(&huart2,&inputChar,1);
	/* Insert Dummy Delay */
	HAL_Delay(3000);
	/* RUN Timer2 (timestamp) */
	HAL_TIM_Base_Start(&htim2);
	/* Initialize UART3 For LOG */
	UartInit();


	ringbuf_init(&rb,rxRingBuff,MAX_BUFF_SIZE);

//  RadioInitMcu();
//  RadioInit();
//  Radio.Sleep();

//  Radio.Write(0x0c,0x33);
//  int radiost = Radio.Read(0x0c);
//  sprintf(tmpBuffer,"%x",radiost);
//  gsm.disp((char*)"Hi STM32");

//  GpioInit( &ledpd14, PD_14, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
//  GpioWrite(&ledpd14,0);
  LOG((char*)"HI MOTE\r\n");

//  TimerInit(&mytim2,func);
//
//  TimerSetValue(&mytim2,3000);
//
//  TimerStart(&mytim2);

  gsm.gsmInit();

   while (1)
  {

//    	testPrepareJSON();

//    	sprintf(tmpBuffer,"%u",delayBetweenCode);
//    	CDC_Transmit_FS((uint8_t*)tmpBuffer,strlen((char*)tmpBuffer));

//    		HAL_Delay(10);
//    		si.UART_read();
//    		si.free();
//    		HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_14);
//    	}
//    	__HAL_UART_ENABLE_IT(&huart2,UART_FLAG_RXNE);
//    	gsm.gprs_UDP_Tx((char*)"UDPHUB-PULL",11);
//    	uartState = HAL_UART_GetError(&huart2);
//    	uartError = HAL_UART_GetError(&huart2);

    	gsm.gprs_UDP_Tx((char*)"UDPHUB-PULL",11);
    	HAL_Delay(1000);

    	gsm.gprs_UDP_Rx(udpRxBuffer,&udpRxLen);
    	HAL_Delay(1000);

		gsm.disp(udpRxBuffer);



  }

}

/** System Clock Configuration
*/
/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}




/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void func(void){
//static int i=0;
//char tttttt[50];
	TimerStop(&mytim2);
//   	GpioWrite(&ledpd14,1);
//   	HAL_Delay(1000);
//   	sprintf(tttttt,"OKOK______________RTC_%d\r\n",i++);
//   	si.UART_write(tttttt);
//   	GpioWrite(&ledpd14,0);
	HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_14);
	recievedDataFlag=1;
//	gsm.gprs_UDP_Tx((char*)"UDPHUB-PULL",11);
   	TimerStart(&mytim2);

}
/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

//  /* GPIO Ports Clock Enable */
//	  __HAL_RCC_GPIOC_CLK_ENABLE();
//	  __HAL_RCC_GPIOE_CLK_ENABLE();
//	  __HAL_RCC_GPIOF_CLK_ENABLE();
//
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_14;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);



}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void MX_TIM2_Init(void)
{



  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }



}


static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }

}
void Error_Handler(void)
{

}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
