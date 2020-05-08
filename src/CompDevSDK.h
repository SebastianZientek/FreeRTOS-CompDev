#ifndef _COMPDEVSDK_H_
#define _COMPDEVSDK_H_

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "ArduinoConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ports.h"
void CompDevSDKMain();

#ifdef __cplusplus
}
#endif

#endif // _COMPDEVSDK_H_
