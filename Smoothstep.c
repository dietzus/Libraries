/*
 * Smoothstep.c
 *
 *  Created on: 09.06.2021
 *      Author: BER83WI
 */

#include "Smoothstep.h"

#if USESMOOTHSTEP

#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include "math.h"

#include "usbd_cdc_if.h"

//#if SMOOTHSTEPUSBDEBUG

#if SMOOTHSTEPDEBUGBUFFER
float SmoothstepUSBdebug(int32_t startpos, int32_t endpos, float v_0, float v_1, float v_max, float a_0, float a_1, uint16_t *buf, int32_t bufsize, uint16_t nr_reps) {
#else
float SmoothstepUSBdebug(int32_t startpos, int32_t endpos, float v_0, float v_1, float v_max, float a_0, float a_1, uint16_t nr_reps) {
#endif

	uint32_t steps;
	if(startpos < endpos) {
		steps = endpos - startpos;
	} else {
		steps = startpos - endpos;
	}

	uint32_t starttime0 = HAL_GetTick();

	Smoothstep_t tempsmooths;
	for(uint32_t count=0; count<nr_reps; count++) {
		initSmoothstep(&tempsmooths, startpos, endpos, v_0, v_1, v_max, a_0, a_1);
	}

	uint32_t endtime0 = HAL_GetTick();

	float dur0 = (float)(endtime0 - starttime0) / nr_reps;

#if SMOOTHSTEPDEBUGBUFFER



#else
	float *tempsmoothstep = malloc(sizeof(float) * steps);
	float *tempsmoothstepv = malloc(sizeof(float) * steps);
	float *tempsmoothstepa = malloc(sizeof(float) * steps);
	uint32_t *tempsmoothstept = malloc(sizeof(uint32_t) * steps);
#endif

	uint32_t starttime1 = HAL_GetTick();
	for(uint32_t count=0; count<nr_reps; count++) {
		for(uint32_t temppos=0; temppos<steps; temppos++) {
			tempsmoothstep[temppos] = smoothstepint(&tempsmooths, temppos);
		}
	}
	uint32_t endtime1 = HAL_GetTick();
	float dur1 = (float)(endtime1 - starttime1) / nr_reps;

	uint32_t starttime2 = HAL_GetTick();
	for(uint32_t count=0; count<nr_reps; count++) {
		for(uint32_t temppos=0; temppos<steps; temppos++) {
			tempsmoothstepv[temppos] = smoothstepvint(&tempsmooths, temppos);
		}
	}
	uint32_t endtime2 = HAL_GetTick();
	float dur2 = (float)(endtime2 - starttime2) / nr_reps;

	uint32_t starttime3 = HAL_GetTick();
	for(uint32_t count=0; count<nr_reps; count++) {
		for(uint32_t temppos=0; temppos<steps; temppos++) {
			tempsmoothstepa[temppos] = smoothstepaint(&tempsmooths, temppos);
		}
	}
	uint32_t endtime3 = HAL_GetTick();
	float dur3 = (float)(endtime3 - starttime3) / nr_reps;

	uint32_t starttime4 = HAL_GetTick();
	for(uint32_t count=0; count<nr_reps; count++) {
		for(uint32_t temppos=0; temppos<steps; temppos++) {
			tempsmoothstept[temppos] = smoothsteptint(&tempsmooths, temppos);
		}
	}
	uint32_t endtime4 = HAL_GetTick();
	float dur4 = (float)(endtime4 - starttime4) / nr_reps;

	uint8_t tempbuf[100];
	sprintf((char*)tempbuf, "Startpos\tEndpos\tv_0\tv_1\tv_max\ta_0\ta_1\tnr_reps\n");
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);
	sprintf((char*)tempbuf, "%li\t%li\t%f\t%f\t%f\t%f\t%f\t%u\n\n", startpos, endpos, v_0, v_1, v_max, a_0, a_1, nr_reps);
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);

	sprintf((char*)tempbuf, "Shortmove\ta_0\ta_1\ta_2\ta_3\ta_4\ta_5\ta_6\ta_7\n");
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);
	sprintf((char*)tempbuf, "%u\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n\n", tempsmooths.shortmove, tempsmooths.a0, tempsmooths.a1, tempsmooths.a2, tempsmooths.a3, tempsmooths.a4, tempsmooths.a5, tempsmooths.a6, tempsmooths.a7);
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);

	sprintf((char*)tempbuf, "Dur N Inits\tDur N steps\tDur N v\tDur N a\tDur N t\n");
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);
	sprintf((char*)tempbuf, "%f\t%f\t%f\t%f\t%f\n\n", dur0, dur1, dur2, dur3, dur4);
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);

	sprintf((char*)tempbuf, "Name\tx\tstep\tv\ta\tt\t\n");
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	for(uint32_t temppos=0; temppos<steps; temppos++) {
		sprintf((char*)tempbuf, "Values:\t%f%f\t%f\t%f\t%u\n", smoothstepInttoFloat(tempsmooths.pos0, tempsmooths.posn, temppos), tempsmoothstep[temppos], tempsmoothstepv[temppos], tempsmoothstepa[temppos], (unsigned int)tempsmoothstept[temppos]);
		CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
//		HAL_Delay(1);
	}

	sprintf((char*)tempbuf, "\n");
	CDC_Transmit_FS(tempbuf, strlen((char*)tempbuf));
	HAL_Delay(1);

