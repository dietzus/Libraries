/*
 * Utils.c
 *
 *  Created on: 21.01.2020
 *      Author: Martin
 */

#include "Utils.h"
#include "string.h"
#include <ctype.h>
#include <stdio.h>
//#include "stm32l4xx_hal.h"
//#include "modules/flash/inc/flash.h"

#ifdef STM32F1
#include "system_stm32f1xx.h"
#endif
#ifdef STM32F3
#include "system_stm32f3xx.h"
#endif
#ifdef STM32F4
#include "system_stm32f4xx.h"
#endif
#ifdef STM32H7
#include "system_stm32h7xx.h"
#endif

#define DTZ '.'

uint8_t usdiv = 1;

uint8_t DWT_Delay_Init(void) {
	SystemCoreClockUpdate();

// Disable TRC
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000
// Enable TRC
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000
//	Disable Clock Cycle Counter
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001
//	Enable Clock Cycle Counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //~0x00000001
//	Reset the clock cycle counter value
	DWT->CYCCNT = 0;
//	3 NOP Operations as a function test
	asm("NOP");
	asm("NOP");
	asm("NOP");
//	Check if the counter has actually started
	if(DWT->CYCCNT) {
		usdiv = HAL_RCC_GetHCLKFreq() / 1000000;
		return 0; //Clock cycle counter did start
	} else {
		return 1; //Clock cycle counter
	}
}

uint32_t DWT_get_us(void) {
	return DWT->CYCCNT / usdiv;
}

/**
 * @brief This function provides a delay (in microseconds)
 * @param microseconds: delay in microseconds
 */
void DWT_Delay_us(volatile uint32_t microseconds) {
	uint32_t clk_cycle_start = DWT_get_us();
	/* Delay till end */
	while ((DWT_get_us() - clk_cycle_start) < microseconds);
}

uint32_t diffTime(uint32_t starttime, uint32_t endtime) {
	if (starttime < endtime) {
		return (endtime - starttime);
	} else {
		return ((UINT32_MAX - starttime) + endtime);
	}
}

uint8_t findParaInt (char *string, uint16_t i, int32_t *para) {

	int sign = 1;
	if(string[i] == '-') {
		i++;
		if (i >= strlen(string)) {
			return 0;
		}
		sign = -1;
	} else if(!isdigit((unsigned char)string[i])) {
		return 0;
	}

	int tempint = string[i] - 48;
	i++;
	while((i < strlen(string)) && isdigit((unsigned char)string[i])) {
		tempint = (tempint * 10) + (string[i] - 48);
		i++;
	}
	*para = tempint * sign;

	return 1;
}

uint8_t findParaFloat (char *string, uint16_t pos, float *para) {
//	char *string = (char*)origstring;
	int sign = 1;
	if(string[pos] == '-') {
		pos++;
		if (pos >= strlen(string)) {
			return 0;
		}
		sign = -1;
	} else if(!isdigit((unsigned char)string[pos])) {
		return 0;
	}

	unsigned int j = 1;
	unsigned int tempint = 0;
	int nachkomma = 0;
	while(isdigit((unsigned char)string[pos]) || (string[pos] == DTZ)) {
		if(string[pos] == DTZ) {
			if(nachkomma == 1) {
				return 0;
			} else {
				nachkomma = 1;
			}
		} else {
			tempint = (tempint * 10) + (string[pos] - 48);
			if(nachkomma) {
				j *= 10;
			}
		}
		pos++;
	}

	*para = ((float)tempint) / j * sign;

	return 1;
}

float clamp(float x, float lowerlimit, float upperlimit) {
  if (x < lowerlimit)
    x = lowerlimit;
  if (x > upperlimit)
    x = upperlimit;
  return x;
}

int32_t mapint(int32_t lowold, int32_t highold, int32_t lownew, int32_t highnew, int32_t value) {
	return (1 - ((float)(highold - value) / (highold - lowold))) * (highnew - lownew) + lownew;
}

uint32_t mapuint(uint32_t lowold, uint32_t highold, uint32_t lownew, uint32_t highnew, uint32_t value) {

	//lowold = 40, highold = 50, lownew = 20, highnew = 30, value = 45
	//                   5                   10                    10=5           20
	float tempfact = (float)(highold - value) / (highold - lowold);
	tempfact = clamp(tempfact, 0.001, 1);
	return (1 - tempfact) * (highnew - lownew) + lownew;
}

float mapfloat(float lowold, float highold, float lownew, float highnew, float value) {
	return (1 - ((highold - value) / (highold - lowold))) * (highnew - lownew) + lownew;
}
