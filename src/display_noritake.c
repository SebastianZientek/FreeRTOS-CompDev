#include "CompDevSDK.h"

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

#include <stdarg.h>
#include <stdio.h>
#include "ports.h"
#include "delay.h"
#include "display.h"

static void write(uint8_t data, uint8_t rs)
{
    uint8_t value = 0xf8 + 2 * rs;

    setPinValue(NORITAKE_STB, 0);
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, 0);
        if (value & i)
            setPinValue(NORITAKE_SIO, 1);
        else
            setPinValue(NORITAKE_SIO, 0);
        setPinValue(NORITAKE_SCK, 1);
    }

    value = data;
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, 0);
        if (value & i)
            setPinValue(NORITAKE_SIO, 1);
        else
            setPinValue(NORITAKE_SIO, 0);
        setPinValue(NORITAKE_SCK, 1);
    }
    setPinValue(NORITAKE_STB, 1);
}

static uint8_t read(uint8_t rs)
{
    uint8_t data = 0xfc + 2 * rs;

    setPinValue(NORITAKE_STB, PIN_LOW);
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        setPinValue(NORITAKE_SCK, PIN_LOW);
        if (data & i)
            setPinValue(NORITAKE_SIO, PIN_HIGH);
        else
            setPinValue(NORITAKE_SIO, PIN_LOW);
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

static void cmd(uint8_t data)
{
    delay(54);
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

    if (addr < 0x14)
        addr = 0x40;
    else if (0x40 <= addr && addr < 0x54)
        addr = 0x14;
    else if (0x14 <= addr && addr <= 0x28)
        addr = 0x54;
    else
        addr = 0x00;
    cmd(0x80 | addr);

	delayUs(5);
}

void displayInit()
{
    setPinMode(NORITAKE_SIO, PIN_OUTPUT);
    setPinMode(NORITAKE_STB, PIN_OUTPUT);
    setPinMode(NORITAKE_SCK, PIN_OUTPUT);

    setPinValue(NORITAKE_SIO, PIN_HIGH);
    setPinValue(NORITAKE_STB, PIN_HIGH);
    setPinValue(NORITAKE_SCK, PIN_HIGH);

    delayUs(11000);

    // Send 3 times command for 8-bit mode
    write(0x30, 0);
    delayUs(10000);

    write(0x30, 0);
    delayUs(110);

    write(0x30, 0);
    delayUs(110);

    // Function set command (for two lines display)
    write(0x38, 0);
    delayUs(60);

    // Display On
    cmd(0x08);
    delayUs(60);

    // Clear display
    cmd(0x01);
    delayUs(4000);

    // Entry mode set
    cmd(0x06);
    delayUs(60);

    // Set properties (cursor on and blinking)
    cmd(0x0F);
    delayUs(60);
}

void displaySetPos(uint8_t x, uint8_t y)
{
    switch (y)
    {
    case 0:
        cmd(0x80 | 0x00 + x);
        break;
    case 1:
        cmd(0x80 | 0x40 + x);
        break;
    case 2:
        cmd(0x80 | 0x14 + x);
        break;
    case 3:
        cmd(0x80 | 0x54 + x);
        break;
    }
    delayUs(500);
}

void displayPrint(const char *str)
{
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
    char buffer[DISPLAY_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, DISPLAY_BUFFER_SIZE, fmt, args);
    va_end(args);

    displayPrint(buffer);
}

#endif
