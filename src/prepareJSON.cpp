/*
 * prepareJSON.cpp
 *
 *  Created on: Dec 26, 2016
 *      Author: Ashkan
 */


#include "prepareJSON.h"
#include <string.h>
#include <stdio.h>
#include "base64.h"
#include "serialInterface.h"

char		databuf[500];
char		MAC[8] = Gateway_EUI;
uint32_t	rndNum;

void	prepareJSONPacket(rx_json_t	*json)
{

//	uint8_t	 	payload_bin[255];

	int			b64_dlen=0;
	int 		buff_index;
//	char		x;

	int i = 0;
	HAL_RNG_GenerateRandomNumber(&hrng,&rndNum);

	databuf[0] = PROTOCOL_VERSION;
	databuf[1] = (rndNum & 0x000000FF);
	databuf[2] = (rndNum & 0x0000FF00)>>8;
	databuf[3] = PKT_PUSH_DATA;

	buff_index = 4;

	memcpy((void *)(databuf + buff_index),MAC,8);
	buff_index += 8;

    /* start of JSON structure */
    memcpy((void *)(databuf + buff_index), (void *)"{\"rxpk\":[{", 10);
    buff_index += 10;

    i = snprintf((char *)(databuf + buff_index),30,(char *)"\"time\":\"%s\"",json->time);
    if ((i>=0) && (i < 30)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),20,(char *)",\"tmst\":%lu",json->tmst);
    if ((i>=0) && (i < 20)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),12,(char *)",\"chan\":%d",json->chan);
    if ((i>=0) && (i < 12)) {
        buff_index += i;
    }

    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"rfch\":%d",json->rfch);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),20,(char *)",\"freq\":%.6f",json->freq);
    if ((i>=0) && (i < 20)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),12,(char *)",\"stat\":%d",json->stat);
    if ((i>=0) && (i < 12)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"modu\":\"%s\"",json->modu);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),30,(char *)",\"datr\":\"%s\"",json->datr);
    if ((i>=0) && (i < 30)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"codr\":\"%s\"",json->codr);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"rssi\":%d",json->rssi);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"lsnr\":%.1f",json->lsnr);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    i = snprintf((char *)(databuf + buff_index),15,(char *)",\"size\":%d",json->size);
    if ((i>=0) && (i < 15)) {
        buff_index += i;
    }


    //--------------------------------------------------------------------------------------------//
    si.free((char*)payload_b64,350);
    while(base64_encode(payload_b64,(size_t*)&b64_dlen,json->data,255) != 0);

    memcpy((char *)(databuf + buff_index),(char *)",\"data\":\"",9);
    buff_index += 9;
    memcpy((char *)(databuf + buff_index),payload_b64,b64_dlen);
    buff_index += b64_dlen;
    memcpy((char *)(databuf + buff_index),(char *)"\"}]}",3);
    buff_index += 3;

    databuf[buff_index] = 0;



}

void	testPrepareJSON(){

	rx_json_t jtest;


	memcpy(jtest.time,"12/26/2016",10);
	jtest.tmst =  __HAL_TIM_GET_COUNTER(&htim2);
	jtest.chan = 9;
	jtest.rfch = 1;
	jtest.freq = 433.12;
	jtest.stat = 1;
	memcpy(jtest.modu,"LORA",4);
	memcpy(jtest.datr,"SF10BW125",9);
	jtest.rssi = -54;
	jtest.size = 16;
	si.free((char*)jtest.data,255);
	memcpy(jtest.data,"0000",5);

	prepareJSONPacket(&jtest);


}
