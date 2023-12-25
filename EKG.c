/*
 * EGK.c
 *
 *  Created on: Nov 8, 2020
 *      Author: Dietz
 */

#include "EKG.h"
#ifdef USEEKG

#include "string.h"


#define EKGDEBUG 0
#define BUFFERSIZE 128
#define BLOCKSIZE 32
#define BEATBUFFERSIZE 127
#define STARTTHRESH 2600
#define STARTRINGBUFSIZE 10
#define SENDINBLOCKS 1
#define SENDINFRAMES 0
#define LOGSTATS 1
#define IDENTIFIER "EKG_Testdevice_STM32F411"

uint8_t equals(char *, char *, uint32_t);
uint8_t addToBuf(uint16_t *, uint16_t *, uint16_t *, uint16_t);
uint8_t addToBufBef(uint16_t);
uint8_t addToBufBeat(uint16_t);
uint8_t addToBufAft(uint16_t);
void sendSinglePoint(uint16_t);
void sendBuf(uint8_t *, uint16_t *, uint8_t *, uint16_t, uint16_t);
void sendBufBef();
void sendBufBeat();
void sendBufAft();

uint8_t ringBufRdy = 0;
uint16_t ringBufCount = 0;
uint16_t ringBuf[STARTRINGBUFSIZE];

float statmaxfreq = 0;
float statminfreq = 100000;
float statavgfreq = 0;
uint32_t EKGmaxAmplpre = 0;
uint32_t EKGminAmplpre = 100000;
uint32_t EKGavgAmplpre = 0;
uint32_t EKGmaxAmplbeat = 0;
uint32_t EKGminAmplbeat = 100000;
uint32_t EKGavgAmplbeat = 0;
uint32_t EKGmaxAmplaft = 0;
uint32_t EKGminAmplaft = 100000;
uint32_t EKGavgAmplaft = 0;
uint32_t EKGmaxAmpl = 0;
uint32_t EKGminAmpl = 100000;
uint32_t EKGavgAmpl = 0;

uint8_t curState = 0;
uint16_t curThreshold = 0;
uint8_t sending = 0;
uint8_t cursending = 0;
uint8_t *curIdentifier = IDENTIFIER;
uint16_t curRingBufSize = STARTRINGBUFSIZE;

uint16_t blockBuffer[BLOCKSIZE];
uint32_t blockiter = 0;

uint16_t startBufBef = 0;
uint16_t curBufBef = 0;
uint16_t bufferBefore[BUFFERSIZE];
uint16_t startBufBeat = 0;
uint16_t curBufBeat = 0;
uint16_t bufferBeat[BUFFERSIZE];
uint16_t maxAmpl = 0;
uint32_t timeMaxAmpl = 0;
uint32_t lasttimeMaxAmpl = 0;
uint16_t startBufAft = 0;
uint16_t curBufAft = 0;
uint16_t bufferAfter[BUFFERSIZE];

uint16_t delaycount = 0;

uint8_t curHFreq = 0;

void EKGinit() {
	curState = 0;
	curThreshold = STARTTHRESH;
	curRingBufSize = STARTRINGBUFSIZE;

	ringBufRdy = 0;
	ringBufCount = 0;

	startBufBef = 0;
	curBufBef = 0;
	startBufBeat = 0;
	curBufBeat = 0;
	startBufAft = 0;
	curBufAft = 0;

	delaycount = 0;

	maxAmpl = 0;
	timeMaxAmpl = 0;
	curHFreq = 0;

	cursending = sending;

	statmaxfreq = 0;
	statminfreq = 100000;
	statavgfreq = 0;

	EKGmaxAmplpre = 0;
	EKGminAmplpre = 100000;
	EKGavgAmplpre = 0;

	EKGmaxAmplbeat = 0;
	EKGminAmplbeat = 100000;
	EKGavgAmplbeat = 0;

	EKGmaxAmplaft = 0;
	EKGminAmplaft = 100000;
	EKGavgAmplaft = 0;

	if(EKGmaxAmpl > 0) {
		curThreshold = EKGmaxAmpl * 0.8;
	}

	EKGmaxAmpl = 0;
	EKGminAmpl = 100000;
	EKGavgAmpl = 0;

	blockiter = 0;
}

void parseEKGString(uint8_t* Buf, uint32_t *Len) {
	char tempstring[*Len];
	for(uint8_t i=0; i<(*Len-1); i++) {
		tempstring[i] = Buf[i];
	}
	tempstring[*Len-1] = '\0';

	if(equals("*IDN?", tempstring, *Len)) {
		CDC_Transmit_FS(getIdentifier(), strlen(getIdentifier()));
	} else if(equals("*START", tempstring, *Len)) {
		setSending(1);
	} else if(equals("*STOP", tempstring, *Len)) {
		setSending(0);
	}
}

uint8_t equals(char *string1, char *string2, uint32_t len) {
	for(uint32_t i=0; i<len; i++) {
		if(string1[i] != string2[i]) {
			return 0;
		}
	}
	return 1;
}

uint8_t getSending() {
	return sending;
}

