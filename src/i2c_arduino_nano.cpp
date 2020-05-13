#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO

#include <Wire.h>
#include "delay.h"
#include "i2c.h"

void i2cInit()
{
    Wire.begin();
}

void i2cSend(uint8_t addr, uint8_t data)
{
    Wire.beginTransmission(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t i2cRead(uint8_t addr)
{
    uint8_t data = 0x00;
    Wire.requestFrom(addr, (uint8_t)1);

    while(Wire.available())
    {
        data = Wire.read();
    }

    delayUs(500);
    return data;
}

#endif
