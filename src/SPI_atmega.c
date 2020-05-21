#include "platform_config.h"

// Compatible with most of Atmega MCUs series
// Tested on Atmega32 and Atmega328p (on arduino nano)

#include "SPI.h"

#include <avr/io.h>
#include "ports.h"

#define SPI_MODE_MASK 0x0C

static uint8_t busyFlag = 0;

static void setDataMode(SpiDataMode_t mode)
{
    SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

static void setBitOrder(SpiBitOrder_t order)
{
    switch (order)
    {
    case SPI_ORDER_LSB_FIRST:
        SPCR |= (1 << DORD);
    break;
    case SPI_ORDER_MSB_FIRST:
        SPCR &= ~(1 << DORD);
    break;
    }
}

static void setClockDivider(SpiClkDiv_t divider)
{
    uint8_t spr1 = 0;
    uint8_t spr0 = 0;
    uint8_t spi2x = 0;

    switch (divider)
    {
    case SPI_CLKDIV_2:
        spi2x = 1;
    break;
    case SPI_CLKDIV_4:
        // All are 0
    break;
    case SPI_CLKDIV_8:
        spi2x = 1;
        spr0 = 1;
    break;
    case SPI_CLKDIV_16:
        spr0 = 1;
    break;
    case SPI_CLKDIV_32:
        spi2x = 1;
        spr1 = 1;
    break;
    case SPI_CLKDIV_64:
        spr1 = 1;
    break;
    case SPI_CLKDIV_128:
        spr1 = 1;
        spr0 = 1;
    break;
    }

    if (spr1)
        SPCR |= (1 << SPR1);
    else
        SPCR &= ~(1 << SPR1);

    if (spr0)
        SPCR |= (1 << SPR0);
    else
        SPCR &= ~(1 << SPR0);

    if (spi2x)
        SPSR |= (1 << SPI2X);
    else
        SPSR &= ~(1 << SPI2X);
}

void SPIInit(SPIConfig spiConfig)
{
    setPinMode(SPI_MOSI_PIN, PIN_OUTPUT);
    setPinMode(SPI_SCK_PIN, PIN_OUTPUT);
    setPinMode(SPI_MISO_PIN, PIN_INPUT);
    setPinMode(SPI_SS_PIN, PIN_OUTPUT);

    setPinValue(SPI_SS_PIN, PIN_HIGH);

    SPCR |= (1 << MSTR) | (1 << SPE);

    setDataMode(spiConfig.dataMode);
    setBitOrder(spiConfig.bitOrder);
    setClockDivider(spiConfig.clkDivider);
}

void SPIDisable()
{
    SPCR = ~(1 << SPE);
}

uint8_t SPITransfer(uint8_t data)
{
    while (busyFlag) taskYIELD();
    busyFlag = 1;

    SPDR = data;

    while (!(SPSR & (1 << SPIF))) taskYIELD();
    data = SPDR;

    busyFlag = 0;
    return data;
}
