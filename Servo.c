/*
 * Servo.c
 *
 *  Created on: Jan 5, 2021
 *      Author: Martin
 */

#include "Servo.h"
#ifdef USESERVO

#include "Utils.h"

uint8_t curInitChannels = 0;
uint8_t curActChannels = 0;

Servochannel_t Servochannels[14];

void ServoInitAll() {

	curInitChannels = 0;

	uint32_t minDu = (float)UINT16_MAX / 100 * 2;
	uint32_t maxDu = (float)UINT16_MAX / 100 * 12;
	uint32_t corMaxDu = maxDu - minDu;
	uint32_t meanDu = (float)UINT16_MAX / 100 * 7;

	Servochannels[0].curPWMTim = htim1;
	Servochannels[0].TIMchannel = TIM_CHANNEL_1;
	Servochannels[1].curPWMTim = htim1;
	Servochannels[1].TIMchannel = TIM_CHANNEL_2;
	Servochannels[2].curPWMTim = htim1;
	Servochannels[2].TIMchannel = TIM_CHANNEL_3;

	Servochannels[3].curPWMTim = htim3;
	Servochannels[3].TIMchannel = TIM_CHANNEL_1;
	Servochannels[4].curPWMTim = htim3;
	Servochannels[4].TIMchannel = TIM_CHANNEL_2;
	Servochannels[5].curPWMTim = htim3;
	Servochannels[5].TIMchannel = TIM_CHANNEL_3;
	Servochannels[6].curPWMTim = htim3;
	Servochannels[6].TIMchannel = TIM_CHANNEL_4;

	Servochannels[7].curPWMTim = htim4;
	Servochannels[7].TIMchannel = TIM_CHANNEL_1;
	Servochannels[8].curPWMTim = htim4;
	Servochannels[8].TIMchannel = TIM_CHANNEL_2;
	Servochannels[9].curPWMTim = htim4;
	Servochannels[9].TIMchannel = TIM_CHANNEL_3;

	uint8_t i = 0;
	for(; i<10; i++) {
		HAL_TIM_PWM_Stop(&Servochannels[i].curPWMTim, Servochannels[i].TIMchannel);
		Servochannels[i].isHQ = 0;
		Servochannels[i].isActive = 0;
		Servochannels[i].minDuty = minDu;
		Servochannels[i].maxDuty = maxDu;
		Servochannels[i].corMaxDuty = corMaxDu;
		Servochannels[i].curDuty = meanDu;
		Servochannels[i].nextDuty = meanDu;
		Servochannels[i].minAngle = -180;
		Servochannels[i].maxAngle = 180;
		Servochannels[i].corMaxAngle = 360;

		ServoInitChannel(&Servochannels[i]);
	}

	curInitChannels += 10;

	minDu = (double)UINT32_MAX / 100 * 2;
	maxDu = (double)UINT32_MAX / 100 * 12;
	corMaxDu = maxDu - minDu;
	meanDu = (double)UINT32_MAX / 100 * 7;

	Servochannels[10].curPWMTim = htim2;
	Servochannels[10].TIMchannel = TIM_CHANNEL_1;
	Servochannels[11].curPWMTim = htim2;
	Servochannels[11].TIMchannel = TIM_CHANNEL_2;
	Servochannels[12].curPWMTim = htim2;
	Servochannels[12].TIMchannel = TIM_CHANNEL_3;
	Servochannels[13].curPWMTim = htim2;
	Servochannels[13].TIMchannel = TIM_CHANNEL_4;

	for(; i<14; i++) {
		HAL_TIM_PWM_Stop(&Servochannels[i].curPWMTim, Servochannels[i].TIMchannel);
		Servochannels[i].isHQ = 1;
		Servochannels[i].isActive = 0;
		Servochannels[i].minDuty = minDu;
		Servochannels[i].maxDuty = maxDu;
		Servochannels[i].corMaxDuty = corMaxDu;
		Servochannels[i].curDuty = meanDu;
		Servochannels[i].nextDuty = meanDu;
		Servochannels[i].minAngle = -180;
		Servochannels[i].maxAngle = 180;
		Servochannels[i].corMaxAngle = 360;

		ServoInitChannel(&Servochannels[i]);
	}

	curInitChannels += 4;
}

