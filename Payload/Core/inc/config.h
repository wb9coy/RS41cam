/*
 * config.h
 *
 *  Created on: Oct 25, 2021
 *      Author: eswiech
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

//#define TEST_MODE

#define VERSION_INFO ((char *)"Payload Version 1.2")

#define CALL_SIGN ((char *)"W6SUN")

#define TRANSMIT_FREQUENCY  434.500f //Mhz

// TX Power
#define TX_POWER  7 // PWR 0...7 0- MIN ... 7 - MAX
// Power Levels measured at 434.650 MHz, using a Rigol DSA815, and a 10 kHz RBW
// Power measured by connecting a short (30cm) length of RG316 directly to the
// antenna/ground pads at the bottom of the RS41 PCB.
// 0 --> -1.9dBm
// 1 --> 1.3dBm
// 2 --> 3.6dBm
// 3 --> 7.0dBm
// 4 --> 10.0dBm
// 5 --> 13.1dBm - DEFAULT
// 6 --> 15.0dBm
// 7 --> 16.3dBm

#define ADC_BAT_CAL  0.48f

#define ADC_TEMP_CAL_REG12  0xe0
#define ADC_TEMP_CAL_REG13  0x09

#define PROTOCOL_DELAY 100

//#define CALEXTTEMP

//V1240937
//#define EXTTEMPREFFREQ   66700.0f
//#define EXTTEMPOFFSET    18.8f
//#define EXTTEMPDIVISION  226.8f

//U3471019
#define EXTTEMPREFFREQ   71000.0f
#define EXTTEMPOFFSET    18.8f
#define EXTTEMPDIVISION  240.0f

//#define HUMREFFREQ  43475.0f
//#define HUMDIVISION 6.76f


#endif /* INC_CONFIG_H_ */
