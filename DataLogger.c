/*
 * DataLogger.c
 *
 *  Created on: Apr 15, 2021
 *      Author: Martin
 */

#include "DataLogger.h"
#ifdef USEDATALOG

#include "string.h"
#include "ff.h"

#ifdef USESDIO
#define HASSD
#elif USEFATFSSD
#define HASSD
#endif

#ifdef HASSD



uint8_t DLOG_sendData(DataLogger_t log, uint8_t* string) {
#ifdef USESDIO
	FRESULT fresult;
	FileH_MountSD("/");
	fresult = FileH_UpdateFile(log.filename, log.buffer);
	FileH_UnmountSD("/");
	return fresult;
#elif USEFATFSSD


#endif
	return 1;
}

uint8_t DLOG_init(void) {


	return 0;
}

uint8_t DLOG_setMode(DataLogger_t log, uint8_t mode) {
	return 0;
}

uint8_t DLOG_addData(DataLogger_t log, uint8_t* data, uint32_t size) {
	return 0;
}

uint8_t DLOG_flush(DataLogger_t log) {
#ifdef USESDIO
	FRESULT fresult;
	FileH_MountSD("/");
	fresult = FileH_UpdateFile(log.filename, log.buffer);
	FileH_UnmountSD("/");
	return fresult;
#elif USEFATFSSD


#endif
	return 1;
}




#endif
#endif
