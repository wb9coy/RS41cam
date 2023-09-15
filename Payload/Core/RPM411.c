/*
 * RPM411.c
 *
 *  Created on: Oct 6, 2022
 *      Author: gene
 */

#include <string.h>
#include "RPM411.h"
#include "main.h"

static SPI_HandleTypeDef *hspi;

void busyWait(int num)
{
	for(int i =0;i<num;i++)
	{

	}
}

RPM411StatusTypeDef RPM411Init(SPI_HandleTypeDef *hspiParam)
{
	RPM411StatusTypeDef RPM411Status = RPM411_OK;
	HAL_StatusTypeDef HAL_Status     = HAL_OK;

	uint8_t rsp[SIZE_OF_RPM411_PRESSURE_DATA];

	int initCmds[RPM_411_NUM_INIT_CMDS][RPM_411_SIZE_OF_INIT_CMD] = {
			{0x03,0x02,0x1E,0x00,0x00,0x51,0x00},
			{0x03,0x02,0x28,0x00,0x33,0xFE,0x00},
			{0x03,0x02,0x0A,0x00,0xB7,0x9E,0x00},
			{0x03,0x02,0x64,0x00,0x92,0xB6,0x00},
			{0x03,0x02,0x6E,0x00,0x59,0x59,0x00},
			{0x03,0x02,0x78,0x00,0x8c,0xF0,0x00},
			{0x03,0x02,0x82,0x00,0x86,0x0C,0x00},
			{0x03,0x02,0x8C,0x00,0x89,0x2F,0x00},
			{0x03,0x02,0x96,0x00,0x31,0xC3,0x00},
			{0x03,0x02,0xA0,0x00,0x02,0x6C,0x00},
			{0x03,0x02,0xAA,0x00,0xC9,0x83,0x00},
			{0x03,0x02,0xB4,0x00,0xB5,0xA3,0x00},
			{0x03,0x02,0xBE,0x00,0x7E,0x4C,0x00},
			{0x03,0x02,0xC8,0x00,0x81,0xEE,0x00},
			{0x03,0x02,0xD2,0x00,0x39,0x02,0x00},
			{0x03,0x02,0xDC,0x00,0x36,0x21,0x00},
			{0x03,0x02,0xE6,0x00,0x68,0xCB,0x00},
			{0x03,0x02,0xF0,0x00,0xBD,0x62,0x00},
			{0x03,0x02,0xFA,0x00,0x76,0x8D,0x00},
			{0x03,0x02,0x04,0x01,0x99,0xAD,0x00},
			{0x03,0x02,0x0E,0x01,0x52,0x42,0x00}
	};

	hspi = hspiParam;

	for(int i=0;i<RPM_411_NUM_INIT_CMDS;i++)
	{
		for(int j =0;j<RPM_411_SIZE_OF_INIT_CMD;j++)
		{
			HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);
			HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&initCmds[i][j], 1, 1000);
			if(HAL_Status != HAL_OK)
			{
				RPM411Status = RPM411_FAIL;
			}
			HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);
		}

		//425 us
		busyWait(300);
		//busyWait(900);

		for(int k =0;k<SIZE_OF_RPM411_PRESSURE_DATA;k++)
		{
			HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);
			HAL_Status = HAL_SPI_Receive(hspi, &rsp[k], 1, 1000);
			if(HAL_Status != HAL_OK)
			{
				RPM411Status = RPM411_FAIL;
			}
			HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);
			//busyWait(shapingNum);
		}

		HAL_Delay(50);
	}

	HAL_Delay(280);

	return RPM411Status;
}

RPM411StatusTypeDef RPM411ReadPressure(float *RPM411PressureData)
{
	RPM411StatusTypeDef RPM411Status = RPM411_OK;
	HAL_StatusTypeDef HAL_Status;
	uint8_t    pressureDataBuf[SIZE_OF_RPM411_PRESSURE_DATA];
	float      RPM411PressureDataTemp = 0.0;

	uint8_t cmd2[] = {0x01,0x00,0x3E,0x2E,0x00};
	uint8_t cmd3[] = {0x02,0x00,0x6D,0x7B,0x00};

	HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);

	for(int i =0;i<sizeof(cmd2);i++)
	{
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);
		HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&cmd2[i], 1, 1000);
		if(HAL_Status != HAL_OK)
		{
			RPM411Status = RPM411_FAIL;
		}
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);
		//busyWait(shapingNum);
	}
	HAL_Delay(193);

	for(int i =0;i<sizeof(cmd3);i++)
	{
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);
		HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&cmd3[i], 1, 1000);
		if(HAL_Status != HAL_OK)
		{
			RPM411Status = RPM411_FAIL;
		}
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);
		//busyWait(shapingNum);
	}

	//388us
	busyWait(275);
	//busyWait(825);

	for(int k =0;k<SIZE_OF_RPM411_PRESSURE_DATA;k++)
	{
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);
		HAL_Status = HAL_SPI_Receive(hspi, &pressureDataBuf[k], 1, 1000);
		HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);
	}
	if(pressureDataBuf[0] == 0xFF)
	{
		RPM411Status = RPM411_FAIL;
	}
	else
	{
		memcpy(&RPM411PressureDataTemp,&pressureDataBuf[19],4);
		*RPM411PressureData = RPM411PressureDataTemp;
	}


	return RPM411Status;
}
