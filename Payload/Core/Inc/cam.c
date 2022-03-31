/*
 * cam.c
 *
 *  Created on: Dec 30, 2021
 *      Author: eswiech
 */

#include <string.h>
#include "checksum.h"
#include "ecc.h"
#include "cam.h"
#include "radio.h"
#include "config.h"
#include "packetDefs.h"

int totReveived 	= 0;
int lenFromCam 		= 0;
struct HABPacketImageDataType    HABPacketImageData;
struct HABPacketImageStartType   HABPacketImageStart;
struct HABPacketImageEndType     HABPacketImageEnd;

uint8_t header[CAM_HEADER_LEN];
uint8_t footer[CAM_FOOTER_LEN];

uint8_t headerRsp[] = {0x76, 0x00, 0x32, 0x00, 0x00};

uint8_t readDataCmd[] =
{0x56,
 0x00,
 0x32,
 0x0C,
 0x00,
 0x0A,
 0x00,
 0x00,
 0x00,//startAddr h
 0x00,//startAddr l
 0x00,
 0x00,
 0x00,//kh
 0x00,//kl
 0x00,//xh
 0x00 //xl
};

CAM_StatusTypeDef resetCam(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef HAL_Status;
	CAM_StatusTypeDef camStatus  = CAM_OK;

	uint8_t resetCmd[] = { 0x56, 0x00, 0x26, 0x00 };
	uint8_t expectedRsp[] = {0x76, 0x00, 0x26, 0x00 };
	uint8_t rspBuf[4];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,resetCmd,sizeof(resetCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		  camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				  camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

  return camStatus;
}

CAM_StatusTypeDef clearCache(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef HAL_Status;
	CAM_StatusTypeDef camStatus  = CAM_OK;

	uint8_t clearCacheCmd[] = { 0x56, 0x00, 0x36, 0x01, 0x02};
	uint8_t expectedRsp[] = {0x76, 0x00, 0x36, 0x00, 0x00 };
	uint8_t rspBuf[5];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,clearCacheCmd,sizeof(clearCacheCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		  camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				  camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

  return camStatus;
}

CAM_StatusTypeDef stopTakingPicture(UART_HandleTypeDef *huart)
{

	CAM_StatusTypeDef camStatus  = CAM_OK;

	HAL_StatusTypeDef HAL_Status;

	uint8_t stopTakingPictureCmd[] = { 0x56, 0x00, 0x36, 0x01, 0x03 };
	uint8_t expectedRsp[] = {0x76, 0x00, 0x36, 0x00, 0x00 };
	uint8_t rspBuf[5];
	uint16_t received;

	totReveived     = 0;

	HAL_Status = HAL_UART_Transmit(huart,stopTakingPictureCmd,sizeof(stopTakingPictureCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

  return camStatus;
}

CAM_StatusTypeDef setPhotoSize(UART_HandleTypeDef *huart)
{
	CAM_StatusTypeDef camStatus  = CAM_OK;
	HAL_StatusTypeDef HAL_Status;

	uint8_t setPhotoSizeCmd[] = { 0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x00 };
	uint8_t expectedRsp[] = {0x76, 0x00, 0x31, 0x00, 0x00 };
	uint8_t rspBuf[5];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,setPhotoSizeCmd,sizeof(setPhotoSizeCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

	return camStatus;
}


CAM_StatusTypeDef setCompression(UART_HandleTypeDef *huart)
{
	CAM_StatusTypeDef camStatus  = CAM_OK;
	HAL_StatusTypeDef HAL_Status;

	uint8_t setCompressionCmd[] = { 0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x36 };
	uint8_t expectedRsp[] = {0x76, 0x00, 0x31, 0x00, 0x00 };
	uint8_t rspBuf[5];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,setCompressionCmd,sizeof(setCompressionCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

	return camStatus;
}

CAM_StatusTypeDef getLenOfPhoto(UART_HandleTypeDef *huart, uint16_t *len)
{
	CAM_StatusTypeDef camStatus  = CAM_OK;
	HAL_StatusTypeDef HAL_Status;

	uint16_t rtnLen;

	uint8_t getLenOfPhotoCmd[] = { 0x56, 0x00, 0x34, 0x01, 0x00};
	uint8_t expectedRsp[] = {0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00};
	uint8_t rspBuf[9];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,getLenOfPhotoCmd,sizeof(getLenOfPhotoCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		 camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf)-2;i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				 camStatus = CAM_FAIL;
			  }
		  }

		  if(camStatus == CAM_OK)
		  {
			  rtnLen = (uint16_t)(rspBuf[sizeof(rspBuf)-2] << 8 | rspBuf[sizeof(rspBuf)-1]);
			  *len = rtnLen;
			  lenFromCam = rtnLen;
			  if( (rtnLen % IMG_BUF_DATA_SIZE) != 0 )
			  {
				  camStatus = CAM_FAIL;
					HAL_Delay(100);
			  }
		  }
	  }
	}

	return camStatus;
}

CAM_StatusTypeDef takePhoto(UART_HandleTypeDef *huart)
{
	CAM_StatusTypeDef camStatus  = CAM_OK;
	HAL_StatusTypeDef HAL_Status;

	uint8_t takePhotoCmd[] = { 0x56, 0x00, 0x36, 0x01, 0x00 };
	uint8_t expectedRsp[] = {0x76, 0x00, 0x36, 0x00, 0x00 };
	uint8_t rspBuf[5];
	uint16_t received;

	HAL_Status = HAL_UART_Transmit(huart,takePhotoCmd,sizeof(takePhotoCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		camStatus = CAM_FAIL;
	}
	else
	{
	  HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, rspBuf, sizeof(rspBuf), &received, 3000);
	  if(received != sizeof(rspBuf))
	  {
		  camStatus = CAM_FAIL;
	  }
	  else
	  {
		  for(int i=0;i<sizeof(rspBuf);i++)
		  {
			  if(rspBuf[i] != expectedRsp[i])
			  {
				  camStatus = CAM_FAIL;
			  }
		  }
	  }
	}

  return camStatus;
}

CAM_StatusTypeDef readData(UART_HandleTypeDef *huart, uint8_t camDataBuf[], uint16_t *len )
{
	CAM_StatusTypeDef status = CAM_OK;
	HAL_StatusTypeDef HAL_Status;

	uint16_t received;

	uint16_t k = CAM_DATA_LEN; // packet size
	uint16_t x = 0; //interval time xx xx * 0.01m[sec]

	uint8_t lenMSB;
	uint8_t lenLSB;
	uint8_t adrMSB;
	uint8_t adrLSB;
	uint8_t intervalMSB;
	uint8_t intervalLSB;

	adrMSB = totReveived >> 8;
	adrLSB = (uint8_t)totReveived;
	readDataCmd[8] = adrMSB;
	readDataCmd[9] = adrLSB;

	lenMSB = k >> 8;
	lenLSB = (uint8_t)k;
	readDataCmd[12] = lenMSB;
	readDataCmd[13] = lenLSB;

	intervalMSB = x >> 8;
	intervalLSB = (uint8_t)x;
	readDataCmd[14] = intervalMSB;
	readDataCmd[15] = intervalLSB;

	status = CAM_OK;

	HAL_Status = HAL_UART_Transmit(huart,readDataCmd,sizeof(readDataCmd),1000);
	if(HAL_Status != HAL_OK)
	{
		status = CAM_FAIL;
	}
	else
	{
		HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, header, sizeof(header), &received, 3000);
		if(received != sizeof(header))
		{
			 status = CAM_FAIL;
		}
		else
		{
			for(int i=0;i<sizeof(header);i++)
			{
				if(header[i] != headerRsp[i])
				{
					status = CAM_FAIL;
				}
			}
		}
	}

	if(status == CAM_OK)
	{
		HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, camDataBuf, CAM_DATA_LEN, &received, 3000);
		if(received != CAM_DATA_LEN)
		{
		  status = CAM_FAIL;
		}
		else
		{
			*len = received;
			totReveived = totReveived + received;
			for(int i=0;i<received;i++)
			{
				if((camDataBuf[i-1] == 0xff) && (camDataBuf[i-0] == 0xD9))
				{
					status = CAM_END;
					break;
				}
			}
			if(status == CAM_END)
			{
				if(lenFromCam != totReveived)
				{
					status = CAM_FAIL;
				}
//				HAL_Status = HAL_UARTEx_ReceiveToIdle(huart, footer, sizeof(footer), &received, 3000);
//				if(HAL_Status != HAL_OK)
//				{
//					status = CAM_FAIL;
//				}
			}
		}
	}

	return status;
}

