/*
 * gps.c
 *
 *  Created on: Jan 25, 2022
 *      Author: eswiech
 */

#include <string.h>
#include <ublox.h>

struct ubxChecksumType calcChecksum(uint8_t *message, uint16_t size)
{
	struct ubxChecksumType ck = {0, 0};

	for(int i = 0;i<size;i++)
	{
		ck.ck_a = ck.ck_a + message[i];
		ck.ck_b = ck.ck_b + ck.ck_a;
	}

	return ck;
}

// Configure flight mode - needed above 18km altitude
// Notes: Tweaked the PDOP limits a bit, to make it a bit more likely to report a position.
GPS_StatusTypeDef setGPS_DynamicModel6(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef HAL_Status;
	GPS_StatusTypeDef gpsStatus  = GPS_OK;

	struct ubxNavPacketType ubxNavPacket;

	uint8_t rspBuf[10];
	uint16_t received;
	uint8_t expectedRsp[10];

	uint8_t resetCmd = 0xff;

	memset(&ubxNavPacket,'\0',sizeof(ubxNavPacket));
	ubxNavPacket.ubxSync.sc1          = 0xb5;
	ubxNavPacket.ubxSync.sc2          = 0x62;
	ubxNavPacket.ubxNavMsgData.messageClass = 0x06;
	ubxNavPacket.ubxNavMsgData.messageId    = 0x24;
	ubxNavPacket.ubxNavMsgData.payloadSize  = sizeof(ubxNavPacket.ubxNavMsgData.ubxNavPayload);
	//ubxNavPacket.ubxNavMsgData.ubxNavPayload.mask = 0b00000001111111111;
	ubxNavPacket.ubxNavMsgData.ubxNavPayload.mask =   0b00000000000000011;
	ubxNavPacket.ubxNavMsgData.ubxNavPayload.dynModel        =6;
	ubxNavPacket.ubxNavMsgData.ubxNavPayload.fixMode         =3;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.fixedAlt        =0;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.fixedAltVar     =10000;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.minElv          =5;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.drLimit         =0;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.pDop            =100;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.tDop            =100;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.pAcc            =100;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.tAcc            =100;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.staticHoldThres =0;
//	ubxNavPacket.ubxNavMsgData.ubxNavPayload.dgpsTimeOut     =2;

	ubxNavPacket.ubxChecksum = calcChecksum((uint8_t *)&ubxNavPacket.ubxNavMsgData,sizeof(ubxNavPacket.ubxNavMsgData));

	expectedRsp[0] = ubxNavPacket.ubxSync.sc1; // header
	expectedRsp[1] = ubxNavPacket.ubxSync.sc2; // header
	expectedRsp[2] = 0x05; // class
	expectedRsp[3] = 0x01; // id
	expectedRsp[4] = 0x02; // length
	expectedRsp[5] = 0x00;
	expectedRsp[6] = ubxNavPacket.ubxNavMsgData.messageClass; // ACK class
	expectedRsp[7] =ubxNavPacket.ubxNavMsgData.messageId; // ACK id
	expectedRsp[8] = 0x32; // CK_A
	expectedRsp[9] = 0x5b; // CK_B

	HAL_Status = HAL_UART_Transmit(huart,&resetCmd,sizeof(resetCmd),1000);
	HAL_Delay(1000);
	if(HAL_Status != HAL_OK)
	{
		gpsStatus = GPS_FAIL;
	}
	else
	{
		HAL_Status = HAL_UART_Transmit(huart,(uint8_t *)&ubxNavPacket,sizeof(ubxNavPacket),1000);
		if(HAL_Status != HAL_OK)
		{
			gpsStatus = GPS_FAIL;
		}
		else
		{
			HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
			if(received != sizeof(rspBuf))
			{
				gpsStatus = GPS_FAIL;
			}
			else
			{
				if(HAL_Status == HAL_OK)
				{
					for(int i=0;i<sizeof(rspBuf);i++)
				    {
						if(rspBuf[i] != expectedRsp[i])
					    {
							gpsStatus = GPS_FAIL;
					    }
				    }
				}
			}
		}
	}

	return gpsStatus;
}

GPS_StatusTypeDef disableNMEA(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef HAL_Status;
	GPS_StatusTypeDef gpsStatus  = GPS_OK;
	uint8_t disableSentenceList[] = {0x01,  //GLL
									0x02,   //GSA
									0x03,   //GSV
									0x05,   //VTG
									0x08};  //ZDA

	struct ubxCFGMSGPacketType ubxCFGMSGPacket;

	ubxCFGMSGPacket.ubxSync.sc1                   = 0xb5;
	ubxCFGMSGPacket.ubxSync.sc2                   = 0x62;
	ubxCFGMSGPacket.ubxCFGMSGMsgData.messageClass = 0x06;
	ubxCFGMSGPacket.ubxCFGMSGMsgData.messageId    = 0x01;
	ubxCFGMSGPacket.ubxCFGMSGMsgData.payloadSize  = sizeof(ubxCFGMSGPacket.ubxCFGMSGMsgData.ubxCFGMSGPayload);
	ubxCFGMSGPacket.ubxCFGMSGMsgData.ubxCFGMSGPayload.msgClass = 0xF0;
	ubxCFGMSGPacket.ubxCFGMSGMsgData.ubxCFGMSGPayload.rate     = 0;
	for (int i =0; i < sizeof(disableSentenceList);i++)
	{
		ubxCFGMSGPacket.ubxCFGMSGMsgData.ubxCFGMSGPayload.msgID = disableSentenceList[i];
		ubxCFGMSGPacket.ubxChecksum = calcChecksum((uint8_t *)&ubxCFGMSGPacket.ubxCFGMSGMsgData,sizeof(ubxCFGMSGPacket.ubxCFGMSGMsgData));
		HAL_Status = HAL_UART_Transmit(huart,(uint8_t *)&ubxCFGMSGPacket,sizeof(ubxCFGMSGPacket),1000);
		if(HAL_Status != HAL_OK)
		{
			gpsStatus = GPS_FAIL;
		}

	}

	return gpsStatus;
}
