/*
 * batt.c
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */
#include <string.h>
#include <math.h>
#include "config.h"
#include "packetDefs.h"
#include "checksum.h"
#include "radio.h"
#include "batt.h"
#include "config.h"

int processBattery(struct rscode_driver *rsDriver,ADC_HandleTypeDef* hadc)
{
	HAL_StatusTypeDef HAL_Status;
	int status   = 1;
	uint16_t len = 0;
	uint32_t adcVal;
	float    adcValFloat;
	uint8_t txBuf[MTU_SIZE];

	struct HABPacketBattInfoDataType HABPacketBattInfoData;

	//2032 = 3v
	//1785 = 2.7
	memset(&HABPacketBattInfoData, '\0', sizeof(HABPacketBattInfoData));

	HAL_ADC_Start(hadc);
	HAL_Status = HAL_ADC_PollForConversion(hadc,100);
	if(HAL_Status == HAL_OK)
	{
	  adcVal = HAL_ADC_GetValue(hadc);
	  HAL_ADC_Stop(hadc);
	  adcValFloat = (float)adcVal / 4095 * 2 * 3.3 * 1.1;
	  adcValFloat = adcValFloat - ADC_BAT_CAL;
	  HABPacketBattInfoData.battInfoData = round(adcValFloat*10)/10;
	  //HABPacketBattInfoData.battInfoData = round(adcValTemp * 10000.0)/10000.0;
	}

	HABPacketBattInfoData.packetType  = BATT_INFO;
	len = sizeof(HABPacketBattInfoData)-sizeof(HABPacketBattInfoData.crc16)-NPAR;
	HABPacketBattInfoData.crc16 = crc_16((unsigned char *)&HABPacketBattInfoData,len);
	rscode_encode(rsDriver, (unsigned char *)&HABPacketBattInfoData, sizeof(HABPacketBattInfoData)-NPAR, (unsigned char *)&HABPacketBattInfoData);
	memcpy(txBuf,&HABPacketBattInfoData,sizeof(HABPacketBattInfoData));
	HAL_Status =  radioTxData(txBuf,sizeof(HABPacketBattInfoData));
	HAL_Delay(PROTOCOL_DELAY);
	if(HAL_Status != HAL_OK)
	{
	status = 0;
	}

	  return status;
}
