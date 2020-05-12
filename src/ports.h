#ifndef _PORTS_H_
#define _PORTS_H_

#include <inttypes.h>
#include "CompDevSDK.h"

/**
 * @brief Set the pin mode
 *
 * @param pin       pin to set the mode
 * @param mode      PIN_OUTPUT or PIN_INPUT
 */
void setPinMode(const uint8_t pin, const uint8_t mode);

/**
 * @brief Set the Pin value
 *
 * @param pin       pin to set the value
 * @param value     PIN_LOW or PIN_HIGH
 */
void setPinValue(const uint8_t pin, const uint8_t value);

/**
 * @brief Get the Pin value
 *
 * @param pin       pin to get the value
 * @return uint8_t  value of the pin
 */
uint8_t getPinValue(const uint8_t pin);

#endif // _PORTS_H_
