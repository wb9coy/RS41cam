/*
 * packetDefs.h
 *
 *  Created on: Dec 27, 2021
 *      Author: eswiech
 */

#ifndef INC_PACKETDEFS_H_
#define INC_PACKETDEFS_H_

#include <stdint.h>
#include "ecc.h"

#define MTU_SIZE                  64

#define SENSOR_BUF_DATA_SIZE	  58
#define GPS_BUF_DATA_SIZE		  62
#define IMG_BUF_DATA_SIZE		  56
#define CALL_SIGN_SIZE		      12
#define INFO_DATA_SIZE		      32


//Packet Type
#define	START_IMAGE	0x01
#define	IMAGE_DATA	0x02
#define	END_IMAGE	0x03
#define	GPS	0x10
#define	GPS_GGA_1	0x10
#define	GPS_GGA_2	0x11
#define	GPS_RMC_1	0x12
#define	GPS_RMC_2	0x13
#define	INT_TEMP	0x20
#define	EXT_TEMP	0x25
#define	HUM			0x30
#define PRESSURE    0x40
#define INFO_DATA   0x60
#define CW_ID       0x70
#define BATT_INFO   0x80

struct __attribute__((__packed__)) HABPacketImageStartType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint32_t fileSize;
	uint16_t crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageEndType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint16_t crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageDataType
{
	uint8_t   packetType;
	uint8_t   imageFileID;
	uint16_t  imageSeqnum;
	uint8_t   imageDataLen;
	uint8_t   imageData[IMG_BUF_DATA_SIZE];
	uint16_t  crc16;
	//uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketGPSDataType
{
	uint8_t   packetType;
	uint8_t   gpsDataLen;
	uint8_t   gpsData[GPS_BUF_DATA_SIZE];
};

struct __attribute__((__packed__)) HABPacketSensorDataType
{
	uint8_t   packetType;
	uint8_t   sensorDataLen;
	uint8_t   sensorData[SENSOR_BUF_DATA_SIZE];
};

struct __attribute__((__packed__)) HABPacketInfoDataType
{
	uint8_t   packetType;
	uint8_t   infoDataLen;
	uint8_t   infoData[INFO_DATA_SIZE];
	uint16_t  crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketCallSignDataType
{
	uint8_t   packetType;
	uint8_t   callSignDataLen;
	uint8_t   callSignData[CALL_SIGN_SIZE];
	uint16_t  crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketBattInfoDataType
{
	uint8_t   packetType;
	float     battInfoData;
	uint16_t  crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketIntTempInfoDataType
{
	uint8_t   packetType;
	int8_t    intTempInfoData;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

#endif /* INC_PACKETDEFS_H_ */
