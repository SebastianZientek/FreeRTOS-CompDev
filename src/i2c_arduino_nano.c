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
    REPEATED_SEND = 2
};

typedef struct{
    volatile enum I2CState state;
    uint8_t inBufferIndex;
    uint8_t inDataReady;
    uint8_t inBytesToRead;
    uint8_t outDataLen;
    volatile uint8_t outBufferIndex;
    uint8_t outBuffer[OUT_BUFFER_SIZE];
    volatile uint8_t inBuffer[IN_BUFFER_SIZE];
} I2CInfo;

static I2CInfo i2cInfo = {
    .state = READY,
    .outBufferIndex = 0,
    .outDataLen = 0,
    .inBufferIndex = 0,
    .inDataReady = 0,
    .inBytesToRead = 0
};

static void i2cStart()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
}

static void i2cStop()
{
    TWCR = TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
}

static void i2cTransmit()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

static void i2cNack()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

static void i2cAck()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
}

ISR(TWI_vect)
{
    switch (TW_STATUS)
    {
        case I2C_SLA_W_ACK:
        case I2C_DATA_TX_ACK:
        case I2C_START_TRANSMITTED:
            if (i2cInfo.outBufferIndex < i2cInfo.outDataLen)
            {
                TWDR = i2cInfo.outBuffer[i2cInfo.outBufferIndex++];
                i2cTransmit();
            }
            else
            {
                i2cStop();
                i2cInfo.state = READY;
            }
            break;
        case I2C_REP_START_TRANSMITTED:
            i2cInfo.state = REPEATED_SEND;
        break;

        case I2C_SLA_R_ACK:
            if (i2cInfo.inBufferIndex < i2cInfo.inBytesToRead - 1)
            {
                i2cAck();
            }
            else
            {
                i2cNack();
            }
        break;

        case I2C_DATA_RX_ACK:
            i2cInfo.inBuffer[i2cInfo.inBufferIndex++] = TWDR;
            if (i2cInfo.inBufferIndex < i2cInfo.inBytesToRead - 1)
            {
                i2cAck();
            }
            else
            {
                i2cNack();
            }
        break;

        case I2C_DATA_RX_NACK:
            i2cInfo.inBuffer[i2cInfo.inBufferIndex++] = TWDR;
            i2cStop();
            i2cInfo.inDataReady = 1;
            i2cInfo.state = READY;
        break;

        case I2C_SLA_R_NACK:
        case I2C_SLA_W_NACK:
            i2cInfo.state = READY;
            i2cStop();
        break;

        case I2C_NOT_RELEVANT:
        break;
    }
}

void i2cInit()
{
    // No prescaler
    TWSR = 0;

    // Frequency for i2c
    TWBR = ((F_CPU / I2C_FREQ) - 16) / 2;

    // Enable i2c
    TWCR = (1 << TWIE) | (1 << TWEN);
}

static int8_t i2cWriteWithMode(uint8_t addr_and_mode, uint8_t *data, uint8_t size)
{
    if (size + 1 > OUT_BUFFER_SIZE)
    {
        return -1;
    }

    while (i2cInfo.state != READY &&
        i2cInfo.state != REPEATED_SEND)
    {
        taskYIELD();
    }

    i2cInfo.state = TRANSMITTING;
    i2cInfo.outDataLen = size + 1; // +1 for address
    i2cInfo.outBufferIndex = 0;

    i2cInfo.outBuffer[0] = addr_and_mode;
    memcpy(i2cInfo.outBuffer + 1, data, size);

	if (i2cInfo.state == REPEATED_SEND)
	{
        TWDR = i2cInfo.outBuffer[i2cInfo.outBufferIndex++];
        i2cTransmit();
	}
    else
    {
        i2cStart();
    }

    return i2cInfo.outBufferIndex - 1;
}

int8_t i2cWrite(uint8_t addr, uint8_t *data, uint8_t size)
{
    i2cWriteWithMode((addr << 1) | I2C_WRITE, data, size);
}

void i2cWriteOneByte(uint8_t addr, uint8_t data)
{
    i2cWriteWithMode((addr << 1) | I2C_WRITE, &data, 1);
}

int8_t i2cRead(uint8_t addr, uint8_t *buffer, uint8_t bufferSize)
{
    if (bufferSize > IN_BUFFER_SIZE)
    {
        return -1;
    }

    i2cInfo.inDataReady = 0;
    i2cInfo.inBufferIndex = 0;
    i2cInfo.inBytesToRead = bufferSize;
    i2cWriteWithMode((addr << 1) | I2C_READ, NULL, 0);

    while (!i2cInfo.inDataReady)
    {
        taskYIELD();
    }

    int8_t readDataSize = i2cInfo.inBufferIndex - 1;
    memcpy(buffer, i2cInfo.outBuffer, readDataSize);

    return readDataSize;
}

uint8_t i2cReadOneByte(uint8_t addr)
{
    uint8_t byte;
    i2cRead(addr, &byte, 1);
    return byte;
}

#endif
