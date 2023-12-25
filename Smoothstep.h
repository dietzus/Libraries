/*
 * Smoothstep.h
 *
 *  Created on: 09.06.2021
 *      Author: BER83WI
 */

#include "main.h"

#if USESMOOTHSTEP

#ifndef SMOOTHSTEP_H_
#define SMOOTHSTEP_H_

//#include "Smoothstep.c"

#define SMOOTHSTEPUSBDEBUG 1
#define SMOOTHSTEPDEBUGBUFFER 0

typedef struct Smoothstep {
	uint8_t calculated;
	uint8_t shortmove;
	uint32_t fullvsteps0;
	uint32_t fullvsteps1;
	float fullvstepsf0;
	float fullvstepsf1;
	float a0;
	float a1;
	float a2;
	float a3;
	float a4;
	float a5;
	float a6;
	float a7;
	int8_t dir;
	int32_t pos0;
	int32_t posn;
	uint32_t v0;
	uint32_t vn;
	uint32_t aa;
	uint32_t ab;
	int32_t time;
	float vmax;
	float vtotfac;
	float v2t;
	float va1;
	float va2;
	float va3;
	float va4;
	float va5;
	float va6;
	float va7;
} Smoothstep_t;

uint8_t initSmoothstep(Smoothstep_t *, int32_t, int32_t, float , float, float, float, float);

float smoothstep(Smoothstep_t *, float);
float smoothstepv(Smoothstep_t *, float x);
float smoothstepa(Smoothstep_t *, float x);
uint32_t smoothstept(Smoothstep_t *, float);

float smoothstepttot(Smoothstep_t *);

float smoothstepint(Smoothstep_t *, uint32_t);
float smoothstepvint(Smoothstep_t *, uint32_t);
float smoothstepaint(Smoothstep_t *, uint32_t);
uint32_t smoothsteptint(Smoothstep_t *, uint32_t);
uint8_t smoothstepintegr(Smoothstep_t *, float);

float smoothstepInttoFloat(int32_t, int32_t, int32_t);

#if SMOOTHSTEPDEBUGBUFFER
float SmoothstepUSBdebug(int32_t startpos, int32_t endpos, float v_0, float v_1, float v_max, float a_0, float a_1, uint16_t *buf, int32_t bufsize, uint16_t nr_reps);
#else
float SmoothstepUSBdebug(int32_t startpos, int32_t endpos, float v_0, float v_1, float v_max, float a_0, float a_1, uint16_t nr_reps);
#endif


#endif /* SMOOTHSTEP_H_ */

#endif
