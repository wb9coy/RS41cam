#include <string.h>
#include "bme280.h"
#include "main.h"


static SPI_HandleTypeDef *hspi;
static struct SensorCalibrationType SensorCalibration;
static struct bme280SensorSettingsType  bme280SensorSettings;

uint8_t setBit(uint8_t n, int k)
{
    return (n | (1 << (k - 1)));
}

// Function to clear the kth bit of n
uint8_t clearBit(uint8_t n, int k)
{
    return (n & (~(1 << (k - 1))));
}

uint8_t bmeReadRegister(const uint8_t register_addr)
{
  HAL_StatusTypeDef HAL_Status;
  uint8_t val;
  uint8_t addr;

  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);

  addr = setBit(register_addr,8);

  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
  HAL_Delay(1);
  HAL_Status = HAL_SPI_Receive(hspi, &val, 1, 1000);

  if(HAL_Status != HAL_OK)
  {
	  val = -1;
  }

  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);

  return val;
}

uint8_t bmeWriteRegister(const uint8_t register_addr, uint8_t value)
{
	  HAL_StatusTypeDef HAL_Status;
	  uint8_t addr;

	  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);

	  addr = clearBit(register_addr,8);

	  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
	  HAL_Delay(1);
	  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&value, 1, 1000);

	  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);

	  return HAL_Status;
}

HAL_StatusTypeDef bmeReadBurst(const uint8_t register_addr, uint8_t *pData, int len)
{
  HAL_StatusTypeDef HAL_Status;
  uint8_t addr;

  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_RESET);

  addr = setBit(register_addr,8);

  HAL_Status = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 1000);
  HAL_Delay(1);
  for(int i = 0; i < len;i++)
  {
	  HAL_Status = HAL_SPI_Receive(hspi,&pData[i], 1, 1000);
	  HAL_Delay(1);
  }

  HAL_GPIO_WritePin(PRESSURE_NSS_GPIO_Port, PRESSURE_NSS_Pin, GPIO_PIN_SET);

  return HAL_Status;
}