#if SMOOTHSTEPDEBUGBUFFER == 0
	free(tempsmoothstep);
	free(tempsmoothstepv);
	free(tempsmoothstepa);
	free(tempsmoothstept);
#endif

	return tempsmooths.shortmove;
}

//#endif

uint8_t initSmoothstep(Smoothstep_t *sm, int32_t pos_0, int32_t pos_1, float v_0, float v_1, float v_max, float a_0, float a_1) {
	sm->calculated = 0;
	sm->vmax = v_max;

	sm->a0 = pos_0;
	sm->a1 = v_0;
	sm->a2 = a_0/2;
	sm->a3 = 0;
	sm->a4 = -(30*v_1 + 40*v_0 - 70*pos_1 + 70*pos_0 - 5*a_1 + 10*a_0)/2;
	sm->a5 = 39*v_1 + 45*v_0 - 84*pos_1 + 84*pos_0 - 7*a_1 + 10*a_0;
	sm->a6 = -(68*v_1 + 72*v_0 - 140*pos_1 + 140*pos_0 - 13*a_1 + 15*a_0)/2;
	sm->a7 = 10*v_1 + 10*v_0 - 20*pos_1 + 20*pos_0 - 2*a_1 + 2*a_0;

	float tempvmax = 0.109375*sm->a7 + 0.1875*sm->a6 + 0.3125*sm->a5 + 0.5*sm->a4 + 0.75*sm->a3 + 1.0*sm->a2 + sm->a1;

	if(tempvmax > sm->vmax) {
		sm->shortmove = 0;

		sm->a4 = -((-32*v_max) + 11*v_1 + 21*v_0 - 3*a_1 + 8*a_0)/2;
		sm->a5 = ((-192*v_max) + 81*v_1 + 111*v_0 - 23*a_1 + 38*a_0)/5;
		sm->a6 = -((-64*v_max) + 30*v_1 + 34*v_0 - 9*a_1 + 11*a_0)/2;
		sm->a7 = ((-64*v_max) + 32*v_1 + 32*v_0 - 10*a_1 + 10*a_0)/7;

		int32_t temppos1 = smoothstep(sm, 1);

		sm->fullvsteps0 = pos_0 + pos_1 / 2;
		sm->fullvsteps1 = pos_0 + temppos1 - pos_1/2; // pos_0 + temppos1 / 2 + pos_1 - temppos1

		if(sm->fullvsteps0 < 0) sm->fullvsteps0 = 0;
		if(sm->fullvsteps1 < 0) sm->fullvsteps1 = 0;

		if(sm->fullvsteps0 == 0) {
			sm->fullvstepsf0 = 0;
		} else {
			sm->fullvstepsf0 = smoothstepInttoFloat(pos_0, pos_1, sm->fullvsteps0);
		}

		if(sm->fullvsteps1 == 0) {
			sm->fullvstepsf1 = 0;
		} else {
			sm->fullvstepsf1 = smoothstepInttoFloat(pos_0, pos_1, sm->fullvsteps1);
		}

	} else {
		sm->shortmove = 1;
		sm->fullvsteps0 = 0;
		sm->fullvsteps1 = 0;
	}

	sm->va1 = sm->a1;
	sm->va2 = 2 * sm->a2;
	sm->va3 = 3 * sm->a3;
	sm->va4 = 4 * sm->a4;
	sm->va5 = 5 * sm->a5;
	sm->va6 = 6 * sm->a6;
	sm->va7 = 7 * sm->a7;

	sm->calculated = 1;
	return 0;
}