uint8_t setSending(uint8_t newsending) {
	uint8_t tempsending = sending;
	sending = newsending;
	return tempsending;
}

uint8_t * getIdentifier() {
	return curIdentifier;
}

uint8_t * getHardIdentifier() {
	return IDENTIFIER;
}

void setIdentifier(uint8_t *newIdentifier) {
	strcpy((char*)curIdentifier, (char*)newIdentifier);
}

uint32_t average(uint32_t *values, uint8_t numValues) {
	uint32_t tempvalues[numValues];
	for(uint8_t i=0; i<numValues; i++) {
		tempvalues[i] = values[i];
	}

	uint32_t tempvalue = 0;
	for(uint8_t i=0; i<numValues; i++) {
		tempvalue += tempvalues[i];
	}

	return ((float)tempvalue + 0.5) / numValues;
}

uint32_t medianaverage(uint32_t *values, uint8_t numValues, uint8_t nummeanvalues) {
	uint32_t tempvalues[numValues];
	for(uint8_t i=0; i<numValues; i++) {
		tempvalues[i] = values[i];
	}

	uint8_t switches = 0;
	do {
		switches = 0;
		for(uint8_t i=0; i<(numValues-1); i++) {
			if(tempvalues[i] > tempvalues[i+1]) {
				switches++;
				dreieckstausch(&(tempvalues[i]), &(tempvalues[i+1]));
			}
		}
	} while (switches>0);

	uint32_t tempvalue = 0;
	uint8_t startindex = (numValues - nummeanvalues) / 2;
	for(uint8_t i=startindex; i<(nummeanvalues+startindex); i++) {
		tempvalue += tempvalues[i];
	}

	return ((float)tempvalue + 0.5) / nummeanvalues;
}

void dreieckstausch(uint32_t *value1, uint32_t *value2) {
	uint32_t tempvar = *value1;
	*value1 = *value2;
	*value2 = tempvar;
}

void addNewEKGPoint(uint16_t singleNewPoint) {

	ringBuf[ringBufCount] = singleNewPoint;

	if(ringBufCount < curRingBufSize) {
		ringBufCount++;
	} else {
		ringBufCount = 0;
		ringBufRdy = 1;
	}

	if(!ringBufRdy) {
		return;
	}

	uint32_t tempvalue = ringBuf[0];
	for(uint16_t i=1; i<curRingBufSize; i++) {
		tempvalue += ringBuf[i];
	}

	uint16_t newPoint = ((float)tempvalue + 0.5) / curRingBufSize;

#if LOGSTATS
	if(newPoint > EKGmaxAmpl) EKGmaxAmpl = newPoint;
	if(newPoint < EKGminAmpl) EKGminAmpl = newPoint;
#endif
#if SENDINFRAMES
	if(curState == 0) {
#if LOGSTATS
	if(newPoint > EKGmaxAmplpre) EKGmaxAmplpre = newPoint;
	if(newPoint < EKGminAmplpre) EKGminAmplpre = newPoint;
#endif
		if(newPoint <= curThreshold) {
			delaycount = 0;
		} else {
			delaycount++;
		}
#if EKGDEBUG
		if(addToBufBef(newPoint)) {
			delaycount = 0;
			curState = 1;
			sendBufBef();
		}
#else
		addToBufBef(newPoint);
		if(delaycount >= 3) {
			delaycount = 0;
			curState = 1;
			sendBufBef();
		}
#endif

	} else if(curState == 1) {
#if LOGSTATS
	if(newPoint > EKGmaxAmplbeat) EKGmaxAmplbeat = newPoint;
	if(newPoint < EKGminAmplbeat) EKGminAmplbeat = newPoint;
#endif
		if(newPoint > curThreshold) {
			delaycount = 0;
			if(newPoint > maxAmpl) {
				maxAmpl = newPoint;
				timeMaxAmpl = HAL_GetTick();
			}
		} else {
			delaycount++;
		}
#if EKGDEBUG
		if(addToBufBeat(newPoint)) {
			delaycount = 0;
			curHFreq = (float)60 / ((timeMaxAmpl - lasttimeMaxAmpl) / 1000) + 0.5;
			lasttimeMaxAmpl = timeMaxAmpl;
			curState = 2;
			sendBufBeat();
		}
#else
		addToBufBeat(newPoint);
		if(delaycount >= 3) {
			delaycount = 0;
			curHFreq = (float)60 / ((timeMaxAmpl - lasttimeMaxAmpl) / 1000) + 0.5;
			lasttimeMaxAmpl = timeMaxAmpl;
			curState = 2;
			sendBufBeat();
		}
#endif

	} else if(curState == 2) {
#if LOGSTATS
	if(newPoint > EKGmaxAmplaft) EKGmaxAmplaft = newPoint;
	if(newPoint < EKGminAmplaft) EKGminAmplaft = newPoint;
#endif
		addToBufAft(newPoint);
		delaycount++;
		if(delaycount >= BUFFERSIZE) {
			sendBufAft();
			EKGinit();
		}
	}
#else
	if(sending) {
		sendSinglePoint(newPoint);
	}
#endif
}

