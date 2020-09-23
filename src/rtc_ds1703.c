#include "CompDevSDK.h"

#include "rtc_ds1703.h"

#include "i2c.h"

#define DS1307_ADDRESS  0x68
#define DS1307_TIME     0x00

static uint8_t dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

static uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

// void ds1703SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t day, uint8_t month, uint16_t year)
// {
//     uint8_t buffer[] = {sec, min, hour, dow, day, month, year};

//     i2cWriteByte(DS1307_ADDRESS, DS1307_TIME, 0);
//     i2cWrite(DS1307_ADDRESS, buffer, sizeof(buffer), 0);
// }

void ds1703SetTime(ds1703_time_t time)
{
    uint8_t buffer[] = {
        DS1307_TIME,
        dec2bcd(time.sec),
        dec2bcd(time.min),
        dec2bcd(time.hour),
        dec2bcd(time.dow),
        dec2bcd(time.day),
        dec2bcd(time.month),
        dec2bcd(time.year - 2000)
    };

    // i2cWriteByte(DS1307_ADDRESS, DS1307_TIME, 1);
    i2cWrite(DS1307_ADDRESS, buffer, sizeof(buffer), 0);
    // i2cWriteByte(DS1307_ADDRESS, DS1307_TIME, 0);
}


ds1703_time_t ds1703GetTime()
{
    uint8_t buffer[7];

    i2cWriteByte(DS1307_ADDRESS, DS1307_TIME, 0);
    i2cRead(DS1307_ADDRESS, buffer, 7, 0);

    ds1703_time_t time = {
        .sec = bcd2dec(buffer[0]),
        .min = bcd2dec(buffer[1]),
        .hour = bcd2dec(buffer[2]),
        .dow = bcd2dec(buffer[3]),
        .day = bcd2dec(buffer[4]),
        .month = bcd2dec(buffer[5]),
        .year = bcd2dec(buffer[6]) + 2000
    };

    return time;
}
