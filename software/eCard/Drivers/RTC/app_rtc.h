/*
 * display.h
 *
 *  Created on: Jul 19, 2022
 *      Author: TECH66
 */

#ifndef _APP_RTC_H
#define _APP_RTC_H

#include <Debug/debug.h>

typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;

    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;
} _calendar_obj;

extern _calendar_obj calendar;

/* Exported_Functions */
u8 APP_RTC_Init(void);
void APP_RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
// u8 Is_Leap_Year(u16 year);
// u8 RTC_Alarm_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
// u8 RTC_Get(void);
// u8 RTC_Get_Week(u16 year, u8 month, u8 day);
// u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
#endif
