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
#define GPS_BUF_DATA_SIZE		  61
#define IMG_BUF_DATA_SIZE		  56
#define CALL_SIGN_SIZE		      12
#define INFO_DATA_SIZE		      32


//Packet Type
#define	START_IMAGE	0xff
#define	IMAGE_DATA	0xee
#define	END_IMAGE	0xdd
#define	GPS_GGA_1	0xcc
#define	GPS_GGA_2	0xbb
#define	GPS_RMC_1	0xaa
#define	GPS_RMC_2	0x99
#define	INT_TEMP	0x88
#define BATT_INFO   0x77
#define INFO_DATA   0x66
#define CW_ID       0x55

struct __attribute__((__packed__)) HABPacketImageStartType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint32_t  fileSize;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageEndType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageDataType
{
	uint16_t   packetType;
	uint8_t    imageFileID;
	uint16_t   imageSeqnum;
	uint8_t    imageDataLen;
	uint8_t    imageData[IMG_BUF_DATA_SIZE];
	uint16_t   crc16;
	//uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketGPSDataType
{
	uint16_t   packetType;
	uint8_t    gpsDataLen;
	uint8_t    gpsData[GPS_BUF_DATA_SIZE];
};

struct __attribute__((__packed__)) HABPacketSensorDataType
{
	uint16_t   packetType;
	uint8_t    sensorDataLen;
	uint8_t    sensorData[SENSOR_BUF_DATA_SIZE];
};

struct __attribute__((__packed__)) HABPacketInfoDataType
{
	uint16_t   packetType;
	uint8_t    infoDataLen;
	uint8_t    infoData[INFO_DATA_SIZE];
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketCallSignDataType
{
	uint16_t   packetType;
	uint8_t    callSignDataLen;
	uint8_t    callSignData[CALL_SIGN_SIZE];
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketBattInfoDataType
{
	uint16_t   packetType;
	float      battInfoData;
	uint16_t   crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketIntTempInfoDataType
{
	uint16_t   packetType;
	int8_t     intTempInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

#endif /* INC_PACKETDEFS_H_ */
