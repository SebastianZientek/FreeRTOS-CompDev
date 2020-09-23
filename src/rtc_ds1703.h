#ifndef _RTC_DS1703_H_
#define _RTC_DS1703_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t dow;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} ds1703_time_t;

// void ds1703SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t day, uint8_t month, uint16_t year);
void ds1703SetTime(ds1703_time_t time);
ds1703_time_t ds1703GetTime();

#ifdef __cplusplus
}
#endif

#endif // _RTC_DS1703_H_
