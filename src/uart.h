#ifndef _UART_H_
#define _UART_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void uartInit(uint32_t baud);
void uartPrint(const char *str);
void uartPrintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // _UART_H_
