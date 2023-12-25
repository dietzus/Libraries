/*
 * Motionplanner.h
 *
 *  Created on: Jun 2, 2020
 *      Author: Martin
 */

#ifdef USESMOOTHMOTION
#ifndef INC_SMOTIONPLANNER_H_
#define INC_SMOTIONPLANNER_H_

#include "main.h"
#include "GCode.h"
#include "pin.h"

#define NRAXIS 6

extern TIM_HandleTypeDef htim1;

const static uint8_t SPMM[NRAXIS] = { 34, 68, 30, 30, 30, 30 };
const static uint16_t MINV[NRAXIS] = { 3500, 3500, 1500, 1500, 1500, 1500 };
const static uint16_t MAXV[NRAXIS] = { 250, 250, 250, 250, 250, 250 };
const static uint8_t MAXA[NRAXIS] = { 20, 20, 255, 255, 255, 255 };
const static uint8_t A[NRAXIS] = { 10, 10, 10, 10, 10, 10 };
const static uint8_t minHighTime[NRAXIS] = { 20, 20, 20, 20, 20, 20 };

uint8_t initDefaultPins(uint8_t);
uint8_t initDefaultSmoothstep(uint8_t);

uint8_t initPins(pin_t*[], pin_t*[], pin_t*[], uint8_t);

uint8_t calcSMove(GCodePara_t);
uint8_t parsePara(GCodePara_t, uint8_t);
uint8_t parseGpara(GCodePara_t);
uint8_t parseMpara(GCodePara_t);

uint8_t getRamps(uint8_t);
float getCurX();
float getCurY();
float getCurZ();
float getCurPos(uint8_t);
uint8_t isMoving();

void steptick();
void home(GCodePara_t, uint8_t);

int32_t getAcelAtPos(int32_t, uint16_t, uint16_t);

void StepHigh(uint8_t);
void StepLow(uint8_t);
void SetDirFor(uint8_t);
void SetDirRev(uint8_t);
void SetEn(uint8_t);
void SetDis(uint8_t);

#endif /* INC_SMOTIONPLANNER_H_ */
#endif
