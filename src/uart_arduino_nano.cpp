#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include "ports.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    volatile uint8_t busy;
    volatile uint8_t numBytesToTransmit;
    volatile const char *bufferPtr;
} OutputBuffer_t;

static OutputBuffer_t outputBuffer = {
    .busy = 0,
    .numBytesToTransmit = 0,
    .bufferPtr = NULL
};

static void sendFromOutBuffer()
{
    if (outputBuffer.numBytesToTransmit > 0)
    {
        USART_DATA_REGISTER = *outputBuffer.bufferPtr++;
        outputBuffer.numBytesToTransmit--;
    }
    else
    {
        // CLear flag, there is nothing to send
        CONFIG_REGISER_B &= ~(1 << DATA_REGISTER_FLAG);
        outputBuffer.busy = 0;
    }
}

// Send data when ready
ISR(USART_UDRE_vect)
{
    sendFromOutBuffer();
}

void uartInit(uint32_t baud)
{
    uint8_t baudPrescale = (F_CPU - (baud * 8L)) / (baud * 16UL);

    BAUD_RATE_H  = (baudPrescale >> 8);
    BAUD_RATE_L  = baudPrescale;

    CONFIG_REGISER_B |= (1 << TRANSMITTER_ENABLE);

    // Set character size (8 bits per character)
    CONFIG_REGISER_C |= (1 << UART_CHAR_SIZE_0) | (1 << UART_CHAR_SIZE_1);
}

void uartPrint(const char *str)
{
    while (outputBuffer.busy) { taskYIELD(); }
    outputBuffer.busy = 1;
    outputBuffer.numBytesToTransmit = strlen(str);

    outputBuffer.bufferPtr = str;

    // Transmitter buffer is ready to send data
    CONFIG_REGISER_B |= (1 << DATA_REGISTER_FLAG);
}

void uartPrintf(const char *fmt, ...)
{
    char buffer[UART_PRINTF_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, UART_PRINTF_BUFFER_SIZE, fmt, args);
    va_end(args);

    uartPrint(buffer);
}

#ifdef __cplusplus
}
#endif

#endif
