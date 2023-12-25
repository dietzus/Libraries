/*
 * SICL.h
 *
 *  Created on: April 28, 2021
 *      Author: Martin
 */

#ifdef USESICL
#ifndef INC_SICL_H_
#define INC_SICL_H_

#include <stdint.h>
#include "Utils.h"
#include "GCode.h"

typedef struct SICLPara {
	uint8_t valid;
	uint8_t reqReturn;
	uint8_t *retValue;			//do NOT forget to free the memory after using this!
	uint8_t reqGCode;
	GCodePara_t *GCode;				//do NOT forget to free the memory after using this!
} SICLPara_t;

uint8_t parseSICLString(SICLPara_t *, uint8_t *);
void printSICLString(SICLPara_t, uint8_t *);

void copySICL(SICLPara_t *, SICLPara_t *);
SICLPara_t *createSICL(void);
void destroySICL(SICLPara_t *);

uint8_t addParaSICL(SICLPara_t);
uint8_t getParaSICL(SICLPara_t *);
uint8_t isEmptySICL();
uint8_t isFullSICL();
uint8_t clearQueueSICL();

uint8_t SICLSendUSB(uint8_t *);
HAL_StatusTypeDef SICLSendUART(UART_HandleTypeDef *, uint8_t *);

#endif /* INC_SICL_H_ */
#endif
