#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"


#define Buaderate 115200


#define MaxArgLog 5    /// maximum argument  for LOG function --> can not be change this define
#define MaxLenBufLog 500 // maximum size for LOG buffer  -->can be change this define



extern UART_HandleTypeDef huart3;

/**<for Print in UART 1
 * PARAM[IN] first --> string for print
 * PARAM[IN] else --> int variable for insert in string and print that
 *
 * PARAM[OUT] ---> lengh of made string
 * */
uint32_t LOG(char *buff , ...);


void UartInit(void);

#ifdef __cplusplus
}
#endif

#endif /* UART */
