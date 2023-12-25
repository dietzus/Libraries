/*
 * joystick.c
 *
 *  Created on: Apr 26, 2020
 *      Author: Martin
 */

#include "joystick.h"
#ifdef USEJOYSTICK

#include "math.h"

#include "usb_device.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

#define NRADCCH 10
#define NRMEANVAL 10
#define MAX_ADC (pow(2,12)-0)

struct HIDtype_s HIDtype;
struct joystickHID_s joystickHID;
struct gamepadHID_s gamepadHID;

extern uint8_t HIDbuffer[];

int8_t debugoffset = 0;

extern USBD_HandleTypeDef hUsbDeviceFS;
USBD_HandleTypeDef HIDUSBhandle;
uint8_t buff[CHIDBUFSIZE];

uint8_t changes = 1;
uint32_t* ADCV[10];
uint32_t ADCVal[NRADCCH * NRMEANVAL];
uint8_t usbtransfer = 0;
uint32_t ADCmeanval[NRADCCH];
const GPIO_TypeDef* Ports[12] = { Button1_GPIO_Port, Button2_GPIO_Port, Button3_GPIO_Port, Button4_GPIO_Port, Button5_GPIO_Port, Button6_GPIO_Port, Button7_GPIO_Port, Button8_GPIO_Port, Button9_GPIO_Port, Button10_GPIO_Port, Button11_GPIO_Port, Button12_GPIO_Port };
const uint16_t Pins[12] = { Button1_Pin, Button2_Pin, Button3_Pin, Button4_Pin, Button5_Pin, Button6_Pin, Button7_Pin, Button8_Pin, Button9_Pin, Button10_Pin, Button11_Pin, Button12_Pin };
const uint32_t ADCCon = 4096 / 256;

uint8_t initJoyHID() {
	joystickHID.x = 0;
	joystickHID.y = 0;
	joystickHID.z = 0;
	joystickHID.Rx = 0;
	joystickHID.Ry = 0;
	joystickHID.Rz = 0;

	joystickHID.buttons = 0;

	HIDtype.gamepad = 0;
	HIDtype.joystick = 1;

	return initHIDUSBCon();
}

uint8_t initGamepadHID() {
	int32_t MAX_8bit = (pow(2,8))-0;
//	int32_t MAX_12bit = (2^12)-0;

	gamepadHID.stick1x = 0;
	gamepadHID.max1x = MAX_8bit;
	gamepadHID.min1x = 0;
	gamepadHID.offsetStick1x = 0; //- (4095 / 2);
	gamepadHID.middle1x = MAX_ADC / 2;
	gamepadHID.deadzoneStick1x = 20;

	gamepadHID.stick1y = 0;
	gamepadHID.max1y = MAX_8bit;
	gamepadHID.min1y = 0;
	gamepadHID.offsetStick1y = 0; //- (4095 / 2);
	gamepadHID.middle1y = MAX_ADC / 2;
	gamepadHID.deadzoneStick1y = 20;

	gamepadHID.stick2x = 0;
	gamepadHID.max2x = MAX_8bit;
	gamepadHID.min2x = 0;
	gamepadHID.offsetStick2x = 0; //- (4095 / 2);
	gamepadHID.middle2x = MAX_ADC / 2;
	gamepadHID.deadzoneStick2x = 20;

	gamepadHID.stick2y = 0;
	gamepadHID.max2y = MAX_8bit;
	gamepadHID.min2y = 0;
	gamepadHID.offsetStick2y = 0; //- (4095 / 2);
	gamepadHID.middle2y = MAX_ADC / 2;
	gamepadHID.deadzoneStick2y = 20;

	gamepadHID.Trigger1 = 0;
	gamepadHID.maxtrigger1 = MAX_8bit;
	gamepadHID.mintrigger1 = 0;
	gamepadHID.offsetTrigger1 = 0; //- (4095 / 2);
	gamepadHID.middletrigger1 = MAX_ADC / 2;
	gamepadHID.deadzoneTrigger1 = 20;

	gamepadHID.Trigger2 = 0;
	gamepadHID.maxtrigger2 = MAX_8bit;
	gamepadHID.mintrigger2 = 0;
	gamepadHID.offsetTrigger2 = 0; //- (4095 / 2);
	gamepadHID.middletrigger2 = MAX_ADC / 2;
	gamepadHID.deadzoneTrigger2 = 20;

	gamepadHID.buttons = 0;
	gamepadHID.invbuttons = 0;

	HIDtype.gamepad = 1;
	HIDtype.joystick = 0;

	return initHIDUSBCon();
}

