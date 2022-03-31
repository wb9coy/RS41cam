/*
 * radio.c
 *
 *  Created on: Jan 27, 2022
 *      Author: eswiech
 */
#include <string.h>
#include "Setup_Si4032.h"
#include "radio.h"
#include "led.h"
#include "config.h"
#include "packetDefs.h"
#include "main.h"

static SPI_HandleTypeDef *hspi;

HAL_StatusTypeDef setupRadio(SPI_HandleTypeDef *hspiParam)
{
	  HAL_StatusTypeDef HAL_Status;
	  hspi = hspiParam;

	  //Soft Reset
	  HAL_Status = radio_write_register(0x07, 0x80);
	  HAL_Delay(1000);

	  //Set Tx Power
	  HAL_Status = radio_write_register(0x6D, 00 | (TX_POWER & 0x0007));

	  HAL_Status = radio_set_tx_frequency(TRANSMIT_FREQUENCY);

	  //Data Rate 9.6kbps
	  HAL_Status = radio_write_register(0x6E, Si4032_TX_DATA_RATE_1);
	  HAL_Status = radio_write_register(0x6F, Si4032_TX_DATA_RATE_0);
	  HAL_Status = radio_write_register(0x70, Si4032_MODULATION_MODE_CONTROL_1);
	  //HAL_Status = radio_write_register(0x70, 0x20);

	  // 10KHz Dev
		HAL_Status = radio_write_register(0x72, Si4032_FREQUENCY_DEVIATION);

	  //Preamble
	  HAL_Status = setPreamble();

	  //Header Control 2  sync b11 Fix Packet Length no header 4 sync words
	  HAL_Status = radio_write_register(0x33, 0x0E);


	  HAL_Status = radio_write_register(0x36, 0x08);
	  HAL_Status = radio_write_register(0x37, 0x6D);
	  HAL_Status = radio_write_register(0x38, 0x53);
	  HAL_Status = radio_write_register(0x39, 0x88);

	  //enable the TX packet handler  CCITT
	  radio_write_register(0x30, 0x08);
	  //radio_write_register(0x30, 0x80); // no packet handling no CRC

	  // GFSK FIFO Mode FIFO Mode
	  HAL_Status = setGFSK();

	  //HAL_Status = radio_write_register(0x71, 0x63);

	 //write 0x04 to the Interrupt Enable 1 register
	 HAL_Status = radio_write_register(0x05, 0x04);
	 //write 0x00 to the Interrupt Enable 2 register
	 HAL_Status = radio_write_register(0x06, 0x00);

	 HAL_GPIO_WritePin(GPIOB, GREEN_LED_Pin, GPIO_PIN_RESET);

	 return HAL_Status;
}

HAL_StatusTypeDef clearFIFO()
{
	  HAL_StatusTypeDef HAL_Status;

	  HAL_Status = radio_write_register(0x08, 0x01);

	  return HAL_Status;
}

HAL_StatusTypeDef setCW()
{
	  HAL_StatusTypeDef HAL_Status;

	  HAL_Status = radio_write_register(0x71, 0x20);

	  return HAL_Status;
}

HAL_StatusTypeDef setGFSK()
{
	  HAL_StatusTypeDef HAL_Status;

	  // GFSK FIFO Mode FIFO Mode
	  HAL_Status = radio_write_register(0x71, Si4032_MODULATION_MODE_CONTROL_2);

	  return HAL_Status;
}

HAL_StatusTypeDef setPreamble()
{
	  HAL_StatusTypeDef HAL_Status;

	  // GFSK FIFO Mode FIFO Mode
	  //Preamble
	  HAL_Status = radio_write_register(0x34, 0x10);

	  return HAL_Status;
}

HAL_StatusTypeDef radio_read_register(const uint8_t register_addr, uint8_t *pData)
{
  HAL_StatusTypeDef HAL_Status;
  uint16_t data_word;
  uint8_t addr;
  uint8_t val;

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_RESET);

  data_word = ((register_addr | 0x0) << 8);
  addr = data_word >> 8;

  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
  HAL_Delay(1);
  HAL_Status = HAL_SPI_Receive(hspi, &val, 1, 1000);

  memcpy(pData,&val,1);

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_SET);

  return HAL_Status;
}

