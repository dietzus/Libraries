/*
 * RTC.h
 *
 *  Created on: Apr 11, 2021
 *      Author: Martin
 */

#ifndef RTC_H_
#define RTC_H_

#include "main.h"
#ifdef USERTC

uint8_t RTC_settime (uint8_t, uint8_t, uint8_t);
uint8_t RTC_setdate (uint8_t, uint8_t, int16_t);
uint8_t RTC_gettime(uint8_t *, uint8_t *, uint8_t *);
uint8_t RTC_getdate (uint8_t *, uint8_t *, int16_t *);
uint8_t RTC_setalarm (uint8_t, uint8_t, uint8_t);

uint8_t RTC_istimeinit(void);
uint8_t RTC_isdateinit(void);
uint8_t RTC_isinit(void);

#ifdef USEDS3231
uint8_t RTC_init(I2C_HandleTypeDef *);
float Get_Temp (void);
#endif

#endif
#endif /* RTC_H_ */
