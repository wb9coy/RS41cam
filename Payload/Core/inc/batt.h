/*
 * batt.h
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */

#ifndef INC_BATT_H_
#define INC_BATT_H_

#include "stm32f1xx_hal.h"
#include "ecc.h"

int processBattery(struct rscode_driver *rsDriver,ADC_HandleTypeDef* hadc);

#endif /* INC_BATT_H_ */