HAL_StatusTypeDef radio_write_register(const uint8_t register_addr, uint8_t value)
{
  HAL_StatusTypeDef HAL_Status;

  uint8_t WR = 0x80;
  uint16_t data_word;
  uint8_t addr;
  uint8_t val;

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_RESET);

  data_word = ((register_addr | WR) << 8) | value;
  addr = data_word >> 8;
  val  = (uint8_t)data_word;


  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
  HAL_Delay(1);
  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&val, 1, 1000);

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_SET);

  return HAL_Status;
}

HAL_StatusTypeDef radio_write_register_burst(const uint8_t register_addr, uint8_t txData[],uint8_t len)
{
  HAL_StatusTypeDef HAL_Status;

  uint8_t WR = 0x80;
  uint16_t data_word;
  uint8_t addr;
  uint8_t val;
  int i;

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_RESET);

  data_word = ((register_addr | WR) << 8) | txData[0];
  addr = data_word >> 8;
  val  = (uint8_t)data_word;


  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
  HAL_Delay(1);
  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&val, 1, 1000);

  for(i=1;i<len;i++)
  {
	  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&txData[i], 1, 1000);
  }

  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port, RADIO_NSS_Pin, GPIO_PIN_SET);

  return HAL_Status;
}

HAL_StatusTypeDef radio_set_tx_frequency(float freq_in_mhz)
{
  HAL_StatusTypeDef HAL_Status;
  float SI4032_CLOCK = 26.0;

  uint8_t hbsel = (uint8_t) ((freq_in_mhz * (30.0f / SI4032_CLOCK)) >= 480.0f ? 1 : 0);

  uint8_t fb = (uint8_t) ((((uint8_t)((freq_in_mhz * (30.0f / SI4032_CLOCK)) / 10) - 24) - (24 * hbsel)) / (1 + hbsel));
  uint8_t gen_div  =  3;  // constant - not possible to change!
  uint16_t fc = (uint16_t) (((freq_in_mhz / ((SI4032_CLOCK / gen_div) * (hbsel + 1))) - fb - 24) * 64000);
  HAL_Status = radio_write_register(0x75, (uint8_t) (0b01000000 | (fb & 0b11111) | ((hbsel & 0b1) << 5)));
  HAL_Status = radio_write_register(0x76, (uint8_t) (((uint16_t)fc >> 8) & 0xff));
  HAL_Status = radio_write_register(0x77, (uint8_t) ((uint16_t)fc & 0xff));

  HAL_Status = radio_write_register(0x73, Si4032_FREQUENCY_OFFSET_1);
  HAL_Status = radio_write_register(0x74, Si4032_FREQUENCY_OFFSET_2);
  HAL_Status = radio_write_register(0x7A, Si4032_FREQUENCY_HOPPING_STEP_SIZE);
  HAL_Status = radio_write_register(0x79, Si4032_FREQUENCY_HOPPING_CHANNEL);

  return HAL_Status;
}

HAL_StatusTypeDef radioTxData(uint8_t txData[],uint8_t len)
{
	HAL_StatusTypeDef HAL_Status;
	uint8_t  regData;

	HAL_Status = radio_write_register(0x3E, len);
	HAL_Status = radio_write_register_burst(0x7F, &txData[0],len);

	//read the Interrupt Status1 register
	HAL_Status =  radio_read_register(0x3,&regData);
	//read the Interrupt Status2 register
	HAL_Status =  radio_read_register(0x4,&regData);

	HAL_GPIO_WritePin(GPIOB, RED_LED_Pin, GPIO_PIN_RESET);
	HAL_Status = radio_write_register(0x07, Si4032_OPERATING_AND_FUNCTION_CONTROL_1);
	//HAL_Status = radio_write_register(0x07, 0x09);
	regData = 0;
	while((regData & 0x04) != 0x4)
	{
		//read the Interrupt Status1 register
		HAL_Status =  radio_read_register(0x3,&regData);
	}

	if(len != 1)
	{
		HAL_Delay(PROTOCOL_DELAY);
		if(len < MTU_SIZE)
		{
			HAL_Delay(PROTOCOL_DELAY*3);
		}
	}

	HAL_GPIO_WritePin(GPIOB, RED_LED_Pin, GPIO_PIN_SET);

	//read the Interrupt Status2 register
	HAL_Status =  radio_read_register(0x4,&regData);

	return HAL_Status;

}
