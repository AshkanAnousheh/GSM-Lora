/*
 * prepareJSON.h
 *
 *  Created on: Dec 26, 2016
 *      Author: Ashkan
 */

#ifndef PREPAREJSON_H_
#define PREPAREJSON_H_

#include "board.h"

#include <inttypes.h>
#define PROTOCOL_VERSION 2
#define PKT_PUSH_DATA   0
#define PKT_PUSH_ACK    1
#define PKT_PULL_DATA   2
#define PKT_PULL_RESP   3
#define PKT_PULL_ACK    4

#define	Gateway_EUI	{0,1,2,3,3,2,1,0}


extern	char				databuf[500];
extern	RNG_HandleTypeDef	hrng;

extern	uint8_t				payload_b64[350];
extern	TIM_HandleTypeDef	htim2;

void	testPrepareJSON();

void	prepareJSONPacket(rx_json_t*);



#endif /* PREPAREJSON_H_ */
