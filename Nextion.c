/*
 * Nexion.c
 *
 *  Created on: Apr 6, 2021
 *      Author: Martin
 */

#ifdef USENEXTION
#include <Nextion.h>

#define MAXLOGROWS 6
#define MAXLOGCHAR 40

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const char *const nexnumbers[] = {"x1", "x2", "x3", "x4", "x5", "x6"};
const char *nextb = "textbox";

UART_HandleTypeDef *nextionuart;
uint8_t Cmd_End[3] = {0xff,0xff,0xff};
uint8_t RxData = 0;
char logbuffer[MAXLOGROWS][MAXLOGCHAR];		//to store the information displayed on the nextion display
uint8_t logindex = 0;

uint8_t NEXTION_Init(UART_HandleTypeDef *uart) {
	nextionuart = uart;

	HAL_UART_Receive_IT(nextionuart, &RxData, 1);

	return 0;
}

void NEXTION_Send(uint8_t* string) {
	HAL_UART_Transmit(nextionuart, string, strlen((char*)string), 100);
	HAL_UART_Transmit(nextionuart, Cmd_End, 3, 100);
}

void NEXTION_SendNum (const char *obj, int32_t num)
{
	uint8_t *buffer = malloc(30*sizeof (char));
	sprintf ((char *)buffer, "%s.val=%ld", obj, num);
	NEXTION_Send(buffer);
	free(buffer);
}

void NEXTION_SendFloat (const char *obj, float num, int dp)
{
	// convert to the integer
	int32_t number = num*(pow(10,dp));

	uint8_t *buffer = malloc(30*sizeof (char));
	sprintf ((char *)buffer, "%s.vvs1=%d", obj, dp);
	NEXTION_Send(buffer);

	sprintf ((char *)buffer, "%s.val=%ld", obj, number);
	NEXTION_Send(buffer);
	free(buffer);
}

void NEXTION_SendText(const char *obj, const char *text) {
	uint8_t *buffer = malloc(255*sizeof (char));

	sprintf ((char *)buffer, "%s.txt=\"%s\"", obj, text);
	NEXTION_Send(buffer);

	free(buffer);
}

void NEXTION_SendQR (const char *obj, const char *str)
{
	uint8_t *buffer = malloc(80*sizeof (char));

	NEXTION_Send((uint8_t*)"page 1");

	sprintf ((char *)buffer, "%s.txt=\"%s\"", obj, str);
	NEXTION_Send(buffer);
	free(buffer);
}

void NEXTION_AddtoLog(const char *obj, const char *text) {
	uint8_t *buffer = malloc((MAXLOGROWS * (MAXLOGCHAR + 3) + 10) * sizeof (char));

	for(uint16_t i=0; i<MAXLOGCHAR; i++) logbuffer[logindex][i] = '\0';

	strncpy(logbuffer[logindex], text, fmin(strlen(text)-1, (MAXLOGCHAR - 2)));

	sprintf((char*)buffer, "%s.txt=\"\"", obj);
	NEXTION_Send(buffer);

	for(int16_t i = logindex; i>=0; i--) {
		sprintf((char*)buffer, "%s.txt+=\"", obj);
		strcat((char*)buffer, logbuffer[i]);
		strcat((char*)buffer, "\\r\"");
		NEXTION_Send(buffer);
	}

	for(int16_t i = MAXLOGROWS - 1; i>logindex; i--) {
		sprintf((char*)buffer, "%s.txt+=\"", obj);
		strcat((char*)buffer, logbuffer[i]);
		strcat((char*)buffer, "\\r\"");
		NEXTION_Send(buffer);
	}

	logindex++;
	if(logindex >= MAXLOGROWS) {
		logindex=0;
	}

	free(buffer);
}

void NEXTION_ClearLog(const char *obj) {

	logindex = 0;

	for(uint8_t i=0; i<MAXLOGROWS; i++) {
		for(uint16_t j=0; j<MAXLOGCHAR; j++) {
			logbuffer[i][j] = '\0';
		}
	}

	NEXTION_SendText(obj, "");

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (RxData == 0x01)
	{
		NEXTION_Send((uint8_t*)"page 1");
	}

	if (RxData == 0x02)
	{
		NEXTION_Send((uint8_t*)"page 0");
	}

	 HAL_UART_Receive_IT(huart, &RxData, 1);
}

#endif
