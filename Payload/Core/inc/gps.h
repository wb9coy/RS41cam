/*
 * gps.h
 *
 *  Created on: Feb 5, 2022
 *      Author: eswiech
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

#include "stm32f1xx_hal.h"

#define GPS_UART_BUF_DATA_SIZE    82
#define UART_DATA_SIZE 		     300

typedef enum
{
  GPS_OK    	= 0x00U,
  GPS_FAIL	 	= 0x01U
} GPS_StatusTypeDef;

GPS_StatusTypeDef processGPS(UART_HandleTypeDef *huart);


#endif /* INC_GPS_H_ */
