/*
 * DS18B20.h
 *
 *  Created on: Mar 27, 2021
 *      Author: Martin
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "main.h"

#ifdef USEDS18B20
#define MAXDS18B20SENSORS 6

uint8_t DS18B20_Init(uint8_t, GPIO_TypeDef*, uint16_t);
uint8_t DS18B20_DeInit(uint8_t);

uint8_t DS18B20_Start (uint8_t);
uint8_t DS18B20_Start_Pin (GPIO_TypeDef*, uint16_t);
void DS18B20_Write (uint8_t, uint8_t);

uint8_t DS18B20_Read (uint8_t);

#endif
#endif /* DS18B20_H_ */
