
#ifdef USESICL
#include "SICL.h"

#include "main.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USEUSB
#include "usbd_cdc_if.h"
#endif

#define DTZ '.'
#define SICLBUFSIZE 16

extern uint8_t devicename[];
extern float curX;
extern float curY;
extern uint8_t deviceConnected;
extern uint8_t deviceBusy;

extern UART_HandleTypeDef huart1;
uint8_t newUARTData;
uint8_t UARTchar = 0x00;
extern uint32_t lastConnect;
uint16_t curUARTBuffPos = 0;
extern uint8_t UARTbuffer[];

SICLPara_t SICLBuffer[SICLBUFSIZE];
int8_t curSICLPara = 0;
int8_t newSICLPara = 0;
int8_t SICLparaCount = 0;

uint8_t parseSICLString(SICLPara_t *newPara, uint8_t *origstring) {
	char *string = (char*)origstring;
	newPara->valid = 0;

	GCodePara_t tempgcode;
	initGCode(&tempgcode);

	if(strstr(string, "*IDN?")) {
		newPara->valid = 1;
		newPara->reqReturn = 1;
		newPara->retValue = malloc(strlen((char*)devicename) + 1);
		strcpy((char*)newPara->retValue, (char*)devicename);
	} else if(string[1] == 'x' || string[1] == 'X') {
		if(string[2] == '?') {
			newPara->valid = 1;
			newPara->reqReturn = 1;
			newPara->retValue = malloc(6 * sizeof(char));
			sprintf((char*)newPara->retValue, "%.1f", curX);
		} else if(!(tempgcode.validX = findParaFloat(string, 2, &tempgcode.X))) {
			return tempgcode.validX;
		}
	} else if(string[1] == 'y' || string[1] == 'Y') {
		if(string[2] == '?') {
			newPara->valid = 1;
			newPara->reqReturn = 1;
			newPara->retValue = malloc(6 * sizeof(char));
			sprintf((char*)newPara->retValue, "%.1f", curY);
		} else if(!(tempgcode.validY = findParaFloat(string, 2, &tempgcode.Y))) {
			return tempgcode.validY;
		}
	} else if((string[1] == 'g' || string[1] == 'G') && string[2] == '0') {
		uint8_t i = 0;
		while(string[i] != '\n') {
			if(string[i] == 'x' || string[i] == 'X') {
				if(!(tempgcode.validX = findParaFloat(string, i+1, &tempgcode.X))) {
					return tempgcode.validX;
				}
			} else if(string[i] == 'y' || string[i] == 'Y') {
				if(!(tempgcode.validY = findParaFloat(string, i+1, &tempgcode.Y))) {
					return tempgcode.validY;
				}
			}
			i++;
		}
	} else if(strstr(string, "*CONNECTED?")) {
		newPara->valid = 1;
		newPara->reqReturn = 1;
		newPara->retValue = malloc((5 + 1) * sizeof(char));
		sprintf((char*)newPara->retValue, "%u", deviceConnected);
	} else if(strstr(string, "*STATUS?")) {
		newPara->valid = 1;
		newPara->reqReturn = 1;
		char tempbuffer[64];
		sprintf(tempbuffer, "X%.2fY%.2fM%d", curX, curY, deviceBusy);
		newPara->retValue = malloc((strlen(tempbuffer)+1) * sizeof(char));
		sprintf((char*)newPara->retValue, "%s", tempbuffer);
	} else if(strstr(string, "*TRG")) {
		newPara->valid = 1;
		newPara->reqReturn = 1;
		char tempbuffer[] = "23.45,56.1289";
		newPara->retValue = malloc((strlen(tempbuffer)+1) * sizeof(char));
		sprintf((char*)newPara->retValue, "%s", tempbuffer);
	}

	if(tempgcode.validX || tempgcode.validY) {
		newPara->valid = 1;
		newPara->reqGCode = 1;
		tempgcode.valid = 1;
		tempgcode.character = 'G';
		tempgcode.number = 0;
		newPara->GCode = malloc(sizeof(GCodePara_t));
		initGCode(newPara->GCode);
		copyGCode(newPara->GCode, &tempgcode);
	}

	//TODO: add more commands

	return newPara->valid;
}

