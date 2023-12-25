/*
 * RTC.c
 *
 *  Created on: Apr 11, 2021
 *      Author: Martin
 */

#include "RTC.h"
#ifdef USERTC

extern void Error_Handler(void);
#ifdef USEINTRTC
extern RTC_HandleTypeDef hrtc;
#endif

#ifdef USEDS3231
#define DS3231_ADDRESS 0xD0
I2C_HandleTypeDef *RTCi2c;

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} TIME;

TIME time;
#endif

uint8_t istimeinit = 0;
uint8_t isdateinit = 0;

uint8_t decToBcd(int);
int bcdToDec(uint8_t);

#ifdef USEDS3231
uint8_t RTC_init(I2C_HandleTypeDef *newHandle) {
	if(newHandle != NULL) {
		RTCi2c = newHandle;
		isdateinit = 1;
		istimeinit = 1;
		return 0;
	}
	return 1;
}
#endif

uint8_t RTC_settime (uint8_t hours, uint8_t minutes, uint8_t seconds) {
	if(hours > 24 || minutes > 60 || seconds > 60) {
		return 1;
	}

	HAL_StatusTypeDef HALstatus = HAL_ERROR;

#ifdef USEINTRTC
	RTC_TimeTypeDef sTime;
	sTime.Hours = hours; // set hours
	sTime.Minutes = minutes; // set minutes
	sTime.Seconds = seconds; // set seconds
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HALstatus = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
#endif
#ifdef USEDS3231
	uint8_t set_time[3];
	set_time[0] = decToBcd(seconds);
	set_time[1] = decToBcd(minutes);
	set_time[2] = decToBcd(hours);

	HALstatus = HAL_I2C_Mem_Write(RTCi2c, DS3231_ADDRESS, 0x00, 1, set_time, 3, 1000);
#endif

	if (HALstatus != HAL_OK)
	{
		return 1;
	} else {
		istimeinit = 1;
	}
	return 0;
}

uint8_t RTC_setdate (uint8_t day, uint8_t month, int16_t year) {
	if(day > 31 || month > 12) {
		return 1;
	}

	HAL_StatusTypeDef HALstatus = HAL_ERROR;

#ifdef USEINTRTC
	RTC_DateTypeDef sDate;
	//	sDate.WeekDay = RTC_WEEKDAY_THURSDAY; //day
	sDate.Date = day; // date
	sDate.Month = month; //month
	sDate.Year = year; // year

	HALstatus = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
#endif
#ifdef USEDS3231
	uint8_t set_time[7];
//	set_time[3] = decToBcd(weekday);
	set_time[4] = decToBcd(day);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	HALstatus = HAL_I2C_Mem_Write(RTCi2c, DS3231_ADDRESS, 0x04, 1, &set_time[4], 3, 1000);
#endif

	if (HALstatus != HAL_OK)
	{
		return 1;
	} else {
		isdateinit = 1;
	}

	return 0;
}

uint8_t RTC_gettime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
	if(!istimeinit) {
		return 1;
	}

	HAL_StatusTypeDef HALstatus = HAL_ERROR;

#ifdef USEINTRTC
	RTC_TimeTypeDef gTime;

	/* Get the RTC current Time */
	HALstatus = HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);

	if (HALstatus != HAL_OK) {
		return 1;
	}

	/* Display time Format: hh:mm:ss */
	//	sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
	*hours = gTime.Hours;
	*minutes = gTime.Minutes;
	*seconds = gTime.Seconds;
#endif
#ifdef USEDS3231
	uint8_t get_time[7];
	HALstatus = HAL_I2C_Mem_Read(RTCi2c, DS3231_ADDRESS, 0x00, 1, get_time, 3, 1000);

	if (HALstatus != HAL_OK) {
		HAL_I2C_Init(RTCi2c);
		return 1;
	}

	*seconds = bcdToDec(get_time[0]);
	*minutes = bcdToDec(get_time[1]);
	*hours = bcdToDec(get_time[2]);
#endif
	return 0;
}

uint8_t RTC_getdate (uint8_t *day, uint8_t *month, int16_t *year) {
	if(!isdateinit) {
		return 1;
	}

	HAL_StatusTypeDef HALstatus = HAL_ERROR;

#ifdef USEINTRTC
	RTC_DateTypeDef gDate;

	/* Get the RTC current Date */
	HALstatus = HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	if (HALstatus != HAL_OK) {
		return 1;
	}

	/* Display date Format: dd-mm-yy */
	//	sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);
	*day = gDate.Date;
	*month = gDate.Month;
	*year = gDate.Year;

#endif
#ifdef USEDS3231
	uint8_t get_date[3];
	HALstatus = HAL_I2C_Mem_Read(RTCi2c, DS3231_ADDRESS, 0x04, 1, get_date, 3, 1000);

	if (HALstatus != HAL_OK) {
		return 1;
	}

//	*weekday = bcdToDec(get_time[3]);
	*day = bcdToDec(get_date[0]);
	*month = bcdToDec(get_date[1]);
	*year = bcdToDec(get_date[2]);
#endif

	return 0;
}

uint8_t RTC_setalarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {

	HAL_StatusTypeDef HALstatus = HAL_ERROR;

#ifdef USEINTRTC
	RTC_AlarmTypeDef sAlarm;

	sAlarm.AlarmTime.Hours = 0x10; // hours
	sAlarm.AlarmTime.Minutes = 0x31; // min
	sAlarm.AlarmTime.Seconds = 0x00; //seconds
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	//  sAlarm.AlarmDateWeekDay = 0x9; // DATE
	sAlarm.Alarm = RTC_ALARM_A;

	HALstatus = HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
#endif
#ifdef USEDS3231

#endif
	if (HALstatus != HAL_OK)
	{
		return 1;
	}
	return 0;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	//do Stuff
}

#ifdef USEDS3231
float Get_Temp (void)
{

	HAL_StatusTypeDef HALstatus = HAL_ERROR;
	uint8_t temp[2];

	HALstatus = HAL_I2C_Mem_Read(RTCi2c, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);

	if (HALstatus != HAL_OK)
	{
		return -500;
	}

	return ((temp[0])+(temp[1]>>6)/4.0);
}
#endif

uint8_t RTC_istimeinit(void) {
	return istimeinit;
}

uint8_t RTC_isdateinit(void) {
	return isdateinit;
}

uint8_t RTC_isinit(void) {
	if(istimeinit && isdateinit) {
		return 1;
	}
	return 0;
}

// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

#endif
