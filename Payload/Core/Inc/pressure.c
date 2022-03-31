/*
 * pressure.c
 *
 *  Created on: Mar 27, 2022
 *      Author: eswiech
 */

#include <string.h>
#include "checksum.h"
#include "radio.h"
#include "config.h"
#include "packetDefs.h"
#include "pressure.h"
#include "stm32f1xx_hal.h"

int processPressure(struct rscode_driver *rsDriver, float pressureParm)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];

	struct HABPacketPressureInfoDataType HABPacketPressureInfoData;

	memset(&HABPacketPressureInfoData, '\0', sizeof(HABPacketPressureInfoData));

	HABPacketPressureInfoData.packetType  		= PRESS_INFO;
	HABPacketPressureInfoData.pressureInfoData  = pressureParm;

	len = sizeof(HABPacketPressureInfoData)-sizeof(HABPacketPressureInfoData.crc16)-NPAR;
	HABPacketPressureInfoData.crc16 = crc_16((unsigned char *)&HABPacketPressureInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketPressureInfoData, sizeof(HABPacketPressureInfoData)-NPAR, (unsigned char *)&HABPacketPressureInfoData);
	memcpy(txBuf,&HABPacketPressureInfoData,sizeof(HABPacketPressureInfoData));
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketPressureInfoData));
	if(HAL_Status != HAL_OK)
	{

	}

	return status;
}

