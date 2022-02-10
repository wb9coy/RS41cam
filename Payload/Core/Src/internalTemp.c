/*
 * internalTemp.c
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */

#include <string.h>
#include "config.h"
#include "checksum.h"
#include "radio.h"
#include "packetDefs.h"
#include "internalTemp.h"
#include "stm32f1xx_hal.h"

int processInternalTemp(struct rscode_driver *rsDriver)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint8_t txBuf[MTU_SIZE];
	uint8_t adcInternalTemperatureVal;

	struct HABPacketIntTempInfoDataType HABPacketIntTempInfoData;

	memset(&HABPacketIntTempInfoData, '\0', sizeof(HABPacketIntTempInfoData));

	// Set for Deg F
	HAL_Status = radio_write_register(0x12, ADC_TEMP_CAL_REG12);
	HAL_Status = radio_write_register(0x13, ADC_TEMP_CAL_REG13);

	// Trigger ADC to capture.
	HAL_Status = radio_write_register(0x0f, 0x80);
	HAL_Delay(100);

	HAL_Status = radio_read_register(0x11, &adcInternalTemperatureVal);
	if(HAL_Status == HAL_OK)
	{
	  // Convert ADC value to signed temperature value. Deg F
	  HABPacketIntTempInfoData.intTempInfoData = (uint8_t)-60 + (int16_t)adcInternalTemperatureVal;
	  HABPacketIntTempInfoData.packetType  = INT_TEMP;
	  len = sizeof(HABPacketIntTempInfoData)-sizeof(HABPacketIntTempInfoData.crc16)-NPAR;
	  HABPacketIntTempInfoData.crc16 = crc_16((unsigned char *)&HABPacketIntTempInfoData,len);
	  rscode_encode(rsDriver, (unsigned char *)&HABPacketIntTempInfoData, sizeof(HABPacketIntTempInfoData)-NPAR, (unsigned char *)&HABPacketIntTempInfoData);
	  memcpy(txBuf,&HABPacketIntTempInfoData,sizeof(HABPacketIntTempInfoData));
	  HAL_Delay(300);
	  HAL_Status =  radioTxData(txBuf,sizeof(HABPacketIntTempInfoData));

	}

	return status;
}
