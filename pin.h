/*
 * pin.h
 *
 *  Created on: Dec 30, 2019
 *      Author: Martin
 */

#ifndef INC_PIN_H_
#define INC_PIN_H_

#include "main.h"
#ifdef USEPINS

typedef struct pin {
	GPIO_TypeDef *Port;
	uint16_t num;
} pin_t;

#endif

#endif /* INC_PIN_H_ */
