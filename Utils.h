/*
 * Utils.h
 *
 *  Created on: 21.01.2020
 *      Author: Martin
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "main.h"

uint8_t DWT_Delay_Init(void);

uint32_t DWT_get_us(void);
void DWT_Delay_us(volatile uint32_t);

uint32_t diffTime(uint32_t starttime, uint32_t endtime);

uint8_t findParaInt (char *, uint16_t, int32_t *);
uint8_t findParaFloat (char *, uint16_t, float *);

float clamp(float, float, float);

int32_t mapint(int32_t lowold, int32_t highold, int32_t lownew, int32_t highnew, int32_t value);
uint32_t mapuint(uint32_t lowold, uint32_t highold, uint32_t lownew, uint32_t highnew, uint32_t value);
float mapfloat(float lowold, float highold, float lownew, float highnew, float value);

#endif /* INC_UTILS_H_ */
