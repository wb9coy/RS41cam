#include <string.h>
#include "checksum.h"
#include "radio.h"
#include "packetDefs.h"
#include "externalTemp.h"
#include "stm32f1xx_hal.h"
#include "config.h"
#include "main.h"

#ifdef EXTTEMPREFFREQ
int processExternalTemp(struct rscode_driver *rsDriver,float frequency)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];

	struct HABPacketExtTempInfoDataType HABPacketExtTempInfoData;

	memset(&HABPacketExtTempInfoData, '\0', sizeof(HABPacketExtTempInfoData));
	HABPacketExtTempInfoData.packetType  		= EXT_TEMP;

#ifdef CALEXTTEMP
	HABPacketExtTempInfoData.extTempInfoData = frequency;
#else
	float tempc = 0.0;
	float tempf = 0.0;
	tempc = ((EXTTEMPREFFREQ - frequency)/EXTTEMPDIVISION) - EXTTEMPOFFSET;
	tempf = (tempc * 1.8) + 32;
	HABPacketExtTempInfoData.extTempInfoData  	= tempf;
#endif

	len = sizeof(HABPacketExtTempInfoData)-sizeof(HABPacketExtTempInfoData.crc16)-NPAR;
	HABPacketExtTempInfoData.crc16 = crc_16((unsigned char *)&HABPacketExtTempInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketExtTempInfoData, sizeof(HABPacketExtTempInfoData)-NPAR, (unsigned char *)&HABPacketExtTempInfoData);
	memcpy(txBuf,&HABPacketExtTempInfoData,sizeof(HABPacketExtTempInfoData));
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketExtTempInfoData));
	HAL_Delay(PROTOCOL_DELAY);
	if(HAL_Status != HAL_OK)
	{
		status = 0;
	}

	return status;
}
#endif
