#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO

#include "uart.h"
#include <Arduino.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void uartInit(uint32_t baud)
{
    Serial.begin(baud);
}

void uartPrint(const char *str)
{
    Serial.print(str);
}

void uartPrintf(const char *fmt, ...)
{
    char buffer[UART_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, UART_BUFFER_SIZE, fmt, args);
    va_end(args);

    Serial.print(buffer);
}

#ifdef __cplusplus
}
#endif

#endif