void ServoInitChannel(Servochannel_t *channel) {
	ServoDeactivateChannel(*channel);

	if(channel->TIMchannel == TIM_CHANNEL_1) {
		channel->CCR = &channel->curPWMTim.Instance->CCR1;
	} else if(channel->TIMchannel == TIM_CHANNEL_2) {
		channel->CCR = &channel->curPWMTim.Instance->CCR2;
	} else if(channel->TIMchannel == TIM_CHANNEL_3) {
		channel->CCR = &channel->curPWMTim.Instance->CCR3;
	} else if(channel->TIMchannel == TIM_CHANNEL_4) {
		channel->CCR = &channel->curPWMTim.Instance->CCR4;
#ifdef TIM_CHANNEL_5
	} else if(channel->TIMchannel == TIM_CHANNEL_5) {
		channel->CCR = &channel->curPWMTim.Instance->CCR5;
#endif
#ifdef TIM_CHANNEL_6
	} else if(channel->TIMchannel == TIM_CHANNEL_6) {
		channel->CCR = &channel->curPWMTim.Instance->CCR6;
	}
#endif

	*channel->CCR = channel->nextDuty;
}

void ServoActivateAll() {

	for(uint8_t i=0; i<14; i++) {
		ServoActivateChannel(Servochannels[i]);
	}
}

void ServoDeactivateAll() {

	for(uint8_t i=0; i<14; i++) {
		ServoDeactivateChannel(Servochannels[i]);
	}
}

uint8_t ServoActivateFirst(uint8_t number) {
	if(number > curInitChannels) return 1;
	ServoDeactivateAll();

	for(uint8_t i=0; i<14; i++) {
		if(i >= number) return 0;
		if(!Servochannels[i].isActive) {
			curActChannels++;
		}
		ServoActivateChannel(Servochannels[i]);
	}

	return 0;
}

uint8_t ServoActivateChannel(Servochannel_t channel) {

	HAL_TIM_PWM_Start(&channel.curPWMTim, channel.TIMchannel);
	uint8_t prevStatus = channel.isActive;
	if(!channel.isActive) {
		curActChannels++;
	}
	channel.isActive = 1;

	return prevStatus;
}

uint8_t ServoDeactivateChannel(Servochannel_t channel) {

	HAL_TIM_PWM_Stop(&channel.curPWMTim, channel.TIMchannel);
	uint8_t prevStatus = channel.isActive;
	if(channel.isActive) {
		curActChannels--;
	}
	channel.isActive = 0;

	return prevStatus;
}

uint8_t ServoCountActive() {

	uint8_t tempstatus = 0;

	for(uint8_t i=0; i<14; i++) {
		if(Servochannels[i].isActive) {
			tempstatus++;
		}
	}

	return tempstatus;
}

uint8_t ServoGetActiveChannels() {
	return curActChannels;
}

uint8_t ServoSetChannel(uint8_t channel, uint32_t value) {
	if(Servochannels[channel].isHQ) {
		*Servochannels[channel].CCR = value;
	} else if (value > UINT16_MAX) {
		return 1;
	} else {
		*Servochannels[channel].CCR = value;
	}

	return 0;
}

uint8_t ServoSetChannelFloat(uint8_t channel, float value) {
	if(value < (Servochannels[channel].minAngle) || value > Servochannels[channel].maxAngle) return 1;

	ServoSetChannel(channel, (float)Servochannels[channel].corMaxDuty * (value - Servochannels[channel].minAngle) / Servochannels[channel].corMaxAngle + Servochannels[channel].minDuty + 0.5);

	return 0;
}

uint8_t ServoParseString(uint8_t *string) {
	uint8_t gefPara = 0;
	if(string[0] < 65 || string[0] > 123 || (string[0] > 90 && string[0] < 97)) return gefPara;
	uint8_t i = 0;

	uint8_t offset = 0;
	uint8_t tempchannel = 0;
	float tempval = 0;
	while(string[i] != '\n') {
		if(string[i] < 65 || string[i] > 123 || (string[i] > 90 && string[i] < 97)) return gefPara;
		if(string[i] > 90) {
			offset = 97;
		} else {
			offset = 65;
		}
		tempchannel = string[i] - offset;
		uint8_t temppos;
		i++;
		if((temppos = findParaFloat((char*)string, i, &tempval)) == i) {
			return gefPara;
		} else {
			ServoSetChannelFloat(tempchannel, tempval);
			i = temppos;
			gefPara++;
		}
	}

	return gefPara;
}

#endif
