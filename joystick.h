/*
 * joystick.h
 *
 *  Created on: Apr 26, 2020
 *      Author: Martin
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "main.h"
#ifdef USEJOYSTICK

#include <stdint.h>
#include "usb_device.h"

//#define CUSTOM_HID_EPIN_SIZE                         0x12U
//#define CUSTOM_HID_EPOUT_SIZE                        0x12U

#define CHIDBUFSIZE 8

struct HIDtype_s {
	uint8_t joystick;
	uint8_t gamepad;
} HIDtype_t;

struct joystickHID_s {
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t Rz;
	int8_t Rx;
	int8_t Ry;
	uint16_t buttons;
} joystickHID_t;

struct gamepadHID_s {
	int16_t stick1x;
	int32_t min1x;
	int32_t max1x;
	int16_t offsetStick1x;
	int16_t deadzoneStick1x;
	int16_t middle1x;

	int16_t stick1y;
	int32_t min1y;
	int32_t max1y;
	int16_t offsetStick1y;
	int16_t deadzoneStick1y;
	int16_t middle1y;

	int16_t stick2x;
	int32_t min2x;
	int32_t max2x;
	int16_t offsetStick2x;
	int16_t deadzoneStick2x;
	int16_t middle2x;

	int16_t stick2y;
	int32_t min2y;
	int32_t max2y;
	int16_t offsetStick2y;
	int16_t deadzoneStick2y;
	int16_t middle2y;

	int16_t Trigger1;
	int32_t mintrigger1;
	int32_t maxtrigger1;
	int16_t offsetTrigger1;
	int16_t deadzoneTrigger1;
	int16_t middletrigger1;

	int16_t Trigger2;
	int32_t mintrigger2;
	int32_t maxtrigger2;
	int16_t offsetTrigger2;
	int16_t deadzoneTrigger2;
	int16_t middletrigger2;

	uint16_t buttons;
	uint16_t invbuttons;
} gamepadHID_t;

uint8_t initJoyHID();
uint8_t initGamepadHID();

uint8_t initHIDADC(ADC_HandleTypeDef*);

uint8_t initHIDUSBCon();
void setHIDUSBHandle(USBD_HandleTypeDef);
USBD_HandleTypeDef getHIDUSBHandle();

uint8_t calcHIDADCValues();
void sendHIDReport();
void setADCBuf(uint32_t*);

int8_t mapADCV(uint32_t);

void setChange();
void unsetChange();
uint8_t getChange();

void setX(int8_t);
void setY(int8_t);
void setZ(int8_t);
void setRx(int8_t);
void setRy(int8_t);
void setRz(int8_t);
void setButtons(uint16_t);
void setButton(uint8_t);
void unsetButton(uint8_t);
uint8_t getButton(uint8_t);

#endif /* INC_JOYSTICK_H_ */
#endif