uint8_t initHIDADC(ADC_HandleTypeDef* newADC) {
	HAL_ADC_Start_DMA(newADC, &ADCVal[0], NRADCCH * NRMEANVAL);

	return 0;
}

uint8_t initHIDUSBCon() {
//	USBD_HandleTypeDef temphandle;
//	temphandle = getUSBhandle();

//	if(temphandle.id != hUsbDeviceFS.id) {
		HIDUSBhandle = hUsbDeviceFS;
//		return 1;
//	}
	return 0;
}

void setHIDUSBHandle(USBD_HandleTypeDef newUSBHandle) {
	HIDUSBhandle = newUSBHandle;
}

USBD_HandleTypeDef getHIDUSBHandle() {
	return HIDUSBhandle;
}

uint8_t calcHIDADCValues() {
	if(usbtransfer == 1) return 1;

	for(uint8_t i=0; i<NRADCCH; i++) {
		ADCmeanval[i] = 0;
		for(uint8_t j=0; j<NRMEANVAL; j++) {
			uint32_t tempvalue = ADCVal[i + j*NRADCCH];
			ADCmeanval[i] += tempvalue;
		}
		ADCmeanval[i] /= NRMEANVAL;
	}

	if(HIDtype.joystick == 1) {

	}

	if(HIDtype.gamepad == 1) {
		gamepadHID.stick1x = ADCmeanval[0] + gamepadHID.offsetStick1x;
		if(abs(gamepadHID.stick1x - gamepadHID.middle1x) < gamepadHID.deadzoneStick1x) gamepadHID.stick1x = gamepadHID.middle1x;
		gamepadHID.stick1y = ADCmeanval[1] + gamepadHID.offsetStick1y;
		if(abs(gamepadHID.stick1y - gamepadHID.middle1y) < gamepadHID.deadzoneStick1y) gamepadHID.stick1y = gamepadHID.middle1y;
		gamepadHID.stick2x = ADCmeanval[2] + gamepadHID.offsetStick2x;
		if(abs(gamepadHID.stick2x - gamepadHID.middle2x) < gamepadHID.deadzoneStick2x) gamepadHID.stick2x = gamepadHID.middle2x;
		gamepadHID.stick2y = ADCmeanval[3] + gamepadHID.offsetStick2y;
		if(abs(gamepadHID.stick2y - gamepadHID.middle2y) < gamepadHID.deadzoneStick2y) gamepadHID.stick2y = gamepadHID.middle2y;
		gamepadHID.Trigger1 = ADCmeanval[4] + gamepadHID.offsetTrigger1;
		if(abs(gamepadHID.Trigger1 - gamepadHID.middletrigger1) < gamepadHID.deadzoneTrigger1) gamepadHID.Trigger1 = gamepadHID.middletrigger1;
		gamepadHID.Trigger2 = ADCmeanval[5] + gamepadHID.offsetTrigger2;
		if(abs(gamepadHID.Trigger2 - gamepadHID.middletrigger2) < gamepadHID.deadzoneTrigger2) gamepadHID.Trigger2 = gamepadHID.middletrigger2;
	}

	return 0;
}