BME_StatusTypeDef getCalibrationData()
{
	BME_StatusTypeDef BME_Status = BME_OK;

	//Reading all compensation data, range 0x88:A1, 0xE1:E7
	SensorCalibration.dig_T1 = ((uint16_t)((bmeReadRegister(BME280_DIG_T1_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_T1_LSB_REG)));
	SensorCalibration.dig_T2 = ((int16_t)((bmeReadRegister(BME280_DIG_T2_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_T2_LSB_REG)));
	SensorCalibration.dig_T3 = ((int16_t)((bmeReadRegister(BME280_DIG_T3_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_T3_LSB_REG)));

	SensorCalibration.dig_P1 = ((uint16_t)((bmeReadRegister(BME280_DIG_P1_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P1_LSB_REG)));
	SensorCalibration.dig_P2 = ((int16_t)((bmeReadRegister(BME280_DIG_P2_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P2_LSB_REG)));
	SensorCalibration.dig_P3 = ((int16_t)((bmeReadRegister(BME280_DIG_P3_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P3_LSB_REG)));
	SensorCalibration.dig_P4 = ((int16_t)((bmeReadRegister(BME280_DIG_P4_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P4_LSB_REG)));
	SensorCalibration.dig_P5 = ((int16_t)((bmeReadRegister(BME280_DIG_P5_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P5_LSB_REG)));
	SensorCalibration.dig_P6 = ((int16_t)((bmeReadRegister(BME280_DIG_P6_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P6_LSB_REG)));
	SensorCalibration.dig_P7 = ((int16_t)((bmeReadRegister(BME280_DIG_P7_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P7_LSB_REG)));
	SensorCalibration.dig_P8 = ((int16_t)((bmeReadRegister(BME280_DIG_P8_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P8_LSB_REG)));
	SensorCalibration.dig_P9 = ((int16_t)((bmeReadRegister(BME280_DIG_P9_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_P9_LSB_REG)));

	SensorCalibration.dig_H1 = ((uint8_t)(bmeReadRegister(BME280_DIG_H1_REG)));
	SensorCalibration.dig_H2 = ((int16_t)((bmeReadRegister(BME280_DIG_H2_MSB_REG) << 8) + bmeReadRegister(BME280_DIG_H2_LSB_REG)));
	SensorCalibration.dig_H3 = ((uint8_t)(bmeReadRegister(BME280_DIG_H3_REG)));
	SensorCalibration.dig_H4 = ((int16_t)((bmeReadRegister(BME280_DIG_H4_MSB_REG) << 4) + (bmeReadRegister(BME280_DIG_H4_LSB_REG) & 0x0F)));
	SensorCalibration.dig_H5 = ((int16_t)((bmeReadRegister(BME280_DIG_H5_MSB_REG) << 4) + ((bmeReadRegister(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
	SensorCalibration.dig_H6 = ((int8_t)bmeReadRegister(BME280_DIG_H6_REG));

	return BME_Status;
}

BME_StatusTypeDef setStandbyTime(uint8_t timeSetting)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	if(timeSetting > 0b111) timeSetting = 0; //Error check. Default to 0.5ms

	uint8_t controlData = bmeReadRegister(BME280_CONFIG_REG);
	controlData &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear the 7/6/5 bits
	controlData |= (timeSetting << 5); //Align with bits 7/6/5
	bmeWriteRegister(BME280_CONFIG_REG, controlData);

	return BME_Status;
}

BME_StatusTypeDef setFilter(uint8_t filterSetting)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	if(filterSetting > 0b111) filterSetting = 0; //Error check. Default to filter off

	uint8_t controlData = bmeReadRegister(BME280_CONFIG_REG);
	controlData &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear the 4/3/2 bits
	controlData |= (filterSetting << 2); //Align with bits 4/3/2
	bmeWriteRegister(BME280_CONFIG_REG, controlData);

	return BME_Status;
}

uint8_t checkSampleValue(uint8_t userValue)
{
	switch(userValue)
	{
		case(0):
			return 0;
			break; //Valid
		case(1):
			return 1;
			break; //Valid
		case(2):
			return 2;
			break; //Valid
		case(4):
			return 3;
			break; //Valid
		case(8):
			return 4;
			break; //Valid
		case(16):
			return 5;
			break; //Valid
		default:
			return 1; //Default to 1x
			break; //Good
	}
}

uint8_t getMode()
{
	uint8_t controlData = bmeReadRegister(BME280_CTRL_MEAS_REG);
	return(controlData & 0b00000011); //Clear bits 7 through 2
}

BME_StatusTypeDef setMode(uint8_t mode)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	if(mode > 0b11) mode = 0; //Error check. Default to sleep mode

	uint8_t controlData = bmeReadRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<1) | (1<<0) ); //Clear the mode[1:0] bits
	controlData |= mode; //Set
	bmeWriteRegister(BME280_CTRL_MEAS_REG, controlData);
	return BME_Status;
}

BME_StatusTypeDef setTempOverSample(uint8_t overSampleAmount)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	overSampleAmount = checkSampleValue(overSampleAmount); //Error check

	uint8_t originalMode = getMode(); //Get the current mode so we can go back to it at the end

	setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_t bits (7, 6, 5) to overSampleAmount
	uint8_t controlData = bmeReadRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear bits 765
	controlData |= overSampleAmount << 5; //Align overSampleAmount to bits 7/6/5
	bmeWriteRegister(BME280_CTRL_MEAS_REG, controlData);

	setMode(originalMode); //Return to the original user's choice

	return BME_Status;
}

BME_StatusTypeDef setPressureOverSample(uint8_t overSampleAmount)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	overSampleAmount = checkSampleValue(overSampleAmount); //Error check

	uint8_t originalMode = getMode(); //Get the current mode so we can go back to it at the end

	setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_p bits (4, 3, 2) to overSampleAmount
	uint8_t controlData = bmeReadRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear bits 432
	controlData |= overSampleAmount << 2; //Align overSampleAmount to bits 4/3/2
	bmeWriteRegister(BME280_CTRL_MEAS_REG, controlData);

	setMode(originalMode); //Return to the original user's choice


	return BME_Status;
}

BME_StatusTypeDef setHumidityOverSample(uint8_t overSampleAmount)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	overSampleAmount = checkSampleValue(overSampleAmount); //Error check

	uint8_t originalMode = getMode(); //Get the current mode so we can go back to it at the end

	setMode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_h bits (2, 1, 0) to overSampleAmount
	uint8_t controlData = bmeReadRegister(BME280_CTRL_HUMIDITY_REG);
	controlData &= ~( (1<<2) | (1<<1) | (1<<0) ); //Clear bits 2/1/0
	controlData |= overSampleAmount << 0; //Align overSampleAmount to bits 2/1/0
	bmeWriteRegister(BME280_CTRL_HUMIDITY_REG, controlData);

	setMode(originalMode); //Return to the original user's choice

	return BME_Status;
}

float readTempFromBurst(uint8_t buffer[])
{
	int32_t adc_T = ((uint32_t)buffer[3] << 12) | ((uint32_t)buffer[4] << 4) | ((buffer[5] >> 4) & 0x0F);
	int32_t t_fine;

	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)SensorCalibration.dig_T1<<1))) * ((int32_t)SensorCalibration.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)SensorCalibration.dig_T1)) * ((adc_T>>4) - ((int32_t)SensorCalibration.dig_T1))) >> 12) *
	((int32_t)SensorCalibration.dig_T3)) >> 14;
	t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	output = output / 100 + bme280SensorSettings.tempCorrection;

 	return output;
}

BME_StatusTypeDef readTempFFromBurst(uint8_t buffer[], struct bme280SensorMeasurementsType *measurements)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	float output = readTempFromBurst(buffer);
	output = (output * 9) / 5 + 32;

	measurements->temperature = output;

	return BME_Status;
}

BME_StatusTypeDef readTempCFromBurst(uint8_t buffer[], struct bme280SensorMeasurementsType *measurements)
{
	BME_StatusTypeDef BME_Status = BME_OK;

	measurements->temperature = readTempFromBurst(buffer);

	return BME_Status;
}

