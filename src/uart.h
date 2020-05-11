#ifndef _UART_H_
#define _UART_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

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
