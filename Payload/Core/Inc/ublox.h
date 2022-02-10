/*
 * gps.h
 *
 *  Created on: Jan 25, 2022
 *      Author: eswiech
 */

#ifndef INC_UBLOX_H_
#define INC_UBLOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "gps.h"


struct  __attribute__((packed)) ubxSyncType
{
  uint8_t sc1; // 0xB5
  uint8_t sc2; // 0x62
};

struct __attribute__((__packed__)) ubxChecksumType
{
  uint8_t ck_a;
  uint8_t ck_b;
};

struct __attribute__((__packed__)) ubxCFGMSGPayloadType
{
	uint8_t msgClass;	//Message Class [- -]
	uint8_t msgID;		//Message Identifier [- -]
	uint8_t rate;		//Send rate on current Port [- -]
};

struct __attribute__((__packed__)) ubxNavPayloadType
{
	uint16_t mask;
	uint8_t  dynModel;
	uint8_t  fixMode;
	uint32_t fixedAlt;
	uint32_t fixedAltVar;
	uint8_t  minElv;
	uint8_t  drLimit;
	uint16_t pDop;
	uint16_t tDop;
	uint16_t pAcc;
	uint16_t tAcc;
	uint8_t  staticHoldThres;
	uint8_t  dgpsTimeOut;
	uint32_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
};

struct __attribute__((__packed__)) ubxNavMsgDataType
{
	uint8_t messageClass;
	uint8_t messageId;
	uint16_t payloadSize;
	struct ubxNavPayloadType ubxNavPayload;
};

struct __attribute__((__packed__)) ubxCFGMSGMsgDataType
{
	uint8_t messageClass;
	uint8_t messageId;
	uint16_t payloadSize;
	struct ubxCFGMSGPayloadType ubxCFGMSGPayload;
};

struct __attribute__((__packed__)) ubxNavPacketType
{
	struct ubxSyncType       ubxSync;
	struct ubxNavMsgDataType ubxNavMsgData;
	struct ubxChecksumType   ubxChecksum;
};

struct __attribute__((__packed__)) ubxCFGMSGPacketType
{
	struct ubxSyncType          ubxSync;
	struct ubxCFGMSGMsgDataType ubxCFGMSGMsgData;
	struct ubxChecksumType      ubxChecksum;
};

GPS_StatusTypeDef setGPS_DynamicModel6(UART_HandleTypeDef *huart);
GPS_StatusTypeDef disableNMEA(UART_HandleTypeDef *huart);
struct ubxChecksumType calcChecksum(uint8_t *message, uint16_t size);

#endif /* INC_UBLOX_H_ */