BME_StatusTypeDef readFloatPressureFromBurst(uint8_t buffer[], struct bme280SensorMeasurementsType *measurements)
{

	BME_StatusTypeDef BME_Status = BME_OK;
	int32_t t_fine = 0;

	// Set pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa

	int32_t adc_P = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);

	int64_t var1, var2, p_acc;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)SensorCalibration.dig_P6;
	var2 = var2 + ((var1 * (int64_t)SensorCalibration.dig_P5)<<17);
	var2 = var2 + (((int64_t)SensorCalibration.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)SensorCalibration.dig_P3)>>8) + ((var1 * (int64_t)SensorCalibration.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)SensorCalibration.dig_P1)>>33;
	if (var1 == 0)
	{
		measurements->pressure = 0; // avoid exception caused by division by zero
	}
	else
	{
		p_acc = 1048576 - adc_P;
		p_acc = (((p_acc<<31) - var2)*3125)/var1;
		var1 = (((int64_t)SensorCalibration.dig_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
		var2 = (((int64_t)SensorCalibration.dig_P8) * p_acc) >> 19;
		p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)SensorCalibration.dig_P7)<<4);

		measurements->pressure = (float)p_acc / 256.0;
	}

	return BME_Status;
}

BME_StatusTypeDef readFloatHumidityFromBurst(uint8_t buffer[], struct bme280SensorMeasurementsType *measurements)
{
	BME_StatusTypeDef BME_Status = BME_OK;
	int32_t t_fine = 0;

	// Set humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
	int32_t adc_H = ((uint32_t)buffer[6] << 8) | ((uint32_t)buffer[7]);

	int32_t var1;
	var1 = (t_fine - ((int32_t)76800));
	var1 = (((((adc_H << 14) - (((int32_t)SensorCalibration.dig_H4) << 20) - (((int32_t)SensorCalibration.dig_H5) * var1)) +
	((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)SensorCalibration.dig_H6)) >> 10) * (((var1 * ((int32_t)SensorCalibration.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
	((int32_t)SensorCalibration.dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)SensorCalibration.dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);

	measurements->humidity = (float)(var1>>12) / 1024.0;

	return BME_Status;
}

uint8_t isMeasuring(void)
{
	uint8_t rtn = 1;

	uint8_t stat = bmeReadRegister(BME280_STAT_REG);
	if( (stat & (1<<3)) == 0)
	{
		rtn = 0;
	}
	return rtn;
}

BME_StatusTypeDef readAllMeasurements(struct bme280SensorMeasurementsType *measurements, uint8_t tempScale)
{

	BME_StatusTypeDef BME_Status = BME_OK;
	HAL_StatusTypeDef HAL_Status;
	int measureCount = 0;

	uint8_t dataBurst[8];

	while(isMeasuring() != 0)
	{
		measureCount++;
		if(measureCount > 1000)
		{
			break;
		}
	}

	HAL_Status =  bmeReadBurst(BME280_MEASUREMENTS_REG, dataBurst, sizeof(dataBurst));
	if(HAL_Status == HAL_OK)
	{
		if(tempScale == CELSIUS_SCALE)
		{
			readTempCFromBurst(dataBurst, measurements);
		}
		else
		{
			readTempFFromBurst(dataBurst, measurements);
		}

		readFloatPressureFromBurst(dataBurst, measurements);
		readFloatHumidityFromBurst(dataBurst, measurements);
	}
	else
	{
		BME_Status = BME_ERROR;
	}

	return BME_Status;
}

BME_StatusTypeDef bme280_init(SPI_HandleTypeDef *hspiParam)
{
	BME_StatusTypeDef BME_Status = BME_OK;
	hspi = hspiParam;
	uint8_t chipID;

	bme280SensorSettings.runMode = MODE_NORMAL; //Normal/Run
	bme280SensorSettings.tStandby = 0; //0.5ms
	bme280SensorSettings.filter = 0; //Filter off
	bme280SensorSettings.tempOverSample  = 1;
	bme280SensorSettings.pressOverSample = 1;
	bme280SensorSettings.humidOverSample = 1;
	bme280SensorSettings.tempCorrection = 0.f; // correction of temperature - added to the result


	chipID = bmeReadRegister(BME280_CHIP_ID_REG);
	if(chipID !=BME280_CHIP_ID )
	{
		BME_Status = BME_ERROR;
	}
	else
	{
		setMode(MODE_SLEEP);
		BME_Status = getCalibrationData();
		BME_Status = setStandbyTime(bme280SensorSettings.tStandby);
		BME_Status = setFilter(bme280SensorSettings.filter);
		BME_Status = setPressureOverSample(bme280SensorSettings.pressOverSample);
		BME_Status = setHumidityOverSample(bme280SensorSettings.humidOverSample);
		BME_Status = setTempOverSample(bme280SensorSettings.tempOverSample);
		setMode(bme280SensorSettings.runMode);
	}

	return BME_Status;
}

