#include "CompDevSDK.h"

#include <stdarg.h>
#include <stdio.h>
#include "ports.h"
#include "delay.h"
#include "display.h"

#ifdef DISPLAY_NORITAKE

#ifndef NORITAKE_SIO
#warning NORITAKE_SIO not defined, pin 10 will be used
#define NORITAKE_SIO 10
#endif
#ifndef NORITAKE_STB
#warning NORITAKE_STB not defined, pin 11 will be used
#define NORITAKE_STB 11
#endif
#ifndef NORITAKE_SCK
#warning NORITAKE_SCK not defined, pin 12 will be used
#define NORITAKE_SCK 12
#endif

static uint8_t displayInitialized = 0;

static void write(uint8_t data, uint8_t registerSelect)
{
    uint8_t value = 0xf8 + 2 * registerSelect;

    setPinValue(NORITAKE_STB, PIN_LOW);
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, PIN_LOW);
        setPinValue(NORITAKE_SIO, value & i);
        setPinValue(NORITAKE_SCK, PIN_HIGH);
    }

    value = data;
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, PIN_LOW);
        setPinValue(NORITAKE_SIO, value & i);
        setPinValue(NORITAKE_SCK, PIN_HIGH);
    }
    setPinValue(NORITAKE_STB, PIN_HIGH);
}

static uint8_t read(uint8_t registerSelect)
{
    uint8_t data = 0xfc + 2 * registerSelect;

    setPinValue(NORITAKE_STB, PIN_LOW);
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, PIN_LOW);
        setPinValue(NORITAKE_SIO, data & i);
        setPinValue(NORITAKE_SCK, PIN_HIGH);
    }

    setPinMode(NORITAKE_SIO, PIN_INPUT);
    delayUs(1);

    for (uint8_t i = 0; i < 8; ++i)
    {
        setPinValue(NORITAKE_SCK, PIN_LOW);
        delayUs(1);
        setPinValue(NORITAKE_SCK, PIN_HIGH);

        data <<= 1;
        if (getPinValue(NORITAKE_SIO))
            data |= 1;
    }

    setPinValue(NORITAKE_STB, PIN_HIGH);
    setPinMode(NORITAKE_SIO, PIN_OUTPUT);
    return data;
}

static void setupDisplay()
{
    setPinMode(NORITAKE_SIO, PIN_OUTPUT);
    setPinMode(NORITAKE_STB, PIN_OUTPUT);
    setPinMode(NORITAKE_SCK, PIN_OUTPUT);

    setPinValue(NORITAKE_SIO, PIN_HIGH);
    setPinValue(NORITAKE_STB, PIN_HIGH);
    setPinValue(NORITAKE_SCK, PIN_HIGH);

    delayUs(11000);

    // Send 3 times command for 8-bit mode
    write(DISPLAY_FUNCTION_SET, 0);
    delayUs(10000);

    write(DISPLAY_FUNCTION_SET, 0);
    delayUs(110);

    write(DISPLAY_FUNCTION_SET, 0);
    delayUs(110);

    write(DISPLAY_SET_COMMAND_FOR_8_BIT_MODE, 0);
    delayUs(60);
}

static void cmd(uint8_t data)
{
    delayUs(54);
    write(data, 0);
}

static uint8_t readCmd()
{
    uint8_t data = read(0);
	delayUs(5);
	return data;
}

static uint8_t readAddress()
{
	return readCmd() & ~0x80;
}

static void newLine()
{
	uint8_t addr = readAddress();

    if (addr < DISPLAY_LINE1_ADDR + 20)
        addr = DISPLAY_LINE2_ADDR;
    else if (addr <= DISPLAY_LINE3_ADDR + 20)
        addr = DISPLAY_LINE4_ADDR;
    else if (addr < DISPLAY_LINE2_ADDR + 20)
        addr = DISPLAY_LINE3_ADDR;
    else
        addr = DISPLAY_LINE1_ADDR;
    cmd(DISPLAY_DDRAM | addr);

	delayUs(5);
}

void displayInit()
{
    setupDisplay();

    cmd(DISPLAY_OFF);
    delayUs(60);

    cmd(DISPLAY_CLEAR);
    delayUs(4000);

    cmd(DISPLAY_MODE_SET);
    delayUs(60);

    cmd(DISPLAY_SETTINGS | DISPLAY_ON /* | DISPLAY_CURSOR_ON | DISPLAY_CURSOR_BLINK */);
    delayUs(60);

    displayInitialized = 1;
}

void displaySetPos(uint8_t x, uint8_t y)
{
    while (!displayInitialized)
    {
        taskYIELD();
    }

    switch (y)
    {
    case 0:
        cmd(DISPLAY_DDRAM | DISPLAY_LINE1_ADDR + x);
        break;
    case 1:
        cmd(DISPLAY_DDRAM | DISPLAY_LINE2_ADDR + x);
        break;
    case 2:
        cmd(DISPLAY_DDRAM | DISPLAY_LINE3_ADDR + x);
        break;
    case 3:
        cmd(DISPLAY_DDRAM | DISPLAY_LINE4_ADDR + x);
        break;
    }
    delayUs(500);
}

void displayPrint(const char *str)
{
    while (!displayInitialized)
    {
        taskYIELD();
    }

    while (*str)
    {
        if (*str == '\n')
            newLine();
        else
            write(*str, 1);
        str++;
    }
}

void displayPrintf(const char *fmt, ...)
{
    while (!displayInitialized)
    {
        taskYIELD();
    }

    char buffer[DISPLAY_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, DISPLAY_BUFFER_SIZE, fmt, args);
    va_end(args);

    displayPrint(buffer);
}

#endif
