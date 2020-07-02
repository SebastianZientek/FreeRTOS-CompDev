#include "CompDevSDK.h"

#ifdef PLATFORM_ARDUINO_NANO

#include <avr/io.h>
#include <compat/twi.h>
#include "i2c.h"

#define I2C_FREQ 100000L
#define I2C_WRITE 0
#define I2C_READ 1

// From Atmega328p doc, table 21-3,4
#define I2C_START_TRANSMITTED 0x08
#define I2C_REP_START_TRANSMITTED 0x10
#define I2C_SLA_W_ACK 0x18
#define I2C_SLA_W_NACK 0x20
#define I2C_DATA_TX_ACK 0x28
#define I2C_DATA_TX_NACK 0x30
#define I2C_ARBIT_LOST 0x38
#define I2C_SLA_R_ACK 0x40
#define I2C_SLA_R_NACK 0x48
#define I2C_DATA_RX_ACK 0x50
#define I2C_DATA_RX_NACK 0x58
#define I2C_NOT_RELEVANT 0xF8

#define OUT_BUFFER_SIZE 10
#define IN_BUFFER_SIZE 10

enum I2CState{
    READY = 0,
    TRANSMITTING = 1,
    REPEATED_SEND = 2,
    READING = 3
};

typedef struct{
    volatile enum I2CState state;
    volatile bool repeatStart;
    volatile bool addrTransmited;
    volatile uint8_t addrWithMode;
    volatile uint8_t inputBufferDataLength;
    volatile uint8_t inputBufferIndex;
    volatile uint8_t *inputBuffer;
    volatile uint8_t outputBufferIndex;
    volatile uint8_t outputBufferDataLength;
    volatile uint8_t outputBuffer[OUT_BUFFER_SIZE];
} I2CInfo;

static I2CInfo i2cInfo = {
    .state = READY,
    .repeatStart = 0,
    .addrTransmited = 0,
    .addrWithMode = 0x00,
    .inputBufferDataLength = 0,
    .inputBufferIndex = 0,
    .inputBuffer = NULL,
    .outputBufferIndex = 0,
    .outputBufferDataLength = 0
};

static void i2cTransmit()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

static void i2cAck()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
}

static void i2cNak()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

static void i2cStop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static void i2cStart()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
}

static void i2cSendAddr()
{
    TWDR = i2cInfo.addrWithMode;
    i2cTransmit();
}

void i2cInit()
{
    // No prescaler
    TWSR = 0;

    // Frequency for i2c
    TWBR = ((F_CPU / I2C_FREQ) - 16) / 2;

    // Enable i2c
    TWCR = (0 << TWIE) | (1 << TWEN);
}

int8_t i2cWrite(uint8_t addr, uint8_t *data, uint8_t len, bool repeatStart)
{
    if (len > OUT_BUFFER_SIZE)
    {
        return -1;
    }

    while (i2cInfo.state != READY &&
        i2cInfo.state != REPEATED_SEND)
    {
        taskYIELD();
    }

    i2cInfo.state = TRANSMITTING;
    i2cInfo.addrTransmited = 0;
    i2cInfo.addrWithMode = (addr << 1) | I2C_WRITE;

    memcpy(i2cInfo.outputBuffer, data, len);
    i2cInfo.outputBufferDataLength = len;
    i2cInfo.outputBufferIndex = 0;

    i2cInfo.repeatStart = repeatStart;

    if (repeatStart)
    {
        i2cTransmit();
    }
    else
    {
        i2cStart();
    }

    return i2cInfo.outputBufferIndex;
}

int8_t i2cWriteByte(uint8_t addr, uint8_t data, bool repeatStart)
{
    return i2cWrite(addr, &data, 1, repeatStart);
}

int8_t i2cRead(uint8_t addr, uint8_t *buffer, uint8_t len, bool repeatStart)
{
    while (i2cInfo.state != READY &&
        i2cInfo.state != REPEATED_SEND)
    {
        taskYIELD();
    }

    i2cInfo.state = READING;
    i2cInfo.inputBuffer = buffer;
    i2cInfo.inputBufferDataLength = len;
    i2cInfo.inputBufferIndex = 0;

    i2cInfo.repeatStart = repeatStart;
    i2cInfo.addrTransmited = 0;
    i2cInfo.addrWithMode = (addr << 1) | I2C_READ;

    if (repeatStart)
    {
        i2cTransmit();
    }
    else
    {
        i2cStart();
    }

    // Wait for finish reading
    while (i2cInfo.state != READY &&
        i2cInfo.state != REPEATED_SEND)
    {
        taskYIELD();
    }

    return i2cInfo.inputBufferIndex - 1;
}

uint8_t i2cReadByte(uint8_t addr, bool repeatStart)
{
    uint8_t byte;
    i2cRead(addr, &byte, 1, repeatStart);
    return byte;
}

ISR(TWI_vect)
{
    switch (TW_STATUS)
    {
        case I2C_START_TRANSMITTED:
        case I2C_REP_START_TRANSMITTED:
        case I2C_SLA_W_ACK:
        case I2C_DATA_TX_ACK:
            if (!i2cInfo.addrTransmited)
            {
                TWDR = i2cInfo.addrWithMode;
                i2cTransmit();
                i2cInfo.addrTransmited = 1;
            }
            else if (i2cInfo.outputBufferIndex < i2cInfo.outputBufferDataLength)
            {
                TWDR = i2cInfo.outputBuffer[i2cInfo.outputBufferIndex++];
                i2cTransmit();
            }
            else
            {
                i2cStop();
                i2cInfo.state = READY;
            }
        break;
        case I2C_SLA_R_ACK:
            if (i2cInfo.inputBufferIndex < i2cInfo.inputBufferDataLength)
            {
                i2cAck();
            }
            else
            {
                i2cNak();
            }
        break;
        case I2C_DATA_RX_ACK:
            i2cInfo.inputBuffer[i2cInfo.inputBufferIndex++] = TWDR;
            if (i2cInfo.inputBufferIndex < i2cInfo.inputBufferDataLength)
            {
                i2cAck();
            }
            else
            {
                i2cNak();
            }
        break;
        case I2C_DATA_RX_NACK:
            i2cInfo.inputBuffer[i2cInfo.inputBufferIndex++] = TWDR;
            if (i2cInfo.repeatStart)
            {
                i2cStart();
            }
            else
            {
                i2cStop();
                i2cInfo.state = READY;
            }
        break;
        case I2C_DATA_TX_NACK:
            if (i2cInfo.repeatStart)
            {
                i2cStart();
            }
            else
            {
                i2cInfo.state = READY;
                i2cStop();
            }
        break;
    }
}

#endif