float smoothstep(Smoothstep_t *sm, float x) {
	if(!sm->shortmove) {
		if(x > sm->fullvstepsf0 && x < sm->fullvstepsf1) {
			return 0; //sm->
		}
	}

	float x2 = x * x;
	float x3 = x2 * x;
	float x4 = x3 * x;
	float x5 = x4 * x;
	float x6 = x5 * x;
	float x7 = x6 * x;
	return sm->a7*x7 + sm->a6*x6 - sm->a5*x5 + sm->a4*x4 + sm->a3*x3 + sm->a2*x2 + sm->a1*x + sm->a0;
}

float smoothstepv(Smoothstep_t *sm, float x) {
	if(!sm->shortmove) {
		if(x > sm->fullvstepsf0 && x < sm->fullvstepsf1) {
			return sm->vmax;
		}
	}

	float x2 = x * x;
	float x3 = x2 * x;
	float x4 = x3 * x;
	float x5 = x4 * x;
	float x6 = x5 * x;
	return sm->va7*x6 + sm->va6*x5 + sm->va5*x4 + sm->va4*x3 + sm->va3*x2 + sm->va2*x + sm->va1;
}

float smoothstepa(Smoothstep_t *sm, float x) {
	if(!sm->shortmove) {
		if(x > sm->fullvstepsf0 && x < sm->fullvstepsf1) {
			return 0; //sm->
		}
	}

	float x2 = x * x;
	float x3 = x2 * x;
	float x4 = x3 * x;
	float x5 = x4 * x;
	return 42*sm->a7*x5 + 30*sm->a6*x4 - 20*sm->a5*x3 +12*sm->a4*x2 + 6*sm->a3*x + 2*sm->a2;
}

uint32_t smoothstept(Smoothstep_t *sm, float x) {
	if(!sm->shortmove) {
		if(x > sm->fullvstepsf0 && x < sm->fullvstepsf1) {
			return 0; //sm->
		}
	}

	float x2 = x * x;
	float x3 = x2 * x;
	float x4 = x3 * x;
	float x5 = x4 * x;
	float x6 = x5 * x;
	return sm->v2t / (sm->va7*x6 + sm->va6*x5 + sm->va5*x4 + sm->va4*x3 + sm->va3*x2 + sm->va2*x + sm->va1);
}

float smoothstepttot(Smoothstep_t *sm) {
	return (sm->a7 + sm->a6 + sm->a5 + sm->a4 + sm->a3 + sm->a2 + sm->a1);
}

//Convinience Aufrufe
float smoothstepint(Smoothstep_t *sm, uint32_t curPos) {
	return smoothstep(sm, smoothstepInttoFloat(sm->pos0, sm->posn, curPos));
}

float smoothstepvint(Smoothstep_t *sm, uint32_t curPos) {
	return smoothstepv(sm, smoothstepInttoFloat(sm->pos0, sm->posn, curPos));
}

float smoothstepaint(Smoothstep_t *sm, uint32_t curPos) {
	return smoothstepa(sm, smoothstepInttoFloat(sm->pos0, sm->posn, curPos));
}

uint32_t smoothsteptint(Smoothstep_t *sm, uint32_t curPos) {
	return smoothstept(sm, smoothstepInttoFloat(sm->pos0, sm->posn, curPos));
}

uint8_t smoothstepintegr(Smoothstep_t *sm, float divfac) {
	if(sm->calculated == 0) {
		return 1;
	}

	int32_t temptime = 0;
	float pos0 = (float)sm->pos0 / divfac; double pos02 = pos0 * pos0; double pos03 = pos0 * pos02;
	float posn = (float)sm->posn / divfac; double posn2 = posn * posn; double posn3 = posn * posn2;

	double pos04 = pos03 * pos0; double pos05 = pos04 * pos0; double pos06 = pos05 * pos0; double pos07 = pos06 * pos0;
	double posn4 = posn3 * posn; double posn5 = posn4 * posn; double posn6 = posn5 * posn; double posn7 = posn6 * posn;
	uint32_t vmin;
	if(sm->v0 > sm->vn) { vmin = sm->vn; } else { vmin = sm->v0; }
	temptime = -((320*vmin-320*sm->vmax)*posn7+(1120*sm->vmax-1120*vmin)*posn6+(1344*vmin-1344*sm->vmax)*posn5+(560*sm->vmax-560*vmin)*posn4-35*sm->vmax*posn+(320*sm->vmax-320*vmin)*pos07+(1120*vmin-1120*sm->vmax)*pos06+(1344*sm->vmax-1344*vmin)*pos05+(560*vmin-560*sm->vmax)*pos04+35*sm->vmax*pos0)/35;

	sm->time = temptime * divfac;

	return 0;
}

float smoothstepInttoFloat(int32_t startPos, int32_t endPos, int32_t curPos) {
	return (float)curPos/(abs((int)(endPos + startPos)));
}

#endif
