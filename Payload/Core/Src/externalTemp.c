#include <string.h>
#include "checksum.h"
#include "radio.h"
#include "packetDefs.h"
#include "externalTemp.h"
#include "stm32f1xx_hal.h"

int processExternalTemp(struct rscode_driver *rsDriver, float externalTemp)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];

	struct HABPacketExtTempInfoDataType HABPacketExtTempInfoData;

	memset(&HABPacketExtTempInfoData, '\0', sizeof(HABPacketExtTempInfoData));

	HABPacketExtTempInfoData.packetType  		= EXT_TEMP;
	HABPacketExtTempInfoData.extTempInfoData  	= externalTemp;

	len = sizeof(HABPacketExtTempInfoData)-sizeof(HABPacketExtTempInfoData.crc16)-NPAR;
	HABPacketExtTempInfoData.crc16 = crc_16((unsigned char *)&HABPacketExtTempInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketExtTempInfoData, sizeof(HABPacketExtTempInfoData)-NPAR, (unsigned char *)&HABPacketExtTempInfoData);
	memcpy(txBuf,&HABPacketExtTempInfoData,sizeof(HABPacketExtTempInfoData));;
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketExtTempInfoData));
	if(HAL_Status != HAL_OK)
	{

	}

	return status;
}