CAM_StatusTypeDef processCAM(UART_HandleTypeDef *huart,struct rscode_driver *rsDriver)
{
	HAL_StatusTypeDef HAL_Status;
	CAM_StatusTypeDef camStatus  = CAM_OK;

	uint16_t imageLen     = 0;
	uint16_t imageXferLen = 0;
    uint16_t len          = 0;

	static CAM_StateTypeDef CAM_State = CAM_SETUP;
	static uint8_t   imageFileID = 0;
	static uint16_t  imageSeqnum = 0;

	uint8_t txBuf[MTU_SIZE];
	uint8_t camDataBuf[CAM_DATA_LEN];

	switch(CAM_State)
	{
		case CAM_SETUP:
		{
			camStatus = setPhotoSize(huart);
			camStatus = resetCam(huart);
			if(camStatus == CAM_OK)
			{
				CAM_State = CAM_TAKE_PIC;
			}
			else
			{
				CAM_State = CAM_SETUP;
			}
			break;
		}
		case CAM_TAKE_PIC:
		{
			camStatus = stopTakingPicture(huart);
			HAL_Delay(500);
			camStatus =  setCompression(huart);
			if(camStatus == CAM_OK)
			{
				camStatus = takePhoto(huart);
				if(camStatus == CAM_OK)
				{
					camStatus = getLenOfPhoto(huart, &imageLen);
					if(camStatus == CAM_OK)
					{
						imageSeqnum = 0;
						memset(&HABPacketImageStart,'\0',sizeof(HABPacketImageStart));
						HABPacketImageStart.packetType  = START_IMAGE;
						HABPacketImageStart.imageFileID = imageFileID;
						HABPacketImageStart.fileSize    = imageLen;
						len = sizeof(HABPacketImageStart)-sizeof(HABPacketImageStart.crc16)-NPAR;
						HABPacketImageStart.crc16 = crc_16((unsigned char *)&HABPacketImageStart,len);
						rscode_encode(rsDriver, (unsigned char *)&HABPacketImageStart, sizeof(HABPacketImageStart)-NPAR, (unsigned char *)&HABPacketImageStart);

						memset(&txBuf,'\0',sizeof(txBuf));
						memcpy(txBuf,&HABPacketImageStart,sizeof(HABPacketImageStart));
						HAL_Status =  radioTxData(txBuf,sizeof(HABPacketImageStart));
						if(HAL_Status != HAL_OK)
						{
							CAM_State = CAM_SETUP;
						}
						CAM_State = CAM_DATA_XFER;
					}
					else
					{
						CAM_State = CAM_SETUP;
					}
				}
				else
				{
					CAM_State = CAM_TAKE_PIC;
				}
			}
			break;
		}
		case CAM_DATA_XFER:
		{
			camStatus =  readData(huart, camDataBuf, &imageXferLen);
			switch(camStatus)
			{
				case CAM_OK:
				case CAM_END:
				{
					HABPacketImageData.packetType = IMAGE_DATA;
					HABPacketImageData.imageFileID = imageFileID;
					HABPacketImageData.imageSeqnum = imageSeqnum;
					HABPacketImageData.imageDataLen = imageXferLen;
					memcpy(&HABPacketImageData.imageData,&camDataBuf,HABPacketImageData.imageDataLen);
					//len = sizeof(HABPacketImageData)-sizeof(HABPacketImageData.crc16)-NPAR;
					len = sizeof(HABPacketImageData)-sizeof(HABPacketImageData.crc16);
					HABPacketImageData.crc16 = crc_16((unsigned char *)&HABPacketImageData,len);
					//rscode_encode(rsDriver, (unsigned char *)&HABPacketImageData, sizeof(HABPacketImageData)-NPAR, (unsigned char *)&HABPacketImageData);

					memset(&txBuf,'\0',sizeof(txBuf));
					memcpy(txBuf,&HABPacketImageData,sizeof(HABPacketImageData));
					HAL_Status =  radioTxData(txBuf,sizeof(HABPacketImageData));
					if(HAL_Status != HAL_OK)
					{
						CAM_State = CAM_SETUP;
					}
					else
					{
						if(camStatus == CAM_END)
						{
							CAM_State = CAM_END_XFER;
						}
					}
					imageSeqnum = imageSeqnum + 1;
					break;
				}
				case CAM_FAIL:
				{
					CAM_State = CAM_TAKE_PIC;
					break;
				}
			}
			break;
		}
		case CAM_END_XFER:
		{

			HABPacketImageEnd.packetType    = END_IMAGE;
			HABPacketImageEnd.imageFileID   = imageFileID;
			len = sizeof(HABPacketImageEnd)-sizeof(HABPacketImageEnd.crc16)-NPAR;
			HABPacketImageEnd.crc16 = crc_16((unsigned char *)&HABPacketImageEnd,len);
			rscode_encode(rsDriver, (unsigned char *)&HABPacketImageEnd, sizeof(HABPacketImageEnd)-NPAR, (unsigned char *)&HABPacketImageEnd);
			memset(&txBuf,'\0',sizeof(txBuf));
			memcpy(txBuf,&HABPacketImageEnd,sizeof(HABPacketImageEnd));
			HAL_Status =  radioTxData(txBuf,sizeof(HABPacketImageEnd));
			if(HAL_Status == HAL_OK)
			{
				CAM_State = CAM_TAKE_PIC;
				imageFileID = imageFileID +1;
			}
			else
			{
				CAM_State = CAM_TAKE_PIC;
			}
			break;
		}
	}

	return camStatus;
};
