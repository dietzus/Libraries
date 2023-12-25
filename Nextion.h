/*
 * Nexion.h
 *
 *  Created on: Apr 6, 2021
 *      Author: Martin
 */

#ifndef NEXTION_H_
#define NEXTION_H_

#include "main.h"
#ifdef USENEXTION

uint8_t NEXTION_Init(UART_HandleTypeDef *);

void NEXTION_SendNum (const char *, int32_t);
void NEXTION_SendFloat (const char *, float, int);
void NEXTION_SendText(const char *, const char *);
void NEXTION_SendQR (const char *, const char *);

void NEXTION_AddtoLog(const char *, const char *);
void NEXTION_ClearLog(const char *);

#endif

#endif /* NEXTION_H_ */
