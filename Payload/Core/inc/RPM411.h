/*
 * RPM411.h
 *
 *  Created on: Oct 6, 2022
 *      Author: gene
 */

#ifndef INC_RPM411_H_
#define INC_RPM411_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stddef.h>

#define RPM_411_NUM_INIT_CMDS		 21
#define RPM_411_SIZE_OF_INIT_CMD	  7
#define SIZE_OF_RPM411_PRESSURE_DATA 33

typedef enum
{
  RPM411_OK    	= 0x00U,
  RPM411_FAIL	= 0x01U
} RPM411StatusTypeDef;

RPM411StatusTypeDef RPM411ReadPressure(float *RPM411PressureData);
RPM411StatusTypeDef RPM411Init(SPI_HandleTypeDef *hspiParam);


#endif /* INC_RPM411_H_ */
