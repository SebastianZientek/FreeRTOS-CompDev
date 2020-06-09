#ifndef _UART_H_
#define _UART_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USART_DATA_REGISTER UDR0
#define CONFIG_REGISER_B UCSR0B
#define CONFIG_REGISER_C UCSR0C
#define TRANSMITTER_ENABLE TXEN0
#define DATA_REGISTER_FLAG UDRE0
#define BAUD_RATE_H UBRR0H
#define BAUD_RATE_L UBRR0L
#define UART_CHAR_SIZE_0 UCSZ00
#define UART_CHAR_SIZE_1 UCSZ01

/**
 * @brief Inits uart port
 *
 * @param baud      baud rate for communication
 */
void uartInit(uint32_t baud);

/**
 * @brief Prints cstring to the uart
 *
 * @param str       string to print
 */
void uartPrint(const char *str);

/**
 * @brief Formatted printing to uart
 *
 * @param fmt       formatting string
 * @param ...       values to uart
 */
void uartPrintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // _UART_H_
