/*
 * radio.h
 *
 *  Created on: Jan 27, 2022
 *      Author: eswiech
 */

#ifndef INC_RADIO_H_
#define INC_RADIO_H_
#include "stm32f1xx_hal.h"
//#include "stm32f1xx_hal.h"

//#include <stm32f1xx_hal_def.h>
//#include <stm32f1xx_hal_spi.h>
//#include <stm32f1xx_hal_gpio.h>

#define RADIO_NSS_Pin GPIO_PIN_13
#define RADIO_NSS_GPIO_Port GPIOC

HAL_StatusTypeDef setupRadio(SPI_HandleTypeDef *hspiParam);
HAL_StatusTypeDef radio_write_register(const uint8_t register_addr, uint8_t value);
HAL_StatusTypeDef radio_read_register(const uint8_t register_addr, uint8_t *pData);
HAL_StatusTypeDef radio_set_tx_frequency(float freq_in_mhz);
HAL_StatusTypeDef radioTxData(uint8_t txData[],uint8_t len);
HAL_StatusTypeDef setCW();
HAL_StatusTypeDef setGFSK();
HAL_StatusTypeDef setPreamble();
HAL_StatusTypeDef clearFIFO();
HAL_StatusTypeDef clearRadio();
HAL_StatusTypeDef setRadio();



#endif /* INC_RADIO_H_ */
