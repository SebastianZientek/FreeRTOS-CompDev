#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <inttypes.h>

void displayInit();
void displaySetPos(uint8_t x, uint8_t y);
void displayPrint(const char *str);
void displayPrintf(const char *fmt, ...);

#endif // _DISPLAY_H_
