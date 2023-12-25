/*
 * calibrations.h
 *
 *  Created on: 03.03.2022
 *      Author: Martin
 */

#ifndef CALIBRATIONS_H_
#define CALIBRATIONS_H_

#include "main.h"

typedef struct {
	uint8_t isInit;
	uint8_t useAddition;
	double measuredValue;
	double correctValue;
	float cvalue;
} singlePointCalib_t;

typedef struct {
	uint8_t isInit;
	double measuredValue[2];
	double correctValue[2];
	float cvalue[2];
} twoPointCalib_t;

typedef struct {
	uint8_t isInit;
	double measuredValue[3];
	double correctValue[3];
	float cvalue[3];
} threePointCalib_t;

typedef struct {
	uint8_t isInit;
	uint8_t order;
	double *measuredValue;
	double *correctValue;
	float *cvalue;
} multiPointCalib_t;

uint8_t initSinglePointCalib(singlePointCalib_t *calib, uint8_t useAddition);
uint8_t initTwoPointCalib(twoPointCalib_t *calib);
uint8_t initThreePointCalib(threePointCalib_t *calib);
uint8_t initMultiPointCalib(multiPointCalib_t *calib, uint8_t order);

uint8_t setSinglePointCalib(double measuredValue, double CorrectValue);
uint8_t setTwoPointCalib(uint8_t number, double measuredValues, double CorrectValues);
uint8_t setThreePointtCalib(uint8_t number, double measuredValues, double CorrectValues);
uint8_t setMultiPointCalib(uint8_t number, double measuredValues, double CorrectValues);

uint8_t calcSinglePointCalib();
uint8_t calcTwoPointCalib();
uint8_t calcThreePointCalib();
uint8_t calcMultiPointCalib();

float getSinglePointCalib(float);
float getTwoPointCalib(float);
float getThreePointCalib(float);
float getMultiPointCalib(float);

#endif /* CALIBRATIONS_H_ */
