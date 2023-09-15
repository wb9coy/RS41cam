/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <ublox.h>
#include "cam.h"
#include "led.h"
#include "radio.h"
#include "callSign.h"
#include "info.h"
#include "batt.h"
#include "internalTemp.h"
#include "externalTemp.h"
#include "humidity.h"
#include "pressure.h"
#include "Setup_Si4032.h"
#include "RPM411.h"
#include "config.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int oneSecTic	    			= 0;

int resetSI4032			        = 0;
int sendGPSInfo					= 0;
int sendInfo					= 0;
int sendCallSignInfo			= 0;
int sendBattInfo    			= 0;
int sendInternalTemperatureInfo = 0;
int sendExternalTemperatureInfo = 0;
int sendHumidityInfo            = 0;
int sendPressureInfo         	= 0;

int tempClockEnabled           	= 0;
int tempClockStable           	= 0;

int humClockEnabled             = 0;
int humClockStable              = 0;

int pulseCount = 0;
int frequency  = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	pulseCount++;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  HAL_StatusTypeDef HAL_Status;

  GPS_StatusTypeDef gpsStatus	   = GPS_OK;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

	struct rscode_driver rsDriver;

	HAL_GPIO_WritePin(GPIOB, GREEN_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(4000); // For Cam POR
	HAL_GPIO_WritePin(GPIOB, RED_LED_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(HUMI_MODE_SELECT_START_GPIO_Port, HUMI_MODE_SELECT_START_Pin, GPIO_PIN_RESET); //Low
	HAL_GPIO_WritePin(HUMI_SEL0_GPIO_Port, HUMI_SEL0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HUMI_SEL1_GPIO_Port, HUMI_SEL1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HUMI_SEL2_GPIO_Port, HUMI_SEL2_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(TEMP_MODE_SELECT_START_GPIO_Port, TEMP_MODE_SELECT_START_Pin, GPIO_PIN_RESET); //Low
	HAL_GPIO_WritePin(T_MEAS_SEL0_GPIO_Port, T_MEAS_SEL0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(T_MEAS_SEL_HUMITEMP_GPIO_Port, T_MEAS_SEL_HUMITEMP_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(T_MEAS_SEL_MAINTEMP_GPIO_Port, T_MEAS_SEL_MAINTEMP_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(T_MEAS_SEL1_GPIO_Port, T_MEAS_SEL1_Pin, GPIO_PIN_RESET);

	gpsStatus = setGPS_DynamicModel6(&huart1); // - 6 􀀀 Airborne with <1g Acceleration HAB
	while(gpsStatus == GPS_FAIL)
	{
	  gpsStatus = setGPS_DynamicModel6(&huart1);
	  HAL_GPIO_TogglePin(GPIOB, GREEN_LED_Pin);
	}

	gpsStatus = disableNMEA(&huart1);
	if(gpsStatus != GPS_FAIL)
	{
	  // Nothing we can do
	  gpsStatus  = GPS_FAIL;
	}

	rscode_init(&rsDriver);

	RPM411Init(&hspi2);

	oneSecTic = 0;
	resetSI4032			        = 0;
	sendGPSInfo					= 0;
	sendInfo					= 1;
	sendCallSignInfo			= 1;
	sendBattInfo    			= 1;
	sendInternalTemperatureInfo = 0;
	sendPressureInfo         	= 0;

	HAL_Status = HAL_ERROR;
	while(HAL_Status != HAL_OK)
	{
		HAL_Status = setupRadio(&hspi2);
	}

	HAL_Status = clearRadio();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_GPIO_WritePin(T_MEAS_SEL_MAINTEMP_GPIO_Port, T_MEAS_SEL_MAINTEMP_Pin, GPIO_PIN_SET); //pin 12 boom
	HAL_GPIO_WritePin(TEMP_MODE_SELECT_START_GPIO_Port, TEMP_MODE_SELECT_START_Pin, GPIO_PIN_SET);
	HAL_Status = setRadio();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	    /* USER CODE END WHILE */

	if(resetSI4032)
	{
		HAL_Status = HAL_ERROR;
		while(HAL_Status != HAL_OK)
		{
			HAL_Status = setupRadio(&hspi2);
			HAL_Status = setRadio();
		}

		resetSI4032 = 0;
	}

	if(sendGPSInfo == 1)
	{
		processGPS(&huart1);
		sendGPSInfo 	= 0;
	}

	if(sendInfo)
	{
		processInfo(&rsDriver);
		sendInfo = 0;
	}

	if(sendCallSignInfo)
	{
		processCallSign(&rsDriver);
		sendCallSignInfo = 0;
	}

	if(sendBattInfo)
	{
		HAL_Status = clearRadio();
		processBattery(&rsDriver,&hadc1);
		HAL_Status = setRadio();
		tempClockStable  = 0;
		humClockStable   = 0;
		sendBattInfo    = 0;
	}

	if(sendInternalTemperatureInfo)
	{
		HAL_Status = clearRadio();
		processInternalTemp(&rsDriver);
		HAL_Status = setRadio();
		tempClockStable  = 0;
		humClockStable   = 0;
		sendInternalTemperatureInfo = 0;
	}

	//sendExternalTemperatureInfo = 1;
	if(sendExternalTemperatureInfo)
	{
		if(tempClockEnabled == 0)
		{
			HAL_GPIO_WritePin(T_MEAS_SEL_MAINTEMP_GPIO_Port, T_MEAS_SEL_MAINTEMP_Pin, GPIO_PIN_SET); //pin 12 boom
			HAL_GPIO_WritePin(TEMP_MODE_SELECT_START_GPIO_Port, TEMP_MODE_SELECT_START_Pin, GPIO_PIN_SET);
			HAL_Status = setRadio();
			tempClockEnabled = 1;
			tempClockStable  = 0;
		}
		if(tempClockStable > 5)
		{
			//HAL_Delay(1000);
			processExternalTemp(&rsDriver,frequency);
			HAL_GPIO_WritePin(T_MEAS_SEL_MAINTEMP_GPIO_Port, T_MEAS_SEL_MAINTEMP_Pin, GPIO_PIN_RESET); //pin 12 boom
			HAL_GPIO_WritePin(TEMP_MODE_SELECT_START_GPIO_Port, TEMP_MODE_SELECT_START_Pin, GPIO_PIN_RESET);
			HAL_Status = clearRadio();

			sendExternalTemperatureInfo = 0;
			tempClockEnabled            = 0;
			sendHumidityInfo            = 1;
		}
	}

	//sendHumidityInfo = 0;
	if(sendHumidityInfo)
	{
		if(humClockEnabled == 0)
		{
			HAL_GPIO_WritePin(HUMI_SEL0_GPIO_Port, HUMI_SEL0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(HUMI_MODE_SELECT_START_GPIO_Port, HUMI_MODE_SELECT_START_Pin, GPIO_PIN_SET);
			HAL_Status = setRadio();
			humClockEnabled = 1;
			humClockStable  = 0;
		}
		if(humClockStable  > 5)
		{
			//HAL_Delay(1000);
			processHumidity(&rsDriver,frequency);
			HAL_GPIO_WritePin(HUMI_SEL0_GPIO_Port, HUMI_SEL0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(HUMI_MODE_SELECT_START_GPIO_Port, HUMI_MODE_SELECT_START_Pin, GPIO_PIN_RESET);
			HAL_Status = clearRadio();

			sendHumidityInfo = 0;
			humClockEnabled  = 0;
		}
	}

	if(sendPressureInfo)
	{
		HAL_Status = clearRadio();
		__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
		processPressure(&rsDriver);
		__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSE);
		HAL_Status = setRadio();
		tempClockStable  = 0;
		humClockStable   = 0;
		sendPressureInfo = 0;
	}

#ifdef TEST_MODE
	processTestCam(&rsDriver);
#else
	processCAM(&huart3,&rsDriver);
#endif


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  if (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }


  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 23999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK)
  {
    Error_Handler();
  }


  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RADIO_NSS_Pin|T_MEAS_SEL_HUMITEMP_Pin|T_MEAS_SEL_MAINTEMP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, HUMI_MODE_SELECT_START_Pin|T_MEAS_SEL0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PRESSURE_NSS_Pin|TEMP_MODE_SELECT_START_Pin|HUMI_SEL0_Pin|HUMI_SEL1_Pin
                          |HUMI_SEL2_Pin|T_MEAS_SEL1_Pin|GREEN_LED_Pin|RED_LED_Pin
                          |HUMIDITY_HEATING_ON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RADIO_NSS_Pin T_MEAS_SEL_HUMITEMP_Pin T_MEAS_SEL_MAINTEMP_Pin */
  GPIO_InitStruct.Pin = RADIO_NSS_Pin|T_MEAS_SEL_HUMITEMP_Pin|T_MEAS_SEL_MAINTEMP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : HUMI_MODE_SELECT_START_Pin T_MEAS_SEL0_Pin */
  GPIO_InitStruct.Pin = HUMI_MODE_SELECT_START_Pin|T_MEAS_SEL0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EXT_PIN4_Pin */
  GPIO_InitStruct.Pin = EXT_PIN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXT_PIN4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PRESSURE_NSS_Pin TEMP_MODE_SELECT_START_Pin HUMI_SEL0_Pin HUMI_SEL1_Pin
                           HUMI_SEL2_Pin T_MEAS_SEL1_Pin GREEN_LED_Pin RED_LED_Pin
                           HUMIDITY_HEATING_ON_Pin */
  GPIO_InitStruct.Pin = PRESSURE_NSS_Pin|TEMP_MODE_SELECT_START_Pin|HUMI_SEL0_Pin|HUMI_SEL1_Pin
                          |HUMI_SEL2_Pin|T_MEAS_SEL1_Pin|GREEN_LED_Pin|RED_LED_Pin
                          |HUMIDITY_HEATING_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RCC_MCO_PRESS_Pin */
  GPIO_InitStruct.Pin = RCC_MCO_PRESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RCC_MCO_PRESS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  if (htim->Instance == TIM7)
  {	  frequency = pulseCount;
  	  pulseCount = 0;

	  //collectGPSData = 1;
	  HAL_GPIO_TogglePin(GPIOB, GREEN_LED_Pin);

	  oneSecTic++;

	  if(tempClockEnabled == 1)
	  {
		  tempClockStable++;
	  }

	  if(humClockEnabled == 1)
	  {
		  humClockStable++;
	  }

	  if(oneSecTic % 12 == 0)
	  {
		  sendGPSInfo = 1;
	  }

	  if(oneSecTic % 600 == 0)
	  {
		  sendInfo = 1;
	  }

	  if(oneSecTic % 600 == 0)
	  {
		  sendCallSignInfo = 1;
	  }

	  if(oneSecTic % 60 == 0)
	  {
		  sendBattInfo = 1;
	  }

	  if(oneSecTic % 60 == 0)
	  {
		  sendInternalTemperatureInfo = 1;
	  }

	  if(oneSecTic % 30 == 0)
	  {
		  sendExternalTemperatureInfo = 1;
	  }

//	  if(oneSecTic % 45 == 0)
//	  {
//		  sendHumidityInfo = 1;
//	  }

	  if(oneSecTic % 20 == 0)
	  {
		  sendPressureInfo = 1;
	  }

	  if(oneSecTic % 60 == 0)
	  {
		  resetSI4032 = 1;
	  }
  }

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
