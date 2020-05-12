#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO
#include <Arduino.h>

void delayUs(unsigned int us)
{
    delayMicroseconds(us);
}

#endif
