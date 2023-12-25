/*
 * stepper.h
 *
 *  Created on: Dec 4, 2021
 *      Author: Martin
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "main.h"
#ifdef USESTEPPERS

typedef struct stepperaxis {
	uint8_t curMoving;
	int32_t curPos;
	uint32_t curCalcV;
	uint8_t calcFinished;
	int8_t curDir;
	int32_t lastPos;
	int32_t nextPos;
	uint32_t curMovPos;
	uint32_t curMoveAccelRampEnd;
	uint32_t curMoveDecelRampStart;
	uint16_t buffer[BUFSIZE];
	TIM_HandleTypeDef tim;
	uint32_t maxa;
	uint32_t maxv;
	uint32_t accelsteps;
	uint32_t decelsteps;
} stepperaxis_t;

uint8_t StepperInit(HAL_StatusTypeDef, TIM_HandleTypeDef);

#endif /* STEPPER_H_ */
#endif
