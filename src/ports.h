#pragma once

#include <inttypes.h>

#include "CompDevSDK.h"

void setPinMode(const uint8_t pin, const uint8_t value);
void setPinValue(const uint8_t pin, const uint8_t value);
uint8_t getPinValue(const uint8_t pin);
