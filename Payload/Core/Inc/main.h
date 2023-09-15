/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RADIO_NSS_Pin GPIO_PIN_13
#define RADIO_NSS_GPIO_Port GPIOC
#define T_MEAS_SEL_HUMITEMP_Pin GPIO_PIN_14
#define T_MEAS_SEL_HUMITEMP_GPIO_Port GPIOC
#define T_MEAS_SEL_MAINTEMP_Pin GPIO_PIN_15
#define T_MEAS_SEL_MAINTEMP_GPIO_Port GPIOC
#define INPUT_MEASURED_VALUE_COUNT_Pin GPIO_PIN_1
#define INPUT_MEASURED_VALUE_COUNT_GPIO_Port GPIOA
#define HUMI_MODE_SELECT_START_Pin GPIO_PIN_2
#define HUMI_MODE_SELECT_START_GPIO_Port GPIOA
#define T_MEAS_SEL0_Pin GPIO_PIN_3
#define T_MEAS_SEL0_GPIO_Port GPIOA
#define Batt_Volt_Pin GPIO_PIN_5
#define Batt_Volt_GPIO_Port GPIOA
#define EXT_PIN4_Pin GPIO_PIN_1
#define EXT_PIN4_GPIO_Port GPIOB
#define PRESSURE_NSS_Pin GPIO_PIN_2
#define PRESSURE_NSS_GPIO_Port GPIOB
#define TEMP_MODE_SELECT_START_Pin GPIO_PIN_12
#define TEMP_MODE_SELECT_START_GPIO_Port GPIOB
#define RCC_MCO_PRESS_Pin GPIO_PIN_8
#define RCC_MCO_PRESS_GPIO_Port GPIOA
#define HUMI_SEL0_Pin GPIO_PIN_3
#define HUMI_SEL0_GPIO_Port GPIOB
#define HUMI_SEL1_Pin GPIO_PIN_4
#define HUMI_SEL1_GPIO_Port GPIOB
#define HUMI_SEL2_Pin GPIO_PIN_5
#define HUMI_SEL2_GPIO_Port GPIOB
#define T_MEAS_SEL1_Pin GPIO_PIN_6
#define T_MEAS_SEL1_GPIO_Port GPIOB
#define GREEN_LED_Pin GPIO_PIN_7
#define GREEN_LED_GPIO_Port GPIOB
#define RED_LED_Pin GPIO_PIN_8
#define RED_LED_GPIO_Port GPIOB
#define HUMIDITY_HEATING_ON_Pin GPIO_PIN_9
#define HUMIDITY_HEATING_ON_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
