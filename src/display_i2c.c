#include "CompDevSDK.h"

#include <stdarg.h>
#include <stdio.h>
#include "ports.h"
#include "delay.h"
#include "display.h"

#ifdef DISPLAY_I2C

#include "i2c.h"

#define LCD_D0 4
#define LCD_D1 5
#define LCD_D2 6
#define LCD_D3 7
#define LCD_RS 0
#define LCD_RW 1
#define LCD_E 2
#define LCD_LED 3

static uint8_t displayCurrentData = 0x00;
static uint8_t displayInitialized = 0;

static void setDataBit(uint8_t bit, uint8_t value)
{
    if (value)
        displayCurrentData |= (1 << bit);
    else
        displayCurrentData &= ~(1 << bit);
}

static void triggerEnable()
{
    setDataBit(LCD_E, 1);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(2);

    setDataBit(LCD_E, 0);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(2);
}

static void write4bit(uint8_t data, uint8_t registerSelect)
{
    setDataBit(LCD_RS, registerSelect);
    setDataBit(LCD_RW, 0);

    for (uint8_t i = 0; i < 4; ++i)
    {
        setDataBit(LCD_D3 - i, data & (0x08 >> i));
    }

    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    triggerEnable();
}

static void write(uint8_t data, uint8_t registerSelect)
{
    // Write high part
    write4bit(data >> 4, registerSelect);

    // Write low part
    write4bit(data, registerSelect);
}

static uint8_t read(uint8_t registerSelect)
{
    uint8_t ret = 0x00;
    setDataBit(LCD_RS, registerSelect);
    setDataBit(LCD_RW, 1);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);

    // Read high part of data
    setDataBit(LCD_E, 1);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(10);

    uint8_t data = ~i2cReadByte(DISPLAY_I2C_ADDR, 0);

    for (uint8_t i = 4; i; --i)
    {
        ret |= (data & 0x80) >> 7;
        ret << 1;
    }

    setDataBit(LCD_E, 0);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(10);

    // Read low part of data
    setDataBit(LCD_E, 1);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(10);

    data = ~i2cReadByte(DISPLAY_I2C_ADDR, 0);

    for (uint8_t i = 4; i; --i)
    {
        ret |= (data & 0x80) >> 7;
        ret << 1;
    }

    setDataBit(LCD_E, 0);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);
    delayUs(10);

    return ret;
}

static void setupDisplay()
{
    // Backlight on
    setDataBit(LCD_LED, 1);
    i2cWriteByte(DISPLAY_I2C_ADDR, displayCurrentData, 0);

    // Send 3 times command for initialization in 8-bit mode (initial state).
    // We are sending only high part of data, rest is ignored.
    write4bit(DISPLAY_FUNCTION_SET >> 4, 0);
    delayUs(5000);

    write4bit(DISPLAY_FUNCTION_SET >> 4, 0);
    delayUs(100);

    write4bit(DISPLAY_FUNCTION_SET >> 4, 0);
    delayUs(100);

    // Now we can switch to 4bit mode
    write4bit(DISPLAY_ENTER_4_BIT_MODE, 0);
    delayUs(1000);

    write(DISPLAY_SET_COMMAND_FOR_4_BIT_MODE, 0);
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

    cmd(DISPLAY_SETTINGS | DISPLAY_ON | DISPLAY_CURSOR_ON | DISPLAY_CURSOR_BLINK);
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
