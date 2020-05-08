#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO

#include <Arduino.h>
#include "ports.h"

void setPinMode(const uint8_t pin, const uint8_t value)
{
    pinMode(pin, value);
}

void setPinValue(const uint8_t pin, const uint8_t value)
{
    digitalWrite(pin, value);
}

uint8_t getPinValue(const uint8_t pin)
{
    return digitalRead(pin);
}

#endif

