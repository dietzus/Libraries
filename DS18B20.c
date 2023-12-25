/*
 * DS18B20.c
 *
 *  Created on: Mar 27, 2021
 *      Author: Martin
 */


#include "DS18B20.h"
#ifdef USEDS18B20

#include "Utils.h"
GPIO_TypeDef* Ports[MAXDS18B20SENSORS];
uint16_t Pins[MAXDS18B20SENSORS];

uint8_t DS18B20numSensors = 0;
uint8_t DS18B20isActive[MAXDS18B20SENSORS];

uint8_t DS18B20_Init(uint8_t place, GPIO_TypeDef* port, uint16_t pin) {
	if(place < MAXDS18B20SENSORS && DS18B20_Start_Pin(port, pin)) {
		Ports[place] = port;
		Pins[place] = pin;
		if(!DS18B20isActive[place]) {
			DS18B20isActive[place] = 1;
			DS18B20numSensors++;
		}
		return 1;
	}
	return 0;
}

uint8_t DS18B20_DeInit(uint8_t place) {
	if(place < MAXDS18B20SENSORS && !DS18B20isActive[place]) {
		DS18B20isActive[place] = 0;
		DS18B20numSensors--;
		return 1;
	}
	return 0;
}

uint8_t DS18B20_Start (uint8_t curSensor) {
	return DS18B20_Start_Pin(Ports[curSensor], Pins[curSensor]);
}

uint8_t DS18B20_Start_Pin (GPIO_TypeDef* port, uint16_t pin)
{
	uint8_t Response = 0;
	Set_Pin_Output(port, pin);   // set the pin as output
	HAL_GPIO_WritePin (port, pin, 0);  // pull the pin low
	DWT_Delay_us(480);   // delay according to datasheet

	Set_Pin_Input(port, pin);    // set the pin as input
	DWT_Delay_us(80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (port, pin))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	//else Response = 0;

	DWT_Delay_us(400); // 480 us delay totally.

	return Response;
}

void DS18B20_Write (uint8_t curSensor, uint8_t data)
{
	Set_Pin_Output(Ports[curSensor], Pins[curSensor]);  // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			Set_Pin_Output(Ports[curSensor], Pins[curSensor]);  // set as output
			HAL_GPIO_WritePin (Ports[curSensor], Pins[curSensor], 0);  // pull the pin LOW
			DWT_Delay_us(1);  // wait for 1 us

			Set_Pin_Input(Ports[curSensor], Pins[curSensor]);  // set as input
			DWT_Delay_us(50);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			Set_Pin_Output(Ports[curSensor], Pins[curSensor]);
			HAL_GPIO_WritePin (Ports[curSensor], Pins[curSensor], 0);  // pull the pin LOW
			DWT_Delay_us(50);  // wait for 60 us

			Set_Pin_Input(Ports[curSensor], Pins[curSensor]);
		}
	}
}

uint8_t DS18B20_Read (uint8_t curSensor)
{
	uint8_t value=0;

	Set_Pin_Input(Ports[curSensor], Pins[curSensor]);

	for (int i=0;i<8;i++)
	{
		Set_Pin_Output(Ports[curSensor], Pins[curSensor]);   // set as output

		HAL_GPIO_WritePin (Ports[curSensor], Pins[curSensor], 0);  // pull the data pin LOW
		DWT_Delay_us(1);  // wait for > 1us

		Set_Pin_Input(Ports[curSensor], Pins[curSensor]);  // set as input
		if (HAL_GPIO_ReadPin (Ports[curSensor], Pins[curSensor]))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DWT_Delay_us(50);  // wait for 60 us
	}
	return value;
}

#endif