SICLPara_t *createSICL(void) {
	SICLPara_t *temppara = (SICLPara_t*)malloc(sizeof(SICLPara_t));
	temppara->reqReturn = 0;
	temppara->retValue = 0;
	temppara->reqGCode = 0;
	temppara->valid = 0;
	temppara->GCode = 0;

	return temppara;
}

void destroySICL(SICLPara_t *Para) {
	if(Para->retValue) free(Para->retValue);
	if(Para->GCode) free(Para->GCode);
	free(Para);
	Para = NULL;
}

uint8_t addParaSICL(SICLPara_t para) {
	if(isFullSICL()) {
		return 0;
	}

	SICLBuffer[newSICLPara] = para;
	newSICLPara++;
	if(newSICLPara > (SICLBUFSIZE - 1)) {
		newSICLPara = 0;
	}
	SICLparaCount++;

	return 1;
}

uint8_t getParaSICL(SICLPara_t *temppara) {
	if(SICLparaCount == 0) {
		return 0;
	}

	int tempPos = curSICLPara;
	curSICLPara++;
	if(curSICLPara > (SICLBUFSIZE - 1)) {
		curSICLPara = 0;
	}
	if(SICLparaCount > 0) SICLparaCount--;

	*temppara = SICLBuffer[tempPos];
	return 1;
}

uint8_t isEmptySICL() {
	if(SICLparaCount == 0) {
		return 1;
	}
	return 0;
}

uint8_t isFullSICL() {
	return (SICLparaCount == (SICLBUFSIZE - 1));
}

uint8_t clearQueueSICL() {
	if(isEmptySICL()) {
		return 0;
	}
	curSICLPara = 0;
	newSICLPara = 0;
	SICLparaCount = 0;

	return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if((lastConnect + 100) < HAL_GetTick()) {
		curUARTBuffPos = 0;
		memset(UARTbuffer, '\0', UARTBUFSIZE);
		lastConnect = HAL_GetTick();
	}
	UARTbuffer[curUARTBuffPos] = UARTchar;
	HAL_UART_Receive_IT(&huart1, &UARTchar, 1);

	if(UARTchar != '\0' && curUARTBuffPos < UARTBUFSIZE) {
		curUARTBuffPos++;
	} else if (curUARTBuffPos > 0) {
		curUARTBuffPos = 0;
		newUARTData = 1;
		lastConnect = HAL_GetTick();
	}
}

uint8_t SICLSendUSB(uint8_t *data) {
	data = realloc((char*)data, strlen((char*)data) + 2);
	strcat((char*)data, "\n");

	uint8_t status = CDC_Transmit_FS(data, strlen((char*)data));

//	free(data);
	return status;
}

HAL_StatusTypeDef SICLSendUART(UART_HandleTypeDef *huart, uint8_t *data) {
	deviceBusy = 1;
	HAL_Delay(10);
	HAL_StatusTypeDef tempstatus = HAL_UART_Transmit(huart, (uint8_t*)strcat((char*)data, "\0"), strlen((char*)data) + 1, 100);
	deviceBusy = 0;
	return tempstatus;
}

HAL_StatusTypeDef SICLSendMultInt(UART_HandleTypeDef *huart, uint8_t num, int32_t *data) {
	char buffer[128];
	char tempbuffer[12];

	sprintf(buffer, "%ld", *data);
	for(uint8_t i=1; i<num; i++) {
//		strcat(buffer, ",");
		sprintf(tempbuffer, ",%ld", data[i]);
		strcat(buffer, tempbuffer);
	}

	deviceBusy = 1;
	HAL_Delay(1);
	HAL_StatusTypeDef tempstatus = HAL_UART_Transmit(huart, (uint8_t*)strcat(buffer, "\0"), strlen(buffer) + 1, 100);
	deviceBusy = 0;
	return tempstatus;
}
#endif
