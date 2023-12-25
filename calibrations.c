/*
 * calibrations.c
 *
 *  Created on: 03.03.2022
 *      Author: Martin
 */

#include "calibrations.h"

uint8_t initSinglePointCalib(singlePointCalib_t *calib, uint8_t useAddition);
uint8_t initTwoPointCalib(twoPointCalib_t *calib);
uint8_t initThreePointCalib(threePointCalib_t *calib);
uint8_t initMultiPointCalib(multiPointCalib_t *calib, uint8_t order);

uint8_t setSinglePointCalib(double measuredValue, double CorrectValue) {
//	if()

	return 0;
}

uint8_t setTwoPointCalib(uint8_t number, double measuredValues, double CorrectValues) {


	return 0;
}

uint8_t setThreePointtCalib(uint8_t number, double measuredValues, double CorrectValues) {


	return 0;
}

uint8_t setMultiPointCalib(uint8_t number, double measuredValues, double CorrectValues) {


	return 0;
}

uint8_t calcSinglePointCalib();
uint8_t calcTwoPointCalib();
uint8_t calcThreePointCalib();
uint8_t calcMultiPointCalib();

float getSinglePointCalib(float);
float getTwoPointCalib(float);
float getThreePointCalib(float);
float getMultiPointCalib(float);
