
#ifndef BME280_H_
#define BME280_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stddef.h>

typedef enum
{
  BME_OK       = 0x00U,
  BME_ERROR    = 0x01U
} BME_StatusTypeDef;

#define MODE_SLEEP 0b00
#define MODE_FORCED 0b01
#define MODE_NORMAL 0b11

#define BME280_CHIP_ID					0x60

//Register names:
#define BME280_DIG_T1_LSB_REG			0x88
#define BME280_DIG_T1_MSB_REG			0x89
#define BME280_DIG_T2_LSB_REG			0x8A
#define BME280_DIG_T2_MSB_REG			0x8B
#define BME280_DIG_T3_LSB_REG			0x8C
#define BME280_DIG_T3_MSB_REG			0x8D
#define BME280_DIG_P1_LSB_REG			0x8E
#define BME280_DIG_P1_MSB_REG			0x8F
#define BME280_DIG_P2_LSB_REG			0x90
#define BME280_DIG_P2_MSB_REG			0x91
#define BME280_DIG_P3_LSB_REG			0x92
#define BME280_DIG_P3_MSB_REG			0x93
#define BME280_DIG_P4_LSB_REG			0x94
#define BME280_DIG_P4_MSB_REG			0x95
#define BME280_DIG_P5_LSB_REG			0x96
#define BME280_DIG_P5_MSB_REG			0x97
#define BME280_DIG_P6_LSB_REG			0x98
#define BME280_DIG_P6_MSB_REG			0x99
#define BME280_DIG_P7_LSB_REG			0x9A
#define BME280_DIG_P7_MSB_REG			0x9B
#define BME280_DIG_P8_LSB_REG			0x9C
#define BME280_DIG_P8_MSB_REG			0x9D
#define BME280_DIG_P9_LSB_REG			0x9E
#define BME280_DIG_P9_MSB_REG			0x9F
#define BME280_DIG_H1_REG				0xA1
#define BME280_CHIP_ID_REG				0xD0 //Chip ID
#define BME280_RST_REG					0xE0 //Softreset Reg
#define BME280_DIG_H2_LSB_REG			0xE1
#define BME280_DIG_H2_MSB_REG			0xE2
#define BME280_DIG_H3_REG				0xE3
#define BME280_DIG_H4_MSB_REG			0xE4
#define BME280_DIG_H4_LSB_REG			0xE5
#define BME280_DIG_H5_MSB_REG			0xE6
#define BME280_DIG_H6_REG				0xE7
#define BME280_CTRL_HUMIDITY_REG		0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG					0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG			0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG				0xF5 //Configuration Reg
#define BME280_MEASUREMENTS_REG			0xF7 //Measurements register start
#define BME280_PRESSURE_MSB_REG			0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG			0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG		0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG		0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG		0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG		0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG			0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG			0xFE //Humidity LSB

#define CELSIUS_SCALE 0 //Default
#define FAHRENHEIT_SCALE 1

struct SensorCalibrationType
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;

	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;

};

struct bme280SensorSettingsType
{
	//Deprecated settings
	uint8_t runMode;
	uint8_t tStandby;
	uint8_t filter;
	uint8_t tempOverSample;
	uint8_t pressOverSample;
	uint8_t humidOverSample;
    float tempCorrection; // correction of temperature - added to the result
};

struct bme280SensorMeasurementsType
{
	float temperature;
	float pressure;
	float humidity;
};

BME_StatusTypeDef bme280_init(SPI_HandleTypeDef *hspiParam);
BME_StatusTypeDef readAllMeasurements(struct bme280SensorMeasurementsType *measurements, uint8_t tempScale);

#endif /* BME280_H_ */
/** @}*/
