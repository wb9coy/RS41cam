#include <string.h>
#include "checksum.h"
#include "radio.h"
#include "packetDefs.h"
#include "externalTemp.h"
#include "stm32f1xx_hal.h"
#include "config.h"
#include "main.h"

int processExternalTemp(struct rscode_driver *rsDriver,float frequency)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];

	float tempc = 0.0;
	float tempf = 0.0;

	struct HABPacketExtTempInfoDataType HABPacketExtTempInfoData;

	memset(&HABPacketExtTempInfoData, '\0', sizeof(HABPacketExtTempInfoData));

	tempc = (REFFREQ - frequency)/DIVISION;
	tempf = (tempc * 1.8) + 32;

	HABPacketExtTempInfoData.packetType  		= EXT_TEMP;
	HABPacketExtTempInfoData.extTempInfoData  	= tempf;

	len = sizeof(HABPacketExtTempInfoData)-sizeof(HABPacketExtTempInfoData.crc16)-NPAR;
	HABPacketExtTempInfoData.crc16 = crc_16((unsigned char *)&HABPacketExtTempInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketExtTempInfoData, sizeof(HABPacketExtTempInfoData)-NPAR, (unsigned char *)&HABPacketExtTempInfoData);
	memcpy(txBuf,&HABPacketExtTempInfoData,sizeof(HABPacketExtTempInfoData));;
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketExtTempInfoData));
	HAL_Delay(PROTOCOL_DELAY);
	if(HAL_Status != HAL_OK)
	{

	}

	return status;
}
