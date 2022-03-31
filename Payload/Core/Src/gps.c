/*
 * gps.c
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */

#include <string.h>
#include "radio.h"
#include "config.h"
#include "gps.h"
#include "packetDefs.h"

GPS_StatusTypeDef processGPS(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef HAL_Status;
	GPS_StatusTypeDef gpsStatus  = GPS_OK;
	uint16_t uartRxLen;
	int indx = 0;
	uint8_t gpsSentence[GPS_UART_BUF_DATA_SIZE];
	uint8_t gpsUARTBuf[UART_DATA_SIZE];
	uint8_t txBuf[MTU_SIZE];

	static int sendGGA 		= 0;
	static int sendRMC 		= 0;
	static uint16_t ggaLen  = 0;
	static uint16_t rmcLen  = 0;
	static uint8_t GPGGASentence[GPS_UART_BUF_DATA_SIZE];
	static uint8_t GPRMCSentence[GPS_UART_BUF_DATA_SIZE];
	struct HABPacketGPSDataType HABPacketGPSData;


	HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, (uint8_t *)&gpsUARTBuf, UART_DATA_SIZE, &uartRxLen, 3000);
	HAL_Delay(100);
	if(HAL_Status == HAL_OK)
	{
	  for(int i=0;i<uartRxLen;i++)
	  {
		gpsSentence[indx] = gpsUARTBuf[i];
		indx++;
		if( (gpsUARTBuf[i] == '\n') | (indx > GPS_UART_BUF_DATA_SIZE-1) )
		{
		  if(gpsSentence[0] == '$')
		  {
			if(strncmp("GGA",(const char *)&gpsSentence[3],3) == 0)
			{
			  memset(GPGGASentence, '\0', GPS_UART_BUF_DATA_SIZE);
			  memcpy(GPGGASentence,gpsSentence,indx);
			  sendGGA = 1;
				 //if(sscanf(&GPGGASentence, "GPGGA,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c,%f", &time, &latitude, &ns, &longitude, &ew, &lock, &sats, &hdop, &alt, &unit, &geoid) >= 1)
			 }
			 if(strncmp("RMC",(const char *)&gpsSentence[3],3) == 0)
			 {
				 memset(GPRMCSentence, '\0', GPS_UART_BUF_DATA_SIZE);
				 memcpy(GPRMCSentence,gpsSentence,indx);
				 sendRMC = 1;
			 }
		 }
		 indx = 0;
		}
	  }
	}
	else
	{
	  HAL_Delay(1);
	}

	if(sendGGA == 1 && sendRMC ==1)
	{
		rmcLen = strlen((const char *)GPRMCSentence);
		ggaLen = strlen((const char *)GPGGASentence);

		HABPacketGPSData.packetType = GPS_GGA_1;
		if(ggaLen < GPS_BUF_DATA_SIZE)
		{
			HABPacketGPSData.gpsDataLen = ggaLen;
		}
		else
		{
			HABPacketGPSData.gpsDataLen = GPS_BUF_DATA_SIZE;
		}
		memcpy(HABPacketGPSData.gpsData,GPGGASentence,HABPacketGPSData.gpsDataLen);
		memcpy(txBuf,&HABPacketGPSData,sizeof(HABPacketGPSData));
		HAL_Status =  radioTxData(txBuf,MTU_SIZE);

		memset(HABPacketGPSData.gpsData, '\0', GPS_BUF_DATA_SIZE);
		if(ggaLen < GPS_BUF_DATA_SIZE)
		{
			HABPacketGPSData.gpsDataLen = 0;
		}
		else
		{
			HABPacketGPSData.gpsDataLen = ggaLen - GPS_BUF_DATA_SIZE;
			memcpy(HABPacketGPSData.gpsData,&GPGGASentence[GPS_BUF_DATA_SIZE],HABPacketGPSData.gpsDataLen);
		}

		HABPacketGPSData.packetType = GPS_GGA_2;
		memcpy(txBuf,&HABPacketGPSData,sizeof(HABPacketGPSData));
		HAL_Status =  radioTxData(txBuf,MTU_SIZE);
		///////////////////////////////////////////

		HABPacketGPSData.packetType = GPS_RMC_1;
		if(rmcLen < GPS_BUF_DATA_SIZE)
		{
			HABPacketGPSData.gpsDataLen = rmcLen;
		}
		else
		{
			HABPacketGPSData.gpsDataLen = GPS_BUF_DATA_SIZE;
		}
		memcpy(HABPacketGPSData.gpsData,GPRMCSentence,HABPacketGPSData.gpsDataLen);
		memcpy(txBuf,&HABPacketGPSData,sizeof(HABPacketGPSData));
		HAL_Status =  radioTxData(txBuf,MTU_SIZE);

		memset(HABPacketGPSData.gpsData, '\0', GPS_BUF_DATA_SIZE);
		if(ggaLen < GPS_BUF_DATA_SIZE)
		{
			HABPacketGPSData.gpsDataLen = 0;
		}
		else
		{
			HABPacketGPSData.gpsDataLen = rmcLen - GPS_BUF_DATA_SIZE;
			memcpy(HABPacketGPSData.gpsData,&GPRMCSentence[GPS_BUF_DATA_SIZE],HABPacketGPSData.gpsDataLen);
		}

		HABPacketGPSData.packetType = GPS_RMC_2;
		memcpy(txBuf,&HABPacketGPSData,sizeof(HABPacketGPSData));
		HAL_Status =  radioTxData(txBuf,MTU_SIZE);

		sendGGA 		= 0;
		sendRMC 		= 0;
	}

	return gpsStatus;
}

