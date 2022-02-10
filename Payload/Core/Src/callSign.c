/*
 * callSign.c
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */
#include <string.h>
#include "packetDefs.h"
#include "config.h"
#include "ecc.h"
#include "checksum.h"
#include "radio.h"
#include "cw.h"
#include "callSign.h"

int processCallSign(struct rscode_driver *rsDriver)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;

	uint8_t txBuf[MTU_SIZE];
	struct HABPacketCallSignDataType HABPacketCallSignData;

	memset(&HABPacketCallSignData, '\0', sizeof(HABPacketCallSignData));
	HABPacketCallSignData.packetType      = CW_ID;
	HABPacketCallSignData.callSignDataLen = strlen(CALL_SIGN);
	memcpy(HABPacketCallSignData.callSignData,CALL_SIGN,HABPacketCallSignData.callSignDataLen);
	len = sizeof(HABPacketCallSignData)-sizeof(HABPacketCallSignData.crc16)-NPAR;
	HABPacketCallSignData.crc16 = crc_16((unsigned char *)&HABPacketCallSignData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketCallSignData, sizeof(HABPacketCallSignData)-NPAR, (unsigned char *)&HABPacketCallSignData);
	memcpy(txBuf,&HABPacketCallSignData,sizeof(HABPacketCallSignData));
	HAL_Delay(300);
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketCallSignData));
	if(HAL_Status != HAL_OK)
	{
	  status = 0;
	}
	HAL_Status = setCW();
	if(HAL_Status != HAL_OK)
	{
	  status = 0;
	}
	sendCallCW(CALL_SIGN,strlen(CALL_SIGN));
	HAL_Delay(300);
	HAL_Status = setPreamble();
	if(HAL_Status != HAL_OK)
	{
	  status = 0;
	}
	HAL_Status = setGFSK();
	if(HAL_Status != HAL_OK)
	{
	  status = 0;
	}
	return status;
}

