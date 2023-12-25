/*
 * EGK.h
 *
 *  Created on: Nov 8, 2020
 *      Author: Acer
 */

#ifndef INC_EKG_H_
#define INC_EKG_H_

#include "main.h"
#ifdef USEEKG

#include "stdio.h"
#include "usbd_cdc_if.h"

void EKGinit();

void addNewEKGPoint(uint16_t newPoint);

void sendEGKData();

uint32_t average(uint32_t *, uint8_t);
uint32_t medianaverage(uint32_t *, uint8_t, uint8_t);
void dreieckstausch(uint32_t *, uint32_t *);

uint8_t getCurEKGState();
void parseEString(uint8_t*, uint32_t *);
uint8_t getSending();
uint8_t setSending(uint8_t );
uint8_t * getIdentifier();
uint8_t * getHardIdentifier();
void setIdentifier(uint8_t *);

#endif

#endif /* INC_EKG_H_ */
