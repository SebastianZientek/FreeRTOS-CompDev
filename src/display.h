#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define DISPLAY_DDRAM 0x80
#define DISPLAY_LINE1_ADDR 0x00
#define DISPLAY_LINE2_ADDR 0x40
#define DISPLAY_LINE3_ADDR 0x14
#define DISPLAY_LINE4_ADDR 0x54

#define DISPLAY_SET_COMMAND_FOR_8_BIT_MODE 0x38
#define DISPLAY_SET_COMMAND_FOR_4_BIT_MODE 0x28
#define DISPLAY_FUNCTION_SET 0x30
#define DISPLAY_ENTER_4_BIT_MODE 0x2
#define DISPLAY_MODE_SET 0x06
#define DISPLAY_SETTINGS 0x08

#define DISPLAY_CLEAR 0x01
#define DISPLAY_OFF 0x08
#define DISPLAY_ON 0x04
#define DISPLAY_CURSOR_ON 0x02
#define DISPLAY_CURSOR_BLINK 0x01

#define DISPLAY_BUFFER_SIZE 40

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of display
 *
 */
void displayInit();

/**
 * @brief Set position of cursor
 *
 * @param x     column
 * @param y     line
 */
void displaySetPos(uint8_t x, uint8_t y);

/**
 * @brief Prints cstring to the display
 *
 * @param str       string to print
 */
void displayPrint(const char *str);

/**
 * @brief Formatted printing to the display
 *
 * @param fmt       formatting string
 * @param ...       values to display
 */
void displayPrintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // _DISPLAY_H_