void sendHIDReport()  {
	for(uint8_t i=0; i<CHIDBUFSIZE; i++) buff[i]=0;

	calcHIDADCValues();

	usbtransfer = 1;
	if(HIDtype.joystick == 1) {
		uint8_t i = 0;

//		buff[i++] = 0;
		buff[i++] = joystickHID.buttons;
		buff[i++] = joystickHID.buttons >> 7;
		buff[i++] = joystickHID.x;
		buff[i++] = joystickHID.y;
		buff[i++] = joystickHID.z;
		buff[i++] = joystickHID.Rx;
		buff[i++] = joystickHID.Ry;
//		buff[i++] = joystickHID.Rz;
	/*	buff[i++] =  mapADCV(*ADCV[j++]);	//x
		buff[i++] =  mapADCV(*ADCV[j++]);	//y
		buff[i++] =  mapADCV(*ADCV[j++]);	//z
		buff[i++] =  mapADCV(*ADCV[j++]);	//Rx
		buff[i++] =  mapADCV(*ADCV[j++]);	//Ry
		buff[i++] =  mapADCV(*ADCV[j++]);	//Rz
	*/
	}

	if(HIDtype.gamepad == 1) {
		uint8_t i = 0;

//		buff[i++] = 0x03;
		buff[i++] = gamepadHID.Trigger1 / (MAX_ADC / gamepadHID.max1x);
		buff[i++] = gamepadHID.Trigger2 / (MAX_ADC / gamepadHID.max1y);
		buff[i++] = gamepadHID.stick1x / (MAX_ADC / gamepadHID.max2x);
		buff[i++] = gamepadHID.stick1y / (MAX_ADC / gamepadHID.max2y);
		buff[i++] = gamepadHID.stick2x / (MAX_ADC / gamepadHID.maxtrigger1);
		buff[i++] = gamepadHID.stick2y / (MAX_ADC / gamepadHID.maxtrigger2);

		gamepadHID.buttons = (int16_t)0b101010101010101010;						//for Debugging
		buff[i++] = LOBYTE(gamepadHID.buttons);
		buff[i++] = HIBYTE(gamepadHID.buttons);
	}
	usbtransfer = 0;

		USBD_StatusTypeDef tempstatus = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff, CHIDBUFSIZE);
		if(tempstatus == 0) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		}

//	for(uint8_t i = 0; i<12; i++) {
//		if(getButton(i+1)) {
//			if(!HAL_GPIO_ReadPin(Ports[i], Pins[i])) {
//				unsetButton(i+1);
//			}
//		}
//	}
}

int8_t mapADCV(uint32_t adcv) {
	return (int8_t)(adcv / ADCCon - 127);
}

void setADCBuf(uint32_t* adcv) {
	ADCV[0] = adcv;
}

void setChange() {
	changes = 1;
}

void unsetChange() {
	changes = 0;
}

uint8_t getChange() {
	return changes;
}

void setX(int8_t newValue) {
	joystickHID.x = newValue;
	setChange();
}

void setY(int8_t newValue) {
	joystickHID.y = newValue;
	setChange();
}

void setZ(int8_t newValue) {
	joystickHID.z = newValue;
	setChange();
}

void setRx(int8_t newValue) {
	joystickHID.Rx = newValue;
	setChange();
}

void setRy(int8_t newValue) {
	joystickHID.Ry = newValue;
	setChange();
}

void setRz(int8_t newValue) {
	joystickHID.Rz = newValue;
	setChange();
}

void setButtons(uint16_t newValue) {
	joystickHID.buttons = newValue;
	setChange();
}

void setButton(uint8_t num) {
	joystickHID.buttons |= 1<<(num-1);
	setChange();
}

void unsetButton(uint8_t num) {
	joystickHID.buttons ^= 1<<(num-1);
	setChange();
}

uint8_t getButton(uint8_t num) {
	uint16_t tempint = joystickHID.buttons << (15-num);
	return tempint >> num;
}

#endif