uint8_t getCurEKGState() {
	return curState;
}

uint8_t addToBuf(uint16_t *buffer, uint16_t* startindex, uint16_t* stopindex, uint16_t newPoint) {
	uint8_t overflow = 0;
	uint16_t startPos = *startindex;
	uint16_t curPos = *stopindex;

	if(curPos == (BUFFERSIZE - 1)) {
		curPos = 0;
	} else {
		curPos++;
	}

	if((curPos == startPos) && (startPos == (BUFFERSIZE - 1))) {
		startPos = 0;
		overflow = 1;
	} else if (curPos == startPos) {
		startPos++;
	}

	buffer[curPos] = newPoint;

	*startindex = startPos;
	*stopindex = curPos;

	return overflow;
}

uint8_t addToBufBef(uint16_t newPoint) {
	return addToBuf(bufferBefore, &startBufBef, &curBufBef, newPoint);
}

uint8_t addToBufBeat(uint16_t newPoint) {
	return addToBuf(bufferBeat, &startBufBeat, &curBufBeat, newPoint);
}

uint8_t addToBufAft(uint16_t newPoint) {
	return addToBuf(bufferAfter, &startBufAft, &curBufAft, newPoint);
}

void sendEGKData() {
	sendBufBef();
	sendBufBeat();
	sendBufAft();
}

void sendSinglePoint(uint16_t EKGPoint) {
#if SENDINBLOCKS
	blockBuffer[blockiter] = EKGPoint;
	blockiter++;

	if(blockiter == BLOCKSIZE) {
		char sendbuffer[BLOCKSIZE * (5 + 1) + 1];
		sendbuffer[0] = '\0';
		for(uint32_t i=0; i<(BLOCKSIZE-1); i++) {
			char tempbuffer[6 + 1];
			sprintf(tempbuffer, "%05d\n", blockBuffer[i]);
			strcat(sendbuffer, tempbuffer);
		}
		sendbuffer[BLOCKSIZE * (5 + 1) - 1] = '\n';
		sendbuffer[BLOCKSIZE * (5 + 1)] = '\0';
		uint8_t temptries = 0;
		uint8_t usbstatus = USBD_BUSY;
		while (temptries < 3 && usbstatus != USBD_OK) {
			if((usbstatus = CDC_Transmit_FS(sendbuffer, strlen(sendbuffer))) != USBD_OK) {
				temptries++;
				HAL_Delay(1);
			}
		}
		blockiter = 0;
	} else {
		return;
	}
#else
	char sendbuffer[5 + 2];
	sprintf(sendbuffer, "%05d\n", EKGPoint);
	uint8_t temptries = 0;
	uint8_t usbstatus = USBD_BUSY;
	while (temptries < 3 && usbstatus != USBD_OK) {
		if((usbstatus = CDC_Transmit_FS(sendbuffer, strlen(sendbuffer))) != USBD_OK) {
			temptries++;
			HAL_Delay(1);
		}
	}
#endif
}

void sendBuf(uint8_t *prefix, uint16_t *buffer, uint8_t *postfix, uint16_t startindex, uint16_t stopindex) {
	if(!cursending) {
		return;
	}
	char sendbuffer[1000];
	strcpy(sendbuffer, (char*)prefix);
	uint8_t finished = 0;
	uint16_t curIndex = startindex;
	while(!finished) {
		if(curIndex == stopindex) {
			finished = 1;
			char tempbuffer[5 + 1];
			sprintf(tempbuffer, "%05d", buffer[curIndex]);
			strcat(sendbuffer, tempbuffer);
			continue;
		} else if(curIndex == (BUFFERSIZE -1)) {
			curIndex = 0;
		} else {
			curIndex++;
		}

		char tempbuffer[5 + 2];
		sprintf(tempbuffer, "%05d;", buffer[curIndex]);
		strcat(sendbuffer, tempbuffer);
	}

	strcat((char*)sendbuffer, (char*)postfix);

	uint8_t temptries = 0;
	uint8_t usbstatus = USBD_BUSY;
	while (temptries < 3 && usbstatus != USBD_OK) {
		if((usbstatus = CDC_Transmit_FS(sendbuffer, strlen(sendbuffer))) != USBD_OK) {
			temptries++;
			HAL_Delay(1);
		}
	}
}

void sendBufBef() {
	uint8_t prefix[] = ":START:preBeat:";
	uint8_t postfix[] = ":";
	sendBuf(prefix, bufferBefore, postfix, startBufBef, curBufBef);
}

void sendBufBeat() {
	uint8_t prefix[50];
	sprintf((char*)prefix, "Beat:%ld:%03d:", lasttimeMaxAmpl, curHFreq);
	uint8_t postfix[] = ":";
	sendBuf(prefix, bufferBefore, postfix, startBufBef, curBufBef);
}

void sendBufAft() {
	uint8_t prefix[] = "postBeat:";
	uint8_t postfix[] = ":END:\n";
	sendBuf(prefix, bufferBefore, postfix, startBufBef, curBufBef);
}

#endif
