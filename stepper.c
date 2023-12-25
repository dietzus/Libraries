/*
 * stepper.c
 *
 *  Created on: Dec 4, 2021
 *      Author: Martin
 */

#include "stepper.h"
#ifdef USESTEPPERS

#include "math.h"

stepperaxis_t stepperaxis[NUMAXIS];

#if DEBUGSTEPPERTIMING
uint32_t lastBufferEndtime[NUMAXIS];
uint32_t curBufferEndtime[NUMAXIS];
uint32_t worstBufferDuration[NUMAXIS];
uint32_t lastBufferCalctime[NUMAXIS];
uint32_t curBufferCalctime[NUMAXIS];
uint32_t worstBufferCalcDuration[NUMAXIS];
#endif

HAL_StatusTypeDef StepperDMAInit(TIM_HandleTypeDef, uint32_t *, uint32_t);
uint8_t getNextTimes(uint16_t *, uint8_t, uint32_t);

HAL_StatusTypeDef StepperInit(uint8_t tempaxis, TIM_HandleTypeDef temptim) {
	if(tempaxis >= NUMAXIS) return HAL_ERROR;

	stepperaxis[tempaxis].tim = temptim;
	stepperaxis[tempaxis].curPos = 0;
	stepperaxis[tempaxis].curDir = 1;
	stepperaxis[tempaxis].curMovPos = 200000;
	stepperaxis[tempaxis].curCalcV = 50000;
	stepperaxis[tempaxis].maxv = 5000;
	stepperaxis[tempaxis].maxa = 1;
	stepperaxis[tempaxis].curMoveAccelRampEnd = (stepperaxis[tempaxis].curCalcV - stepperaxis[tempaxis].maxv) / stepperaxis[tempaxis].maxa;
	stepperaxis[tempaxis].curMoveDecelRampStart = stepperaxis[tempaxis].curMovPos - stepperaxis[tempaxis].curMoveAccelRampEnd;
	stepperaxis[tempaxis].calcFinished = 0;
	getNextTimes(stepperaxis[tempaxis].buffer, tempaxis, BUFSIZE);
	if(StepperDMAInit(stepperaxis[tempaxis].tim, (uint32_t*)stepperaxis[tempaxis].buffer, BUFSIZE) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

HAL_StatusTypeDef StepperDMAInit(TIM_HandleTypeDef temptim, uint32_t *newarray, uint32_t newsize) {
	if(HAL_TIM_Base_Start_DMA(&temptim, newarray, newsize) == HAL_OK) {
		return HAL_TIM_PWM_Start(&temptim, TIM_CHANNEL_1);
	}
	return HAL_ERROR;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	for(uint32_t i=0; i<NUMAXIS; i++) {
		if(htim->Instance == stepperaxis[i].tim.Instance) {
#if DEBUGSTEPPERTIMING
			curBufferEndtime[i] = HAL_GetTick();
			uint32_t curBufferDuration = curBufferEndtime[i] - lastBufferEndtime[i];
			if(curBufferDuration > worstBufferDuration[i]) worstBufferDuration[i] = curBufferDuration;
#endif
			if(stepperaxis[i].calcFinished < 3) {
				getNextTimes(stepperaxis[i].buffer + BUFSIZE/2, i, BUFSIZE/2);
			}
#if DEBUGSTEPPERTIMING
			curBufferCalctime[i] = HAL_GetTick() - curBufferEndtime[i];
			if(curBufferCalctime[i] > worstBufferCalcDuration[i]) worstBufferCalcDuration[i] = curBufferCalctime[i];
			lastBufferEndtime[i] = curBufferEndtime[i];
#endif
		}
	}
}

void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) {
	for(uint32_t i=0; i<NUMAXIS; i++) {
		if(htim->Instance == stepperaxis[i].tim.Instance) {
#if DEBUGSTEPPERTIMING
			curBufferEndtime[i] = HAL_GetTick();
			uint32_t curBufferDuration = curBufferEndtime[i] - lastBufferEndtime[i];
			if(curBufferDuration < worstBufferDuration[i]) worstBufferDuration[i] = curBufferDuration;
#endif
			if(stepperaxis[i].calcFinished < 3) {
				getNextTimes(stepperaxis[i].buffer, i, BUFSIZE/2);
			}
#if DEBUGSTEPPERTIMING
			curBufferCalctime[i] = HAL_GetTick() - curBufferEndtime[i];
			if(curBufferCalctime[i] > worstBufferCalcDuration[i]) worstBufferCalcDuration[i] = curBufferCalctime[i];
			lastBufferEndtime[i] = curBufferEndtime[i];
#endif
		}
	}
}

uint8_t getNextTimes(uint16_t *tempbuffer, uint8_t axis, uint32_t size) {
	int32_t curPos = stepperaxis[axis].curPos;
	uint8_t curDir = stepperaxis[axis].curDir;
	uint32_t endPos = stepperaxis[axis].curMovPos;
	uint32_t startRamp = stepperaxis[axis].curMoveAccelRampEnd;
	uint32_t endRamp = stepperaxis[axis].curMoveDecelRampStart;
	uint32_t vmax = stepperaxis[axis].maxv;
	uint32_t aAccel = stepperaxis[axis].maxa;
	uint32_t aDecel = stepperaxis[axis].maxa;
	uint32_t curV = stepperaxis[axis].curCalcV;

	for(uint32_t i=0; i<size; i++) {
		if(curPos < startRamp) {
			curV -= aAccel;
			tempbuffer[i] = curV;
		} else if (curPos > endPos) {
			uint16_t temppulse = stepperaxis[axis].tim.Instance->CCR1 - 1;
			for(uint32_t j=i; j<size; j++) {
				tempbuffer[j] = temppulse;
			}
			stepperaxis[axis].calcFinished++;
			stepperaxis[axis].curPos = curPos;
			return 1;
		} else if (curPos > endRamp) {
			curV += aDecel;
			tempbuffer[i] = curV;
		} else {
			tempbuffer[i] = vmax;
		}
		curPos += curDir;

//		tempbuffer[i] = sin((float)stepperaxis[axis].curPos/180 * M_PI) * 500 + 550;
//		stepperaxis[axis].curPos += 1;//curDir[axis];
//		if(stepperaxis[axis].curPos > 360) stepperaxis[axis].curPos = 0;
	}

	stepperaxis[axis].curCalcV = curV;
	stepperaxis[axis].curPos = curPos;
	return 0;
}

#endif
