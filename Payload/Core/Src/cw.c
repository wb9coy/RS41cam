/*
 * cw.c
 *
 *  Created on: Jan 27, 2022
 *      Author: eswiech
 */
#include <string.h>
#include "stm32f1xx_hal.h"
#include "packetDefs.h"
#include "radio.h"

HAL_StatusTypeDef HAL_Status;
int spaceDelay  = 20;
uint8_t txBuf[] = {0};

void space()
{
    HAL_Delay(spaceDelay);
}

void characterSpace()
{
	HAL_Delay(spaceDelay*9);
}

void dit()
{
	HAL_Status = radio_write_register(0x34, 0x60);
	HAL_Status =  radioTxData(txBuf,sizeof(txBuf));
	space();
}

void dah()
{
	HAL_Status = radio_write_register(0x34, 0xff);
	HAL_Status =  radioTxData(txBuf,sizeof(txBuf));
	space();
}

void sendCharacter(char c)
{
	switch(c)
	{
		case 'a':
			dit();
			dah();
			break;
		case 'b':
			dah();
			dit();
			dit();
			dit();
			break;
		case 'c':
			dah();
			dit();
			dah();
			dit();
			break;
		case 'd':
			dah();
			dit();
			dit();
			break;
		case 'e':
			dit();
			break;
		case 'f':
			dit();
			dit();
			dah();
			dit();
		case 'g':
			dah();
			dah();
			dit();
			break;
		case 'h':
			dit();
			dit();
			dit();
			dit();
			break;
		case 'i':
			dit();
			dit();
			break;
		case 'j':
			dit();
			dah();
			dah();
			dah();
			break;
		case 'k':
			dah();
			dit();
			dah();
			break;
		case 'l':
			dit();
			dah();
			dit();
			dit();
			break;
		case 'm':
			dah();
			dah();
			break;
		case 'n':
			dah();
			dit();
			break;
		case 'o':
			dah();
			dah();
			dah();
			break;
		case 'p':
			dit();
			dah();
			dah();
			dit();
			break;
		case 'q':
			dah();
			dah();
			dit();
			dah();
			break;
		case 'r':
			dit();
			dah();
			dit();
			break;
		case 's':
			dit();
			dit();
			dit();
			break;
		case 't':
			dah();
			break;
		case 'u':
			dit();
			dit();
			dah();
			break;
		case 'v':
			dit();
			dit();
			dit();
			dah();
			break;
		case 'w':
			dit();
			dah();
			dah();
			break;
		case 'x':
			dah();
			dit();
			dit();
			dah();
			break;
		case 'y':
			dah();
			dit();
			dah();
			dah();
			break;
		case 'z':
			dah();
			dah();
			dit();
			dit();
			break;
		case '1':
			dit();
			dah();
			dah();
			dah();
			dah();
			break;
		case '2':
			dit();
			dit();
			dah();
			dah();
			dah();
			break;
		case '3':
			dit();
			dit();
			dit();
			dah();
			dah();

			break;
		case '4':
			dit();
			dit();
			dit();
			dit();
			dah();
			break;
		case '5':
			dit();
			dit();
			dit();
			dit();
			dit();
			break;
		case '6':
			dah();
			dit();
			dit();
			dit();
			dit();
			break;
		case '7':
			dah();
			dah();
			dit();
			dit();
			dit();
			break;
		case '8':
			dah();
			dah();
			dah();
			dit();
			dit();
			break;
		case '9':
			dah();
			dah();
			dah();
			dah();
			dit();
			break;
		case '0':
			dah();
			dah();
			dah();
			dah();
			dah();
			break;
		case ' ':
			characterSpace();
			break;
	}

	characterSpace();
}

void sendCallCW(char * call, int len)
{
	int i;;

	characterSpace();
	characterSpace();

	for(i=0;i< len;i++)
		sendCharacter(call[i]);
}
