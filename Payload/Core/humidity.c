/*
 * humitity.c
 *
 *  Created on: Mar 27, 2022
 *      Author: eswiech
 */

#include <humidity.h>
#include <string.h>
#include "checksum.h"
#include "config.h"
#include "radio.h"
#include "packetDefs.h"
#include "stm32f1xx_hal.h"

int processHumidity(struct rscode_driver *rsDriver,float frequency)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];

	float humidity = 0.0;

	struct HABPacketHumidityInfoDataType HABPacketHumidityInfoData;

	memset(&HABPacketHumidityInfoData, '\0', sizeof(HABPacketHumidityInfoData));

	humidity = (frequency - HUMREFFREQ)/HUMDIVISION;
	humidity = 100.0 - humidity;

	HABPacketHumidityInfoData.packetType  		= HUM_INFO;
	HABPacketHumidityInfoData.humidityInfoData  = humidity;

	len = sizeof(HABPacketHumidityInfoData)-sizeof(HABPacketHumidityInfoData.crc16)-NPAR;
	HABPacketHumidityInfoData.crc16 = crc_16((unsigned char *)&HABPacketHumidityInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketHumidityInfoData, sizeof(HABPacketHumidityInfoData)-NPAR, (unsigned char *)&HABPacketHumidityInfoData);
	memcpy(txBuf,&HABPacketHumidityInfoData,sizeof(HABPacketHumidityInfoData));
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketHumidityInfoData));
	HAL_Delay(PROTOCOL_DELAY);
	if(HAL_Status != HAL_OK)
	{

	}

	return status;
}

