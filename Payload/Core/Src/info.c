/*
 * info.c
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */

#include <string.h>
#include "config.h"
#include "checksum.h"
#include "radio.h"
#include "packetDefs.h"
#include "info.h"
#include "stm32f1xx_hal.h"

int processInfo(struct rscode_driver *rsDriver)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];
	struct HABPacketInfoDataType HABPacketInfoData;

	memset(&HABPacketInfoData, '\0', sizeof(HABPacketInfoData));
	HABPacketInfoData.packetType  = INFO_DATA;
	HABPacketInfoData.infoDataLen = strlen(VERSION_INFO);
	memcpy(HABPacketInfoData.infoData,VERSION_INFO,HABPacketInfoData.infoDataLen);
	len = sizeof(HABPacketInfoData)-sizeof(HABPacketInfoData.crc16)-NPAR;
	HABPacketInfoData.crc16 = crc_16((unsigned char *)&HABPacketInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketInfoData, sizeof(HABPacketInfoData)-NPAR, (unsigned char *)&HABPacketInfoData);
	memcpy(txBuf,&HABPacketInfoData,sizeof(HABPacketInfoData));
	HAL_Delay(300);
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketInfoData));
	if(HAL_Status != HAL_OK)
	{
	  status = 0;
	}

	return status;
}
