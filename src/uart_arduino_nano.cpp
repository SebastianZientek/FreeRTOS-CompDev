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
    volatile uint8_t start;
    volatile uint8_t end;
    volatile uint8_t buffer[UART_BUFFER_SIZE];
} OutputBuffer_t;

static OutputBuffer_t outputBuffer = {
    .busy = 0,
    .numBytesToTransmit = 0,
    .start = 0,
    .end = 0,
};

static uint8_t isOutBufferFull()
{
    return (outputBuffer.end + 1) % UART_BUFFER_SIZE == outputBuffer.start;
}

static uint8_t isEmpty()
{
    return outputBuffer.end == outputBuffer.start;
}

static void putCharInOutBuffer(char c)
{
    outputBuffer.end = (outputBuffer.end + 1) % UART_BUFFER_SIZE;
    outputBuffer.buffer[outputBuffer.end] = c;
}

static char getFromOutBuffer()
{
    char c = outputBuffer.buffer[outputBuffer.start];
    outputBuffer.start = (outputBuffer.start + 1) % UART_BUFFER_SIZE;
    return c;
}

static void sendFromOutBuffer()
{
    if (outputBuffer.numBytesToTransmit > 0)
    {
        if (!isEmpty()) UDR0 = getFromOutBuffer();
    }
    else
    {
        UCSR0B &= ~(1 << UDRE0);
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

    UBRR0H  = (baudPrescale >> 8);
    UBRR0L  = baudPrescale;
    UCSR0B |= (1<<TXEN0);
    UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
}

void uartPrint(const char *str)
{
    while (outputBuffer.busy) { taskYIELD(); }
    outputBuffer.busy = 1;
    outputBuffer.numBytesToTransmit = strlen(str);

    const char *ptr = str;

    while (outputBuffer.numBytesToTransmit > 0)
    {
        while (isOutBufferFull()) { taskYIELD(); }

        putCharInOutBuffer(*ptr++);
        outputBuffer.numBytesToTransmit--;

        UCSR0B |= (1 << UDRE0);
    }
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
