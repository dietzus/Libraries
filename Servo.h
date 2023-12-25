/*
 * Servo.h
 *
 *  Created on: Jan 5, 2021
 *      Author: Martin
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "main.h"

#ifdef USESERVO

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim15;

typedef struct Servochannel_s {
	uint8_t isHQ;
	uint8_t isActive;
	TIM_HandleTypeDef curPWMTim;
	uint32_t TIMchannel;
	volatile uint32_t *CCR;
	uint32_t curDuty;
	uint32_t nextDuty;
	uint32_t minDuty;
	uint32_t maxDuty;
	uint32_t corMaxDuty;
	int16_t minAngle;
	int16_t maxAngle;
	int16_t corMaxAngle;
} Servochannel_t;

void ServoInitAll();
void ServoInitChannel(Servochannel_t *);

void ServoActivateAll();
void ServoDeactivateAll();
uint8_t ServoActivateFirst(uint8_t);
uint8_t ServoActivateChannel(Servochannel_t);
uint8_t ServoDeactivateChannel(Servochannel_t);

uint8_t ServoCountActive();
uint8_t ServoGetActiveChannels();

uint8_t ServoSetChannel(uint8_t, uint32_t);
uint8_t ServoSetChannelFloat(uint8_t, float);

uint32_t ServoGetChannel(uint8_t);
float ServoGetChannelFloat(uint8_t);

uint8_t ServoParseString(uint8_t *);

#endif

#endif /* INC_SERVO_H_ */
