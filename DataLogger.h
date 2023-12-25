/*
 * DataLogger.h
 *
 *  Created on: Apr 15, 2021
 *      Author: Martin
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include "main.h"
#ifdef USEDATALOG

typedef struct DataLogger {
	uint8_t* buffer;
	uint32_t buflen;
	uint16_t numElem;
	uint16_t bufent;
	uint8_t* filename;
} DataLogger_t;

uint8_t DLOG_init(void);


#endif
#endif /* DATALOGGER_H_ */
