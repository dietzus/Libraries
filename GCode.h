/*
 * GCode.h
 *
 *  Created on: Dec 8, 2019
 *      Author: Martin
 */

#ifndef INC_GCODE_H_
#define INC_GCODE_H_

#ifdef USEGCODE
#include "Utils.h"
#include <stdint.h>

typedef struct GCodePara {
	uint8_t valid;
	char character;
	uint16_t number;
	uint8_t validP;
	int32_t P;
	uint8_t validS;
	int32_t S;
	uint8_t validI;
	int32_t I;
	uint8_t validJ;
	int32_t J;
	uint8_t validK;
	int32_t K;
	uint8_t validO;
	float O;
	uint8_t validX;
	float X;
	uint8_t validY;
	float Y;
	uint8_t validZ;
	float Z;
	uint8_t validE;
	float E;
	uint8_t validF;
	int32_t F;
} GCodePara_t;

GCodePara_t parseGCString(uint8_t *);
void initGCode(GCodePara_t *);
void printGCString(GCodePara_t, uint8_t *);
uint8_t createGCString(GCodePara_t *, uint8_t *);
void copyGCode(GCodePara_t *, GCodePara_t *);

void appendIntPara(char *, char, int32_t, uint8_t);
void appendFloatPara(char *, char, float, uint8_t);

uint8_t addParaGC(GCodePara_t);
uint8_t getParaGC(GCodePara_t *);
uint8_t isEmptyGC();
uint8_t isFullGC();
uint8_t clearQueueGC();

uint8_t isMovementGCPara(GCodePara_t *);
uint8_t isNextMovementGCPara();
uint8_t isImmediateGCPara(GCodePara_t *);
uint8_t isEmergencyGCPara(GCodePara_t *);
uint8_t isGCQueueClear(GCodePara_t *);

uint8_t getGCMovementCom(GCodePara_t *);
void exeNonMoveGCPara();
uint8_t GCMovementDone(GCodePara_t *);

void createM0String(uint8_t *, float, float, uint8_t);
uint8_t parseM0String(uint8_t *, float *, float *, uint8_t *);

#endif /* INC_GCODE_H_ */
#endif
