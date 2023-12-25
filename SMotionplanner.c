/*
 * Motionplanner.c
 *
 *  Created on: Jun 2, 2020
 *      Author: Martin
 */
#ifdef USESMOOTHMOTION
#include "SMotionplanner.h"
#include "Utils.h"
#include "stdlib.h"
#include "string.h"

#include "Smoothstep.h"

uint8_t initialized = 0;
pin_t steppins[NRAXIS];
pin_t dirpins[NRAXIS];
pin_t enpins[NRAXIS];
pin_t endstpins[NRAXIS];
uint8_t curNrAxis = 0;
int32_t curPos[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int32_t tarPos[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int8_t curDir[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int32_t offset[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint16_t startV[NRAXIS] = { 500, 500, 500, 500, 500, 500 };
uint16_t maxV[NRAXIS] = { 1, 1, 1, 1, 1, 1 };
uint16_t curV[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
float a[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
float scalef[NRAXIS] = { 1, 1, 1, 1, 1, 1 };
Smoothstep_t smooth[NRAXIS];
int32_t sRamp[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int32_t eRamp[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int32_t curacount[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint32_t lastStep[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint32_t nextStep[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint32_t nextLow[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint8_t stepped[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint8_t axismoving[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
uint32_t curSteps[NRAXIS] = { 0, 0, 0, 0, 0, 0 };
int8_t curmoving = 0;
uint8_t busy = 0;

uint8_t paraVal[NRAXIS] = { 0, 0, 0, 0, 0, 0};
int32_t paraPos[NRAXIS] = { 0, 0, 0, 0, 0, 0};

uint8_t calculated = 0;

#if MYDEBUG
int32_t lastX = 0; //for DEBUG purposes
int32_t lastY = 0; //for DEBUG purposes
#endif

uint8_t initDefaultPins(uint8_t num) {
	if(num > 3) {
		return 1;
	}
	initialized = 0;
	curNrAxis = num;

	steppins[0].Port = X_Step_GPIO_Port;
	steppins[0].num = X_Step_Pin;
	dirpins[0].Port = X_Dir_GPIO_Port;
	dirpins[0].num = X_Dir_Pin;
	enpins[0].Port = En_Pin_GPIO_Port;
	enpins[0].num = En_Pin_Pin;
	endstpins[0].Port = X_Endst_GPIO_Port;
	endstpins[0].num = X_Endst_Pin;

	if(num > 1) {
		steppins[1].Port = Y_Step_GPIO_Port;
		steppins[1].num = Y_Step_Pin;
		dirpins[1].Port = Y_Dir_GPIO_Port;
		dirpins[1].num = Y_Dir_Pin;
		enpins[1].Port = En_Pin_GPIO_Port;
		enpins[1].num = En_Pin_Pin;
		endstpins[1].Port = Y_Endst_GPIO_Port;
		endstpins[1].num = Y_Endst_Pin;
	}
	if(num > 2) {
		steppins[2].Port = Z_Step_GPIO_Port;
		steppins[2].num = Z_Step_Pin;
		dirpins[2].Port = Z_Dir_GPIO_Port;
		dirpins[2].num = Z_Dir_Pin;
		enpins[2].Port = En_Pin_GPIO_Port;
		enpins[2].num = En_Pin_Pin;
		endstpins[2].Port = Z_Endst_GPIO_Port;
		endstpins[2].num = Z_Endst_Pin;
	}

	initialized = 1;
	return 0;
}

uint8_t initDefaultSmoothstep(uint8_t num) {
	if(num > NRAXIS || initialized == 0) {
			return 1;
	}
	for(uint8_t i=0; i<curNrAxis; i++) {
		initSmoothstep(&smooth[i], 7, 100, 10000);
	}

	return 0;
}

uint8_t initPins(pin_t* newsteppins[], pin_t* newdirpins[], pin_t* newenpins[], uint8_t num) {
	if(num > NRAXIS) {
		return 1;
	}
	initialized = 0;
	curNrAxis = num;
	for(uint8_t i=0; i<curNrAxis; i++) {
		steppins[i].Port = newsteppins[i]->Port;
		steppins[i].num = newsteppins[i]->num;
		dirpins[i].Port = newdirpins[i]->Port;
		dirpins[i].num = newdirpins[i]->num;
		enpins[i].Port = newenpins[i]->Port;
		enpins[i].num = newenpins[i]->num;
	}

	initialized = 1;
	return 0;
}

uint8_t calcSMove(GCodePara_t gcode) {
	if(!initialized) {
		return 1;
	}

#if MYDEBUG
	if(gcode.validX) {
		int32_t tempX = gcode.X;
		int32_t diffX = tempX - lastX;
		if(tempX > 30 && diffX > 1) {
			__asm("NOP");
		}
		lastX = tempX;
	}
	if(gcode.validY) {
		int32_t tempY = gcode.Y;
		int32_t diffY = tempY - lastY;
		if(tempY > 20 && diffY > 1) {
			__asm("NOP");
		}
		lastY = tempY;
	}
#endif

	calculated = 0;
	busy = 1;

	if(gcode.validX) {
		paraPos[0] = gcode.X * SPMM[0] - offset[0];
	}
	if(gcode.validY) {
		paraPos[1] = gcode.Y * SPMM[1] - offset[1];
	}
	if(gcode.validZ) {
		paraPos[2] = gcode.Z * SPMM[2] - offset[2];
	}
	if(gcode.validE) {
		paraPos[3] = gcode.E * SPMM[3] - offset[3];
	}

	curmoving = 0;
	for(uint8_t i=0; i<curNrAxis; i++) {
		if(paraVal[i]) {
#if MYDEBUG
			if(i == 1) {
				__asm("NOP");
			}
#endif
			if(paraPos[i] != curPos[i]) {
				tarPos[i] = paraPos[i];
				axismoving[i] = 1;
				curmoving++;
				if(tarPos[i] > curPos[i]) {
					curSteps[i] = tarPos[i] - curPos[i];
					SetDirFor(i);
				} else {
					curSteps[i] = curPos[i] - tarPos[i];
					SetDirRev(i);
				}
				SetEn(i);
			} else {
				curSteps[i] = 0;
//				SetDis(i);
			}
		}
	}

	float maxSteps = 0;
	uint8_t maxSPMM = 1;
	for(uint8_t i=0; i<curNrAxis; i++) {
		if((curSteps[i] / SPMM[i]) > (maxSteps / maxSPMM)) {
			maxSteps = curSteps[i];
			maxSPMM = SPMM[i];
		}
	}


	for(uint8_t i=0; i<curNrAxis; i++) {
			if(curSteps[i] > 0) {
				scalef[i] = maxSteps / curSteps[i];

#ifdef USESIMPLESTEP
				maxV[i] = MAXV[i] * scalef[i]; /// maxSPMM * SPMM[i];
				if(maxV[i] > MINV[i]) {
					maxV[i] = MINV[i];
				}
				a[i] = A[i] * scalef[i] * maxSPMM / SPMM[i];
				if(a[i] > MAXA[i]) {
					a[i] = MAXA[i];
				} else if (a[i] < 1) {
					a[i] = 1;
				}

				curV[i] = startV[i] * scalef[i];
				if(curV[i] > MINV[i]) {
					curV[i] = MINV[i];
				}


				if(getRamps(i)) {
					return 1;
				}
#else
				if(smooth->calculated == 0) {
					initSmoothstep(&smooth[i], 7, 50, 2000);		//TODO: use correct values
				}
				smooth[i].pos0 = curPos[i];
				smooth[i].posn = tarPos[i];
#endif
				nextStep[i] = 0;
				nextLow[i] = minHighTime[i];

				curacount[i] = 0;
			}
		}

	DWT->CYCCNT = 0;

	calculated = 1;

	HAL_TIM_Base_Start_IT(&htim1);

	return 0;
}

uint8_t getRamps(uint8_t axis) {
	int32_t tempramp = (curV[axis] - maxV[axis]) * a[axis] + 1;

	if(tempramp < 1) {
		return 1;
	}

	if(curSteps[axis] < 2 * tempramp) {
		if(curDir[axis] == 1) {
			sRamp[axis] = curPos[axis] + curSteps[axis] / 2;
			eRamp[axis] = tarPos[axis] - curSteps[axis] / 2 + 1;
		} else {
			sRamp[axis] = curPos[axis] - curSteps[axis] / 2;
			eRamp[axis] = tarPos[axis] + curSteps[axis] / 2 - 1;
		}
	} else {
		if(curDir[axis] == 1) {
			sRamp[axis] = curPos[axis] + tempramp;
			eRamp[axis] = tarPos[axis] - tempramp;
		} else {
			sRamp[axis] = curPos[axis] - tempramp;
			eRamp[axis] = tarPos[axis] + tempramp;
		}
	}

	return 0;
}

void getNextSmoothTime(uint8_t axis) {

	uint32_t tempt = smoothstept(&smooth[axis], curPos[axis]);

//	if(axis==1 && curPos[axis]%100==0) {
//		uint8_t tempbuffer[50];
//		sprintf(tempbuffer, "curPos:%ld\tcurTime:%ld\n", curPos[axis], tempt);
//		CDC_Transmit_FS(tempbuffer, strlen(tempbuffer));
////		HAL_Delay(100);
//	}

	nextStep[axis] = nextStep[axis] + tempt;

	nextLow[axis] = nextStep[axis] + minHighTime[axis];
}

void getNextTime(uint8_t axis) {
	curacount[axis]++;
	if(curDir[axis] == 1) {
		if(curPos[axis] < sRamp[axis]) {
			if(curacount[axis] >= a[axis]) {
				curV[axis]--;
				curacount[axis] = 0;
			}
			nextStep[axis] = lastStep[axis] + curV[axis];
		} else if (curPos[axis] > eRamp[axis]) {
			if(curacount[axis] >= a[axis]) {
				curV[axis]++;
				curacount[axis] = 0;
			}
			nextStep[axis] = lastStep[axis] + curV[axis];
		} else {
			nextStep[axis] = lastStep[axis] + curV[axis];
		}
	} else {
		if(curPos[axis] > sRamp[axis]) {
			if(curacount[axis] >= a[axis]) {
				curV[axis]--;
				curacount[axis] = 0;
			}
			nextStep[axis] = lastStep[axis] + curV[axis];
		} else if (curPos[axis] < eRamp[axis]) {
			if(curacount[axis] >= a[axis]) {
				curV[axis]++;
				curacount[axis] = 0;
			}
			nextStep[axis] = lastStep[axis] + curV[axis];
		} else {
			nextStep[axis] = lastStep[axis] + curV[axis];
		}
	}

	nextLow[axis] = nextStep[axis] + minHighTime[axis];
}

void steptick() {
	if(calculated) {
		uint32_t currenttime = DWT_get_us();

		for(uint8_t i=0; i<curNrAxis; i++) {
			if(curPos[i] == tarPos[i]) {
				if(axismoving[i]) {
					axismoving[i] = 0;
					curmoving--;
				}
				continue;
			}

			if(stepped[i] && nextLow[i] <= currenttime) {
				StepLow(i);
				stepped[i] = 0;
				curPos[i] += curDir[i];
#if USESMOOTHSTEP
				getNextSmoothTime(i);
#elif
				getNextTime(i);
#endif
			}
			if(!stepped[i] && (nextStep[i] <= currenttime)) {
				StepHigh(i);
				stepped[i] = 1;
				lastStep[i] = currenttime;
			}
		}

		if(curmoving <= 0) {
			if(!isEmptyGC()) {
				GCodePara_t temppara;
				getParaGC(&temppara);
				parsePara(temppara, 1);
			} else {
				HAL_TIM_Base_Stop_IT(&htim1);
				busy = 0;
				for(uint8_t i=0; i<curNrAxis; i++) {
					SetDis(i);
				}
				calculated = 0;
			}
		}
	}
}

uint8_t parsePara(GCodePara_t gcode, uint8_t exec) {
	if(gcode.character != 'G' && gcode.character != 'M') {
		return 1;
	}

	busy = 1;
	uint8_t countPara = 0;

	paraVal[0] = 0;
	if(gcode.validX) {
		paraVal[0] = 1;
//		paraPos[0] = gcode.X * SPMM[0] - offset[0];
		countPara++;
	}
	paraVal[1] = 0;
	if(gcode.validY) {
		paraVal[1] = 1;
//		paraPos[1] = gcode.Y * SPMM[1] - offset[1];
		countPara++;
	}
	paraVal[2] = 0;
	if(gcode.validZ) {
		paraVal[2] = 1;
//		paraPos[2] = gcode.Z * SPMM[2] - offset[2];
		countPara++;
	}
	paraVal[3] = 0;
	if(gcode.validE) {
		paraVal[3] = 1;
//		paraPos[3] = gcode.E * SPMM[3] - offset[3];
		countPara++;
	}

	uint8_t retval;
	if(exec) {
		if(gcode.character == 'G') {
			retval = parseGpara(gcode);
		} else {
			retval = parseMpara(gcode);
		}
	}

	busy = 0;

	return retval;
}

uint8_t parseGpara(GCodePara_t gcode) {
	if(gcode.character != 'G') {
		return 1;
	}

	if(gcode.number == 0) {
		if(curmoving) {
			addParaGC(gcode);
		} else {
			calcSMove(gcode);
		}
		return 0;
	} else if(gcode.number == 28) {
		home(gcode, 0);
		gcode.number = 92;
		parseMpara(gcode);
		return 0;
	} else if(gcode.number == 29) {
		home(gcode, 1);
		parsePara(gcode, 0);
		calcSMove(gcode);
		return 0;
	}

	return 1;

}

uint8_t parseMpara(GCodePara_t gcode) {
	if(gcode.character != 'M') {
		return 1;
	}

	if(gcode.number == 92) {
		for(uint8_t i=0; i<curNrAxis; i++) {
			if(paraVal[i]) {
				curPos[i] = paraPos[i];
			} else {
				curPos[i] = 0;
			}
		}
		return 0;
	} else if(gcode.number == 28) {
		for(uint8_t i=0; i<curNrAxis; i++) {
			if(paraVal[i]) {
				offset[i] = paraPos[i] + offset[i];
			}
		}
		return 0;
	}

	return 1;
}

void home(GCodePara_t gcode, uint8_t setOffset) {
	uint8_t homedone[NRAXIS] = { 0, 0, 0, 0, 0, 0};
	uint8_t done = curNrAxis;

	for(uint8_t i=0; i<curNrAxis; i++) {
		SetEn(i);
		SetDirRev(i);
		offset[i] = 0;
	}

	while(done > 0) {
		for(uint8_t i=0; i<curNrAxis; i++) {
			if(homedone[i] == 0) {
				if(HAL_GPIO_ReadPin(endstpins[i].Port, endstpins[i].num)) {
					homedone[i] = 1;
					curPos[i] = 0;
					done--;
				} else {
					StepHigh(i);
					offset[i] -= setOffset;
				}
			}
		}

		DWT_Delay_us(getAcelAtPos(offset[0], 300, 1000));
		for(uint8_t i=0; i<curNrAxis; i++) {
			StepLow(i);
		}
		DWT_Delay_us(30);
	}

	for(uint8_t i=0; i<curNrAxis; i++) {
		SetDis(i);
	}
}

int32_t getAcelAtPos(int32_t pos, uint16_t mint, uint16_t maxt) {
	int32_t tempt = maxt - abs(pos) / a[0];//A[0];
	if(tempt < mint) {
		tempt = mint;
	}
	return tempt;
}

void StepHigh(uint8_t axis) {
	HAL_GPIO_WritePin(steppins[axis].Port, steppins[axis].num, GPIO_PIN_SET);
}

void StepLow(uint8_t axis) {
	HAL_GPIO_WritePin(steppins[axis].Port, steppins[axis].num, GPIO_PIN_RESET);
}

void SetDirFor(uint8_t axis) {
	HAL_GPIO_WritePin(dirpins[axis].Port, dirpins[axis].num, GPIO_PIN_RESET);
	curDir[axis] = 1;
}

void SetDirRev(uint8_t axis) {
	HAL_GPIO_WritePin(dirpins[axis].Port, dirpins[axis].num, GPIO_PIN_SET);
	curDir[axis] = -1;
}

void SetEn(uint8_t axis) {
	HAL_GPIO_WritePin(enpins[axis].Port, enpins[axis].num, GPIO_PIN_RESET);
}

void SetDis(uint8_t axis) {
	HAL_GPIO_WritePin(enpins[axis].Port, enpins[axis].num, GPIO_PIN_SET);
}

float getCurX() {
	return (float)(curPos[0]+offset[0])/SPMM[0];
}

float getCurY() {
	return (float)(curPos[1]+offset[1])/SPMM[1];
}

float getCurZ() {
	return (float)(curPos[2]+offset[2])/SPMM[2];
}

float getCurPos(uint8_t axis) {
	return (float)(curPos[axis]+offset[axis])/SPMM[axis];
}

uint8_t isMoving() {
	return curmoving;
}
#endif
